#include <assert.h>

#include "SSTLdef/STLdef.h"
#include "Sxml/XPubMarkUp.h"
#include "Sxml/XMLTagNames.h"
#include "XMLModelClasses.h"





////////////////////////////////////////////////////////////////////////////////
// CHierarchyItem
////////////////////////////////////////////////////////////////////////////////
CHierarchyItem::CHierarchyItem()
{
}

CHierarchyItem::~CHierarchyItem()
{
}

bool CHierarchyItem::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_cHE_OpenedNames.erase(m_cHE_OpenedNames.begin(), m_cHE_OpenedNames.end());
  m_cHE_Names.erase(m_cHE_Names.begin(), m_cHE_Names.end());
  m_cHE_Types.erase(m_cHE_Types.begin(), m_cHE_Types.end());
  m_cHE_ShowSubIFS.erase(m_cHE_ShowSubIFS.begin(), m_cHE_ShowSubIFS.end());
  m_cHE_ShowHierarchical.erase(m_cHE_ShowHierarchical.begin(), m_cHE_ShowHierarchical.end());
  m_cHE_ShowExpanded.erase(m_cHE_ShowExpanded.begin(), m_cHE_ShowExpanded.end());
  m_cHE_ShowBehaviour.erase(m_cHE_ShowBehaviour.begin(), m_cHE_ShowBehaviour.end());
  m_cHE_ShowAddCondition.erase(m_cHE_ShowAddCondition.begin(), m_cHE_ShowAddCondition.end());
  m_cHE_ShowAddFreeSelection.erase(m_cHE_ShowAddFreeSelection.begin(), m_cHE_ShowAddFreeSelection.end());

  xtstring sOpenedName;
  xtstring sName;
  THierarchyItemElement nType;
  bool bShowSubIFS;
  bool bShowHierarchical;
  bool bShowExpanded;
  TShowBehaviour nShowBehaviour;
  bool bShowAddCondition;
  bool bShowAddFreeSelection;
  char sKey[50];

  // XMLDoc in <irgendwo>
  for (int nI = 0;;nI++)
  {
    // suchen, suchen, suchen
    sprintf(sKey, HIERARCHYITEM_OPENEDNAME, nI);
      sOpenedName = pXMLDoc->GetAttrib(sKey);
    sprintf(sKey, HIERARCHYITEM_NAME, nI);
      sName = pXMLDoc->GetAttrib(sKey);
    sprintf(sKey, HIERARCHYITEM_TYPE, nI);
      nType = (THierarchyItemElement)pXMLDoc->GetAttribLong(sKey);
    sprintf(sKey, HIERARCHYITEM_SHOWSUBIFS, nI);
      bShowSubIFS = (pXMLDoc->GetAttribLong(sKey) != 0);
    sprintf(sKey, HIERARCHYITEM_SHOWHIERARCHICAL, nI);
      bShowHierarchical = (pXMLDoc->GetAttribLong(sKey) != 0);
    sprintf(sKey, HIERARCHYITEM_SHOWEXPANDED, nI);
      bShowExpanded = (pXMLDoc->GetAttribLong(sKey) != 0);
    sprintf(sKey, HIERARCHYITEM_SHOWBEHAVIOUR, nI);
      nShowBehaviour = (TShowBehaviour)pXMLDoc->GetAttribLong(sKey);
      if (nShowBehaviour <= tShowBehaviour_FirstDummy || nShowBehaviour >= tShowBehaviour_LastDummy)
        nShowBehaviour = tShowBehaviour_Default;
    sprintf(sKey, HIERARCHYITEM_SHOWADDCONDITION, nI);
      bShowAddCondition = (pXMLDoc->GetAttribLong(sKey) != 0);
    sprintf(sKey, HIERARCHYITEM_SHOWADDFREESELECTION, nI);
      bShowAddFreeSelection = (pXMLDoc->GetAttribLong(sKey) != 0);

    if (!sName.size())
      break;

    m_cHE_OpenedNames.push_back(sOpenedName);
    m_cHE_Names.push_back(sName);
    m_cHE_Types.push_back(nType);
    m_cHE_ShowSubIFS.push_back(bShowSubIFS);
    m_cHE_ShowHierarchical.push_back(bShowHierarchical);
    m_cHE_ShowExpanded.push_back(bShowExpanded);
    m_cHE_ShowBehaviour.push_back(nShowBehaviour);
    m_cHE_ShowAddCondition.push_back(bShowAddCondition);
    m_cHE_ShowAddFreeSelection.push_back(bShowAddFreeSelection);
  }

  BuildHierarchyPath();
  return true;
}

bool CHierarchyItem::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  assert(m_cHE_OpenedNames.size() == m_cHE_Names.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_Types.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowSubIFS.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowHierarchical.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowExpanded.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowBehaviour.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddCondition.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddFreeSelection.size());

  char sKey[50];
  int nI = 0;

  // XMLDoc in <irgendwo>
  for (CxtstringVectorIterator it = m_cHE_Names.begin(); it != m_cHE_Names.end(); it++)
  {
    sprintf(sKey, HIERARCHYITEM_OPENEDNAME, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_OpenedNames[nI].c_str());
    sprintf(sKey, HIERARCHYITEM_NAME, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_Names[nI].c_str());
    sprintf(sKey, HIERARCHYITEM_TYPE, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_Types[nI]);
    sprintf(sKey, HIERARCHYITEM_SHOWSUBIFS, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_ShowSubIFS[nI]);
    sprintf(sKey, HIERARCHYITEM_SHOWHIERARCHICAL, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_ShowHierarchical[nI]);
    sprintf(sKey, HIERARCHYITEM_SHOWEXPANDED, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_ShowExpanded[nI]);
    sprintf(sKey, HIERARCHYITEM_SHOWBEHAVIOUR, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_ShowBehaviour[nI]);
    sprintf(sKey, HIERARCHYITEM_SHOWADDCONDITION, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_ShowAddCondition[nI]);
    sprintf(sKey, HIERARCHYITEM_SHOWADDFREESELECTION, nI);
      pXMLDoc->AddAttrib(sKey, m_cHE_ShowAddFreeSelection[nI]);
    nI++;
  }
  return (DeepOfHierarchy() != 0);
}

xtstring CHierarchyItem::GetOpenedNameOfLevel(size_t nLevel) const
{
  assert(nLevel < m_cHE_OpenedNames.size());
  if (nLevel >= m_cHE_OpenedNames.size())
    return _XT("");
  return m_cHE_OpenedNames[nLevel];
}

xtstring CHierarchyItem::GetNameOfLevel(size_t nLevel) const
{
  assert(nLevel < m_cHE_Names.size());
  if (nLevel >= m_cHE_Names.size())
    return _XT("");
  return m_cHE_Names[nLevel];
}

THierarchyItemElement CHierarchyItem::GetTypeOfLevel(size_t nLevel) const
{
  assert(nLevel < m_cHE_Types.size());
  if (nLevel >= m_cHE_Types.size())
    return tHIE_Unknown;
  return m_cHE_Types[nLevel];
}

bool CHierarchyItem::GetShowSubIFS(size_t nLevel) const
{
  assert(nLevel < m_cHE_ShowSubIFS.size());
  if (nLevel >= m_cHE_ShowSubIFS.size())
    return true;
  return m_cHE_ShowSubIFS[nLevel];
}

bool CHierarchyItem::GetShowHierarchical(size_t nLevel) const
{
  assert(nLevel < m_cHE_ShowHierarchical.size());
  if (nLevel >= m_cHE_ShowHierarchical.size())
    return true;
  return m_cHE_ShowHierarchical[nLevel];
}

bool CHierarchyItem::GetShowExpanded(size_t nLevel) const
{
  assert(nLevel < m_cHE_ShowExpanded.size());
  if (nLevel >= m_cHE_ShowExpanded.size())
    return false;
  return m_cHE_ShowExpanded[nLevel];
}

TShowBehaviour CHierarchyItem::GetShowBehaviour(size_t nLevel) const
{
  assert(nLevel < m_cHE_ShowBehaviour.size());
  if (nLevel >= m_cHE_ShowBehaviour.size())
    return tShowBehaviour_Default;
  return m_cHE_ShowBehaviour[nLevel];
}

bool CHierarchyItem::GetShowAddCondition(size_t nLevel) const
{
  assert(nLevel < m_cHE_ShowAddCondition.size());
  if (nLevel >= m_cHE_ShowAddCondition.size())
    return false;
  return m_cHE_ShowAddCondition[nLevel];
}

bool CHierarchyItem::GetShowAddFreeSelection(size_t nLevel) const
{
  assert(nLevel < m_cHE_ShowAddFreeSelection.size());
  if (nLevel >= m_cHE_ShowAddFreeSelection.size())
    return false;
  return m_cHE_ShowAddFreeSelection[nLevel];
}

xtstring CHierarchyItem::GetLastOpenedName() const
{
  if (m_cHE_OpenedNames.size() == 0)
    return _XT("");

  return GetOpenedNameOfLevel(m_cHE_OpenedNames.size() - 1);
}

