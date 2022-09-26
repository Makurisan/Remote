#if !defined(_SYNCENGINEBASE_H_)
#define _SYNCENGINEBASE_H_


#include <assert.h>
#include "SModelInd/ModelExpImp.h"
#include "SModelInd/ModelDef.h"
#include "SSTLdef/STLdef.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CSyncEngine;
class CSyncGroup;
class CSyncERNotifiersGroup;
class CSyncIFSNotifiersGroup;
class CSyncWPNotifiersGroup;
class CSyncResponse;


class CERModelFieldInd;
class CERModelParamInd;
class CERModelRelationInd;



typedef enum TModulesCountChanged
{
  tMCC_ERModel_Created,
  tMCC_ERModel_Added,
  tMCC_ERModel_Removed,
  tMCC_IFS_Created,
  tMCC_IFS_Added,
  tMCC_IFS_Removed,
  tMCC_Stone_Created,
  tMCC_Stone_Added,
  tMCC_Stone_Removed,
  tMCC_Template_Created,
  tMCC_Template_Added,
  tMCC_Template_Removed,
  tMCC_PythonModule_Created,
  tMCC_PythonModule_Added,
  tMCC_PythonModule_Removed,
};



typedef enum TTypeOfResponseObject
{
  tRO_NotDefined = 0,

  tRO_MainProperty_ER,
  tRO_MainProperty_IFS,
  tRO_MainProperty_Stone,
  tRO_MainProperty_Template,
  tRO_MainProperty_Python,
  tRO_MainProperty_KeyField,
  tRO_MainProperty_TextField,
  tRO_MainProperty_AreaField,
  tRO_MainProperty_LinkField,
  tRO_MainProperty_GraphicField,

  tRO_ERModel_Field,
  tRO_ERModel_Param,
  tRO_ERModel_Relation,
  tRO_ERModel_Table,

  tRO_IFS_Field,
  tRO_IFS_Connection,
  tRO_IFS_Entity,
  tRO_IFS_HitName,
  tRO_IFS_Assignment,
  tRO_IFS_Variable,
  tRO_IFS_PreProcess,
  tRO_IFS_PostProcess,
  tRO_IFS_OneCondition,
  tRO_IFS_Parameters,
  tRO_IFS_DetermineConditionViaEntityAndVariable,
  tRO_IFS_DetermineSQLQueryViaEntityAndVariable,
  tRO_IFS_GlobalERTable,

  tRO_DBStone,
  tRO_DBStone_Field,
  tRO_DBTemplate,
  tRO_DBTemplate_Table,

  tRO_PropStone,
  tRO_PropStone_Property,
};

