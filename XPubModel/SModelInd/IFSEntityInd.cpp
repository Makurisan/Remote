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







/////////////////////////////////////////////////////////////////////////////
// CIFSEntityGlobalERTableInd
/////////////////////////////////////////////////////////////////////////////
CIFSEntityGlobalERTableInd::CIFSEntityGlobalERTableInd(CSyncIFSNotifiersGroup* pGroup)
                           :CModelBaseInd(tIFSEntityGlobalERTable),
                            CSyncNotifier_IFS(tIFSNotifier_GlobalERTable, pGroup),
                            CSyncResponse_ER(tRO_IFS_GlobalERTable)
{
  m_pOwnerEntity = 0;

  m_sName.clear();
  m_sERTableName.clear();
  m_bTableIsTemporary = false;

  m_sRealERTableName.clear();
  m_bInExecUsed = false;
}
CIFSEntityGlobalERTableInd::CIFSEntityGlobalERTableInd(CIFSEntityGlobalERTableInd& cGlobalERTable)
                           :CModelBaseInd(tIFSEntityGlobalERTable),
                            CSyncNotifier_IFS(tIFSNotifier_GlobalERTable, NULL),
                            CSyncResponse_ER(tRO_IFS_GlobalERTable)
{
  SetSyncGroup(cGlobalERTable.GetSyncGroup());
  SetIFSEntityGlobalERTableInd(&cGlobalERTable);
}
CIFSEntityGlobalERTableInd::CIFSEntityGlobalERTableInd(CIFSEntityGlobalERTableInd* pGlobalERTable)
                           :CModelBaseInd(tIFSEntityGlobalERTable),
                            CSyncNotifier_IFS(tIFSNotifier_GlobalERTable, NULL),
                            CSyncResponse_ER(tRO_IFS_GlobalERTable)
{
  assert(pGlobalERTable);
  SetSyncGroup(pGlobalERTable->GetSyncGroup());
  SetIFSEntityGlobalERTableInd(pGlobalERTable);
}
CIFSEntityGlobalERTableInd::~CIFSEntityGlobalERTableInd()
{
}
void CIFSEntityGlobalERTableInd::SetIFSEntityGlobalERTableInd(CIFSEntityGlobalERTableInd* pGlobalERTable)
{
  assert(pGlobalERTable);
  if (!pGlobalERTable)
    return;

  // Das duerfen wir nicht machen. Wenn wir zwischen Objekten einem Child Objekt kopieren moechten,
  // dann wuerden wir dem Child Objekt falschem Parent (Owner) zuweisen.
  m_pOwnerEntity = pGlobalERTable->m_pOwnerEntity;

  m_sName = pGlobalERTable->m_sName;
  m_sERTableName = pGlobalERTable->m_sERTableName;
  m_bTableIsTemporary = pGlobalERTable->m_bTableIsTemporary;

  m_sRealERTableName = pGlobalERTable->m_sRealERTableName;
  m_bInExecUsed = pGlobalERTable->m_bInExecUsed;
}
bool CIFSEntityGlobalERTableInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  if (pXMLDoc->AddChildElem(GLOBALERTABLE_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();

    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_GLOBALERTABLENAME, GetERTableName().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_TABLEISTEMPORARY, GetTableIsTemporary());

    pXMLDoc->OutOfElem();
    return true;
  }
  return false;
}
bool CIFSEntityGlobalERTableInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  SetName(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_NAME));
  SetERTableName(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_GLOBALERTABLENAME));
  SetTableIsTemporary(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_TABLEISTEMPORARY) != 0);

  return true;
}
xtstring CIFSEntityGlobalERTableInd::GetOwnerEntityName() const
{
  if (GetOwnerEntity())
    return GetOwnerEntity()->GetName();
  return _XT("");
}










/////////////////////////////////////////////////////////////////////////////
// CIFSEntityPrePostProcessInd
/////////////////////////////////////////////////////////////////////////////
CIFSEntityPrePostProcessInd::CIFSEntityPrePostProcessInd(bool bPreProcessObject,
                                                         CSyncIFSNotifiersGroup* pGroup)
                            :CModelBaseInd(tIFSEntityPreProcess),
                             CSyncNotifier_IFS(tIFSNotifier_PreProcess, pGroup),
                             CSyncResponse_Proj(tRO_IFS_PreProcess),
                             CSyncResponse_ER(tRO_IFS_PreProcess),
                             CSyncResponse_IFS(tRO_IFS_PreProcess),
                             CSyncResponse_WP(tRO_IFS_PreProcess),
                             m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, false)
{
  m_pOwnerEntity = 0;

  m_bPreProcessObject = bPreProcessObject;
  if (!m_bPreProcessObject)
  {
    SetTypeOfModel(tIFSEntityPostProcess);
    SetTypeOfNotifier(tIFSNotifier_PostProcess);
    CSyncResponse_Proj::SetTypeOfResponseObject(tRO_IFS_PostProcess);
    CSyncResponse_ER::SetTypeOfResponseObject(tRO_IFS_PostProcess);
    CSyncResponse_IFS::SetTypeOfResponseObject(tRO_IFS_PostProcess);
    CSyncResponse_WP::SetTypeOfResponseObject(tRO_IFS_PostProcess);
  }

  m_sName.clear();
  m_caVisibility.Reset();
  m_sPropertyStone.clear();
}
CIFSEntityPrePostProcessInd::CIFSEntityPrePostProcessInd(CIFSEntityPrePostProcessInd& cPProcess)
                            :CModelBaseInd(tIFSEntityPreProcess),
                             CSyncNotifier_IFS(tIFSNotifier_PreProcess, NULL),
                             CSyncResponse_Proj(tRO_IFS_PreProcess),
                             CSyncResponse_ER(tRO_IFS_PreProcess),
                             CSyncResponse_IFS(tRO_IFS_PreProcess),
                             CSyncResponse_WP(tRO_IFS_PreProcess),
                             m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, false)
{
  m_bPreProcessObject = cPProcess.m_bPreProcessObject;
  SetTypeOfModel(cPProcess.GetTypeOfModel());
  SetTypeOfNotifier(cPProcess.GetTypeOfNotifier());
  CSyncResponse_Proj::SetTypeOfResponseObject(cPProcess.CSyncResponse_Proj::GetTypeOfResponseObject());
  CSyncResponse_ER::SetTypeOfResponseObject(cPProcess.CSyncResponse_ER::GetTypeOfResponseObject());
  CSyncResponse_IFS::SetTypeOfResponseObject(cPProcess.CSyncResponse_IFS::GetTypeOfResponseObject());
  CSyncResponse_WP::SetTypeOfResponseObject(cPProcess.CSyncResponse_WP::GetTypeOfResponseObject());

  SetSyncGroup(cPProcess.GetSyncGroup());
  SetIFSEntityPrePostProcessInd(&cPProcess);
}
CIFSEntityPrePostProcessInd::CIFSEntityPrePostProcessInd(CIFSEntityPrePostProcessInd* pPProcess)
                            :CModelBaseInd(tIFSEntityPreProcess),
                             CSyncNotifier_IFS(tIFSNotifier_PreProcess, NULL),
                             CSyncResponse_Proj(tRO_IFS_PreProcess),
                             CSyncResponse_ER(tRO_IFS_PreProcess),
                             CSyncResponse_IFS(tRO_IFS_PreProcess),
                             CSyncResponse_WP(tRO_IFS_PreProcess),
                             m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, false)
{
  assert(pPProcess);

  m_bPreProcessObject = pPProcess->m_bPreProcessObject;
  SetTypeOfModel(pPProcess->GetTypeOfModel());
  SetTypeOfNotifier(pPProcess->GetTypeOfNotifier());
  CSyncResponse_Proj::SetTypeOfResponseObject(pPProcess->CSyncResponse_Proj::GetTypeOfResponseObject());
  CSyncResponse_ER::SetTypeOfResponseObject(pPProcess->CSyncResponse_ER::GetTypeOfResponseObject());
  CSyncResponse_IFS::SetTypeOfResponseObject(pPProcess->CSyncResponse_IFS::GetTypeOfResponseObject());
  CSyncResponse_WP::SetTypeOfResponseObject(pPProcess->CSyncResponse_WP::GetTypeOfResponseObject());

  SetSyncGroup(pPProcess->GetSyncGroup());
  SetIFSEntityPrePostProcessInd(pPProcess);
}
CIFSEntityPrePostProcessInd::~CIFSEntityPrePostProcessInd()
{
}
void CIFSEntityPrePostProcessInd::SetIFSEntityPrePostProcessInd(CIFSEntityPrePostProcessInd* pPProcess)
{
  assert(pPProcess);
  if (!pPProcess)
    return;

  // Das duerfen wir nicht machen. Wenn wir zwischen Objekten einem Child Objekt kopieren moechten,
  // dann wuerden wir dem Child Objekt falschem Parent (Owner) zuweisen.
  m_pOwnerEntity = pPProcess->m_pOwnerEntity;

  m_sName = pPProcess->m_sName;
  m_caVisibility = pPProcess->m_caVisibility;
  m_sPropertyStone = pPProcess->m_sPropertyStone;
}
bool CIFSEntityPrePostProcessInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet;
  if (m_bPreProcessObject)
    bRet = pXMLDoc->AddChildElem(PREPROCESS_ELEM_NAME, _XT(""));
  else
    bRet = pXMLDoc->AddChildElem(POSTPROCESS_ELEM_NAME, _XT(""));
  if (bRet)
  {
    pXMLDoc->IntoElem();
    // in <HitName>
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_NAME, GetName().c_str());

    m_caVisibility.SaveXMLContent(pXMLDoc);

    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_PREPOSTPROCESSSTONE, GetPropertyStone().c_str());

    pXMLDoc->OutOfElem();
    return true;
  }
  return false;
}
bool CIFSEntityPrePostProcessInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  SetName(xtstring(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_NAME)));

  m_caVisibility.ReadXMLContent(pXMLDoc);

  SetPropertyStone(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_PREPOSTPROCESSSTONE));

  return true;
}
xtstring CIFSEntityPrePostProcessInd::GetOwnerEntityName() const
{
  if (GetOwnerEntity())
    return GetOwnerEntity()->GetName();
  return _XT("");
}


/////////////////////////////////////////////////////////////////////////////
// CIFSEntityVariableInd
/////////////////////////////////////////////////////////////////////////////
CIFSEntityVariableInd::CIFSEntityVariableInd(CSyncIFSNotifiersGroup* pGroup)
                      :CModelBaseInd(tIFSEntityVariable),
                       CSyncNotifier_IFS(tIFSNotifier_Variable, pGroup),
                       CSyncResponse_Common(tRO_IFS_Variable)
{
  m_pOwnerEntity = 0;

  m_sName.erase();
  m_sComment.erase();
  m_nType = tFieldTypeInteger;
  m_nDefinedSize = 20;
}

CIFSEntityVariableInd::CIFSEntityVariableInd(CIFSEntityVariableInd& cVar)
                      :CModelBaseInd(tIFSEntityVariable),
                       CSyncNotifier_IFS(tIFSNotifier_Variable, NULL),
                       CSyncResponse_Common(tRO_IFS_Variable)
{
  SetSyncGroup(cVar.GetSyncGroup());
  SetIFSEntityVariableInd(&cVar);
}

CIFSEntityVariableInd::CIFSEntityVariableInd(CIFSEntityVariableInd* pVar)
                      :CModelBaseInd(tIFSEntityVariable),
                       CSyncNotifier_IFS(tIFSNotifier_Variable, NULL),
                       CSyncResponse_Common(tRO_IFS_Variable)
{
  assert(pVar);
  SetSyncGroup(pVar->GetSyncGroup());
  SetIFSEntityVariableInd(pVar);
}

CIFSEntityVariableInd::~CIFSEntityVariableInd()
{
}

void CIFSEntityVariableInd::SetIFSEntityVariableInd(CIFSEntityVariableInd* pVar)
{
  if (!pVar)
    return;

  // Das duerfen wir nicht machen. Wenn wir zwischen Objekten einem Child Objekt kopieren moechten,
  // dann wuerden wir dem Child Objekt falschem Parent (Owner) zuweisen.
  m_pOwnerEntity = pVar->m_pOwnerEntity;

  m_sName = pVar->m_sName;
  m_sComment = pVar->m_sComment;
  m_nType = pVar->m_nType;
  m_nDefinedSize = pVar->m_nDefinedSize;
}

xtstring CIFSEntityVariableInd::GetOwnerEntityName() const
{
  if (GetOwnerEntity())
    return GetOwnerEntity()->GetName();
  return _XT("");
}

bool CIFSEntityVariableInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(VARIABLE_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Variable>
    pXMLDoc->SetAttrib(VARIABLE_PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(VARIABLE_PROPERTY_ATTRIB_COMMENT, GetComment().c_str());
    pXMLDoc->SetAttrib(VARIABLE_PROPERTY_ATTRIB_TYPE, GetType());
    pXMLDoc->SetAttrib(VARIABLE_PROPERTY_ATTRIB_DEFINEDSIZE, GetDefinedSize());

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CIFSEntityVariableInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Variable>
  SetName(xtstring(pXMLDoc->GetAttrib(VARIABLE_PROPERTY_ATTRIB_NAME)));
  SetComment(xtstring(pXMLDoc->GetAttrib(VARIABLE_PROPERTY_ATTRIB_COMMENT)));
  SetType((TFieldType)pXMLDoc->GetAttribLong(VARIABLE_PROPERTY_ATTRIB_TYPE));
  SetDefinedSize(pXMLDoc->GetAttribLong(VARIABLE_PROPERTY_ATTRIB_DEFINEDSIZE));
  bRet = true;

  return bRet;
}



/////////////////////////////////////////////////////////////////////////////
// CIFSEntityAssignmentInd
/////////////////////////////////////////////////////////////////////////////
CIFSEntityAssignmentInd::CIFSEntityAssignmentInd(CSyncIFSNotifiersGroup* pGroup)
                        :CModelBaseInd(tIFSEntityAssignment),
                         CSyncNotifier_IFS(tIFSNotifier_Assignment, pGroup),
                         CSyncResponse_Proj(tRO_IFS_Assignment),
                         CSyncResponse_ER(tRO_IFS_Assignment),
                         CSyncResponse_IFS(tRO_IFS_Assignment),
                         m_caAssignment(CConditionOrAssignment::tCOAT_Assignment_Assignment, false),
                         m_caAssignField(CConditionOrAssignment::tCOAT_AssignField_Assignment, false),
                         m_caAssignValue(CConditionOrAssignment::tCOAT_AssignValue_Assignment, false)
{
  m_pOwnerEntity = 0;

  m_sName.erase();
  m_caAssignment.Reset();
  m_sEntityOfInformationSight.erase();
  m_caAssignField.Reset();
  m_caAssignValue.Reset();
}
CIFSEntityAssignmentInd::CIFSEntityAssignmentInd(CIFSEntityAssignmentInd& cModel)
                        :CModelBaseInd(tMPModelText),
                         CSyncNotifier_IFS(tIFSNotifier_Assignment, NULL),
                         CSyncResponse_Proj(tRO_IFS_Assignment),
                         CSyncResponse_ER(tRO_IFS_Assignment),
                         CSyncResponse_IFS(tRO_IFS_Assignment),
                         m_caAssignment(CConditionOrAssignment::tCOAT_Assignment_Assignment, false),
                         m_caAssignField(CConditionOrAssignment::tCOAT_AssignField_Assignment, false),
                         m_caAssignValue(CConditionOrAssignment::tCOAT_AssignValue_Assignment, false)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetIFSEntityAssignmentInd(&cModel);
}
CIFSEntityAssignmentInd::CIFSEntityAssignmentInd(CIFSEntityAssignmentInd* pModel)
                        :CModelBaseInd(tMPModelText),
                         CSyncNotifier_IFS(tIFSNotifier_Assignment, NULL),
                         CSyncResponse_Proj(tRO_IFS_Assignment),
                         CSyncResponse_ER(tRO_IFS_Assignment),
                         CSyncResponse_IFS(tRO_IFS_Assignment),
                         m_caAssignment(CConditionOrAssignment::tCOAT_Assignment_Assignment, false),
                         m_caAssignField(CConditionOrAssignment::tCOAT_AssignField_Assignment, false),
                         m_caAssignValue(CConditionOrAssignment::tCOAT_AssignValue_Assignment, false)
{
  assert(pModel);
  SetSyncGroup(pModel->GetSyncGroup());
  SetIFSEntityAssignmentInd(pModel);
}
CIFSEntityAssignmentInd::~CIFSEntityAssignmentInd()
{
}
void CIFSEntityAssignmentInd::SetIFSEntityAssignmentInd(CIFSEntityAssignmentInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  // Das duerfen wir nicht machen. Wenn wir zwischen Objekten einem Child Objekt kopieren moechten,
  // dann wuerden wir dem Child Objekt falschem Parent (Owner) zuweisen.
  m_pOwnerEntity = pModel->m_pOwnerEntity;

  m_sName = pModel->m_sName;
  m_caAssignment = pModel->m_caAssignment;
  m_sEntityOfInformationSight = pModel->m_sEntityOfInformationSight;
  m_caAssignField = pModel->m_caAssignField;
  m_caAssignValue = pModel->m_caAssignValue;
}
xtstring CIFSEntityAssignmentInd::GetOwnerEntityName() const
{
  if (GetOwnerEntity())
    return GetOwnerEntity()->GetName();
  return _XT("");
}
bool CIFSEntityAssignmentInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(ASSIGNMENT_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Text>
    pXMLDoc->SetAttrib(ASSIGNMENT_PROPERTY_ATTRIB_NAME, GetName().c_str());
    m_caAssignment.SaveXMLContent(pXMLDoc);
    pXMLDoc->SetAttrib(ASSIGNMENT_PROPERTY_ATTRIB_TABLE, GetEntityOfInformationSight().c_str());
    m_caAssignField.SaveXMLContent(pXMLDoc);
    m_caAssignValue.SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}
bool CIFSEntityAssignmentInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Text>
  SetName(xtstring(pXMLDoc->GetAttrib(ASSIGNMENT_PROPERTY_ATTRIB_NAME)));
  m_caAssignment.ReadXMLContent(pXMLDoc);
  SetEntityOfInformationSight(xtstring(pXMLDoc->GetAttrib(ASSIGNMENT_PROPERTY_ATTRIB_TABLE)));
  m_caAssignField.ReadXMLContent(pXMLDoc);
  m_caAssignValue.ReadXMLContent(pXMLDoc);
  bRet = true;

  return bRet;
}








//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityFieldInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CIFSEntityFieldInd::CIFSEntityFieldInd(CSyncIFSNotifiersGroup* pGroup)
                   :CModelBaseInd(tIFSEntityField),
                    CSyncNotifier_IFS(tIFSNotifier_Field, pGroup)
{
  m_sName = _XT("Field");
  m_sComment.erase();
}

CIFSEntityFieldInd::CIFSEntityFieldInd(CIFSEntityFieldInd& cField)
                   :CModelBaseInd(tIFSEntityField),
                    CSyncNotifier_IFS(tIFSNotifier_Field, NULL)
{
  SetSyncGroup(cField.GetSyncGroup());
  SetIFSEntityFieldInd(&cField);
}

CIFSEntityFieldInd::CIFSEntityFieldInd(CIFSEntityFieldInd* pField)
                   :CModelBaseInd(tIFSEntityField),
                    CSyncNotifier_IFS(tIFSNotifier_Field, NULL)
{
  assert(pField);
  SetSyncGroup(pField->GetSyncGroup());
  SetIFSEntityFieldInd(pField);
}

CIFSEntityFieldInd::~CIFSEntityFieldInd()
{
}

void CIFSEntityFieldInd::SetIFSEntityFieldInd(CIFSEntityFieldInd* pField)
{
  if (!pField)
    return;

  m_sName = pField->m_sName;
  m_sComment = pField->m_sComment;
}

bool CIFSEntityFieldInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(FIELD_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Field>
    pXMLDoc->SetAttrib(FIELD_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_COMMENT, GetComment().c_str());

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CIFSEntityFieldInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Field>
  SetName(xtstring(pXMLDoc->GetAttrib(FIELD_ATTRIB_NAME)));
  SetComment(xtstring(pXMLDoc->GetAttrib(FIELD_ATTRIB_COMMENT)));
  bRet = true;

  return bRet;
}

xtstring CIFSEntityFieldInd::GetNameForShow()
{
  return m_sName;
}















//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityConstantForTextForm
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSEntityConstantForTextForm::CIFSEntityConstantForTextForm()
{
  m_sName.clear();
  m_nFieldType = tFieldTypeEmpty;
  m_sFormat.clear();
  m_nMaxChars = -1;  // -1 no max
  m_cBackground = 0xffffff; // white
  m_cForeground = 0;        // black
  m_bAllowNull = false;
  m_bBorder = true;
  m_sDefaultText.clear();
  m_sFontName = _XT("Arial");
  m_nFontHeight = -13;
  // diese Variable hat zu Entwicklungszeit keine Wirkung
  // und gar nicht dargestellt
  m_sValue.clear();
  // help variables
  m_nCompoundIndexInSymbolsValuesOffsets = -1;
}

CIFSEntityConstantForTextForm::~CIFSEntityConstantForTextForm()
{
}

bool CIFSEntityConstantForTextForm::AssignFrom(const CIFSEntityConstantForTextForm* pConstant)
{
  assert(pConstant);
  if (!pConstant)
    return false;

  m_sName = pConstant->m_sName;
  m_nFieldType = pConstant->m_nFieldType;
  m_sFormat = pConstant->m_sFormat;
  m_nMaxChars = pConstant->m_nMaxChars;
  m_cBackground = pConstant->m_cBackground;
  m_cForeground = pConstant->m_cForeground;
  m_bAllowNull = pConstant->m_bAllowNull;
  m_bBorder = pConstant->m_bBorder;
  m_sDefaultText = pConstant->m_sDefaultText;
  m_sFontName = pConstant->m_sFontName;
  m_nFontHeight = pConstant->m_nFontHeight;
  m_sValue = pConstant->m_sValue;
  m_nCompoundIndexInSymbolsValuesOffsets = pConstant->m_nCompoundIndexInSymbolsValuesOffsets;

  return true;
}

bool CIFSEntityConstantForTextForm::CopyFrom(const CIFSEntityConstantForTextForm* pConstant, bool& bChanged)
{
  bChanged = false;

  if (!pConstant)
    return false;

  if (m_sName != pConstant->m_sName)
    bChanged = true;
  else if (m_nFieldType != pConstant->m_nFieldType)
    bChanged = true;
  else if (m_sFormat != pConstant->m_sFormat)
    bChanged = true;
  else if (m_nMaxChars != pConstant->m_nMaxChars)
    bChanged = true;
  else if (m_cBackground != pConstant->m_cBackground)
    bChanged = true;
  else if (m_cForeground != pConstant->m_cForeground)
    bChanged = true;
  else if (m_bAllowNull != pConstant->m_bAllowNull)
    bChanged = true;
  else if (m_bBorder != pConstant->m_bBorder)
    bChanged = true;
  else if (m_sDefaultText != pConstant->m_sDefaultText)
    bChanged = true;
  else if (m_sFontName != pConstant->m_sFontName)
    bChanged = true;
  else if (m_nFontHeight != pConstant->m_nFontHeight)
    bChanged = true;
  else if (m_sValue != pConstant->m_sValue)
    bChanged = true;
  else if (m_nCompoundIndexInSymbolsValuesOffsets != pConstant->m_nCompoundIndexInSymbolsValuesOffsets)
    bChanged = true;

  return AssignFrom(pConstant);
}

bool CIFSEntityConstantForTextForm::operator == (const CIFSEntityConstantForTextForm& cConstant) const
{
  if (m_sName != cConstant.m_sName)
    return false;
  if (m_nFieldType != cConstant.m_nFieldType)
    return false;
  if (m_sFormat != cConstant.m_sFormat)
    return false;
  if (m_nMaxChars != cConstant.m_nMaxChars)
    return false;
  if (m_cBackground != cConstant.m_cBackground)
    return false;
  if (m_cForeground != cConstant.m_cForeground)
    return false;
  if (m_bAllowNull != cConstant.m_bAllowNull)
    return false;
  if (m_bBorder != cConstant.m_bBorder)
    return false;
  if (m_sDefaultText != cConstant.m_sDefaultText)
    return false;
  if (m_sFontName != cConstant.m_sFontName)
    return false;
  if (m_nFontHeight != cConstant.m_nFontHeight)
    return false;
  if (m_sValue != cConstant.m_sValue)
    return false;
  if (m_nCompoundIndexInSymbolsValuesOffsets != cConstant.m_nCompoundIndexInSymbolsValuesOffsets)
    return false;

  return true;
}