xtstring CHierarchyItem::GetLastName() const
{
  if (m_cHE_Names.size() == 0)
    return _XT("");

  return GetNameOfLevel(m_cHE_Names.size() - 1);
}

THierarchyItemElement CHierarchyItem::GetLastType() const
{
  if (m_cHE_Types.size() == 0)
    return tHIE_Unknown;

  return GetTypeOfLevel(m_cHE_Types.size() - 1);
}

bool CHierarchyItem::GetLastShowSubIFS() const
{
  if (m_cHE_ShowSubIFS.size() == 0)
    return true;

  return GetShowSubIFS(m_cHE_ShowSubIFS.size() - 1);
}

bool CHierarchyItem::GetLastShowHierarchical() const
{
  if (m_cHE_ShowHierarchical.size() == 0)
    return true;

  return GetShowHierarchical(m_cHE_ShowHierarchical.size() - 1);
}

bool CHierarchyItem::GetLastShowExpanded() const
{
  if (m_cHE_ShowExpanded.size() == 0)
    return false;

  return GetShowExpanded(m_cHE_ShowExpanded.size() - 1);
}

TShowBehaviour CHierarchyItem::GetLastShowBehaviour() const
{
  if (m_cHE_ShowBehaviour.size() == 0)
    return tShowBehaviour_Default;

  return GetShowBehaviour(m_cHE_ShowBehaviour.size() - 1);
}

bool CHierarchyItem::GetLastShowAddCondition() const
{
  if (m_cHE_ShowAddCondition.size() == 0)
    return false;

  return GetShowAddCondition(m_cHE_ShowAddCondition.size() - 1);
}

bool CHierarchyItem::GetLastShowAddFreeSelection() const
{
  if (m_cHE_ShowAddFreeSelection.size() == 0)
    return false;

  return GetShowAddFreeSelection(m_cHE_ShowAddFreeSelection.size() - 1);
}

bool CHierarchyItem::AssignFrom(const CHierarchyItem* pItem)
{
  if (!pItem)
    return false;

  m_cHE_OpenedNames.erase(m_cHE_OpenedNames.begin(), m_cHE_OpenedNames.end());
  m_cHE_Names.erase(m_cHE_Names.begin(), m_cHE_Names.end());
  m_cHE_Types.erase(m_cHE_Types.begin(), m_cHE_Types.end());
  m_cHE_ShowSubIFS.erase(m_cHE_ShowSubIFS.begin(), m_cHE_ShowSubIFS.end());
  m_cHE_ShowHierarchical.erase(m_cHE_ShowHierarchical.begin(), m_cHE_ShowHierarchical.end());
  m_cHE_ShowExpanded.erase(m_cHE_ShowExpanded.begin(), m_cHE_ShowExpanded.end());
  m_cHE_ShowBehaviour.erase(m_cHE_ShowBehaviour.begin(), m_cHE_ShowBehaviour.end());
  m_cHE_ShowAddCondition.erase(m_cHE_ShowAddCondition.begin(), m_cHE_ShowAddCondition.end());
  m_cHE_ShowAddFreeSelection.erase(m_cHE_ShowAddFreeSelection.begin(), m_cHE_ShowAddFreeSelection.end());

  for (size_t nI = 0; nI < pItem->DeepOfHierarchy(); nI++)
    AddHierarchyElement(pItem->GetOpenedNameOfLevel(nI),
                        pItem->GetNameOfLevel(nI),
                        pItem->GetTypeOfLevel(nI),
                        pItem->GetShowSubIFS(nI),
                        pItem->GetShowHierarchical(nI),
                        pItem->GetShowExpanded(nI),
                        pItem->GetShowBehaviour(nI),
                        pItem->GetShowAddCondition(nI),
                        pItem->GetShowAddFreeSelection(nI));

  return true;
}

void CHierarchyItem::AddHierarchyElement(const xtstring& sOpenedName,
                                         const xtstring& sName,
                                         THierarchyItemElement nType,
                                         bool bShowSubIFS,
                                         bool bShowHierarchical,
                                         bool bShowExpanded,
                                         TShowBehaviour nShowBehaviour,
                                         bool bShowAddCondition,
                                         bool bShowAddFreeSelection)
{
  assert(m_cHE_OpenedNames.size() == m_cHE_Names.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_Types.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowSubIFS.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowHierarchical.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowExpanded.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowBehaviour.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddCondition.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddFreeSelection.size());

  m_cHE_OpenedNames.push_back(sOpenedName);
  m_cHE_Names.push_back(sName);
  m_cHE_Types.push_back(nType);
  m_cHE_ShowSubIFS.push_back(bShowSubIFS);
  m_cHE_ShowHierarchical.push_back(bShowHierarchical);
  m_cHE_ShowExpanded.push_back(bShowExpanded);
  m_cHE_ShowBehaviour.push_back(nShowBehaviour);
  m_cHE_ShowAddCondition.push_back(bShowAddCondition);
  m_cHE_ShowAddFreeSelection.push_back(bShowAddFreeSelection);

  BuildHierarchyPath();
}

void CHierarchyItem::AddHierarchyElement_InformationSight(const xtstring& sOpenedName,
                                                          const xtstring& sName,
                                                          bool bShowSubIFS,
                                                          bool bShowHierarchical,
                                                          bool bShowExpanded,
                                                          TShowBehaviour nShowBehaviour,
                                                          bool bShowAddCondition,
                                                          bool bShowAddFreeSelection)
{
  assert(m_cHE_OpenedNames.size() == m_cHE_Names.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_Types.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowSubIFS.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowHierarchical.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowExpanded.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowBehaviour.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddCondition.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddFreeSelection.size());

  m_cHE_OpenedNames.push_back(sOpenedName);
  m_cHE_Names.push_back(sName);
  m_cHE_Types.push_back(tHIE_InformationSight);
  m_cHE_ShowSubIFS.push_back(bShowSubIFS);
  m_cHE_ShowHierarchical.push_back(bShowHierarchical);
  m_cHE_ShowExpanded.push_back(bShowExpanded);
  m_cHE_ShowBehaviour.push_back(nShowBehaviour);
  m_cHE_ShowAddCondition.push_back(bShowAddCondition);
  m_cHE_ShowAddFreeSelection.push_back(bShowAddFreeSelection);

  BuildHierarchyPath();
}

void CHierarchyItem::AddHierarchyElement_OpenedEntity(const xtstring& sOpenedName,
                                                      const xtstring& sName,
                                                      bool bShowExpanded,
                                                      TShowBehaviour nShowBehaviour,
                                                      bool bShowAddCondition,
                                                      bool bShowAddFreeSelection)
{
  assert(m_cHE_OpenedNames.size() == m_cHE_Names.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_Types.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowSubIFS.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowHierarchical.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowExpanded.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowBehaviour.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddCondition.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddFreeSelection.size());

  m_cHE_OpenedNames.push_back(sOpenedName);
  m_cHE_Names.push_back(sName);
  m_cHE_Types.push_back(tHIE_OpenedEntity);
  m_cHE_ShowSubIFS.push_back(GetLastShowSubIFS());
  m_cHE_ShowHierarchical.push_back(GetLastShowHierarchical());
  m_cHE_ShowExpanded.push_back(bShowExpanded);
  m_cHE_ShowBehaviour.push_back(nShowBehaviour);
  m_cHE_ShowAddCondition.push_back(bShowAddCondition);
  m_cHE_ShowAddFreeSelection.push_back(bShowAddFreeSelection);

  BuildHierarchyPath();
}

void CHierarchyItem::AddHierarchyElement_EntityForSubIFS(const xtstring& sOpenedName,
                                                         const xtstring& sName,
                                                         bool bShowExpanded,
                                                         TShowBehaviour nShowBehaviour,
                                                         bool bShowAddCondition,
                                                         bool bShowAddFreeSelection)
{
  assert(m_cHE_OpenedNames.size() == m_cHE_Names.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_Types.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowSubIFS.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowHierarchical.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowExpanded.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowBehaviour.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddCondition.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddFreeSelection.size());

  m_cHE_OpenedNames.push_back(sOpenedName);
  m_cHE_Names.push_back(sName);
  m_cHE_Types.push_back(tHIE_EntityForSubIFS);
  m_cHE_ShowSubIFS.push_back(GetLastShowSubIFS());
  m_cHE_ShowHierarchical.push_back(GetLastShowHierarchical());
  m_cHE_ShowExpanded.push_back(bShowExpanded);
  m_cHE_ShowBehaviour.push_back(nShowBehaviour);
  m_cHE_ShowAddCondition.push_back(bShowAddCondition);
  m_cHE_ShowAddFreeSelection.push_back(bShowAddFreeSelection);

  BuildHierarchyPath();
}

