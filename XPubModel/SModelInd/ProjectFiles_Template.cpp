#include "ProjectFiles_Template.h"

#include <assert.h>

#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"
#include "STools/Utils.h"




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProjectFileTemplate
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProjectFileTemplate::CProjectFileTemplate(const xtstring& sBaseName,
                                           bool bCreateObjectsImmediately)
                     :CProjectFileBase(tProjectFileTemplate),
                      CSyncWPNotifiersGroup(_XT("SetGroupNameLater"), tProjectFileTemplate)
{
  m_bCreateObjectsImmediately = bCreateObjectsImmediately;
  m_bInCreateSyncObjects = false;
  m_pSyncMPTemplate = NULL;
  m_pSyncDBTemplate = NULL;
  SetXMLFileContent(_XT(""));
  SetDataFileContent(_XT(""));
  SetMD5Sum_XML(GetMD5Sum(_XT("")));
  SetMD5Sum_DATA(GetMD5Sum(_XT("")));

  SetTime_XML(0);
  SetTime_DATA(0);

  SetModuleName(sBaseName);
  m_nDataFormat = tDataFormatST_FirstDummy;
  m_bMixedFormatsInProject = false;
}

CProjectFileTemplate::~CProjectFileTemplate()
{
  DeleteSyncObjects(false);
}

xtstring CProjectFileTemplate::GetNameForShow()
{
  if (!m_bMixedFormatsInProject)
    return CProjectFileBase::GetNameForShow();
  // Wenn wir "Project Tree" auf "Project Grid" geaendert haben,
  // muessen wir nicht mehr Format darstellen.
  return CProjectFileBase::GetNameForShow();

  xtstring sShowName;
  sShowName = GetModuleName();
  switch (GetDataFormat())
  {
  case tDataFormatST_Plain:
    sShowName += _XT("(PLAIN)");    break;
  case tDataFormatST_Html:
    sShowName += _XT("(HTML)");     break;
  case tDataFormatST_Rtf:
    sShowName += _XT("(RTF)");      break;
  case tDataFormatST_Tag:
    sShowName += _XT("(TAG)");      break;
  case tDataFormatST_Db:
    sShowName += _XT("(DB)");       break;
  case tDataFormatST_Pdf:
    sShowName += _XT("(PDF)");      break;
  case tDataFormatST_Property:
    sShowName += _XT("(PROP)");     break;
  case tDataFormatST_Xml:
    sShowName += _XT("(XML)");      break;
  default:
    assert(false);
    break;
  }
  return sShowName;
}

void CProjectFileTemplate::SetDataFileContent(const xtstring& sContent)
{
  m_sDataFileContent = sContent;
}

void CProjectFileTemplate::SetModuleName(const xtstring& sModuleName)
{
  SetModuleSyncGroupName(sModuleName);
  CProjectFileBase::SetModuleName(sModuleName);
  if (m_pSyncMPTemplate)
    m_pSyncMPTemplate->SetName(sModuleName);
}

xtstring CProjectFileTemplate::GetFileContentForCopy()
{
  xtstring sFileContent;
  sFileContent = GetXMLFileName();
  sFileContent += _XT("\n");
  sFileContent += GetDataFileName();
  sFileContent += _XT("\n");
  sFileContent += XML_LAYER_START;
  sFileContent += GetXMLFileContent();
  sFileContent += XML_LAYER_END;
  sFileContent += GetDataFileContent();
  return sFileContent;
}

