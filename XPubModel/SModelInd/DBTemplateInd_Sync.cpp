// DBTemplateInd_Sync.cpp: implementation of the CDBTemplateInd classes.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"

#include "ModelInd.h"
#include "DBTemplateInd.h"

#include "MPModelFieldsInd.h"
#include "MPModelTemplateInd.h"

#include "SSyncEng/SyncGroups.h"


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CDBTemplateTableInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CDBTemplateTableInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
void CDBTemplateTableInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteProjConstant(const xtstring& sName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_DeleteProjConstant(const xtstring& sName,
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
void CDBTemplateTableInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                               CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                   CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteERModel(const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_DeleteERModel(const xtstring& sName,
                                            CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // table name
  if (GetTableName() == sOldName
      && m_pParent->GetERModelName() == sERModelName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_TableName,
                                        1,
                                        GetTableName());
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_ChangeTableName(const xtstring& sERModelName,
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
void CDBTemplateTableInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteTable(const xtstring& sERModelName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
  // das ist in CDBTemplateInd erledigt
}
void CDBTemplateTableInd::ROC_DeleteTable(const xtstring& sERModelName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
  // das ist in CDBTemplateInd erledigt
}
void CDBTemplateTableInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
  if (m_pParent && m_pParent->m_pMPTemplate)
    sIFSModul = m_pParent->m_pMPTemplate->GetInformationSight();
  if (FieldExistInExpression(sIFSModul, sERModelName, sTableName, sOldName,
                             m_caValue.GetExprText(), m_caValue.GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
}
void CDBTemplateTableInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteField(const xtstring& sERModelName,
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
  if (m_pParent && m_pParent->m_pMPTemplate)
    sIFSModul = m_pParent->m_pMPTemplate->GetInformationSight();
  if (FieldExistInExpression(sIFSModul, sERModelName, sTableName, sName,
                             m_caValue.GetExprText(), m_caValue.GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_DeleteField(const xtstring& sERModelName,
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
}
void CDBTemplateTableInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteParam(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_DeleteParam(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_ParamAdded(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sName,
                                         CERModelParamInd* pParam,
                                         CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_ParamAdded(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sName,
                                         CERModelParamInd* pParam,
                                         CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_ParamChanged(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sName,
                                           CERModelParamInd* pParam,
                                           CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_ParamChanged(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sName,
                                           CERModelParamInd* pParam,
                                           CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                         const xtstring& sTableName,
                                                         const xtstring& sParentTableName,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sName,
                                             CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sName,
                                             CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_RelationAdded(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sName,
                                            CERModelRelationInd* pRelation,
                                            CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_RelationAdded(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sName,
                                            CERModelRelationInd* pRelation,
                                            CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_RelationChanged(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sName,
                                              CERModelRelationInd* pRelation,
                                              CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_RelationChanged(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sName,
                                              CERModelRelationInd* pRelation,
                                              CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteIFS(const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_DeleteIFS(const xtstring& sName,
                                        CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                               const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pParent
      && m_pParent->m_pMPTemplate
      && m_pParent->m_pMPTemplate->GetInformationSight().size() != 0
      && m_pParent->m_pMPTemplate->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (EntityExistInExpression(sOldName,
                              m_caValue.GetExprText(), m_caValue.GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
void CDBTemplateTableInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                       const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteEntity(const xtstring& sIFS,
                                           const xtstring& sName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pParent
      && m_pParent->m_pMPTemplate
      && m_pParent->m_pMPTemplate->GetInformationSight().size() != 0
      && m_pParent->m_pMPTemplate->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (EntityExistInExpression(sName,
                              m_caValue.GetExprText(), m_caValue.GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_DeleteEntity(const xtstring& sIFS,
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
void CDBTemplateTableInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                                 const xtstring& sEntityName,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pParent
      && m_pParent->m_pMPTemplate
      && m_pParent->m_pMPTemplate->GetInformationSight().size() != 0
      && m_pParent->m_pMPTemplate->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (VariableExistInExpression(sEntityName, sOldName,
                                m_caValue.GetExprText(), m_caValue.GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
void CDBTemplateTableInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                         const xtstring& sEntityName,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteVariable(const xtstring& sIFS,
                                             const xtstring& sEntityName,
                                             const xtstring& sName,
                                             CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pParent
      && m_pParent->m_pMPTemplate
      && m_pParent->m_pMPTemplate->GetInformationSight().size() != 0
      && m_pParent->m_pMPTemplate->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // value
  if (VariableExistInExpression(sEntityName, sName,
                                m_caValue.GetExprText(), m_caValue.GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetTableName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_Value,
                                        1,
                                        m_caValue.GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateTableInd::ROC_DeleteVariable(const xtstring& sIFS,
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
void CDBTemplateTableInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObject* pObject)
{
}
void CDBTemplateTableInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                              const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CDBTemplateTableInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CDBTemplateTableInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                  const xtstring& sName,
                                                  CUserOfObject* pObject)
{
}














//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CDBTemplateInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CDBTemplateInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er model name
  if (GetERModelName() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       SE_MF_GetModuleName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplate_ERModel,
                                        1,
                                        GetERModelName());
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateInd::ROC_ChangeERModelName(const xtstring& sOldName,
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
void CDBTemplateInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CDBTemplateInd::ROC_DeleteERModel(const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // er model name
  if (GetERModelName() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       SE_MF_GetModuleName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplate_ERModel,
                                        1,
                                        GetERModelName());
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateInd::ROC_DeleteERModel(const xtstring& sName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // er model name
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetERModelName(_XT(""));
    RemoveAllTables();
    ROC_PropertyChanged_ER();
  }
}
void CDBTemplateInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
  // das ist in CDBTemplateTableInd erledigt
}
void CDBTemplateInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
  // das ist in CDBTemplateTableInd erledigt
}
void CDBTemplateInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CDBTemplateInd::ROC_DeleteTable(const xtstring& sERModelName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // my tables
  if (GetERModelName() == sERModelName
      && GetTable(sName) != NULL)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       SE_MF_GetModuleName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_DBTemplateTable_TableName,
                                        1,
                                        sName);
    cUsers.push_back(pResponseObject);
  }
}
void CDBTemplateInd::ROC_DeleteTable(const xtstring& sERModelName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // my tables
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    RemoveTable(sName);
  }
}
void CDBTemplateInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CDBTemplateInd::ROC_DeleteField(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_DeleteField(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CDBTemplateInd::ROC_DeleteParam(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_DeleteParam(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_ParamAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CERModelParamInd* pParam,
                                    CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_ParamAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CERModelParamInd* pParam,
                                    CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_ParamChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_ParamChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                    const xtstring& sTableName,
                                                    const xtstring& sParentTableName,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CDBTemplateInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_RelationAdded(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CERModelRelationInd* pRelation,
                                       CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_RelationAdded(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CERModelRelationInd* pRelation,
                                       CUserOfObject* pObject)
{
}
void CDBTemplateInd::ROC_RelationChanged(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObjectArray& cUsers)
{
}
void CDBTemplateInd::ROC_RelationChanged(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObject* pObject)
{
}
