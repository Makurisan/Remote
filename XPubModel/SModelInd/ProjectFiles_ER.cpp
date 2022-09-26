#include "ProjectFiles_ER.h"

#include <assert.h>

#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"
#include "../STools/Utils.h"




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProjectFileER
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProjectFileER::CProjectFileER(const xtstring& sBaseName,
                               bool bCreateObjectsImmediately)
               :CProjectFileBase(tProjectFileER),
                CSyncERNotifiersGroup(_XT("SetGroupNameLater"), tProjectFileER)
{
  m_bCreateObjectsImmediately = bCreateObjectsImmediately;
  m_bInCreateSyncObjects = false;
  m_pSyncMP = NULL;
  SetXMLFileContent(_XT(""));
  SetMD5Sum_XML(GetMD5Sum(_XT("")));
  SetMD5Sum_DATA(GetMD5Sum(_XT("")));

  SetTime_XML(0);
  SetTime_DATA(0);

  SetModuleName(sBaseName);
}

CProjectFileER::~CProjectFileER()
{
  DeleteSyncObjects(false);
}

void CProjectFileER::SetModuleName(const xtstring& sModuleName)
{
  SetModuleSyncGroupName(sModuleName);
  CProjectFileBase::SetModuleName(sModuleName);
  if (m_pSyncMP)
    m_pSyncMP->SetName(sModuleName);
}

xtstring CProjectFileER::GetFileContentForCopy()
{
  xtstring sFileContent;
  sFileContent = GetXMLFileName();
  sFileContent += _XT("\n");
  sFileContent += GetXMLFileContent();
  return sFileContent;
}

bool CProjectFileER::SetFileContentFromPaste(const xtstring& sContent)
{
  size_t nFileNameEnd = sContent.find(_XT("\n"), 0);
  if (nFileNameEnd == xtstring::npos)
    return false;
  // parse
  xtstring sFileName;
  xtstring sXMLFileContent;
  sFileName.assign(sContent.begin(), sContent.begin() + nFileNameEnd);
  sXMLFileContent.assign(sContent.begin() + nFileNameEnd + 1, sContent.end());
  // module name
  xtstring sModuleName;
  GetModuleNameFromXMLContent(sXMLFileContent, sModuleName);
  SetModuleName(sModuleName);
  // file name and content
  SetXMLFileName(sFileName);
  SetXMLFileContent(sXMLFileContent);
  return true;
}

const xtstring& CProjectFileER::GetXMLFileContent()
{
  if (!m_bInCreateSyncObjects)
    SaveSyncObjectsIfChanged();
  return m_sXMLFileContent;
}

void CProjectFileER::SetXMLFileContent(const xtstring& sContent)
{
  if (SyncObjectsCreated())
    DeleteSyncObjects();
  m_sXMLFileContent = sContent;
  CreateSyncObjects();
}

void CProjectFileER::operator = (CProjectFileER* pFile)
{
  if (!pFile)
    return;
  CProjectFileBase::operator = (pFile);
  SetXMLFileContent(pFile->GetXMLFileContent());
}

bool CProjectFileER::Rename(const xtstring& sNewBaseFileName)
{
  xtstring sOldBaseFileName = GetBaseFileName();

  xtstring sFolder = GetBaseFolder();
  if (sFolder.size() == 0)
    // nicht auf der Festplatte
    return false;
  sFolder += SUBFOLDER_ER;
  sFolder += _XT("/");

  // alte Datei
  xtstring sOldXMLFile = sFolder;
  sOldXMLFile += GetXMLFileName();

  SetBaseFileName(sNewBaseFileName);
  xtstring sNewXMLFile = sFolder;
  sNewXMLFile += GetXMLFileName();

  bool bRetVal = FileWrite(sNewXMLFile, GetXMLFileContent());
  if (!bRetVal)
  {
    SetBaseFileName(sOldBaseFileName);
    return false;
  }
  if (m_pDirChecker)
    m_pDirChecker->RemoveFile(sOldXMLFile);
  return true;
}