bool CProjectFileTemplate::SetFileContentFromPaste(const xtstring& sContent_)
{
  xtstring sContent(sContent_);
  size_t nFileNameEnd = sContent.find(_XT("\n"), 0);
  if (nFileNameEnd == xtstring::npos)
    return false;

  xtstring sXMLFileName;
  xtstring sDataFileName;
  xtstring sFileContent;
  // parse 1
  sXMLFileName.assign(sContent.begin(), sContent.begin() + nFileNameEnd);
  sFileContent.assign(sContent.begin() + nFileNameEnd + 1, sContent.end());
  sContent = sFileContent;
  // parse 1.5
  nFileNameEnd = sContent.find(_XT("\n"), 0);
  if (nFileNameEnd == xtstring::npos)
    return false;
  sDataFileName.assign(sContent.begin(), sContent.begin() + nFileNameEnd);
  sFileContent.assign(sContent.begin() + nFileNameEnd + 1, sContent.end());
  // parse 2
  xtstring sXMLFileContent;
  xtstring sDataFileContent;
  size_t nPosStart = sFileContent.find(XML_LAYER_START, 0);
  size_t nPosEnd = sFileContent.find(XML_LAYER_END, 0);
  nPosStart += strlen(XML_LAYER_START);
  if (nPosStart == xtstring::npos || nPosEnd == xtstring::npos
      || nPosEnd < nPosStart || nPosEnd - nPosStart <= 0)
    return false;
  sXMLFileContent.assign(sFileContent, nPosStart, nPosEnd - nPosStart);
  nPosEnd += strlen(XML_LAYER_END);
  sDataFileContent.assign(sFileContent, nPosEnd, sFileContent.length() - nPosEnd);
  // module name
  xtstring sModuleName;
  GetModuleNameFromXMLContent(sXMLFileContent, sModuleName);
  SetModuleName(sModuleName);
  // file name and content
  SetXMLFileName(sXMLFileName);
  SetXMLFileContent(sXMLFileContent);
  SetDataFileContent(sDataFileContent);
  return true;
}

const xtstring& CProjectFileTemplate::GetXMLFileContent()
{
  if (!m_bInCreateSyncObjects)
    SaveSyncObjectsIfChanged();
  return m_sXMLFileContent;
}

void CProjectFileTemplate::SetXMLFileContent(const xtstring& sContent)
{
  if (SyncObjectsCreated())
    DeleteSyncObjects();

  m_sXMLFileContent = sContent;
  m_nDataFormat = tDataFormatST_FirstDummy;

  CreateSyncObjects();

  m_nDataFormat = GetMPModelTemplateInd()->GetDataFormat();
}

void CProjectFileTemplate::operator = (CProjectFileTemplate* pFile)
{
  if (!pFile)
    return;
  CProjectFileBase::operator = (pFile);
  SetXMLFileContent(pFile->GetXMLFileContent());
  SetDataFileContent(pFile->GetDataFileContent());
}

bool CProjectFileTemplate::Rename(const xtstring& sNewBaseFileName)
{
  xtstring sOldBaseFileName = GetBaseFileName();

  xtstring sFolder = GetBaseFolder();
  if (sFolder.size() == 0)
    // nicht auf der Festplatte
    return false;
  sFolder += SUBFOLDER_TEMPLATE;
  sFolder += _XT("/");

  // alte Datei
  xtstring sOldXMLFile = sFolder;
  sOldXMLFile += GetXMLFileName();
  xtstring sOldDataFile = sFolder;
  sOldDataFile += GetDataFileName();

  SetBaseFileName(sNewBaseFileName);
  xtstring sNewXMLFile = sFolder;
  sNewXMLFile += GetXMLFileName();
  xtstring sNewDataFile = sFolder;
  sNewDataFile += GetDataFileName();

  bool bRetVal = FileWrite(sNewXMLFile, GetXMLFileContent());
  if (!bRetVal)
  {
    SetBaseFileName(sOldBaseFileName);
    return false;
  }
  bRetVal = FileWrite(sNewDataFile, GetDataFileContent());
  if (!bRetVal)
  {
    SetBaseFileName(sOldBaseFileName);
    return false;
  }
  if (m_pDirChecker)
  {
    m_pDirChecker->RemoveFile(sOldXMLFile);
    m_pDirChecker->RemoveFile(sOldDataFile);
  }
  return true;
}

bool CProjectFileTemplate::Reload(const xtstring& sBaseFolder)
{
  SetBaseFolder(sBaseFolder);

  bool bRet = false;
  xtstring sXMLFileContent;
  xtstring sDataFileContent;
  xtstring sXMLFile;
  xtstring sDataFile;
  if (GetXMLFileName().length())
  {
    // die Datei existiert, nicht neue Datei
    // ganze Dateiname erzeugen
    sXMLFile = sBaseFolder;
    sXMLFile += SUBFOLDER_TEMPLATE;
    sXMLFile += _XT('/');
    sDataFile = sXMLFile;
    sXMLFile += GetXMLFileName();

    bRet = FileRead_Uncompressed(sXMLFile, sXMLFileContent);
    if (bRet)
    {
      sDataFile += GetDataFileName(sXMLFileContent.c_str());
      bRet = FileRead_Uncompressed(sDataFile, sDataFileContent);
    }
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
    GetFileTimeChanged(sXMLFile, timeChanged);
    SetTime_XML(timeChanged);
    GetFileTimeChanged(sDataFile, timeChanged);
    SetTime_DATA(timeChanged);

    xtstring sModuleName;
    GetModuleNameFromXMLContent(m_sXMLFileContent, sModuleName);
    SetModuleName(sModuleName);
  }
  return bRet;
}

