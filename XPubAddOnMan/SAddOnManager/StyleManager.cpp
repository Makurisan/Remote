#if defined(WIN32)
#include "Shlwapi.h"
#endif // WIN32

#include <assert.h>

#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"

#include "StyleManager.h"





///////////////////////////////////////////////////////////////////////////////
// CXPubClassProperty
////////////////////////////////////////////////////////////////////////////////
CXPubClassProperty::CXPubClassProperty()
{
}
CXPubClassProperty::CXPubClassProperty(const CXPubClassProperty& cProp)
{
  CopyFrom(&cProp);
}
CXPubClassProperty::CXPubClassProperty(const CXPubClassProperty* pProp)
{
  CopyFrom(pProp);
}
CXPubClassProperty::~CXPubClassProperty()
{
}
void CXPubClassProperty::operator = (const CXPubClassProperty& cProp)
{
  CopyFrom(&cProp);
}
void CXPubClassProperty::CopyFrom(const CXPubClassProperty* pProp)
{
  if (!pProp)
    return;

  m_sName = pProp->m_sName;
  m_nType = pProp->m_nType;
  m_sValue = pProp->m_sValue;
}





///////////////////////////////////////////////////////////////////////////////
// CXPubTabStop
////////////////////////////////////////////////////////////////////////////////
CXPubTabStop::CXPubTabStop()
{
  m_nTabFlag = 0;
  m_nTabPos = 0;
  m_sTabDecChar = _T(".");
  m_sTabLeader = _T("");
}

CXPubTabStop::CXPubTabStop(const CXPubTabStop& cTabStop)
{
  CopyFrom(&cTabStop);
}

CXPubTabStop::CXPubTabStop(const CXPubTabStop* pTabStop)
{
  CopyFrom(pTabStop);
}

CXPubTabStop::~CXPubTabStop()
{
}

void CXPubTabStop::operator = (const CXPubTabStop& cTabStop)
{
  CopyFrom(&cTabStop);
}

void CXPubTabStop::CopyFrom(const CXPubTabStop* pTabStop)
{
  if (!pTabStop)
    return;

  m_nTabFlag = pTabStop->m_nTabFlag;
  m_nTabPos = pTabStop->m_nTabPos;
  m_sTabDecChar = pTabStop->m_sTabDecChar;
  m_sTabLeader = pTabStop->m_sTabLeader;
}












///////////////////////////////////////////////////////////////////////////////
// CXPubParagraphStyle
////////////////////////////////////////////////////////////////////////////////
CXPubParagraphStyle::CXPubParagraphStyle()
{
  SetDefaultValues(false);
}

CXPubParagraphStyle::CXPubParagraphStyle(const CXPubParagraphStyle& cStyle)
{
  CopyFrom(&cStyle);
}

CXPubParagraphStyle::~CXPubParagraphStyle()
{
  RemoveAllTabStops();
  RemoveAllClassProperties();
}

void CXPubParagraphStyle::operator = (const CXPubParagraphStyle& cStyle)
{
  CopyFrom(&cStyle);
}

void CXPubParagraphStyle::CopyFrom(const CXPubParagraphStyle* pStyle)
{
  if (!pStyle)
    return;

  m_sNextStyle = pStyle->m_sNextStyle;
  m_nChangedProperty = 0; // da machen wir nicht copy

  m_sFontName = pStyle->m_sFontName;
  m_nFontSize = pStyle->m_nFontSize;
  m_nFontColor = pStyle->m_nFontColor;
  m_nFontBackColor = pStyle->m_nFontBackColor;
  m_nCodePage = pStyle->m_nCodePage;
  m_bBold = pStyle->m_bBold;
  m_bItalic = pStyle->m_bItalic;
  m_bUnderline = pStyle->m_bUnderline;
  m_bDoubleUnderline = pStyle->m_bDoubleUnderline;
  m_bStrikeOut = pStyle->m_bStrikeOut;
  m_nLeft = pStyle->m_nLeft;
  m_nRight = pStyle->m_nRight;
  m_nIndent = pStyle->m_nIndent;
  m_nTextAlign = pStyle->m_nTextAlign;
  m_nKeep = pStyle->m_nKeep;
  m_nKeepNext = pStyle->m_nKeepNext;
  m_nWindowOrphan = pStyle->m_nWindowOrphan;
  m_nPageBreakBefore = pStyle->m_nPageBreakBefore;
  m_nSpaceBefore = pStyle->m_nSpaceBefore;
  m_nSpaceAfter = pStyle->m_nSpaceAfter;
  m_nSpaceBetween = pStyle->m_nSpaceBetween;
  m_nLineSpacing = pStyle->m_nLineSpacing;
  m_nTabDefaultPos = pStyle->m_nTabDefaultPos;
  m_nTabDefaultDecimalChar = pStyle->m_nTabDefaultDecimalChar;
  // tab stops kopieren
  RemoveAllTabStops();
  for (size_t nI = 0; nI < pStyle->CountOfTabStops(); nI++)
  {
    CXPubTabStop* pTabStop = pStyle->GetTabStop(nI);
    CXPubTabStop* pNewTabStop = new CXPubTabStop(pTabStop);
    m_cTabStops.push_back(pNewTabStop);
  }
  CopyClassProperties(pStyle);
}

