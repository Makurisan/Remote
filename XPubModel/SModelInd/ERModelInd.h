// ERModelInd.h: interface for the CERModelXXXInd classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ERMODELIND_H_)
#define _ERMODELIND_H_


#include "ModelExpImp.h"

#include "SSyncEng/SyncEngineBase.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32




typedef enum TFieldAttributes
{
  tFieldAttrNone = 0,
  tFieldAttrFixed = 1,
  tFieldAttrNullable = 2
};

typedef enum TRelationType
{
  tRelation_11,
  tRelation_1N,
  tRelation_NN,
  tRelation_Hierarchy_1N
};

typedef enum TRelationRule
{
  tRelationRuleRINone = 0,
  tRelationRuleRICascade = 1,
  tRelationRuleRISetNull = 2,
  tRelationRuleRISetDefault = 3
};


// names for XML automatic fields and relation
#define XML_FIELD_NODE_IN_PARENT    _XT("NodeID_Parent")
#define XML_FIELD_NODE_IN_CHILD     _XT("NodeID_Child")
#define XML_FIELD_NODE_ATTRIBUTES   tFieldAttrNone
#define XML_FIELD_NODE_DEFINEDSIZE  0
#define XML_FIELD_NODE_TYPE         tFieldTypeInteger
#define XML_RELATION_NODE           _XT("XMLNodesRelation")


// PARAM FLAGS
#define DLL_PARAM_ALWAYS_SELECTED         0x0001
#define DLL_PARAM_PRESELECT               0x0002
#define DLL_PARAM_NOTREMOVABLE            0x0004
#define DLL_PARAM_NAME_NOTCHANGEABLE      0x0010
#define DLL_PARAM_TYPE_NOTCHANGEABLE      0x0020
#define DLL_PARAM_LENGTH_NOTCHANGEABLE    0x0040
#define DLL_PARAM_NOTFORSQL               0x0080
// FIELD FLAGS
#define DLL_FIELD_ALWAYS_SELECTED         0x0001
#define DLL_FIELD_PRESELECT               0x0002
#define DLL_FIELD_NOTREMOVABLE            0x0004
#define DLL_FIELD_NAME_NOTCHANGEABLE      0x0010
#define DLL_FIELD_TYPE_NOTCHANGEABLE      0x0020
#define DLL_FIELD_LENGTH_NOTCHANGEABLE    0x0040
// RELATION FLAGS
#define DLL_RELATION_NOTREMOVABLE         0x0001
#define DLL_RELATION_NAME_NOTCHANGEABLE   0x0010
#define DLL_RELATION_TYPE_NOTCHANGEABLE   0x0020
#define DLL_RELATION_RULES_NOTCHANGEABLE  0x0040
#define DLL_RELATION_FIELDS_NOTCHANGEABLE 0x0080
// TABLE FLAGS
#define DLL_TABLE_SQLQUERY                0x0001
#define DLL_TABLE_PARAMS_NOTADDABLE       0x0010
#define DLL_TABLE_PARAMS_NOTREMOVABLE     0x0020
#define DLL_TABLE_FIELDS_NOTADDABLE       0x0040
#define DLL_TABLE_FIELDS_NOTREMOVABLE     0x0080
#define DLL_TABLE_RELATIONS_NOTADDABLE    0x0100
#define DLL_TABLE_RELATIONS_NOTREMOVABLE  0x0200






class CXPubMarkUp;



class CERModelFieldInd;
class CERModelParamInd;
class CERModelTableInd;
class CERModelRelationInd;