bool CIFSEntityConstantForTextForm::operator != (const CIFSEntityConstantForTextForm& cConstant) const
{
  return !(operator == (cConstant));
}

bool CIFSEntityConstantForTextForm::IsIdenticalForXPubClient(const CIFSEntityConstantForTextForm* pConst) const
{
  assert(pConst);
  if (!pConst)
    return false;

  if (m_sName != pConst->m_sName)
    return false;
  if (m_nFieldType != pConst->m_nFieldType)
    return false;
  if (m_sFormat != pConst->m_sFormat)
    return false;
  if (m_nMaxChars != pConst->m_nMaxChars)
    return false;
  if (m_cBackground != pConst->m_cBackground)
    return false;
  if (m_cForeground != pConst->m_cForeground)
    return false;
  if (m_bAllowNull != pConst->m_bAllowNull)
    return false;
  if (m_bBorder != pConst->m_bBorder)
    return false;
  if (m_sDefaultText != pConst->m_sDefaultText)
    return false;
  if (m_sFontName != pConst->m_sFontName)
    return false;
  if (m_nFontHeight != pConst->m_nFontHeight)
    return false;
//  if (m_sValue != m_sValue)
//    return false;
  if (m_nCompoundIndexInSymbolsValuesOffsets != pConst->m_nCompoundIndexInSymbolsValuesOffsets)
    return false;
  return true;
}

#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_NAME              _XT("Name")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FIELDTYPE         _XT("FieldType")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FORMAT            _XT("Format")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_MAXCHARS          _XT("MaxChars")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_BACKGROUND        _XT("Background")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FOREGROUND        _XT("Foreground")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_ALLOWNULL         _XT("AllowNull")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_BORDER            _XT("Border")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_DEFAULTTEXT       _XT("DefaultText")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FONTNAME          _XT("FontName")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FONTHEIGHT        _XT("FontHeight")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_VALUE             _XT("Value")
#define XMLTAG_ONECONDVARIATION_TEXTFORMCONST_COMPINDINSYMBOLS  _XT("CompIndInSymbValOffs")

bool CIFSEntityConstantForTextForm::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  // XMLDoc in <irgendwo>
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_NAME, m_sName.c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FIELDTYPE, m_nFieldType);
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FORMAT, m_sFormat.c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_MAXCHARS, m_nMaxChars);
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_BACKGROUND, m_cBackground);
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FOREGROUND, m_cForeground);
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_ALLOWNULL, m_bAllowNull);
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_BORDER, m_bBorder);
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_DEFAULTTEXT, m_sDefaultText.c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FONTNAME, m_sFontName.c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FONTHEIGHT, m_nFontHeight);
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_VALUE, m_sValue.c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_COMPINDINSYMBOLS, m_nCompoundIndexInSymbolsValuesOffsets);

  return true;
}

bool CIFSEntityConstantForTextForm::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  m_sName         = pXMLDoc->GetAttrib    (XMLTAG_ONECONDVARIATION_TEXTFORMCONST_NAME);
  m_nFieldType    = (TFieldType)pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FIELDTYPE);
  m_sFormat       = pXMLDoc->GetAttrib    (XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FORMAT);
  m_nMaxChars     = pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_MAXCHARS);
  m_cBackground   = pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_BACKGROUND);
  m_cForeground   = pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FOREGROUND);
  m_bAllowNull    = pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_ALLOWNULL) != 0;
  m_bBorder       = pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_BORDER) != 0;
  m_sDefaultText  = pXMLDoc->GetAttrib    (XMLTAG_ONECONDVARIATION_TEXTFORMCONST_DEFAULTTEXT);
  m_sFontName     = pXMLDoc->GetAttrib    (XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FONTNAME);
  m_nFontHeight   = pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_FONTHEIGHT);
  m_sValue        = pXMLDoc->GetAttrib    (XMLTAG_ONECONDVARIATION_TEXTFORMCONST_VALUE);
  m_nCompoundIndexInSymbolsValuesOffsets  = pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_TEXTFORMCONST_COMPINDINSYMBOLS);

  return true;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityOneConditionVariation
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#define CONSTANT_LEFT_TOKEN     _XT("<")
#define CONSTANT_RIGHT_TOKEN    _XT(">")
#define CONSTANT_BASE_TOKEN     _XT("Konstante")
#define CONSTANT_NUMBER_TOKEN   _XT("%ld")
#define CONSTANT_FORMAT_BASE    CONSTANT_BASE_TOKEN CONSTANT_NUMBER_TOKEN
#define CONSTANT_FORMAT_FULL    CONSTANT_LEFT_TOKEN CONSTANT_FORMAT_BASE + CONSTANT_RIGHT_TOKEN

CIFSEntityOneConditionVariation::CIFSEntityOneConditionVariation(CSyncIFSNotifiersGroup* pGroup,
                                                                 CIFSEntityConditionVariations* pParent)
                                :CModelBaseInd(tIFSEntityOneCondition),
                                 CSyncNotifier_IFS(tIFSNotifier_OneCondition, pGroup),
                                 CSyncResponse_Proj(tRO_IFS_OneCondition),
                                 CSyncResponse_ER(tRO_IFS_OneCondition),
                                 CSyncResponse_IFS(tRO_IFS_OneCondition),
                                 m_caCondition(CConditionOrAssignment::tCOAT_OneConditionVariation_Condition, false)
{
  m_pParent = pParent;
  m_sName.clear();
  m_sDescription.clear();
  m_caCondition.Reset();
  m_bDontOptimizeCondition = false;
  m_bDontShowCondition = false;
  m_nConditionShowMethod = tConditionShowMethod_Default;
  m_sTextFormOfCondition.clear();
}
CIFSEntityOneConditionVariation::CIFSEntityOneConditionVariation(CIFSEntityOneConditionVariation& cOneCond)
                                :CModelBaseInd(tIFSEntityOneCondition),
                                 CSyncNotifier_IFS(tIFSNotifier_OneCondition, NULL),
                                 CSyncResponse_Proj(tRO_IFS_OneCondition),
                                 CSyncResponse_ER(tRO_IFS_OneCondition),
                                 CSyncResponse_IFS(tRO_IFS_OneCondition),
                                 m_caCondition(CConditionOrAssignment::tCOAT_OneConditionVariation_Condition, false)
{
  SetSyncGroup(cOneCond.GetSyncGroup());
  AssignFrom(&cOneCond);
}
CIFSEntityOneConditionVariation::CIFSEntityOneConditionVariation(CIFSEntityOneConditionVariation* pOneCond)
                                :CModelBaseInd(tIFSEntityOneCondition),
                                 CSyncNotifier_IFS(tIFSNotifier_OneCondition, NULL),
                                 CSyncResponse_Proj(tRO_IFS_OneCondition),
                                 CSyncResponse_ER(tRO_IFS_OneCondition),
                                 CSyncResponse_IFS(tRO_IFS_OneCondition),
                                 m_caCondition(CConditionOrAssignment::tCOAT_OneConditionVariation_Condition, false)
{
  assert(pOneCond);
  SetSyncGroup(pOneCond->GetSyncGroup());
  AssignFrom(pOneCond);
}
CIFSEntityOneConditionVariation::~CIFSEntityOneConditionVariation()
{
  RemoveAllConstants();
}

bool CIFSEntityOneConditionVariation::AssignFrom(const CIFSEntityOneConditionVariation* pOneCond)
{
  assert(pOneCond);
  if (!pOneCond)
    return false;

  m_sName = pOneCond->m_sName;
  m_sDescription = pOneCond->m_sDescription;
  m_caCondition = pOneCond->m_caCondition;
  m_bDontOptimizeCondition = pOneCond->m_bDontOptimizeCondition;
  m_bDontShowCondition = pOneCond->m_bDontShowCondition;
  m_nConditionShowMethod = pOneCond->m_nConditionShowMethod;
  m_sTextFormOfCondition = pOneCond->m_sTextFormOfCondition;

  RemoveAllConstants();
  for (size_t nI = 0; nI < pOneCond->CountOfConstants(); nI++)
    AddConstant(pOneCond->GetConstant(nI));

  return true;
}
bool CIFSEntityOneConditionVariation::CopyFrom(const CIFSEntityOneConditionVariation* pOneCond,
                                               bool& bChanged)
{
  bChanged = false;

  if (!pOneCond)
    return false;

  if (m_sName != pOneCond->m_sName)
    bChanged = true;
  else if (m_sDescription != pOneCond->m_sDescription)
    bChanged = true;
  else if (m_caCondition != pOneCond->m_caCondition)
    bChanged = true;
  else if (m_bDontOptimizeCondition != pOneCond->m_bDontOptimizeCondition)
    bChanged = true;
  else if (m_bDontShowCondition != pOneCond->m_bDontShowCondition)
    bChanged = true;
  else if (m_nConditionShowMethod != pOneCond->m_nConditionShowMethod)
    bChanged = true;
  else if (m_sTextFormOfCondition != pOneCond->m_sTextFormOfCondition)
    bChanged = true;

  if (CountOfConstants() != pOneCond->CountOfConstants())
    bChanged = true;
  else
  {
    for (size_t nI = 0; nI < pOneCond->CountOfConstants(); nI++)
    {
      if ((*GetConstant(nI)) != (*pOneCond->GetConstant(nI)))
      {
        bChanged = true;
        break;
      }
    }
  }

  return AssignFrom(pOneCond);
}
bool CIFSEntityOneConditionVariation::operator == (const CIFSEntityOneConditionVariation& cOneCond) const
{
  if (m_sName != cOneCond.m_sName)
    return false;
  else if (m_sDescription != cOneCond.m_sDescription)
    return false;
  else if (m_caCondition != cOneCond.m_caCondition)
    return false;
  else if (m_bDontOptimizeCondition != cOneCond.m_bDontOptimizeCondition)
    return false;
  else if (m_bDontShowCondition != cOneCond.m_bDontShowCondition)
    return false;
  else if (m_nConditionShowMethod != cOneCond.m_nConditionShowMethod)
    return false;
  else if (m_sTextFormOfCondition != cOneCond.m_sTextFormOfCondition)
    return false;

  if (CountOfConstants() != cOneCond.CountOfConstants())
    return false;
  else
  {
    for (size_t nI = 0; nI < cOneCond.CountOfConstants(); nI++)
    {
      if ((*GetConstant(nI)) != (*cOneCond.GetConstant(nI)))
      {
        return false;
      }
    }
  }

  return true;
}
bool CIFSEntityOneConditionVariation::operator != (const CIFSEntityOneConditionVariation& cOneCond) const
{
  return !(operator == (cOneCond));
}

bool CIFSEntityOneConditionVariation::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  // XMLDoc in <irgendwo>
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_NAME,        GetName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_DESCRIPTION, GetDescription().c_str());
  m_caCondition.SaveXMLContent(pXMLDoc);
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_DONTOPTIMIZECONDITION, GetDontOptimizeCondition());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_DONTSHOWCONDITION, GetDontShowCondition());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_CONDITIONSHOWMETHOD, GetConditionShowMethod());
  pXMLDoc->SetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMOFCONDITION, GetTextFormOfCondition().c_str());

  if (pXMLDoc->AddChildElem(XMLTAG_ONECONDVARIATION_CONSTANTS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <Constants>
    for (size_t nI = 0; nI < CountOfConstants(); nI++)
    {
      if (pXMLDoc->AddChildElem(XMLTAG_ONECONDVARIATION_CONSTANT, _XT("")))
      {
        pXMLDoc->IntoElem();
        // XMLDoc in <Constant>
        m_cConstants[nI]->SaveXMLContent(pXMLDoc);
        pXMLDoc->OutOfElem();
        // XMLDoc in <Constants>
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }

  return true;
}
bool CIFSEntityOneConditionVariation::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  // XMLDoc in <irgendwo>
  SetName                         (pXMLDoc->GetAttrib(XMLTAG_ONECONDVARIATION_NAME));
  SetDescription                  (pXMLDoc->GetAttrib(XMLTAG_ONECONDVARIATION_DESCRIPTION));
  m_caCondition.ReadXMLContent(pXMLDoc);
  SetDontOptimizeCondition        (pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_DONTOPTIMIZECONDITION) != 0);
  SetDontShowCondition            (pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_DONTSHOWCONDITION) != 0);
  SetConditionShowMethod          ((TConditionShowMethod)pXMLDoc->GetAttribLong(XMLTAG_ONECONDVARIATION_CONDITIONSHOWMETHOD));
  SetTextFormOfCondition          (pXMLDoc->GetAttrib(XMLTAG_ONECONDVARIATION_TEXTFORMOFCONDITION));

  if (GetConditionShowMethod() <= tConditionShowMethod_FirstDummy
      || GetConditionShowMethod() >= tConditionShowMethod_LastDummy)
    SetConditionShowMethod(tConditionShowMethod_Default);

  RemoveAllConstants();

  if (pXMLDoc->FindChildElem(XMLTAG_ONECONDVARIATION_CONSTANTS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <Constants>

    while (pXMLDoc->FindChildElem(XMLTAG_ONECONDVARIATION_CONSTANT))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <Constant>
      CIFSEntityConstantForTextForm* pConstant;
      pConstant = new CIFSEntityConstantForTextForm;
      if (pConstant)
      {
        if (pConstant->ReadXMLContent(pXMLDoc))
          m_cConstants.push_back(pConstant);
        else
          delete pConstant;
      }
      pXMLDoc->OutOfElem();
      // XMLDoc in <Constants>
    }
    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }

  return true;
}

void CIFSEntityOneConditionVariation::RemoveAllConstants()
{
  for (CIFSEntityTextFormConstantsIterator it = m_cConstants.begin(); it != m_cConstants.end(); it++)
    delete (*it);
  m_cConstants.erase(m_cConstants.begin(), m_cConstants.end());
}

void CIFSEntityOneConditionVariation::RemoveLastConstant()
{
  if (m_cConstants.size() == 0)
    return;
  CIFSEntityTextFormConstantsIterator it = m_cConstants.end();
  it--;
  delete (*it);
  m_cConstants.erase(it);
}

xtstring CIFSEntityOneConditionVariation::GetNewNameForConstant()
{
  XTCHAR sName[100];
  xtstring sNewName;
  for (size_t nI = 1;; nI++)
  {
    sprintf(sName, CONSTANT_FORMAT_BASE, nI);
    sNewName = sName;
    CIFSEntityConstantForTextForm* pC = GetConstant(sNewName);
    if (!pC)
      break;
  }

  return sNewName;
}

xtstring CIFSEntityOneConditionVariation::GetNameForConstant(int nIndex)
{
  XTCHAR sName[100];
  xtstring sNewName;

  sprintf(sName, CONSTANT_FORMAT_BASE, nIndex);
  sNewName = sName;

  return sNewName;
}

xtstring CIFSEntityOneConditionVariation::GetConstantNameForTextForm(const xtstring& sConstantName) const
{
  xtstring sName;
  sName = CONSTANT_LEFT_TOKEN;
  sName += sConstantName;
  sName += CONSTANT_RIGHT_TOKEN;
  return sName;
}

bool CIFSEntityOneConditionVariation::GetPositionOfNameForTextFormInTextForm(const xtstring& sConstantNameForTextForm,
                                                                             int& nPos,
                                                                             int& nLen)
{
  nPos = -1;
  nLen = 0;
  size_t n = m_sTextFormOfCondition.find(sConstantNameForTextForm);
  if (n == xtstring::npos)
    return false;

  nPos = (int)n;
  nLen = (int)sConstantNameForTextForm.size();
  return true;
}

bool CIFSEntityOneConditionVariation::GetPositionOfConstantInExpression(const xtstring& sConstantName,
                                                                        int& nPos,
                                                                        int& nLen)
{
  nPos = -1;
  nLen = 0;
  CIFSEntityConstantForTextForm* pC = GetConstant(sConstantName);
  if (!pC)
    return false;

  return GetPosAndLenOfConstantInExprOffsets(m_caCondition.GetExprOffsets(), pC->m_nCompoundIndexInSymbolsValuesOffsets, nPos, nLen);
}

void CIFSEntityOneConditionVariation::ExpressionChanged()
{
  CintVector cPositions;
  xtstring sExprSymbols;
  xtstring sExprValues;
  xtstring sExprOffsets;

  sExprSymbols = m_caCondition.GetExprSymbols();
  sExprValues = m_caCondition.GetExprValues();
  sExprOffsets = m_caCondition.GetExprOffsets();

  // spezielle Funktion, die alle Konstanten findet
  // und fur jede Konstante spezielles Index erzeugt
  FindAllConstantsInExprSymbols(sExprSymbols, cPositions);

  // jetzt haben wir in cPositions alle Positionen, wo sich eine Konstante befindet
  // Anzahl Konstanten korrigieren
  if (cPositions.size() != CountOfConstants())
  {
    // Anzahl den Konstanten wurde geaendert
    if (cPositions.size() < CountOfConstants())
    {
      // wir muessen was loeschen
      size_t nOldCount = CountOfConstants();
      for (size_t nI = cPositions.size(); nI < nOldCount; nI++)
        RemoveLastConstant();
    }
    else
    {
      // wir muessen was erzeugen
      for (size_t nI = CountOfConstants(); nI < cPositions.size(); nI++)
      {
        CIFSEntityConstantForTextForm* pNew;
        pNew = new CIFSEntityConstantForTextForm;
        if (!pNew)
          continue;
        pNew->m_sName = GetNewNameForConstant();
        m_cConstants.push_back(pNew);
      }
    }
  }

  // nur fuer Sicherheit
  assert(CountOfConstants() == cPositions.size());

  // jetzt muessen wir die POS Variablen in Ordnung bringen
  // und gleichzeitig TextForm
  xtstring sNewTextForm;
  sNewTextForm = GetTextFormOfCondition();
  for (size_t nI = 0; nI < CountOfConstants(); nI++)
  {
    CIFSEntityConstantForTextForm* pC = GetConstant(nI);
    if (!pC)
      continue;
    if (pC->m_sDefaultText.size() == 0)
    {
      pC->m_sDefaultText = GetValueFromExprValues(sExprValues, cPositions[nI]);
      pC->m_sValue = pC->m_sDefaultText;
    }
    if (pC->m_nFieldType == tFieldTypeEmpty)
      pC->m_nFieldType = GetFieldTypeFromExprSymbols(sExprSymbols, cPositions[nI]);
    pC->m_nCompoundIndexInSymbolsValuesOffsets = cPositions[nI];

    xtstring sNameForTextForm = GetConstantNameForTextForm(pC->m_sName);
    size_t nPos = sNewTextForm.find(sNameForTextForm);
    if (nPos == xtstring::npos)
    {
      if (sNewTextForm.size())
        sNewTextForm += _XT(" ");
      sNewTextForm += sNameForTextForm;
    }
  }

  // jetzt probieren wir von Text Form alle Konstanten loeschen,
  // die wir nicht mehr haben
  // wir werden einfach bis 100 suchen
  for (int nIndex = 1; nIndex < 100; nIndex++)
  {
    xtstring sCName = GetNameForConstant(nIndex);
    CIFSEntityConstantForTextForm* pC = GetConstant(sCName);
    if (pC)
      // existiert, so lassen
      continue;

    xtstring sNameForTextForm = GetConstantNameForTextForm(sCName);
    size_t nPos = sNewTextForm.find(sNameForTextForm);
    if (nPos != xtstring::npos)
      sNewTextForm.erase(nPos, sNameForTextForm.size());
  }

  // und Text Form setzen
  SetTextFormOfCondition(sNewTextForm);
}

bool CIFSEntityOneConditionVariation::IsIdenticalForXPubClient(const CIFSEntityOneConditionVariation* pVariation) const
{
  assert(pVariation);
  if (!pVariation)
    return false;

  if (GetName() != pVariation->GetName())
    return false;
  if (GetDescription() != pVariation->GetDescription())
    return false;
  if (m_caCondition != pVariation->m_caCondition)
    return false;
  if (GetDontOptimizeCondition() != pVariation->GetDontOptimizeCondition())
    return false;
  if (GetDontShowCondition() != pVariation->GetDontShowCondition())
    return false;
  if (GetConditionShowMethod() != pVariation->GetConditionShowMethod())
    return false;
  if (GetTextFormOfCondition() != pVariation->GetTextFormOfCondition())
    return false;

  if (CountOfConstants() != pVariation->CountOfConstants())
    return false;

  for (size_t nI = 0; nI < CountOfConstants(); nI++)
  {
    CIFSEntityConstantForTextForm* pThis = GetConstant(nI);
    CIFSEntityConstantForTextForm* pOther = pVariation->GetConstant(nI);
    assert(pThis);
    assert(pOther);
    if (!pThis || !pOther)
      return false;
    if (!pThis->IsIdenticalForXPubClient(pOther))
      return false;
  }

  return true;
}

bool CIFSEntityOneConditionVariation::AddConstant(const CIFSEntityConstantForTextForm* pConstant)
{
  assert(pConstant);
  if (!pConstant)
    return false;
  CIFSEntityConstantForTextForm* p = new CIFSEntityConstantForTextForm;
  if (!p)
    return false;
  if (!p->AssignFrom(pConstant))
  {
    delete p;
    return false;
  }

  size_t nCount = m_cConstants.size();
  m_cConstants.push_back(p);
  if (nCount == m_cConstants.size())
  {
    delete p;
    return false;
  }
  return true;
}

CIFSEntityConstantForTextForm* CIFSEntityOneConditionVariation::GetConstant(size_t nIndex) const
{
  assert(nIndex < m_cConstants.size());
  if (nIndex >= m_cConstants.size())
    return 0;

  return m_cConstants[nIndex];
}

CIFSEntityConstantForTextForm* CIFSEntityOneConditionVariation::GetConstant(const xtstring& sName) const
{
  for (CIFSEntityTextFormConstantsConstIterator it = m_cConstants.begin(); it != m_cConstants.end(); it++)
  {
    if ((*it)->m_sName == sName)
      return (*it);
  }
  return 0;
}

bool CIFSEntityOneConditionVariation::AssignTextFormFrom(const CIFSEntityOneConditionVariation* pOneCond)
{
  assert(pOneCond);
  if (!pOneCond)
    return false;

  m_sTextFormOfCondition = pOneCond->m_sTextFormOfCondition;
  RemoveAllConstants();
  for (size_t nI = 0; nI < pOneCond->CountOfConstants(); nI++)
    AddConstant(pOneCond->GetConstant(nI));

  return true;
}