bool CProjectFileTemplate::ReloadAfterAlterByAnotherApp(bool bXMLChanged)
{
  return Reload(GetBaseFolder());
}

bool CProjectFileTemplate::ReadXMLDocAndFile(const xtstring& sBaseFolder,
                                             CXPubMarkUp* pXMLDoc,
                                             const xtstring& sCompressedFileName)
{
  SetBaseFolder(sBaseFolder);

  // XMLDoc in <HTML-File>
  SetXMLFileName(pXMLDoc->GetAttrib(TEMPLATE_FILE_FILENAME));

  bool bRet = false;
  xtstring sXMLFileContent;
  xtstring sDataFileContent;
  xtstring sXMLFile;
  xtstring sDataFile;
  if (GetXMLFileName().length())
  {
    // die Datei existiert, nicht neue Datei
    // ganze Dateiname erzeugen
    if (sCompressedFileName.size())
      sXMLFile.clear();
    else
      sXMLFile = sBaseFolder;
    sXMLFile += SUBFOLDER_TEMPLATE;
    sXMLFile += _XT('/');
    sDataFile = sXMLFile;
    sXMLFile += GetXMLFileName();

    if (sCompressedFileName.size())
      bRet = FileRead_Compressed(sCompressedFileName, sXMLFile, sXMLFileContent);
    else
      bRet = FileRead_Uncompressed(sXMLFile, sXMLFileContent);
    if (bRet)
    {
      sDataFile += GetDataFileName(sXMLFileContent.c_str());
      if (sCompressedFileName.size())
        bRet = FileRead_Compressed(sCompressedFileName, sDataFile, sDataFileContent);
      else
        bRet = FileRead_Uncompressed(sDataFile, sDataFileContent);
    }
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
    GetFileTimeChanged(sXMLFile, timeChanged);
    SetTime_XML(timeChanged);
    GetFileTimeChanged(sDataFile, timeChanged);
    SetTime_DATA(timeChanged);

    xtstring sModuleName;
    GetModuleNameFromXMLContent(m_sXMLFileContent, sModuleName);
    SetModuleName(sModuleName);
  }
  return bRet;
}

bool CProjectFileTemplate::SaveXMLDocAndFile(const xtstring& sBaseFolder,
                                             CXPubMarkUp* pXMLDoc)
{
  SetBaseFolder(sBaseFolder);

  if (m_pDirChecker)
    m_pDirChecker->CheckOrCreateDirectory(sBaseFolder, SUBFOLDER_TEMPLATE);

  xtstring sXMLFile;
  xtstring sDataFile;
  sXMLFile = sBaseFolder;
  sXMLFile += SUBFOLDER_TEMPLATE;
  sXMLFile += _XT('/');
  sDataFile = sXMLFile;

  if (!GetXMLFileName().length())
    return false;

  // ganze Dateiname erzeugen
  sXMLFile += GetXMLFileName();
  sDataFile += GetDataFileName();

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
      FileRead_Uncompressed(sXMLFile, sXMLTemp);
      FileRead_Uncompressed(sDataFile, sDataTemp);
      if (sXMLTemp != sXMLFileContent || sDataTemp != sDataFileContent)
        bDoSave = true;
    }
  }
  if (bDoSave)
  {
    if (GetMPModelTemplateInd())
      GetMPModelTemplateInd()->SetAutor(CUtils::GetLoggedUser());
    // zuerst abspeichern
    xtstring sXMLFileContent = GetXMLFileContent();
    xtstring sDataFileContent = GetDataFileContent();
    bool bRetVal = FileWrite(sXMLFile, sXMLFileContent);
    if (!bRetVal)
      return false;
    SetMD5Sum_XML(GetMD5Sum(sXMLFileContent));
    bRetVal = FileWrite(sDataFile, sDataFileContent);
    if (!bRetVal)
      return false;
    SetMD5Sum_DATA(GetMD5Sum(sDataFileContent));

    time_t timeChanged;
    GetFileTimeChanged(sXMLFile, timeChanged);
    SetTime_XML(timeChanged);
    GetFileTimeChanged(sDataFile, timeChanged);
    SetTime_DATA(timeChanged);
  }

  if (pXMLDoc)
  {
    // XMLDoc in <HTML-Files>
    if (pXMLDoc->AddChildElem(TEMPLATE_FILE_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <HTML-File>
      pXMLDoc->SetAttrib(TEMPLATE_FILE_FILENAME, GetXMLFileName().c_str());

      pXMLDoc->OutOfElem();
    }
  }
  return true;
}