bool CXPubParagraphStyle::IsDefaultStyle()
{
  if (m_sStyleName == DEFAULT_STYLE_NAME)
    return true;
  return false;
}

void CXPubParagraphStyle::SetDefaultValues(bool bWithDefaultName)
{
  if (bWithDefaultName)
    m_sStyleName = DEFAULT_STYLE_NAME;
  m_nStyleType = tST_None;
  m_nStyleID = -1;
  m_nChangedProperty = 0;
  m_bStyleIsPermanent = true;
  m_sNextStyle = _XT("");
  m_sFontName = _XT("Arial");
  m_nFontSize = 12;
  m_nFontColor = RGB(0,0,0);
  m_nFontBackColor = RGB(255,255,255);
  m_nCodePage = 1252;
  m_bBold = false;
  m_bItalic = false;
  m_bUnderline = false;
  m_bDoubleUnderline = false;
  m_bStrikeOut = false;
  m_nLeft = -1;
  m_nRight = -1;
  m_nIndent = -1;
  m_nTextAlign = 2; //TAG_TEXT_TEXTL
  m_nKeep = -1;
  m_nKeepNext = -1;
  m_nWindowOrphan = -1;
  m_nPageBreakBefore = 0;
  m_nSpaceBefore = 0;
  m_nSpaceAfter = 0;
  m_nSpaceBetween = 0;
  m_nLineSpacing = 0;
  m_nTabDefaultPos = 0;
  m_nTabDefaultDecimalChar = 0;
  RemoveAllTabStops();
  RemoveAllClassProperties();
}

void CXPubParagraphStyle::SetParagraphStyle(const CXPubParagraphStyle* pStyle)
{
  if (!pStyle)
    return;

  m_sStyleName = pStyle->m_sStyleName;
  m_nStyleType = pStyle->m_nStyleType;
  m_nStyleID = pStyle->m_nStyleID;

  m_nChangedProperty = pStyle->m_nChangedProperty;
  m_bStyleIsPermanent = pStyle->m_bStyleIsPermanent;

  m_sNextStyle = pStyle->m_sNextStyle;

  m_sFontName = pStyle->m_sFontName;
  m_nFontSize = pStyle->m_nFontSize;
  m_nFontColor = pStyle->m_nFontColor;
  m_nFontBackColor = pStyle->m_nFontBackColor;
  m_nCodePage = pStyle->m_nCodePage;
  m_bBold = pStyle->m_bBold;
  m_bItalic = pStyle->m_bItalic;
  m_bUnderline = pStyle->m_bUnderline;
  m_bDoubleUnderline = pStyle->m_bDoubleUnderline;
  m_bStrikeOut = pStyle->m_bStrikeOut;

  m_nLeft = pStyle->m_nLeft;
  m_nRight = pStyle->m_nRight;
  m_nIndent = pStyle->m_nIndent;
  m_nTextAlign = pStyle->m_nTextAlign;

  m_nKeep = pStyle->m_nKeep;
  m_nKeepNext = pStyle->m_nKeepNext;
  m_nWindowOrphan = pStyle->m_nWindowOrphan;
  m_nPageBreakBefore = pStyle->m_nPageBreakBefore;

  m_nSpaceBefore = pStyle->m_nSpaceBefore;
  m_nSpaceAfter = pStyle->m_nSpaceAfter;
  m_nSpaceBetween = pStyle->m_nSpaceBetween;
  m_nLineSpacing = pStyle->m_nLineSpacing;

  m_nTabDefaultPos = pStyle->m_nTabDefaultPos;
  m_nTabDefaultDecimalChar = pStyle->m_nTabDefaultDecimalChar;

  RemoveAllTabStops();
  for (size_t nI = 0; nI < pStyle->CountOfTabStops(); nI++)
  {
    CXPubTabStop* pTabStop = pStyle->GetTabStop(nI);
    CXPubTabStop* pNewTabStop = new CXPubTabStop(pTabStop);
    m_cTabStops.push_back(pNewTabStop);
  }
  CopyClassProperties(pStyle);
}

