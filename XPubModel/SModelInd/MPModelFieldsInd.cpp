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




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CVisibilityPreprocess
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CVisibilityPreprocess::CVisibilityPreprocess()
{
}
CVisibilityPreprocess::~CVisibilityPreprocess()
{
}
void CVisibilityPreprocess::SetVisibilityPreprocess(CVisibilityPreprocess* p)
{
  assert(p);
  if (!p)
    return;
  m_cValues.erase(m_cValues.begin(), m_cValues.end());
  m_cValues = p->m_cValues;
}
bool CVisibilityPreprocess::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  if (pXMLDoc->AddChildElem(_XT("VisibilityPreprocess"), _XT("")))
  {
    pXMLDoc->IntoElem();
    for (XPubVariantFieldListIterator it = m_cValues.begin(); it != m_cValues.end(); it++)
    {
      xtstring sName;
      TFieldType vt;
      xtstring sVal;
      sName = it->first;
      it->second.Serialize(vt, sVal);
      if (pXMLDoc->AddChildElem(_XT("VisibilityPreprocess_Value"), _XT("")))
      {
        pXMLDoc->IntoElem();
        pXMLDoc->SetAttrib(_XT("Name"), sName.c_str());
        pXMLDoc->SetAttrib(_XT("vt"), vt);
        pXMLDoc->SetAttrib(_XT("Val"), sVal.c_str());
        pXMLDoc->OutOfElem();
      }
    }
    pXMLDoc->OutOfElem();
  }
  return true;
}
bool CVisibilityPreprocess::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  if (pXMLDoc->FindChildElem(_XT("VisibilityPreprocess")))
  {
    pXMLDoc->IntoElem();
    while (pXMLDoc->FindChildElem(_XT("VisibilityPreprocess_Value")))
    {
      pXMLDoc->IntoElem();

      xtstring sName;
      TFieldType vt;
      xtstring sVal;

      sName = pXMLDoc->GetAttrib(_XT("Name"));
      vt = (TFieldType)pXMLDoc->GetAttribLong(_XT("vt"));
      sVal = pXMLDoc->GetAttrib(_XT("Val"));
      CXPubVariant xValue;
      xValue.Deserialize(vt, sVal);
      AddValue(sName, xValue);

      pXMLDoc->OutOfElem();
    }
    pXMLDoc->OutOfElem();
  }
  return true;
}
bool CVisibilityPreprocess::AddValue(const xtstring& sName,
                                     const CXPubVariant& cValue)
{
  m_cValues[sName] = cValue;
  return true;
}
size_t CVisibilityPreprocess::CountOfValues()
{
  return m_cValues.size();
}
xtstring CVisibilityPreprocess::GetValueName(size_t nIndex)
{
  for (XPubVariantFieldListIterator it = m_cValues.begin(); it != m_cValues.end(); it++)
  {
    if (nIndex == 0)
      return it->first;
    nIndex--;
  }
  return _XT("");
}
CXPubVariant* CVisibilityPreprocess::GetValueValue(size_t nIndex)
{
  for (XPubVariantFieldListIterator it = m_cValues.begin(); it != m_cValues.end(); it++)
  {
    if (nIndex == 0)
      return &(it->second);
    nIndex--;
  }
  return 0;
}

CXPubVariant* CVisibilityPreprocess::GetValueValue(const xtstring& sName)
{
  XPubVariantFieldListIterator it = m_cValues.find(sName);
  if (it == m_cValues.end())
    return 0;
  return &(it->second);
}





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelConstantInd
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelConstantInd::CMPModelConstantInd(CSyncWPNotifiersGroup* pGroup)
                    :CModelBaseInd(tMPModelConstant),
                     CSyncNotifier_WP(tWPNotifier_ConstantInd, pGroup)
{
  m_sName.erase();
  m_sComment.erase();
  m_nFieldIndex = 0;
}
CMPModelConstantInd::CMPModelConstantInd(CMPModelConstantInd& cModel)
                    :CModelBaseInd(tMPModelConstant),
                     CSyncNotifier_WP(tWPNotifier_ConstantInd, NULL)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelConstantInd(&cModel);
}
CMPModelConstantInd::CMPModelConstantInd(CMPModelConstantInd* pModel)
                    :CModelBaseInd(tMPModelConstant),
                     CSyncNotifier_WP(tWPNotifier_ConstantInd, NULL)
{
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelConstantInd(pModel);
}
CMPModelConstantInd::~CMPModelConstantInd()
{
}
void CMPModelConstantInd::SetMPModelConstantInd(CMPModelConstantInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;
  m_sName = pModel->m_sName;
  m_sComment = pModel->m_sComment;
  m_nFieldIndex = pModel->m_nFieldIndex;
}
bool CMPModelConstantInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(CONSTANT_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT, GetComment().c_str());
    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}