bool CIFSEntityOneConditionVariation::CopyTextFormFrom(const CIFSEntityOneConditionVariation* pOneCond,
                                                       bool& bChanged)
{
  assert(pOneCond);
  if (!pOneCond)
    return false;

  bChanged = false;
  if (m_sTextFormOfCondition != pOneCond->m_sTextFormOfCondition)
    bChanged = true;
  if (CountOfConstants() != pOneCond->CountOfConstants())
    bChanged = true;
  else
  {
    for (size_t nI = 0; nI < pOneCond->CountOfConstants(); nI++)
    {
      if ((*GetConstant(nI)) != (*pOneCond->GetConstant(nI)))
      {
        bChanged = true;
        break;
      }
    }
  }

  return AssignTextFormFrom(pOneCond);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityConditionVariations
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSEntityConditionVariations::CIFSEntityConditionVariations(CSyncIFSNotifiersGroup* pGroup)
                              :CModelBaseInd(tIFSEntityConditionVariations),
                               CSyncNotifier_IFS(tIFSNotifier_ConditionVariations, pGroup),
                               CSyncResponse_IFS(tRO_IFS_DetermineConditionViaEntityAndVariable)
{
  m_pOwnerEntity = 0;
  m_sActiveConditionName.clear();
  m_bUseDetermineViaEntityAndVariable = false;
  m_sDetermineViaEntity.clear();
  m_sDetermineViaVariable.clear();
  RemoveAll();
}
CIFSEntityConditionVariations::CIFSEntityConditionVariations(CIFSEntityConditionVariations& cVariations)
                              :CModelBaseInd(tIFSEntityConditionVariations),
                               CSyncNotifier_IFS(tIFSNotifier_ConditionVariations, NULL),
                               CSyncResponse_IFS(tRO_IFS_DetermineConditionViaEntityAndVariable)
{
  SetSyncGroup(cVariations.GetSyncGroup());
  AssignFrom(&cVariations);
}
CIFSEntityConditionVariations::CIFSEntityConditionVariations(CIFSEntityConditionVariations* pVariations)
                              :CModelBaseInd(tIFSEntityConditionVariations),
                               CSyncNotifier_IFS(tIFSNotifier_ConditionVariations, NULL),
                               CSyncResponse_IFS(tRO_IFS_DetermineConditionViaEntityAndVariable)
{
  assert(pVariations);
  SetSyncGroup(pVariations->GetSyncGroup());
  AssignFrom(pVariations);
}
CIFSEntityConditionVariations::~CIFSEntityConditionVariations()
{
  RemoveAll();
}

bool CIFSEntityConditionVariations::AssignFrom(const CIFSEntityConditionVariations* pVariations)
{
  if (!pVariations)
    return false;

  bool bRet = true;

  RemoveAll();

  // Das duerfen wir nicht machen. Wenn wir zwischen Objekten einem Child Objekt kopieren moechten,
  // dann wuerden wir dem Child Objekt falschem Parent (Owner) zuweisen.
  m_pOwnerEntity = pVariations->m_pOwnerEntity;

  m_sActiveConditionName = pVariations->m_sActiveConditionName;
  m_bUseDetermineViaEntityAndVariable = pVariations->m_bUseDetermineViaEntityAndVariable;
  m_sDetermineViaEntity = pVariations->m_sDetermineViaEntity;
  m_sDetermineViaVariable = pVariations->m_sDetermineViaVariable;
  for (size_t nI = 0; nI < pVariations->CountOfVariations(); nI++)
    bRet &= AddCondition(pVariations->GetConstCondition(nI));

  return bRet;
}
bool CIFSEntityConditionVariations::CopyFrom(const CIFSEntityConditionVariations* pVariations,
                                             bool& bChanged)
{
  bChanged = false;

  if (!pVariations)
    return false;

  //
  if (m_sActiveConditionName != pVariations->m_sActiveConditionName)
    bChanged = true;
  else if (m_bUseDetermineViaEntityAndVariable != pVariations->m_bUseDetermineViaEntityAndVariable)
    bChanged = true;
  else if (m_sDetermineViaEntity != pVariations->m_sDetermineViaEntity)
    bChanged = true;
  else if (m_sDetermineViaVariable != pVariations->m_sDetermineViaVariable)
    bChanged = true;

  if (!bChanged)
  {
    // zuerst kontrollieren, ob was verschwunden ist
    for (size_t nI = 0; nI < CountOfVariations(); nI++)
    {
      CIFSEntityOneConditionVariation* pMy = GetCondition(nI);
      if (!pMy)
        continue;
      const CIFSEntityOneConditionVariation* p = pVariations->GetConstCondition(pMy->GetName());
      if (!p)
      {
        bChanged = true;
        break;
      }
    }
  }

  if (!bChanged)
  {
    // jetzt kontrollieren, ob was neu ist
    for (size_t nI = 0; nI < pVariations->CountOfVariations(); nI++)
    {
      const CIFSEntityOneConditionVariation* p = pVariations->GetConstCondition(nI);
      if (!p)
        continue;
      const CIFSEntityOneConditionVariation* pMy = GetConstCondition(p->GetName());
      if (!pMy)
      {
        bChanged = true;
        break;
      }
      else
      {
        if ((*pMy) != (*p))
        {
          bChanged = true;
          break;
        }
      }
    }
  }
  return AssignFrom(pVariations);
}

#define XMLTAG_CONDITIONVARIATIONS                                    _XT("ConditionVariations")
#define XMLTAG_CONDITIONVARIATION                                     _XT("ConditionVariation")
#define XMLTAG_CONDITIONVARIATIONS_ACTIVECONDITION                    _XT("ActiveCondition")
#define XMLTAG_CONDITIONVARIATIONS_USEDETERMINEVIAENTITYANDVARIABLE   _XT("UseDetermineViaEntityAndVariable")
#define XMLTAG_CONDITIONVARIATIONS_DETERMINEVIAENTITY                 _XT("DetermineViaEntity")
#define XMLTAG_CONDITIONVARIATIONS_DETERMINEVIAVARIABLE               _XT("DetermineViaVariable")
bool CIFSEntityConditionVariations::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  // XMLDoc irgendwo
  if (pXMLDoc->AddChildElem(XMLTAG_CONDITIONVARIATIONS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ConditionVariations>

    pXMLDoc->SetAttrib(XMLTAG_CONDITIONVARIATIONS_ACTIVECONDITION, GetActiveConditionName().c_str());
    pXMLDoc->SetAttrib(XMLTAG_CONDITIONVARIATIONS_USEDETERMINEVIAENTITYANDVARIABLE, GetUseDetermineViaEntityAndVariable());
    pXMLDoc->SetAttrib(XMLTAG_CONDITIONVARIATIONS_DETERMINEVIAENTITY, GetDetermineViaEntity().c_str());
    pXMLDoc->SetAttrib(XMLTAG_CONDITIONVARIATIONS_DETERMINEVIAVARIABLE, GetDetermineViaVariable().c_str());

    for (size_t nI = 0; nI < CountOfVariations(); nI++)
    {
      if (pXMLDoc->AddChildElem(XMLTAG_CONDITIONVARIATION, _XT("")))
      {
        pXMLDoc->IntoElem();
        // XMLDoc in <ConditionVariation>
        m_cVariations[nI]->SaveXMLContent(pXMLDoc);
        pXMLDoc->OutOfElem();
        // XMLDoc in <ConditionVariations>
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }
  return true;
}
bool CIFSEntityConditionVariations::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  // XMLDoc irgendwo
  if (pXMLDoc->FindChildElem(XMLTAG_CONDITIONVARIATIONS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ConditionVariations>

    RemoveAll();
    SetActiveConditionName(pXMLDoc->GetAttrib(XMLTAG_CONDITIONVARIATIONS_ACTIVECONDITION));
    SetUseDetermineViaEntityAndVariable(pXMLDoc->GetAttribLong(XMLTAG_CONDITIONVARIATIONS_USEDETERMINEVIAENTITYANDVARIABLE) != 0);
    SetDetermineViaEntity(pXMLDoc->GetAttrib(XMLTAG_CONDITIONVARIATIONS_DETERMINEVIAENTITY));
    SetDetermineViaVariable(pXMLDoc->GetAttrib(XMLTAG_CONDITIONVARIATIONS_DETERMINEVIAVARIABLE));

    while (pXMLDoc->FindChildElem(XMLTAG_CONDITIONVARIATION))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <ConditionVariation>
      CIFSEntityOneConditionVariation* pOneCond;
      pOneCond = new CIFSEntityOneConditionVariation(GetSyncGroup(), this);
      if (pOneCond)
      {
        if (pOneCond->ReadXMLContent(pXMLDoc))
          m_cVariations.push_back(pOneCond);
        else
          delete pOneCond;
      }
      pXMLDoc->OutOfElem();
      // XMLDoc in <ConditionVariations>
    }
    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }
  return true;
}

xtstring CIFSEntityConditionVariations::GetActiveConditionName()
{
  CIFSEntityOneConditionVariation* pAct = GetActiveCondition();
  if (!pAct)
    m_sActiveConditionName = _XT("");
  else
    m_sActiveConditionName = pAct->GetName();
  return m_sActiveConditionName;
}
/*
bool CIFSEntityConditionVariations::MigrateToOpenedEntity(const xtstring& sCondName,
                                                          const xtstring& sExpr,
                                                          const xtstring& sExprValues,
                                                          const xtstring& sExprSymbols,
                                                          const xtstring& sExprOffsets)
{
  // wir suchen zuerst die Bedingung
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetExpression() == sExpr)
    {
      // die Bedingung gefunden - diese default machen
      m_sActiveConditionName = m_cVariations[nI]->GetName();
      return true;
    }
  }
  // diese Bedingung war nicht gefunden
  // eine erzeugen, uberbegene Name benutzen und aktiv machen
  CIFSEntityOneConditionVariation* pNew;
  pNew = new CIFSEntityOneConditionVariation(GetSyncGroup(), this);
  if (!pNew)
    return false;

  pNew->SetName(sCondName);
  pNew->SetExpression(sExpr);
  pNew->SetExprValues(sExprValues);
  pNew->SetExprSymbols(sExprSymbols);
  pNew->SetExprOffsets(sExprOffsets);
  pNew->ExpressionChanged();

  size_t nCount = m_cVariations.size();
  m_cVariations.push_back(pNew);
  bool bRet = (nCount != m_cVariations.size());
  if (bRet)
    SetActiveConditionName(pNew->GetName());
  else
    delete pNew;
  return bRet;
}
*/

xtstring CIFSEntityConditionVariations::GetOwnerEntityName() const
{
  if (GetOwnerEntity())
    return GetOwnerEntity()->GetName();
  return _XT("");
}

bool CIFSEntityConditionVariations::IsIdenticalForXPubClient(const CIFSEntityConditionVariations* pCondVariations) const
{
  assert(pCondVariations);
  if (!pCondVariations)
    return false;

  if (CountOfVariations() != pCondVariations->CountOfVariations())
    return false;

  for (size_t nI = 0; nI < CountOfVariations(); nI++)
  {
    const CIFSEntityOneConditionVariation* pThis = GetConstCondition(nI);
    const CIFSEntityOneConditionVariation* pOther = pCondVariations->GetConstCondition(nI);
    assert(pThis);
    assert(pOther);
    if (!pThis || !pOther)
      return false;
    if (!pThis->IsIdenticalForXPubClient(pOther))
      return false;
  }
  return true;
}

size_t CIFSEntityConditionVariations::GetIndexOfItem(const CIFSEntityOneConditionVariation* pItem) const
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI] == pItem)
      return nI;
  }
  return (size_t)-1;
}

CIFSEntityOneConditionVariation* CIFSEntityConditionVariations::GetActiveCondition()
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == m_sActiveConditionName)
      return m_cVariations[nI];
  }
  if (m_cVariations.size())
  {
    m_sActiveConditionName = m_cVariations[0]->GetName();
    return m_cVariations[0];
  }
  return 0;
}
const CIFSEntityOneConditionVariation* CIFSEntityConditionVariations::GetConstActiveCondition() const
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == m_sActiveConditionName)
      return m_cVariations[nI];
  }
  /*
  if (m_cVariations.size())
  {
    m_sActiveConditionName = m_cVariations[0]->GetName();
    return m_cVariations[0];
  }
  */
  return 0;
}
CIFSEntityOneConditionVariation* CIFSEntityConditionVariations::GetCondition(size_t nIndex)
{
  if (nIndex < 0 || nIndex >= m_cVariations.size())
    return 0;

  return m_cVariations[nIndex];
}
const CIFSEntityOneConditionVariation* CIFSEntityConditionVariations::GetConstCondition(size_t nIndex) const
{
  if (nIndex < 0 || nIndex >= m_cVariations.size())
    return 0;

  return m_cVariations[nIndex];
}
CIFSEntityOneConditionVariation* CIFSEntityConditionVariations::GetCondition(const xtstring& sName)
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == sName)
      return m_cVariations[nI];
  }
  return 0;
}
const CIFSEntityOneConditionVariation* CIFSEntityConditionVariations::GetConstCondition(const xtstring& sName) const
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == sName)
      return m_cVariations[nI];
  }
  return 0;
}
bool CIFSEntityConditionVariations::AddCondition(const CIFSEntityOneConditionVariation* pOneCond)
{
  CIFSEntityOneConditionVariation* pNew;
  pNew = new CIFSEntityOneConditionVariation(GetSyncGroup(), this);
  if (!pNew)
    return false;

  bool bDummy;
  if (!pNew->CopyFrom(pOneCond, bDummy))
  {
    delete pNew;
    return false;
  }

  size_t nCount = m_cVariations.size();
  m_cVariations.push_back(pNew);
  return (nCount != m_cVariations.size());
}
bool CIFSEntityConditionVariations::RemoveCondition(size_t nIndex)
{
  if (nIndex < 0 || nIndex >= m_cVariations.size())
    return false;

  delete m_cVariations[nIndex];
  m_cVariations.erase(m_cVariations.begin() + nIndex);
  return true;
}
bool CIFSEntityConditionVariations::RemoveCondition(const xtstring& sName)
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == sName)
    {
      delete m_cVariations[nI];
      m_cVariations.erase(m_cVariations.begin() + nI);
      return true;
    }
  }
  return false;
}
void CIFSEntityConditionVariations::RemoveAll()
{
  for (CIFSConditionVariationsIterator it = m_cVariations.begin(); it != m_cVariations.end(); it++)
    delete (*it);
  m_cVariations.erase(m_cVariations.begin(), m_cVariations.end());
  return;
}






//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityParameterForTextForm
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSEntityParameterForTextForm::CIFSEntityParameterForTextForm()
                               :m_caAssignment(CConditionOrAssignment::tCOAT_ParameterForTextForm_Assignment, false)
{
  m_sName.clear();
  m_bShowParam = true;
  m_caAssignment.Reset();
  m_nFieldType = tFieldTypeEmpty;
  m_sFormat.clear();
  m_nMaxChars = -1;  // -1 no max
  m_cBackground = 0xffffff; // white
  m_cForeground = 0;        // black
  m_bAllowNull = false;
  m_bBorder = true;
  m_sDefaultText.clear();
  m_sFontName = _XT("Arial");
  m_nFontHeight = -13;
  // diese Variable hat zu Entwicklungszeit keine Wirkung
  // und gar nicht dargestellt
  m_sValue.clear();
}

CIFSEntityParameterForTextForm::~CIFSEntityParameterForTextForm()
{
}

bool CIFSEntityParameterForTextForm::AssignFrom(const CIFSEntityParameterForTextForm* pParameter)
{
  assert(pParameter);
  if (!pParameter)
    return false;

  m_sName = pParameter->m_sName;
  m_bShowParam = pParameter->m_bShowParam;
  m_caAssignment = pParameter->m_caAssignment;
  m_nFieldType = pParameter->m_nFieldType;
  m_sFormat = pParameter->m_sFormat;
  m_nMaxChars = pParameter->m_nMaxChars;
  m_cBackground = pParameter->m_cBackground;
  m_cForeground = pParameter->m_cForeground;
  m_bAllowNull = pParameter->m_bAllowNull;
  m_bBorder = pParameter->m_bBorder;
  m_sDefaultText = pParameter->m_sDefaultText;
  m_sFontName = pParameter->m_sFontName;
  m_nFontHeight = pParameter->m_nFontHeight;
  m_sValue = pParameter->m_sValue;

  return true;
}

bool CIFSEntityParameterForTextForm::CopyFrom(const CIFSEntityParameterForTextForm* pParameter, bool& bChanged)
{
  bChanged = false;

  if (!pParameter)
    return false;

  if (m_sName != pParameter->m_sName)
    bChanged = true;
  else if (m_bShowParam != pParameter->m_bShowParam)
    bChanged = true;
  else if (m_caAssignment != pParameter->m_caAssignment)
    bChanged = true;
  else if (m_nFieldType != pParameter->m_nFieldType)
    bChanged = true;
  else if (m_sFormat != pParameter->m_sFormat)
    bChanged = true;
  else if (m_nMaxChars != pParameter->m_nMaxChars)
    bChanged = true;
  else if (m_cBackground != pParameter->m_cBackground)
    bChanged = true;
  else if (m_cForeground != pParameter->m_cForeground)
    bChanged = true;
  else if (m_bAllowNull != pParameter->m_bAllowNull)
    bChanged = true;
  else if (m_bBorder != pParameter->m_bBorder)
    bChanged = true;
  else if (m_sDefaultText != pParameter->m_sDefaultText)
    bChanged = true;
  else if (m_sFontName != pParameter->m_sFontName)
    bChanged = true;
  else if (m_nFontHeight != pParameter->m_nFontHeight)
    bChanged = true;
  else if (m_sValue != pParameter->m_sValue)
    bChanged = true;

  return AssignFrom(pParameter);
}

bool CIFSEntityParameterForTextForm::operator == (const CIFSEntityParameterForTextForm& cParameter) const
{
  if (m_sName != cParameter.m_sName)
    return false;
  if (m_bShowParam != cParameter.m_bShowParam)
    return false;
  if (m_caAssignment != cParameter.m_caAssignment)
    return false;
  if (m_nFieldType != cParameter.m_nFieldType)
    return false;
  if (m_sFormat != cParameter.m_sFormat)
    return false;
  if (m_nMaxChars != cParameter.m_nMaxChars)
    return false;
  if (m_cBackground != cParameter.m_cBackground)
    return false;
  if (m_cForeground != cParameter.m_cForeground)
    return false;
  if (m_bAllowNull != cParameter.m_bAllowNull)
    return false;
  if (m_bBorder != cParameter.m_bBorder)
    return false;
  if (m_sDefaultText != cParameter.m_sDefaultText)
    return false;
  if (m_sFontName != cParameter.m_sFontName)
    return false;
  if (m_nFontHeight != cParameter.m_nFontHeight)
    return false;
  if (m_sValue != cParameter.m_sValue)
    return false;

  return true;
}

bool CIFSEntityParameterForTextForm::operator != (const CIFSEntityParameterForTextForm& cParameter) const
{
  return !(operator == (cParameter));
}

bool CIFSEntityParameterForTextForm::IsIdenticalForXPubClient(const CIFSEntityParameterForTextForm* pParameter) const
{
  assert(pParameter);
  if (!pParameter)
    return false;

  if (m_sName != pParameter->m_sName)
    return false;
  if (m_bShowParam != pParameter->m_bShowParam)
    return false;
  if (m_caAssignment != pParameter->m_caAssignment)
    return false;
  if (m_nFieldType != pParameter->m_nFieldType)
    return false;
  if (m_sFormat != pParameter->m_sFormat)
    return false;
  if (m_nMaxChars != pParameter->m_nMaxChars)
    return false;
  if (m_cBackground != pParameter->m_cBackground)
    return false;
  if (m_cForeground != pParameter->m_cForeground)
    return false;
  if (m_bAllowNull != pParameter->m_bAllowNull)
    return false;
  if (m_bBorder != pParameter->m_bBorder)
    return false;
  if (m_sDefaultText != pParameter->m_sDefaultText)
    return false;
  if (m_sFontName != pParameter->m_sFontName)
    return false;
  if (m_nFontHeight != pParameter->m_nFontHeight)
    return false;
//  if (m_sValue != m_sValue)
//    return false;

  return true;
}

bool CIFSEntityParameterForTextForm::AssignFromERModelParamInd(CERModelParamInd* pParam)
{
  if (!pParam)
    return false;

  m_sName         = pParam->GetName();
  m_bShowParam    = true;
  m_caAssignment.Reset();
  m_nFieldType    = pParam->GetType();
  m_sFormat       = _XT("");
//  m_nMaxChars     = 
//  m_cBackground   = 
//  m_cForeground   = 
//  m_bAllowNull    = 
//  m_bBorder       = 
  m_sDefaultText  = pParam->GetDefaultValue();
//  m_sFontName     = 
//  m_nFontHeight   = 
  m_sValue        = pParam->GetDefaultValue();
//  m_nCompoundIndexInSymbolsValuesOffsets  = 

  return true;
}

bool CIFSEntityParameterForTextForm::AssignFromChangedERModelParamInd(CERModelParamInd* pParam)
{
  if (!pParam)
    return false;

//  m_sName         = 
//  m_bShowParam    = 
//  m_sExprText     = 
//  m_sExprValues   = 
//  m_sExprSymbols  = 
//  m_sExprOffsets  = 
  m_nFieldType    = pParam->GetType();
//  m_sFormat       = 
//  m_nMaxChars     = 
//  m_cBackground   = 
//  m_cForeground   = 
//  m_bAllowNull    = 
//  m_bBorder       = 
  m_sDefaultText  = pParam->GetDefaultValue();
//  m_sFontName     = 
//  m_nFontHeight   = 
  m_sValue        = pParam->GetDefaultValue();
//  m_nCompoundIndexInSymbolsValuesOffsets  = 

  return true;
}

bool CIFSEntityParameterForTextForm::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  // XMLDoc in <irgendwo>
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_NAME, m_sName.c_str());
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_SHOWPARAMETER, m_bShowParam);
  m_caAssignment.SaveXMLContent(pXMLDoc);
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_FIELDTYPE, m_nFieldType);
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_FORMAT, m_sFormat.c_str());
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_MAXCHARS, m_nMaxChars);
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_BACKGROUND, m_cBackground);
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_FOREGROUND, m_cForeground);
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_ALLOWNULL, m_bAllowNull);
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_BORDER, m_bBorder);
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_DEFAULTTEXT, m_sDefaultText.c_str());
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_FONTNAME, m_sFontName.c_str());
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_FONTHEIGHT, m_nFontHeight);
  pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_PARAMETER_VALUE, m_sValue.c_str());

  return true;
}

bool CIFSEntityParameterForTextForm::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  m_sName         = pXMLDoc->GetAttrib    (XMLTAG_PARAMETERS_PARAMETER_NAME);
  m_bShowParam    = pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_PARAMETER_SHOWPARAMETER) != 0;
  m_caAssignment.ReadXMLContent(pXMLDoc);
  m_nFieldType    = (TFieldType)pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_PARAMETER_FIELDTYPE);
  m_sFormat       = pXMLDoc->GetAttrib    (XMLTAG_PARAMETERS_PARAMETER_FORMAT);
  m_nMaxChars     = pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_PARAMETER_MAXCHARS);
  m_cBackground   = pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_PARAMETER_BACKGROUND);
  m_cForeground   = pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_PARAMETER_FOREGROUND);
  m_bAllowNull    = pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_PARAMETER_ALLOWNULL) != 0;
  m_bBorder       = pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_PARAMETER_BORDER) != 0;
  m_sDefaultText  = pXMLDoc->GetAttrib    (XMLTAG_PARAMETERS_PARAMETER_DEFAULTTEXT);
  m_sFontName     = pXMLDoc->GetAttrib    (XMLTAG_PARAMETERS_PARAMETER_FONTNAME);
  m_nFontHeight   = pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_PARAMETER_FONTHEIGHT);
  m_sValue        = pXMLDoc->GetAttrib    (XMLTAG_PARAMETERS_PARAMETER_VALUE);

  return true;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityAllParameters
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSEntityAllParameters::CIFSEntityAllParameters(CSyncIFSNotifiersGroup* pGroup)
                        :CModelBaseInd(tIFSEntityParameters),
                         CSyncNotifier_IFS(tIFSNotifier_Parameters, pGroup),
                         CSyncResponse_Proj(tRO_IFS_Parameters),
                         CSyncResponse_ER(tRO_IFS_Parameters),
                         CSyncResponse_IFS(tRO_IFS_Parameters)
{
  m_pOwnerEntity = 0;
  m_sName.clear();
  m_sTextFormOfParameters.clear();
  m_bShowTextForm = true;
  m_bSPSQLAttached = false;
  RemoveAllParameters();
}
CIFSEntityAllParameters::CIFSEntityAllParameters(CIFSEntityAllParameters& cParameters)
                        :CModelBaseInd(tIFSEntityParameters),
                         CSyncNotifier_IFS(tIFSNotifier_Parameters, NULL),
                         CSyncResponse_Proj(tRO_IFS_Parameters),
                         CSyncResponse_ER(tRO_IFS_Parameters),
                         CSyncResponse_IFS(tRO_IFS_Parameters)
{
  SetSyncGroup(cParameters.GetSyncGroup());
  AssignFrom(&cParameters);
}
CIFSEntityAllParameters::CIFSEntityAllParameters(CIFSEntityAllParameters* pParameters)
                        :CModelBaseInd(tIFSEntityParameters),
                         CSyncNotifier_IFS(tIFSNotifier_Parameters, NULL),
                         CSyncResponse_Proj(tRO_IFS_Parameters),
                         CSyncResponse_ER(tRO_IFS_Parameters),
                         CSyncResponse_IFS(tRO_IFS_Parameters)
{
  assert(pParameters);
  SetSyncGroup(pParameters->GetSyncGroup());
  AssignFrom(pParameters);
}
CIFSEntityAllParameters::~CIFSEntityAllParameters()
{
  RemoveAllParameters();
}
bool CIFSEntityAllParameters::AssignFrom(const CIFSEntityAllParameters* pParameters)
{
  assert(pParameters);
  if (!pParameters)
    return false;

  m_sName = pParameters->m_sName;
  m_sTextFormOfParameters = pParameters->m_sTextFormOfParameters;
  m_bShowTextForm = pParameters->m_bShowTextForm;
  m_bSPSQLAttached = pParameters->m_bSPSQLAttached;
  // Das duerfen wir nicht machen. Wenn wir zwischen Objekten einem Child Objekt kopieren moechten,
  // dann wuerden wir dem Child Objekt falschem Parent (Owner) zuweisen.
  m_pOwnerEntity = pParameters->m_pOwnerEntity;

  RemoveAllParameters();
  for (size_t nI = 0; nI < pParameters->CountOfParameters(); nI++)
    AddParameter(pParameters->GetConstParameter(nI));

  return true;
}
bool CIFSEntityAllParameters::CopyFrom(const CIFSEntityAllParameters* pParameters, bool& bChanged)
{
  bChanged = false;

  if (!pParameters)
    return false;

  if (m_sName != pParameters->m_sName)
    bChanged = true;
  if (m_sTextFormOfParameters != pParameters->m_sTextFormOfParameters)
    bChanged = true;
  if (m_bShowTextForm != pParameters->m_bShowTextForm)
    bChanged = true;
  if (m_bSPSQLAttached != pParameters->m_bSPSQLAttached)
    bChanged = true;

  if (CountOfParameters() != pParameters->CountOfParameters())
    bChanged = true;
  else
  {
    for (size_t nI = 0; nI < pParameters->CountOfParameters(); nI++)
    {
      if ((*GetConstParameter(nI)) != (*pParameters->GetConstParameter(nI)))
      {
        bChanged = true;
        break;
      }
    }
  }

  return AssignFrom(pParameters);
}