void CHierarchyItem::AddHierarchyElement_OpenedEntityForSubIFS(const xtstring& sOpenedName,
                                                               const xtstring& sName,
                                                               bool bShowExpanded,
                                                               TShowBehaviour nShowBehaviour,
                                                               bool bShowAddCondition,
                                                               bool bShowAddFreeSelection)
{
  assert(m_cHE_OpenedNames.size() == m_cHE_Names.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_Types.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowSubIFS.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowHierarchical.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowExpanded.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowBehaviour.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddCondition.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddFreeSelection.size());

  m_cHE_OpenedNames.push_back(sOpenedName);
  m_cHE_Names.push_back(sName);
  m_cHE_Types.push_back(tHIE_OpenedEntityForSubIFS);
  m_cHE_ShowSubIFS.push_back(GetLastShowSubIFS());
  m_cHE_ShowHierarchical.push_back(GetLastShowHierarchical());
  m_cHE_ShowExpanded.push_back(bShowExpanded);
  m_cHE_ShowBehaviour.push_back(nShowBehaviour);
  m_cHE_ShowAddCondition.push_back(bShowAddCondition);
  m_cHE_ShowAddFreeSelection.push_back(bShowAddFreeSelection);

  BuildHierarchyPath();
}

void CHierarchyItem::RemoveLastHierarchyElement()
{
  assert(m_cHE_OpenedNames.size() == m_cHE_Names.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_Types.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowSubIFS.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowHierarchical.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowExpanded.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowBehaviour.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddCondition.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddFreeSelection.size());

  if (m_cHE_Names.size() == 0)
    return;

  m_cHE_OpenedNames.erase(m_cHE_OpenedNames.end() - 1);
  m_cHE_Names.erase(m_cHE_Names.end() - 1);
  m_cHE_Types.erase(m_cHE_Types.end() - 1);
  m_cHE_ShowSubIFS.erase(m_cHE_ShowSubIFS.end() - 1);
  m_cHE_ShowHierarchical.erase(m_cHE_ShowHierarchical.end() - 1);
  m_cHE_ShowExpanded.erase(m_cHE_ShowExpanded.end() - 1);
  m_cHE_ShowBehaviour.erase(m_cHE_ShowBehaviour.end() - 1);
  m_cHE_ShowAddCondition.erase(m_cHE_ShowAddCondition.end() - 1);
  m_cHE_ShowAddFreeSelection.erase(m_cHE_ShowAddFreeSelection.end() - 1);

  BuildHierarchyPath();
}

void CHierarchyItem::BuildHierarchyPath()
{
  xtstring sPath;

  for (CxtstringConstVectorIterator it = m_cHE_Names.begin(); it != m_cHE_Names.end(); it++)
  {
    if (sPath.size())
      sPath += HIERARCHY_DELIMITER;
    sPath += (*it);
  }
  m_sHierarchyPath = sPath;
}

bool CHierarchyItem::ItemIsChild(const CHierarchyItem* pItem) const
{
  if (!pItem)
    return false;

  assert(m_cHE_OpenedNames.size() == m_cHE_Names.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_Types.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowSubIFS.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowHierarchical.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowExpanded.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowBehaviour.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddCondition.size());
  assert(m_cHE_OpenedNames.size() == m_cHE_ShowAddFreeSelection.size());

  assert(pItem->m_cHE_OpenedNames.size() == pItem->m_cHE_Names.size());
  assert(pItem->m_cHE_OpenedNames.size() == pItem->m_cHE_Types.size());
  assert(pItem->m_cHE_OpenedNames.size() == pItem->m_cHE_ShowSubIFS.size());
  assert(pItem->m_cHE_OpenedNames.size() == pItem->m_cHE_ShowHierarchical.size());
  assert(pItem->m_cHE_OpenedNames.size() == pItem->m_cHE_ShowExpanded.size());
  assert(pItem->m_cHE_OpenedNames.size() == pItem->m_cHE_ShowBehaviour.size());
  assert(pItem->m_cHE_OpenedNames.size() == pItem->m_cHE_ShowAddCondition.size());
  assert(pItem->m_cHE_OpenedNames.size() == pItem->m_cHE_ShowAddFreeSelection.size());


  // child muss mindestens um ein element mehr haben
  if (m_cHE_Names.size() >= pItem->m_cHE_Names.size())
    return false;

  // jetzt kontrollieren wir, ob 'm_cHierarchyElementNames.size()'
  // elemente identisch sind
  for (size_t nI = 0; nI < m_cHE_Names.size(); nI++)
  {
    if (m_cHE_Names[nI] != pItem->m_cHE_Names[nI])
      return false;
    if (m_cHE_Types[nI] != pItem->m_cHE_Types[nI])
      return false;
  }
  return true;
}

bool CHierarchyItem::ItemIsDirectChild(const CHierarchyItem* pItem,
                                       bool bIFSToo) const
{
  if (!ItemIsChild(pItem))
    return false;

  // wenn das direkt child ist, dann muss ab Position 'm_cHierarchyElementNames.size()'
  // erstes geoeffnetes Element gleichzeitig letztes sein
  for (size_t nI = m_cHE_Names.size(); nI < pItem->m_cHE_Names.size(); nI++)
  {
    if (pItem->m_cHE_Types[nI] == tHIE_OpenedEntity
          || pItem->m_cHE_Types[nI] == tHIE_OpenedEntityForSubIFS
          || (bIFSToo && pItem->m_cHE_Types[nI] == tHIE_InformationSight))
    {
      // es kann direct child sein
      if (nI + 1 == pItem->m_cHE_Names.size())
        return true;
      if (bIFSToo)
      {
        if (nI + 2 == pItem->m_cHE_Names.size()
            && pItem->m_cHE_Types[nI + 1] == tHIE_EntityForSubIFS)
          return true;
      }
    }
  }
  return false;
}

bool CHierarchyItem::ItemIsParent(const CHierarchyItem* pItem) const
{
  // wir drehen es um
  return pItem->ItemIsChild((const CHierarchyItem*)this);
}

bool CHierarchyItem::ItemIsDirectParent(const CHierarchyItem* pItem,
                                        bool bIFSToo) const
{
  // wir drehen es um
  return pItem->ItemIsDirectChild(this, bIFSToo);
}




















////////////////////////////////////////////////////////////////////////////////
// COpenedItem
////////////////////////////////////////////////////////////////////////////////
COpenedItem::COpenedItem()
            :m_cConditions((CSyncIFSNotifiersGroup*)0),
             m_cParameters((CSyncIFSNotifiersGroup*)0),
             m_cSQLCommands((CSyncIFSNotifiersGroup*)0)
{
}

COpenedItem::~COpenedItem()
{
}

bool COpenedItem::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_cConditions.ReadXMLContent(pXMLDoc);
  m_cParameters.ReadXMLContent(pXMLDoc);
  m_cSQLCommands.ReadXMLContent(pXMLDoc);
  return CHierarchyItem::ReadXMLDoc(pXMLDoc);
}

bool COpenedItem::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_cConditions.SaveXMLContent(pXMLDoc);
  m_cParameters.SaveXMLContent(pXMLDoc);
  m_cSQLCommands.SaveXMLContent(pXMLDoc);
  return CHierarchyItem::SaveXMLDoc(pXMLDoc);
}

bool COpenedItem::AssignFrom(const CHierarchyItem* pItem)
{
  return CHierarchyItem::AssignFrom(pItem);
}

bool COpenedItem::AssignFrom(const COpenedItem* pItem)
{
  bool bRet = m_cConditions.AssignFrom(pItem->GetConstConditions());
  bRet &= m_cParameters.AssignFrom(pItem->GetConstParameters());
  bRet &= m_cSQLCommands.AssignFrom(pItem->GetConstSQLCommands());
  return CHierarchyItem::AssignFrom(pItem);
}

bool COpenedItem::AssignFrom(const CConditionItem* pItem)
{
  return CHierarchyItem::AssignFrom(pItem);
}

bool COpenedItem::AssignConditionsParametersSQLCommandsFrom(const COpenedItem* pItem)
{
  if (!pItem)
    return false;
  return SetConditionsParametersSQLCommands(pItem->GetConstConditions(),
                                            pItem->GetConstParameters(),
                                            pItem->GetConstSQLCommands());
}

bool COpenedItem::IsIdenticalForXPubClient(const COpenedItem* pItem)
{
  assert(pItem);
  if (!pItem)
    return false;

  if (GetHierarchyPath() != pItem->GetHierarchyPath())
    return false;

  bool bRet = m_cConditions.IsIdenticalForXPubClient(pItem->GetConstConditions());
  if (bRet)
    bRet = m_cParameters.IsIdenticalForXPubClient(pItem->GetConstParameters());
  if (bRet)
    bRet = m_cSQLCommands.IsIdenticalForXPubClient(pItem->GetConstSQLCommands());
  return bRet;
}

size_t COpenedItem::GetIndexOfItem(const CIFSEntityOneConditionVariation* pItem) const
{
  return m_cConditions.GetIndexOfItem(pItem);
}