bool CMPModelConstantInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  SetName(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT));
  bRet = true;

  return bRet;
}





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelConstantInd_ForFrm
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelConstantInd_ForFrm::CMPModelConstantInd_ForFrm()
                           :CMPModelConstantInd((CSyncWPNotifiersGroup*)NULL)
{
}
CMPModelConstantInd_ForFrm::CMPModelConstantInd_ForFrm(CMPModelConstantInd_ForFrm& cModel)
                           :CMPModelConstantInd((CSyncWPNotifiersGroup*)NULL)
{
  SetMPModelConstantInd_ForFrm(&cModel);
}
CMPModelConstantInd_ForFrm::CMPModelConstantInd_ForFrm(CMPModelConstantInd_ForFrm* pModel)
                           :CMPModelConstantInd((CSyncWPNotifiersGroup*)NULL)
{
  SetMPModelConstantInd_ForFrm(pModel);
}
CMPModelConstantInd_ForFrm::~CMPModelConstantInd_ForFrm()
{
}
void CMPModelConstantInd_ForFrm::SetMPModelConstantInd_ForFrm(CMPModelConstantInd_ForFrm* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  CMPModelConstantInd::SetMPModelConstantInd(pModel);
}
void CMPModelConstantInd_ForFrm::SetMPModelConstantInd_ForFrm(CMPModelConstantInd* pModel)
{
  CMPModelConstantInd::SetMPModelConstantInd(pModel);
}
bool CMPModelConstantInd_ForFrm::SaveXMLContent(CXPubMarkUp* pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  if (pXMLDoc->AddChildElem(CONSTANT_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    bRet = CMPModelConstantInd::SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  return bRet;
}
bool CMPModelConstantInd_ForFrm::ReadXMLContent(CXPubMarkUp* pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  if (pXMLDoc->FindChildElem(CONSTANT_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    bRet = CMPModelConstantInd::ReadXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  return bRet;
}





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelKeyValueInd
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelKeyValueInd::CMPModelKeyValueInd()
                    :m_caKeyValue(CConditionOrAssignment::tCOAT_KeyFieldValue_Assignment, true)
{
  m_sKeyColumn.erase();
  m_caKeyValue.Reset();
}
CMPModelKeyValueInd::CMPModelKeyValueInd(CMPModelKeyValueInd& cModel)
                    :m_caKeyValue(CConditionOrAssignment::tCOAT_KeyFieldValue_Assignment, true)
{
  SetMPModelKeyValueInd(&cModel);
}
CMPModelKeyValueInd::CMPModelKeyValueInd(CMPModelKeyValueInd* pModel)
                    :m_caKeyValue(CConditionOrAssignment::tCOAT_KeyFieldValue_Assignment, true)
{
  SetMPModelKeyValueInd(pModel);
}
CMPModelKeyValueInd::~CMPModelKeyValueInd()
{
}
void CMPModelKeyValueInd::SetMPModelKeyValueInd(CMPModelKeyValueInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;
  m_sKeyColumn = pModel->m_sKeyColumn;
  m_caKeyValue = pModel->m_caKeyValue;
}
bool CMPModelKeyValueInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  pXMLDoc->SetAttrib(KEYVALUE_KEYCOLUMN, GetKeyColumn().c_str());
  m_caKeyValue.SaveXMLContent(pXMLDoc);
  return true;
}
bool CMPModelKeyValueInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  SetKeyColumn(pXMLDoc->GetAttrib(KEYVALUE_KEYCOLUMN));
  m_caKeyValue.ReadXMLContent(pXMLDoc);
  return true;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelKeyInd
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelKeyInd::CMPModelKeyInd(CUserOfObject::TModuleType nUserOfObject_ModuleType,
                               const xtstring& sInformationSight,
                               CSyncWPNotifiersGroup* pGroup)
               :CModelBaseInd(tMPModelKey),
                CSyncNotifier_WP(tWPNotifier_FieldKeyInd, pGroup),
                CSyncResponse_Proj(tRO_MainProperty_KeyField),
                CSyncResponse_ER(tRO_MainProperty_KeyField),
                CSyncResponse_IFS(tRO_MainProperty_KeyField),
                m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  SetInformationSight(sInformationSight);

  m_sName.erase();
  m_sComment.erase();
  m_nFieldIndex = 0;
  m_caVisibility.Reset();
  m_caTag.Reset();
  m_sTextTable.erase();
  m_sTextColumn.erase();

  m_nUserOfObject_ModuleType = nUserOfObject_ModuleType;
}
CMPModelKeyInd::CMPModelKeyInd(CMPModelKeyInd& cModel)
               :CModelBaseInd(tMPModelKey),
                CSyncNotifier_WP(tWPNotifier_FieldKeyInd, NULL),
                CSyncResponse_Proj(tRO_MainProperty_KeyField),
                CSyncResponse_ER(tRO_MainProperty_KeyField),
                CSyncResponse_IFS(tRO_MainProperty_KeyField),
                m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelKeyInd(&cModel);
}
CMPModelKeyInd::CMPModelKeyInd(CMPModelKeyInd* pModel)
               :CModelBaseInd(tMPModelKey),
                CSyncNotifier_WP(tWPNotifier_FieldKeyInd, NULL),
                CSyncResponse_Proj(tRO_MainProperty_KeyField),
                CSyncResponse_ER(tRO_MainProperty_KeyField),
                CSyncResponse_IFS(tRO_MainProperty_KeyField),
                m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  assert(pModel);
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelKeyInd(pModel);
}
CMPModelKeyInd::~CMPModelKeyInd()
{
  RemoveAllKeyValues();
}
void CMPModelKeyInd::SetMPModelKeyInd(CMPModelKeyInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  m_sInformationSight = pModel->m_sInformationSight;

  m_sName = pModel->m_sName;
  m_sComment = pModel->m_sComment;
  m_nFieldIndex = pModel->m_nFieldIndex;
  m_caVisibility = pModel->m_caVisibility;
  m_caTag = pModel->m_caTag;
  m_sTextTable = pModel->m_sTextTable;
  m_sTextColumn = pModel->m_sTextColumn;

  RemoveAllKeyValues();
  for (size_t nI = 0; nI < pModel->CountOfKeyValues(); nI++)
    AddKeyValue(pModel->GetKeyValue(nI));

  m_nUserOfObject_ModuleType = pModel->m_nUserOfObject_ModuleType;
}
bool CMPModelKeyInd::operator == (CMPModelKeyInd* pModel)
{
  if (!pModel)
    return false;

  if (m_sName != pModel->m_sName)
    return false;
  if (m_sComment != pModel->m_sComment)
    return false;
  if (m_nFieldIndex != pModel->m_nFieldIndex)
    return false;
  if (m_caVisibility != pModel->m_caVisibility)
    return false;
  if (m_caTag != pModel->m_caTag)
    return false;
  if (m_sTextTable != pModel->m_sTextTable)
    return false;
  if (m_sTextColumn != pModel->m_sTextColumn)
    return false;

  if (CountOfKeyValues() != pModel->CountOfKeyValues())
    return false;
  for (size_t nI = 0; nI < pModel->CountOfKeyValues(); nI++)
  {
    CMPModelKeyValueInd* p1 = GetKeyValue(nI);
    CMPModelKeyValueInd* p2 = pModel->GetKeyValue(nI);
    if (p1->GetKeyColumn() != p2->GetKeyColumn())
      return false;
    if (p1->GetKeyValue() != p2->GetKeyValue())
      return false;
  }
  return true;
}
bool CMPModelKeyInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(KEY_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Text>
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT, GetComment().c_str());
    m_caVisibility.SaveXMLContent(pXMLDoc);
    m_caTag.SaveXMLContent(pXMLDoc);
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_TEXTTABLE, GetTextTable().c_str());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_TEXTCOLUMN, GetTextColumn().c_str());
    if (pXMLDoc->AddChildElem(KEYVALUE_KEYVALUES, _XT("")))
    {
      pXMLDoc->IntoElem();
      for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
      {
        if (pXMLDoc->AddChildElem(KEYVALUE_KEYVALUE, _XT("")))
        {
          pXMLDoc->IntoElem();
          GetKeyValue(nI)->SaveXMLContent(pXMLDoc);
          pXMLDoc->OutOfElem();
        }
      }
      pXMLDoc->OutOfElem();
    }

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}
bool CMPModelKeyInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Text>
  SetName(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT));
  m_caVisibility.ReadXMLContent(pXMLDoc);
  m_caTag.ReadXMLContent(pXMLDoc);
  SetTextTable(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_TEXTTABLE));
  SetTextColumn(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_TEXTCOLUMN));

  RemoveAllKeyValues();
  if (pXMLDoc->FindChildElem(KEYVALUE_KEYVALUES))
  {
    pXMLDoc->IntoElem();
    while (pXMLDoc->FindChildElem(KEYVALUE_KEYVALUE))
    {
      pXMLDoc->IntoElem();
      CMPModelKeyValueInd cModel;
      cModel.ReadXMLContent(pXMLDoc);
      AddKeyValue(&cModel);
      pXMLDoc->OutOfElem();
    }
    pXMLDoc->OutOfElem();
  }

  bRet = true;

  return bRet;
}
CMPModelKeyValueInd* CMPModelKeyInd::GetKeyValue(const xtstring& sKeyColumn)
{
  for (CMPModelKeyValueIndsIterator it = m_arrKeyValues.begin(); it != m_arrKeyValues.end(); it++)
  {
    if ((*it)->GetKeyColumn() == sKeyColumn)
      return (*it);
  }
  return 0;
}
bool CMPModelKeyInd::RemoveKeyValue(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrKeyValues.size());
  if (nIndex >= 0 && nIndex < m_arrKeyValues.size())
  {
    CMPModelKeyValueInd* pModel = m_arrKeyValues[nIndex];
    delete pModel;
    m_arrKeyValues.erase(m_arrKeyValues.begin() + nIndex);
    return true;
  }
  return false;
}
bool CMPModelKeyInd::RemoveKeyValue(const xtstring& sKeyColumn)
{
  for (size_t nI = 0; nI < CountOfKeyValues(); nI++)
  {
    if (sKeyColumn == GetKeyValue(nI)->GetKeyColumn())
      return RemoveKeyValue(nI);
  }
  return false;
}
bool CMPModelKeyInd::RemoveAllKeyValues()
{
  for (CMPModelKeyValueIndsIterator it = m_arrKeyValues.begin(); it != m_arrKeyValues.end(); it++)
    delete (*it);
  m_arrKeyValues.erase(m_arrKeyValues.begin(), m_arrKeyValues.end());
  return true;
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelKeyInd_ForFrm
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelKeyInd_ForFrm::CMPModelKeyInd_ForFrm()
                      :CMPModelKeyInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
}
CMPModelKeyInd_ForFrm::CMPModelKeyInd_ForFrm(CMPModelKeyInd_ForFrm& cModel)
                      :CMPModelKeyInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
  SetMPModelKeyInd_ForFrm(&cModel);
}
CMPModelKeyInd_ForFrm::CMPModelKeyInd_ForFrm(CMPModelKeyInd_ForFrm* pModel)
                      :CMPModelKeyInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
  SetMPModelKeyInd_ForFrm(pModel);
}
CMPModelKeyInd_ForFrm::~CMPModelKeyInd_ForFrm()
{
}
void CMPModelKeyInd_ForFrm::SetMPModelKeyInd_ForFrm(CMPModelKeyInd_ForFrm* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  CMPModelKeyInd::SetMPModelKeyInd(pModel);
  SetVisibilityPreprocess(pModel->GetVisibilityPreprocess());
  m_sKeyResult = pModel->m_sKeyResult;
  m_sTagPropertyResult = pModel->m_sTagPropertyResult;
}
void CMPModelKeyInd_ForFrm::SetMPModelKeyInd_ForFrm(CMPModelKeyInd* pModel)
{
  CMPModelKeyInd::SetMPModelKeyInd(pModel);
}
bool CMPModelKeyInd_ForFrm::SaveXMLContent(CXPubMarkUp* pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  if (pXMLDoc->AddChildElem(KEY_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    pXMLDoc->SetAttrib(_XT("KeyResult"), GetKeyResult().c_str());
    pXMLDoc->SetAttrib(_XT("TagPropertyResult"), GetTagPropertyResult().c_str());
    bRet = CMPModelKeyInd::SaveXMLContent(pXMLDoc);
    bRet &= CVisibilityPreprocess::SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  return bRet;
}
bool CMPModelKeyInd_ForFrm::ReadXMLContent(CXPubMarkUp* pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  SetKeyResult(pXMLDoc->GetAttrib(_XT("KeyResult")));
  SetTagPropertyResult(pXMLDoc->GetAttrib(_XT("TagPropertyResult")));
  if (pXMLDoc->FindChildElem(KEY_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    bRet = CMPModelKeyInd::ReadXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
    bRet &= CVisibilityPreprocess::ReadXMLContent(pXMLDoc);
  }
  return bRet;
}




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelTextInd
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelTextInd::CMPModelTextInd(CUserOfObject::TModuleType nUserOfObject_ModuleType,
                                 const xtstring& sInformationSight,
                                 CSyncWPNotifiersGroup* pGroup)
                :CModelBaseInd(tMPModelText),
                 CSyncNotifier_WP(tWPNotifier_FieldTextInd, pGroup),
                 CSyncResponse_Proj(tRO_MainProperty_TextField),
                 CSyncResponse_ER(tRO_MainProperty_TextField),
                 CSyncResponse_IFS(tRO_MainProperty_TextField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                 m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true),
                 m_caContent(CConditionOrAssignment::tCOAT_Content_Assignment, true)
{
  SetInformationSight(sInformationSight);

  m_sName.erase();
  m_sComment.erase();
  m_nFieldIndex = 0;
  m_caVisibility.Reset();
  m_caTag.Reset();
  m_caContent.Reset();
  m_sContentTextMask.erase();

  m_nUserOfObject_ModuleType = nUserOfObject_ModuleType;
}
CMPModelTextInd::CMPModelTextInd(CMPModelTextInd& cModel)
                :CModelBaseInd(tMPModelText),
                 CSyncNotifier_WP(tWPNotifier_FieldTextInd, NULL),
                 CSyncResponse_Proj(tRO_MainProperty_TextField),
                 CSyncResponse_ER(tRO_MainProperty_TextField),
                 CSyncResponse_IFS(tRO_MainProperty_TextField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                 m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true),
                 m_caContent(CConditionOrAssignment::tCOAT_Content_Assignment, true)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelTextInd(&cModel);
}
CMPModelTextInd::CMPModelTextInd(CMPModelTextInd* pModel)
                :CModelBaseInd(tMPModelText),
                 CSyncNotifier_WP(tWPNotifier_FieldTextInd, NULL),
                 CSyncResponse_Proj(tRO_MainProperty_TextField),
                 CSyncResponse_ER(tRO_MainProperty_TextField),
                 CSyncResponse_IFS(tRO_MainProperty_TextField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                 m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true),
                 m_caContent(CConditionOrAssignment::tCOAT_Content_Assignment, true)
{
  assert(pModel);
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelTextInd(pModel);
}
CMPModelTextInd::~CMPModelTextInd()
{
}
void CMPModelTextInd::SetMPModelTextInd(CMPModelTextInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  m_sInformationSight = pModel->m_sInformationSight;

  m_sName = pModel->m_sName;
  m_sComment = pModel->m_sComment;
  m_nFieldIndex = pModel->m_nFieldIndex;
  m_caTag = pModel->m_caTag;
  m_caContent = pModel->m_caContent;
  m_sContentTextMask = pModel->m_sContentTextMask;

  m_nUserOfObject_ModuleType = pModel->m_nUserOfObject_ModuleType;
}
bool CMPModelTextInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(TEXT_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Text>
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT, GetComment().c_str());
    m_caVisibility.SaveXMLContent(pXMLDoc);
    m_caTag.SaveXMLContent(pXMLDoc);
    m_caContent.SaveXMLContent(pXMLDoc);
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT_MASK, GetContentTextMask().c_str());
    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}