//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelFieldInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CERModelFieldInd : public CModelBaseInd,
                                                public CSyncNotifier_ER,
                                                public CSyncResponse_Common
{
  friend class CERModelTableInd;
public:
  CERModelFieldInd(CSyncERNotifiersGroup* pGroup);
  CERModelFieldInd(CERModelFieldInd& cField);
  CERModelFieldInd(CERModelFieldInd* pField);
  virtual ~CERModelFieldInd();

  void SetERModelFieldInd(CERModelFieldInd* pField);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetNameForShow() const;
  xtstring GetName() const {return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
  TFieldAttributes GetAttributes(){return m_nAttributes;};
  void SetAttributes(TFieldAttributes nAttributes){m_nAttributes = nAttributes;};
  long GetDefinedSize(){return m_nDefinedSize;};
  void SetDefinedSize(long nDefinedSize){m_nDefinedSize = nDefinedSize;};
  TFieldType GetType(){return m_nType;};
  void SetType(TFieldType nType){m_nType = nType;};
  int GetFlags(){return m_nFlags;};
  void SetFlags(int nFlags){m_nFlags = nFlags;};
  xtstring GetPersistentData(){return m_sPersistentData;};
  void SetPersistentData(const xtstring& sPersistentData){m_sPersistentData = sPersistentData;};

  xtstring GetImportTableDefStructName(){return m_sImportTableDefStructName;};
  void SetImportTableDefStructName(const xtstring& sImportTableDefStructName){m_sImportTableDefStructName = sImportTableDefStructName;};
  int GetImportTableDefStructFieldIndex(){return m_nImportTableDefStructFieldIndex;};
  void SetImportTableDefStructFieldIndex(int nImportTableDefStructFieldIndex){m_nImportTableDefStructFieldIndex = nImportTableDefStructFieldIndex;};

  static bool SizeChangeableFromType(TFieldType nType);
  static bool TypeNeedApo(TFieldType nType);
  static bool TypeNeedHash(TFieldType nType);
  static xtstring GetFieldTypeName(TFieldType nType);
  static xtstring GetFormatedFieldType(TFieldType nType, long nDefinedSize);
  xtstring GetFormatedFieldType();

  xtstring GetERTableName();
  bool FieldRemovable();
private:
  // internal
  void SetERTableInd(CERModelTableInd* pERTable){m_pERTable = pERTable;};
  CERModelTableInd* m_pERTable;
  // data
  xtstring          m_sName;
  xtstring          m_sComment;
  TFieldAttributes  m_nAttributes;
  long              m_nDefinedSize;
  TFieldType        m_nType;
  xtstring          m_sImportTableDefStructName;
  int               m_nImportTableDefStructFieldIndex;
  int               m_nFlags;
  xtstring          m_sPersistentData;
protected:
  // non stream data -->
  void* m_pPropName;
  void* m_pPropComment;
  void* m_pPropType;
  void* m_pPropSize;
  void* m_pPropAttributes;
  // non stream data <--

protected:
  virtual bool SE_TableExist(const xtstring& sTableName){return false;};
  virtual bool SE_FieldExist(const xtstring& sFieldName){return (sFieldName == GetName());};
  virtual bool SE_EntityExist(const xtstring& sEntityName){return false;};
  virtual bool SE_VariableExist(const xtstring& sVariableName){return false;};

  virtual void SE_OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnTableDelete(const xtstring& sName){};
  virtual void SE_OnFieldNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnParamNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual bool SE_OnRelationNameWillBeChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){return true;};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual bool SE_GetShowRelationNamesER(){return true;};
  virtual xtstring SE_GetDBDefinitionName_Production(){return _XT("");};
  virtual xtstring SE_GetDBDefinitionName_Test(){return _XT("");};
  virtual const CERModelTableInd* SE_GetERModelTableInd(){return 0;};

  virtual void SE_OnSyncFontChanged(const xtstring& sFontName, int nFontHeight){};
  virtual void SE_OnSyncColorChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorBackChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncMoveableChanged(bool bVal){};
  virtual void SE_OnSyncSelectableChanged(bool bVal){};
  virtual void SE_OnSyncVisibleChanged(bool bVal){};
  virtual void SE_OnSyncSizeableChanged(bool bVal){};
};



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelParamInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CERModelParamInd : public CModelBaseInd,
                                                public CSyncNotifier_ER,
                                                public CSyncResponse_Common
{
  friend class CERModelTableInd;
public:
  CERModelParamInd(CSyncERNotifiersGroup* pGroup);
  CERModelParamInd(CERModelParamInd& cParam);
  CERModelParamInd(CERModelParamInd* pParam);
  virtual ~CERModelParamInd();

  void SetERModelParamInd(CERModelParamInd* pParam);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetNameForShow() const;
  xtstring GetName() const {return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
  xtstring GetDefaultValue(){return m_sDefaultValue;};
  void SetDefaultValue(const xtstring& sDefaultValue){m_sDefaultValue = sDefaultValue;};
  long GetDefinedSize(){return m_nDefinedSize;};
  void SetDefinedSize(long nDefinedSize){m_nDefinedSize = nDefinedSize;};
  TFieldType GetType(){return m_nType;};
  void SetType(TFieldType nType){m_nType = nType;};
  int GetFlags(){return m_nFlags;};
  void SetFlags(int nFlags){m_nFlags = nFlags;};
  xtstring GetPersistentData(){return m_sPersistentData;};
  void SetPersistentData(const xtstring& sPersistentData){m_sPersistentData = sPersistentData;};

  xtstring GetERTableName();
  bool ParamRemovable();
private:
  // internal
  void SetERTableInd(CERModelTableInd* pERTable){m_pERTable = pERTable;};
  CERModelTableInd* m_pERTable;
  // data
  xtstring          m_sName;
  xtstring          m_sComment;
  xtstring          m_sDefaultValue;
  long              m_nDefinedSize;
  TFieldType        m_nType;
  int               m_nFlags;
  xtstring          m_sPersistentData;

protected:
  virtual bool SE_TableExist(const xtstring& sTableName){return false;};
  virtual bool SE_FieldExist(const xtstring& sFieldName){return false;};
  virtual bool SE_EntityExist(const xtstring& sEntityName){return false;};
  virtual bool SE_VariableExist(const xtstring& sVariableName){return false;};

  virtual void SE_OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnTableDelete(const xtstring& sName){};
  virtual void SE_OnFieldNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnParamNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual bool SE_OnRelationNameWillBeChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){return true;};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual bool SE_GetShowRelationNamesER(){return true;};
  virtual xtstring SE_GetDBDefinitionName_Production(){return _XT("");};
  virtual xtstring SE_GetDBDefinitionName_Test(){return _XT("");};
  virtual const CERModelTableInd* SE_GetERModelTableInd(){return 0;};

  virtual void SE_OnSyncFontChanged(const xtstring& sFontName, int nFontHeight){};
  virtual void SE_OnSyncColorChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorBackChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncMoveableChanged(bool bVal){};
  virtual void SE_OnSyncSelectableChanged(bool bVal){};
  virtual void SE_OnSyncVisibleChanged(bool bVal){};
  virtual void SE_OnSyncSizeableChanged(bool bVal){};
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelRelationInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CERModelRelationInd : public CModelBaseInd,
                                                   public CSyncNotifier_ER
{
  friend class CERModelTableInd;
public:
  CERModelRelationInd(bool bRelationIsInChildTable, CSyncERNotifiersGroup* pGroup);
  CERModelRelationInd(CERModelRelationInd& cRelation);
  CERModelRelationInd(CERModelRelationInd* pRelation);
  virtual ~CERModelRelationInd();
  void operator =(CERModelRelationInd& cRelation);
  bool operator ==(CERModelRelationInd& cRelation);
  bool CompareRelation(CERModelRelationInd& cRelation);

  void SetERModelRelationInd(const CERModelRelationInd* pRelation);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  void SetType(TRelationType nType){m_nType = nType;};
  TRelationType GetType() const {return m_nType;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetName() const {return m_sName;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
  xtstring GetComment() const {return m_sComment;};
  void SetParentTable(const xtstring& sTable){m_sParentTable = sTable;}
  xtstring GetParentTable() const {return m_sParentTable;};
  void SetChildTable(const xtstring& sTable){m_sChildTable = sTable;};
  xtstring GetChildTable() const {return m_sChildTable;};
  void SetParentUpdateRule(TRelationRule nRule){m_nParentUpdateRule = nRule;};
  TRelationRule GetParentUpdateRule() const {return m_nParentUpdateRule;};
  void SetParentDeleteRule(TRelationRule nRule){m_nParentDeleteRule = nRule;};
  TRelationRule GetParentDeleteRule() const {return m_nParentDeleteRule;};
  void SetChildUpdateRule(TRelationRule nRule){m_nChildUpdateRule = nRule;};
  TRelationRule GetChildUpdateRule() const {return m_nChildUpdateRule;};
  void SetChildDeleteRule(TRelationRule nRule){m_nChildDeleteRule = nRule;};
  TRelationRule GetChildDeleteRule() const {return m_nChildDeleteRule;};
  int GetFlags() const {return m_nFlags;};
  void SetFlags(int nFlags){m_nFlags = nFlags;};
  xtstring GetPersistentData() const {return m_sPersistentData;};
  void SetPersistentData(const xtstring& sPersistentData){m_sPersistentData = sPersistentData;};

  size_t CountOfFields() const;
  void RemoveAllFields();
  void GetFields(size_t nIndex, xtstring& sParentField, xtstring& sChildField) const;
  void AddFields(const xtstring& sParentField, const xtstring& sChildField);
  xtstring GetParentFieldsDelimited(bool bForProperty) const;
  xtstring GetChildFieldsDelimited(bool bForProperty) const;
  void ParseAndAddDelimitedFields(const xtstring sParentFields, const xtstring sChildFields);

  // spezielle Funktion, die in AddOn benutzt ist
  void RenameTable(const xtstring& sOldName, const xtstring& sNewName);

  xtstring GetERTableName();
  bool RelationRemovable();
private:
  // internal
  void SetERTableInd(CERModelTableInd* pERTable){m_pERTable = pERTable;};
  CERModelTableInd* m_pERTable;
  // non stream data -->
  bool m_bRelationIsInChildTable;
  // non stream data <--
  // data
  TRelationType   m_nType;
  xtstring        m_sName;
  xtstring        m_sComment;
  xtstring        m_sParentTable;
  xtstring        m_sChildTable;
  TRelationRule   m_nParentUpdateRule;
  TRelationRule   m_nParentDeleteRule;
  TRelationRule   m_nChildUpdateRule;
  TRelationRule   m_nChildDeleteRule;
  CxtstringVector m_arrParentFields;
  CxtstringVector m_arrChildFields;
  int             m_nFlags;
  xtstring        m_sPersistentData;

protected:
  virtual void SE_OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName);
  virtual void SE_OnTableDelete(const xtstring& sName){};
  virtual void SE_OnFieldNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName);
  virtual void SE_OnParamNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName);
  virtual bool SE_OnRelationNameWillBeChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName);

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual bool SE_GetShowRelationNamesER(){return true;};
  virtual xtstring SE_GetDBDefinitionName_Production(){return _XT("");};
  virtual xtstring SE_GetDBDefinitionName_Test(){return _XT("");};
  virtual const CERModelTableInd* SE_GetERModelTableInd(){return 0;};

  virtual void SE_OnSyncFontChanged(const xtstring& sFontName, int nFontHeight){};
  virtual void SE_OnSyncColorChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorBackChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncMoveableChanged(bool bVal){};
  virtual void SE_OnSyncSelectableChanged(bool bVal){};
  virtual void SE_OnSyncVisibleChanged(bool bVal){};
  virtual void SE_OnSyncSizeableChanged(bool bVal){};
};





