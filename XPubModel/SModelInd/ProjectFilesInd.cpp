// ProjectFilesInd.cpp : implementation file
//

#include <assert.h>

#include "SSTLdef/STLdef.h"
#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"
#include "STools/Utils.h"

#include "STools/GlobalSettings.h"

#include "ProjectFilesInd.h"





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProjectFilesInd
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProjectFilesInd::CProjectFilesInd()
                 :m_cTestProjectConstants(PROJ_CONSTANTS_TEST),
                  m_cProductionProjectConstants(PROJ_CONSTANTS_PRODUCTION)
{
  m_bProjectFromZIPFile = false;

  m_pDirChecker = NULL;
  m_pOutMsgContainer = NULL;
  m_pGlobalSettings = 0;
}

CProjectFilesInd::~CProjectFilesInd()
{
  RemoveAllFiles();
}

void CProjectFilesInd::SetCallBacks(CDirectoryCheckerCallBack*    pDirChecker,
                                    COutMessageContainerCallBack* pOutMsgContainer)
{
  m_pDirChecker = pDirChecker;
  m_pOutMsgContainer = pOutMsgContainer;

  for (CFileArrayERIterator it = m_cFilesER.begin(); it != m_cFilesER.end(); it++)
    (*it)->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
  for (CFileArrayIFSIterator it = m_cFilesIFS.begin(); it != m_cFilesIFS.end(); it++)
    (*it)->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
  for (CFileArrayStoneIterator it = m_cFilesStone.begin(); it != m_cFilesStone.end(); it++)
    (*it)->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
  for (CFileArrayTemplateIterator it = m_cFilesTemplate.begin(); it != m_cFilesTemplate.end(); it++)
    (*it)->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
  for (CFileArrayPythonModuleIterator it = m_cFilesPythonModule.begin(); it != m_cFilesPythonModule.end(); it++)
    (*it)->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
}

CProjectFileER* CProjectFilesInd::GetERFile(size_t nIndex)
{
  if (GetCountOfERFiles() > nIndex)
    return (m_cFilesER[nIndex]);
  return 0;
}

CProjectFileER* CProjectFilesInd::GetERFile(LPCXTCHAR pName)
{
  for (CFileArrayERIterator it = m_cFilesER.begin(); it != m_cFilesER.end(); it++)
    if ((*it)->GetModuleName() == pName)
      return (*it);
  return 0;
}

CProjectFileIFS* CProjectFilesInd::GetIFSFile(size_t nIndex)
{
  if (GetCountOfIFSFiles() > nIndex)
    return (m_cFilesIFS[nIndex]);
  return 0;
}

CProjectFileIFS* CProjectFilesInd::GetIFSFile(LPCXTCHAR pName)
{
  for (CFileArrayIFSIterator it = m_cFilesIFS.begin(); it != m_cFilesIFS.end(); it++)
    if ((*it)->GetModuleName() == pName)
      return (*it);
  return 0;
}

CProjectFileStone* CProjectFilesInd::GetStoneFile(size_t nIndex)
{
  if (GetCountOfStoneFiles() > nIndex)
    return (m_cFilesStone[nIndex]);
  return 0;
}

CProjectFileStone* CProjectFilesInd::GetStoneFile(LPCXTCHAR pName)
{
  for (CFileArrayStoneIterator it = m_cFilesStone.begin(); it != m_cFilesStone.end(); it++)
    if ((*it)->GetModuleName() == pName)
      return (*it);
  return 0;
}

CProjectFileTemplate* CProjectFilesInd::GetTemplateFile(size_t nIndex)
{
  if (GetCountOfTemplateFiles() > nIndex)
    return (m_cFilesTemplate[nIndex]);
  return 0;
}

CProjectFileTemplate* CProjectFilesInd::GetTemplateFile(LPCXTCHAR pName)
{
  for (CFileArrayTemplateIterator it = m_cFilesTemplate.begin(); it != m_cFilesTemplate.end(); it++)
    if ((*it)->GetModuleName() == pName)
      return (*it);
  return 0;
}

CProjectFilePythonModule* CProjectFilesInd::GetPythonModuleFile(size_t nIndex)
{
  if (GetCountOfPythonModuleFiles() > nIndex)
    return (m_cFilesPythonModule[nIndex]);
  return 0;
}

CProjectFilePythonModule* CProjectFilesInd::GetPythonModuleFile(LPCXTCHAR pName)
{
  for (CFileArrayPythonModuleIterator it = m_cFilesPythonModule.begin(); it != m_cFilesPythonModule.end(); it++)
    if ((*it)->GetModuleName() == pName)
      return (*it);
  return 0;
}

bool CProjectFilesInd::RemoveProjectFileER(size_t nIndex)
{
  if (nIndex >= m_cFilesER.size())
    return false;

  delete m_cFilesER[nIndex];
  m_cFilesER.erase(m_cFilesER.begin() + nIndex);
  return true;
}

bool CProjectFilesInd::RemoveProjectFileIFS(size_t nIndex)
{
  if (nIndex >= m_cFilesIFS.size())
    return false;

  delete m_cFilesIFS[nIndex];
  m_cFilesIFS.erase(m_cFilesIFS.begin() + nIndex);
  return true;
}

bool CProjectFilesInd::RemoveProjectFileStone(size_t nIndex)
{
  if (nIndex >= m_cFilesStone.size())
    return false;

  delete m_cFilesStone[nIndex];
  m_cFilesStone.erase(m_cFilesStone.begin() + nIndex);
  return true;
}

bool CProjectFilesInd::RemoveProjectFileTemplate(size_t nIndex)
{
  if (nIndex >= m_cFilesTemplate.size())
    return false;

  delete m_cFilesTemplate[nIndex];
  m_cFilesTemplate.erase(m_cFilesTemplate.begin() + nIndex);
  return true;
}

bool CProjectFilesInd::RemoveProjectFilePythonModule(size_t nIndex)
{
  if (nIndex >= m_cFilesPythonModule.size())
    return false;

  delete m_cFilesPythonModule[nIndex];
  m_cFilesPythonModule.erase(m_cFilesPythonModule.begin() + nIndex);
  return true;
}








void CProjectFilesInd::RemoveAllFiles()
{
  unsigned int nI;
  for (nI = 0; nI < m_cFilesER.size(); nI++)
    delete m_cFilesER[nI];
  m_cFilesER.clear();
  for (nI = 0; nI < m_cFilesIFS.size(); nI++)
    delete m_cFilesIFS[nI];
  m_cFilesIFS.clear();
  for (nI = 0; nI < m_cFilesStone.size(); nI++)
    delete m_cFilesStone[nI];
  m_cFilesStone.clear();
  for (nI = 0; nI < m_cFilesTemplate.size(); nI++)
    delete m_cFilesTemplate[nI];
  m_cFilesTemplate.clear();
  for (nI = 0; nI < m_cFilesPythonModule.size(); nI++)
    delete m_cFilesPythonModule[nI];
  m_cFilesPythonModule.clear();
}


