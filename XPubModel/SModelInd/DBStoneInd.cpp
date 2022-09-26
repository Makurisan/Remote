// DBStoneInd.cpp: implementation of the CDBStoneInd classes.
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





//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CDBStoneFieldInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CDBStoneFieldInd::CDBStoneFieldInd(CDBStoneInd* pParent,
                                   CSyncWPNotifiersGroup* pGroup)
                 :CModelBaseInd(tDBStoneField),
                  CSyncNotifier_WP(tWPNotifier_DBStoneField, pGroup),
                  CSyncResponse_Proj(tRO_DBStone_Field),
                  CSyncResponse_ER(tRO_DBStone_Field),
                  CSyncResponse_IFS(tRO_DBStone_Field),
                  m_caValue(CConditionOrAssignment::tCOAT_DBStoneValue_Assignment, true)
{
  m_bChanged = false;

  m_pParent = pParent;
  m_sFieldName.erase();
  m_bFieldIsKey = false;
  m_varValue.Clear();
  m_caValue.Reset();
}

CDBStoneFieldInd::CDBStoneFieldInd(CDBStoneFieldInd& cField)
                 :CModelBaseInd(tDBStoneField),
                  CSyncNotifier_WP(tWPNotifier_DBStoneField, NULL),
                  CSyncResponse_Proj(tRO_DBStone_Field),
                  CSyncResponse_ER(tRO_DBStone_Field),
                  CSyncResponse_IFS(tRO_DBStone_Field),
                  m_caValue(CConditionOrAssignment::tCOAT_DBStoneValue_Assignment, true)
{
  SetSyncGroup(cField.GetSyncGroup());
  SetDBStoneFieldInd(&cField);
}

CDBStoneFieldInd::CDBStoneFieldInd(CDBStoneFieldInd* pField)
                 :CModelBaseInd(tDBStoneField),
                  CSyncNotifier_WP(tWPNotifier_DBStoneField, NULL),
                  CSyncResponse_Proj(tRO_DBStone_Field),
                  CSyncResponse_ER(tRO_DBStone_Field),
                  CSyncResponse_IFS(tRO_DBStone_Field),
                  m_caValue(CConditionOrAssignment::tCOAT_DBStoneValue_Assignment, true)
{
  assert(pField);
  SetSyncGroup(pField->GetSyncGroup());
  SetDBStoneFieldInd(pField);
}

CDBStoneFieldInd::~CDBStoneFieldInd()
{
}

void CDBStoneFieldInd::SetDBStoneFieldInd(CDBStoneFieldInd* pField)
{
  if (!pField)
    return;

  m_bChanged = true;

  m_pParent = pField->m_pParent;
  m_sFieldName = pField->m_sFieldName;
  m_bFieldIsKey = pField->m_bFieldIsKey;
  m_varValue = pField->m_varValue;
  m_caValue = pField->m_caValue;
}

bool CDBStoneFieldInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(DBSTONEFIELD_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    TFieldType vt;
    xtstring sVal;
    m_varValue.Serialize(vt, sVal);

    // in <Field>
    pXMLDoc->SetAttrib(DBSTONEFIELD_ATTRIB_FIELDNAME, GetFieldName().c_str());
    pXMLDoc->SetAttrib(DBSTONEFIELD_ATTRIB_FIELDISKEY, GetFieldIsKey());
    pXMLDoc->SetAttrib(DBSTONEFIELD_ATTRIB_VARIANTTYPE, vt);
    pXMLDoc->SetAttrib(DBSTONEFIELD_ATTRIB_VARIANTVALUE, sVal.c_str());
    m_caValue.SaveXMLContent(pXMLDoc);

    pXMLDoc->OutOfElem();
    bRet = true;
    m_bChanged = false;
  }

  return bRet;
}

bool CDBStoneFieldInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Field>
  TFieldType vt;
  xtstring sVal;

  SetFieldName(pXMLDoc->GetAttrib(DBSTONEFIELD_ATTRIB_FIELDNAME));
  SetFieldIsKey(pXMLDoc->GetAttribLong(DBSTONEFIELD_ATTRIB_FIELDISKEY) != 0);
  vt = (TFieldType)pXMLDoc->GetAttribLong(DBSTONEFIELD_ATTRIB_VARIANTTYPE);
  sVal = pXMLDoc->GetAttrib(DBSTONEFIELD_ATTRIB_VARIANTVALUE);
  m_varValue.Deserialize(vt, sVal);
  m_caValue.ReadXMLContent(pXMLDoc);
  bRet = true;
  m_bChanged = false;

  return bRet;
}

xtstring CDBStoneFieldInd::GetNameForShow() const
{
  return m_sFieldName;
}





























//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CDBStoneInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CDBStoneInd::CDBStoneInd(CSyncWPNotifiersGroup* pGroup)
            :CModelBaseInd(tDBStone),
             CSyncNotifier_WP(tWPNotifier_DBStone, pGroup),
             CSyncResponse_ER(tRO_DBStone)
{
  m_pMPStone = 0;
  m_bChanged = false;

  m_sERModelName.erase();
  m_sTableName.erase();
  m_nProcessingType = tDBStonePT_Default;
}

