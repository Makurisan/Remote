#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "ModelDef.h"

#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"




CConditionOrAssignment::CConditionOrAssignment(TConditionOrAssignemntType nType,
                                               bool bUsedInWorkPaper)
{
  m_nType = nType;
  m_bUsedInWorkPaper = bUsedInWorkPaper;

  Reset();
}
CConditionOrAssignment::~CConditionOrAssignment()
{
}
void CConditionOrAssignment::Reset()
{
  m_sExprText.clear();
  m_sExprValues.clear();
  m_sExprSymbols.clear();
  m_sExprOffsets.clear();
  m_sLastUsedIFS.clear();
  m_sLastUsedEntity.clear();
  m_bLastUsedUseEntityInExpression = true;
}
bool CConditionOrAssignment::IsEmpty() const
{
  if (m_sExprText.size() == 0)
    return true;
  return false;
}
void CConditionOrAssignment::CopyWithUsedInWorkPaper(const CConditionOrAssignment& cClass)
{
  operator = (cClass);
  m_bUsedInWorkPaper = cClass.m_bUsedInWorkPaper;
}
void CConditionOrAssignment::CopyWithUsedInWorkPaper(const CConditionOrAssignment* pClass)
{
  operator = (pClass);
  if (pClass)
    m_bUsedInWorkPaper = pClass->m_bUsedInWorkPaper;
}
void CConditionOrAssignment::operator = (const CConditionOrAssignment& cClass)
{
  operator = (&cClass);
}
void CConditionOrAssignment::operator = (const CConditionOrAssignment* pClass)
{
  assert(pClass);
  if (!pClass)
    return;

  m_sExprText = pClass->m_sExprText;
  m_sExprValues = pClass->m_sExprValues;
  m_sExprSymbols = pClass->m_sExprSymbols;
  m_sExprOffsets = pClass->m_sExprOffsets;
  m_sLastUsedIFS = pClass->m_sLastUsedIFS;
  m_sLastUsedEntity = pClass->m_sLastUsedEntity;
  m_bLastUsedUseEntityInExpression = pClass->m_bLastUsedUseEntityInExpression;
}
bool CConditionOrAssignment::operator == (const CConditionOrAssignment& cClass) const
{
  if (m_sExprText != cClass.m_sExprText)
    return false;
  if (m_sExprValues != cClass.m_sExprValues)
    return false;
  if (m_sExprSymbols != cClass.m_sExprSymbols)
    return false;
  if (m_sExprOffsets != cClass.m_sExprOffsets)
    return false;
  if (m_sLastUsedIFS != cClass.m_sLastUsedIFS)
    return false;
  if (m_sLastUsedEntity != cClass.m_sLastUsedEntity)
    return false;
  if (m_bLastUsedUseEntityInExpression != cClass.m_bLastUsedUseEntityInExpression)
    return false;
  return true;
}
bool CConditionOrAssignment::operator != (const CConditionOrAssignment& cClass) const
{
  if (*this == cClass)
    return false;
  return true;
}
void CConditionOrAssignment::GetXMLTags(xtstring& sXML_Text,
                                        xtstring& sXML_ExprValues,
                                        xtstring& sXML_ExprSymbols,
                                        xtstring& sXML_ExprOffsets)
{
  sXML_ExprOffsets =      _XT("");
  switch (m_nType)
  {
  case tCOAT_Main_Condition:
    sXML_Text =             NODE_ATTRIB_CONDITION;
    sXML_ExprValues =       NODE_ATTRIB_EXPRVALUES;
    sXML_ExprSymbols =      NODE_ATTRIB_EXPRSYMBOLS;
    sXML_ExprOffsets =      NODE_ATTRIB_EXPROFFSETS;
    break;
  case tCOAT_Port_Condition:
    sXML_Text =             NODE_ATTRIB_PORTCONDITION;
    sXML_ExprValues =       NODE_ATTRIB_PORTCONDITION_EXPRVALUES;
    sXML_ExprSymbols =      NODE_ATTRIB_PORTCONDITION_EXPRSYMBOLS;
    break;
  case tCOAT_ERTableRealName_Assignment:
    sXML_Text =             NODE_ATTRIB_ERTABLEREALNAME;
    sXML_ExprValues =       NODE_ATTRIB_ERTABLEREALNAMEEXPRVALUES;
    sXML_ExprSymbols =      NODE_ATTRIB_ERTABLEREALNAMEEXPRSYMBOLS;
    break;
  case tCOAT_HitName_Assignment:
    sXML_Text =             NODE_ATTRIB_HIT_NAME_TEXT;
    sXML_ExprValues =       NODE_ATTRIB_HIT_NAME_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      NODE_ATTRIB_HIT_NAME_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_Hit_Condition:
    sXML_Text =             NODE_ATTRIB_HIT_CONDITION;
    sXML_ExprValues =       NODE_ATTRIB_HIT_CONDITION_EXPRVALUES;
    sXML_ExprSymbols =      NODE_ATTRIB_HIT_CONDITION_EXPRSYMBOLS;
    break;
  case tCOAT_Tag_Assignment:
    sXML_Text =             PROPERTY_ATTRIB_TAG_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_TAG_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_TAG_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_TemplateFileShowName_Assignment:
    sXML_Text =             PROPERTY_ATTRIB_TEMPLATEFILESHOWNAME_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_TEMPLATEFILESHOWNAME_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_TEMPLATEFILESHOWNAME_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_TemplateFile_Assignment:
    sXML_Text =             PROPERTY_ATTRIB_TEMPLATEFILE_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_TEMPLATEFILE_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_TEMPLATEFILE_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_TemplateFolder_Assignment:
    sXML_Text =             PROPERTY_ATTRIB_TEMPLATEFOLDER_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_TEMPLATEFOLDER_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_TEMPLATEFOLDER_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_TemplateDBName_Assignment:
    sXML_Text =             PROPERTY_ATTRIB_TEMPLATEDBNAME_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_TEMPLATEDBNAME_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_TEMPLATEDBNAME_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_Visibility_Condition:
    sXML_Text =             PROPERTY_ATTRIB_VISIBILITY_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_VISIBILITY_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_VISIBILITY_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_Content_Assignment:
    sXML_Text =             STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT;
    sXML_ExprValues =       STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_Address_Assignment:
    sXML_Text =             STONETEMPLATE_PROPERTY_ATTRIB_ADDRESS_TEXT;
    sXML_ExprValues =       STONETEMPLATE_PROPERTY_ATTRIB_ADDRESS_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      STONETEMPLATE_PROPERTY_ATTRIB_ADDRESS_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_QuickInfo_Assignment:
    sXML_Text =             STONETEMPLATE_PROPERTY_ATTRIB_QUICKINFO_TEXT;
    sXML_ExprValues =       STONETEMPLATE_PROPERTY_ATTRIB_QUICKINFO_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      STONETEMPLATE_PROPERTY_ATTRIB_QUICKINFO_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_PicFileNameLocal_Assignment:
    sXML_Text =             STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAMELOCAL_TEXT;
    sXML_ExprValues =       STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAMELOCAL_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAMELOCAL_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_PicFileName_Assignment:
    sXML_Text =             STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAME_TEXT;
    sXML_ExprValues =       STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAME_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAME_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_Assignment_Assignment:
    sXML_Text =             PROPERTY_ATTRIB_ASSIGNMENT_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_ASSIGNMENT_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_ASSIGNMENT_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_AssignField_Assignment:
    sXML_Text =             PROPERTY_ATTRIB_ASSIGNFIELD_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_ASSIGNFIELD_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_ASSIGNFIELD_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_AssignValue_Assignment:
    sXML_Text =             PROPERTY_ATTRIB_ASSIGNVALUE_TEXT;
    sXML_ExprValues =       PROPERTY_ATTRIB_ASSIGNVALUE_TEXT_EXPRVALUES;
    sXML_ExprSymbols =      PROPERTY_ATTRIB_ASSIGNVALUE_TEXT_EXPRSYMBOLS;
    break;
  case tCOAT_ProjConstant_Assignment:
    sXML_Text =             PROJECTCONSTANT_ATTRIB_EXPRESSION;
    sXML_ExprValues =       PROJECTCONSTANT_ATTRIB_EXPRESSIONVALUES;
    sXML_ExprSymbols =      PROJECTCONSTANT_ATTRIB_EXPRESSIONSYMBOLS;
    break;
  case tCOAT_PropStoneProperty_Assignment:
    sXML_Text =             PROPSTONE_PROPERTY_EXPRESSION;
    sXML_ExprValues =       PROPSTONE_PROPERTY_EXPRESSIONVALUES;
    sXML_ExprSymbols =      PROPSTONE_PROPERTY_EXPRESSIONSYMBOLS;
    break;
  case tCOAT_DBStoneValue_Assignment:
    sXML_Text =             DBSTONEFIELD_ATTRIB_VALUE;
    sXML_ExprValues =       DBSTONEFIELD_ATTRIB_VALUEEXPRVALUES;
    sXML_ExprSymbols =      DBSTONEFIELD_ATTRIB_VALUEEXPRSYMBOLS;
    break;
  case tCOAT_DBTemplateValue_Assignment:
    sXML_Text =             DBTEMPLATETABLE_ATTRIB_VALUE;
    sXML_ExprValues =       DBTEMPLATETABLE_ATTRIB_VALUEEXPRVALUES;
    sXML_ExprSymbols =      DBTEMPLATETABLE_ATTRIB_VALUEEXPRSYMBOLS;
    break;
  case tCOAT_OneConditionVariation_Condition:
    sXML_Text =             XMLTAG_ONECONDVARIATION_EXPRESSION;
    sXML_ExprValues =       XMLTAG_ONECONDVARIATION_EXPRVALUES;
    sXML_ExprSymbols =      XMLTAG_ONECONDVARIATION_EXPRSYMBOLS;
    sXML_ExprOffsets =      XMLTAG_ONECONDVARIATION_EXPROFFSETS;
    break;
  case tCOAT_ParameterForTextForm_Assignment:
    sXML_Text =             XMLTAG_PARAMETERS_PARAMETER_EXPRTEXT;
    sXML_ExprValues =       XMLTAG_PARAMETERS_PARAMETER_EXPRVALUES;
    sXML_ExprSymbols =      XMLTAG_PARAMETERS_PARAMETER_EXPRSYMBOLS;
    sXML_ExprOffsets =      XMLTAG_PARAMETERS_PARAMETER_EXPROFFSETS;
    break;
  case tCOAT_KeyFieldValue_Assignment:
    sXML_Text =             KEYVALUE_KEYVALUE_TEXT;
    sXML_ExprValues =       KEYVALUE_KEYVALUE_TEXTEXPRVALUES;
    sXML_ExprSymbols =      KEYVALUE_KEYVALUE_TEXTEXPRSYMBOLS;
    break;
  case tCOAT_HierarchyItem_Condition:
    sXML_Text =             HIERARCHYITEM_COND_EXPR;
    sXML_ExprValues =       HIERARCHYITEM_COND_EXPRVALUES;
    sXML_ExprSymbols =      HIERARCHYITEM_COND_EXPRSYMBOLS;
    break;
  default:
    assert(false);
    break;
  }
}
#define CA_CHILD_MAIN_C                     _XT("Main_Condition")
#define CA_CHILD_PORT_C                     _XT("Port_Condition")
#define CA_CHILD_ERTABLEREALNAME_C          _XT("ERTableRealName_Condition")
#define CA_CHILD_HITNAME_A_OLD                      _XT("HitName_Condition")
#define CA_CHILD_HITNAME_A                  _XT("HitName_Assignment")
#define CA_CHILD_HIT_C                      _XT("Hit_Condition")
#define CA_CHILD_TAG_A                      _XT("Tag_Assignment")
#define CA_CHILD_TEMPLATEFILESHOWNAME_A     _XT("TemplateFileShowName_Assignment")
#define CA_CHILD_TEMPLATEFILE_A             _XT("TemplateFile_Assignment")
#define CA_CHILD_TEMPLATEFOLDER_A           _XT("TemplateFolder_Assignment")
#define CA_CHILD_TEMPLATEDBNAME_A           _XT("TemplateDBName_Assignment")
#define CA_CHILD_VISIBILITY_C               _XT("Visibility_Condition")
#define CA_CHILD_CONTENT_A                  _XT("Content_Assignment")
#define CA_CHILD_ADDRESS_A                  _XT("Address_Assignment")
#define CA_CHILD_QUICKINFO_A                _XT("QuickInfo_Assignment")
#define CA_CHILD_PYTHONFUNCTION_A           _XT("PythonFunction_Assignment")
#define CA_CHILD_PICFILENAMELOCAL_A         _XT("PicFileNameLocal_Assignment")
#define CA_CHILD_PICFILENAME_A              _XT("PicFileName_Assignment")
#define CA_CHILD_ASSIGNMENT_A               _XT("Assignment_Assignment")
#define CA_CHILD_ASSIGNFIELD_A              _XT("AssignField_Assignment")
#define CA_CHILD_ASSIGNVALUE_A              _XT("AssignValue_Assignment")
#define CA_CHILD_PROJCONSTANT_A             _XT("ProjConstant_Assignment")
#define CA_CHILD_PROPSTONEPROPERTY_A        _XT("PropStoneProperty_Assignment")
#define CA_CHILD_DBSTONEVALUE_A             _XT("DBStoneValue_Assignment")
#define CA_CHILD_DBTEMPLATEVALUE_A          _XT("DBTemplateValue_Assignment")
#define CA_CHILD_ONECONDITIONVARIATION_C    _XT("OneConditionVariation_Condition")
#define CA_CHILD_PARAMETERFORTEXTFORM_A     _XT("ParameterForTextForm_Assignment")
#define CA_CHILD_KEYFIELDVALUE_A            _XT("KeyFieldValue_Assignment")
#define CA_CHILD_HIERARCHYITEM_C            _XT("HierarchyItem_Condition")
#define CA_EXPRESSION                     _XT("Expression")
#define CA_EXPRESSIONVALUES               _XT("ExpressionValues")
#define CA_EXPRESSIONSYMBOLS              _XT("ExpressionSymbols")
#define CA_EXPRESSIONOFFSETS              _XT("ExpressionOffsets")
#define CA_LASTUSEDIFS                    _XT("LastUsedIFS")
#define CA_LASTUSEDENTITY                 _XT("LastUsedEntity")
#define CA_LASTUSEDUSEENTITYINEXPRESSION  _XT("LastUsedUseEntityInExpression")
xtstring CConditionOrAssignment::GetChildXMLTag() const
{
  xtstring sChildXMLTag;
  switch (m_nType)
  {
  case tCOAT_Main_Condition:
    sChildXMLTag = CA_CHILD_MAIN_C;
    break;
  case tCOAT_Port_Condition:
    sChildXMLTag = CA_CHILD_PORT_C;
    break;
  case tCOAT_ERTableRealName_Assignment:
    sChildXMLTag = CA_CHILD_ERTABLEREALNAME_C;
    break;
  case tCOAT_HitName_Assignment:
    sChildXMLTag = CA_CHILD_HITNAME_A;
    break;
  case tCOAT_Hit_Condition:
    sChildXMLTag = CA_CHILD_HIT_C;
    break;
  case tCOAT_Tag_Assignment:
    sChildXMLTag = CA_CHILD_TAG_A;
    break;
  case tCOAT_TemplateFileShowName_Assignment:
    sChildXMLTag = CA_CHILD_TEMPLATEFILESHOWNAME_A;
    break;
  case tCOAT_TemplateFile_Assignment:
    sChildXMLTag = CA_CHILD_TEMPLATEFILE_A;
    break;
  case tCOAT_TemplateFolder_Assignment:
    sChildXMLTag = CA_CHILD_TEMPLATEFOLDER_A;
    break;
  case tCOAT_TemplateDBName_Assignment:
    sChildXMLTag = CA_CHILD_TEMPLATEDBNAME_A;
    break;
  case tCOAT_Visibility_Condition:
    sChildXMLTag = CA_CHILD_VISIBILITY_C;
    break;
  case tCOAT_Content_Assignment:
    sChildXMLTag = CA_CHILD_CONTENT_A;
    break;
  case tCOAT_Address_Assignment:
    sChildXMLTag = CA_CHILD_ADDRESS_A;
    break;
  case tCOAT_QuickInfo_Assignment:
    sChildXMLTag = CA_CHILD_QUICKINFO_A;
    break;
  case tCOAT_PicFileNameLocal_Assignment:
    sChildXMLTag = CA_CHILD_PICFILENAMELOCAL_A;
    break;
  case tCOAT_PicFileName_Assignment:
    sChildXMLTag = CA_CHILD_PICFILENAME_A;
    break;
  case tCOAT_Assignment_Assignment:
    sChildXMLTag = CA_CHILD_ASSIGNMENT_A;
    break;
  case tCOAT_AssignField_Assignment:
    sChildXMLTag = CA_CHILD_ASSIGNFIELD_A;
    break;
  case tCOAT_AssignValue_Assignment:
    sChildXMLTag = CA_CHILD_ASSIGNVALUE_A;
    break;
  case tCOAT_ProjConstant_Assignment:
    sChildXMLTag = CA_CHILD_PROJCONSTANT_A;
    break;
  case tCOAT_PropStoneProperty_Assignment:
    sChildXMLTag = CA_CHILD_PROPSTONEPROPERTY_A;
    break;
  case tCOAT_DBStoneValue_Assignment:
    sChildXMLTag = CA_CHILD_DBSTONEVALUE_A;
    break;
  case tCOAT_DBTemplateValue_Assignment:
    sChildXMLTag = CA_CHILD_DBTEMPLATEVALUE_A;
    break;
  case tCOAT_OneConditionVariation_Condition:
    sChildXMLTag = CA_CHILD_ONECONDITIONVARIATION_C;
    break;
  case tCOAT_ParameterForTextForm_Assignment:
    sChildXMLTag = CA_CHILD_PARAMETERFORTEXTFORM_A;
    break;
  case tCOAT_KeyFieldValue_Assignment:
    sChildXMLTag = CA_CHILD_KEYFIELDVALUE_A;
    break;
  case tCOAT_HierarchyItem_Condition:
    sChildXMLTag = CA_CHILD_HIERARCHYITEM_C;
    break;
  default:
    assert(false);
    sChildXMLTag = _XT("WhatIsIt");
  }
  return sChildXMLTag;
}
bool CConditionOrAssignment::SaveXMLContent(CXPubMarkUp *pXMLDoc) const
{
  // Bei Apspeichern benutzen wir nur die neue Methode.
  // In XML ist Child erzeugt und dort sind alle Eigenschaften eingetragen.
  if (pXMLDoc->AddChildElem(GetChildXMLTag().c_str(), _XT("")))
  {
    pXMLDoc->IntoElem();

    pXMLDoc->SetAttrib(CA_EXPRESSION,                     GetExprText().c_str());
    pXMLDoc->SetAttrib(CA_EXPRESSIONVALUES,               GetExprValues().c_str());
    pXMLDoc->SetAttrib(CA_EXPRESSIONSYMBOLS,              GetExprSymbols().c_str());
    pXMLDoc->SetAttrib(CA_EXPRESSIONOFFSETS,              GetExprOffsets().c_str());
    pXMLDoc->SetAttrib(CA_LASTUSEDIFS,                    GetLastUsedIFS().c_str());
    pXMLDoc->SetAttrib(CA_LASTUSEDENTITY,                 GetLastUsedEntity().c_str());
    pXMLDoc->SetAttrib(CA_LASTUSEDUSEENTITYINEXPRESSION,  GetLastUsedUseEntityInExpression());

    pXMLDoc->OutOfElem();
  }

  return true;
}
bool CConditionOrAssignment::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  Reset();

  // Aenderung am 19.12.2004

  // In alten XML waren die Bedingungen / Zuweisungen direkt an der Stelle
  // abgelegt, wo Objekt seine Eigenschaften hat.
  // In neuem XML sind die Bedingungen / Zuweisungen in Child eingetragen.

  // ELSE IF und ELSE Block kann man nach gewisse Zeit entfernen. Nach einem Jahr?

  // Wir suchen zuerst Child Element. Falls gefunden, wir lesen die Eigenschaften dort.
  if (pXMLDoc->FindChildElem(GetChildXMLTag().c_str()))
  {
    pXMLDoc->IntoElem();

    SetExprText                     (pXMLDoc->GetAttrib(CA_EXPRESSION));
    SetExprValues                   (pXMLDoc->GetAttrib(CA_EXPRESSIONVALUES));
    SetExprSymbols                  (pXMLDoc->GetAttrib(CA_EXPRESSIONSYMBOLS));
    SetExprOffsets                  (pXMLDoc->GetAttrib(CA_EXPRESSIONOFFSETS));
    SetLastUsedIFS                  (pXMLDoc->GetAttrib(CA_LASTUSEDIFS));
    SetLastUsedEntity               (pXMLDoc->GetAttrib(CA_LASTUSEDENTITY));
    SetLastUsedUseEntityInExpression(pXMLDoc->GetAttribLong(CA_LASTUSEDUSEENTITYINEXPRESSION) != 0);

    pXMLDoc->OutOfElem();
  }
  // Bei Uebergang von altem System nach neuen System habe ein Fehler gemacht.
  // Bei Treffer habe ich statt "HitName_Assignment" anderem Text definiert "HitName_Condition".
  // Das war sehr unschoen, deshalb ist jetzt folgendes Block da notwendig.
  // Wenn man dem ELSE Block entfern, man kann auch diesem ELSE IF Block entfernen.
  else if (m_nType == tCOAT_HitName_Assignment && pXMLDoc->FindChildElem(CA_CHILD_HITNAME_A_OLD))
  {
    pXMLDoc->IntoElem();

    SetExprText                       (pXMLDoc->GetAttrib(CA_EXPRESSION));
    SetExprValues                     (pXMLDoc->GetAttrib(CA_EXPRESSIONVALUES));
    SetExprSymbols                    (pXMLDoc->GetAttrib(CA_EXPRESSIONSYMBOLS));
    SetExprOffsets                    (pXMLDoc->GetAttrib(CA_EXPRESSIONOFFSETS));
    SetLastUsedIFS                    (pXMLDoc->GetAttrib(CA_LASTUSEDIFS));
    SetLastUsedEntity                 (pXMLDoc->GetAttrib(CA_LASTUSEDENTITY));
    SetLastUsedUseEntityInExpression  (pXMLDoc->GetAttribLong(CA_LASTUSEDUSEENTITYINEXPRESSION) != 0);

    pXMLDoc->OutOfElem();
  }
  // Falls wir Child Element nicht gefunden haben, es handelt sich um alte XML.
  // Wir holen die alte Namen und lesen es direkt von der Stelle.
  else
  {
    xtstring sXML_Text, sXML_ExprValues, sXML_ExprSymbols, sXML_ExprOffsets;
    GetXMLTags(sXML_Text, sXML_ExprValues, sXML_ExprSymbols, sXML_ExprOffsets);

    sXML_ExprOffsets.clear();

    // Folgende drei muessen da sein.
    SetExprText       (pXMLDoc->GetAttrib(sXML_Text.c_str()));
    SetExprValues     (pXMLDoc->GetAttrib(sXML_ExprValues.c_str()));
    SetExprSymbols    (pXMLDoc->GetAttrib(sXML_ExprSymbols.c_str()));
    // Folgendes kann da sein.
    if (sXML_ExprOffsets.size())
      SetExprOffsets    (pXMLDoc->GetAttrib(sXML_ExprOffsets.c_str()));
    // Folgendes ist nie da.
    SetLastUsedIFS                    (_XT(""));
    SetLastUsedEntity                 (_XT(""));
    SetLastUsedUseEntityInExpression  (true);
  }

  return true;
}




