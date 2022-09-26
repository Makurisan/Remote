// PythonInterface.cpp: implementation of the CPythonInterface class.
//
//In my embedding, I use the following (adapting the example above):
//
//// initialize the Python interpreter
//Py_Initialize();
//
//PyEval_InitThreads();
//
///* Swap out and return current thread state and release the GIL */
//PyThreadState tstate = PyEval_SaveThread();
//
//PyGILState_STATE gstate;
//gstate = PyGILState_Ensure();
//
//PyRun_SimpleString("import random\n");
//
//PyGILState_Release(gstate); 
//
//
//You don't have to free the tstate returned by PyEval_SaveThread because
//it is the thread state of the main thread (as established during
//Py_Initialize), and so it will be freed during Python's shut-down.
//
//I think in general you should rarely need to call PyEval_ReleaseLock
//directly; instead use PyEval_SaveThread, the Py_BEGIN_ALLOW_THREADS
//macro, or PyGILState_Release (as appropriate).  The documentation should
//probably say as much.
//////////////////////////////////////////////////////////////////////
#if !defined(NO_PYTHON)

#if defined(WIN32)
#include <windows.h>
#endif

#include "assert.h"
#include "SSTLdef/STLdef.h"

#include <list>

#include "SModelInd/ModelInd.h"
#include "SModelInd/ERModelInd.h"
#include "SModelInd/XPubVariant.h"

#include "Python.h"

#include "PythonInterface.h"

#include "HtmlStoneImage.h"
//#include "Namespaces.h"


#define PYTHON_ERROR_CODEBASIS			0x0100

#define PYTHONERROR_NONE										0x0000 // OK
#define PYTHONERROR_ARGUMENT_FAILED					PYTHON_ERROR_CODEBASIS + 0x0001 // Cannot convert argument
#define PYTHONERROR_FUNCTIONCALL_FAILED			PYTHON_ERROR_CODEBASIS + 0x0002 // Function call failed
#define PYTHONERROR_CANNOTFIND_FUNCTION			PYTHON_ERROR_CODEBASIS + 0x0003 // Cannot find function '%s'.
#define PYTHONERROR_CANNOTFIND_MODULE				PYTHON_ERROR_CODEBASIS + 0x0004 // Cannot find module '%s'.
#define PYTHONERROR_LIBRARY_NOT_INITIALIZED	PYTHON_ERROR_CODEBASIS + 0x0005 // the library is not initialized
#define PYTHONERROR_UNDEFINED								PYTHON_ERROR_CODEBASIS + 0x0006 // undefined call
#define PYTHONERROR_MODULE_NOTFOUND					PYTHON_ERROR_CODEBASIS + 0x0007 // module not found

class CPyThreadState
{
public:
	CPyThreadState(PyThreadState *p) : m_p(p)
	{
		PyEval_RestoreThread(p);   
	}
	~CPyThreadState()
	{
		PyEval_SaveThread();		
	}
protected:
	PyThreadState *m_p;
};

char CPythonInterface::m_sProgramName[]={"XPubExec"};
char CPythonInterface::m_sErrorMsg[]={""};
long CPythonInterface::m_lLastError=0;

// Define methods available to Python
PyThreadState *CPythonInterface::m_gState = NULL;

PyMethodDef CPythonInterface::m_ioMethods[] = {
	{"XPubStdOut", StdOut, METH_VARARGS, "XPubStdOut"},
	{NULL, NULL, 0, NULL}
};

static char cError[2000]={0};

const char*  CPythonInterface::GetLastError()
{
  return cError;
}

void CPythonInterface::ClearError()
{
  *cError = 0; 
}

PyObject* CPythonInterface::StdOut(PyObject*, PyObject *args)
{
	char *string;
	if (!PyArg_ParseTuple(args, "s", &string))
		return 0;
  
  if(strlen(cError) + strlen(string) < sizeof(cError))
  	strcat(cError, string);
  else
    strcpy(cError, string);

  //gld::gMsgView()->write( "%s", string);
	return Py_BuildValue("");
}

CPythonInterface::CPythonInterface(void)
{

}
CPythonInterface::~CPythonInterface(void)
{
}

void CPythonInterface::Finalize()
{ 
	if(Py_IsInitialized())
	{
		PyEval_RestoreThread(m_gState);  
		Py_Finalize();
	} 
} 

