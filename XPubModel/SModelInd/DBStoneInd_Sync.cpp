// DBStoneInd_Sync.cpp: implementation of the CDBStoneInd classes.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"

#include "ModelInd.h"
#include "DBStoneInd.h"

#include "MPModelFieldsInd.h"
#include "MPModelStoneInd.h"

#include "SSyncEng/SyncGroups.h"



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CDBStoneFieldInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CDBStoneFieldInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (ProjConstantExistInExpression(sOldName,
                                    m_caValue.GetExprText(), m_caValue.GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetFieldName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneFieldInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // value
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    xtstring sExpression = m_caValue.GetExprText();
    xtstring sValues = m_caValue.GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      m_caValue.SetExprText(sExpression);
      m_caValue.SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CDBStoneFieldInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteProjConstant(const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (ProjConstantExistInExpression(sName,
                                    m_caValue.GetExprText(), m_caValue.GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetFieldName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneFieldInd::ROC_DeleteProjConstant(const xtstring& sName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // value
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    xtstring sExpression = m_caValue.GetExprText();
    xtstring sValues = m_caValue.GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      m_caValue.SetExprText(sExpression);
      m_caValue.SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CDBStoneFieldInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteERModel(const xtstring& sName,
                                         CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_DeleteERModel(const xtstring& sName,
                                         CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteTable(const xtstring& sERModelName,
                                       const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_DeleteTable(const xtstring& sERModelName,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  xtstring sIFSModul;
  if (m_pParent && m_pParent->m_pMPStone)
    sIFSModul = m_pParent->m_pMPStone->GetInformationSight();
  if (FieldExistInExpression(sIFSModul, sERModelName, sTableName, sOldName,
                             m_caValue.GetExprText(), m_caValue.GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetFieldName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // field name
  if (GetFieldName() == sOldName
      && m_pParent->GetERModelName() == sERModelName
      && m_pParent->GetTableName() == sTableName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetFieldName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_FieldName,
                                        2,
                                        GetFieldName());
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneFieldInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // value
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    xtstring sExpression = m_caValue.GetExprText();
    xtstring sValues = m_caValue.GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      m_caValue.SetExprText(sExpression);
      m_caValue.SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // field name
  if (pObject->m_nIdentify == 2 && pObject->GetChange())
  {
    SetFieldName(sNewName);
    ROC_PropertyChanged_ER();
  }
}
void CDBStoneFieldInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteField(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  xtstring sIFSModul;
  if (m_pParent && m_pParent->m_pMPStone)
    sIFSModul = m_pParent->m_pMPStone->GetInformationSight();
  if (FieldExistInExpression(sIFSModul, sERModelName, sTableName, sName,
                             m_caValue.GetExprText(), m_caValue.GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetFieldName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // field name
  // das ist in CDBStoneInd erledigt
}
void CDBStoneFieldInd::ROC_DeleteField(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // value
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    xtstring sExpression = m_caValue.GetExprText();
    xtstring sValues = m_caValue.GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      m_caValue.SetExprText(sExpression);
      m_caValue.SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // field name
  // das ist in CDBStoneInd erledigt
}
void CDBStoneFieldInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteParam(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_DeleteParam(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_ParamAdded(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ParamAdded(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_ParamChanged(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sName,
                                        CERModelParamInd* pParam,
                                        CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ParamChanged(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sName,
                                        CERModelParamInd* pParam,
                                        CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sParentTableName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_RelationAdded(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_RelationAdded(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_RelationChanged(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParentTableName,
                                           const xtstring& sName,
                                           CERModelRelationInd* pRelation,
                                           CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_RelationChanged(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParentTableName,
                                           const xtstring& sName,
                                           CERModelRelationInd* pRelation,
                                           CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteIFS(const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_DeleteIFS(const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pParent
      && m_pParent->m_pMPStone
      && m_pParent->m_pMPStone->GetInformationSight().size() != 0
      && m_pParent->m_pMPStone->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (EntityExistInExpression(sOldName,
                              m_caValue.GetExprText(), m_caValue.GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetFieldName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneFieldInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // value
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    xtstring sExpression = m_caValue.GetExprText();
    xtstring sValues = m_caValue.GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      m_caValue.SetExprText(sExpression);
      m_caValue.SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CDBStoneFieldInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteEntity(const xtstring& sIFS,
                                        const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pParent
      && m_pParent->m_pMPStone
      && m_pParent->m_pMPStone->GetInformationSight().size() != 0
      && m_pParent->m_pMPStone->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (EntityExistInExpression(sName,
                              m_caValue.GetExprText(), m_caValue.GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       m_caValue.GetExprText(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneFieldInd::ROC_DeleteEntity(const xtstring& sIFS,
                                        const xtstring& sName,
                                        CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // value
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    xtstring sExpression = m_caValue.GetExprText();
    xtstring sValues = m_caValue.GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      m_caValue.SetExprText(sExpression);
      m_caValue.SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CDBStoneFieldInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                              const xtstring& sEntityName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pParent
      && m_pParent->m_pMPStone
      && m_pParent->m_pMPStone->GetInformationSight().size() != 0
      && m_pParent->m_pMPStone->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (VariableExistInExpression(sEntityName, sOldName,
                                m_caValue.GetExprText(), m_caValue.GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetFieldName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneFieldInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                              const xtstring& sEntityName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // value
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    xtstring sExpression = m_caValue.GetExprText();
    xtstring sValues = m_caValue.GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      m_caValue.SetExprText(sExpression);
      m_caValue.SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CDBStoneFieldInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                      const xtstring& sEntityName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteVariable(const xtstring& sIFS,
                                          const xtstring& sEntityName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pParent
      && m_pParent->m_pMPStone
      && m_pParent->m_pMPStone->GetInformationSight().size() != 0
      && m_pParent->m_pMPStone->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (VariableExistInExpression(sEntityName, sName,
                                m_caValue.GetExprText(), m_caValue.GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetFieldName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneFieldInd::ROC_DeleteVariable(const xtstring& sIFS,
                                          const xtstring& sEntityName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // value
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    xtstring sExpression = m_caValue.GetExprText();
    xtstring sValues = m_caValue.GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      m_caValue.SetExprText(sExpression);
      m_caValue.SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CDBStoneFieldInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
}
void CDBStoneFieldInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                           const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CDBStoneFieldInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
}
void CDBStoneFieldInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObject* pObject)
{
}














//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CDBStoneInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CDBStoneInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er model name
  if (GetERModelName() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       SE_MF_GetModuleName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStone_ERModel,
                                        1,
                                        GetERModelName());
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // er model name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERModelName(sNewName);
    ROC_PropertyChanged_ER();
  }
}
void CDBStoneInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CDBStoneInd::ROC_DeleteERModel(const xtstring& sName,
                                    CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er model name
  if (GetERModelName() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       SE_MF_GetModuleName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStone_ERModel,
                                        1,
                                        GetERModelName());
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneInd::ROC_DeleteERModel(const xtstring& sName,
                                    CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // er model name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERModelName(_XT(""));
    SetTableName(_XT(""));
    RemoveAllFields();
    ROC_PropertyChanged_ER();
  }
}
void CDBStoneInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                      const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetERModelName() != sERModelName)
    return;

  // table name
  if (GetTableName() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       SE_MF_GetModuleName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStone_ERTable,
                                        1,
                                        GetTableName());
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                      const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // table name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetTableName(sNewName);
    ROC_PropertyChanged_ER();
  }
}
void CDBStoneInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CDBStoneInd::ROC_DeleteTable(const xtstring& sERModelName,
                                  const xtstring& sName,
                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetERModelName() != sERModelName)
    return;

  // table name
  if (GetTableName() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       SE_MF_GetModuleName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStone_ERTable,
                                        1,
                                        GetTableName());
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneInd::ROC_DeleteTable(const xtstring& sERModelName,
                                  const xtstring& sName,
                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // table name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetTableName(_XT(""));
    RemoveAllFields();
    ROC_PropertyChanged_ER();
  }
}
void CDBStoneInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObjectArray& cUsers)
{
  // das ist in CDBStoneFieldInd erledigt
}
void CDBStoneInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObject* pObject)
{
  // das ist in CDBStoneFieldInd erledigt
}
void CDBStoneInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CDBStoneInd::ROC_DeleteField(const xtstring& sERModelName,
                                  const xtstring& sTableName,
                                  const xtstring& sName,
                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetERModelName() != sERModelName || GetTableName() != sTableName)
    return;

  // my fields
  if (GetField(sName) != NULL)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       SE_MF_GetModuleName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBStoneField_FieldName,
                                        1,
                                        sName);
    cUsers.push_back(pResponseObject);
  }
}
void CDBStoneInd::ROC_DeleteField(const xtstring& sERModelName,
                                  const xtstring& sTableName,
                                  const xtstring& sName,
                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // my fields
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    RemoveField(sName);
    ROC_PropertyChanged_ER();
  }
}
void CDBStoneInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObjectArray& cUsers)
{
}
void CDBStoneInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sOldName,
                                      const xtstring& sNewName,
                                      CUserOfObject* pObject)
{
}
void CDBStoneInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CDBStoneInd::ROC_DeleteParam(const xtstring& sERModelName,
                                  const xtstring& sTableName,
                                  const xtstring& sName,
                                  CUserOfObjectArray& cUsers)
{
}
void CDBStoneInd::ROC_DeleteParam(const xtstring& sERModelName,
                                  const xtstring& sTableName,
                                  const xtstring& sName,
                                  CUserOfObject* pObject)
{
}
void CDBStoneInd::ROC_ParamAdded(const xtstring& sERModelName,
                                 const xtstring& sTableName,
                                 const xtstring& sName,
                                 CERModelParamInd* pParam,
                                 CUserOfObjectArray& cUsers)
{
}
void CDBStoneInd::ROC_ParamAdded(const xtstring& sERModelName,
                                 const xtstring& sTableName,
                                 const xtstring& sName,
                                 CERModelParamInd* pParam,
                                 CUserOfObject* pObject)
{
}
void CDBStoneInd::ROC_ParamChanged(const xtstring& sERModelName,
                                   const xtstring& sTableName,
                                   const xtstring& sName,
                                   CERModelParamInd* pParam,
                                   CUserOfObjectArray& cUsers)
{
}
void CDBStoneInd::ROC_ParamChanged(const xtstring& sERModelName,
                                   const xtstring& sTableName,
                                   const xtstring& sName,
                                   CERModelParamInd* pParam,
                                   CUserOfObject* pObject)
{
}
void CDBStoneInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CDBStoneInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CDBStoneInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CDBStoneInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sParentTableName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CDBStoneInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sParentTableName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CDBStoneInd::ROC_RelationAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sParentTableName,
                                    const xtstring& sName,
                                    CERModelRelationInd* pRelation,
                                    CUserOfObjectArray& cUsers)
{
}
void CDBStoneInd::ROC_RelationAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sParentTableName,
                                    const xtstring& sName,
                                    CERModelRelationInd* pRelation,
                                    CUserOfObject* pObject)
{
}
void CDBStoneInd::ROC_RelationChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sParentTableName,
                                      const xtstring& sName,
                                      CERModelRelationInd* pRelation,
                                      CUserOfObjectArray& cUsers)
{
}
void CDBStoneInd::ROC_RelationChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sParentTableName,
                                      const xtstring& sName,
                                      CERModelRelationInd* pRelation,
                                      CUserOfObject* pObject)
{
}
