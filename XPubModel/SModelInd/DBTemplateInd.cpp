// DBTemplateInd.cpp: implementation of the CDBTemplateInd classes.
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







//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CDBTemplateTableInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CDBTemplateTableInd::CDBTemplateTableInd(CDBTemplateInd* pParent,
                                         CSyncWPNotifiersGroup* pGroup)
                    :CModelBaseInd(tDBTemplateTable),
                     CSyncNotifier_WP(tWPNotifier_DBTemplateTable, pGroup),
                     CSyncResponse_Proj(tRO_DBTemplate_Table),
                     CSyncResponse_ER(tRO_DBTemplate_Table),
                     CSyncResponse_IFS(tRO_DBTemplate_Table),
                     m_caValue(CConditionOrAssignment::tCOAT_DBTemplateValue_Assignment, true)
{
  m_bChanged = false;

  m_pParent = pParent;
  m_sTableName.erase();
  m_bCreateTable = true;
  m_caValue.Reset();
}

CDBTemplateTableInd::CDBTemplateTableInd(CDBTemplateTableInd& cTable)
                    :CModelBaseInd(tDBTemplateTable),
                     CSyncNotifier_WP(tWPNotifier_DBTemplateTable, NULL),
                     CSyncResponse_Proj(tRO_DBTemplate_Table),
                     CSyncResponse_ER(tRO_DBTemplate_Table),
                     CSyncResponse_IFS(tRO_DBTemplate_Table),
                     m_caValue(CConditionOrAssignment::tCOAT_DBTemplateValue_Assignment, true)
{
  SetSyncGroup(cTable.GetSyncGroup());
  SetDBTemplateTableInd(&cTable);
}

CDBTemplateTableInd::CDBTemplateTableInd(CDBTemplateTableInd* pTable)
                    :CModelBaseInd(tDBTemplateTable),
                     CSyncNotifier_WP(tWPNotifier_DBTemplateTable, NULL),
                     CSyncResponse_Proj(tRO_DBTemplate_Table),
                     CSyncResponse_ER(tRO_DBTemplate_Table),
                     CSyncResponse_IFS(tRO_DBTemplate_Table),
                     m_caValue(CConditionOrAssignment::tCOAT_DBTemplateValue_Assignment, true)
{
  assert(pTable);
  SetSyncGroup(pTable->GetSyncGroup());
  SetDBTemplateTableInd(pTable);
}

CDBTemplateTableInd::~CDBTemplateTableInd()
{
}

void CDBTemplateTableInd::SetDBTemplateTableInd(CDBTemplateTableInd* pTable)
{
  if (!pTable)
    return;

  m_bChanged = true;

  m_pParent = pTable->m_pParent;
  m_sTableName = pTable->m_sTableName;
  m_bCreateTable = pTable->m_bCreateTable;
  m_caValue = pTable->m_caValue;
}

bool CDBTemplateTableInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(DBTEMPLATETABLE_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Table>
    pXMLDoc->SetAttrib(DBTEMPLATETABLE_ATTRIB_TABLENAME, GetTableName().c_str());
    pXMLDoc->SetAttrib(DBTEMPLATETABLE_ATTRIB_CREATETABLE, GetCreateTable());
    m_caValue.SaveXMLContent(pXMLDoc);

    pXMLDoc->OutOfElem();
    bRet = true;
    m_bChanged = false;
  }

  return bRet;
}

bool CDBTemplateTableInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Table>
  SetTableName(pXMLDoc->GetAttrib(DBTEMPLATETABLE_ATTRIB_TABLENAME));
  SetCreateTable(pXMLDoc->GetAttribLong(DBTEMPLATETABLE_ATTRIB_CREATETABLE) != 0);
  m_caValue.ReadXMLContent(pXMLDoc);
  bRet = true;
  m_bChanged = false;

  return bRet;
}

xtstring CDBTemplateTableInd::GetNameForShow()
{
  return m_sTableName;
}