//
// redirect the stdout and stderr
//
void CPythonInterface::SetOutput()
{
		PyObject* sys_module = PyImport_ImportModule("sys"); 
    PyObject* xpubmodule = Py_InitModule("XPubStdOut", m_ioMethods); 
    PyObject* xpubdict = PyModule_GetDict(xpubmodule); 
    PyObject* aux; 
    char * code = "class Sout:\n" 
                  "    def write(self, s): XPubStdOut(s)\n" 
                  "sout = Sout()\n"; 

    aux = PyRun_String(code, Py_file_input, xpubdict, xpubdict); 
    Py_XDECREF(aux); 
    aux = PyObject_GetAttrString(xpubmodule, "sout"); 
    PyObject_SetAttrString(sys_module, "stdout", aux); 
    PyObject_SetAttrString(sys_module, "stderr", aux); 
    Py_XDECREF(aux); 
}

//
// append our path and set the stdout, stderr to our module
//
int CPythonInterface::AppendPath(LPCXTCHAR pPythonToolPath)
{
  //HANDLE hThread = AfxGetThread();
	
  if(Py_IsInitialized())
	{
		XTCHAR sPythAddFile[XPUB_MAX_PATH * 2];
	 
		// give python our module dir too
		// get segmentation error if path is not set
		sprintf(sPythAddFile, "import sys\nsys.path.append('%s')\n", pPythonToolPath);
		int nRet = PyRun_SimpleString(sPythAddFile);

		sprintf(sPythAddFile, "import sys\nversion = sys.version[:3]\nprint version + ' Python'", pPythonToolPath);
		
		nRet = PyRun_SimpleString(sPythAddFile);
		//not complete: GetModulePath
		if(nRet != -1)
		{
			XTCHAR new_path[MAX_PATH+1];

			// set the python path relative to the process to "\\Lib"
			::GetModuleFileName(NULL, new_path, _MAX_PATH);
			xtstring sPath(new_path);
			size_t nPos = sPath.find_last_of(_XT("\\/"));
			if (nPos != xtstring::npos)
				sPath.assign(sPath.begin(), sPath.begin() + nPos + 1);

			sPath += _XT("Lib");
			SetEnvironmentVariable("PYTHONPATH", sPath.c_str());
		}
		SetOutput();

		return nRet != -1;
	} 
	return false;
}

PyObject* CPythonInterface::InitModule(const char *name, const PyMethodDef *methods)
{
	if(Py_IsInitialized())
	{
		PyObject* pObj = Py_InitModule((char*)name, (PyMethodDef*)methods);
		return pObj;
	}
	return NULL;
}

PythonState* CPythonInterface::InitSession(LPCXTCHAR pPythonToolPath)
{
	PyEval_AcquireLock();

	PythonState *pPytState = new PythonState;

	pPytState->pThreadState  = Py_NewInterpreter();
	PyEval_SaveThread();  

	CPyThreadState lock(pPytState->pThreadState );

	pPytState->pOwnModObj = InitModule(CHtmlStoneImage::GetImportName(), CHtmlStoneImage::GetMethodDef());

	AppendPath(pPythonToolPath);

	return pPytState;
}

int CPythonInterface::CloseSession(PythonState* pPytState)
{

	PyEval_RestoreThread(pPytState->pThreadState);  
	Py_EndInterpreter(pPytState->pThreadState);
	PyEval_ReleaseLock(); // release lock  

	delete pPytState;

	return 0;
}

int CPythonInterface::Initialize(LPCXTCHAR pProgramName/*=NULL*/)
{
	if(pProgramName)
		strcpy(m_sProgramName, pProgramName);
	else
		strcpy(m_sProgramName, "XPubExec");

	Py_SetProgramName(m_sProgramName);
		
	if(!Py_IsInitialized())
	{  
		PyEval_InitThreads();
		
		Py_Initialize();
		m_gState = PyEval_SaveThread(); /* Release the thread state */

		return true;

	}
	return false;
}