void CXPubParagraphStyle::SetCharacterStyle(const CXPubParagraphStyle* pStyle)
{
  if (!pStyle)
  {
    m_nChangedProperty = tCP_StyleElement;
    return;
  }

  m_sStyleName = pStyle->m_sStyleName;
  m_nStyleType = pStyle->m_nStyleType;
  m_nStyleID = pStyle->m_nStyleID;
  m_nChangedProperty = pStyle->m_nChangedProperty;
  m_bStyleIsPermanent = pStyle->m_bStyleIsPermanent;

  if (pStyle->GetChangedProperty() & tCP_FontName)
    m_sFontName = pStyle->m_sFontName;
  if (pStyle->GetChangedProperty() & tCP_FontSize)
    m_nFontSize = pStyle->m_nFontSize;
  if (pStyle->GetChangedProperty() & tCP_FontColor)
    m_nFontColor = pStyle->m_nFontColor;
  if (pStyle->GetChangedProperty() & tCP_FontBackColor)
    m_nFontBackColor = pStyle->m_nFontBackColor;
  if (pStyle->GetChangedProperty() & tCP_FontCodePage)
    m_nCodePage = pStyle->m_nCodePage;
  if (pStyle->GetChangedProperty() & tCP_Bold)
    m_bBold = pStyle->m_bBold;
  if (pStyle->GetChangedProperty() & tCP_Italic)
    m_bItalic = pStyle->m_bItalic;
  if (pStyle->GetChangedProperty() & tCP_Underline)
    m_bUnderline = pStyle->m_bUnderline;
  if (pStyle->GetChangedProperty() & tCP_DoubleUnderline)
    m_bDoubleUnderline = pStyle->m_bDoubleUnderline;
  if (pStyle->GetChangedProperty() & tCP_StrikeOut)
    m_bStrikeOut = pStyle->m_bStrikeOut;
  CopyClassProperties(pStyle);

  m_nChangedProperty = pStyle->m_nChangedProperty | tCP_StyleElement;
}

bool CXPubParagraphStyle::AddTabStop(const CXPubTabStop& cTabStop)
{
  CXPubTabStop* pNewTabStop = new CXPubTabStop();
  if (!pNewTabStop)
    return false;

  pNewTabStop->CopyFrom(&cTabStop);
  size_t nCount = m_cTabStops.size();
  m_cTabStops.push_back(pNewTabStop);
  if (nCount == m_cTabStops.size())
  {
    delete pNewTabStop;
    return false;
  }
  return true;
}

bool CXPubParagraphStyle::AddTabStop(int nTabFlag,
                                     int nTabPos,
                                     const xtstring& sTabDecChar,
                                     const xtstring& sTabLeader)
{
  CXPubTabStop cNewTabStop;
  cNewTabStop.SetTabFlag(nTabFlag);
  cNewTabStop.SetTabPos(nTabPos);
  cNewTabStop.SetTabDecChar(sTabDecChar);
  cNewTabStop.SetTabLeader(sTabLeader);
  return AddTabStop(cNewTabStop);
}

void CXPubParagraphStyle::RemoveAllTabStops()
{
  for (CTabStopsIterator it = m_cTabStops.begin(); it != m_cTabStops.end(); it++)
    delete *it;
  m_cTabStops.erase(m_cTabStops.begin(), m_cTabStops.end());
}

