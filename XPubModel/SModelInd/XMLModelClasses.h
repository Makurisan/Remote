#if !defined(_XMLMODELCLASSES_H_)
#define _XMLMODELCLASSES_H_


#include "ModelExpImp.h"
#include "ModelDef.h"
#include "ModelInd.h"
#include "ERModelInd.h"
#include "IFSEntityInd.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CXPubMarkUp;

#define HIERARCHY_DELIMITER   _XT('.')



class CHierarchyItem;
class COpenedItem;
class CConditionItem;
class CShowBehaviourItem;
class CParametersItem;
class CShowExpandedItem;
class COpenedItemsContainer;
class CSettingsOfOpenedItemsContainer;


typedef enum THierarchyItemElement
{
  tHIE_Unknown,
  tHIE_InformationSight,
  tHIE_OpenedEntity,
  tHIE_EntityForSubIFS,
  tHIE_OpenedEntityForSubIFS,
};

typedef enum THierarchyItemActiveCondition
{
  tHIAC_Unknown,
  tHIAC_Index,              // originale condition
  tHIAC_IndexAndCondition,  // overloaded original condition
  tHIAC_Name,               // originale condition
  tHIAC_NameAndCondition,   // overloaded original condition
  tHIAC_NewCondition,       // new condition
};

typedef vector<THierarchyItemElement>             CHierarchyItemElements;
typedef CHierarchyItemElements::iterator          CHierarchyItemElementsIterator;
typedef CHierarchyItemElements::const_iterator    CHierarchyItemElementsConstIterator;
typedef CHierarchyItemElements::reverse_iterator  CHierarchyItemElementsReverseIterator;

typedef vector<TShowBehaviour>              CShowBehaviours;
typedef CShowBehaviours::iterator           CShowBehavioursIterator;
typedef CShowBehaviours::const_iterator     CShowBehavioursConstIterator;
typedef CShowBehaviours::reverse_iterator   CShowBehavioursReverseIterator;


////////////////////////////////////////////////////////////////////////////////
// CHierarchyItem
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CHierarchyItem
{
  friend class COpenedItem;
  friend class CConditionItem;
  friend class COpenedItemsContainer;
  friend class CSettingsOfOpenedItemsContainer;
public:
  CHierarchyItem();
  virtual ~CHierarchyItem();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  size_t DeepOfHierarchy() const {return m_cHE_Names.size();};

  xtstring              GetOpenedNameOfLevel(size_t nLevel) const;
  xtstring              GetNameOfLevel(size_t nLevel) const;
  THierarchyItemElement GetTypeOfLevel(size_t nLevel) const;
  bool                  GetShowSubIFS(size_t nLevel) const;
  bool                  GetShowHierarchical(size_t nLevel) const;
  bool                  GetShowExpanded(size_t nLevel) const;
  TShowBehaviour        GetShowBehaviour(size_t nLevel) const;
  bool                  GetShowAddCondition(size_t nLevel) const;
  bool                  GetShowAddFreeSelection(size_t nLevel) const;

  xtstring              GetLastOpenedName() const;
  xtstring              GetLastName() const;
  THierarchyItemElement GetLastType() const;
  bool                  GetLastShowSubIFS() const;
  bool                  GetLastShowHierarchical() const;
  bool                  GetLastShowExpanded() const;
  TShowBehaviour        GetLastShowBehaviour() const;
  bool                  GetLastShowAddCondition() const;
  bool                  GetLastShowAddFreeSelection() const;

  xtstring GetHierarchyPath() const {return m_sHierarchyPath;};

  bool ItemIsChild(const CHierarchyItem* pItem) const;
  bool ItemIsDirectChild(const CHierarchyItem* pItem, bool bIFSToo) const;
  bool ItemIsParent(const CHierarchyItem* pItem) const;
  bool ItemIsDirectParent(const CHierarchyItem* pItem, bool bIFSToo) const;

  void AddHierarchyElement_InformationSight(const xtstring& sOpenedName,
                                            const xtstring& sName,
                                            bool bShowSubIFS,
                                            bool bShowHierarchical,
                                            bool bShowExpanded,
                                            TShowBehaviour nShowBehaviour,
                                            bool bShowAddCondition,
                                            bool bShowAddFreeSelection);
  void AddHierarchyElement_OpenedEntity(const xtstring& sOpenedName,
                                        const xtstring& sName,
                                        bool bShowExpanded,
                                        TShowBehaviour nShowBehaviour,
                                        bool bShowAddCondition,
                                        bool bShowAddFreeSelection);
  void AddHierarchyElement_EntityForSubIFS(const xtstring& sOpenedName,
                                           const xtstring& sName,
                                           bool bShowExpanded,
                                           TShowBehaviour nShowBehaviour,
                                           bool bShowAddCondition,
                                           bool bShowAddFreeSelection);
  void AddHierarchyElement_OpenedEntityForSubIFS(const xtstring& sOpenedName,
                                                 const xtstring& sName,
                                                 bool bShowExpanded,
                                                 TShowBehaviour nShowBehaviour,
                                                 bool bShowAddCondition,
                                                 bool bShowAddFreeSelection);

  void RemoveLastHierarchyElement();

  // da ist es fraglich, ob das PUBLIC sein darf
  bool AssignFrom(const CHierarchyItem* pItem);
protected:
  void AddHierarchyElement(const xtstring& sOpenedName,
                           const xtstring& sName,
                           THierarchyItemElement nType,
                           bool bShowSubIFS,
                           bool bShowHierarchical,
                           bool bShowExpanded,
                           TShowBehaviour nShowBehaviour,
                           bool bShowAddCondition,
                           bool bShowAddFreeSelection);


  void BuildHierarchyPath();

  // erstes Element ist top element
  CxtstringVector                 m_cHE_OpenedNames;
  CxtstringVector                 m_cHE_Names;
  CHierarchyItemElements          m_cHE_Types;
  CboolVector                     m_cHE_ShowSubIFS;
  CboolVector                     m_cHE_ShowHierarchical;
  CboolVector                     m_cHE_ShowExpanded;
  CShowBehaviours                 m_cHE_ShowBehaviour;
  CboolVector                     m_cHE_ShowAddCondition;
  CboolVector                     m_cHE_ShowAddFreeSelection;

  xtstring                        m_sHierarchyPath;
};

