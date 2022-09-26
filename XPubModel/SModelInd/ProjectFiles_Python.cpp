#include "ProjectFiles_Python.h"

#include <assert.h>

#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"
#include "STools/Utils.h"




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProjectFilePythonModule
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProjectFilePythonModule::CProjectFilePythonModule(const xtstring& sBaseName,
                                                   bool bCreateObjectsImmediately)
                         :CProjectFileBase(tProjectFilePythonModule)
{
  m_bCreateObjectsImmediately = bCreateObjectsImmediately;
  m_bInCreateSyncObjects = false;
  SetXMLFileContent(_XT(""));
  SetDataFileContent(_XT(""));
  SetMD5Sum_XML(GetMD5Sum(_XT("")));
  SetMD5Sum_DATA(GetMD5Sum(_XT("")));

  SetTime_XML(0);
  SetTime_DATA(0);

  SetModuleName(sBaseName);
}

CProjectFilePythonModule::~CProjectFilePythonModule()
{
  DeleteSyncObjects(false);
}

void CProjectFilePythonModule::SetDataFileContent(const xtstring& sContent)
{
  m_sDataFileContent = sContent;
}

void CProjectFilePythonModule::SetModuleName(const xtstring& sModuleName)
{
  CProjectFileBase::SetModuleName(sModuleName);
}

xtstring CProjectFilePythonModule::GetFileContentForCopy()
{
  xtstring sFileContent;
  sFileContent = GetXMLFileName();
  sFileContent += _XT("\n");
  sFileContent += GetDataFileContent();
  return sFileContent;
}

bool CProjectFilePythonModule::SetFileContentFromPaste(const xtstring& sContent)
{
  size_t nFileNameEnd = sContent.find(_XT("\n"), 0);
  if (nFileNameEnd == xtstring::npos)
    return false;
  // parse
  xtstring sFileName;
  xtstring sDataFileContent;
  sFileName.assign(sContent.begin(), sContent.begin() + nFileNameEnd);
  sDataFileContent.assign(sContent.begin() + nFileNameEnd + 1, sContent.end());
  // file name and content
  SetXMLFileName(sFileName);
  SetDataFileContent(sDataFileContent);
  return true;
}

const xtstring& CProjectFilePythonModule::GetXMLFileContent()
{
  if (!m_bInCreateSyncObjects)
    SaveSyncObjectsIfChanged();
  return m_sXMLFileContent;
}

void CProjectFilePythonModule::SetXMLFileContent(const xtstring& sContent)
{
  if (SyncObjectsCreated())
    DeleteSyncObjects();

  m_sXMLFileContent = sContent;


  CreateSyncObjects();


}

void CProjectFilePythonModule::operator = (CProjectFilePythonModule* pFile)
{
  if (!pFile)
    return;
  CProjectFileBase::operator = (pFile);
  SetXMLFileContent(pFile->m_sXMLFileContent);
  SetDataFileContent(pFile->m_sDataFileContent);
}

bool CProjectFilePythonModule::Rename(const xtstring& sNewBaseFileName)
{
  xtstring sOldBaseFileName = GetBaseFileName();

  xtstring sFolder = GetBaseFolder();
  if (sFolder.size() == 0)
    // nicht auf der Festplatte
    return false;
  sFolder += SUBFOLDER_PYTHON;
  sFolder += _XT("/");

  // alte Datei
  xtstring sOldXMLFile = sFolder;
  sOldXMLFile += GetXMLFileName();

  SetBaseFileName(sNewBaseFileName);
  xtstring sNewXMLFile = sFolder;
  sNewXMLFile += GetXMLFileName();

  bool bRetVal = FileWrite(sNewXMLFile, GetXMLFileContent(), GetDataFileContent());
  if (!bRetVal)
  {
    SetBaseFileName(sOldBaseFileName);
    return false;
  }
  if (m_pDirChecker)
    m_pDirChecker->RemoveFile(sOldXMLFile);
  return true;
}