size_t COpenedItem::GetIndexOfItem(const CIFSEntityOneSQLCommandVariation* pItem) const
{
  return m_cSQLCommands.GetIndexOfItem(pItem);
}

bool COpenedItem::SetConditionsParametersSQLCommands(const CIFSEntityConditionVariations* pConditions,
                                                     const CIFSEntityAllParameters* pParameters,
                                                     const CIFSEntitySQLCommandVariations* pSQLCommands)
{
  bool bRet = m_cConditions.AssignFrom(pConditions);
  if (bRet)
    bRet = m_cParameters.AssignFrom(pParameters);
  if (bRet)
    bRet = m_cSQLCommands.AssignFrom(pSQLCommands);
  return bRet;
}














////////////////////////////////////////////////////////////////////////////////
// CConditionItem
////////////////////////////////////////////////////////////////////////////////
CConditionItem::CConditionItem()
               :m_caCondition(CConditionOrAssignment::tCOAT_HierarchyItem_Condition, false)
{
  m_nConditionType = tHIAC_Unknown;
//  m_nIndex = ;
//  m_sName = ;
//  m_sExpression = ;
//  m_sExprValues = ;
//  m_sExprSymbols = ;
  m_caCondition.Reset();
}

CConditionItem::~CConditionItem()
{
}

bool CConditionItem::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  // XMLDoc in <irgendwo>

  bool bRet = CHierarchyItem::ReadXMLDoc(pXMLDoc);
  if (!bRet)
    return bRet;

  m_nConditionType = (THierarchyItemActiveCondition)pXMLDoc->GetAttribLong(HIERARCHYITEM_COND_TYPE);
  if (m_nConditionType == tHIAC_Index
          || m_nConditionType == tHIAC_IndexAndCondition)
    m_nIndex = pXMLDoc->GetAttribLong(HIERARCHYITEM_COND_INDEX);
  if (m_nConditionType == tHIAC_Name
          || m_nConditionType == tHIAC_NameAndCondition)
    m_sName = pXMLDoc->GetAttrib(HIERARCHYITEM_COND_NAME);
  if (m_nConditionType == tHIAC_NewCondition
          || m_nConditionType == tHIAC_IndexAndCondition
          || m_nConditionType == tHIAC_NameAndCondition)
  {
    m_caCondition.ReadXMLContent(pXMLDoc);
    m_bDontOptimizeCondition = pXMLDoc->GetAttribLong(HIERARCHYITEM_COND_DONTOPTIMIZECONDITION) != 0;
  }

  return true;
}

bool CConditionItem::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  // XMLDoc in <irgendwo>

  bool bRet = CHierarchyItem::SaveXMLDoc(pXMLDoc);
  if (!bRet)
    return bRet;

  pXMLDoc->AddAttrib(HIERARCHYITEM_COND_TYPE, m_nConditionType);
  if (m_nConditionType == tHIAC_Index
          || m_nConditionType == tHIAC_IndexAndCondition)
    pXMLDoc->AddAttrib(HIERARCHYITEM_COND_INDEX, (int)m_nIndex);
  if (m_nConditionType == tHIAC_Name
          || m_nConditionType == tHIAC_NameAndCondition)
    pXMLDoc->AddAttrib(HIERARCHYITEM_COND_NAME, m_sName.c_str());
  if (m_nConditionType == tHIAC_NewCondition
          || m_nConditionType == tHIAC_IndexAndCondition
          || m_nConditionType == tHIAC_NameAndCondition)
  {
    m_caCondition.SaveXMLContent(pXMLDoc);
    pXMLDoc->AddAttrib(HIERARCHYITEM_COND_DONTOPTIMIZECONDITION, m_bDontOptimizeCondition);
  }

  return true;
}

bool CConditionItem::AssignFrom(const CHierarchyItem* pItem)
{
  m_nConditionType = tHIAC_Unknown;
  return CHierarchyItem::AssignFrom(pItem);
}

bool CConditionItem::AssignFrom(const CConditionItem* pItem)
{
  bool bRet = CHierarchyItem::AssignFrom(pItem);
  if (!bRet)
    return bRet;

  if (!pItem)
    return false;

  m_nConditionType = pItem->m_nConditionType;
  m_nIndex = pItem->m_nIndex;
  m_sName = pItem->m_sName;
  m_caCondition = pItem->m_caCondition;
  m_bDontOptimizeCondition = pItem->m_bDontOptimizeCondition;

  return true;
}

bool CConditionItem::AssignFrom(const COpenedItem* pItem)
{
  m_nConditionType = tHIAC_Unknown;
  return CHierarchyItem::AssignFrom(pItem);
}







////////////////////////////////////////////////////////////////////////////////
// CShowBehaviourItem
////////////////////////////////////////////////////////////////////////////////
CShowBehaviourItem::CShowBehaviourItem()
{
  m_nShowBehaviour = tShowBehaviour_Default;
}

CShowBehaviourItem::~CShowBehaviourItem()
{
}

bool CShowBehaviourItem::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_nShowBehaviour = (TShowBehaviour)pXMLDoc->GetAttribLong(HIERARCHYITEM_SB_SHOWBEHAVIOUR);
  if (m_nShowBehaviour <= tShowBehaviour_FirstDummy || m_nShowBehaviour > tShowBehaviour_LastDummy)
    m_nShowBehaviour = tShowBehaviour_Default;

  return CHierarchyItem::ReadXMLDoc(pXMLDoc);;
}

bool CShowBehaviourItem::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  pXMLDoc->AddAttrib(HIERARCHYITEM_SB_SHOWBEHAVIOUR, m_nShowBehaviour);
  return CHierarchyItem::SaveXMLDoc(pXMLDoc);
}

bool CShowBehaviourItem::AssignFrom(const CHierarchyItem* pItem)
{
  m_nShowBehaviour = tShowBehaviour_Default;
  return CHierarchyItem::AssignFrom(pItem);
}

bool CShowBehaviourItem::AssignFrom(const COpenedItem* pItem)
{
  m_nShowBehaviour = tShowBehaviour_Default;
  return CHierarchyItem::AssignFrom(pItem);
}

bool CShowBehaviourItem::AssignFrom(const CConditionItem* pItem)
{
  m_nShowBehaviour = tShowBehaviour_Default;
  return CHierarchyItem::AssignFrom(pItem);
}

bool CShowBehaviourItem::AssignFrom(const CShowBehaviourItem* pItem)
{
  m_nShowBehaviour = pItem->m_nShowBehaviour;
  return CHierarchyItem::AssignFrom(pItem);
}







////////////////////////////////////////////////////////////////////////////////
// CParametersItem
////////////////////////////////////////////////////////////////////////////////
CParametersItem::CParametersItem()
                :m_cParameters((CSyncIFSNotifiersGroup*)0)
{
  m_cParameters.RemoveAllParameters();
}
CParametersItem::~CParametersItem()
{
}
bool CParametersItem::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_cParameters.ReadXMLContent(pXMLDoc);
  return CHierarchyItem::ReadXMLDoc(pXMLDoc);
}
bool CParametersItem::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_cParameters.SaveXMLContent(pXMLDoc);
  return CHierarchyItem::SaveXMLDoc(pXMLDoc);
}
bool CParametersItem::AssignFrom(const CHierarchyItem* pItem)
{
  m_cParameters.RemoveAllParameters();
  return CHierarchyItem::AssignFrom(pItem);
}
bool CParametersItem::AssignFrom(const COpenedItem* pItem)
{
  m_cParameters.AssignFrom(pItem->GetConstParameters());
  return CHierarchyItem::AssignFrom(pItem);
}
bool CParametersItem::AssignFrom(const CConditionItem* pItem)
{
  m_cParameters.RemoveAllParameters();
  return CHierarchyItem::AssignFrom(pItem);
}
bool CParametersItem::AssignFrom(const CParametersItem* pItem)
{
  m_cParameters.AssignFrom(pItem->GetParameters());
  return CHierarchyItem::AssignFrom(pItem);
}







////////////////////////////////////////////////////////////////////////////////
// CFreeSelectionItem
////////////////////////////////////////////////////////////////////////////////
CFreeSelectionItem::CFreeSelectionItem()
                   :m_cSQLCommand(0, 0)
{
}
CFreeSelectionItem::~CFreeSelectionItem()
{
}
bool CFreeSelectionItem::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_cSQLCommand.ReadXMLContent(pXMLDoc);
  return CHierarchyItem::ReadXMLDoc(pXMLDoc);
}
bool CFreeSelectionItem::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_cSQLCommand.SaveXMLContent(pXMLDoc);
  return CHierarchyItem::SaveXMLDoc(pXMLDoc);
}
bool CFreeSelectionItem::AssignFrom(const CHierarchyItem* pItem)
{
  return CHierarchyItem::AssignFrom(pItem);
}
bool CFreeSelectionItem::AssignFrom(const COpenedItem* pItem)
{
  return CHierarchyItem::AssignFrom(pItem);
}
bool CFreeSelectionItem::AssignFrom(const CConditionItem* pItem)
{
  return CHierarchyItem::AssignFrom(pItem);
}
bool CFreeSelectionItem::AssignFrom(const CFreeSelectionItem* pItem)
{
  m_cSQLCommand.AssignFrom(pItem->GetSQLCommand());
  return CHierarchyItem::AssignFrom(pItem);
}