CDBStoneInd::CDBStoneInd(CDBStoneInd& cStone)
            :CModelBaseInd(tDBStone),
             CSyncNotifier_WP(tWPNotifier_DBStone, NULL),
             CSyncResponse_ER(tRO_DBStone)
{
  m_pMPStone = 0;
  SetSyncGroup(cStone.GetSyncGroup());
  SetDBStoneInd(&cStone);
}

CDBStoneInd::CDBStoneInd(CDBStoneInd* pStone)
            :CModelBaseInd(tDBStone),
             CSyncNotifier_WP(tWPNotifier_DBStone, NULL),
             CSyncResponse_ER(tRO_DBStone)
{
  m_pMPStone = 0;
  assert(pStone);
  SetSyncGroup(pStone->GetSyncGroup());
  SetDBStoneInd(pStone);
}

CDBStoneInd::~CDBStoneInd()
{
  RemoveAllFields();
}

void CDBStoneInd::SetDBStoneInd(CDBStoneInd* pStone)
{
  if (!pStone)
    return;

  m_bChanged = true;

  m_sERModelName = pStone->m_sERModelName;
  m_sTableName = pStone->m_sTableName;
  m_nProcessingType = pStone->m_nProcessingType;
  RemoveAllFields();
  for (size_t nI = 0; nI < pStone->CountOfFields(); nI++)
  {
    AddField(pStone->GetField(nI));
  }
}

bool CDBStoneInd::GetChanged()
{
  if (m_bChanged)
    return true;
  for (size_t nI = 0; nI < CountOfFields(); nI++)
    if (GetField(nI)->GetChanged())
      return true;
  return false;
}

bool CDBStoneInd::SaveXMLContent(xtstring& sXMLDocText)
{
  bool bRet = false;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(EMPTY_DBSTONE_XML);
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_DBSTONE))
  {
    bRet = SaveXMLContent(&cXMLDoc);
  }
  sXMLDocText = cXMLDoc.GetDoc();
  return bRet;
}