bool CProjectFilePythonModule::Reload(const xtstring& sBaseFolder)
{
  SetBaseFolder(sBaseFolder);

  bool bRet = false;
  xtstring sXMLFileContent;
  xtstring sDataFileContent;
  xtstring sFile;
  if (GetXMLFileName().length())
  {
    // die Datei existiert, nicht neue Datei
    // ganze Dateiname erzeugen
    sFile = sBaseFolder;
    sFile += SUBFOLDER_PYTHON;
    sFile += _XT('/');
    sFile += GetXMLFileName();

    bRet = FileRead_Uncompressed(sFile, sXMLFileContent, sDataFileContent);
  }
  if (bRet)
  {
    SetDataFileContent(sDataFileContent);
    SetMD5Sum_DATA(GetMD5Sum(sDataFileContent));
    SetXMLFileContent(sXMLFileContent);
    SetMD5Sum_XML(GetMD5Sum(sXMLFileContent));

    time_t timeChanged;
    GetFileTimeChanged(sFile, timeChanged);
    SetTime_XML(timeChanged);
    SetTime_DATA(timeChanged);

    xtstring sModuleName;
    GetModuleNameFromXMLContent(m_sXMLFileContent, sModuleName);
    SetModuleName(sModuleName);
  }
  return bRet;
}

bool CProjectFilePythonModule::ReloadAfterAlterByAnotherApp(bool bXMLChanged)
{
  return Reload(GetBaseFolder());
}

bool CProjectFilePythonModule::ReadXMLDocAndFile(const xtstring& sBaseFolder,
                                                 CXPubMarkUp* pXMLDoc,
                                                 const xtstring& sCompressedFileName)
{
  SetBaseFolder(sBaseFolder);

  // XMLDoc in <HTML-File>
  SetXMLFileName(pXMLDoc->GetAttrib(PYTHON_FILE_FILENAME));

  bool bRet = false;
  xtstring sXMLFileContent;
  xtstring sDataFileContent;
  xtstring sFile;
  if (GetXMLFileName().length())
  {
    // die Datei existiert, nicht neue Datei
    // ganze Dateiname erzeugen
    if (sCompressedFileName.size())
      sFile.clear();
    else
      sFile = sBaseFolder;
    sFile += SUBFOLDER_PYTHON;
    sFile += _XT('/');
    sFile += GetXMLFileName();

    if (sCompressedFileName.size())
      bRet = FileRead_Compressed(sCompressedFileName, sFile, sXMLFileContent, sDataFileContent);
    else
      bRet = FileRead_Uncompressed(sFile, sXMLFileContent, sDataFileContent);
  }
  if (bRet)
  {
    // wegen SyncEng Objekten muss SetDataFileContent frueher
    // als SetXMLFileContent aufgerufen
    SetDataFileContent(sDataFileContent);
    SetMD5Sum_DATA(GetMD5Sum(sDataFileContent));
    SetXMLFileContent(sXMLFileContent);
    SetMD5Sum_XML(GetMD5Sum(sXMLFileContent));

    time_t timeChanged;
    GetFileTimeChanged(sFile, timeChanged);
    SetTime_XML(timeChanged);
    SetTime_DATA(timeChanged);

    xtstring sModuleName;
    GetModuleNameFromXMLContent(m_sXMLFileContent, sModuleName);
    SetModuleName(sModuleName);
  }
  return bRet;
}