bool CMPModelTextInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Text>
  SetName(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT));
  m_caVisibility.ReadXMLContent(pXMLDoc);
  m_caTag.ReadXMLContent(pXMLDoc);
  m_caContent.ReadXMLContent(pXMLDoc);
  SetContentTextMask(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT_MASK));
  bRet = true;

  return bRet;
}




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelTextInd_ForFrm
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelTextInd_ForFrm::CMPModelTextInd_ForFrm()
                       :CMPModelTextInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
}
CMPModelTextInd_ForFrm::CMPModelTextInd_ForFrm(CMPModelTextInd_ForFrm& cModel)
                       :CMPModelTextInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
  SetMPModelTextInd_ForFrm(&cModel);
}
CMPModelTextInd_ForFrm::CMPModelTextInd_ForFrm(CMPModelTextInd_ForFrm* pModel)
                       :CMPModelTextInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
  SetMPModelTextInd_ForFrm(pModel);
}
CMPModelTextInd_ForFrm::~CMPModelTextInd_ForFrm()
{
}
void CMPModelTextInd_ForFrm::SetMPModelTextInd_ForFrm(CMPModelTextInd_ForFrm* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  CMPModelTextInd::SetMPModelTextInd(pModel);
  SetVisibilityPreprocess(pModel->GetVisibilityPreprocess());
  m_sContentResult = pModel->m_sContentResult;
  m_sTagPropertyResult = pModel->m_sTagPropertyResult;
}
void CMPModelTextInd_ForFrm::SetMPModelTextInd_ForFrm(CMPModelTextInd* pModel)
{
  CMPModelTextInd::SetMPModelTextInd(pModel);
}
bool CMPModelTextInd_ForFrm::SaveXMLContent(CXPubMarkUp* pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  if (pXMLDoc->AddChildElem(TEXT_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    pXMLDoc->SetAttrib(_XT("ContentTextResult"), GetContentResult().c_str());
    pXMLDoc->SetAttrib(_XT("TagPropertyResult"), GetTagPropertyResult().c_str());
    bRet = CMPModelTextInd::SaveXMLContent(pXMLDoc);
    bRet &= CVisibilityPreprocess::SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  return bRet;
}
bool CMPModelTextInd_ForFrm::ReadXMLContent(CXPubMarkUp* pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  SetContentResult(pXMLDoc->GetAttrib(_XT("ContentTextResult")));
  SetTagPropertyResult(pXMLDoc->GetAttrib(_XT("TagPropertyResult")));
  if (pXMLDoc->FindChildElem(TEXT_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    bRet = CMPModelTextInd::ReadXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
    bRet &= CVisibilityPreprocess::ReadXMLContent(pXMLDoc);
  }
  return bRet;
}







/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelAreaInd
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelAreaInd::CMPModelAreaInd(CUserOfObject::TModuleType nUserOfObject_ModuleType,
                                 const xtstring& sInformationSight,
                                 CSyncWPNotifiersGroup* pGroup)
                :CModelBaseInd(tMPModelArea),
                 CSyncNotifier_WP(tWPNotifier_FieldAreaInd, pGroup),
                 CSyncResponse_Proj(tRO_MainProperty_AreaField),
                 CSyncResponse_ER(tRO_MainProperty_AreaField),
                 CSyncResponse_IFS(tRO_MainProperty_AreaField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true)
{
  SetInformationSight(sInformationSight);

  m_sName.erase();
  m_sComment.erase();
  m_nFieldIndex = 0;
  m_caVisibility.Reset();

  m_nUserOfObject_ModuleType = nUserOfObject_ModuleType;
  m_bVisibilityResult = false;
}
CMPModelAreaInd::CMPModelAreaInd(CMPModelAreaInd& cModel)
                :CModelBaseInd(tMPModelArea),
                 CSyncNotifier_WP(tWPNotifier_FieldAreaInd, NULL),
                 CSyncResponse_Proj(tRO_MainProperty_AreaField),
                 CSyncResponse_ER(tRO_MainProperty_AreaField),
                 CSyncResponse_IFS(tRO_MainProperty_AreaField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelAreaInd(&cModel);
}
CMPModelAreaInd::CMPModelAreaInd(CMPModelAreaInd* pModel)
                :CModelBaseInd(tMPModelArea),
                 CSyncNotifier_WP(tWPNotifier_FieldAreaInd, NULL),
                 CSyncResponse_Proj(tRO_MainProperty_AreaField),
                 CSyncResponse_ER(tRO_MainProperty_AreaField),
                 CSyncResponse_IFS(tRO_MainProperty_AreaField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true)
{
  assert(pModel);
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelAreaInd(pModel);
}
CMPModelAreaInd::~CMPModelAreaInd()
{
}
void CMPModelAreaInd::SetMPModelAreaInd(CMPModelAreaInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  m_sInformationSight = pModel->m_sInformationSight;

  m_sName = pModel->m_sName;
  m_sComment = pModel->m_sComment;
  m_nFieldIndex = pModel->m_nFieldIndex;
  m_caVisibility = pModel->m_caVisibility;

  m_nUserOfObject_ModuleType = pModel->m_nUserOfObject_ModuleType;
  m_bVisibilityResult = pModel->m_bVisibilityResult;
  m_cVisibilityPreprocessResult.SetVisibilityPreprocess(&pModel->GetVisibilityPreprocessResult());
}
bool CMPModelAreaInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(AREA_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Area>
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT, GetComment().c_str());
    m_caVisibility.SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}
bool CMPModelAreaInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Area>
  SetName(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT));
  m_caVisibility.ReadXMLContent(pXMLDoc);
  bRet = true;

  return bRet;
}





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelAreaInd_ForFrm
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelAreaInd_ForFrm::CMPModelAreaInd_ForFrm()
                       :CMPModelAreaInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
}
CMPModelAreaInd_ForFrm::CMPModelAreaInd_ForFrm(CMPModelAreaInd_ForFrm& cModel)
                       :CMPModelAreaInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
  SetMPModelAreaInd_ForFrm(&cModel);
}
CMPModelAreaInd_ForFrm::CMPModelAreaInd_ForFrm(CMPModelAreaInd_ForFrm* pModel)
                       :CMPModelAreaInd(CUserOfObject::tMT_AnyModule, _XT(""), NULL)
{
  SetMPModelAreaInd_ForFrm(pModel);
}
CMPModelAreaInd_ForFrm::~CMPModelAreaInd_ForFrm()
{
}
void CMPModelAreaInd_ForFrm::SetMPModelAreaInd_ForFrm(CMPModelAreaInd_ForFrm* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  CMPModelAreaInd::SetMPModelAreaInd(pModel);
  SetVisibilityPreprocess(pModel->GetVisibilityPreprocess());
}
void CMPModelAreaInd_ForFrm::SetMPModelAreaInd_ForFrm(CMPModelAreaInd* pModel)
{
  CMPModelAreaInd::SetMPModelAreaInd(pModel);
}
bool CMPModelAreaInd_ForFrm::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  if (pXMLDoc->AddChildElem(AREA_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    bRet = CMPModelAreaInd::SaveXMLContent(pXMLDoc);
    bRet &= CVisibilityPreprocess::SaveXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }
  return bRet;
}
bool CMPModelAreaInd_ForFrm::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);
  if (pXMLDoc->FindChildElem(AREA_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    bRet = CMPModelAreaInd::ReadXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
    bRet &= CVisibilityPreprocess::ReadXMLContent(pXMLDoc);
  }
  return bRet;
}