bool CProjectFileTemplate::GetModuleNameFromXMLContent(const xtstring& sXMLContent,
                                                       xtstring& sModuleName)
{
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLContent.c_str());

  // we have now XML document
  cXMLDoc.ResetPos();

  bool bRet = false;

  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_TEMPLATE)
  {
    // in <X-Publisher-HTMLTemp>
    if (cXMLDoc.FindChildElem(TEMPLATE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <HTMLTemp>
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <MainProperty>
          xtstring sText;
          bRet = CMPModelTemplateInd::ReadXMLModuleName(&cXMLDoc, sText);
          sModuleName = sText;
        }
      }
    }
  }

  return bRet;
}

bool CProjectFileTemplate::FileRead_Compressed(const xtstring& sCompressedFileName,
                                               const xtstring& sFileName,
                                               xtstring& sFileContent)
{
  sFileContent.clear();

  if (!CProjectFileBase::ReadFile_Compressed(sCompressedFileName, sFileName, sFileContent))
    return false;

  return true;
}

bool CProjectFileTemplate::FileRead_Uncompressed(const xtstring& sFileName,
                                                 xtstring& sFileContent)
{
  sFileContent.clear();

  if (!CProjectFileBase::ReadFile_Uncompressed(sFileName, sFileContent))
    return false;

  return true;
}

bool CProjectFileTemplate::FileWrite(const xtstring& sFileName,
                                     const xtstring& sFileContent)
{
  if (!CProjectFileBase::WriteFile(sFileName, sFileContent))
    return false;

  return true;
}

CMPModelTemplateInd* CProjectFileTemplate::GetMPModelTemplateInd()
{
  if (m_pSyncMPTemplate)
    return m_pSyncMPTemplate;

  // create XML object
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(GetXMLFileContent().c_str());
  // we have now XML document
  cXMLDoc.ResetPos();
  // 
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_TEMPLATE)
  {
    // in <X-Publisher-HTMLTemp>
    if (cXMLDoc.FindChildElem(TEMPLATE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <HTMLTemp>
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <MainProperty>

          CMPModelTemplateInd* pModel;
          pModel = new CMPModelTemplateInd(this);
          pModel->ReadXMLContent(&cXMLDoc);
          return pModel;
        }
      }
    }
  }

  return 0;
}

xtstring CProjectFileTemplate::GetDataFileName(LPCXTCHAR pXMLDoc /*= 0*/)
{
  xtstring sXMLDoc;
  if (pXMLDoc)
    sXMLDoc = pXMLDoc;
  else
    sXMLDoc = GetXMLFileContent();

  TDataFormatForStoneAndTemplate nDataType = tDataFormatST_FirstDummy;
  // create XML object
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLDoc.c_str());
  // we have now XML document
  cXMLDoc.ResetPos();
  // 
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_TEMPLATE)
  {
    if (cXMLDoc.FindChildElem(TEMPLATE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();

          long lVal;
          lVal = cXMLDoc.GetAttribLong(PROPERTY_ATTRIB_DATAFORMAT);
          nDataType = (TDataFormatForStoneAndTemplate)lVal;
        }
      }
    }
  }
  xtstring sDataFileName = GetBaseFileName();
  switch (nDataType)
  {
  case tDataFormatST_Plain:
    sDataFileName += PLAIN_FILE_EXTENSION;
    break;
  case tDataFormatST_Html:
    sDataFileName += HTML_FILE_EXTENSION;
    break;
  case tDataFormatST_Rtf:
    sDataFileName += RTF_FILE_EXTENSION;
    break;
  case tDataFormatST_Tag:
    sDataFileName += TAG_FILE_EXTENSION;
    break;
  case tDataFormatST_Db:
    sDataFileName += DB_FILE_EXTENSION;
    break;
  case tDataFormatST_Property:
    sDataFileName += PROPERTY_FILE_EXTENSION;
    break;
  case tDataFormatST_Xml:
    sDataFileName += XML_FILE_EXTENSION;
    break;
  default:
    sDataFileName += UNKNOWN_FILE_EXTENSION;
    break;
  }
  return sDataFileName;
}