xtstring CGeneratedFile::GetFileName() const
{
  xtstring sRet;

  sRet = m_sFullFileName;
  size_t nPos = sRet.find_last_of(_XT("/\\"));
  if (nPos != xtstring::npos)
    sRet.erase(0, nPos + 1);

  return sRet;
}
xtstring CGeneratedFile::GetFileFolder() const
{
  xtstring sRet;

  sRet = m_sFullFileName;
  size_t nPos = sRet.find_last_of(_XT("/\\"));
  if (nPos != xtstring::npos)
    sRet.erase(nPos, sRet.size());

  return sRet;
}

CHitEntryInfo::CHitEntryInfo(const xtstring& sIFSEntityName)
{
  m_pFirstChildEntryR = NULL;
  m_pFirstChildEntryNR = NULL;
  m_pNextSiblingEntryR = NULL;
  m_pNextSiblingEntryNR = NULL;
  m_nOutMsgItem = 0;
  m_nLogMsgItem = 0;

  m_sHEI_EntityName = sIFSEntityName;
  m_sHEI_WPName.clear();
  m_sHEI_WPComment.clear();
  m_sHEI_TargetInformationSight.clear();
  m_sHEI_TargetEntityName.clear();
  m_sHEI_TargetWPName.clear();
  m_sHEI_PlaceHolderToReplace.clear();
  m_nHEI_WorkPaperCopyType = tCopyType_Default;
  m_nHEI_WorkPaperProcessingMode = tProcessingMode_Default;
  m_nHEI_WorkPaperType = tWorkPaperType_Stone;
  m_nHEI_TargetForTemplate = tTemplateTarget_Default;
  m_nHEI_SaveTemplateInFormat = tSaveTemplateInFormat_Default;
}
CHitEntryInfo::CHitEntryInfo(CHitEntryInfo* pEntry)
{
  m_pFirstChildEntryR = NULL;
  m_pFirstChildEntryNR = NULL;
  m_pNextSiblingEntryR = NULL;
  m_pNextSiblingEntryNR = NULL;
  m_nOutMsgItem = pEntry->m_nOutMsgItem;
  m_nLogMsgItem = pEntry->m_nLogMsgItem;

  m_sHEI_EntityName = pEntry->m_sHEI_EntityName;

  m_sHEI_WPName = pEntry->m_sHEI_WPName;
  m_sHEI_WPComment = pEntry->m_sHEI_WPComment;
  m_sHEI_TargetInformationSight = pEntry->m_sHEI_TargetInformationSight;
  m_sHEI_TargetEntityName = pEntry->m_sHEI_TargetEntityName;
  m_sHEI_TargetWPName = pEntry->m_sHEI_TargetWPName;
  m_sHEI_PlaceHolderToReplace = pEntry->m_sHEI_PlaceHolderToReplace;
  m_nHEI_WorkPaperCopyType = pEntry->m_nHEI_WorkPaperCopyType;
  m_nHEI_WorkPaperProcessingMode = pEntry->m_nHEI_WorkPaperProcessingMode;
  m_nHEI_WorkPaperType = pEntry->m_nHEI_WorkPaperType;
  m_nHEI_TargetForTemplate = pEntry->m_nHEI_TargetForTemplate;
  m_nHEI_SaveTemplateInFormat = pEntry->m_nHEI_SaveTemplateInFormat;

  CHitEntryInfo* pIterEntry;
  
  CHitEntryInfo* pNextEntry;
  // realised
  pIterEntry = pEntry;
  pNextEntry = pIterEntry->GetFirstChildHitEntryR();
  if (pNextEntry)
  {
    AddChildHitEntryR(pNextEntry);
    pIterEntry = pNextEntry->GetNextSiblingHitEntryR();
    while (pIterEntry)
    {
      AddChildHitEntryR(pIterEntry);
      pIterEntry = pIterEntry->GetNextSiblingHitEntryR();
    }
  }
  // not realised
  pIterEntry = pEntry;
  pNextEntry = pIterEntry->GetFirstChildHitEntryNR();
  if (pNextEntry)
  {
    AddChildHitEntryNR(pNextEntry);
    pIterEntry = pNextEntry->GetNextSiblingHitEntryNR();
    while (pIterEntry)
    {
      AddChildHitEntryNR(pIterEntry);
      pIterEntry = pIterEntry->GetNextSiblingHitEntryNR();
    }
  }
}

