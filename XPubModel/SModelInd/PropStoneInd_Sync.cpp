// PropStoneInd_Sync.cpp: implementation of the CPropStoneInd classes.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "SSTLdef/STLdef.h"
#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"

#include "XPubVariant.h"
#include "ModelInd.h"
#include "PropStoneInd.h"

#include "MPModelFieldsInd.h"
#include "MPModelStoneInd.h"

#include "SSyncEng/SyncGroups.h"



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CPropStonePropertiesInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CPropStonePropertiesInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;

  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pProp = GetProperty(nI);
    if (!pProp)
      continue;
    // expression
    if (ProjConstantExistInExpression(sOldName,
                                      pProp->GetExpression().GetExprText(), pProp->GetExpression().GetExprValues(),
                                      PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                         SE_MF_GetModuleName(),
                                                         _XT(""),
                                                         pProp->GetInternalName(),
                                                         CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                         CSyncResponse_Proj::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_PropStone_Expression,
                                          nI,
                                          pProp->GetExpression().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CPropStonePropertiesInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // in pObject->m_nIdentify haben wir index der Eigenschaft
  assert(pObject->m_nIdentify < CountOfProperties());
  // expression
  if (pObject->m_nIdentify < CountOfProperties() && pObject->GetChange())
  {
    CPropStonePropertyInd* pProp = GetProperty(pObject->m_nIdentify);
    if (!pProp)
    {
      assert(false);
      return;
    }
    xtstring sExpression = pProp->GetExpression().GetExprText();
    xtstring sValues = pProp->GetExpression().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      pProp->GetExpression().SetExprText(sExpression);
      pProp->GetExpression().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CPropStonePropertiesInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;

  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pProp = GetProperty(nI);
    if (!pProp)
      continue;
    // expression
    if (ProjConstantExistInExpression(sName,
                                      pProp->GetExpression().GetExprText(), pProp->GetExpression().GetExprValues(),
                                      PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                         SE_MF_GetModuleName(),
                                                         _XT(""),
                                                         pProp->GetInternalName(),
                                                         CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                         CSyncResponse_Proj::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_PropStone_Expression,
                                          nI,
                                          pProp->GetExpression().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CPropStonePropertiesInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // in pObject->m_nIdentify haben wir index der Eigenschaft
  assert(pObject->m_nIdentify < CountOfProperties());
  // expression
  if (pObject->m_nIdentify < CountOfProperties() && pObject->GetChange())
  {
    CPropStonePropertyInd* pProp = GetProperty(pObject->m_nIdentify);
    if (!pProp)
    {
      assert(false);
      return;
    }
    xtstring sExpression = pProp->GetExpression().GetExprText();
    xtstring sValues = pProp->GetExpression().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      pProp->GetExpression().SetExprText(sExpression);
      pProp->GetExpression().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CPropStonePropertiesInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                       const xtstring& sNewName,
                                                       CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                       CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                    const xtstring& sNewName,
                                                    CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteERModel(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_DeleteERModel(const xtstring& sName,
                                                CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteTable(const xtstring& sERModelName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_DeleteTable(const xtstring& sERModelName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;

  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pProp = GetProperty(nI);
    if (!pProp)
      continue;
    // expression
    xtstring sIFSModul;
    if (m_pPropStoneParent && m_pPropStoneParent->m_pMPStone)
      sIFSModul = m_pPropStoneParent->m_pMPStone->GetInformationSight();
    if (FieldExistInExpression(sIFSModul, sERModelName, sTableName, sOldName,
                               pProp->GetExpression().GetExprText(), pProp->GetExpression().GetExprValues(),
                               PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                         SE_MF_GetModuleName(),
                                                         _XT(""),
                                                         pProp->GetInternalName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_PropStone_Expression,
                                          nI,
                                          pProp->GetExpression().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CPropStonePropertiesInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // in pObject->m_nIdentify haben wir index der Eigenschaft
  assert(pObject->m_nIdentify < CountOfProperties());
  // expression
  if (pObject->m_nIdentify < CountOfProperties() && pObject->GetChange())
  {
    CPropStonePropertyInd* pProp = GetProperty(pObject->m_nIdentify);
    if (!pProp)
    {
      assert(false);
      return;
    }
    xtstring sExpression = pProp->GetExpression().GetExprText();
    xtstring sValues = pProp->GetExpression().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      pProp->GetExpression().SetExprText(sExpression);
      pProp->GetExpression().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CPropStonePropertiesInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteField(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;

  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pProp = GetProperty(nI);
    if (!pProp)
      continue;
    // expression
    xtstring sIFSModul;
    if (m_pPropStoneParent && m_pPropStoneParent->m_pMPStone)
      sIFSModul = m_pPropStoneParent->m_pMPStone->GetInformationSight();
    if (FieldExistInExpression(sIFSModul, sERModelName, sTableName, sName,
                               pProp->GetExpression().GetExprText(), pProp->GetExpression().GetExprValues(),
                               PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                         SE_MF_GetModuleName(),
                                                         _XT(""),
                                                         pProp->GetInternalName(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_PropStone_Expression,
                                          nI,
                                          pProp->GetExpression().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CPropStonePropertiesInd::ROC_DeleteField(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // in pObject->m_nIdentify haben wir index der Eigenschaft
  assert(pObject->m_nIdentify < CountOfProperties());
  // expression
  if (pObject->m_nIdentify < CountOfProperties() && pObject->GetChange())
  {
    CPropStonePropertyInd* pProp = GetProperty(pObject->m_nIdentify);
    if (!pProp)
    {
      assert(false);
      return;
    }
    xtstring sExpression = pProp->GetExpression().GetExprText();
    xtstring sValues = pProp->GetExpression().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      pProp->GetExpression().SetExprText(sExpression);
      pProp->GetExpression().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CPropStonePropertiesInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                          const xtstring& sTableName,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteParam(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_DeleteParam(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_ParamAdded(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sName,
                                             CERModelParamInd* pParam,
                                             CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ParamAdded(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sName,
                                             CERModelParamInd* pParam,
                                             CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_ParamChanged(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sName,
                                               CERModelParamInd* pParam,
                                               CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ParamChanged(const xtstring& sERModelName,
                                               const xtstring& sTableName,
                                               const xtstring& sName,
                                               CERModelParamInd* pParam,
                                               CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                             const xtstring& sTableName,
                                                             const xtstring& sParentTableName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_RelationAdded(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sName,
                                                CERModelRelationInd* pRelation,
                                                CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_RelationAdded(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sName,
                                                CERModelRelationInd* pRelation,
                                                CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_RelationChanged(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sParentTableName,
                                                  const xtstring& sName,
                                                  CERModelRelationInd* pRelation,
                                                  CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_RelationChanged(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sParentTableName,
                                                  const xtstring& sName,
                                                  CERModelRelationInd* pRelation,
                                                  CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteIFS(const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_DeleteIFS(const xtstring& sName,
                                            CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pPropStoneParent
      && m_pPropStoneParent->m_pMPStone
      && m_pPropStoneParent->m_pMPStone->GetInformationSight().size() != 0
      && m_pPropStoneParent->m_pMPStone->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;

  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pProp = GetProperty(nI);
    if (!pProp)
      continue;
    // expression
    if (EntityExistInExpression(sOldName,
                                pProp->GetExpression().GetExprText(), pProp->GetExpression().GetExprValues(),
                                PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                         SE_MF_GetModuleName(),
                                                         _XT(""),
                                                         pProp->GetInternalName(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_PropStone_Expression,
                                          nI,
                                          pProp->GetExpression().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CPropStonePropertiesInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // in pObject->m_nIdentify haben wir index der Eigenschaft
  assert(pObject->m_nIdentify < CountOfProperties());
  // expression
  if (pObject->m_nIdentify < CountOfProperties() && pObject->GetChange())
  {
    CPropStonePropertyInd* pProp = GetProperty(pObject->m_nIdentify);
    if (!pProp)
    {
      assert(false);
      return;
    }
    xtstring sExpression = pProp->GetExpression().GetExprText();
    xtstring sValues = pProp->GetExpression().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      pProp->GetExpression().SetExprText(sExpression);
      pProp->GetExpression().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CPropStonePropertiesInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                           const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteEntity(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pPropStoneParent
      && m_pPropStoneParent->m_pMPStone
      && m_pPropStoneParent->m_pMPStone->GetInformationSight().size() != 0
      && m_pPropStoneParent->m_pMPStone->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;

  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pProp = GetProperty(nI);
    if (!pProp)
      continue;
    // expression
    if (EntityExistInExpression(sName,
                                pProp->GetExpression().GetExprText(), pProp->GetExpression().GetExprValues(),
                                PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                         SE_MF_GetModuleName(),
                                                         _XT(""),
                                                         pProp->GetInternalName(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_PropStone_Expression,
                                          nI,
                                          pProp->GetExpression().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CPropStonePropertiesInd::ROC_DeleteEntity(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // in pObject->m_nIdentify haben wir index der Eigenschaft
  assert(pObject->m_nIdentify < CountOfProperties());
  // expression
  if (pObject->m_nIdentify < CountOfProperties() && pObject->GetChange())
  {
    CPropStonePropertyInd* pProp = GetProperty(pObject->m_nIdentify);
    if (!pProp)
    {
      assert(false);
      return;
    }
    xtstring sExpression = pProp->GetExpression().GetExprText();
    xtstring sValues = pProp->GetExpression().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      pProp->GetExpression().SetExprText(sExpression);
      pProp->GetExpression().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CPropStonePropertiesInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pPropStoneParent
      && m_pPropStoneParent->m_pMPStone
      && m_pPropStoneParent->m_pMPStone->GetInformationSight().size() != 0
      && m_pPropStoneParent->m_pMPStone->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;

  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pProp = GetProperty(nI);
    if (!pProp)
      continue;
    // expression
    if (VariableExistInExpression(sEntityName, sOldName,
                                  pProp->GetExpression().GetExprText(), pProp->GetExpression().GetExprValues(),
                                  PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                         SE_MF_GetModuleName(),
                                                         _XT(""),
                                                         pProp->GetInternalName(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_PropStone_Expression,
                                          nI,
                                          pProp->GetExpression().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CPropStonePropertiesInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // in pObject->m_nIdentify haben wir index der Eigenschaft
  assert(pObject->m_nIdentify < CountOfProperties());
  // expression
  if (pObject->m_nIdentify < CountOfProperties() && pObject->GetChange())
  {
    CPropStonePropertyInd* pProp = GetProperty(pObject->m_nIdentify);
    if (!pProp)
    {
      assert(false);
      return;
    }
    xtstring sExpression = pProp->GetExpression().GetExprText();
    xtstring sValues = pProp->GetExpression().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      pProp->GetExpression().SetExprText(sExpression);
      pProp->GetExpression().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CPropStonePropertiesInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                             const xtstring& sEntityName,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteVariable(const xtstring& sIFS,
                                                 const xtstring& sEntityName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (m_pPropStoneParent
      && m_pPropStoneParent->m_pMPStone
      && m_pPropStoneParent->m_pMPStone->GetInformationSight().size() != 0
      && m_pPropStoneParent->m_pMPStone->GetInformationSight() != sIFS)
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;

  for (size_t nI = 0; nI < CountOfProperties(); nI++)
  {
    CPropStonePropertyInd* pProp = GetProperty(nI);
    if (!pProp)
      continue;
    // expression
    if (VariableExistInExpression(sEntityName, sName,
                                  pProp->GetExpression().GetExprText(), pProp->GetExpression().GetExprValues(),
                                  PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                         SE_MF_GetModuleName(),
                                                         _XT(""),
                                                         pProp->GetInternalName(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_PropStone_Expression,
                                          nI,
                                          pProp->GetExpression().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CPropStonePropertiesInd::ROC_DeleteVariable(const xtstring& sIFS,
                                                 const xtstring& sEntityName,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // in pObject->m_nIdentify haben wir index der Eigenschaft
  assert(pObject->m_nIdentify < CountOfProperties());
  // expression
  if (pObject->m_nIdentify < CountOfProperties() && pObject->GetChange())
  {
    CPropStonePropertyInd* pProp = GetProperty(pObject->m_nIdentify);
    if (!pProp)
    {
      assert(false);
      return;
    }
    xtstring sExpression = pProp->GetExpression().GetExprText();
    xtstring sValues = pProp->GetExpression().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      pProp->GetExpression().SetExprText(sExpression);
      pProp->GetExpression().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CPropStonePropertiesInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                          const xtstring& sOldName,
                                                          const xtstring& sNewName,
                                                          CUserOfObject* pObject)
{
}
void CPropStonePropertiesInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                                  const xtstring& sName,
                                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CPropStonePropertiesInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CPropStonePropertiesInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObject* pObject)
{
}




