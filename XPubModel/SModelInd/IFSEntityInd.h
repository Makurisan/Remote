// IFSEntityInd.h: interface for the CIFSEntityXXXInd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DMODELIND_H_)
#define _DMODELIND_H_


#include "ModelExpImp.h"

#include "SSyncEng/SyncEngineBase.h"
#include "DBDefinitions.h"
#include "ERModelInd.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


typedef enum TConnectionType
{
  tConnection_1,
};






class CXPubMarkUp;



class CIFSEntityFieldInd;
class CIFSEntityInd;
class CIFSEntityConnectionInd;


/////////////////////////////////////////////////////////////////////////////
// CIFSEntityGlobalERTableInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityGlobalERTableInd : public CModelBaseInd,
                                                          public CSyncNotifier_IFS,
                                                          public CSyncResponse_ER
{
public:
  CIFSEntityGlobalERTableInd(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityGlobalERTableInd(CIFSEntityGlobalERTableInd& cGlobalERTable);
  CIFSEntityGlobalERTableInd(CIFSEntityGlobalERTableInd* pGlobalERTable);
  virtual ~CIFSEntityGlobalERTableInd();
  bool SE_DeleteGlobalERTable(size_t& nCountOfReferences, bool bCancelAllowed)
    {return CSyncNotifier_IFS::SE_DeleteGlobalERTable(SE_MF_GetModuleName(), GetName(), nCountOfReferences, bCancelAllowed);};

  void SetIFSEntityGlobalERTableInd(CIFSEntityGlobalERTableInd* pGlobalERTable);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetERTableName(){return m_sERTableName;};
  void SetERTableName(const xtstring& sERTableName){m_sERTableName = sERTableName;};
  bool GetTableIsTemporary(){return m_bTableIsTemporary;};
  void SetTableIsTemporary(bool bTableIsTemporary){m_bTableIsTemporary = bTableIsTemporary;};

  xtstring GetRealERTableName(){return m_sRealERTableName;};
  void SetRealERTableName(const xtstring& sRealERTableName){m_sRealERTableName = sRealERTableName;};
  bool GetInExecUsed(){return m_bInExecUsed;};
  void SetInExecUsed(bool bInExecUsed){m_bInExecUsed = bInExecUsed;};

  void SetOwnerEntity(CIFSEntityInd* pOwnerEntity){m_pOwnerEntity = pOwnerEntity;};
  xtstring GetOwnerEntityName() const;
  CIFSEntityInd* GetOwnerEntity() const {return m_pOwnerEntity;};
protected:
  CIFSEntityInd* m_pOwnerEntity;

protected:
  xtstring  m_sName;
  xtstring  m_sERTableName;
  bool      m_bTableIsTemporary;
  // non stream data -->
  xtstring m_sRealERTableName;
  bool      m_bInExecUsed;
  // non stream data <--

protected:
  // sync engine functions
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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
  // requirement on change
  virtual void ROC_PropertyChanged_ER();
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
};

typedef std::vector<CIFSEntityGlobalERTableInd*>        CIFSEntityGlobalERTableInds;
typedef CIFSEntityGlobalERTableInds::iterator           CIFSEntityGlobalERTableIndsIterator;
typedef CIFSEntityGlobalERTableInds::reverse_iterator   CIFSEntityGlobalERTableIndsReverseIterator;
typedef CIFSEntityGlobalERTableInds::const_iterator     CIFSEntityGlobalERTableIndsConstIterator;


/////////////////////////////////////////////////////////////////////////////
// CIFSEntityPrePostProcessInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityPrePostProcessInd : public CModelBaseInd,
                                                           public CSyncNotifier_IFS,
                                                           public CSyncResponse_Proj,
                                                           public CSyncResponse_ER,
                                                           public CSyncResponse_IFS,
                                                           public CSyncResponse_WP
{
public:
  CIFSEntityPrePostProcessInd(bool bPreProcessObject, CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityPrePostProcessInd(CIFSEntityPrePostProcessInd& cPProcess);
  CIFSEntityPrePostProcessInd(CIFSEntityPrePostProcessInd* pPProcess);
  virtual ~CIFSEntityPrePostProcessInd();

  void SetIFSEntityPrePostProcessInd(CIFSEntityPrePostProcessInd* pPProcess);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);


  bool IsPreProcessObject(){return (m_bPreProcessObject == true);};
  bool IsPostProcessObject(){return (m_bPreProcessObject != true);};

  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  // visibility
  CConditionOrAssignment&         GetVisibility(){return m_caVisibility;};
  const CConditionOrAssignment&   GetVisibility_Const() const {return m_caVisibility;};
  CConditionOrAssignment*         GetVisibility_Ptr(){return &m_caVisibility;};
  // property stone
  xtstring GetPropertyStone(){return m_sPropertyStone;};
  void SetPropertyStone(const xtstring& sPropertyStone){m_sPropertyStone = sPropertyStone;};

  void SetOwnerEntity(CIFSEntityInd* pOwnerEntity){m_pOwnerEntity = pOwnerEntity;};
  xtstring GetOwnerEntityName() const;
  CIFSEntityInd* GetOwnerEntity() const {return m_pOwnerEntity;};
protected:
  CIFSEntityInd* m_pOwnerEntity;

protected:
  // non stream data
  bool                m_bPreProcessObject;
  // stream data
  xtstring                  m_sName;
  CConditionOrAssignment    m_caVisibility;
  xtstring                  m_sPropertyStone;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj();
  virtual void ROC_PropertyChanged_ER();
  virtual void ROC_PropertyChanged_IFS();
  virtual void ROC_PropertyChanged_WP();
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);

  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWP(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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

typedef std::vector<CIFSEntityPrePostProcessInd*>       CIFSEntityPrePostProcessInds;
typedef CIFSEntityPrePostProcessInds::iterator          CIFSEntityPrePostProcessIndsIterator;
typedef CIFSEntityPrePostProcessInds::reverse_iterator  CIFSEntityPrePostProcessIndsReverseIterator;
typedef CIFSEntityPrePostProcessInds::const_iterator    CIFSEntityPrePostProcessIndsConstIterator;


/////////////////////////////////////////////////////////////////////////////
// CIFSEntityVariableInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityVariableInd : public CModelBaseInd,
                                                     public CSyncNotifier_IFS,
                                                     public CSyncResponse_Common
{
public:
  CIFSEntityVariableInd(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityVariableInd(CIFSEntityVariableInd& cField);
  CIFSEntityVariableInd(CIFSEntityVariableInd* pField);
  virtual ~CIFSEntityVariableInd();
  bool SE_DeleteVariable(size_t& nCountOfReferences, bool bCancelAllowed)
    {return CSyncNotifier_IFS::SE_DeleteVariable(SE_MF_GetModuleName(), GetOwnerEntityName(), GetName(), nCountOfReferences, bCancelAllowed);};

  void SetIFSEntityVariableInd(CIFSEntityVariableInd* pField);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
  TFieldType GetType(){return m_nType;};
  void SetType(TFieldType nType){m_nType = nType;};
  long GetDefinedSize(){return m_nDefinedSize;};
  void SetDefinedSize(long nDefinedSize){m_nDefinedSize = nDefinedSize;};

  void SetOwnerEntity(CIFSEntityInd* pOwnerEntity){m_pOwnerEntity = pOwnerEntity;};
  xtstring GetOwnerEntityName() const;
  CIFSEntityInd* GetOwnerEntity() const {return m_pOwnerEntity;};
protected:
  CIFSEntityInd* m_pOwnerEntity;

private:
  xtstring    m_sName;
  xtstring    m_sComment;
  TFieldType  m_nType;
  long        m_nDefinedSize;

protected:
  // sync engine functions
  virtual bool SE_TableExist(const xtstring& sTableName){return false;};
  virtual bool SE_FieldExist(const xtstring& sFieldName){return false;};
  virtual bool SE_EntityExist(const xtstring& sEntityName){return false;};
  virtual bool SE_VariableExist(const xtstring& sVariableName){return (sVariableName == GetName());};

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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

typedef std::vector<CIFSEntityVariableInd*>       CIFSEntityVariableInds;
typedef CIFSEntityVariableInds::iterator          CIFSEntityVariableIndsIterator;
typedef CIFSEntityVariableInds::reverse_iterator  CIFSEntityVariableIndsReverseIterator;
typedef CIFSEntityVariableInds::const_iterator    CIFSEntityVariableIndsConstIterator;


/////////////////////////////////////////////////////////////////////////////
// CIFSEntityAssignmentInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityAssignmentInd : public CModelBaseInd,
                                                       public CSyncNotifier_IFS,
                                                       public CSyncResponse_Proj,
                                                       public CSyncResponse_ER,
                                                       public CSyncResponse_IFS
{
public:
  CIFSEntityAssignmentInd(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityAssignmentInd(CIFSEntityAssignmentInd& cModel);
  CIFSEntityAssignmentInd(CIFSEntityAssignmentInd* pModel);
  virtual ~CIFSEntityAssignmentInd();

  void SetIFSEntityAssignmentInd(CIFSEntityAssignmentInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};

  CConditionOrAssignment&         GetAssignment(){return m_caAssignment;};
  const CConditionOrAssignment&   GetAssignment_Const() const {return m_caAssignment;};
  CConditionOrAssignment*         GetAssignment_Ptr(){return &m_caAssignment;};

  xtstring GetEntityOfInformationSight(){return m_sEntityOfInformationSight;};
  void SetEntityOfInformationSight(const xtstring& sEntityOfInformationSight){m_sEntityOfInformationSight = sEntityOfInformationSight;};

  CConditionOrAssignment&         GetAssignField(){return m_caAssignField;};
  const CConditionOrAssignment&   GetAssignField_Const() const {return m_caAssignField;};
  CConditionOrAssignment*         GetAssignField_Ptr(){return &m_caAssignField;};

  CConditionOrAssignment&         GetAssignValue(){return m_caAssignValue;};
  const CConditionOrAssignment&   GetAssignValue_Const() const {return m_caAssignValue;};
  CConditionOrAssignment*         GetAssignValue_Ptr(){return &m_caAssignValue;};

  void SetOwnerEntity(CIFSEntityInd* pOwnerEntity){m_pOwnerEntity = pOwnerEntity;};
  xtstring GetOwnerEntityName() const;
  CIFSEntityInd* GetOwnerEntity() const {return m_pOwnerEntity;};
protected:
  CIFSEntityInd* m_pOwnerEntity;

private:
  xtstring                m_sName;
  CConditionOrAssignment  m_caAssignment;
  xtstring                m_sEntityOfInformationSight;
  CConditionOrAssignment  m_caAssignField;
  CConditionOrAssignment  m_caAssignValue;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj();
  virtual void ROC_PropertyChanged_ER();
  virtual void ROC_PropertyChanged_IFS();
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);

  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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



typedef std::vector<CIFSEntityAssignmentInd*>       CIFSEntityAssignmentInds;
typedef CIFSEntityAssignmentInds::iterator          CIFSEntityAssignmentIndsIterator;
typedef CIFSEntityAssignmentInds::reverse_iterator  CIFSEntityAssignmentIndsReverseIterator;
typedef CIFSEntityAssignmentInds::const_iterator    CIFSEntityAssignmentIndsConstIterator;







/////////////////////////////////////////////////////////////////////////////
// CIFSEntityFieldInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityFieldInd : public CModelBaseInd,
                                                  public CSyncNotifier_IFS
{
public:
  CIFSEntityFieldInd(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityFieldInd(CIFSEntityFieldInd& cField);
  CIFSEntityFieldInd(CIFSEntityFieldInd* pField);
  virtual ~CIFSEntityFieldInd();

  void SetIFSEntityFieldInd(CIFSEntityFieldInd* pField);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetNameForShow();
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};

private:
  xtstring m_sName;
  xtstring m_sComment;

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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




/////////////////////////////////////////////////////////////////////////////
// CRelationFieldToUse
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CRelationFieldToUse
{
public:
  CRelationFieldToUse();
  ~CRelationFieldToUse();

  void operator = (const CRelationFieldToUse& cRelFields);
  bool operator == (const CRelationFieldToUse& cRelFields);

  void SetParentField(const xtstring& sParentField){m_sParentField = sParentField;};
  xtstring GetParentField(){return m_sParentField;};
  void SetChildField(const xtstring& sChildField){m_sChildField = sChildField;};
  xtstring GetChildField(){return m_sChildField;};
  void SetUseField(bool bUseField){m_bUseField = bUseField;};
  bool GetUseField(){return m_bUseField;};

protected:
  xtstring  m_sParentField;
  xtstring  m_sChildField;
  bool      m_bUseField;
};
typedef vector<CRelationFieldToUse*>                CRelationFieldToUseVector;
typedef CRelationFieldToUseVector::iterator         CRelationFieldToUseVectorIterator;
typedef CRelationFieldToUseVector::reverse_iterator CRelationFieldToUseVectorReverseIterator;
typedef CRelationFieldToUseVector::const_iterator   CRelationFieldToUseVectorConstIterator;

/////////////////////////////////////////////////////////////////////////////
// CRelationToUse
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CRelationToUse
{
public:
  CRelationToUse();
  ~CRelationToUse();

  void operator = (const CERModelRelationInd& cRelation);
  void operator = (const CRelationToUse& cRelation);
  void RelationToUseChanged(const CERModelRelationInd& cRelation);
  void RelationToUseChanged(const CRelationToUse& cRelation);
  void TakeOverOldRelation(const CRelationToUse& cRelation);

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetName() const {return m_sName;};
  void SetParentTable(const xtstring& sParentTable){m_sParentTable = sParentTable;};
  xtstring GetParentTable() const {return m_sParentTable;};
  void SetChildTable(const xtstring& sChildTable){m_sChildTable = sChildTable;};
  xtstring GetChildTable() const {return m_sChildTable;};

  bool IsAnyFieldToUse() const;
  size_t CountOfFields() const;
  bool IsFieldToUse(const xtstring& sParentField, const xtstring& sChildField);
  CRelationFieldToUse* GetFields(size_t nIndex) const;
  void RemoveAllFields();

protected:
  xtstring  m_sName;
  xtstring  m_sParentTable;
  xtstring  m_sChildTable;
  CRelationFieldToUseVector m_cFields;
};
typedef vector<CRelationToUse*>                 CRelationToUseVector;
typedef CRelationToUseVector::iterator          CRelationToUseVectorIterator;
typedef CRelationToUseVector::reverse_iterator  CRelationToUseVectorReverseIterator;
typedef CRelationToUseVector::const_iterator    CRelationToUseVectorConstIterator;
/////////////////////////////////////////////////////////////////////////////
// CAllRelationsToUse
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CAllRelationsToUse
{
public:
  CAllRelationsToUse();
  ~CAllRelationsToUse();

  void operator = (const CAllRelationsToUse& cAllRelationsToUse);

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  bool IsAnyRelationToUse() const;
  size_t CountOfRelations() const;
  CRelationToUse* GetRelationToUse(size_t nIndex) const;
  CRelationToUse* GetRelationToUse(const xtstring& sRelationName) const;
  void AddRelationToUse(const CERModelRelationInd& cRelation);
  void AddRelationToUse(const CRelationToUse& cRelation);
  void RelationToUseChanged(const CERModelRelationInd& cRelation);
  void RelationToUseChanged(const CRelationToUse& cRelation);
  void TakeOverOldRelation(const CRelationToUse& cRelation);
  void RemoveRelationToUse(const xtstring& sRelationName);
  void RemoveAllRelationToUse();

protected:
  CRelationToUseVector  m_cAllRelationToUse;
};

/////////////////////////////////////////////////////////////////////////////
// CIFSEntityConnectionInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityConnectionInd : public CModelBaseInd,
                                                       public CSyncNotifier_IFS
{
public:
  typedef enum TPCHType
  {
    tPCHTypeFirst = 1,
    tPCHTypeTop,
    tPCHTypeBottomTrue,
    tPCHTypeBottomFalse,
    tPCHTypeLast,
  };
  CIFSEntityConnectionInd(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityConnectionInd(CIFSEntityConnectionInd& cConnection);
  CIFSEntityConnectionInd(CIFSEntityConnectionInd* pConnection);
  virtual ~CIFSEntityConnectionInd();
  void operator =(CIFSEntityConnectionInd& cConnection);
  bool operator ==(CIFSEntityConnectionInd& cConnection);

  void SetIFSEntityConnectionInd(CIFSEntityConnectionInd* pConnection);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  void SetType(TConnectionType nType){m_nType = nType;};
  TConnectionType GetType(){return m_nType;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetName(){return m_sName;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
  xtstring GetComment(){return m_sComment;};
  void SetParentEntity(const xtstring& sEntity){m_sParentEntity = sEntity;}
  xtstring GetParentEntity(){return m_sParentEntity;};
  void SetChildEntity(const xtstring& sEntity){m_sChildEntity = sEntity;};
  xtstring GetChildEntity(){return m_sChildEntity;};
  void SetParentType(TPCHType nType){m_nParentType = nType;};
  TPCHType GetParentType(){return m_nParentType;};
  void SetChildType(TPCHType nType){m_nChildType = nType;};
  TPCHType GetChildType(){return m_nChildType;};

  CAllRelationsToUse& GetRelationsToUse(){return m_cRelationsToUse;};

private:
  TConnectionType m_nType;
  xtstring        m_sName;
  xtstring        m_sComment;
  xtstring        m_sParentEntity;
  xtstring        m_sChildEntity;
  TPCHType        m_nParentType;
  TPCHType        m_nChildType;
  CAllRelationsToUse  m_cRelationsToUse;

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName);
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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

/////////////////////////////////////////////////////////////////////////////
// CIFSHitNameInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSHitNameInd : public CModelBaseInd,
                                              public CSyncNotifier_IFS,
                                              public CSyncResponse_Proj,
                                              public CSyncResponse_ER,
                                              public CSyncResponse_IFS,
                                              public CSyncResponse_WP
{
public:
  CIFSHitNameInd(CSyncIFSNotifiersGroup* pGroup);
  CIFSHitNameInd(CIFSHitNameInd& cName);
  CIFSHitNameInd(CIFSHitNameInd* pName);
  virtual ~CIFSHitNameInd();
  void operator =(CIFSHitNameInd& cName);
  bool operator ==(CIFSHitNameInd& cName);

  void SetIFSHitNameInd(CIFSHitNameInd* pName);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  THitType GetHitType(){return m_nHitType;};
  void SetHitType(THitType nHitType){m_nHitType = nHitType;};

  CConditionOrAssignment&         GetHitName(){return m_caHitName;};
  const CConditionOrAssignment&   GetHitName_Const() const {return m_caHitName;};
  CConditionOrAssignment*         GetHitName_Ptr(){return &m_caHitName;};

  // overload properties
  bool GetOverloadProperties(){return m_bOverloadProperties;};
  void SetOverloadProperties(bool bOverloadProperties){m_bOverloadProperties = bOverloadProperties;};
  bool GetOverloadPropertiesChanged(){return m_bOverloadedPropertiesChanged;};
  void SetOverloadPropertiesChanged(bool bOverloadPropertiesChanged){m_bOverloadedPropertiesChanged = bOverloadPropertiesChanged;};
  
  ///////////////////////////////////////////////
  // properties from stone or template
  // visibility
  CConditionOrAssignment&         GetVisibility(){return m_caVisibility;};
  const CConditionOrAssignment&   GetVisibility_Const() const {return m_caVisibility;};
  CConditionOrAssignment*         GetVisibility_Ptr(){return &m_caVisibility;};
  // information sight
  xtstring GetInformationSight(){return m_sInformationSight;};
  void SetInformationSight(const xtstring& sInformationSight){m_sInformationSight = sInformationSight;};
  bool GetUseInformationSight(){return m_bUseInformationSight;};
  void SetUseInformationSight(bool bUse){m_bUseInformationSight = bUse;};
  // ifs target entity
  xtstring GetIFSTargetEntity(){return m_sIFSTargetEntity;};
  void SetIFSTargetEntity(const xtstring& sName){m_sIFSTargetEntity = sName;};
  bool GetUseIFSTargetEntity(){return m_bUseIFSTargetEntity;};
  void SetUseIFSTargetEntity(bool bUse){m_bUseIFSTargetEntity = bUse;};
  // stone or template
  xtstring GetStoneOrTemplate(){return m_sStoneOrTemplate;};
  void SetStoneOrTemplate(const xtstring& sStoneOrTemplate){m_sStoneOrTemplate = sStoneOrTemplate;};
  bool GetUseStoneOrTemplate(){return m_bUseStoneOrTemplate;};
  void SetUseStoneOrTemplate(bool bUse){m_bUseStoneOrTemplate = bUse;};
  // stone or template ref
  xtstring GetStoneOrTemplateRef(){return m_sStoneOrTemplateRef;};
  void SetStoneOrTemplateRef(const xtstring& sStoneOrTemplateRef){m_sStoneOrTemplateRef = sStoneOrTemplateRef;};
  // copy type
  TWorkPaperCopyType GetWorkPaperCopyType(){return  m_nWorkPaperCopyType;};
  void SetWorkPaperCopyType(TWorkPaperCopyType nType){m_nWorkPaperCopyType = nType;};
  // processing mode
  TWorkPaperProcessingMode GetWorkPaperProcessingMode(){return m_nWorkPaperProcessingMode;};
  void SetWorkPaperProcessingMode(TWorkPaperProcessingMode nWorkPaperProcessingMode){m_nWorkPaperProcessingMode = nWorkPaperProcessingMode;};
  // addon value
  xtstring GetAddOnValue(){return m_sAddOnValue;};
  void SetAddOnValue(const xtstring& sAddOnValue){m_sAddOnValue = sAddOnValue;};
  // target for template
  TTemplateTarget GetTargetForTemplate(){return m_nTargetForTemplate;};
  void SetTargetForTemplate(TTemplateTarget nTargetForTemplate){m_nTargetForTemplate = nTargetForTemplate;};
  // save template in format
  TSaveTemplateInFormat GetSaveTemplateInFormat(){return m_nSaveTemplateInFormat;};
  void SetSaveTemplateInFormat(TSaveTemplateInFormat nSaveTemplateInFormat){m_nSaveTemplateInFormat = nSaveTemplateInFormat;};
  // template file icon
  int GetTemplateFileIcon(){return m_nTemplateFileIcon;}
  void SetTemplateFileIcon(int nFileIcon){m_nTemplateFileIcon = nFileIcon;};
  // template file show name
  CConditionOrAssignment&         GetTemplateFileShowName(){return m_caTemplateFileShowName;};
  const CConditionOrAssignment&   GetTemplateFileShowName_Const() const {return m_caTemplateFileShowName;};
  CConditionOrAssignment*         GetTemplateFileShowName_Ptr(){return &m_caTemplateFileShowName;};
  // template file
  CConditionOrAssignment&         GetTemplateFile(){return m_caTemplateFile;};
  const CConditionOrAssignment&   GetTemplateFile_Const() const {return m_caTemplateFile;};
  CConditionOrAssignment*         GetTemplateFile_Ptr(){return &m_caTemplateFile;};
  // template folder
  CConditionOrAssignment&         GetTemplateFolder(){return m_caTemplateFolder;};
  const CConditionOrAssignment&   GetTemplateFolder_Const() const {return m_caTemplateFolder;};
  CConditionOrAssignment*         GetTemplateFolder_Ptr(){return &m_caTemplateFolder;};
  // template symbolic db name
  xtstring GetTemplateSymbolicDBName(){return m_sTemplateSymbolicDBName;};
  void SetTemplateSymbolicDBName(const xtstring& sName){m_sTemplateSymbolicDBName = sName;};
  // template database name
  CConditionOrAssignment&         GetTemplateDBName(){return m_caTemplateDBName;};
  const CConditionOrAssignment&   GetTemplateDBName_Const() const {return m_caTemplateDBName;};
  CConditionOrAssignment*         GetTemplateDBName_Ptr(){return &m_caTemplateDBName;};

  void SetOwnerEntity(CIFSEntityInd* pOwnerEntity){m_pOwnerEntity = pOwnerEntity;};
  xtstring GetOwnerEntityName() const;
  CIFSEntityInd* GetOwnerEntity() const {return m_pOwnerEntity;};
protected:
  CIFSEntityInd* m_pOwnerEntity;

protected:
  THitType                m_nHitType;
  CConditionOrAssignment  m_caHitName;

  bool                m_bOverloadProperties;
  bool                m_bOverloadedPropertiesChanged;
  ///////////////////////////////////////////////
  // properties from stone or template
  CConditionOrAssignment    m_caVisibility;
  xtstring                  m_sInformationSight;
  bool                      m_bUseInformationSight;
  xtstring                  m_sIFSTargetEntity;
  bool                      m_bUseIFSTargetEntity;
  xtstring                  m_sStoneOrTemplate;
  bool                      m_bUseStoneOrTemplate;
  xtstring                  m_sStoneOrTemplateRef;
  TWorkPaperCopyType        m_nWorkPaperCopyType; // Art, wie Baustein nach Ziel kopiert wird
  TWorkPaperProcessingMode  m_nWorkPaperProcessingMode; // Art, wann Baustein nach Zeile kopiert wird
  xtstring                  m_sAddOnValue;

  TTemplateTarget         m_nTargetForTemplate;
  TSaveTemplateInFormat   m_nSaveTemplateInFormat;
  int                     m_nTemplateFileIcon; // 0 -> use original
  CConditionOrAssignment  m_caTemplateFileShowName;
  CConditionOrAssignment  m_caTemplateFile;
  CConditionOrAssignment  m_caTemplateFolder;
  xtstring                m_sTemplateSymbolicDBName;
  CConditionOrAssignment  m_caTemplateDBName;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj();
  virtual void ROC_PropertyChanged_ER();
  virtual void ROC_PropertyChanged_IFS();
  virtual void ROC_PropertyChanged_WP();
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);

  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWP(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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
// CIFSEntityConstantForTextForm
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityConstantForTextForm
{
public:
  CIFSEntityConstantForTextForm();
  ~CIFSEntityConstantForTextForm();

  bool AssignFrom(const CIFSEntityConstantForTextForm* pConstant);
  bool CopyFrom(const CIFSEntityConstantForTextForm* pConstant, bool& bChanged);

  bool operator == (const CIFSEntityConstantForTextForm& cConstant) const;
  bool operator != (const CIFSEntityConstantForTextForm& cConstant) const;

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  bool IsIdenticalForXPubClient(const CIFSEntityConstantForTextForm* pConst) const;

  xtstring      m_sName;
  TFieldType    m_nFieldType;
  xtstring      m_sFormat;
  long          m_nMaxChars;  // -1 no max
  MODEL_COLOR   m_cBackground;
  MODEL_COLOR   m_cForeground;
  bool          m_bAllowNull;
  bool          m_bBorder;
  xtstring      m_sDefaultText;
  xtstring      m_sFontName;
  int           m_nFontHeight;
  // diese Variable hat zu Entwicklungszeit keine Wirkung
  // und gar nicht dargestellt
  xtstring      m_sValue;
  // help variables
  int           m_nCompoundIndexInSymbolsValuesOffsets;
};

typedef std::vector<CIFSEntityConstantForTextForm*>   CIFSEntityTextFormConstants;
typedef CIFSEntityTextFormConstants::iterator         CIFSEntityTextFormConstantsIterator;
typedef CIFSEntityTextFormConstants::reverse_iterator CIFSEntityTextFormConstantsReverseIterator;
typedef CIFSEntityTextFormConstants::const_iterator   CIFSEntityTextFormConstantsConstIterator;

//////////////////////////////////////////////////////////////////////
// CIFSEntityOneConditionVariation
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityOneConditionVariation : public CModelBaseInd,
                                                               public CSyncNotifier_IFS,
                                                               public CSyncResponse_Proj,
                                                               public CSyncResponse_ER,
                                                               public CSyncResponse_IFS
{
  friend class CIFSEntityConditionVariations;
public:
  CIFSEntityOneConditionVariation(CSyncIFSNotifiersGroup* pGroup, CIFSEntityConditionVariations* pParent);
  CIFSEntityOneConditionVariation(CIFSEntityOneConditionVariation& cOneCond);
  CIFSEntityOneConditionVariation(CIFSEntityOneConditionVariation* pOneCond);
  virtual ~CIFSEntityOneConditionVariation();

  bool AssignFrom(const CIFSEntityOneConditionVariation* pOneCond);
  bool CopyFrom(const CIFSEntityOneConditionVariation* pOneCond, bool& bChanged);

  bool operator == (const CIFSEntityOneConditionVariation& cOneCond) const;
  bool operator != (const CIFSEntityOneConditionVariation& cOneCond) const;

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetName() const {return m_sName;};
  void SetName(const xtstring& sText){m_sName = sText;};
  xtstring GetDescription() const {return m_sDescription;};
  void SetDescription(const xtstring& sText){m_sDescription = sText;};

  CConditionOrAssignment&         GetCondition(){return m_caCondition;};
  const CConditionOrAssignment&   GetCondition_Const() const {return m_caCondition;};
  CConditionOrAssignment*         GetCondition_Ptr(){return &m_caCondition;};

  bool GetDontOptimizeCondition() const {return m_bDontOptimizeCondition;};
  void SetDontOptimizeCondition(bool bDontOptimizeCondition){m_bDontOptimizeCondition = bDontOptimizeCondition;}
  bool GetDontShowCondition() const {return m_bDontShowCondition;};
  void SetDontShowCondition(bool bDontShowCondition){m_bDontShowCondition = bDontShowCondition;};
  TConditionShowMethod GetConditionShowMethod() const {return m_nConditionShowMethod;};
  void SetConditionShowMethod(TConditionShowMethod nConditionShowMethod){m_nConditionShowMethod = nConditionShowMethod;};
  xtstring GetTextFormOfCondition() const {return m_sTextFormOfCondition;};
  void SetTextFormOfCondition(const xtstring& sTextFormOfCondition){m_sTextFormOfCondition = sTextFormOfCondition;};

  bool IsIdenticalForXPubClient(const CIFSEntityOneConditionVariation* pVariation) const;

  size_t CountOfConstants() const {return m_cConstants.size();};
  bool AddConstant(const CIFSEntityConstantForTextForm* pConstant);
  CIFSEntityConstantForTextForm* GetConstant(size_t nIndex) const;
  CIFSEntityConstantForTextForm* GetConstant(const xtstring& sName) const;
  xtstring GetConstantNameForTextForm(const xtstring& sConstantName) const;
  bool GetPositionOfNameForTextFormInTextForm(const xtstring& sConstantNameForTextForm, int& nPos, int& nLen);
  bool GetPositionOfConstantInExpression(const xtstring& sConstantName, int& nPos, int& nLen);

  bool AssignTextFormFrom(const CIFSEntityOneConditionVariation* pOneCond);
  bool CopyTextFormFrom(const CIFSEntityOneConditionVariation* pOneCond, bool& bChanged);

  void ExpressionChanged();
protected:
  xtstring GetNewNameForConstant();
  xtstring GetNameForConstant(int nIndex);
  void RemoveAllConstants();
  void RemoveLastConstant();

  CIFSEntityConditionVariations* m_pParent;
  xtstring                m_sName;
  xtstring                m_sDescription;
  CConditionOrAssignment  m_caCondition;
  bool                    m_bDontOptimizeCondition;
  bool                    m_bDontShowCondition;            // ganze Bedingung bei Client wird nicht dargestellt
  TConditionShowMethod    m_nConditionShowMethod;
  xtstring                m_sTextFormOfCondition;
  CIFSEntityTextFormConstants m_cConstants;

protected:
  virtual void ROC_PropertyChanged_Proj();
  // requirement on change
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObject* pObject);

  virtual void ROC_PropertyChanged_ER();
  // requirement on change
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);

  virtual void ROC_PropertyChanged_IFS();
  // requirement on change
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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

typedef std::vector<CIFSEntityOneConditionVariation*> CIFSConditionVariations;
typedef CIFSConditionVariations::iterator             CIFSConditionVariationsIterator;
typedef CIFSConditionVariations::reverse_iterator     CIFSConditionVariationsReverseIterator;
typedef CIFSConditionVariations::const_iterator       CIFSConditionVariationsConstIterator;

//////////////////////////////////////////////////////////////////////
// CIFSEntityConditionVariations
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityConditionVariations : public CModelBaseInd,
                                                             public CSyncNotifier_IFS,
                                                             public CSyncResponse_IFS
{
  friend class CIFSEntityOneConditionVariation;
  friend class CIFSEntityInd;
public:
  CIFSEntityConditionVariations(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityConditionVariations(CIFSEntityConditionVariations& cVariations);
  CIFSEntityConditionVariations(CIFSEntityConditionVariations* pVariations);
  virtual ~CIFSEntityConditionVariations();

  bool AssignFrom(const CIFSEntityConditionVariations* pVariations);
  bool CopyFrom(const CIFSEntityConditionVariations* pVariations, bool& bChanged);

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetActiveConditionName();
  void SetActiveConditionName(xtstring sName){m_sActiveConditionName = sName;};

  bool GetUseDetermineViaEntityAndVariable(){return m_bUseDetermineViaEntityAndVariable;};
  void SetUseDetermineViaEntityAndVariable(bool bUseDetermineViaEntityAndVariable){m_bUseDetermineViaEntityAndVariable = bUseDetermineViaEntityAndVariable;};
  xtstring GetDetermineViaEntity(){return m_sDetermineViaEntity;};
  void SetDetermineViaEntity(const xtstring& sDetermineViaEntity){m_sDetermineViaEntity = sDetermineViaEntity;};
  xtstring GetDetermineViaVariable(){return m_sDetermineViaVariable;};
  void SetDetermineViaVariable(const xtstring& sDetermineViaVariable){m_sDetermineViaVariable = sDetermineViaVariable;};

//  bool MigrateToOpenedEntity(const xtstring& sCondName, const xtstring& sExpr, const xtstring& sExprValues, const xtstring& sExprSymbols, const xtstring& sExprOffsets);
  bool IsIdenticalForXPubClient(const CIFSEntityConditionVariations* pCondVariations) const;

  size_t CountOfVariations() const {return m_cVariations.size();};
  size_t GetIndexOfItem(const CIFSEntityOneConditionVariation* pItem) const;
  const CIFSConditionVariations* GetConditionVariations(){return &m_cVariations;};
  CIFSEntityOneConditionVariation* GetActiveCondition();
  const CIFSEntityOneConditionVariation* GetConstActiveCondition() const;
  CIFSEntityOneConditionVariation* GetCondition(size_t nIndex);
  const CIFSEntityOneConditionVariation* GetConstCondition(size_t nIndex) const;
  CIFSEntityOneConditionVariation* GetCondition(const xtstring& sName);
  const CIFSEntityOneConditionVariation* GetConstCondition(const xtstring& sName) const;
  bool AddCondition(const CIFSEntityOneConditionVariation* pOneCond);
  bool RemoveCondition(size_t nIndex);
  bool RemoveCondition(const xtstring& sName);

  void SetOwnerEntity(CIFSEntityInd* pOwnerEntity){m_pOwnerEntity = pOwnerEntity;};
  xtstring GetOwnerEntityName() const;
  CIFSEntityInd* GetOwnerEntity() const {return m_pOwnerEntity;};
protected:
  CIFSEntityInd* m_pOwnerEntity;

protected:
  void RemoveAll();

  xtstring                m_sActiveConditionName;
  CIFSConditionVariations m_cVariations;
  bool                    m_bUseDetermineViaEntityAndVariable;
  xtstring                m_sDetermineViaEntity;
  xtstring                m_sDetermineViaVariable;

protected:
  virtual void ROC_PropertyChanged_IFS(){};
  // requirement on change
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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
// CIFSEntityParameterForTextForm
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityParameterForTextForm
{
public:
  CIFSEntityParameterForTextForm();
  ~CIFSEntityParameterForTextForm();

  bool AssignFrom(const CIFSEntityParameterForTextForm* pParameter);
  bool CopyFrom(const CIFSEntityParameterForTextForm* pParameter, bool& bChanged);

  bool operator == (const CIFSEntityParameterForTextForm& cParameter) const;
  bool operator != (const CIFSEntityParameterForTextForm& cParameter) const;

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  bool IsIdenticalForXPubClient(const CIFSEntityParameterForTextForm* pParameter) const;
  bool AssignFromERModelParamInd(CERModelParamInd* pParam);
  bool AssignFromChangedERModelParamInd(CERModelParamInd* pParam);

  xtstring                m_sName;
  xtstring                m_sTag;
  bool                    m_bShowParam;
  CConditionOrAssignment  m_caAssignment;
  TFieldType              m_nFieldType;
  xtstring                m_sFormat;
  long                    m_nMaxChars;  // -1 no max
  MODEL_COLOR             m_cBackground;
  MODEL_COLOR             m_cForeground;
  bool                    m_bAllowNull;
  bool                    m_bBorder;
  xtstring                m_sDefaultText;
  xtstring                m_sFontName;
  int                     m_nFontHeight;
  // diese Variable hat zu Entwicklungszeit keine Wirkung
  // und gar nicht dargestellt
  xtstring                m_sValue;
};

typedef std::vector<CIFSEntityParameterForTextForm*>    CIFSEntityTextFormParameters;
typedef CIFSEntityTextFormParameters::iterator          CIFSEntityTextFormParametersIterator;
typedef CIFSEntityTextFormParameters::reverse_iterator  CIFSEntityTextFormParametersReverseIterator;
typedef CIFSEntityTextFormParameters::const_iterator    CIFSEntityTextFormParametersConstIterator;

//////////////////////////////////////////////////////////////////////
// CIFSEntityAllParameters
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityAllParameters : public CModelBaseInd,
                                                       public CSyncNotifier_IFS,
                                                       public CSyncResponse_Proj,
                                                       public CSyncResponse_ER,
                                                       public CSyncResponse_IFS
{
public:
  CIFSEntityAllParameters(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityAllParameters(CIFSEntityAllParameters& cParameters);
  CIFSEntityAllParameters(CIFSEntityAllParameters* pParameters);
  ~CIFSEntityAllParameters();

  bool AssignFrom(const CIFSEntityAllParameters* cParameters);
  bool CopyFrom(const CIFSEntityAllParameters* cParameters, bool& bChanged);

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetName() const {return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetTextFormOfParameters() const {return m_sTextFormOfParameters;};
  void SetTextFormOfParameters(const xtstring& sTextFormOfParameters){m_sTextFormOfParameters = sTextFormOfParameters;};
  bool GetShowTextForm() const {return m_bShowTextForm;};
  void SetShowTextForm(bool bShowTextForm){m_bShowTextForm = bShowTextForm;};
  bool GetSPSQLAttached() const {return m_bSPSQLAttached;};
  void SetSPSQLAttached(bool bSPSQLAttached){m_bSPSQLAttached = bSPSQLAttached;};

  bool IsIdenticalForXPubClient(const CIFSEntityAllParameters* pParameters) const;

  size_t CountOfParameters() const {return m_cTextFormParameters.size();};
  CIFSEntityParameterForTextForm* GetParameter(size_t nIndex);
  const CIFSEntityParameterForTextForm* GetConstParameter(size_t nIndex) const;
  CIFSEntityParameterForTextForm* GetParameter(const xtstring& sName);
  const CIFSEntityParameterForTextForm* GetConstParameter(const xtstring& sName) const;
  bool AddParameter(const CIFSEntityParameterForTextForm* pParameter);
  bool RemoveParameter(size_t nIndex);
  bool RemoveParameter(const xtstring& sName);

  void RemoveAllParameters();

  xtstring GetParameterNameForTextForm(const xtstring& sParameterName) const;
  bool GetPositionOfNameForTextFormInTextForm(const xtstring& sParameterNameForTextForm, int& nPos, int& nLen);
  bool GetPositionOfNameForSyncEngineInTextForm(const xtstring& sParameterNameForTextForm, const xtstring& sParamName, int& nPos, int& nLen);
  void SettingOfERModelChanged(CERModelTableInd* pERModelTable);

  void SetOwnerEntity(CIFSEntityInd* pOwnerEntity){m_pOwnerEntity = pOwnerEntity;};
  xtstring GetOwnerEntityName() const;
  CIFSEntityInd* GetOwnerEntity() const {return m_pOwnerEntity;};
protected:
  CIFSEntityInd* m_pOwnerEntity;

protected:

  xtstring                      m_sName;
  xtstring                      m_sTextFormOfParameters;
  bool                          m_bShowTextForm;
  bool                          m_bSPSQLAttached;
  CIFSEntityTextFormParameters  m_cTextFormParameters;

protected:
  virtual void ROC_PropertyChanged_Proj();
  // requirement on change
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObject* pObject);

  virtual void ROC_PropertyChanged_ER();
  // requirement on change
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);

  virtual void ROC_PropertyChanged_IFS();
  // requirement on change
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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
// CIFSEntityOneSQLCommandVariation
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityOneSQLCommandVariation : public CModelBaseInd,
                                                                public CSyncNotifier_IFS
{
  friend class CIFSEntitySQLCommandVariations;
public:
  CIFSEntityOneSQLCommandVariation(CSyncIFSNotifiersGroup* pGroup, CIFSEntitySQLCommandVariations* pParent);
  CIFSEntityOneSQLCommandVariation(CIFSEntityOneSQLCommandVariation& cSQLCommand);
  CIFSEntityOneSQLCommandVariation(CIFSEntityOneSQLCommandVariation* pSQLCommand);
  virtual ~CIFSEntityOneSQLCommandVariation();

  bool AssignFrom(const CIFSEntityOneSQLCommandVariation* pSQLCommand);
  bool CopyFrom(const CIFSEntityOneSQLCommandVariation* pSQLCommand, bool& bChanged);

  bool operator == (const CIFSEntityOneSQLCommandVariation& cSQLCommand) const;
  bool operator != (const CIFSEntityOneSQLCommandVariation& cSQLCommand) const;

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetName() const {return m_sName;};
  void SetName(const xtstring& sText){m_sName = sText;};
  xtstring GetDescriptionShort() const {return m_sDescriptionShort;};
  void SetDescriptionShort(const xtstring& sText){m_sDescriptionShort = sText;};
  xtstring GetDescriptionLong() const {return m_sDescriptionLong;};
  void SetDescriptionLong(const xtstring& sText){m_sDescriptionLong = sText;};
  xtstring GetSQLCommand() const {return m_sSQLCommand;};
  void SetSQLCommand(const xtstring& sSQLCommand){m_sSQLCommand = sSQLCommand;};
  xtstring GetFreeSelectionData() const {return m_sFreeSelectionData;};
  void SetFreeSelectionData(const xtstring& sFreeSelectionData){m_sFreeSelectionData = sFreeSelectionData;};
  bool GetDontShowSQLCommand() const {return m_bDontShowSQLCommand;};
  void SetDontShowSQLCommand(bool bDontShowSQLCommand){m_bDontShowSQLCommand = bDontShowSQLCommand;};
  TSQLCommandShowMethod GetSQLCommandShowMethod() const {return m_nSQLCommandShowMethod;};
  void SetSQLCommandShowMethod(TSQLCommandShowMethod nSQLCommandShowMethod){m_nSQLCommandShowMethod = nSQLCommandShowMethod;};

  bool IsIdenticalForXPubClient(const CIFSEntityOneSQLCommandVariation* pVariation) const;
protected:
  CIFSEntitySQLCommandVariations* m_pParent;
  xtstring              m_sName;
  xtstring              m_sDescriptionShort;
  xtstring              m_sDescriptionLong;
  xtstring              m_sSQLCommand;
  xtstring              m_sFreeSelectionData;
  bool                  m_bDontShowSQLCommand;            // SQLCommand wird bei Client nicht dargestellt
  TSQLCommandShowMethod m_nSQLCommandShowMethod;

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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

typedef std::vector<CIFSEntityOneSQLCommandVariation*>  CIFSSQLCommandVariations;
typedef CIFSSQLCommandVariations::iterator              CIFSSQLCommandVariationsIterator;
typedef CIFSSQLCommandVariations::reverse_iterator      CIFSSQLCommandVariationsReverseIterator;
typedef CIFSSQLCommandVariations::const_iterator        CIFSSQLCommandVariationsConstIterator;

//////////////////////////////////////////////////////////////////////
// CIFSEntitySQLCommandVariations
//////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntitySQLCommandVariations : public CModelBaseInd,
                                                              public CSyncNotifier_IFS,
                                                              public CSyncResponse_IFS
{
  friend class CIFSEntityOneSQLCommandVariation;
  friend class CIFSEntityInd;
public:
  CIFSEntitySQLCommandVariations(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntitySQLCommandVariations(CIFSEntitySQLCommandVariations& cVariations);
  CIFSEntitySQLCommandVariations(CIFSEntitySQLCommandVariations* pVariations);
  virtual ~CIFSEntitySQLCommandVariations();

  bool AssignFrom(const CIFSEntitySQLCommandVariations* pVariations);
  bool CopyFrom(const CIFSEntitySQLCommandVariations* pVariations, bool& bChanged);

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetActiveSQLCommandName();
  void SetActiveSQLCommandName(xtstring sName){m_sActiveSQLCommandName = sName;};

  bool GetUseDetermineViaEntityAndVariable(){return m_bUseDetermineViaEntityAndVariable;};
  void SetUseDetermineViaEntityAndVariable(bool bUseDetermineViaEntityAndVariable){m_bUseDetermineViaEntityAndVariable = bUseDetermineViaEntityAndVariable;};
  xtstring GetDetermineViaEntity(){return m_sDetermineViaEntity;};
  void SetDetermineViaEntity(const xtstring& sDetermineViaEntity){m_sDetermineViaEntity = sDetermineViaEntity;};
  xtstring GetDetermineViaVariable(){return m_sDetermineViaVariable;};
  void SetDetermineViaVariable(const xtstring& sDetermineViaVariable){m_sDetermineViaVariable = sDetermineViaVariable;};

  bool IsIdenticalForXPubClient(const CIFSEntitySQLCommandVariations* pSQLVariations) const;

  size_t CountOfVariations() const {return m_cVariations.size();};
  size_t GetIndexOfItem(const CIFSEntityOneSQLCommandVariation* pItem) const;
  const CIFSSQLCommandVariations* GetSQLCommandVariations(){return &m_cVariations;};
  CIFSEntityOneSQLCommandVariation* GetActiveSQLCommand();
  const CIFSEntityOneSQLCommandVariation* GetConstActiveSQLCommand() const;
  CIFSEntityOneSQLCommandVariation* GetSQLCommand(size_t nIndex);
  const CIFSEntityOneSQLCommandVariation* GetConstSQLCommand(size_t nIndex) const;
  CIFSEntityOneSQLCommandVariation* GetSQLCommand(const xtstring& sName);
  const CIFSEntityOneSQLCommandVariation* GetConstSQLCommand(const xtstring& sName) const;

  bool AddSQLCommand(const CIFSEntityOneSQLCommandVariation* pSQLCommand);
  bool RemoveSQLCommand(size_t nIndex);
  bool RemoveSQLCommand(const xtstring& sName);

  void SetOwnerEntity(CIFSEntityInd* pOwnerEntity){m_pOwnerEntity = pOwnerEntity;};
  xtstring GetOwnerEntityName() const;
  CIFSEntityInd* GetOwnerEntity() const {return m_pOwnerEntity;};
protected:
  CIFSEntityInd* m_pOwnerEntity;

protected:
  void RemoveAll();

  xtstring                  m_sActiveSQLCommandName;
  CIFSSQLCommandVariations  m_cVariations;
  bool                      m_bUseDetermineViaEntityAndVariable;
  xtstring                  m_sDetermineViaEntity;
  xtstring                  m_sDetermineViaVariable;

protected:
  virtual void ROC_PropertyChanged_IFS(){};
  // requirement on change
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged(){};
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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





typedef std::vector<CIFSEntityFieldInd*>    CIFSEntityFields;
typedef CIFSEntityFields::iterator          CIFSEntityFieldsIterator;
typedef CIFSEntityFields::reverse_iterator  CIFSEntityFieldsReverseIterator;
typedef CIFSEntityFields::const_iterator    CIFSEntityFieldsConstIterator;

typedef std::vector<CIFSEntityConnectionInd*>   CIFSEntityConnections;
typedef CIFSEntityConnections::iterator         CIFSEntityConnectionsIterator;
typedef CIFSEntityConnections::reverse_iterator CIFSEntityConnectionsReverseIterator;
typedef CIFSEntityConnections::const_iterator   CIFSEntityConnectionsConstIterator;

typedef std::vector<CIFSHitNameInd*>    CIFSHitNames;
typedef CIFSHitNames::iterator          CIFSHitNamesIterator;
typedef CIFSHitNames::reverse_iterator  CIFSHitNamesReverseIterator;
typedef CIFSHitNames::const_iterator    CIFSHitNamesConstIterator;





/////////////////////////////////////////////////////////////////////////////
// CIFSEntityInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CIFSEntityChannelToGUI
{
public:
  CIFSEntityChannelToGUI(){};
  virtual ~CIFSEntityChannelToGUI(){};

  virtual void PropertiesChanged() = 0;
};
class XPUBMODEL_EXPORTIMPORT CIFSEntityInd : public CModelBaseInd,
                                             public CSyncNotifier_IFS,
                                             public CSyncResponse_Common,
                                             public CSyncResponse_Proj,
                                             public CSyncResponse_ER,
                                             public CSyncResponse_IFS,
                                             public CSyncResponse_WP
{
  friend class CIFSEntityAllParameters;
  friend class CIFSEntityOneConditionVariation;
  friend class CIFSHitNameInd;
  friend class CIFSEntityAssignmentInd;
  friend class CIFSEntityVariableInd;
  friend class CIFSEntityGlobalERTableInd;
public:
  CIFSEntityInd(CSyncIFSNotifiersGroup* pGroup);
  CIFSEntityInd(CIFSEntityInd& cTable);
  CIFSEntityInd(CIFSEntityInd* pTable);
  virtual ~CIFSEntityInd();

  virtual const CIFSEntityInd* GetIFSEntityInd(){return this;};

  void SetChannelToGUI(CIFSEntityChannelToGUI* pChannelToGUI){m_pChannelToGUI = pChannelToGUI;};

  void SetIFSEntityInd(CIFSEntityInd* pTable, bool bWithConnections = true);
  xtstring GetXMLDoc();
  bool SetXMLDoc(const xtstring& sXMLDoc);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);
  void CopyDiverseProperties(CIFSEntityInd* pTable);
  void CopyPositionProperties(CIFSEntityInd* pTable);

  /////////////////////////////////////////////////////////////
  // interface fo common properties
  // Name
  xtstring GetName() const {return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  TTypeOfEntity GetTypeOfEntity() const {return m_nTypeOfEntity;};
  void SetTypeOfEntity(TTypeOfEntity nTypeOfEntity){m_nTypeOfEntity = nTypeOfEntity;};
  xtstring GetSubIFS() const {return m_sSubIFS;};
  void SetSubIFS(const xtstring& sText){m_sSubIFS = sText;};

  CConditionOrAssignment&         GetPortCondition(){return m_caPortCondition;};
  const CConditionOrAssignment&   GetPortCondition_Const() const {return m_caPortCondition;};
  CConditionOrAssignment*         GetPortCondition_Ptr(){return &m_caPortCondition;};

  TReadMethod GetReadMethod() const {return m_nReadMethod;};
  void SetReadMethod(TReadMethod nReadMethod){m_nReadMethod = nReadMethod;};

  xtstring GetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw() const {return m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw;};
  void SetERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw(const xtstring& sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName){m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw = sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName;};
  xtstring GetERTable_FromERTableOrGlobalTable();
  bool GetERTableUsed() const;
  bool GetGlobalTableUsed() const;
  bool GetPropStoneTypeUsed() const;
  bool GetDBStoneNameUsed() const;
  xtstring GetERTableFromRaw() const;
  void SetERTableToRaw(const xtstring& sERTable);
  xtstring GetGlobalTableFromRaw() const;
  void SetGlobalTableToRaw(const xtstring& sGlobalTable);
  int GetPropStoneTypeFromRaw() const;
  void SetPropStoneTypeToRaw(int nPropStoneType);
  xtstring GetDBStoneNameFromRaw() const;
  void SetDBStoneNameToRaw(const xtstring& sDBStoneName);


  CConditionOrAssignment&         GetERTableRealName(){return m_caERTableRealName;};
  const CConditionOrAssignment&   GetERTableRealName_Const() const {return m_caERTableRealName;};
  CConditionOrAssignment*         GetERTableRealName_Ptr(){return &m_caERTableRealName;};

  xtstring GetIdentity() const {return m_sIdentity;};
  void SetIdentity(const xtstring& sIdentity){m_sIdentity = sIdentity;};

  CConditionOrAssignment&         GetMainCondition(){return m_caMainCondition;};
  const CConditionOrAssignment&   GetMainCondition_Const() const {return m_caMainCondition;};
  CConditionOrAssignment*         GetMainCondition_Ptr(){return &m_caMainCondition;};

  bool GetDontOptimizeCondition() const {return m_bDontOptimizeCondition;};
  void SetDontOptimizeCondition(bool bDontOptimizeCondition){m_bDontOptimizeCondition = bDontOptimizeCondition;};
  CIFSEntityConditionVariations* GetConditionVariations(){return &m_cConditionVariations;};
  CIFSEntityAllParameters* GetEntityParameters(){return &m_cParameters;};
  CIFSEntitySQLCommandVariations* GetSQLCommandVariations(){return &m_cSQLCommandVariations;};
  xtstring GetWhereClause() const {return m_sWhereClause;};
  void SetWhereClause(const xtstring& sText){m_sWhereClause = sText;};
  xtstring GetSortField() const {return m_sSortField;};
  void SetSortField(const xtstring& sSortField){m_sSortField = sSortField;};
  bool GetSortDescending() const {return m_bSortDescending;};
  void SetSortDescending(bool bSortDescending){m_bSortDescending = bSortDescending;};

  xtstring GetComment() const {return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
  bool GetMainEntity() const {return m_bMainEntity;};
  void SetMainEntity(bool bMainEntity){m_bMainEntity = bMainEntity;};
  bool GetDebugMode() const {return m_bDebugMode;};
  void SetDebugMode(bool bDebugMode){m_bDebugMode = bDebugMode;};
  bool GetOpenedEntity() const {return m_bOpenedEntity;};
  void SetOpenedEntity(bool bOpened){m_bOpenedEntity = bOpened;};
  xtstring GetOpenedEntityName() const {return m_sOpenedEntityName;};
  void SetOpenedEntityName(const xtstring& sName){m_sOpenedEntityName = sName;};
  bool GetShowExpanded() const {return m_bShowExpanded;};
  void SetShowExpanded(bool bShowExpanded){m_bShowExpanded = bShowExpanded;};
  TShowBehaviour GetShowBehaviour() const {return m_nShowBehaviour;};
  void SetShowBehaviour(TShowBehaviour nShowBehaviour){m_nShowBehaviour = nShowBehaviour;};
  bool GetShowAddCondition() const {return m_bShowAddCondition;};
  void SetShowAddCondition(bool bShowAddCondition){m_bShowAddCondition = bShowAddCondition;};
  bool GetShowAddFreeSelection() const {return m_bShowAddFreeSelection;};
  void SetShowAddFreeSelection(bool bShowAddFreeSelection){m_bShowAddFreeSelection = bShowAddFreeSelection;};

  size_t CountOfFields(){return m_arrFields.size();};
  CIFSEntityFieldInd* GetField(size_t nIndex){assert(nIndex < m_arrFields.size()); if (nIndex < m_arrFields.size()) return m_arrFields[nIndex]; else return NULL;};
  CIFSEntityFieldInd* GetField(const xtstring& sName);
  void AddField(CIFSEntityFieldInd *pField){assert(pField);
                                      CIFSEntityFieldInd* ppField;
                                      ppField = new CIFSEntityFieldInd(pField);
                                      ppField->SetSyncGroup(GetSyncGroup());
                                      m_arrFields.push_back(ppField);};
  bool RemoveField(size_t nIndex);
  bool RemoveField(const xtstring& sName);
  bool RemoveAllFields();

  size_t CountOfConnections(){return m_arrConnections.size();};
  CIFSEntityConnectionInd* GetConnection(size_t nIndex){assert(nIndex < m_arrConnections.size()); if (nIndex < m_arrConnections.size()) return m_arrConnections[nIndex]; else return NULL;};
  CIFSEntityConnectionInd* GetConnection(const xtstring& sName);
  void AddConnection(CIFSEntityConnectionInd *pConnection){assert(pConnection);
                                                     CIFSEntityConnectionInd* ppConnection;
                                                     ppConnection = new CIFSEntityConnectionInd(pConnection);
                                                     ppConnection->SetSyncGroup(GetSyncGroup());
                                                     m_arrConnections.push_back(ppConnection);};
  bool RemoveConnection(CIFSEntityConnectionInd *pConnection);
  bool RemoveAllConnections();

  size_t CountOfConnectionsToChilds(){return m_arrConnectionsToChilds.size();};
  CIFSEntityConnectionInd* GetConnectionToChilds(size_t nIndex){assert(nIndex < m_arrConnectionsToChilds.size()); if (nIndex < m_arrConnectionsToChilds.size()) return m_arrConnectionsToChilds[nIndex]; else return NULL;};
  CIFSEntityConnectionInd* GetConnectionToChilds(const xtstring& sName, xtstring sChildEntity);
  void AddConnectionToChilds(CIFSEntityConnectionInd *pConnection){assert(pConnection);
                                                             CIFSEntityConnectionInd* ppConnection;
                                                             ppConnection = new CIFSEntityConnectionInd(pConnection);
                                                             ppConnection->SetSyncGroup(GetSyncGroup());
                                                             m_arrConnectionsToChilds.push_back(ppConnection);};
  bool RemoveConnectionToChilds(CIFSEntityConnectionInd *pConnection);
  bool RemoveAllConnectionsToChilds();
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

  // hit condition
  CConditionOrAssignment&         GetHitCondition(){return m_caHitCondition;};
  const CConditionOrAssignment&   GetHitCondition_Const() const {return m_caHitCondition;};
  CConditionOrAssignment*         GetHitCondition_Ptr(){return &m_caHitCondition;};

  TChildExecution GetChildExecution(){return m_nChildExecution;};
  void SetChildExecution(TChildExecution nChildExecution){m_nChildExecution = nChildExecution;};

  size_t CountOfHitNames(){return m_arrHitNames.size();};
  CIFSHitNameInd* GetHitName(size_t nIndex){assert(nIndex < m_arrHitNames.size()); if (nIndex < m_arrHitNames.size()) return m_arrHitNames[nIndex]; else return NULL;};
  void AddHitName(CIFSHitNameInd *pName){assert(pName);
                                         CIFSHitNameInd* ppName;
                                         ppName = new CIFSHitNameInd(pName);
                                         ppName->SetOwnerEntity(this);
                                         ppName->SetSyncGroup(GetSyncGroup());
                                         m_arrHitNames.push_back(ppName);};
  bool RemoveHitName(CIFSHitNameInd *pName);
  bool RemoveAllHitNames();

  bool GetAllHitStones(CxtstringVector& vecNames);
  bool GetAllHitTemplates(CxtstringVector& vecNames);

  // parameter table values (only main entity)
  const CxtstringMap& GetParamTableFields(){return m_mapParamTableValues;};
  size_t GetCountOfParamTableValues(){return m_mapParamTableValues.size();};
  bool GetParamTableFieldName(size_t nIndex, xtstring& sName);
  bool GetParamTableFieldValue(size_t nIndex, xtstring& sValue);
  bool GetParamTableFieldValue(const xtstring& sFieldName, xtstring& sValue);
  bool AddParamTableFieldValue(const xtstring& sFieldName, const xtstring& sFieldValue);
  bool RemoveAllParamTableFieldValues();
  bool AppendParamTableFieldValues(const CxtstringMap& cMap);

  virtual void PropertiesChanged();

  size_t CountOfIFSVariableInds(){return m_arrVariables.size();};
  CIFSEntityVariableInd* GetIFSVariableInd(size_t nIndex){assert(nIndex < m_arrVariables.size()); if (nIndex < m_arrVariables.size()) return m_arrVariables[nIndex]; else return NULL;};
  CIFSEntityVariableInd* GetIFSVariableInd(const xtstring& sName);
  void AddIFSVariableInd(CIFSEntityVariableInd* pModel){assert(pModel);
                                                        CIFSEntityVariableInd* ppModel;
                                                        ppModel = new CIFSEntityVariableInd(pModel);
                                                        ppModel->SetOwnerEntity(this);
                                                        ppModel->SetSyncGroup(GetSyncGroup());
                                                        m_arrVariables.push_back(ppModel);};
  bool RemoveIFSVariableInd(size_t nIndex);
  bool RemoveIFSVariableInd(const xtstring& sName);
  bool RemoveAllIFSVariableInds();

  size_t CountOfIFSAssignmentInds(){return m_arrAssignments.size();};
  CIFSEntityAssignmentInd* GetIFSAssignmentInd(size_t nIndex);
  CIFSEntityAssignmentInd* GetIFSAssignmentInd(const xtstring& sName);
  xtstring GetIFSAssignmentIndIndexedName(const xtstring& sName);
  void AddIFSAssignmentInd(CIFSEntityAssignmentInd* pModel);
  bool RemoveIFSAssignmentInd(size_t nIndex);
  bool RemoveIFSAssignmentInd(const xtstring& sName);
  bool RemoveAllIFSAssignmentInds();

  size_t CountOfPreProcessInds(){return m_arrPreProcesses.size();};
  CIFSEntityPrePostProcessInd* GetPreProcessInd(size_t nIndex);
  CIFSEntityPrePostProcessInd* GetPreProcessInd(const xtstring& sName);
  xtstring GetPreProcessIndIndexedName(const xtstring& sName);
  void AddPreProcessInd(CIFSEntityPrePostProcessInd* pModel);
  bool RemovePreProcessInd(size_t nIndex);
  bool RemovePreProcessInd(const xtstring& sName);
  bool RemoveAllPreProcessInds();

  size_t CountOfPostProcessInds(){return m_arrPostProcesses.size();};
  CIFSEntityPrePostProcessInd* GetPostProcessInd(size_t nIndex);
  CIFSEntityPrePostProcessInd* GetPostProcessInd(const xtstring& sName);
  xtstring GetPostProcessIndIndexedName(const xtstring& sName);
  void AddPostProcessInd(CIFSEntityPrePostProcessInd* pModel);
  bool RemovePostProcessInd(size_t nIndex);
  bool RemovePostProcessInd(const xtstring& sName);
  bool RemoveAllPostProcessInds();

  size_t CountOfGlobalERTableInds(){return m_arrGlobalERTables.size();};
  CIFSEntityGlobalERTableInd* GetGlobalERTableInd(size_t nIndex);
  CIFSEntityGlobalERTableInd* GetGlobalERTableInd(const xtstring& sName);
  xtstring GetGlobalERTableIndIndexedName(const xtstring& sName);
  void AddGlobalERTableInd(CIFSEntityGlobalERTableInd* pModel);
  bool RemoveGlobalERTableInd(size_t nIndex);
  bool RemoveGlobalERTableInd(const xtstring& sName);
  bool RemoveAllGlobalERTableInds();
private:
  // common properties
  xtstring      m_sName;
  xtstring      m_sComment;
  bool          m_bMainEntity;
  bool          m_bDebugMode;
  bool          m_bOpenedEntity;
  xtstring      m_sOpenedEntityName;
  bool          m_bShowExpanded;
  TShowBehaviour  m_nShowBehaviour;
  bool          m_bShowAddCondition;
  bool          m_bShowAddFreeSelection;

  TTypeOfEntity           m_nTypeOfEntity;
  xtstring                m_sSubIFS;
  CConditionOrAssignment  m_caPortCondition;
  TReadMethod             m_nReadMethod;

  xtstring                m_sERTableOrGlobalTableOrPropStoneTypeOrDBStoneName_Raw;
  CConditionOrAssignment  m_caERTableRealName;

  xtstring      m_sIdentity;
  CConditionOrAssignment  m_caMainCondition;
  bool          m_bDontOptimizeCondition;
  CIFSEntityConditionVariations   m_cConditionVariations;
  CIFSEntityAllParameters         m_cParameters;
  CIFSEntitySQLCommandVariations  m_cSQLCommandVariations;
  xtstring      m_sWhereClause;
  xtstring      m_sSortField;
  bool          m_bSortDescending;
  CIFSEntityFields      m_arrFields;
  CIFSEntityConnections m_arrConnections;
  CIFSEntityConnections m_arrConnectionsToChilds;
  // parameter table values (only main entity)
  CxtstringMap  m_mapParamTableValues;
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

  CIFSEntityChannelToGUI* m_pChannelToGUI;

protected:
  // hit table properties
  CConditionOrAssignment        m_caHitCondition;
  TChildExecution               m_nChildExecution;
  CIFSHitNames                  m_arrHitNames;
  CIFSEntityVariableInds        m_arrVariables;
  CIFSEntityAssignmentInds      m_arrAssignments;
  CIFSEntityPrePostProcessInds  m_arrPreProcesses;
  CIFSEntityPrePostProcessInds  m_arrPostProcesses;
  CIFSEntityGlobalERTableInds   m_arrGlobalERTables;

protected:
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){PropertiesChanged();};
  virtual void ROC_PropertyChanged_ER(){PropertiesChanged();};
  virtual void ROC_PropertyChanged_IFS(){PropertiesChanged();};
  virtual void ROC_PropertyChanged_WP(){PropertiesChanged();};
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteProjConstant(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteERModel(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers);
  virtual void ROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sName, CERModelParamInd* pParam, CUserOfObject* pObject);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers);
  virtual void ROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObject* pObject);

  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteIFS(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWP(const xtstring& sName, CUserOfObject* pObject);
  virtual void ROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers);
  virtual void ROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObject* pObject);
  virtual void ROC_CheckReferencesForWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  virtual void ROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObject* pObject);

protected:
  virtual void OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName);
protected:
  virtual bool SE_TableExist(const xtstring& sTableName){return false;};
  virtual bool SE_FieldExist(const xtstring& sFieldName){return false;};
  virtual bool SE_EntityExist(const xtstring& sEntityName){return (sEntityName == GetName());};
  virtual bool SE_VariableExist(const xtstring& sVariableName){return false;};

  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return _XT("");};
  virtual xtstring SE_GetERModelName() const {return _XT("");};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames);
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName);
  virtual bool SE_GetShowRelationNames(){return true;};
  virtual bool SE_GetShowHitLines(){return true;};
public:
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return GetIFSEntityInd();};
protected:
  virtual xtstring SE_GetParamTableName() const {return _XT("");};
  virtual void SE_SetParamTableName(const xtstring sParamTable){};
  virtual void SE_OnParamTableChanged();
  virtual void SE_OnERModelChanged(const xtstring& sNewERModelName){};

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


#endif // !defined(_DMODELIND_H_)
