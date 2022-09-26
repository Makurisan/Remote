#if !defined(_SYNCENGINE_H_)
#define _SYNCENGINE_H_


#include "SModelInd/ModelExpImp.h"
#include "SyncEngineBase.h"
#include "SyncGroups.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32






typedef std::vector<CSyncERNotifiersGroup*>   CERGroups;
typedef CERGroups::iterator                   CERGroupsIterator;
typedef CERGroups::const_iterator             CERGroupsConstIterator;
typedef CERGroups::reverse_iterator           CERGroupsReverseIterator;

typedef std::vector<CSyncIFSNotifiersGroup*>  CIFSGroups;
typedef CIFSGroups::iterator                  CIFSGroupsIterator;
typedef CIFSGroups::const_iterator            CIFSGroupsConstIterator;
typedef CIFSGroups::reverse_iterator          CIFSGroupsReverseIterator;

typedef std::vector<CSyncWPNotifiersGroup*>   CWPGroups;
typedef CWPGroups::iterator                   CWPGroupsIterator;
typedef CWPGroups::const_iterator             CWPGroupsConstIterator;
typedef CWPGroups::reverse_iterator           CWPGroupsReverseIterator;



typedef std::vector<CSyncResponse_Common*>      CCommonResponseGroup;
typedef CCommonResponseGroup::iterator          CCommonResponseGroupIterator;
typedef CCommonResponseGroup::const_iterator    CCommonResponseGroupConstIterator;
typedef CCommonResponseGroup::reverse_iterator  CCommonResponseGroupReverseIterator;

typedef std::vector<CSyncResponse_Proj*>        CProjResponseGroup;
typedef CProjResponseGroup::iterator            CProjResponseGroupIterator;
typedef CProjResponseGroup::const_iterator      CProjResponseGroupConstIterator;
typedef CProjResponseGroup::reverse_iterator    CProjResponseGroupReverseIterator;

typedef std::vector<CSyncResponse_ER*>          CERResponseGroup;
typedef CERResponseGroup::iterator              CERResponseGroupIterator;
typedef CERResponseGroup::const_iterator        CERResponseGroupConstIterator;
typedef CERResponseGroup::reverse_iterator      CERResponseGroupReverseIterator;

typedef std::vector<CSyncResponse_IFS*>         CIFSResponseGroup;
typedef CIFSResponseGroup::iterator             CIFSResponseGroupIterator;
typedef CIFSResponseGroup::const_iterator       CIFSResponseGroupConstIterator;
typedef CIFSResponseGroup::reverse_iterator     CIFSResponseGroupReverseIterator;

typedef std::vector<CSyncResponse_WP*>          CWPResponseGroup;
typedef CWPResponseGroup::iterator              CWPResponseGroupIterator;
typedef CWPResponseGroup::const_iterator        CWPResponseGroupConstIterator;
typedef CWPResponseGroup::reverse_iterator      CWPResponseGroupReverseIterator;



class XPUBMODEL_EXPORTIMPORT CSyncEngine : public CSyncEngineBase
{
public:
  CSyncEngine();
  virtual ~CSyncEngine();

  // functions for notifier groups
  // 
  CSyncERNotifiersGroup*  GetDefaultERGroup();
  CSyncIFSNotifiersGroup* GetDefaultIFSGroup();
  CSyncWPNotifiersGroup*  GetDefaultWPGroup();
  //
  bool AddERGroup(CSyncERNotifiersGroup* pGroup);
  bool RemoveERGroup(CSyncERNotifiersGroup* pGroup);
  bool AddIFSGroup(CSyncIFSNotifiersGroup* pGroup);
  bool RemoveIFSGroup(CSyncIFSNotifiersGroup* pGroup);
  bool AddWPGroup(CSyncWPNotifiersGroup* pGroup);
  bool RemoveWPGroup(CSyncWPNotifiersGroup* pGroup);

  // functions for response classes
  bool AddCommonResponse(CSyncResponse_Common* pResponse);
  bool RemoveCommonResponse(CSyncResponse_Common* pResponse);
  bool AddProjResponse(CSyncResponse_Proj* pResponse);
  bool RemoveProjResponse(CSyncResponse_Proj* pResponse);
  bool AddERResponse(CSyncResponse_ER* pResponse);
  bool RemoveERResponse(CSyncResponse_ER* pResponse);
  bool AddIFSResponse(CSyncResponse_IFS* pResponse);
  bool RemoveIFSResponse(CSyncResponse_IFS* pResponse);
  bool AddWPResponse(CSyncResponse_WP* pResponse);
  bool RemoveWPResponse(CSyncResponse_WP* pResponse);