typedef enum TTypeOfSyncProperty
{
  tSyncProp_ER_DBNameProd,
  tSyncProp_ER_DBNameTest,

  tSyncProp_IFS_ERModelName,
  tSyncProp_IFS_ParamTable,

  tSyncProp_IFSEntity_ERTableStoneType,
  tSyncProp_IFSEntity_Condition,
  tSyncProp_IFSEntity_PortCondition,
  tSyncProp_IFSEntity_HitCondition,
  tSyncProp_IFSEntity_ParamValues,
  tSyncProp_IFSEntity_SubIFS,
  tSyncProp_IFSEntity_Identity,
  tSyncProp_IFSEntity_UseRelation,
  tSyncProp_IFSEntity_DetermineCondition_ViaEntity,
  tSyncProp_IFSEntity_DetermineCondition_ViaVariable,
  tSyncProp_IFSEntity_DetermineSQLQuery_ViaEntity,
  tSyncProp_IFSEntity_DetermineSQLQuery_ViaVariable,
  tSyncProp_IFSEntity_GlobalERTable,

  tSyncProp_IFSEntityOneCondition_Condition,

  tSyncProp_IFSParameters_TextForm,
  tSyncProp_IFSParameters_Parameter,
  tSyncProp_IFSParameters_ParameterExpression,

  tSyncProp_IFSEntityHit_IFS,
  tSyncProp_IFSEntityHit_IFSTargetEntity,
  tSyncProp_IFSEntityHit_HitName,
  tSyncProp_IFSEntityHit_StoneOrTemplate,
  tSyncProp_IFSEntityHit_StoneOrTemplateRef,
  tSyncProp_IFSEntityHit_TemplateFileShowName,
  tSyncProp_IFSEntityHit_TemplateFile,
  tSyncProp_IFSEntityHit_TemplateFolder,
  tSyncProp_IFSEntityHit_Visibility,
  
  tSyncProp_IFSEntityAssignment_Entity,
  tSyncProp_IFSEntityAssignment_Assignment,
  tSyncProp_IFSEntityAssignment_AssignField,
  tSyncProp_IFSEntityAssignment_AssignValue,

  tSyncProp_IFSEntityPrePostProcess_Visibility,
  tSyncProp_IFSEntityPrePostProcess_PropertyStone,

  tSyncProp_Stone_IFS,
  tSyncProp_Stone_IFSTargetEntity,
  tSyncProp_Stone_StoneOrTemplate,
  tSyncProp_Stone_StoneOrTemplateRef,
  tSyncProp_Stone_Visibility,
  tSyncProp_Stone_TagProperty,

  tSyncProp_KeyField_Entity,
  tSyncProp_KeyField_Visibility,
  tSyncProp_KeyField_TagProperty,
  tSyncProp_KeyField_KeyValue,
  tSyncProp_TextField_Entity,
  tSyncProp_TextField_Visibility,
  tSyncProp_TextField_Content,
  tSyncProp_TextField_TagProperty,
  tSyncProp_AreaField_Entity,
  tSyncProp_AreaField_Visibility,
  tSyncProp_LinkField_Entity,
  tSyncProp_LinkField_Visibility,
  tSyncProp_LinkField_Address,
  tSyncProp_LinkField_QuickInfo,
  tSyncProp_LinkField_Tag,
  tSyncProp_GraphicField_Entity,
  tSyncProp_GraphicField_Visibility,
  tSyncProp_GraphicField_PicFileNameLocal,
  tSyncProp_GraphicField_PicFileName,
  tSyncProp_GraphicField_QuickInfo,
  tSyncProp_GraphicField_Tag,

  tSyncProp_Template_IFS,
  tSyncProp_Template_IFSTargetEntity,
  tSyncProp_Template_StoneOrTemplate,
  tSyncProp_Template_TemplateFileShowName,
  tSyncProp_Template_TemplateFile,
  tSyncProp_Template_TemplateFolder,
  tSyncProp_Template_SymbolicDBName,
  tSyncProp_Template_Visibility,
  tSyncProp_Template_TagProperty,

  tSyncProp_DBStone_ERModel,
  tSyncProp_DBStone_ERTable,
  tSyncProp_DBStoneField_Value,
  tSyncProp_DBStoneField_FieldName,
  tSyncProp_DBTemplate_ERModel,
  tSyncProp_DBTemplateTable_Value,
  tSyncProp_DBTemplateTable_TableName,

  tSyncProp_PropStone_Expression,
};

class XPUBMODEL_EXPORTIMPORT CUserOfObject
{
public:
  typedef enum TModuleType
  {
    tMT_AnyModule,
    tMT_ERModule = 1,
    tMT_IFSModule,
    tMT_StoneModule,
    tMT_TemplateModule,
    tMT_ModulePythonModule,
  };

