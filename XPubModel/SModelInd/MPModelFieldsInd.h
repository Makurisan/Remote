// MPModelFieldsInd.h: interface for the CMPModelXXXXInd classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MPMODELFIELDSIND_H_)
#define _MPMODELFIELDSIND_H_


#include "ModelExpImp.h"

#include "SSyncEng/SyncEngineBase.h"
#include "XPubVariant.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CXPubMarkUp;


/////////////////////////////////////////////////////////////////////////////
// CVisibilityPreprocess
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CVisibilityPreprocess
{
public:
  CVisibilityPreprocess();
  ~CVisibilityPreprocess();
  void SetVisibilityPreprocess(CVisibilityPreprocess* p);
  CVisibilityPreprocess* GetVisibilityPreprocess(){return this;};

  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  bool AddValue(const xtstring& sName, const CXPubVariant& cValue);
  size_t CountOfValues();
  xtstring GetValueName(size_t nIndex);
  CXPubVariant* GetValueValue(size_t nIndex);
  CXPubVariant* GetValueValue(const xtstring& sName);

private:
  XPubVariantFieldList  m_cValues;
};




/////////////////////////////////////////////////////////////////////////////
// CMPModelConstantInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelConstantInd : public CModelBaseInd,
                                                   public CSyncNotifier_WP
{
public:
  CMPModelConstantInd(CSyncWPNotifiersGroup* pGroup);
  CMPModelConstantInd(CMPModelConstantInd& cModel);
  CMPModelConstantInd(CMPModelConstantInd* pModel);
  virtual ~CMPModelConstantInd();

  void SetMPModelConstantInd(CMPModelConstantInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  size_t GetFieldIndex(){ return m_nFieldIndex;};
  void SetFieldIndex(const size_t nIndex){m_nFieldIndex = nIndex;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
private:
  xtstring m_sName;
  xtstring m_sComment;
  size_t   m_nFieldIndex;
protected:
  virtual xtstring SE_GetModuleName(){return _XT("");};
};




/////////////////////////////////////////////////////////////////////////////
// CMPModelConstantInd_ForFrm
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelConstantInd_ForFrm : public CMPModelConstantInd
{
public:
  CMPModelConstantInd_ForFrm();
  CMPModelConstantInd_ForFrm(CMPModelConstantInd_ForFrm& cModel);
  CMPModelConstantInd_ForFrm(CMPModelConstantInd_ForFrm* pModel);
  virtual ~CMPModelConstantInd_ForFrm();

  void SetMPModelConstantInd_ForFrm(CMPModelConstantInd_ForFrm* pModel);
  void SetMPModelConstantInd_ForFrm(CMPModelConstantInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

};




/////////////////////////////////////////////////////////////////////////////
// CMPModelKeyValueInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelKeyValueInd
{
public:
  CMPModelKeyValueInd();
  CMPModelKeyValueInd(CMPModelKeyValueInd& cModel);
  CMPModelKeyValueInd(CMPModelKeyValueInd* pModel);
  virtual ~CMPModelKeyValueInd();

  void SetMPModelKeyValueInd(CMPModelKeyValueInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetKeyColumn(){return m_sKeyColumn;};
  void SetKeyColumn(const xtstring& sKeyColumn){m_sKeyColumn = sKeyColumn;};

  CConditionOrAssignment&         GetKeyValue(){return m_caKeyValue;};
  const CConditionOrAssignment&   GetKeyValue_Const() const {return m_caKeyValue;};
  CConditionOrAssignment*         GetKeyValue_Ptr(){return &m_caKeyValue;};

protected:
  xtstring                m_sKeyColumn;
  CConditionOrAssignment  m_caKeyValue;
};

typedef std::vector<CMPModelKeyValueInd*>         CMPModelKeyValueInds;
typedef CMPModelKeyValueInds::iterator            CMPModelKeyValueIndsIterator;
typedef CMPModelKeyValueInds::reverse_iterator    CMPModelKeyValueIndsReverseIterator;


/////////////////////////////////////////////////////////////////////////////
// CMPModelKeyInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelKeyInd : public CModelBaseInd,
                                              public CSyncNotifier_WP,
                                              public CSyncResponse_Proj,
                                              public CSyncResponse_ER,
                                              public CSyncResponse_IFS
{
public:
  CMPModelKeyInd(CUserOfObject::TModuleType nUserOfObject_ModuleType, const xtstring& sInformationSight, CSyncWPNotifiersGroup* pGroup);
  CMPModelKeyInd(CMPModelKeyInd& cModel);
  CMPModelKeyInd(CMPModelKeyInd* pModel);
  virtual ~CMPModelKeyInd();

  bool operator == (CMPModelKeyInd* pModel);

  void SetMPModelKeyInd(CMPModelKeyInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  size_t GetFieldIndex(){ return m_nFieldIndex;};
  void SetFieldIndex(const size_t nIndex){m_nFieldIndex = nIndex;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};

  // visibility
  CConditionOrAssignment&         GetVisibility(){return m_caVisibility;};
  const CConditionOrAssignment&   GetVisibility_Const() const {return m_caVisibility;};
  CConditionOrAssignment*         GetVisibility_Ptr(){return &m_caVisibility;};
  // tag property
  CConditionOrAssignment&         GetTag(){return m_caTag;};
  const CConditionOrAssignment&   GetTag_Const() const {return m_caTag;};
  CConditionOrAssignment*         GetTag_Ptr(){return &m_caTag;};

  xtstring GetTextTable(){return m_sTextTable;};
  void SetTextTable(const xtstring& sTextTable){m_sTextTable = sTextTable;};
  xtstring GetTextColumn(){return m_sTextColumn;};
  void SetTextColumn(const xtstring& sTextColumn){m_sTextColumn = sTextColumn;};

  void SetInformationSight(const xtstring& sInformationSight){m_sInformationSight = sInformationSight;};
  CUserOfObject::TModuleType GetUserOfObject_ModuleType(){return m_nUserOfObject_ModuleType;};


  size_t CountOfKeyValues(){return m_arrKeyValues.size();};
  CMPModelKeyValueInd* GetKeyValue(size_t nIndex){assert(nIndex < m_arrKeyValues.size()); if (nIndex < m_arrKeyValues.size()) return m_arrKeyValues[nIndex]; else return NULL;};
  CMPModelKeyValueInd* GetKeyValue(const xtstring& sKeyColumn);
  void AddKeyValue(CMPModelKeyValueInd* pModel){assert(pModel);
                                           CMPModelKeyValueInd* ppModel;
                                           ppModel = new CMPModelKeyValueInd(pModel);
                                           m_arrKeyValues.push_back(ppModel);};
  bool RemoveKeyValue(size_t nIndex);
  bool RemoveKeyValue(const xtstring& sKeyColumn);
  bool RemoveAllKeyValues();

private:
  xtstring                m_sName;
  xtstring                m_sComment;
  size_t                  m_nFieldIndex;
  CConditionOrAssignment  m_caVisibility;
  CConditionOrAssignment  m_caTag;
  xtstring                m_sTextTable;
  xtstring                m_sTextColumn;
  CMPModelKeyValueInds    m_arrKeyValues;
  // non stream data -->
  CUserOfObject::TModuleType  m_nUserOfObject_ModuleType;
  // non stream data <--

protected:
  xtstring GetInformationSight(){return m_sInformationSight;};
  xtstring m_sInformationSight;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){};
  virtual void ROC_PropertyChanged_ER(){};
  virtual void ROC_PropertyChanged_IFS(){};
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
  virtual xtstring SE_GetModuleName(){return _XT("");};
};




/////////////////////////////////////////////////////////////////////////////
// CMPModelKeyInd_ForFrm
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelKeyInd_ForFrm : public CMPModelKeyInd,
                                                     public CVisibilityPreprocess
{
public:
  CMPModelKeyInd_ForFrm();
  CMPModelKeyInd_ForFrm(CMPModelKeyInd_ForFrm& cModel);
  CMPModelKeyInd_ForFrm(CMPModelKeyInd_ForFrm* pModel);
  virtual ~CMPModelKeyInd_ForFrm();

  void SetMPModelKeyInd_ForFrm(CMPModelKeyInd_ForFrm* pModel);
  void SetMPModelKeyInd_ForFrm(CMPModelKeyInd* pModel);
  bool SaveXMLContent(CXPubMarkUp* pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp* pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetKeyResult(){return m_sKeyResult;};
  void SetKeyResult(const xtstring& sKeyResult){m_sKeyResult = sKeyResult;};
  xtstring GetTagPropertyResult(){return m_sTagPropertyResult;};
  void SetTagPropertyResult(const xtstring& sTagPropertyResult){m_sTagPropertyResult = sTagPropertyResult;};

private:
  xtstring  m_sKeyResult;
  xtstring  m_sTagPropertyResult;
};



/////////////////////////////////////////////////////////////////////////////
// CMPModelTextInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelTextInd : public CModelBaseInd,
                                               public CSyncNotifier_WP,
                                               public CSyncResponse_Proj,
                                               public CSyncResponse_ER,
                                               public CSyncResponse_IFS
{
public:
  CMPModelTextInd(CUserOfObject::TModuleType nUserOfObject_ModuleType, const xtstring& sInformationSight, CSyncWPNotifiersGroup* pGroup);
  CMPModelTextInd(CMPModelTextInd& cModel);
  CMPModelTextInd(CMPModelTextInd* pModel);
  virtual ~CMPModelTextInd();

  void SetMPModelTextInd(CMPModelTextInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  size_t GetFieldIndex(){ return m_nFieldIndex;};
  void SetFieldIndex(const size_t nIndex){m_nFieldIndex = nIndex;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};

  // visibility
  CConditionOrAssignment&         GetVisibility(){return m_caVisibility;};
  const CConditionOrAssignment&   GetVisibility_Const() const {return m_caVisibility;};
  CConditionOrAssignment*         GetVisibility_Ptr(){return &m_caVisibility;};
  // tag property
  CConditionOrAssignment&         GetTag(){return m_caTag;};
  const CConditionOrAssignment&   GetTag_Const() const {return m_caTag;};
  CConditionOrAssignment*         GetTag_Ptr(){return &m_caTag;};
  // content
  CConditionOrAssignment&         GetContent(){return m_caContent;};
  const CConditionOrAssignment&   GetContent_Const() const {return m_caContent;};
  CConditionOrAssignment*         GetContent_Ptr(){return &m_caContent;};

  xtstring GetContentTextMask(){return m_sContentTextMask;};
  void SetContentTextMask(const xtstring& sText){m_sContentTextMask = sText;};

  void SetInformationSight(const xtstring& sInformationSight){m_sInformationSight = sInformationSight;};
  CUserOfObject::TModuleType GetUserOfObject_ModuleType(){return m_nUserOfObject_ModuleType;};
private:
  xtstring                m_sName;
  size_t                  m_nFieldIndex;
  xtstring                m_sComment;
  CConditionOrAssignment  m_caVisibility;
  CConditionOrAssignment  m_caTag;
  CConditionOrAssignment  m_caContent;
  xtstring                m_sContentTextMask;
  // non stream data -->
  CUserOfObject::TModuleType  m_nUserOfObject_ModuleType;
  // non stream data <--

protected:
  xtstring GetInformationSight(){return m_sInformationSight;};
  xtstring m_sInformationSight;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){};
  virtual void ROC_PropertyChanged_ER(){};
  virtual void ROC_PropertyChanged_IFS(){};
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
  virtual xtstring SE_GetModuleName(){return _XT("");};
};

/////////////////////////////////////////////////////////////////////////////
// CMPModelTextInd_ForFrm
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelTextInd_ForFrm : public CMPModelTextInd,
                                                      public CVisibilityPreprocess
{
public:
  CMPModelTextInd_ForFrm();
  CMPModelTextInd_ForFrm(CMPModelTextInd_ForFrm& cModel);
  CMPModelTextInd_ForFrm(CMPModelTextInd_ForFrm* pModel);
  virtual ~CMPModelTextInd_ForFrm();

  void SetMPModelTextInd_ForFrm(CMPModelTextInd_ForFrm* pModel);
  void SetMPModelTextInd_ForFrm(CMPModelTextInd* pModel);
  bool SaveXMLContent(CXPubMarkUp* pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp* pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetContentResult(){return m_sContentResult;};
  void SetContentResult(const xtstring& sContentResult){m_sContentResult = sContentResult;};
  xtstring GetTagPropertyResult(){return m_sTagPropertyResult;};
  void SetTagPropertyResult(const xtstring& sTagPropertyResult){m_sTagPropertyResult = sTagPropertyResult;};

private:
  xtstring  m_sContentResult;
  xtstring  m_sTagPropertyResult;
};




/////////////////////////////////////////////////////////////////////////////
// CMPModelAreaInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelAreaInd : public CModelBaseInd,
                                               public CSyncNotifier_WP,
                                               public CSyncResponse_Proj,
                                               public CSyncResponse_ER,
                                               public CSyncResponse_IFS
{
public:
  CMPModelAreaInd(CUserOfObject::TModuleType nUserOfObject_ModuleType, const xtstring& sInformationSight, CSyncWPNotifiersGroup* pGroup);
  CMPModelAreaInd(CMPModelAreaInd& cModel);
  CMPModelAreaInd(CMPModelAreaInd* pModel);
  virtual ~CMPModelAreaInd();

  void SetMPModelAreaInd(CMPModelAreaInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  size_t GetFieldIndex(){ return m_nFieldIndex;};
  void SetFieldIndex(const size_t nIndex){m_nFieldIndex = nIndex;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};

  // visibility
  CConditionOrAssignment&         GetVisibility(){return m_caVisibility;};
  const CConditionOrAssignment&   GetVisibility_Const() const {return m_caVisibility;};
  CConditionOrAssignment*         GetVisibility_Ptr(){return &m_caVisibility;};

  void SetInformationSight(const xtstring& sInformationSight){m_sInformationSight = sInformationSight;};
  CUserOfObject::TModuleType GetUserOfObject_ModuleType(){return m_nUserOfObject_ModuleType;};
  void SetVisibilityResult(bool bVisibilityResult){m_bVisibilityResult = bVisibilityResult;};
  bool GetVisibilityResult(){return m_bVisibilityResult;};
  CVisibilityPreprocess& GetVisibilityPreprocessResult(){return m_cVisibilityPreprocessResult;};
private:
  xtstring                m_sName;
  size_t                  m_nFieldIndex;
  xtstring                m_sComment;
  CConditionOrAssignment  m_caVisibility;
  // non stream data -->
  CUserOfObject::TModuleType  m_nUserOfObject_ModuleType;
  bool                        m_bVisibilityResult;
  CVisibilityPreprocess       m_cVisibilityPreprocessResult;
  // non stream data <--

protected:
  xtstring GetInformationSight(){return m_sInformationSight;};
  xtstring m_sInformationSight;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){};
  virtual void ROC_PropertyChanged_ER(){};
  virtual void ROC_PropertyChanged_IFS(){};
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
  virtual xtstring SE_GetModuleName(){return _XT("");};
};




/////////////////////////////////////////////////////////////////////////////
// CMPModelAreaInd_ForFrm
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelAreaInd_ForFrm : public CMPModelAreaInd,
                                                      public CVisibilityPreprocess
{
public:
  CMPModelAreaInd_ForFrm();
  CMPModelAreaInd_ForFrm(CMPModelAreaInd_ForFrm& cModel);
  CMPModelAreaInd_ForFrm(CMPModelAreaInd_ForFrm* pModel);
  virtual ~CMPModelAreaInd_ForFrm();

  void SetMPModelAreaInd_ForFrm(CMPModelAreaInd_ForFrm* pModel);
  void SetMPModelAreaInd_ForFrm(CMPModelAreaInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

};









/////////////////////////////////////////////////////////////////////////////
// CMPModelLinkInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelLinkInd : public CModelBaseInd,
                                               public CSyncNotifier_WP,
                                               public CSyncResponse_Proj,
                                               public CSyncResponse_ER,
                                               public CSyncResponse_IFS
{
public:
  CMPModelLinkInd(CUserOfObject::TModuleType nUserOfObject_ModuleType, const xtstring& sInformationSight, CSyncWPNotifiersGroup* pGroup);
  CMPModelLinkInd(CMPModelLinkInd& cModel);
  CMPModelLinkInd(CMPModelLinkInd* pModel);
  virtual ~CMPModelLinkInd();

  void SetMPModelLinkInd(CMPModelLinkInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  size_t GetFieldIndex(){ return m_nFieldIndex;};
  void SetFieldIndex(const size_t nIndex){m_nFieldIndex = nIndex;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};

  // visibility
  CConditionOrAssignment&         GetVisibility(){return m_caVisibility;};
  const CConditionOrAssignment&   GetVisibility_Const() const {return m_caVisibility;};
  CConditionOrAssignment*         GetVisibility_Ptr(){return &m_caVisibility;};
  // address
  CConditionOrAssignment&         GetAddress(){return m_caAddress;};
  const CConditionOrAssignment&   GetAddress_Const() const {return m_caAddress;};
  CConditionOrAssignment*         GetAddress_Ptr(){return &m_caAddress;};
  // quick info
  CConditionOrAssignment&         GetQuickInfo(){return m_caQuickInfo;};
  const CConditionOrAssignment&   GetQuickInfo_Const() const {return m_caQuickInfo;};
  CConditionOrAssignment*         GetQuickInfo_Ptr(){return &m_caQuickInfo;};
  // python function
  CConditionOrAssignment&         GetTag(){return m_caTag;};
  const CConditionOrAssignment&   GetTag_Const() const {return m_caTag;};
  CConditionOrAssignment*         GetTag_Ptr(){return &m_caTag;};

  xtstring GetTargetFrame(){return m_sTargetFrame;};
  void SetTargetFrame(const xtstring& sText){m_sTargetFrame = sText;};

  void SetInformationSight(const xtstring& sInformationSight){m_sInformationSight = sInformationSight;};
  CUserOfObject::TModuleType GetUserOfObject_ModuleType(){return m_nUserOfObject_ModuleType;};
private:
  xtstring                m_sName;
  size_t                  m_nFieldIndex;
  xtstring                m_sComment;
  CConditionOrAssignment  m_caVisibility;
  CConditionOrAssignment  m_caAddress;
  CConditionOrAssignment  m_caQuickInfo;
  CConditionOrAssignment  m_caTag;
  xtstring m_sTargetFrame;
  // non stream data -->
  CUserOfObject::TModuleType  m_nUserOfObject_ModuleType;
  // non stream data <--

protected:
  xtstring GetInformationSight(){return m_sInformationSight;};
  xtstring m_sInformationSight;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){};
  virtual void ROC_PropertyChanged_ER(){};
  virtual void ROC_PropertyChanged_IFS(){};
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
  virtual xtstring SE_GetModuleName(){return _XT("");};
};










/////////////////////////////////////////////////////////////////////////////
// CMPModelGraphicInd
/////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CMPModelGraphicInd : public CModelBaseInd,
                                                  public CSyncNotifier_WP,
                                                  public CSyncResponse_Proj,
                                                  public CSyncResponse_ER,
                                                  public CSyncResponse_IFS
{
public:
  CMPModelGraphicInd(CUserOfObject::TModuleType nUserOfObject_ModuleType, const xtstring& sInformationSight, CSyncWPNotifiersGroup* pGroup);
  CMPModelGraphicInd(CMPModelGraphicInd& cModel);
  CMPModelGraphicInd(CMPModelGraphicInd* pModel);
  virtual ~CMPModelGraphicInd();

  void SetMPModelGraphicInd(CMPModelGraphicInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  /////////////////////////////////////////////////////////////
  // 
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  size_t GetFieldIndex(){ return m_nFieldIndex;};
  void SetFieldIndex(const size_t nIndex){m_nFieldIndex = nIndex;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};

  // visibility
  CConditionOrAssignment&         GetVisibility(){return m_caVisibility;};
  const CConditionOrAssignment&   GetVisibility_Const() const {return m_caVisibility;};
  CConditionOrAssignment*         GetVisibility_Ptr(){return &m_caVisibility;};
  // pic file name local
  CConditionOrAssignment&         GetPicFileNameLocal(){return m_caPicFileNameLocal;};
  const CConditionOrAssignment&   GetPicFileNameLocal_Const() const {return m_caPicFileNameLocal;};
  CConditionOrAssignment*         GetPicFileNameLocal_Ptr(){return &m_caPicFileNameLocal;};
  // pic file name
  CConditionOrAssignment&         GetPicFileName(){return m_caPicFileName;};
  const CConditionOrAssignment&   GetPicFileName_Const() const {return m_caPicFileName;};
  CConditionOrAssignment*         GetPicFileName_Ptr(){return &m_caPicFileName;};
  // quick info
  CConditionOrAssignment&         GetQuickInfo(){return m_caQuickInfo;};
  const CConditionOrAssignment&   GetQuickInfo_Const() const {return m_caQuickInfo;};
  CConditionOrAssignment*         GetQuickInfo_Ptr(){return &m_caQuickInfo;};
  // python function
  CConditionOrAssignment&         GetTag(){return m_caTag;};
  const CConditionOrAssignment&   GetTag_Const() const {return m_caTag;};
  CConditionOrAssignment*         GetTag_Ptr(){return &m_caTag;};

  long GetPicSizeInPercent(){return m_nPicSizeInPercent;};
  void SetPicSizeInPercent(long nSize){m_nPicSizeInPercent = nSize;};
  long GetPicWidth(){return m_nWidth;};
  void SetPicWidth(long nWidth){m_nWidth = nWidth;};
  long GetPicHeight(){return m_nHeight;};
  void SetPicHeight(long nHeight){m_nHeight = nHeight;};

  void SetInformationSight(const xtstring& sInformationSight){m_sInformationSight = sInformationSight;};
  CUserOfObject::TModuleType GetUserOfObject_ModuleType(){return m_nUserOfObject_ModuleType;};
private:
  xtstring                m_sName;
  size_t                  m_nFieldIndex;
  xtstring                m_sComment;
  CConditionOrAssignment  m_caVisibility;
  CConditionOrAssignment  m_caPicFileNameLocal;
  CConditionOrAssignment  m_caPicFileName;
  CConditionOrAssignment  m_caQuickInfo;
  CConditionOrAssignment  m_caTag;
  long                    m_nPicSizeInPercent;
  long                    m_nWidth;
  long                    m_nHeight;
  // non stream data -->
  CUserOfObject::TModuleType  m_nUserOfObject_ModuleType;
  // non stream data <--

protected:
  xtstring GetInformationSight(){return m_sInformationSight;};
  xtstring m_sInformationSight;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){};
  virtual void ROC_PropertyChanged_ER(){};
  virtual void ROC_PropertyChanged_IFS(){};
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
  virtual xtstring SE_GetModuleName(){return _XT("");};
};





typedef std::vector<CMPModelConstantInd*>       CMPModelConstantInds;
typedef CMPModelConstantInds::iterator          CMPModelConstantIndsIterator;
typedef CMPModelConstantInds::reverse_iterator  CMPModelConstantIndsReverseIterator;

typedef std::vector<CMPModelKeyInd*>            CMPModelKeyInds;
typedef CMPModelKeyInds::iterator               CMPModelKeyIndsIterator;
typedef CMPModelKeyInds::reverse_iterator       CMPModelKeyIndsReverseIterator;

typedef std::vector<CMPModelTextInd*>           CMPModelTextInds;
typedef CMPModelTextInds::iterator              CMPModelTextIndsIterator;
typedef CMPModelTextInds::reverse_iterator      CMPModelTextIndsReverseIterator;

typedef std::vector<CMPModelAreaInd*>           CMPModelAreaInds;
typedef CMPModelAreaInds::iterator              CMPModelAreaIndsIterator;
typedef CMPModelAreaInds::reverse_iterator      CMPModelAreaIndsReverseIterator;

typedef std::vector<CMPModelLinkInd*>           CMPModelLinkInds;
typedef CMPModelLinkInds::iterator              CMPModelLinkIndsIterator;
typedef CMPModelLinkInds::reverse_iterator      CMPModelLinkIndsReverseIterator;

typedef std::vector<CMPModelGraphicInd*>        CMPModelGraphicInds;
typedef CMPModelGraphicInds::iterator           CMPModelGraphicIndsIterator;
typedef CMPModelGraphicInds::reverse_iterator   CMPModelGraphicIndsReverseIterator;





typedef std::vector<CMPModelConstantInd_ForFrm*>        CMPModelConstantInds_ForFrm;
typedef CMPModelConstantInds_ForFrm::iterator           CMPModelConstantInds_ForFrmIterator;
typedef CMPModelConstantInds_ForFrm::reverse_iterator   CMPModelConstantInds_ForFrmReverseIterator;

typedef std::vector<CMPModelKeyInd_ForFrm*>             CMPModelKeyInds_ForFrm;
typedef CMPModelKeyInds_ForFrm::iterator                CMPModelKeyInds_ForFrmIterator;
typedef CMPModelKeyInds_ForFrm::reverse_iterator        CMPModelKeyInds_ForFrmReverseIterator;

typedef std::vector<CMPModelTextInd_ForFrm*>            CMPModelTextInds_ForFrm;
typedef CMPModelTextInds_ForFrm::iterator               CMPModelTextInds_ForFrmIterator;
typedef CMPModelTextInds_ForFrm::reverse_iterator       CMPModelTextInds_ForFrmReverseIterator;

typedef std::vector<CMPModelAreaInd_ForFrm*>            CMPModelAreaInds_ForFrm;
typedef CMPModelAreaInds_ForFrm::iterator               CMPModelAreaInds_ForFrmIterator;
typedef CMPModelAreaInds_ForFrm::reverse_iterator       CMPModelAreaInds_ForFrmReverseIterator;




#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_MPMODELFIELDSIND_H_)
