#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../SModelInd/ModelInd.h"
#include "../SModelInd/ERModelInd.h"
#include "../SModelInd/IFSEntityInd.h"
#include "SyncGroups.h"
#include "SyncEngine.h"





////////////////////////////////////////////////////////////////////////////////
// CSyncGroup
////////////////////////////////////////////////////////////////////////////////
CSyncGroup::CSyncGroup(const xtstring& sModuleSyncGroupName,
                       TProjectFileType nModuleSyncGroupType)
{
  m_sModuleSyncGroupName = sModuleSyncGroupName;
  m_nModuleSyncGroupType = nModuleSyncGroupType;
  m_bAtLeastOneObjectInGroupChanged = false;
}

CSyncGroup::~CSyncGroup()
{
}

////////////////////////////////////////////////////////////////////////////////
// CSyncERNotifiersGroup
////////////////////////////////////////////////////////////////////////////////
CSyncERNotifiersGroup::CSyncERNotifiersGroup(const xtstring& sModuleSyncGroupName,
                                             TProjectFileType nModuleSyncGroupType)
                      :CSyncGroup(sModuleSyncGroupName, nModuleSyncGroupType)
{
  if (m_pSyncEngine)
    m_pSyncEngine->AddERGroup(this);
}
CSyncERNotifiersGroup::~CSyncERNotifiersGroup()
{
  assert(m_cERNotifiers.size() == 0);
#ifdef _DEBUG
  size_t n = m_cERNotifiers.size();
  n = n; // unused variable

  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    CSyncNotifier_ER::TTypeOfERNotifier nType = (*cElement)->GetTypeOfNotifier();
    nType = nType; // unused variable
    cElement++;
  }
