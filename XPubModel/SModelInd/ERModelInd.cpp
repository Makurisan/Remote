// ERModelInd.cpp: implementation of the CERModelXXXInd classes.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"

#include "ModelInd.h"
#include "ERModelInd.h"







//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERFreeSelectTables
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CERFreeSelectTables::CERFreeSelectTables(CSyncERNotifiersGroup* pGroup)
                    :CModelBaseInd(tERFreeSelectTables),
                     CSyncNotifier_ER(tERNotifier_TablesForFreeSelect, pGroup)
{
}
CERFreeSelectTables::~CERFreeSelectTables()
{
}
bool CERFreeSelectTables::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  char sKey[50];
  for (size_t nI = 0; nI < m_arrTables.size(); nI++)
  {
    sprintf(sKey, NODE_ELEM_TABLE_FOR_FREE_SELECT, nI);
    pXMLDoc->AddAttrib(sKey, m_arrTables[nI].c_str());
  }
  return true;
}
bool CERFreeSelectTables::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  m_arrTables.erase(m_arrTables.begin(), m_arrTables.end());

  xtstring sTable;
  char sKey[50];
  for (size_t nI = 0;;nI++)
  {
    sprintf(sKey, NODE_ELEM_TABLE_FOR_FREE_SELECT, nI);
    sTable = pXMLDoc->GetAttrib(sKey);
    if (!sTable.size())
      break;
    m_arrTables.push_back(sTable);
  }
  return true;
}
bool CERFreeSelectTables::AssignFrom(const CERFreeSelectTables* pTables)
{
  if (!pTables)
    return false;

  m_arrTables.erase(m_arrTables.begin(), m_arrTables.end());
  for (size_t nI = 0; nI < pTables->CountOfTables(); nI++)
  {
    xtstring sTable;
    sTable = pTables->GetTable(nI);
    m_arrTables.push_back(sTable);
  }
  return true;
}
bool CERFreeSelectTables::TableInList(const xtstring& sTable) const
{
  for (size_t nI = 0; nI < CountOfTables(); nI++)
  {
    if (GetTable(nI) == sTable)
      return true;
  }
  return false;
}
xtstring CERFreeSelectTables::GetTable(size_t nIndex) const
{
  xtstring sTable;
  if (nIndex < CountOfTables())
    sTable = m_arrTables[nIndex];
  return sTable;
}
void CERFreeSelectTables::AddTable(const xtstring& sTable)
{
  if (!TableInList(sTable))
    m_arrTables.push_back(sTable);
}
bool CERFreeSelectTables::RemoveTable(size_t nIndex)
{
  if (nIndex >= CountOfTables())
    return false;
  m_arrTables.erase(m_arrTables.begin() + nIndex);
  return true;
}
bool CERFreeSelectTables::RemoveTable(const xtstring& sTable)
{
  for (CxtstringVectorIterator it = m_arrTables.begin(); it != m_arrTables.end(); it++)
  {
    if ((*it) == sTable)
    {
      m_arrTables.erase(it);
      return true;
    }
  }
  return false;
}
bool CERFreeSelectTables::RemoveAllTables()
{
  m_arrTables.erase(m_arrTables.begin(), m_arrTables.end());
  return true;
}
void CERFreeSelectTables::GetAllTableNames(CxtstringVector& arrTableNames)
{
  arrTableNames.erase(arrTableNames.begin(), arrTableNames.end());
  arrTableNames.assign(m_arrTables.begin(), m_arrTables.end());
}
xtstring CERFreeSelectTables::GetAllTablesDelimited()
{
  xtstring sDelimited;
  for (CxtstringVectorIterator it = m_arrTables.begin(); it != m_arrTables.end(); it++)
  {
    if (sDelimited.size())
      sDelimited += _XT(", ");
    sDelimited += (*it);
  }
  return sDelimited;
}
void CERFreeSelectTables::SE_OnTableNameChanged(const xtstring& sOldName,
                                                const xtstring& sNewName)
{
  if (!TableInList(sOldName))
    return;
  for (CxtstringVectorIterator it = m_arrTables.begin(); it != m_arrTables.end(); it++)
  {
    if ((*it) == sOldName)
    {
      (*it) = sNewName;
      break;
    }
  }
}
void CERFreeSelectTables::SE_OnTableDelete(const xtstring& sName)
{
  if (!TableInList(sName))
    return;
  for (CxtstringVectorIterator it = m_arrTables.begin(); it != m_arrTables.end(); it++)
  {
    if ((*it) == sName)
    {
      m_arrTables.erase(it);
      break;
    }
  }
}







































bool CERModelFieldInd::SizeChangeableFromType(TFieldType nType)
{
  switch (nType)
  {
  case tFieldTypeEmpty:               return false;
  case tFieldTypeTinyInt:             return false;
  case tFieldTypeSmallInt:            return false;
  case tFieldTypeInteger:             return false;
  case tFieldTypeBigInt:              return false;
  case tFieldTypeUnsignedTinyInt:     return false;
  case tFieldTypeUnsignedSmallInt:    return false;
  case tFieldTypeUnsignedInt:         return false;
  case tFieldTypeUnsignedBigInt:      return false;
  case tFieldTypeSingle:              return false;
  case tFieldTypeDouble:              return false;
  case tFieldTypeCurrency:            return false;
  case tFieldTypeDecimal:             return false;
  case tFieldTypeNumeric:             return false;
  case tFieldTypeBoolean:             return false;
  case tFieldTypeError:               return false;
  case tFieldTypeUserDefined:         return true;
  case tFieldTypeVariant:             return false;
  case tFieldTypeIDispatch:           return false;
  case tFieldTypeIUnknown:            return false;
  case tFieldTypeGUID:                return false;
  case tFieldTypeDate:                return false;
  case tFieldTypeDBDate:              return false;
  case tFieldTypeDBTime:              return false;
  case tFieldTypeDBTimeStamp:         return false;
  case tFieldTypeBSTR:                return true;
  case tFieldTypeChar:                return true;
  case tFieldTypeVarChar:             return true;
  case tFieldTypeLongVarChar:         return false;
  case tFieldTypeWChar:               return true;
  case tFieldTypeVarWChar:            return true;
  case tFieldTypeLongVarWChar:        return false;
  case tFieldTypeBinary:              return true;
  case tFieldTypeVarBinary:           return true;
  case tFieldTypeLongVarBinary:       return false;
  case tFieldTypeChapter:             return false;
  case tFieldTypeFileTime:            return false;
  case tFieldTypePropVariant:         return false;
  case tFieldTypeVarNumeric:          return false;
  }
  return false;
}

bool CERModelFieldInd::TypeNeedApo(TFieldType nType)
{
  switch (nType)
  {
  case tFieldTypeEmpty:               return false;
  case tFieldTypeTinyInt:             return false;
  case tFieldTypeSmallInt:            return false;
  case tFieldTypeInteger:             return false;
  case tFieldTypeBigInt:              return false;
  case tFieldTypeUnsignedTinyInt:     return false;
  case tFieldTypeUnsignedSmallInt:    return false;
  case tFieldTypeUnsignedInt:         return false;
  case tFieldTypeUnsignedBigInt:      return false;
  case tFieldTypeSingle:              return false;
  case tFieldTypeDouble:              return false;
  case tFieldTypeCurrency:            return false;
  case tFieldTypeDecimal:             return false;
  case tFieldTypeNumeric:             return false;
  case tFieldTypeBoolean:             return false;
  case tFieldTypeError:               return false;
  case tFieldTypeUserDefined:         return false;
  case tFieldTypeVariant:             return false;
  case tFieldTypeIDispatch:           return false;
  case tFieldTypeIUnknown:            return false;
  case tFieldTypeGUID:                return false;
  case tFieldTypeDate:                return false;
  case tFieldTypeDBDate:              return false;
  case tFieldTypeDBTime:              return false;
  case tFieldTypeDBTimeStamp:         return false;
  case tFieldTypeBSTR:                return true;
  case tFieldTypeChar:                return true;
  case tFieldTypeVarChar:             return true;
  case tFieldTypeLongVarChar:         return true;
  case tFieldTypeWChar:               return true;
  case tFieldTypeVarWChar:            return true;
  case tFieldTypeLongVarWChar:        return true;
  case tFieldTypeBinary:              return false;
  case tFieldTypeVarBinary:           return false;
  case tFieldTypeLongVarBinary:       return false;
  case tFieldTypeChapter:             return false;
  case tFieldTypeFileTime:            return false;
  case tFieldTypePropVariant:         return false;
  case tFieldTypeVarNumeric:          return false;
  }
  return false;
}

bool CERModelFieldInd::TypeNeedHash(TFieldType nType)
{
  switch (nType)
  {
  case tFieldTypeEmpty:               return false;
  case tFieldTypeTinyInt:             return false;
  case tFieldTypeSmallInt:            return false;
  case tFieldTypeInteger:             return false;
  case tFieldTypeBigInt:              return false;
  case tFieldTypeUnsignedTinyInt:     return false;
  case tFieldTypeUnsignedSmallInt:    return false;
  case tFieldTypeUnsignedInt:         return false;
  case tFieldTypeUnsignedBigInt:      return false;
  case tFieldTypeSingle:              return false;
  case tFieldTypeDouble:              return false;
  case tFieldTypeCurrency:            return false;
  case tFieldTypeDecimal:             return false;
  case tFieldTypeNumeric:             return false;
  case tFieldTypeBoolean:             return false;
  case tFieldTypeError:               return false;
  case tFieldTypeUserDefined:         return false;
  case tFieldTypeVariant:             return false;
  case tFieldTypeIDispatch:           return false;
  case tFieldTypeIUnknown:            return false;
  case tFieldTypeGUID:                return false;
  case tFieldTypeDate:                return true;
  case tFieldTypeDBDate:              return true;
  case tFieldTypeDBTime:              return true;
  case tFieldTypeDBTimeStamp:         return false;
  case tFieldTypeBSTR:                return false;
  case tFieldTypeChar:                return false;
  case tFieldTypeVarChar:             return false;
  case tFieldTypeLongVarChar:         return false;
  case tFieldTypeWChar:               return false;
  case tFieldTypeVarWChar:            return false;
  case tFieldTypeLongVarWChar:        return false;
  case tFieldTypeBinary:              return false;
  case tFieldTypeVarBinary:           return false;
  case tFieldTypeLongVarBinary:       return false;
  case tFieldTypeChapter:             return false;
  case tFieldTypeFileTime:            return false;
  case tFieldTypePropVariant:         return false;
  case tFieldTypeVarNumeric:          return false;
  }
  return false;
}