#define XMLTAG_PARAMETERS                         _XT("Parameters")
#define XMLTAG_PARAMETERS_NAME                    _XT("Name")
#define XMLTAG_PARAMETERS_TEXTFORMOFPARAMETERS    _XT("TextFormOfParameters")
#define XMLTAG_PARAMETERS_SHOWTEXTFORM            _XT("ShowTextForm")
#define XMLTAG_PARAMETERS_SPSQLATTACHED           _XT("SPSQLAttached")
#define XMLTAG_PARAMETERS_CONSTANTS               _XT("ParameterConstants")
#define XMLTAG_PARAMETERS_CONSTANT                _XT("ParameterConstant")
bool CIFSEntityAllParameters::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  if (pXMLDoc->AddChildElem(XMLTAG_PARAMETERS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <Parameters>
    pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_TEXTFORMOFPARAMETERS, GetTextFormOfParameters().c_str());
    pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_SHOWTEXTFORM, GetShowTextForm());
    pXMLDoc->SetAttrib(XMLTAG_PARAMETERS_SPSQLATTACHED, GetSPSQLAttached());

    if (pXMLDoc->AddChildElem(XMLTAG_PARAMETERS_CONSTANTS, _XT("")))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <ParameterConstants>
      for (size_t nI = 0; nI < CountOfParameters(); nI++)
      {
        if (pXMLDoc->AddChildElem(XMLTAG_PARAMETERS_CONSTANT, _XT("")))
        {
          pXMLDoc->IntoElem();
          // XMLDoc in <ParameterConstant>
          m_cTextFormParameters[nI]->SaveXMLContent(pXMLDoc);
          pXMLDoc->OutOfElem();
          // XMLDoc in <ParameterConstant>
        }
      }
      pXMLDoc->OutOfElem();
      // XMLDoc in <Parameters>
    }
    pXMLDoc->OutOfElem();
  }

  return true;
}
bool CIFSEntityAllParameters::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  if (pXMLDoc->FindChildElem(XMLTAG_PARAMETERS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <Parameters>

    SetName(pXMLDoc->GetAttrib(XMLTAG_PARAMETERS_NAME));
    SetTextFormOfParameters(pXMLDoc->GetAttrib(XMLTAG_PARAMETERS_TEXTFORMOFPARAMETERS));
    SetShowTextForm(pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_SHOWTEXTFORM) != 0);
    SetSPSQLAttached(pXMLDoc->GetAttribLong(XMLTAG_PARAMETERS_SPSQLATTACHED) != 0);

    RemoveAllParameters();

    if (pXMLDoc->FindChildElem(XMLTAG_PARAMETERS_CONSTANTS))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <ParameterConstants>

      while (pXMLDoc->FindChildElem(XMLTAG_PARAMETERS_CONSTANT))
      {
        pXMLDoc->IntoElem();
        // XMLDoc in <ParameterConstant>
        CIFSEntityParameterForTextForm* pParameter;
        pParameter = new CIFSEntityParameterForTextForm;
        if (pParameter)
        {
          if (pParameter->ReadXMLContent(pXMLDoc))
            m_cTextFormParameters.push_back(pParameter);
          else
            delete pParameter;
        }
        pXMLDoc->OutOfElem();
        // XMLDoc in <ParameterConstants>
      }
      pXMLDoc->OutOfElem();
      // XMLDoc <Parameters>
    }
    pXMLDoc->OutOfElem();
  }

  return true;
}
bool CIFSEntityAllParameters::IsIdenticalForXPubClient(const CIFSEntityAllParameters* pParameters) const
{
  if (GetName() != pParameters->GetName())
    return false;
  if (GetTextFormOfParameters() != pParameters->GetTextFormOfParameters())
    return false;
  if (GetShowTextForm() != pParameters->GetShowTextForm())
    return false;
  if (GetSPSQLAttached() != pParameters->GetSPSQLAttached())
    return false;

  if (CountOfParameters() != pParameters->CountOfParameters())
    return false;

  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    const CIFSEntityParameterForTextForm* pThis = GetConstParameter(nI);
    const CIFSEntityParameterForTextForm* pOther = pParameters->GetConstParameter(nI);
    assert(pThis);
    assert(pOther);
    if (!pThis || !pOther)
      return false;
    if (!pThis->IsIdenticalForXPubClient(pOther))
      return false;
  }

  return true;
}
CIFSEntityParameterForTextForm* CIFSEntityAllParameters::GetParameter(size_t nIndex)
{
  assert(nIndex < m_cTextFormParameters.size());
  if (nIndex >= m_cTextFormParameters.size())
    return 0;

  return m_cTextFormParameters[nIndex];
}
const CIFSEntityParameterForTextForm* CIFSEntityAllParameters::GetConstParameter(size_t nIndex) const
{
  assert(nIndex < m_cTextFormParameters.size());
  if (nIndex >= m_cTextFormParameters.size())
    return 0;

  return m_cTextFormParameters[nIndex];
}
CIFSEntityParameterForTextForm* CIFSEntityAllParameters::GetParameter(const xtstring& sName)
{
  for (CIFSEntityTextFormParametersConstIterator it = m_cTextFormParameters.begin(); it != m_cTextFormParameters.end(); it++)
  {
    if ((*it)->m_sName == sName)
      return (*it);
  }
  return 0;
}
const CIFSEntityParameterForTextForm* CIFSEntityAllParameters::GetConstParameter(const xtstring& sName) const
{
  for (CIFSEntityTextFormParametersConstIterator it = m_cTextFormParameters.begin(); it != m_cTextFormParameters.end(); it++)
  {
    if ((*it)->m_sName == sName)
      return (*it);
  }
  return 0;
}
bool CIFSEntityAllParameters::AddParameter(const CIFSEntityParameterForTextForm* pParameter)
{
  assert(pParameter);
  if (!pParameter)
    return false;
  CIFSEntityParameterForTextForm* p = new CIFSEntityParameterForTextForm;
  if (!p)
    return false;
  if (!p->AssignFrom(pParameter))
  {
    delete p;
    return false;
  }

  size_t nCount = m_cTextFormParameters.size();
  m_cTextFormParameters.push_back(p);
  if (nCount == m_cTextFormParameters.size())
  {
    delete p;
    return false;
  }
  return true;
}
bool CIFSEntityAllParameters::RemoveParameter(size_t nIndex)
{
  assert(nIndex < m_cTextFormParameters.size());
  if (nIndex >= m_cTextFormParameters.size())
    return false;

  delete m_cTextFormParameters[nIndex];
  m_cTextFormParameters.erase(m_cTextFormParameters.begin() + nIndex);
  return true;
}
bool CIFSEntityAllParameters::RemoveParameter(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* pParam = GetParameter(nI);
    if (pParam && sName == pParam->m_sName)
      return RemoveParameter(nI);
  }
  return false;
}
void CIFSEntityAllParameters::RemoveAllParameters()
{
  for (CIFSEntityTextFormParametersIterator it = m_cTextFormParameters.begin(); it != m_cTextFormParameters.end(); it++)
    delete (*it);
  m_cTextFormParameters.erase(m_cTextFormParameters.begin(), m_cTextFormParameters.end());
}

xtstring CIFSEntityAllParameters::GetParameterNameForTextForm(const xtstring& sParameterName) const
{
  xtstring sName;
  sName = CONSTANT_LEFT_TOKEN;
  sName += sParameterName;
  sName += CONSTANT_RIGHT_TOKEN;
  return sName;
}
bool CIFSEntityAllParameters::GetPositionOfNameForTextFormInTextForm(const xtstring& sParameterNameForTextForm,
                                                                     int& nPos,
                                                                     int& nLen)
{
  nPos = -1;
  nLen = 0;
  size_t n = m_sTextFormOfParameters.find(sParameterNameForTextForm);
  if (n == xtstring::npos)
    return false;

  nPos = (int)n;
  nLen = (int)sParameterNameForTextForm.size();
  return true;
}
bool CIFSEntityAllParameters::GetPositionOfNameForSyncEngineInTextForm(const xtstring& sParameterNameForTextForm,
                                                                       const xtstring& sParamName,
                                                                       int& nPos,
                                                                       int& nLen)
{
  nPos = -1;
  nLen = 0;
  size_t nPosition = sParameterNameForTextForm.find(sParamName);
  if (nPosition == xtstring::npos)
    return false;

  if (!GetPositionOfNameForTextFormInTextForm(sParameterNameForTextForm, nPos, nLen))
    return false;
  nPos += (int)nPosition;
  nLen -= (int)(sParameterNameForTextForm.size() - sParamName.size());
  return true;
}
void CIFSEntityAllParameters::SettingOfERModelChanged(CERModelTableInd* pERModelTable)
{
  if (!pERModelTable)
    return;

  if (pERModelTable->GetERTableType() == CERModelTableInd::tERTT_Table)
  {
    SetSPSQLAttached(false);
    return;
  }
  SetSPSQLAttached(true);

  // delete params, if not exist in new params
  bool bDeleted = true;
  while (bDeleted)
  {
    bDeleted = false;
    for (size_t nI = 0; nI < CountOfParameters(); nI++)
    {
      bool bDeleteIt = false;
      CERModelParamInd* pERParameter = pERModelTable->GetParam(GetParameter(nI)->m_sName);
      if (!pERParameter)
        // In neuer Tabelle existiert nicht.
        bDeleteIt = true;
      if (bDeleteIt)
      {
        xtstring s = GetParameterNameForTextForm(GetParameter(nI)->m_sName);
        int nPos, nLen;
        GetPositionOfNameForTextFormInTextForm(s, nPos, nLen);
        if (nPos != -1)
          m_sTextFormOfParameters.erase(nPos, nLen);
        RemoveParameter(nI);
        bDeleted = true;
        break;
      }
    }
  }

  // create parameters
  for (size_t nI = 0; nI < pERModelTable->CountOfParams(); nI++)
  {
    CERModelParamInd* pERParameter = pERModelTable->GetParam(nI);
    if (pERParameter)
    {
      if (!GetParameter(pERParameter->GetName()))
      {
        CIFSEntityParameterForTextForm param;
        if (param.AssignFromERModelParamInd(pERModelTable->GetParam(nI)))
          AddParameter(&param);
      }
    }
  }

  // copy properties
  for (size_t nI = 0; nI < pERModelTable->CountOfParams(); nI++)
  {
    CERModelParamInd* pParamFrom = pERModelTable->GetParam(nI);
    CIFSEntityParameterForTextForm* pParamTo = GetParameter(pERModelTable->GetParam(nI)->GetName());
    if (!pParamFrom || !pParamTo)
      continue;
    if (pParamFrom->GetName() != pParamTo->m_sName)
      // Diese Bedingung ist da wahrscheinlich nur deshalb, dass das Wasserdicht ist,
      // aber wahrscheinlich nicht noetig.
      continue;
    pParamTo->AssignFromERModelParamInd(pParamFrom);
  }

  // create text form
  for (size_t nI = 0; nI < CountOfParameters(); nI++)
  {
    CIFSEntityParameterForTextForm* param = GetParameter(nI);
    if (!param)
      continue;
    xtstring s = GetParameterNameForTextForm(param->m_sName);
    int nPos, nLen;
    GetPositionOfNameForTextFormInTextForm(s, nPos, nLen);
    if (nPos == -1)
      m_sTextFormOfParameters += s;
  }
}
xtstring CIFSEntityAllParameters::GetOwnerEntityName() const
{
  if (GetOwnerEntity())
    return GetOwnerEntity()->GetName();
  return _XT("");
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityOneSQLCommandVariation
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSEntityOneSQLCommandVariation::CIFSEntityOneSQLCommandVariation(CSyncIFSNotifiersGroup* pGroup,
                                                                   CIFSEntitySQLCommandVariations* pParent)
                                 :CModelBaseInd(tIFSEntitySQLCommand),
                                  CSyncNotifier_IFS(tIFSNotifier_OneSQLCommand, pGroup)
{
  m_pParent = pParent;
  m_sName.clear();
  m_sDescriptionShort.clear();
  m_sDescriptionLong.clear();
  m_sSQLCommand.clear();
  m_sFreeSelectionData.clear();
  m_bDontShowSQLCommand = false;
  m_nSQLCommandShowMethod = tSQLCommandShowMethod_Default;
}
CIFSEntityOneSQLCommandVariation::CIFSEntityOneSQLCommandVariation(CIFSEntityOneSQLCommandVariation& cSQLCommand)
                                 :CModelBaseInd(tIFSEntitySQLCommand),
                                  CSyncNotifier_IFS(tIFSNotifier_OneSQLCommand, NULL)
{
  SetSyncGroup(cSQLCommand.GetSyncGroup());
  AssignFrom(&cSQLCommand);
}
CIFSEntityOneSQLCommandVariation::CIFSEntityOneSQLCommandVariation(CIFSEntityOneSQLCommandVariation* pSQLCommand)
                                 :CModelBaseInd(tIFSEntitySQLCommand),
                                  CSyncNotifier_IFS(tIFSNotifier_OneSQLCommand, NULL)
{
  assert(pSQLCommand);
  SetSyncGroup(pSQLCommand->GetSyncGroup());
  AssignFrom(pSQLCommand);
}

CIFSEntityOneSQLCommandVariation::~CIFSEntityOneSQLCommandVariation()
{
}
bool CIFSEntityOneSQLCommandVariation::AssignFrom(const CIFSEntityOneSQLCommandVariation* pSQLCommand)
{
  assert(pSQLCommand);
  if (!pSQLCommand)
    return false;

  m_sName = pSQLCommand->m_sName;
  m_sDescriptionShort = pSQLCommand->m_sDescriptionShort;
  m_sDescriptionLong = pSQLCommand->m_sDescriptionLong;
  m_sSQLCommand = pSQLCommand->m_sSQLCommand;
  m_sFreeSelectionData = pSQLCommand->m_sFreeSelectionData;
  m_bDontShowSQLCommand = pSQLCommand->m_bDontShowSQLCommand;
  m_nSQLCommandShowMethod = pSQLCommand->m_nSQLCommandShowMethod;

  return true;
}
bool CIFSEntityOneSQLCommandVariation::CopyFrom(const CIFSEntityOneSQLCommandVariation* pSQLCommand,
                                                bool& bChanged)
{
  bChanged = false;

  if (!pSQLCommand)
    return false;

  if (m_sName != pSQLCommand->m_sName)
    bChanged = true;
  else if (m_sDescriptionShort != pSQLCommand->m_sDescriptionShort)
    bChanged = true;
  else if (m_sDescriptionLong != pSQLCommand->m_sDescriptionLong)
    bChanged = true;
  else if (m_sSQLCommand != pSQLCommand->m_sSQLCommand)
    bChanged = true;
  else if (m_sFreeSelectionData != pSQLCommand->m_sFreeSelectionData)
    bChanged = true;
  else if (m_bDontShowSQLCommand != pSQLCommand->m_bDontShowSQLCommand)
    bChanged = true;
  else if (m_nSQLCommandShowMethod != pSQLCommand->m_nSQLCommandShowMethod)
    bChanged = true;

  return AssignFrom(pSQLCommand);
}
bool CIFSEntityOneSQLCommandVariation::operator == (const CIFSEntityOneSQLCommandVariation& cSQLCommand) const
{
  if (m_sName != cSQLCommand.m_sName)
    return false;
  else if (m_sDescriptionShort != cSQLCommand.m_sDescriptionShort)
    return false;
  else if (m_sDescriptionLong != cSQLCommand.m_sDescriptionLong)
    return false;
  else if (m_sSQLCommand != cSQLCommand.m_sSQLCommand)
    return false;
  else if (m_sFreeSelectionData != cSQLCommand.m_sFreeSelectionData)
    return false;
  else if (m_bDontShowSQLCommand != cSQLCommand.m_bDontShowSQLCommand)
    return false;
  else if (m_nSQLCommandShowMethod != cSQLCommand.m_nSQLCommandShowMethod)
    return false;

  return true;
}
bool CIFSEntityOneSQLCommandVariation::operator != (const CIFSEntityOneSQLCommandVariation& cSQLCommand) const
{
  return !(operator == (cSQLCommand));
}
#define XMLTAG_ONESQLCOMMAND_NAME                 _XT("Name")
#define XMLTAG_ONESQLCOMMAND_DESCRIPTION_SHORT    _XT("DescriptionShort")
#define XMLTAG_ONESQLCOMMAND_DESCRIPTION_LONG     _XT("DescriptionLong")
#define XMLTAG_ONESQLCOMMAND_SQLCOMMAND           _XT("SQLCommand")
#define XMLTAG_ONESQLCOMMAND_FREESELECTIONDATA    _XT("FreeSelectionData")
#define XMLTAG_ONESQLCOMMAND_DONTSHOWSQLCOMMAND   _XT("DontShowSQLCommand")
#define XMLTAG_ONESQLCOMMAND_SQLCOMMANDSHOWMETHOD _XT("SQLCommandShowMethod")
bool CIFSEntityOneSQLCommandVariation::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  pXMLDoc->SetAttrib(XMLTAG_ONESQLCOMMAND_NAME,                 GetName().c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONESQLCOMMAND_DESCRIPTION_SHORT,    GetDescriptionShort().c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONESQLCOMMAND_DESCRIPTION_LONG,     GetDescriptionLong().c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONESQLCOMMAND_SQLCOMMAND,           GetSQLCommand().c_str());
  pXMLDoc->SetAttrib(XMLTAG_ONESQLCOMMAND_DONTSHOWSQLCOMMAND,   GetDontShowSQLCommand());
  pXMLDoc->SetAttrib(XMLTAG_ONESQLCOMMAND_SQLCOMMANDSHOWMETHOD, GetSQLCommandShowMethod());

  pXMLDoc->AddChildElem(XMLTAG_ONESQLCOMMAND_FREESELECTIONDATA, GetFreeSelectionData().c_str());

  return true;
}
bool CIFSEntityOneSQLCommandVariation::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  SetName                         (pXMLDoc->GetAttrib(XMLTAG_ONESQLCOMMAND_NAME));
  SetDescriptionShort             (pXMLDoc->GetAttrib(XMLTAG_ONESQLCOMMAND_DESCRIPTION_SHORT));
  SetDescriptionLong              (pXMLDoc->GetAttrib(XMLTAG_ONESQLCOMMAND_DESCRIPTION_LONG));
  SetSQLCommand                   (pXMLDoc->GetAttrib(XMLTAG_ONESQLCOMMAND_SQLCOMMAND));
  SetDontShowSQLCommand           (pXMLDoc->GetAttribLong(XMLTAG_ONESQLCOMMAND_DONTSHOWSQLCOMMAND) != 0);
  SetSQLCommandShowMethod         ((TSQLCommandShowMethod)pXMLDoc->GetAttribLong(XMLTAG_ONESQLCOMMAND_SQLCOMMANDSHOWMETHOD));

  if (pXMLDoc->FindChildElem(XMLTAG_ONESQLCOMMAND_FREESELECTIONDATA))
  {
    pXMLDoc->IntoElem();
    SetFreeSelectionData(pXMLDoc->GetData());
    pXMLDoc->OutOfElem();
  }

  return true;
}

