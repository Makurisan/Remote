#include <assert.h>
#include "SyncEngine.h"
#include "../SModelInd/ModelInd.h"
#include "../SModelInd/IFSEntityInd.h"



CSyncEngine::CSyncEngine()
{
  m_pSyncEngine = this;

  // Die Gruppen sind nicht in m_cXXGroups eingefuegt.
  // Diese Gruppen erledigen es selbst.
  CSyncERNotifiersGroup*  pNewER = new  CSyncERNotifiersGroup(DEFAULT_GROUP_NAME, tProjectFile_Nothing);
  pNewER = pNewER; // wegen warning
  CSyncIFSNotifiersGroup* pNewIFS = new CSyncIFSNotifiersGroup(DEFAULT_GROUP_NAME, tProjectFile_Nothing);
  pNewIFS = pNewIFS; // wegen warning
  CSyncWPNotifiersGroup*  pNewWP = new  CSyncWPNotifiersGroup(DEFAULT_GROUP_NAME, tProjectFile_Nothing);
  pNewWP = pNewWP; // wegen warning
}

CSyncEngine::~CSyncEngine()
{
  assert(m_cERGroups.size() == 1);
  assert(m_cIFSGroups.size() == 1);
  assert(m_cWPGroups.size() == 1);
#ifdef _DEBUG
  size_t n;

  n = m_cProjResponseGroup.size();
  assert(m_cProjResponseGroup.size() == 0);
  for (CProjResponseGroupIterator it = m_cProjResponseGroup.begin(); it != m_cProjResponseGroup.end(); it++)
  {
    CSyncResponse_Proj* p = *it;
    TTypeOfResponseObject nType = p->GetTypeOfResponseObject();
    nType = nType; // unused variable
  }

  n = m_cERResponseGroup.size();
  assert(m_cERResponseGroup.size() == 0);
  for (CERResponseGroupIterator it = m_cERResponseGroup.begin(); it != m_cERResponseGroup.end(); it++)
  {
    CSyncResponse_ER* p = *it;
    TTypeOfResponseObject nType = p->GetTypeOfResponseObject();
    nType = nType; // unused variable
  }

  n = m_cIFSResponseGroup.size();
  assert(m_cIFSResponseGroup.size() == 0);
  for (CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin(); it != m_cIFSResponseGroup.end(); it++)
  {
    CSyncResponse_IFS* p = *it;
    TTypeOfResponseObject nType = p->GetTypeOfResponseObject();
    nType = nType; // unused variable
  }

  n = m_cWPResponseGroup.size();
  assert(m_cWPResponseGroup.size() == 0);
  for (CWPResponseGroupIterator it = m_cWPResponseGroup.begin(); it != m_cWPResponseGroup.end(); it++)
  {
    CSyncResponse_WP* p = *it;
    TTypeOfResponseObject nType = p->GetTypeOfResponseObject();
    nType = nType; // unused variable
  }
#endif

  CERGroupsIterator itER = m_cERGroups.begin();
  delete (*itER);
  m_cERGroups.erase(m_cERGroups.begin(), m_cERGroups.end());

  CIFSGroupsIterator itIFS = m_cIFSGroups.begin();
  delete (*itIFS);
  m_cIFSGroups.erase(m_cIFSGroups.begin(), m_cIFSGroups.end());

  CWPGroupsIterator itWP = m_cWPGroups.begin();
  delete (*itWP);
  m_cWPGroups.erase(m_cWPGroups.begin(), m_cWPGroups.end());
}

CSyncERNotifiersGroup* CSyncEngine::GetDefaultERGroup()
{
  return (*m_cERGroups.begin());
}

CSyncIFSNotifiersGroup* CSyncEngine::GetDefaultIFSGroup()
{
  return (*m_cIFSGroups.begin());
}

CSyncWPNotifiersGroup* CSyncEngine::GetDefaultWPGroup()
{
  return (*m_cWPGroups.begin());
}