/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelLinkInd
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelLinkInd::CMPModelLinkInd(CUserOfObject::TModuleType nUserOfObject_ModuleType,
                                 const xtstring& sInformationSight,
                                 CSyncWPNotifiersGroup* pGroup)
                :CModelBaseInd(tMPModelLink),
                 CSyncNotifier_WP(tWPNotifier_FieldLinkInd, pGroup),
                 CSyncResponse_Proj(tRO_MainProperty_LinkField),
                 CSyncResponse_ER(tRO_MainProperty_LinkField),
                 CSyncResponse_IFS(tRO_MainProperty_LinkField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                 m_caAddress(CConditionOrAssignment::tCOAT_Address_Assignment, true),
                 m_caQuickInfo(CConditionOrAssignment::tCOAT_QuickInfo_Assignment, true),
                 m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  SetInformationSight(sInformationSight);

  m_sName.erase();
  m_sComment.erase();
  m_nFieldIndex = 0;
  m_caVisibility.Reset();
  m_caAddress.Reset();
  m_caQuickInfo.Reset();
  m_caTag.Reset();
  m_sTargetFrame.erase();

  m_nUserOfObject_ModuleType = nUserOfObject_ModuleType;
}
CMPModelLinkInd::CMPModelLinkInd(CMPModelLinkInd& cModel)
                :CModelBaseInd(tMPModelLink),
                 CSyncNotifier_WP(tWPNotifier_FieldLinkInd, NULL),
                 CSyncResponse_Proj(tRO_MainProperty_LinkField),
                 CSyncResponse_ER(tRO_MainProperty_LinkField),
                 CSyncResponse_IFS(tRO_MainProperty_LinkField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                 m_caAddress(CConditionOrAssignment::tCOAT_Address_Assignment, true),
                 m_caQuickInfo(CConditionOrAssignment::tCOAT_QuickInfo_Assignment, true),
                 m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelLinkInd(&cModel);
}
CMPModelLinkInd::CMPModelLinkInd(CMPModelLinkInd* pModel)
                :CModelBaseInd(tMPModelLink),
                 CSyncNotifier_WP(tWPNotifier_FieldLinkInd, NULL),
                 CSyncResponse_Proj(tRO_MainProperty_LinkField),
                 CSyncResponse_ER(tRO_MainProperty_LinkField),
                 CSyncResponse_IFS(tRO_MainProperty_LinkField),
                 m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                 m_caAddress(CConditionOrAssignment::tCOAT_Address_Assignment, true),
                 m_caQuickInfo(CConditionOrAssignment::tCOAT_QuickInfo_Assignment, true),
                 m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelLinkInd(pModel);
}
CMPModelLinkInd::~CMPModelLinkInd()
{
}
void CMPModelLinkInd::SetMPModelLinkInd(CMPModelLinkInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  m_sInformationSight = pModel->m_sInformationSight;

  m_sName = pModel->m_sName;
  m_sComment = pModel->m_sComment;
  m_nFieldIndex = pModel->m_nFieldIndex;
  m_caVisibility = pModel->m_caVisibility;
  m_caAddress = pModel->m_caAddress;
  m_caQuickInfo = pModel->m_caQuickInfo;
  m_caTag = pModel->m_caTag;
  m_sTargetFrame = pModel->m_sTargetFrame;

  m_nUserOfObject_ModuleType = pModel->m_nUserOfObject_ModuleType;
}
bool CMPModelLinkInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(LINK_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Link>
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT, GetComment().c_str());
    m_caVisibility.SaveXMLContent(pXMLDoc);
    m_caAddress.SaveXMLContent(pXMLDoc);
    m_caQuickInfo.SaveXMLContent(pXMLDoc);
    m_caTag.SaveXMLContent(pXMLDoc);
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_TARGETFRAME, GetTargetFrame().c_str());
    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}