xtstring CERModelFieldInd::GetFieldTypeName(TFieldType nType)
{
  xtstring sType;

  sType.erase();
  switch (nType)
  {
  case tFieldTypeEmpty:             sType = _XT("Empty");               break;
  case tFieldTypeTinyInt:           sType = _XT("TinyInt");             break;
  case tFieldTypeSmallInt:          sType = _XT("SmallInt");            break;
  case tFieldTypeInteger:           sType = _XT("Integer");             break;
  case tFieldTypeBigInt:            sType = _XT("BigInt");              break;
  case tFieldTypeUnsignedTinyInt:   sType = _XT("UnsignedTinyInt");     break;
  case tFieldTypeUnsignedSmallInt:  sType = _XT("UnsignedSmallInt");    break;
  case tFieldTypeUnsignedInt:       sType = _XT("UnsignedInt");         break;
  case tFieldTypeUnsignedBigInt:    sType = _XT("UnsignedBigInt");      break;
  case tFieldTypeSingle:            sType = _XT("Single");              break;
  case tFieldTypeDouble:            sType = _XT("Double");              break;
  case tFieldTypeCurrency:          sType = _XT("Currency");            break;
  case tFieldTypeDecimal:           sType = _XT("Decimal");             break;
  case tFieldTypeNumeric:           sType = _XT("Numeric");             break;
  case tFieldTypeBoolean:           sType = _XT("Boolean");             break;
  case tFieldTypeError:             sType = _XT("Error");               break;
  case tFieldTypeUserDefined:       sType = _XT("UserDefined");         break;
  case tFieldTypeVariant:           sType = _XT("Variant");             break;
  case tFieldTypeIDispatch:         sType = _XT("IDispatch");           break;
  case tFieldTypeIUnknown:          sType = _XT("IUnknown");            break;
  case tFieldTypeGUID:              sType = _XT("GUID");                break;
  case tFieldTypeDate:              sType = _XT("Date");                break;
  case tFieldTypeDBDate:            sType = _XT("DBDate");              break;
  case tFieldTypeDBTime:            sType = _XT("DBTime");              break;
  case tFieldTypeDBTimeStamp:       sType = _XT("DBTimeStamp");         break;
  case tFieldTypeBSTR:              sType = _XT("BSTR");                break;
  case tFieldTypeChar:              sType = _XT("Char");                break;
  case tFieldTypeVarChar:           sType = _XT("VarChar");             break;
  case tFieldTypeLongVarChar:       sType = _XT("LongVarChar");         break;
  case tFieldTypeWChar:             sType = _XT("WChar");               break;
  case tFieldTypeVarWChar:          sType = _XT("VarWChar");            break;
  case tFieldTypeLongVarWChar:      sType = _XT("LongVarWChar");        break;
  case tFieldTypeBinary:            sType = _XT("Binary");              break;
  case tFieldTypeVarBinary:         sType = _XT("VarBinary");           break;
  case tFieldTypeLongVarBinary:     sType = _XT("LongVarBinary");       break;
  case tFieldTypeChapter:           sType = _XT("Chapter");             break;
  case tFieldTypeFileTime:          sType = _XT("FileTime");            break;
  case tFieldTypePropVariant:       sType = _XT("PropVariant");         break;
  case tFieldTypeVarNumeric:        sType = _XT("VarNumeric");          break;
  default:                          sType = _XT("UNKNOWN");             break;
  }

  return sType;
}

xtstring CERModelFieldInd::GetFormatedFieldType(TFieldType nType,
                                                long nDefinedSize)
{
  xtstring sType;
  sType = GetFieldTypeName(nType);
  if (SizeChangeableFromType(nType))
  {
    xtstring sTemp;
    sTemp.Format(_XT("%s(%ld)"), sType.c_str(), nDefinedSize);
    sType = sTemp;
  }
  return sType;
}

xtstring CERModelFieldInd::GetFormatedFieldType()
{
  return GetFormatedFieldType(m_nType, m_nDefinedSize);
}








//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelFieldInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CERModelFieldInd::CERModelFieldInd(CSyncERNotifiersGroup* pGroup)
                 :CModelBaseInd(tERModelField),
                  CSyncNotifier_ER(tERNotifier_Field, pGroup),
                  CSyncResponse_Common(tRO_ERModel_Field)
{
  m_sName.erase();
  m_sComment.erase();
  m_nAttributes = tFieldAttrNone;
  m_nDefinedSize = 255;
  m_nType = tFieldTypeVarChar;
  m_sImportTableDefStructName.erase();
  m_nImportTableDefStructFieldIndex = 0;
  m_nFlags = 0;
  m_sPersistentData.erase();

  m_pPropName = 0;
  m_pPropComment = 0;
  m_pPropType = 0;
  m_pPropSize = 0;
  m_pPropAttributes = 0;

  m_pERTable = 0;
}

CERModelFieldInd::CERModelFieldInd(CERModelFieldInd& cField)
                 :CModelBaseInd(tERModelField),
                  CSyncNotifier_ER(tERNotifier_Field, NULL),
                  CSyncResponse_Common(tRO_ERModel_Field)
{
  SetSyncGroup(cField.GetSyncGroup());
  SetERModelFieldInd(&cField);

  m_pPropName = 0;
  m_pPropComment = 0;
  m_pPropType = 0;
  m_pPropSize = 0;
  m_pPropAttributes = 0;

  m_pERTable = 0;
}

CERModelFieldInd::CERModelFieldInd(CERModelFieldInd* pField)
                 :CModelBaseInd(tERModelField),
                  CSyncNotifier_ER(tERNotifier_Field, NULL),
                  CSyncResponse_Common(tRO_ERModel_Field)
{
  assert(pField);
  SetSyncGroup(pField->GetSyncGroup());
  SetERModelFieldInd(pField);

  m_pPropName = 0;
  m_pPropComment = 0;
  m_pPropType = 0;
  m_pPropSize = 0;
  m_pPropAttributes = 0;

  m_pERTable = 0;
}

CERModelFieldInd::~CERModelFieldInd()
{
}

xtstring CERModelFieldInd::GetERTableName()
{
  assert(m_pERTable);
  if (m_pERTable)
    return m_pERTable->GetName();
  return _XT("");
}

bool CERModelFieldInd::FieldRemovable()
{
  assert(m_pERTable);
  if (!m_pERTable)
    return true;
  if (m_pERTable->GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_FIELD_NOTREMOVABLE)
    return false;
  return true;
}

void CERModelFieldInd::SetERModelFieldInd(CERModelFieldInd* pField)
{
  if (!pField)
    return;

  m_nAttributes = pField->m_nAttributes;
  m_nDefinedSize = pField->m_nDefinedSize;
  m_nType = pField->m_nType;
  m_sName = pField->m_sName;
  m_sComment = pField->m_sComment;
  m_sImportTableDefStructName = pField->m_sImportTableDefStructName;
  m_nImportTableDefStructFieldIndex = pField->m_nImportTableDefStructFieldIndex;
  m_nFlags = pField->m_nFlags;
  m_sPersistentData = pField->m_sPersistentData;
}

bool CERModelFieldInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(FIELD_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Field>
    pXMLDoc->SetAttrib(FIELD_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_COMMENT, GetComment().c_str());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_TYPE, GetType());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_DEFINEDSIZE, GetDefinedSize());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_ATTRIBUTES, GetAttributes());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_IMPORTTABLEDEFSTRUCTNAME, GetImportTableDefStructName().c_str());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_IMPORTTABLEDEFSTRUCTFIELDINDEX, GetImportTableDefStructFieldIndex());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_FLAGS, GetFlags());
    pXMLDoc->SetAttrib(FIELD_ATTRIB_PERSISTENTDATA, GetPersistentData().c_str());

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CERModelFieldInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Field>
  SetName(pXMLDoc->GetAttrib(FIELD_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(FIELD_ATTRIB_COMMENT));
  SetType((TFieldType)pXMLDoc->GetAttribLong(FIELD_ATTRIB_TYPE));
  SetDefinedSize(pXMLDoc->GetAttribLong(FIELD_ATTRIB_DEFINEDSIZE));
  SetAttributes((TFieldAttributes)pXMLDoc->GetAttribLong(FIELD_ATTRIB_ATTRIBUTES));
  SetImportTableDefStructName(pXMLDoc->GetAttrib(FIELD_ATTRIB_IMPORTTABLEDEFSTRUCTNAME));
  SetImportTableDefStructFieldIndex(pXMLDoc->GetAttribLong(FIELD_ATTRIB_IMPORTTABLEDEFSTRUCTFIELDINDEX));
  SetFlags(pXMLDoc->GetAttribLong(FIELD_ATTRIB_FLAGS));
  SetPersistentData(pXMLDoc->GetAttrib(FIELD_ATTRIB_PERSISTENTDATA));
  bRet = true;

  return bRet;
}

xtstring CERModelFieldInd::GetNameForShow() const
{
  return m_sName;
/*
  // fuer gewisse Zeit wird nicht benutzt.
  CString sType;
  CString sLen;
  CString sName;
  CString sAttribute;

  sType = GetFieldTypeName(m_nType);

  if (m_nDefinedSize)
    sLen.Format(_T("(%ld)"), m_nDefinedSize);

  if (m_nAttributes == ADOX::adColFixed)
    sAttribute = _T("FIXED");
  else if (m_nAttributes == ADOX::adColNullable)
    sAttribute = _T("NULL");;

  sName = m_sName;
  sName += _T(" - ");
  sName += sType;
  if (!sAttribute.IsEmpty())
  {
    sName += _T(" ");
    sName += sAttribute;
  }
  if (!sLen.IsEmpty())
  {
    sName += _T(" ");
    sName += sLen;
  }
  return sName;
*/
}

























