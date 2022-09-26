#if !defined(_XPUBPYTHONWRAPPER_H_)
#define _XPUBPYTHONWRAPPER_H_

#include "SModelInd/ModelExpImp.h"
#include "SModelInd/XPubVariant.h"


typedef struct
{
  void* pOriginalState;
} XPubPythonState;

class CPythonInterface;
class CHtmlStoneImage;


class XPUBMODEL_EXPORTIMPORT CPythonFunctionParameters
{

public:

	CPythonFunctionParameters(void){};
	virtual ~CPythonFunctionParameters(void){};

public:
	void Add(const xtstring& cFieldName, const xtstring& cSymbolName);
	void SetValue(const CXPubVariant& cValue);
	xtstring m_cSymbolName;    // Symbolname
	xtstring m_cFieldName;     // Variablenname, wenn es ID ist

	CXPubVariant m_cValue;			 // der Wert

};

inline void CPythonFunctionParameters::Add(const xtstring& cFieldName, const xtstring& cSymbolName)
{	m_cFieldName = cFieldName; m_cSymbolName = cSymbolName; }

inline void CPythonFunctionParameters::SetValue(const CXPubVariant& cValue)
{	m_cValue = cValue; }

typedef list<CPythonFunctionParameters>       PythonFunctionParameterList;
typedef PythonFunctionParameterList::iterator PythonFunctionParameterListIterator;





class XPUBMODEL_EXPORTIMPORT CXPubPythonWrapperInterface
{
public:
  CXPubPythonWrapperInterface();
  ~CXPubPythonWrapperInterface();

	static int Initialize(LPCXTCHAR sProgramName = 0);
	static void Finalize();
	static XPubPythonState* InitSession(LPCXTCHAR pPythonToolPath);
	static int CloseSession(XPubPythonState* pXPubPythonState);


  const char* GetLastError();
	long Function(XPubPythonState *pXPubPythonState,
                const xtstring& slibfile,
                const xtstring& sFunction,
                PythonFunctionParameterList& FuncParaList,
                CXPubVariant& cResult);
  int RunFile(XPubPythonState *pXPubPythonState,
              xtstring& sModulPathName,
              xtstring& sModulName,
              xtstring& sOutput);
  int CompileFile(XPubPythonState *pXPubPythonState,
                  xtstring& sModulPathName,
                  xtstring& sImportName,
                  xtstring& sOutput);
private:
  CPythonInterface* m_pInterface;
};

class XPUBMODEL_EXPORTIMPORT CHtmlStoneImageWrapper
{
public:
  CHtmlStoneImageWrapper();
  ~CHtmlStoneImageWrapper();

	CHtmlStoneImageWrapper& Clear();

  CHtmlStoneImageWrapper& SetSrc(xtstring& cSrc);
	CHtmlStoneImageWrapper& SetWidth(xtstring& cWidth);
	CHtmlStoneImageWrapper& SetHeight(xtstring& cHeight);
	CHtmlStoneImageWrapper& SetAlign(xtstring& cAlign);
	CHtmlStoneImageWrapper& SetName(xtstring& cName);
	CHtmlStoneImageWrapper& SetAlt(xtstring& cAlt);
	CHtmlStoneImageWrapper& SetBorder(xtstring& cBorder);
	CHtmlStoneImageWrapper& SetLongdesc(xtstring& cLongdesc);

	CHtmlStoneImageWrapper& SetHSpace(xtstring& cHSpace);
	CHtmlStoneImageWrapper& SetVSpace(xtstring& cVSpace);
	CHtmlStoneImageWrapper& SetUsemap(xtstring& cUsemap);

private:
  CHtmlStoneImage* m_pHtmlStoneImage;
};


#endif // !defined(_XPUBPYTHONWRAPPER_H_)
