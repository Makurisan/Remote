// MPModelStoneInd.cpp: implementation of the CMPModelStoneInd classes.
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
#include "MPModelFieldsInd.h"
#include "MPModelStoneInd.h"

#include "SSyncEng/SyncGroups.h"















void CMPModelStoneInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag property
  if (ProjConstantExistInExpression(sOldName,
                                    GetTag().GetExprText(), GetTag().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
  // tag property
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CMPModelStoneInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteProjConstant(const xtstring& sName,
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
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag property
  if (ProjConstantExistInExpression(sName,
                                    GetTag().GetExprText(), GetTag().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_DeleteProjConstant(const xtstring& sName,
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
  // tag property
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CMPModelStoneInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteERModel(const xtstring& sName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_DeleteERModel(const xtstring& sName,
                                         CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteTable(const xtstring& sERModelName,
                                       const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_DeleteTable(const xtstring& sERModelName,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag property
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetTag().GetExprText(), GetTag().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
  // tag property
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CMPModelStoneInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteField(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // visibility
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag property
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetTag().GetExprText(), GetTag().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_DeleteField(const xtstring& sERModelName,
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
  // tag property
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CMPModelStoneInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                   const xtstring& sTableName,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteParam(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_DeleteParam(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_ParamAdded(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_ParamAdded(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_ParamChanged(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sName,
                                        CERModelParamInd* pParam,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_ParamChanged(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sName,
                                        CERModelParamInd* pParam,
                                        CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sParentTableName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_RelationAdded(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_RelationAdded(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_RelationChanged(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParentTableName,
                                           const xtstring& sName,
                                           CERModelRelationInd* pRelation,
                                           CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_RelationChanged(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParentTableName,
                                           const xtstring& sName,
                                           CERModelRelationInd* pRelation,
                                           CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // information sight
  if (GetInformationSight() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_IFS,
                                        1,
                                        GetInformationSight());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_ChangeIFSName(const xtstring& sOldName,
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
void CMPModelStoneInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteIFS(const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // information sight
  if (GetInformationSight() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_IFS,
                                        1,
                                        GetInformationSight());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_DeleteIFS(const xtstring& sName,
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
void CMPModelStoneInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_IFSTargetEntity,
                                        1,
                                        GetIFSTargetEntity());
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (EntityExistInExpression(sOldName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag property
  if (EntityExistInExpression(sOldName,
                              GetTag().GetExprText(), GetTag().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                        SE_MF_GetModuleName(),
                                                        _XT(""),
                                                        GetName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_TagProperty,
                                        3,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
  // visibility
  if (pObject->m_nIdentify == 2)
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
  // tag property
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelStoneInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteEntity(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_IFSTargetEntity,
                                        1,
                                        GetIFSTargetEntity());
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (EntityExistInExpression(sName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag property
  if (EntityExistInExpression(sName,
                              GetTag().GetExprText(), GetTag().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                        SE_MF_GetModuleName(),
                                                        _XT(""),
                                                        GetName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_TagProperty,
                                        3,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_DeleteEntity(const xtstring& sIFS,
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
  // visibility
  if (pObject->m_nIdentify == 2)
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
  // tag property
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelStoneInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
  // visibility
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag property
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetTag().GetExprText(), GetTag().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
  // tag property
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CMPModelStoneInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                      const xtstring& sEntityName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteVariable(const xtstring& sIFS,
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
  // visibility
  if (VariableExistInExpression(sEntityName, sName,
                                GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag property
  if (VariableExistInExpression(sEntityName, sName,
                                GetTag().GetExprText(), GetTag().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_DeleteVariable(const xtstring& sIFS,
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
  // tag property
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
}
void CMPModelStoneInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                           const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelStoneInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                               const xtstring& sName,
                                               CUserOfObject* pObject)
{
}
void CMPModelStoneInd::ROC_ChangeWPName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // stone or template
  if (GetStoneOrTemplate() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_StoneOrTemplate,
                                        1,
                                        GetStoneOrTemplate());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_ChangeWPName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // stone or template
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetStoneOrTemplate(sNewName);
    ROC_PropertyChanged_WP();
  }
}
void CMPModelStoneInd::ROC_CheckReferencesForWP(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteWP(sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteWP(const xtstring& sName,
                                    CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // stone or template
  if (GetStoneOrTemplate() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_StoneOrTemplate,
                                        1,
                                        GetStoneOrTemplate());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_DeleteWP(const xtstring& sName,
                                    CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_WP::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_WP::GetSyncResponseObject())
    return;

  // stone or template
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetStoneOrTemplate(_XT(""));
    ROC_PropertyChanged_WP();
  }
}
void CMPModelStoneInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_StoneOrTemplateRef,
                                        1,
                                        GetStoneOrTemplateRef());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
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
void CMPModelStoneInd::ROC_CheckReferencesForWPConstant(const xtstring& sWPName,
                                                        const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteWPConstant(sWPName, sName, cUsers);
}
void CMPModelStoneInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                            const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // stone or template ref
  if (GetStoneOrTemplate() == sWPName && GetStoneOrTemplateRef() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_StoneModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Stone_StoneOrTemplateRef,
                                        1,
                                        GetStoneOrTemplateRef());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelStoneInd::ROC_DeleteWPConstant(const xtstring& sWPName,
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