  CUserOfObject(TModuleType nModuleType,
                const xtstring& sModuleName,
                const xtstring& sMasterObjectName,
                const xtstring& sObjectName,
                TTypeOfResponseObject nTypeOfResponseObject,
                CSyncResponse* pSyncResponseObject)
  {
    m_nModuleType = nModuleType;
    m_sModuleName = sModuleName;
    m_sMasterObjectName = sMasterObjectName;
    m_sObjectName = sObjectName;
    m_nTypeOfResponseObject = nTypeOfResponseObject;
    m_pSyncResponseObject = pSyncResponseObject;
  }
  ~CUserOfObject(){};
  void SetChangePositions(TTypeOfSyncProperty nPropertyType,
                          size_t nIdentify,
                          const xtstring& sPropertyContent,
                          int nPositionInPropertyContent = 0)
  {
    m_nPropertyType = nPropertyType;
    m_nIdentify = nIdentify;
    m_sPropertyContent = sPropertyContent;

    m_bOnePositionVersion = true;

    m_PositionsE.push_back(nPositionInPropertyContent);
    m_PositionsV.push_back(0);
    m_Changes.push_back(false);
  }
  void SetChangePositions(TTypeOfSyncProperty nPropertyType,
                          size_t nIdentify,
                          const xtstring& sPropertyContent,
                          CsizetVector& PositionsE,
                          CsizetVector& PositionsV)
  {
    m_nPropertyType = nPropertyType;
    m_nIdentify = nIdentify;
    m_sPropertyContent = sPropertyContent;

    m_bOnePositionVersion = false;

    assert(PositionsE.size() == PositionsV.size());
    m_PositionsE.assign(PositionsE.begin(), PositionsE.end());
    m_PositionsV.assign(PositionsV.begin(), PositionsV.end());
    for (size_t nI = 0; nI < m_PositionsE.size(); nI++)
      m_Changes.push_back(false);
  }
  bool GetChange(){assert(m_Changes.size() == 1); return (*(m_Changes.begin()));};
  //////////////////////////////////////////////////////////////////
  // ModelObjekt - Identifikation
  // Mit folgenden Variablen ist ModelObjekt eindeutig identifiziert
  //////////////////////////////////////////////////////////////////
  TModuleType   m_nModuleType;
  xtstring      m_sModuleName;
  // Da kann ERTable Name, oder IFSEntity Name stehen.
  // Falls leer, nicht vorhanden. (Baustein / Vorlage)
  xtstring      m_sMasterObjectName;
  // Objektname
  xtstring      m_sObjectName;
  // Jeder ModelObjekt, der CUserOfObject ist eindeutig mit TTypeOfResponseObject identifiziert
  TTypeOfResponseObject m_nTypeOfResponseObject;
  // Jedes ModelObjekt, der CUserOfObject anlegen kann, muss von CSyncResponse abgeleitet werden
  // Da muss Zeiger auf diese Klasse eingetragen, dass man je nach Benutzerauswahl alle Objekte
  // ansprechen kann, wo Aenderung gemacht wird
  CSyncResponse*  m_pSyncResponseObject;

  //////////////////////////////////////////////////////////////////
  // ModelObjekt / Informationen ueber Aenderungsstellen
  // Mit folgenden Variablen ist Stelle / sind Stellen beschrieben,
  // wo die Aenderungen ggf. gemacht werden.
  //////////////////////////////////////////////////////////////////
  // Eigenschafttype, in welcher eine Aenderung ggf. gemacht wird.
  TTypeOfSyncProperty m_nPropertyType;
  // Wenn ein ModelObjekt mehrere CUserOfObject erzeugt, aufgrund diesem Zahl
  // kann man spaeter entscheiden, um welche Eigenschaft geht.
  // (Diese Variable musste eingefuehrt werden, weil nicht in allen Faellen reicht
  // vorhergehende Variable. (So ein Fall ist in CIFSEntityAllParameters)
  size_t m_nIdentify;
  // Eigenschaftinhalt, wo Aenderung ggf. gemacht wird.
  // Inhalt kann einfaches Text (Wort) sein, oder Bedingung / Zuweisung
  xtstring m_sPropertyContent;
  // Diese Variable sagt, ob in 'm_sPropertyContent' eine Stelle / mehrere Stellen
  // ggf. geaendert werden.
  bool m_bOnePositionVersion;
  // In folgenden Variablen sind so viele Elemente, wieviele Stellen in 'm_bOnePositionVersion'
  // zur Aenderung gefunden wurden. Dadurch in SyncDialog wird zu diesem Objekt so viele Zeilen
  // dargestellt, wie viele Elemente in diesen Variablen vorhanden sind.
  CsizetVector  m_PositionsE;
  CsizetVector  m_PositionsV;
  CboolVector   m_Changes;
};

