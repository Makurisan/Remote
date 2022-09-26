// MPModelFieldsInd.cpp: implementation of the CMPModelXXXXInd classes.
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

#include "SSyncEng/SyncGroups.h"





/////////////////////////////////////////////////////////////////////////////
// CMPModelKeyInd
/////////////////////////////////////////////////////////////////////////////
void CMPModelKeyInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_Visibility,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // key values
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(nI);
    if (!pKeyValue)
      continue;
    if (ProjConstantExistInExpression(sOldName,
                                      pKeyValue->GetKeyValue().GetExprText(), pKeyValue->GetKeyValue().GetExprValues(),
                                      PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         pKeyValue->GetKeyColumn(),
                                                         CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                         CSyncResponse_Proj::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_KeyField_KeyValue,
                                          1000 + nI,
                                          pKeyValue->GetKeyValue().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CMPModelKeyInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
      ROC_PropertyChanged_Proj();
    }
  }
  // key values
  if (pObject->m_nIdentify >= 1000)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(pObject->m_nIdentify - 1000);
    if (pKeyValue)
    {
      xtstring sExpression = pKeyValue->GetKeyValue().GetExprText();
      xtstring sValues = pKeyValue->GetKeyValue().GetExprValues();
      if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                          pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pKeyValue->GetKeyValue().SetExprText(sExpression);
        pKeyValue->GetKeyValue().SetExprValues(sValues);
        ROC_PropertyChanged_Proj();
      }
    }
  }
}
void CMPModelKeyInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteProjConstant(const xtstring& sName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_Visibility,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // key values
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(nI);
    if (!pKeyValue)
      continue;
    if (ProjConstantExistInExpression(sName,
                                      pKeyValue->GetKeyValue().GetExprText(), pKeyValue->GetKeyValue().GetExprValues(),
                                      PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         pKeyValue->GetKeyColumn(),
                                                         CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                         CSyncResponse_Proj::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_KeyField_KeyValue,
                                          1000 + nI,
                                          pKeyValue->GetKeyValue().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CMPModelKeyInd::ROC_DeleteProjConstant(const xtstring& sName,
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
      ROC_PropertyChanged_Proj();
    }
  }
  // key values
  if (pObject->m_nIdentify >= 1000)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(pObject->m_nIdentify - 1000);
    if (pKeyValue)
    {
      xtstring sExpression = pKeyValue->GetKeyValue().GetExprText();
      xtstring sValues = pKeyValue->GetKeyValue().GetExprValues();
      if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                          pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pKeyValue->GetKeyValue().SetExprText(sExpression);
        pKeyValue->GetKeyValue().SetExprValues(sValues);
        ROC_PropertyChanged_Proj();
      }
    }
  }
}
void CMPModelKeyInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                              CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteERModel(const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_DeleteERModel(const xtstring& sName,
                                       CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteTable(const xtstring& sERModelName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_DeleteTable(const xtstring& sERModelName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_Visibility,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // key values
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(nI);
    if (!pKeyValue)
      continue;
    if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                               pKeyValue->GetKeyValue().GetExprText(), pKeyValue->GetKeyValue().GetExprValues(),
                               PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         pKeyValue->GetKeyColumn(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_KeyField_KeyValue,
                                          1000 + nI,
                                          pKeyValue->GetKeyValue().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CMPModelKeyInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
  // key values
  if (pObject->m_nIdentify >= 1000)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(pObject->m_nIdentify - 1000);
    if (pKeyValue)
    {
      xtstring sExpression = pKeyValue->GetKeyValue().GetExprText();
      xtstring sValues = pKeyValue->GetKeyValue().GetExprValues();
      if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                   pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pKeyValue->GetKeyValue().SetExprText(sExpression);
        pKeyValue->GetKeyValue().SetExprValues(sValues);
        ROC_PropertyChanged_ER();
      }
    }
  }
}
void CMPModelKeyInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteField(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_Visibility,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // key values
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(nI);
    if (!pKeyValue)
      continue;
    if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                               pKeyValue->GetKeyValue().GetExprText(), pKeyValue->GetKeyValue().GetExprValues(),
                               PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         pKeyValue->GetKeyColumn(),
                                                         CSyncResponse_ER::GetTypeOfResponseObject(),
                                                         CSyncResponse_ER::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_KeyField_KeyValue,
                                          1000 + nI,
                                          pKeyValue->GetKeyValue().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CMPModelKeyInd::ROC_DeleteField(const xtstring& sERModelName,
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
  // key values
  if (pObject->m_nIdentify >= 1000)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(pObject->m_nIdentify - 1000);
    if (pKeyValue)
    {
      xtstring sExpression = pKeyValue->GetKeyValue().GetExprText();
      xtstring sValues = pKeyValue->GetKeyValue().GetExprValues();
      if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                   pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pKeyValue->GetKeyValue().SetExprText(sExpression);
        pKeyValue->GetKeyValue().SetExprValues(sValues);
        ROC_PropertyChanged_ER();
      }
    }
  }
}
void CMPModelKeyInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sOldName,
                                         const xtstring& sNewName,
                                         CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteParam(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_DeleteParam(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_ParamAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CERModelParamInd* pParam,
                                    CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ParamAdded(const xtstring& sERModelName,
                                    const xtstring& sTableName,
                                    const xtstring& sName,
                                    CERModelParamInd* pParam,
                                    CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_ParamChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ParamChanged(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CERModelParamInd* pParam,
                                      CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                    const xtstring& sTableName,
                                                    const xtstring& sParentTableName,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_RelationAdded(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CERModelRelationInd* pRelation,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_RelationAdded(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sParentTableName,
                                       const xtstring& sName,
                                       CERModelRelationInd* pRelation,
                                       CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_RelationChanged(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_RelationChanged(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CERModelRelationInd* pRelation,
                                         CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                       const xtstring& sNewName,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                       const xtstring& sNewName,
                                       CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteIFS(const xtstring& sName,
                                   CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_DeleteIFS(const xtstring& sName,
                                   CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
  if (EntityExistInExpression(sOldName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_Visibility,
                                        1,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // key values
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(nI);
    if (!pKeyValue)
      continue;
    if (EntityExistInExpression(sOldName,
                                pKeyValue->GetKeyValue().GetExprText(), pKeyValue->GetKeyValue().GetExprValues(),
                                PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         pKeyValue->GetKeyColumn(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_KeyField_KeyValue,
                                          1000 + nI,
                                          pKeyValue->GetKeyValue().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CMPModelKeyInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
  // tag property
  if (pObject->m_nIdentify == 2)
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
  // key values
  if (pObject->m_nIdentify >= 1000)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(pObject->m_nIdentify - 1000);
    if (pKeyValue)
    {
      xtstring sExpression = pKeyValue->GetKeyValue().GetExprText();
      xtstring sValues = pKeyValue->GetKeyValue().GetExprValues();
      if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pKeyValue->GetKeyValue().SetExprText(sExpression);
        pKeyValue->GetKeyValue().SetExprValues(sValues);
        ROC_PropertyChanged_IFS();
      }
    }
  }
}
void CMPModelKeyInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteEntity(const xtstring& sIFS,
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
  if (EntityExistInExpression(sName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_Visibility,
                                        1,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // key values
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(nI);
    if (!pKeyValue)
      continue;
    if (EntityExistInExpression(sName,
                                pKeyValue->GetKeyValue().GetExprText(), pKeyValue->GetKeyValue().GetExprValues(),
                                PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         pKeyValue->GetKeyColumn(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_KeyField_KeyValue,
                                          1000 + nI,
                                          pKeyValue->GetKeyValue().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CMPModelKeyInd::ROC_DeleteEntity(const xtstring& sIFS,
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
  // tag property
  if (pObject->m_nIdentify == 2)
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
  // key values
  if (pObject->m_nIdentify >= 1000)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(pObject->m_nIdentify - 1000);
    if (pKeyValue)
    {
      xtstring sExpression = pKeyValue->GetKeyValue().GetExprText();
      xtstring sValues = pKeyValue->GetKeyValue().GetExprValues();
      if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pKeyValue->GetKeyValue().SetExprText(sExpression);
        pKeyValue->GetKeyValue().SetExprValues(sValues);
        ROC_PropertyChanged_IFS();
      }
    }
  }
}
void CMPModelKeyInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_Visibility,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // key values
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(nI);
    if (!pKeyValue)
      continue;
    if (VariableExistInExpression(sEntityName, sOldName,
                                  pKeyValue->GetKeyValue().GetExprText(), pKeyValue->GetKeyValue().GetExprValues(),
                                  PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         pKeyValue->GetKeyColumn(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_KeyField_KeyValue,
                                          1000 + nI,
                                          pKeyValue->GetKeyValue().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CMPModelKeyInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
      ROC_PropertyChanged_IFS();
    }
  }
  // key values
  if (pObject->m_nIdentify >= 1000)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(pObject->m_nIdentify - 1000);
    if (pKeyValue)
    {
      xtstring sExpression = pKeyValue->GetKeyValue().GetExprText();
      xtstring sValues = pKeyValue->GetKeyValue().GetExprValues();
      if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                      pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pKeyValue->GetKeyValue().SetExprText(sExpression);
        pKeyValue->GetKeyValue().SetExprValues(sValues);
        ROC_PropertyChanged_IFS();
      }
    }
  }
}
void CMPModelKeyInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                    const xtstring& sEntityName,
                                                    const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteVariable(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_Visibility,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_KeyField_TagProperty,
                                        2,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // key values
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(nI);
    if (!pKeyValue)
      continue;
    if (VariableExistInExpression(sEntityName, sName,
                                  pKeyValue->GetKeyValue().GetExprText(), pKeyValue->GetKeyValue().GetExprValues(),
                                  PositionsE, PositionsV))
    {
      // create object
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         pKeyValue->GetKeyColumn(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_KeyField_KeyValue,
                                          1000 + nI,
                                          pKeyValue->GetKeyValue().GetExprText(),
                                          PositionsE,
                                          PositionsV);
      cUsers.push_back(pResponseObject);
    }
  }
}
void CMPModelKeyInd::ROC_DeleteVariable(const xtstring& sIFS,
                                        const xtstring& sEntityName,
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
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
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
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // key values
  if (pObject->m_nIdentify >= 1000)
  {
    CMPModelKeyValueInd* pKeyValue = GetKeyValue(pObject->m_nIdentify - 1000);
    if (pKeyValue)
    {
      xtstring sExpression = pKeyValue->GetKeyValue().GetExprText();
      xtstring sValues = pKeyValue->GetKeyValue().GetExprValues();
      if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                      pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
      {
        pKeyValue->GetKeyValue().SetExprText(sExpression);
        pKeyValue->GetKeyValue().SetExprValues(sValues);
        ROC_PropertyChanged_IFS();
      }
    }
  }
}
void CMPModelKeyInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObject* pObject)
{
}
void CMPModelKeyInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CMPModelKeyInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                             const xtstring& sName,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelKeyInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                             const xtstring& sName,
                                             CUserOfObject* pObject)
{
}




































/////////////////////////////////////////////////////////////////////////////
// CMPModelTextInd
/////////////////////////////////////////////////////////////////////////////
void CMPModelTextInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // content
  if (ProjConstantExistInExpression(sOldName,
                                    GetContent().GetExprText(), GetContent().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Content,
                                        2,
                                        GetContent().GetExprText(),
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_TagProperty,
                                        3,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTextInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
  // content
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetContent().GetExprText();
    xtstring sValues = GetContent().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetContent().SetExprText(sExpression);
      GetContent().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // tag property
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CMPModelTextInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteProjConstant(const xtstring& sName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // content
  if (ProjConstantExistInExpression(sName,
                                    GetContent().GetExprText(), GetContent().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Content,
                                        2,
                                        GetContent().GetExprText(),
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_TagProperty,
                                        3,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTextInd::ROC_DeleteProjConstant(const xtstring& sName,
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
  // content
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetContent().GetExprText();
    xtstring sValues = GetContent().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetContent().SetExprText(sExpression);
      GetContent().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // tag property
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CMPModelTextInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                               CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteERModel(const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_DeleteERModel(const xtstring& sName,
                                        CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteTable(const xtstring& sERModelName,
                                      const xtstring& sName,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_DeleteTable(const xtstring& sERModelName,
                                      const xtstring& sName,
                                      CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // content
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetContent().GetExprText(), GetContent().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Content,
                                        2,
                                        GetContent().GetExprText(),
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_TagProperty,
                                        3,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTextInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
  // content
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetContent().GetExprText();
    xtstring sValues = GetContent().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetContent().SetExprText(sExpression);
      GetContent().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // tag property
  if (pObject->m_nIdentify == 3)
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
void CMPModelTextInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteField(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // content
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetContent().GetExprText(), GetContent().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Content,
                                        2,
                                        GetContent().GetExprText(),
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_TagProperty,
                                        3,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTextInd::ROC_DeleteField(const xtstring& sERModelName,
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
  // content
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetContent().GetExprText();
    xtstring sValues = GetContent().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetContent().SetExprText(sExpression);
      GetContent().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // tag property
  if (pObject->m_nIdentify == 3)
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
void CMPModelTextInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteParam(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_DeleteParam(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_ParamAdded(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CERModelParamInd* pParam,
                                     CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ParamAdded(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CERModelParamInd* pParam,
                                     CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_ParamChanged(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CERModelParamInd* pParam,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ParamChanged(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CERModelParamInd* pParam,
                                       CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_RelationAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_RelationAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_RelationChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CERModelRelationInd* pRelation,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_RelationChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CERModelRelationInd* pRelation,
                                          CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteIFS(const xtstring& sName,
                                    CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_DeleteIFS(const xtstring& sName,
                                    CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
  if (EntityExistInExpression(sOldName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // content
  if (EntityExistInExpression(sOldName,
                              GetContent().GetExprText(), GetContent().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Content,
                                        3,
                                        GetContent().GetExprText(),
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_TagProperty,
                                        4,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTextInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

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
  // content
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetContent().GetExprText();
    xtstring sValues = GetContent().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetContent().SetExprText(sExpression);
      GetContent().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag property
  if (pObject->m_nIdentify == 4)
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
void CMPModelTextInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteEntity(const xtstring& sIFS,
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
  if (EntityExistInExpression(sName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // content
  if (EntityExistInExpression(sName,
                              GetContent().GetExprText(), GetContent().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Content,
                                        3,
                                        GetContent().GetExprText(),
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_TagProperty,
                                        4,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTextInd::ROC_DeleteEntity(const xtstring& sIFS,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

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
  // content
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetContent().GetExprText();
    xtstring sValues = GetContent().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetContent().SetExprText(sExpression);
      GetContent().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag property
  if (pObject->m_nIdentify == 4)
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
void CMPModelTextInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // content
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetContent().GetExprText(), GetContent().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Content,
                                        2,
                                        GetContent().GetExprText(),
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_TagProperty,
                                        3,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTextInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
  // content
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetContent().GetExprText();
    xtstring sValues = GetContent().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetContent().SetExprText(sExpression);
      GetContent().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag property
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelTextInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteVariable(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // content
  if (VariableExistInExpression(sEntityName, sName,
                                GetContent().GetExprText(), GetContent().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_Content,
                                        2,
                                        GetContent().GetExprText(),
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_TextField_TagProperty,
                                        3,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTextInd::ROC_DeleteVariable(const xtstring& sIFS,
                                         const xtstring& sEntityName,
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
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // content
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetContent().GetExprText();
    xtstring sValues = GetContent().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetContent().SetExprText(sExpression);
      GetContent().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag property
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelTextInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CMPModelTextInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CMPModelTextInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelTextInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
}






























/////////////////////////////////////////////////////////////////////////////
// CMPModelAreaInd
/////////////////////////////////////////////////////////////////////////////
void CMPModelAreaInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_AreaField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelAreaInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
void CMPModelAreaInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteProjConstant(const xtstring& sName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_AreaField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelAreaInd::ROC_DeleteProjConstant(const xtstring& sName,
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
void CMPModelAreaInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                               CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteERModel(const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_DeleteERModel(const xtstring& sName,
                                        CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteTable(const xtstring& sERModelName,
                                      const xtstring& sName,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_DeleteTable(const xtstring& sERModelName,
                                      const xtstring& sName,
                                      CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_AreaField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelAreaInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
void CMPModelAreaInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteField(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_AreaField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelAreaInd::ROC_DeleteField(const xtstring& sERModelName,
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
void CMPModelAreaInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteParam(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_DeleteParam(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_ParamAdded(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CERModelParamInd* pParam,
                                     CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ParamAdded(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CERModelParamInd* pParam,
                                     CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_ParamChanged(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CERModelParamInd* pParam,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ParamChanged(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CERModelParamInd* pParam,
                                       CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_RelationAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_RelationAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_RelationChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CERModelRelationInd* pRelation,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_RelationChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CERModelRelationInd* pRelation,
                                          CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteIFS(const xtstring& sName,
                                    CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_DeleteIFS(const xtstring& sName,
                                    CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
  if (EntityExistInExpression(sOldName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_AreaField_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelAreaInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

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
}
void CMPModelAreaInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteEntity(const xtstring& sIFS,
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
  if (EntityExistInExpression(sName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_AreaField_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelAreaInd::ROC_DeleteEntity(const xtstring& sIFS,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

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
}
void CMPModelAreaInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
      CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                         SE_MF_GetModuleName(),
                                                         GetName(),
                                                         GetName(),
                                                         CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                         CSyncResponse_IFS::GetSyncResponseObject());
      if (!pResponseObject)
        return;
      // set changes positions
      pResponseObject->SetChangePositions(tSyncProp_AreaField_Visibility,
                                          1,
                                          GetVisibility().GetExprText(),
                                          PositionsE,
                                          PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelAreaInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
void CMPModelAreaInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteVariable(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_AreaField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelAreaInd::ROC_DeleteVariable(const xtstring& sIFS,
                                         const xtstring& sEntityName,
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
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelAreaInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CMPModelAreaInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CMPModelAreaInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelAreaInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
}






























/////////////////////////////////////////////////////////////////////////////
// CMPModelLinkInd
/////////////////////////////////////////////////////////////////////////////
void CMPModelLinkInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // address
  if (ProjConstantExistInExpression(sOldName,
                                    GetAddress().GetExprText(), GetAddress().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Address,
                                        2,
                                        GetAddress().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (ProjConstantExistInExpression(sOldName,
                                    GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_QuickInfo,
                                        3,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (ProjConstantExistInExpression(sOldName,
                                    GetTag().GetExprText(), GetTag().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Tag,
                                        4,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelLinkInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
  // address
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAddress().GetExprText();
    xtstring sValues = GetAddress().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAddress().SetExprText(sExpression);
      GetAddress().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // tag
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CMPModelLinkInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteProjConstant(const xtstring& sName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // address
  if (ProjConstantExistInExpression(sName,
                                    GetAddress().GetExprText(), GetAddress().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Address,
                                        2,
                                        GetAddress().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (ProjConstantExistInExpression(sName,
                                    GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_QuickInfo,
                                        3,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (ProjConstantExistInExpression(sName,
                                    GetTag().GetExprText(), GetTag().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Tag,
                                        4,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelLinkInd::ROC_DeleteProjConstant(const xtstring& sName,
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
  // address
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAddress().GetExprText();
    xtstring sValues = GetAddress().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAddress().SetExprText(sExpression);
      GetAddress().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // tag
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CMPModelLinkInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                               CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteERModel(const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_DeleteERModel(const xtstring& sName,
                                        CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteTable(const xtstring& sERModelName,
                                      const xtstring& sName,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_DeleteTable(const xtstring& sERModelName,
                                      const xtstring& sName,
                                      CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // address
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetAddress().GetExprText(), GetAddress().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Address,
                                        2,
                                        GetAddress().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_QuickInfo,
                                        3,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetTag().GetExprText(), GetTag().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Tag,
                                        4,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelLinkInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
  // address
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAddress().GetExprText();
    xtstring sValues = GetAddress().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAddress().SetExprText(sExpression);
      GetAddress().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // tag
  if (pObject->m_nIdentify == 4)
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
void CMPModelLinkInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteField(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // address
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetAddress().GetExprText(), GetAddress().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Address,
                                        2,
                                        GetAddress().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_QuickInfo,
                                        3,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetTag().GetExprText(), GetTag().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Tag,
                                        4,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelLinkInd::ROC_DeleteField(const xtstring& sERModelName,
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
  // address
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAddress().GetExprText();
    xtstring sValues = GetAddress().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAddress().SetExprText(sExpression);
      GetAddress().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // tag
  if (pObject->m_nIdentify == 4)
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
void CMPModelLinkInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sOldName,
                                          const xtstring& sNewName,
                                          CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                  const xtstring& sTableName,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteParam(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_DeleteParam(const xtstring& sERModelName,
                                      const xtstring& sTableName,
                                      const xtstring& sName,
                                      CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_ParamAdded(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CERModelParamInd* pParam,
                                     CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ParamAdded(const xtstring& sERModelName,
                                     const xtstring& sTableName,
                                     const xtstring& sName,
                                     CERModelParamInd* pParam,
                                     CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_ParamChanged(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CERModelParamInd* pParam,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ParamChanged(const xtstring& sERModelName,
                                       const xtstring& sTableName,
                                       const xtstring& sName,
                                       CERModelParamInd* pParam,
                                       CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sParentTableName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sParentTableName,
                                         const xtstring& sName,
                                         CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_RelationAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_RelationAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sParentTableName,
                                        const xtstring& sName,
                                        CERModelRelationInd* pRelation,
                                        CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_RelationChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CERModelRelationInd* pRelation,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_RelationChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sParentTableName,
                                          const xtstring& sName,
                                          CERModelRelationInd* pRelation,
                                          CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                        const xtstring& sNewName,
                                        CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteIFS(const xtstring& sName,
                                    CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_DeleteIFS(const xtstring& sName,
                                    CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
  if (EntityExistInExpression(sOldName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // address
  if (EntityExistInExpression(sOldName,
                              GetAddress().GetExprText(), GetAddress().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Address,
                                        3,
                                        GetAddress().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (EntityExistInExpression(sOldName,
                              GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_QuickInfo,
                                        4,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (EntityExistInExpression(sOldName,
                              GetTag().GetExprText(), GetTag().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Tag,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelLinkInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                           const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

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
  // address
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAddress().GetExprText();
    xtstring sValues = GetAddress().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAddress().SetExprText(sExpression);
      GetAddress().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag
  if (pObject->m_nIdentify == 5)
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
void CMPModelLinkInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                   const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteEntity(const xtstring& sIFS,
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
  if (EntityExistInExpression(sName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // address
  if (EntityExistInExpression(sName,
                              GetAddress().GetExprText(), GetAddress().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Address,
                                        3,
                                        GetAddress().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (EntityExistInExpression(sName,
                              GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_QuickInfo,
                                        4,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (EntityExistInExpression(sName,
                              GetTag().GetExprText(), GetTag().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Tag,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelLinkInd::ROC_DeleteEntity(const xtstring& sIFS,
                                       const xtstring& sName,
                                       CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

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
  // address
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetAddress().GetExprText();
    xtstring sValues = GetAddress().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAddress().SetExprText(sExpression);
      GetAddress().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag
  if (pObject->m_nIdentify == 5)
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
void CMPModelLinkInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // address
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetAddress().GetExprText(), GetAddress().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Address,
                                        2,
                                        GetAddress().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_QuickInfo,
                                        3,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetTag().GetExprText(), GetTag().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Tag,
                                        4,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelLinkInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
  // address
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAddress().GetExprText();
    xtstring sValues = GetAddress().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAddress().SetExprText(sExpression);
      GetAddress().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelLinkInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                     const xtstring& sEntityName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteVariable(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // address
  if (VariableExistInExpression(sEntityName, sName,
                                GetAddress().GetExprText(), GetAddress().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Address,
                                        2,
                                        GetAddress().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (VariableExistInExpression(sEntityName, sName,
                                GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_QuickInfo,
                                        3,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (VariableExistInExpression(sEntityName, sName,
                                GetTag().GetExprText(), GetTag().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_LinkField_Tag,
                                        4,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelLinkInd::ROC_DeleteVariable(const xtstring& sIFS,
                                         const xtstring& sEntityName,
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
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // address
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetAddress().GetExprText();
    xtstring sValues = GetAddress().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetAddress().SetExprText(sExpression);
      GetAddress().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelLinkInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                  const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CMPModelLinkInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                          const xtstring& sName,
                                                          CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CMPModelLinkInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                              const xtstring& sName,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelLinkInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                              const xtstring& sName,
                                              CUserOfObject* pObject)
{
}






























/////////////////////////////////////////////////////////////////////////////
// CMPModelGraphicInd
/////////////////////////////////////////////////////////////////////////////
void CMPModelGraphicInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name local
  if (ProjConstantExistInExpression(sOldName,
                                    GetPicFileNameLocal().GetExprText(), GetPicFileNameLocal().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileNameLocal,
                                        2,
                                        GetPicFileNameLocal().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name
  if (ProjConstantExistInExpression(sOldName,
                                    GetPicFileName().GetExprText(), GetPicFileName().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileName,
                                        3,
                                        GetPicFileName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (ProjConstantExistInExpression(sOldName,
                                    GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_QuickInfo,
                                        4,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (ProjConstantExistInExpression(sOldName,
                                    GetTag().GetExprText(), GetTag().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Tag,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelGraphicInd::ROC_ChangeProjConstant(const xtstring& sOldName,
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
  // pic file name local
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPicFileNameLocal().GetExprText();
    xtstring sValues = GetPicFileNameLocal().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileNameLocal().SetExprText(sExpression);
      GetPicFileNameLocal().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // pic file name
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPicFileName().GetExprText();
    xtstring sValues = GetPicFileName().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileName().SetExprText(sExpression);
      GetPicFileName().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // tag
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CMPModelGraphicInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                            CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteProjConstant(const xtstring& sName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name local
  if (ProjConstantExistInExpression(sName,
                                    GetPicFileNameLocal().GetExprText(), GetPicFileNameLocal().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileNameLocal,
                                        2,
                                        GetPicFileNameLocal().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name
  if (ProjConstantExistInExpression(sName,
                                    GetPicFileName().GetExprText(), GetPicFileName().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileName,
                                        3,
                                        GetPicFileName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (ProjConstantExistInExpression(sName,
                                    GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_QuickInfo,
                                        4,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (ProjConstantExistInExpression(sName,
                                    GetTag().GetExprText(), GetTag().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Tag,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelGraphicInd::ROC_DeleteProjConstant(const xtstring& sName,
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
  // pic file name local
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPicFileNameLocal().GetExprText();
    xtstring sValues = GetPicFileNameLocal().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileNameLocal().SetExprText(sExpression);
      GetPicFileNameLocal().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // pic file name
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPicFileName().GetExprText();
    xtstring sValues = GetPicFileName().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileName().SetExprText(sExpression);
      GetPicFileName().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // tag
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceProjConstantInExpression(sName, _XT(""), sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
}
void CMPModelGraphicInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                  const xtstring& sNewName,
                                                  CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                  CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                               const xtstring& sNewName,
                                               CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteERModel(const xtstring& sName,
                                           CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_DeleteERModel(const xtstring& sName,
                                           CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteTable(const xtstring& sERModelName,
                                         const xtstring& sName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_DeleteTable(const xtstring& sERModelName,
                                         const xtstring& sName,
                                         CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name local
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetPicFileNameLocal().GetExprText(), GetPicFileNameLocal().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileNameLocal,
                                        2,
                                        GetPicFileNameLocal().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetPicFileName().GetExprText(), GetPicFileName().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileName,
                                        3,
                                        GetPicFileName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_QuickInfo,
                                        4,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetTag().GetExprText(), GetTag().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Tag,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelGraphicInd::ROC_ChangeFieldName(const xtstring& sERModelName,
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
  // pic file name local
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPicFileNameLocal().GetExprText();
    xtstring sValues = GetPicFileNameLocal().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileNameLocal().SetExprText(sExpression);
      GetPicFileNameLocal().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // pic file name
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPicFileName().GetExprText();
    xtstring sValues = GetPicFileName().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileName().SetExprText(sExpression);
      GetPicFileName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceFieldInExpression(sOldName, sNewName, sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // tag
  if (pObject->m_nIdentify == 5)
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
void CMPModelGraphicInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteField(const xtstring& sERModelName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name local
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetPicFileNameLocal().GetExprText(), GetPicFileNameLocal().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileNameLocal,
                                        2,
                                        GetPicFileNameLocal().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetPicFileName().GetExprText(), GetPicFileName().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileName,
                                        3,
                                        GetPicFileName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_QuickInfo,
                                        4,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetTag().GetExprText(), GetTag().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Tag,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelGraphicInd::ROC_DeleteField(const xtstring& sERModelName,
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
  // pic file name local
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPicFileNameLocal().GetExprText();
    xtstring sValues = GetPicFileNameLocal().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileNameLocal().SetExprText(sExpression);
      GetPicFileNameLocal().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // pic file name
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPicFileName().GetExprText();
    xtstring sValues = GetPicFileName().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileName().SetExprText(sExpression);
      GetPicFileName().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceFieldInExpression(sName, _XT(""), sExpression, sValues,
                                 pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_ER();
    }
  }
  // tag
  if (pObject->m_nIdentify == 5)
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
void CMPModelGraphicInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName,
                                             CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                     const xtstring& sTableName,
                                                     const xtstring& sName,
                                                     CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteParam(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sName,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_DeleteParam(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sName,
                                         CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_ParamAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sName,
                                        CERModelParamInd* pParam,
                                        CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ParamAdded(const xtstring& sERModelName,
                                        const xtstring& sTableName,
                                        const xtstring& sName,
                                        CERModelParamInd* pParam,
                                        CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_ParamChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sName,
                                          CERModelParamInd* pParam,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ParamChanged(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sName,
                                          CERModelParamInd* pParam,
                                          CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                const xtstring& sTableName,
                                                const xtstring& sParentTableName,
                                                const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                        const xtstring& sTableName,
                                                        const xtstring& sParentTableName,
                                                        const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sName,
                                            CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_RelationAdded(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParentTableName,
                                           const xtstring& sName,
                                           CERModelRelationInd* pRelation,
                                           CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_RelationAdded(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sParentTableName,
                                           const xtstring& sName,
                                           CERModelRelationInd* pRelation,
                                           CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_RelationChanged(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sName,
                                             CERModelRelationInd* pRelation,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_RelationChanged(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sName,
                                             CERModelRelationInd* pRelation,
                                             CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteIFS(const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_DeleteIFS(const xtstring& sName,
                                       CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
  if (EntityExistInExpression(sOldName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name local
  if (EntityExistInExpression(sOldName,
                              GetPicFileNameLocal().GetExprText(), GetPicFileNameLocal().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileNameLocal,
                                        3,
                                        GetPicFileNameLocal().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name
  if (EntityExistInExpression(sOldName,
                              GetPicFileName().GetExprText(), GetPicFileName().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileName,
                                        4,
                                        GetPicFileName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (EntityExistInExpression(sOldName,
                              GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_QuickInfo,
                                        5,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (EntityExistInExpression(sOldName,
                              GetTag().GetExprText(), GetTag().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Tag,
                                        6,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelGraphicInd::ROC_ChangeEntityName(const xtstring& sIFS,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

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
  // pic file name local
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPicFileNameLocal().GetExprText();
    xtstring sValues = GetPicFileNameLocal().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileNameLocal().SetExprText(sExpression);
      GetPicFileNameLocal().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // pic file name
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetPicFileName().GetExprText();
    xtstring sValues = GetPicFileName().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileName().SetExprText(sExpression);
      GetPicFileName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceEntityInExpression(sOldName, sNewName, sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag
  if (pObject->m_nIdentify == 6)
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
void CMPModelGraphicInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteEntity(const xtstring& sIFS,
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
  if (EntityExistInExpression(sName,
                              GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Visibility,
                                        2,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name local
  if (EntityExistInExpression(sName,
                              GetPicFileNameLocal().GetExprText(), GetPicFileNameLocal().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileNameLocal,
                                        3,
                                        GetPicFileNameLocal().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name
  if (EntityExistInExpression(sName,
                              GetPicFileName().GetExprText(), GetPicFileName().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileName,
                                        4,
                                        GetPicFileName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (EntityExistInExpression(sName,
                              GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_QuickInfo,
                                        5,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (EntityExistInExpression(sName,
                              GetTag().GetExprText(), GetTag().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Tag,
                                        6,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelGraphicInd::ROC_DeleteEntity(const xtstring& sIFS,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

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
  // pic file name local
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPicFileNameLocal().GetExprText();
    xtstring sValues = GetPicFileNameLocal().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileNameLocal().SetExprText(sExpression);
      GetPicFileNameLocal().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // pic file name
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetPicFileName().GetExprText();
    xtstring sValues = GetPicFileName().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileName().SetExprText(sExpression);
      GetPicFileName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceEntityInExpression(sName, _XT(""), sExpression, sValues,
                                  pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag
  if (pObject->m_nIdentify == 6)
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
void CMPModelGraphicInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name local
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetPicFileNameLocal().GetExprText(), GetPicFileNameLocal().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileNameLocal,
                                        2,
                                        GetPicFileNameLocal().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetPicFileName().GetExprText(), GetPicFileName().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileName,
                                        3,
                                        GetPicFileName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_QuickInfo,
                                        4,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetTag().GetExprText(), GetTag().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Tag,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelGraphicInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
  // pic file name local
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPicFileNameLocal().GetExprText();
    xtstring sValues = GetPicFileNameLocal().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileNameLocal().SetExprText(sExpression);
      GetPicFileNameLocal().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // pic file name
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPicFileName().GetExprText();
    xtstring sValues = GetPicFileName().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileName().SetExprText(sExpression);
      GetPicFileName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelGraphicInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                        const xtstring& sEntityName,
                                                        const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteVariable(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Visibility,
                                        1,
                                        GetVisibility().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name local
  if (VariableExistInExpression(sEntityName, sName,
                                GetPicFileNameLocal().GetExprText(), GetPicFileNameLocal().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileNameLocal,
                                        2,
                                        GetPicFileNameLocal().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // pic file name
  if (VariableExistInExpression(sEntityName, sName,
                                GetPicFileName().GetExprText(), GetPicFileName().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_PicFileName,
                                        3,
                                        GetPicFileName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // quick info
  if (VariableExistInExpression(sEntityName, sName,
                                GetQuickInfo().GetExprText(), GetQuickInfo().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_QuickInfo,
                                        4,
                                        GetQuickInfo().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // tag
  if (VariableExistInExpression(sEntityName, sName,
                                GetTag().GetExprText(), GetTag().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(GetUserOfObject_ModuleType(),
                                                       SE_MF_GetModuleName(),
                                                       GetName(),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_GraphicField_Tag,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelGraphicInd::ROC_DeleteVariable(const xtstring& sIFS,
                                            const xtstring& sEntityName,
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
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetVisibility().SetExprText(sExpression);
      GetVisibility().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // pic file name local
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetPicFileNameLocal().GetExprText();
    xtstring sValues = GetPicFileNameLocal().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileNameLocal().SetExprText(sExpression);
      GetPicFileNameLocal().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // pic file name
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetPicFileName().GetExprText();
    xtstring sValues = GetPicFileName().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetPicFileName().SetExprText(sExpression);
      GetPicFileName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // quick info
  if (pObject->m_nIdentify == 4)
  {
    xtstring sExpression = GetQuickInfo().GetExprText();
    xtstring sValues = GetQuickInfo().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetQuickInfo().SetExprText(sExpression);
      GetQuickInfo().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // tag
  if (pObject->m_nIdentify == 5)
  {
    xtstring sExpression = GetTag().GetExprText();
    xtstring sValues = GetTag().GetExprValues();
    if (ReplaceVariableInExpression(sName, _XT(""), sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTag().SetExprText(sExpression);
      GetTag().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
}
void CMPModelGraphicInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                     const xtstring& sOldName,
                                                     const xtstring& sNewName,
                                                     CUserOfObject* pObject)
{
}
void CMPModelGraphicInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                             const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CMPModelGraphicInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                 const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CMPModelGraphicInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                 const xtstring& sName,
                                                 CUserOfObject* pObject)
{
}