bool CIFSEntityOneSQLCommandVariation::IsIdenticalForXPubClient(const CIFSEntityOneSQLCommandVariation* pVariation) const
{
  assert(pVariation);
  if (!pVariation)
    return false;

  if (GetName() != pVariation->GetName())
    return false;
  if (GetDescriptionShort() != pVariation->GetDescriptionShort())
    return false;
  if (GetDescriptionLong() != pVariation->GetDescriptionLong())
    return false;
  if (GetSQLCommand() != pVariation->GetSQLCommand())
    return false;
  if (GetFreeSelectionData() != pVariation->GetFreeSelectionData())
    return false;
  if (GetDontShowSQLCommand() != pVariation->GetDontShowSQLCommand())
    return false;
  if (GetSQLCommandShowMethod() != pVariation->GetSQLCommandShowMethod())
    return false;

  return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntitySQLCommandVariations
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSEntitySQLCommandVariations::CIFSEntitySQLCommandVariations(CSyncIFSNotifiersGroup* pGroup)
                               :CModelBaseInd(tIFSEntitySQLCommandVariations),
                                CSyncNotifier_IFS(tIFSNotifier_SQLCommandVariations, pGroup),
                                CSyncResponse_IFS(tRO_IFS_DetermineSQLQueryViaEntityAndVariable)
{
  m_pOwnerEntity = 0;
  m_sActiveSQLCommandName.clear();
  m_bUseDetermineViaEntityAndVariable = false;
  m_sDetermineViaEntity.clear();
  m_sDetermineViaVariable.clear();
  RemoveAll();
}
CIFSEntitySQLCommandVariations::CIFSEntitySQLCommandVariations(CIFSEntitySQLCommandVariations& cVariations)
                               :CModelBaseInd(tIFSEntitySQLCommandVariations),
                                CSyncNotifier_IFS(tIFSNotifier_SQLCommandVariations, NULL),
                                CSyncResponse_IFS(tRO_IFS_DetermineSQLQueryViaEntityAndVariable)
{
  SetSyncGroup(cVariations.GetSyncGroup());
  AssignFrom(&cVariations);
}
CIFSEntitySQLCommandVariations::CIFSEntitySQLCommandVariations(CIFSEntitySQLCommandVariations* pVariations)
                               :CModelBaseInd(tIFSEntitySQLCommandVariations),
                                CSyncNotifier_IFS(tIFSNotifier_SQLCommandVariations, NULL),
                                CSyncResponse_IFS(tRO_IFS_DetermineSQLQueryViaEntityAndVariable)
{
  assert(pVariations);
  SetSyncGroup(pVariations->GetSyncGroup());
  AssignFrom(pVariations);
}
CIFSEntitySQLCommandVariations::~CIFSEntitySQLCommandVariations()
{
  RemoveAll();
}
bool CIFSEntitySQLCommandVariations::AssignFrom(const CIFSEntitySQLCommandVariations* pVariations)
{
  if (!pVariations)
    return false;

  bool bRet = true;

  RemoveAll();

  // Das duerfen wir nicht machen. Wenn wir zwischen Objekten einem Child Objekt kopieren moechten,
  // dann wuerden wir dem Child Objekt falschem Parent (Owner) zuweisen.
  m_pOwnerEntity = pVariations->m_pOwnerEntity;

  m_sActiveSQLCommandName = pVariations->m_sActiveSQLCommandName;
  m_bUseDetermineViaEntityAndVariable = pVariations->m_bUseDetermineViaEntityAndVariable;
  m_sDetermineViaEntity = pVariations->m_sDetermineViaEntity;
  m_sDetermineViaVariable = pVariations->m_sDetermineViaVariable;
  for (size_t nI = 0; nI < pVariations->CountOfVariations(); nI++)
    bRet &= AddSQLCommand(pVariations->GetConstSQLCommand(nI));

  return bRet;
}
bool CIFSEntitySQLCommandVariations::CopyFrom(const CIFSEntitySQLCommandVariations* pVariations, bool& bChanged)
{
  bChanged = false;

  if (!pVariations)
    return false;

  //
  if (m_sActiveSQLCommandName != pVariations->m_sActiveSQLCommandName)
    bChanged = true;
  else if (m_bUseDetermineViaEntityAndVariable != pVariations->m_bUseDetermineViaEntityAndVariable)
    bChanged = true;
  else if (m_sDetermineViaEntity != pVariations->m_sDetermineViaEntity)
    bChanged = true;
  else if (m_sDetermineViaVariable != pVariations->m_sDetermineViaVariable)
    bChanged = true;

  if (!bChanged)
  {
    // zuerst kontrollieren, ob was verschwunden ist
    for (size_t nI = 0; nI < CountOfVariations(); nI++)
    {
      CIFSEntityOneSQLCommandVariation* pMy = GetSQLCommand(nI);
      if (!pMy)
        continue;
      const CIFSEntityOneSQLCommandVariation* p = pVariations->GetConstSQLCommand(pMy->GetName());
      if (!p)
      {
        bChanged = true;
        break;
      }
    }
  }

  if (!bChanged)
  {
    // jetzt kontrollieren, ob was neu ist
    for (size_t nI = 0; nI < pVariations->CountOfVariations(); nI++)
    {
      const CIFSEntityOneSQLCommandVariation* p = pVariations->GetConstSQLCommand(nI);
      if (!p)
        continue;
      const CIFSEntityOneSQLCommandVariation* pMy = GetConstSQLCommand(p->GetName());
      if (!pMy)
      {
        bChanged = true;
        break;
      }
      else
      {
        if ((*pMy) != (*p))
        {
          bChanged = true;
          break;
        }
      }
    }
  }
  return AssignFrom(pVariations);
}
#define XMLTAG_SQLCOMMANDVARIATIONS                                   _XT("SQLCommandVariations")
#define XMLTAG_SQLCOMMANDVARIATION                                    _XT("SQLCommandVariation")
#define XMLTAG_SQLCOMMANDVARIATIONS_ACTIVESQLCOMMAND                  _XT("ActiveSQLCommand")
#define XMLTAG_SQLCOMMANDVARIATIONS_USEDETERMINEVIAENTITYANDVARIABLE  _XT("UseDetermineViaEntityAndVariable")
#define XMLTAG_SQLCOMMANDVARIATIONS_DETERMINEVIAENTITY                _XT("DetermineViaEntity")
#define XMLTAG_SQLCOMMANDVARIATIONS_DETERMINEVIAVARIABLE              _XT("DetermineViaVariable")
bool CIFSEntitySQLCommandVariations::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  // XMLDoc irgendwo
  if (pXMLDoc->AddChildElem(XMLTAG_SQLCOMMANDVARIATIONS, _XT("")))
  {
    pXMLDoc->IntoElem();
    pXMLDoc->SetAttrib(XMLTAG_SQLCOMMANDVARIATIONS_ACTIVESQLCOMMAND, GetActiveSQLCommandName().c_str());
    pXMLDoc->SetAttrib(XMLTAG_SQLCOMMANDVARIATIONS_USEDETERMINEVIAENTITYANDVARIABLE, GetUseDetermineViaEntityAndVariable());
    pXMLDoc->SetAttrib(XMLTAG_SQLCOMMANDVARIATIONS_DETERMINEVIAENTITY, GetDetermineViaEntity().c_str());
    pXMLDoc->SetAttrib(XMLTAG_SQLCOMMANDVARIATIONS_DETERMINEVIAVARIABLE, GetDetermineViaVariable().c_str());

    for (size_t nI = 0; nI < CountOfVariations(); nI++)
    {
      if (pXMLDoc->AddChildElem(XMLTAG_SQLCOMMANDVARIATION, _XT("")))
      {
        pXMLDoc->IntoElem();
        m_cVariations[nI]->SaveXMLContent(pXMLDoc);
        pXMLDoc->OutOfElem();
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }
  return true;
}
bool CIFSEntitySQLCommandVariations::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  // XMLDoc irgendwo
  if (pXMLDoc->FindChildElem(XMLTAG_SQLCOMMANDVARIATIONS))
  {
    pXMLDoc->IntoElem();

    RemoveAll();
    SetActiveSQLCommandName(pXMLDoc->GetAttrib(XMLTAG_SQLCOMMANDVARIATIONS_ACTIVESQLCOMMAND));
    SetUseDetermineViaEntityAndVariable(pXMLDoc->GetAttribLong(XMLTAG_SQLCOMMANDVARIATIONS_USEDETERMINEVIAENTITYANDVARIABLE) != 0);
    SetDetermineViaEntity(pXMLDoc->GetAttrib(XMLTAG_SQLCOMMANDVARIATIONS_DETERMINEVIAENTITY));
    SetDetermineViaVariable(pXMLDoc->GetAttrib(XMLTAG_SQLCOMMANDVARIATIONS_DETERMINEVIAVARIABLE));

    while (pXMLDoc->FindChildElem(XMLTAG_SQLCOMMANDVARIATION))
    {
      pXMLDoc->IntoElem();
      CIFSEntityOneSQLCommandVariation* pSQLCommand;
      pSQLCommand = new CIFSEntityOneSQLCommandVariation(GetSyncGroup(), this);
      if (pSQLCommand)
      {
        if (pSQLCommand->ReadXMLContent(pXMLDoc))
          m_cVariations.push_back(pSQLCommand);
        else
          delete pSQLCommand;
      }
      pXMLDoc->OutOfElem();
    }
    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }
  return true;
}
xtstring CIFSEntitySQLCommandVariations::GetActiveSQLCommandName()
{
  CIFSEntityOneSQLCommandVariation* pAct = GetActiveSQLCommand();
  if (!pAct)
    m_sActiveSQLCommandName = _XT("");
  else
    m_sActiveSQLCommandName = pAct->GetName();
  return m_sActiveSQLCommandName;
}
bool CIFSEntitySQLCommandVariations::IsIdenticalForXPubClient(const CIFSEntitySQLCommandVariations* pSQLVariations) const
{
  assert(pSQLVariations);
  if (!pSQLVariations)
    return false;

  if (CountOfVariations() != pSQLVariations->CountOfVariations())
    return false;

  for (size_t nI = 0; nI < CountOfVariations(); nI++)
  {
    const CIFSEntityOneSQLCommandVariation* pThis = GetConstSQLCommand(nI);
    const CIFSEntityOneSQLCommandVariation* pOther = pSQLVariations->GetConstSQLCommand(nI);
    assert(pThis);
    assert(pOther);
    if (!pThis || !pOther)
      return false;
    if (!pThis->IsIdenticalForXPubClient(pOther))
      return false;
  }
  return true;
}
size_t CIFSEntitySQLCommandVariations::GetIndexOfItem(const CIFSEntityOneSQLCommandVariation* pItem) const
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI] == pItem)
      return nI;
  }
  return (size_t)-1;
}
CIFSEntityOneSQLCommandVariation* CIFSEntitySQLCommandVariations::GetActiveSQLCommand()
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == m_sActiveSQLCommandName)
      return m_cVariations[nI];
  }
  if (m_cVariations.size())
  {
    m_sActiveSQLCommandName = m_cVariations[0]->GetName();
    return m_cVariations[0];
  }
  return 0;
}
const CIFSEntityOneSQLCommandVariation* CIFSEntitySQLCommandVariations::GetConstActiveSQLCommand() const
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == m_sActiveSQLCommandName)
      return m_cVariations[nI];
  }
  /*
  if (m_cVariations.size())
  {
    m_sActiveConditionName = m_cVariations[0]->GetName();
    return m_cVariations[0];
  }
  */
  return 0;
}
CIFSEntityOneSQLCommandVariation* CIFSEntitySQLCommandVariations::GetSQLCommand(size_t nIndex)
{
  if (nIndex < 0 || nIndex >= m_cVariations.size())
    return 0;

  return m_cVariations[nIndex];
}
const CIFSEntityOneSQLCommandVariation* CIFSEntitySQLCommandVariations::GetConstSQLCommand(size_t nIndex) const
{
  if (nIndex < 0 || nIndex >= m_cVariations.size())
    return 0;

  return m_cVariations[nIndex];
}
CIFSEntityOneSQLCommandVariation* CIFSEntitySQLCommandVariations::GetSQLCommand(const xtstring& sName)
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == sName)
      return m_cVariations[nI];
  }
  return 0;
}
const CIFSEntityOneSQLCommandVariation* CIFSEntitySQLCommandVariations::GetConstSQLCommand(const xtstring& sName) const
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == sName)
      return m_cVariations[nI];
  }
  return 0;
}
bool CIFSEntitySQLCommandVariations::AddSQLCommand(const CIFSEntityOneSQLCommandVariation* pSQLCommand)
{
  CIFSEntityOneSQLCommandVariation* pNew;
  pNew = new CIFSEntityOneSQLCommandVariation(GetSyncGroup(), this);
  if (!pNew)
    return false;

  bool bDummy;
  if (!pNew->CopyFrom(pSQLCommand, bDummy))
  {
    delete pNew;
    return false;
  }

  size_t nCount = m_cVariations.size();
  m_cVariations.push_back(pNew);
  return (nCount != m_cVariations.size());
}
bool CIFSEntitySQLCommandVariations::RemoveSQLCommand(size_t nIndex)
{
  if (nIndex < 0 || nIndex >= m_cVariations.size())
    return false;

  delete m_cVariations[nIndex];
  m_cVariations.erase(m_cVariations.begin() + nIndex);
  return true;
}
bool CIFSEntitySQLCommandVariations::RemoveSQLCommand(const xtstring& sName)
{
  for (size_t nI = 0; nI < m_cVariations.size(); nI++)
  {
    if (m_cVariations[nI]->GetName() == sName)
    {
      delete m_cVariations[nI];
      m_cVariations.erase(m_cVariations.begin() + nI);
      return true;
    }
  }
  return false;
}
void CIFSEntitySQLCommandVariations::RemoveAll()
{
  for (CIFSSQLCommandVariationsIterator it = m_cVariations.begin(); it != m_cVariations.end(); it++)
    delete (*it);
  m_cVariations.erase(m_cVariations.begin(), m_cVariations.end());
  return;
}
xtstring CIFSEntitySQLCommandVariations::GetOwnerEntityName() const
{
  if (GetOwnerEntity())
    return GetOwnerEntity()->GetName();
  return _XT("");
}











//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSEntityInd::CIFSEntityInd(CSyncIFSNotifiersGroup* pGroup)
              :CModelBaseInd(tIFSEntity),
               CSyncNotifier_IFS(tIFSNotifier_Entity, pGroup),
               CSyncResponse_Common(tRO_IFS_Entity),
               CSyncResponse_Proj(tRO_IFS_Entity),
               CSyncResponse_ER(tRO_IFS_Entity),
               CSyncResponse_IFS(tRO_IFS_Entity),
               CSyncResponse_WP(tRO_IFS_Entity),
               m_cConditionVariations(pGroup),
               m_cParameters(pGroup),
               m_cSQLCommandVariations(pGroup),
               m_caHitCondition(CConditionOrAssignment::tCOAT_Hit_Condition, false),
               m_caPortCondition(CConditionOrAssignment::tCOAT_Port_Condition, false),
               m_caERTableRealName(CConditionOrAssignment::tCOAT_ERTableRealName_Assignment, false),
               m_caMainCondition(CConditionOrAssignment::tCOAT_Main_Condition, false)
{
  m_pChannelToGUI = 0;

  m_cConditionVariations.SetOwnerEntity(this);
  m_cParameters.SetOwnerEntity(this);
  m_cSQLCommandVariations.SetOwnerEntity(this);

  m_sName.clear();
  m_sComment.clear();
  m_caHitCondition.Reset();
  m_nChildExecution = tChildExecution_Default;
  m_bMainEntity = false;
  m_bDebugMode = false;
  m_bOpenedEntity = false;
  m_sOpenedEntityName.clear();
  m_bShowExpanded = true;
  m_nShowBehaviour = tShowBehaviour_Default;
  m_bShowAddCondition = true;
  m_bShowAddFreeSelection = true;

  m_nTypeOfEntity = tTypeOfEntity_Default;
  m_sSubIFS.clear();
  m_caPortCondition.Reset();
  m_nReadMethod = tReadMethod_Default;
  m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.clear();
  m_caERTableRealName.Reset();
  m_sIdentity.clear();
  m_caMainCondition.Reset();
  m_bDontOptimizeCondition = false;

  m_cConditionVariations.RemoveAll();
  m_cParameters.RemoveAllParameters();
  m_cSQLCommandVariations.RemoveAll();

  m_sWhereClause.clear();
  m_sSortField.clear();
  m_bSortDescending = false;
  m_arrConnections.erase(m_arrConnections.begin(), m_arrConnections.end());
  m_arrConnectionsToChilds.erase(m_arrConnectionsToChilds.begin(), m_arrConnectionsToChilds.end());
  m_mapParamTableValues.erase(m_mapParamTableValues.begin(), m_mapParamTableValues.end());
  m_nPosLeft = 0;
  m_nPosTop = 0;
  m_nSizeWidth = 0;
  m_nSizeHeight = 0;
  //
  m_nColor = 0;
  m_nColorHdr = 0;
  m_nColorHdrText = 0;
  m_nColorHdrSelText = 0;
  m_nColorSelItem = 0;
  m_bMoveable = true;
  m_bSelectable = true;
  m_bVisible = true;
  m_bSizeable = true;
}

CIFSEntityInd::CIFSEntityInd(CIFSEntityInd& cTable)
              :CModelBaseInd(tIFSEntity),
               CSyncNotifier_IFS(tIFSNotifier_Entity, NULL),
               CSyncResponse_Common(tRO_IFS_Entity),
               CSyncResponse_Proj(tRO_IFS_Entity),
               CSyncResponse_ER(tRO_IFS_Entity),
               CSyncResponse_IFS(tRO_IFS_Entity),
               CSyncResponse_WP(tRO_IFS_Entity),
               m_cConditionVariations((CSyncIFSNotifiersGroup*)NULL),
               m_cParameters((CSyncIFSNotifiersGroup*)NULL),
               m_cSQLCommandVariations((CSyncIFSNotifiersGroup*)NULL),
               m_caHitCondition(CConditionOrAssignment::tCOAT_Hit_Condition, false),
               m_caPortCondition(CConditionOrAssignment::tCOAT_Port_Condition, false),
               m_caERTableRealName(CConditionOrAssignment::tCOAT_ERTableRealName_Assignment, false),
               m_caMainCondition(CConditionOrAssignment::tCOAT_Main_Condition, false)
{
  m_pChannelToGUI = 0;

  m_cConditionVariations.SetOwnerEntity(this);
  m_cParameters.SetOwnerEntity(this);
  m_cSQLCommandVariations.SetOwnerEntity(this);

  SetSyncGroup(cTable.GetSyncGroup());
  m_cConditionVariations.SetSyncGroup(cTable.GetSyncGroup());
  m_cParameters.SetSyncGroup(cTable.GetSyncGroup());
  m_cSQLCommandVariations.SetSyncGroup(cTable.GetSyncGroup());
  SetIFSEntityInd(&cTable);
}

CIFSEntityInd::CIFSEntityInd(CIFSEntityInd* pTable)
              :CModelBaseInd(tIFSEntity),
               CSyncNotifier_IFS(tIFSNotifier_Entity, NULL),
               CSyncResponse_Common(tRO_IFS_Entity),
               CSyncResponse_Proj(tRO_IFS_Entity),
               CSyncResponse_ER(tRO_IFS_Entity),
               CSyncResponse_IFS(tRO_IFS_Entity),
               CSyncResponse_WP(tRO_IFS_Entity),
               m_cConditionVariations((CSyncIFSNotifiersGroup*)NULL),
               m_cParameters((CSyncIFSNotifiersGroup*)NULL),
               m_cSQLCommandVariations((CSyncIFSNotifiersGroup*)NULL),
               m_caHitCondition(CConditionOrAssignment::tCOAT_Hit_Condition, false),
               m_caPortCondition(CConditionOrAssignment::tCOAT_Port_Condition, false),
               m_caERTableRealName(CConditionOrAssignment::tCOAT_ERTableRealName_Assignment, false),
               m_caMainCondition(CConditionOrAssignment::tCOAT_Main_Condition, false)
{
  m_pChannelToGUI = 0;

  m_cConditionVariations.SetOwnerEntity(this);
  m_cParameters.SetOwnerEntity(this);
  m_cSQLCommandVariations.SetOwnerEntity(this);

  assert(pTable);
  SetSyncGroup(pTable->GetSyncGroup());
  m_cConditionVariations.SetSyncGroup(pTable->GetSyncGroup());
  m_cParameters.SetSyncGroup(pTable->GetSyncGroup());
  m_cSQLCommandVariations.SetSyncGroup(pTable->GetSyncGroup());
  SetIFSEntityInd(pTable);
}

CIFSEntityInd::~CIFSEntityInd()
{
  RemoveAllFields();
  RemoveAllConnections();
  RemoveAllConnectionsToChilds();
  RemoveAllHitNames();
  RemoveAllIFSVariableInds();
  RemoveAllIFSAssignmentInds();
  RemoveAllPreProcessInds();
  RemoveAllPostProcessInds();
  RemoveAllGlobalERTableInds();
}

#define GLOBALTABLEPREFIX           _XT("__GTBL__:")
#define PROPERTYSTONETYPEPREFIX     _XT("__PST__:")
#define DBSTONENAMEPREFIX           _XT("__DBST__:")
xtstring CIFSEntityInd::GetERTable_FromERTableOrGlobalTable()
{
  if (GetERTableFromRaw().size())
  {
    return GetERTableFromRaw();
  }
  if (GetGlobalTableFromRaw().size())
  {
    if (GetMainEntity())
    {
      if (GetGlobalERTableInd(GetGlobalTableFromRaw()))
        return GetGlobalERTableInd(GetGlobalTableFromRaw())->GetERTableName();
    }
    else
    {
      return SE_MF_GetERTableFromGlobalTable(GetGlobalTableFromRaw());
    }
  }
  return _XT("");
}
bool CIFSEntityInd::GetERTableUsed() const
{
  return (GetERTableFromRaw().size() != 0);
}
bool CIFSEntityInd::GetGlobalTableUsed() const
{
  return (GetGlobalTableFromRaw().size() != 0);
}
bool CIFSEntityInd::GetPropStoneTypeUsed() const
{
  return (GetPropStoneTypeFromRaw() != -1);
}
bool CIFSEntityInd::GetDBStoneNameUsed() const
{
  return (GetDBStoneNameFromRaw().size() != 0);
}
xtstring CIFSEntityInd::GetERTableFromRaw() const
{
  size_t nPos1 = m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.find(GLOBALTABLEPREFIX);
  size_t nPos2 = m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.find(PROPERTYSTONETYPEPREFIX);
  size_t nPos3 = m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.find(DBSTONENAMEPREFIX);
  if (nPos1 == xtstring::npos && nPos2 == xtstring::npos && nPos3 == xtstring::npos)
    return m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw;
  return _XT("");
}
void CIFSEntityInd::SetERTableToRaw(const xtstring& sERTable)
{
  m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw = sERTable;
}
xtstring CIFSEntityInd::GetGlobalTableFromRaw() const
{
  size_t nPos = m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.find(GLOBALTABLEPREFIX);
  if (nPos == xtstring::npos)
    return _XT("");
  xtstring sPrefix;
  sPrefix = GLOBALTABLEPREFIX;
  xtstring sRetVal;
  sRetVal.assign(m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.begin() + sPrefix.size(),
                 m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.end());
  return sRetVal;
}
void CIFSEntityInd::SetGlobalTableToRaw(const xtstring& sGlobalTable)
{
  xtstring sVal;
  sVal.Format(_XT("%s%s"), GLOBALTABLEPREFIX, sGlobalTable.c_str());
  m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw = sVal;
}
int CIFSEntityInd::GetPropStoneTypeFromRaw() const
{
  size_t nPos = m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.find(PROPERTYSTONETYPEPREFIX);
  if (nPos == xtstring::npos)
    return -1;
  xtstring sPrefix;
  sPrefix = PROPERTYSTONETYPEPREFIX;
  xtstring sVal;
  sVal.assign(m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.begin() + sPrefix.size(),
              m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.end());
  int nRetVal;
  nRetVal = _ttoi(sVal.c_str());
  return nRetVal;
}
void CIFSEntityInd::SetPropStoneTypeToRaw(int nPropStoneType)
{
  xtstring sVal;
  sVal.Format(_XT("%s%ld"), PROPERTYSTONETYPEPREFIX, nPropStoneType);
  m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw = sVal;
}
xtstring CIFSEntityInd::GetDBStoneNameFromRaw() const
{
  size_t nPos = m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.find(DBSTONENAMEPREFIX);
  if (nPos == xtstring::npos)
    return _XT("");
  xtstring sPrefix;
  sPrefix = DBSTONENAMEPREFIX;
  xtstring sRetVal;
  sRetVal.assign(m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.begin() + sPrefix.size(),
                 m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw.end());
  return sRetVal;
}
void CIFSEntityInd::SetDBStoneNameToRaw(const xtstring& sDBStoneName)
{
  xtstring sVal;
  sVal.Format(_XT("%s%s"), DBSTONENAMEPREFIX, sDBStoneName.c_str());
  m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw = sVal;
}

void CIFSEntityInd::PropertiesChanged()
{
  if (m_pChannelToGUI)
    m_pChannelToGUI->PropertiesChanged();
}

void CIFSEntityInd::SE_OnParamTableChanged()
{
  if (GetMainEntity())
    PropertiesChanged();
}

void CIFSEntityInd::OnTableNameChanged(const xtstring& sOldName,
                                       const xtstring& sNewName)
{
//  if (!GetMainEntity())
  {
    if (GetERTableFromRaw() == sOldName)
    {
      SetERTableToRaw(sNewName);
      PropertiesChanged();
    }
  }
}

void CIFSEntityInd::SetIFSEntityInd(CIFSEntityInd* pTable,
                                    bool bWithConnections /*= true*/)
{
  if (!pTable)
    return;

  m_sName = pTable->m_sName;
  m_sComment = pTable->m_sComment;
  m_bMainEntity = pTable->m_bMainEntity;
  m_bDebugMode = pTable->m_bDebugMode;
  m_bOpenedEntity = pTable->m_bOpenedEntity;
  m_sOpenedEntityName = pTable->m_sOpenedEntityName;
  m_bShowExpanded = pTable->m_bShowExpanded;
  m_nShowBehaviour = pTable->m_nShowBehaviour;
  m_bShowAddCondition = pTable->m_bShowAddCondition;
  m_bShowAddFreeSelection = pTable->m_bShowAddFreeSelection;

  m_nTypeOfEntity = pTable->m_nTypeOfEntity;
  m_sSubIFS = pTable->m_sSubIFS;
  m_caPortCondition = pTable->m_caPortCondition;
  m_nReadMethod = pTable->m_nReadMethod;
  m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw = pTable->m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw;
  m_caERTableRealName = pTable->m_caERTableRealName;
  m_sIdentity = pTable->m_sIdentity;
  m_caMainCondition = pTable->m_caMainCondition;
  m_bDontOptimizeCondition = pTable->m_bDontOptimizeCondition;
  m_sWhereClause = pTable->m_sWhereClause;
  m_sSortField = pTable->m_sSortField;
  m_bSortDescending = pTable->m_bSortDescending;
  m_nPosLeft = pTable->m_nPosLeft;
  m_nPosTop = pTable->m_nPosTop;
  m_nSizeWidth = pTable->m_nSizeWidth;
  m_nSizeHeight = pTable->m_nSizeHeight;
  m_bSynchronize = pTable->m_bSynchronize;
  m_nColor = pTable->m_nColor;
  m_nColorBack = pTable->m_nColorBack;
  m_nColorHdr = pTable->m_nColorHdr;
  m_nColorHdrSel = pTable->m_nColorHdrSel;
  m_nColorHdrText = pTable->m_nColorHdrText;
  m_nColorHdrSelText = pTable->m_nColorHdrSelText;
  m_nColorSelItem = pTable->m_nColorSelItem;
  m_bMoveable = pTable->m_bMoveable;
  m_bSelectable = pTable->m_bSelectable;
  m_bVisible = pTable->m_bVisible;
  m_bSizeable = pTable->m_bSizeable;
  m_sFontName = pTable->m_sFontName;
  m_nFontHeight = pTable->m_nFontHeight;
  m_caHitCondition = pTable->m_caHitCondition;
  m_nChildExecution = pTable->m_nChildExecution;

  m_cConditionVariations.AssignFrom(pTable->GetConditionVariations());
  m_cConditionVariations.SetOwnerEntity(this);
  m_cParameters.AssignFrom(pTable->GetEntityParameters());
  m_cParameters.SetOwnerEntity(this);
  m_cSQLCommandVariations.AssignFrom(pTable->GetSQLCommandVariations());
  m_cSQLCommandVariations.SetOwnerEntity(this);


  size_t nI;
  RemoveAllFields();
  for ( nI = 0; nI < pTable->m_arrFields.size(); nI++)
    m_arrFields.push_back(pTable->m_arrFields[nI]);

  if (bWithConnections)
  {
    RemoveAllConnections();
    for (nI = 0; nI < pTable->m_arrConnections.size(); nI++)
      AddConnection(pTable->m_arrConnections[nI]);
    RemoveAllConnectionsToChilds();
    for (nI = 0; nI < pTable->m_arrConnectionsToChilds.size(); nI++)
      AddConnectionToChilds(pTable->m_arrConnectionsToChilds[nI]);
  }

  RemoveAllHitNames();
  for (nI = 0; nI < pTable->m_arrHitNames.size(); nI++)
    AddHitName(pTable->m_arrHitNames[nI]);
  RemoveAllIFSVariableInds();
  for (nI = 0; nI < pTable->CountOfIFSVariableInds(); nI++)
    AddIFSVariableInd(pTable->GetIFSVariableInd(nI));
  RemoveAllIFSAssignmentInds();
  for (nI = 0; nI < pTable->CountOfIFSAssignmentInds(); nI++)
    AddIFSAssignmentInd(pTable->GetIFSAssignmentInd(nI));
  RemoveAllPreProcessInds();
  for (nI = 0; nI < pTable->CountOfPreProcessInds(); nI++)
    AddPreProcessInd(pTable->GetPreProcessInd(nI));
  RemoveAllPostProcessInds();
  for (nI = 0; nI < pTable->CountOfPostProcessInds(); nI++)
    AddPostProcessInd(pTable->GetPostProcessInd(nI));
  RemoveAllGlobalERTableInds();
  for (nI = 0; nI < pTable->CountOfGlobalERTableInds(); nI++)
    AddGlobalERTableInd(pTable->GetGlobalERTableInd(nI));

  m_mapParamTableValues.erase(m_mapParamTableValues.begin(), m_mapParamTableValues.end());
  m_mapParamTableValues.insert(pTable->m_mapParamTableValues.begin(),
                               pTable->m_mapParamTableValues.end());
}