bool CProjectFileER::Reload(const xtstring& sBaseFolder)
{
  SetBaseFolder(sBaseFolder);

  bool bRet = false;
  xtstring sXMLFileContent;
  xtstring sFile;
  if (GetXMLFileName().length())
  {
    // die Datei existiert, nicht neue Datei
    // ganze Dateiname erzeugen
    sFile = sBaseFolder;
    sFile += SUBFOLDER_ER;
    sFile += _XT('/');
    sFile += GetXMLFileName();

    bRet = FileRead_Uncompressed(sFile, sXMLFileContent);
  }
  if (bRet)
  {
    SetXMLFileContent(sXMLFileContent);
    SetMD5Sum_XML(GetMD5Sum(sXMLFileContent));
    SetMD5Sum_DATA(GetMD5Sum(_XT("")));

    time_t timeChanged;
    GetFileTimeChanged(sFile, timeChanged);
    SetTime_XML(timeChanged);
    SetTime_DATA(0);

    xtstring sModuleName;
    GetModuleNameFromXMLContent(m_sXMLFileContent, sModuleName);
    SetModuleName(sModuleName);
  }
  return bRet;
}

bool CProjectFileER::ReloadAfterAlterByAnotherApp(bool bXMLChanged)
{
  return Reload(GetBaseFolder());
}

bool CProjectFileER::ReadXMLDocAndFile(const xtstring& sBaseFolder,
                                       CXPubMarkUp* pXMLDoc,
                                       const xtstring& sCompressedFileName)
{
  SetBaseFolder(sBaseFolder);

  // XMLDoc in <ER-File>
  SetXMLFileName(pXMLDoc->GetAttrib(ER_FILE_FILENAME));

  bool bRet = false;
  xtstring sXMLFileContent;
  xtstring sFile;
  if (GetXMLFileName().length())
  {
    // die Datei existiert, nicht neue Datei
    // ganze Dateiname erzeugen
    if (sCompressedFileName.size())
      sFile.clear();
    else
      sFile = sBaseFolder;
    sFile += SUBFOLDER_ER;
    sFile += _XT('/');
    sFile += GetXMLFileName();

    if (sCompressedFileName.size())
      bRet = FileRead_Compressed(sCompressedFileName, sFile, sXMLFileContent);
    else
      bRet = FileRead_Uncompressed(sFile, sXMLFileContent);
  }
  if (bRet)
  {
    SetXMLFileContent(sXMLFileContent);
    SetMD5Sum_XML(GetMD5Sum(sXMLFileContent));
    SetMD5Sum_DATA(GetMD5Sum(_XT("")));

    time_t timeChanged;
    GetFileTimeChanged(sFile, timeChanged);
    SetTime_XML(timeChanged);
    SetTime_DATA(0);

    xtstring sModuleName;
    GetModuleNameFromXMLContent(m_sXMLFileContent, sModuleName);
    SetModuleName(sModuleName);
  }
  return bRet;
}

bool CProjectFileER::SaveXMLDocAndFile(const xtstring& sBaseFolder,
                                       CXPubMarkUp* pXMLDoc)
{
  SetBaseFolder(sBaseFolder);

  if (m_pDirChecker)
    m_pDirChecker->CheckOrCreateDirectory(sBaseFolder, SUBFOLDER_ER);

  xtstring sFile;
  sFile = sBaseFolder;
  sFile += SUBFOLDER_ER;
  sFile += _XT('/');

  if (!GetXMLFileName().length())
    return false;

  // ganze Dateiname erzeugen
  sFile += GetXMLFileName();

  bool bDoSave = false;
  {
    xtstring sXMLFileContent = GetXMLFileContent();
    xtstring sMD5Sum_XML = GetMD5Sum(sXMLFileContent);
    if (sMD5Sum_XML != GetMD5Sum_XML())
      bDoSave = true;
    else
    {
      xtstring sXMLTemp;
      FileRead_Uncompressed(sFile, sXMLTemp);
      if (sXMLTemp != sXMLFileContent)
        bDoSave = true;
    }
  }
  if (bDoSave)
  {
    if (GetMPModelERModelInd())
      GetMPModelERModelInd()->SetAutor(CUtils::GetLoggedUser());
    // zuerst abspeichern
    xtstring sXMLFileContent = GetXMLFileContent();
    bool bRetVal = FileWrite(sFile, sXMLFileContent);
    if (!bRetVal)
      return false;
    SetMD5Sum_XML(GetMD5Sum(sXMLFileContent));
    SetMD5Sum_DATA(GetMD5Sum(_XT("")));

    time_t timeChanged;
    GetFileTimeChanged(sFile, timeChanged);
    SetTime_XML(timeChanged);
    SetTime_DATA(0);
  }

  if (pXMLDoc)
  {
    // XMLDoc in <ER-Files>
    if (pXMLDoc->AddChildElem(ER_FILE_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <ER-File>
      pXMLDoc->SetAttrib(ER_FILE_FILENAME, GetXMLFileName().c_str());

      pXMLDoc->OutOfElem();
    }
  }
  return true;
}

bool CProjectFileER::GetModuleNameFromXMLContent(const xtstring& sXMLContent,
                                                 xtstring& sModuleName)
{
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLContent.c_str());

  // we have now XML document
  cXMLDoc.ResetPos();

  bool bRet = false;

  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_ER)
  {
    // in <X-Publisher-ER>
    if (cXMLDoc.FindChildElem(ERMODEL_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <ER-Model>
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <MainProperty>
          xtstring sText;
          bRet = CMPModelERModelInd::ReadXMLModuleName(&cXMLDoc, sText);
          sModuleName = sText;
        }
      }
    }
  }

  return bRet;
}

