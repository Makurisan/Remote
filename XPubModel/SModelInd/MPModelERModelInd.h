// MPModelERModelInd.h: interface for the CMPModelERModelInd classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MPMODELERMODELIND_H_)
#define _MPMODELERMODELIND_H_


#include "ModelExpImp.h"

#include "SSyncEng/SyncEngineBase.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CXPubMarkUp;

class CMPModelERModelInd;


class XPUBMODEL_EXPORTIMPORT CMPModelERModelInd : public CModelBaseInd,
                                                  public CSyncNotifier_ER,
                                                  public CSyncResponse_Proj
{
public:
  CMPModelERModelInd(CSyncERNotifiersGroup* pGroup);
  CMPModelERModelInd(CMPModelERModelInd& cModel);
  CMPModelERModelInd(CMPModelERModelInd* pModel);
  virtual ~CMPModelERModelInd();

  void SetMPModelERModelInd(CMPModelERModelInd* pModel);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);
  static bool ReadXMLModuleName(CXPubMarkUp *pXMLDoc, xtstring& sModuleName);
  void CopyDiverseProperties(CMPModelERModelInd* pModel);
  void CopyPositionProperties(CMPModelERModelInd* pModel);

  bool SE_DeleteERModel(size_t& nCountOfReferences, bool bCancelAllowed)
    {return CSyncNotifier_ER::SE_DeleteERModel(GetName(), nCountOfReferences, bCancelAllowed);};

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
  /////////////////////////////////////////////////////////////
  // interface for database properties
  // production database
  xtstring GetDBNameProd(){return m_sDBNameProd;};
  void SetDBNameProd(const xtstring& sName){m_sDBNameProd = sName;};
  // test database
  xtstring GetDBNameTest(){return m_sDBNameTest;};
  void SetDBNameTest(const xtstring& sName){m_sDBNameTest = sName;};

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
  virtual void ShowNamesPropertyChanged(){};

  bool GetShowRelationNamesER(){return m_bShowRelationNamesER;};
  void SetShowRelationNamesER(bool bShowRelationNames){m_bShowRelationNamesER = bShowRelationNames;};

private:
  bool m_bShowRelationNamesER;
  // common properties
  xtstring m_sName;
  xtstring m_sAutor;
  xtstring m_sPassword;
  // database properties
  xtstring m_sDBNameProd;
  xtstring m_sDBNameTest;
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
  // sync engine functions
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){PropertiesChanged();};
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

protected:
  virtual void SE_OnTableNameChanged(const xtstring& sOldName, const xtstring& sNewName);
  virtual void SE_OnTableDelete(const xtstring& sName){};
  virtual void SE_OnFieldNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnParamNameChanged(const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_OnRelationNameChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual bool SE_OnRelationNameWillBeChanged(const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sOldName, const xtstring& sNewName){return true;};

  virtual xtstring SE_GetModuleName(){return GetName();};
  virtual bool SE_GetShowRelationNamesER(){return GetShowRelationNamesER();};
  virtual xtstring SE_GetDBDefinitionName_Production(){return GetDBNameProd();};
  virtual xtstring SE_GetDBDefinitionName_Test(){return GetDBNameTest();};
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


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_MPMODELERMODELIND_H_)