xtstring CIFSEntityInd::GetXMLDoc()
{
  bool bRet = true;
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(EMPTY_STANDALONE_NODE_DOCUMENT);

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(EMPTY_STANDALONE_NODE))
    bRet = SaveXMLContent(&cXMLDoc);
  else
    bRet = false;

  if (!bRet)
  {
    assert(false);
    return _XT("");
  }
  xtstring sXMLDocText = cXMLDoc.GetDoc();
  return sXMLDocText;
}

bool CIFSEntityInd::SetXMLDoc(const xtstring& sXMLDoc)
{
  bool bRet = true;
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXMLDoc.c_str()))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == EMPTY_STANDALONE_NODE)
  {
    if (cXMLDoc.FindChildElem(NODE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      bRet = ReadXMLContent(&cXMLDoc);
      cXMLDoc.OutOfElem();
    }
    else
      bRet = false;
  }
  else
    bRet = false;

  if (!bRet)
  {
    assert(false);
    return false;
  }
  return true;
}

bool CIFSEntityInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(NODE_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Node>
    pXMLDoc->SetAttrib(NODE_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_COMMENT, GetComment().c_str());
    m_caHitCondition.SaveXMLContent(pXMLDoc);
    pXMLDoc->SetAttrib(NODE_ATTRIB_MAINNODE, GetMainEntity());
    pXMLDoc->SetAttrib(NODE_ATTRIB_DEBUGMODE, GetDebugMode());
    pXMLDoc->SetAttrib(NODE_ATTRIB_OPENEDENTITY, GetOpenedEntity());
    pXMLDoc->SetAttrib(NODE_ATTRIB_OPENEDENTITYNAME, GetOpenedEntityName().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_SHOWEXPANDED, GetShowExpanded());
    pXMLDoc->SetAttrib(NODE_ATTRIB_SHOWBEHAVIOUR, GetShowBehaviour());
    pXMLDoc->SetAttrib(NODE_ATTRIB_SHOWADDCONDITION, GetShowAddCondition());
    pXMLDoc->SetAttrib(NODE_ATTRIB_SHOWADDFREESELECTION, GetShowAddFreeSelection());
//    if (!GetMainEntity())
    {
      pXMLDoc->SetAttrib(NODE_ATTRIB_CHILD_EXECUTION, GetChildExecution());
      pXMLDoc->SetAttrib(NODE_ATTRIB_TYPEOFENTITY, GetTypeOfEntity());
      if (!GetMainEntity())
        pXMLDoc->SetAttrib(NODE_ATTRIB_SUBIFS, GetSubIFS().c_str());
      m_caPortCondition.SaveXMLContent(pXMLDoc);
      pXMLDoc->SetAttrib(NODE_ATTRIB_READMETHOD, GetReadMethod());
      pXMLDoc->SetAttrib(NODE_ATTRIB_ERTABLEORGLOBALTABLEORPROPSTONETYPEORDBSTONENAME, GetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw().c_str());
      m_caERTableRealName.SaveXMLContent(pXMLDoc);
      pXMLDoc->SetAttrib(NODE_ATTRIB_IDENTITY, GetIdentity().c_str());
      m_caMainCondition.SaveXMLContent(pXMLDoc);
      pXMLDoc->SetAttrib(NODE_ATTRIB_DONTOPTIMIZECONDITION, GetDontOptimizeCondition());
      m_cConditionVariations.SaveXMLContent(pXMLDoc);
      m_cParameters.SaveXMLContent(pXMLDoc);
      m_cSQLCommandVariations.SaveXMLContent(pXMLDoc);
      pXMLDoc->SetAttrib(NODE_ATTRIB_WHERECLAUSE, GetWhereClause().c_str());
      pXMLDoc->SetAttrib(NODE_ATTRIB_SORTFIELD, GetSortField().c_str());
      pXMLDoc->SetAttrib(NODE_ATTRIB_SORTDESC, GetSortDescending());
    }
    if (GetMainEntity())
    {
      // parameter table values (only main entity)
      if (pXMLDoc->AddChildElem(PARAMTABLE_VALUES_NAME, _XT("")))
      {
        pXMLDoc->IntoElem();
        // in <ParamTableValues>
        for (size_t nI = 0; nI < GetCountOfParamTableValues(); nI++)
        {
          XTCHAR sNameA[100];
          XTCHAR sValueA[100];
          sprintf(sNameA, NODE_ATTRIB_PARAMTABLE_FIELD, nI);
          sprintf(sValueA, NODE_ATTRIB_PARAMTABLE_VALUE, nI);
          xtstring sName;
          xtstring sValue;
          if (GetParamTableFieldName(nI, sName))
          {
            if (GetParamTableFieldValue(nI, sValue))
            {
              pXMLDoc->SetAttrib(sNameA, sName.c_str());
              pXMLDoc->SetAttrib(sValueA, sValue.c_str());
            }
          }
        }
        pXMLDoc->OutOfElem();
        // in <Node>
      }
    }
    // save fields
    if (pXMLDoc->AddChildElem(FIELDS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Fields>
      for (size_t nI = 0; nI < CountOfFields(); nI++)
        GetField(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    // save relations to Parent
    if (pXMLDoc->AddChildElem(CONNECTIONS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Connections>
      for (size_t nI = 0; nI < CountOfConnections(); nI++)
        GetConnection(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    // save relations to Child
    if (pXMLDoc->AddChildElem(CONNECTIONSCHILD_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Connections>
      for (size_t nI = 0; nI < CountOfConnectionsToChilds(); nI++)
        GetConnectionToChilds(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    // save hit names
    if (pXMLDoc->AddChildElem(HITNAMES_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <HitNames>
      for (size_t nI = 0; nI < CountOfHitNames(); nI++)
        GetHitName(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    // save variables
    if (pXMLDoc->AddChildElem(VARIABLES_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Variables>
      for (size_t nI = 0; nI < CountOfIFSVariableInds(); nI++)
        GetIFSVariableInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    // save assignments
    if (pXMLDoc->AddChildElem(ASSIGNMENTS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Assignments>
      for (size_t nI = 0; nI < CountOfIFSAssignmentInds(); nI++)
        GetIFSAssignmentInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    // save pre process
    if (pXMLDoc->AddChildElem(PREPROCESSES_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <PreProcesses>
      for (size_t nI = 0; nI < CountOfPreProcessInds(); nI++)
        GetPreProcessInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    // save post process
    if (pXMLDoc->AddChildElem(POSTPROCESSES_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <PostProcesses>
      for (size_t nI = 0; nI < CountOfPostProcessInds(); nI++)
        GetPostProcessInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    // save global ertable
    if (pXMLDoc->AddChildElem(GLOBALERTABLES_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <GlobalERTables>
      for (size_t nI = 0; nI < CountOfGlobalERTableInds(); nI++)
        GetGlobalERTableInd(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <MainProperty>
    }
    // load UI
    if (pXMLDoc->AddChildElem(NODEUI_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <NodeUI>
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_X, GetLeft());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_Y, GetTop());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_CX, GetWidth());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_CY, GetHeight());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_SYNC, GetSynchronize());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR, GetColor());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR_BACK, GetColorBack());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR_HDR, GetColorHdr());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR_HDR_SEL, GetColorHdrSel());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR_HDR_TEXT, GetColorHdrText());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR_HDR_SEL_TEXT, GetColorHdrSelText());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_COLOR_SEL_ITEM, GetColorSelItem());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_MOVEABLE, GetMoveable());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_SELECTABLE, GetSelectable());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_VISIBLE, GetVisible());
      pXMLDoc->SetAttrib(NODE_ATTRIB_UI_SIZEABLE, GetSizeable());

      pXMLDoc->OutOfElem();
      // in <Node>
    }

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CIFSEntityInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  long nVal;

  // in <Node>
  SetName(xtstring(pXMLDoc->GetAttrib(NODE_ATTRIB_NAME)));
  SetComment(xtstring(pXMLDoc->GetAttrib(NODE_ATTRIB_COMMENT)));
  m_caHitCondition.ReadXMLContent(pXMLDoc);
  SetMainEntity(pXMLDoc->GetAttribLong(NODE_ATTRIB_MAINNODE) != 0);
  SetDebugMode(pXMLDoc->GetAttribLong(NODE_ATTRIB_DEBUGMODE) != 0);
  SetOpenedEntity(pXMLDoc->GetAttribLong(NODE_ATTRIB_OPENEDENTITY) != 0);
  SetOpenedEntityName(pXMLDoc->GetAttrib(NODE_ATTRIB_OPENEDENTITYNAME));
  SetShowExpanded(pXMLDoc->GetAttribLong(NODE_ATTRIB_SHOWEXPANDED) != 0);
  SetShowBehaviour((TShowBehaviour)pXMLDoc->GetAttribLong(NODE_ATTRIB_SHOWBEHAVIOUR));
  if (GetShowBehaviour() <= tShowBehaviour_FirstDummy || GetShowBehaviour() >= tShowBehaviour_LastDummy)
    SetShowBehaviour(tShowBehaviour_Default);
  SetShowAddCondition(pXMLDoc->GetAttribLong(NODE_ATTRIB_SHOWADDCONDITION) != 0);
  SetShowAddFreeSelection(pXMLDoc->GetAttribLong(NODE_ATTRIB_SHOWADDFREESELECTION) != 0);
//  if (!GetMainEntity())
  {
    nVal = pXMLDoc->GetAttribLong(NODE_ATTRIB_CHILD_EXECUTION);
    if (nVal <= tChildExecution_FirstDummy || nVal >= tChildExecution_LastDummy)
      nVal = tChildExecution_Default;
    SetChildExecution((TChildExecution)nVal);
    nVal = pXMLDoc->GetAttribLong(NODE_ATTRIB_TYPEOFENTITY);
    if (nVal <= tTypeOfEntity_FirstDummy || nVal >= tTypeOfEntity_LastDummy)
      nVal = tTypeOfEntity_Default;
    SetTypeOfEntity((TTypeOfEntity)nVal);
    if (!GetMainEntity())
      SetSubIFS(pXMLDoc->GetAttrib(NODE_ATTRIB_SUBIFS));
    m_caPortCondition.ReadXMLContent(pXMLDoc);
    nVal = pXMLDoc->GetAttribLong(NODE_ATTRIB_READMETHOD);
    if (nVal <= tReadMethod_FirstDummy || nVal >= tReadMethod_LastDummy)
      nVal = tReadMethod_Default;
    SetReadMethod((TReadMethod)nVal);
    SetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw(xtstring(pXMLDoc->GetAttrib(NODE_ATTRIB_ERTABLEORGLOBALTABLEORPROPSTONETYPEORDBSTONENAME)));
    // folgende sechs Zeilen irgendwann loeschen -->
    if (GetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw().size() == 0)
      SetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw(pXMLDoc->GetAttrib(_XT("ERTableOrGlobalTableOrPropStoneType")));
    if (GetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw().size() == 0)
      SetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw(pXMLDoc->GetAttrib(_XT("ERTableOrPropStoneType")));
    if (GetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw().size() == 0)
      SetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw(pXMLDoc->GetAttrib(_XT("ERTable")));
    // vorherige sechs Zeilen irgendwann loeschen <--
    m_caERTableRealName.ReadXMLContent(pXMLDoc);
    SetIdentity(xtstring(pXMLDoc->GetAttrib(NODE_ATTRIB_IDENTITY)));
    m_caMainCondition.ReadXMLContent(pXMLDoc);
    SetDontOptimizeCondition(pXMLDoc->GetAttribLong(NODE_ATTRIB_DONTOPTIMIZECONDITION) != 0);
    m_cConditionVariations.ReadXMLContent(pXMLDoc);
    m_cParameters.ReadXMLContent(pXMLDoc);
    m_cSQLCommandVariations.ReadXMLContent(pXMLDoc);
    SetWhereClause(pXMLDoc->GetAttrib(NODE_ATTRIB_WHERECLAUSE));
    SetSortField(xtstring(pXMLDoc->GetAttrib(NODE_ATTRIB_SORTFIELD)));
    SetSortDescending(pXMLDoc->GetAttribLong(NODE_ATTRIB_SORTDESC) != 0);
  }
  RemoveAllParamTableFieldValues();
  if (GetMainEntity())
  {
    // parameter table values (only main entity)
    if (pXMLDoc->FindChildElem(PARAMTABLE_VALUES_NAME))
    {
      pXMLDoc->IntoElem();
      // in <ParamTableValues>
      for (size_t nI = 0; ; nI++)
      {
        XTCHAR sNameA[100];
        XTCHAR sValueA[100];
        sprintf(sNameA, NODE_ATTRIB_PARAMTABLE_FIELD, nI);
        sprintf(sValueA, NODE_ATTRIB_PARAMTABLE_VALUE, nI);
        xtstring sName;
        xtstring sValue;
        sName = pXMLDoc->GetAttrib(sNameA);
        if (!sName.size())
          break;
        sValue = pXMLDoc->GetAttrib(sValueA);
        AddParamTableFieldValue(sName, sValue);
      }
      pXMLDoc->OutOfElem();
      // in <Node>
    }
  }
  RemoveAllFields();
  // load fields
  if (pXMLDoc->FindChildElem(FIELDS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Fields>
    while (pXMLDoc->FindChildElem(FIELD_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Field>

      CIFSEntityFieldInd cField(GetSyncGroup());
      cField.ReadXMLContent(pXMLDoc);
      AddField(&cField);

      pXMLDoc->OutOfElem();
      // in <Fields>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
  RemoveAllConnections();
  // load relations to Parent
  if (pXMLDoc->FindChildElem(CONNECTIONS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Connections>
    while (pXMLDoc->FindChildElem(CONNECTION_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Connection>

      CIFSEntityConnectionInd cConnection(GetSyncGroup());
      cConnection.ReadXMLContent(pXMLDoc);
      AddConnection(&cConnection);

      pXMLDoc->OutOfElem();
      // in <Fields>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
  RemoveAllConnectionsToChilds();
  // load relations to Child
  if (pXMLDoc->FindChildElem(CONNECTIONSCHILD_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Connections>
    while (pXMLDoc->FindChildElem(CONNECTION_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Connection>

      CIFSEntityConnectionInd cConnection(GetSyncGroup());
      cConnection.ReadXMLContent(pXMLDoc);
      AddConnectionToChilds(&cConnection);

      pXMLDoc->OutOfElem();
      // in <Fields>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
  RemoveAllHitNames();
  // load hit names
  if (pXMLDoc->FindChildElem(HITNAMES_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <HitNames>
    while (pXMLDoc->FindChildElem(HITNAME_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <HitName>

      CIFSHitNameInd cHitName(GetSyncGroup());
      cHitName.SetOwnerEntity(this);
      cHitName.ReadXMLContent(pXMLDoc);
      AddHitName(&cHitName);

      pXMLDoc->OutOfElem();
      // in <Fields>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
/* HITCOUNT Es ist nicht mehr verlangt, dass ein Treffer immer da sein muss
  if (CountOfHitNames() == 0)
  {
    // add default
    CIFSHitNameInd cHitName(GetSyncGroup());
    cHitName.SetOwnerEntity(this);
    AddHitName(&cHitName);
  }
*/
  // load variables
  RemoveAllIFSVariableInds();
  if (pXMLDoc->FindChildElem(VARIABLES_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Variables>
    while (pXMLDoc->FindChildElem(VARIABLE_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Variable>
      CIFSEntityVariableInd cModel(GetSyncGroup());
      cModel.SetOwnerEntity(this);
      cModel.ReadXMLContent(pXMLDoc);
      AddIFSVariableInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <Variables>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load assignements
  RemoveAllIFSAssignmentInds();
  if (pXMLDoc->FindChildElem(ASSIGNMENTS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Assignments>
    while (pXMLDoc->FindChildElem(ASSIGNMENT_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Assignment>
      CIFSEntityAssignmentInd cModel(GetSyncGroup());
      cModel.SetOwnerEntity(this);
      cModel.ReadXMLContent(pXMLDoc);
      AddIFSAssignmentInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <Assignments>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load pre process
  RemoveAllPreProcessInds();
  if (pXMLDoc->FindChildElem(PREPROCESSES_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <PreProcesses>
    while (pXMLDoc->FindChildElem(PREPROCESS_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <PreProcess>
      CIFSEntityPrePostProcessInd cModel(true, GetSyncGroup());
      cModel.SetOwnerEntity(this);
      cModel.ReadXMLContent(pXMLDoc);
      AddPreProcessInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <PreProcesses>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load post process
  RemoveAllPostProcessInds();
  if (pXMLDoc->FindChildElem(POSTPROCESSES_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <PostProcesses>
    while (pXMLDoc->FindChildElem(POSTPROCESS_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <PostProcess>
      CIFSEntityPrePostProcessInd cModel(false, GetSyncGroup());
      cModel.SetOwnerEntity(this);
      cModel.ReadXMLContent(pXMLDoc);
      AddPostProcessInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <PostProcesses>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load global ertable
  RemoveAllGlobalERTableInds();
  if (pXMLDoc->FindChildElem(GLOBALERTABLES_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <GlobalERTables>
    while (pXMLDoc->FindChildElem(GLOBALERTABLE_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <GlobalERTable>
      CIFSEntityGlobalERTableInd cModel(GetSyncGroup());
      cModel.SetOwnerEntity(this);
      cModel.ReadXMLContent(pXMLDoc);
      AddGlobalERTableInd(&cModel);
      pXMLDoc->OutOfElem();
      // in <GlobalERTables>
    }
    pXMLDoc->OutOfElem();
    // in <MainProperty>
  }
  // load UI
  if (pXMLDoc->FindChildElem(NODEUI_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <NodeUI>

    SetLeft(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_X));
    SetTop(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_Y));
    SetWidth(abs(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_CX)));
    SetHeight(abs(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_CY)));
    SetSynchronize(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_SYNC) != 0);
    SetColor(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR));
    SetColorBack(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_BACK));
    SetColorHdr(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_HDR));
    SetColorHdrSel(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_HDR_SEL));
    SetColorHdrText(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_HDR_TEXT));
    SetColorHdrSelText(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_HDR_SEL_TEXT));
    SetColorSelItem(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_SEL_ITEM));
    SetMoveable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_MOVEABLE) != 0);
    SetSelectable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_SELECTABLE) != 0);
    SetVisible(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_VISIBLE) != 0);
    SetSizeable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_SIZEABLE) != 0);

    pXMLDoc->OutOfElem();
    // in <Node>
    bRet = true;
  }

  return bRet;
}

// parameter table values (only main entity)
bool CIFSEntityInd::GetParamTableFieldName(size_t nIndex,
                                           xtstring& sName)
{
  if (m_mapParamTableValues.size() <= nIndex)
    return false;
  CxtstringMapIterator it = m_mapParamTableValues.begin();
  for (size_t nI = 0; nI < m_mapParamTableValues.size(); it++, nI++)
  {
    if (nI == nIndex)
    {
      sName = it->first;
      return true;
    }
  }
  return false;
}

// parameter table values (only main entity)
bool CIFSEntityInd::GetParamTableFieldValue(size_t nIndex,
                                            xtstring& sValue)
{
  if (m_mapParamTableValues.size() <= nIndex)
    return false;
  CxtstringMapIterator it = m_mapParamTableValues.begin();
  for (size_t nI = 0; nI < m_mapParamTableValues.size(); it++, nI++)
  {
    if (nI == nIndex)
    {
      sValue = it->second;
      return true;
    }
  }
  return false;
}

// parameter table values (only main entity)
bool CIFSEntityInd::GetParamTableFieldValue(const xtstring& sFieldName,
                                            xtstring& sValue)
{
  CxtstringMapConstIterator cit;
  cit = m_mapParamTableValues.find(sFieldName);
  if (cit == m_mapParamTableValues.end())
    return false;
  sValue = cit->second;
  return true;
}

// parameter table values (only main entity)
bool CIFSEntityInd::AddParamTableFieldValue(const xtstring& sFieldName,
                                            const xtstring& sFieldValue)
{
  CxtstringMapPair cNewValue(sFieldName, sFieldValue);
  CxtstringMapInsert cResult = m_mapParamTableValues.insert(cNewValue);
  return cResult.second;
}

// parameter table values (only main entity)
bool CIFSEntityInd::RemoveAllParamTableFieldValues()
{
  m_mapParamTableValues.erase(m_mapParamTableValues.begin(), m_mapParamTableValues.end());
  return true;
}

// parameter table values (only main entity)
bool CIFSEntityInd::AppendParamTableFieldValues(const CxtstringMap& cMap)
{
  m_mapParamTableValues.insert(cMap.begin(), cMap.end());
  return true;
}

void CIFSEntityInd::CopyDiverseProperties(CIFSEntityInd* pTable)
{
  m_bSynchronize = pTable->m_bSynchronize;
  m_nColor = pTable->m_nColor;
  m_nColorBack = pTable->m_nColorBack;
  m_nColorHdr = pTable->m_nColorHdr;
  m_nColorHdrSel = pTable->m_nColorHdrSel;
  m_nColorHdrText = pTable->m_nColorHdrText;
  m_nColorHdrSelText = pTable->m_nColorHdrSelText;
  m_nColorSelItem = pTable->m_nColorSelItem;
  m_bMoveable = pTable->m_bMoveable;
  m_bSelectable = pTable->m_bSelectable;
  m_bVisible = pTable->m_bVisible;
  m_bSizeable = pTable->m_bSizeable;
}

void CIFSEntityInd::CopyPositionProperties(CIFSEntityInd* pTable)
{
  m_nPosLeft = pTable->m_nPosLeft;
  m_nPosTop = pTable->m_nPosTop;
  m_nSizeWidth = pTable->m_nSizeWidth;
  m_nSizeHeight = pTable->m_nSizeHeight;
}

CIFSEntityFieldInd* CIFSEntityInd::GetField(const xtstring& sName)
{
  CIFSEntityFieldInd* pRetField = NULL;
  for (size_t nI = 0; nI < m_arrFields.size(); nI++)
  {
    CIFSEntityFieldInd* pField = m_arrFields[nI];
    if (pField->GetName() == sName)
    {
      pRetField = pField;
      break;
    }
  }
  return pRetField;
}

bool CIFSEntityInd::RemoveField(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrFields.size());
  if (nIndex >= 0 && nIndex < m_arrFields.size())
  {
    CIFSEntityFieldInd* pR = m_arrFields[nIndex];
    delete pR;
    m_arrFields.erase(m_arrFields.begin() + nIndex);
    return true;
  }
  return false;
}

bool CIFSEntityInd::RemoveField(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfFields(); nI++)
  {
    CIFSEntityFieldInd* pField = GetField(nI);
    if (sName == pField->GetName())
      return RemoveField(nI);
  }
  return false;
}

bool CIFSEntityInd::RemoveAllFields()
{
  while (m_arrFields.size())
    RemoveField(0);
  m_arrFields.erase(m_arrFields.begin(), m_arrFields.end());
  return true;
};

CIFSEntityConnectionInd* CIFSEntityInd::GetConnection(const xtstring& sName)
{
  CIFSEntityConnectionInd* pRetRel = NULL;
  for (size_t nI = 0; nI < m_arrConnections.size(); nI++)
  {
    CIFSEntityConnectionInd* pRel = m_arrConnections[nI];
    if (pRel->GetName() == sName)
    {
      pRetRel = pRel;
      break;
    }
  }
  return pRetRel;
}

CIFSEntityConnectionInd* CIFSEntityInd::GetConnectionToChilds(const xtstring& sName,
                                                              xtstring sChildEntity)
{
  CIFSEntityConnectionInd* pRetRel = NULL;
  for (size_t nI = 0; nI < m_arrConnectionsToChilds.size(); nI++)
  {
    CIFSEntityConnectionInd* pRel = m_arrConnectionsToChilds[nI];
    if (pRel->GetName() == sName
      && pRel->GetChildEntity() == sChildEntity)
    {
      pRetRel = pRel;
      break;
    }
  }
  return pRetRel;
}

bool CIFSEntityInd::RemoveConnection(CIFSEntityConnectionInd *pConnection)
{
  for (size_t nI = 0; nI < m_arrConnections.size(); nI++)
  {
    if (*(m_arrConnections[nI]) == *pConnection)
    {
      CIFSEntityConnectionInd* pRel = m_arrConnections[nI];
      delete pRel;
      m_arrConnections.erase(m_arrConnections.begin() + nI);
      return true;
    }
  }
  return false;
}

bool CIFSEntityInd::RemoveAllConnections()
{
  while (m_arrConnections.size())
    RemoveConnection(m_arrConnections[0]);
  m_arrConnections.erase(m_arrConnections.begin(), m_arrConnections.end());
  return true;
}

bool CIFSEntityInd::RemoveConnectionToChilds(CIFSEntityConnectionInd *pConnection)
{
  for (size_t nI = 0; nI < m_arrConnectionsToChilds.size(); nI++)
  {
    if (*(m_arrConnectionsToChilds[nI]) == *pConnection)
    {
      CIFSEntityConnectionInd* pRel = m_arrConnectionsToChilds[nI];
      delete pRel;
      m_arrConnectionsToChilds.erase(m_arrConnectionsToChilds.begin() + nI);
      return true;
    }
  }
  return false;
}

bool CIFSEntityInd::RemoveAllConnectionsToChilds()
{
  while (m_arrConnectionsToChilds.size())
    RemoveConnectionToChilds(m_arrConnectionsToChilds[0]);
  m_arrConnectionsToChilds.erase(m_arrConnectionsToChilds.begin(), m_arrConnectionsToChilds.end());
  return true;
}

bool CIFSEntityInd::RemoveHitName(CIFSHitNameInd *pName)
{
  for (size_t nI = 0; nI < m_arrHitNames.size(); nI++)
  {
    if (*(m_arrHitNames[nI]) == *pName)
    {
      CIFSHitNameInd* pHitName = m_arrHitNames[nI];
      delete pHitName;
      m_arrHitNames.erase(m_arrHitNames.begin() + nI);
      return true;
    }
  }
  return false;
}

bool CIFSEntityInd::RemoveAllHitNames()
{
  while (m_arrHitNames.size())
    RemoveHitName(m_arrHitNames[0]);
  m_arrHitNames.erase(m_arrHitNames.begin(), m_arrHitNames.end());
  return true;
}

CIFSEntityVariableInd* CIFSEntityInd::GetIFSVariableInd(const xtstring& sName)
{
  CIFSEntityVariableInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfIFSVariableInds(); nI++)
  {
    if (sName == m_arrVariables[nI]->GetName())
    {
      pRetModel = m_arrVariables[nI];
      break;
    }
  }
  return pRetModel;
}

bool CIFSEntityInd::RemoveIFSVariableInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrVariables.size());
  if (nIndex >= 0 && nIndex < m_arrVariables.size())
  {
    CIFSEntityVariableInd* pModel = m_arrVariables[nIndex];
    delete pModel;
    m_arrVariables.erase(m_arrVariables.begin() + nIndex);
    return true;
  }
  return false;
}

bool CIFSEntityInd::RemoveIFSVariableInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfIFSVariableInds(); nI++)
  {
    if (sName == GetIFSVariableInd(nI)->GetName())
      return RemoveIFSVariableInd(nI);
  }
  return false;
}

bool CIFSEntityInd::RemoveAllIFSVariableInds()
{
  while (m_arrVariables.size())
    RemoveIFSVariableInd(0);
  m_arrVariables.erase(m_arrVariables.begin(), m_arrVariables.end());
  return true;
}

CIFSEntityAssignmentInd* CIFSEntityInd::GetIFSAssignmentInd(size_t nIndex)
{
  assert(nIndex < m_arrAssignments.size());
  if (nIndex < m_arrAssignments.size())
    return m_arrAssignments[nIndex];
  return 0;
}

CIFSEntityAssignmentInd* CIFSEntityInd::GetIFSAssignmentInd(const xtstring& sName)
{
  CIFSEntityAssignmentInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfIFSAssignmentInds(); nI++)
  {
    if (sName == m_arrAssignments[nI]->GetName())
    {
      pRetModel = m_arrAssignments[nI];
      break;
    }
  }
  return pRetModel;
}

xtstring CIFSEntityInd::GetIFSAssignmentIndIndexedName(const xtstring& sName)
{
  size_t nIndex = 0;
  for (size_t nI = 0; nI < CountOfIFSAssignmentInds(); nI++)
  {
    if (sName == m_arrAssignments[nI]->GetName())
    {
      nIndex = nI;
      break;
    }
  }
  xtstring sRet;
  ostrstream sVal;
  sVal << _XT("Zuweisung(") << (unsigned int)(nIndex + 1) << _XT(")");
  sRet.append(sVal.str(), sVal.pcount());
  return sRet;
}

void CIFSEntityInd::AddIFSAssignmentInd(CIFSEntityAssignmentInd* pModel)
{
  assert(pModel);
  CIFSEntityAssignmentInd* ppModel;
  ppModel = new CIFSEntityAssignmentInd(pModel);
  ppModel->SetOwnerEntity(this);
  ppModel->SetSyncGroup(GetSyncGroup());
  m_arrAssignments.push_back(ppModel);
}

bool CIFSEntityInd::RemoveIFSAssignmentInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrAssignments.size());
  if (nIndex >= 0 && nIndex < m_arrAssignments.size())
  {
    CIFSEntityAssignmentInd* pModel = m_arrAssignments[nIndex];
    delete pModel;
    m_arrAssignments.erase(m_arrAssignments.begin() + nIndex);
    return true;
  }
  return false;
}

bool CIFSEntityInd::RemoveIFSAssignmentInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfIFSAssignmentInds(); nI++)
  {
    if (sName == GetIFSAssignmentInd(nI)->GetName())
      return RemoveIFSAssignmentInd(nI);
  }
  return false;
}

bool CIFSEntityInd::RemoveAllIFSAssignmentInds()
{
  while (m_arrAssignments.size())
    RemoveIFSAssignmentInd(0);
  m_arrAssignments.erase(m_arrAssignments.begin(), m_arrAssignments.end());
  return true;
}

CIFSEntityPrePostProcessInd* CIFSEntityInd::GetPreProcessInd(size_t nIndex)
{
  assert(nIndex < m_arrPreProcesses.size());
  if (nIndex < m_arrPreProcesses.size())
    return m_arrPreProcesses[nIndex];
  return 0;
}

CIFSEntityPrePostProcessInd* CIFSEntityInd::GetPreProcessInd(const xtstring& sName)
{
  CIFSEntityPrePostProcessInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfPreProcessInds(); nI++)
  {
    if (sName == m_arrPreProcesses[nI]->GetName())
    {
      pRetModel = m_arrPreProcesses[nI];
      break;
    }
  }
  return pRetModel;
}

xtstring CIFSEntityInd::GetPreProcessIndIndexedName(const xtstring& sName)
{
  size_t nIndex = 0;
  for (size_t nI = 0; nI < CountOfPreProcessInds(); nI++)
  {
    if (sName == m_arrPreProcesses[nI]->GetName())
    {
      nIndex = nI;
      break;
    }
  }
  xtstring sRet;
  ostrstream sVal;
  sVal << _XT("PreProcess(") << (unsigned int)(nIndex + 1) << _XT(")");
  sRet.append(sVal.str(), sVal.pcount());
  return sRet;
}

void CIFSEntityInd::AddPreProcessInd(CIFSEntityPrePostProcessInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;
  CIFSEntityPrePostProcessInd* ppModel;
  ppModel = new CIFSEntityPrePostProcessInd(pModel);
  ppModel->SetOwnerEntity(this);
  ppModel->SetSyncGroup(GetSyncGroup());
  m_arrPreProcesses.push_back(ppModel);
}

bool CIFSEntityInd::RemovePreProcessInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrPreProcesses.size());
  if (nIndex >= 0 && nIndex < m_arrPreProcesses.size())
  {
    CIFSEntityPrePostProcessInd* pModel = m_arrPreProcesses[nIndex];
    delete pModel;
    m_arrPreProcesses.erase(m_arrPreProcesses.begin() + nIndex);
    return true;
  }
  return false;
}

bool CIFSEntityInd::RemovePreProcessInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfPreProcessInds(); nI++)
  {
    if (sName == GetPreProcessInd(nI)->GetName())
      return RemovePreProcessInd(nI);
  }
  return false;
}

bool CIFSEntityInd::RemoveAllPreProcessInds()
{
  while (m_arrPreProcesses.size())
    RemovePreProcessInd(0);
  m_arrPreProcesses.erase(m_arrPreProcesses.begin(), m_arrPreProcesses.end());
  return true;
}

CIFSEntityPrePostProcessInd* CIFSEntityInd::GetPostProcessInd(size_t nIndex)
{
  assert(nIndex < m_arrPostProcesses.size());
  if (nIndex < m_arrPostProcesses.size())
    return m_arrPostProcesses[nIndex];
  return 0;
}

CIFSEntityPrePostProcessInd* CIFSEntityInd::GetPostProcessInd(const xtstring& sName)
{
  CIFSEntityPrePostProcessInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfPostProcessInds(); nI++)
  {
    if (sName == m_arrPostProcesses[nI]->GetName())
    {
      pRetModel = m_arrPostProcesses[nI];
      break;
    }
  }
  return pRetModel;
}

xtstring CIFSEntityInd::GetPostProcessIndIndexedName(const xtstring& sName)
{
  size_t nIndex = 0;
  for (size_t nI = 0; nI < CountOfPostProcessInds(); nI++)
  {
    if (sName == m_arrPostProcesses[nI]->GetName())
    {
      nIndex = nI;
      break;
    }
  }
  xtstring sRet;
  ostrstream sVal;
  sVal << _XT("PostProcess(") << (unsigned int)(nIndex + 1) << _XT(")");
  sRet.append(sVal.str(), sVal.pcount());
  return sRet;
}

void CIFSEntityInd::AddPostProcessInd(CIFSEntityPrePostProcessInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;
  CIFSEntityPrePostProcessInd* ppModel;
  ppModel = new CIFSEntityPrePostProcessInd(pModel);
  ppModel->SetOwnerEntity(this);
  ppModel->SetSyncGroup(GetSyncGroup());
  m_arrPostProcesses.push_back(ppModel);
}

bool CIFSEntityInd::RemovePostProcessInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrPostProcesses.size());
  if (nIndex >= 0 && nIndex < m_arrPostProcesses.size())
  {
    CIFSEntityPrePostProcessInd* pModel = m_arrPostProcesses[nIndex];
    delete pModel;
    m_arrPostProcesses.erase(m_arrPostProcesses.begin() + nIndex);
    return true;
  }
  return false;
}

bool CIFSEntityInd::RemovePostProcessInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfPostProcessInds(); nI++)
  {
    if (sName == GetPostProcessInd(nI)->GetName())
      return RemovePostProcessInd(nI);
  }
  return false;
}

bool CIFSEntityInd::RemoveAllPostProcessInds()
{
  while (m_arrPostProcesses.size())
    RemovePostProcessInd(0);
  m_arrPostProcesses.erase(m_arrPostProcesses.begin(), m_arrPostProcesses.end());
  return true;
}

CIFSEntityGlobalERTableInd* CIFSEntityInd::GetGlobalERTableInd(size_t nIndex)
{
  assert(nIndex < m_arrGlobalERTables.size());
  if (nIndex < m_arrGlobalERTables.size())
    return m_arrGlobalERTables[nIndex];
  return 0;
}

CIFSEntityGlobalERTableInd* CIFSEntityInd::GetGlobalERTableInd(const xtstring& sName)
{
  CIFSEntityGlobalERTableInd* pRetModel = NULL;
  for (size_t nI = 0; nI < CountOfGlobalERTableInds(); nI++)
  {
    if (sName == m_arrGlobalERTables[nI]->GetName())
    {
      pRetModel = m_arrGlobalERTables[nI];
      break;
    }
  }
  return pRetModel;
}

xtstring CIFSEntityInd::GetGlobalERTableIndIndexedName(const xtstring& sName)
{
  size_t nIndex = 0;
  for (size_t nI = 0; nI < CountOfGlobalERTableInds(); nI++)
  {
    if (sName == m_arrGlobalERTables[nI]->GetName())
    {
      nIndex = nI;
      break;
    }
  }
  xtstring sRet;
  ostrstream sVal;
  sVal << _XT("GlobalTable(") << (unsigned int)(nIndex + 1) << _XT(")");
  sRet.append(sVal.str(), sVal.pcount());
  return sRet;
}

void CIFSEntityInd::AddGlobalERTableInd(CIFSEntityGlobalERTableInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;
  CIFSEntityGlobalERTableInd* ppModel;
  ppModel = new CIFSEntityGlobalERTableInd(pModel);
  ppModel->SetOwnerEntity(this);
  ppModel->SetSyncGroup(GetSyncGroup());
  m_arrGlobalERTables.push_back(ppModel);
}

bool CIFSEntityInd::RemoveGlobalERTableInd(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrGlobalERTables.size());
  if (nIndex >= 0 && nIndex < m_arrGlobalERTables.size())
  {
    CIFSEntityGlobalERTableInd* pModel = m_arrGlobalERTables[nIndex];
    delete pModel;
    m_arrGlobalERTables.erase(m_arrGlobalERTables.begin() + nIndex);
    return true;
  }
  return false;
}

bool CIFSEntityInd::RemoveGlobalERTableInd(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfGlobalERTableInds(); nI++)
  {
    if (sName == GetGlobalERTableInd(nI)->GetName())
      return RemovePostProcessInd(nI);
  }
  return false;
}

