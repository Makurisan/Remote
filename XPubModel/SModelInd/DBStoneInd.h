// DBStoneInd.h: interface for the CDBStoneInd classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DBSTONEIND_H_)
#define _DBSTONEIND_H_


#include "ModelExpImp.h"
#include "XPubVariant.h"

#include "SModelInd/ModelDef.h"
#include "SSyncEng/SyncEngineBase.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32






class CXPubMarkUp;
class CDBStoneInd;
class CMPModelStoneInd;


class XPUBMODEL_EXPORTIMPORT CDBStoneFieldInd : public CModelBaseInd,
                                                public CSyncNotifier_WP,
                                                public CSyncResponse_Proj,
                                                public CSyncResponse_ER,
                                                public CSyncResponse_IFS
{
  friend class CDBStoneInd;
public:
  CDBStoneFieldInd(CDBStoneInd* pParent, CSyncWPNotifiersGroup* pGroup);
  CDBStoneFieldInd(CDBStoneFieldInd& cField);
  CDBStoneFieldInd(CDBStoneFieldInd* pField);
  virtual ~CDBStoneFieldInd();

  void SetDBStoneFieldInd(CDBStoneFieldInd* pField);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  xtstring GetNameForShow() const;

  xtstring GetFieldName() const {return m_sFieldName;};
  void SetFieldName(const xtstring& sName){m_sFieldName = sName; m_bChanged = true;};
  bool GetFieldIsKey() const {return m_bFieldIsKey;};
  void SetFieldIsKey(bool bFieldIsKey){m_bFieldIsKey = bFieldIsKey; m_bChanged = true;};

  CConditionOrAssignment&         GetValue(){return m_caValue;};
  const CConditionOrAssignment&   GetValue_Const() const {return m_caValue;};
  CConditionOrAssignment*         GetValue_Ptr(){return &m_caValue;};

  CXPubVariant GetVariantValue() const {return m_varValue;};
  void SetVariantValue(const CXPubVariant& varValue){m_varValue = varValue; m_bChanged = true;};

  bool UseFieldForInsert(){return (GetValue().GetExprText().size() != 0);};
  bool UseFieldForUpdate(){return (GetValue().GetExprText().size() != 0);};

  bool GetChanged(){return m_bChanged;};
protected:
  void ClearChanged(){m_bChanged = false;};
  void SetParent(CDBStoneInd* pParent){m_pParent = pParent;};

private:
  CDBStoneInd* m_pParent;

  xtstring                m_sFieldName;
  bool                    m_bFieldIsKey;
  CConditionOrAssignment  m_caValue;

  // folgende Variable ist nur in Ausfuehrungsmodul gesetzt
  // dadurch in AddOnFormatWorker kann man die Funktion GetVariantValue benutzen
  // und dadurch kriegt man auch die Information ueber Typ
  CXPubVariant m_varValue;

  // diese Variable ist wegen SyncEngine definiert
  // wenn sich Stand durch SyncEngine aendert, View merkt es nicht,
  // dass sich da was geaendert hat und muss nachfragen
  bool m_bChanged;

protected:
  // requirement on change
  virtual void ROC_PropertyChanged_Proj(){m_bChanged = true;};
  virtual void ROC_PropertyChanged_ER(){m_bChanged = true;};
  virtual void ROC_PropertyChanged_IFS(){m_bChanged = true;};

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

public:
  virtual xtstring SE_GetModuleName(){return _XT("");};
};




typedef std::vector<CDBStoneFieldInd*>    CDBStoneFields;
typedef CDBStoneFields::iterator          CDBStoneFieldsIterator;
typedef CDBStoneFields::reverse_iterator  CDBStoneFieldsReverseIterator;