CXPubClassProperty* CXPubParagraphStyle::GetClassProperty(size_t nIndex) const
{
  CXPubClassProperty* pRetVal = 0;
  size_t nI = 0;
  for (CClassPropertiesConstIterator it = m_cClassProperties.begin(); it != m_cClassProperties.end(); it++)
  {
    if (nI == nIndex)
    {
      pRetVal = it->second;
      break;
    }
    nI++;
  }
  return pRetVal;
}
CXPubClassProperty* CXPubParagraphStyle::GetClassProperty(const xtstring& sName)
{
  CXPubClassProperty* pRetVal = 0;
  CClassPropertiesConstIterator it = m_cClassProperties.find(sName);
  if (it != m_cClassProperties.end())
  {
    pRetVal = it->second;
  }
  return pRetVal;
}
void CXPubParagraphStyle::CopyClassProperties(const CXPubParagraphStyle* pStyle)
{
  RemoveAllClassProperties();
  for (size_t nI = 0; nI < pStyle->CountOfClassProperties(); nI++)
  {
    CXPubClassProperty* pClass = pStyle->GetClassProperty(nI);
    CXPubClassProperty* pNewClass = new CXPubClassProperty(pClass);
    CClassPropertiesPair pair;
    pair.first = pNewClass->GetName();
    pair.second = pNewClass;
    m_cClassProperties.insert(pair);
  }
}
bool CXPubParagraphStyle::AddClassProperty(const CXPubClassProperty& cClassProperty)
{
  return AddClassProperty(&cClassProperty);
}
bool CXPubParagraphStyle::AddClassProperty(const CXPubClassProperty* pClassProperty)
{
  if (!pClassProperty)
    return false;
  RemoveClassProperty(pClassProperty->GetName());

  CXPubClassProperty* pNewClass = new CXPubClassProperty(pClassProperty);
  CClassPropertiesPair pair;
  pair.first = pNewClass->GetName();
  pair.second = pNewClass;
  m_cClassProperties.insert(pair);
  return true;
}
void CXPubParagraphStyle::RemoveClassProperty(size_t nIndex)
{
  size_t nI = 0;
  for (CClassPropertiesConstIterator it = m_cClassProperties.begin(); it != m_cClassProperties.end(); it++)
  {
    if (nI == nIndex)
    {
      RemoveClassProperty(it->second->GetName());
      break;
    }
    nI++;
  }
}
void CXPubParagraphStyle::RemoveClassProperty(const xtstring& sName)
{
  CXPubClassProperty* pVal = 0;
  CClassPropertiesIterator it = m_cClassProperties.find(sName);
  if (it != m_cClassProperties.end())
  {
    pVal = it->second;
    m_cClassProperties.erase(it);
  }
  if (pVal)
    delete pVal;
}
void CXPubParagraphStyle::RemoveAllClassProperties()
{
  CXPubClassProperty* pRetVal = 0;
  for (CClassPropertiesConstIterator it = m_cClassProperties.begin(); it != m_cClassProperties.end(); it++)
  {
    delete it->second;
  }
  m_cClassProperties.erase(m_cClassProperties.begin(), m_cClassProperties.end());
}







///////////////////////////////////////////////////////////////////////////////
// CFormatStackItem
////////////////////////////////////////////////////////////////////////////////
CFormatStackItem::CFormatStackItem()
{
  m_bItemIsFirstInSegment = false;
}

CFormatStackItem::CFormatStackItem(const CFormatStackItem& cItem)
{
  CopyFrom(&cItem);
}

CFormatStackItem::~CFormatStackItem()
{
}

void CFormatStackItem::operator = (const CFormatStackItem& cItem)
{
  CopyFrom(&cItem);
}

void CFormatStackItem::CopyFrom(const CFormatStackItem* pItem)
{
  if (!pItem)
    return;
  CXPubParagraphStyle::CopyFrom(pItem);
  m_bItemIsFirstInSegment = pItem->m_bItemIsFirstInSegment;
}




///////////////////////////////////////////////////////////////////////////////
// CFormatStack
////////////////////////////////////////////////////////////////////////////////
CFormatStack::CFormatStack()
{
  // wir muessen gleich einem item erzeugen
  CFormatStackItem cStartItem;
  cStartItem.SetDefaultValues(true);
  cStartItem.SetItemIsFirstInSegment(true);
  AddItem(cStartItem, -1);
}
CFormatStack::~CFormatStack()
{
  // alles aufraeumen
  RemoveAllItems();
}

CFormatStackItem* CFormatStack::GetActualFormatInStack()
{
  // es muss so sein, dass in m_cItems IMMER mindestens ein Element ist
  assert(m_cItems.size());
  if (!m_cItems.size())
    return 0;
  // wir liefern einfach den letzten element
  return *(m_cItems.rbegin());
}

bool CFormatStack::CreateNewItemInFormatStack(size_t nPos)
{
  // wir erzeugen neuem element als kopie von aktuellem element
  CFormatStackItem* pActual = 0;
  if (nPos == -1 || m_cItems.size() < nPos)
    pActual = GetActualFormatInStack();
  else
    pActual = m_cItems[nPos - 1];
  if (!pActual)
    return false;
  CFormatStackItem cNewItem;
  cNewItem.CopyFrom(pActual);
  cNewItem.SetItemIsFirstInSegment(false);
  bool bRet = AddItem(cNewItem, nPos);

  return bRet;
}