bool CProjectFilesInd::ReadGUIProperties()
{
  xtstring sFolder = GetProjectFolder();
  xtstring sBaseFileName = GetProjectBaseFileName();

  if (sFolder.size() == 0 || sBaseFileName.size() == 0)
    return false;

  xtstring sFile;
  if (m_bProjectFromZIPFile)
    sFile.clear();
  else
    sFile = sFolder;
  sFile += sBaseFileName;
  sFile += _XT(".xpubgui");

  xtstring sXMLDocText;
  bool bRet;
  if (m_bProjectFromZIPFile)
    bRet = CProjectFileBase::ReadFile_Compressed(GetProjectFullFileName().c_str(), sFile.c_str(), sXMLDocText);
  else
    bRet = CProjectFileBase::ReadFile_Uncompressed(sFile.c_str(), sXMLDocText);
  if (!bRet)
    return false;
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXMLDocText.c_str()))
    return false;
  // we have now XML document
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_TAG_GUIPROJ)
  {
    // opened modules
    if (cXMLDoc.FindChildElem(PROJ_OPENED_MODULES))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <OpenedModules>
      m_cOpenedModules.ReadXMLDoc(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
    // zoom factors
    if (cXMLDoc.FindChildElem(PROJ_ZOOMFACTORS_ER))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <ZoomFactors-ER>
      m_cZoomFactorsER.ReadXMLDoc(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
    if (cXMLDoc.FindChildElem(PROJ_ZOOMFACTORS_IFS))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <ZoomFactors-IFS>
      m_cZoomFactorsIFS.ReadXMLDoc(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
    // last filter
    if (cXMLDoc.FindChildElem(PROJ_LASTFILTER))
    {
      cXMLDoc.IntoElem();
      m_sLastFilterIFS = cXMLDoc.GetAttrib(PROJ_LASTFILTER_IFS);
      cXMLDoc.OutOfElem();
    }
    // last ifs for exec
    if (cXMLDoc.FindChildElem(PROJ_LASTIFS))
    {
      cXMLDoc.IntoElem();
      m_sLastIFSForExec = cXMLDoc.GetAttrib(PROJ_LASTIFSFOREXEC);
      cXMLDoc.OutOfElem();
    }
    return true;
  }
  return false;
}

bool CProjectFilesInd::WriteGUIProperties()
{
  xtstring sFolder = GetProjectFolder();
  xtstring sBaseFileName = GetProjectBaseFileName();

  if (sFolder.size() == 0 || sBaseFileName.size() == 0 || m_bProjectFromZIPFile)
    return false;

  xtstring sFile;
  sFile = sFolder;
  sFile += sBaseFileName;
  sFile += _XT(".xpubgui");

  // XML
  xtstring sXMLDocText = EMPTY_GUIPROJ_XPUB_XML_DOCUMENT;
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLDocText.c_str());

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_GUIPROJ))
  {
    // opened modules
    if (cXMLDoc.AddChildElem(PROJ_OPENED_MODULES, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <OpenedModules>
      m_cOpenedModules.SaveXMLDoc(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
    // zoom factors
    if (cXMLDoc.AddChildElem(PROJ_ZOOMFACTORS_ER, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <ZoomFactors-ER>
      m_cZoomFactorsER.SaveXMLDoc(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
    if (cXMLDoc.AddChildElem(PROJ_ZOOMFACTORS_IFS, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <ZoomFactors-IFS>
      m_cZoomFactorsIFS.SaveXMLDoc(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
    // last filter
    if (cXMLDoc.AddChildElem(PROJ_LASTFILTER, _XT("")))
    {
      cXMLDoc.IntoElem();
      cXMLDoc.SetAttrib(PROJ_LASTFILTER_IFS, m_sLastFilterIFS.c_str());
      cXMLDoc.OutOfElem();
    }
    // last ifs for exec
    if (cXMLDoc.AddChildElem(PROJ_LASTIFS, _XT("")))
    {
      cXMLDoc.IntoElem();
      cXMLDoc.SetAttrib(PROJ_LASTIFSFOREXEC, m_sLastIFSForExec.c_str());
      cXMLDoc.OutOfElem();
    }
    // write out
    sXMLDocText = cXMLDoc.GetDoc();
    if (!CProjectFileBase::WriteFile(sFile.c_str(), sXMLDocText))
      return false;
    return true;
  }
  return false;
}

bool CProjectFilesInd::OnDocumentNew()
{
  m_bProjectFromZIPFile = false;

  WriteGUIProperties();

  RemoveAllFiles();

  // base properties
  SetProjectName(_XT(""));
  SetProjectSubject(_XT(""));
  SetProjectVersion(_XT(""));
  SetProjectFolder(_XT(""));
  SetProjectFileName(_XT(""));
  SetProjectBaseFileName(_XT(""));
  SetProjectFullFileName(_XT(""));

  SetTestFolder(_XT(""));
  // date / time / number settings
  SetLongDateFormat(tLDF_Default);
  SetYearWithCentury(DEFAULT_YEARWITHCENTURY);
  SetDateSeparator(DEFAULT_DATESEPARATOR);
  SetTimeSeparator(DEFAULT_TIMESEPARATOR);
  SetUseSymbolsFromSystem(DEFAULT_USESYMBOLSFROMSYSTEM);
  SetDecimalSymbol(DEFAULT_DECIMALSYMBOL);
  SetDigitGroupingSymbol(DEFAULT_DIGITGROUPINGSYMBOL);
  SetUseCountOfDigitsAfterDecimal(DEFAULT_USECOUNTOFDIGITSAFTERDECIMAL);
  SetCountOfDigitsAfterDecimal(DEFAULT_COUNTOFDIGITSAFTERDECIMAL);
  // gui - opened modules
  m_cOpenedModules.RemoveAll();
  // gui - zoom factors
  m_cZoomFactorsER.RemoveAll();
  m_cZoomFactorsIFS.RemoveAll();
  // gui - last filter
  m_sLastFilterIFS.clear();
  // gui - last ifs for exec
  m_sLastIFSForExec.clear();
  // project constants
  m_cTestProjectConstants.RemoveAll();
  m_cProductionProjectConstants.RemoveAll();
  SetUseTestConstantsForProduction(false);
  // DB Definitions
  m_cDBDefinitions.RemoveAll();
  // external editors
  m_cExternalEditors.ResetAll();
  // extras
  m_nPreviewType = tPreviewType_Default;
  m_sTranslateCodeFile.clear();

  ShowAllInProjectWorkBar();

  // XML
  m_sXMLDocText = EMPTY_PROJ_XPUB_XML_DOCUMENT;

  SetMD5Sum_XML(CProjectFileBase::GetMD5Sum(m_sXMLDocText));
  SetTime_XML(0);

  return true;
}

bool CProjectFilesInd::OnDocumentOpen(LPCXTCHAR lpszPathName,
                                      bool bCreateObjectsImmediately)
{
  // alles entfernen
  RemoveAllFiles();

  // reset
  SetMD5Sum_XML(CProjectFileBase::GetMD5Sum(m_sXMLDocText));
  SetTime_XML(0);

  if (CProjectFileBase::ProjectFileIsCompressed(lpszPathName))
    m_bProjectFromZIPFile = true;
  else
    m_bProjectFromZIPFile = false;


  // estimate folder, file, ...
  xtstring sBaseFolder;
  xtstring sFileName;
  xtstring sBaseFileName;
  xtstring sProjectFile(lpszPathName);
  xtstring sProjectFileNameInCompressedProjectFile;
  {
    size_t nPos = sProjectFile.rfind(_XT('/'));

    if (nPos != xtstring::npos)
    {
      sBaseFolder.assign(sProjectFile, 0, nPos + 1);
      sBaseFileName.assign(sProjectFile.begin() + nPos + 1, sProjectFile.end());
    }
    else
    {
      nPos = sProjectFile.rfind(_XT('\\'));
      if (nPos != xtstring::npos)
      {
        sBaseFolder.assign(sProjectFile, 0, nPos + 1);
        sBaseFileName.assign(sProjectFile.begin() + nPos + 1, sProjectFile.end());
      }
      else
      {
        CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
        cMessage.m_sModule = GetProjectName();
        cMessage.m_sMsgKeyword = GetProjectName();
        cMessage.m_sMsgText = _XT("Project File: '");
        cMessage.m_sMsgText += lpszPathName;
        cMessage.m_sMsgText += _XT("' kein Pfad!");
        cMessage.m_sMsgDescription = _XT("");
        cMessage.m_nNumber = 0;
        if (GetOutMessageContainter())
          GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
        return false;
      }
    }
    sFileName = sBaseFileName;
    nPos = sBaseFileName.find_last_of(_XT("."));
    if (nPos != xtstring::npos)
      sBaseFileName.erase(sBaseFileName.begin() + nPos, sBaseFileName.end());

#define UNPACKED_EXTENSION    _XT(".xpubproj")
#define PACKED_EXTENSION      _XT(".xpubzip")

    sProjectFileNameInCompressedProjectFile = sFileName;
    nPos = sFileName.rfind(PACKED_EXTENSION);
    if (nPos != xtstring::npos)
    {
      sProjectFileNameInCompressedProjectFile.erase(sProjectFileNameInCompressedProjectFile.begin() + nPos,
                                                    sProjectFileNameInCompressedProjectFile.end());
      sProjectFileNameInCompressedProjectFile += UNPACKED_EXTENSION;
      // !!! Wenn wir da sind, wir haben ZIP Datei.
      // Das bedeutet, in diesem Moment haben wir in sBaseFileName so was "BiblioLoop.xpubproj"
      // Wir muessen noch 'xpubproj' entfernen
      nPos = sBaseFileName.find_last_of(_XT("."));
      if (nPos != xtstring::npos)
        sBaseFileName.erase(sBaseFileName.begin() + nPos, sBaseFileName.end());
    }
  }


  // read file
  bool bRes;
  // Vorsicht. Die Variablen sind noch nicht gesetzt.
  if (m_bProjectFromZIPFile)
    bRes = CProjectFileBase::ReadFile_Compressed(/*GetProjectFullFileName()*/sProjectFile.c_str(),
                                                 /*GetProjectFileName()*/sProjectFileNameInCompressedProjectFile.c_str(),
                                                 m_sXMLDocText);
  else
    bRes = CProjectFileBase::ReadFile_Uncompressed(/*GetProjectFileName()*/sProjectFile.c_str(),
                                                   m_sXMLDocText);
  if (!bRes)
  {
    if (GetOutMessageContainter())
    {
      CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
      cMessage.m_sModule = GetProjectName();
      cMessage.m_sMsgKeyword = GetProjectName();
      cMessage.m_sMsgText = _XT("Project File: '");
      cMessage.m_sMsgText += lpszPathName;
      cMessage.m_sMsgText += _XT("' ReadFile failed.");
      cMessage.m_sMsgDescription = _XT("");
      cMessage.m_nNumber = 0;
      if (GetOutMessageContainter())
        GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
    }
    return false;
  }

  SetMD5Sum_XML(CProjectFileBase::GetMD5Sum(m_sXMLDocText));
  time_t timeChanged;
  CProjectFileBase::GetFileTimeChanged(lpszPathName, timeChanged);
  SetTime_XML(timeChanged);

  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(m_sXMLDocText.c_str()))
  {
    if (GetOutMessageContainter())
    {
      CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
      cMessage.m_sModule = GetProjectName();
      cMessage.m_sMsgKeyword = GetProjectName();
      cMessage.m_sMsgText = _XT("Project File: SetDocument failed.");
      cMessage.m_sMsgDescription = _XT("");
      cMessage.m_nNumber = 0;
      if (GetOutMessageContainter())
        GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
    }
    return false;
  }




#if defined(WIN32)
  // in Python gibt es Probleme beim Laden der Module, wenn die Verzeichnisnamen des �ergebenen Pfades
  // kleine Buchstaben im ersten Buchstaben des Verzeichnisnamen haben. Aus diesem Grund setzen wir alles
  // in Gro�uchstaben!
  // MAK: Keine Ahnung was das sein soll!
  sBaseFolder.MakeUpper();
#endif // WIN32

  SetProjectFolder(sBaseFolder);
  SetProjectFileName(sFileName);
  SetProjectBaseFileName(sBaseFileName);
  SetProjectFullFileName(sProjectFile);
  ReadGUIProperties();

  xtstring sProperty;
  long nProperty;

  // we have now XML document
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_PROJ)
  {
    // XMLDoc in <X-Publisher-Project>

    // base properties
    sProperty = cXMLDoc.GetAttrib(PROJ_PROJECTNAME);
    SetProjectName(sProperty);
    sProperty = cXMLDoc.GetAttrib(PROJ_PROJECTSUBJECT);
    SetProjectSubject(sProperty);
    sProperty = cXMLDoc.GetAttrib(PROJ_PROJECTVERSION);
    SetProjectVersion(sProperty);
    sProperty = cXMLDoc.GetAttrib(PROJ_TESTFOLDER);
    SetTestFolder(sProperty);
    // date / time / number settings
    TLongDateFormat nFormat = (TLongDateFormat)cXMLDoc.GetAttribLong(PROJ_DTN_LONGDATEFORMAT);
    if (nFormat <= tLDFFirst_Dummy || nFormat >= tLDFLast_Dummy)
      nFormat = tLDF_Default;
    SetLongDateFormat(nFormat);
    nProperty = cXMLDoc.GetAttribLong(PROJ_DTN_YEARWITHCENTURY);
    SetYearWithCentury(nProperty != 0);
    sProperty = cXMLDoc.GetAttrib(PROJ_DTN_DATESEPARATOR);
    SetDateSeparator(sProperty);
    sProperty = cXMLDoc.GetAttrib(PROJ_DTN_TIMESEPARATOR);
    SetTimeSeparator(sProperty);
    nProperty = cXMLDoc.GetAttribLong(PROJ_DTN_USESYMBOLSFROMSYSTEM);
    SetUseSymbolsFromSystem(nProperty != 0);
    sProperty = cXMLDoc.GetAttrib(PROJ_DTN_DECIMALSYMBOL);
    SetDecimalSymbol(sProperty);
    sProperty = cXMLDoc.GetAttrib(PROJ_DTN_DIGITGROUPINGSYMBOL);
    SetDigitGroupingSymbol(sProperty);
    nProperty = cXMLDoc.GetAttribLong(PROJ_DTN_USECOUNTOFDIGITSAFTERDECIMAL);
    SetUseCountOfDigitsAfterDecimal(nProperty != 0);
    nProperty = cXMLDoc.GetAttribLong(PROJ_DTN_COUNTOFDIGITSAFTERDECIMAL);
    SetCountOfDigitsAfterDecimal(nProperty);

    if (!m_pGlobalSettings)
      m_pGlobalSettings = new CGS_DateTimeNumber(CGlobalSettings::tGSU_Set);
    if (m_pGlobalSettings)
    {
      m_pGlobalSettings->SetLongDateFormat((CGS_DateTimeNumber::TLongDateFormat)GetLongDateFormat());
      m_pGlobalSettings->SetYearWithCentury(GetYearWithCentury());
      m_pGlobalSettings->SetDateSeparator((string&)GetDateSeparator());
      m_pGlobalSettings->SetTimeSeparator((string&)GetTimeSeparator());
      m_pGlobalSettings->SetDecimalSymbol((string&)GetDecimalSymbol());
      m_pGlobalSettings->SetDigitGroupingSymbol((string&)GetDigitGroupingSymbol());
      m_pGlobalSettings->SetUseCountOfDigitsAfterDecimal(GetUseCountOfDigitsAfterDecimal());
      m_pGlobalSettings->SetCountOfDigitsAfterDecimal(GetCountOfDigitsAfterDecimal());
      m_pGlobalSettings->WriteGlobalSettings();
    }

    // project constants
    nProperty = cXMLDoc.GetAttribLong(PROJ_USE_TEST_CONSTANTS_FOR_PRODUCTION);
    SetUseTestConstantsForProduction(nProperty != false);
    if (cXMLDoc.FindChildElem(PROJ_CONSTANTSALL_NAME))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <Constants>
      m_cTestProjectConstants.ReadXMLDoc(&cXMLDoc);
      m_cProductionProjectConstants.ReadXMLDoc(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
    // DB Definitions
    m_cDBDefinitions.ReadXMLDoc(&cXMLDoc);
    // external editors
    m_cExternalEditors.ReadXMLDoc(&cXMLDoc);
    // extras
    TPreviewType nPreviewType = (TPreviewType)cXMLDoc.GetAttribLong(PROJ_PREVIEW_TYPE);
    if (nPreviewType <= tPreviewType_FirstDummy || nPreviewType >=tPreviewType_LastDummy)
      nPreviewType = tPreviewType_Default;
    SetPreviewType(nPreviewType);
    SetTranslateCodeFile(cXMLDoc.GetAttrib(PROJ_TRANSLATECODEFILE));

    // er files einlesen
    if (cXMLDoc.FindChildElem(ER_FILES_NAME))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <ER-Files>
      while (cXMLDoc.FindChildElem(ER_FILE_NAME))
      {
        cXMLDoc.IntoElem();
        // XMLDoc in <ER-File>
        CProjectFileER* pERFile;
        pERFile = new CProjectFileER(GetBaseNameForER(), bCreateObjectsImmediately);
        pERFile->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
        if (pERFile->ReadXMLDocAndFile(sBaseFolder, &cXMLDoc, m_bProjectFromZIPFile ? GetProjectFullFileName() : _XT("")))
        {
          // erfolg -> in liste anhaengen
          m_cFilesER.push_back(pERFile);
        }
        else
        {
          if (GetOutMessageContainter())
          {
            CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
            cMessage.m_sModule = GetProjectName();
            cMessage.m_sMsgKeyword = GetProjectName();
            cMessage.m_sMsgText = _XT("File not loaded");
            cMessage.m_sMsgDescription = _XT("");
            cMessage.m_nNumber = 0;
            if (GetOutMessageContainter())
              GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
          }
          delete pERFile;
        }
        cXMLDoc.OutOfElem();
        // XMLDoc in <ER-Files>
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

    // ifs files einlesen
    if (cXMLDoc.FindChildElem(IFS_FILES_NAME))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <IFS-Files>
      while (cXMLDoc.FindChildElem(IFS_FILE_NAME))
      {
        cXMLDoc.IntoElem();
        // XMLDoc in <IFS-File>
        CProjectFileIFS* pIFSFile;
        pIFSFile = new CProjectFileIFS(GetBaseNameForIFS(), bCreateObjectsImmediately);
        pIFSFile->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
        if (pIFSFile->ReadXMLDocAndFile(sBaseFolder, &cXMLDoc, m_bProjectFromZIPFile ? GetProjectFullFileName() : _XT("")))
        {
          // erfolg -> in liste anhaengen
          m_cFilesIFS.push_back(pIFSFile);
        }
        else
        {
          if (GetOutMessageContainter())
          {
            CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
            cMessage.m_sModule = GetProjectName();
            cMessage.m_sMsgKeyword = GetProjectName();
            cMessage.m_sMsgText = _XT("File not loaded");
            cMessage.m_sMsgDescription = _XT("");
            cMessage.m_nNumber = 0;
            if (GetOutMessageContainter())
              GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
          }
          delete pIFSFile;
        }
        cXMLDoc.OutOfElem();
        // XMLDoc in <IFS-Files>
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

    // stone files einlesen
    if (cXMLDoc.FindChildElem(STONE_FILES_NAME))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <Stone-Files>
      while (cXMLDoc.FindChildElem(STONE_FILE_NAME))
      {
        cXMLDoc.IntoElem();
        // XMLDoc in <Stone-File>
        CProjectFileStone* pHTMLStoneFile;
        pHTMLStoneFile = new CProjectFileStone(GetBaseNameForStone(), bCreateObjectsImmediately);
        pHTMLStoneFile->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
        if (pHTMLStoneFile->ReadXMLDocAndFile(sBaseFolder, &cXMLDoc, m_bProjectFromZIPFile ? GetProjectFullFileName() : _XT("")))
        {
          // erfolg -> in liste anhaengen
          m_cFilesStone.push_back(pHTMLStoneFile);
        }
        else
        {
          if (GetOutMessageContainter())
          {
            CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
            cMessage.m_sModule = GetProjectName();
            cMessage.m_sMsgKeyword = GetProjectName();
            cMessage.m_sMsgText = _XT("File not loaded");
            cMessage.m_sMsgDescription = _XT("");
            cMessage.m_nNumber = 0;
            if (GetOutMessageContainter())
              GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
          }
          delete pHTMLStoneFile;
        }
        cXMLDoc.OutOfElem();
        // XMLDoc in <Stone-Files>
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

    // template files einlesen
    if (cXMLDoc.FindChildElem(TEMPLATE_FILES_NAME))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <Template-Files>
      while (cXMLDoc.FindChildElem(TEMPLATE_FILE_NAME))
      {
        cXMLDoc.IntoElem();
        // XMLDoc in <Template-File>
        CProjectFileTemplate* pHTMLTempFile;
        pHTMLTempFile = new CProjectFileTemplate(GetBaseNameForTemplate(), bCreateObjectsImmediately);
        pHTMLTempFile->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
        if (pHTMLTempFile->ReadXMLDocAndFile(sBaseFolder, &cXMLDoc, m_bProjectFromZIPFile ? GetProjectFullFileName() : _XT("")))
        {
          // erfolg -> in liste anhaengen
          m_cFilesTemplate.push_back(pHTMLTempFile);
        }
        else
        {
          if (GetOutMessageContainter())
          {
            CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
            cMessage.m_sModule = GetProjectName();
            cMessage.m_sMsgKeyword = GetProjectName();
            cMessage.m_sMsgText = _XT("File not loaded");
            cMessage.m_sMsgDescription = _XT("");
            cMessage.m_nNumber = 0;
            if (GetOutMessageContainter())
              GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
          }
          delete pHTMLTempFile;
        }
        cXMLDoc.OutOfElem();
        // XMLDoc in <Template-Files>
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

    // python module files einlesen
    if (cXMLDoc.FindChildElem(PYTHON_FILES_NAME))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <Python-Files>
      while (cXMLDoc.FindChildElem(PYTHON_FILE_NAME))
      {
        cXMLDoc.IntoElem();
        // XMLDoc in <Python-File>
        CProjectFilePythonModule* pPythonFile;
        pPythonFile = new CProjectFilePythonModule(GetBaseNameForPythonModule(), bCreateObjectsImmediately);
        pPythonFile->SetCallBacks(m_pDirChecker, m_pOutMsgContainer);
        if (pPythonFile->ReadXMLDocAndFile(sBaseFolder, &cXMLDoc, m_bProjectFromZIPFile ? GetProjectFullFileName() : _XT("")))
        {
          // erfolg -> in liste anhaengen
          m_cFilesPythonModule.push_back(pPythonFile);
        }
        else
        {
          if (GetOutMessageContainter())
          {
            CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
            cMessage.m_sModule = GetProjectName();
            cMessage.m_sMsgKeyword = GetProjectName();
            cMessage.m_sMsgText = _XT("File not loaded");
            cMessage.m_sMsgDescription = _XT("");
            cMessage.m_nNumber = 0;
            if (GetOutMessageContainter())
              GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
          }
          delete pPythonFile;
        }
        cXMLDoc.OutOfElem();
        // XMLDoc in <Python-Files>
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
  }
  else
  {
    if (GetOutMessageContainter())
    {
      CExecutionMessage cMessage(CExecutionMessage::tMessageError, tMessageSourceProjectLoad);
      cMessage.m_sModule = GetProjectName();
      cMessage.m_sMsgKeyword = GetProjectName();
      cMessage.m_sMsgText = _XT("Project File: bad format");
      cMessage.m_sMsgDescription = _XT("");
      cMessage.m_nNumber = 0;
      if (GetOutMessageContainter())
        GetOutMessageContainter()->ShowOutMessage(0, &cMessage);
    }
    return false;
  }

  ScanAfterAddOrDelWorkPaper();
  ShowAllInProjectWorkBar();

  return true;
}

bool CProjectFilesInd::OnDocumentSave(LPCXTCHAR lpszPathName)
{
//MAK: Das muss noch geändert werden!
	bool bReturn=true;

  if (m_bProjectFromZIPFile)
    // In diesem Fall koennen wir nichts abspeichern.
    // (Mindestens zur Zeit.)
    return true;

  // base folder
  xtstring sBaseFolder;
  xtstring sFileName;
  xtstring sBaseFileName;
  xtstring sProjectFile(lpszPathName);
  size_t nPos = sProjectFile.rfind(_XT('/'));
  if (nPos != xtstring::npos)
  {
    sBaseFolder.assign(sProjectFile, 0, nPos + 1);
    sBaseFileName.assign(sProjectFile.begin() + nPos + 1, sProjectFile.end());
  }
  else
  {
    nPos = sProjectFile.rfind(_XT('\\'));
    if (nPos != xtstring::npos)
    {
      sBaseFolder.assign(sProjectFile, 0, nPos + 1);
      sBaseFileName.assign(sProjectFile.begin() + nPos + 1, sProjectFile.end());
    }
    else
    {
      // !!!!
      sBaseFolder = sProjectFile;
      return false;
    }
  }
  sFileName = sBaseFileName;
  nPos = sBaseFileName.find_last_of(_XT("."));
  if (nPos != xtstring::npos)
    sBaseFileName.erase(sBaseFileName.begin() + nPos, sBaseFileName.end());

  SetProjectFolder(sBaseFolder);
  SetProjectFileName(sFileName);
  SetProjectBaseFileName(sBaseFileName);
  SetProjectFullFileName(sFileName);
  WriteGUIProperties();

  // XML
  m_sXMLDocText = EMPTY_PROJ_XPUB_XML_DOCUMENT;
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(m_sXMLDocText.c_str());

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_PROJ))
  {
    // XMLDoc in <X-Publisher-Project>

    // base properties
    cXMLDoc.SetAttrib(PROJ_PROJECTNAME,     GetProjectName().c_str());
    cXMLDoc.SetAttrib(PROJ_PROJECTSUBJECT,  GetProjectSubject().c_str());
    cXMLDoc.SetAttrib(PROJ_PROJECTVERSION,  GetProjectVersion().c_str());
    cXMLDoc.SetAttrib(PROJ_TESTFOLDER,      GetTestFolder().c_str());
    // date / time / number settings
    cXMLDoc.SetAttrib(PROJ_DTN_LONGDATEFORMAT,                GetLongDateFormat());
    cXMLDoc.SetAttrib(PROJ_DTN_YEARWITHCENTURY,               GetYearWithCentury());
    cXMLDoc.SetAttrib(PROJ_DTN_DATESEPARATOR,                 GetDateSeparator().c_str());
    cXMLDoc.SetAttrib(PROJ_DTN_TIMESEPARATOR,                 GetTimeSeparator().c_str());
    cXMLDoc.SetAttrib(PROJ_DTN_USESYMBOLSFROMSYSTEM,          GetUseSymbolsFromSystem());
    cXMLDoc.SetAttrib(PROJ_DTN_DECIMALSYMBOL,                 GetDecimalSymbol().c_str());
    cXMLDoc.SetAttrib(PROJ_DTN_DIGITGROUPINGSYMBOL,           GetDigitGroupingSymbol().c_str());
    cXMLDoc.SetAttrib(PROJ_DTN_USECOUNTOFDIGITSAFTERDECIMAL,  GetUseCountOfDigitsAfterDecimal());
    cXMLDoc.SetAttrib(PROJ_DTN_COUNTOFDIGITSAFTERDECIMAL,     GetCountOfDigitsAfterDecimal());

    // project constants
    cXMLDoc.SetAttrib(PROJ_USE_TEST_CONSTANTS_FOR_PRODUCTION, GetUseTestConstantsForProduction());
    if (cXMLDoc.AddChildElem(PROJ_CONSTANTSALL_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <Constants>
      m_cTestProjectConstants.SaveXMLDoc(&cXMLDoc);
      m_cProductionProjectConstants.SaveXMLDoc(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }
    // DB Definitions
    m_cDBDefinitions.SaveXMLDoc(&cXMLDoc);
    // external editors
    m_cExternalEditors.SaveXMLDoc(&cXMLDoc);
    // extras
    cXMLDoc.SetAttrib(PROJ_PREVIEW_TYPE, GetPreviewType());
    cXMLDoc.SetAttrib(PROJ_TRANSLATECODEFILE, GetTranslateCodeFile().c_str());

    // er files speichern
    if (cXMLDoc.AddChildElem(ER_FILES_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <ER-Files>
      for (size_t nI = 0; nI < m_cFilesER.size(); nI++)
      {
				bReturn &= m_cFilesER[nI]->SaveXMLDocAndFile(sBaseFolder, &cXMLDoc);
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

    // ifs files speichern
    if (cXMLDoc.AddChildElem(IFS_FILES_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <IFS-Files>
      for (size_t nI = 0; nI < m_cFilesIFS.size(); nI++)
      {
        bReturn &= m_cFilesIFS[nI]->SaveXMLDocAndFile(sBaseFolder, &cXMLDoc);
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

    // html files speichern
    if (cXMLDoc.AddChildElem(STONE_FILES_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <HTML-Files>
      for (size_t nI = 0; nI < m_cFilesStone.size(); nI++)
      {
        bReturn &= m_cFilesStone[nI]->SaveXMLDocAndFile(sBaseFolder, &cXMLDoc);
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

    // htmltemp files speichern
    if (cXMLDoc.AddChildElem(TEMPLATE_FILES_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <HTMLTemp-Files>
      for (size_t nI = 0; nI < m_cFilesTemplate.size(); nI++)
      {
        bReturn &= m_cFilesTemplate[nI]->SaveXMLDocAndFile(sBaseFolder, &cXMLDoc);
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

    // python module files speichern
    if (cXMLDoc.AddChildElem(PYTHON_FILES_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // XMLDoc in <Python-Files>
      for (size_t nI = 0; nI < m_cFilesPythonModule.size(); nI++)
      {
        bReturn &= m_cFilesPythonModule[nI]->SaveXMLDocAndFile(sBaseFolder, &cXMLDoc);
      }
      cXMLDoc.OutOfElem();
      // XMLDoc in <X-Publisher-Project>
    }

  }
  else
    assert(false);

  // write out
  m_sXMLDocText = cXMLDoc.GetDoc();
  if (!CProjectFileBase::WriteFile(lpszPathName, m_sXMLDocText))
    return false;

  SetMD5Sum_XML(CProjectFileBase::GetMD5Sum(m_sXMLDocText));
  time_t timeChanged;
  CProjectFileBase::GetFileTimeChanged(lpszPathName, timeChanged);
  SetTime_XML(timeChanged);

  return bReturn;
}

void CProjectFilesInd::OnDocumentClose()
{
  m_bProjectFromZIPFile = false;

  WriteGUIProperties();
  RemoveAllFiles();

  if (m_pGlobalSettings)
    delete m_pGlobalSettings;
  m_pGlobalSettings = 0;
}



bool CProjectFilesInd::GetIFSStruct(TIFSTypeForThirdParty nType,
                                    xtstring& sXMLDoc)
{
  bool bRet = true;
  COpenedItemsContainer cStruct;

  for (size_t nI = 0; nI < GetCountOfIFSFiles(); nI++)
  {
    CProjectFileIFS* pProjFile = GetIFSFile(nI);
    if (!pProjFile)
      continue;

    CMPModelIFSInd* pMainProperty;
    pMainProperty = pProjFile->GetMPModelIFSInd();
    if (!pMainProperty)
      return false;

    TIFSTypeForThirdParty nIFSType = pMainProperty->GetIFSTypeForThirdParty();
    xtstring sIFSNameForThirdParty = pMainProperty->GetIFSNameForThirdParty();
    xtstring sName = pMainProperty->GetName();
    bool bShowHierarchical = pMainProperty->GetShowOpenedEntitiesHierarchical();
    bool bShowSubIFS = pMainProperty->GetShowOpenedNameAlways(); // das hat sinn nur bei SubIFS
    bool bShowExpanded = pMainProperty->GetShowExpanded();
    TShowBehaviour nShowBehaviour = pMainProperty->GetShowBehaviour();

    if (nIFSType == nType)
    {
      cStruct.InsertLevel_InformationSight(sIFSNameForThirdParty,
                                           sName,
                                           bShowSubIFS,
                                           bShowHierarchical,
                                           bShowExpanded,
                                           nShowBehaviour,
                                           true,
                                           false);
      cStruct.InsertActualLevel(0, 0, 0); // in modul keine Bedingungen
      cStruct.LeaveLevel();
      if (!bRet)
        break;
    }
  }

  sXMLDoc = cStruct.GetXMLDoc();
  return bRet;
}

bool CProjectFilesInd::GetEntitiesStruct(TIFSTypeForThirdParty nType,
                                         xtstring& sXMLDoc)
{
  bool bRet = true;
  COpenedItemsContainer cStruct;

  for (size_t nI = 0; nI < GetCountOfIFSFiles(); nI++)
  {
    CProjectFileIFS* pProjFile = GetIFSFile(nI);
    if (!pProjFile)
      continue;

    CMPModelIFSInd* pMainProperty;
    pMainProperty = pProjFile->GetMPModelIFSInd();
    if (!pMainProperty)
      return false;

    TIFSTypeForThirdParty nIFSType = pMainProperty->GetIFSTypeForThirdParty();
    xtstring sIFSNameForThirdParty = pMainProperty->GetIFSNameForThirdParty();
    xtstring sName = pMainProperty->GetName();
    bool bShowHierarchical = pMainProperty->GetShowOpenedEntitiesHierarchical();
    bool bShowSubIFS = pMainProperty->GetShowOpenedNameAlways(); // das hat sinn nur bei SubIFS
    bool bShowExpanded = pMainProperty->GetShowExpanded();
    TShowBehaviour nShowBehaviour = pMainProperty->GetShowBehaviour();

    if (nIFSType == nType)
    {
      cStruct.InsertLevel_InformationSight(sIFSNameForThirdParty,
                                           sName,
                                           bShowSubIFS,
                                           bShowHierarchical,
                                           bShowExpanded,
                                           nShowBehaviour,
                                           true,
                                           false);
      cStruct.InsertActualLevel(0, 0, 0); // in modul keine Bedingungen
      bRet = MakeOpenedEntitiesPath(pProjFile, _XT(""), &cStruct);
      cStruct.LeaveLevel();
      if (!bRet)
        break;
    }
  }

  sXMLDoc = cStruct.GetXMLDoc();
  return bRet;
}

bool CProjectFilesInd::GetEntitiesStruct(TIFSTypeForThirdParty nType,
                                         const xtstring& sIFSName,
                                         xtstring& sXMLDoc)
{
  CProjectFileIFS* pProjFile = GetIFSFile(sIFSName.c_str());
  if (!pProjFile)
    return false;

  CMPModelIFSInd* pMainProperty;
  pMainProperty = pProjFile->GetMPModelIFSInd();
  if (!pMainProperty)
    return false;

  TIFSTypeForThirdParty nIFSType = pMainProperty->GetIFSTypeForThirdParty();
  xtstring sIFSNameForThirdParty = pMainProperty->GetIFSNameForThirdParty();
  xtstring sName = pMainProperty->GetName();
  bool bShowHierarchical = pMainProperty->GetShowOpenedEntitiesHierarchical();
  bool bShowSubIFS = pMainProperty->GetShowOpenedNameAlways(); // das hat sinn nur bei SubIFS
  bool bShowExpanded = pMainProperty->GetShowExpanded();
  TShowBehaviour nShowBehaviour = pMainProperty->GetShowBehaviour();

  COpenedItemsContainer cStruct;

  bool bRet = true;
  if (nIFSType == nType)
  {
    cStruct.InsertLevel_InformationSight(sIFSNameForThirdParty,
                                         sName,
                                         bShowSubIFS,
                                         bShowHierarchical,
                                         bShowExpanded,
                                         nShowBehaviour,
                                         true,
                                         false);
    cStruct.InsertActualLevel(0, 0, 0); // in modul keine Bedingungen
    bRet = MakeOpenedEntitiesPath(pProjFile, _XT(""), &cStruct);
    cStruct.LeaveLevel();
  }

  sXMLDoc = cStruct.GetXMLDoc();
  return bRet;
}

bool CProjectFilesInd::MakeOpenedEntitiesPath(CProjectFileIFS* pProjFile,
                                              const xtstring& sEntityName,
                                              COpenedItemsContainer* pStruct)
{
  CIFSEntityInd* pStartEntity = 0;
  if (sEntityName.size())
    pStartEntity = pProjFile->GetIFSEntityInd(sEntityName.c_str());
  else
    pStartEntity = pProjFile->GetMainIFSEntityInd();

  if (!pStartEntity)
    return false;

  bool bLevelAdded = false;
  if (pStartEntity->GetOpenedEntity())
  {
    // geoeffnete Entitaet -> add
    if (pStartEntity->GetSubIFS().size())
      pStruct->InsertLevel_OpenedEntityForSubIFS(pStartEntity->GetOpenedEntityName(),
                                                 pStartEntity->GetName(),
                                                 pStartEntity->GetShowExpanded(),
                                                 pStartEntity->GetShowBehaviour(),
                                                 pStartEntity->GetShowAddCondition(),
                                                 pStartEntity->GetShowAddFreeSelection());
    else
      pStruct->InsertLevel_OpenedEntity(pStartEntity->GetOpenedEntityName(),
                                        pStartEntity->GetName(),
                                        pStartEntity->GetShowExpanded(),
                                        pStartEntity->GetShowBehaviour(),
                                        pStartEntity->GetShowAddCondition(),
                                        pStartEntity->GetShowAddFreeSelection());
    pStruct->InsertActualLevel(pStartEntity->GetConditionVariations(),
                               pStartEntity->GetEntityParameters(),
                               pStartEntity->GetSQLCommandVariations());
    bLevelAdded = true;
  }

typedef std::vector<CIFSEntityInd*>         CIFSEntityVector;
typedef CIFSEntityVector::iterator          CIFSEntityVectorIterator;
  CIFSEntityVector allChilds;
  // alle childs holen
  for (size_t nI = 0; nI < pStartEntity->CountOfConnectionsToChilds(); nI++)
  {
    CIFSEntityConnectionInd* pConn = pStartEntity->GetConnectionToChilds(nI);
    CIFSEntityInd* pChEntity = pProjFile->GetIFSEntityInd(pConn->GetChildEntity().c_str());
    allChilds.push_back(pChEntity);
  }
  // childs sortieren
  if (allChilds.size() > 1)
  {
    bool bChanged = true;
    while (bChanged)
    {
      bChanged = false;
      for (CIFSEntityVectorIterator itOut = allChilds.begin();
          itOut != allChilds.end() - 1;
          itOut++)
      {
        for (CIFSEntityVectorIterator itIn = itOut + 1;
            itIn != allChilds.end();
            itIn++)
        {
#ifdef _DEBUG
          xtstring sOutName;
          xtstring sInName;
          int nOutLeft;
          int nInLoft;
          sOutName = (*itOut)->GetName();
          sInName = (*itIn)->GetName();
          nOutLeft = (*itOut)->GetLeft();
          nInLoft = (*itIn)->GetLeft();
#endif // _DEBUG
          if ((*itOut)->GetLeft() > (*itIn)->GetLeft())
          {
            CIFSEntityInd* p = *itOut;
            *itOut = *itIn;
            *itIn = p;
            bChanged = true;
            break;
          }
        }
        if (bChanged)
          break;
      }
    }
  }
  bool bRet = true;
  // childs verarbeiten
  for (CIFSEntityVectorIterator it = allChilds.begin(); it != allChilds.end(); it++)
  {
    bRet = MakeOpenedEntitiesPath(pProjFile, (*it)->GetName(), pStruct);
    if (!bRet)
      break;
  }
  allChilds.erase(allChilds.begin(), allChilds.end());

  // test SubIFS
  if (pStartEntity->GetSubIFS().size())
  {
    // subifs eingestellt
    if (!bLevelAdded)
    {
      pStruct->InsertLevel_EntityForSubIFS(pStartEntity->GetOpenedEntityName(),
                                           pStartEntity->GetName(),
                                           pStartEntity->GetShowExpanded(),
                                           pStartEntity->GetShowBehaviour(),
                                           pStartEntity->GetShowAddCondition(),
                                           pStartEntity->GetShowAddFreeSelection());
      pStruct->InsertActualLevel(pStartEntity->GetConditionVariations(),
                                 pStartEntity->GetEntityParameters(),
                                 pStartEntity->GetSQLCommandVariations());
      bLevelAdded = true;
    }
    // in SubIFS weiter fahren
    CProjectFileIFS* pPF = GetIFSFile(pStartEntity->GetSubIFS().c_str());
    if (!pPF)
    {
      bRet = false;
    }
    else
    {
      CMPModelIFSInd* pMainProperty;
      pMainProperty = pPF->GetMPModelIFSInd();
      if (!pMainProperty)
        bRet = false;
      {
        xtstring sIFSNameForThirdParty = pMainProperty->GetIFSNameForThirdParty();
        xtstring sName = pMainProperty->GetName();
        bool bShowHierarchical = pMainProperty->GetShowOpenedEntitiesHierarchical();
        bool bShowSubIFS = pMainProperty->GetShowOpenedNameAlways(); // das hat sinn nur bei SubIFS
        bool bShowExpanded = pMainProperty->GetShowExpanded();
        TShowBehaviour nShowBehaviour = pMainProperty->GetShowBehaviour();

        xtstring sMainEntityName;
        CIFSEntityInd* pMain = pPF->GetMainIFSEntityInd();
        if (!pMain)
          bRet = false;
        else
        {
          sMainEntityName = pMain->GetName();

          pStruct->InsertLevel_InformationSight(sIFSNameForThirdParty,
                                                sName,
                                                bShowSubIFS,
                                                bShowHierarchical,
                                                bShowExpanded,
                                                nShowBehaviour,
                                                true,
                                                false);
          pStruct->InsertActualLevel(0, 0, 0);
          bRet = MakeOpenedEntitiesPath(pPF, sMainEntityName, pStruct);
          pStruct->LeaveLevel();
        }
      }
    }
  }

  if (bLevelAdded)
    pStruct->LeaveLevel();

  return bRet;
}

void CProjectFilesInd::SetForSettings(CProjectFilesInd& cPFiles)
{
  cPFiles.SetProjectFolder(GetProjectFolder());

  // base properties
  cPFiles.SetProjectName(GetProjectName());
  cPFiles.SetProjectSubject(GetProjectSubject());
  cPFiles.SetProjectVersion(GetProjectVersion());
  cPFiles.SetTestFolder(GetTestFolder());
  // date / time / number settings
  cPFiles.SetLongDateFormat(GetLongDateFormat());
  cPFiles.SetYearWithCentury(GetYearWithCentury());
  cPFiles.SetDateSeparator(GetDateSeparator());
  cPFiles.SetTimeSeparator(GetTimeSeparator());
  cPFiles.SetUseSymbolsFromSystem(GetUseSymbolsFromSystem());
  cPFiles.SetDecimalSymbol(GetDecimalSymbol());
  cPFiles.SetDigitGroupingSymbol(GetDigitGroupingSymbol());
  cPFiles.SetUseCountOfDigitsAfterDecimal(GetUseCountOfDigitsAfterDecimal());
  cPFiles.SetCountOfDigitsAfterDecimal(GetCountOfDigitsAfterDecimal());
  // project constants
  cPFiles.SetUseTestConstantsForProduction(GetUseTestConstantsForProduction());
  cPFiles.SetTestConstants(GetTestConstants());
  cPFiles.SetProductionConstants(GetProductionConstants());
  // DB Definitions
  cPFiles.AssignDBDefinitions(GetDBDefinitions());
  // external editors
  cPFiles.AssignExternalEditors(GetExternalEditors());
  // extras
  cPFiles.SetPreviewType(GetPreviewType());
  cPFiles.SetTranslateCodeFile(GetTranslateCodeFile());
}

void CProjectFilesInd::SetFromSettings(CProjectFilesInd& cPFiles)
{
  SetProjectFolder(cPFiles.GetProjectFolder());

  // base properties
  SetProjectName(cPFiles.GetProjectName());
  SetProjectSubject(cPFiles.GetProjectSubject());
  SetProjectVersion(cPFiles.GetProjectVersion());
  SetTestFolder(cPFiles.GetTestFolder());
  // date / time / number settings
  SetLongDateFormat(cPFiles.GetLongDateFormat());
  SetYearWithCentury(cPFiles.GetYearWithCentury());
  SetDateSeparator(cPFiles.GetDateSeparator());
  SetTimeSeparator(cPFiles.GetTimeSeparator());
  SetUseSymbolsFromSystem(cPFiles.GetUseSymbolsFromSystem());
  SetDecimalSymbol(cPFiles.GetDecimalSymbol());
  SetDigitGroupingSymbol(cPFiles.GetDigitGroupingSymbol());
  SetUseCountOfDigitsAfterDecimal(cPFiles.GetUseCountOfDigitsAfterDecimal());
  SetCountOfDigitsAfterDecimal(cPFiles.GetCountOfDigitsAfterDecimal());
  // project constants
  SetUseTestConstantsForProduction(cPFiles.GetUseTestConstantsForProduction());
  SetTestConstants(cPFiles.GetTestConstants());
  SetProductionConstants(cPFiles.GetProductionConstants());
  // DB Definitions
  AssignDBDefinitions(cPFiles.GetDBDefinitions());
  // external editors
  AssignExternalEditors(cPFiles.GetExternalEditors());
  // extras
  SetPreviewType(cPFiles.GetPreviewType());
  SetTranslateCodeFile(cPFiles.GetTranslateCodeFile());
}

xtstring CProjectFilesInd::GetFileBaseName(TProjectFileType nProjectFileType)
{
  xtstring sBaseDefFileName;
  xtstring sBaseFileName;
  switch (nProjectFileType)
  {
  case tProjectFileER:
    sBaseDefFileName = m_sBaseFileNameER;
    break;
  case tProjectFileIFS:
    sBaseDefFileName = m_sBaseFileNameIFS;
    break;
  case tProjectFileStone:
    sBaseDefFileName = m_sBaseFileNameStone;
    break;
  case tProjectFileTemplate:
    sBaseDefFileName = m_sBaseFileNameTemplate;
    break;
  case tProjectFilePythonModule:
    sBaseDefFileName = m_sBaseFileNamePythonModule;
    break;
  default:
    assert(false);
    break;
  }

  for (int nI = 1; nI < 10000; nI++)
  {
    XTCHAR sBuf[50];
    sprintf(sBuf, _XT("%s%04ld"), sBaseDefFileName.c_str(), nI);
    sBaseFileName = sBuf;
    if (!ExistThisFileBaseName(nProjectFileType, sBaseFileName)
      && !ExistThisFileBaseNameAsFile(nProjectFileType, sBaseFileName))
      return sBaseFileName;
  }
  assert(false);
  return sBaseDefFileName;
}

xtstring CProjectFilesInd::GetFileExtension(TProjectFileType nProjectFileType)
{
  xtstring sFileExtension;
  switch (nProjectFileType)
  {
  case tProjectFileER:
    sFileExtension = FILE_EXT_ER;
    break;
  case tProjectFileIFS:
    sFileExtension = FILE_EXT_IFS;
    break;
  case tProjectFileStone:
    sFileExtension = FILE_EXT_STONE;
    break;
  case tProjectFileTemplate:
    sFileExtension = FILE_EXT_TEMPLATE;
    break;
  case tProjectFilePythonModule:
    sFileExtension = FILE_EXT_PYTHON;
    break;
  default:
    sFileExtension = _XT("");
  }
  return sFileExtension;
}

bool CProjectFilesInd::ExistThisModuleName(TProjectFileType nProjectFileType,
                                           const xtstring& sModuleName)
{
  xtstring::iterator it;
  xtstring sMNameLoCase = sModuleName;
  xtstring sMName;

  // lowercase
  // unter LINUX existiert transform nicht
  // transform(sMNameLoCase.begin(), sMNameLoCase.end(), sMNameLoCase.begin(), tolower);
  xtstring::iterator is;
  for (is = sMNameLoCase.begin(); is != sMNameLoCase.end(); is++)
    *is = tolower(*is);

  switch (nProjectFileType)
  {
  case tProjectFileER:
    {
      for (size_t nI = 0; nI < m_cFilesER.size(); nI++)
      {
        sMName = m_cFilesER[nI]->GetModuleName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sMName.begin(), sMName.end(), sMName.begin(), tolower);
        xtstring::iterator is;
        for (is = sMName.begin(); is != sMName.end(); is++)
          *is = tolower(*is);
        if (sMName == sMNameLoCase)
          return true;
      }
    }
    break;
  case tProjectFileIFS:
    {
      for (size_t nI = 0; nI < m_cFilesIFS.size(); nI++)
      {
        sMName = m_cFilesIFS[nI]->GetModuleName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sMName.begin(), sMName.end(), sMName.begin(), tolower);
        xtstring::iterator is;
        for (is = sMName.begin(); is != sMName.end(); is++)
          *is = tolower(*is);
        if (sMName == sMNameLoCase)
          return true;
      }
    }
    break;
  case tProjectFileStone:
  case tProjectFileTemplate:
    // Namen in diesen Gruppen muessen eindeutig sein
    {
      for (size_t nI = 0; nI < m_cFilesStone.size(); nI++)
      {
        sMName = m_cFilesStone[nI]->GetModuleName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sMName.begin(), sMName.end(), sMName.begin(), tolower);
        xtstring::iterator is;
        for (is = sMName.begin(); is != sMName.end(); is++)
          *is = tolower(*is);
        if (sMName == sMNameLoCase)
          return true;
      }
    }
    {
      for (size_t nI = 0; nI < m_cFilesTemplate.size(); nI++)
      {
        sMName = m_cFilesTemplate[nI]->GetModuleName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sMName.begin(), sMName.end(), sMName.begin(), tolower);
        xtstring::iterator is;
        for (is = sMName.begin(); is != sMName.end(); is++)
          *is = tolower(*is);
        if (sMName == sMNameLoCase)
          return true;
      }
    }
    break;
  case tProjectFilePythonModule:
    {
      for (size_t nI = 0; nI < m_cFilesPythonModule.size(); nI++)
      {
        sMName = m_cFilesPythonModule[nI]->GetModuleName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sMName.begin(), sMName.end(), sMName.begin(), tolower);
        xtstring::iterator is;
        for (is = sMName.begin(); is != sMName.end(); is++)
          *is = tolower(*is);
        if (sMName == sMNameLoCase)
          return true;
      }
    }
    break;
  default:
    assert(false);
    break;
  }
  return false;
}

bool CProjectFilesInd::ExistThisFileBaseName(TProjectFileType nProjectFileType,
                                             const xtstring& sBaseFileName)
{
  xtstring::iterator it;
  xtstring sBFN;
  xtstring sBFNLoCase = sBaseFileName;

  // lowercase
  // unter LINUX existiert transform nicht
  // transform(sBFNLoCase.begin(), sBFNLoCase.end(), sBFNLoCase.begin(), tolower);
  xtstring::iterator is;
  for (is = sBFNLoCase.begin(); is != sBFNLoCase.end(); is++)
    *is = tolower(*is);

  switch (nProjectFileType)
  {
  case tProjectFileER:
    {
      for (size_t nI = 0; nI < m_cFilesER.size(); nI++)
      {
        sBFN = m_cFilesER[nI]->GetBaseFileName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sBFN.begin(), sBFN.end(), sBFN.begin(), tolower);
        xtstring::iterator is;
        for (is = sBFN.begin(); is != sBFN.end(); is++)
          *is = tolower(*is);
        if (sBFN == sBFNLoCase)
          return true;
      }
    }
    break;
  case tProjectFileIFS:
    {
      for (size_t nI = 0; nI < m_cFilesIFS.size(); nI++)
      {
        sBFN = m_cFilesIFS[nI]->GetBaseFileName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sBFN.begin(), sBFN.end(), sBFN.begin(), tolower);
        xtstring::iterator is;
        for (is = sBFN.begin(); is != sBFN.end(); is++)
          *is = tolower(*is);
        if (sBFN == sBFNLoCase)
          return true;
      }
    }
    break;
  case tProjectFileStone:
    {
      for (size_t nI = 0; nI < m_cFilesStone.size(); nI++)
      {
        sBFN = m_cFilesStone[nI]->GetBaseFileName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sBFN.begin(), sBFN.end(), sBFN.begin(), tolower);
        xtstring::iterator is;
        for (is = sBFN.begin(); is != sBFN.end(); is++)
          *is = tolower(*is);
        if (sBFN == sBFNLoCase)
          return true;
      }
    }
    break;
  case tProjectFileTemplate:
    {
      for (size_t nI = 0; nI < m_cFilesTemplate.size(); nI++)
      {
        sBFN = m_cFilesTemplate[nI]->GetBaseFileName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sBFN.begin(), sBFN.end(), sBFN.begin(), tolower);
        xtstring::iterator is;
        for (is = sBFN.begin(); is != sBFN.end(); is++)
          *is = tolower(*is);
        if (sBFN == sBFNLoCase)
          return true;
      }
    }
    break;
  case tProjectFilePythonModule:
    {
      for (size_t nI = 0; nI < m_cFilesPythonModule.size(); nI++)
      {
        sBFN = m_cFilesPythonModule[nI]->GetBaseFileName();
        // lowercase
        // unter LINUX existiert transform nicht
        // transform(sBFN.begin(), sBFN.end(), sBFN.begin(), tolower);
        xtstring::iterator is;
        for (is = sBFN.begin(); is != sBFN.end(); is++)
          *is = tolower(*is);
        if (sBFN == sBFNLoCase)
          return true;
      }
    }
    break;
  default:
    assert(false);
    break;
  }
  return false;
}

bool CProjectFilesInd::ExistThisFileBaseNameAsFile(TProjectFileType nProjectFileType,
                                                   const xtstring& sBaseFileName)
{
  xtstring sSubfolder, sExt;
  switch (nProjectFileType)
  {
  case tProjectFileER:
    sSubfolder = SUBFOLDER_ER;
    sExt = FILE_EXT_ER;
    break;
  case tProjectFileIFS:
    sSubfolder = SUBFOLDER_IFS;
    sExt = FILE_EXT_IFS;
    break;
  case tProjectFileStone:
    sSubfolder = SUBFOLDER_STONE;
    sExt = FILE_EXT_STONE;
    break;
  case tProjectFileTemplate:
    sSubfolder = SUBFOLDER_TEMPLATE;
    sExt = FILE_EXT_TEMPLATE;
    break;
  case tProjectFilePythonModule:
    sSubfolder = SUBFOLDER_PYTHON;
    sExt = FILE_EXT_PYTHON;
    break;
  default:
    assert(false);
    break;
  }
  xtstring sBaseFolder = GetProjectFolder();
  sBaseFolder += sSubfolder;
  sBaseFolder += _XT('/');
  sBaseFolder += sBaseFileName;
  sBaseFolder += sExt;

  if (!m_pDirChecker)
    // !!!! 
    return true;
  return m_pDirChecker->ExistThisFile(sBaseFolder);
}

bool CProjectFilesInd::GetModuleName(TProjectFileType nProjectFileType,
                                     const xtstring& sFileName,
                                     xtstring& sModuleName)
{
  xtstring sXMLFileContent;
  xtstring sDataFileContent;
  xtstring sPrintFileContent;

  switch (nProjectFileType)
  {
  case tProjectFileER:
    if (CProjectFileER::FileRead_Uncompressed(sFileName, sXMLFileContent))
      return CProjectFileER::GetModuleNameFromXMLContent(sXMLFileContent, sModuleName);
    break;
  case tProjectFileIFS:
    if (CProjectFileIFS::FileRead_Uncompressed(sFileName, sXMLFileContent))
      return CProjectFileIFS::GetModuleNameFromXMLContent(sXMLFileContent, sModuleName);
    break;
  case tProjectFileStone:
    if (CProjectFileStone::FileRead_Uncompressed(sFileName, sXMLFileContent))
      return CProjectFileStone::GetModuleNameFromXMLContent(sXMLFileContent, sModuleName);
    break;
  case tProjectFileTemplate:
    if (CProjectFileTemplate::FileRead_Uncompressed(sFileName, sXMLFileContent))
      return CProjectFileTemplate::GetModuleNameFromXMLContent(sXMLFileContent, sModuleName);
    break;
  case tProjectFilePythonModule:
    if (CProjectFilePythonModule::FileRead_Uncompressed(sFileName, sXMLFileContent, sDataFileContent))
      return CProjectFilePythonModule::GetModuleNameFromXMLContent(sXMLFileContent, sModuleName);
    break;
  default:
    assert(false);
    break;
  }
  return false;
}

void CProjectFilesInd::ScanAfterAddOrDelWorkPaper()
{
  size_t nI;
  bool bCheckTemplates = true;

  TDataFormatForStoneAndTemplate nFormat;
  // in dieser Funktion kontrollieren wir, ob wir Bausteine / Vorlagen
  // von gleichem Typ haben

  // Bausteine
  for (nI = 0; nI < m_cFilesStone.size(); nI++)
  {
    if (nI == 0)
      // bei erstem uebernehmen wir Format
      nFormat = m_cFilesStone[nI]->GetDataFormat();
    else
    {
      // bei naechsten vergleichen wir Format
      if (nFormat != m_cFilesStone[nI]->GetDataFormat())
      {
        nFormat = tDataFormatST_FirstDummy;
        bCheckTemplates = false;
        break;
      }
    }
  }

  // Vorlagen
  // de muessen wir auch beruecksichtigen, ob Bausteine da sind
  if (bCheckTemplates)
  {
    for (nI = 0; nI < m_cFilesTemplate.size(); nI++)
    {
      if (nI == 0 && m_cFilesStone.size() == 0)
        // falls kein Baustein, dann bei erstem uebernehmen wir Format
        nFormat = m_cFilesTemplate[nI]->GetDataFormat();
      else
      {
        // bei naechsten vergleichen wir Format
        if (nFormat != m_cFilesTemplate[nI]->GetDataFormat())
        {
          nFormat = tDataFormatST_FirstDummy;
          break;
        }
      }
    }
  }
  // falls in nFormat tDataFormatST_FirstDummy ist,
  // wir haben im Projekt verschiedene Formate

  // jedem Baustein sagen, ob verschiedene Formate im Projekt sind
  for (nI = 0; nI < m_cFilesStone.size(); nI++)
    m_cFilesStone[nI]->m_bMixedFormatsInProject = (nFormat == tDataFormatST_FirstDummy);
  // jeder Vorlage sagen, ob verschiedene Formate im Projekt sind
  for (nI = 0; nI < m_cFilesTemplate.size(); nI++)
    m_cFilesTemplate[nI]->m_bMixedFormatsInProject = (nFormat == tDataFormatST_FirstDummy);
}

bool CProjectFilesInd::CreateFromScratch_ProjectFile(const xtstring& sProjectFolder,
                                                     const xtstring& sFileName,
                                                     unsigned long nDataSize,
                                                     unsigned char *data,
                                                     time_t timeChanged)
{
  xtstring sFullFileName;
  sFullFileName = sProjectFolder;
  if (sFullFileName[sFullFileName.size() - 1] != _XT('/')
      && sFullFileName[sFullFileName.size() - 1] != _XT('\\'))
    sFullFileName += _XT('/');
  sFullFileName += sFileName;

  if (!CUtils::CreateFileWithContent(sFullFileName, nDataSize, data))
    return false;
  CUtils::SetFileChangedTime(sFullFileName, timeChanged);
  return true;
}

bool CProjectFilesInd::CreateFromScratch_ERFile(const xtstring& sProjectFolder,
                                                const xtstring& sFileName,
                                                unsigned long nDataSize,
                                                unsigned char *data,
                                                time_t timeChanged)
{
  xtstring sFullFileName;
  sFullFileName = sProjectFolder;
  if (sFullFileName[sFullFileName.size() - 1] != _XT('/')
      && sFullFileName[sFullFileName.size() - 1] != _XT('\\'))
    sFullFileName += _XT('/');
  sFullFileName += SUBFOLDER_ER;
  sFullFileName += _XT('/');
  sFullFileName += sFileName;

  if (!CUtils::CreateFileWithContent(sFullFileName, nDataSize, data))
    return false;
  CUtils::SetFileChangedTime(sFullFileName, timeChanged);
  return true;
}

bool CProjectFilesInd::CreateFromScratch_IFSFile(const xtstring& sProjectFolder,
                                                 const xtstring& sFileName,
                                                 unsigned long nDataSize,
                                                 unsigned char *data,
                                                 time_t timeChanged)
{
  xtstring sFullFileName;
  sFullFileName = sProjectFolder;
  if (sFullFileName[sFullFileName.size() - 1] != _XT('/')
      && sFullFileName[sFullFileName.size() - 1] != _XT('\\'))
    sFullFileName += _XT('/');
  sFullFileName += SUBFOLDER_IFS;
  sFullFileName += _XT('/');
  sFullFileName += sFileName;

  if (!CUtils::CreateFileWithContent(sFullFileName, nDataSize, data))
    return false;
  CUtils::SetFileChangedTime(sFullFileName, timeChanged);
  return true;
}

bool CProjectFilesInd::CreateFromScratch_StoneFile(const xtstring& sProjectFolder,
                                                   const xtstring& sFileName,
                                                   unsigned long nDataSize,
                                                   unsigned char *data,
                                                   time_t timeChanged)
{
  xtstring sFullFileName;
  sFullFileName = sProjectFolder;
  if (sFullFileName[sFullFileName.size() - 1] != _XT('/')
      && sFullFileName[sFullFileName.size() - 1] != _XT('\\'))
    sFullFileName += _XT('/');
  sFullFileName += SUBFOLDER_STONE;
  sFullFileName += _XT('/');
  sFullFileName += sFileName;

  if (!CUtils::CreateFileWithContent(sFullFileName, nDataSize, data))
    return false;
  CUtils::SetFileChangedTime(sFullFileName, timeChanged);
  return true;
}

bool CProjectFilesInd::CreateFromScratch_TemplateFile(const xtstring& sProjectFolder,
                                                      const xtstring& sFileName,
                                                      unsigned long nDataSize,
                                                      unsigned char *data,
                                                      time_t timeChanged)
{
  xtstring sFullFileName;
  sFullFileName = sProjectFolder;
  if (sFullFileName[sFullFileName.size() - 1] != _XT('/')
      && sFullFileName[sFullFileName.size() - 1] != _XT('\\'))
    sFullFileName += _XT('/');
  sFullFileName += SUBFOLDER_TEMPLATE;
  sFullFileName += _XT('/');
  sFullFileName += sFileName;

  if (!CUtils::CreateFileWithContent(sFullFileName, nDataSize, data))
    return false;
  CUtils::SetFileChangedTime(sFullFileName, timeChanged);
  return true;
}

bool CProjectFilesInd::CreateFromScratch_PythonFile(const xtstring& sProjectFolder,
                                                    const xtstring& sFileName,
                                                    unsigned long nDataSize,
                                                    unsigned char *data,
                                                    time_t timeChanged)
{
  xtstring sFullFileName;
  sFullFileName = sProjectFolder;
  if (sFullFileName[sFullFileName.size() - 1] != _XT('/')
      && sFullFileName[sFullFileName.size() - 1] != _XT('\\'))
    sFullFileName += _XT('/');
  sFullFileName += SUBFOLDER_PYTHON;
  sFullFileName += _XT('/');
  sFullFileName += sFileName;

  if (!CUtils::CreateFileWithContent(sFullFileName, nDataSize, data))
    return false;
  CUtils::SetFileChangedTime(sFullFileName, timeChanged);
  return true;
}