CHitEntryInfo::~CHitEntryInfo()
{
  DeleteAllHitEntries();
}

void CHitEntryInfo::DeleteAllHitEntries()
{
  CHitEntryInfo* pEntryR;
  CHitEntryInfo* pEntryNR;
  CHitEntryInfo* pNextEntryR = NULL;
  CHitEntryInfo* pNextEntryNR = NULL;

  pEntryR = GetFirstChildHitEntryR();
  pEntryNR = GetFirstChildHitEntryNR();
  while (pEntryR || pEntryNR)
  {
    if (pEntryR)
    {
      pNextEntryR = pEntryR->GetNextSiblingHitEntryR();
      delete pEntryR;
    }
    if (pEntryNR)
    {
      pNextEntryNR = pEntryNR->GetNextSiblingHitEntryNR();
      delete pEntryNR;
    }
    pEntryR = pNextEntryR;
    pEntryNR = pNextEntryNR;
  }
  m_pFirstChildEntryR = NULL;
  m_pFirstChildEntryNR = NULL;
  m_pNextSiblingEntryR = NULL;
  m_pNextSiblingEntryNR = NULL;
}

void CHitEntryInfo::ChangeTextForXMLLog(xtstring& sText)
{
  // so was muessen wir nicht mehr machen, weil wir in UTF8 XML schreiben
  return;
/*
  size_t nPos;
  xtstring sToken;

  sToken = _XT("ü");
  nPos = sText.find(sToken);
  while (nPos != xtstring::npos)
  {
    sText.replace(nPos, sToken.size(), _XT("_"));
    nPos = sText.find(sToken);
  }
  sToken = _XT("Ü");
  nPos = sText.find(sToken);
  while (nPos != xtstring::npos)
  {
    sText.replace(nPos, sToken.size(), _XT("_"));
    nPos = sText.find(sToken);
  }
  
  sToken = _XT("ä");
  nPos = sText.find(sToken);
  while (nPos != xtstring::npos)
  {
    sText.replace(nPos, sToken.size(), _XT("_"));
    nPos = sText.find(sToken);
  }
  sToken = _XT("Ä");
  nPos = sText.find(sToken);
  while (nPos != xtstring::npos)
  {
    sText.replace(nPos, sToken.size(), _XT("_"));
    nPos = sText.find(sToken);
  }
  
  sToken = _XT("ö");
  nPos = sText.find(sToken);
  while (nPos != xtstring::npos)
  {
    sText.replace(nPos, sToken.size(), _XT("_"));
    nPos = sText.find(sToken);
  }
  sToken = _XT("Ö");
  nPos = sText.find(sToken);
  while (nPos != xtstring::npos)
  {
    sText.replace(nPos, sToken.size(), _XT("_"));
    nPos = sText.find(sToken);
  }
*/
}