long CPythonInterface::GetFunctions(PythonState *pStateInter, const xtstring& slibfile, list<xtstring>& cFunctionList, 
																		const xtstring& sType)
{
	if(!Py_IsInitialized())
		return m_lLastError = PYTHONERROR_LIBRARY_NOT_INITIALIZED;

	CPyThreadState lock(pStateInter->pThreadState);

	PyObject *pName = PyString_FromString(slibfile.c_str());
	/* Error checking of pName left out */
	if(!pName)
		return m_lLastError = PYTHONERROR_MODULE_NOTFOUND;

	PyObject *pModule = PyImport_Import(pName);
	if (pModule != NULL)
	{
		PyObject *pDict = PyModule_GetDict(pModule);
		int pos = 0;
		PyObject *key, *val;
		while (PyDict_Next(pDict, &pos, &key, &val) > 0)
		{
			if(!PyString_Check(val))
			{	
				xtstring sReturn;
				PyObject_IsTrue(val);
				sReturn =((PyTypeObject*)PyObject_Type(val))->tp_name;
				if(sReturn == sType)
				{
					sReturn = PyString_AsString(key);
					cFunctionList.push_front(sReturn);
				}
			}
		}
		Py_DECREF(pModule);
	}
	Py_DECREF(pName);
	return cFunctionList.size()>0?0:PYTHONERROR_MODULE_NOTFOUND;

}

long CPythonInterface::Function(PythonState *pStateInter, const xtstring& slibfile, const xtstring& sFunction,
														    PythonFunctionParameterList& cFuncParaList, CXPubVariant& cResult)
{
	CPyThreadState lock(pStateInter->pThreadState);

  ClearError();
 // HANDLE hThread = AfxGetThread();
 
	m_lLastError = PYTHONERROR_UNDEFINED;

	if(!Py_IsInitialized())
		return m_lLastError = PYTHONERROR_LIBRARY_NOT_INITIALIZED;

	PyObject *pName = PyString_FromString(slibfile.c_str());
	/* Error checking of pName left out */
	if(!pName)
		return m_lLastError = PYTHONERROR_MODULE_NOTFOUND;

  Py_INCREF(pName);

	PyObject *pModule = PyImport_Import(pName);

  Py_DECREF(pName);

	if (pModule != NULL)
	{
		PyObject *pDict = PyModule_GetDict(pModule);

		/* pDict is a borrowed reference */
		PyObject *pFunc = PyDict_GetItemString(pDict, (char*)sFunction.c_str());


		/* pFun: Borrowed reference */
		if (pFunc && PyCallable_Check(pFunc))
		{
			PyObject *pArgs=NULL;
			
			PythonFunctionParameterListIterator First = cFuncParaList.begin(); 
			
			if(cFuncParaList.size())
				pArgs = PyTuple_New((int)cFuncParaList.size());
			else
				pArgs = PyTuple_New(0);

			PyObject *pValue;
			for (int i = 0; First != cFuncParaList.end(); ++First, i++)
			{
				xtstring sString = (xtstring)(*First).m_cValue;
				TGroupOfFieldTypes eGroupFieldType = CXPubVariant::GetGroupOfFieldType((*First).m_cValue.GetType());
		
				if( eGroupFieldType == tFieldTypeGroupText || eGroupFieldType == tFieldTypeGroupDateTime )
					pValue = PyString_FromString(sString.c_str());
				else
					pValue = PyInt_FromLong(atoi(sString.c_str()));

				if (!pValue) {
					return m_lLastError = PYTHONERROR_ARGUMENT_FAILED;
				}
				/* pValue reference stolen here: */
				PyTuple_SetItem(pArgs, i, pValue);
			}
			pValue = PyObject_CallObject(pFunc, pArgs);
			if (pValue != NULL)
			{
				m_lLastError = PYTHONERROR_NONE;

				if(PyString_Check(pValue)) // string
				{
					cResult.SetTypedValue(PyString_AsString(pValue), tFieldTypeVarChar);				
				}
				else
				if(PyLong_Check(pValue)) // long
				{
					sprintf(m_sErrorMsg, "%ld", PyInt_AsLong(pValue));
					cResult.SetTypedValue(m_sErrorMsg, tFieldTypeBigInt);
				}
				else
				if(PyInt_Check(pValue)) // int
				{
					sprintf(m_sErrorMsg, "%ld", PyInt_AsLong(pValue));
					cResult.SetTypedValue(m_sErrorMsg, tFieldTypeInteger);
				}
				else
				if(PyBool_Check(pValue)) // bool
				{
					sprintf(m_sErrorMsg, "%ld", PyInt_AsLong(pValue));
					cResult.SetTypedValue(m_sErrorMsg, tFieldTypeInteger);
				}
				else
				if(PyBuffer_Check(pValue)) // buffer
				{
				}
				// PyDate_Check  PyDateTime_Check PyTime_Check PyFloat_Check
				Py_DECREF(pValue);
			}
			else
			{			
        PyErr_Print();
				m_lLastError = PYTHONERROR_FUNCTIONCALL_FAILED;
			}
			if(pArgs)
				Py_DECREF(pArgs);
		}
		else {
      PyErr_Print();
			m_lLastError = PYTHONERROR_CANNOTFIND_FUNCTION;
		}
		Py_DECREF(pModule);
	}
	else {
    PyErr_Print();
		m_lLastError = PYTHONERROR_CANNOTFIND_MODULE;
	}
	Py_DECREF(pName);
	return m_lLastError;

}