bool CIFSEntityInd::RemoveAllGlobalERTableInds()
{
  while (m_arrGlobalERTables.size())
    RemoveGlobalERTableInd(0);
  m_arrGlobalERTables.erase(m_arrGlobalERTables.begin(), m_arrGlobalERTables.end());
  return true;
}




bool CIFSEntityInd::GetAllHitStones(CxtstringVector& vecNames)
{
  for (size_t nI = 0; nI < m_arrHitNames.size(); nI++)
  {
    CIFSHitNameInd* pHitName = m_arrHitNames[nI];
    if (pHitName)
      if (pHitName->GetHitType() == tHitType_Stone)
        if (pHitName->GetHitName().GetExprText().size())
          vecNames.push_back(pHitName->GetHitName().GetExprText());
  }
  return true;
}

bool CIFSEntityInd::GetAllHitTemplates(CxtstringVector& vecNames)
{
  for (size_t nI = 0; nI < m_arrHitNames.size(); nI++)
  {
    CIFSHitNameInd* pHitName = m_arrHitNames[nI];
    if (pHitName)
      if (pHitName->GetHitType() == tHitType_Template)
        if (pHitName->GetHitName().GetExprText().size())
          vecNames.push_back(pHitName->GetHitName().GetExprText());
  }
  return true;
}







/////////////////////////////////////////////////////////////////////////////
// CRelationFieldToUse
/////////////////////////////////////////////////////////////////////////////
CRelationFieldToUse::CRelationFieldToUse()
{
  m_sParentField = _XT("");
  m_sChildField = _XT("");
  m_bUseField = false;
}
CRelationFieldToUse::~CRelationFieldToUse()
{
}
void CRelationFieldToUse::operator = (const CRelationFieldToUse& cRelFields)
{
  m_sParentField = cRelFields.m_sParentField;
  m_sChildField = cRelFields.m_sChildField;
  m_bUseField = cRelFields.m_bUseField;
}
bool CRelationFieldToUse::operator == (const CRelationFieldToUse& cRelFields)
{
  if (m_sParentField != cRelFields.m_sParentField)
    return false;
  if (m_sChildField != cRelFields.m_sChildField)
    return false;
  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CRelationToUse
/////////////////////////////////////////////////////////////////////////////
CRelationToUse::CRelationToUse()
{
}
CRelationToUse::~CRelationToUse()
{
  RemoveAllFields();
}
void CRelationToUse::operator = (const CERModelRelationInd& cRelation)
{
  RemoveAllFields();

  m_sName = cRelation.GetName();
  m_sParentTable = cRelation.GetParentTable();
  m_sChildTable = cRelation.GetChildTable();

  for (size_t nI = 0; nI < cRelation.CountOfFields(); nI++)
  {
    xtstring sParentField, sChildField;
    cRelation.GetFields(nI, sParentField, sChildField);
    CRelationFieldToUse* pRelField = new CRelationFieldToUse;
    if (pRelField)
    {
      pRelField->SetParentField(sParentField);
      pRelField->SetChildField(sChildField);
      pRelField->SetUseField(false);

      m_cFields.push_back(pRelField);
    }
  }
}
void CRelationToUse::operator = (const CRelationToUse& cRelation)
{
  RemoveAllFields();

  m_sName = cRelation.m_sName;
  m_sParentTable = cRelation.m_sParentTable;
  m_sChildTable = cRelation.m_sChildTable;

  for (CRelationFieldToUseVectorConstIterator it = cRelation.m_cFields.begin(); it != cRelation.m_cFields.end(); it++)
  {
    const CRelationFieldToUse* pOrig = *it;
    CRelationFieldToUse* pRelField = new CRelationFieldToUse;
    if (pRelField)
    {
      (*pRelField) = (*pOrig);
      m_cFields.push_back(pRelField);
    }
  }
}
void CRelationToUse::RelationToUseChanged(const CERModelRelationInd& cRelation)
{
  CRelationToUse cRel;
  cRel = cRelation;
  RelationToUseChanged(cRel);
}
void CRelationToUse::RelationToUseChanged(const CRelationToUse& cRelation)
{
  CRelationToUse cRel;
  cRel = cRelation;
  // allte Einstellungen kopieren
  for (size_t nI = 0; nI < cRel.CountOfFields(); nI++)
  {
    CRelationFieldToUse* pNew = cRel.GetFields(nI);
    if (!pNew)
      continue;
    for (size_t nJ = 0; nJ < CountOfFields(); nJ++)
    {
      CRelationFieldToUse* pOld = GetFields(nJ);
      if (pNew && pOld
          && pNew->GetChildField() == pOld->GetChildField()
          && pNew->GetParentField() == pOld->GetParentField())
      {
        pNew->SetUseField(pOld->GetUseField());
        break;
      }
    }
  }
  (*this) = cRel;
}
void CRelationToUse::TakeOverOldRelation(const CRelationToUse& cRelation)
{
  for (size_t nI = 0; nI < cRelation.CountOfFields(); nI++)
  {
    CRelationFieldToUse* pNew = cRelation.GetFields(nI);
    if (!pNew)
      continue;
    for (size_t nJ = 0; nJ < CountOfFields(); nJ++)
    {
      CRelationFieldToUse* pMy = GetFields(nJ);
      if (pNew && pMy
          && pNew->GetChildField() == pMy->GetChildField()
          && pNew->GetParentField() == pMy->GetParentField())
      {
        pMy->SetUseField(pNew->GetUseField());
        break;
      }
    }
  }
}
#define USERELATION_RELNAME       _XT("RelationName")
#define USERELATION_PARENTTABLE   _XT("ParentTable")
#define USERELATION_CHILDTABLE    _XT("ChildTable")
#define USERELATION_FIELDINREL    _XT("FieldInRelation")
#define USERELATION_PARENTFIELD   _XT("ParentField")
#define USERELATION_CHILDFIELD    _XT("ChildField")
#define USERELATION_USEFIELD      _XT("UseField")
bool CRelationToUse::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  pXMLDoc->SetAttrib(USERELATION_RELNAME, m_sName.c_str());
  pXMLDoc->SetAttrib(USERELATION_PARENTTABLE, m_sParentTable.c_str());
  pXMLDoc->SetAttrib(USERELATION_CHILDTABLE, m_sChildTable.c_str());
  for (CRelationFieldToUseVectorIterator it = m_cFields.begin(); it != m_cFields.end(); it++)
  {
    if (pXMLDoc->AddChildElem(USERELATION_FIELDINREL, _XT("")))
    {
      pXMLDoc->IntoElem();

      pXMLDoc->SetAttrib(USERELATION_PARENTFIELD, (*it)->GetParentField().c_str());
      pXMLDoc->SetAttrib(USERELATION_CHILDFIELD, (*it)->GetChildField().c_str());
      pXMLDoc->SetAttrib(USERELATION_USEFIELD, (*it)->GetUseField());

      pXMLDoc->OutOfElem();
    }
  }

  return true;
}
bool CRelationToUse::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  RemoveAllFields();

  m_sName = pXMLDoc->GetAttrib(USERELATION_RELNAME);
  m_sParentTable = pXMLDoc->GetAttrib(USERELATION_PARENTTABLE);
  m_sChildTable = pXMLDoc->GetAttrib(USERELATION_CHILDTABLE);
  while (pXMLDoc->FindChildElem(USERELATION_FIELDINREL))
  {
    pXMLDoc->IntoElem();
    CRelationFieldToUse* pNew = new CRelationFieldToUse;
    if (pNew)
    {
      pNew->SetParentField(pXMLDoc->GetAttrib(USERELATION_PARENTFIELD));
      pNew->SetChildField(pXMLDoc->GetAttrib(USERELATION_CHILDFIELD));
      pNew->SetUseField(pXMLDoc->GetAttribLong(USERELATION_USEFIELD) != 0);
      m_cFields.push_back(pNew);
    }
    pXMLDoc->OutOfElem();
  }

  return true;
}
bool CRelationToUse::IsAnyFieldToUse() const
{
  for (CRelationFieldToUseVectorConstIterator it = m_cFields.begin(); it != m_cFields.end(); it++)
    if ((*it)->GetUseField())
      return true;
  return false;
}
inline size_t CRelationToUse::CountOfFields() const
{
  return m_cFields.size();
}
bool CRelationToUse::IsFieldToUse(const xtstring& sParentField, const xtstring& sChildField)
{
  for (CRelationFieldToUseVectorIterator it = m_cFields.begin(); it != m_cFields.end(); it++)
  {
    if ((*it)->GetParentField() == sParentField && (*it)->GetChildField() == sChildField)
      return (*it)->GetUseField();
  }
  return false;
}
CRelationFieldToUse* CRelationToUse::GetFields(size_t nIndex) const
{
  if (nIndex >= CountOfFields())
    return 0;
  return m_cFields[nIndex];
}
void CRelationToUse::RemoveAllFields()
{
  for (CRelationFieldToUseVectorIterator it = m_cFields.begin(); it != m_cFields.end(); it++)
    delete (*it);
  m_cFields.erase(m_cFields.begin(), m_cFields.end());
}
/////////////////////////////////////////////////////////////////////////////
// CAllRelationsToUse
/////////////////////////////////////////////////////////////////////////////
CAllRelationsToUse::CAllRelationsToUse()
{
}
CAllRelationsToUse::~CAllRelationsToUse()
{
  RemoveAllRelationToUse();
}
void CAllRelationsToUse::operator = (const CAllRelationsToUse& cAllRelationsToUse)
{
  RemoveAllRelationToUse();

  for (size_t nI = 0; nI < cAllRelationsToUse.CountOfRelations(); nI++)
  {
    CRelationToUse* pRel = cAllRelationsToUse.GetRelationToUse(nI);
    if (!pRel)
      continue;
    CRelationToUse* pNew = new CRelationToUse;
    if (!pNew)
      continue;
    (*pNew) = (*pRel);
    m_cAllRelationToUse.push_back(pNew);
  }
}
#define ALLRELATIONSTOUSE_ALLRELATIONS  _XT("AllRelationsToUse")
#define ALLRELATIONSTOUSE_RELATIONTOUSE _XT("RelationToUse")
bool CAllRelationsToUse::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  if (pXMLDoc->AddChildElem(ALLRELATIONSTOUSE_ALLRELATIONS, _XT("")))
  {
    pXMLDoc->IntoElem();
    for (CRelationToUseVectorIterator it = m_cAllRelationToUse.begin(); it != m_cAllRelationToUse.end(); it++)
    {
      if (pXMLDoc->AddChildElem(ALLRELATIONSTOUSE_RELATIONTOUSE, _XT("")))
      {
        pXMLDoc->IntoElem();
        (*it)->SaveXMLContent(pXMLDoc);
        pXMLDoc->OutOfElem();
      }
    }
    pXMLDoc->OutOfElem();
  }
  return true;
}
bool CAllRelationsToUse::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  RemoveAllRelationToUse();

  if (pXMLDoc->FindChildElem(ALLRELATIONSTOUSE_ALLRELATIONS))
  {
    pXMLDoc->IntoElem();
    while (pXMLDoc->FindChildElem(ALLRELATIONSTOUSE_RELATIONTOUSE))
    {
      pXMLDoc->IntoElem();
      CRelationToUse* pNew = new CRelationToUse;
      if (pNew)
      {
        if (pNew->ReadXMLContent(pXMLDoc))
          m_cAllRelationToUse.push_back(pNew);
        else
          delete pNew;
      }
      pXMLDoc->OutOfElem();
    }
    pXMLDoc->OutOfElem();
  }
  return true;
}
bool CAllRelationsToUse::IsAnyRelationToUse() const
{
  for (CRelationToUseVectorConstIterator it = m_cAllRelationToUse.begin(); it != m_cAllRelationToUse.end(); it++)
    if ((*it)->IsAnyFieldToUse())
      return true;
  return false;
}
inline size_t CAllRelationsToUse::CountOfRelations() const
{
  return m_cAllRelationToUse.size();
}
CRelationToUse* CAllRelationsToUse::GetRelationToUse(size_t nIndex) const
{
  if (nIndex >= CountOfRelations())
    return 0;
  return m_cAllRelationToUse[nIndex];
}
CRelationToUse* CAllRelationsToUse::GetRelationToUse(const xtstring& sRelationName) const
{
  for (CRelationToUseVectorConstIterator it = m_cAllRelationToUse.begin(); it != m_cAllRelationToUse.end(); it++)
  {
    if ((*it)->GetName() == sRelationName)
      return (*it);
  }
  return 0;
}
void CAllRelationsToUse::AddRelationToUse(const CERModelRelationInd& cRelation)
{
  CRelationToUse* pNew = new CRelationToUse;
  if (!pNew)
    return;
  (*pNew) = cRelation;
  m_cAllRelationToUse.push_back(pNew);
}
void CAllRelationsToUse::AddRelationToUse(const CRelationToUse& cRelation)
{
  CRelationToUse* pNew = new CRelationToUse;
  if (!pNew)
    return;
  (*pNew) = cRelation;
  m_cAllRelationToUse.push_back(pNew);
}
void CAllRelationsToUse::RelationToUseChanged(const CERModelRelationInd& cRelation)
{
  CRelationToUse* pRelationToUse = GetRelationToUse(cRelation.GetName());
  if (pRelationToUse)
    pRelationToUse->RelationToUseChanged(cRelation);
}
void CAllRelationsToUse::RelationToUseChanged(const CRelationToUse& cRelation)
{
  CRelationToUse* pRelationToUse = GetRelationToUse(cRelation.GetName());
  if (pRelationToUse)
    pRelationToUse->RelationToUseChanged(cRelation);
}
void CAllRelationsToUse::TakeOverOldRelation(const CRelationToUse& cRelation)
{
  CRelationToUse* pRelationToUse = GetRelationToUse(cRelation.GetName());
  if (pRelationToUse)
    pRelationToUse->TakeOverOldRelation(cRelation);
}
void CAllRelationsToUse::RemoveRelationToUse(const xtstring& sRelationName)
{
  for (CRelationToUseVectorIterator it = m_cAllRelationToUse.begin(); it != m_cAllRelationToUse.end(); it++)
  {
    if ((*it)->GetName() == sRelationName)
    {
      delete (*it);
      m_cAllRelationToUse.erase(it);
      break;
    }
  }
}
void CAllRelationsToUse::RemoveAllRelationToUse()
{
  for (CRelationToUseVectorIterator it = m_cAllRelationToUse.begin(); it != m_cAllRelationToUse.end(); it++)
    delete (*it);
  m_cAllRelationToUse.erase(m_cAllRelationToUse.begin(), m_cAllRelationToUse.end());
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSEntityConnectionInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSEntityConnectionInd::CIFSEntityConnectionInd(CSyncIFSNotifiersGroup* pGroup)
                        :CModelBaseInd(tIFSEntityConnection),
                         CSyncNotifier_IFS(tIFSNotifier_Connection, pGroup)
{
  m_nType = tConnection_1;
  m_sName = _XT("Connection");
  m_sComment.erase();
  m_sParentEntity = _XT("ParentEntity");
  m_sChildEntity = _XT("ChildEntity");
  m_nParentType = tPCHTypeBottomTrue;
  m_nChildType = tPCHTypeTop;
  m_cRelationsToUse.RemoveAllRelationToUse();
}

CIFSEntityConnectionInd::CIFSEntityConnectionInd(CIFSEntityConnectionInd& cConnection)
                        :CModelBaseInd(tIFSEntityConnection),
                         CSyncNotifier_IFS(tIFSNotifier_Connection, NULL)
{
  SetSyncGroup(cConnection.GetSyncGroup());
  SetIFSEntityConnectionInd(&cConnection);
}

CIFSEntityConnectionInd::CIFSEntityConnectionInd(CIFSEntityConnectionInd* pConnection)
                        :CModelBaseInd(tIFSEntityConnection),
                         CSyncNotifier_IFS(tIFSNotifier_Connection, NULL)
{
  assert(pConnection);
  SetSyncGroup(pConnection->GetSyncGroup());
  SetIFSEntityConnectionInd(pConnection);
}

CIFSEntityConnectionInd::~CIFSEntityConnectionInd()
{
}

void CIFSEntityConnectionInd::operator =(CIFSEntityConnectionInd& cConnection)
{
  SetIFSEntityConnectionInd(&cConnection);
}

bool CIFSEntityConnectionInd::operator ==(CIFSEntityConnectionInd& cConnection)
{
  if (m_nType != cConnection.m_nType)
    return false;
  if (m_sName != cConnection.m_sName)
    return false;
  if (m_sComment != cConnection.m_sComment)
    return false;
  if (m_sParentEntity != cConnection.m_sParentEntity)
    return false;
  if (m_sChildEntity != cConnection.m_sChildEntity)
    return false;

  return true;
}

void CIFSEntityConnectionInd::SetIFSEntityConnectionInd(CIFSEntityConnectionInd* pConnection)
{
  if (!pConnection)
    return;

  m_nType = pConnection->m_nType;
  m_sName = pConnection->m_sName;
  m_sComment = pConnection->m_sComment;
  m_sParentEntity = pConnection->m_sParentEntity;
  m_sChildEntity = pConnection->m_sChildEntity;
  m_nParentType = pConnection->m_nParentType;
  m_nChildType = pConnection->m_nChildType;
  m_cRelationsToUse = pConnection->m_cRelationsToUse;
}

bool CIFSEntityConnectionInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(CONNECTION_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Field>
    xtstring sText;
    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_NAME,              GetName().c_str());
    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_COMMENT,           GetComment().c_str());
    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_TYPE,              GetType());
    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_PARENTTABLE,       GetParentEntity().c_str());
    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_CHILDTABLE,        GetChildEntity().c_str());
    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_PARENTTYPE,        GetParentType());
    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_CHILDTYPE,         GetChildType());
    m_cRelationsToUse.SaveXMLContent(pXMLDoc);