typedef std::map<xtstring, CERModelFieldInd*>   CERModelFieldsMap;
typedef CERModelFieldsMap::iterator             CERModelFieldsMapIterator;
typedef CERModelFieldsMap::const_iterator       CERModelFieldsMapConstIterator;
typedef CERModelFieldsMap::reverse_iterator     CERModelFieldsMapReverseIterator;

typedef std::vector<CERModelFieldInd*>    CERModelFields;
typedef CERModelFields::iterator          CERModelFieldsIterator;
typedef CERModelFields::reverse_iterator  CERModelFieldsReverseIterator;

typedef std::vector<CERModelParamInd*>    CERModelParams;
typedef CERModelParams::iterator          CERModelParamsIterator;
typedef CERModelParams::reverse_iterator  CERModelParamsReverseIterator;

typedef std::vector<CERModelRelationInd*>   CERModelRelations;
typedef CERModelRelations::iterator         CERModelRelationsIterator;
typedef CERModelRelations::reverse_iterator CERModelRelationsReverseIterator;


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERFreeSelectTables
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CERFreeSelectTables : public CModelBaseInd,
                                                   public CSyncNotifier_ER
{
public:
  CERFreeSelectTables(CSyncERNotifiersGroup* pGroup);
  virtual ~CERFreeSelectTables();

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  bool AssignFrom(const CERFreeSelectTables* pTables);
//  bool CopyFrom(const CERFreeSelectTables* pTables, bool& bChanged);
/*
  // gui fx -->
  void gui_SetParentTable(const xtstring& sParentTableName){m_sParentTableName = sParentTableName;};
  bool gui_InGUIChanged(){return m_bInGUIChanged;};
  void gui_TransportTables();
  void gui_Close();
  size_t gui_CountOfTables(){return m_guiTables.size();};
  CERModelTableInd* gui_GetTable(const xtstring& sTableName);
  CERModelTableInd* gui_GetTable_const(const xtstring& sTableName) const;
  CERModelTableInd* gui_GetTable(size_t nIndex){if (nIndex < m_guiTables.size())return m_guiTables[nIndex];return 0;};
  void gui_BuildClassForTable(const xtstring& sTableName, bool bSetChanged);
  void gui_RemoveClassTable(const xtstring& sTableName);
  void gui_AddFieldForClassTable(const xtstring& sTableName, const CERModelFieldInd* pField);
  void gui_RemoveFieldFromClassTable(const xtstring& sTableName, const xtstring& sFieldName);
  bool gui_FieldExistInAnyTable(const xtstring& sFieldName);
  bool gui_FieldToImportInAnyTable(const xtstring& sFieldName);
  // gui fx <--
*/
  size_t CountOfTables() const {return m_arrTables.size();};
  bool TableInList(const xtstring& sTable) const;
  xtstring GetTable(size_t nIndex) const;
  void AddTable(const xtstring& sTable);
  bool RemoveTable(size_t nIndex);
  bool RemoveTable(const xtstring& sTable);
  bool RemoveAllTables();
  void GetAllTableNames(CxtstringVector& arrTableNames);
  xtstring GetAllTablesDelimited();

protected:
  CxtstringVector m_arrTables;
  // non stream data -->
  xtstring        m_sParentTableName;
/*
  bool            m_bInGUIChanged;
  CERModelTables  m_guiTables;
*/
  // non stream data <--

protected:
  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual bool SE_GetShowRelationNamesER(){return true;};
  virtual xtstring SE_GetDBDefinitionName_Production(){return _XT("");};
  virtual xtstring SE_GetDBDefinitionName_Test(){return _XT("");};
  virtual const CERModelTableInd* SE_GetERModelTableInd(){return 0;};

  virtual void SE_OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName);
  virtual void SE_OnTableDelete(const xtstring& sName);
  virtual void SE_OnFieldNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnParamNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual bool SE_OnRelationNameWillBeChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){return true;};

  virtual void SE_OnSyncFontChanged(const xtstring& sFontName, int nFontHeight){};
  virtual void SE_OnSyncColorChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorBackChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncMoveableChanged(bool bVal){};
  virtual void SE_OnSyncSelectableChanged(bool bVal){};
  virtual void SE_OnSyncVisibleChanged(bool bVal){};
  virtual void SE_OnSyncSizeableChanged(bool bVal){};
};

