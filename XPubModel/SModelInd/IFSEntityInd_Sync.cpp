// IFSEntityInd.cpp: implementation of the CIFSEntityXXXInd class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"

#include "ModelDef.h"
#include "ModelInd.h"
#include "IFSEntityInd.h"

#include "SSyncEng/SyncGroups.h"



#define IDENTIFY_CONNECTION_BASE          200000
#define IDENTIFY_CONNECTION_END           299999
#define IDENTIFY_CONNECTIONTOCHILD_BASE   400000
#define IDENTIFY_CONNECTIONTOCHILD_END    499999
#define IDENTIFY_RELATION_STEP            1000


/////////////////////////////////////////////////////////////////////////////
// CIFSEntityGlobalERTableInd
/////////////////////////////////////////////////////////////////////////////
void CIFSEntityGlobalERTableInd::ROC_PropertyChanged_ER(){if (GetOwnerEntity())GetOwnerEntity()->ROC_PropertyChanged_ER();}
void CIFSEntityGlobalERTableInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                               CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_DeleteERModel(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_DeleteERModel(const xtstring& sName,
                                                   CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  // er table
  if (GetERTableName() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_GlobalERTable,
                                        1,
                                        GetERTableName());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityGlobalERTableInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // er table
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERTableName(sNewName);
    ROC_PropertyChanged_ER();
  }
}
void CIFSEntityGlobalERTableInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CIFSEntityGlobalERTableInd::ROC_DeleteTable(const xtstring& sERModelName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  // er table
  if (GetERTableName() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_GlobalERTable,
                                        1,
                                        GetERTableName());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityGlobalERTableInd::ROC_DeleteTable(const xtstring& sERModelName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // er table
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERTableName(_XT(""));
    ROC_PropertyChanged_ER();
  }
}
void CIFSEntityGlobalERTableInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                             const xtstring& sTableName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_DeleteField(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_DeleteField(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                             const xtstring& sTableName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_DeleteParam(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_DeleteParam(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_ParamAdded(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sName,
                                                CERModelParamInd* pParam,
                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_ParamAdded(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sName,
                                                CERModelParamInd* pParam,
                                                CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_ParamChanged(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CERModelParamInd* pParam,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_ParamChanged(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CERModelParamInd* pParam,
                                                  CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                        const xtstring& sTableName,
                                                        const xtstring& sParentTableName,
                                                        const xtstring& sOldName,
                                                        const xtstring& sNewName,
                                                        CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                        const xtstring& sTableName,
                                                        const xtstring& sParentTableName,
                                                        const xtstring& sOldName,
                                                        const xtstring& sNewName,
                                                        CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                                const xtstring& sTableName,
                                                                const xtstring& sParentTableName,
                                                                const xtstring& sName,
                                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                                    const xtstring& sTableName,
                                                    const xtstring& sParentTableName,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                                    const xtstring& sTableName,
                                                    const xtstring& sParentTableName,
                                                    const xtstring& sName,
                                                    CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_RelationAdded(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sParentTableName,
                                                   const xtstring& sName,
                                                   CERModelRelationInd* pRelation,
                                                   CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_RelationAdded(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sParentTableName,
                                                   const xtstring& sName,
                                                   CERModelRelationInd* pRelation,
                                                   CUserOfObject* pObject)
{
}
void CIFSEntityGlobalERTableInd::ROC_RelationChanged(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sName,
                                                     CERModelRelationInd* pRelation,
                                                     CUserOfObjectArray& cUsers)
{
}
void CIFSEntityGlobalERTableInd::ROC_RelationChanged(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sName,
                                                     CERModelRelationInd* pRelation,
                                                     CUserOfObject* pObject)
{
}



























/////////////////////////////////////////////////////////////////////////////
// CIFSEntityOneConditionVariation
/////////////////////////////////////////////////////////////////////////////
void CIFSEntityOneConditionVariation::ROC_PropertyChanged_Proj()
{
  assert(m_pParent);
  if (!m_pParent)
    return;
  assert(m_pParent->GetOwnerEntity());
  if (!m_pParent->GetOwnerEntity())
    return;
  m_pParent->GetOwnerEntity()->ROC_PropertyChanged_Proj();
}
void CIFSEntityOneConditionVariation::ROC_PropertyChanged_ER()
{
  assert(m_pParent);
  if (!m_pParent)
    return;
  assert(m_pParent->GetOwnerEntity());
  if (!m_pParent->GetOwnerEntity())
    return;
  m_pParent->GetOwnerEntity()->ROC_PropertyChanged_ER();
}
void CIFSEntityOneConditionVariation::ROC_PropertyChanged_IFS()
{
  assert(m_pParent);
  if (!m_pParent)
    return;
  assert(m_pParent->GetOwnerEntity());
  if (!m_pParent->GetOwnerEntity())
    return;
  m_pParent->GetOwnerEntity()->ROC_PropertyChanged_IFS();
}
void CIFSEntityOneConditionVariation::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                             const xtstring& sNewName,
                                                             CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (ProjConstantExistInExpression(sOldName,
                                    GetCondition().GetExprText(), GetCondition().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       m_pParent->GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityOneCondition_Condition,
                                        1,
                                        GetCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityOneConditionVariation::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                             const xtstring& sNewName,
                                                             CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetCondition().GetExprText();
    xtstring sValues = GetCondition().GetExprValues();
    xtstring sOffsets = GetCondition().GetExprOffsets();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetCondition().SetExprText(sExpression);
      GetCondition().SetExprValues(sValues);
      GetCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteProjConstant(const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (ProjConstantExistInExpression(sName,
                                    GetCondition().GetExprText(), GetCondition().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       m_pParent->GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityOneCondition_Condition,
                                        1,
                                        GetCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityOneConditionVariation::ROC_DeleteProjConstant(const xtstring& sName,
                                                             CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetCondition().GetExprText();
    xtstring sValues = GetCondition().GetExprValues();
    xtstring sOffsets = GetCondition().GetExprOffsets();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetCondition().SetExprText(sExpression);
      GetCondition().SetExprValues(sValues);
      GetCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CIFSEntityOneConditionVariation::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                               const xtstring& sNewName,
                                                               CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                               const xtstring& sNewName,
                                                               CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                               CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                               CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeERModelName(const xtstring& sOldName,
                                                            const xtstring& sNewName,
                                                            CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeERModelName(const xtstring& sOldName,
                                                            const xtstring& sNewName,
                                                            CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteERModel(const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_DeleteERModel(const xtstring& sName,
                                                        CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeTableName(const xtstring& sERModelName,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeTableName(const xtstring& sERModelName,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                                  const xtstring& sName,
                                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteTable(const xtstring& sERModelName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_DeleteTable(const xtstring& sERModelName,
                                                      const xtstring& sName,
                                                      CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetCondition().GetExprText(), GetCondition().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       m_pParent->GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityOneCondition_Condition,
                                        1,
                                        GetCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityOneConditionVariation::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetCondition().GetExprText();
    xtstring sValues = GetCondition().GetExprValues();
    xtstring sOffsets = GetCondition().GetExprOffsets();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetCondition().SetExprText(sExpression);
      GetCondition().SetExprValues(sValues);
      GetCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                                  const xtstring& sTableName,
                                                                  const xtstring& sName,
                                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteField(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetCondition().GetExprText(), GetCondition().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       m_pParent->GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityOneCondition_Condition,
                                        1,
                                        GetCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityOneConditionVariation::ROC_DeleteField(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sName,
                                                      CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetCondition().GetExprText();
    xtstring sValues = GetCondition().GetExprValues();
    xtstring sOffsets = GetCondition().GetExprOffsets();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetCondition().SetExprText(sExpression);
      GetCondition().SetExprValues(sValues);
      GetCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityOneConditionVariation::ROC_ChangeParamName(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeParamName(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                                  const xtstring& sTableName,
                                                                  const xtstring& sName,
                                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteParam(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_DeleteParam(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sName,
                                                      CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_ParamAdded(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sName,
                                                     CERModelParamInd* pParam,
                                                     CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ParamAdded(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sName,
                                                     CERModelParamInd* pParam,
                                                     CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_ParamChanged(const xtstring& sERModelName,
                                                       const xtstring& sTableName,
                                                       const xtstring& sName,
                                                       CERModelParamInd* pParam,
                                                       CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ParamChanged(const xtstring& sERModelName,
                                                       const xtstring& sTableName,
                                                       const xtstring& sName,
                                                       CERModelParamInd* pParam,
                                                       CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                             const xtstring& sTableName,
                                                             const xtstring& sParentTableName,
                                                             const xtstring& sOldName,
                                                             const xtstring& sNewName,
                                                             CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                             const xtstring& sTableName,
                                                             const xtstring& sParentTableName,
                                                             const xtstring& sOldName,
                                                             const xtstring& sNewName,
                                                             CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                                     const xtstring& sTableName,
                                                                     const xtstring& sParentTableName,
                                                                     const xtstring& sName,
                                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteRelation(const xtstring& sERModelName,
                                                         const xtstring& sTableName,
                                                         const xtstring& sParentTableName,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_DeleteRelation(const xtstring& sERModelName,
                                                         const xtstring& sTableName,
                                                         const xtstring& sParentTableName,
                                                         const xtstring& sName,
                                                         CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_RelationAdded(const xtstring& sERModelName,
                                                        const xtstring& sTableName,
                                                        const xtstring& sParentTableName,
                                                        const xtstring& sName,
                                                        CERModelRelationInd* pRelation,
                                                        CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_RelationAdded(const xtstring& sERModelName,
                                                        const xtstring& sTableName,
                                                        const xtstring& sParentTableName,
                                                        const xtstring& sName,
                                                        CERModelRelationInd* pRelation,
                                                        CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_RelationChanged(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sParentTableName,
                                                          const xtstring& sName,
                                                          CERModelRelationInd* pRelation,
                                                          CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_RelationChanged(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sParentTableName,
                                                          const xtstring& sName,
                                                          CERModelRelationInd* pRelation,
                                                          CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeIFSName(const xtstring& sOldName,
                                                        const xtstring& sNewName,
                                                        CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeIFSName(const xtstring& sOldName,
                                                        const xtstring& sNewName,
                                                        CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteIFS(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_DeleteIFS(const xtstring& sName,
                                                    CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeEntityName(const xtstring& sIFS,
                                                           const xtstring& sOldName,
                                                           const xtstring& sNewName,
                                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (EntityExistInExpression(sOldName,
                              GetCondition().GetExprText(), GetCondition().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       m_pParent->GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityOneCondition_Condition,
                                        1,
                                        GetCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityOneConditionVariation::ROC_ChangeEntityName(const xtstring& sIFS,
                                                           const xtstring& sOldName,
                                                           const xtstring& sNewName,
                                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetCondition().GetExprText();
    xtstring sValues = GetCondition().GetExprValues();
    xtstring sOffsets = GetCondition().GetExprOffsets();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetCondition().SetExprText(sExpression);
      GetCondition().SetExprValues(sValues);
      GetCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                                   const xtstring& sName,
                                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteEntity(const xtstring& sIFS,
                                                       const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (EntityExistInExpression(sName,
                              GetCondition().GetExprText(), GetCondition().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       m_pParent->GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityOneCondition_Condition,
                                        1,
                                        GetCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityOneConditionVariation::ROC_DeleteEntity(const xtstring& sIFS,
                                                       const xtstring& sName,
                                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetCondition().GetExprText();
    xtstring sValues = GetCondition().GetExprValues();
    xtstring sOffsets = GetCondition().GetExprOffsets();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetCondition().SetExprText(sExpression);
      GetCondition().SetExprValues(sValues);
      GetCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityOneConditionVariation::ROC_ChangeVariableName(const xtstring& sIFS,
                                                             const xtstring& sEntityName,
                                                             const xtstring& sOldName,
                                                             const xtstring& sNewName,
                                                             CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetCondition().GetExprText(), GetCondition().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       m_pParent->GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityOneCondition_Condition,
                                        1,
                                        GetCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityOneConditionVariation::ROC_ChangeVariableName(const xtstring& sIFS,
                                                             const xtstring& sEntityName,
                                                             const xtstring& sOldName,
                                                             const xtstring& sNewName,
                                                             CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetCondition().GetExprText();
    xtstring sValues = GetCondition().GetExprValues();
    xtstring sOffsets = GetCondition().GetExprOffsets();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetCondition().SetExprText(sExpression);
      GetCondition().SetExprValues(sValues);
      GetCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                                     const xtstring& sEntityName,
                                                                     const xtstring& sName,
                                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteVariable(const xtstring& sIFS,
                                                         const xtstring& sEntityName,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (VariableExistInExpression(sEntityName, sName,
                                GetCondition().GetExprText(), GetCondition().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       m_pParent->GetOwnerEntityName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityOneCondition_Condition,
                                        1,
                                        GetCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityOneConditionVariation::ROC_DeleteVariable(const xtstring& sIFS,
                                                         const xtstring& sEntityName,
                                                         const xtstring& sName,
                                                         CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetCondition().GetExprText();
    xtstring sValues = GetCondition().GetExprValues();
    xtstring sOffsets = GetCondition().GetExprOffsets();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetCondition().SetExprText(sExpression);
      GetCondition().SetExprValues(sValues);
      GetCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityOneConditionVariation::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                                  const xtstring& sOldName,
                                                                  const xtstring& sNewName,
                                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                                  const xtstring& sOldName,
                                                                  const xtstring& sNewName,
                                                                  CUserOfObject* pObject)
{
}
void CIFSEntityOneConditionVariation::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                                          const xtstring& sName,
                                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CIFSEntityOneConditionVariation::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                              const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
}
void CIFSEntityOneConditionVariation::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                              const xtstring& sName,
                                                              CUserOfObject* pObject)
{
}
































/////////////////////////////////////////////////////////////////////////////
// CIFSEntityAllParameters
/////////////////////////////////////////////////////////////////////////////
void CIFSEntityAllParameters::ROC_PropertyChanged_Proj()
{
  assert(GetOwnerEntity());
  if (!GetOwnerEntity())
    return;
  GetOwnerEntity()->ROC_PropertyChanged_Proj();
}
void CIFSEntityAllParameters::ROC_PropertyChanged_ER()
{
  assert(GetOwnerEntity());
  if (!GetOwnerEntity())
    return;
  GetOwnerEntity()->ROC_PropertyChanged_ER();
}
void CIFSEntityAllParameters::ROC_PropertyChanged_IFS()
{
  assert(GetOwnerEntity());
  if (!GetOwnerEntity())
    return;
  GetOwnerEntity()->ROC_PropertyChanged_IFS();
}
void CIFSEntityAllParameters::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // expression in parameters
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (!pParam)
      continue;
    if (ProjConstantExistInExpression(sOldName,
                                      pParam->m_caAssignment.GetExprText(), pParam->m_caAssignment.GetExprValues(),
                                      PositionsE, PositionsV))
    {
      xtstring sShowName = pParam->m_sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                         CSyncResponse_Proj::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_ParameterExpression,
                                          nI,
                                          pParam->m_caAssignment.GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // expression in parameters
  if (pObject->m_nIdentify < CountOfParameters())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(pObject->m_nIdentify);
    if (pParam)
    {
      xtstring sExpression = pParam->m_caAssignment.GetExprText();
      xtstring sValues = pParam->m_caAssignment.GetExprValues();
      xtstring sOffsets = pParam->m_caAssignment.GetExprOffsets();
      if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                          pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pParam->m_caAssignment.SetExprText(sExpression);
        pParam->m_caAssignment.SetExprValues(sValues);
        pParam->m_caAssignment.SetExprOffsets(sOffsets);
        ROC_PropertyChanged_Proj();
      }
    }
  }
}
void CIFSEntityAllParameters::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteProjConstant(const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // expression in parameters
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (!pParam)
      continue;
    if (ProjConstantExistInExpression(sName,
                                      pParam->m_caAssignment.GetExprText(), pParam->m_caAssignment.GetExprValues(),
                                      PositionsE, PositionsV))
    {
      xtstring sShowName = pParam->m_sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                         CSyncResponse_Proj::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_ParameterExpression,
                                          nI,
                                          pParam->m_caAssignment.GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_DeleteProjConstant(const xtstring& sName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // expression in parameters
  if (pObject->m_nIdentify < CountOfParameters())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(pObject->m_nIdentify);
    if (pParam)
    {
      xtstring sExpression = pParam->m_caAssignment.GetExprText();
      xtstring sValues = pParam->m_caAssignment.GetExprValues();
      xtstring sOffsets = pParam->m_caAssignment.GetExprOffsets();
      if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                          pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pParam->m_caAssignment.SetExprText(sExpression);
        pParam->m_caAssignment.SetExprValues(sValues);
        pParam->m_caAssignment.SetExprOffsets(sOffsets);
        ROC_PropertyChanged_Proj();
      }
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                       CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_ChangeERModelName(const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_ChangeERModelName(const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteERModel(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_DeleteERModel(const xtstring& sName,
                                                CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_ChangeTableName(const xtstring& sERModelName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_ChangeTableName(const xtstring& sERModelName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteTable(const xtstring& sERModelName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_DeleteTable(const xtstring& sERModelName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // expression in parameters
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (!pParam)
      continue;
    if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                               pParam->m_caAssignment.GetExprText(), pParam->m_caAssignment.GetExprValues(),
                               PositionsE, PositionsV))
    {
      xtstring sShowName = pParam->m_sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_ParameterExpression,
                                          nI,
                                          pParam->m_caAssignment.GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // expression in parameters
  if (pObject->m_nIdentify < CountOfParameters())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(pObject->m_nIdentify);
    if (pParam)
    {
      xtstring sExpression = pParam->m_caAssignment.GetExprText();
      xtstring sValues = pParam->m_caAssignment.GetExprValues();
      xtstring sOffsets = pParam->m_caAssignment.GetExprOffsets();
      if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                   pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pParam->m_caAssignment.SetExprText(sExpression);
        pParam->m_caAssignment.SetExprValues(sValues);
        pParam->m_caAssignment.SetExprOffsets(sOffsets);
        ROC_PropertyChanged_ER();
      }
    }
  }
}
void CIFSEntityAllParameters::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteField(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // expression in parameters
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (!pParam)
      continue;
    if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                               pParam->m_caAssignment.GetExprText(), pParam->m_caAssignment.GetExprValues(),
                               PositionsE, PositionsV))
    {
      xtstring sShowName = pParam->m_sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_ParameterExpression,
                                          nI,
                                          pParam->m_caAssignment.GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_DeleteField(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // expression in parameters
  if (pObject->m_nIdentify < CountOfParameters())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(pObject->m_nIdentify);
    if (pParam)
    {
      xtstring sExpression = pParam->m_caAssignment.GetExprText();
      xtstring sValues = pParam->m_caAssignment.GetExprValues();
      xtstring sOffsets = pParam->m_caAssignment.GetExprOffsets();
      if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                   pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pParam->m_caAssignment.SetExprText(sExpression);
        pParam->m_caAssignment.SetExprValues(sValues);
        pParam->m_caAssignment.SetExprOffsets(sOffsets);
        ROC_PropertyChanged_ER();
      }
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeParamName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

#ifdef _DEBUG
  assert(GetOwnerEntity());
#endif // _DEBUG
  if (GetOwnerEntity())
  {
    // TextFormOfParameters
    if (GetOwnerEntity()->GetERTable_FromERTableOrGlobalTable() == sTableName
        && GetParameter(sOldName))
    {
      CIFSEntityParameterForTextForm* pParam = GetParameter(sOldName);
      xtstring sTextFormParamName = GetParameterNameForTextForm(pParam->m_sName);
      int nPos, nLen;
      if (GetPositionOfNameForSyncEngineInTextForm(sTextFormParamName, pParam->m_sName, nPos, nLen)
          && nPos != -1)
      {
        // create object
        CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                           SE_MF_GetModuleName(),
                                                           GetOwnerEntityName(),
                                                           GetName(),
                                                           CSyncResponse_ER::GetTypeOfResponseObject(),
                                                           CSyncResponse_ER::GetSyncResponseObject());
        if (!pResponseObject)
          return;
        // set changes positions
        pResponseObject->SetChangePositions(tSyncProp_IFSParameters_TextForm,
                                            1000,
                                            GetTextFormOfParameters(),
                                            nPos);
        cUsers.push_back(pResponseObject);
      }
    }
    // parameter
    if (GetOwnerEntity()->GetERTable_FromERTableOrGlobalTable() == sTableName
        && GetParameter(sOldName))
    {
      xtstring sShowName = sOldName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_Parameter,
                                          2000,
                                          sOldName);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeParamName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // TextFormOfParameters
  if (pObject->m_nIdentify == 1000 && pObject->GetChange())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(sOldName);
    xtstring sTextFormParamName = GetParameterNameForTextForm(pParam->m_sName);
    xtstring sNewTextFormParamName = GetParameterNameForTextForm(sNewName);
    int nPos, nLen;
    if (GetPositionOfNameForTextFormInTextForm(sTextFormParamName, nPos, nLen)
        && nPos != -1)
    {
      m_sTextFormOfParameters.replace(m_sTextFormOfParameters.begin() + nPos,
                                      m_sTextFormOfParameters.begin() + nPos + nLen,
                                      sNewTextFormParamName.c_str());
      ROC_PropertyChanged_ER();
    }
  }
  // parameter
  if (pObject->m_nIdentify == 2000 && pObject->GetChange())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(sOldName);
    pParam->m_sName = sNewName;
    ROC_PropertyChanged_ER();
  }
}
void CIFSEntityAllParameters::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteParam(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

#ifdef _DEBUG
  assert(GetOwnerEntity());
#endif // _DEBUG
  if (GetOwnerEntity())
  {
    // TextFormOfParameters
    if (GetOwnerEntity()->GetERTable_FromERTableOrGlobalTable() == sTableName
        && GetParameter(sName))
    {
      CIFSEntityParameterForTextForm* pParam = GetParameter(sName);
      xtstring sTextFormParamName = GetParameterNameForTextForm(pParam->m_sName);
      int nPos, nLen;
      if (GetPositionOfNameForSyncEngineInTextForm(sTextFormParamName, pParam->m_sName, nPos, nLen)
          && nPos != -1)
      {
        // create object
        CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                           SE_MF_GetModuleName(),
                                                           GetOwnerEntityName(),
                                                           GetName(),
                                                           CSyncResponse_ER::GetTypeOfResponseObject(),
                                                           CSyncResponse_ER::GetSyncResponseObject());
        if (!pResponseObject)
          return;
        // set changes positions
        pResponseObject->SetChangePositions(tSyncProp_IFSParameters_TextForm,
                                            1000,
                                            GetTextFormOfParameters(),
                                            nPos);
        cUsers.push_back(pResponseObject);
      }
    }
    // parameter
    if (GetOwnerEntity()->GetERTable_FromERTableOrGlobalTable() == sTableName
        && GetParameter(sName))
    {
      xtstring sShowName = sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_Parameter,
                                          2000,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_DeleteParam(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // TextFormOfParameters
  if (pObject->m_nIdentify == 1000 && pObject->GetChange())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(sName);
    xtstring sTextFormParamName = GetParameterNameForTextForm(pParam->m_sName);
    int nPos, nLen;
    if (GetPositionOfNameForTextFormInTextForm(sTextFormParamName, nPos, nLen)
        && nPos != -1)
    {
      m_sTextFormOfParameters.replace(m_sTextFormOfParameters.begin() + nPos,
                                      m_sTextFormOfParameters.begin() + nPos + nLen,
                                      _XT(""));
      ROC_PropertyChanged_ER();
    }
  }
  // parameter
  if (pObject->m_nIdentify == 2000 && pObject->GetChange())
  {
    RemoveParameter(sName);
    ROC_PropertyChanged_ER();
  }
}
void CIFSEntityAllParameters::ROC_ParamAdded(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sName,
                                             CERModelParamInd* pParam,
                                             CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

#ifdef _DEBUG
  assert(GetOwnerEntity());
#endif // _DEBUG
  if (GetOwnerEntity())
  {
    // TextFormOfParameters
    if (GetOwnerEntity()->GetERTable_FromERTableOrGlobalTable() == sTableName
        && pParam)
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_TextForm,
                                          1000,
                                          GetTextFormOfParameters());
      cUsers.push_back(pResponseObject);
    }
    // parameter
    if (GetOwnerEntity()->GetERTable_FromERTableOrGlobalTable() == sTableName
        && pParam)
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetOwnerEntityName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_Parameter,
                                          2000,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_ParamAdded(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sName,
                                             CERModelParamInd* pParam,
                                             CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // TextFormOfParameters
  if (pObject->m_nIdentify == 1000 && pObject->GetChange() && pParam)
  {
    CERModelParamInd* pNewParam = (CERModelParamInd*)pParam;
    xtstring sTextFormParamName = GetParameterNameForTextForm(pNewParam->GetName());
    m_sTextFormOfParameters += sTextFormParamName;
    ROC_PropertyChanged_ER();
  }
  // parameter
  if (pObject->m_nIdentify == 2000 && pObject->GetChange() && pParam)
  {
    CERModelParamInd* pNewParam = (CERModelParamInd*)pParam;
    CIFSEntityParameterForTextForm cParameter;
    cParameter.AssignFromERModelParamInd(pNewParam);
    AddParameter(&cParameter);
    ROC_PropertyChanged_ER();
  }
}
void CIFSEntityAllParameters::ROC_ParamChanged(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sName,
                                               CERModelParamInd* pParam,
                                               CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

#ifdef _DEBUG
  assert(GetOwnerEntity());
#endif // _DEBUG
  // attributes in parameters
  if (GetOwnerEntity())
  {
    if (GetOwnerEntity()->GetERTable_FromERTableOrGlobalTable() == sTableName
        && GetParameter(sName)
        && pParam)
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_Parameter,
                                          1000,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_ParamChanged(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sName,
                                               CERModelParamInd* pParam,
                                               CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // attributes in parameters
  if (pObject->m_nIdentify == 1000 && pObject->GetChange() && pParam)
  {
    CERModelParamInd* pChangedParam = (CERModelParamInd*)pParam;
    CIFSEntityParameterForTextForm* pMyParam = GetParameter(sName);
    pMyParam->AssignFromChangedERModelParamInd(pChangedParam);
    ROC_PropertyChanged_ER();
  }
}
void CIFSEntityAllParameters::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                             const xtstring& sTableName,
                                                             const xtstring& sParentTableName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteRelation(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_DeleteRelation(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_RelationAdded(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sName,
                                                CERModelRelationInd* pRelation,
                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_RelationAdded(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sName,
                                                CERModelRelationInd* pRelation,
                                                CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_RelationChanged(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sParentTableName,
                                                  const xtstring& sName,
                                                  CERModelRelationInd* pRelation,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_RelationChanged(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sParentTableName,
                                                  const xtstring& sName,
                                                  CERModelRelationInd* pRelation,
                                                  CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_ChangeIFSName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_ChangeIFSName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteIFS(const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_DeleteIFS(const xtstring& sName,
                                            CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_ChangeEntityName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // expression in parameters
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (!pParam)
      continue;
    if (EntityExistInExpression(sOldName,
                                pParam->m_caAssignment.GetExprText(), pParam->m_caAssignment.GetExprValues(),
                                PositionsE, PositionsV))
    {
      xtstring sShowName = pParam->m_sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_ParameterExpression,
                                          nI,
                                          pParam->m_caAssignment.GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeEntityName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // expression in parameters
  if (pObject->m_nIdentify < CountOfParameters())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(pObject->m_nIdentify);
    if (pParam)
    {
      xtstring sExpression = pParam->m_caAssignment.GetExprText();
      xtstring sValues = pParam->m_caAssignment.GetExprValues();
      xtstring sOffsets = pParam->m_caAssignment.GetExprOffsets();
      if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pParam->m_caAssignment.SetExprText(sExpression);
        pParam->m_caAssignment.SetExprValues(sValues);
        pParam->m_caAssignment.SetExprOffsets(sOffsets);
        ROC_PropertyChanged_IFS();
      }
    }
  }
}
void CIFSEntityAllParameters::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                           const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteEntity(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // expression in parameters
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (!pParam)
      continue;
    if (EntityExistInExpression(sName,
                                pParam->m_caAssignment.GetExprText(), pParam->m_caAssignment.GetExprValues(),
                                PositionsE, PositionsV))
    {
      xtstring sShowName = pParam->m_sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_ParameterExpression,
                                          nI,
                                          pParam->m_caAssignment.GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_DeleteEntity(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // expression in parameters
  if (pObject->m_nIdentify < CountOfParameters())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(pObject->m_nIdentify);
    if (pParam)
    {
      xtstring sExpression = pParam->m_caAssignment.GetExprText();
      xtstring sValues = pParam->m_caAssignment.GetExprValues();
      xtstring sOffsets = pParam->m_caAssignment.GetExprOffsets();
      if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pParam->m_caAssignment.SetExprText(sExpression);
        pParam->m_caAssignment.SetExprValues(sValues);
        pParam->m_caAssignment.SetExprOffsets(sOffsets);
        ROC_PropertyChanged_IFS();
      }
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeVariableName(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // expression in parameters
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (!pParam)
      continue;
    if (VariableExistInExpression(sEntityName, sOldName,
                                  pParam->m_caAssignment.GetExprText(), pParam->m_caAssignment.GetExprValues(),
                                  PositionsE, PositionsV))
    {
      xtstring sShowName = pParam->m_sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_ParameterExpression,
                                          nI,
                                          pParam->m_caAssignment.GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeVariableName(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // expression in parameters
  if (pObject->m_nIdentify < CountOfParameters())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(pObject->m_nIdentify);
    if (pParam)
    {
      xtstring sExpression = pParam->m_caAssignment.GetExprText();
      xtstring sValues = pParam->m_caAssignment.GetExprValues();
      xtstring sOffsets = pParam->m_caAssignment.GetExprOffsets();
      if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                      pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pParam->m_caAssignment.SetExprText(sExpression);
        pParam->m_caAssignment.SetExprValues(sValues);
        pParam->m_caAssignment.SetExprOffsets(sOffsets);
        ROC_PropertyChanged_IFS();
      }
    }
  }
}
void CIFSEntityAllParameters::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                             const xtstring& sEntityName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteVariable(const xtstring& sIFS,
                                                 const xtstring& sEntityName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // expression in parameters
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (!pParam)
      continue;
    if (VariableExistInExpression(sEntityName, sName,
                                  pParam->m_caAssignment.GetExprText(), pParam->m_caAssignment.GetExprValues(),
                                  PositionsE, PositionsV))
    {
      xtstring sShowName = pParam->m_sName + _XT("/") + GetName();
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         sShowName,
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSParameters_ParameterExpression,
                                          nI,
                                          pParam->m_caAssignment.GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityAllParameters::ROC_DeleteVariable(const xtstring& sIFS,
                                                 const xtstring& sEntityName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // expression in parameters
  if (pObject->m_nIdentify < CountOfParameters())
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(pObject->m_nIdentify);
    if (pParam)
    {
      xtstring sExpression = pParam->m_caAssignment.GetExprText();
      xtstring sValues = pParam->m_caAssignment.GetExprValues();
      xtstring sOffsets = pParam->m_caAssignment.GetExprOffsets();
      if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                      pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pParam->m_caAssignment.SetExprText(sExpression);
        pParam->m_caAssignment.SetExprValues(sValues);
        pParam->m_caAssignment.SetExprOffsets(sOffsets);
        ROC_PropertyChanged_IFS();
      }
    }
  }
}
void CIFSEntityAllParameters::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObject* pObject)
{
}
void CIFSEntityAllParameters::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                                  const xtstring& sName,
                                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CIFSEntityAllParameters::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAllParameters::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObject* pObject)
{
}

































/////////////////////////////////////////////////////////////////////////////
// CIFSEntityInd
/////////////////////////////////////////////////////////////////////////////
void CIFSEntityInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (ProjConstantExistInExpression(sOldName,
                                    GetMainCondition().GetExprText(), GetMainCondition().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Condition,
                                        1,
                                        GetMainCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // port condition
  if (ProjConstantExistInExpression(sOldName,
                                    GetPortCondition().GetExprText(), GetPortCondition().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_PortCondition,
                                        2,
                                        GetPortCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // hit condition
  if (ProjConstantExistInExpression(sOldName,
                                    GetHitCondition().GetExprText(), GetHitCondition().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_HitCondition,
                                        3,
                                        GetHitCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetMainCondition().GetExprText();
    xtstring sValues = GetMainCondition().GetExprValues();
    xtstring sOffsets = GetMainCondition().GetExprOffsets();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetMainCondition().SetExprText(sExpression);
      GetMainCondition().SetExprValues(sValues);
      GetMainCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_Proj();
    }
  }
  // port condition
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPortCondition().GetExprText();
    xtstring sValues = GetPortCondition().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPortCondition().SetExprText(sExpression);
      GetPortCondition().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // hit condition
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetHitCondition().GetExprText();
    xtstring sValues = GetHitCondition().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitCondition().SetExprText(sExpression);
      GetHitCondition().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CIFSEntityInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteProjConstant(const xtstring& sName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (ProjConstantExistInExpression(sName,
                                    GetMainCondition().GetExprText(), GetMainCondition().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Condition,
                                        1,
                                        GetMainCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // port condition
  if (ProjConstantExistInExpression(sName,
                                    GetPortCondition().GetExprText(), GetPortCondition().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_PortCondition,
                                        2,
                                        GetPortCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // hit condition
  if (ProjConstantExistInExpression(sName,
                                    GetHitCondition().GetExprText(), GetHitCondition().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_HitCondition,
                                        3,
                                        GetHitCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_DeleteProjConstant(const xtstring& sName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetMainCondition().GetExprText();
    xtstring sValues = GetMainCondition().GetExprValues();
    xtstring sOffsets = GetMainCondition().GetExprOffsets();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetMainCondition().SetExprText(sExpression);
      GetMainCondition().SetExprValues(sValues);
      GetMainCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_Proj();
    }
  }
  // port condition
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPortCondition().GetExprText();
    xtstring sValues = GetPortCondition().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPortCondition().SetExprText(sExpression);
      GetPortCondition().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // hit condition
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetHitCondition().GetExprText();
    xtstring sValues = GetHitCondition().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitCondition().SetExprText(sExpression);
      GetHitCondition().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CIFSEntityInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                             CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                             CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteERModel(const xtstring& sName,
                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_DeleteERModel(const xtstring& sName,
                                      CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  // er table - er table
  if (GetERTableFromRaw() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_ERTableStoneType,
                                        1,
                                        GetERTableFromRaw());
    cUsers.push_back(pResponseObject);
  }
  // use relations
  for (size_t nI = 0; nI < CountOfConnections(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnection(nI);
    if (!pConn)
      continue;
    CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
    for (size_t nJ = 0; nJ < cRelations.CountOfRelations(); nJ++)
    {
      CRelationToUse* pRelToUse = cRelations.GetRelationToUse(nJ);
      if (!pRelToUse)
        continue;
      if (pRelToUse->GetParentTable() == sOldName || pRelToUse->GetChildTable() == sOldName)
      {
        // create object
        CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                          SE_MF_GetModuleName(),
                                                          GetName(),
                                                          GetName(),
                                                          CSyncResponse_ER::GetTypeOfResponseObject(),
                                                          CSyncResponse_ER::GetSyncResponseObject());
        if (!pResponseObject)
          return;
        // set changes positions
        pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                            IDENTIFY_CONNECTION_BASE + IDENTIFY_RELATION_STEP * nI + nJ,
                                            sOldName);
        cUsers.push_back(pResponseObject);
      }
    }
  }
  for (size_t nI = 0; nI < CountOfConnectionsToChilds(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(nI);
    if (!pConn)
      continue;
    CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
    for (size_t nJ = 0; nJ < cRelations.CountOfRelations(); nJ++)
    {
      CRelationToUse* pRelToUse = cRelations.GetRelationToUse(nJ);
      if (!pRelToUse)
        continue;
      if (pRelToUse->GetParentTable() == sOldName || pRelToUse->GetChildTable() == sOldName)
      {
        // create object
        CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                          SE_MF_GetModuleName(),
                                                          GetName(),
                                                          GetName(),
                                                          CSyncResponse_ER::GetTypeOfResponseObject(),
                                                          CSyncResponse_ER::GetSyncResponseObject());
        if (!pResponseObject)
          return;
        // set changes positions
        pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                            IDENTIFY_CONNECTIONTOCHILD_BASE + IDENTIFY_RELATION_STEP * nI + nJ,
                                            sOldName);
        cUsers.push_back(pResponseObject);
      }
    }
  }
}
void CIFSEntityInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // er table - er table
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERTableToRaw(sNewName);
    ROC_PropertyChanged_ER();
  }
  // use relations
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTION_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTION_END
      && pObject->GetChange())
  {
    size_t nIndex = pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE;
    size_t nConnectionIndex = nIndex / IDENTIFY_RELATION_STEP;
    size_t nRelationIndex = nIndex % IDENTIFY_RELATION_STEP;
    CIFSEntityConnectionInd* pConn = GetConnection(nConnectionIndex);
    if (pConn)
    {
      CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
      CRelationToUse* pRelToUse = cRelations.GetRelationToUse(nRelationIndex);
      if (pRelToUse)
      {
        if (pRelToUse->GetParentTable() == sOldName)
          pRelToUse->SetParentTable(sNewName);
        if (pRelToUse->GetChildTable() == sOldName)
          pRelToUse->SetChildTable(sNewName);
        ROC_PropertyChanged_ER();
        SE_RelationNameChanged();
      }
    }
  }
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTIONTOCHILD_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTIONTOCHILD_END
      && pObject->GetChange())
  {
    size_t nIndex = pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE;
    size_t nConnectionIndex = nIndex / IDENTIFY_RELATION_STEP;
    size_t nRelationIndex = nIndex % IDENTIFY_RELATION_STEP;
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(nConnectionIndex);
    if (pConn)
    {
      CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
      CRelationToUse* pRelToUse = cRelations.GetRelationToUse(nRelationIndex);
      if (pRelToUse)
      {
        if (pRelToUse->GetParentTable() == sOldName)
          pRelToUse->SetParentTable(sNewName);
        if (pRelToUse->GetChildTable() == sOldName)
          pRelToUse->SetChildTable(sNewName);
        ROC_PropertyChanged_ER();
        SE_RelationNameChanged();
      }
    }
  }
}
void CIFSEntityInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteTable(const xtstring& sERModelName,
                                    const xtstring& sName,
                                    CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er table - er table
  if (SE_MF_GetERModelName() == sERModelName && GetERTableFromRaw() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_ERTableStoneType,
                                        1,
                                        GetERTableFromRaw());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_DeleteTable(const xtstring& sERModelName,
                                    const xtstring& sName,
                                    CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // er table - er table
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw(_XT(""));
    ROC_PropertyChanged_ER();
  }
}
void CIFSEntityInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetMainCondition().GetExprText(), GetMainCondition().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Condition,
                                        1,
                                        GetMainCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // port condition
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetPortCondition().GetExprText(), GetPortCondition().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_PortCondition,
                                        2,
                                        GetPortCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // hit condition
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetHitCondition().GetExprText(), GetHitCondition().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_HitCondition,
                                        3,
                                        GetHitCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // parameter table values (only main entity)
  for (size_t nI = 0; nI < GetCountOfParamTableValues(); nI++)
  {
    xtstring sPFName;
    if (GetParamTableFieldName(nI, sPFName))
    {
      if (sOldName == sPFName)
      {
        // create object
        CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                           SE_MF_GetModuleName(),
                                                           GetName(),
                                                           GetName(),
                                                           CSyncResponse_ER::GetTypeOfResponseObject(),
                                                           CSyncResponse_ER::GetSyncResponseObject());
        if (!pResponseObject)
          return;
        // set changes positions
        pResponseObject->SetChangePositions(tSyncProp_IFSEntity_ParamValues,
                                            4,
                                            sPFName);
        cUsers.push_back(pResponseObject);
      }
    }
  }
  // use relations
  for (size_t nI = 0; nI < CountOfConnections(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnection(nI);
    if (!pConn)
      continue;
    CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
    for (size_t nJ = 0; nJ < cRelations.CountOfRelations(); nJ++)
    {
      CRelationToUse* pRelToUse = cRelations.GetRelationToUse(nJ);
      if (!pRelToUse)
        continue;
      if (pRelToUse->GetParentTable() == sTableName || pRelToUse->GetChildTable() == sTableName)
      {
        for (size_t nK = 0; nK < pRelToUse->CountOfFields(); nK++)
        {
          CRelationFieldToUse* pFields = pRelToUse->GetFields(nK);
          if (!pFields)
            continue;
          if (pRelToUse->GetParentTable() == sTableName && pFields->GetParentField() == sOldName
              || pRelToUse->GetChildTable() == sTableName && pFields->GetChildField() == sOldName)
          {
            // create object
            CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                              SE_MF_GetModuleName(),
                                                              GetName(),
                                                              GetName(),
                                                              CSyncResponse_ER::GetTypeOfResponseObject(),
                                                              CSyncResponse_ER::GetSyncResponseObject());
            if (!pResponseObject)
              return;
            // set changes positions
            pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                                IDENTIFY_CONNECTION_BASE + IDENTIFY_RELATION_STEP * nI + nJ,
                                                sOldName);
            cUsers.push_back(pResponseObject);
            break;
          }
        }
      }
    }
  }
  for (size_t nI = 0; nI < CountOfConnectionsToChilds(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(nI);
    if (!pConn)
      continue;
    CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
    for (size_t nJ = 0; nJ < cRelations.CountOfRelations(); nJ++)
    {
      CRelationToUse* pRelToUse = cRelations.GetRelationToUse(nJ);
      if (!pRelToUse)
        continue;
      if (pRelToUse->GetParentTable() == sTableName || pRelToUse->GetChildTable() == sTableName)
      {
        for (size_t nK = 0; nK < pRelToUse->CountOfFields(); nK++)
        {
          CRelationFieldToUse* pFields = pRelToUse->GetFields(nK);
          if (!pFields)
            continue;
          if (pRelToUse->GetParentTable() == sTableName && pFields->GetParentField() == sOldName
              || pRelToUse->GetChildTable() == sTableName && pFields->GetChildField() == sOldName)
          {
            // create object
            CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                              SE_MF_GetModuleName(),
                                                              GetName(),
                                                              GetName(),
                                                              CSyncResponse_ER::GetTypeOfResponseObject(),
                                                              CSyncResponse_ER::GetSyncResponseObject());
            if (!pResponseObject)
              return;
            // set changes positions
            pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                                IDENTIFY_CONNECTIONTOCHILD_BASE + IDENTIFY_RELATION_STEP * nI + nJ,
                                                sOldName);
            cUsers.push_back(pResponseObject);
            break;
          }
        }
      }
    }
  }
}
void CIFSEntityInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetMainCondition().GetExprText();
    xtstring sValues = GetMainCondition().GetExprValues();
    xtstring sOffsets = GetMainCondition().GetExprOffsets();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetMainCondition().SetExprText(sExpression);
      GetMainCondition().SetExprValues(sValues);
      GetMainCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_ER();
    }
  }
  // port condition
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPortCondition().GetExprText();
    xtstring sValues = GetPortCondition().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPortCondition().SetExprText(sExpression);
      GetPortCondition().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // hit condition
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetHitCondition().GetExprText();
    xtstring sValues = GetHitCondition().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitCondition().SetExprText(sExpression);
      GetHitCondition().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // parameter table values (only main entity)
  if (pObject->m_nIdentify == 4 && pObject->GetChange())
  {
    CxtstringMap newFieldValues;
    for (size_t nI = 0; nI < GetCountOfParamTableValues(); nI++)
    {
      xtstring sPFName;
      xtstring sPFValue;
      if (GetParamTableFieldName(nI, sPFName))
      {
        if (GetParamTableFieldValue(nI, sPFValue))
        {
          if (sOldName == sPFName)
            sPFName = sNewName;
          CxtstringMapPair cNewValue(sPFName, sPFValue);
          newFieldValues.insert(cNewValue);
        }
      }
    }
    RemoveAllParamTableFieldValues();
    AppendParamTableFieldValues(newFieldValues);
  }
  // use relations
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTION_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTION_END
      && pObject->GetChange())
  {
    size_t nIndex = pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE;
    size_t nConnectionIndex = nIndex / IDENTIFY_RELATION_STEP;
    size_t nRelationIndex = nIndex % IDENTIFY_RELATION_STEP;
    CIFSEntityConnectionInd* pConn = GetConnection(nConnectionIndex);
    if (pConn)
    {
      CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
      CRelationToUse* pRelToUse = cRelations.GetRelationToUse(nRelationIndex);
      if (pRelToUse)
      {
        for (size_t nI = 0; nI < pRelToUse->CountOfFields(); nI++)
        {
          CRelationFieldToUse* pFields = pRelToUse->GetFields(nI);
          if (!pFields)
            continue;
          if (pRelToUse->GetParentTable() == sTableName && pFields->GetParentField() == sOldName)
            pFields->SetParentField(sNewName);
          if (pRelToUse->GetChildTable() == sTableName && pFields->GetChildField() == sOldName)
            pFields->SetChildField(sNewName);
        }
        ROC_PropertyChanged_ER();
        SE_RelationNameChanged();
      }
    }
  }
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTIONTOCHILD_BASE
    && pObject->m_nIdentify <= IDENTIFY_CONNECTIONTOCHILD_END
    && pObject->GetChange())
  {
    size_t nIndex = pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE;
    size_t nConnectionIndex = nIndex / IDENTIFY_RELATION_STEP;
    size_t nRelationIndex = nIndex % IDENTIFY_RELATION_STEP;
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(nConnectionIndex);
    if (pConn)
    {
      CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
      CRelationToUse* pRelToUse = cRelations.GetRelationToUse(nRelationIndex);
      if (pRelToUse)
      {
        for (size_t nI = 0; nI < pRelToUse->CountOfFields(); nI++)
        {
          CRelationFieldToUse* pFields = pRelToUse->GetFields(nI);
          if (!pFields)
            continue;
          if (pRelToUse->GetParentTable() == sTableName && pFields->GetParentField() == sOldName)
            pFields->SetParentField(sNewName);
          if (pRelToUse->GetChildTable() == sTableName && pFields->GetChildField() == sOldName)
            pFields->SetChildField(sNewName);
        }
        ROC_PropertyChanged_ER();
        SE_RelationNameChanged();
      }
    }
  }
}
void CIFSEntityInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteField(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetMainCondition().GetExprText(), GetMainCondition().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Condition,
                                        1,
                                        GetMainCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // port condition
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetPortCondition().GetExprText(), GetPortCondition().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_PortCondition,
                                        2,
                                        GetPortCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // hit condition
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetHitCondition().GetExprText(), GetHitCondition().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_HitCondition,
                                        3,
                                        GetHitCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // parameter table values (only main entity)
  for (size_t nI = 0; nI < GetCountOfParamTableValues(); nI++)
  {
    xtstring sPFName;
    if (GetParamTableFieldName(nI, sPFName))
    {
      if (sName == sPFName)
      {
        // create object
        CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                           SE_MF_GetModuleName(),
                                                           GetName(),
                                                           GetName(),
                                                           CSyncResponse_ER::GetTypeOfResponseObject(),
                                                           CSyncResponse_ER::GetSyncResponseObject());
        if (!pResponseObject)
          return;
        // set changes positions
        pResponseObject->SetChangePositions(tSyncProp_IFSEntity_ParamValues,
                                            4,
                                            sPFName);
        cUsers.push_back(pResponseObject);
      }
    }
  }
}
void CIFSEntityInd::ROC_DeleteField(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetMainCondition().GetExprText();
    xtstring sValues = GetMainCondition().GetExprValues();
    xtstring sOffsets = GetMainCondition().GetExprOffsets();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetMainCondition().SetExprText(sExpression);
      GetMainCondition().SetExprValues(sValues);
      GetMainCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_ER();
    }
  }
  // port condition
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPortCondition().GetExprText();
    xtstring sValues = GetPortCondition().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPortCondition().SetExprText(sExpression);
      GetPortCondition().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // hit condition
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetHitCondition().GetExprText();
    xtstring sValues = GetHitCondition().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitCondition().SetExprText(sExpression);
      GetHitCondition().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // parameter table values (only main entity)
  if (pObject->m_nIdentify == 4 && pObject->GetChange())
  {
    CxtstringMap newFieldValues;
    for (size_t nI = 0; nI < GetCountOfParamTableValues(); nI++)
    {
      xtstring sPFName;
      xtstring sPFValue;
      if (GetParamTableFieldName(nI, sPFName))
      {
        if (GetParamTableFieldValue(nI, sPFValue))
        {
          if (sName != sPFName)
          {
            CxtstringMapPair cNewValue(sPFName, sPFValue);
            newFieldValues.insert(cNewValue);
          }
        }
      }
    }
    RemoveAllParamTableFieldValues();
    AppendParamTableFieldValues(newFieldValues);
  }
}
void CIFSEntityInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteParam(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_DeleteParam(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_ParamAdded(const xtstring& sERModelName,
                                   const xtstring& sTableName,
                                   const xtstring& sName,
                                   CERModelParamInd* pParam,
                                   CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_ParamAdded(const xtstring& sERModelName,
                                   const xtstring& sTableName,
                                   const xtstring& sName,
                                   CERModelParamInd* pParam,
                                   CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_ParamChanged(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CERModelParamInd* pParam,
                                     CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_ParamChanged(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CERModelParamInd* pParam,
                                     CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParentTableName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  // use relations
  for (size_t nI = 0; nI < CountOfConnections(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnection(nI);
    if (!pConn)
      continue;
    CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
    if (cRelations.GetRelationToUse(sOldName))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                          IDENTIFY_CONNECTION_BASE + nI,
                                          sOldName);
      cUsers.push_back(pResponseObject);
    }
  }
  for (size_t nI = 0; nI < CountOfConnectionsToChilds(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(nI);
    if (!pConn)
      continue;
    CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
    if (cRelations.GetRelationToUse(sOldName))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                          IDENTIFY_CONNECTIONTOCHILD_BASE + nI,
                                          sOldName);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParentTableName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // use relations
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTION_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTION_END
      && pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE < CountOfConnections()
      && pObject->GetChange())
  {
    CIFSEntityConnectionInd* pConn = GetConnection(pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE);
    if (pConn)
    {
      CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
      if (cRelations.GetRelationToUse(sOldName))
      {
        cRelations.GetRelationToUse(sOldName)->SetName(sNewName);
        ROC_PropertyChanged_ER();
        SE_RelationNameChanged();
      }
    }
  }
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTIONTOCHILD_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTIONTOCHILD_END
      && pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE < CountOfConnectionsToChilds()
      && pObject->GetChange())
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE);
    if (pConn)
    {
      CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
      if (cRelations.GetRelationToUse(sOldName))
      {
        cRelations.GetRelationToUse(sOldName)->SetName(sNewName);
        ROC_PropertyChanged_ER();
        SE_RelationNameChanged();
      }
    }
  }
}
void CIFSEntityInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sParentTableName,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetERModelName() != sERModelName)
    return;

  // use relations
  for (size_t nI = 0; nI < CountOfConnections(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnection(nI);
    if (!pConn)
      continue;
    CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
    if (cRelations.GetRelationToUse(sName))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                          IDENTIFY_CONNECTION_BASE + nI,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
  for (size_t nI = 0; nI < CountOfConnectionsToChilds(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(nI);
    if (!pConn)
      continue;
    CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
    if (cRelations.GetRelationToUse(sName))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                          IDENTIFY_CONNECTIONTOCHILD_BASE + nI,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // use relations
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTION_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTION_END
      && pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE < CountOfConnections()
      && pObject->GetChange())
  {
    CIFSEntityConnectionInd* pConn = GetConnection(pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE);
    if (pConn)
    {
      CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
      if (cRelations.GetRelationToUse(sName))
      {
        cRelations.RemoveRelationToUse(sName);
        ROC_PropertyChanged_ER();
        SE_RelationNameChanged();
      }
    }
  }
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTIONTOCHILD_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTIONTOCHILD_END
      && pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE < CountOfConnectionsToChilds()
      && pObject->GetChange())
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE);
    if (pConn)
    {
      CAllRelationsToUse& cRelations = pConn->GetRelationsToUse();
      if (cRelations.GetRelationToUse(sName))
      {
        cRelations.RemoveRelationToUse(sName);
        ROC_PropertyChanged_ER();
        SE_RelationNameChanged();
      }
    }
  }
}
void CIFSEntityInd::ROC_RelationAdded(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sParentTableName,
                                      const xtstring& sName,
                                      CERModelRelationInd* pRelation,
                                      CUserOfObjectArray& cUsers)
{
  if (!pRelation)
    return;
  if (SE_MF_GetERModelName() != sERModelName)
    return;
/*
  // use relations
  for (size_t nI = 0; nI < CountOfConnections(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnection(nI);
    if (!pConn)
      continue;
    xtstring sChildEntity = pConn->GetChildEntity();
    xtstring sParentEntity = pConn->GetParentEntity();
    const CIFSEntityInd* pChildEntity = SE_MF_GetIFSEntityInd(sChildEntity);
    const CIFSEntityInd* pParentEntity = SE_MF_GetIFSEntityInd(sParentEntity);
    if (!pChildEntity || !pParentEntity)
      continue;
    if (pChildEntity->GetERTableFromRaw() == sTableName && pParentEntity->GetERTableFromRaw() == sParentTableName
        || pParentEntity->GetERTableFromRaw() == sTableName && pChildEntity->GetERTableFromRaw() == sParentTableName)
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                          IDENTIFY_CONNECTION_BASE + nI,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
  for (size_t nI = 0; nI < CountOfConnectionsToChilds(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(nI);
    if (!pConn)
      continue;
    xtstring sChildEntity = pConn->GetChildEntity();
    xtstring sParentEntity = pConn->GetParentEntity();
    const CIFSEntityInd* pChildEntity = SE_MF_GetIFSEntityInd(sChildEntity);
    const CIFSEntityInd* pParentEntity = SE_MF_GetIFSEntityInd(sParentEntity);
    if (!pChildEntity || !pParentEntity)
      continue;
    if (pChildEntity->GetERTableFromRaw() == sTableName && pParentEntity->GetERTableFromRaw() == sParentTableName
        || pParentEntity->GetERTableFromRaw() == sTableName && pChildEntity->GetERTableFromRaw() == sParentTableName)
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                          IDENTIFY_CONNECTIONTOCHILD_BASE + nI,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
*/
}
void CIFSEntityInd::ROC_RelationAdded(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sParentTableName,
                                      const xtstring& sName,
                                      CERModelRelationInd* pRelation,
                                      CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  if (!pRelation)
    return;
/*
  // use relations
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTION_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTION_END
      && pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE < CountOfConnections()
      && pObject->GetChange())
  {
    CIFSEntityConnectionInd* pConn = GetConnection(pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE);
    if (pConn)
    {
      pConn->GetRelationsToUse().AddRelationToUse(*pRelation);
      ROC_PropertyChanged_ER();
      SE_RelationNameAdded();
    }
  }
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTIONTOCHILD_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTIONTOCHILD_END
      && pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE < CountOfConnectionsToChilds()
      && pObject->GetChange())
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE);
    if (pConn)
    {
      pConn->GetRelationsToUse().AddRelationToUse(*pRelation);
      ROC_PropertyChanged_ER();
      SE_RelationNameAdded();
    }
  }
*/
}
void CIFSEntityInd::ROC_RelationChanged(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        CUserOfObjectArray& cUsers)
{
  if (!pRelation)
    return;
  if (SE_MF_GetERModelName() != sERModelName)
    return;

  // use relations
  for (size_t nI = 0; nI < CountOfConnections(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnection(nI);
    if (!pConn)
      continue;
    xtstring sChildEntity = pConn->GetChildEntity();
    xtstring sParentEntity = pConn->GetParentEntity();
    const CIFSEntityInd* pChildEntity = SE_MF_GetIFSEntityInd(sChildEntity);
    const CIFSEntityInd* pParentEntity = SE_MF_GetIFSEntityInd(sParentEntity);
    if (!pChildEntity || !pParentEntity)
      continue;
    if (pChildEntity->GetERTableFromRaw() == sTableName
            && pParentEntity->GetERTableFromRaw() == sParentTableName
        || pParentEntity->GetERTableFromRaw() == sTableName
            && pChildEntity->GetERTableFromRaw() == sParentTableName)
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                          IDENTIFY_CONNECTION_BASE + nI,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
  for (size_t nI = 0; nI < CountOfConnectionsToChilds(); nI++)
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(nI);
    if (!pConn)
      continue;
    xtstring sChildEntity = pConn->GetChildEntity();
    xtstring sParentEntity = pConn->GetParentEntity();
    const CIFSEntityInd* pChildEntity = SE_MF_GetIFSEntityInd(sChildEntity);
    const CIFSEntityInd* pParentEntity = SE_MF_GetIFSEntityInd(sParentEntity);
    if (!pChildEntity || !pParentEntity)
      continue;
    if (pChildEntity->GetERTableFromRaw() == sTableName
            && pParentEntity->GetERTableFromRaw() == sParentTableName
        || pParentEntity->GetERTableFromRaw() == sTableName
            && pChildEntity->GetERTableFromRaw() == sParentTableName)
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntity_UseRelation,
                                          IDENTIFY_CONNECTIONTOCHILD_BASE + nI,
                                          sName);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CIFSEntityInd::ROC_RelationChanged(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  if (!pRelation)
    return;

  // use relations
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTION_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTION_END
      && pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE < CountOfConnections()
      && pObject->GetChange())
  {
    CIFSEntityConnectionInd* pConn = GetConnection(pObject->m_nIdentify - IDENTIFY_CONNECTION_BASE);
    if (pConn)
    {
      pConn->GetRelationsToUse().RelationToUseChanged(*pRelation);
      ROC_PropertyChanged_ER();
      SE_RelationNameAdded();
    }
  }
  if (pObject->m_nIdentify >= IDENTIFY_CONNECTIONTOCHILD_BASE
      && pObject->m_nIdentify <= IDENTIFY_CONNECTIONTOCHILD_END
      && pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE < CountOfConnectionsToChilds()
      && pObject->GetChange())
  {
    CIFSEntityConnectionInd* pConn = GetConnectionToChilds(pObject->m_nIdentify - IDENTIFY_CONNECTIONTOCHILD_BASE);
    if (pConn)
    {
      pConn->GetRelationsToUse().RelationToUseChanged(*pRelation);
      ROC_PropertyChanged_ER();
      SE_RelationNameAdded();
    }
  }
}
void CIFSEntityInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // sub IFS
  if (GetSubIFS() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_SubIFS,
                                        1,
                                        GetSubIFS());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // sub IFS
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetSubIFS(sNewName);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntityInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteIFS(const xtstring& sName,
                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // sub IFS
  if (GetSubIFS() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_SubIFS,
                                        1,
                                        GetSubIFS());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_DeleteIFS(const xtstring& sName,
                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // sub IFS
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetSubIFS(_XT(""));
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntityInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // identity
  if (GetIdentity() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Identity,
                                        1,
                                        GetIdentity());
    cUsers.push_back(pResponseObject);
  }
  // condition
  if (EntityExistInExpression(sOldName,
                              GetMainCondition().GetExprText(), GetMainCondition().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Condition,
                                        2,
                                        GetMainCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // port condition
  if (EntityExistInExpression(sOldName,
                              GetPortCondition().GetExprText(), GetPortCondition().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_PortCondition,
                                        3,
                                        GetPortCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // hit condition
  if (EntityExistInExpression(sOldName,
                              GetHitCondition().GetExprText(), GetHitCondition().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_HitCondition,
                                        4,
                                        GetHitCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // identity
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetIdentity(sNewName);
    ROC_PropertyChanged_IFS();
  }
  // condition - replace
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetMainCondition().GetExprText();
    xtstring sValues = GetMainCondition().GetExprValues();
    xtstring sOffsets = GetMainCondition().GetExprOffsets();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetMainCondition().SetExprText(sExpression);
      GetMainCondition().SetExprValues(sValues);
      GetMainCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_IFS();
    }
  }
  // port condition
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPortCondition().GetExprText();
    xtstring sValues = GetPortCondition().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPortCondition().SetExprText(sExpression);
      GetPortCondition().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // hit condition
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetHitCondition().GetExprText();
    xtstring sValues = GetHitCondition().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitCondition().SetExprText(sExpression);
      GetHitCondition().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteEntity(const xtstring& sIFS,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // identity
  if (GetIdentity() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Identity,
                                        1,
                                        GetIdentity());
    cUsers.push_back(pResponseObject);
  }
  // condition
  if (EntityExistInExpression(sName,
                              GetMainCondition().GetExprText(), GetMainCondition().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Condition,
                                        2,
                                        GetMainCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // port condition
  if (EntityExistInExpression(sName,
                              GetPortCondition().GetExprText(), GetPortCondition().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_PortCondition,
                                        3,
                                        GetPortCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // hit condition
  if (EntityExistInExpression(sName,
                              GetHitCondition().GetExprText(), GetHitCondition().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_HitCondition,
                                        4,
                                        GetHitCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_DeleteEntity(const xtstring& sIFS,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // identity
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetIdentity(_XT(""));
    ROC_PropertyChanged_IFS();
  }
  // condition - replace
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetMainCondition().GetExprText();
    xtstring sValues = GetMainCondition().GetExprValues();
    xtstring sOffsets = GetMainCondition().GetExprOffsets();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetMainCondition().SetExprText(sExpression);
      GetMainCondition().SetExprValues(sValues);
      GetMainCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_IFS();
    }
  }
  // port condition
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPortCondition().GetExprText();
    xtstring sValues = GetPortCondition().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPortCondition().SetExprText(sExpression);
      GetPortCondition().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // hit condition
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetHitCondition().GetExprText();
    xtstring sValues = GetHitCondition().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitCondition().SetExprText(sExpression);
      GetHitCondition().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                           const xtstring& sEntityName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetMainCondition().GetExprText(), GetMainCondition().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Condition,
                                        1,
                                        GetMainCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // port condition
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetPortCondition().GetExprText(), GetPortCondition().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_PortCondition,
                                        2,
                                        GetPortCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // hit condition
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetHitCondition().GetExprText(), GetHitCondition().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_HitCondition,
                                        3,
                                        GetHitCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                           const xtstring& sEntityName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetMainCondition().GetExprText();
    xtstring sValues = GetMainCondition().GetExprValues();
    xtstring sOffsets = GetMainCondition().GetExprOffsets();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues, sOffsets,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetMainCondition().SetExprText(sExpression);
      GetMainCondition().SetExprValues(sValues);
      GetMainCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_IFS();
    }
  }
  // port condition
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPortCondition().GetExprText();
    xtstring sValues = GetPortCondition().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPortCondition().SetExprText(sExpression);
      GetPortCondition().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // hit condition
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetHitCondition().GetExprText();
    xtstring sValues = GetHitCondition().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitCondition().SetExprText(sExpression);
      GetHitCondition().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                   const xtstring& sEntityName,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteVariable(const xtstring& sIFS,
                                       const xtstring& sEntityName,
                                       const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // condition
  if (VariableExistInExpression(sEntityName, sName,
                                GetMainCondition().GetExprText(), GetMainCondition().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_Condition,
                                        1,
                                        GetMainCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // port condition
  if (VariableExistInExpression(sEntityName, sName,
                                GetPortCondition().GetExprText(), GetPortCondition().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_PortCondition,
                                        2,
                                        GetPortCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // hit condition
  if (VariableExistInExpression(sEntityName, sName,
                                GetHitCondition().GetExprText(), GetHitCondition().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_HitCondition,
                                        3,
                                        GetHitCondition().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_DeleteVariable(const xtstring& sIFS,
                                       const xtstring& sEntityName,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // condition - replace
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetMainCondition().GetExprText();
    xtstring sValues = GetMainCondition().GetExprValues();
    xtstring sOffsets = GetMainCondition().GetExprOffsets();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues, sOffsets,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetMainCondition().SetExprText(sExpression);
      GetMainCondition().SetExprValues(sValues);
      GetMainCondition().SetExprOffsets(sOffsets);
      ROC_PropertyChanged_IFS();
    }
  }
  // port condition
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPortCondition().GetExprText();
    xtstring sValues = GetPortCondition().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPortCondition().SetExprText(sExpression);
      GetPortCondition().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // hit condition
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetHitCondition().GetExprText();
    xtstring sValues = GetHitCondition().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitCondition().SetExprText(sExpression);
      GetHitCondition().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // er table - global table
  if (GetGlobalTableFromRaw() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_ERTableStoneType,
                                        1,
                                        GetGlobalTableFromRaw());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // er table - global table
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetGlobalTableToRaw(sNewName);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntityInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                        const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                            const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er table - global table
  if (SE_MF_GetModuleName() == sIFS && GetGlobalTableFromRaw() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_ERTableStoneType,
                                        1,
                                        GetGlobalTableFromRaw());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                            const xtstring& sName,
                                            CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // er table - global table
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw(_XT(""));
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntityInd::ROC_ChangeWPName(const xtstring& sOldName,
                                     const xtstring& sNewName,
                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er table - db stone name
  if (GetDBStoneNameFromRaw() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_ERTableStoneType,
                                        1,
                                        GetDBStoneNameFromRaw());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_ChangeWPName(const xtstring& sOldName,
                                     const xtstring& sNewName,
                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // er table - db stone name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDBStoneNameToRaw(sNewName);
    ROC_PropertyChanged_WP();
  }
}
void CIFSEntityInd::ROC_CheckReferencesForWP(const xtstring& sName,
                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteWP(sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteWP(const xtstring& sName,
                                 CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er table - db stone name
  if (GetDBStoneNameFromRaw() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_ERTableStoneType,
                                        1,
                                        GetDBStoneNameFromRaw());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityInd::ROC_DeleteWP(const xtstring& sName,
                                 CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // er table - db stone name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw(_XT(""));
    ROC_PropertyChanged_WP();
  }
}
void CIFSEntityInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CIFSEntityInd::ROC_CheckReferencesForWPConstant(const xtstring& sWPName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteWPConstant(sWPName, sName, cUsers);
}
void CIFSEntityInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                         const xtstring& sName,
                                         CUserOfObjectArray& cUsers)
{
}
void CIFSEntityInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                         const xtstring& sName,
                                         CUserOfObject* pObject)
{
}






































/////////////////////////////////////////////////////////////////////////////
// CIFSHitNameInd
/////////////////////////////////////////////////////////////////////////////
void CIFSHitNameInd::ROC_PropertyChanged_Proj(){if (GetOwnerEntity())GetOwnerEntity()->ROC_PropertyChanged_Proj();}
void CIFSHitNameInd::ROC_PropertyChanged_ER(){if (GetOwnerEntity())GetOwnerEntity()->ROC_PropertyChanged_ER();}
void CIFSHitNameInd::ROC_PropertyChanged_IFS(){if (GetOwnerEntity())GetOwnerEntity()->ROC_PropertyChanged_IFS();}
void CIFSHitNameInd::ROC_PropertyChanged_WP(){if (GetOwnerEntity())GetOwnerEntity()->ROC_PropertyChanged_WP();}
void CIFSHitNameInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // hit name text - python
  if (GetHitType() == tHitType_UserFunction)
  {
    if (ProjConstantExistInExpression(sOldName,
                                      GetHitName().GetExprText(), GetHitName().GetExprValues(),
                                      PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetOwnerEntityName(),
                                                         CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                         CSyncResponse_Proj::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                          1,
                                          GetHitName().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
  // template file show name
  if (ProjConstantExistInExpression(sOldName,
                                    GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFileShowName,
                                        2,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (ProjConstantExistInExpression(sOldName,
                                    GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFile,
                                        3,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (ProjConstantExistInExpression(sOldName,
                                    GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFolder,
                                        4,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (ProjConstantExistInExpression(sOldName,
                                    GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_Visibility,
                                        5,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // hit name text - python
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetHitName().GetExprText();
    xtstring sValues = GetHitName().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitName().SetExprText(sExpression);
      GetHitName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file show name
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CIFSHitNameInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteProjConstant(const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // hit name text - python
  if (GetHitType() == tHitType_UserFunction)
  {
    if (ProjConstantExistInExpression(sName,
                                      GetHitName().GetExprText(), GetHitName().GetExprValues(),
                                      PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetOwnerEntityName(),
                                                         CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                         CSyncResponse_Proj::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                          1,
                                          GetHitName().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
  // template file show name
  if (ProjConstantExistInExpression(sName,
                                    GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFileShowName,
                                        2,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (ProjConstantExistInExpression(sName,
                                    GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFile,
                                        3,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (ProjConstantExistInExpression(sName,
                                    GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFolder,
                                        4,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (ProjConstantExistInExpression(sName,
                                    GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_Visibility,
                                        5,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_DeleteProjConstant(const xtstring& sName,
                                            CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // hit name text - python
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetHitName().GetExprText();
    xtstring sValues = GetHitName().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitName().SetExprText(sExpression);
      GetHitName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CIFSHitNameInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                              CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteERModel(const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_DeleteERModel(const xtstring& sName,
                                       CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteTable(const xtstring& sERModelName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_DeleteTable(const xtstring& sERModelName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // hit name text - python
  if (GetHitType() == tHitType_UserFunction)
  {
    if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                               GetHitName().GetExprText(), GetHitName().GetExprValues(),
                               PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetOwnerEntityName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                          1,
                                          GetHitName().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
  // template file show name
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFileShowName,
                                        2,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFile,
                                        3,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFolder,
                                        4,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_Visibility,
                                        5,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // hit name text - python
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetHitName().GetExprText();
    xtstring sValues = GetHitName().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitName().SetExprText(sExpression);
      GetHitName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSHitNameInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteField(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // hit name text - python
  if (GetHitType() == tHitType_UserFunction)
  {
    if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                               GetHitName().GetExprText(), GetHitName().GetExprValues(),
                               PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetOwnerEntityName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                          1,
                                          GetHitName().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
  // template file show name
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFileShowName,
                                        2,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFile,
                                        3,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFolder,
                                        4,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_Visibility,
                                        5,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_DeleteField(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // hit name text - python
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetHitName().GetExprText();
    xtstring sValues = GetHitName().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitName().SetExprText(sExpression);
      GetHitName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file show name
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSHitNameInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteParam(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_DeleteParam(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_ParamAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CERModelParamInd* pParam,
                                    CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_ParamAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CERModelParamInd* pParam,
                                    CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_ParamChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_ParamChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                    const xtstring& sTableName,
                                                    const xtstring& sParentTableName,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_RelationAdded(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CERModelRelationInd* pRelation,
                                       CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_RelationAdded(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CERModelRelationInd* pRelation,
                                       CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_RelationChanged(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_RelationChanged(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                       const xtstring& sNewName,
                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // information sight
  if (GetInformationSight() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_IFS,
                                        1,
                                        GetInformationSight());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                       const xtstring& sNewName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // information sight
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetInformationSight(sNewName);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSHitNameInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteIFS(const xtstring& sName,
                                   CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // information sight
  if (GetInformationSight() == sName
      && SE_MF_GetModuleName() != sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_IFS,
                                        1,
                                        GetInformationSight());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_DeleteIFS(const xtstring& sName,
                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // information sight
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetInformationSight(_XT(""));
    ROC_PropertyChanged_IFS();
  }
}
void CIFSHitNameInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetInformationSight().size() != 0 && GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // ifs target entity
  if (GetIFSTargetEntity() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_IFSTargetEntity,
                                        1,
                                        GetIFSTargetEntity());
    cUsers.push_back(pResponseObject);
  }
  // hit name text - python
  if (GetHitType() == tHitType_UserFunction)
  {
    if (EntityExistInExpression(sOldName,
                                GetHitName().GetExprText(), GetHitName().GetExprValues(),
                                PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                          SE_MF_GetModuleName(),
                                                          GetOwnerEntityName(),
                                                          GetOwnerEntityName(),
                                                          CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                          CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                          2,
                                          GetHitName().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
  // template file show name
  if (EntityExistInExpression(sOldName,
                              GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFileShowName,
                                        3,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (EntityExistInExpression(sOldName,
                              GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFile,
                                        4,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (EntityExistInExpression(sOldName,
                              GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFolder,
                                        5,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (EntityExistInExpression(sOldName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_Visibility,
                                        6,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // ifs target entity
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetIFSTargetEntity(sNewName);
    ROC_PropertyChanged_IFS();
  }
  // hit name text - python
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetHitName().GetExprText();
    xtstring sValues = GetHitName().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitName().SetExprText(sExpression);
      GetHitName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // template file show name
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // template file
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 6)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSHitNameInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteEntity(const xtstring& sIFS,
                                      const xtstring& sName,
                                      CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetInformationSight().size() != 0 && GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // ifs target entity
  if (GetIFSTargetEntity() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_IFSTargetEntity,
                                        1,
                                        GetIFSTargetEntity());
    cUsers.push_back(pResponseObject);
  }
  // hit name text - python
  if (GetHitType() == tHitType_UserFunction)
  {
    if (EntityExistInExpression(sName,
                                GetHitName().GetExprText(), GetHitName().GetExprValues(),
                                PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                          SE_MF_GetModuleName(),
                                                          GetOwnerEntityName(),
                                                          GetOwnerEntityName(),
                                                          CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                          CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                          2,
                                          GetHitName().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
  // template file show name
  if (EntityExistInExpression(sName,
                              GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFileShowName,
                                        3,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (EntityExistInExpression(sName,
                              GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFile,
                                        4,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (EntityExistInExpression(sName,
                              GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFolder,
                                        5,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (EntityExistInExpression(sName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                        SE_MF_GetModuleName(),
                                                        GetOwnerEntityName(),
                                                        GetOwnerEntityName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_Visibility,
                                        6,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_DeleteEntity(const xtstring& sIFS,
                                      const xtstring& sName,
                                      CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // ifs target entity
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetIFSTargetEntity(_XT(""));
    ROC_PropertyChanged_IFS();
  }
  // hit name text - python
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetHitName().GetExprText();
    xtstring sValues = GetHitName().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitName().SetExprText(sExpression);
      GetHitName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // template file show name
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // template file
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 6)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSHitNameInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                            const xtstring& sEntityName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetInformationSight().size() != 0 && GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // hit name text - python
  if (GetHitType() == tHitType_UserFunction)
  {
    if (VariableExistInExpression(sEntityName, sOldName,
                                  GetHitName().GetExprText(), GetHitName().GetExprValues(),
                                  PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetOwnerEntityName(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                          1,
                                          GetHitName().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
  // template file show name
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFileShowName,
                                        2,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFile,
                                        3,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFolder,
                                        4,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_Visibility,
                                        5,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                            const xtstring& sEntityName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // hit name text - python
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetHitName().GetExprText();
    xtstring sValues = GetHitName().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitName().SetExprText(sExpression);
      GetHitName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file show name
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSHitNameInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                    const xtstring& sEntityName,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteVariable(const xtstring& sIFS,
                                        const xtstring& sEntityName,
                                        const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetInformationSight().size() != 0 && GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // hit name text - python
  if (GetHitType() == tHitType_UserFunction)
  {
    if (VariableExistInExpression(sEntityName, sName,
                                  GetHitName().GetExprText(), GetHitName().GetExprValues(),
                                  PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                         SE_MF_GetModuleName(),
                                                         GetOwnerEntityName(),
                                                         GetOwnerEntityName(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                          1,
                                          GetHitName().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
  // template file show name
  if (VariableExistInExpression(sEntityName, sName,
                                GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFileShowName,
                                        2,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (VariableExistInExpression(sEntityName, sName,
                                GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFile,
                                        3,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (VariableExistInExpression(sEntityName, sName,
                                GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_TemplateFolder,
                                        4,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (VariableExistInExpression(sEntityName, sName,
                                GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_Visibility,
                                        5,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_DeleteVariable(const xtstring& sIFS,
                                        const xtstring& sEntityName,
                                        const xtstring& sName,
                                        CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // hit name text - python
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetHitName().GetExprText();
    xtstring sValues = GetHitName().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetHitName().SetExprText(sExpression);
      GetHitName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file show name
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template file
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSHitNameInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                             const xtstring& sName,
                                             CUserOfObjectArray& cUsers)
{
}
void CIFSHitNameInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                             const xtstring& sName,
                                             CUserOfObject* pObject)
{
}
void CIFSHitNameInd::ROC_ChangeWPName(const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // hit name text - stone / template
  if ((GetHitType() == tHitType_Template || GetHitType() == tHitType_Stone)
      && GetHitName().GetExprText() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                        1,
                                        GetHitName().GetExprText());
    cUsers.push_back(pResponseObject);
  }
  // stone or template
  if (GetStoneOrTemplate() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_StoneOrTemplate,
                                        2,
                                        GetStoneOrTemplate());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_ChangeWPName(const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // hit name text - stone / template
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    GetHitName().Reset();
    GetHitName().SetExprText(sNewName);
    ROC_PropertyChanged_WP();
  }
  // stone or template
  if (pObject->m_nIdentify == 2 && pObject->GetChange())
  {
    SetStoneOrTemplate(sNewName);
    ROC_PropertyChanged_WP();
  }
}
void CIFSHitNameInd::ROC_CheckReferencesForWP(const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteWP(sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteWP(const xtstring& sName,
                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // hit name text - stone / template
  if ((GetHitType() == tHitType_Template || GetHitType() == tHitType_Stone)
      && GetHitName().GetExprText() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_HitName,
                                        1,
                                        GetHitName().GetExprText());
    cUsers.push_back(pResponseObject);
  }
  // stone or template
  if (GetStoneOrTemplate() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_StoneOrTemplate,
                                        2,
                                        GetStoneOrTemplate());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_DeleteWP(const xtstring& sName,
                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // hit name text - stone / template
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    GetHitName().Reset();
    ROC_PropertyChanged_WP();
  }
  // stone or template
  if (pObject->m_nIdentify == 2 && pObject->GetChange())
  {
    SetStoneOrTemplate(_XT(""));
    ROC_PropertyChanged_WP();
  }
}
void CIFSHitNameInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // stone or template ref
  if (GetStoneOrTemplate() == sWPName && GetStoneOrTemplateRef() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_StoneOrTemplateRef,
                                        1,
                                        GetStoneOrTemplateRef());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // stone or template ref
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetStoneOrTemplateRef(sNewName);
    ROC_PropertyChanged_WP();
  }
}
void CIFSHitNameInd::ROC_CheckReferencesForWPConstant(const xtstring& sWPName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteWPConstant(sWPName, sName, cUsers);
}
void CIFSHitNameInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // stone or template ref
  if (GetStoneOrTemplate() == sWPName && GetStoneOrTemplateRef() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityHit_StoneOrTemplateRef,
                                        1,
                                        GetStoneOrTemplateRef());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSHitNameInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // stone or template ref
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetStoneOrTemplateRef(_XT(""));
    ROC_PropertyChanged_WP();
  }
}































/////////////////////////////////////////////////////////////////////////////
// CIFSEntityAssignmentInd
/////////////////////////////////////////////////////////////////////////////
void CIFSEntityAssignmentInd::ROC_PropertyChanged_Proj(){if (GetOwnerEntity())GetOwnerEntity()->ROC_PropertyChanged_Proj();}
void CIFSEntityAssignmentInd::ROC_PropertyChanged_ER(){if (GetOwnerEntity())GetOwnerEntity()->ROC_PropertyChanged_ER();}
void CIFSEntityAssignmentInd::ROC_PropertyChanged_IFS(){if (GetOwnerEntity())GetOwnerEntity()->ROC_PropertyChanged_IFS();}
void CIFSEntityAssignmentInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // assignment
  if (ProjConstantExistInExpression(sOldName,
                                    GetAssignment().GetExprText(), GetAssignment().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Assignment,
                                        1,
                                        GetAssignment().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignfield
  if (ProjConstantExistInExpression(sOldName,
                                    GetAssignField().GetExprText(), GetAssignField().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignField,
                                        2,
                                        GetAssignField().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignvalue
  if (ProjConstantExistInExpression(sOldName,
                                    GetAssignValue().GetExprText(), GetAssignValue().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignValue,
                                        3,
                                        GetAssignValue().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityAssignmentInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // assignment
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetAssignment().GetExprText();
    xtstring sValues = GetAssignment().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignment().SetExprText(sExpression);
      GetAssignment().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignfield
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAssignField().GetExprText();
    xtstring sValues = GetAssignField().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignField().SetExprText(sExpression);
      GetAssignField().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignvalue
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAssignValue().GetExprText();
    xtstring sValues = GetAssignValue().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignValue().SetExprText(sExpression);
      GetAssignValue().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // assignment
  if (ProjConstantExistInExpression(sName,
                                    GetAssignment().GetExprText(), GetAssignment().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Assignment,
                                        1,
                                        GetAssignment().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignfield
  if (ProjConstantExistInExpression(sName,
                                    GetAssignField().GetExprText(), GetAssignField().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignField,
                                        2,
                                        GetAssignField().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignvalue
  if (ProjConstantExistInExpression(sName,
                                    GetAssignValue().GetExprText(), GetAssignValue().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignValue,
                                        3,
                                        GetAssignValue().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityAssignmentInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // assignment
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetAssignment().GetExprText();
    xtstring sValues = GetAssignment().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignment().SetExprText(sExpression);
      GetAssignment().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignfield
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAssignField().GetExprText();
    xtstring sValues = GetAssignField().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignField().SetExprText(sExpression);
      GetAssignField().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignvalue
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAssignValue().GetExprText();
    xtstring sValues = GetAssignValue().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignValue().SetExprText(sExpression);
      GetAssignValue().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityAssignmentInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                       CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteERModel(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_DeleteERModel(const xtstring& sName,
                                                CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteTable(const xtstring& sERModelName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_DeleteTable(const xtstring& sERModelName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // assignment
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetAssignment().GetExprText(), GetAssignment().GetExprValues(),
                             PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Assignment,
                                        1,
                                        GetAssignment().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignfield
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetAssignField().GetExprText(), GetAssignField().GetExprValues(),
                             PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignField,
                                        2,
                                        GetAssignField().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignvalue
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetAssignValue().GetExprText(), GetAssignValue().GetExprValues(),
                             PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignValue,
                                        3,
                                        GetAssignValue().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityAssignmentInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // assignment
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetAssignment().GetExprText();
    xtstring sValues = GetAssignment().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignment().SetExprText(sExpression);
      GetAssignment().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignfield
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAssignField().GetExprText();
    xtstring sValues = GetAssignField().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignField().SetExprText(sExpression);
      GetAssignField().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignvalue
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAssignValue().GetExprText();
    xtstring sValues = GetAssignValue().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignValue().SetExprText(sExpression);
      GetAssignValue().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteField(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // assignment
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetAssignment().GetExprText(), GetAssignment().GetExprValues(),
                             PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Assignment,
                                        1,
                                        GetAssignment().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignfield
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetAssignField().GetExprText(), GetAssignField().GetExprValues(),
                             PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignField,
                                        2,
                                        GetAssignField().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignvalue
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetAssignValue().GetExprText(), GetAssignValue().GetExprValues(),
                             PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignValue,
                                        3,
                                        GetAssignValue().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityAssignmentInd::ROC_DeleteField(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // assignment
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetAssignment().GetExprText();
    xtstring sValues = GetAssignment().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignment().SetExprText(sExpression);
      GetAssignment().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignfield
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAssignField().GetExprText();
    xtstring sValues = GetAssignField().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignField().SetExprText(sExpression);
      GetAssignField().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignvalue
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAssignValue().GetExprText();
    xtstring sValues = GetAssignValue().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignValue().SetExprText(sExpression);
      GetAssignValue().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityAssignmentInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteParam(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_DeleteParam(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_ParamAdded(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sName,
                                             CERModelParamInd* pParam,
                                             CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ParamAdded(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sName,
                                             CERModelParamInd* pParam,
                                             CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_ParamChanged(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sName,
                                               CERModelParamInd* pParam,
                                               CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ParamChanged(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sName,
                                               CERModelParamInd* pParam,
                                               CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                             const xtstring& sTableName,
                                                             const xtstring& sParentTableName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_RelationAdded(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sName,
                                                CERModelRelationInd* pRelation,
                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_RelationAdded(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sName,
                                                CERModelRelationInd* pRelation,
                                                CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_RelationChanged(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sParentTableName,
                                                  const xtstring& sName,
                                                  CERModelRelationInd* pRelation,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_RelationChanged(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sParentTableName,
                                                  const xtstring& sName,
                                                  CERModelRelationInd* pRelation,
                                                  CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteIFS(const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_DeleteIFS(const xtstring& sName,
                                            CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // entity
  if (GetEntityOfInformationSight() == sOldName)
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Entity,
                                        1,
                                        GetEntityOfInformationSight());
    cUsers.push_back(pResponseObject);
  }
  // assignment
  if (EntityExistInExpression(sOldName,
                              GetAssignment().GetExprText(), GetAssignment().GetExprValues(),
                              PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Assignment,
                                        2,
                                        GetAssignment().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignfield
  if (EntityExistInExpression(sOldName,
                              GetAssignField().GetExprText(), GetAssignField().GetExprValues(),
                              PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignField,
                                        3,
                                        GetAssignField().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignvalue
  if (EntityExistInExpression(sOldName,
                              GetAssignValue().GetExprText(), GetAssignValue().GetExprValues(),
                              PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignValue,
                                        4,
                                        GetAssignValue().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityAssignmentInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // entity
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetEntityOfInformationSight(sNewName);
    ROC_PropertyChanged_IFS();
  }
  // assignment
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAssignment().GetExprText();
    xtstring sValues = GetAssignment().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignment().SetExprText(sExpression);
      GetAssignment().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // assignfield
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAssignField().GetExprText();
    xtstring sValues = GetAssignField().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignField().SetExprText(sExpression);
      GetAssignField().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // assignvalue
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetAssignValue().GetExprText();
    xtstring sValues = GetAssignValue().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignValue().SetExprText(sExpression);
      GetAssignValue().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                           const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteEntity(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // entity
  if (GetEntityOfInformationSight() == sName)
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Entity,
                                        1,
                                        GetEntityOfInformationSight());
    cUsers.push_back(pResponseObject);
  }
  // assignment
  if (EntityExistInExpression(sName,
                              GetAssignment().GetExprText(), GetAssignment().GetExprValues(),
                              PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Assignment,
                                        2,
                                        GetAssignment().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignfield
  if (EntityExistInExpression(sName,
                              GetAssignField().GetExprText(), GetAssignField().GetExprValues(),
                              PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignField,
                                        3,
                                        GetAssignField().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignvalue
  if (EntityExistInExpression(sName,
                              GetAssignValue().GetExprText(), GetAssignValue().GetExprValues(),
                              PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignValue,
                                        4,
                                        GetAssignValue().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityAssignmentInd::ROC_DeleteEntity(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // entity
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetEntityOfInformationSight(_XT(""));
    ROC_PropertyChanged_IFS();
  }
  // assignment
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAssignment().GetExprText();
    xtstring sValues = GetAssignment().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignment().SetExprText(sExpression);
      GetAssignment().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // assignfield
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAssignField().GetExprText();
    xtstring sValues = GetAssignField().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignField().SetExprText(sExpression);
      GetAssignField().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // assignvalue
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetAssignValue().GetExprText();
    xtstring sValues = GetAssignValue().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignValue().SetExprText(sExpression);
      GetAssignValue().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityAssignmentInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // assignment
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetAssignment().GetExprText(), GetAssignment().GetExprValues(),
                                PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Assignment,
                                        1,
                                        GetAssignment().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignfield
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetAssignField().GetExprText(), GetAssignField().GetExprValues(),
                                PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignField,
                                        2,
                                        GetAssignField().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignvalue
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetAssignValue().GetExprText(), GetAssignValue().GetExprValues(),
                                PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignValue,
                                        3,
                                        GetAssignValue().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityAssignmentInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // assignment
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetAssignment().GetExprText();
    xtstring sValues = GetAssignment().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignment().SetExprText(sExpression);
      GetAssignment().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignfield
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAssignField().GetExprText();
    xtstring sValues = GetAssignField().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignField().SetExprText(sExpression);
      GetAssignField().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignvalue
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAssignValue().GetExprText();
    xtstring sValues = GetAssignValue().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignValue().SetExprText(sExpression);
      GetAssignValue().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                             const xtstring& sEntityName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteVariable(const xtstring& sIFS,
                                                 const xtstring& sEntityName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // assignment
  if (VariableExistInExpression(sEntityName, sName,
                                GetAssignment().GetExprText(), GetAssignment().GetExprValues(),
                                PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_Assignment,
                                        1,
                                        GetAssignment().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignfield
  if (VariableExistInExpression(sEntityName, sName,
                                GetAssignField().GetExprText(), GetAssignField().GetExprValues(),
                                PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignField,
                                        2,
                                        GetAssignField().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // assignvalue
  if (VariableExistInExpression(sEntityName, sName,
                                GetAssignValue().GetExprText(), GetAssignValue().GetExprValues(),
                                PositionsE, PositionsV))
  {
    xtstring sShowName = GetOwnerEntity()->GetIFSAssignmentIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityAssignment_AssignValue,
                                        3,
                                        GetAssignValue().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityAssignmentInd::ROC_DeleteVariable(const xtstring& sIFS,
                                                 const xtstring& sEntityName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // assignment
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetAssignment().GetExprText();
    xtstring sValues = GetAssignment().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignment().SetExprText(sExpression);
      GetAssignment().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignfield
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAssignField().GetExprText();
    xtstring sValues = GetAssignField().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignField().SetExprText(sExpression);
      GetAssignField().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // assignvalue
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAssignValue().GetExprText();
    xtstring sValues = GetAssignValue().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAssignValue().SetExprText(sExpression);
      GetAssignValue().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityAssignmentInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObject* pObject)
{
}
void CIFSEntityAssignmentInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                                  const xtstring& sName,
                                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CIFSEntityAssignmentInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityAssignmentInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObject* pObject)
{
}



























/////////////////////////////////////////////////////////////////////////////
// CIFSEntityPrePostProcessInd
/////////////////////////////////////////////////////////////////////////////
void CIFSEntityPrePostProcessInd::ROC_PropertyChanged_Proj(){}
void CIFSEntityPrePostProcessInd::ROC_PropertyChanged_ER(){}
void CIFSEntityPrePostProcessInd::ROC_PropertyChanged_IFS(){}
void CIFSEntityPrePostProcessInd::ROC_PropertyChanged_WP(){}
void CIFSEntityPrePostProcessInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                         const xtstring& sNewName,
                                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (ProjConstantExistInExpression(sOldName,
                                    GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                         const xtstring& sNewName,
                                                         CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // visibility
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (ProjConstantExistInExpression(sName,
                                    GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                         CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // visibility
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                           const xtstring& sNewName,
                                                           CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                           const xtstring& sNewName,
                                                           CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                                       CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                           CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                        const xtstring& sNewName,
                                                        CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                        const xtstring& sNewName,
                                                        CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteERModel(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_DeleteERModel(const xtstring& sName,
                                                    CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                              const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteTable(const xtstring& sERModelName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_DeleteTable(const xtstring& sERModelName,
                                                  const xtstring& sName,
                                                  CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sOldName,
                             GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                             PositionsE, PositionsV))
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // visibility
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                              const xtstring& sTableName,
                                                              const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteField(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (FieldExistInExpression(SE_MF_GetModuleName(), sERModelName, sTableName, sName,
                             GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                             PositionsE, PositionsV))
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_DeleteField(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // visibility
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                              const xtstring& sTableName,
                                                              const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteParam(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_DeleteParam(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ParamAdded(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CERModelParamInd* pParam,
                                                 CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ParamAdded(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CERModelParamInd* pParam,
                                                 CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ParamChanged(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sName,
                                                   CERModelParamInd* pParam,
                                                   CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ParamChanged(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sName,
                                                   CERModelParamInd* pParam,
                                                   CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                         const xtstring& sTableName,
                                                         const xtstring& sParentTableName,
                                                         const xtstring& sOldName,
                                                         const xtstring& sNewName,
                                                         CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                         const xtstring& sTableName,
                                                         const xtstring& sParentTableName,
                                                         const xtstring& sOldName,
                                                         const xtstring& sNewName,
                                                         CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                                 const xtstring& sTableName,
                                                                 const xtstring& sParentTableName,
                                                                 const xtstring& sName,
                                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sName,
                                                     CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_RelationAdded(const xtstring& sERModelName,
                                                    const xtstring& sTableName,
                                                    const xtstring& sParentTableName,
                                                    const xtstring& sName,
                                                    CERModelRelationInd* pRelation,
                                                    CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_RelationAdded(const xtstring& sERModelName,
                                                    const xtstring& sTableName,
                                                    const xtstring& sParentTableName,
                                                    const xtstring& sName,
                                                    CERModelRelationInd* pRelation,
                                                    CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_RelationChanged(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sParentTableName,
                                                      const xtstring& sName,
                                                      CERModelRelationInd* pRelation,
                                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_RelationChanged(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sParentTableName,
                                                      const xtstring& sName,
                                                      CERModelRelationInd* pRelation,
                                                      CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteIFS(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_DeleteIFS(const xtstring& sName,
                                                CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                                       const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (EntityExistInExpression(sOldName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                                       const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // visibility
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                               const xtstring& sName,
                                                               CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteEntity(const xtstring& sIFS,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (EntityExistInExpression(sName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_DeleteEntity(const xtstring& sIFS,
                                                   const xtstring& sName,
                                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // visibility
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                                         const xtstring& sEntityName,
                                                         const xtstring& sOldName,
                                                         const xtstring& sNewName,
                                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                PositionsE, PositionsV))
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                                         const xtstring& sEntityName,
                                                         const xtstring& sOldName,
                                                         const xtstring& sNewName,
                                                         CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // visibility
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                                 const xtstring& sEntityName,
                                                                 const xtstring& sName,
                                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteVariable(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (VariableExistInExpression(sEntityName, sName,
                                GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                PositionsE, PositionsV))
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_DeleteVariable(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // visibility
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetVisibility().GetExprText();
    xtstring sValues = GetVisibility().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                              const xtstring& sOldName,
                                                              const xtstring& sNewName,
                                                              CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                              const xtstring& sOldName,
                                                              const xtstring& sNewName,
                                                              CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                                      const xtstring& sName,
                                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                          const xtstring& sName,
                                                          CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeWPName(const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // property stone
  if (GetPropertyStone() == sOldName)
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_PropertyStone,
                                        1,
                                        GetPropertyStone());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeWPName(const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // property stone
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetPropertyStone(sNewName);
    ROC_PropertyChanged_WP();
  }
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForWP(const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteWP(sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteWP(const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // property stone
  if (GetPropertyStone() == sName)
  {
    xtstring sShowName;
    if (m_bPreProcessObject)
      sShowName = GetOwnerEntity()->GetPreProcessIndIndexedName(GetName());
    else
      sShowName = GetOwnerEntity()->GetPostProcessIndIndexedName(GetName());
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       sShowName,
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntityPrePostProcess_PropertyStone,
                                        1,
                                        GetPropertyStone());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityPrePostProcessInd::ROC_DeleteWP(const xtstring& sName,
                                               CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // property stone
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetPropertyStone(_XT(""));
    ROC_PropertyChanged_WP();
  }
}
void CIFSEntityPrePostProcessInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
                                                           const xtstring& sOldName,
                                                           const xtstring& sNewName,
                                                           CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
                                                           const xtstring& sOldName,
                                                           const xtstring& sNewName,
                                                           CUserOfObject* pObject)
{
}
void CIFSEntityPrePostProcessInd::ROC_CheckReferencesForWPConstant(const xtstring& sWPName,
                                                                   const xtstring& sName,
                                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteWPConstant(sWPName, sName, cUsers);
}
void CIFSEntityPrePostProcessInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                                       const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CIFSEntityPrePostProcessInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                                       const xtstring& sName,
                                                       CUserOfObject* pObject)
{
}












































/////////////////////////////////////////////////////////////////////////////
// CIFSEntityConditionVariations
/////////////////////////////////////////////////////////////////////////////
void CIFSEntityConditionVariations::ROC_ChangeIFSName(const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CIFSEntityConditionVariations::ROC_ChangeIFSName(const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObject* pObject)
{
}
void CIFSEntityConditionVariations::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CIFSEntityConditionVariations::ROC_DeleteIFS(const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CIFSEntityConditionVariations::ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject)
{
}
void CIFSEntityConditionVariations::ROC_ChangeEntityName(const xtstring& sIFS,
                                                         const xtstring& sOldName,
                                                         const xtstring& sNewName,
                                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // determine via entity and variable
  if (sOldName == GetDetermineViaEntity())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_DetermineCondition_ViaEntity,
                                        1,
                                        GetDetermineViaEntity());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityConditionVariations::ROC_ChangeEntityName(const xtstring& sIFS,
                                                         const xtstring& sOldName,
                                                         const xtstring& sNewName,
                                                         CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // determine via entity and variable
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDetermineViaEntity(sNewName);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntityConditionVariations::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                                 const xtstring& sName,
                                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CIFSEntityConditionVariations::ROC_DeleteEntity(const xtstring& sIFS,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // determine via entity and variable
  if (sName == GetDetermineViaEntity())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_DetermineCondition_ViaEntity,
                                        1,
                                        GetDetermineViaEntity());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityConditionVariations::ROC_DeleteEntity(const xtstring& sIFS,
                                                     const xtstring& sName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // determine via entity and variable
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDetermineViaEntity(_XT(""));
    SetDetermineViaVariable(_XT(""));
    SetUseDetermineViaEntityAndVariable(false);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntityConditionVariations::ROC_ChangeVariableName(const xtstring& sIFS,
                                                           const xtstring& sEntityName,
                                                           const xtstring& sOldName,
                                                           const xtstring& sNewName,
                                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // determine via entity and variable
  if (sEntityName == GetDetermineViaEntity() && sOldName == GetDetermineViaVariable())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_DetermineCondition_ViaVariable,
                                        1,
                                        GetDetermineViaVariable());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityConditionVariations::ROC_ChangeVariableName(const xtstring& sIFS,
                                                           const xtstring& sEntityName,
                                                           const xtstring& sOldName,
                                                           const xtstring& sNewName,
                                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // determine via entity and variable
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDetermineViaVariable(sNewName);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntityConditionVariations::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                                   const xtstring& sEntityName,
                                                                   const xtstring& sName,
                                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CIFSEntityConditionVariations::ROC_DeleteVariable(const xtstring& sIFS,
                                                       const xtstring& sEntityName,
                                                       const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // determine via entity and variable
  if (sEntityName == GetDetermineViaEntity() && sName == GetDetermineViaVariable())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_DetermineCondition_ViaVariable,
                                        1,
                                        GetDetermineViaVariable());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntityConditionVariations::ROC_DeleteVariable(const xtstring& sIFS,
                                                       const xtstring& sEntityName,
                                                       const xtstring& sName,
                                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // determine via entity and variable
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDetermineViaEntity(_XT(""));
    SetDetermineViaVariable(_XT(""));
    SetUseDetermineViaEntityAndVariable(false);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntityConditionVariations::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                                const xtstring& sOldName,
                                                                const xtstring& sNewName,
                                                                CUserOfObjectArray& cUsers)
{
}
void CIFSEntityConditionVariations::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                                const xtstring& sOldName,
                                                                const xtstring& sNewName,
                                                                CUserOfObject* pObject)
{
}
void CIFSEntityConditionVariations::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                                        const xtstring& sName,
                                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CIFSEntityConditionVariations::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                            const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
}
void CIFSEntityConditionVariations::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                            const xtstring& sName,
                                                            CUserOfObject* pObject)
{
}



























/////////////////////////////////////////////////////////////////////////////
// CIFSEntitySQLCommandVariations
/////////////////////////////////////////////////////////////////////////////
void CIFSEntitySQLCommandVariations::ROC_ChangeIFSName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CIFSEntitySQLCommandVariations::ROC_ChangeIFSName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObject* pObject)
{
}
void CIFSEntitySQLCommandVariations::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                               CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CIFSEntitySQLCommandVariations::ROC_DeleteIFS(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
}
void CIFSEntitySQLCommandVariations::ROC_DeleteIFS(const xtstring& sName,
                                                   CUserOfObject* pObject)
{
}
void CIFSEntitySQLCommandVariations::ROC_ChangeEntityName(const xtstring& sIFS,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // determine via entity and variable
  if (sOldName == GetDetermineViaEntity())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_DetermineSQLQuery_ViaEntity,
                                        1,
                                        GetDetermineViaEntity());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntitySQLCommandVariations::ROC_ChangeEntityName(const xtstring& sIFS,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // determine via entity and variable
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDetermineViaEntity(sNewName);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntitySQLCommandVariations::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                                  const xtstring& sName,
                                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CIFSEntitySQLCommandVariations::ROC_DeleteEntity(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // determine via entity and variable
  if (sName == GetDetermineViaEntity())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_DetermineSQLQuery_ViaEntity,
                                        1,
                                        GetDetermineViaEntity());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntitySQLCommandVariations::ROC_DeleteEntity(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // determine via entity and variable
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDetermineViaEntity(_XT(""));
    SetDetermineViaVariable(_XT(""));
    SetUseDetermineViaEntityAndVariable(false);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntitySQLCommandVariations::ROC_ChangeVariableName(const xtstring& sIFS,
                                                            const xtstring& sEntityName,
                                                            const xtstring& sOldName,
                                                            const xtstring& sNewName,
                                                            CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // determine via entity and variable
  if (sEntityName == GetDetermineViaEntity() && sOldName == GetDetermineViaVariable())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_DetermineSQLQuery_ViaVariable,
                                        1,
                                        GetDetermineViaVariable());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntitySQLCommandVariations::ROC_ChangeVariableName(const xtstring& sIFS,
                                                            const xtstring& sEntityName,
                                                            const xtstring& sOldName,
                                                            const xtstring& sNewName,
                                                            CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // determine via entity and variable
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDetermineViaVariable(sNewName);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntitySQLCommandVariations::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                                    const xtstring& sEntityName,
                                                                    const xtstring& sName,
                                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CIFSEntitySQLCommandVariations::ROC_DeleteVariable(const xtstring& sIFS,
                                                        const xtstring& sEntityName,
                                                        const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (SE_MF_GetModuleName() != sIFS)
    return;

  // determine via entity and variable
  if (sEntityName == GetDetermineViaEntity() && sName == GetDetermineViaVariable())
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_IFSModule,
                                                       SE_MF_GetModuleName(),
                                                       GetOwnerEntityName(),
                                                       GetOwnerEntityName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_IFSEntity_DetermineSQLQuery_ViaVariable,
                                        1,
                                        GetDetermineViaVariable());
    cUsers.push_back(pResponseObject);
  }
}
void CIFSEntitySQLCommandVariations::ROC_DeleteVariable(const xtstring& sIFS,
                                                        const xtstring& sEntityName,
                                                        const xtstring& sName,
                                                        CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // determine via entity and variable
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetDetermineViaEntity(_XT(""));
    SetDetermineViaVariable(_XT(""));
    SetUseDetermineViaEntityAndVariable(false);
    ROC_PropertyChanged_IFS();
  }
}
void CIFSEntitySQLCommandVariations::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                                 const xtstring& sOldName,
                                                                 const xtstring& sNewName,
                                                                 CUserOfObjectArray& cUsers)
{
}
void CIFSEntitySQLCommandVariations::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                                 const xtstring& sOldName,
                                                                 const xtstring& sNewName,
                                                                 CUserOfObject* pObject)
{
}
void CIFSEntitySQLCommandVariations::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                                         const xtstring& sName,
                                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CIFSEntitySQLCommandVariations::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
}
void CIFSEntitySQLCommandVariations::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                             const xtstring& sName,
                                                             CUserOfObject* pObject)
{
}