////////////////////////////////////////////////////////////////////////////////
// COpenedItem
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT COpenedItem : public CHierarchyItem
{
  friend class COpenedItemsContainer;
public:
  COpenedItem();
  ~COpenedItem();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool AssignFrom(const CHierarchyItem* pItem);
  bool AssignFrom(const COpenedItem* pItem);
  bool AssignFrom(const CConditionItem* pItem);

  bool AssignConditionsParametersSQLCommandsFrom(const COpenedItem* pItem);

  size_t GetIndexOfItem(const CIFSEntityOneConditionVariation* pItem) const;
  size_t GetIndexOfItem(const CIFSEntityOneSQLCommandVariation* pItem) const;

  CIFSEntityConditionVariations* GetConditions(){return &m_cConditions;};
  const CIFSEntityConditionVariations* GetConstConditions() const {return &m_cConditions;};

  CIFSEntityAllParameters* GetParameters(){return &m_cParameters;};
  const CIFSEntityAllParameters* GetConstParameters() const {return &m_cParameters;};

  CIFSEntitySQLCommandVariations* GetSQLCommands(){return &m_cSQLCommands;};
  const CIFSEntitySQLCommandVariations* GetConstSQLCommands() const {return &m_cSQLCommands;};

  bool IsIdenticalForXPubClient(const COpenedItem* pItem);
protected:
  bool SetConditionsParametersSQLCommands(const CIFSEntityConditionVariations* pConditions,
                                          const CIFSEntityAllParameters* pParameters,
                                          const CIFSEntitySQLCommandVariations* pSQLCommands);

  CIFSEntityConditionVariations   m_cConditions;
  CIFSEntityAllParameters         m_cParameters;
  CIFSEntitySQLCommandVariations  m_cSQLCommands;
};

////////////////////////////////////////////////////////////////////////////////
// CConditionItem
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CConditionItem : public CHierarchyItem
{
  friend class CSettingsOfOpenedItemsContainer;
public:
  CConditionItem();
  ~CConditionItem();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool AssignFrom(const CHierarchyItem* pItem);
  bool AssignFrom(const COpenedItem* pItem);
  bool AssignFrom(const CConditionItem* pItem);

  THierarchyItemActiveCondition GetConditionType() const {return m_nConditionType;};
  size_t GetConditionIndex() const {return m_nIndex;};
  xtstring GetConditionName() const {return m_sName;};

  CConditionOrAssignment&         GetCondition(){return m_caCondition;};
  const CConditionOrAssignment&   GetCondition_Const() const {return m_caCondition;};
  CConditionOrAssignment*         GetCondition_Ptr(){return &m_caCondition;};

  bool GetDontOptimizeCondition() const {return m_bDontOptimizeCondition;};
  
protected:
  void SetConditionType(THierarchyItemActiveCondition nType){m_nConditionType = nType;};
  void SetConditionIndex(size_t nIndex){m_nIndex = nIndex;};
  void SetConditionName(const xtstring& sName){m_sName = sName;};
  void SetDontOptimizeCondition(bool bDontOptimizeCondition){m_bDontOptimizeCondition = bDontOptimizeCondition;}


  THierarchyItemActiveCondition   m_nConditionType;
  // m_nConditionType --> tHIAC_Index || tHIAC_IndexAndCondition
  size_t    m_nIndex;
  // m_nConditionType --> tHIAC_Name || tHIAC_NameAndCondition
  xtstring  m_sName;
  // m_nConditionType --> tHIAC_NewCondition || tHIAC_IndexAndCondition || tHIAC_NameAndCondition
  CConditionOrAssignment  m_caCondition;
  bool      m_bDontOptimizeCondition;
};