//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelParamInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CERModelParamInd::CERModelParamInd(CSyncERNotifiersGroup* pGroup)
                 :CModelBaseInd(tERModelParam),
                  CSyncNotifier_ER(tERNotifier_Param, pGroup),
                  CSyncResponse_Common(tRO_ERModel_Param)
{
  m_sName.erase();
  m_sComment.erase();
  m_sDefaultValue.erase();
  m_nDefinedSize = 20;
  m_nType = tFieldTypeVarChar;
  m_nFlags = 0;
  m_sPersistentData.erase();

  m_pERTable = 0;
}

CERModelParamInd::CERModelParamInd(CERModelParamInd& cParam)
                 :CModelBaseInd(tERModelParam),
                  CSyncNotifier_ER(tERNotifier_Param, NULL),
                  CSyncResponse_Common(tRO_ERModel_Param)
{
  SetSyncGroup(cParam.GetSyncGroup());
  SetERModelParamInd(&cParam);

  m_pERTable = 0;
}

CERModelParamInd::CERModelParamInd(CERModelParamInd* pParam)
                 :CModelBaseInd(tERModelParam),
                  CSyncNotifier_ER(tERNotifier_Param, NULL),
                  CSyncResponse_Common(tRO_ERModel_Param)
{
  assert(pParam);
  SetSyncGroup(pParam->GetSyncGroup());
  SetERModelParamInd(pParam);

  m_pERTable = 0;
}

CERModelParamInd::~CERModelParamInd()
{
}

xtstring CERModelParamInd::GetERTableName()
{
  assert(m_pERTable);
  if (m_pERTable)
    return m_pERTable->GetName();
  return _XT("");
}

bool CERModelParamInd::ParamRemovable()
{
  assert(m_pERTable);
  if (!m_pERTable)
    return true;
  if (m_pERTable->GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_PARAM_NOTREMOVABLE)
    return false;
  return true;
}

void CERModelParamInd::SetERModelParamInd(CERModelParamInd* pParam)
{
  if (!pParam)
    return;

  m_sDefaultValue = pParam->m_sDefaultValue;
  m_nDefinedSize = pParam->m_nDefinedSize;
  m_nType = pParam->m_nType;
  m_sName = pParam->m_sName;
  m_sComment = pParam->m_sComment;
  m_nFlags = pParam->m_nFlags;
  m_sPersistentData = pParam->m_sPersistentData;
}

bool CERModelParamInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(PARAM_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Field>
    pXMLDoc->SetAttrib(PARAM_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(PARAM_ATTRIB_COMMENT, GetComment().c_str());
    pXMLDoc->SetAttrib(PARAM_ATTRIB_TYPE, GetType());
    pXMLDoc->SetAttrib(PARAM_ATTRIB_DEFINEDSIZE, GetDefinedSize());
    pXMLDoc->SetAttrib(PARAM_ATTRIB_DEFAULTVALUE, GetDefaultValue().c_str());
    pXMLDoc->SetAttrib(PARAM_ATTRIB_FLAGS, GetFlags());
    pXMLDoc->SetAttrib(PARAM_ATTRIB_PERSISTENTDATA, GetPersistentData().c_str());

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CERModelParamInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Field>
  SetName(pXMLDoc->GetAttrib(PARAM_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(PARAM_ATTRIB_COMMENT));
  SetType((TFieldType)pXMLDoc->GetAttribLong(PARAM_ATTRIB_TYPE));
  SetDefinedSize(pXMLDoc->GetAttribLong(PARAM_ATTRIB_DEFINEDSIZE));
  SetDefaultValue(pXMLDoc->GetAttrib(PARAM_ATTRIB_DEFAULTVALUE));
  SetFlags(pXMLDoc->GetAttribLong(PARAM_ATTRIB_FLAGS));
  SetPersistentData(pXMLDoc->GetAttrib(PARAM_ATTRIB_PERSISTENTDATA));
  bRet = true;

  return bRet;
}

xtstring CERModelParamInd::GetNameForShow() const
{
  return m_sName;
}

































//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelRelationInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CERModelRelationInd::CERModelRelationInd(bool bRelationIsInChildTable,
                                         CSyncERNotifiersGroup* pGroup)
                    :CModelBaseInd(tERModelRelation),
                     CSyncNotifier_ER(tERNotifier_Relation, pGroup)
{
  m_bRelationIsInChildTable = bRelationIsInChildTable;
  m_nType = tRelation_1N;
  m_sName.erase();
  m_sComment.erase();
  m_nParentUpdateRule = tRelationRuleRINone;
  m_nParentDeleteRule = tRelationRuleRICascade;
  m_nChildUpdateRule = tRelationRuleRINone;
  m_nChildDeleteRule = tRelationRuleRINone;
  m_sParentTable = _XT("ParentTable");
  m_sChildTable = _XT("ChildTable");
  m_arrParentFields.erase(m_arrParentFields.begin(), m_arrParentFields.end());
  m_arrChildFields.erase(m_arrChildFields.begin(), m_arrChildFields.end());
  m_nFlags = 0;
  m_sPersistentData.erase();

  m_pERTable = 0;
}

CERModelRelationInd::CERModelRelationInd(CERModelRelationInd& cRelation)
                    :CModelBaseInd(tERModelRelation),
                     CSyncNotifier_ER(tERNotifier_Relation, NULL)
{
  SetSyncGroup(cRelation.GetSyncGroup());
  SetERModelRelationInd(&cRelation);

  m_pERTable = 0;
}

CERModelRelationInd::CERModelRelationInd(CERModelRelationInd* pRelation)
                    :CModelBaseInd(tERModelRelation),
                     CSyncNotifier_ER(tERNotifier_Relation, NULL)
{
  assert(pRelation);
  SetSyncGroup(pRelation->GetSyncGroup());
  SetERModelRelationInd(pRelation);

  m_pERTable = 0;
}

CERModelRelationInd::~CERModelRelationInd()
{
}

xtstring CERModelRelationInd::GetERTableName()
{
  assert(m_pERTable);
  if (m_pERTable)
    return m_pERTable->GetName();
  return _XT("");
}

bool CERModelRelationInd::RelationRemovable()
{
  assert(m_pERTable);
  if (!m_pERTable)
    return true;
  if (m_pERTable->GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_RELATION_NOTREMOVABLE)
    return false;
  return true;
}

void CERModelRelationInd::operator =(CERModelRelationInd& cRelation)
{
  SetERModelRelationInd(&cRelation);
}

bool CERModelRelationInd::operator ==(CERModelRelationInd& cRelation)
{
//  if (m_bRelationIsInChildTable != cRelation.m_bRelationIsInChildTable)
//    return false;
  if (m_nType != cRelation.m_nType)
    return false;
  if (m_sName != cRelation.m_sName)
    return false;
  if (m_sComment != cRelation.m_sComment)
    return false;
  if (m_sParentTable != cRelation.m_sParentTable)
    return false;
  if (m_sChildTable != cRelation.m_sChildTable)
    return false;
  if (m_nParentUpdateRule != cRelation.m_nParentUpdateRule)
    return false;
  if (m_nParentDeleteRule != cRelation.m_nParentDeleteRule)
    return false;
  if (m_nChildUpdateRule != cRelation.m_nChildUpdateRule)
    return false;
  if (m_nChildDeleteRule != cRelation.m_nChildDeleteRule)
    return false;
  if (m_nFlags != cRelation.m_nFlags)
    return false;
  if (m_sPersistentData != cRelation.m_sPersistentData)
    return false;

  if (m_arrParentFields.size() != cRelation.m_arrParentFields.size())
    return false;
  if (m_arrChildFields.size() != cRelation.m_arrChildFields.size())
    return false;

  for (size_t nI = 0; nI < m_arrParentFields.size(); nI++)
    if (m_arrParentFields[nI] != cRelation.m_arrParentFields[nI])
      return false;
  for (size_t nI = 0; nI < m_arrChildFields.size(); nI++)
    if (m_arrChildFields[nI] != cRelation.m_arrChildFields[nI])
      return false;

  return true;
}

bool CERModelRelationInd::CompareRelation(CERModelRelationInd& cRelation)
{
//  if (m_bRelationIsInChildTable != cRelation.m_bRelationIsInChildTable)
//    return false;
  if (m_nType != cRelation.m_nType)
    return false;
//  if (m_sName != cRelation.m_sName)
//    return false;
//  if (m_sComment != cRelation.m_sComment)
//    return false;
  if (m_sParentTable != cRelation.m_sParentTable)
    return false;
  if (m_sChildTable != cRelation.m_sChildTable)
    return false;
//  if (m_nParentUpdateRule != cRelation.m_nParentUpdateRule)
//    return false;
//  if (m_nParentDeleteRule != cRelation.m_nParentDeleteRule)
//    return false;
//  if (m_nChildUpdateRule != cRelation.m_nChildUpdateRule)
//    return false;
//  if (m_nChildDeleteRule != cRelation.m_nChildDeleteRule)
//    return false;

  if (m_arrParentFields.size() != cRelation.m_arrParentFields.size())
    return false;
  if (m_arrChildFields.size() != cRelation.m_arrChildFields.size())
    return false;

  for (size_t nI = 0; nI < m_arrParentFields.size(); nI++)
    if (m_arrParentFields[nI] != cRelation.m_arrParentFields[nI])
      return false;
  for (size_t nI = 0; nI < m_arrChildFields.size(); nI++)
    if (m_arrChildFields[nI] != cRelation.m_arrChildFields[nI])
      return false;

  return true;
}

void CERModelRelationInd::SetERModelRelationInd(const CERModelRelationInd* pRelation)
{
  if (!pRelation)
    return;

//  m_bRelationIsInChildTable = pRelation->m_bRelationIsInChildTable;
  m_nType = pRelation->m_nType;
  m_sName = pRelation->m_sName;
  m_sComment = pRelation->m_sComment;
  m_sParentTable = pRelation->m_sParentTable;
  m_sChildTable = pRelation->m_sChildTable;
  m_nParentUpdateRule = pRelation->m_nParentUpdateRule;
  m_nParentDeleteRule = pRelation->m_nParentDeleteRule;
  m_nChildUpdateRule = pRelation->m_nChildUpdateRule;
  m_nChildDeleteRule = pRelation->m_nChildDeleteRule;
  m_arrParentFields.erase(m_arrParentFields.begin(), m_arrParentFields.end());
  m_arrChildFields.erase(m_arrChildFields.begin(), m_arrChildFields.end());
  assert(pRelation->m_arrParentFields.size() == pRelation->m_arrChildFields.size());
  for (size_t nI = 0; nI < pRelation->m_arrParentFields.size(); nI++)
  {
    m_arrParentFields.push_back(pRelation->m_arrParentFields[nI]);
    m_arrChildFields.push_back(pRelation->m_arrChildFields[nI]);
  }
  m_nFlags = pRelation->m_nFlags;
  m_sPersistentData = pRelation->m_sPersistentData;
}

bool CERModelRelationInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(RELATION_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Field>
    xtstring sText;
    pXMLDoc->SetAttrib(RELATION_ATTRIB_NAME,              GetName().c_str());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_COMMENT,           GetComment().c_str());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_TYPE,              GetType());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_PARENTTABLE,       GetParentTable().c_str());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_CHILDTABLE,        GetChildTable().c_str());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_PARENTUPDATERULE,  GetParentUpdateRule());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_PARENTDELETERULE,  GetParentDeleteRule());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_CHILDUPDATERULE,   GetChildUpdateRule());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_CHILDDELETERULE,   GetChildDeleteRule());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_PARENTFIELDS,      GetParentFieldsDelimited(false).c_str());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_CHILDFIELDS,       GetChildFieldsDelimited(false).c_str());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_FLAGS,             GetFlags());
    pXMLDoc->SetAttrib(RELATION_ATTRIB_PERSISTENTDATA,    GetPersistentData().c_str());

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CERModelRelationInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Relation>
  SetName(pXMLDoc->GetAttrib(RELATION_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(RELATION_ATTRIB_COMMENT));
  SetType((TRelationType)pXMLDoc->GetAttribLong(RELATION_ATTRIB_TYPE));
  SetParentTable(pXMLDoc->GetAttrib(RELATION_ATTRIB_PARENTTABLE));
  SetChildTable(pXMLDoc->GetAttrib(RELATION_ATTRIB_CHILDTABLE));
  SetParentUpdateRule((TRelationRule)pXMLDoc->GetAttribLong(RELATION_ATTRIB_PARENTUPDATERULE));
  SetParentDeleteRule((TRelationRule)pXMLDoc->GetAttribLong(RELATION_ATTRIB_PARENTDELETERULE));
  SetChildUpdateRule((TRelationRule)pXMLDoc->GetAttribLong(RELATION_ATTRIB_CHILDUPDATERULE));
  SetChildDeleteRule((TRelationRule)pXMLDoc->GetAttribLong(RELATION_ATTRIB_CHILDDELETERULE));
  SetFlags(pXMLDoc->GetAttribLong(RELATION_ATTRIB_FLAGS));
  SetPersistentData(pXMLDoc->GetAttrib(RELATION_ATTRIB_PERSISTENTDATA));

  xtstring sText1;
  xtstring sText2;

  sText1 = pXMLDoc->GetAttrib(RELATION_ATTRIB_PARENTFIELDS);
  sText2 = pXMLDoc->GetAttrib(RELATION_ATTRIB_CHILDFIELDS);
  ParseAndAddDelimitedFields(sText1, sText2);



  bRet = true;
  return bRet;
}