bool CProjectFilePythonModule::SaveXMLDocAndFile(const xtstring& sBaseFolder,
                                                 CXPubMarkUp* pXMLDoc)
{
  SetBaseFolder(sBaseFolder);

  if (m_pDirChecker)
    m_pDirChecker->CheckOrCreateDirectory(sBaseFolder, SUBFOLDER_PYTHON);

  xtstring sFile;
  sFile = sBaseFolder;
  sFile += SUBFOLDER_PYTHON;
  sFile += _XT('/');

  if (!GetXMLFileName().length())
    return false;

  // ganze Dateiname erzeugen
  sFile += GetXMLFileName();

  bool bDoSave = false;
  {
    xtstring sXMLFileContent = GetXMLFileContent();
    xtstring sDataFileContent = GetDataFileContent();
    xtstring sMD5Sum_XML = GetMD5Sum(sXMLFileContent);
    xtstring sMD5Sum_DATA = GetMD5Sum(sDataFileContent);
    if (sMD5Sum_XML != GetMD5Sum_XML() && sMD5Sum_DATA != GetMD5Sum_DATA())
      bDoSave = true;
    {
      xtstring sXMLTemp;
      xtstring sDataTemp;
      FileRead_Uncompressed(sFile, sXMLTemp, sDataTemp);
      // Bei Python haben wir speziellem Fall.
      // XML Teil ist zur Zeit leer - nicht benutzt.
      // Wenn man einem Modul anlegen will, Daten Teil ist auch leer.
      // Wenn Data Teil leer ist, wir werden es abspeichern
      if (sXMLTemp != GetXMLFileContent() || sDataTemp != GetDataFileContent() || sDataTemp.size() == 0)
        bDoSave = true;
    }
  }
  if (bDoSave)
  {
    // zuerst abspeichern
    xtstring sXMLFileContent = GetXMLFileContent();
    xtstring sDataFileContent = GetDataFileContent();
    bool bRetVal = FileWrite(sFile, GetXMLFileContent(), sDataFileContent);
    if (!bRetVal)
      return false;
    SetMD5Sum_XML(GetMD5Sum(sXMLFileContent));
    SetMD5Sum_DATA(GetMD5Sum(sDataFileContent));

    time_t timeChanged;
    GetFileTimeChanged(sFile, timeChanged);
    SetTime_XML(timeChanged);
    SetTime_DATA(timeChanged);
  }

  if (pXMLDoc)
  {
    // XMLDoc in <HTML-Files>
    if (pXMLDoc->AddChildElem(PYTHON_FILE_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <HTML-File>
      pXMLDoc->SetAttrib(PYTHON_FILE_FILENAME, GetXMLFileName().c_str());

      pXMLDoc->OutOfElem();
    }
  }
  return true;
}

bool CProjectFilePythonModule::GetModuleNameFromXMLContent(const xtstring& sXMLContent,
                                                           xtstring& sModuleName)
{
  sModuleName = sXMLContent;
  return true;
/*
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLContent.c_str());

  // we have now XML document
  cXMLDoc.ResetPos();

  bool bRet = false;

  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_HTMLSTONE)
  {
    // in <X-Publisher-HTML>
    if (cXMLDoc.FindChildElem(HTMLSTONE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <HTML>
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <MainProperty>
          xtstring sText;
          bRet = CMPModelPythonInd::ReadXMLModuleName(&cXMLDoc, sText);
          sModuleName = sText;
        }
      }
    }
  }

  return bRet;
*/
}

bool CProjectFilePythonModule::FileRead_Compressed(const xtstring& sCompressedFileName,
                                                   const xtstring& sFileName,
                                                   xtstring& sXMLFileContent,
                                                   xtstring& sPythonFileContent)
{
  sXMLFileContent.clear();
  sPythonFileContent.clear();

  xtstring sFileContent;
  if (!CProjectFileBase::ReadFile_Compressed(sCompressedFileName, sFileName, sFileContent))
    return false;

  size_t nPos1 = sFileName.rfind(_XT('/'));
  size_t nPos2 = sFileName.rfind(_XT('\\'));
  if (nPos1 == xtstring::npos)
    nPos1 = 0;
  else
    nPos1++;
  if (nPos2 == xtstring::npos)
    nPos2 = 0;
  else
    nPos2++;
  if (nPos1 < nPos2)
    sXMLFileContent.assign(sFileName, nPos2, sFileName.length() - nPos2);
  else
    sXMLFileContent.assign(sFileName, nPos1, sFileName.length() - nPos1);
  nPos1 = sXMLFileContent.rfind(FILE_EXT_PYTHON);
  if (nPos1 != xtstring::npos)
    sXMLFileContent.erase(nPos1);
  sPythonFileContent.assign(sFileContent, 0, sFileContent.length());
  return true;
}

bool CProjectFilePythonModule::FileRead_Uncompressed(const xtstring& sFileName,
                                                     xtstring& sXMLFileContent,
                                                     xtstring& sPythonFileContent)
{
  sXMLFileContent.clear();
  sPythonFileContent.clear();

  xtstring sFileContent;
  if (!CProjectFileBase::ReadFile_Uncompressed(sFileName, sFileContent))
    return false;

  size_t nPos1 = sFileName.rfind(_XT('/'));
  size_t nPos2 = sFileName.rfind(_XT('\\'));
  if (nPos1 == xtstring::npos)
    nPos1 = 0;
  else
    nPos1++;
  if (nPos2 == xtstring::npos)
    nPos2 = 0;
  else
    nPos2++;
  if (nPos1 < nPos2)
    sXMLFileContent.assign(sFileName, nPos2, sFileName.length() - nPos2);
  else
    sXMLFileContent.assign(sFileName, nPos1, sFileName.length() - nPos1);
  nPos1 = sXMLFileContent.rfind(FILE_EXT_PYTHON);
  if (nPos1 != xtstring::npos)
    sXMLFileContent.erase(nPos1);
  sPythonFileContent.assign(sFileContent, 0, sFileContent.length());
  return true;
}

bool CProjectFilePythonModule::FileWrite(const xtstring& sFileName,
                                         const xtstring& sXMLFileContent,
                                         const xtstring& sPythonFileContent)
{
  // file content vorbereiten
  xtstring sFileContent;
  sFileContent = sXMLFileContent; // warning C4100
  sFileContent = sPythonFileContent;

  if (!CProjectFileBase::WriteFile(sFileName, sFileContent))
    return false;

  return true;
}

CMPModelPythonInd* CProjectFilePythonModule::GetMPModelPythonInd()
{
  CMPModelPythonInd* pModel;
  pModel = new CMPModelPythonInd();
  pModel->SetName(GetBaseFileName());
  return pModel;
/*
  // create XML object
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(GetXMLFileContent().c_str());
  // we have now XML document
  cXMLDoc.ResetPos();
  // 
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_HTMLSTONE)
  {
    // in <X-Publisher-HTML>
    if (cXMLDoc.FindChildElem(HTMLSTONE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <HTML>
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <MainProperty>

          CMPModelPythonInd* pModel;
          pModel = new CMPModelPythonInd();
          pModel->ReadXMLContent(&cXMLDoc);
          return pModel;
        }
      }
    }
  }
  return 0;
*/
}

xtstring CProjectFilePythonModule::GetDataFileName(LPCXTCHAR pXMLDoc /*= 0*/)
{
  return GetXMLFileName();
}

xtstring CProjectFilePythonModule::GetFullDataFileName()
{
  return GetFullXMLFileName();
}

void CProjectFilePythonModule::GetCreatedAndChangedTime(xtstring& sCreated,
                                                        xtstring& sChanged,
                                                        bool bWithSeconds)
{
  CProjectFileBase::GetCreatedAndChangedTime(GetFullXMLFileName(), sCreated, sChanged, bWithSeconds);
}
xtstring CProjectFilePythonModule::GetFullXMLFileName()
{
  xtstring sXMLFile;
  sXMLFile = GetBaseFolder();
  sXMLFile += SUBFOLDER_PYTHON;
  sXMLFile += _XT('/');
  sXMLFile += GetXMLFileName();
  return sXMLFile;
}

bool CProjectFilePythonModule::SaveSyncObjectsIfChanged()
{
  return true;
}

bool CProjectFilePythonModule::CreateSyncObjects()
{
  DeleteSyncObjects();
  m_bInCreateSyncObjects = true;
  m_bInCreateSyncObjects = false;
  return true;
}

bool CProjectFilePythonModule::DeleteSyncObjects(bool bSaveSyncObjectsIfChanged /*= true*/)
{
  if (bSaveSyncObjectsIfChanged)
    SaveSyncObjectsIfChanged();
  return true;
}

bool CProjectFilePythonModule::ContentChanged()
{
  xtstring sMD5Sum_XML;
  xtstring sMD5Sum_DATA;
  sMD5Sum_XML = GetMD5Sum(GetXMLFileContent());
  sMD5Sum_DATA = GetMD5Sum(GetDataFileContent());
  if (sMD5Sum_XML != GetMD5Sum_XML() && sMD5Sum_DATA != GetMD5Sum_DATA())
    return true;
  return false;
}