typedef vector<CUserOfObject*>                CUserOfObjectArray;
typedef CUserOfObjectArray::iterator          CUserOfObjectArrayIterator;
typedef CUserOfObjectArray::const_iterator    CUserOfObjectArrayConstIterator;
typedef CUserOfObjectArray::reverse_iterator  CUserOfObjectArrayReverseIterator;











// alle Objekte, die synchronisiert sein muessen,
// muessen von dieser Klasse abgeleitet werden.
// in dieser Klasse sind die Funktionen implementiert,
// mit welchen kann man die erzeugte Klassen in SyncEngine
// an gewuenschte Stelle anhaengen

// in der Applikation sind drei Objekte moeglich
// 1. Objekt meldet bestimmte Aenderungen
// 2. Objekt will ueber bestimmten Aenderungen benachrichtig werden
// 3. Objekt mit beiden obigen Eigenschften

class XPUBMODEL_EXPORTIMPORT CSyncEngineBase
{
public:
  CSyncEngineBase(){};
  virtual ~CSyncEngineBase(){};
protected:

  // hier ist eine Instanz von CSyncEngine
  // In der Applikation kann nur ein SyncEngine instanziert werden
  // und diese Variable ist in SyncEngine Konstruktor gesetzt.
  static CSyncEngine* m_pSyncEngine;
};

class XPUBMODEL_EXPORTIMPORT CSyncEngineObjects : public CSyncEngineBase
{
public:
  CSyncEngineObjects(){};
  virtual ~CSyncEngineObjects(){};

  CSyncGroup* GetSyncGroup(){return m_pSyncGroup;};

protected:
  void SetSyncGroup(CSyncGroup* pSyncGroup){m_pSyncGroup = (CSyncGroup*)pSyncGroup;};

  CSyncGroup* m_pSyncGroup;

protected:
  bool ProjConstantExistInExpression(const xtstring& sConstantName, const xtstring& sExpression, const xtstring& sValues, CsizetVector& PositionsE, CsizetVector& PositionsV);
  bool ReplaceProjConstantInExpression(const xtstring& sConstantName, const xtstring& sNewConstantName, xtstring& sExpression, xtstring& sValues, xtstring& sOffsets, CsizetVector& PositionsE, CsizetVector& PositionsV, CboolVector& Changes);
  bool ReplaceProjConstantInExpression(const xtstring& sConstantName, const xtstring& sNewConstantName, xtstring& sExpression, xtstring& sValues, CsizetVector& PositionsE, CsizetVector& PositionsV, CboolVector& Changes);

  xtstring GetEntityFromPositionForFieldOrVariable(const xtstring sExpressionOrValues, size_t nFieldPos);

  bool FieldExistInExpression(const xtstring& sIFSModul, const xtstring& sERModul, const xtstring& sERTable, const xtstring& sFieldName, const xtstring& sExpression, const xtstring& sValues, CsizetVector& PositionsE, CsizetVector& PositionsV);
  bool ReplaceFieldInExpression(const xtstring& sFieldName, const xtstring& sNewFieldName, xtstring& sExpression, xtstring& sValues, xtstring& sOffsets, CsizetVector& PositionsE, CsizetVector& PositionsV, CboolVector& Changes);
  bool ReplaceFieldInExpression(const xtstring& sFieldName, const xtstring& sNewFieldName, xtstring& sExpression, xtstring& sValues, CsizetVector& PositionsE, CsizetVector& PositionsV, CboolVector& Changes);

  bool EntityExistInExpression(const xtstring& sEntityName, const xtstring& sExpression, const xtstring& sValues, CsizetVector& PositionsE, CsizetVector& PositionsV);
  bool ReplaceEntityInExpression(const xtstring& sEntityName, const xtstring& sNewEntityName, xtstring& sExpression, xtstring& sValues, xtstring& sOffsets, CsizetVector& PositionsE, CsizetVector& PositionsV, CboolVector& Changes);
  bool ReplaceEntityInExpression(const xtstring& sEntityName, const xtstring& sNewEntityName, xtstring& sExpression, xtstring& sValues, CsizetVector& PositionsE, CsizetVector& PositionsV, CboolVector& Changes);

