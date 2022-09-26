#if !defined(_SYNCGROUPS_H_)
#define _SYNCGROUPS_H_


#include "SModelInd/ModelExpImp.h"
#include "SyncEngineBase.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32



#define DEFAULT_GROUP_NAME    _XT("_%$$%_DEFAULTGROUP_%$$%_")


////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSyncGroup : public CSyncEngineBase
{
public:
  CSyncGroup(const xtstring& sModuleSyncGroupName, TProjectFileType nModuleSyncGroupType);
  virtual ~CSyncGroup();

  xtstring GetModuleSyncGroupName(){return m_sModuleSyncGroupName;};
  bool IsDefaultModuleGroup(){return (m_nModuleSyncGroupType == tProjectFile_Nothing);};//{return (m_sModuleSyncGroupName == DEFAULT_GROUP_NAME);};
  TProjectFileType GetModuleSyncGroupType(){return m_nModuleSyncGroupType;};

  // Wenn irgendein Objekt aendert sein Zustand (meistenst name wird geaendert|,
  // dann sind durch SyncEngine alle Response Objekte in Liste eingetragen (CUserOfObject)
  // Dann ist ein Dialog durch SyncEngine dargestellt und Benutzer kann auswaehlen,
  // in welchem Objekt will er die Name tatsaechlich aendern
  // Dann sind alle diese Objekte wieder aufgerufen mit Information, was Benutzer
  // eingestellt hat.
  // Alle Response Objekte sind gleichzeitig in irgendeiner CSyncGroup auch eingetragen

  // Funktion StartOfChangesInSyncEngine
  //    ist aufgerufen vor Darstellung des Dialoges und in jeder Gruppe aufgerufen,
  //    von welcher sich ein Response Objekt in Liste eingetragen hat
  // Funktion EndOfChangesInSyncEngine
  //    ist aufterufen danach, was alle Response Objekte benachrichtig wurden,
  //    was Benutzer in dem Dialog eingestellt hat und wieder in jeder Gruppe aufgerufen,
  //    von welcher sich ein Response Objekt in Liste eingetragen hat
  // Beide Funktionen koenen deshalb mehr mal aufgerufen werden.

  // Dadurch kann von CSyncGroup abgeleitete Klasse (es geht um Views) in EndOfChangesInSyncEngine
  // entsprechend reagieren (Refresh View)
  // Natuerlich, mindestens ein Objekt in Gruppe muss die Funktion ObjectStatusChanged aufrufen
  // und dann kann die von CSyncGroup abgeleitete Klasse aufgrund Member m_bAtLeastOneObjectInGroupChanged
  // entscheiden, ob zu Reaktion (Refresh View) kommen sollte
  virtual void StartOfChangesInSyncEngine(){m_bAtLeastOneObjectInGroupChanged = false;};
  virtual void EndOfChangesInSyncEngine(){};
  virtual void ObjectStatusChanged(){m_bAtLeastOneObjectInGroupChanged = true;};


  // neue Funktionsgruppe, die spaeter benannt wird
  virtual void SE_SymbolicDBNameAddedDeletedRenamed(){};
  virtual void SE_ModulesCountChanged(TModulesCountChanged nChanged){};
  virtual void SE_ModuleNameChangedER(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_ModuleNameChangedIFS(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_ModuleNameChangedStone(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_ModuleNameChangedTemplate(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_ModuleNameChangedPython(const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_CountOfTablesInERModelChanged(const xtstring& sERModelName){};
  virtual void SE_CountOfFieldsInERTableChanged(const xtstring& sERModelName, const xtstring& sERTable){};
  virtual void SE_CountOfParamsInERTableChanged(const xtstring& sERModelName, const xtstring& sERTable){};
  virtual void SE_ERTableNameChanged(const xtstring& sERModelName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_ERFieldNameChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sOldName, const xtstring& sNewName){};
  virtual void SE_IFSEntityNameChanged(const xtstring& sIFSModelName, const xtstring& sOldName, const xtstring& sNewName){};
protected:
  void SetModuleSyncGroupName(const xtstring& sModuleSyncGroupName){m_sModuleSyncGroupName = sModuleSyncGroupName;};

  xtstring          m_sModuleSyncGroupName;
  TProjectFileType  m_nModuleSyncGroupType;

private:
  bool m_bAtLeastOneObjectInGroupChanged;
};

////////////////////////////////////////////////////////////////////////////////
typedef std::vector<CSyncNotifier_ER*>  CERNotifiers;
typedef CERNotifiers::iterator          CERNotifiersIterator;
typedef CERNotifiers::const_iterator    CERNotifiersConstIterator;
typedef CERNotifiers::reverse_iterator  CERNotifiersReverseIterator;

class XPUBMODEL_EXPORTIMPORT CSyncERNotifiersGroup : public CSyncGroup
{
public:
  CSyncERNotifiersGroup(const xtstring& sModuleSyncGroupName, TProjectFileType nModuleSyncGroupType);
  virtual ~CSyncERNotifiersGroup();
  CSyncERNotifiersGroup* GetGroup(){return this;};

  bool AddNotifier(CSyncNotifier_ER* pNotifier);
  bool RemoveNotifier(CSyncNotifier_ER* pNotifier);

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

  bool SE_ParamAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, bool bCancelAllowed);
  bool SE_ParamChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParamName, CERModelParamInd* pParam, bool bCancelAllowed);
  bool SE_RelationAdded(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, bool bCancelAllowed);
  bool SE_RelationChanged(const xtstring& sERModelName, const xtstring& sTableName, const xtstring& sParentTableName, const xtstring& sName, CERModelRelationInd* pRelation, bool bCancelAllowed);

  xtstring                  SE_MF_GetModuleName();
  bool                      SE_MF_GetShowRelationNamesER();
  const CERModelTableInd*   SE_MF_GetERModelTableInd(const xtstring& sName);
  xtstring                  SE_MF_GetDBDefinitionName_Production();
  xtstring                  SE_MF_GetDBDefinitionName_Test();
public:
  void SE_SyncFontChanged(CSyncNotifier_ER* pNotifier, const xtstring& sFontName, int nFontHeight);
  void SE_SyncColorChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorBackChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorHeaderChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorHeaderSelectedChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorHeaderTextChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorHeaderSelectedTextChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorSelectedItemChanged(CSyncNotifier_ER* pNotifier, MODEL_COLOR nColor);
  void SE_SyncMoveableChanged(CSyncNotifier_ER* pNotifier, bool bVal);
  void SE_SyncSelectableChanged(CSyncNotifier_ER* pNotifier, bool bVal);
  void SE_SyncVisibleChanged(CSyncNotifier_ER* pNotifier, bool bVal);
  void SE_SyncSizeableChanged(CSyncNotifier_ER* pNotifier, bool bVal);

private:
  CERNotifiers  m_cERNotifiers;
};

////////////////////////////////////////////////////////////////////////////////
typedef std::vector<CSyncNotifier_IFS*> CIFSNotifiers;
typedef CIFSNotifiers::iterator         CIFSNotifiersIterator;
typedef CIFSNotifiers::const_iterator   CIFSNotifiersConstIterator;
typedef CIFSNotifiers::reverse_iterator CIFSNotifiersReverseIterator;

class XPUBMODEL_EXPORTIMPORT CSyncIFSNotifiersGroup : public CSyncGroup
{
public:
  CSyncIFSNotifiersGroup(const xtstring& sModuleSyncGroupName, TProjectFileType nModuleSyncGroupType);
  virtual ~CSyncIFSNotifiersGroup();
  CSyncIFSNotifiersGroup* GetGroup(){return this;};

  bool AddNotifier(CSyncNotifier_IFS* pNotifier);
  bool RemoveNotifier(CSyncNotifier_IFS* pNotifier);

  bool SE_ChangeIFSName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeEntityName(const xtstring& sIFS, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeVariableName(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_ChangeGlobalERTableName(const xtstring& sIFS, const xtstring sOldName, const xtstring& sNewName, bool bCancelAllowed);

  bool SE_CheckReferencesForIFS(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForEntity(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_CheckReferencesForGlobalERTable(const xtstring& sIFS, const xtstring& sName, CUserOfObjectArray& cUsers);

  bool SE_DeleteIFS(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteEntity(const xtstring& sIFS, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteVariable(const xtstring& sIFS, const xtstring& sEntityName, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
  bool SE_DeleteGlobalERTable(const xtstring& sIFS, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);

  void SE_RelationNameChanged();
  void SE_RelationNameDeleted();
  void SE_RelationNameAdded();

  xtstring                  SE_MF_GetModuleName();
  bool                      SE_MF_GetShowRelationNames();
  bool                      SE_MF_GetShowHitLines();
  const CIFSEntityInd*      SE_MF_GetIFSEntityInd(const xtstring& sName);
  xtstring                  SE_MF_GetERModelName();
  void                      SE_MF_GetGlobalTableNames(CxtstringVector& arrNames);
  xtstring                  SE_MF_GetERTableFromGlobalTable(const xtstring& sGlobalTableName);

  void                      SE_MF_ParamTableChanged();
  void                      SE_MF_ERModelChanged(const xtstring& sNewERModelName);
  xtstring                  SE_MF_GetParamTable();
  bool                      SE_MF_SetParamTable(const xtstring& sParamTable);
public:
  void SE_SyncFontChanged(CSyncNotifier_IFS* pNotifier, const xtstring& sFontName, int nFontHeight);
  void SE_SyncColorChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorBackChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorHeaderChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorHeaderSelectedChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorHeaderTextChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorHeaderSelectedTextChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor);
  void SE_SyncColorSelectedItemChanged(CSyncNotifier_IFS* pNotifier, MODEL_COLOR nColor);
  void SE_SyncMoveableChanged(CSyncNotifier_IFS* pNotifier, bool bVal);
  void SE_SyncSelectableChanged(CSyncNotifier_IFS* pNotifier, bool bVal);
  void SE_SyncVisibleChanged(CSyncNotifier_IFS* pNotifier, bool bVal);
  void SE_SyncSizeableChanged(CSyncNotifier_IFS* pNotifier, bool bVal);

private:
  CIFSNotifiers  m_cIFSNotifiers;
};

////////////////////////////////////////////////////////////////////////////////
typedef std::vector<CSyncNotifier_WP*>  CWPNotifiers;
typedef CWPNotifiers::iterator          CWPNotifiersIterator;
typedef CWPNotifiers::const_iterator    CWPNotifiersConstIterator;
typedef CWPNotifiers::reverse_iterator  CWPNotifiersReverseIterator;

class XPUBMODEL_EXPORTIMPORT CSyncWPNotifiersGroup : public CSyncGroup
{
public:
  CSyncWPNotifiersGroup(const xtstring& sModuleSyncGroupName, TProjectFileType nModuleSyncGroupType);
  virtual ~CSyncWPNotifiersGroup();
  CSyncWPNotifiersGroup* GetGroup(){return this;};

  bool AddNotifier(CSyncNotifier_WP* pNotifier);
  bool RemoveNotifier(CSyncNotifier_WP* pNotifier);

  xtstring                  SE_MF_GetModuleName();

  bool SE_ChangeWPName(const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForWP(const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteWP(const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);

  bool SE_ChangeWPConstantName(const xtstring& sWPName, const xtstring& sOldName, const xtstring& sNewName, bool bCancelAllowed);
  bool SE_CheckReferencesForWPConstant(const xtstring& sWPName, const xtstring& sName, CUserOfObjectArray& cUsers);
  bool SE_DeleteWPConstant(const xtstring& sWPName, const xtstring& sName, size_t& nCountOfReferences, bool bCancelAllowed);
private:
  CWPNotifiers  m_cWPNotifiers;
};



#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // _SYNCGROUPS_H_
