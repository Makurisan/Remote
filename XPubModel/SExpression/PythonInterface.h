#if !defined(_PYTHONINTERFACE_H_)
#define _PYTHONINTERFACE_H_

#include "SModelInd/ModelExpImp.h"
#include "SModelInd/XPubVariant.h"
#include "XPubPythonWrapper.h"

#if !defined(NO_PYTHON)

struct PythonState
{
	PyThreadState* pThreadState;
	PyObject* pOwnModObj;	// unsere Zugriffsschicht
};

class XPUBMODEL_EXPORTIMPORT CPythonInterface
{
public:

	CPythonInterface(void);
	virtual ~CPythonInterface(void);

	static PythonState* InitSession(LPCXTCHAR pPythonToolPath);
	static int CloseSession(PythonState* pPyThreadState);

	static int Initialize(LPCXTCHAR sProgramName = NULL);
	static void Finalize();
	 
	long Function(PythonState *pStateInter, const xtstring& slibfile, const xtstring& sFunction,
		            PythonFunctionParameterList& FuncParaList, CXPubVariant& cResult);

	int RunFile(PythonState *pStateInter, xtstring& sModulPathName, xtstring& sModulName, xtstring& sOutput);
	int CompileFile(PythonState *pStateInter, xtstring& sModulPathName, xtstring& sImportName, xtstring& sOutput);
	long GetFunctions(PythonState *pStateInter, const xtstring& slibfile,  list<xtstring>& cFunctionList, 
		const xtstring& sType);

  const char* GetLastError();
  void ClearError();

protected:

	static PyObject* CPythonInterface::StdOut(PyObject *self, PyObject *args);
	static PyObject* InitModule(const char *name, const PyMethodDef *methods);
	static void SetOutput();

	static void Error();

protected:
	static PyObject *ImportModule(char *name);
	static int AppendPath(LPCXTCHAR pPythonToolPath);

protected:		
	
	static XTCHAR m_sProgramName[500];
	static XTCHAR m_sErrorMsg[500];
	static long m_lLastError;
	static PyThreadState *m_gState;
	
	static PyMethodDef CPythonInterface::m_ioMethods[];

};

#endif // NO_PYTHON

#endif // !defined(_PYTHONINTERFACE_H_)