size_t CERModelRelationInd::CountOfFields() const
{
  if (m_arrParentFields.size() == m_arrChildFields.size())
    return m_arrChildFields.size();
  // RemoveAllFields();
  return 0;
}

void CERModelRelationInd::RemoveAllFields()
{
  m_arrParentFields.erase(m_arrParentFields.begin(), m_arrParentFields.end());
  m_arrChildFields.erase(m_arrChildFields.begin(), m_arrChildFields.end());
}

void CERModelRelationInd::GetFields(size_t nIndex,
                                    xtstring& sParentField,
                                    xtstring& sChildField) const
{
  // MAK: es kam hier zu Problem, dass beide Felder waren, aber Aufruf erfolgte 
  if (m_arrChildFields.size() == 0 || m_arrParentFields.size() == 0)
    return;
  assert(m_arrParentFields.size() == m_arrChildFields.size() && nIndex < m_arrChildFields.size());
  sParentField = m_arrParentFields[nIndex];
  sChildField = m_arrChildFields[nIndex];
}

void CERModelRelationInd::AddFields(const xtstring& sParentField,
                                    const xtstring& sChildField)
{
  m_arrParentFields.push_back(sParentField);
  m_arrChildFields.push_back(sChildField);
}
#define REL_FIELDS_DELIMITER    _XT("@")
xtstring CERModelRelationInd::GetParentFieldsDelimited(bool bForProperty) const
{
  xtstring sRet;
  for (CxtstringConstVectorIterator it = m_arrParentFields.begin(); it != m_arrParentFields.end(); it++)
  {
    if (it != m_arrParentFields.begin())
      if (bForProperty)
        sRet += _XT(", ");
      else
        sRet += REL_FIELDS_DELIMITER;
    sRet += (*it);
  }
  return sRet;
}

xtstring CERModelRelationInd::GetChildFieldsDelimited(bool bForProperty) const
{
  xtstring sRet;
  for (CxtstringConstVectorIterator it = m_arrChildFields.begin(); it != m_arrChildFields.end(); it++)
  {
    if (it != m_arrChildFields.begin())
      if (bForProperty)
        sRet += _XT(", ");
      else
        sRet += REL_FIELDS_DELIMITER;
    sRet += (*it);
  }
  return sRet;
}

void CERModelRelationInd::ParseAndAddDelimitedFields(const xtstring sParentFields,
                                                     const xtstring sChildFields)
{
  xtstring sPFields;
  xtstring sChFields;
  int nPCount;
  int nChCount;

  sPFields = sParentFields;
  sChFields = sChildFields;

  nPCount = sPFields.GetFieldCount(REL_FIELDS_DELIMITER);
  nChCount = sChFields.GetFieldCount(REL_FIELDS_DELIMITER);
  if (nPCount != nChCount)
  {
    if (nChCount > nPCount)
      nChCount = nPCount;
    else
      nPCount = nChCount;
  }

  xtstring sP;
  xtstring sCh;
  for (int nI = 0; nI < nPCount; nI++)
  {
    sP = sPFields.GetField(REL_FIELDS_DELIMITER, nI);
    sCh = sChFields.GetField(REL_FIELDS_DELIMITER, nI);
    AddFields(sP, sCh);
  }
}

void CERModelRelationInd::SE_OnTableNameChanged(const xtstring& sOldName,
                                                const xtstring& sNewName)
{
  if (m_sParentTable == sOldName)
    m_sParentTable = sNewName;
  if (m_sChildTable == sOldName)
    m_sChildTable = sNewName;
}

void CERModelRelationInd::SE_OnFieldNameChanged(const xtstring& sTableName,
                                                const xtstring& sOldName,
                                                const xtstring& sNewName)
{
  if (m_sParentTable == sTableName)
  {
    for (size_t nI = 0; nI < m_arrParentFields.size(); nI++)
    {
      if (m_arrParentFields[nI] == sOldName)
        m_arrParentFields[nI] = sNewName;
    }
  }
  if (m_sChildTable == sTableName)
  {
    for (size_t nI = 0; nI < m_arrChildFields.size(); nI++)
    {
      if (m_arrChildFields[nI] == sOldName)
        m_arrChildFields[nI] = sNewName;
    }
  }
}

void CERModelRelationInd::SE_OnRelationNameChanged(const xtstring& sTableName,
                                                   const xtstring& sParentTableName,
                                                   const xtstring& sOldName,
                                                   const xtstring& sNewName)
{
//  if (/*sTableName == GetParentTable() && !m_bRelationIsInChildTable
//      || */sTableName == GetChildTable()/* && m_bRelationIsInChildTable*/)
//    if (sOldName == GetName())
//      SetName(sNewName);
  if (sTableName == GetChildTable() && sParentTableName == GetParentTable())
    if (sOldName == GetName())
      SetName(sNewName);
}

bool CERModelRelationInd::SE_OnRelationNameWillBeChanged(const xtstring& sTableName,
                                                         const xtstring& sParentTableName,
                                                         const xtstring& sOldName,
                                                         const xtstring& sNewName)
{
  // Diese Funktion ist dazu, dass man damit kontrollieren kann,
  // ob Relationsname schon existiert. Falls ja, melden wir FALSE
  // und damit kann man die sNewName nicht benutzen.
  if (!m_bRelationIsInChildTable)
    return true;
  if (sTableName == GetChildTable() && sParentTableName == GetParentTable() && sNewName == GetName())
    // don't change
    return false;
  // can be changed
  return true;
}

void CERModelRelationInd::RenameTable(const xtstring& sOldName,
                                      const xtstring& sNewName)
{
  if (GetParentTable() == sOldName)
    SetParentTable(sNewName);
  if (GetChildTable() == sOldName)
    SetChildTable(sNewName);
}