#endif // _DEBUG

  if (m_pSyncEngine)
    m_pSyncEngine->RemoveERGroup(this);
}
bool CSyncERNotifiersGroup::AddNotifier(CSyncNotifier_ER* pNotifier)
{
  size_t nCount = m_cERNotifiers.size();
  m_cERNotifiers.push_back(pNotifier);
  return (m_cERNotifiers.size() != nCount);
}
bool CSyncERNotifiersGroup::RemoveNotifier(CSyncNotifier_ER* pNotifier)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier == *cElement)
    {
      m_cERNotifiers.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
bool CSyncERNotifiersGroup::SE_ChangeERModelName(const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_ChangeERModelName(sOldName, sNewName, bCancelAllowed);
}
bool CSyncERNotifiersGroup::SE_ChangeTableName(const xtstring& sERModelName,
                                               const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  bool bRet = m_pSyncEngine->SE_ChangeTableName(sERModelName, sOldName, sNewName, bCancelAllowed);

  if (bRet)
  {
    CERNotifiersIterator cElement;
    cElement = m_cERNotifiers.begin();
    while (cElement != m_cERNotifiers.end())
    {
      (*cElement)->SE_OnTableNameChanged(sOldName, sNewName);
      cElement++;
    }
  }
  return bRet;
}
bool CSyncERNotifiersGroup::SE_ChangeFieldName(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  bool bRet = m_pSyncEngine->SE_ChangeFieldName(sERModelName, sTableName, sOldName, sNewName, bCancelAllowed);

  if (bRet)
  {
    CERNotifiersIterator cElement;
    cElement = m_cERNotifiers.begin();
    while (cElement != m_cERNotifiers.end())
    {
      (*cElement)->SE_OnFieldNameChanged(sTableName, sOldName, sNewName);
      cElement++;
    }
  }
  return bRet;
}
bool CSyncERNotifiersGroup::SE_ChangeParamName(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  bool bRet = m_pSyncEngine->SE_ChangeParamName(sERModelName, sTableName, sOldName, sNewName, bCancelAllowed);

  if (bRet)
  {
    CERNotifiersIterator cElement;
    cElement = m_cERNotifiers.begin();
    while (cElement != m_cERNotifiers.end())
    {
      (*cElement)->SE_OnParamNameChanged(sTableName, sOldName, sNewName);
      cElement++;
    }
  }
  return bRet;
}
bool CSyncERNotifiersGroup::SE_ChangeRelationName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sParentTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  bool bCancelAllowed)
{
  {
    CERNotifiersIterator cElement;
    cElement = m_cERNotifiers.begin();
    while (cElement != m_cERNotifiers.end())
    {
      if (!(*cElement)->SE_OnRelationNameWillBeChanged(sTableName, sParentTableName, sOldName, sNewName))
        return false;
      cElement++;
    }
  }
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  bool bRet = m_pSyncEngine->SE_ChangeRelationName(sERModelName, sTableName, sParentTableName, sOldName, sNewName, bCancelAllowed);

  if (bRet)
  {
    CERNotifiersIterator cElement;
    cElement = m_cERNotifiers.begin();
    while (cElement != m_cERNotifiers.end())
    {
      (*cElement)->SE_OnRelationNameChanged(sTableName, sParentTableName, sOldName, sNewName);
      cElement++;
    }
  }
  return bRet;
}
bool CSyncERNotifiersGroup::SE_CheckReferencesForERModel(const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForERModel(sName, cUsers);
}
bool CSyncERNotifiersGroup::SE_CheckReferencesForTable(const xtstring& sERModelName,
                                                       const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForTable(sERModelName, sName, cUsers);
}
bool CSyncERNotifiersGroup::SE_CheckReferencesForField(const xtstring& sERModelName,
                                                       const xtstring& sTableName,
                                                       const xtstring& sFieldName,
                                                       CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForField(sERModelName, sTableName, sFieldName, cUsers);
}
bool CSyncERNotifiersGroup::SE_CheckReferencesForParam(const xtstring& sERModelName,
                                                       const xtstring& sTableName,
                                                       const xtstring& sParamName,
                                                       CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForParam(sERModelName, sTableName, sParamName, cUsers);
}
bool CSyncERNotifiersGroup::SE_CheckReferencesForRelation(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sParentTableName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
bool CSyncERNotifiersGroup::SE_DeleteERModel(const xtstring& sName,
                                             size_t& nCountOfReferences,
                                             bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteERModel(sName, nCountOfReferences, bCancelAllowed);
}
bool CSyncERNotifiersGroup::SE_DeleteTable(const xtstring& sERModelName,
                                           const xtstring& sName,
                                           size_t& nCountOfReferences,
                                           bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  bool bRet = m_pSyncEngine->SE_DeleteTable(sERModelName, sName, nCountOfReferences, bCancelAllowed);

  if (bRet)
  {
    CERNotifiersIterator cElement;
    cElement = m_cERNotifiers.begin();
    while (cElement != m_cERNotifiers.end())
    {
      (*cElement)->SE_OnTableDelete(sName);
      cElement++;
    }
  }
  return bRet;
}
bool CSyncERNotifiersGroup::SE_DeleteField(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sFieldName,
                                           size_t& nCountOfReferences,
                                           bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteField(sERModelName, sTableName, sFieldName, nCountOfReferences, bCancelAllowed);
}
bool CSyncERNotifiersGroup::SE_DeleteParam(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParamName,
                                           size_t& nCountOfReferences,
                                           bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteParam(sERModelName, sTableName, sParamName, nCountOfReferences, bCancelAllowed);
}
bool CSyncERNotifiersGroup::SE_DeleteRelation(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sName,
                                              bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, bCancelAllowed);
}
bool CSyncERNotifiersGroup::SE_ParamAdded(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParamName,
                                          CERModelParamInd* pParam,
                                          bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_ParamAdded(sERModelName, sTableName, sParamName, pParam, bCancelAllowed);
}
bool CSyncERNotifiersGroup::SE_ParamChanged(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParamName,
                                            CERModelParamInd* pParam,
                                            bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_ParamChanged(sERModelName, sTableName, sParamName, pParam, bCancelAllowed);
}
bool CSyncERNotifiersGroup::SE_RelationAdded(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sName,
                                             CERModelRelationInd* pRelation,
                                             bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_RelationAdded(sERModelName, sTableName, sParentTableName, sName, pRelation, bCancelAllowed);
}
bool CSyncERNotifiersGroup::SE_RelationChanged(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sParentTableName,
                                               const xtstring& sName,
                                               CERModelRelationInd* pRelation,
                                               bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_RelationChanged(sERModelName, sTableName, sParentTableName, sName, pRelation, bCancelAllowed);
}
xtstring CSyncERNotifiersGroup::SE_MF_GetModuleName()
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_ER::tERNotifier_MainProperty)
      return (*cElement)->SE_GetModuleName();
    cElement++;
  }
  return _XT("");
}
bool CSyncERNotifiersGroup::SE_MF_GetShowRelationNamesER()
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_ER::tERNotifier_MainProperty)
      return (*cElement)->SE_GetShowRelationNamesER();
    cElement++;
  }
  return true;
}
const CERModelTableInd* CSyncERNotifiersGroup::SE_MF_GetERModelTableInd(const xtstring& sName)
{
  CERNotifiersConstIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_ER::tERNotifier_Table)
    {
      const CERModelTableInd* pTable = (((CERModelTableInd*)(*cElement))->SE_GetERModelTableInd());
      if (pTable->GetName() == sName)
        return pTable;
    }
    cElement++;
  }
  return 0;
}
xtstring CSyncERNotifiersGroup::SE_MF_GetDBDefinitionName_Production()
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_ER::tERNotifier_MainProperty)
      return (*cElement)->SE_GetDBDefinitionName_Production();
    cElement++;
  }
  return _XT("");
}
xtstring CSyncERNotifiersGroup::SE_MF_GetDBDefinitionName_Test()
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_ER::tERNotifier_MainProperty)
      return (*cElement)->SE_GetDBDefinitionName_Test();
    cElement++;
  }
  return _XT("");
}
void CSyncERNotifiersGroup::SE_SyncFontChanged(CSyncNotifier_ER* pNotifier, const xtstring& sFontName, int nFontHeight)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncFontChanged(sFontName, nFontHeight);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncColorChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorChanged(nColor);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncColorBackChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorBackChanged(nColor);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncColorHeaderChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorHeaderChanged(nColor);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncColorHeaderSelectedChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorHeaderSelectedChanged(nColor);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncColorHeaderTextChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorHeaderTextChanged(nColor);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncColorHeaderSelectedTextChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorHeaderSelectedTextChanged(nColor);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncColorSelectedItemChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorSelectedItemChanged(nColor);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncMoveableChanged(CSyncNotifier_ER* pNotifier, bool bVal)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncMoveableChanged(bVal);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncSelectableChanged(CSyncNotifier_ER* pNotifier, bool bVal)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncSelectableChanged(bVal);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncVisibleChanged(CSyncNotifier_ER* pNotifier, bool bVal)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncVisibleChanged(bVal);
    cElement++;
  }
}
void CSyncERNotifiersGroup::SE_SyncSizeableChanged(CSyncNotifier_ER* pNotifier, bool bVal)
{
  CERNotifiersIterator cElement;
  cElement = m_cERNotifiers.begin();
  while (cElement != m_cERNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncSizeableChanged(bVal);
    cElement++;
  }
}







