////////////////////////////////////////////////////////////////////////////////
// CShowBehaviourItem
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CShowBehaviourItem : public CHierarchyItem
{
  friend class CSettingsOfOpenedItemsContainer;
public:
  CShowBehaviourItem();
  ~CShowBehaviourItem();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool AssignFrom(const CHierarchyItem* pItem);
  bool AssignFrom(const COpenedItem* pItem);
  bool AssignFrom(const CConditionItem* pItem);
  bool AssignFrom(const CShowBehaviourItem* pItem);

  TShowBehaviour GetShowBehaviour() const {return m_nShowBehaviour;};
  void SetShowBehaviour(TShowBehaviour nShowBehaviour){m_nShowBehaviour = nShowBehaviour;};

protected:
  TShowBehaviour m_nShowBehaviour;
};

////////////////////////////////////////////////////////////////////////////////
// CParametersItem
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CParametersItem : public CHierarchyItem
{
  friend class CSettingsOfOpenedItemsContainer;
public:
  CParametersItem();
  ~CParametersItem();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool AssignFrom(const CHierarchyItem* pItem);
  bool AssignFrom(const COpenedItem* pItem);
  bool AssignFrom(const CConditionItem* pItem);
  bool AssignFrom(const CParametersItem* pItem);

  const CIFSEntityAllParameters* GetParameters() const {return &m_cParameters;};
  void SetParameters(const CIFSEntityAllParameters* pParameters){if (pParameters) m_cParameters.AssignFrom(pParameters);};

protected:
  CIFSEntityAllParameters m_cParameters;
};

////////////////////////////////////////////////////////////////////////////////
// CFreeSelectionItem
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CFreeSelectionItem : public CHierarchyItem
{
  friend class CSettingsOfOpenedItemsContainer;
public:
  CFreeSelectionItem();
  ~CFreeSelectionItem();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool AssignFrom(const CHierarchyItem* pItem);
  bool AssignFrom(const COpenedItem* pItem);
  bool AssignFrom(const CConditionItem* pItem);
  bool AssignFrom(const CFreeSelectionItem* pItem);

  const CIFSEntityOneSQLCommandVariation* GetSQLCommand() const {return &m_cSQLCommand;};
  void SetSQLCommand(const CIFSEntityOneSQLCommandVariation* pSQLCommand){if (pSQLCommand) m_cSQLCommand.AssignFrom(pSQLCommand);};

protected:
  CIFSEntityOneSQLCommandVariation m_cSQLCommand;
};


////////////////////////////////////////////////////////////////////////////////
// CShowExpandedItem
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CShowExpandedItem : public CHierarchyItem
{
  friend class CSettingsOfOpenedItemsContainer;
public:
  CShowExpandedItem();
  ~CShowExpandedItem();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool AssignFrom(const CHierarchyItem* pItem);
  bool AssignFrom(const COpenedItem* pItem);
  bool AssignFrom(const CConditionItem* pItem);
  bool AssignFrom(const CShowExpandedItem* pItem);

  bool GetShowExpanded() const {return m_bShowExpanded;};
  void SetShowExpanded(bool bShowExpanded){m_bShowExpanded = bShowExpanded;};

protected:
  bool        m_bShowExpanded;
};





typedef vector<COpenedItem*>                  COpenedItems;
typedef COpenedItems::iterator                COpenedItemsIterator;
typedef COpenedItems::const_iterator          COpenedItemsConstIterator;
typedef COpenedItems::reverse_iterator        COpenedItemsReverseIterator;

typedef vector<CConditionItem*>               CConditionItems;
typedef CConditionItems::iterator             CConditionItemsIterator;
typedef CConditionItems::const_iterator       CConditionItemsConstIterator;
typedef CConditionItems::reverse_iterator     CConditionItemsReverseIterator;