/*
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelTablesForFreeSelect
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CERModelTablesForFreeSelect::CERModelTablesForFreeSelect(CSyncERNotifiersGroup* pGroup)
                            :CModelBaseInd(tERModelTablesForFreeSelect),
                             CSyncNotifier_ER(tERNotifier_TablesForFreeSelect, pGroup)
{
}
CERModelTablesForFreeSelect::~CERModelTablesForFreeSelect()
{
}
bool CERModelTablesForFreeSelect::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  char sKey[50];
  for (size_t nI = 0; nI < m_arrTables.size(); nI++)
  {
    sprintf(sKey, NODE_ELEM_TABLE_FOR_FREE_SELECT, nI);
    pXMLDoc->AddAttrib(sKey, m_arrTables[nI].c_str());
  }
  return true;
}
bool CERModelTablesForFreeSelect::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  m_arrTables.erase(m_arrTables.begin(), m_arrTables.end());

  xtstring sTable;
  char sKey[50];
  for (size_t nI = 0;;nI++)
  {
    sprintf(sKey, NODE_ELEM_TABLE_FOR_FREE_SELECT, nI);
    sTable = pXMLDoc->GetAttrib(sKey);
    if (!sTable.size())
      break;
    m_arrTables.push_back(sTable);
  }
  return true;
}
bool CERModelTablesForFreeSelect::AssignFrom(const CERModelTablesForFreeSelect* pTables)
{
  if (!pTables)
    return false;

  m_arrTables.erase(m_arrTables.begin(), m_arrTables.end());
  for (size_t nI = 0; nI < pTables->CountOfTables(); nI++)
  {
    xtstring sTable;
    sTable = pTables->GetTable(nI);
    m_arrTables.push_back(sTable);
  }
  return true;
}
bool CERModelTablesForFreeSelect::CopyFrom(const CERModelTablesForFreeSelect* pTables, bool& bChanged)
{
  if (!pTables)
    return false;

  bChanged = false;

  // sind alle pTables Tabellen da?
  for (size_t nI = 0; nI < pTables->CountOfTables(); nI++)
  {
    if (!TableInList(pTables->GetTable(nI)))
    {
      bChanged = true;
      break;
    }
  }
  // sind alle meine Tabellen in pTables?
  if (!bChanged)
  {
    for (size_t nI = 0; nI < CountOfTables(); nI++)
    {
      if (!pTables->TableInList(GetTable(nI)))
      {
        bChanged = true;
        break;
      }
    }
  }
  // sind alle Felder in Tabllen identisch?
  if (!bChanged)
  {
    for (size_t nI = 0; nI < gui_CountOfTables(); nI++)
    {
      CERModelTableInd* pMyTable = gui_GetTable(nI);
      if (!pMyTable)
        continue;
      CERModelTableInd* pYourTable = pTables->gui_GetTable_const(pMyTable->GetName());
      if (!pYourTable)
        continue;
      if (pMyTable->CountOfFields() != pYourTable->CountOfFields())
      {
        bChanged = true;
        break;
      }
      for (size_t nJ = 0; nJ < pMyTable->CountOfFields(); nJ++)
      {
        CERModelFieldInd* pMyField = pMyTable->GetField(nJ);
        if (!pMyField)
        {
          bChanged = true;
          break;
        }
        CERModelFieldInd* pYourField = pYourTable->GetField(pMyField->GetName());
        if (!pYourField)
        {
          bChanged = true;
          break;
        }
      }
      if (!bChanged)
      {
        for (size_t nJ = 0; nJ < pYourTable->CountOfFields(); nJ++)
        {
          CERModelFieldInd* pYourField = pYourTable->GetField(nJ);
          if (!pYourField)
          {
            bChanged = true;
            break;
          }
          CERModelFieldInd* pMyField = pMyTable->GetField(pYourField->GetName());
          if (!pMyField)
          {
            bChanged = true;
            break;
          }
        }
      }
      if (bChanged)
        break;
    }
  }
  if (bChanged)
    return AssignFrom(pTables);
  return true;
}
bool CERModelTablesForFreeSelect::TableInList(const xtstring& sTable) const
{
  for (size_t nI = 0; nI < CountOfTables(); nI++)
  {
    if (GetTable(nI) == sTable)
      return true;
  }
  return false;
}
xtstring CERModelTablesForFreeSelect::GetTable(size_t nIndex) const
{
  xtstring sTable;
  if (nIndex < CountOfTables())
    sTable = m_arrTables[nIndex];
  return sTable;
}
void CERModelTablesForFreeSelect::AddTable(const xtstring& sTable)
{
  if (!TableInList(sTable))
    m_arrTables.push_back(sTable);
}
bool CERModelTablesForFreeSelect::RemoveTable(size_t nIndex)
{
  if (nIndex >= CountOfTables())
    return false;
  m_arrTables.erase(m_arrTables.begin() + nIndex);
  return true;
}
bool CERModelTablesForFreeSelect::RemoveTable(const xtstring& sTable)
{
  for (CxtstringVectorIterator it = m_arrTables.begin(); it != m_arrTables.end(); it++)
  {
    if ((*it) == sTable)
    {
      m_arrTables.erase(it);
      return true;
    }
  }
  return false;
}
bool CERModelTablesForFreeSelect::RemoveAllTables()
{
  m_arrTables.erase(m_arrTables.begin(), m_arrTables.end());
  return true;
}
void CERModelTablesForFreeSelect::GetAllTableNames(CxtstringVector& arrTableNames)
{
  arrTableNames.erase(arrTableNames.begin(), arrTableNames.end());
  arrTableNames.assign(m_arrTables.begin(), m_arrTables.end());
}
xtstring CERModelTablesForFreeSelect::GetAllTablesDelimited()
{
  xtstring sDelimited;
  for (CxtstringVectorIterator it = m_arrTables.begin(); it != m_arrTables.end(); it++)
  {
    if (sDelimited.size())
      sDelimited += _XT(", ");
    sDelimited += (*it);
  }
  return sDelimited;
}
void CERModelTablesForFreeSelect::SE_OnTableNameChanged(const xtstring& sOldName,
                                                        const xtstring& sNewName)
{
  if (!TableInList(sOldName))
    return;
  for (CxtstringVectorIterator it = m_arrTables.begin(); it != m_arrTables.end(); it++)
  {
    if ((*it) == sOldName)
    {
      (*it) = sNewName;
      break;
    }
  }
}
void CERModelTablesForFreeSelect::SE_OnTableDelete(const xtstring& sName)
{
  if (!TableInList(sName))
    return;
  for (CxtstringVectorIterator it = m_arrTables.begin(); it != m_arrTables.end(); it++)
  {
    if ((*it) == sName)
    {
      m_arrTables.erase(it);
      break;
    }
  }
}

void CERModelTablesForFreeSelect::gui_TransportTables()
{
  RemoveAllTables();
  for (CERModelTablesIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
    AddTable((*it)->GetName());
}
void CERModelTablesForFreeSelect::gui_Close()
{
  m_bInGUIChanged = false;
  for (CERModelTablesIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
    delete (*it);
  m_guiTables.erase(m_guiTables.begin(), m_guiTables.end());
}
CERModelTableInd* CERModelTablesForFreeSelect::gui_GetTable(const xtstring& sTableName)
{
  for (CERModelTablesIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
  {
    if ((*it)->GetName() == sTableName)
      return (*it);
  }
  return 0;
}
CERModelTableInd* CERModelTablesForFreeSelect::gui_GetTable_const(const xtstring& sTableName) const
{
  for (CERModelTablesConstIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
  {
    if ((*it)->GetName() == sTableName)
      return (*it);
  }
  return 0;
}
void CERModelTablesForFreeSelect::gui_BuildClassForTable(const xtstring& sTableName, bool bSetChanged)
{
  if (bSetChanged)
    m_bInGUIChanged = true;
  const CERModelTableInd* pParentTable = SE_MF_GetERModelTableInd(m_sParentTableName);
  const CERModelTableInd* pTableToCopy = SE_MF_GetERModelTableInd(sTableName);
  if (!pParentTable || !pTableToCopy)
    return;
  CERModelTableInd* pNewTable = new CERModelTableInd((CSyncERNotifiersGroup*)NULL);
  if (!pNewTable)
    return;
  pNewTable->SetERModelTableInd((CERModelTableInd*)pTableToCopy, false);

  bool bDeleted;
  do
  {
    bDeleted = false;
    for (size_t nI = 0; nI < pNewTable->CountOfFields(); nI++)
    {
      const CERModelFieldInd* pField = pNewTable->GetFieldConst(nI);
      if (!pField)
        continue;
      if (!pParentTable->GetFieldConst(pField->GetName()))
      {
        bDeleted = true;
        pNewTable->RemoveField(nI);
        break;
      }
    }
  }
  while (bDeleted);

  m_guiTables.push_back(pNewTable);
}
void CERModelTablesForFreeSelect::gui_RemoveClassTable(const xtstring& sTableName)
{
  m_bInGUIChanged = true;
  for (CERModelTablesIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
  {
    if ((*it)->GetName() == sTableName)
    {
      delete (*it);
      m_guiTables.erase(it);
      return;
    }
  }
}
void CERModelTablesForFreeSelect::gui_AddFieldForClassTable(const xtstring& sTableName,
                                                            const CERModelFieldInd* pField)
{
  m_bInGUIChanged = true;
  for (CERModelTablesIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
  {
    if ((*it)->GetName() == sTableName)
    {
      (*it)->AddField((CERModelFieldInd*)pField);
      return;
    }
  }
}
void CERModelTablesForFreeSelect::gui_RemoveFieldFromClassTable(const xtstring& sTableName,
                                                                const xtstring& sFieldName)
{
  m_bInGUIChanged = true;
  for (CERModelTablesIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
  {
    if ((*it)->GetName() == sTableName)
    {
      (*it)->RemoveField(sFieldName);
      return;
    }
  }
}
bool CERModelTablesForFreeSelect::gui_FieldExistInAnyTable(const xtstring& sFieldName)
{
  for (CERModelTablesIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
  {
    xtstring sTableName = (*it)->GetName();
    const CERModelTableInd* pImport = SE_MF_GetERModelTableInd(sTableName);
    if (!pImport)
      continue;
    if (pImport->GetField(sFieldName))
      return true;
  }
  return false;
}
bool CERModelTablesForFreeSelect::gui_FieldToImportInAnyTable(const xtstring& sFieldName)
{
  for (CERModelTablesIterator it = m_guiTables.begin(); it != m_guiTables.end(); it++)
  {
    if ((*it)->GetField(sFieldName))
      return true;
  }
  return false;
}
*/