bool CSyncEngine::AddERGroup(CSyncERNotifiersGroup* pGroup)
{
  size_t nCount = m_cERGroups.size();
  m_cERGroups.push_back(pGroup);
  return (m_cERGroups.size() != nCount);
}
bool CSyncEngine::RemoveERGroup(CSyncERNotifiersGroup* pGroup)
{
  CERGroupsIterator cElement;
  cElement = m_cERGroups.begin();
  while (cElement != m_cERGroups.end())
  {
    if (pGroup == *cElement)
    {
      m_cERGroups.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
bool CSyncEngine::AddIFSGroup(CSyncIFSNotifiersGroup* pGroup)
{
  size_t nCount = m_cIFSGroups.size();
  m_cIFSGroups.push_back(pGroup);
  return (m_cIFSGroups.size() != nCount);
}
bool CSyncEngine::RemoveIFSGroup(CSyncIFSNotifiersGroup* pGroup)
{
  CIFSGroupsIterator cElement;
  cElement = m_cIFSGroups.begin();
  while (cElement != m_cIFSGroups.end())
  {
    if (pGroup == *cElement)
    {
      m_cIFSGroups.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
bool CSyncEngine::AddWPGroup(CSyncWPNotifiersGroup* pGroup)
{
  size_t nCount = m_cWPGroups.size();
  m_cWPGroups.push_back(pGroup);
  return (m_cWPGroups.size() != nCount);
}
bool CSyncEngine::RemoveWPGroup(CSyncWPNotifiersGroup* pGroup)
{
  CWPGroupsIterator cElement;
  cElement = m_cWPGroups.begin();
  while (cElement != m_cWPGroups.end())
  {
    if (pGroup == *cElement)
    {
      m_cWPGroups.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}



bool CSyncEngine::AddCommonResponse(CSyncResponse_Common* pResponse)
{
  size_t nCount = m_cCommonResponseGroup.size();
  m_cCommonResponseGroup.push_back(pResponse);
  return (m_cCommonResponseGroup.size() != nCount);
}
bool CSyncEngine::RemoveCommonResponse(CSyncResponse_Common* pResponse)
{
  CCommonResponseGroupIterator cElement;
  cElement = m_cCommonResponseGroup.begin();
  while (cElement != m_cCommonResponseGroup.end())
  {
    if (pResponse == *cElement)
    {
      m_cCommonResponseGroup.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
bool CSyncEngine::AddProjResponse(CSyncResponse_Proj* pResponse)
{
  size_t nCount = m_cProjResponseGroup.size();
  m_cProjResponseGroup.push_back(pResponse);
  return (m_cProjResponseGroup.size() != nCount);
}
bool CSyncEngine::RemoveProjResponse(CSyncResponse_Proj* pResponse)
{
  CProjResponseGroupIterator cElement;
  cElement = m_cProjResponseGroup.begin();
  while (cElement != m_cProjResponseGroup.end())
  {
    if (pResponse == *cElement)
    {
      m_cProjResponseGroup.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
bool CSyncEngine::AddERResponse(CSyncResponse_ER* pResponse)
{
  size_t nCount = m_cERResponseGroup.size();
  m_cERResponseGroup.push_back(pResponse);
  return (m_cERResponseGroup.size() != nCount);
}
bool CSyncEngine::RemoveERResponse(CSyncResponse_ER* pResponse)
{
  CERResponseGroupIterator cElement;
  cElement = m_cERResponseGroup.begin();
  while (cElement != m_cERResponseGroup.end())
  {
    if (pResponse == *cElement)
    {
      m_cERResponseGroup.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
bool CSyncEngine::AddIFSResponse(CSyncResponse_IFS* pResponse)
{
  size_t nCount = m_cIFSResponseGroup.size();
  m_cIFSResponseGroup.push_back(pResponse);
  return (m_cIFSResponseGroup.size() != nCount);
}
bool CSyncEngine::RemoveIFSResponse(CSyncResponse_IFS* pResponse)
{
  CIFSResponseGroupIterator cElement;
  cElement = m_cIFSResponseGroup.begin();
  while (cElement != m_cIFSResponseGroup.end())
  {
    if (pResponse == *cElement)
    {
      m_cIFSResponseGroup.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
bool CSyncEngine::AddWPResponse(CSyncResponse_WP* pResponse)
{
  size_t nCount = m_cWPResponseGroup.size();
  m_cWPResponseGroup.push_back(pResponse);
  return (m_cWPResponseGroup.size() != nCount);
}
bool CSyncEngine::RemoveWPResponse(CSyncResponse_WP* pResponse)
{
  CWPResponseGroupIterator cElement;
  cElement = m_cWPResponseGroup.begin();
  while (cElement != m_cWPResponseGroup.end())
  {
    if (pResponse == *cElement)
    {
      m_cWPResponseGroup.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}








////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// exist functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_TableExist(const xtstring& sTableName)
{
  CCommonResponseGroupIterator it = m_cCommonResponseGroup.begin();
  while (it != m_cCommonResponseGroup.end())
  {
    if ((*it)->SE_TableExist(sTableName))
      return true;
    it++;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_FieldExist(const xtstring& sFieldName)
{
  CCommonResponseGroupIterator it = m_cCommonResponseGroup.begin();
  while (it != m_cCommonResponseGroup.end())
  {
    if ((*it)->SE_FieldExist(sFieldName))
      return true;
    it++;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_EntityExist(const xtstring& sEntityName)
{
  CCommonResponseGroupIterator it = m_cCommonResponseGroup.begin();
  while (it != m_cCommonResponseGroup.end())
  {
    if ((*it)->SE_EntityExist(sEntityName))
      return true;
    it++;
  }
  return false;
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// functions notifier to response
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeProjConstant(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CProjResponseGroupIterator it = m_cProjResponseGroup.begin();
  while (it != m_cProjResponseGroup.end())
  {
    (*it)->ROC_ChangeProjConstant(sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeProjConstant(sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_Proj*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeProjConstant(sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeProjConstant_Post(sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForProjConstant(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CProjResponseGroupIterator it = m_cProjResponseGroup.begin();
  while (it != m_cProjResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForProjConstant(sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteProjConstant(const xtstring& sName,
                                        size_t& nCountOfReferences,
                                        bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CProjResponseGroupIterator it = m_cProjResponseGroup.begin();
  while (it != m_cProjResponseGroup.end())
  {
    (*it)->ROC_DeleteProjConstant(sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteProjConstant(sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_Proj*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteProjConstant(sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteProjConstant_Post(sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeSymbolicDBName(const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CProjResponseGroupIterator it = m_cProjResponseGroup.begin();
  while (it != m_cProjResponseGroup.end())
  {
    (*it)->ROC_ChangeSymbolicDBName(sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeSymbolicDBName(sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_Proj*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeSymbolicDBName(sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeSymbolicDBName_Post(sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CProjResponseGroupIterator it = m_cProjResponseGroup.begin();
  while (it != m_cProjResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForSymbolicDBName(sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteSymbolicDBName(const xtstring& sName,
                                          size_t& nCountOfReferences,
                                          bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CProjResponseGroupIterator it = m_cProjResponseGroup.begin();
  while (it != m_cProjResponseGroup.end())
  {
    (*it)->ROC_DeleteSymbolicDBName(sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteSymbolicDBName(sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_Proj*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteSymbolicDBName(sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteSymbolicDBName_Post(sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeERModelName(const xtstring& sOldName,
                                       const xtstring& sNewName,
                                       bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_ChangeERModelName(sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeERModelName(sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeERModelName(sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeERModelName_Post(sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForERModel(const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForERModel(sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteERModel(const xtstring& sName,
                                   size_t& nCountOfReferences,
                                   bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_DeleteERModel(sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteERModel(sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteERModel(sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteERModel_Post(sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeTableName(const xtstring& sERModelName,
                                     const xtstring& sOldName,
                                     const xtstring& sNewName,
                                     bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_ChangeTableName(sERModelName, sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeTableName(sERModelName, sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeTableName(sERModelName, sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeTableName_Post(sERModelName, sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForTable(const xtstring& sERModelName,
                                             const xtstring& sName,
                                             CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForTable(sERModelName, sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteTable(const xtstring& sERModelName,
                                 const xtstring& sName,
                                 size_t& nCountOfReferences,
                                 bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_DeleteTable(sERModelName, sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteTable(sERModelName, sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteTable(sERModelName, sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteTable_Post(sERModelName, sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeFieldName(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sOldName,
                                     const xtstring& sNewName,
                                     bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_ChangeFieldName(sERModelName, sTableName, sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeFieldName(sERModelName, sTableName, sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeFieldName(sERModelName, sTableName, sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeFieldName_Post(sERModelName, sTableName, sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForField(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sFieldName,
                                             CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForField(sERModelName, sTableName, sFieldName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteField(const xtstring& sERModelName,
                                 const xtstring& sTableName,
                                 const xtstring& sFieldName,
                                 size_t& nCountOfReferences,
                                 bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_DeleteField(sERModelName, sTableName, sFieldName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteField(sERModelName, sTableName, sFieldName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteField(sERModelName, sTableName, sFieldName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteField_Post(sERModelName, sTableName, sFieldName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeParamName(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sOldName,
                                     const xtstring& sNewName,
                                     bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_ChangeParamName(sERModelName, sTableName, sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeParamName(sERModelName, sTableName, sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeParamName(sERModelName, sTableName, sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeParamName_Post(sERModelName, sTableName, sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForParam(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParamName,
                                             CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForParam(sERModelName, sTableName, sParamName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteParam(const xtstring& sERModelName,
                                 const xtstring& sTableName,
                                 const xtstring& sParamName,
                                 size_t& nCountOfReferences,
                                 bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_DeleteParam(sERModelName, sTableName, sParamName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteParam(sERModelName, sTableName, sParamName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteParam(sERModelName, sTableName, sParamName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteParam_Post(sERModelName, sTableName, sParamName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeRelationName(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        bool bCancelAllowed)
{
  // !!!!!!!!!!!!!!!!!!!
  // Ausnahme - in deisem Fall werden wir dem Benutzer gar nicht fragen
  // !!!!!!!!!!!!!!!!!!!
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_ChangeRelationName(sERModelName, sTableName, sParentTableName, sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange = true;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeRelationName(sERModelName, sTableName, sParentTableName, sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeRelationName(sERModelName, sTableName, sParentTableName, sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeRelationName_Post(sERModelName, sTableName, sParentTableName, sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForRelation(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteRelation(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sParentTableName,
                                    const xtstring& sName,
                                    bool bCancelAllowed)
{
  // !!!!!!!!!!!!!!!!!!!
  // Ausnahme - in deisem Fall werden wir dem Benutzer gar nicht fragen
  // !!!!!!!!!!!!!!!!!!!
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
    it++;
  }

  bool bDoChange = true;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteRelation_Post(sERModelName, sTableName, sParentTableName, sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ParamAdded(const xtstring& sERModelName,
                                const xtstring& sTableName,
                                const xtstring& sParamName,
                                CERModelParamInd* pParam,
                                bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_ParamAdded(sERModelName, sTableName, sParamName, pParam, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ParamAdded(sERModelName, sTableName, sParamName, pParam, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_ParamAdded(sERModelName, sTableName, sParamName, pParam, *itUsers);
      itUsers++;
    }
    SEROC_ParamAdded_Post(sERModelName, sTableName, sParamName, pParam, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ParamChanged(const xtstring& sERModelName,
                                  const xtstring& sTableName,
                                  const xtstring& sParamName,
                                  CERModelParamInd* pParam,
                                  bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_ParamChanged(sERModelName, sTableName, sParamName, pParam, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ParamChanged(sERModelName, sTableName, sParamName, pParam, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_ParamChanged(sERModelName, sTableName, sParamName, pParam, *itUsers);
      itUsers++;
    }
    SEROC_ParamChanged_Post(sERModelName, sTableName, sParamName, pParam, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_RelationAdded(const xtstring& sERModelName,
                                   const xtstring& sTableName,
                                   const xtstring& sParentTableName,
                                   const xtstring& sName,
                                   CERModelRelationInd* pRelation,
                                   bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_RelationAdded(sERModelName, sTableName, sParentTableName, sName, pRelation, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_RelationAdded(sERModelName, sTableName, sParentTableName, sName, pRelation, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_RelationAdded(sERModelName, sTableName, sParentTableName, sName, pRelation, *itUsers);
      itUsers++;
    }
    SEROC_RelationAdded_Post(sERModelName, sTableName, sParentTableName, sName, pRelation, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_RelationChanged(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sParentTableName,
                                     const xtstring& sName,
                                     CERModelRelationInd* pRelation,
                                     bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CERResponseGroupIterator it = m_cERResponseGroup.begin();
  while (it != m_cERResponseGroup.end())
  {
    (*it)->ROC_RelationChanged(sERModelName, sTableName, sParentTableName, sName, pRelation, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_RelationChanged(sERModelName, sTableName, sParentTableName, sName, pRelation, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_ER*)(*itUsers)->m_pSyncResponseObject)->ROC_RelationChanged(sERModelName, sTableName, sParentTableName, sName, pRelation, *itUsers);
      itUsers++;
    }
    SEROC_RelationChanged_Post(sERModelName, sTableName, sParentTableName, sName, pRelation, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeIFSName(const xtstring& sOldName,
                                   const xtstring& sNewName,
                                   bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_ChangeIFSName(sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeIFSName(sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_IFS*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeIFSName(sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeIFSName_Post(sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForIFS(const xtstring& sName,
                                           CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForIFS(sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteIFS(const xtstring& sName,
                               size_t& nCountOfReferences,
                               bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_DeleteIFS(sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteIFS(sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_IFS*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteIFS(sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteIFS_Post(sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeEntityName(const xtstring& sIFS,
                                      const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_ChangeEntityName(sIFS, sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeEntityName(sIFS, sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_IFS*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeEntityName(sIFS, sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeEntityName_Post(sIFS, sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForEntity(const xtstring& sIFS,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForEntity(sIFS, sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteEntity(const xtstring& sIFS,
                                  const xtstring& sName,
                                  size_t& nCountOfReferences,
                                  bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_DeleteEntity(sIFS, sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteEntity(sIFS, sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_IFS*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteEntity(sIFS, sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteEntity_Post(sIFS, sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeVariableName(const xtstring& sIFS,
                                        const xtstring& sEntityName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_ChangeVariableName(sIFS, sEntityName, sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeVariableName(sIFS, sEntityName, sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_IFS*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeVariableName(sIFS, sEntityName, sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeVariableName_Post(sIFS, sEntityName, sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForVariable(const xtstring& sIFS,
                                                const xtstring& sEntityName,
                                                const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForVariable(sIFS, sEntityName, sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteVariable(const xtstring& sIFS,
                                    const xtstring& sEntityName,
                                    const xtstring& sName,
                                    size_t& nCountOfReferences,
                                    bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteVariable(sIFS, sEntityName, sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_IFS*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteVariable(sIFS, sEntityName, sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteVariable_Post(sIFS, sEntityName, sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeGlobalERTableName(const xtstring& sIFS,
                                             const xtstring sOldName,
                                             const xtstring& sNewName,
                                             bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_ChangeGlobalERTableName(sIFS, sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeGlobalERTableName(sIFS, sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_IFS*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeGlobalERTableName(sIFS, sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeGlobalERTableName_Post(sIFS, sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForGlobalERTable(sIFS, sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteGlobalERTable(const xtstring& sIFS,
                                         const xtstring& sName,
                                         size_t& nCountOfReferences,
                                         bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CIFSResponseGroupIterator it = m_cIFSResponseGroup.begin();
  while (it != m_cIFSResponseGroup.end())
  {
    (*it)->ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteGlobalERTable(sIFS, sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_IFS*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteGlobalERTable(sIFS, sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteGlobalERTable_Post(sIFS, sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeWPName(const xtstring& sOldName,
                                  const xtstring& sNewName,
                                  bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CWPResponseGroupIterator it = m_cWPResponseGroup.begin();
  while (it != m_cWPResponseGroup.end())
  {
    (*it)->ROC_ChangeWPName(sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeWPName(sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_WP*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeWPName(sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeWPName_Post(sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForWP(const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CWPResponseGroupIterator it = m_cWPResponseGroup.begin();
  while (it != m_cWPResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForWP(sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteWP(const xtstring& sName,
                              size_t& nCountOfReferences,
                              bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CWPResponseGroupIterator it = m_cWPResponseGroup.begin();
  while (it != m_cWPResponseGroup.end())
  {
    (*it)->ROC_DeleteWP(sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteWP(sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_WP*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteWP(sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteWP_Post(sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_ChangeWPConstantName(const xtstring& sWPName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CWPResponseGroupIterator it = m_cWPResponseGroup.begin();
  while (it != m_cWPResponseGroup.end())
  {
    (*it)->ROC_ChangeWPConstantName(sWPName, sOldName, sNewName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_ChangeWPConstantName(sWPName, sOldName, sNewName, cUsers, bCancelAllowed);

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_WP*)(*itUsers)->m_pSyncResponseObject)->ROC_ChangeWPConstantName(sWPName, sOldName, sNewName, *itUsers);
      itUsers++;
    }
    SEROC_ChangeWPConstantName_Post(sWPName, sOldName, sNewName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_CheckReferencesForWPConstant(const xtstring& sWPName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  // alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CWPResponseGroupIterator it = m_cWPResponseGroup.begin();
  while (it != m_cWPResponseGroup.end())
  {
    (*it)->ROC_CheckReferencesForWPConstant(sWPName, sName, cUsers);
    it++;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CSyncEngine::SE_DeleteWPConstant(const xtstring& sWPName,
                                      const xtstring& sName,
                                      size_t& nCountOfReferences,
                                      bool bCancelAllowed)
{
  CUserOfObjectArray cUsers;

  // zuerst alle Objekte sammeln
  // die Objekte muessen sich in cUsers eintragen,
  // wenn diese was dazu sagen moechten
  CWPResponseGroupIterator it = m_cWPResponseGroup.begin();
  while (it != m_cWPResponseGroup.end())
  {
    (*it)->ROC_DeleteWPConstant(sWPName, sName, cUsers);
    it++;
  }

  bool bDoChange;
  // show dialog
  // in dem Dialog ist definiert, ob die Aenderung ueberhapt gemacht werden darf
  bDoChange = SEROC_DeleteWPConstant(sWPName, sName, cUsers, bCancelAllowed);

  nCountOfReferences = cUsers.size();

  if (bDoChange)
  {
    // allen Objekten Bescheid geben, dass diese die eingestellt Aktion
    // machen muessen
    CUserOfObjectArrayIterator itUsers = cUsers.begin();
    while (itUsers != cUsers.end())
    {
      ((CSyncResponse_WP*)(*itUsers)->m_pSyncResponseObject)->ROC_DeleteWPConstant(sWPName, sName, *itUsers);
      itUsers++;
    }
    SEROC_DeleteWPConstant_Post(sWPName, sName, cUsers);
  }

  CUserOfObjectArrayIterator itUs = cUsers.begin();
  while (itUs != cUsers.end())
  {
    delete (*itUs);
    itUs++;
  }
  cUsers.erase(cUsers.begin(), cUsers.end());

  return bDoChange;
}
















void CSyncEngine::SE_SymbolicDBNameAddedDeletedRenamed()
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_SymbolicDBNameAddedDeletedRenamed();
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_SymbolicDBNameAddedDeletedRenamed();
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_SymbolicDBNameAddedDeletedRenamed();
}
void CSyncEngine::SE_ModulesCountChanged(TModulesCountChanged nChanged)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_ModulesCountChanged(nChanged);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_ModulesCountChanged(nChanged);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_ModulesCountChanged(nChanged);
}
void CSyncEngine::SE_ModuleNameChangedER(const xtstring& sOldName,
                                         const xtstring& sNewName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_ModuleNameChangedER(sOldName, sNewName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_ModuleNameChangedER(sOldName, sNewName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_ModuleNameChangedER(sOldName, sNewName);
}
void CSyncEngine::SE_ModuleNameChangedIFS(const xtstring& sOldName,
                                          const xtstring& sNewName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_ModuleNameChangedIFS(sOldName, sNewName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_ModuleNameChangedIFS(sOldName, sNewName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_ModuleNameChangedIFS(sOldName, sNewName);
}
void CSyncEngine::SE_ModuleNameChangedStone(const xtstring& sOldName,
                                            const xtstring& sNewName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_ModuleNameChangedStone(sOldName, sNewName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_ModuleNameChangedStone(sOldName, sNewName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_ModuleNameChangedStone(sOldName, sNewName);
}
void CSyncEngine::SE_ModuleNameChangedTemplate(const xtstring& sOldName,
                                               const xtstring& sNewName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_ModuleNameChangedTemplate(sOldName, sNewName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_ModuleNameChangedTemplate(sOldName, sNewName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_ModuleNameChangedTemplate(sOldName, sNewName);
}
void CSyncEngine::SE_ModuleNameChangedPython(const xtstring& sOldName,
                                             const xtstring& sNewName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_ModuleNameChangedPython(sOldName, sNewName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_ModuleNameChangedPython(sOldName, sNewName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_ModuleNameChangedPython(sOldName, sNewName);
}
void CSyncEngine::SE_CountOfTablesInERModelChanged(const xtstring& sERModelName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_CountOfTablesInERModelChanged(sERModelName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_CountOfTablesInERModelChanged(sERModelName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_CountOfTablesInERModelChanged(sERModelName);
}
void CSyncEngine::SE_CountOfFieldsInERTableChanged(const xtstring& sERModelName,
                                                   const xtstring& sERTable)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_CountOfFieldsInERTableChanged(sERModelName, sERTable);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_CountOfFieldsInERTableChanged(sERModelName, sERTable);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_CountOfFieldsInERTableChanged(sERModelName, sERTable);
}
void CSyncEngine::SE_CountOfParamsInERTableChanged(const xtstring& sERModelName,
                                                   const xtstring& sERTable)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_CountOfParamsInERTableChanged(sERModelName, sERTable);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_CountOfParamsInERTableChanged(sERModelName, sERTable);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_CountOfParamsInERTableChanged(sERModelName, sERTable);
}
void CSyncEngine::SE_ERTableNameChanged(const xtstring& sERModelName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_ERTableNameChanged(sERModelName, sOldName, sNewName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_ERTableNameChanged(sERModelName, sOldName, sNewName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_ERTableNameChanged(sERModelName, sOldName, sNewName);
}
void CSyncEngine::SE_ERFieldNameChanged(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_ERFieldNameChanged(sERModelName, sTableName, sOldName, sNewName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_ERFieldNameChanged(sERModelName, sTableName, sOldName, sNewName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_ERFieldNameChanged(sERModelName, sTableName, sOldName, sNewName);
}
void CSyncEngine::SE_IFSEntityNameChanged(const xtstring& sIFSModelName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName)
{
  for (CERGroupsIterator it = m_cERGroups.begin(); it != m_cERGroups.end(); it++)
    (*it)->SE_IFSEntityNameChanged(sIFSModelName, sOldName, sNewName);
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
    (*it)->SE_IFSEntityNameChanged(sIFSModelName, sOldName, sNewName);
  for (CWPGroupsIterator it = m_cWPGroups.begin(); it != m_cWPGroups.end(); it++)
    (*it)->SE_IFSEntityNameChanged(sIFSModelName, sOldName, sNewName);
}

















xtstring CSyncEngine::SE_Intern_GetERFromIFS(const xtstring& sIFSModulName)
{
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
  {
    if ((*it)->IsDefaultModuleGroup())
      continue;
    if ((*it)->GetModuleSyncGroupName() == sIFSModulName)
      return ((*it)->SE_MF_GetERModelName());
  }
  return _XT("");
}

xtstring CSyncEngine::SE_Intern_GetERTableFromEntity(const xtstring& sIFSModulName,
                                                     const xtstring& sEntityName)
{
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
  {
    if ((*it)->IsDefaultModuleGroup())
      continue;
    if ((*it)->GetModuleSyncGroupName() == sIFSModulName)
    {
      CIFSEntityInd* pEntity = (CIFSEntityInd*)(*it)->SE_MF_GetIFSEntityInd(sEntityName);
      if (pEntity)
        return pEntity->GetERTable_FromERTableOrGlobalTable();
      return _XT("");
    }
  }
  return _XT("");
}
void CSyncEngine::SE_Intern_GetGlobalTableNames(const xtstring& sIFSModulName,
                                                CxtstringVector& arrNames)
{
  for (CIFSGroupsIterator it = m_cIFSGroups.begin(); it != m_cIFSGroups.end(); it++)
  {
    if ((*it)->IsDefaultModuleGroup())
      continue;
    if ((*it)->GetModuleSyncGroupName() == sIFSModulName)
    {
      (*it)->SE_MF_GetGlobalTableNames(arrNames);
      return;
    }
  }
}