bool CFormatStack::RecopyProperties(size_t nPos)
{
  if (m_cItems.size() <= nPos)
    nPos = m_cItems.size() - 1;
  CFormatStackItem* pCopyFrom = m_cItems[nPos];
  for (size_t nI = nPos + 1; nI < m_cItems.size(); nI++)
  {
    CFormatStackItem* pCopyTo = m_cItems[nI];
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontName))
      pCopyTo->m_sFontName = pCopyFrom->m_sFontName;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontSize))
      pCopyTo->m_nFontSize = pCopyFrom->m_nFontSize;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontColor))
      pCopyTo->m_nFontColor = pCopyFrom->m_nFontColor;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontBackColor))
      pCopyTo->m_nFontBackColor = pCopyFrom->m_nFontBackColor;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontCodePage))
      pCopyTo->m_nCodePage = pCopyFrom->m_nCodePage;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_Bold))
      pCopyTo->m_bBold = pCopyFrom->m_bBold;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_Italic))
      pCopyTo->m_bItalic = pCopyFrom->m_bItalic;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_Underline))
      pCopyTo->m_bUnderline = pCopyFrom->m_bUnderline;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_DoubleUnderline))
      pCopyTo->m_bDoubleUnderline = pCopyFrom->m_bDoubleUnderline;
    if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_StrikeOut))
      pCopyTo->m_bStrikeOut = pCopyFrom->m_bStrikeOut;
    pCopyFrom = pCopyTo;
  }
  return true;
}

CXPubParagraphStyle* CFormatStack::GetItemInFormatStack(size_t nPos)
{
  if (m_cItems.size() <= nPos)
    nPos = m_cItems.size() - 1;
  return m_cItems[nPos];
}

CXPubParagraphStyle* CFormatStack::GetItemForRelease(CXPubParagraphStyle::TChangedProperty nChangedProperty)
{
  // Erstem element im segment koennen wir nicht entfernen
  if (m_cItems.size() == 1)
    return 0;

  size_t nIndex = m_cItems.size();

  // Jetzt suchen wir von hinten
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    nIndex--;

    CFormatStackItem* pItem = *it;
    if (pItem->GetItemIsFirstInSegment())
      // Wir sind bei erstem Element in Segment - diesen element nicht entfernen.
      // Wenn wir da sind, es bedeutet, wir haben nicht richtige 'Ende' gefunden.
      // Vielleicht deshalb, weil </bold> ohne <bold> benutzt war
      break;

    if (pItem->GetChangedProperty() & nChangedProperty)
    {
      // Wir haben erstem Element (von Hinten) gefunden, wo nChangedProperty gesetzt wurde
      if (nIndex == 0 || pItem->GetItemIsFirstInSegment())
      {
        // Das kann nicht vorkommen.
        // Start Element ist eine direkte Kopie vom Style
        break;
      }
      return pItem;
    }
  }
  return 0;
}