//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CDBTemplateInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CDBTemplateInd::CDBTemplateInd(CSyncWPNotifiersGroup* pGroup)
               :CModelBaseInd(tDBTemplate),
                CSyncNotifier_WP(tWPNotifier_DBTemplate, pGroup),
                CSyncResponse_ER(tRO_DBTemplate)
{
  m_pMPTemplate = 0;
  m_bChanged = false;

  m_sERModelName.erase();
  m_bDropTables = true;
  m_bDeleteDBAfterNoHit = true;
  m_bDeleteDBTablesAfterNoHit = true;
}

CDBTemplateInd::CDBTemplateInd(CDBTemplateInd& cTemplate)
               :CModelBaseInd(tDBTemplate),
                CSyncNotifier_WP(tWPNotifier_DBTemplate, NULL),
                CSyncResponse_ER(tRO_DBTemplate)
{
  m_pMPTemplate = 0;
  SetSyncGroup(cTemplate.GetSyncGroup());
  SetDBTemplateInd(&cTemplate);
}

CDBTemplateInd::CDBTemplateInd(CDBTemplateInd* pTemplate)
               :CModelBaseInd(tDBTemplate),
                CSyncNotifier_WP(tWPNotifier_DBTemplate, NULL),
                CSyncResponse_ER(tRO_DBTemplate)
{
  m_pMPTemplate = 0;
  assert(pTemplate);
  SetSyncGroup(pTemplate->GetSyncGroup());
  SetDBTemplateInd(pTemplate);
}

CDBTemplateInd::~CDBTemplateInd()
{
  RemoveAllTables();
}

void CDBTemplateInd::SetDBTemplateInd(CDBTemplateInd* pTemplate)
{
  if (!pTemplate)
    return;

  m_bChanged = false;

  m_sERModelName = pTemplate->m_sERModelName;
  m_bDropTables = pTemplate->m_bDropTables;
  m_bDeleteDBAfterNoHit = pTemplate->m_bDeleteDBAfterNoHit;
  m_bDeleteDBTablesAfterNoHit = pTemplate->m_bDeleteDBTablesAfterNoHit;
}

bool CDBTemplateInd::GetChanged()
{
  if (m_bChanged)
    return true;
  for (size_t nI = 0; nI < CountOfTables(); nI++)
    if (GetTable(nI)->GetChanged())
      return true;
  return false;
}

bool CDBTemplateInd::SaveXMLContent(xtstring& sXMLDocText)
{
  bool bRet = false;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(EMPTY_DBTEMPLATE_XML);
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_DBTEMPLATE))
  {
    if (cXMLDoc.AddChildElem(DBTEMPLATE_ELEM_NAME, _XT("")))
    {
      cXMLDoc.IntoElem();
      // in <DBTemplate>
      cXMLDoc.SetAttrib(DBTEMPLATE_ATTRIB_ERMODELNAME, GetERModelName().c_str());
      cXMLDoc.SetAttrib(DBTEMPLATE_ATTRIB_DROPTABLES, GetDropTables());
      cXMLDoc.SetAttrib(DBTEMPLATE_ATTRIB_DELETEDBAFTERNOHIT, GetDeleteDBAfterNoHit());
      cXMLDoc.SetAttrib(DBTEMPLATE_ATTRIB_DELETEDBTABLESAFTERNOHIT, GetDeleteDBTablesAfterNoHit());
      if (cXMLDoc.AddChildElem(DBTEMPLATE_TABLES_ELEM_NAME, _XT("")))
      {
        cXMLDoc.IntoElem();
        // in <Tables>
        for (size_t nI = 0; nI < CountOfTables(); nI++)
          GetTable(nI)->SaveXMLContent(&cXMLDoc);
        cXMLDoc.OutOfElem();
        // in <Node>
      }

      cXMLDoc.OutOfElem();
      bRet = true;
      m_bChanged = false;
    }
  }
  sXMLDocText = cXMLDoc.GetDoc();
  return bRet;
}