////////////////////////////////////////////////////////////////////////////////
// CShowExpandedItem
////////////////////////////////////////////////////////////////////////////////
CShowExpandedItem::CShowExpandedItem()
{
  m_bShowExpanded = true;
}

CShowExpandedItem::~CShowExpandedItem()
{
}

bool CShowExpandedItem::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  m_bShowExpanded = pXMLDoc->GetAttribLong(HIERARCHYITEM_SHOWEXP_SHOWEXPANDED) != 0;
  return CHierarchyItem::ReadXMLDoc(pXMLDoc);;
}

bool CShowExpandedItem::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  pXMLDoc->AddAttrib(HIERARCHYITEM_SHOWEXP_SHOWEXPANDED, m_bShowExpanded);
  return CHierarchyItem::SaveXMLDoc(pXMLDoc);
}

bool CShowExpandedItem::AssignFrom(const CHierarchyItem* pItem)
{
  m_bShowExpanded = true;
  return CHierarchyItem::AssignFrom(pItem);
}

bool CShowExpandedItem::AssignFrom(const COpenedItem* pItem)
{
  m_bShowExpanded = true;
  return CHierarchyItem::AssignFrom(pItem);
}

bool CShowExpandedItem::AssignFrom(const CConditionItem* pItem)
{
  m_bShowExpanded = true;
  return CHierarchyItem::AssignFrom(pItem);
}

bool CShowExpandedItem::AssignFrom(const CShowExpandedItem* pItem)
{
  m_bShowExpanded = pItem->m_bShowExpanded;
  return CHierarchyItem::AssignFrom(pItem);
}







////////////////////////////////////////////////////////////////////////////////
// COpenedItemsContainer
////////////////////////////////////////////////////////////////////////////////
COpenedItemsContainer::COpenedItemsContainer()
{
}

COpenedItemsContainer::~COpenedItemsContainer()
{
  ResetContent();
}

#define EMPTY_OPENED_ITEMS_XML_DOCUMENT _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<OpenedItems></OpenedItems>\r\n")
#define MAIN_OPENED_ITEMS               _XT("OpenedItems")
#define OPENED_ITEM                     _XT("OpenedItem")

xtstring COpenedItemsContainer::GetXMLDoc()
{
  bool bRet = true;
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(EMPTY_OPENED_ITEMS_XML_DOCUMENT);

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_OPENED_ITEMS))
    bRet = SaveXMLDoc(&cXMLDoc);
  else
    bRet = false;

  if (!bRet)
  {
    assert(false);
    return _XT("");
  }
  xtstring sXMLDocText = cXMLDoc.GetDoc();
  return sXMLDocText;
}

bool COpenedItemsContainer::SetXMLDoc(const xtstring& sXMLDoc)
{
  ResetContent();

  bool bRet = true;
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXMLDoc.c_str()))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_OPENED_ITEMS)
    bRet = ReadXMLDoc(&cXMLDoc);
  else
    bRet = false;

  if (!bRet)
  {
    assert(false);
    return false;
  }
  return true;
}

bool COpenedItemsContainer::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  bool bRet = true;

  // XMLDoc in <irgendwo>
  while (pXMLDoc->FindChildElem(OPENED_ITEM))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <OpenedItem>
    COpenedItem* pNew;
    pNew = new COpenedItem;
    if (!pNew)
    {
      bRet = false;
      break;
    }
    if (!pNew->ReadXMLDoc(pXMLDoc))
    {
      bRet = false;
      delete pNew;
      break;
    }
    m_Items.push_back(pNew);

    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  return bRet;
}

bool COpenedItemsContainer::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  bool bRet = true;

  // XMLDoc in <irgendwo>
  for (COpenedItemsIterator it = m_Items.begin(); it != m_Items.end(); it++)
  {
    if (pXMLDoc->AddChildElem(OPENED_ITEM, _XT("")))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <OpenedItem>
      if (!(*it)->SaveXMLDoc(pXMLDoc))
      {
        bRet = false;
        break;
      }
      pXMLDoc->OutOfElem();
      // XMLDoc in <irgendwo>
    }
    else
    {
      bRet = false;
      break;
    }
  }
  return bRet;
}

void COpenedItemsContainer::ResetContent()
{
  for (COpenedItemsIterator it = m_Items.begin(); it != m_Items.end(); it++)
    delete (*it);
  m_Items.erase(m_Items.begin(), m_Items.end());
}

