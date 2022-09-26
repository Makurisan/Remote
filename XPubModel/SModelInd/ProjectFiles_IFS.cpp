#include "ProjectFiles_IFS.h"

#include <assert.h>

#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"
#include "STools/Utils.h"

#include "PropStoneInd.h"


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProjectFileIFS
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProjectFileIFS::CProjectFileIFS(const xtstring& sBaseName,
                                 bool bCreateObjectsImmediately)
                :CProjectFileBase(tProjectFileIFS),
                 CSyncIFSNotifiersGroup(_XT("SetGroupNameLater"), tProjectFileIFS)
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

CProjectFileIFS::~CProjectFileIFS()
{
  DeleteSyncObjects(false);
}

void CProjectFileIFS::SetModuleName(const xtstring& sModuleName)
{
  SetModuleSyncGroupName(sModuleName);
  CProjectFileBase::SetModuleName(sModuleName);
  if (m_pSyncMP)
    m_pSyncMP->SetName(sModuleName);
}

xtstring CProjectFileIFS::GetFileContentForCopy()
{
  xtstring sFileContent;
  sFileContent = GetXMLFileName();
  sFileContent += _XT("\n");
  sFileContent += GetXMLFileContent();
  return sFileContent;
}

bool CProjectFileIFS::SetFileContentFromPaste(const xtstring& sContent)
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

const xtstring& CProjectFileIFS::GetXMLFileContent()
{
  if (!m_bInCreateSyncObjects)
    SaveSyncObjectsIfChanged();
  return m_sXMLFileContent;
}

void CProjectFileIFS::SetXMLFileContent(const xtstring& sContent)
{
  if (SyncObjectsCreated())
    DeleteSyncObjects();
  m_sXMLFileContent = sContent;
  CreateSyncObjects();
}

void CProjectFileIFS::operator = (CProjectFileIFS* pFile)
{
  if (!pFile)
    return;
  CProjectFileBase::operator = (pFile);
  SetXMLFileContent(pFile->GetXMLFileContent());
}

bool CProjectFileIFS::Rename(const xtstring& sNewBaseFileName)
{
  xtstring sOldBaseFileName = GetBaseFileName();

  xtstring sFolder = GetBaseFolder();
  if (sFolder.size() == 0)
    // nicht auf der Festplatte
    return false;
  sFolder += SUBFOLDER_IFS;
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

bool CProjectFileIFS::Reload(const xtstring& sBaseFolder)
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
    sFile += SUBFOLDER_IFS;
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

bool CProjectFileIFS::ReloadAfterAlterByAnotherApp(bool bXMLChanged)
{
  return Reload(GetBaseFolder());
}

bool CProjectFileIFS::ReadXMLDocAndFile(const xtstring& sBaseFolder,
                                        CXPubMarkUp* pXMLDoc,
                                        const xtstring& sCompressedFileName)
{
  SetBaseFolder(sBaseFolder);

  // XMLDoc in <IFS-File>
  SetXMLFileName(pXMLDoc->GetAttrib(IFS_FILE_FILENAME));

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
    sFile += SUBFOLDER_IFS;
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

bool CProjectFileIFS::SaveXMLDocAndFile(const xtstring& sBaseFolder,
                                        CXPubMarkUp* pXMLDoc)
{
  SetBaseFolder(sBaseFolder);

  if (m_pDirChecker)
    m_pDirChecker->CheckOrCreateDirectory(sBaseFolder, SUBFOLDER_IFS);

  xtstring sFile;
  sFile = sBaseFolder;
  sFile += SUBFOLDER_IFS;
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
    if (GetMPModelIFSInd())
      GetMPModelIFSInd()->SetAutor(CUtils::GetLoggedUser());
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
    // XMLDoc in <IFS-Files>
    if (pXMLDoc->AddChildElem(IFS_FILE_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <IFS-File>
      pXMLDoc->SetAttrib(IFS_FILE_FILENAME, GetXMLFileName().c_str());

      pXMLDoc->OutOfElem();
    }
  }
  return true;
}

bool CProjectFileIFS::GetModuleNameFromXMLContent(const xtstring& sXMLContent,
                                                  xtstring& sModuleName)
{
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLContent.c_str());

  // we have now XML document
  cXMLDoc.ResetPos();

  bool bRet = false;

  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_IFS)
  {
    // in <X-Publisher-IFS>
    if (cXMLDoc.FindChildElem(IFSENTITY_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <IFS-Entity>
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <MainProperty>
          xtstring sText;
          bRet = CMPModelIFSInd::ReadXMLModuleName(&cXMLDoc, sText);
          sModuleName = sText;
        }
      }
    }
  }

  return bRet;
}

