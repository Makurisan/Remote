// MPModelIFSInd.h: interface for the CMPModelIFSInd classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MPMODELIFSIND_H_)
#define _MPMODELIFSIND_H_


#include "ModelExpImp.h"

#include "SSyncEng/SyncEngineBase.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CXPubMarkUp;

class CMPModelIFSInd;


class XPUBMODEL_EXPORTIMPORT CMPModelIFSInd : public CModelBaseInd,
                                              public CSyncNotifier_IFS,
                                              public CSyncResponse_ER
{
public:
  CMPModelIFSInd(CSyncIFSNotifiersGroup* pGroup);
  CMPModelIFSInd(CMPModelIFSInd& cModel);
  CMPModelIFSInd(CMPModelIFSInd* pModel);
  virtual ~CMPModelIFSInd();

  void SetMPModelIFSInd(CMPModelIFSInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);
  static bool ReadXMLModuleName(CXPubMarkUp *pXMLDoc, xtstring& sModuleName);
  void CopyDiverseProperties(CMPModelIFSInd* pModel);
  void CopyPositionProperties(CMPModelIFSInd* pModel);

  bool SE_DeleteInformationSight(size_t& nCountOfReferences, bool bCancelAllowed)
  {return SE_DeleteIFS(GetName(), nCountOfReferences, bCancelAllowed);};

  /////////////////////////////////////////////////////////////
  // interface for common properties
  // Name
  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  // autor
  xtstring GetAutor(){return m_sAutor;};
  void SetAutor(const xtstring& sAutor){m_sAutor = sAutor;};
  // password
  xtstring GetPassword(){return m_sPassword;};
  void SetPassword(const xtstring& sPassword){m_sPassword = sPassword;};

  TIFSTypeForThirdParty GetIFSTypeForThirdParty(){return m_nIFSTypeForThirdParty;};
  void SetIFSTypeForThirdParty(TIFSTypeForThirdParty nIFSTypeForThirdParty){m_nIFSTypeForThirdParty = nIFSTypeForThirdParty;};
  xtstring GetIFSNameForThirdParty(){return m_sIFSNameForThirdParty;};
  void SetIFSNameForThirdParty(const xtstring& sName){m_sIFSNameForThirdParty = sName;};
  bool GetShowOpenedEntitiesHierarchical(){return m_bShowOpenedEntitiesHierarchical;};
  void SetShowOpenedEntitiesHierarchical(bool bHierarchical){m_bShowOpenedEntitiesHierarchical = bHierarchical;};
  bool GetShowOpenedNameAlways(){return m_bShowOpenedNameAlways;};
  void SetShowOpenedNameAlways(bool bShowAlways){m_bShowOpenedNameAlways = bShowAlways;};
  bool GetShowExpanded(){return m_bShowExpanded;};
  void SetShowExpanded(bool bShowExpanded){m_bShowExpanded = bShowExpanded;};
  TShowBehaviour GetShowBehaviour(){return m_nShowBehaviour;};
  void SetShowBehaviour(TShowBehaviour nShowBehaviour){m_nShowBehaviour = nShowBehaviour;};

  /////////////////////////////////////////////////////////////
  // interface for ermodel properties
  xtstring GetERModelName() const {return m_sERModelName;};
  void SetERModelName(const xtstring& sName){m_sERModelName = sName;};


  /////////////////////////////////////////////////////////////
  // interface for font properties
  // yet not implemented
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
  MODEL_COLOR GetColor(){return m_nColor;};
  void SetColor(MODEL_COLOR nColor){m_nColor = nColor;};
  MODEL_COLOR GetColorBack(){return m_nColorBack;};
  void SetColorBack(MODEL_COLOR nColor){m_nColorBack = nColor;};
  bool GetMoveable(){return m_bMoveable;};
  void SetMoveable(bool bMoveable){m_bMoveable = bMoveable;};
  bool GetSelectable(){return m_bSelectable;};
  void SetSelectable(bool bSelectable){m_bSelectable = bSelectable;};
  bool GetVisible(){return m_bVisible;};
  void SetVisible(bool bVisible){m_bVisible = bVisible;};
  bool GetSizeable(){return m_bSizeable;};
  void SetSizeable(bool bSizeable){m_bSizeable = bSizeable;};

  virtual void PropertiesChanged(){};

  xtstring GetParamTableName() const {return m_sParamTable;};
  void SetParamTableName(const xtstring& sParamTable){m_sParamTable = sParamTable;};
  xtstring GetIFSPropertyStones() const {return m_sIFSPropertyStones;};
  void SetIFSPropertyStones(const xtstring& sIFSPropertyStones){m_sIFSPropertyStones = sIFSPropertyStones;};
  xtstring GetIFSDBStones() const {return m_sIFSDBStones;};
  void SetIFSDBStones(const xtstring& sIFSDBStones){m_sIFSDBStones = sIFSDBStones;};

  bool GetShowRelationNames(){return m_bShowRelationNames;};
  void SetShowRelationNames(bool bShowRelationNames){m_bShowRelationNames = bShowRelationNames;};
  bool GetShowHitLines(){return m_bShowHitLines;};
  void SetShowHitLines(bool bShowHitLines){m_bShowHitLines = bShowHitLines;};
protected:

  virtual void ShowNamesPropertyChanged(){};

private:
  xtstring m_sParamTable;
  xtstring m_sIFSPropertyStones;
  xtstring m_sIFSDBStones;
  bool m_bShowRelationNames;
  bool m_bShowHitLines;
  // common properties
  xtstring m_sName;
  xtstring m_sAutor;
  xtstring m_sPassword;
  TIFSTypeForThirdParty m_nIFSTypeForThirdParty;
  xtstring              m_sIFSNameForThirdParty;
  bool          m_bShowOpenedEntitiesHierarchical;
  bool          m_bShowOpenedNameAlways;
  bool          m_bShowExpanded;
  TShowBehaviour  m_nShowBehaviour;
  // er model
  xtstring m_sERModelName;
  // position properties
  long    m_nPosLeft;
  long    m_nPosTop;
  long    m_nSizeWidth;
  long    m_nSizeHeight;
  // diverse properties
  MODEL_COLOR m_nColor;
  MODEL_COLOR m_nColorBack;
  bool        m_bMoveable;
  bool        m_bSelectable;
  bool        m_bVisible;
  bool        m_bSizeable;
protected:
  // for internal use
  virtual void OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName);

protected:
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_ER(){PropertiesChanged();};
  virtual void ROC_PropertyChanged_IFS(){PropertiesChanged();};
  virtual void ROC_PropertyChanged_WP(){PropertiesChanged();};
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

protected:
  virtual void SE_OnEntityNameChanged(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(){};
  virtual void SE_OnRelationNameDeleted(){};
  virtual void SE_OnRelationNameAdded(){};

  virtual xtstring SE_GetModuleName(){return GetName();};
  virtual xtstring SE_GetERModelName() const {return GetERModelName();};
  virtual void SE_GetGlobalTableNames(CxtstringVector& arrNames){};
  virtual xtstring SE_GetERTableFromGlobalTable(const xtstring& sGlobalTableName){return _XT("");};
  virtual bool SE_GetShowRelationNames(){return GetShowRelationNames();};
  virtual bool SE_GetShowHitLines(){return GetShowHitLines();};
  virtual const CIFSEntityInd* SE_GetIFSEntityInd(){return 0;};
  virtual xtstring SE_GetParamTableName() const {return GetParamTableName();};
  virtual void SE_SetParamTableName(const xtstring sParamTable){SetParamTableName(sParamTable);};
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


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_MPMODELIFSIND_H_)