////////////////////////////////////////////////////////////////////////////////
// CSyncIFSNotifiersGroup
////////////////////////////////////////////////////////////////////////////////
CSyncIFSNotifiersGroup::CSyncIFSNotifiersGroup(const xtstring& sModuleSyncGroupName,
                                               TProjectFileType nModuleSyncGroupType)
                       :CSyncGroup(sModuleSyncGroupName, nModuleSyncGroupType)
{
  if (m_pSyncEngine)
    m_pSyncEngine->AddIFSGroup(this);
}
CSyncIFSNotifiersGroup::~CSyncIFSNotifiersGroup()
{
  assert(m_cIFSNotifiers.size() == 0);
#ifdef _DEBUG
  size_t n = m_cIFSNotifiers.size();
  n = n; // unused variable

  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    CSyncNotifier_IFS::TTypeOfIFSNotifier nType = (*cElement)->GetTypeOfNotifier();
    nType = nType; // unused variable
    cElement++;
  }
#endif // _DEBUG


  if (m_pSyncEngine)
    m_pSyncEngine->RemoveIFSGroup(this);
}
bool CSyncIFSNotifiersGroup::AddNotifier(CSyncNotifier_IFS* pNotifier)
{
  size_t nCount = m_cIFSNotifiers.size();
  m_cIFSNotifiers.push_back(pNotifier);
  return (m_cIFSNotifiers.size() != nCount);
}
bool CSyncIFSNotifiersGroup::RemoveNotifier(CSyncNotifier_IFS* pNotifier)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier == *cElement)
    {
      m_cIFSNotifiers.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
bool CSyncIFSNotifiersGroup::SE_ChangeIFSName(const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_ChangeIFSName(sOldName, sNewName, bCancelAllowed);
}
bool CSyncIFSNotifiersGroup::SE_ChangeEntityName(const xtstring& sIFS,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  bool bRet = m_pSyncEngine->SE_ChangeEntityName(sIFS, sOldName, sNewName, bCancelAllowed);

  if (bRet)
  {
    CIFSNotifiersIterator cElement;
    cElement = m_cIFSNotifiers.begin();
    while (cElement != m_cIFSNotifiers.end())
    {
      (*cElement)->SE_OnEntityNameChanged(sOldName, sNewName);
      cElement++;
    }
  }
  return bRet;
}
bool CSyncIFSNotifiersGroup::SE_ChangeVariableName(const xtstring& sIFS,
                                                   const xtstring& sEntityName,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_ChangeVariableName(sIFS, sEntityName, sOldName, sNewName, bCancelAllowed);
}
bool CSyncIFSNotifiersGroup::SE_ChangeGlobalERTableName(const xtstring& sIFS,
                                                        const xtstring sOldName,
                                                        const xtstring& sNewName,
                                                        bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_ChangeGlobalERTableName(sIFS, sOldName, sNewName, bCancelAllowed);
}
bool CSyncIFSNotifiersGroup::SE_CheckReferencesForIFS(const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForIFS(sName, cUsers);
}
bool CSyncIFSNotifiersGroup::SE_CheckReferencesForEntity(const xtstring& sIFS,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForEntity(sIFS, sName, cUsers);
}
bool CSyncIFSNotifiersGroup::SE_CheckReferencesForVariable(const xtstring& sIFS,
                                                           const xtstring& sEntityName,
                                                           const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForVariable(sIFS, sEntityName, sName, cUsers);
}
bool CSyncIFSNotifiersGroup::SE_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                                const xtstring& sName,
                                                                CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForGlobalERTable(sIFS, sName, cUsers);
}
bool CSyncIFSNotifiersGroup::SE_DeleteIFS(const xtstring& sName,
                                          size_t& nCountOfReferences,
                                          bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteIFS(sName, nCountOfReferences, bCancelAllowed);
}
bool CSyncIFSNotifiersGroup::SE_DeleteEntity(const xtstring& sIFS,
                                             const xtstring& sName,
                                             size_t& nCountOfReferences,
                                             bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteEntity(sIFS, sName, nCountOfReferences, bCancelAllowed);
}
bool CSyncIFSNotifiersGroup::SE_DeleteVariable(const xtstring& sIFS,
                                               const xtstring& sEntityName,
                                               const xtstring& sName,
                                               size_t& nCountOfReferences,
                                               bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteVariable(sIFS, sEntityName, sName, nCountOfReferences, bCancelAllowed);
}
bool CSyncIFSNotifiersGroup::SE_DeleteGlobalERTable(const xtstring& sIFS,
                                                    const xtstring& sName,
                                                    size_t& nCountOfReferences,
                                                    bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteGlobalERTable(sIFS, sName, nCountOfReferences, bCancelAllowed);
}
void CSyncIFSNotifiersGroup::SE_RelationNameChanged()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    (*cElement)->SE_OnRelationNameChanged();
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_RelationNameDeleted()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    (*cElement)->SE_OnRelationNameDeleted();
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_RelationNameAdded()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    (*cElement)->SE_OnRelationNameAdded();
    cElement++;
  }
}
xtstring CSyncIFSNotifiersGroup::SE_MF_GetModuleName()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_IFS::tIFSNotifier_MainProperty)
      return (*cElement)->SE_GetModuleName();
    cElement++;
  }
  return _XT("");
}
bool CSyncIFSNotifiersGroup::SE_MF_GetShowRelationNames()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_IFS::tIFSNotifier_MainProperty)
      return (*cElement)->SE_GetShowRelationNames();
    cElement++;
  }
  return true;
}
bool CSyncIFSNotifiersGroup::SE_MF_GetShowHitLines()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_IFS::tIFSNotifier_MainProperty)
      return (*cElement)->SE_GetShowHitLines();
    cElement++;
  }
  return true;
}
const CIFSEntityInd* CSyncIFSNotifiersGroup::SE_MF_GetIFSEntityInd(const xtstring& sName)
{
  CIFSNotifiersConstIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_IFS::tIFSNotifier_Entity)
    {
      const CIFSEntityInd* pTable = (((CIFSEntityInd*)(*cElement))->SE_GetIFSEntityInd());
      if (pTable && pTable->GetName() == sName)
        return pTable;
    }
    cElement++;
  }
  return 0;
}
xtstring CSyncIFSNotifiersGroup::SE_MF_GetERModelName()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_IFS::tIFSNotifier_MainProperty)
      return (*cElement)->SE_GetERModelName();
    cElement++;
  }
  return _XT("");
}
void CSyncIFSNotifiersGroup::SE_MF_GetGlobalTableNames(CxtstringVector& arrNames)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    const CIFSEntityInd* pTable = (((CIFSEntityInd*)(*cElement))->SE_GetIFSEntityInd());
    if (pTable && pTable->GetMainEntity())
    {
      (*cElement)->SE_GetGlobalTableNames(arrNames);
      return;
    }
    cElement++;
  }
}
xtstring CSyncIFSNotifiersGroup::SE_MF_GetERTableFromGlobalTable(const xtstring& sGlobalTableName)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    const CIFSEntityInd* pTable = (((CIFSEntityInd*)(*cElement))->SE_GetIFSEntityInd());
    if (pTable && pTable->GetMainEntity())
      return (*cElement)->SE_GetERTableFromGlobalTable(sGlobalTableName);
    cElement++;
  }
  return _XT("");
}
void CSyncIFSNotifiersGroup::SE_MF_ParamTableChanged()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    (*cElement)->SE_OnParamTableChanged();
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_MF_ERModelChanged(const xtstring& sNewERModelName)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    (*cElement)->SE_OnERModelChanged(sNewERModelName);
    cElement++;
  }
}
xtstring CSyncIFSNotifiersGroup::SE_MF_GetParamTable()
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_IFS::tIFSNotifier_MainProperty)
      return (*cElement)->SE_GetParamTableName();
    cElement++;
  }
  return _XT("");
}
bool CSyncIFSNotifiersGroup::SE_MF_SetParamTable(const xtstring& sParamTable)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_IFS::tIFSNotifier_MainProperty)
    {
      (*cElement)->SE_SetParamTableName(sParamTable);
      break;
    }
    cElement++;
  }
  return true;
}
void CSyncIFSNotifiersGroup::SE_SyncFontChanged(CSyncNotifier_IFS* pNotifier, const xtstring& sFontName, int nFontHeight)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncFontChanged(sFontName, nFontHeight);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncColorChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorChanged(nColor);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncColorBackChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorBackChanged(nColor);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncColorHeaderChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorHeaderChanged(nColor);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncColorHeaderSelectedChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorHeaderSelectedChanged(nColor);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncColorHeaderTextChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorHeaderTextChanged(nColor);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncColorHeaderSelectedTextChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorHeaderSelectedTextChanged(nColor);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncColorSelectedItemChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncColorSelectedItemChanged(nColor);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncMoveableChanged(CSyncNotifier_IFS* pNotifier, bool bVal)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncMoveableChanged(bVal);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncSelectableChanged(CSyncNotifier_IFS* pNotifier, bool bVal)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncSelectableChanged(bVal);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncVisibleChanged(CSyncNotifier_IFS* pNotifier, bool bVal)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncVisibleChanged(bVal);
    cElement++;
  }
}
void CSyncIFSNotifiersGroup::SE_SyncSizeableChanged(CSyncNotifier_IFS* pNotifier, bool bVal)
{
  CIFSNotifiersIterator cElement;
  cElement = m_cIFSNotifiers.begin();
  while (cElement != m_cIFSNotifiers.end())
  {
    if (pNotifier != *cElement)
      (*cElement)->SE_OnSyncSizeableChanged(bVal);
    cElement++;
  }
}
