bool COpenedItemsContainer::InsertActualLevel(CIFSEntityConditionVariations* pConditions,
                                              CIFSEntityAllParameters* pParameters,
                                              CIFSEntitySQLCommandVariations* pSQLCommands)
{
  COpenedItem* pNew;
  pNew = new COpenedItem;
  if (!pNew)
    return false;

  pNew->AssignFrom(&m_cStack);
  pNew->SetConditionsParametersSQLCommands(pConditions, pParameters, pSQLCommands);

  size_t nCount = m_Items.size();
  m_Items.push_back(pNew);
  bool bRet = (nCount != m_Items.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

bool COpenedItemsContainer::InsertItem(const CHierarchyItem* pItem)
{
  assert(pItem);
  if (!pItem)
    return false;

  COpenedItem* pNew;
  pNew = GetItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
    // existiert schon
    return true;

  // existiert noch nicht -> erzeugen
  pNew = new COpenedItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }

  size_t nCount = m_Items.size();
  m_Items.push_back(pNew);
  bool bRet = (nCount != m_Items.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

#define ORIGINAL_SUFFIX   _XT("o_r_i_g_i_n_a_l")
bool COpenedItemsContainer::InsertItem_Original(const CHierarchyItem* pItem_NotOrig)
{
  assert(pItem_NotOrig);
  if (!pItem_NotOrig)
    return false;

  COpenedItem cNewOriginal;
  if (!cNewOriginal.AssignFrom(pItem_NotOrig))
    return false;
  cNewOriginal.AddHierarchyElement_EntityForSubIFS(ORIGINAL_SUFFIX, ORIGINAL_SUFFIX, true, tShowBehaviour_NotLockable, true, false);
  return InsertItem(&cNewOriginal);
}
const COpenedItem* COpenedItemsContainer::GetItemFromPath_Original(const xtstring& sPath_NotOrig)
{
  xtstring sPath = sPath_NotOrig;
  sPath += HIERARCHY_DELIMITER;
  sPath += ORIGINAL_SUFFIX;
  for (COpenedItemsConstIterator it = m_Items.begin(); it != m_Items.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}
COpenedItem* COpenedItemsContainer::GetItemNotConstFromPath_Original(const xtstring& sPath_NotOrig)
{
  xtstring sPath = sPath_NotOrig;
  sPath += HIERARCHY_DELIMITER;
  sPath += ORIGINAL_SUFFIX;
  for (COpenedItemsConstIterator it = m_Items.begin(); it != m_Items.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}
const COpenedItem* COpenedItemsContainer::GetItemForEntity(const xtstring& sEntity)
{
  return GetItemNotConstForEntity(sEntity);
}
COpenedItem* COpenedItemsContainer::GetItemNotConstForEntity(const xtstring& sEntity)
{
  for (COpenedItemsIterator it = m_Items.begin(); it != m_Items.end(); it++)
  {
    COpenedItem* pItem = (*it);
    if (pItem->GetLastName() == sEntity)
      return pItem;
  }
  return 0;
}


size_t COpenedItemsContainer::GetIndexOfItem(const COpenedItem* pItem) const
{
  size_t nIndex = 0;
  for (COpenedItemsConstIterator it = m_Items.begin(); it != m_Items.end(); it++, nIndex++)
    if ((*it) == pItem)
      return nIndex;
  return (size_t)-1;
}

const COpenedItem* COpenedItemsContainer::GetItem(size_t nIndex)
{
  if (nIndex >= m_Items.size())
    return 0;

  return m_Items[nIndex];
}

const COpenedItem* COpenedItemsContainer::GetItemFromPath(const xtstring& sPath)
{
  return GetItemFromPathNotConst(sPath);
}

COpenedItem* COpenedItemsContainer::GetItemFromPathNotConst(const xtstring& sPath)
{
  for (COpenedItemsConstIterator it = m_Items.begin(); it != m_Items.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

COpenedItem* COpenedItemsContainer::GetItemNotConstFromPath(const xtstring& sPath)
{
  for (COpenedItemsConstIterator it = m_Items.begin(); it != m_Items.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

const COpenedItemsConstIterator COpenedItemsContainer::GetNextItemFromList(const COpenedItem* pItem)
{
  assert(pItem);
  if (!pItem)
    // fuer so ein element koennen wir kein naechstem suchen
    return m_Items.end();

  COpenedItemsConstIterator it;
  // wir suchen element in Liste
  for (it = m_Items.begin(); it != m_Items.end(); it++)
    if (pItem->GetHierarchyPath() == (*it)->GetHierarchyPath())
      break;
  // gefunden?
  if (it == m_Items.end())
    // nicht gefunden -> kein next
    return m_Items.end();

  // gefunden
  // naechstes element zurueckgeben
  it++;

  return it;
}

const COpenedItem* COpenedItemsContainer::GetFirstItem()
{
  if (m_Items.size())
    return (*m_Items.begin());
  return 0;
}

const COpenedItem* COpenedItemsContainer::GetNextSiblingItem(const COpenedItem* pItem)
{
  COpenedItemsConstIterator it;

  // wir holen naechstem Item in liste
  it = GetNextItemFromList(pItem);

  // ist ueberhaupt naechstes element?
  if (it == m_Items.end())
    // nicht mehr was vorhanden -> kein sibling
    return 0;

  // next sibling item suchen
  for (; it != m_Items.end(); it++)
  {
    if ((*it)->DeepOfHierarchy() < pItem->DeepOfHierarchy())
      // wenn wir einem item finden, der weniger elemente hat,
      // dann ist es naechstes block und wir muessen suche beenden
      return 0;

    // next sibling item muss gleiche tiefe haben ...
    if ((*it)->DeepOfHierarchy() > pItem->DeepOfHierarchy())
      continue;

    // ... mit einzigem unterschied und zwar an letzter stelle
    // wenn tiefe 1 ist, koennen wir gleich beenden
    if ((*it)->DeepOfHierarchy() == 1)
      return (*it);
    // tiefe ist mehr als 1 -> size() - 1 Stellen kontrollieren
    for (size_t nI = 0; nI < (*it)->DeepOfHierarchy() - 1; nI++)
    {
      if ((*it)->GetOpenedNameOfLevel(nI) != pItem->GetOpenedNameOfLevel(nI)
          || (*it)->GetNameOfLevel(nI) != pItem->GetNameOfLevel(nI)
          || (*it)->GetTypeOfLevel(nI) != pItem->GetTypeOfLevel(nI))
        // das ist kein sibling
        return 0;
    }
    // wenn wir da sind (*it) ist next sibling
    return (*it);
  }
  return 0;
}
/*
size_t COpenedItemsContainer::GetCountOfChilds(const COpenedItem* pItem)
{
  COpenedItemsConstIterator it;

  // wir holen naechstem Item in liste
  it = GetNextItemFromList(pItem);

  // ist ueberhaupt naechstes element?
  if (it == m_Items.end())
    // nicht mehr was vorhanden -> keine childs
    return 0;

  size_t nCount = 0;
  // rest der liste durchsuchen
  for (; it != m_Items.end(); it++)
  {
    if ((*it)->DeepOfHierarchy() < pItem->DeepOfHierarchy())
      // wenn wir einem item finden, der weniger elementen hat,
      // dann ist es naechstes block und wir muessen suche beenden
      return nCount;
    if ((*it)->DeepOfHierarchy() == pItem->DeepOfHierarchy())
      // wenn wir einem item finden, der gleiche anzahl den elementen hat,
      // dann ist es naechstes block und wir muessen suche beenden
      return nCount;
    if (pItem->ItemIsDirectChild(*it))
      nCount++;
  }

  return nCount;
}
*/
const COpenedItem* COpenedItemsContainer::GetFirstChildItem(const COpenedItem* pItem,
                                                            bool bIFSToo)
{
  COpenedItemsConstIterator it;

  // wir holen naechstem Item in liste
  it = GetNextItemFromList(pItem);

  // ist ueberhaupt naechstes element?
  if (it == m_Items.end())
    // nicht mehr was vorhanden -> keine childs
    return 0;

  // wenn wir erstem child suchen, es muss schon in 'it' sein
  if (pItem->ItemIsDirectChild(*it, bIFSToo))
    return (*it);

  // naechstes item ist was ganz anderes
  return 0;
}









////////////////////////////////////////////////////////////////////////////////
// CSettingsOfOpenedItemsContainer
////////////////////////////////////////////////////////////////////////////////
CSettingsOfOpenedItemsContainer::CSettingsOfOpenedItemsContainer()
{
}

CSettingsOfOpenedItemsContainer::~CSettingsOfOpenedItemsContainer()
{
  ResetContent();
}

#define EMPTY_CONDITIONLOCK_ITEMS_XML_DOCUMENT  _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<ConditionLockItems></ConditionLockItems>\r\n")
#define MAIN_CONDITIONLOCK_ITEMS                _XT("ConditionLockItems")

#define CONDITION__ITEMS                        _XT("ConditionItems")
#define LOCK__ITEMS                             _XT("LockItems")
#define PARAMETERS__ITEMS                       _XT("ParametersItems")
#define FREESELECTIONS__ITEMS                   _XT("FreeSelectionsItems")
#define SHOWEXPANDED__ITEMS                     _XT("ShowExpandedItems")
#define CONDITION__ITEM                         _XT("ConditionItem")
#define LOCK__ITEM                              _XT("LockItem")
#define PARAMETERS__ITEM                        _XT("ParametersItem")
#define FREESELECTIONS__ITEM                    _XT("FreeSelectionsItem")
#define SHOWEXPANDED__ITEM                      _XT("ShowExpandedItem")

xtstring CSettingsOfOpenedItemsContainer::GetXMLDoc()
{
  bool bRet = true;
  CXPubMarkUp cXMLDoc;
  cXMLDoc.SetDoc(EMPTY_CONDITIONLOCK_ITEMS_XML_DOCUMENT);

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem(MAIN_CONDITIONLOCK_ITEMS))
    bRet = SaveXMLDoc(&cXMLDoc);
  else
    bRet = false;

  if (!bRet)
  {
    assert(false);
    return _XT("");
  }
  xtstring sXMLDocText = cXMLDoc.GetDoc();
  return sXMLDocText;
}

bool CSettingsOfOpenedItemsContainer::SetXMLDoc(const xtstring& sXMLDoc)
{
  bool bRet = true;
  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXMLDoc.c_str()))
    return false;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_CONDITIONLOCK_ITEMS)
    bRet = ReadXMLDoc(&cXMLDoc);
  else
    bRet = false;

  if (!bRet)
  {
    assert(false);
    return false;
  }
  return true;
}

bool CSettingsOfOpenedItemsContainer::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  bool bRet = true;

  // XMLDoc in <irgendwo>

  // CConditionItems
  if (pXMLDoc->FindChildElem(CONDITION__ITEMS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ConditionItems>
    while (pXMLDoc->FindChildElem(CONDITION__ITEM))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <ConditionItem>
      CConditionItem* pNew;
      pNew = new CConditionItem;
      if (!pNew)
      {
        bRet = false;
        break;
      }
      if (!pNew->ReadXMLDoc(pXMLDoc))
      {
        bRet = false;
        delete pNew;
        break;
      }
      m_CItems.push_back(pNew);

      pXMLDoc->OutOfElem();
      // XMLDoc in <ConditionItems>
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  // CShowBehaviourItems
  if (pXMLDoc->FindChildElem(LOCK__ITEMS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <LockItems>
    while (pXMLDoc->FindChildElem(LOCK__ITEM))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <LockItem>
      CShowBehaviourItem* pNew;
      pNew = new CShowBehaviourItem;
      if (!pNew)
      {
        bRet = false;
        break;
      }
      if (!pNew->ReadXMLDoc(pXMLDoc))
      {
        bRet = false;
        delete pNew;
        break;
      }
      m_LItems.push_back(pNew);

      pXMLDoc->OutOfElem();
      // XMLDoc in <LockItems>
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  // CParametersItems
  if (pXMLDoc->FindChildElem(PARAMETERS__ITEMS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ParametersItems>
    while (pXMLDoc->FindChildElem(PARAMETERS__ITEM))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <ParametersItem>
      CParametersItem* pNew;
      pNew = new CParametersItem;
      if (!pNew)
      {
        bRet = false;
        break;
      }
      if (!pNew->ReadXMLDoc(pXMLDoc))
      {
        bRet = false;
        delete pNew;
        break;
      }
      m_PItems.push_back(pNew);

      pXMLDoc->OutOfElem();
      // XMLDoc in <ParametersItems>
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  // CFreeSelectionItems
  if (pXMLDoc->FindChildElem(FREESELECTIONS__ITEMS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <FreeSelectionsItems>
    while (pXMLDoc->FindChildElem(FREESELECTIONS__ITEM))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <FreeSelectionsItem>
      CFreeSelectionItem* pNew;
      pNew = new CFreeSelectionItem;
      if (!pNew)
      {
        bRet = false;
        break;
      }
      if (!pNew->ReadXMLDoc(pXMLDoc))
      {
        bRet = false;
        delete pNew;
        break;
      }
      m_FItems.push_back(pNew);

      pXMLDoc->OutOfElem();
      // XMLDoc in <FreeSelectionsItems>
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  // CShowExpandedItems
  if (pXMLDoc->FindChildElem(SHOWEXPANDED__ITEMS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ShowExpandedItems>
    while (pXMLDoc->FindChildElem(SHOWEXPANDED__ITEM))
    {
      pXMLDoc->IntoElem();
      // XMLDoc in <ShowExpandedItem>
      CShowExpandedItem* pNew;
      pNew = new CShowExpandedItem;
      if (!pNew)
      {
        bRet = false;
        break;
      }
      if (!pNew->ReadXMLDoc(pXMLDoc))
      {
        bRet = false;
        delete pNew;
        break;
      }
      m_SEItems.push_back(pNew);

      pXMLDoc->OutOfElem();
      // XMLDoc in <ShowExpandedItems>
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  return bRet;
}

bool CSettingsOfOpenedItemsContainer::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  bool bRet = true;

  // XMLDoc in <irgendwo>

  // CConditionItems
  if (pXMLDoc->AddChildElem(CONDITION__ITEMS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ConditionItems>
    for (CConditionItemsIterator it = m_CItems.begin(); it != m_CItems.end(); it++)
    {
      if (pXMLDoc->AddChildElem(CONDITION__ITEM, _XT("")))
      {
        pXMLDoc->IntoElem();
        // XMLDoc in <ConditionItem>
        if (!(*it)->SaveXMLDoc(pXMLDoc))
        {
          bRet = false;
          break;
        }
        pXMLDoc->OutOfElem();
        // XMLDoc in <ConditionItems>
      }
      else
      {
        bRet = false;
        break;
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  // CShowBehaviourItems
  if (pXMLDoc->AddChildElem(LOCK__ITEMS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <LockItems>
    for (CShowBehaviourItemsIterator it = m_LItems.begin(); it != m_LItems.end(); it++)
    {
      if (pXMLDoc->AddChildElem(LOCK__ITEM, _XT("")))
      {
        pXMLDoc->IntoElem();
        // XMLDoc in <LockItem>
        if (!(*it)->SaveXMLDoc(pXMLDoc))
        {
          bRet = false;
          break;
        }
        pXMLDoc->OutOfElem();
        // XMLDoc in <LockItems>
      }
      else
      {
        bRet = false;
        break;
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  // CParametersItems
  if (pXMLDoc->AddChildElem(PARAMETERS__ITEMS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ParametersItems>
    for (CParametersItemsIterator it = m_PItems.begin(); it != m_PItems.end(); it++)
    {
      if (pXMLDoc->AddChildElem(PARAMETERS__ITEM, _XT("")))
      {
        pXMLDoc->IntoElem();
        // XMLDoc in <ParametersItem>
        if (!(*it)->SaveXMLDoc(pXMLDoc))
        {
          bRet = false;
          break;
        }
        pXMLDoc->OutOfElem();
        // XMLDoc in <ParametersItems>
      }
      else
      {
        bRet = false;
        break;
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  // CFreeSelectionItems
  if (pXMLDoc->AddChildElem(FREESELECTIONS__ITEMS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <FreeSelectionsItems>
    for (CFreeSelectionItemsIterator it = m_FItems.begin(); it != m_FItems.end(); it++)
    {
      if (pXMLDoc->AddChildElem(FREESELECTIONS__ITEM, _XT("")))
      {
        pXMLDoc->IntoElem();
        // XMLDoc in <FreeSelectionsItem>
        if (!(*it)->SaveXMLDoc(pXMLDoc))
        {
          bRet = false;
          break;
        }
        pXMLDoc->OutOfElem();
        // XMLDoc in <FreeSelectionsItems>
      }
      else
      {
        bRet = false;
        break;
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  // CShowExpandedItems
  if (pXMLDoc->AddChildElem(SHOWEXPANDED__ITEMS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ShowExpandedItems>
    for (CShowExpandedItemsIterator it = m_SEItems.begin(); it != m_SEItems.end(); it++)
    {
      if (pXMLDoc->AddChildElem(SHOWEXPANDED__ITEM, _XT("")))
      {
        pXMLDoc->IntoElem();
        // XMLDoc in <ShowExpandedItem>
        if (!(*it)->SaveXMLDoc(pXMLDoc))
        {
          bRet = false;
          break;
        }
        pXMLDoc->OutOfElem();
        // XMLDoc in <ShowExpandedItems>
      }
      else
      {
        bRet = false;
        break;
      }
    }
    pXMLDoc->OutOfElem();
    // XMLDoc in <irgendwo>
  }
  return bRet;
}

void CSettingsOfOpenedItemsContainer::ResetContent()
{
  for (CConditionItemsIterator it = m_CItems.begin(); it != m_CItems.end(); it++)
    delete (*it);
  m_CItems.erase(m_CItems.begin(), m_CItems.end());

  for (CShowBehaviourItemsIterator it = m_LItems.begin(); it != m_LItems.end(); it++)
    delete (*it);
  m_LItems.erase(m_LItems.begin(), m_LItems.end());

  for (CParametersItemsIterator it = m_PItems.begin(); it != m_PItems.end(); it++)
    delete (*it);
  m_PItems.erase(m_PItems.begin(), m_PItems.end());

  for (CFreeSelectionItemsIterator it = m_FItems.begin(); it != m_FItems.end(); it++)
    delete (*it);
  m_FItems.erase(m_FItems.begin(), m_FItems.end());

  for (CShowExpandedItemsIterator it = m_SEItems.begin(); it != m_SEItems.end(); it++)
    delete (*it);
  m_SEItems.erase(m_SEItems.begin(), m_SEItems.end());
}

bool CSettingsOfOpenedItemsContainer::AddConditionsAndLocks(CSettingsOfOpenedItemsContainer* pItem)
{
#ifdef _DEBUG
  assert(pItem);
#endif // _DEBUG
  if (!pItem)
    return true;

  for (size_t nI = 0; nI < pItem->CountOfConditionItems(); nI++)
  {
    const CConditionItem* pC = pItem->GetConditionItem(nI);
    if (!pC)
      continue;
    switch (pC->GetConditionType())
    {
    case tHIAC_Index:
      SetActiveConditionForItem(pC,
                                pC->GetConditionIndex());
      break;
    case tHIAC_IndexAndCondition:
      SetActiveConditionForItem(pC,
                                pC->GetConditionIndex(),
                                pC->GetCondition_Const(),
                                pC->GetDontOptimizeCondition());
      break;
    case tHIAC_Name:
      SetActiveConditionForItem(pC,
                                pC->GetConditionName());
      break;
    case tHIAC_NameAndCondition:
      SetActiveConditionForItem(pC,
                                pC->GetConditionName(),
                                pC->GetCondition_Const(),
                                pC->GetDontOptimizeCondition());
      break;
    case tHIAC_NewCondition:
      SetActiveConditionForItem(pC,
                                pC->GetCondition_Const(),
                                pC->GetDontOptimizeCondition());
      break;
    default:
#ifdef _DEBUG
      assert(false);
#endif // _DEBUG
      break;
    }
  }
  for (size_t nI = 0; nI < pItem->CountOfLockItems(); nI++)
  {
    const CShowBehaviourItem* pL = pItem->GetShowBehaviourItem(nI);
    if (!pL)
      continue;
    SetShowBehaviourForItem(pL, pL->GetShowBehaviour());
  }
  return true;
}

const CConditionItem* CSettingsOfOpenedItemsContainer::GetConditionItem(size_t nIndex)
{
  if (nIndex >= m_CItems.size())
    return 0;

  return m_CItems[nIndex];
}

const CConditionItem* CSettingsOfOpenedItemsContainer::GetConditionItemFromPath(const xtstring& sPath)
{
  for (CConditionItemsConstIterator it = m_CItems.begin(); it != m_CItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

CConditionItem* CSettingsOfOpenedItemsContainer::GetConditionItemNotConstFromPath(const xtstring& sPath)
{
  for (CConditionItemsConstIterator it = m_CItems.begin(); it != m_CItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

CShowBehaviourItem* CSettingsOfOpenedItemsContainer::GetShowBehaviourItemNotConstFromPath(const xtstring& sPath)
{
  for (CShowBehaviourItemsConstIterator it = m_LItems.begin(); it != m_LItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

CParametersItem* CSettingsOfOpenedItemsContainer::GetParametersItemNotConstFromPath(const xtstring& sPath)
{
  for (CParametersItemsConstIterator it = m_PItems.begin(); it != m_PItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

CFreeSelectionItem* CSettingsOfOpenedItemsContainer::GetFreeSelectionItemNotConstFromPath(const xtstring& sPath)
{
  for (CFreeSelectionItemsConstIterator it = m_FItems.begin(); it != m_FItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

CShowExpandedItem* CSettingsOfOpenedItemsContainer::GetShowExpandedItemNotConstFromPath(const xtstring& sPath)
{
  for (CShowExpandedItemsConstIterator it = m_SEItems.begin(); it != m_SEItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

bool CSettingsOfOpenedItemsContainer::SetActiveConditionForItem(const CHierarchyItem* pItem,
                                                                size_t nConditionIndex)
{
  assert(pItem);
  if (!pItem)
    return false;

  CConditionItem* pNew;
  pNew = GetConditionItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetConditionType(tHIAC_Index);
    pNew->SetConditionIndex(nConditionIndex);
    return true;
  }

  pNew = new CConditionItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetConditionType(tHIAC_Index);
  pNew->SetConditionIndex(nConditionIndex);

  size_t nCount = m_CItems.size();
  m_CItems.push_back(pNew);
  bool bRet = (nCount != m_CItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

bool CSettingsOfOpenedItemsContainer::SetActiveConditionForItem(const CHierarchyItem* pItem,
                                                                size_t nConditionIndex,
                                                                const CConditionOrAssignment& caCondition,
                                                                bool bDontOptimizeCondition)
{
  assert(pItem);
  if (!pItem)
    return false;

  CConditionItem* pNew;
  pNew = GetConditionItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetConditionType(tHIAC_IndexAndCondition);
    pNew->SetConditionIndex(nConditionIndex);
    pNew->GetCondition() = caCondition;
    pNew->SetDontOptimizeCondition(bDontOptimizeCondition);
    return true;
  }

  pNew = new CConditionItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetConditionType(tHIAC_IndexAndCondition);
  pNew->SetConditionIndex(nConditionIndex);
  pNew->GetCondition() = caCondition;
  pNew->SetDontOptimizeCondition(bDontOptimizeCondition);

  size_t nCount = m_CItems.size();
  m_CItems.push_back(pNew);
  bool bRet = (nCount != m_CItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

bool CSettingsOfOpenedItemsContainer::SetActiveConditionForItem(const CHierarchyItem* pItem,
                                                                const xtstring& sConditionName)
{
  assert(pItem);
  if (!pItem)
    return false;

  CConditionItem* pNew;
  pNew = GetConditionItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetConditionType(tHIAC_Name);
    pNew->SetConditionName(sConditionName);
    return true;
  }

  pNew = new CConditionItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetConditionType(tHIAC_Name);
  pNew->SetConditionName(sConditionName);

  size_t nCount = m_CItems.size();
  m_CItems.push_back(pNew);
  bool bRet = (nCount != m_CItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

bool CSettingsOfOpenedItemsContainer::SetActiveConditionForItem(const CHierarchyItem* pItem,
                                                                const xtstring& sConditionName,
                                                                const CConditionOrAssignment& caCondition,
                                                                bool bDontOptimizeCondition)
{
  assert(pItem);
  if (!pItem)
    return false;

  CConditionItem* pNew;
  pNew = GetConditionItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetConditionType(tHIAC_NameAndCondition);
    pNew->SetConditionName(sConditionName);
    pNew->GetCondition() = caCondition;
    pNew->SetDontOptimizeCondition(bDontOptimizeCondition);
    return true;
  }

  pNew = new CConditionItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetConditionType(tHIAC_NameAndCondition);
  pNew->SetConditionName(sConditionName);
  pNew->GetCondition() = caCondition;
  pNew->SetDontOptimizeCondition(bDontOptimizeCondition);

  size_t nCount = m_CItems.size();
  m_CItems.push_back(pNew);
  bool bRet = (nCount != m_CItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

bool CSettingsOfOpenedItemsContainer::SetActiveConditionForItem(const CHierarchyItem* pItem,
                                                                const CConditionOrAssignment& caCondition,
                                                                bool bDontOptimizeCondition)
{
  assert(pItem);
  if (!pItem)
    return false;

  CConditionItem* pNew;
  pNew = GetConditionItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetConditionType(tHIAC_NewCondition);
    pNew->GetCondition() = caCondition;
    pNew->SetDontOptimizeCondition(bDontOptimizeCondition);
    return true;
  }

  pNew = new CConditionItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetConditionType(tHIAC_NewCondition);
  pNew->GetCondition() = caCondition;
  pNew->SetDontOptimizeCondition(bDontOptimizeCondition);

  size_t nCount = m_CItems.size();
  m_CItems.push_back(pNew);
  bool bRet = (nCount != m_CItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

const CShowBehaviourItem* CSettingsOfOpenedItemsContainer::GetShowBehaviourItem(size_t nIndex)
{
  if (nIndex >= m_LItems.size())
    return 0;

  return m_LItems[nIndex];
}

const CShowBehaviourItem* CSettingsOfOpenedItemsContainer::GetShowBehaviourItemFromPath(const xtstring& sPath)
{
  for (CShowBehaviourItemsConstIterator it = m_LItems.begin(); it != m_LItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

bool CSettingsOfOpenedItemsContainer::SetShowBehaviourForItem(const CHierarchyItem* pItem,
                                                              TShowBehaviour nShowBehaviour)
{
  assert(pItem);
  if (!pItem)
    return false;

  CShowBehaviourItem* pNew;
  pNew = GetShowBehaviourItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetShowBehaviour(nShowBehaviour);
    return true;
  }

  pNew = new CShowBehaviourItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetShowBehaviour(nShowBehaviour);

  size_t nCount = m_LItems.size();
  m_LItems.push_back(pNew);
  bool bRet = (nCount != m_LItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

const CParametersItem* CSettingsOfOpenedItemsContainer::GetParametersItem(size_t nIndex)
{
  if (nIndex >= m_PItems.size())
    return 0;

  return m_PItems[nIndex];
}

const CParametersItem* CSettingsOfOpenedItemsContainer::GetParametersItemFromPath(const xtstring& sPath)
{
  for (CParametersItemsConstIterator it = m_PItems.begin(); it != m_PItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

bool CSettingsOfOpenedItemsContainer::SetParametersForItem(const CHierarchyItem* pItem,
                                                           const CIFSEntityAllParameters* pParameters)
{
  assert(pItem);
  if (!pItem)
    return false;

  CParametersItem* pNew;
  pNew = GetParametersItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetParameters(pParameters);
    return true;
  }

  pNew = new CParametersItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetParameters(pParameters);

  size_t nCount = m_PItems.size();
  m_PItems.push_back(pNew);
  bool bRet = (nCount != m_PItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

const CFreeSelectionItem* CSettingsOfOpenedItemsContainer::GetFreeSelectionItem(size_t nIndex)
{
  if (nIndex >= m_FItems.size())
    return 0;

  return m_FItems[nIndex];
}

const CFreeSelectionItem* CSettingsOfOpenedItemsContainer::GetFreeSelectionItemFromPath(const xtstring& sPath)
{
  for (CFreeSelectionItemsConstIterator it = m_FItems.begin(); it != m_FItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

bool CSettingsOfOpenedItemsContainer::SetActiveFreeSelectionItem(const CHierarchyItem* pItem,
                                                                 const CIFSEntityOneSQLCommandVariation* pSQLCommand)
{
  assert(pItem);
  if (!pItem)
    return false;

  CFreeSelectionItem* pNew;
  pNew = GetFreeSelectionItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetSQLCommand(pSQLCommand);
    return true;
  }

  pNew = new CFreeSelectionItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetSQLCommand(pSQLCommand);

  size_t nCount = m_FItems.size();
  m_FItems.push_back(pNew);
  bool bRet = (nCount != m_FItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}

const CShowExpandedItem* CSettingsOfOpenedItemsContainer::GetShowExpandedItem(size_t nIndex)
{
  if (nIndex >= m_SEItems.size())
    return 0;

  return m_SEItems[nIndex];
}

const CShowExpandedItem* CSettingsOfOpenedItemsContainer::GetShowExpandedItemFromPath(const xtstring& sPath)
{
  for (CShowExpandedItemsConstIterator it = m_SEItems.begin(); it != m_SEItems.end(); it++)
    if ((*it)->GetHierarchyPath() == sPath)
      return (*it);
  return 0;
}

bool CSettingsOfOpenedItemsContainer::SetShowExpandedForItem(const CHierarchyItem* pItem,
                                                             bool bShowExpanded)
{
  assert(pItem);
  if (!pItem)
    return false;

  CShowExpandedItem* pNew;
  pNew = GetShowExpandedItemNotConstFromPath(pItem->GetHierarchyPath());
  if (pNew)
  {
    pNew->SetShowExpanded(bShowExpanded);
    return true;
  }

  pNew = new CShowExpandedItem;
  if (!pNew)
    return false;

  if (!pNew->AssignFrom(pItem))
  {
    delete pNew;
    return false;
  }
  pNew->SetShowExpanded(bShowExpanded);

  size_t nCount = m_SEItems.size();
  m_SEItems.push_back(pNew);
  bool bRet = (nCount != m_SEItems.size());
  if (!bRet)
    delete pNew;
  return bRet;
}