bool CFormatStack::ReleaseItemInFormatStack(CXPubParagraphStyle::TChangedProperty nChangedProperty,
                                            size_t& nPos)
{
  nPos = -1;
  // Wenn wir von oben im FormatStack fahren, erstem Element mit 'nChangedProperty' entfernen.
  // Benutzer kann fehler machen (mehr mals </font> benutzen)

  // Erstem element im segment koennen wir nicht entfernen
  if (m_cItems.size() == 1)
    return true;

  size_t nIndex = m_cItems.size();

  // Jetzt suchen wir von hinten
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    nIndex--;

    CFormatStackItem* pItem = *it;
    if (pItem->GetItemIsFirstInSegment())
      // Wir sind bei erstem Element in Segment - diesen element nicht entfernen.
      // Wenn wir da sind, es bedeutet, wir haben nicht richtige 'Ende' gefunden.
      // Vielleicht deshalb, weil </bold> ohne <bold> benutzt war
      break;

    if (pItem->GetChangedProperty() & nChangedProperty)
    {
      // Wir haben erstem Element (von Hinten) gefunden, wo nChangedProperty gesetzt wurde
      if (nIndex == 0 || pItem->GetItemIsFirstInSegment())
      {
        // Das kann nicht vorkommen.
        // Start Element ist eine direkte Kopie vom Style
        break;
      }

      nPos = nIndex;

      // Das ist die Stelle, die wir entfernen muessen
      CFormatStackItem* pCopyFrom = m_cItems[nIndex - 1];
      for (size_t nI = nIndex + 1; nI < m_cItems.size(); nI++)
      {
        CFormatStackItem* pCopyTo = m_cItems[nI];
        if (nChangedProperty & CXPubParagraphStyle::tCP_Bold)
          pCopyTo->m_bBold = pCopyFrom->m_bBold;
        if (nChangedProperty & CXPubParagraphStyle::tCP_Italic)
          pCopyTo->m_bItalic = pCopyFrom->m_bItalic;
        if (nChangedProperty & CXPubParagraphStyle::tCP_Underline)
          pCopyTo->m_bUnderline = pCopyFrom->m_bUnderline;
        if (nChangedProperty & CXPubParagraphStyle::tCP_DoubleUnderline)
          pCopyTo->m_bDoubleUnderline = pCopyFrom->m_bDoubleUnderline;
        if (nChangedProperty & CXPubParagraphStyle::tCP_StrikeOut)
          pCopyTo->m_bStrikeOut = pCopyFrom->m_bStrikeOut;
        if (nChangedProperty & CXPubParagraphStyle::tCP_FontName)
          pCopyTo->m_sFontName = pCopyFrom->m_sFontName;
        if (nChangedProperty & CXPubParagraphStyle::tCP_FontSize)
          pCopyTo->m_nFontSize = pCopyFrom->m_nFontSize;
        if (nChangedProperty & CXPubParagraphStyle::tCP_FontColor)
          pCopyTo->m_nFontColor = pCopyFrom->m_nFontColor;
        if (nChangedProperty & CXPubParagraphStyle::tCP_FontBackColor)
          pCopyTo->m_nFontBackColor = pCopyFrom->m_nFontBackColor;
        if (nChangedProperty & CXPubParagraphStyle::tCP_FontCodePage)
          pCopyTo->m_nCodePage = pCopyFrom->m_nCodePage;
        if (nChangedProperty & CXPubParagraphStyle::tCP_StyleElement)
        {
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontName))
            pCopyTo->m_sFontName = pCopyFrom->m_sFontName;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontSize))
            pCopyTo->m_nFontSize = pCopyFrom->m_nFontSize;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontColor))
            pCopyTo->m_nFontColor = pCopyFrom->m_nFontColor;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontBackColor))
            pCopyTo->m_nFontBackColor = pCopyFrom->m_nFontBackColor;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_FontCodePage))
            pCopyTo->m_nCodePage = pCopyFrom->m_nCodePage;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_Bold))
            pCopyTo->m_bBold = pCopyFrom->m_bBold;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_Italic))
            pCopyTo->m_bItalic = pCopyFrom->m_bItalic;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_Underline))
            pCopyTo->m_bUnderline = pCopyFrom->m_bUnderline;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_DoubleUnderline))
            pCopyTo->m_bDoubleUnderline = pCopyFrom->m_bDoubleUnderline;
          if (!(pCopyTo->GetChangedProperty() & CXPubParagraphStyle::tCP_StrikeOut))
            pCopyTo->m_bStrikeOut = pCopyFrom->m_bStrikeOut;
        }
        pCopyFrom = pCopyTo;
      }
      RemoveItem(nIndex);
      break;
    }
  }
  return true;
}

bool CFormatStack::CreateNewSegmentInFormatStack(CXPubParagraphStyle* pDefaultStyle)
{
  // wir erzeugen neuem element, aber nicht als kopie von aktuellem element
  // wir erzeugen element aufgrund default style
  CFormatStackItem cNewItem;
  cNewItem.CXPubParagraphStyle::CopyFrom(pDefaultStyle);
  cNewItem.SetItemIsFirstInSegment(true);
  return AddItem(cNewItem, -1);
}

bool CFormatStack::ReleaseSegmentInFormatStack()
{
  // alles bis "FirstItemInSegment" entfernen
  // vorsicht aber.
  // Falls "FirstItemInSegment" element 0 ist,
  // dann duerfen wir es nicht entfernen
  if (m_cItems.size() == 1)
    return true;
  // jetzt starten wir von hinten entfernen.
  for (;;)
  {
    CFormatStackItemsReverseIterator it = m_cItems.rbegin();
    CFormatStackItem* pItem = *it;
    if (pItem->GetItemIsFirstInSegment())
    {
      if (m_cItems.size() == 1)
        // nichts entfernen
        break;
      RemoveTopItem();
      break;
    }
    if (!RemoveTopItem())
      // was eigentlich jetzt ???
      // dort ist was schief gelaufen
      break;
  }
  return true;
}

size_t CFormatStack::GetCountOfSegmentsInStack()
{
  size_t nCount = 0;
  for (CFormatStackItemsIterator it = m_cItems.begin(); it != m_cItems.end(); it++)
  {
    if ((*it)->GetItemIsFirstInSegment())
      nCount++;
  }
  return nCount;
}