////////////////////////////////////////////////////////////////////////////////
// CSyncWPNotifiersGroup
////////////////////////////////////////////////////////////////////////////////
CSyncWPNotifiersGroup::CSyncWPNotifiersGroup(const xtstring& sModuleSyncGroupName,
                                             TProjectFileType nModuleSyncGroupType)
                      :CSyncGroup(sModuleSyncGroupName, nModuleSyncGroupType)
{
  if (m_pSyncEngine)
    m_pSyncEngine->AddWPGroup(this);
}
CSyncWPNotifiersGroup::~CSyncWPNotifiersGroup()
{
  assert(m_cWPNotifiers.size() == 0);
#ifdef _DEBUG
  size_t n = m_cWPNotifiers.size();
  n = n; // unused variable

  CWPNotifiersIterator cElement;
  cElement = m_cWPNotifiers.begin();
  while (cElement != m_cWPNotifiers.end())
  {
    CSyncNotifier_WP::TTypeOfWPNotifier nType = (*cElement)->GetTypeOfNotifier();
    nType = nType; // unused variable
    cElement++;
  }
#endif // _DEBUG

  if (m_pSyncEngine)
    m_pSyncEngine->RemoveWPGroup(this);
}
bool CSyncWPNotifiersGroup::AddNotifier(CSyncNotifier_WP* pNotifier)
{
  size_t nCount = m_cWPNotifiers.size();
  m_cWPNotifiers.push_back(pNotifier);
  return (m_cWPNotifiers.size() != nCount);
}
bool CSyncWPNotifiersGroup::RemoveNotifier(CSyncNotifier_WP* pNotifier)
{
  CWPNotifiersIterator cElement;
  cElement = m_cWPNotifiers.begin();
  while (cElement != m_cWPNotifiers.end())
  {
    if (pNotifier == *cElement)
    {
      m_cWPNotifiers.erase(cElement);
      return true;
    }
    cElement++;
  }
  return false;
}
xtstring CSyncWPNotifiersGroup::SE_MF_GetModuleName()
{
  CWPNotifiersIterator cElement;
  cElement = m_cWPNotifiers.begin();
  while (cElement != m_cWPNotifiers.end())
  {
    if ((*cElement)->GetTypeOfNotifier() == CSyncNotifier_WP::tWPNotifier_TemplateMainProperty
        || (*cElement)->GetTypeOfNotifier() == CSyncNotifier_WP::tWPNotifier_StoneMainProperty)
      return (*cElement)->SE_GetModuleName();
    cElement++;
  }
  return _XT("");
}
bool CSyncWPNotifiersGroup::SE_ChangeWPName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_ChangeWPName(sOldName, sNewName, bCancelAllowed);
}
bool CSyncWPNotifiersGroup::SE_CheckReferencesForWP(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForWP(sName, cUsers);
}
bool CSyncWPNotifiersGroup::SE_DeleteWP(const xtstring& sName,
                                        size_t& nCountOfReferences,
                                        bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteWP(sName, nCountOfReferences, bCancelAllowed);
}
bool CSyncWPNotifiersGroup::SE_ChangeWPConstantName(const xtstring& sWPName,
                                                    const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_ChangeWPConstantName(sWPName, sOldName, sNewName, bCancelAllowed);
}
bool CSyncWPNotifiersGroup::SE_CheckReferencesForWPConstant(const xtstring& sWPName,
                                                            const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_CheckReferencesForWPConstant(sWPName, sName, cUsers);
}
bool CSyncWPNotifiersGroup::SE_DeleteWPConstant(const xtstring& sWPName,
                                                const xtstring& sName,
                                                size_t& nCountOfReferences,
                                                bool bCancelAllowed)
{
  assert(m_pSyncEngine);
  if (!m_pSyncEngine)
    return false;
  return m_pSyncEngine->SE_DeleteWPConstant(sWPName, sName, nCountOfReferences, bCancelAllowed);
}