xtstring CProjectFileTemplate::GetFullDataFileName()
{
  xtstring sDataFile;
  sDataFile = GetBaseFolder();
  sDataFile += SUBFOLDER_TEMPLATE;
  sDataFile += _XT('/');
  sDataFile += GetDataFileName();
  return sDataFile;
}

void CProjectFileTemplate::GetCreatedAndChangedTime(xtstring& sCreated,
                                                    xtstring& sChanged,
                                                    bool bWithSeconds)
{
  CProjectFileBase::GetCreatedAndChangedTime(GetFullXMLFileName(), sCreated, sChanged, bWithSeconds);
}
xtstring CProjectFileTemplate::GetFullXMLFileName()
{
  xtstring sXMLFile;
  sXMLFile = GetBaseFolder();
  sXMLFile += SUBFOLDER_TEMPLATE;
  sXMLFile += _XT('/');
  sXMLFile += GetXMLFileName();
  return sXMLFile;
}

bool CProjectFileTemplate::SaveSyncObjectsIfChanged()
{
  if (!SyncObjectsCreated())
    return true;

  assert(m_pSyncMPTemplate);
  xtstring sXMLContent = GetEmptyXMLContent();
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLContent.c_str());
  // we have now XML document
  cXMLDoc.ResetPos();

  bool bRet = false;
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_TEMPLATE)
  {
    // in <X-Publisher-Template>
    if (cXMLDoc.AddChildElem(TEMPLATE_ELEM_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // in <Template>

      cXMLDoc.AddChildElem(PROPERTIES_ELEM_NAME, _XT(""));
      cXMLDoc.IntoElem();
      // in <Properties>
      m_pSyncMPTemplate->SaveXMLContent(&cXMLDoc);

      cXMLDoc.OutOfElem();
      // in <X-Publisher-Template>
      bRet = true;
      m_sXMLFileContent = cXMLDoc.GetDoc();
    }
  }
  if (m_pSyncDBTemplate)
    m_pSyncDBTemplate->SaveXMLContent(m_sDataFileContent);

  return bRet;
}

bool CProjectFileTemplate::CreateSyncObjects()
{
  DeleteSyncObjects();
  m_bInCreateSyncObjects = true;
  if (!m_pSyncMPTemplate)
  {
    m_pSyncMPTemplate = GetMPModelTemplateInd();
    if (!m_pSyncMPTemplate)
      // xml content yet empty
      m_pSyncMPTemplate = new CMPModelTemplateInd(this);
    if (m_pSyncMPTemplate->GetDataFormat() == tDataFormatST_Db)
    {
      m_pSyncDBTemplate = new CDBTemplateInd(this);
      if (m_pSyncDBTemplate)
      {
        m_pSyncDBTemplate->SetMPTemplate(m_pSyncMPTemplate);
        m_pSyncDBTemplate->ReadXMLContent(m_sDataFileContent);
      }
    }
  }
  m_bInCreateSyncObjects = false;
  return true;
}

bool CProjectFileTemplate::DeleteSyncObjects(bool bSaveSyncObjectsIfChanged /*= true*/)
{
  if (bSaveSyncObjectsIfChanged)
    SaveSyncObjectsIfChanged();
  if (m_pSyncMPTemplate)
  {
    delete m_pSyncMPTemplate;
    m_pSyncMPTemplate = NULL;
    if (m_pSyncDBTemplate)
    {
      delete m_pSyncDBTemplate;
      m_pSyncDBTemplate = NULL;
    }
  }
  return true;
}

bool CProjectFileTemplate::ContentChanged()
{
  xtstring sMD5Sum_XML;
  xtstring sMD5Sum_DATA;
  sMD5Sum_XML = GetMD5Sum(GetXMLFileContent());
  sMD5Sum_DATA = GetMD5Sum(GetDataFileContent());
  if (sMD5Sum_XML != GetMD5Sum_XML() && sMD5Sum_DATA != GetMD5Sum_DATA())
    return true;
  return false;
}

