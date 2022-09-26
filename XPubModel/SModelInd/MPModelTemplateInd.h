// MPModelTemplateInd.h: interface for the CMPModelTemplateInd classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MPMODELTEMPLATEIND_H_)
#define _MPMODELTEMPLATEIND_H_


#include "ModelExpImp.h"

#include "SSyncEng/SyncEngineBase.h"
#include "DBDefinitions.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CXPubMarkUp;

class CMPModelTemplateInd;


class XPUBMODEL_EXPORTIMPORT CMPModelTemplateInd : public CModelBaseInd,
                                                   public CSyncNotifier_WP,
                                                   public CSyncResponse_Proj,
                                                   public CSyncResponse_ER,
                                                   public CSyncResponse_IFS,
                                                   public CSyncResponse_WP
{
public:
  CMPModelTemplateInd(CSyncWPNotifiersGroup* pGroup);
  CMPModelTemplateInd(CMPModelTemplateInd& cModel);
  CMPModelTemplateInd(CMPModelTemplateInd* pModel);
  virtual ~CMPModelTemplateInd();

  void SetMPModelTemplateInd(CMPModelTemplateInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);
  static bool ReadXMLModuleName(CXPubMarkUp *pXMLDoc, xtstring& sModuleName);

  bool SE_DeleteWorkPaper(size_t& nCountOfReferences, bool bCancelAllowed)
    {return SE_DeleteWP(GetName(), nCountOfReferences, bCancelAllowed);};

  /////////////////////////////////////////////////////////////
  // interface for common properties
  // Name
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  xtstring GetComment(){return m_sComment;};
  void SetComment(const xtstring& sComment){m_sComment = sComment;};
  // autor
  xtstring GetAutor(){return m_sAutor;};
  void SetAutor(const xtstring& sAutor){m_sAutor = sAutor;};

  // information sight
  xtstring GetInformationSight(){return m_sInformationSight;};
  void SetInformationSight(const xtstring& sInformationSight);
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
  // copy type
  TWorkPaperCopyType GetWorkPaperCopyType(){return  m_nWorkPaperCopyType;};
  void SetWorkPaperCopyType(TWorkPaperCopyType nType){m_nWorkPaperCopyType = nType;};
  // processing mode
  TWorkPaperProcessingMode GetWorkPaperProcessingMode(){return m_nWorkPaperProcessingMode;};
  void SetWorkPaperProcessingMode(TWorkPaperProcessingMode nWorkPaperProcessingMode){m_nWorkPaperProcessingMode = nWorkPaperProcessingMode;};
  // addon value
  xtstring GetAddOnValue(){return m_sAddOnValue;};
  void SetAddOnValue(const xtstring& sAddOnValue){m_sAddOnValue = sAddOnValue;};
  // visibility
  CConditionOrAssignment&         GetVisibility(){return m_caVisibility;};
  const CConditionOrAssignment&   GetVisibility_Const() const {return m_caVisibility;};
  CConditionOrAssignment*         GetVisibility_Ptr(){return &m_caVisibility;};
  // tag property
  CConditionOrAssignment&         GetTag(){return m_caTag;};
  const CConditionOrAssignment&   GetTag_Const() const {return m_caTag;};
  CConditionOrAssignment*         GetTag_Ptr(){return &m_caTag;};
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
  // source view active
  long GetSourceViewActive(){return m_nSourceViewActive;};
  void SetSourceViewActive(long nVal){m_nSourceViewActive = nVal;};
  //
  TDataFormatForStoneAndTemplate GetDataFormat(){return m_nDataFormat;};
  void SetDataFormat(TDataFormatForStoneAndTemplate nDataFormat){m_nDataFormat = nDataFormat;};
  TViewTypeForStoneAndTemplate GetView1(){return m_nView1;};
  void SetView1(TViewTypeForStoneAndTemplate nView1){m_nView1 = nView1;};
  TViewTypeForStoneAndTemplate GetView2(){return m_nView2;};
  void SetView2(TViewTypeForStoneAndTemplate nView2){m_nView2 = nView2;};

  void GetMPConstantInds(CxtstringVector& arrConstants);
  size_t CountOfMPConstantInds(){return m_arrConstants.size();};
  CMPModelConstantInd* GetMPConstantInd(size_t nIndex){assert(nIndex < m_arrConstants.size()); if (nIndex < m_arrConstants.size()) return m_arrConstants[nIndex]; else return NULL;};
  CMPModelConstantInd* GetMPConstantInd(const xtstring& sName);
  void AddMPConstantInd(CMPModelConstantInd* pModel){assert(pModel);
                                                     CMPModelConstantInd* ppModel;
                                                     pModel->SetFieldIndex(m_arrConstants.size()+1);
                                                     ppModel = new CMPModelConstantInd(pModel);
                                                     ppModel->SetSyncGroup(GetSyncGroup());
                                                     m_arrConstants.push_back(ppModel);;
                                                     };
  bool RemoveMPConstantInd(size_t nIndex);
  bool RemoveMPConstantInd(const xtstring& sName);
  bool RemoveAllMPConstantInds();

  size_t CountOfMPKeyInds(){return m_arrKeys.size();};
  CMPModelKeyInd* GetMPKeyInd(size_t nIndex){assert(nIndex < m_arrKeys.size()); if (nIndex < m_arrKeys.size()) return m_arrKeys[nIndex]; else return NULL;};
  CMPModelKeyInd* GetMPKeyInd(const xtstring& sName);
  void AddMPKeyInd(CMPModelKeyInd* pModel){assert(pModel);
                                           CMPModelKeyInd* ppModel;
                                           pModel->SetFieldIndex(m_arrKeys.size()+1);
                                           ppModel = new CMPModelKeyInd(pModel);
                                           ppModel->SetInformationSight(GetInformationSight());
                                           ppModel->SetSyncGroup(GetSyncGroup());
                                           m_arrKeys.push_back(ppModel);;
                                           };
  bool RemoveMPKeyInd(size_t nIndex);
  bool RemoveMPKeyInd(const xtstring& sName);
  bool RemoveAllMPKeyInds();

  size_t CountOfMPTextInds(){return m_arrTexts.size();};
  CMPModelTextInd* GetMPTextInd(size_t nIndex){assert(nIndex < m_arrTexts.size()); if (nIndex < m_arrTexts.size()) return m_arrTexts[nIndex]; else return NULL;};
  CMPModelTextInd* GetMPTextInd(const xtstring& sName);
  void AddMPTextInd(CMPModelTextInd* pModel){assert(pModel);
                                             CMPModelTextInd* ppModel;
                                             pModel->SetFieldIndex(m_arrTexts.size()+1);
                                             ppModel = new CMPModelTextInd(pModel);
                                             ppModel->SetInformationSight(GetInformationSight());
                                             ppModel->SetSyncGroup(GetSyncGroup());
                                             m_arrTexts.push_back(ppModel);;
                                             };
  bool RemoveMPTextInd(size_t nIndex);
  bool RemoveMPTextInd(const xtstring& sName);
  bool RemoveAllMPTextInds();

  size_t CountOfMPAreaInds(){return m_arrAreas.size();};
  CMPModelAreaInd* GetMPAreaInd(size_t nIndex){assert(nIndex < m_arrAreas.size()); if (nIndex < m_arrAreas.size()) return m_arrAreas[nIndex]; else return NULL;};
  CMPModelAreaInd* GetMPAreaInd(const xtstring& sName);
  void AddMPAreaInd(CMPModelAreaInd* pModel){assert(pModel);
                                             CMPModelAreaInd* ppModel;
                                             pModel->SetFieldIndex(m_arrAreas.size()+1);
                                             ppModel = new CMPModelAreaInd(pModel);
                                             ppModel->SetInformationSight(GetInformationSight());
                                             ppModel->SetSyncGroup(GetSyncGroup());
                                             m_arrAreas.push_back(ppModel);;
                                             };
  bool RemoveMPAreaInd(size_t nIndex);
  bool RemoveMPAreaInd(const xtstring& sName);
  bool RemoveAllMPAreaInds();

  size_t CountOfMPLinkInds(){return m_arrLinks.size();};
  CMPModelLinkInd* GetMPLinkInd(size_t nIndex){assert(nIndex < m_arrLinks.size()); if (nIndex < m_arrLinks.size()) return m_arrLinks[nIndex]; else return NULL;};
  CMPModelLinkInd* GetMPLinkInd(const xtstring& sName);
  void AddMPLinkInd(CMPModelLinkInd* pModel){assert(pModel);
                                             CMPModelLinkInd* ppModel;
                                             pModel->SetFieldIndex(m_arrLinks.size()+1);
                                             ppModel = new CMPModelLinkInd(pModel);
                                             ppModel->SetInformationSight(GetInformationSight());
                                             ppModel->SetSyncGroup(GetSyncGroup());
                                             m_arrLinks.push_back(ppModel);;
                                             };
  bool RemoveMPLinkInd(size_t nIndex);
  bool RemoveMPLinkInd(const xtstring& sName);
  bool RemoveAllMPLinkInds();

  size_t CountOfMPGraphicInds(){return m_arrGraphics.size();};
  CMPModelGraphicInd* GetMPGraphicInd(size_t nIndex){assert(nIndex < m_arrGraphics.size()); if (nIndex < m_arrGraphics.size()) return m_arrGraphics[nIndex]; else return NULL;};
  CMPModelGraphicInd* GetMPGraphicInd(const xtstring& sName);
  void AddMPGraphicInd(CMPModelGraphicInd* pModel){assert(pModel);
                                                   CMPModelGraphicInd* ppModel;
                                                   pModel->SetFieldIndex(m_arrGraphics.size()+1);
                                                   ppModel = new CMPModelGraphicInd(pModel);
                                                   ppModel->SetInformationSight(GetInformationSight());
                                                   ppModel->SetSyncGroup(GetSyncGroup());
                                                   m_arrGraphics.push_back(ppModel);;
                                                   };
  bool RemoveMPGraphicInd(size_t nIndex);
  bool RemoveMPGraphicInd(const xtstring& sName);
  bool RemoveAllMPGraphicInds();

  bool FieldInAnyGroupExist(const xtstring& sFieldName);
  bool IsNewFieldNameAsSubstringInAnotherFieldName(const xtstring& sOldFieldName, const xtstring& sNewFieldName, xtstring& sAnotherFieldName);
  bool IsAnotherFieldNameAsSubstringInThisNewFieldName(const xtstring& sOldFieldName, const xtstring& sNewFieldName, xtstring& sAnotherFieldName);

private:
  // common properties
  xtstring m_sName;
  xtstring m_sComment;
  xtstring m_sAutor;
  // folgende sechs Variablen sind nur in tStoneTypeProperty benutzt
  xtstring  m_sInformationSight;
  bool      m_bUseInformationSight;
  xtstring  m_sIFSTargetEntity;
  bool      m_bUseIFSTargetEntity;
  xtstring  m_sStoneOrTemplate;
  bool      m_bUseStoneOrTemplate;
  // folgende zwei Variablen sind nur in tTemplateTypeProperty benutzt
  TTemplateTarget m_nTargetForTemplate;
  TSaveTemplateInFormat m_nSaveTemplateInFormat;
  int m_nTemplateFileIcon; // 0 -> use original
  CConditionOrAssignment  m_caTemplateFileShowName;
  CConditionOrAssignment  m_caTemplateFile;
  CConditionOrAssignment  m_caTemplateFolder;
  xtstring                m_sTemplateSymbolicDBName;
  CConditionOrAssignment  m_caTemplateDBName;
  // folgende Variablen sind fuer tStoneTypeProperty und tTemplateTypeProperty benutzt
  xtstring                        m_sAddOnValue;
  TWorkPaperCopyType              m_nWorkPaperCopyType; // Art, wie Baustein nach Ziel kopiert wird
  TWorkPaperProcessingMode        m_nWorkPaperProcessingMode; // Art, wann Baustein nach Zeile kopiert wird
  CConditionOrAssignment          m_caVisibility;
  CConditionOrAssignment          m_caTag;
  TDataFormatForStoneAndTemplate  m_nDataFormat;
  TViewTypeForStoneAndTemplate    m_nView1;
  TViewTypeForStoneAndTemplate    m_nView2;
  //
  long    m_nSourceViewActive;

  CMPModelConstantInds  m_arrConstants;
  CMPModelKeyInds       m_arrKeys;
  CMPModelTextInds      m_arrTexts;
  CMPModelAreaInds      m_arrAreas;
  CMPModelLinkInds      m_arrLinks;
  CMPModelGraphicInds   m_arrGraphics;

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){};
  virtual void ROC_PropertyChanged_ER(){};
  virtual void ROC_PropertyChanged_IFS(){};
  virtual void ROC_PropertyChanged_WP(){};
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
  virtual xtstring SE_GetModuleName(){return GetName();};
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_MPMODELTEMPLATEIND_H_)