bool CDBTemplateInd::ReadXMLContent(const xtstring& sXMLDocText)
{
  bool bRet = true;

  RemoveAllTables();
  m_sERModelName.erase();
  m_bDropTables = true;
  m_bDeleteDBAfterNoHit = true;
  m_bDeleteDBTablesAfterNoHit = true;

  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(sXMLDocText.c_str());
  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_TAG_DBTEMPLATE))
  {
    if (cXMLDoc.FindChildElem(DBTEMPLATE_ELEM_NAME))
    {
      cXMLDoc.IntoElem();
      // in <DBTemplate>
      SetERModelName(cXMLDoc.GetAttrib(DBTEMPLATE_ATTRIB_ERMODELNAME));
      SetDropTables(cXMLDoc.GetAttribLong(DBTEMPLATE_ATTRIB_DROPTABLES) != 0);
      SetDeleteDBAfterNoHit(cXMLDoc.GetAttribLong(DBTEMPLATE_ATTRIB_DELETEDBAFTERNOHIT) != 0);
      SetDeleteDBTablesAfterNoHit(cXMLDoc.GetAttribLong(DBTEMPLATE_ATTRIB_DELETEDBTABLESAFTERNOHIT) != 0);
      // load tables
      if (cXMLDoc.FindChildElem(DBTEMPLATE_TABLES_ELEM_NAME))
      {
        cXMLDoc.IntoElem();
        // in <Tables>
        while (cXMLDoc.FindChildElem(DBTEMPLATETABLE_ELEM_NAME))
        {
          cXMLDoc.IntoElem();
          // in <Table>

          CDBTemplateTableInd cTable(this, GetSyncGroup());
          cTable.ReadXMLContent(&cXMLDoc);
          AddTable(&cTable);

          cXMLDoc.OutOfElem();
          // in <Tables>
        }
        cXMLDoc.OutOfElem();
        // in <Node>
      }

      cXMLDoc.OutOfElem();
      bRet = true;
      m_bChanged = false;
    }
  }
  return bRet;
}

bool CDBTemplateInd::GetCreateTable(const xtstring& sName)
{
  CDBTemplateTableInd* pTable = GetTable(sName);
  if (!pTable)
    // per default create table
    return true;
  return pTable->GetCreateTable();
}

void CDBTemplateInd::SetCreateTable(const xtstring& sName, bool bVal)
{
  CDBTemplateTableInd* pTable = GetTable(sName);
  if (pTable)
  {
    pTable->SetCreateTable(bVal);
    return;
  }
  CDBTemplateTableInd cTable(this, GetSyncGroup());
  cTable.SetTableName(sName);
  cTable.SetCreateTable(bVal);
  AddTable(&cTable);
}

CDBTemplateTableInd* CDBTemplateInd::GetTable(const xtstring& sName)
{
  CDBTemplateTableInd* pRetTable = NULL;
  for (size_t nI = 0; nI < m_arrTables.size(); nI++)
  {
    CDBTemplateTableInd* pTable = m_arrTables[nI];
    if (pTable->GetTableName() == sName)
    {
      pRetTable = pTable;
      break;
    }
  }
  return pRetTable;
}

bool CDBTemplateInd::ChangeTableName(const xtstring& sOldName,
                                     const xtstring& sNewName)
{
  for (size_t nI = 0; nI < CountOfTables(); nI++)
  {
    CDBTemplateTableInd* pTable = GetTable(nI);
    if (sOldName == pTable->GetTableName())
    {
      pTable->SetTableName(sNewName);
      return true;
    }
  }
  return false;
}

bool CDBTemplateInd::RemoveTable(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrTables.size());
  if (nIndex >= 0 && nIndex < m_arrTables.size())
  {
    CDBTemplateTableInd* pTable = m_arrTables[nIndex];
    delete pTable;
    m_arrTables.erase(m_arrTables.begin() + nIndex);
    m_bChanged = true;
    return true;
  }
  return false;
}

bool CDBTemplateInd::RemoveTable(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfTables(); nI++)
  {
    CDBTemplateTableInd* pTable = GetTable(nI);
    if (sName == pTable->GetTableName())
      return RemoveTable(nI);
  }
  return false;
}

bool CDBTemplateInd::RemoveAllTables()
{
  while (m_arrTables.size())
    RemoveTable(0);
  m_arrTables.erase(m_arrTables.begin(), m_arrTables.end());
  return true;
}