bool CProjectFileER::FileRead_Compressed(const xtstring& sCompressedFileName,
                                         const xtstring& sFileName,
                                         xtstring& sXMLFileContent)
{
  sXMLFileContent.clear();

  if (!CProjectFileBase::ReadFile_Compressed(sCompressedFileName, sFileName, sXMLFileContent))
    return false;

  return true;
}

bool CProjectFileER::FileRead_Uncompressed(const xtstring& sFileName,
                                           xtstring& sXMLFileContent)
{
  sXMLFileContent.clear();

  if (!CProjectFileBase::ReadFile_Uncompressed(sFileName, sXMLFileContent))
    return false;

  return true;
}

bool CProjectFileER::FileWrite(const xtstring& sFileName,
                               const xtstring& sXMLFileContent)
{
  if (!CProjectFileBase::WriteFile(sFileName, sXMLFileContent))
    return false;

  return true;
}

CMPModelERModelInd* CProjectFileER::GetMPModelERModelInd()
{
  if (m_pSyncMP)
    return m_pSyncMP;

  // create XML object
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(GetXMLFileContent().c_str());
  // we have now XML document
  cXMLDoc.ResetPos();
  // 
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_ER)
  {
    // in <X-Publisher-ER>
    if (cXMLDoc.FindChildElem(ERMODEL_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <ER-Model>
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <MainProperty>

          CMPModelERModelInd* pModel;
          pModel = new CMPModelERModelInd(this);
          pModel->ReadXMLContent(&cXMLDoc);
          return pModel;
        }
      }
    }
  }

  return 0;
}

CERModelTableInd* CProjectFileER::CreateERModelTableInd()
{
  if (!m_bCreateObjectsImmediately)
    // In diesem Modus koennen wir nicht Objekte erzeugen
    return 0;

  CERModelTableInd* pER;
  pER = new CERModelTableInd(this);
  if (!pER)
    return 0;
  m_cERTables.push_back(pER);
  return pER;
}

bool CProjectFileER::RemoveERModelTableInd(LPCXTCHAR pName)
{
  if (!m_bCreateObjectsImmediately)
    // In diesem Modus koennen wir nicht Objekte loeschen
    return false;

  CERTablesIterator it = m_cERTables.begin();
  while (it != m_cERTables.end())
  {
    if ((*it)->GetName() == pName)
    {
      delete (*it);
      m_cERTables.erase(it);
      return true;
    }
    it++;
  }
  return false;
}