bool CFormatStack::SetPageBreakBefore(int nPageBreakBefore)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->SetPageBreakBefore(nPageBreakBefore);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::SetParagraphKeepNext(int nKeepNext)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->SetParagraphKeepNext(nKeepNext);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::SetParagraphKeep(int nKeep)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->SetParagraphKeep(nKeep);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::SetParagraphWindowOrphan(int nWindowOrphan)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->SetParagraphWindowOrphan(nWindowOrphan);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::SetParagraphIndents(int nLeft, int nRight, int nIndent)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->SetParagraphIndents(nLeft, nRight, nIndent);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::SetTextAlignment(int nAlignment)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->SetTextAlignment(nAlignment);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::SetParagraphSpacing(int nSpaceBefore, int nSpaceAfter, int nSpaceBetween, int nLineSpacing)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::SetTabDefaults(int nTabDefaultPos, int nTabDefaultDecimalChar)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->SetTabDefaults(nTabDefaultPos, nTabDefaultDecimalChar);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::AddTabStop(int nTabFlag, int nTabPos, const xtstring& sTabDecChar, const xtstring& sTabLeader)
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->AddTabStop(nTabFlag, nTabPos, sTabDecChar, sTabLeader);
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::ClearTabStops()
{
  for (CFormatStackItemsReverseIterator it = m_cItems.rbegin(); it != m_cItems.rend(); it++)
  {
    (*it)->RemoveAllTabStops();
    if ((*it)->GetItemIsFirstInSegment())
      break;
  }
  return true;
}

bool CFormatStack::DefaultStyleChanged(CXPubParagraphStyle* pDefaultStyle)
{
  CFormatStackItemsIterator it = m_cItems.begin();
  if (it == m_cItems.end())
  {
    // dazu kann nie kommen
    assert(false);
    return false;
  }
  (*it)->CXPubParagraphStyle::CopyFrom(pDefaultStyle);
  return true;
}

void CFormatStack::RemoveAllItems()
{
  // clean
  for (CFormatStackItemsIterator it = m_cItems.begin(); it != m_cItems.end(); it++)
    delete *it;
  m_cItems.erase(m_cItems.begin(), m_cItems.end());
}

bool CFormatStack::AddItem(const CFormatStackItem& cItem,
                           size_t nPos)
{
  CFormatStackItem* pNewItem = new CFormatStackItem(cItem);
  if (!pNewItem)
    return false;

  size_t nCount = m_cItems.size();

  if (nPos == -1 || m_cItems.size() < nPos)
    m_cItems.push_back(pNewItem);
  else
    m_cItems.insert(m_cItems.begin() + nPos, pNewItem);

  if (nCount == m_cItems.size())
  {
    delete pNewItem;
    return false;
  }
  return true;
}

bool CFormatStack::RemoveTopItem()
{
  // funktion muss letztem (top) element entfernen
  // bei aufruf diese funktion ist kontrolliert,
  // ob sich nicht um erstem element handelt.
  // wir kontrollieren es aber trotzdem auch da
  if (m_cItems.size() == 1)
    return false;
  CFormatStackItemsIterator it = m_cItems.end() - 1;
  delete *it;
  m_cItems.erase(it);
  return true;
}

bool CFormatStack::RemoveItem(size_t nIndex)
{
  // Da koennen wir fast keine Kontrolle machen
  // Nur eine, ob das Segment Element ist
  if (m_cItems.size() <= nIndex)
    // existiert nicht
    return false;

  CFormatStackItem* pItem = m_cItems[nIndex];
  if (pItem && pItem->GetItemIsFirstInSegment())
  {
    // so ein Element kann man nicht mit dieser funktion entfernen
    assert(false);
    return false;
  }
  if (pItem)
    delete pItem;
  m_cItems.erase(m_cItems.begin() + nIndex);
  return true;
}











///////////////////////////////////////////////////////////////////////////////
// CXPubStyleManager
////////////////////////////////////////////////////////////////////////////////
CXPubStyleManager::CXPubStyleManager()
{
  m_sSelectedStyleName.clear();
  m_pActualStyle = 0;
  m_pActualStyleToSet = 0;
  CXPubParagraphStyle* pNew = new CXPubParagraphStyle();
  if (pNew)
  {
    pNew->SetDefaultValues(true);
    CStylesPair pair;
    pair.first = pNew->GetStyleName();
    pair.second = pNew;
    m_cStyles.insert(pair);
    m_pActualStyle = pNew;
  }
}

CXPubStyleManager::~CXPubStyleManager()
{
  RemoveAllStyles();
}

