// MPModelIFSInd.cpp: implementation of the CMPModelIFSInd classes.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <time.h>
#include <string.h>

#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"

#include "ModelDef.h"
#include "ModelInd.h"
#include "MPModelIFSInd.h"

#include "SSyncEng/SyncGroups.h"













void CMPModelIFSInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er model name
  if (GetERModelName() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       /*CSyncResponse_ER::*/GetTypeOfResponseObject(),
                                                       /*CSyncResponse_ER::*/GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFS_ERModelName,
                                        1,
                                        GetERModelName());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelIFSInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == /*CSyncResponse_ER::*/GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != /*CSyncResponse_ER::*/GetSyncResponseObject())
    return;

  // er model name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERModelName(sNewName);
    ROC_PropertyChanged_ER();
  }
}
void CMPModelIFSInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CMPModelIFSInd::ROC_DeleteERModel(const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er model name
  if (GetERModelName() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       /*CSyncResponse_ER::*/GetTypeOfResponseObject(),
                                                       /*CSyncResponse_ER::*/GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFS_ERModelName,
                                        1,
                                        GetERModelName());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelIFSInd::ROC_DeleteERModel(const xtstring& sName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == /*CSyncResponse_ER::*/GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != /*CSyncResponse_ER::*/GetSyncResponseObject())
    return;

  // er model name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERModelName(_XT(""));
    ROC_PropertyChanged_ER();
  }
}
void CMPModelIFSInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // param table
  if (GetERModelName() == sERModelName && GetParamTableName() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       /*CSyncResponse_ER::*/GetTypeOfResponseObject(),
                                                       /*CSyncResponse_ER::*/GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFS_ParamTable,
                                        1,
                                        GetParamTableName());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelIFSInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == /*CSyncResponse_ER::*/GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != /*CSyncResponse_ER::*/GetSyncResponseObject())
    return;

  // param table
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetParamTableName(sNewName);
    ROC_PropertyChanged_ER();
  }
}
void CMPModelIFSInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CMPModelIFSInd::ROC_DeleteTable(const xtstring& sERModelName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // param table
  if (GetERModelName() == sERModelName && GetParamTableName() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       /*CSyncResponse_ER::*/GetTypeOfResponseObject(),
                                                       /*CSyncResponse_ER::*/GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFS_ParamTable,
                                        1,
                                        GetParamTableName());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelIFSInd::ROC_DeleteTable(const xtstring& sERModelName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == /*CSyncResponse_ER::*/GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != /*CSyncResponse_ER::*/GetSyncResponseObject())
    return;

  // param table
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetParamTableName(_XT(""));
    ROC_PropertyChanged_ER();
  }
}
void CMPModelIFSInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CMPModelIFSInd::ROC_DeleteField(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_DeleteField(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CMPModelIFSInd::ROC_DeleteParam(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_DeleteParam(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_ParamAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CERModelParamInd* pParam,
                                    CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_ParamAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CERModelParamInd* pParam,
                                    CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_ParamChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_ParamChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                    const xtstring& sTableName,
                                                    const xtstring& sParentTableName,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CMPModelIFSInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_RelationAdded(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CERModelRelationInd* pRelation,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_RelationAdded(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CERModelRelationInd* pRelation,
                                       CUserOfObject* pObject)
{
}
void CMPModelIFSInd::ROC_RelationChanged(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelIFSInd::ROC_RelationChanged(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObject* pObject)
{
}