  bool VariableExistInExpression(const xtstring& sEntityName, const xtstring& sVarName, const xtstring& sExpression, const xtstring& sValues, CsizetVector& PositionsE, CsizetVector& PositionsV);
  bool ReplaceVariableInExpression(const xtstring& sVarName, const xtstring& sNewVarName, xtstring& sExpression, xtstring& sValues, xtstring& sOffsets, CsizetVector& PositionsE, CsizetVector& PositionsV, CboolVector& Changes);
  bool ReplaceVariableInExpression(const xtstring& sVarName, const xtstring& sNewVarName, xtstring& sExpression, xtstring& sValues, CsizetVector& PositionsE, CsizetVector& PositionsV, CboolVector& Changes);

public:
  bool FindAllConstantsInExprSymbols(const xtstring& sExprSymbols, CintVector& cCompoundIndexes);
  TFieldType GetFieldTypeFromExprSymbols(const xtstring& sExprSymbols, int nCompoundIndex);

  xtstring GetValueFromExprValues(const xtstring& sExprValues, int nCompoundIndex);
  bool GetPosAndLenOfConstantInExprOffsets(const xtstring& sExprOffsets, int nCompoundIndex, int& nPos, int& nLen);
  bool ReplaceValueInExprValues(xtstring& sExprValues, const xtstring& sOldExprValue, const xtstring& sNewExprValue, int nCompoundIndex);
  bool ChangeOffsetsBecauseValueChanged(xtstring& sExprOffsets, const xtstring& sOldValue, const xtstring& sNewValue, int nPos);

  void SE_Intern_GetGlobalTableNames(const xtstring& sIFSModulName, CxtstringVector& arrNames);
};









////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Notifier classes
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncNotifier : virtual public CSyncEngineObjects
{
public:
  CSyncNotifier(){m_pSyncGroup = 0;};
  virtual ~CSyncNotifier(){};
};

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncNotifier_ER : public CSyncNotifier
{
  friend class CSyncERNotifiersGroup;
public:
  typedef enum TTypeOfERNotifier
  {
    tERNotifier_MainProperty = 1,
    tERNotifier_Field,
    tERNotifier_Param,
    tERNotifier_Relation,
    tERNotifier_Table,
    tERNotifier_TablesForFreeSelect,
  };

  CSyncNotifier_ER(TTypeOfERNotifier nTypeOfNotifier, CSyncERNotifiersGroup* pGroup);
  virtual ~CSyncNotifier_ER();
  CSyncERNotifiersGroup* GetSyncGroup(){return (CSyncERNotifiersGroup*)m_pSyncGroup;};
  void SetSyncGroup(CSyncERNotifiersGroup* pSyncGroup);

  TTypeOfERNotifier GetTypeOfNotifier(){return m_nTypeOfNotifier;};


  // diese Funktion muss vor der Aenderung aufgerufen werden
  // wenn diese Funktion 'false' liefert, dann darf die Name nicht geaendert werden
  bool SE_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);

  bool SE_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sFieldName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);

  bool SE_DeleteERModel(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteTable(const xtstring& sERModelName, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sFieldName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, bool bCancelAllowed);

protected:
  virtual void SE_OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName) = 0;
  virtual void SE_OnTableDelete(const xtstring& sName) = 0;
  virtual void SE_OnFieldNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName) = 0;
  virtual void SE_OnParamNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName) = 0;
  virtual void SE_OnRelationNameChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName) = 0;
  virtual bool SE_OnRelationNameWillBeChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName) = 0;

public:
  bool IsERTableUsed(){return false;};


  bool SE_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, bool bCancelAllowed);
  bool SE_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, bool bCancelAllowed);
  bool SE_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, bool bCancelAllowed);
  bool SE_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, bool bCancelAllowed);