void CXPubStyleManager::SetDefaultStyleAsActualStyle()
{
  m_pActualStyle = GetDefaultStyle();
}

void CXPubStyleManager::SetActualStyle(const xtstring& sStyleName)
{
  m_pActualStyle = GetStyle(sStyleName);
  if (!m_pActualStyle)
    SetDefaultStyleAsActualStyle();
  m_sSelectedStyleName = sStyleName;
  // Wir muessen jetzt im Stack ganzem Segment entfernen.
  size_t nCount = GetCountOfSegmentsInStack();
  ReleaseSegmentInFormatStack();
  if (nCount > 1)
    CreateNewSegmentInFormatStack();
  GetActualFormatInStack()->SetParagraphStyle(m_pActualStyle);
}

CXPubParagraphStyle* CXPubStyleManager::GetActualStyle()
{
  if (!m_pActualStyle)
    SetDefaultStyleAsActualStyle();
  return m_pActualStyle;
}

void CXPubStyleManager::SetDefaultStyleAsActualStyleToSet()
{
  m_pActualStyleToSet = GetDefaultStyle();
}

void CXPubStyleManager::SetActualStyleToSet(const xtstring& sStyleName)
{
  m_pActualStyleToSet = GetStyle(sStyleName);
  if (!m_pActualStyleToSet)
    SetDefaultStyleAsActualStyleToSet();
}

void CXPubStyleManager::SetNoActualStyleToSet()
{
  // Wir kontrollieren, ob nicht default style geaendert wurde.
  // Falls ja, aendern wir FormatStack
  if (m_pActualStyleToSet && m_pActualStyleToSet->IsDefaultStyle())
  {
    m_cFormatStack.DefaultStyleChanged(m_pActualStyleToSet);
  }
  m_pActualStyleToSet = 0;
}

CXPubParagraphStyle* CXPubStyleManager::GetActualStyleToSet()
{
  if (!m_pActualStyleToSet)
    SetDefaultStyleAsActualStyleToSet();
  return m_pActualStyleToSet;
}

CXPubParagraphStyle* CXPubStyleManager::CreateNewStyle(const xtstring& sStyleName,
                                                       const xtstring& sBasedOnStyle)
{
  CXPubParagraphStyle* pNewStyle = GetStyle(sStyleName);
  if (pNewStyle)
    return pNewStyle;
  pNewStyle = new CXPubParagraphStyle;
  pNewStyle->m_sStyleName = sStyleName;

  if (sBasedOnStyle.size() && GetStyle(sBasedOnStyle))
  {
    CXPubParagraphStyle* pCopyFrom = GetStyle(sBasedOnStyle);
    pNewStyle->CopyFrom(pCopyFrom);
    pNewStyle->m_nStyleID = -1;
  }
  else
  {
    pNewStyle->CopyFrom(GetDefaultStyle());
    pNewStyle->m_nStyleID = -1;
  }
  m_cStyles[sStyleName] = pNewStyle;
  return pNewStyle;
}

CXPubParagraphStyle* CXPubStyleManager::GetDefaultStyle()
{
  return GetStyle(DEFAULT_STYLE_NAME);
}

CXPubParagraphStyle* CXPubStyleManager::GetStyle(const xtstring& sStyleName)
{
  CXPubParagraphStyle* pStyle = 0;
  CStylesIterator it = m_cStyles.find(sStyleName);
  if (it == m_cStyles.end())
  {
    // vielleich Klein/Gross schreibung
    for (it = m_cStyles.begin(); it != m_cStyles.end(); it++)
    {
      if (it->second->m_sStyleName.CompareNoCase(sStyleName) == 0)
        break;
    }
  }
  if (it != m_cStyles.end())
    pStyle = it->second;
  return pStyle;
}

void CXPubStyleManager::RemoveStyle(const xtstring& sStyleName)
{
  CStylesIterator it = m_cStyles.find(sStyleName);
  if (it == m_cStyles.end())
  {
    // vielleich Klein/Gross schreibung
    for (it = m_cStyles.begin(); it != m_cStyles.end(); it++)
    {
      if (it->second->m_sStyleName.CompareNoCase(sStyleName) == 0)
        break;
    }
  }
  if (it == m_cStyles.end())
    return;
  if (it->second->IsDefaultStyle())
    return;
  delete it->second;
  m_cStyles.erase(it);
}

void CXPubStyleManager::RemoveAllStyles()
{
  for (CStylesIterator it = m_cStyles.begin(); it != m_cStyles.end(); it++)
    delete (it->second);
  m_cStyles.erase(m_cStyles.begin(), m_cStyles.end());
}



