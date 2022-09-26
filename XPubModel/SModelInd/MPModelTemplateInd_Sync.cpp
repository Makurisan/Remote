// MPModelTemplateInd.cpp: implementation of the CMPModelTemplateInd classes.
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
#include "MPModelTemplateInd.h"

#include "SSyncEng/SyncGroups.h"




















void CMPModelTemplateInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // template file show name
  if (ProjConstantExistInExpression(sOldName,
                                    GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFileShowName,
                                        1,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFile,
                                        2,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFolder,
                                        3,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_Visibility,
                                        4,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TagProperty,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_ChangeProjConstant(const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // template file show name
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
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
      ROC_PropertyChanged_Proj();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceProjConstantInExpression(sOldName, sNewName, sExpression, sValues,
                                        pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_Proj();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 4)
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
  if (pObject->m_nIdentify == 5)
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
void CMPModelTemplateInd::ROC_CheckReferencesForProjConstant(const xtstring& sName,
                                                             CUserOfObjectArray& cUsers)
{
  ROC_DeleteProjConstant(sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                 CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // template file show name
  if (ProjConstantExistInExpression(sName,
                                    GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                                    PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFileShowName,
                                        1,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFile,
                                        2,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFolder,
                                        3,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_Visibility,
                                        4,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TagProperty,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_DeleteProjConstant(const xtstring& sName,
                                                 CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // template file show name
  if (pObject->m_nIdentify == 1)
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
  if (pObject->m_nIdentify == 2)
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
  if (pObject->m_nIdentify == 3)
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
  if (pObject->m_nIdentify == 4)
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
  if (pObject->m_nIdentify == 5)
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
void CMPModelTemplateInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetTemplateSymbolicDBName() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_SymbolicDBName,
                                        1,
                                        GetTemplateSymbolicDBName());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_ChangeSymbolicDBName(const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // information sight
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetTemplateSymbolicDBName(sNewName);
    ROC_PropertyChanged_Proj();
  }
}
void CMPModelTemplateInd::ROC_CheckReferencesForSymbolicDBName(const xtstring& sName,
                                                               CUserOfObjectArray& cUsers)
{
  ROC_DeleteSymbolicDBName(sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  if (GetTemplateSymbolicDBName() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_Proj::GetTypeOfResponseObject(),
                                                       CSyncResponse_Proj::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_SymbolicDBName,
                                        1,
                                        GetTemplateSymbolicDBName());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_DeleteSymbolicDBName(const xtstring& sName,
                                                   CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_Proj::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_Proj::GetSyncResponseObject())
    return;

  // information sight
  if (pObject->m_nIdentify == 1 && pObject->GetChange())
  {
    SetTemplateSymbolicDBName(_XT(""));
    ROC_PropertyChanged_Proj();
  }
}
void CMPModelTemplateInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_ChangeERModelName(const xtstring& sOldName,
                                                const xtstring& sNewName,
                                                CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_CheckReferencesForERModel(const xtstring& sName,
                                                        CUserOfObjectArray& cUsers)
{
  ROC_DeleteERModel(sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteERModel(const xtstring& sName,
                                            CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_DeleteERModel(const xtstring& sName,
                                            CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_ChangeTableName(const xtstring& sERModelName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_CheckReferencesForTable(const xtstring& sERModelName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteTable(sERModelName, sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteTable(const xtstring& sERModelName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_DeleteTable(const xtstring& sERModelName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // template file show name
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFileShowName,
                                        1,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFile,
                                        2,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFolder,
                                        3,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sOldName,
                             GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_Visibility,
                                        4,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TagProperty,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_ChangeFieldName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // template file show name
  if (pObject->m_nIdentify == 1)
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
  if (pObject->m_nIdentify == 2)
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
  if (pObject->m_nIdentify == 3)
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
  if (pObject->m_nIdentify == 4)
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
void CMPModelTemplateInd::ROC_CheckReferencesForField(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteField(sERModelName, sTableName, sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteField(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  CsizetVector PositionsE;
  CsizetVector PositionsV;
  // template file show name
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFileShowName,
                                        1,
                                        GetTemplateFileShowName().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    if (!pResponseObject)
      return;
    cUsers.push_back(pResponseObject);
  }
  // template file
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetTemplateFile().GetExprText(), GetTemplateFile().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFile,
                                        2,
                                        GetTemplateFile().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // template folder
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetTemplateFolder().GetExprText(), GetTemplateFolder().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFolder,
                                        3,
                                        GetTemplateFolder().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
  // visibility
  if (FieldExistInExpression(GetInformationSight(), sERModelName, sTableName, sName,
                             GetVisibility().GetExprText(), GetVisibility().GetExprValues(),
                             PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_Visibility,
                                        4,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_ER::GetTypeOfResponseObject(),
                                                       CSyncResponse_ER::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TagProperty,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_DeleteField(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_ER::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_ER::GetSyncResponseObject())
    return;

  // template file show name
  if (pObject->m_nIdentify == 1)
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
  if (pObject->m_nIdentify == 2)
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
  if (pObject->m_nIdentify == 3)
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
  if (pObject->m_nIdentify == 4)
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
void CMPModelTemplateInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_ChangeParamName(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sOldName,
                                              const xtstring& sNewName,
                                              CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_CheckReferencesForParam(const xtstring& sERModelName,
                                                      const xtstring& sTableName,
                                                      const xtstring& sName,
                                                      CUserOfObjectArray& cUsers)
{
  ROC_DeleteParam(sERModelName, sTableName, sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteParam(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sName,
                                          CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_DeleteParam(const xtstring& sERModelName,
                                          const xtstring& sTableName,
                                          const xtstring& sName,
                                          CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_ParamAdded(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sName,
                                         CERModelParamInd* pParam,
                                         CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_ParamAdded(const xtstring& sERModelName,
                                         const xtstring& sTableName,
                                         const xtstring& sName,
                                         CERModelParamInd* pParam,
                                         CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_ParamChanged(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sName,
                                           CERModelParamInd* pParam,
                                           CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_ParamChanged(const xtstring& sERModelName,
                                           const xtstring& sTableName,
                                           const xtstring& sName,
                                           CERModelParamInd* pParam,
                                           CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_ChangeRelationName(const xtstring& sERModelName,
                                                 const xtstring& sTableName,
                                                 const xtstring& sParentTableName,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_CheckReferencesForRelation(const xtstring& sERModelName,
                                                         const xtstring& sTableName,
                                                         const xtstring& sParentTableName,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteRelation(sERModelName, sTableName, sParentTableName, sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sName,
                                             CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_DeleteRelation(const xtstring& sERModelName,
                                             const xtstring& sTableName,
                                             const xtstring& sParentTableName,
                                             const xtstring& sName,
                                             CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_RelationAdded(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sName,
                                            CERModelRelationInd* pRelation,
                                            CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_RelationAdded(const xtstring& sERModelName,
                                            const xtstring& sTableName,
                                            const xtstring& sParentTableName,
                                            const xtstring& sName,
                                            CERModelRelationInd* pRelation,
                                            CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_RelationChanged(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sName,
                                              CERModelRelationInd* pRelation,
                                              CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_RelationChanged(const xtstring& sERModelName,
                                              const xtstring& sTableName,
                                              const xtstring& sParentTableName,
                                              const xtstring& sName,
                                              CERModelRelationInd* pRelation,
                                              CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_ChangeIFSName(const xtstring& sOldName,
                                            const xtstring& sNewName,
                                            CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // information sight
  if (GetInformationSight() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_IFS,
                                        1,
                                        GetInformationSight());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_ChangeIFSName(const xtstring& sOldName,
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
void CMPModelTemplateInd::ROC_CheckReferencesForIFS(const xtstring& sName,
                                                    CUserOfObjectArray& cUsers)
{
  ROC_DeleteIFS(sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteIFS(const xtstring& sName,
                                        CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // information sight
  if (GetInformationSight() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_IFS,
                                        1,
                                        GetInformationSight());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_DeleteIFS(const xtstring& sName,
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
void CMPModelTemplateInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_IFSTargetEntity,
                                        1,
                                        GetIFSTargetEntity());
    cUsers.push_back(pResponseObject);
  }
  // template file show name
  if (EntityExistInExpression(sOldName,
                              GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFileShowName,
                                        2,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFile,
                                        3,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFolder,
                                        4,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_Visibility,
                                        5,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                        SE_MF_GetModuleName(),
                                                        _XT(""),
                                                        GetName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TagProperty,
                                        6,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_ChangeEntityName(const xtstring& sIFS,
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
  // template file show name
  if (pObject->m_nIdentify == 2)
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
  if (pObject->m_nIdentify == 3)
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
  if (pObject->m_nIdentify == 4)
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
  if (pObject->m_nIdentify == 5)
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
void CMPModelTemplateInd::ROC_CheckReferencesForEntity(const xtstring& sIFS,
                                                       const xtstring& sName,
                                                       CUserOfObjectArray& cUsers)
{
  ROC_DeleteEntity(sIFS, sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteEntity(const xtstring& sIFS,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_IFSTargetEntity,
                                        1,
                                        GetIFSTargetEntity());
    cUsers.push_back(pResponseObject);
  }
  // template file show name
  if (EntityExistInExpression(sName,
                              GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                              PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFileShowName,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFile,
                                        3,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFolder,
                                        4,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                      SE_MF_GetModuleName(),
                                                      _XT(""),
                                                      GetName(),
                                                      CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                      CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_Visibility,
                                        5,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                        SE_MF_GetModuleName(),
                                                        _XT(""),
                                                        GetName(),
                                                        CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                        CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TagProperty,
                                        6,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_DeleteEntity(const xtstring& sIFS,
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
  // template file show name
  if (pObject->m_nIdentify == 2)
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
  if (pObject->m_nIdentify == 3)
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
  if (pObject->m_nIdentify == 4)
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
  if (pObject->m_nIdentify == 5)
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
void CMPModelTemplateInd::ROC_ChangeVariableName(const xtstring& sIFS,
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
  // template file
  if (VariableExistInExpression(sEntityName, sOldName,
                                GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFileShowName,
                                        1,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFile,
                                        2,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFolder,
                                        3,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_Visibility,
                                        4,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TagProperty,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_ChangeVariableName(const xtstring& sIFS,
                                                 const xtstring& sEntityName,
                                                 const xtstring& sOldName,
                                                 const xtstring& sNewName,
                                                 CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // template file show name
  if (pObject->m_nIdentify == 1)
  {
    xtstring sExpression = GetTemplateFileShowName().GetExprText();
    xtstring sValues = GetTemplateFileShowName().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFileShowName().SetExprText(sExpression);
      GetTemplateFileShowName().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // template file
  if (pObject->m_nIdentify == 2)
  {
    xtstring sExpression = GetTemplateFile().GetExprText();
    xtstring sValues = GetTemplateFile().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFile().SetExprText(sExpression);
      GetTemplateFile().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // template folder
  if (pObject->m_nIdentify == 3)
  {
    xtstring sExpression = GetTemplateFolder().GetExprText();
    xtstring sValues = GetTemplateFolder().GetExprValues();
    if (ReplaceVariableInExpression(sOldName, sNewName, sExpression, sValues,
                                    pObject->m_PositionsE, pObject->m_PositionsV, pObject->m_Changes))
    {
      GetTemplateFolder().SetExprText(sExpression);
      GetTemplateFolder().SetExprValues(sValues);
      ROC_PropertyChanged_IFS();
    }
  }
  // visibility
  if (pObject->m_nIdentify == 4)
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
  if (pObject->m_nIdentify == 5)
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
void CMPModelTemplateInd::ROC_CheckReferencesForVariable(const xtstring& sIFS,
                                                         const xtstring& sEntityName,
                                                         const xtstring& sName,
                                                         CUserOfObjectArray& cUsers)
{
  ROC_DeleteVariable(sIFS, sEntityName, sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteVariable(const xtstring& sIFS,
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
  // template file show name
  if (VariableExistInExpression(sEntityName, sName,
                                GetTemplateFileShowName().GetExprText(), GetTemplateFileShowName().GetExprValues(),
                                PositionsE, PositionsV))
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFileShowName,
                                        1,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFile,
                                        2,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TemplateFolder,
                                        3,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_Visibility,
                                        4,
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
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_IFS::GetTypeOfResponseObject(),
                                                       CSyncResponse_IFS::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_TagProperty,
                                        5,
                                        GetTag().GetExprText(),
                                        PositionsE,
                                        PositionsV);
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_DeleteVariable(const xtstring& sIFS,
                                             const xtstring& sEntityName,
                                             const xtstring& sName,
                                             CUserOfObject* pObject)
{
  assert(pObject->m_pSyncResponseObject == CSyncResponse_IFS::GetSyncResponseObject());
  if (pObject->m_pSyncResponseObject != CSyncResponse_IFS::GetSyncResponseObject())
    return;

  // template file
  if (pObject->m_nIdentify == 1)
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
  if (pObject->m_nIdentify == 2)
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
  if (pObject->m_nIdentify == 3)
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
  if (pObject->m_nIdentify == 4)
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
  if (pObject->m_nIdentify == 5)
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
void CMPModelTemplateInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_ChangeGlobalERTableName(const xtstring& sIFS,
                                                      const xtstring& sOldName,
                                                      const xtstring& sNewName,
                                                      CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS,
                                                              const xtstring& sName,
                                                              CUserOfObjectArray& cUsers)
{
  ROC_DeleteGlobalERTable(sIFS, sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                  const xtstring& sName,
                                                  CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_DeleteGlobalERTable(const xtstring& sIFS,
                                                  const xtstring& sName,
                                                  CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_ChangeWPName(const xtstring& sOldName,
                                           const xtstring& sNewName,
                                           CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // stone or template
  if (GetStoneOrTemplate() == sOldName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_StoneOrTemplate,
                                        1,
                                        GetStoneOrTemplate());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_ChangeWPName(const xtstring& sOldName,
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
void CMPModelTemplateInd::ROC_CheckReferencesForWP(const xtstring& sName,
                                                   CUserOfObjectArray& cUsers)
{
  ROC_DeleteWP(sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteWP(const xtstring& sName,
                                       CUserOfObjectArray& cUsers)
{
  if (!GetSyncGroup() || GetSyncGroup()->IsDefaultModuleGroup())
    return;

  // stone or template
  if (GetStoneOrTemplate() == sName)
  {
    // create object
    CUserOfObject* pResponseObject = new CUserOfObject(CUserOfObject::tMT_TemplateModule,
                                                       SE_MF_GetModuleName(),
                                                       _XT(""),
                                                       GetName(),
                                                       CSyncResponse_WP::GetTypeOfResponseObject(),
                                                       CSyncResponse_WP::GetSyncResponseObject());
    if (!pResponseObject)
      return;
    // set changes positions
    pResponseObject->SetChangePositions(tSyncProp_Template_StoneOrTemplate,
                                        1,
                                        GetStoneOrTemplate());
    cUsers.push_back(pResponseObject);
  }
}
void CMPModelTemplateInd::ROC_DeleteWP(const xtstring& sName,
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
void CMPModelTemplateInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_ChangeWPConstantName(const xtstring& sWPName,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName,
                                                   CUserOfObject* pObject)
{
}
void CMPModelTemplateInd::ROC_CheckReferencesForWPConstant(const xtstring& sWPName,
                                                           const xtstring& sName,
                                                           CUserOfObjectArray& cUsers)
{
  ROC_DeleteWPConstant(sWPName, sName, cUsers);
}
void CMPModelTemplateInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                               const xtstring& sName,
                                               CUserOfObjectArray& cUsers)
{
}
void CMPModelTemplateInd::ROC_DeleteWPConstant(const xtstring& sWPName,
                                               const xtstring& sName,
                                               CUserOfObject* pObject)
{
}