class XPUBMODEL_EXPORTIMPORT CDBStoneInd : public CModelBaseInd,
                                           public CSyncNotifier_WP,
                                           public CSyncResponse_ER
{
  friend class CDBStoneFieldInd;
public:
  CDBStoneInd(CSyncWPNotifiersGroup* pGroup);
  CDBStoneInd(CDBStoneInd& cStone);
  CDBStoneInd(CDBStoneInd* pStone);
  virtual ~CDBStoneInd();

  void SetDBStoneInd(CDBStoneInd* pStone);
  bool SaveXMLContent(xtstring& sXMLDocText);
  bool ReadXMLContent(const xtstring& sXMLDocText);
  bool SaveXMLContent(CXPubMarkUp *pXMLDoc);
  bool ReadXMLContent(CXPubMarkUp *pXMLDoc);

  /////////////////////////////////////////////////////////////
  // interface fo common properties
  // ERModel name
  xtstring GetERModelName() const {return m_sERModelName;};
  void SetERModelName(const xtstring& sName){m_sERModelName = sName; m_bChanged = true;};
  // Table name
  xtstring GetTableName() const {return m_sTableName;};
  void SetTableName(const xtstring& sName){m_sTableName = sName; m_bChanged = true;};
  // Processing type
  TDBStoneProcessingType GetProcessingType() const {return m_nProcessingType;};
  void SetProcessingType(TDBStoneProcessingType nProcessingType){m_nProcessingType = nProcessingType;};

  size_t CountOfFields() const {return m_arrFields.size();};
  CDBStoneFieldInd* GetField(size_t nIndex) const {assert(nIndex < m_arrFields.size()); if (nIndex < m_arrFields.size()) return m_arrFields[nIndex]; else return NULL;};
  CDBStoneFieldInd* GetField(const xtstring& sName) const;
  void AddField(CDBStoneFieldInd *pField);
  bool RemoveField(size_t nIndex);
  bool RemoveField(const xtstring& sName);
  bool RemoveAllFields();

  bool GetChanged();
  void ClearChanged(){m_bChanged = false;};

  void SetMPStone(CMPModelStoneInd* pMPStone){m_pMPStone = pMPStone;};
private:
  // common properties
  xtstring                m_sERModelName;
  xtstring                m_sTableName;
  TDBStoneProcessingType  m_nProcessingType;

  CDBStoneFields    m_arrFields;

  CMPModelStoneInd* m_pMPStone;

  // diese Variable ist wegen SyncEngine definiert
  // wenn sich Stand durch SyncEngine aendert, View merkt es nicht,
  // dass sich da was geaendert hat und muss nachfragen
  bool m_bChanged;

protected:
  // requirement on change
  virtual void ROC_PropertyChanged_ER(){m_bChanged = true;};

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

public:
  virtual xtstring SE_GetModuleName(){return _XT("");};
};




class XPUBMODEL_EXPORTIMPORT CDBStoneContainer
{
public:
  class CDBOneStone
  {
  public:
    CDBOneStone()
    {
      sName.clear();
      pStone = new CDBStoneInd((CSyncWPNotifiersGroup*)0);
    };
    CDBOneStone(CDBOneStone* pOneStone)
    {
      sName.clear();
      pStone = new CDBStoneInd((CSyncWPNotifiersGroup*)0);
      if (pOneStone && pStone)
      {
        sName = pOneStone->sName;
        pStone->SetDBStoneInd(pOneStone->pStone);
      }
    };
    ~CDBOneStone(){if (pStone)delete pStone;};

    xtstring      sName;
    CDBStoneInd*  pStone;
  };

typedef vector<CDBOneStone*>        CDBStones;
typedef CDBStones::iterator         CDBStonesIterator;
typedef CDBStones::const_iterator   CDBStonesCIterator;
typedef CDBStones::reverse_iterator CDBStonesRIterator;

public:
  CDBStoneContainer();
  ~CDBStoneContainer();

  bool SaveXMLContent(xtstring& sXMLDocText);
  bool ReadXMLContent(const xtstring& sXMLDocText);

  size_t CountOfDBStones();
  CDBOneStone* GetDBStone(size_t nIndex);
  bool AddDBStone(CDBOneStone* pDBStone);
  bool MoveDBStoneUp(size_t nIndex);
  bool MoveDBStoneDown(size_t nIndex);
  bool RemoveDBStone(size_t nIndex);
  bool RemoveAllDBStones();

protected:
  CDBStones m_cStones;
};

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_DBSTONEIND_H_)