bool CHitEntryInfo::SaveToXMLDoc(CXPubMarkUp* pXMLDoc,
                                 bool bRootItem /*= false*/)
{
  if (!bRootItem)
  {
    xtstring sText;

    sText = m_sHEI_EntityName;              ChangeTextForXMLLog(sText);
    pXMLDoc->SetAttrib(LOG_HIT_ENTITY_NAME,           sText.c_str());
    sText = m_sHEI_WPName;                  ChangeTextForXMLLog(sText);
    pXMLDoc->SetAttrib(LOG_HIT_WPNAME,                sText.c_str());
    sText = m_sHEI_WPComment;               ChangeTextForXMLLog(sText);
    pXMLDoc->SetAttrib(LOG_HIT_WPCOMMENT,             sText.c_str());
    sText = m_sHEI_TargetInformationSight;  ChangeTextForXMLLog(sText);
    pXMLDoc->SetAttrib(LOG_HIT_TARGET_IFS_NAME,       sText.c_str());
    sText = m_sHEI_TargetEntityName;        ChangeTextForXMLLog(sText);
    pXMLDoc->SetAttrib(LOG_HIT_TARGET_ENTITY_NAME,    sText.c_str());
    sText = m_sHEI_TargetWPName;            ChangeTextForXMLLog(sText);
    pXMLDoc->SetAttrib(LOG_HIT_TARGET_WPNAME,         sText.c_str());
    sText = m_sHEI_PlaceHolderToReplace;    ChangeTextForXMLLog(sText);
    pXMLDoc->SetAttrib(LOG_HIT_PLACEHOLDER,           sText.c_str());
    pXMLDoc->SetAttrib(LOG_HIT_WPCOPYTYPE,            m_nHEI_WorkPaperCopyType);
    pXMLDoc->SetAttrib(LOG_HIT_WPPROCESSINGMODE,      m_nHEI_WorkPaperProcessingMode);
    pXMLDoc->SetAttrib(LOG_HIT_WPTYPE,                m_nHEI_WorkPaperType);
    pXMLDoc->SetAttrib(LOG_HIT_TEMPLATETARGET,        m_nHEI_TargetForTemplate);
    pXMLDoc->SetAttrib(LOG_HIT_SAVETEMPLATEINFORMAT,  m_nHEI_SaveTemplateInFormat);

    pXMLDoc->SetAttrib(LOG_OUT_HIT_X_POINT, m_nLogMsgItem);
  }

  CHitEntryInfo* pEntryR;
  CHitEntryInfo* pNextEntryR = NULL;

  pEntryR = GetFirstChildHitEntryR();
  while (pEntryR)
  {
    if (pXMLDoc->AddChildElem(LOG_HIT_ENTRY_REALISED, _XT("")))
    {
      pXMLDoc->IntoElem();
      pEntryR->SaveToXMLDoc(pXMLDoc);
      pXMLDoc->OutOfElem();
    }
    pNextEntryR = pEntryR->GetNextSiblingHitEntryR();
    pEntryR = pNextEntryR;
  }

  CHitEntryInfo* pEntryNR;
  CHitEntryInfo* pNextEntryNR = NULL;

  pEntryNR = GetFirstChildHitEntryNR();
  while (pEntryNR)
  {
    if (pXMLDoc->AddChildElem(LOG_HIT_ENTRY_NOTREALISED, _XT("")))
    {
      pXMLDoc->IntoElem();
      pEntryNR->SaveToXMLDoc(pXMLDoc);
      pXMLDoc->OutOfElem();
    }
    pNextEntryNR = pEntryNR->GetNextSiblingHitEntryNR();
    pEntryNR = pNextEntryNR;
  }
  return true;
}