typedef vector<CShowBehaviourItem*>             CShowBehaviourItems;
typedef CShowBehaviourItems::iterator           CShowBehaviourItemsIterator;
typedef CShowBehaviourItems::const_iterator     CShowBehaviourItemsConstIterator;
typedef CShowBehaviourItems::reverse_iterator   CShowBehaviourItemsReverseIterator;

typedef vector<CParametersItem*>              CParametersItems;
typedef CParametersItems::iterator            CParametersItemsIterator;
typedef CParametersItems::const_iterator      CParametersItemsConstIterator;
typedef CParametersItems::reverse_iterator    CParametersItemsReverseIterator;

typedef vector<CFreeSelectionItem*>           CFreeSelectionItems;
typedef CFreeSelectionItems::iterator         CFreeSelectionItemsIterator;
typedef CFreeSelectionItems::const_iterator   CFreeSelectionItemsConstIterator;
typedef CFreeSelectionItems::reverse_iterator CFreeSelectionItemsReverseIterator;

typedef vector<CShowExpandedItem*>            CShowExpandedItems;
typedef CShowExpandedItems::iterator          CShowExpandedItemsIterator;
typedef CShowExpandedItems::const_iterator    CShowExpandedItemsConstIterator;
typedef CShowExpandedItems::reverse_iterator  CShowExpandedItemsReverseIterator;




////////////////////////////////////////////////////////////////////////////////
// COpenedItemsContainer
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT COpenedItemsContainer
{
public:
  COpenedItemsContainer();
  ~COpenedItemsContainer();

  xtstring GetXMLDoc();
  bool SetXMLDoc(const xtstring& sXMLDoc);
  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  void ResetContent();

  void InsertLevel_InformationSight(const xtstring& sOpenedName,
                                    const xtstring& sName,
                                    bool bShowSubIFS,
                                    bool bShowHierarchical,
                                    bool bShowExpanded,
                                    TShowBehaviour nShowBehaviour,
                                    bool bShowAddCondition,
                                    bool bShowAddFreeSelection)
    {m_cStack.AddHierarchyElement_InformationSight(sOpenedName, sName, bShowSubIFS, bShowHierarchical, bShowExpanded, nShowBehaviour, bShowAddCondition, bShowAddFreeSelection);};
  void InsertLevel_OpenedEntity(const xtstring& sOpenedName,
                                const xtstring& sName,
                                bool bShowExpanded,
                                TShowBehaviour nShowBehaviour,
                                bool bShowAddCondition,
                                bool bShowAddFreeSelection)
    {m_cStack.AddHierarchyElement_OpenedEntity(sOpenedName, sName, bShowExpanded, nShowBehaviour, bShowAddCondition, bShowAddFreeSelection);};
  void InsertLevel_EntityForSubIFS(const xtstring& sOpenedName,
                                   const xtstring& sName,
                                   bool bShowExpanded,
                                   TShowBehaviour nShowBehaviour,
                                   bool bShowAddCondition,
                                   bool bShowAddFreeSelection)
    {m_cStack.AddHierarchyElement_EntityForSubIFS(sOpenedName, sName, bShowExpanded, nShowBehaviour, bShowAddCondition, bShowAddFreeSelection);};
  void InsertLevel_OpenedEntityForSubIFS(const xtstring& sOpenedName,
                                         const xtstring& sName,
                                         bool bShowExpanded,
                                         TShowBehaviour nShowBehaviour,
                                         bool bShowAddCondition,
                                         bool bShowAddFreeSelection)
    {m_cStack.AddHierarchyElement_OpenedEntityForSubIFS(sOpenedName, sName, bShowExpanded, nShowBehaviour, bShowAddCondition, bShowAddFreeSelection);};
  void LeaveLevel(){m_cStack.RemoveLastHierarchyElement();};

  bool InsertActualLevel(CIFSEntityConditionVariations* pConditions,
                         CIFSEntityAllParameters* pParameters,
                         CIFSEntitySQLCommandVariations* pSQLCommands);
  bool InsertItem(const CHierarchyItem* pItem);

  size_t CountOfItems(){return m_Items.size();};
  size_t GetIndexOfItem(const COpenedItem* pItem) const;
  const COpenedItem* GetItem(size_t nIndex);
  const COpenedItem* GetItemFromPath(const xtstring& sPath);
  COpenedItem* GetItemFromPathNotConst(const xtstring& sPath);

  const COpenedItem*    GetFirstItem();
  const COpenedItem*    GetNextSiblingItem(const COpenedItem* pItem);
  //size_t                GetCountOfChilds(const COpenedItem* pItem);
  const COpenedItem*    GetFirstChildItem(const COpenedItem* pItem, bool bIFSToo);

  COpenedItem* GetItemNotConstFromPath(const xtstring& sPath);

  // Funktionen fuer XPubClient
  bool InsertItem_Original(const CHierarchyItem* pItem_NotOrig);
  const COpenedItem* GetItemFromPath_Original(const xtstring& sPath_NotOrig);
  COpenedItem* GetItemNotConstFromPath_Original(const xtstring& sPath_NotOrig);
  const COpenedItem* GetItemForEntity(const xtstring& sEntity);
  COpenedItem* GetItemNotConstForEntity(const xtstring& sEntity);
protected:
  const COpenedItemsConstIterator GetNextItemFromList(const COpenedItem* pItem);

  CHierarchyItem  m_cStack;
  COpenedItems    m_Items;
};