/*
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelTablesForFreeSelect
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CERModelTablesForFreeSelect : public CModelBaseInd,
                                                           public CSyncNotifier_ER
{
private:
  typedef std::vector<CERModelTableInd*>    CERModelTables;
  typedef CERModelTables::iterator          CERModelTablesIterator;
  typedef CERModelTables::const_iterator    CERModelTablesConstIterator;
  typedef CERModelTables::reverse_iterator  CERModelTablesReverseIterator;
public:
  CERModelTablesForFreeSelect(CSyncERNotifiersGroup* pGroup);
  virtual ~CERModelTablesForFreeSelect();

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  bool AssignFrom(const CERModelTablesForFreeSelect* pTables);
  bool CopyFrom(const CERModelTablesForFreeSelect* pTables, bool& bChanged);

  // gui fx -->
  void gui_SetParentTable(const xtstring& sParentTableName){m_sParentTableName = sParentTableName;};
  bool gui_InGUIChanged(){return m_bInGUIChanged;};
  void gui_TransportTables();
  void gui_Close();
  size_t gui_CountOfTables(){return m_guiTables.size();};
  CERModelTableInd* gui_GetTable(const xtstring& sTableName);
  CERModelTableInd* gui_GetTable_const(const xtstring& sTableName) const;
  CERModelTableInd* gui_GetTable(size_t nIndex){if (nIndex < m_guiTables.size())return m_guiTables[nIndex];return 0;};
  void gui_BuildClassForTable(const xtstring& sTableName, bool bSetChanged);
  void gui_RemoveClassTable(const xtstring& sTableName);
  void gui_AddFieldForClassTable(const xtstring& sTableName, const CERModelFieldInd* pField);
  void gui_RemoveFieldFromClassTable(const xtstring& sTableName, const xtstring& sFieldName);
  bool gui_FieldExistInAnyTable(const xtstring& sFieldName);
  bool gui_FieldToImportInAnyTable(const xtstring& sFieldName);
  // gui fx <--

  size_t CountOfTables() const {return m_arrTables.size();};
  bool TableInList(const xtstring& sTable) const;
  xtstring GetTable(size_t nIndex) const;
  void AddTable(const xtstring& sTable);
  bool RemoveTable(size_t nIndex);
  bool RemoveTable(const xtstring& sTable);
  bool RemoveAllTables();
  void GetAllTableNames(CxtstringVector& arrTableNames);
  xtstring GetAllTablesDelimited();

protected:
  CxtstringVector m_arrTables;
  // non stream data -->
  xtstring        m_sParentTableName;
  bool            m_bInGUIChanged;
  CERModelTables  m_guiTables;
  // non stream data <--

protected:
  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual bool SE_GetShowRelationNamesER(){return true;};
  virtual xtstring SE_GetDBDefinitionName_Production(){return _XT("");};
  virtual xtstring SE_GetDBDefinitionName_Test(){return _XT("");};
  virtual const CERModelTableInd* SE_GetERModelTableInd(){return 0;};

  virtual void SE_OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName);
  virtual void SE_OnTableDelete(const xtstring& sName);
  virtual void SE_OnFieldNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnParamNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual bool SE_OnRelationNameWillBeChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){return true;};

  virtual void SE_OnSyncFontChanged(const xtstring& sFontName, int nFontHeight){};
  virtual void SE_OnSyncColorChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorBackChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor){};
  virtual void SE_OnSyncMoveableChanged(bool bVal){};
  virtual void SE_OnSyncSelectableChanged(bool bVal){};
  virtual void SE_OnSyncVisibleChanged(bool bVal){};
  virtual void SE_OnSyncSizeableChanged(bool bVal){};
};
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CERModelTableInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CERModelChannelToGUI
{
public:
  CERModelChannelToGUI(){};
  virtual ~CERModelChannelToGUI(){};

  virtual void PropertiesChanged() = 0;
};
class XPUBMODEL_EXPORTIMPORT CERModelTableInd : public CModelBaseInd,
                                                public CSyncNotifier_ER,
                                                public CSyncResponse_Common
{
public:
  typedef enum TERTableType
  {
    tERTT_FirstDummy = -1,
    tERTT_Table = 0,
    tERTT_StoredProcedure,
    tERTT_SQLCommand,
    tERTT_DLLTable,
    tERTT_LastDummy,
    tERTT_Default = tERTT_Table,
  };
  CERModelTableInd(CSyncERNotifiersGroup* pGroup);
  CERModelTableInd(CERModelTableInd& cTable);
  CERModelTableInd(CERModelTableInd* pTable);
  virtual ~CERModelTableInd();

  virtual const CERModelTableInd* GetERModelTableInd(){return this;};

  void SetChannelToGUI(CERModelChannelToGUI* pChannelToGUI){m_pChannelToGUI = pChannelToGUI;};

  void SetERModelTableInd(CERModelTableInd* pTable, bool bWithRelations = true);
  xtstring GetXMLDoc();
  bool SetXMLDoc(const xtstring& sXMLDoc);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);
  void CopyDiverseProperties(CERModelTableInd* pTable);
  void CopyPositionProperties(CERModelTableInd* pTable);

  bool FieldIsInRelation(const xtstring& sFieldName);

  /////////////////////////////////////////////////////////////
  // interface fo common properties
  TERTableType GetERTableType(){return m_nERTableType;};
  void SetERTableType(TERTableType nERTableType){m_nERTableType = nERTableType;};
  // name
  xtstring GetName() const {return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  // comment
  xtstring GetComment() const {return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
  // sql command
  xtstring GetSQLCommand(){return m_sSQLCommand;};
  void SetSQLCommand(const xtstring& sSQLCommand){m_sSQLCommand = sSQLCommand;};
  // free selection data
  xtstring GetFreeSelectionData(){return m_sFreeSelectionData;};
  void SetFreeSelectionData(const xtstring& sFreeSelectionData){m_sFreeSelectionData = sFreeSelectionData;};
  // dll name
  xtstring GetDLLName(){return m_sDLLName;};
  void SetDLLName(const xtstring& sDLLName){m_sDLLName = sDLLName;};
  // dll interface
  xtstring GetDLLInterface(){return m_sDLLInterface;};
  void SetDLLInterface(const xtstring& sDLLInterface){m_sDLLInterface = sDLLInterface;};
  // XMLFile
  xtstring GetXMLFile(){return m_sXMLFile;};
  void SetXMLFile(const xtstring& sXMLFile){m_sXMLFile = sXMLFile;};
  // XMLPath
  xtstring GetXMLPath(){return m_sXMLPath;};
  void SetXMLPath(const xtstring& sXMLPath){m_sXMLPath = sXMLPath;};

  int GetFlags(){return m_nFlags;};
  void SetFlags(int nFlags){m_nFlags = nFlags;};
  xtstring GetPersistentData(){return m_sPersistentData;};
  void SetPersistentData(const xtstring& sPersistentData){m_sPersistentData = sPersistentData;};

  xtstring GetImportTableDefFile(){return m_sImportTableDefFile;};
  void SetImportTableDefFile(const xtstring& sImportTableDefFile){m_sImportTableDefFile = sImportTableDefFile;};
  xtstring GetImportTableDefName(){return m_sImportTableDefName;};
  void SetImportTableDefName(const xtstring& sImportTableDefName){m_sImportTableDefName = sImportTableDefName;};

  // fields
  size_t CountOfFields() const {return m_arrFields.size();};
  CERModelFieldInd* GetField(size_t nIndex) const;
  const CERModelFieldInd* GetFieldConst(size_t nIndex) const;
  CERModelFieldInd* GetField(const xtstring& sName) const;
  const CERModelFieldInd* GetFieldConst(const xtstring& sName) const;
  void AddField(CERModelFieldInd *pField);
  bool RemoveField(size_t nIndex);
  bool RemoveField(const xtstring& sName);
  bool RemoveAllFields();
  void GetAllFieldNames(CxtstringVector& arrFieldNames);

  // params
  size_t CountOfParams(){return m_arrParams.size();};
  CERModelParamInd* GetParam(size_t nIndex);
  CERModelParamInd* GetParam(const xtstring& sName);
  void AddParam(CERModelParamInd *pParam);
  bool RemoveParam(size_t nIndex);
  bool RemoveParam(const xtstring& sName);
  bool RemoveAllParams();
  void GetAllParamNames(CxtstringVector& arrParamNames);

  // relations
  size_t CountOfRelations() const {return m_arrRelations.size();};
  CERModelRelationInd* GetRelation(size_t nIndex){assert(nIndex < m_arrRelations.size()); if (nIndex < m_arrRelations.size()) return m_arrRelations[nIndex]; else return NULL;};
  CERModelRelationInd* GetRelation(const xtstring& sName);
  void AddRelation(CERModelRelationInd* pRelation){assert(pRelation);
                                                CERModelRelationInd* ppRelation;
                                                ppRelation = new CERModelRelationInd(true, NULL);
                                                ppRelation->SetERModelRelationInd(pRelation);
                                                ppRelation->SetSyncGroup(GetSyncGroup());
                                                ppRelation->SetERTableInd(this);
                                                m_arrRelations.push_back(ppRelation);};
  bool RemoveRelation(CERModelRelationInd* pRelation);
  bool RemoveAllRelations();

  // relations to child
  size_t CountOfRelationsToChilds() const {return m_arrRelationsToChilds.size();};
  CERModelRelationInd* GetRelationToChilds(size_t  nIndex){assert(nIndex < m_arrRelationsToChilds.size()); if (nIndex < m_arrRelationsToChilds.size()) return m_arrRelationsToChilds[nIndex]; else return NULL;};
  CERModelRelationInd* GetRelationToChilds(const xtstring& sName, xtstring sChildTable);
  CERModelRelationInd* GetRelationToChilds(const xtstring& sName);
  void AddRelationToChilds(CERModelRelationInd* pRelation){assert(pRelation);
                                                        CERModelRelationInd* ppRelation;
                                                        ppRelation = new CERModelRelationInd(false, NULL);
                                                        ppRelation->SetERModelRelationInd(pRelation);
                                                        ppRelation->SetSyncGroup(GetSyncGroup());
                                                        ppRelation->SetERTableInd(this);
                                                        m_arrRelationsToChilds.push_back(ppRelation);};
  bool RemoveRelationToChilds(CERModelRelationInd* pRelation);
  bool RemoveAllRelationsToChilds();

  // hierarchies
  size_t CountOfHierarchies() const {return m_arrHierarchies.size();};
  CERModelRelationInd* GetHierarchy(size_t nIndex){assert(nIndex < m_arrHierarchies.size()); if (nIndex < m_arrHierarchies.size()) return m_arrHierarchies[nIndex]; else return NULL;};
  CERModelRelationInd* GetHierarchy(const xtstring& sName);
  void AddHierarchy(CERModelRelationInd* pRelation){assert(pRelation);
                                                CERModelRelationInd* ppRelation;
                                                ppRelation = new CERModelRelationInd(true, NULL);
                                                ppRelation->SetERModelRelationInd(pRelation);
                                                ppRelation->SetSyncGroup(GetSyncGroup());
                                                ppRelation->SetERTableInd(this);
                                                m_arrHierarchies.push_back(ppRelation);};
  bool RemoveHierarchy(CERModelRelationInd* pRelation);
  bool RemoveAllHierarchies();

  // hierarchies to child
  size_t CountOfHierarchiesToChilds() const {return m_arrHierarchiesToChilds.size();};
  CERModelRelationInd* GetHierarchyToChilds(size_t  nIndex){assert(nIndex < m_arrHierarchiesToChilds.size()); if (nIndex < m_arrHierarchiesToChilds.size()) return m_arrHierarchiesToChilds[nIndex]; else return NULL;};
  CERModelRelationInd* GetHierarchyToChilds(const xtstring& sName, xtstring sChildTable);
  CERModelRelationInd* GetHierarchyToChilds(const xtstring& sName);
  void AddHierarchyToChilds(CERModelRelationInd* pRelation){assert(pRelation);
                                                        CERModelRelationInd* ppRelation;
                                                        ppRelation = new CERModelRelationInd(false, NULL);
                                                        ppRelation->SetERModelRelationInd(pRelation);
                                                        ppRelation->SetSyncGroup(GetSyncGroup());
                                                        ppRelation->SetERTableInd(this);
                                                        m_arrHierarchiesToChilds.push_back(ppRelation);};
  bool RemoveHierarchyToChilds(CERModelRelationInd* pRelation);
  bool RemoveAllHierarchiesToChilds();

  /////////////////////////////////////////////////////////////
  // interface for position properties
  long GetLeft(){return m_nPosLeft;};
  void SetLeft(long nLeft){m_nPosLeft = nLeft;};
  long GetTop(){return m_nPosTop;};
  void SetTop(long nTop){m_nPosTop = nTop;};
  long GetWidth(){return m_nSizeWidth;};
  void SetWidth(long nWidth){m_nSizeWidth = nWidth;};
  long GetHeight(){return m_nSizeHeight;};
  void SetHeight(long nHeight){m_nSizeHeight = nHeight;};
  /////////////////////////////////////////////////////////////
  // interface for diverse properties
  bool GetSynchronize(){return m_bSynchronize;};
  void SetSynchronize(bool bSynchronize){m_bSynchronize = bSynchronize;};
  MODEL_COLOR GetColor(){return m_nColor;};
  void SetColor(MODEL_COLOR nColor){m_nColor = nColor;};
  MODEL_COLOR GetColorBack(){return m_nColorBack;};
  void SetColorBack(MODEL_COLOR nColor){m_nColorBack = nColor;};
  MODEL_COLOR GetColorHdr(){return m_nColorHdr;};
  void SetColorHdr(MODEL_COLOR nColor){m_nColorHdr = nColor;};
  MODEL_COLOR GetColorHdrSel(){return m_nColorHdrSel;};
  void SetColorHdrSel(MODEL_COLOR nColor){m_nColorHdrSel = nColor;};
  MODEL_COLOR GetColorHdrText(){return m_nColorHdrText;};
  void SetColorHdrText(MODEL_COLOR nColor){m_nColorHdrText = nColor;};
  MODEL_COLOR GetColorHdrSelText(){return m_nColorHdrSelText;};
  void SetColorHdrSelText(MODEL_COLOR nColor){m_nColorHdrSelText = nColor;};
  MODEL_COLOR GetColorSelItem(){return m_nColorSelItem;};
  void SetColorSelItem(MODEL_COLOR nColor){m_nColorSelItem = nColor;};
  bool GetMoveable(){return m_bMoveable;};
  void SetMoveable(bool bMoveable){m_bMoveable = bMoveable;};
  bool GetSelectable(){return m_bSelectable;};
  void SetSelectable(bool bSelectable){m_bSelectable = bSelectable;};
  bool GetVisible(){return m_bVisible;};
  void SetVisible(bool bVisible){m_bVisible = bVisible;};
  bool GetSizeable(){return m_bSizeable;};
  void SetSizeable(bool bSizeable){m_bSizeable = bSizeable;};
  xtstring GetFontName(){return m_sFontName;};
  void SetFontName(const xtstring& sFontName){m_sFontName = sFontName;};
  int GetFontHeight(){return m_nFontHeight;};
  void SetFontHeight(int nFontHeight){m_nFontHeight = nFontHeight;};

  void PropertiesChanged();

  const CERFreeSelectTables* GetTablesForFreeSelect(){return &m_cTablesForFreeSelect;};
  CERFreeSelectTables* GetTablesNotConstForFreeSelect(){return &m_cTablesForFreeSelect;};

  // spezielle Funktion, die in AddOn benutzt ist
  void RenameTable(const xtstring& sOldName, const xtstring& sNewName);

  bool FieldAddable();
  bool FieldRemovable();
  bool ParamAddable();
  bool ParamRemovable();
  bool RelationAddable();
  bool RelationRemovable();
protected:

  CERFreeSelectTables m_cTablesForFreeSelect;
private:
  // common properties
  TERTableType  m_nERTableType;
  xtstring      m_sName;
  xtstring      m_sComment;
  xtstring      m_sSQLCommand;
  xtstring      m_sFreeSelectionData;
  xtstring      m_sDLLName;
  xtstring      m_sDLLInterface;
  xtstring      m_sXMLFile;
  xtstring      m_sXMLPath;
  int           m_nFlags;
  xtstring      m_sPersistentData;
  xtstring      m_sImportTableDefFile;
  xtstring      m_sImportTableDefName;
  CERModelFields    m_arrFields;
  CERModelFieldsMap m_mapFields;
  CERModelParams    m_arrParams;
  CERModelRelations m_arrRelations;
  CERModelRelations m_arrRelationsToChilds;
  CERModelRelations m_arrHierarchies;
  CERModelRelations m_arrHierarchiesToChilds;
  // position properties
  long    m_nPosLeft;
  long    m_nPosTop;
  long    m_nSizeWidth;
  long    m_nSizeHeight;
  // diverse properties
  bool      m_bSynchronize;
  MODEL_COLOR  m_nColor;
  MODEL_COLOR  m_nColorBack;
  MODEL_COLOR  m_nColorHdr;
  MODEL_COLOR  m_nColorHdrSel;
  MODEL_COLOR  m_nColorHdrText;
  MODEL_COLOR  m_nColorHdrSelText;
  MODEL_COLOR  m_nColorSelItem;
  bool      m_bMoveable;
  bool      m_bSelectable;
  bool      m_bVisible;
  bool      m_bSizeable;
  xtstring  m_sFontName;
  int       m_nFontHeight;

  CERModelChannelToGUI* m_pChannelToGUI;

protected:
  virtual bool SE_TableExist(const xtstring& sTableName){return (sTableName == GetName());};
  virtual bool SE_FieldExist(const xtstring& sFieldName){return false;};
  virtual bool SE_EntityExist(const xtstring& sEntityName){return false;};
  virtual bool SE_VariableExist(const xtstring& sVariableName){return false;};

  virtual void SE_OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnTableDelete(const xtstring& sName){};
  virtual void SE_OnFieldNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName);
  virtual void SE_OnParamNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual bool SE_OnRelationNameWillBeChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){return true;};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual bool SE_GetShowRelationNamesER(){return true;};
  virtual xtstring SE_GetDBDefinitionName_Production(){return _XT("");};
  virtual xtstring SE_GetDBDefinitionName_Test(){return _XT("");};
public:
  virtual const CERModelTableInd* SE_GetERModelTableInd(){return GetERModelTableInd();};
protected:

  virtual void SE_OnSyncFontChanged(const xtstring& sFontName, int nFontHeight);
  virtual void SE_OnSyncColorChanged(MODEL_COLOR nColor);
  virtual void SE_OnSyncColorBackChanged(MODEL_COLOR nColor);
  virtual void SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor);
  virtual void SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor);
  virtual void SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor);
  virtual void SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor);
  virtual void SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor);
  virtual void SE_OnSyncMoveableChanged(bool bVal);
  virtual void SE_OnSyncSelectableChanged(bool bVal);
  virtual void SE_OnSyncVisibleChanged(bool bVal);
  virtual void SE_OnSyncSizeableChanged(bool bVal);
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_ERMODELIND_H_)