bool CHitEntryInfo::AddChildHitEntryR(CHitEntryInfo* pEntry)
{
  if (!m_pFirstChildEntryR)
  {
    CHitEntryInfo* pNewEntry;
    pNewEntry = new CHitEntryInfo(pEntry);
    if (!pNewEntry)
      return false;
    m_pFirstChildEntryR = pNewEntry;
    return true;
  }
  return m_pFirstChildEntryR->AppendHitEntryAsLastSiblingChildEntryR(pEntry);
}

bool CHitEntryInfo::AddChildHitEntryNR(CHitEntryInfo* pEntry)
{
  if (!m_pFirstChildEntryNR)
  {
    CHitEntryInfo* pNewEntry;
    pNewEntry = new CHitEntryInfo(pEntry);
    if (!pNewEntry)
      return false;
    m_pFirstChildEntryNR = pNewEntry;
    return true;
  }
  return m_pFirstChildEntryNR->AppendHitEntryAsLastSiblingChildEntryNR(pEntry);
}

bool CHitEntryInfo::AppendHitEntryAsLastSiblingChildEntryR(CHitEntryInfo* pEntry)
{
  CHitEntryInfo* pLastButOne = this;
  CHitEntryInfo* pLast = m_pNextSiblingEntryR;
  while (pLast)
  {
    pLastButOne = pLast;
    pLast = pLast->m_pNextSiblingEntryR;
  }
  CHitEntryInfo* pNewEntry;
  pNewEntry = new CHitEntryInfo(pEntry);
  if (!pNewEntry)
    return false;
  pLastButOne->m_pNextSiblingEntryR = pNewEntry;
  return true;
/*
  if (!m_pNextSiblingEntryR)
  {
    CHitEntryInfo* pNewEntry;
    pNewEntry = new CHitEntryInfo(pEntry);
    if (!pNewEntry)
      return false;
    m_pNextSiblingEntryR = pNewEntry;
    return true;
  }
  return m_pNextSiblingEntryR->AppendHitEntryAsLastSiblingChildEntryR(pEntry);
*/
}

bool CHitEntryInfo::AppendHitEntryAsLastSiblingChildEntryNR(CHitEntryInfo* pEntry)
{
  CHitEntryInfo* pLastButOne = this;
  CHitEntryInfo* pLast = m_pNextSiblingEntryNR;
  while (pLast)
  {
    pLastButOne = pLast;
    pLast = pLast->m_pNextSiblingEntryNR;
  }
  CHitEntryInfo* pNewEntry;
  pNewEntry = new CHitEntryInfo(pEntry);
  if (!pNewEntry)
    return false;
  pLastButOne->m_pNextSiblingEntryNR = pNewEntry;
  return true;
/*
  if (!m_pNextSiblingEntryNR)
  {
    CHitEntryInfo* pNewEntry;
    pNewEntry = new CHitEntryInfo(pEntry);
    if (!pNewEntry)
      return false;
    m_pNextSiblingEntryNR = pNewEntry;
    return true;
  }
  return m_pNextSiblingEntryNR->AppendHitEntryAsLastSiblingChildEntryNR(pEntry);
*/
}