  // exist functions
  bool SE_TableExist(const xtstring& sTableName);
  bool SE_FieldExist(const xtstring& sFieldName);
  bool SE_EntityExist(const xtstring& sEntityName);

  // functions notifier to response
  bool SE_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteProjConstant(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteSymbolicDBName(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);

  bool SE_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForERModel(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteERModel(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteTable(const xtstring& sERModelName, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sFieldName, CUserOfObjectArray& cUsers);
  bool SE_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sFieldName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CUserOfObjectArray& cUsers);
  bool SE_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, bool bCancelAllowed);
  bool SE_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, bool bCancelAllowed);
  bool SE_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, bool bCancelAllowed);
  bool SE_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, bool bCancelAllowed);
  bool SE_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, bool bCancelAllowed);

  bool SE_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteIFS(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteEntity(const xtstring& sIFS, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);

  bool SE_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteWP(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);

  // functions to GUI
  virtual bool SEROC_ChangeProjConstant(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeProjConstant_Post(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteProjConstant(const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteProjConstant_Post(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeSymbolicDBName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeSymbolicDBName_Post(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteSymbolicDBName(const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteSymbolicDBName_Post(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;

  virtual bool SEROC_ChangeERModelName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeERModelName_Post(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteERModel(const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteERModel_Post(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeTableName(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeTableName_Post(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteTable(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteTable_Post(const xtstring& sERModelName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeFieldName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeFieldName_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteField(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sFieldName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteField_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sFieldName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeParamName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeParamName_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteParam(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteParam_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeRelationName(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeRelationName_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteRelation(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteRelation_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ParamAdded_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ParamChanged_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_RelationAdded_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_RelationChanged_Post(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, CUserOfObjectArray& cUsers) = 0;

  virtual bool SEROC_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeIFSName_Post(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteIFS(const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteIFS_Post(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeEntityName_Post(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteEntity_Post(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeVariableName_Post(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteVariable_Post(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeGlobalERTableName_Post(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteGlobalERTable_Post(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;

  virtual bool SEROC_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeWPName_Post(const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteWP(const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteWP_Post(const xtstring& sName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_ChangeWPConstantName_Post(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, CUserOfObjectArray& cUsers) = 0;
  virtual bool SEROC_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers, bool bCancelAllowed) = 0;
  virtual void SEROC_DeleteWPConstant_Post(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers) = 0;




  // neue Funktionsgruppe, die spaeter benannt wird
  void SE_SymbolicDBNameAddedDeletedRenamed();
  void SE_ModulesCountChanged(TModulesCountChanged nChanged);
  void SE_ModuleNameChangedER(const xtstring& sOldName, const xtstring& sNewName);
  void SE_ModuleNameChangedIFS(const xtstring& sOldName, const xtstring& sNewName);
  void SE_ModuleNameChangedStone(const xtstring& sOldName, const xtstring& sNewName);
  void SE_ModuleNameChangedTemplate(const xtstring& sOldName, const xtstring& sNewName);
  void SE_ModuleNameChangedPython(const xtstring& sOldName, const xtstring& sNewName);
  void SE_CountOfTablesInERModelChanged(const xtstring& sERModelName);
  void SE_CountOfFieldsInERTableChanged(const xtstring& sERModelName, const xtstring& sERTable);
  void SE_CountOfParamsInERTableChanged(const xtstring& sERModelName, const xtstring& sERTable);
  void SE_ERTableNameChanged(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName);
  void SE_ERFieldNameChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName);
  void SE_IFSEntityNameChanged(const xtstring& sIFSModelName, const xtstring& sOldName, const xtstring& sNewName);
private:
  // notifier groups
  CERGroups   m_cERGroups;
  CIFSGroups  m_cIFSGroups;
  CWPGroups   m_cWPGroups;
  // response classes
  CCommonResponseGroup  m_cCommonResponseGroup;
  CProjResponseGroup    m_cProjResponseGroup;
  CERResponseGroup      m_cERResponseGroup;
  CIFSResponseGroup     m_cIFSResponseGroup;
  CWPResponseGroup      m_cWPResponseGroup;

public:
  xtstring SE_Intern_GetERFromIFS(const xtstring& sIFSModulName);
  xtstring SE_Intern_GetERTableFromEntity(const xtstring& sIFSModulName, const xtstring& sEntityName);
  void SE_Intern_GetGlobalTableNames(const xtstring& sIFSModulName, CxtstringVector& arrNames);
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // _SYNCENGINE_H_