public:
  xtstring                  SE_MF_GetModuleName();
  bool                      SE_MF_GetShowRelationNamesER();
  const CERModelTableInd*   SE_MF_GetERModelTableInd(const xtstring& sName);
  xtstring                  SE_MF_GetDBDefinitionName_Production();
  xtstring                  SE_MF_GetDBDefinitionName_Test();
protected:
  virtual xtstring SE_GetModuleName() = 0;
  virtual bool SE_GetShowRelationNamesER() = 0;
  virtual xtstring SE_GetDBDefinitionName_Production() = 0;
  virtual xtstring SE_GetDBDefinitionName_Test() = 0;
  virtual const CERModelTableInd* SE_GetERModelTableInd() = 0;
public:
  void SE_SyncFontChanged(const xtstring& sFontName, int nFontHeight);
  void SE_SyncColorChanged(MODEL_COLOR nColor);
  void SE_SyncColorBackChanged(MODEL_COLOR nColor);
  void SE_SyncColorHeaderChanged(MODEL_COLOR nColor);
  void SE_SyncColorHeaderSelectedChanged(MODEL_COLOR nColor);
  void SE_SyncColorHeaderTextChanged(MODEL_COLOR nColor);
  void SE_SyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor);
  void SE_SyncColorSelectedItemChanged(MODEL_COLOR nColor);
  void SE_SyncMoveableChanged(bool bVal);
  void SE_SyncSelectableChanged(bool bVal);
  void SE_SyncVisibleChanged(bool bVal);
  void SE_SyncSizeableChanged(bool bVal);
protected:
  virtual void SE_OnSyncFontChanged(const xtstring& sFontName, int nFontHeight) = 0;
  virtual void SE_OnSyncColorChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorBackChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncMoveableChanged(bool bVal) = 0;
  virtual void SE_OnSyncSelectableChanged(bool bVal) = 0;
  virtual void SE_OnSyncVisibleChanged(bool bVal) = 0;
  virtual void SE_OnSyncSizeableChanged(bool bVal) = 0;

private:
  TTypeOfERNotifier m_nTypeOfNotifier;
};

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncNotifier_IFS : public CSyncNotifier
{
  friend class CSyncIFSNotifiersGroup;
public:
  typedef enum TTypeOfIFSNotifier
  {
    tIFSNotifier_MainProperty = 1,
    tIFSNotifier_Field,
    tIFSNotifier_Connection,
    tIFSNotifier_Entity,
    tIFSNotifier_HitName,
    tIFSNotifier_Variable,
    tIFSNotifier_Assignment,
    tIFSNotifier_PreProcess,
    tIFSNotifier_PostProcess,
    tIFSNotifier_OneCondition,
    tIFSNotifier_ConditionVariations,
    tIFSNotifier_Parameters,
    tIFSNotifier_OneSQLCommand,
    tIFSNotifier_SQLCommandVariations,
    tIFSNotifier_GlobalERTable,
  };

  CSyncNotifier_IFS(TTypeOfIFSNotifier nTypeOfNotifier, CSyncIFSNotifiersGroup* pGroup);
  virtual ~CSyncNotifier_IFS();
  CSyncIFSNotifiersGroup* GetSyncGroup(){return (CSyncIFSNotifiersGroup*)m_pSyncGroup;};
  void SetSyncGroup(CSyncIFSNotifiersGroup* pSyncGroup);

  TTypeOfIFSNotifier GetTypeOfNotifier(){return m_nTypeOfNotifier;};


  // diese Funktion muss vor der Aenderung aufgerufen werden
  // wenn diese Funktion 'false' liefert, dann darf die Name nicht geaendert werden
  bool SE_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);

  bool SE_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);

  bool SE_DeleteIFS(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteEntity(const xtstring& sIFS, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);

protected:
  void SetTypeOfNotifier(TTypeOfIFSNotifier nTypeOfNotifier){m_nTypeOfNotifier = nTypeOfNotifier;};

  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName) = 0;

  void SE_RelationNameChanged();
  void SE_RelationNameDeleted();
  void SE_RelationNameAdded();
  virtual void SE_OnRelationNameChanged() = 0;
  virtual void SE_OnRelationNameDeleted() = 0;
  virtual void SE_OnRelationNameAdded() = 0;