bool CDBStoneInd::ReadXMLContent(const xtstring& sXMLDocText)
{
  bool bRet = true;

  RemoveAllFields();
  m_sERModelName.erase();
  m_sTableName.erase();
  m_nProcessingType = tDBStonePT_Default;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLDocText.c_str());
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_DBSTONE))
  {
    bRet = ReadXMLContent(&cXMLDoc);
  }
  return bRet;
}
bool CDBStoneInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  if (pXMLDoc->AddChildElem(DBSTONE_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <DBStone>
    pXMLDoc->SetAttrib(DBSTONE_ATTRIB_ERMODELNAME, GetERModelName().c_str());
    pXMLDoc->SetAttrib(DBSTONE_ATTRIB_TABLE, GetTableName().c_str());
    pXMLDoc->SetAttrib(DBSTONE_ATTRIB_PROCESSINGTYPE, GetProcessingType());
    if (pXMLDoc->AddChildElem(DBSTONE_FIELDS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Fields>
      for (size_t nI = 0; nI < CountOfFields(); nI++)
        GetField(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }

    pXMLDoc->OutOfElem();
    bRet = true;
    m_bChanged = false;
  }
  return bRet;
}
bool CDBStoneInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  if (pXMLDoc->FindChildElem(DBSTONE_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <DBStone>
    SetERModelName(pXMLDoc->GetAttrib(DBSTONE_ATTRIB_ERMODELNAME));
    SetTableName(pXMLDoc->GetAttrib(DBSTONE_ATTRIB_TABLE));
    TDBStoneProcessingType n = (TDBStoneProcessingType)pXMLDoc->GetAttribLong(DBSTONE_ATTRIB_PROCESSINGTYPE);
    if (n <= tDBStonePTFirst_Dummy || n >= tDBStonePTLast_Dummy)
      n = tDBStonePT_Default;
    SetProcessingType(n);
    // load fields
    if (pXMLDoc->FindChildElem(DBSTONE_FIELDS_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Fields>
      while (pXMLDoc->FindChildElem(DBSTONEFIELD_ELEM_NAME))
      {
        pXMLDoc->IntoElem();
        // in <Field>

        CDBStoneFieldInd cField(this, GetSyncGroup());
        cField.ReadXMLContent(pXMLDoc);
        AddField(&cField);

        pXMLDoc->OutOfElem();
        // in <Fields>
      }
      pXMLDoc->OutOfElem();
      // in <Node>
    }

    pXMLDoc->OutOfElem();
    bRet = true;
    m_bChanged = false;
  }
  return bRet;
}

CDBStoneFieldInd* CDBStoneInd::GetField(const xtstring& sName) const
{
  CDBStoneFieldInd* pRetField = NULL;
  for (size_t nI = 0; nI < m_arrFields.size(); nI++)
  {
    CDBStoneFieldInd* pField = m_arrFields[nI];
    if (pField->GetFieldName() == sName)
    {
      pRetField = pField;
      break;
    }
  }
  return pRetField;
}

void CDBStoneInd::AddField(CDBStoneFieldInd *pField)
{
  assert(pField);
  CDBStoneFieldInd *ppField;
  ppField = new CDBStoneFieldInd(pField);
  ppField->SetParent(this);
  ppField->SetSyncGroup(GetSyncGroup());
  m_arrFields.push_back(ppField);
  m_bChanged = true;
}

bool CDBStoneInd::RemoveField(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrFields.size());
  if (nIndex >= 0 && nIndex < m_arrFields.size())
  {
    CDBStoneFieldInd* pField = m_arrFields[nIndex];
    delete pField;
    m_arrFields.erase(m_arrFields.begin() + nIndex);
    m_bChanged = true;
    return true;
  }
  return false;
}

bool CDBStoneInd::RemoveField(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfFields(); nI++)
  {
    CDBStoneFieldInd* pField = GetField(nI);
    if (sName == pField->GetFieldName())
      return RemoveField(nI);
  }
  return false;
}

bool CDBStoneInd::RemoveAllFields()
{
  while (m_arrFields.size())
    RemoveField(0);
  m_arrFields.erase(m_arrFields.begin(), m_arrFields.end());
  m_bChanged = true;
  return true;
}














CDBStoneContainer::CDBStoneContainer()
{
}
CDBStoneContainer::~CDBStoneContainer()
{
  RemoveAllDBStones();
}
bool CDBStoneContainer::SaveXMLContent(xtstring& sXMLDocText)
{
  bool bRet = true;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(EMPTY_DBSTONES_XML);
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_DBSTONES))
  {
    for (CDBStonesIterator it = m_cStones.begin(); it != m_cStones.end(); it++)
    {
      CDBOneStone* pStone = (*it);
      if (cXMLDoc.AddChildElem(DBSTONE_CONTAINER_ELEM_NAME, _XT("")))
      {
        cXMLDoc.IntoElem();

        cXMLDoc.SetAttrib(DBSTONE_CONTAINER_STONE_NAME, pStone->sName.c_str());
        pStone->pStone->SaveXMLContent(&cXMLDoc);

        cXMLDoc.OutOfElem();
      }
    }
  }
  sXMLDocText = cXMLDoc.GetDoc();
  return bRet;
}
bool CDBStoneContainer::ReadXMLContent(const xtstring& sXMLDocText)
{
  bool bRet = true;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLDocText.c_str());
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_DBSTONES))
  {
    while (cXMLDoc.FindChildElem(DBSTONE_CONTAINER_ELEM_NAME))
    {
      cXMLDoc.IntoElem();

      CDBOneStone cStone;
      cStone.sName = cXMLDoc.GetAttrib(DBSTONE_CONTAINER_STONE_NAME);
      cStone.pStone->ReadXMLContent(&cXMLDoc);
      AddDBStone(&cStone);

      cXMLDoc.OutOfElem();
    }
  }
  return bRet;
}
size_t CDBStoneContainer::CountOfDBStones()
{
  return m_cStones.size();
}
CDBStoneContainer::CDBOneStone* CDBStoneContainer::GetDBStone(size_t nIndex)
{
  if (nIndex >= m_cStones.size())
    return 0;
  return m_cStones[nIndex];
}
bool CDBStoneContainer::AddDBStone(CDBOneStone* pDBStone)
{
  CDBOneStone* pNew = new CDBOneStone(pDBStone);
  if (!pNew)
    return false;
  size_t nCount = m_cStones.size();
  m_cStones.push_back(pNew);
  if (nCount == m_cStones.size())
    delete pNew;
  return (nCount != m_cStones.size());
}
bool CDBStoneContainer::MoveDBStoneUp(size_t nIndex)
{
  if (nIndex == 0)
    return true;
  if (nIndex >= m_cStones.size())
    return false;

  CDBStonesIterator it1 = m_cStones.begin() + nIndex;
  CDBStonesIterator it2 = m_cStones.begin() + (nIndex - 1);
  CDBOneStone* stone = *it1;
  *it1 = *it2;
  *it2 = stone;
  return true;
}
bool CDBStoneContainer::MoveDBStoneDown(size_t nIndex)
{
  if (nIndex >= m_cStones.size())
    return false;
  if (nIndex + 1 == m_cStones.size())
    return true;

  CDBStonesIterator it1 = m_cStones.begin() + nIndex;
  CDBStonesIterator it2 = m_cStones.begin() + (nIndex + 1);
  CDBOneStone* stone = *it1;
  *it1 = *it2;
  *it2 = stone;
  return true;
}
bool CDBStoneContainer::RemoveDBStone(size_t nIndex)
{
  if (nIndex < m_cStones.size())
  {
    delete (m_cStones[nIndex]);
    m_cStones.erase(m_cStones.begin() + nIndex);
    return true;
  }
  return false;
}
bool CDBStoneContainer::RemoveAllDBStones()
{
  for (CDBStonesIterator it = m_cStones.begin(); it != m_cStones.end(); it++)
    delete (*it);
  m_cStones.erase(m_cStones.begin(), m_cStones.end());
  return true;
}