int CPythonInterface::CompileFile(PythonState *pStateInter, xtstring& sModulPathName, xtstring& sImportName, xtstring& sOutput)
{
	CPyThreadState lock(pStateInter->pThreadState);

	xtstring sStdString =	_XT("import py_compile\n");
  sStdString += _XT("py_compile.compile(r'");
  sStdString += sModulPathName;
  sStdString += _XT("')\n");
	
  int nRet;

	//if( (nRet=RunFile(sModulPathName, sImportName, sOutput)) != -1)
	nRet = PyRun_SimpleString((char*)sStdString.c_str());

	return nRet;

}

int CPythonInterface::RunFile(PythonState *pStateInter, xtstring& sModulPathName, xtstring& sModulName, xtstring& sOutput)
{
	CPyThreadState lock(pStateInter->pThreadState);

	int res;
	FILE *fp = NULL;

	cError[0] = 0;

	fp = fopen(sModulPathName.c_str(), "r");
	res = PyRun_SimpleFile(fp, (char *)sModulName.c_str());
	if(fp)	
		fclose(fp);

  sOutput = cError;

	return res;

}

PyObject *CPythonInterface::ImportModule(char *name)
{
	PyObject *str, *module;
	char msg[128];

	str = PyString_FromString(name);
	if (str == NULL)
		return NULL;
	module = PyImport_Import(str);
	Py_DECREF(str);
	if (module == NULL) {
		sprintf(msg, "Could not import %s module", name);
//		MessageBox(NULL, msg, "Python Error", MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL);
		return NULL;
	}
	return module;
}

void CPythonInterface::Error()
{
	static PyObject *traceback_module;
	static PyObject *string_module;

	PyObject *exc = NULL, *value = NULL, *trace = NULL,
	*func = NULL,
	*lines = NULL,
	*str = NULL,
	*obj = NULL;

	// exc, value, trace = sys.exc_info()
	PyErr_Fetch(&exc, &value, &trace);

	// import traceback
	if (traceback_module == NULL)
		traceback_module = ImportModule("Traceback");
	if (traceback_module == NULL)
		goto error;

	// lines = traceback.format_exception(exc, value, trace, 20)
	obj = PyModule_GetDict(traceback_module);
	func = PyDict_GetItemString(obj, "format_exception");
	Py_DECREF(obj); obj = NULL;
	if (func == NULL)
		goto error;
	lines = PyObject_CallFunction(func, "OOOi", exc, value, trace, 20);
	if (lines == NULL)
		goto error;
	Py_DECREF(func); func = NULL;

	// import string
	if (string_module == NULL)
		string_module = ImportModule("string");
	if (string_module == NULL)
		goto error;

	// str = string.join(lines)
	obj = PyModule_GetDict(string_module);
	func = PyDict_GetItemString(obj, "join");
	Py_DECREF(obj); obj = NULL;
	
	if (func == NULL)
		goto error;
	
	str = PyObject_CallFunction(func, "O", lines);
	if (str == NULL)
		goto error;
	Py_DECREF(func); func = NULL;

	//MessageBox(NULL, PyString_AsString(str), "Python Error", MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL);

	error:
	Py_XDECREF(exc); Py_XDECREF(value); Py_XDECREF(trace);
	Py_XDECREF(func);
	Py_XDECREF(lines);
	Py_XDECREF(str);
	Py_XDECREF(obj);

	PyErr_Clear();
}

#endif // NO_PYTHON