////////////////////////////////////////////////////////////////////////////////
// CSettingsOfOpenedItemsContainer
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CSettingsOfOpenedItemsContainer
{
public:
  CSettingsOfOpenedItemsContainer();
  ~CSettingsOfOpenedItemsContainer();

  xtstring GetXMLDoc();
  bool SetXMLDoc(const xtstring& sXMLDoc);
  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  void ResetContent();

  bool AddConditionsAndLocks(CSettingsOfOpenedItemsContainer* pItem);

  // condition items
  size_t CountOfConditionItems(){return m_CItems.size();};
  const CConditionItem* GetConditionItem(size_t nIndex);
  const CConditionItem* GetConditionItemFromPath(const xtstring& sPath);

  bool SetActiveConditionForItem(const CHierarchyItem* pItem, size_t nConditionIndex);
  bool SetActiveConditionForItem(const CHierarchyItem* pItem, size_t nConditionIndex, const CConditionOrAssignment& caCondition, bool bDontOptimizeCondition);
  bool SetActiveConditionForItem(const CHierarchyItem* pItem, const xtstring& sConditionName);
  bool SetActiveConditionForItem(const CHierarchyItem* pItem, const xtstring& sConditionName, const CConditionOrAssignment& caCondition, bool bDontOptimizeCondition);
  bool SetActiveConditionForItem(const CHierarchyItem* pItem, const CConditionOrAssignment& caCondition, bool bDontOptimizeCondition);

  // lock items
  size_t CountOfLockItems(){return m_LItems.size();};
  const CShowBehaviourItem* GetShowBehaviourItem(size_t nIndex);
  const CShowBehaviourItem* GetShowBehaviourItemFromPath(const xtstring& sPath);

  bool SetShowBehaviourForItem(const CHierarchyItem* pItem, TShowBehaviour nShowBehaviour);

  // parameters items
  size_t CountOfParametersItems(){return m_PItems.size();};
  const CParametersItem* GetParametersItem(size_t nIndex);
  const CParametersItem* GetParametersItemFromPath(const xtstring& sPath);

  bool SetParametersForItem(const CHierarchyItem* pItem, const CIFSEntityAllParameters* pParameters);

  // free selections
  size_t CountOfFreeSelectionItems(){return m_FItems.size();};
  const CFreeSelectionItem* GetFreeSelectionItem(size_t nIndex);
  const CFreeSelectionItem* GetFreeSelectionItemFromPath(const xtstring& sPath);

  bool SetActiveFreeSelectionItem(const CHierarchyItem* pItem, const CIFSEntityOneSQLCommandVariation* pSQLCommand);

  // show expanded items
  size_t CountOfSwhoExpandedItems(){return m_SEItems.size();};
  const CShowExpandedItem* GetShowExpandedItem(size_t nIndex);
  const CShowExpandedItem* GetShowExpandedItemFromPath(const xtstring& sPath);

  bool SetShowExpandedForItem(const CHierarchyItem* pItem, bool bShowExpanded);

protected:

  CConditionItem*             GetConditionItemNotConstFromPath(const xtstring& sPath);
  CShowBehaviourItem*         GetShowBehaviourItemNotConstFromPath(const xtstring& sPath);
  CParametersItem*            GetParametersItemNotConstFromPath(const xtstring& sPath);
  CFreeSelectionItem*         GetFreeSelectionItemNotConstFromPath(const xtstring& sPath);
  CShowExpandedItem*          GetShowExpandedItemNotConstFromPath(const xtstring& sPath);

  CConditionItems           m_CItems;
  CShowBehaviourItems       m_LItems;
  CParametersItems          m_PItems;
  CFreeSelectionItems       m_FItems;
  CShowExpandedItems        m_SEItems;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_XMLMODELCLASSES_H_)