public:
  xtstring                  SE_MF_GetModuleName();
  xtstring                  SE_MF_GetERModelName();
  void                      SE_MF_GetGlobalTableNames(CxtstringVector& arrNames);
  xtstring                  SE_MF_GetERTableFromGlobalTable(const xtstring& sGlobalTableName);
  bool                      SE_MF_GetShowRelationNames();
  bool                      SE_MF_GetShowHitLines();
  const CIFSEntityInd*      SE_MF_GetIFSEntityInd(const xtstring& sName);
  xtstring                  SE_MF_GetParamTable();
  bool                      SE_MF_SetParamTable(const xtstring& sParamTable);
  void                      SE_MF_ParamTableChanged();
  void                      SE_MF_ERModelChanged(const xtstring& sNewERModelName);
protected:
  virtual xtstring SE_GetModuleName() = 0;
  virtual xtstring SE_GetERModelName() const = 0;
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames) = 0;
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName) = 0;
  virtual bool SE_GetShowRelationNames() = 0;
  virtual bool SE_GetShowHitLines() = 0;
  virtual const CIFSEntityInd* SE_GetIFSEntityInd() = 0;
  virtual xtstring SE_GetParamTableName() const = 0;
  virtual void SE_SetParamTableName(const xtstring sParamTable) = 0;
  virtual void SE_OnParamTableChanged() = 0;
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName) = 0;
public:
  void SE_SyncFontChanged(const xtstring& sFontName, int nFontHeight);
  void SE_SyncColorChanged(MODEL_COLOR nColor);
  void SE_SyncColorBackChanged(MODEL_COLOR nColor);
  void SE_SyncColorHeaderChanged(MODEL_COLOR nColor);
  void SE_SyncColorHeaderSelectedChanged(MODEL_COLOR nColor);
  void SE_SyncColorHeaderTextChanged(MODEL_COLOR nColor);
  void SE_SyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor);
  void SE_SyncColorSelectedItemChanged(MODEL_COLOR nColor);
  void SE_SyncMoveableChanged(bool bVal);
  void SE_SyncSelectableChanged(bool bVal);
  void SE_SyncVisibleChanged(bool bVal);
  void SE_SyncSizeableChanged(bool bVal);
protected:
  virtual void SE_OnSyncFontChanged(const xtstring& sFontName, int nFontHeight) = 0;
  virtual void SE_OnSyncColorChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorBackChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorHeaderChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorHeaderSelectedChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorHeaderTextChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorHeaderSelectedTextChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncColorSelectedItemChanged(MODEL_COLOR nColor) = 0;
  virtual void SE_OnSyncMoveableChanged(bool bVal) = 0;
  virtual void SE_OnSyncSelectableChanged(bool bVal) = 0;
  virtual void SE_OnSyncVisibleChanged(bool bVal) = 0;
  virtual void SE_OnSyncSizeableChanged(bool bVal) = 0;
private:
  TTypeOfIFSNotifier m_nTypeOfNotifier;
};

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncNotifier_WP : public CSyncNotifier
{
  friend class CSyncWPNotifiersGroup;
public:
  typedef enum TTypeOfWPNotifier
  {
    tWPNotifier_StoneMainProperty = 1,
    tWPNotifier_TemplateMainProperty,

    tWPNotifier_ConstantInd,
    tWPNotifier_FieldKeyInd,
    tWPNotifier_FieldTextInd,
    tWPNotifier_FieldAreaInd,
    tWPNotifier_FieldLinkInd,
    tWPNotifier_FieldGraphicInd,

    tWPNotifier_DBStone,
    tWPNotifier_DBStoneField,
    tWPNotifier_DBTemplate,
    tWPNotifier_DBTemplateTable,

    tWPNotifier_PropStone,
    tWPNotifier_PropStoneProperties,
  };

  CSyncNotifier_WP(TTypeOfWPNotifier nTypeOfNotifier, CSyncWPNotifiersGroup* pGroup);
  virtual ~CSyncNotifier_WP();
  CSyncWPNotifiersGroup* GetSyncGroup(){return (CSyncWPNotifiersGroup*)m_pSyncGroup;};
  void SetSyncGroup(CSyncWPNotifiersGroup* pSyncGroup);

  TTypeOfWPNotifier GetTypeOfNotifier(){return m_nTypeOfNotifier;};

  // diese Funktion muss vor der Aenderung aufgerufen werden
  // wenn diese Funktion 'false' liefert, dann darf die Name nicht geaendert werden
  bool SE_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteWP(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);

  bool SE_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
public:
  xtstring                  SE_MF_GetModuleName();
protected:
  virtual xtstring SE_GetModuleName() = 0;

private:
  TTypeOfWPNotifier m_nTypeOfNotifier;
};