CERModelTableInd* CProjectFileER::GetERModelTableInd(LPCXTCHAR pName)
{
  // check
  if (!pName || pName[0] == 0)
    return 0;

  if (!m_bInCreateSyncObjects)
  {
    if (m_cERTables.size())
    {
      CERTablesIterator it = m_cERTables.begin();
      while (it != m_cERTables.end())
      {
        if ((*it)->GetName() == pName)
          return (*it);
        it++;
      }
      if (m_bCreateObjectsImmediately)
        return 0;
    }
  }

  size_t nInternIndex = 0;

  // create XML object
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(GetXMLFileContent().c_str());
  // we have now XML document
  cXMLDoc.ResetPos();
  // 
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_ER)
  {
    // in <X-Publisher-ER>
    if (cXMLDoc.FindChildElem(ERMODEL_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <ER-Model>
      if (cXMLDoc.FindChildElem(NODES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Nodes>
        while (cXMLDoc.FindChildElem(NODE_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <Node>

          // create model object
          CERModelTableInd* pModel;

          pModel = new CERModelTableInd(this);
          pModel->ReadXMLContent(&cXMLDoc);
          if (pModel->GetName() == pName)
          {
            // Wenn wir Objekte nicht sofort erzeugt haben und wir sind nicht in Objekt Erzeugen,
            // dann muessen wir es in Vector ablegen, falls es nicht drin ist
            if (!m_bInCreateSyncObjects && !m_bCreateObjectsImmediately && m_cERTables.size() <= nInternIndex)
                m_cERTables.push_back(pModel);
            return pModel;
          }

          if (!m_bInCreateSyncObjects && !m_bCreateObjectsImmediately && m_cERTables.size() <= nInternIndex)
            m_cERTables.push_back(pModel);
          else
            delete pModel;

          cXMLDoc.OutOfElem();
          // in <Nodes>

          nInternIndex++;
        }
      }
    }
  }

  return 0;
}

CERModelTableInd* CProjectFileER::GetERModelTableInd(size_t nIndex)
{
  size_t nInternIndex = 0;

  if (!m_bInCreateSyncObjects)
  {
    if (m_cERTables.size())
    {
      if (m_cERTables.size() <= nIndex)
      {
        if (m_bCreateObjectsImmediately)
          return 0;
      }
      else
      {
        CERTablesIterator it = m_cERTables.begin() + nIndex;
        return (*it);
      }
    }
    else
    {
      if (m_bCreateObjectsImmediately)
        return 0;
    }
  }

  // create XML object
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(GetXMLFileContent().c_str());
  // we have now XML document
  cXMLDoc.ResetPos();
  // 
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_ER)
  {
    // in <X-Publisher-ER>
    if (cXMLDoc.FindChildElem(ERMODEL_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <ER-Model>
      if (cXMLDoc.FindChildElem(NODES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Nodes>
        while (cXMLDoc.FindChildElem(NODE_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <Node>

          if (m_bInCreateSyncObjects)
          {
            if (nIndex == nInternIndex)
            {
              CERModelTableInd* pModel;

              pModel = new CERModelTableInd(this);
              pModel->ReadXMLContent(&cXMLDoc);
              return pModel;
            }
          }
          else
          {
            if (m_cERTables.size() <= nInternIndex)
            {
              // create model object
              CERModelTableInd* pModel;

              pModel = new CERModelTableInd(this);
              pModel->ReadXMLContent(&cXMLDoc);
              if (nIndex == nInternIndex)
              {
                // Wenn wir Objekte nicht sofort erzeugt haben und wir sind nicht in Objekt Erzeugen,
                // dann muessen wir es in Vector ablegen, falls es nicht drin ist
                if (!m_bInCreateSyncObjects && !m_bCreateObjectsImmediately && m_cERTables.size() <= nInternIndex)
                    m_cERTables.push_back(pModel);
                return pModel;
              }

              if (!m_bInCreateSyncObjects && !m_bCreateObjectsImmediately && m_cERTables.size() <= nInternIndex)
                m_cERTables.push_back(pModel);
              else
                delete pModel;
            }
          }

          cXMLDoc.OutOfElem();
          // in <Nodes>

          nInternIndex++;
        }
      }
    }
  }

  return 0;
}

void CProjectFileER::GetAllERModels(CxtstringVector& sNames)
{
  if (!m_bInCreateSyncObjects)
  {
    if (!m_bCreateObjectsImmediately)
    {
      // Es kann sein, dass noch keine CERModelTableInd erzeugt wurde.
      // Wir brauchen alle, deshalb machen wir spezielle Finte.
      for (size_t nI = 0; ; nI++)
      {
        CERModelTableInd* pTable = GetERModelTableInd(nI);
        if (!pTable)
          break;
      }
    }
    // sync objekte existieren
    CERTablesIterator it = m_cERTables.begin();
    while (it != m_cERTables.end())
    {
      sNames.push_back((*it)->GetName());
      it++;
    }
    return;
  }
  // diese Funktion kann nur ausserhalb dieser Klasse aufgerufen
  // deshalb koennen wir da nie kommen
  assert(false);
}

void CProjectFileER::GetCreatedAndChangedTime(xtstring& sCreated,
                                              xtstring& sChanged,
                                              bool bWithSeconds)
{
  CProjectFileBase::GetCreatedAndChangedTime(GetFullXMLFileName(), sCreated, sChanged, bWithSeconds);
}

xtstring CProjectFileER::GetFullXMLFileName()
{
  xtstring sXMLFile;
  sXMLFile = GetBaseFolder();
  sXMLFile += SUBFOLDER_ER;
  sXMLFile += _XT('/');
  sXMLFile += GetXMLFileName();
  return sXMLFile;
}

bool CProjectFileER::SaveSyncObjectsIfChanged()
{
  if (!SyncObjectsCreated())
    return true;

  if (!m_bCreateObjectsImmediately)
    // In diesem Modus koennen wir nichts abspeichern.
    return true;

  assert(m_pSyncMP);
  xtstring sXMLContent = GetEmptyXMLContent();
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLContent.c_str());
  // we have now XML document
  cXMLDoc.ResetPos();

  bool bRet = false;
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_ER)
  {
    // in <X-Publisher-ER>
    if (cXMLDoc.AddChildElem(ERMODEL_ELEM_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // in <ER-Model>

      cXMLDoc.AddChildElem(PROPERTIES_ELEM_NAME, _XT(""));
      cXMLDoc.IntoElem();
      // in <Properties>
      m_pSyncMP->SaveXMLContent(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // in <ER-Model>

      cXMLDoc.AddChildElem(NODES_ELEM_NAME, _XT(""));
      cXMLDoc.IntoElem();
      // in <Nodes>
      CERTablesIterator it = m_cERTables.begin();
      while (it != m_cERTables.end())
      {
        (*it)->SaveXMLContent(&cXMLDoc);
        it++;
      }
      cXMLDoc.OutOfElem();
      // in <ER-Model>

      cXMLDoc.OutOfElem();
      // in <X-Publisher-ER>
      bRet = true;
      m_sXMLFileContent = cXMLDoc.GetDoc();
    }
  }
  return bRet;
}

bool CProjectFileER::CreateSyncObjects()
{
  DeleteSyncObjects();
  m_bInCreateSyncObjects = true;

  if (!m_pSyncMP)
  {
    m_pSyncMP = GetMPModelERModelInd();
    if (!m_pSyncMP)
      // xml content yet empty
      m_pSyncMP = new CMPModelERModelInd(this);
  }
  if (m_bCreateObjectsImmediately)
  {
    for (size_t nI = 0;; nI++)
    {
      CERModelTableInd* pER = GetERModelTableInd(nI);
      if (pER)
        m_cERTables.push_back(pER);
      else
        break;
    }
  }

  m_bInCreateSyncObjects = false;
  return true;
}

bool CProjectFileER::DeleteSyncObjects(bool bSaveSyncObjectsIfChanged /*= true*/)
{
  if (bSaveSyncObjectsIfChanged)
    SaveSyncObjectsIfChanged();
  if (m_pSyncMP)
  {
    delete m_pSyncMP;
    m_pSyncMP = NULL;
  }
  if (m_cERTables.size())
  {
    CERTablesIterator it = m_cERTables.begin();
    while (it != m_cERTables.end())
    {
      delete (*it);
      it++;
    }
    m_cERTables.erase(m_cERTables.begin(), m_cERTables.end());
  }
  return true;
}

bool CProjectFileER::ContentChanged()
{
  xtstring sMD5Sum_XML;
  sMD5Sum_XML = GetMD5Sum(GetXMLFileContent());
  if (sMD5Sum_XML != GetMD5Sum_XML())
    return true;
  return false;
}