bool CMPModelLinkInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Link>
  SetName(xtstring(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME)));
  SetComment(xtstring(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT)));
  m_caVisibility.ReadXMLContent(pXMLDoc);
  m_caAddress.ReadXMLContent(pXMLDoc);
  m_caQuickInfo.ReadXMLContent(pXMLDoc);
  m_caTag.ReadXMLContent(pXMLDoc);
  SetTargetFrame(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_TARGETFRAME));
  bRet = true;

  return bRet;
}










/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMPModelGraphicInd
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CMPModelGraphicInd::CMPModelGraphicInd(CUserOfObject::TModuleType nUserOfObject_ModuleType,
                                       const xtstring& sInformationSight,
                                       CSyncWPNotifiersGroup* pGroup)
                   :CModelBaseInd(tMPModelGraphic),
                    CSyncNotifier_WP(tWPNotifier_FieldGraphicInd, pGroup),
                    CSyncResponse_Proj(tRO_MainProperty_GraphicField),
                    CSyncResponse_ER(tRO_MainProperty_GraphicField),
                    CSyncResponse_IFS(tRO_MainProperty_GraphicField),
                    m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                    m_caPicFileNameLocal(CConditionOrAssignment::tCOAT_PicFileNameLocal_Assignment, true),
                    m_caPicFileName(CConditionOrAssignment::tCOAT_PicFileName_Assignment, true),
                    m_caQuickInfo(CConditionOrAssignment::tCOAT_QuickInfo_Assignment, true),
                    m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  SetInformationSight(sInformationSight);

  m_sName.erase();
  m_sComment.erase();
  m_nFieldIndex = 0;
  m_caVisibility.Reset();
  m_caPicFileNameLocal.Reset();
  m_caPicFileName.Reset();
  m_caQuickInfo.Reset();
  m_caTag.Reset();
  m_nPicSizeInPercent = 100;
  m_nWidth = 10;
  m_nHeight = 10;

  m_nUserOfObject_ModuleType = nUserOfObject_ModuleType;
}
CMPModelGraphicInd::CMPModelGraphicInd(CMPModelGraphicInd& cModel)
                   :CModelBaseInd(tMPModelGraphic),
                    CSyncNotifier_WP(tWPNotifier_FieldGraphicInd, NULL),
                    CSyncResponse_Proj(tRO_MainProperty_GraphicField),
                    CSyncResponse_ER(tRO_MainProperty_GraphicField),
                    CSyncResponse_IFS(tRO_MainProperty_GraphicField),
                    m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                    m_caPicFileNameLocal(CConditionOrAssignment::tCOAT_PicFileNameLocal_Assignment, true),
                    m_caPicFileName(CConditionOrAssignment::tCOAT_PicFileName_Assignment, true),
                    m_caQuickInfo(CConditionOrAssignment::tCOAT_QuickInfo_Assignment, true),
                    m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  SetSyncGroup(cModel.GetSyncGroup());
  SetMPModelGraphicInd(&cModel);
}
CMPModelGraphicInd::CMPModelGraphicInd(CMPModelGraphicInd* pModel)
                   :CModelBaseInd(tMPModelGraphic),
                    CSyncNotifier_WP(tWPNotifier_FieldGraphicInd, NULL),
                    CSyncResponse_Proj(tRO_MainProperty_GraphicField),
                    CSyncResponse_ER(tRO_MainProperty_GraphicField),
                    CSyncResponse_IFS(tRO_MainProperty_GraphicField),
                    m_caVisibility(CConditionOrAssignment::tCOAT_Visibility_Condition, true),
                    m_caPicFileNameLocal(CConditionOrAssignment::tCOAT_PicFileNameLocal_Assignment, true),
                    m_caPicFileName(CConditionOrAssignment::tCOAT_PicFileName_Assignment, true),
                    m_caQuickInfo(CConditionOrAssignment::tCOAT_QuickInfo_Assignment, true),
                    m_caTag(CConditionOrAssignment::tCOAT_Tag_Assignment, true)
{
  assert(pModel);
  SetSyncGroup(pModel->GetSyncGroup());
  SetMPModelGraphicInd(pModel);
}
CMPModelGraphicInd::~CMPModelGraphicInd()
{
}
void CMPModelGraphicInd::SetMPModelGraphicInd(CMPModelGraphicInd* pModel)
{
  assert(pModel);
  if (!pModel)
    return;

  m_sInformationSight = pModel->m_sInformationSight;

  m_sName = pModel->m_sName;
  m_sComment = pModel->m_sComment;
  m_nFieldIndex = pModel->m_nFieldIndex;
  m_caVisibility = pModel->m_caVisibility;
  m_caPicFileNameLocal = pModel->m_caPicFileNameLocal;
  m_caPicFileName = pModel->m_caPicFileName;
  m_caQuickInfo = pModel->m_caQuickInfo;
  m_caTag = pModel->m_caTag;
  m_nPicSizeInPercent = pModel->m_nPicSizeInPercent;
  m_nWidth = pModel->m_nWidth;
  m_nHeight = pModel->m_nHeight;

  m_nUserOfObject_ModuleType = pModel->m_nUserOfObject_ModuleType;
}
bool CMPModelGraphicInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(GRAPHIC_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Graphic>
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT, GetComment().c_str());
    m_caVisibility.SaveXMLContent(pXMLDoc);;
    m_caPicFileNameLocal.SaveXMLContent(pXMLDoc);;
    m_caPicFileName.SaveXMLContent(pXMLDoc);;
    m_caQuickInfo.SaveXMLContent(pXMLDoc);;
    m_caTag.SaveXMLContent(pXMLDoc);;
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_PICSIZEINPERCENT, GetPicSizeInPercent());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_PICWIDTH, GetPicWidth());
    pXMLDoc->SetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_PICHEIGHT, GetPicHeight());
    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}
bool CMPModelGraphicInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Graphic>
  SetName(xtstring(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_NAME)));
  SetComment(xtstring(pXMLDoc->GetAttrib(STONETEMPLATE_PROPERTY_ATTRIB_COMMENT)));
  m_caVisibility.ReadXMLContent(pXMLDoc);;
  m_caPicFileNameLocal.ReadXMLContent(pXMLDoc);;
  m_caPicFileName.ReadXMLContent(pXMLDoc);;
  m_caQuickInfo.ReadXMLContent(pXMLDoc);;
  m_caTag.ReadXMLContent(pXMLDoc);;
  SetPicSizeInPercent(pXMLDoc->GetAttribLong(STONETEMPLATE_PROPERTY_ATTRIB_PICSIZEINPERCENT));
  SetPicWidth(pXMLDoc->GetAttribLong(STONETEMPLATE_PROPERTY_ATTRIB_PICWIDTH));
  SetPicHeight(pXMLDoc->GetAttribLong(STONETEMPLATE_PROPERTY_ATTRIB_PICHEIGHT));
  bRet = true;

  return bRet;
}