////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Response classes
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncResponse : virtual public CSyncEngineObjects
{
public:
  CSyncResponse(TTypeOfResponseObject nTypeOfResponseObject){m_nTypeOfResponseObject = nTypeOfResponseObject;};
  virtual ~CSyncResponse(){};

  TTypeOfResponseObject GetTypeOfResponseObject(){return m_nTypeOfResponseObject;};
  CSyncResponse* GetSyncResponseObject(){return this;};
protected:
  void SetTypeOfResponseObject(TTypeOfResponseObject nTypeOfResponseObject){m_nTypeOfResponseObject = nTypeOfResponseObject;};
private:
  TTypeOfResponseObject m_nTypeOfResponseObject;
};


////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncResponse_Common : public CSyncResponse
{
  friend class CSyncEngine;
public:
  CSyncResponse_Common(TTypeOfResponseObject nTypeOfResponseObject);
  virtual ~CSyncResponse_Common();
protected:
  virtual bool SE_TableExist(const xtstring& sTableName) = 0;
  virtual bool SE_FieldExist(const xtstring& sFieldName) = 0;
  virtual bool SE_EntityExist(const xtstring& sEntityName) = 0;
  virtual bool SE_VariableExist(const xtstring& sVariableName) = 0;
};

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncResponse_Proj : public CSyncResponse
{
  friend class CSyncEngine;
public:
  CSyncResponse_Proj(TTypeOfResponseObject nTypeOfResponseObject);
  virtual ~CSyncResponse_Proj();
protected:
  virtual void ROC_PropertyChanged_Proj() = 0;
  // requirement on change
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;

  virtual void ROC_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;

  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;

  virtual void ROC_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;

  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObject* pObject) = 0;
};

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncResponse_ER : public CSyncResponse
{
  friend class CSyncEngine;
public:
  CSyncResponse_ER(TTypeOfResponseObject nTypeOfResponseObject);
  virtual ~CSyncResponse_ER();
protected:
  virtual void ROC_PropertyChanged_ER() = 0;
  // requirement on change
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject) = 0;
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject) = 0;
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;

  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;

  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject) = 0;
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject) = 0;
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject) = 0;
};

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncResponse_IFS : public CSyncResponse
{
  friend class CSyncEngine;
public:
  CSyncResponse_IFS(TTypeOfResponseObject nTypeOfResponseObject);
  virtual ~CSyncResponse_IFS();
protected:
  virtual void ROC_PropertyChanged_IFS() = 0;
  // requirement on change
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject) = 0;
};

////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncResponse_WP : public CSyncResponse
{
  friend class CSyncEngine;
public:
  CSyncResponse_WP(TTypeOfResponseObject nTypeOfResponseObject);
  virtual ~CSyncResponse_WP();
protected:
  virtual void ROC_PropertyChanged_WP() = 0;
  // requirement on change
  virtual void ROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForWP(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteWP(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteWP(const xtstring& sName, CUserOfObject* pObject) = 0;

  virtual void ROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject) = 0;
  virtual void ROC_CheckReferencesForWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual void ROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObject* pObject) = 0;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // _SYNCENGINEBASE_H_