// MIK REL    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_USERELATIONS,       GetUseRelationBetweenERTables());
// MIK REL    xtstring sRelationsToUse;
// MIK REL    for (size_t nI = 0; nI < m_sRelationsToUse.size(); nI++)
// MIK REL    {
// MIK REL      if (sRelationsToUse.size())
// MIK REL        sRelationsToUse += _XT(";");
// MIK REL      sRelationsToUse += m_sRelationsToUse[nI];
// MIK REL    }
// MIK REL    pXMLDoc->SetAttrib(CONNECTION_ATTRIB_RELATIONSTOUSE,    sRelationsToUse.c_str());

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CIFSEntityConnectionInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Connection>
  SetName(xtstring(pXMLDoc->GetAttrib(CONNECTION_ATTRIB_NAME)));
  SetComment(xtstring(pXMLDoc->GetAttrib(CONNECTION_ATTRIB_COMMENT)));
  SetType((TConnectionType)pXMLDoc->GetAttribLong(CONNECTION_ATTRIB_TYPE));
  SetParentEntity(xtstring(pXMLDoc->GetAttrib(CONNECTION_ATTRIB_PARENTTABLE)));
  SetChildEntity(xtstring(pXMLDoc->GetAttrib(CONNECTION_ATTRIB_CHILDTABLE)));
  int nVal;
  nVal =            pXMLDoc->GetAttribLong(CONNECTION_ATTRIB_PARENTTYPE);
  if (nVal <= tPCHTypeFirst || nVal >= tPCHTypeLast)
    SetParentType(tPCHTypeBottomTrue);
  else
    SetParentType((TPCHType)nVal);
  nVal =            pXMLDoc->GetAttribLong(CONNECTION_ATTRIB_CHILDTYPE);
  if (nVal <= tPCHTypeFirst || nVal >= tPCHTypeLast)
    SetChildType(tPCHTypeTop);
  else
    SetChildType((TPCHType)nVal);
  m_cRelationsToUse.ReadXMLContent(pXMLDoc);
// MIK REL  SetUseRelationBetweenERTables(pXMLDoc->GetAttribLong(CONNECTION_ATTRIB_USERELATIONS) != 0);
// MIK REL
// MIK REL  xtstring sRelationsToUse;
// MIK REL  sRelationsToUse = pXMLDoc->GetAttrib(CONNECTION_ATTRIB_RELATIONSTOUSE);
// MIK REL  RemoveAllRelationsToUse();
// MIK REL
// MIK REL  xtstring::iterator itBegin = sRelationsToUse.begin();
// MIK REL  xtstring::iterator itEnd = sRelationsToUse.begin();
// MIK REL  itEnd = find(itBegin, sRelationsToUse.end(), _XT(';'));
// MIK REL  while (itBegin != itEnd && itEnd != sRelationsToUse.end())
// MIK REL  {
// MIK REL    xtstring sRelation;
// MIK REL    sRelation.insert(sRelation.end(), itBegin, itEnd);
// MIK REL    AddRelationToUse(sRelation);
// MIK REL    sRelationsToUse.replace(itBegin, itEnd + 1, _XT(""));
// MIK REL    itEnd = find(itBegin, sRelationsToUse.end(), _XT(';'));
// MIK REL  }
// MIK REL  if (sRelationsToUse.size())
// MIK REL    AddRelationToUse(sRelationsToUse);

  bRet = true;
  return bRet;
}

void CIFSEntityConnectionInd::SE_OnEntityNameChanged(const xtstring& sOldName,
                                                     const xtstring& sNewName)
{
  if (m_sParentEntity == sOldName)
    m_sParentEntity = sNewName;
  if (m_sChildEntity == sOldName)
    m_sChildEntity = sNewName;
}



































//////////////////////////////////////////////////////////////////////////////////
// synchronization
//////////////////////////////////////////////////////////////////////////////////

void CIFSEntityInd::SE_OnSyncFontChanged(const xtstring& sFontName,
                                         int nFontHeight)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetFontName(sFontName);
  SetFontHeight(nFontHeight);
}

void CIFSEntityInd::SE_OnSyncColorChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetColor(nColor);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncColorBackChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetColorBack(nColor);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetColorHdr(nColor);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetColorHdrSel(nColor);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetColorHdrText(nColor);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetColorHdrSelText(nColor);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetColorSelItem(nColor);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncMoveableChanged(bool bVal)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetMoveable(bVal);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncSelectableChanged(bool bVal)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetSelectable(bVal);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncVisibleChanged(bool bVal)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetVisible(bVal);
  PropertiesChanged();
}

void CIFSEntityInd::SE_OnSyncSizeableChanged(bool bVal)
{
  if (!GetSynchronize()/* || GetMainEntity()*/)
    return;
  SetSizeable(bVal);
  PropertiesChanged();
}

void CIFSEntityInd::SE_GetGlobalTableNames(CxtstringVector& arrNames)
{
  if (GetMainEntity())
  {
    for (size_t nI = 0; nI < CountOfGlobalERTableInds(); nI++)
    {
      if (GetGlobalERTableInd(nI))
        arrNames.push_back(GetGlobalERTableInd(nI)->GetName());
    }
  }
}

xtstring CIFSEntityInd::SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName)
{
  if (GetGlobalERTableInd(sGlobalTableName))
    return GetGlobalERTableInd(sGlobalTableName)->GetERTableName();
  return _XT("");
}














//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CIFSHitNameInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CIFSHitNameInd::CIFSHitNameInd(CSyncIFSNotifiersGroup* pGroup)
               :CModelBaseInd(tIFSEntityHitName),
                CSyncNotifier_IFS(tIFSNotifier_HitName, pGroup),
                CSyncResponse_Proj(tRO_IFS_HitName),
                CSyncResponse_ER(tRO_IFS_HitName),
                CSyncResponse_IFS(tRO_IFS_HitName),
                CSyncResponse_WP(tRO_IFS_HitName),
                m_caHitName(CConditionOrAssignment::tCOAT_HitName_Assignment, false),
                m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, false),
                m_caTemplateFileShowName(CConditionOrAssignment::tCOAT_TemplateFileShowName_Assignment, false),
                m_caTemplateFile(CConditionOrAssignment::tCOAT_TemplateFile_Assignment, false),
                m_caTemplateFolder(CConditionOrAssignment::tCOAT_TemplateFolder_Assignment, false),
                m_caTemplateDBName(CConditionOrAssignment::tCOAT_TemplateDBName_Assignment, false)
{
  m_nHitType = tHitType_Default;
  m_caHitName.Reset();

  m_bOverloadProperties = false;
  m_bOverloadedPropertiesChanged = false;

  m_caVisibility.Reset();
  m_sInformationSight.erase();
  m_bUseInformationSight = true;
  m_sIFSTargetEntity.erase();
  m_bUseIFSTargetEntity = true;
  m_sStoneOrTemplate.erase();
  m_bUseStoneOrTemplate = true;
  m_sStoneOrTemplateRef.erase();
  m_nWorkPaperCopyType = tCopyType_Default;
  m_nWorkPaperProcessingMode = tProcessingMode_Default;
  m_sAddOnValue.clear();

  m_nTargetForTemplate = tTemplateTarget_Default;
  m_nSaveTemplateInFormat = tSaveTemplateInFormat_Default;
  m_nTemplateFileIcon = 0;
  m_caTemplateFileShowName.Reset();
  m_caTemplateFile.Reset();
  m_caTemplateFolder.Reset();
  m_sTemplateSymbolicDBName.erase();
  m_caTemplateDBName.Reset();
}

CIFSHitNameInd::CIFSHitNameInd(CIFSHitNameInd& cName)
               :CModelBaseInd(tIFSEntityHitName),
                CSyncNotifier_IFS(tIFSNotifier_HitName, NULL),
                CSyncResponse_Proj(tRO_IFS_HitName),
                CSyncResponse_ER(tRO_IFS_HitName),
                CSyncResponse_IFS(tRO_IFS_HitName),
                CSyncResponse_WP(tRO_IFS_HitName),
                m_caHitName(CConditionOrAssignment::tCOAT_HitName_Assignment, false),
                m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, false),
                m_caTemplateFileShowName(CConditionOrAssignment::tCOAT_TemplateFileShowName_Assignment, false),
                m_caTemplateFile(CConditionOrAssignment::tCOAT_TemplateFile_Assignment, false),
                m_caTemplateFolder(CConditionOrAssignment::tCOAT_TemplateFolder_Assignment, false),
                m_caTemplateDBName(CConditionOrAssignment::tCOAT_TemplateDBName_Assignment, false)
{
  SetSyncGroup(cName.GetSyncGroup());
  SetIFSHitNameInd(&cName);
}

CIFSHitNameInd::CIFSHitNameInd(CIFSHitNameInd* pName)
               :CModelBaseInd(tIFSEntityHitName),
                CSyncNotifier_IFS(tIFSNotifier_HitName, NULL),
                CSyncResponse_Proj(tRO_IFS_HitName),
                CSyncResponse_ER(tRO_IFS_HitName),
                CSyncResponse_IFS(tRO_IFS_HitName),
                CSyncResponse_WP(tRO_IFS_HitName),
                m_caHitName(CConditionOrAssignment::tCOAT_HitName_Assignment, false),
                m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, false),
                m_caTemplateFileShowName(CConditionOrAssignment::tCOAT_TemplateFileShowName_Assignment, false),
                m_caTemplateFile(CConditionOrAssignment::tCOAT_TemplateFile_Assignment, false),
                m_caTemplateFolder(CConditionOrAssignment::tCOAT_TemplateFolder_Assignment, false),
                m_caTemplateDBName(CConditionOrAssignment::tCOAT_TemplateDBName_Assignment, false)
{
  assert(pName);
  SetSyncGroup(pName->GetSyncGroup());
  SetIFSHitNameInd(pName);
}

CIFSHitNameInd::~CIFSHitNameInd()
{
}
xtstring CIFSHitNameInd::GetOwnerEntityName() const
{
  if (GetOwnerEntity())
    return GetOwnerEntity()->GetName();
  return _XT("");
}
void CIFSHitNameInd::operator =(CIFSHitNameInd& cName)
{
  SetIFSHitNameInd(&cName);
}

bool CIFSHitNameInd::operator ==(CIFSHitNameInd& cName)
{
  if (m_nHitType == cName.m_nHitType
      && m_caHitName == cName.m_caHitName)
    return true;
  else
    return false;
}

void CIFSHitNameInd::SetIFSHitNameInd(CIFSHitNameInd* pName)
{
  assert(pName);
  if (!pName)
    return;

  // Das duerfen wir nicht machen. Wenn wir zwischen Objekten einem Child Objekt kopieren moechten,
  // dann wuerden wir dem Child Objekt falschem Parent (Owner) zuweisen.
  m_pOwnerEntity = pName->m_pOwnerEntity;

  m_nHitType = pName->m_nHitType;
  m_caHitName = pName->m_caHitName;

  m_bOverloadProperties = pName->m_bOverloadProperties;
  m_bOverloadedPropertiesChanged = pName->m_bOverloadedPropertiesChanged;

  m_caVisibility = pName->m_caVisibility;
  m_sInformationSight = pName->m_sInformationSight;
  m_bUseInformationSight = pName->m_bUseInformationSight;
  m_sIFSTargetEntity = pName->m_sIFSTargetEntity;
  m_bUseIFSTargetEntity = pName->m_bUseIFSTargetEntity;
  m_sStoneOrTemplate = pName->m_sStoneOrTemplate;
  m_bUseStoneOrTemplate = pName->m_bUseStoneOrTemplate;
  m_sStoneOrTemplateRef = pName->m_sStoneOrTemplateRef;
  m_nWorkPaperCopyType = pName->m_nWorkPaperCopyType;
  m_nWorkPaperProcessingMode = pName->m_nWorkPaperProcessingMode;
  m_sAddOnValue = pName->m_sAddOnValue;

  m_nTargetForTemplate = pName->m_nTargetForTemplate;
  m_nSaveTemplateInFormat = pName->m_nSaveTemplateInFormat;
  m_nTemplateFileIcon = pName->m_nTemplateFileIcon;
  m_caTemplateFileShowName = pName->m_caTemplateFileShowName;
  m_caTemplateFile = pName->m_caTemplateFile;
  m_caTemplateFolder = pName->m_caTemplateFolder;
  m_sTemplateSymbolicDBName = pName->m_sTemplateSymbolicDBName;
  m_caTemplateDBName = pName->m_caTemplateDBName;
}

bool CIFSHitNameInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(HITNAME_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <HitName>
    pXMLDoc->SetAttrib(NODE_ATTRIB_HIT_TYPE, GetHitType());
    m_caHitName.SaveXMLContent(pXMLDoc);

    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_OVERLOADPROPERTIES, (long)GetOverloadProperties());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_OVERLOADPROPERTIESCHANGED, (long)GetOverloadPropertiesChanged());

    m_caVisibility.SaveXMLContent(pXMLDoc);
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_IFSVIEW, GetInformationSight().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_USEIFSVIEW, GetUseInformationSight());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_IFSTARGETENTITY, GetIFSTargetEntity().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_USEIFSTARGETENTITY, GetUseIFSTargetEntity());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_STONEORTEMPLATE, GetStoneOrTemplate().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_USESTONEORTEMPLATE, GetUseStoneOrTemplate());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_STONEORTEMPLATEREF, GetStoneOrTemplateRef().c_str());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_WPCOPYTYPE, GetWorkPaperCopyType());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_WPPROCESSINGMODE, GetWorkPaperProcessingMode());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_ADDONVALUE, GetAddOnValue().c_str());

    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_TARGETFORTEMPLATE, GetTargetForTemplate());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_SAVETEMPLATEINFORMAT, GetSaveTemplateInFormat());
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_TEMPLATEFILE_ICON, GetTemplateFileIcon());
    m_caTemplateFileShowName.SaveXMLContent(pXMLDoc);
    m_caTemplateFile.SaveXMLContent(pXMLDoc);
    m_caTemplateFolder.SaveXMLContent(pXMLDoc);
    pXMLDoc->SetAttrib(PROPERTY_ATTRIB_TEMPLATESYMBOLICDBNAME, GetTemplateSymbolicDBName().c_str());
    m_caTemplateDBName.SaveXMLContent(pXMLDoc);

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CIFSHitNameInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  long nVal;
  // in <HitName>
  nVal = pXMLDoc->GetAttribLong(NODE_ATTRIB_HIT_TYPE);
  if (nVal <= tHitType_FirstDummy || nVal >= tHitType_LastDummy)
    nVal = tHitType_Default;
  SetHitType((THitType)nVal);
  m_caHitName.ReadXMLContent(pXMLDoc);

  SetOverloadProperties(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_OVERLOADPROPERTIES) != 0);
  SetOverloadPropertiesChanged(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_OVERLOADPROPERTIESCHANGED) != 0);

  m_caVisibility.ReadXMLContent(pXMLDoc);
  SetInformationSight(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_IFSVIEW));
  SetUseInformationSight(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_USEIFSVIEW) != 0);
  SetIFSTargetEntity(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_IFSTARGETENTITY));
  SetUseIFSTargetEntity(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_USEIFSTARGETENTITY) != 0);
  SetStoneOrTemplate(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_STONEORTEMPLATE));
  SetUseStoneOrTemplate(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_USESTONEORTEMPLATE) != 0);
  SetStoneOrTemplateRef(xtstring(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_STONEORTEMPLATEREF)));
  TWorkPaperCopyType nType = (TWorkPaperCopyType)pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_WPCOPYTYPE);
  if (nType <= tCopyType_FirstDummy || nType >= tCopyType_LastDummy)
    nType = tCopyType_Default;
  SetWorkPaperCopyType(nType);
  TWorkPaperProcessingMode nMode = (TWorkPaperProcessingMode)pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_WPPROCESSINGMODE);
  if (nMode <= tProcessingMode_FirstDummy || nMode >= tProcessingMode_LastDummy)
    nMode = tProcessingMode_Default;
  SetWorkPaperProcessingMode(nMode);
  SetAddOnValue(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_ADDONVALUE));

  nVal = pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_TARGETFORTEMPLATE);
  if (nVal <= tTemplateTarget_FirstDummy || nVal >= tTemplateTarget_LastDummy)
    nVal = tTemplateTarget_Default;
  SetTargetForTemplate((TTemplateTarget)nVal);
  nVal = pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_SAVETEMPLATEINFORMAT);
  if (nVal <= tSaveTemplateInFormat_FirstDummy || nVal >= tSaveTemplateInFormat_LastDummy)
    nVal = tSaveTemplateInFormat_Default;
  SetSaveTemplateInFormat((TSaveTemplateInFormat)nVal);
  SetTemplateFileIcon(pXMLDoc->GetAttribLong(PROPERTY_ATTRIB_TEMPLATEFILE_ICON));
  m_caTemplateFileShowName.ReadXMLContent(pXMLDoc);
  m_caTemplateFile.ReadXMLContent(pXMLDoc);
  m_caTemplateFolder.ReadXMLContent(pXMLDoc);
  SetTemplateSymbolicDBName(pXMLDoc->GetAttrib(PROPERTY_ATTRIB_TEMPLATESYMBOLICDBNAME));
  m_caTemplateDBName.ReadXMLContent(pXMLDoc);

  bRet = true;

  return bRet;
}





