// PythonInterface.cpp: implementation of the CPythonInterface class.
//
//////////////////////////////////////////////////////////////////////

#if defined(WIN32)
#include <windows.h>
#endif

#include "assert.h"
#include "SSTLdef/STLdef.h"

#include <list>

#include "SModelInd/ModelInd.h"
#include "SModelInd/ERModelInd.h"
#include "SModelInd/XPubVariant.h"

#include "XPubPythonWrapper.h"

#if !defined(NO_PYTHON)
#include "Python.h"
#include "PythonInterface.h"
#include "HtmlStoneImage.h"
#endif // NO_PYTHON


CXPubPythonWrapperInterface::CXPubPythonWrapperInterface()
{
#if !defined(NO_PYTHON)
  m_pInterface = new CPythonInterface;
#else // NO_PYTHON
  m_pInterface = 0;
#endif // NO_PYTHON
}

CXPubPythonWrapperInterface::~CXPubPythonWrapperInterface()
{
#if !defined(NO_PYTHON)
  if (m_pInterface)
    delete m_pInterface;
#endif // NO_PYTHON
}





int CXPubPythonWrapperInterface::Initialize(LPCXTCHAR sProgramName /*= 0*/)
{
#if !defined(NO_PYTHON)
  return CPythonInterface::Initialize(sProgramName);
#else // NO_PYTHON
  return 0;
#endif // NO_PYTHON
}
void CXPubPythonWrapperInterface::Finalize()
{
#if !defined(NO_PYTHON)
  CPythonInterface::Finalize();
#endif // NO_PYTHON
}
XPubPythonState* CXPubPythonWrapperInterface::InitSession(LPCXTCHAR pPythonToolPath)
{
#if !defined(NO_PYTHON)
  return (XPubPythonState*)CPythonInterface::InitSession(pPythonToolPath);;
#else // NO_PYTHON
  return 0;
#endif // NO_PYTHON
}
int CXPubPythonWrapperInterface::CloseSession(XPubPythonState* pXPubPythonState)
{
#if !defined(NO_PYTHON)
  return CPythonInterface::CloseSession((PythonState*)pXPubPythonState);
#else // NO_PYTHON
  return 0;
#endif // NO_PYTHON
}

const char* CXPubPythonWrapperInterface::GetLastError()
{
#if !defined(NO_PYTHON)
  if (m_pInterface)
    return m_pInterface->GetLastError();
#endif // NO_PYTHON
  return "";
}

long CXPubPythonWrapperInterface::Function(XPubPythonState *pXPubPythonState,
                                           const xtstring& slibfile,
                                           const xtstring& sFunction,
                                           PythonFunctionParameterList& FuncParaList,
                                           CXPubVariant& cResult)
{
if (!m_pInterface || !pXPubPythonState)
    return 0;
#if !defined(NO_PYTHON)
  return m_pInterface->Function((PythonState*)pXPubPythonState, slibfile, sFunction, FuncParaList, cResult);
#else // NO_PYTHON
  return 0;
#endif // NO_PYTHON
}

int CXPubPythonWrapperInterface::RunFile(XPubPythonState *pXPubPythonState,
                                         xtstring& sModulPathName,
                                         xtstring& sModulName,
                                         xtstring& sOutput)
{
  if (!m_pInterface || !pXPubPythonState)
    return 0;
#if !defined(NO_PYTHON)
  return m_pInterface->RunFile((PythonState*)pXPubPythonState, sModulPathName, sModulName, sOutput);
#else // NO_PYTHON
  return 0;
#endif // NO_PYTHON
}

int CXPubPythonWrapperInterface::CompileFile(XPubPythonState *pXPubPythonState,
                                             xtstring& sModulPathName,
                                             xtstring& sImportName,
                                             xtstring& sOutput)
{
  if (!m_pInterface || !pXPubPythonState)
    return 0;
#if !defined(NO_PYTHON)
  return m_pInterface->CompileFile((PythonState*)pXPubPythonState, sModulPathName, sImportName, sOutput);
#else // NO_PYTHON
  return 0;
#endif // NO_PYTHON
}












CHtmlStoneImageWrapper::CHtmlStoneImageWrapper()
{
#if !defined(NO_PYTHON)
  m_pHtmlStoneImage = new CHtmlStoneImage;
#else // NO_PYTHON
  m_pHtmlStoneImage = 0;
#endif // NO_PYTHON
}
CHtmlStoneImageWrapper::~CHtmlStoneImageWrapper()
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    delete m_pHtmlStoneImage;
#endif // NO_PYTHON
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::Clear()
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->Clear();
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetSrc(xtstring& cSrc)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetSrc(cSrc);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetWidth(xtstring& cWidth)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetWidth(cWidth);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetHeight(xtstring& cHeight)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetHeight(cHeight);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetAlign(xtstring& cAlign)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetAlign(cAlign);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetName(xtstring& cName)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetName(cName);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetAlt(xtstring& cAlt)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetAlt(cAlt);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetBorder(xtstring& cBorder)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetBorder(cBorder);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetLongdesc(xtstring& cLongdesc)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetLongdesc(cLongdesc);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetHSpace(xtstring& cHSpace)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetHSpace(cHSpace);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetVSpace(xtstring& cVSpace)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetVSpace(cVSpace);
#endif // NO_PYTHON
  return *this;
}
CHtmlStoneImageWrapper& CHtmlStoneImageWrapper::SetUsemap(xtstring& cUsemap)
{
#if !defined(NO_PYTHON)
  if (m_pHtmlStoneImage)
    m_pHtmlStoneImage->SetUsemap(cUsemap);
#endif // NO_PYTHON
  return *this;
}