//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelTableInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CERModelTableInd::CERModelTableInd(CSyncERNotifiersGroup* pGroup)
                 :CModelBaseInd(tERModelTable),
                  CSyncNotifier_ER(tERNotifier_Table, pGroup),
                  CSyncResponse_Common(tRO_ERModel_Table),
                  m_cTablesForFreeSelect(pGroup)
{
  m_pChannelToGUI = 0;

  m_cTablesForFreeSelect.RemoveAllTables();
  m_nERTableType = tERTT_Default;
  m_sName.erase();
  m_sComment.erase();
  m_sSQLCommand.erase();
  m_sFreeSelectionData.erase();
  m_sDLLName.erase();
  m_sDLLInterface.erase();
  m_sXMLFile.erase();
  m_sXMLPath.erase();
  m_nFlags = 0;
  m_sPersistentData.erase();
  m_sImportTableDefFile.erase();
  m_sImportTableDefName.erase();
  m_arrRelations.erase(m_arrRelations.begin(), m_arrRelations.end());
  m_arrRelationsToChilds.erase(m_arrRelationsToChilds.begin(), m_arrRelationsToChilds.end());
  m_arrHierarchies.erase(m_arrHierarchies.begin(), m_arrHierarchies.end());
  m_arrHierarchiesToChilds.erase(m_arrHierarchiesToChilds.begin(), m_arrHierarchiesToChilds.end());
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

CERModelTableInd::CERModelTableInd(CERModelTableInd& cTable)
                 :CModelBaseInd(tERModelTable),
                  CSyncNotifier_ER(tERNotifier_Table, NULL),
                  CSyncResponse_Common(tRO_ERModel_Table),
                  m_cTablesForFreeSelect(NULL)
{
  m_pChannelToGUI = 0;

  SetSyncGroup(cTable.GetSyncGroup());
  m_cTablesForFreeSelect.SetSyncGroup(cTable.GetSyncGroup());
  SetERModelTableInd(&cTable);
}

CERModelTableInd::CERModelTableInd(CERModelTableInd* pTable)
                 :CModelBaseInd(tERModelTable),
                  CSyncNotifier_ER(tERNotifier_Table, NULL),
                  CSyncResponse_Common(tRO_ERModel_Table),
                  m_cTablesForFreeSelect(NULL)
{
  m_pChannelToGUI = 0;

  assert(pTable);
  if (!pTable)
    return;
  SetSyncGroup(pTable->GetSyncGroup());
  m_cTablesForFreeSelect.SetSyncGroup(pTable->GetSyncGroup());
  SetERModelTableInd(pTable);
}

CERModelTableInd::~CERModelTableInd()
{
  RemoveAllFields();
  RemoveAllParams();
  RemoveAllRelations();
  RemoveAllRelationsToChilds();
  RemoveAllHierarchies();
  RemoveAllHierarchiesToChilds();
}

void CERModelTableInd::PropertiesChanged()
{
  if (m_pChannelToGUI)
    m_pChannelToGUI->PropertiesChanged();
}



void CERModelTableInd::SetERModelTableInd(CERModelTableInd* pTable,
                                          bool bWithRelations /*= true*/)
{
  if (!pTable)
    return;

  m_cTablesForFreeSelect.AssignFrom(pTable->GetTablesForFreeSelect());
  m_nERTableType = pTable->m_nERTableType;
  m_sName = pTable->m_sName;
  m_sComment = pTable->m_sComment;
  m_sSQLCommand = pTable->m_sSQLCommand;
  m_sFreeSelectionData = pTable->m_sFreeSelectionData;
  m_sDLLName = pTable->m_sDLLName;
  m_sDLLInterface = pTable->m_sDLLInterface;
  m_sXMLFile = pTable->m_sXMLFile;
  m_sXMLPath = pTable->m_sXMLPath;
  m_nFlags = pTable->m_nFlags;
  m_sPersistentData = pTable->m_sPersistentData;
  m_sImportTableDefFile = pTable->m_sImportTableDefFile;
  m_sImportTableDefName = pTable->m_sImportTableDefName;
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

  RemoveAllFields();
  for (size_t nI = 0; nI < pTable->CountOfFields(); nI++)
  {
    AddField(pTable->GetField(nI));
  }
  RemoveAllParams();
  for (size_t nI = 0; nI < pTable->CountOfParams(); nI++)
  {
    AddParam(pTable->GetParam(nI));
  }
  if (bWithRelations)
  {
    RemoveAllRelations();
    for (size_t nI = 0; nI < pTable->CountOfRelations(); nI++)
    {
      AddRelation(pTable->GetRelation(nI));
    }
    RemoveAllRelationsToChilds();
    for (size_t nI = 0; nI < pTable->CountOfRelationsToChilds(); nI++)
    {
      AddRelationToChilds(pTable->GetRelationToChilds(nI));
    }
    RemoveAllHierarchies();
    for (size_t nI = 0; nI < pTable->CountOfHierarchies(); nI++)
    {
      AddHierarchy(pTable->GetHierarchy(nI));
    }
    RemoveAllHierarchiesToChilds();
    for (size_t nI = 0; nI < pTable->CountOfHierarchiesToChilds(); nI++)
    {
      AddHierarchyToChilds(pTable->GetHierarchyToChilds(nI));
    }
  }
}

xtstring CERModelTableInd::GetXMLDoc()
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

bool CERModelTableInd::SetXMLDoc(const xtstring& sXMLDoc)
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

bool CERModelTableInd::SaveXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  if (pXMLDoc->AddChildElem(NODE_ELEM_NAME, _XT("")))
  {
    pXMLDoc->IntoElem();
    // in <Node>
    pXMLDoc->SetAttrib(NODE_ATTRIB_OBJECTTYPE, GetERTableType());
    pXMLDoc->SetAttrib(NODE_ATTRIB_NAME, GetName().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_COMMENT, GetComment().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_SQLCOMMAND, GetSQLCommand().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_DLLNAME, GetDLLName().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_DLLINTERFACE, GetDLLInterface().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_XMLFILE, GetXMLFile().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_XMLPATH, GetXMLPath().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_FLAGS, GetFlags());
    pXMLDoc->SetAttrib(NODE_ATTRIB_PERSISTENTDATA, GetPersistentData().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_IMPORTTABLEDEFFILE, GetImportTableDefFile().c_str());
    pXMLDoc->SetAttrib(NODE_ATTRIB_IMPORTTABLEDEFNAME, GetImportTableDefName().c_str());
    pXMLDoc->AddChildElem(NODE_ATTRIB_FREESELECTIONDATA, GetFreeSelectionData().c_str());
    if (pXMLDoc->AddChildElem(FIELDS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Fields>
      for (size_t nI = 0; nI < CountOfFields(); nI++)
        GetField(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    if (pXMLDoc->AddChildElem(PARAMS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Params>
      for (size_t nI = 0; nI < CountOfParams(); nI++)
        GetParam(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    if (pXMLDoc->AddChildElem(RELATIONS_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Relations>
      for (size_t nI = 0; nI < CountOfRelations(); nI++)
        GetRelation(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    if (pXMLDoc->AddChildElem(RELATIONSCHILD_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Relations>
      for (size_t nI = 0; nI < CountOfRelationsToChilds(); nI++)
        GetRelationToChilds(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    if (pXMLDoc->AddChildElem(HIERARCHIES_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Hierarchies>
      for (size_t nI = 0; nI < CountOfHierarchies(); nI++)
        GetHierarchy(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
    if (pXMLDoc->AddChildElem(HIERARCHIESCHILD_ELEM_NAME, _XT("")))
    {
      pXMLDoc->IntoElem();
      // in <Relations>
      for (size_t nI = 0; nI < CountOfHierarchiesToChilds(); nI++)
        GetHierarchyToChilds(nI)->SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
      // in <Node>
    }
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
    // tables for free select
    if (pXMLDoc->AddChildElem(NODE_ELEM_TABLES_FOR_FREE_SELECT, _XT("")))
    {
      pXMLDoc->IntoElem();
      m_cTablesForFreeSelect.SaveXMLContent(pXMLDoc);
      pXMLDoc->OutOfElem();
    }

    pXMLDoc->OutOfElem();
    bRet = true;
  }

  return bRet;
}

bool CERModelTableInd::ReadXMLContent(CXPubMarkUp *pXMLDoc)
{
  bool bRet = false;
  assert(pXMLDoc);

  // in <Node>
  TERTableType nOT = (TERTableType)pXMLDoc->GetAttribLong(NODE_ATTRIB_OBJECTTYPE);
  if (nOT <= tERTT_FirstDummy || nOT >= tERTT_LastDummy)
    nOT = tERTT_Default;
  SetERTableType(nOT);
  SetName(pXMLDoc->GetAttrib(NODE_ATTRIB_NAME));
  SetComment(pXMLDoc->GetAttrib(NODE_ATTRIB_COMMENT));
  SetSQLCommand(pXMLDoc->GetAttrib(NODE_ATTRIB_SQLCOMMAND));
  SetDLLName(pXMLDoc->GetAttrib(NODE_ATTRIB_DLLNAME));
  SetDLLInterface(pXMLDoc->GetAttrib(NODE_ATTRIB_DLLINTERFACE));
  SetXMLFile(pXMLDoc->GetAttrib(NODE_ATTRIB_XMLFILE));
  SetXMLPath(pXMLDoc->GetAttrib(NODE_ATTRIB_XMLPATH));
  SetFlags(pXMLDoc->GetAttribLong(NODE_ATTRIB_FLAGS));
  SetPersistentData(pXMLDoc->GetAttrib(NODE_ATTRIB_PERSISTENTDATA));
  SetImportTableDefFile(pXMLDoc->GetAttrib(NODE_ATTRIB_IMPORTTABLEDEFFILE));
  SetImportTableDefName(pXMLDoc->GetAttrib(NODE_ATTRIB_IMPORTTABLEDEFNAME));
  if (pXMLDoc->FindChildElem(NODE_ATTRIB_FREESELECTIONDATA))
  {
    pXMLDoc->IntoElem();
    SetFreeSelectionData(pXMLDoc->GetData());
    pXMLDoc->OutOfElem();
  }
  // load fields
  RemoveAllFields();
  if (pXMLDoc->FindChildElem(FIELDS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Fields>
    while (pXMLDoc->FindChildElem(FIELD_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Field>

      CERModelFieldInd cField(GetSyncGroup());
      cField.ReadXMLContent(pXMLDoc);
      AddField(&cField);

      pXMLDoc->OutOfElem();
      // in <Fields>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
  // load params
  RemoveAllParams();
  if (pXMLDoc->FindChildElem(PARAMS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Params>
    while (pXMLDoc->FindChildElem(PARAM_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Param>

      CERModelParamInd cParam(GetSyncGroup());
      cParam.ReadXMLContent(pXMLDoc);
      AddParam(&cParam);

      pXMLDoc->OutOfElem();
      // in <Params>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
  // load relations to Parent
  RemoveAllRelations();
  if (pXMLDoc->FindChildElem(RELATIONS_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Relations>
    while (pXMLDoc->FindChildElem(RELATION_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Relation>

      CERModelRelationInd cRelation(true, GetSyncGroup());
      cRelation.ReadXMLContent(pXMLDoc);
      // In einem Zeitpunkt waren Projekte in solchem Stand,
      // dass die Relationen mehrfach in der Tabelle waren.
      // Von diesem Grund werden wir eine Relation nur dann einfuegen,
      // wenn Relation mit dieser Name noch nicht drin ist.
      // Spaeter war definiert, dass es nicht drin bleiben soll.
//      if (!GetRelation(cRelation.GetName()))
      AddRelation(&cRelation);

      pXMLDoc->OutOfElem();
      // in <Relations>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
  // load relations to Child
  RemoveAllRelationsToChilds();
  if (pXMLDoc->FindChildElem(RELATIONSCHILD_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <RelationsToChild>
    while (pXMLDoc->FindChildElem(RELATION_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Relation>

      CERModelRelationInd cRelation(false, GetSyncGroup());
      cRelation.ReadXMLContent(pXMLDoc);
      AddRelationToChilds(&cRelation);

      pXMLDoc->OutOfElem();
      // in <RelationsToChild>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
  // load hierarchies to Parent
  RemoveAllHierarchies();
  if (pXMLDoc->FindChildElem(HIERARCHIES_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <Hierarchies>
    while (pXMLDoc->FindChildElem(RELATION_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Hierarchy>

      CERModelRelationInd cRelation(true, GetSyncGroup());
      cRelation.ReadXMLContent(pXMLDoc);
      AddHierarchy(&cRelation);

      pXMLDoc->OutOfElem();
      // in <Hierarchies>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
  }
  // load hierarchies to Child
  RemoveAllHierarchiesToChilds();
  if (pXMLDoc->FindChildElem(HIERARCHIESCHILD_ELEM_NAME))
  {
    pXMLDoc->IntoElem();
    // in <HierarchiesToChild>
    while (pXMLDoc->FindChildElem(RELATION_ELEM_NAME))
    {
      pXMLDoc->IntoElem();
      // in <Hierarchy>

      CERModelRelationInd cRelation(false, GetSyncGroup());
      cRelation.ReadXMLContent(pXMLDoc);
      AddHierarchyToChilds(&cRelation);

      pXMLDoc->OutOfElem();
      // in <HierarchiesToChild>
    }
    pXMLDoc->OutOfElem();
    // in <Node>
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
    SetSynchronize(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_SYNC) != false);
    SetColor(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR));
    SetColorBack(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_BACK));
    SetColorHdr(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_HDR));
    SetColorHdrSel(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_HDR_SEL));
    SetColorHdrText(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_HDR_TEXT));
    SetColorHdrSelText(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_HDR_SEL_TEXT));
    SetColorSelItem(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_COLOR_SEL_ITEM));
    SetMoveable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_MOVEABLE) != false);
    SetSelectable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_SELECTABLE) != false);
    SetVisible(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_VISIBLE) != false);
    SetSizeable(pXMLDoc->GetAttribLong(NODE_ATTRIB_UI_SIZEABLE) != false);
    
    pXMLDoc->OutOfElem();
    // in <Node>
    bRet = true;
  }
  // tables for free select
  if (pXMLDoc->FindChildElem(NODE_ELEM_TABLES_FOR_FREE_SELECT))
  {
    pXMLDoc->IntoElem();
    m_cTablesForFreeSelect.ReadXMLContent(pXMLDoc);
    pXMLDoc->OutOfElem();
  }

  return bRet;
}

void CERModelTableInd::CopyDiverseProperties(CERModelTableInd* pTable)
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

void CERModelTableInd::CopyPositionProperties(CERModelTableInd* pTable)
{
  m_nPosLeft = pTable->m_nPosLeft;
  m_nPosTop = pTable->m_nPosTop;
  m_nSizeWidth = pTable->m_nSizeWidth;
  m_nSizeHeight = pTable->m_nSizeHeight;
}

bool CERModelTableInd::FieldIsInRelation(const xtstring& sFieldName)
{
  for (size_t nJ = 0; nJ < CountOfRelations(); nJ++)
  {
    CERModelRelationInd* pRel = GetRelation(nJ);
    if (!pRel)
      // so was kann aber eigentlich nicht vorkommen
      continue;

    xtstring sParentField, sChildField;
    for (size_t nK = 0; nK < pRel->CountOfFields(); nK++)
    {
      pRel->GetFields(nK, sParentField, sChildField);
      if (sChildField == sFieldName)
        return true;
    }
  }
  for (size_t nJ = 0; nJ < CountOfRelationsToChilds(); nJ++)
  {
    CERModelRelationInd* pRel = GetRelationToChilds(nJ);
    if (!pRel)
      // so was kann aber eigentlich nicht vorkommen
      continue;

    xtstring sParentField, sChildField;
    for (size_t nK = 0; nK < pRel->CountOfFields(); nK++)
    {
      pRel->GetFields(nK, sParentField, sChildField);
      if (sParentField == sFieldName)
        return true;
    }
  }
  return false;
}

CERModelFieldInd* CERModelTableInd::GetField(size_t nIndex) const
{
  assert(nIndex < m_arrFields.size());
  if (nIndex < m_arrFields.size())
    return m_arrFields[nIndex];
  return 0;
}

const CERModelFieldInd* CERModelTableInd::GetFieldConst(size_t nIndex) const
{
  return GetField(nIndex);
}

CERModelFieldInd* CERModelTableInd::GetField(const xtstring& sName) const
{
  /*
  CERModelFieldInd* pRetField = NULL;
  for (size_t nI = 0; nI < m_arrFields.size(); nI++)
  {
    CERModelFieldInd* pField = m_arrFields[nI];
    if (pField->GetName() == sName)
    {
      pRetField = pField;
      break;
    }
  }
  return pRetField;
  */
  CERModelFieldInd* pRetField = 0;
  CERModelFieldsMapConstIterator it = m_mapFields.find(sName);
  if (it != m_mapFields.end())
    pRetField = it->second;
  return pRetField;
}

const CERModelFieldInd* CERModelTableInd::GetFieldConst(const xtstring& sName) const
{
  return GetField(sName);
}

void CERModelTableInd::AddField(CERModelFieldInd *pField)
{
  assert(pField);
  if (!pField)
    return;
  CERModelFieldInd *ppField;
  ppField = new CERModelFieldInd(pField);
  ppField->SetSyncGroup(GetSyncGroup());
  ppField->SetERTableInd(this);
  m_arrFields.push_back(ppField);
  m_mapFields[pField->GetName()] = ppField;
}

bool CERModelTableInd::RemoveField(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrFields.size());
  if (nIndex >= 0 && nIndex < m_arrFields.size())
  {
    CERModelFieldInd* pField = m_arrFields[nIndex];
    {
      // map
      CERModelFieldsMapIterator it = m_mapFields.find(pField->GetName());
      if (it != m_mapFields.end())
        m_mapFields.erase(it);
    }
    delete pField;
    m_arrFields.erase(m_arrFields.begin() + nIndex);
    return true;
  }
  return false;
}

bool CERModelTableInd::RemoveField(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfFields(); nI++)
  {
    CERModelFieldInd* pField = GetField(nI);
    if (sName == pField->GetName())
      return RemoveField(nI);
  }
  return false;
}

bool CERModelTableInd::RemoveAllFields()
{
  while (m_arrFields.size())
    RemoveField(0);
  m_arrFields.erase(m_arrFields.begin(), m_arrFields.end());
  return true;
}

void CERModelTableInd::GetAllFieldNames(CxtstringVector& arrFieldNames)
{
  arrFieldNames.erase(arrFieldNames.begin(), arrFieldNames.end());
  for (size_t nI = 0; nI < m_arrFields.size(); nI++)
    arrFieldNames.push_back(m_arrFields[nI]->GetName());
}

CERModelParamInd* CERModelTableInd::GetParam(size_t nIndex)
{
  assert(nIndex < m_arrParams.size());
  if (nIndex < m_arrParams.size())
    return m_arrParams[nIndex];
  return 0;
}
CERModelParamInd* CERModelTableInd::GetParam(const xtstring& sName)
{
  CERModelParamInd* pRetParam = NULL;
  for (size_t nI = 0; nI < m_arrParams.size(); nI++)
  {
    CERModelParamInd* pParam = m_arrParams[nI];
    if (pParam->GetName() == sName)
    {
      pRetParam = pParam;
      break;
    }
  }
  return pRetParam;
}

void CERModelTableInd::AddParam(CERModelParamInd *pParam)
{
  assert(pParam);
  if (!pParam)
    return;
  CERModelParamInd *ppParam;
  ppParam = new CERModelParamInd(pParam);
  ppParam->SetSyncGroup(GetSyncGroup());
  ppParam->SetERTableInd(this);
  m_arrParams.push_back(ppParam);
}

bool CERModelTableInd::RemoveParam(size_t nIndex)
{
  assert(nIndex >= 0 && nIndex < m_arrParams.size());
  if (nIndex >= 0 && nIndex < m_arrParams.size())
  {
    CERModelParamInd* pParam = m_arrParams[nIndex];
    delete pParam;
    m_arrParams.erase(m_arrParams.begin() + nIndex);
    return true;
  }
  return false;
}

bool CERModelTableInd::RemoveParam(const xtstring& sName)
{
  for (size_t nI = 0; nI < CountOfParams(); nI++)
  {
    CERModelParamInd* pParam = GetParam(nI);
    if (sName == pParam->GetName())
      return RemoveParam(nI);
  }
  return false;
}

bool CERModelTableInd::RemoveAllParams()
{
  while (m_arrParams.size())
    RemoveParam(0);
  m_arrParams.erase(m_arrParams.begin(), m_arrParams.end());
  return true;
}

void CERModelTableInd::GetAllParamNames(CxtstringVector& arrParamNames)
{
  arrParamNames.erase(arrParamNames.begin(), arrParamNames.end());
  for (size_t nI = 0; nI < m_arrParams.size(); nI++)
    arrParamNames.push_back(m_arrParams[nI]->GetName());
}

CERModelRelationInd* CERModelTableInd::GetRelation(const xtstring& sName)
{
  CERModelRelationInd* pRetRel = NULL;
  for (size_t nI = 0; nI < m_arrRelations.size(); nI++)
  {
    CERModelRelationInd* pRel = m_arrRelations[nI];
    if (pRel->GetName() == sName)
    {
      pRetRel = pRel;
      break;
    }
  }
  return pRetRel;
}

CERModelRelationInd* CERModelTableInd::GetRelationToChilds(const xtstring& sName,
                                                           xtstring sChildTable)
{
  CERModelRelationInd* pRetRel = NULL;
  for (size_t nI = 0; nI < m_arrRelationsToChilds.size(); nI++)
  {
    CERModelRelationInd* pRel = m_arrRelationsToChilds[nI];
    if (pRel->GetName() == sName
      && pRel->GetChildTable() == sChildTable)
    {
      pRetRel = pRel;
      break;
    }
  }
  return pRetRel;
}

CERModelRelationInd* CERModelTableInd::GetRelationToChilds(const xtstring& sName)
{
  CERModelRelationInd* pRetRel = NULL;
  for (size_t nI = 0; nI < m_arrRelationsToChilds.size(); nI++)
  {
    CERModelRelationInd* pRel = m_arrRelationsToChilds[nI];
    if (pRel->GetName() == sName)
    {
      pRetRel = pRel;
      break;
    }
  }
  return pRetRel;
}

bool CERModelTableInd::RemoveRelation(CERModelRelationInd* pRelation)
{
  for (size_t nI = 0; nI < m_arrRelations.size(); nI++)
  {
    if (*(m_arrRelations[nI]) == *pRelation)
    {
      CERModelRelationInd* pRel = m_arrRelations[nI];
      delete pRel;
      m_arrRelations.erase(m_arrRelations.begin() + nI);
      return true;
    }
  }
  return false;
}

bool CERModelTableInd::RemoveAllRelations()
{
  while (m_arrRelations.size())
    RemoveRelation(m_arrRelations[0]);
  m_arrRelations.erase(m_arrRelations.begin(), m_arrRelations.end());
  return true;
}

bool CERModelTableInd::RemoveRelationToChilds(CERModelRelationInd* pRelation)
{
  for (size_t nI = 0; nI < m_arrRelationsToChilds.size(); nI++)
  {
    if (*(m_arrRelationsToChilds[nI]) == *pRelation)
    {
      CERModelRelationInd* pRel = m_arrRelationsToChilds[nI];
      delete pRel;
      m_arrRelationsToChilds.erase(m_arrRelationsToChilds.begin() + nI);
      return true;
    }
  }
  return false;
}

bool CERModelTableInd::RemoveAllRelationsToChilds()
{
  while (m_arrRelationsToChilds.size())
    RemoveRelationToChilds(m_arrRelationsToChilds[0]);
  m_arrRelationsToChilds.erase(m_arrRelationsToChilds.begin(), m_arrRelationsToChilds.end());
  return true;
}

CERModelRelationInd* CERModelTableInd::GetHierarchy(const xtstring& sName)
{
  CERModelRelationInd* pRetRel = NULL;
  for (size_t nI = 0; nI < m_arrHierarchies.size(); nI++)
  {
    CERModelRelationInd* pRel = m_arrHierarchies[nI];
    if (pRel->GetName() == sName)
    {
      pRetRel = pRel;
      break;
    }
  }
  return pRetRel;
}

CERModelRelationInd* CERModelTableInd::GetHierarchyToChilds(const xtstring& sName,
                                                            xtstring sChildTable)
{
  CERModelRelationInd* pRetRel = NULL;
  for (size_t nI = 0; nI < m_arrHierarchiesToChilds.size(); nI++)
  {
    CERModelRelationInd* pRel = m_arrHierarchiesToChilds[nI];
    if (pRel->GetName() == sName
      && pRel->GetChildTable() == sChildTable)
    {
      pRetRel = pRel;
      break;
    }
  }
  return pRetRel;
}

CERModelRelationInd* CERModelTableInd::GetHierarchyToChilds(const xtstring& sName)
{
  CERModelRelationInd* pRetRel = NULL;
  for (size_t nI = 0; nI < m_arrHierarchiesToChilds.size(); nI++)
  {
    CERModelRelationInd* pRel = m_arrHierarchiesToChilds[nI];
    if (pRel->GetName() == sName)
    {
      pRetRel = pRel;
      break;
    }
  }
  return pRetRel;
}

bool CERModelTableInd::RemoveHierarchy(CERModelRelationInd* pRelation)
{
  for (size_t nI = 0; nI < m_arrHierarchies.size(); nI++)
  {
    if (*(m_arrHierarchies[nI]) == *pRelation)
    {
      CERModelRelationInd* pRel = m_arrHierarchies[nI];
      delete pRel;
      m_arrHierarchies.erase(m_arrHierarchies.begin() + nI);
      return true;
    }
  }
  return false;
}

bool CERModelTableInd::RemoveAllHierarchies()
{
  while (m_arrHierarchies.size())
    RemoveHierarchy(m_arrHierarchies[0]);
  m_arrHierarchies.erase(m_arrHierarchies.begin(), m_arrHierarchies.end());
  return true;
}

bool CERModelTableInd::RemoveHierarchyToChilds(CERModelRelationInd* pRelation)
{
  for (size_t nI = 0; nI < m_arrHierarchiesToChilds.size(); nI++)
  {
    if (*(m_arrHierarchiesToChilds[nI]) == *pRelation)
    {
      CERModelRelationInd* pRel = m_arrHierarchiesToChilds[nI];
      delete pRel;
      m_arrHierarchiesToChilds.erase(m_arrHierarchiesToChilds.begin() + nI);
      return true;
    }
  }
  return false;
}

bool CERModelTableInd::RemoveAllHierarchiesToChilds()
{
  while (m_arrHierarchiesToChilds.size())
    RemoveHierarchyToChilds(m_arrHierarchiesToChilds[0]);
  m_arrHierarchiesToChilds.erase(m_arrHierarchiesToChilds.begin(), m_arrHierarchiesToChilds.end());
  return true;
}

void CERModelTableInd::RenameTable(const xtstring& sOldName,
                                   const xtstring& sNewName)
{
  if (GetName() == sOldName)
    SetName(sNewName);

  for (CERModelRelationsIterator it = m_arrRelations.begin(); it != m_arrRelations.end(); it++)
    (*it)->RenameTable(sOldName, sNewName);

  for (CERModelRelationsIterator it = m_arrRelationsToChilds.begin(); it != m_arrRelationsToChilds.end(); it++)
    (*it)->RenameTable(sOldName, sNewName);

  for (CERModelRelationsIterator it = m_arrHierarchies.begin(); it != m_arrHierarchies.end(); it++)
    (*it)->RenameTable(sOldName, sNewName);

  for (CERModelRelationsIterator it = m_arrHierarchiesToChilds.begin(); it != m_arrHierarchiesToChilds.end(); it++)
    (*it)->RenameTable(sOldName, sNewName);
}

bool CERModelTableInd::FieldAddable()
{
  if (GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_TABLE_FIELDS_NOTADDABLE)
    return false;
  return true;
}

bool CERModelTableInd::FieldRemovable()
{
  if (GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_TABLE_FIELDS_NOTREMOVABLE)
    return false;
  return true;
}

bool CERModelTableInd::ParamAddable()
{
  if (GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_TABLE_PARAMS_NOTADDABLE)
    return false;
  return true;
}

bool CERModelTableInd::ParamRemovable()
{
  if (GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_TABLE_PARAMS_NOTREMOVABLE)
    return false;
  return true;
}

bool CERModelTableInd::RelationAddable()
{
  if (GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_TABLE_RELATIONS_NOTADDABLE)
    return false;
  return true;
}

bool CERModelTableInd::RelationRemovable()
{
  if (GetERTableType() == CERModelTableInd::tERTT_DLLTable
      && GetFlags() & DLL_TABLE_RELATIONS_NOTREMOVABLE)
    return false;
  return true;
}

void CERModelTableInd::SE_OnFieldNameChanged(const xtstring& sTableName,
                                             const xtstring& sOldName,
                                             const xtstring& sNewName)
{
  if (sTableName == GetName())
  {
    CERModelFieldsMapIterator it = m_mapFields.find(sOldName);
    if (it != m_mapFields.end())
    {
      m_mapFields.erase(it);
      for (size_t nI = 0; nI < m_arrFields.size(); nI++)
      {
        CERModelFieldInd* pField = m_arrFields[nI];
        // wir muessen mit alte name vergleichen
        if (pField->GetName() == sOldName)
        {
          m_mapFields[sNewName] = pField;
          break;
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////
// synchronization
//////////////////////////////////////////////////////////////////////////////////

void CERModelTableInd::SE_OnSyncFontChanged(const xtstring& sFontName,
                                            int nFontHeight)
{
  if (!GetSynchronize())
    return;
  SetFontName(sFontName);
  SetFontHeight(nFontHeight);
}

void CERModelTableInd::SE_OnSyncColorChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize())
    return;
  SetColor(nColor);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncColorBackChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize())
    return;
  SetColorBack(nColor);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize())
    return;
  SetColorHdr(nColor);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize())
    return;
  SetColorHdrSel(nColor);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize())
    return;
  SetColorHdrText(nColor);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize())
    return;
  SetColorHdrSelText(nColor);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor)
{
  if (!GetSynchronize())
    return;
  SetColorSelItem(nColor);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncMoveableChanged(bool bVal)
{
  if (!GetSynchronize())
    return;
  SetMoveable(bVal);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncSelectableChanged(bool bVal)
{
  if (!GetSynchronize())
    return;
  SetSelectable(bVal);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncVisibleChanged(bool bVal)
{
  if (!GetSynchronize())
    return;
  SetVisible(bVal);
  PropertiesChanged();
}

void CERModelTableInd::SE_OnSyncSizeableChanged(bool bVal)
{
  if (!GetSynchronize())
    return;
  SetSizeable(bVal);
  PropertiesChanged();
}