bool CProjectFileIFS::FileRead_Compressed(const xtstring& sCompressedFileName,
                                          const xtstring& sFileName,
                                          xtstring& sXMLFileContent)
{
  sXMLFileContent.clear();

  if (!CProjectFileBase::ReadFile_Compressed(sCompressedFileName, sFileName, sXMLFileContent))
    return false;

  return true;
}

bool CProjectFileIFS::FileRead_Uncompressed(const xtstring& sFileName,
                                            xtstring& sXMLFileContent)
{
  sXMLFileContent.clear();

  if (!CProjectFileBase::ReadFile_Uncompressed(sFileName, sXMLFileContent))
    return false;

  return true;
}

bool CProjectFileIFS::FileWrite(const xtstring& sFileName,
                                const xtstring& sXMLFileContent)
{
  if (!CProjectFileBase::WriteFile(sFileName, sXMLFileContent))
    return false;

  return true;
}

CMPModelIFSInd* CProjectFileIFS::GetMPModelIFSInd()
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
      && cXMLDoc.GetTagName() == MAIN_TAG_IFS)
  {
    // in <X-Publisher-IFS>
    if (cXMLDoc.FindChildElem(IFSENTITY_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <IFS-Entity>
      if (cXMLDoc.FindChildElem(PROPERTIES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Properties>
        if (cXMLDoc.FindChildElem(PROPERTY_MP_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <MainProperty>

          CMPModelIFSInd* pModel;
          pModel = new CMPModelIFSInd(this);
          pModel->ReadXMLContent(&cXMLDoc);
          return pModel;
        }
      }
    }
  }

  return 0;
}

CIFSEntityInd* CProjectFileIFS::GetMainIFSEntityInd()
{
  if (!m_bInCreateSyncObjects)
  {
    if (m_cIFSEntities.size())
    {
      CIFSEntityVectorIterator it = m_cIFSEntities.begin();
      while (it != m_cIFSEntities.end())
      {
        if ((*it)->GetMainEntity())
          return (*it);
        it++;
      }
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
      && cXMLDoc.GetTagName() == MAIN_TAG_IFS)
  {
    // in <X-Publisher-IFS>
    if (cXMLDoc.FindChildElem(IFSENTITY_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <IFS-Entity>
      if (cXMLDoc.FindChildElem(NODES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Nodes>
        while (cXMLDoc.FindChildElem(NODE_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <Node>

          // create model object
          CIFSEntityInd* pModel;

          pModel = new CIFSEntityInd(this);
          pModel->ReadXMLContent(&cXMLDoc);
          if (pModel->GetMainEntity() == true)
            return pModel;

          delete pModel;
          cXMLDoc.OutOfElem();
          // in <Nodes>
        }
      }
    }
  }

  return 0;
}

CIFSEntityInd* CProjectFileIFS::CreateIFSEntityInd()
{
  CIFSEntityInd* pIFS;
  pIFS = new CIFSEntityInd(this);
  if (!pIFS)
    return 0;
  m_cIFSEntities.push_back(pIFS);
  return pIFS;
}

bool CProjectFileIFS::RemoveIFSEntityInd(LPCXTCHAR pName)
{
  CIFSEntityVectorIterator it = m_cIFSEntities.begin();
  while (it != m_cIFSEntities.end())
  {
    if ((*it)->GetName() == pName)
    {
      delete (*it);
      m_cIFSEntities.erase(it);
      return true;
    }
    it++;
  }
  return false;
}

CIFSEntityInd* CProjectFileIFS::GetIFSEntityInd(LPCXTCHAR pName)
{
  // check
  if (!pName || pName[0] == 0)
    return 0;

  if (!m_bInCreateSyncObjects)
  {
    if (m_cIFSEntities.size())
    {
      CIFSEntityVectorIterator it = m_cIFSEntities.begin();
      while (it != m_cIFSEntities.end())
      {
        if ((*it)->GetName() == pName)
          return (*it);
        it++;
      }
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
      && cXMLDoc.GetTagName() == MAIN_TAG_IFS)
  {
    // in <X-Publisher-IFS>
    if (cXMLDoc.FindChildElem(IFSENTITY_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <IFS-Entity>
      if (cXMLDoc.FindChildElem(NODES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Nodes>
        while (cXMLDoc.FindChildElem(NODE_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <Node>

          // create model object
          CIFSEntityInd* pModel;

          pModel = new CIFSEntityInd(this);
          pModel->ReadXMLContent(&cXMLDoc);
          if (pModel->GetName() == pName)
            return pModel;

          delete pModel;
          cXMLDoc.OutOfElem();
          // in <Nodes>
        }
      }
    }
  }

  return 0;
}

CIFSEntityInd* CProjectFileIFS::GetIFSEntityInd(size_t nIndex)
{
  size_t nInternIndex = 0;

  if (!m_bInCreateSyncObjects)
  {
    if (m_cIFSEntities.size())
    {
      if (m_cIFSEntities.size() <= nIndex)
        return 0;
      CIFSEntityVectorIterator it = m_cIFSEntities.begin() + nIndex;
      return (*it);
    }
  }

  // create XML object
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(GetXMLFileContent().c_str());
  // we have now XML document
  cXMLDoc.ResetPos();
  // 
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_IFS)
  {
    // in <X-Publisher-IFS>
    if (cXMLDoc.FindChildElem(IFSENTITY_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <IFS-Entity>
      if (cXMLDoc.FindChildElem(NODES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Nodes>
        while (cXMLDoc.FindChildElem(NODE_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <Node>
          if (nIndex == nInternIndex)
          {
            // create model object
            CIFSEntityInd* pModel;

            pModel = new CIFSEntityInd(this);
            pModel->ReadXMLContent(&cXMLDoc);
            return pModel;
          }

          nInternIndex++;
          cXMLDoc.OutOfElem();
          // in <Nodes>
        }
      }
    }
  }

  return 0;
}

void CProjectFileIFS::GetAllEntitiesFromProjectFile(CxtstringVector& arrNames,
                                                    CxtstringVector& arrERNames,
                                                    CintVector& arrPropIDs,
                                                    CxtstringVector& arrDBStoneNames)
{
  if (!m_bInCreateSyncObjects)
  {
    if (m_cIFSEntities.size())
    {
      CIFSEntityVectorIterator it = m_cIFSEntities.begin();
      while (it != m_cIFSEntities.end())
      {
        arrNames.push_back((*it)->GetName());
        xtstring sTextToSet;
        sTextToSet = (*it)->GetERTable_FromERTableOrGlobalTable();
        arrERNames.push_back(sTextToSet);
        arrPropIDs.push_back((*it)->GetPropStoneTypeFromRaw());
        sTextToSet = (*it)->GetDBStoneNameFromRaw();
        arrDBStoneNames.push_back(sTextToSet);
        it++;
      }
      return;
    }
  }
  // diese Funktion kann nur ausserhalb dieser Klasse aufgerufen
  // deshalb koennen wir da nie kommen
  assert(false);
}

void CProjectFileIFS::GetAllEntitiesFromProjectFile(bool bStartEntityToo,
                                                    const xtstring& sStartEntity,
                                                    CxtstringVector& arrNames,
                                                    CxtstringVector& arrERNames,
                                                    CintVector& arrPropIDs,
                                                    CxtstringVector& arrDBStoneNames)
{
  CIFSEntityInd* pStartEntity = GetIFSEntityInd(sStartEntity.c_str());

  if (!pStartEntity)
  {
    // Zu folgendem Fall kann kommen, wenn ich in ener Entit]t Treffer habe
    // und diesem Treffer nach Entitaet in ganz andere IFS einfuegen will.
    // Zur Zeit kontrollieren wir nicht von welcher Entitaet ist SubIFS aufgerufen,
    // deshalbe liefern wir zurueck einfach alles von IFS.
    GetAllEntitiesFromProjectFile(arrNames, arrERNames, arrPropIDs, arrDBStoneNames);
    return;
  }
  
  if (bStartEntityToo && arrNames.size() == 0)
  {
    arrNames.push_back(pStartEntity->GetName());
    xtstring sTextToSet;
    sTextToSet = pStartEntity->GetERTable_FromERTableOrGlobalTable();
    arrERNames.push_back(sTextToSet);
    arrPropIDs.push_back(pStartEntity->GetPropStoneTypeFromRaw());
    sTextToSet = pStartEntity->GetDBStoneNameFromRaw();
    arrDBStoneNames.push_back(sTextToSet);
  }

  for (size_t nI = 0; nI < pStartEntity->CountOfConnections(); nI++)
  {
    CIFSEntityConnectionInd* pConn = pStartEntity->GetConnection(nI);
    if (!pConn)
      continue;
    xtstring sParentEntity;
    sParentEntity = pConn->GetParentEntity();
    CIFSEntityInd* pParentEntity = GetIFSEntityInd(sParentEntity.c_str());
    assert(pParentEntity);
    if (pParentEntity)
    {
      arrNames.push_back(pParentEntity->GetName());
      xtstring sTextToSet;
      sTextToSet = pParentEntity->GetERTable_FromERTableOrGlobalTable();
      arrERNames.push_back(sTextToSet);
      arrPropIDs.push_back(pParentEntity->GetPropStoneTypeFromRaw());
      sTextToSet = pParentEntity->GetDBStoneNameFromRaw();
      arrDBStoneNames.push_back(sTextToSet);
    }
    GetAllEntitiesFromProjectFile(bStartEntityToo, sParentEntity, arrNames, arrERNames, arrPropIDs, arrDBStoneNames);
  }
}

void CProjectFileIFS::GetAllVariablesFromProjectFile(const xtstring& sStartEntity,
                                                     CxtstringVector& xtarrEntityNames,
                                                     CxtstringVector& xtarrVariableNames)
{
  CIFSEntityInd* pStartEntity = GetIFSEntityInd(sStartEntity.c_str());

  assert(pStartEntity);
  if (!pStartEntity)
    return;

  for (size_t nI = 0; nI < pStartEntity->CountOfConnections(); nI++)
  {
    CIFSEntityConnectionInd* pConn = pStartEntity->GetConnection(nI);
    if (!pConn)
      continue;
    xtstring sParentEntity;
    sParentEntity = pConn->GetParentEntity();
    CIFSEntityInd* pParentEntity = GetIFSEntityInd(sParentEntity.c_str());
    assert(pParentEntity);
    if (pParentEntity)
    {
      for (size_t nI = 0; nI < pParentEntity->CountOfIFSVariableInds(); nI++)
      {
        CIFSEntityVariableInd* pVar = pParentEntity->GetIFSVariableInd(nI);
        if (!pVar)
          continue;
        xtarrEntityNames.push_back(pParentEntity->GetName());
        xtarrVariableNames.push_back(pVar->GetName());
      }
    }
    GetAllVariablesFromProjectFile(sParentEntity, xtarrEntityNames, xtarrVariableNames);
  }
}

void CProjectFileIFS::GetCreatedAndChangedTime(xtstring& sCreated,
                                               xtstring& sChanged,
                                               bool bWithSeconds)
{
  CProjectFileBase::GetCreatedAndChangedTime(GetFullXMLFileName(), sCreated, sChanged, bWithSeconds);
}
xtstring CProjectFileIFS::GetFullXMLFileName()
{
  xtstring sXMLFile;
  sXMLFile = GetBaseFolder();
  sXMLFile += SUBFOLDER_IFS;
  sXMLFile += _XT('/');
  sXMLFile += GetXMLFileName();
  return sXMLFile;
}

bool CProjectFileIFS::SaveSyncObjectsIfChanged()
{
  if (!SyncObjectsCreated())
    return true;

  assert(m_pSyncMP);
  xtstring sXMLContent = GetEmptyXMLContent();
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLContent.c_str());
  // we have now XML document
  cXMLDoc.ResetPos();

  bool bRet = false;
  if (cXMLDoc.FindElem()
      && cXMLDoc.GetTagName() == MAIN_TAG_IFS)
  {
    // in <X-Publisher-IFS>
    if (cXMLDoc.AddChildElem(IFSENTITY_ELEM_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // in <IFS-Entity>

      cXMLDoc.AddChildElem(PROPERTIES_ELEM_NAME, _XT(""));
      cXMLDoc.IntoElem();
      // in <Properties>
      m_pSyncMP->SaveXMLContent(&cXMLDoc);
      cXMLDoc.OutOfElem();
      // in <IFS-Entity>

      cXMLDoc.AddChildElem(NODES_ELEM_NAME, _XT(""));
      cXMLDoc.IntoElem();
      // in <Nodes>
      CIFSEntityVectorIterator it = m_cIFSEntities.begin();
      while (it != m_cIFSEntities.end())
      {
        (*it)->SaveXMLContent(&cXMLDoc);
        it++;
      }
      cXMLDoc.OutOfElem();
      // in <IFS-Entity>

      cXMLDoc.OutOfElem();
      // in <X-Publisher-IFS>
      bRet = true;
      m_sXMLFileContent = cXMLDoc.GetDoc();
    }
  }
  return bRet;
}

bool CProjectFileIFS::CreateSyncObjects()
{
  DeleteSyncObjects();
  m_bInCreateSyncObjects = true;
  if (!m_pSyncMP)
  {
    m_pSyncMP = GetMPModelIFSInd();
    if (!m_pSyncMP)
      // xml content yet empty
      m_pSyncMP = new CMPModelIFSInd(this);
  }
  for (size_t nI = 0;; nI++)
  {
    CIFSEntityInd* pIFS = GetIFSEntityInd(nI);
    if (pIFS)
      m_cIFSEntities.push_back(pIFS);
    else
      break;
  }
  m_bInCreateSyncObjects = false;
  return true;
}

bool CProjectFileIFS::DeleteSyncObjects(bool bSaveSyncObjectsIfChanged /*= true*/)
{
  if (bSaveSyncObjectsIfChanged)
    SaveSyncObjectsIfChanged();
  if (m_pSyncMP)
  {
    delete m_pSyncMP;
    m_pSyncMP = NULL;
  }
  if (m_cIFSEntities.size())
  {
    CIFSEntityVectorIterator it = m_cIFSEntities.begin();
    while (it != m_cIFSEntities.end())
    {
      delete (*it);
      it++;
    }
    m_cIFSEntities.erase(m_cIFSEntities.begin(), m_cIFSEntities.end());
  }
  return true;
}

bool CProjectFileIFS::ContentChanged()
{
  xtstring sMD5Sum_XML;
  sMD5Sum_XML = GetMD5Sum(GetXMLFileContent());
  if (sMD5Sum_XML != GetMD5Sum_XML())
    return true;
  return false;
}

