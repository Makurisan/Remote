#if !defined(_STYLEMANAGER_H_)
#define _STYLEMANAGER_H_


#include "AddOnManagerExpImp.h"
#include "SErrors/ErrorNumbers.h"
#include "SOpSys/types.h"
#include "TagStructs.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32

typedef enum TColorType
{
  tColorEmpty = 0,
  tColorCMYK,
  tColorRGB,
};

class _ColorDef
{
public:

	_ColorDef::_ColorDef(double c, double m, double y, double k)
	{
		ct = tColorCMYK;
		cmyk.c = c;
		cmyk.m = m;
		cmyk.y = y;
		cmyk.k = k;
	};

	_ColorDef::_ColorDef(COLORDEF clrf)
	{
		ct = tColorEmpty;
		*this = clrf;
	};

	_ColorDef::_ColorDef()
	{
		ct = tColorEmpty;
		cmyk.c = cmyk.m = cmyk.y = cmyk.k = -1;
	};

  const _ColorDef& operator =(COLORDEF clrf)
	{
		if(clrf != -1)
		{
			ct = tColorRGB;
			rgb.r = GetRValue(clrf);
			rgb.g = GetGValue(clrf); 
			rgb.b = GetBValue(clrf);
		}
		return *this;
	}
	
  bool IsEmpty()
	{
		return ct == tColorEmpty;
	}

  void operator =(const _ColorDef& varSrc)
	{
		ct = varSrc.ct;
		cmyk.c = varSrc.cmyk.c;
		cmyk.m = varSrc.cmyk.m;
		cmyk.y = varSrc.cmyk.y;
		cmyk.k = varSrc.cmyk.k;
		sColor = varSrc.sColor;
		sDefColor = varSrc.sColor;
		sDefColor = varSrc.sDefColor;
	};

	operator COLORDEF()
	{
		if(ct == tColorRGB)
			return RGB(rgb.r, rgb.g, rgb.b);
	
		if(ct == tColorCMYK)
			return _cmyk2rgb(cmyk.c, cmyk.m, cmyk.y, cmyk.k);
		
		return (COLORDEF)-1;
	};

  TColorType GetType() const {return ct;};
	TColorType ct;

	typedef struct
	{
		double c;
		double m;
		double y;
		double k;
	}_cmyk;

	typedef struct
	{
		double r;
		double g;
		double b;
	}_rgb;

	union
	{
		_cmyk cmyk;
		_rgb rgb;
	};

	xtstring sColor;
	xtstring sDefColor;

protected:

	COLORDEF _cmyk2rgb(double C, double M, double Y, double K)
	{
		BYTE r, g, b;
		COLORDEF rgb;

		double R, G, B;

		C = C / 255.0;
		M = M / 255.0;
		Y = Y / 255.0;
		K = K / 255.0;

		R = C * (1.0 - K) + K;
		G = M * (1.0 - K) + K;
		B = Y * (1.0 - K) + K;

		R = (1.0 - R) * 255.0 + 0.5;
		G = (1.0 - G) * 255.0 + 0.5;
		B = (1.0 - B) * 255.0 + 0.5;

		r = (INT) R;
		g = (INT) G;
		b = (INT) B;

		rgb = RGB(r,g,b);

		return rgb;
	};

};

class CXPubAddOnManager;
class CConvertWorkerFrom;
class CConvertWorkerTo;



#define DEFAULT_STYLE_NAME   _XT("Normal")
class CXPubStyleManager;
class CXPubParagraphStyle;


typedef enum TStyleEvent
{
  tSE_StartPreProcess,            // Nach PreProcess aufgerufen, da kann man dem Default Style abholen
  tSE_StartWork,                  // Nach PreProcess aufgerufen, da kann man dem Default Style abholen
  tSE_FormatStackStart,           // Es war new style gesetzt
  tSE_FormatStackAdd,             // Es war in FormatStack was eingefuegt
  tSE_FormatStackRemove,          // Es war in FormatStack was entfernt
  tSE_FormatStackFormatToRemove,  // Es wird Element entfernt
  tSE_FormatStackParaChanged,     // Es war Para Eigenschaft in allen Elementen in FS geaendert
  tSE_FormatStackSegmentRemoved,  // Es war ganzes Segment in FS entfernt
};

///////////////////////////////////////////////////////////////////////////////
// CXPubClassProperty
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CXPubClassProperty
{
public:
  CXPubClassProperty();
  CXPubClassProperty(const CXPubClassProperty& cProp);
  CXPubClassProperty(const CXPubClassProperty* pProp);
  virtual ~CXPubClassProperty();

  void operator = (const CXPubClassProperty& cProp);
  void CopyFrom(const CXPubClassProperty* pProp);

  xtstring GetName() const {return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  int GetType() const {return m_nType;};
  void SetType(int nType){m_nType = nType;};
  xtstring GetValue() const {return m_sValue;};
  void SetValue(const xtstring& sValue){m_sValue = sValue;};

protected:
  xtstring  m_sName;
  int       m_nType;
  xtstring  m_sValue;
};


///////////////////////////////////////////////////////////////////////////////
// CXPubParagraphStyle
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CXPubParagraphStyle
{
  friend class CXPubStyleManager;
  friend class CXPubTabStop;
  friend class CFormatStack;

typedef vector<CXPubTabStop*>             CTabStopsVector;
typedef CTabStopsVector::iterator         CTabStopsIterator;
typedef CTabStopsVector::const_iterator   CTabStopsConstIterator;
typedef CTabStopsVector::reverse_iterator CTabStopsReverseIterator;

typedef map<xtstring, CXPubClassProperty*>    CClassProperties;
typedef CClassProperties::iterator            CClassPropertiesIterator;
typedef CClassProperties::const_iterator      CClassPropertiesConstIterator;
typedef CClassProperties::reverse_iterator    CClassPropertiesReverseIterator;
typedef pair<xtstring, CXPubClassProperty*>   CClassPropertiesPair;
typedef pair<CClassPropertiesIterator, bool>  CClassPropertiesInsert;

public:
  typedef enum TChangedProperty
  {
    tCP_Bold            = 0x0001,
    tCP_Italic          = 0x0002,
    tCP_Underline       = 0x0004,
    tCP_DoubleUnderline = 0x0008,
    tCP_StrikeOut       = 0x0010,
    tCP_FontName        = 0x0020,
    tCP_FontSize        = 0x0040,
    tCP_FontColor       = 0x0080,
    tCP_FontBackColor   = 0x0100,
    tCP_FontCodePage    = 0x0200,
    tCP_FontAll         = tCP_FontName|tCP_FontSize|tCP_FontColor|tCP_FontBackColor|tCP_FontCodePage,
    tCP_StyleElement    = 0x8000,
  };
  typedef enum TStyleType
  {
    tST_None,
    tST_ParagraphType,
    tST_CharacterType,
  };

  CXPubParagraphStyle();
  CXPubParagraphStyle(const CXPubParagraphStyle& cStyle);
  ~CXPubParagraphStyle();

  void operator = (const CXPubParagraphStyle& cStyle);
  void CopyFrom(const CXPubParagraphStyle* pStyle);

  bool IsDefaultStyle();
  void SetDefaultValues(bool bWithDefaultName);
  void SetParagraphStyle(const CXPubParagraphStyle* pStyle);
  void SetCharacterStyle(const CXPubParagraphStyle* pStyle);

  xtstring GetStyleName()
    {return m_sStyleName;};
  void SetStyleName(const xtstring& sStyleName)
    {m_sStyleName = sStyleName;};
  TStyleType GetStyleType(){return m_nStyleType;};
  void SetStyleType(TStyleType nStyleType){m_nStyleType = nStyleType;};
  int GetStyleID()
    {return m_nStyleID;};
  void SetStyleID(int nStyleID)
    {m_nStyleID = nStyleID;};

  int GetChangedProperty() const
    {return m_nChangedProperty;};
  void ClearChangedProperty()
    {m_nChangedProperty = 0;};

  bool GetStyleIsPermanent()
    {return m_bStyleIsPermanent;};
  void SetStyleIsPermanent(bool bStyleIsPermanent)
    {m_bStyleIsPermanent = bStyleIsPermanent;};

  xtstring GetNextStyle()
   {return m_sNextStyle;};
  void SetNextStyle(const xtstring& sNextStyle)
   {m_sNextStyle = sNextStyle;};

  xtstring GetFontName()
    {return m_sFontName;};
  void SetFontName(const xtstring& sFontName)
    {m_nChangedProperty |= tCP_FontName; m_sFontName = sFontName;};
  int GetFontSize()
    {return m_nFontSize;};
  void SetFontSize(int nFontSize)
    {m_nChangedProperty |= tCP_FontSize; m_nFontSize = nFontSize;};
  _ColorDef GetFontColor()
    {return m_nFontColor;};
  void SetFontColor(_ColorDef nFontColor)
    {m_nChangedProperty |= tCP_FontColor; m_nFontColor = nFontColor;};
  _ColorDef GetFontBackColor()
    {return m_nFontBackColor;};
  void SetFontBackColor(_ColorDef nFontBackColor)
    {m_nChangedProperty |= tCP_FontBackColor; m_nFontBackColor = nFontBackColor;};
  int GetCodePage()
    {return m_nCodePage;};
  void SetCodePage(int nCodePage)
    {m_nChangedProperty = tCP_FontCodePage; m_nCodePage = nCodePage;};

  bool GetBold()
    {return m_bBold;};
  void SetBold(bool bBold)
    {m_nChangedProperty |= tCP_Bold; m_bBold = bBold;};

  bool GetItalic()
    {return m_bItalic;};
  void SetItalic(bool bItalic)
    {m_nChangedProperty |= tCP_Italic; m_bItalic = bItalic;};

  bool GetUnderline()
    {return m_bUnderline;};
  void SetUnderline(bool bUnderline)
    {m_nChangedProperty |= tCP_Underline; m_bUnderline = bUnderline;};

  bool GetDoubleUnderline()
    {return m_bDoubleUnderline;};
  void SetDoubleUnderline(bool bDoubleUnderline)
    {m_nChangedProperty |= tCP_DoubleUnderline; m_bDoubleUnderline = bDoubleUnderline;};

  bool GetStrikeOut()
    {return m_bStrikeOut;};
  void SetStrikeOut(bool bStrikeOut)
    {m_nChangedProperty |= tCP_StrikeOut; m_bStrikeOut = bStrikeOut;};

  void GetParagraphIndents(int& nLeft, int& nRight, int& nIndent)
    {nLeft = m_nLeft; nRight = m_nRight; nIndent = m_nIndent;};
  void SetParagraphIndents(int nLeft, int nRight, int nIndent)
    {m_nLeft = nLeft; m_nRight = nRight; m_nIndent = nIndent;};

  void GetTextAlignment(int& nTextAlign)
    {nTextAlign = m_nTextAlign;};
  void SetTextAlignment(int nTextAlign)
    {m_nTextAlign = nTextAlign;};


  void GetParagraphControl(int& nKeep, int& nKeepNext, int& nWidowOrphan)
    {nKeep = m_nKeep; nKeepNext = m_nKeepNext; nWidowOrphan = m_nWindowOrphan;};
  void SetParagraphControl(int nKeep, int nKeepNext, int nWidowOrphan)
    {if (nKeep != -1) m_nKeep = nKeep; if (nKeepNext != -1) m_nKeepNext = nKeepNext; if (nWidowOrphan != -1) m_nWindowOrphan = nWidowOrphan;};
  void SetParagraphKeepNext(int nKeepNext)
    {m_nKeepNext = nKeepNext;};
  void SetParagraphKeep(int nKeep)
    {m_nKeep = nKeep;};
  void SetParagraphWindowOrphan(int nWindowOrphan)
    {m_nWindowOrphan = nWindowOrphan;};

  void GetParagraphSpacing(int& nSpaceBefore, int& nSpaceAfter, int& nSpaceBetween, int& nLineSpacing)
    {nSpaceBefore = m_nSpaceBefore; nSpaceAfter = m_nSpaceAfter; nSpaceBetween = m_nSpaceBetween; nLineSpacing = m_nLineSpacing;};
  void SetParagraphSpacing(int nSpaceBefore, int nSpaceAfter, int nSpaceBetween, int nLineSpacing)
    {m_nSpaceBefore = nSpaceBefore; m_nSpaceAfter = nSpaceAfter; m_nSpaceBetween = nSpaceBetween; m_nLineSpacing = nLineSpacing;};

  void GetTabDefaults(int& nTabDefaultPos, int& nTabDefaultDecimalChar)
    {nTabDefaultPos = m_nTabDefaultPos; nTabDefaultDecimalChar = m_nTabDefaultDecimalChar;};
  void SetTabDefaults(int nTabDefaultPos, int nTabDefaultDecimalChar)
    {m_nTabDefaultPos = nTabDefaultPos; m_nTabDefaultDecimalChar = nTabDefaultDecimalChar;};

  void GetPageBreakBefore(int& nPageBreakBefore)
    {nPageBreakBefore = m_nPageBreakBefore;};
  void SetPageBreakBefore(int nPageBreakBefore)
    {m_nPageBreakBefore = nPageBreakBefore;};

  // Tab Stops
  size_t CountOfTabStops() const {return m_cTabStops.size();};
  CXPubTabStop* GetTabStop(size_t nIndex) const {if (nIndex < CountOfTabStops()) return m_cTabStops[nIndex]; return 0;};
  bool AddTabStop(const CXPubTabStop& cTabStop);
  bool AddTabStop(int nTabFlag, int nTabPos, const xtstring& sTabDecChar, const xtstring& sTabLeader);
  void RemoveTabStop(size_t nIndex){if (nIndex < CountOfTabStops()) m_cTabStops.erase(m_cTabStops.begin() + nIndex);};
  void RemoveAllTabStops();
  // Class Properties
  size_t CountOfClassProperties() const {return m_cClassProperties.size();};
  CXPubClassProperty* GetClassProperty(size_t nIndex) const;
  CXPubClassProperty* GetClassProperty(const xtstring& sName);
  void CopyClassProperties(const CXPubParagraphStyle* pStyle);
  bool AddClassProperty(const CXPubClassProperty& cClassProperty);
  bool AddClassProperty(const CXPubClassProperty* pClassProperty);
  void RemoveClassProperty(size_t nIndex);
  void RemoveClassProperty(const xtstring& sName);
  void RemoveAllClassProperties();

protected:
  xtstring    m_sStyleName;
  TStyleType  m_nStyleType;
  int         m_nStyleID;             // interne Style ID wird beim Anlegen in RTF vergeben

  int       m_nChangedProperty;           // gesetzt, wenn SetXXXX Funktion aufgerufen ist
  bool      m_bStyleIsPermanent;    // true - Style sollte man immer erzeugen (wenn auch nicht benutzt)

  // Hinter jeder Variable befinden sich zwei Zeichen (+-)
  // + bedeutet, dass es schon benutzt ist
  // - bedeutet, dass es noch nicht benutzt ist
  // erste Spalte bedeutet, dass es in SetStyle benutzt ist
  // zwei Spalte bedeutet, dass es in FormatStack benutzt ist
  //                                    //  SetStyle      FormatStack   ForWholeStack
  xtstring  m_sNextStyle;               //  *             *             ?

  xtstring  m_sFontName;                //  +             +             -
  int       m_nFontSize;                //  +             +             -
  _ColorDef m_nFontColor;               //  +             +             -
  _ColorDef m_nFontBackColor;           //  +             +             -
  int       m_nCodePage;                //  +             +             -
  bool      m_bBold;                    //  +             +             -
  bool      m_bItalic;                  //  +             +             -
  bool      m_bUnderline;               //  +             +             -
  bool      m_bDoubleUnderline;         //  +             +             -
  bool      m_bStrikeOut;               //  +             +             -

  int       m_nLeft;                    //  +             +             +
  int       m_nRight;                   //  +             +             +
  int       m_nIndent;                  //  +             +             +
  int       m_nTextAlign;               //  +             +             +

  int       m_nKeep;                    //  +             +             +
  int       m_nKeepNext;                //  +             +             +
  int       m_nWindowOrphan;            //  +             +             +
  int       m_nPageBreakBefore;         //  +             +             +

  int       m_nSpaceBefore;             //  +             +             +
  int       m_nSpaceAfter;              //  +             +             +
  int       m_nSpaceBetween;            //  +             +             +
  int       m_nLineSpacing;             //  +             +             +

  int       m_nTabDefaultPos;           //  +             +             +
  int       m_nTabDefaultDecimalChar;   //  +             +             +
  CTabStopsVector m_cTabStops;          //  +             +             +
  CClassProperties  m_cClassProperties; //  +             -             -
};





///////////////////////////////////////////////////////////////////////////////
// CXPubTabStop
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CXPubTabStop
{
public:
  CXPubTabStop();
  CXPubTabStop(const CXPubTabStop& cTabStop);
  CXPubTabStop(const CXPubTabStop* pTabStop);
  ~CXPubTabStop();

  void operator = (const CXPubTabStop& cTabStop);
  void CopyFrom(const CXPubTabStop* pTabStop);

  int       GetTabFlag()
              {return m_nTabFlag;};
  void      SetTabFlag(int nTabFlag)
              {m_nTabFlag = nTabFlag;};
  int       GetTabPos()
              {return m_nTabPos;};
  void      SetTabPos(int nTabPos)
              {m_nTabPos = nTabPos;};
  xtstring  GetTabDecChar()
              {return m_sTabDecChar;};
  void      SetTabDecChar(const xtstring& sTabDecChar)
              {m_sTabDecChar = sTabDecChar;};
  xtstring  GetTabLeader()
              {return m_sTabLeader;};
  void      SetTabLeader(const xtstring& sTabLeader)
              {m_sTabLeader = sTabLeader;};

protected:
  int                   m_nTabFlag;
  int                   m_nTabPos;
  xtstring              m_sTabDecChar;
  xtstring              m_sTabLeader;
};




///////////////////////////////////////////////////////////////////////////////
// CFormatStackItem
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CFormatStackItem : public CXPubParagraphStyle
{
public:
  CFormatStackItem();
  CFormatStackItem(const CFormatStackItem& cItem);
  ~CFormatStackItem();

  void operator = (const CFormatStackItem& cItem);
  void CopyFrom(const CFormatStackItem* pItem);

  bool GetItemIsFirstInSegment(){return m_bItemIsFirstInSegment;};
  void SetItemIsFirstInSegment(bool bItemIsFirstInSegment){m_bItemIsFirstInSegment = bItemIsFirstInSegment;};

protected:
  bool m_bItemIsFirstInSegment;
};

///////////////////////////////////////////////////////////////////////////////
// CFormatStack
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CFormatStack
{

typedef vector<CFormatStackItem*>           CFormatStackItems;
typedef CFormatStackItems::iterator         CFormatStackItemsIterator;
typedef CFormatStackItems::const_iterator   CFormatStackItemsConstIterator;
typedef CFormatStackItems::reverse_iterator CFormatStackItemsReverseIterator;

public:
  CFormatStack();
  ~CFormatStack();

  xtstring GetStackName(){return m_sStackName;};
  void SetStackName(const xtstring& sStackName){m_sStackName = sStackName;};

  CFormatStackItem* GetActualFormatInStack();

  bool CreateNewItemInFormatStack(size_t nPos);
  bool RecopyProperties(size_t nPos);
  CXPubParagraphStyle* GetItemInFormatStack(size_t nPos);
  CXPubParagraphStyle* GetItemForRelease(CXPubParagraphStyle::TChangedProperty nChangedProperty);
  bool ReleaseItemInFormatStack(CXPubParagraphStyle::TChangedProperty nChangedProperty, size_t& nPos);
  bool CreateNewSegmentInFormatStack(CXPubParagraphStyle* pDefaultStyle);
  bool ReleaseSegmentInFormatStack();
  size_t GetCountOfSegmentsInStack();

  bool SetPageBreakBefore(int nPageBreakBefore);
  bool SetParagraphKeepNext(int nKeepNext);
  bool SetParagraphKeep(int nKeep);
  bool SetParagraphWindowOrphan(int nWindowOrphan);
  bool SetParagraphIndents(int nLeft, int nRight, int nIndent);
  bool SetTextAlignment(int nAlignment);
  bool SetParagraphSpacing(int nSpaceBefore, int nSpaceAfter, int nSpaceBetween, int nLineSpacing);
  bool SetTabDefaults(int nTabDefaultPos, int nTabDefaultDecimalChar);
  bool AddTabStop(int nTabFlag, int nTabPos, const xtstring& sTabDecChar, const xtstring& sTabLeader);
  bool ClearTabStops();

  bool DefaultStyleChanged(CXPubParagraphStyle* pDefaultStyle);

  size_t CountOfItems(){return m_cItems.size();};
protected:
  bool AddItem(const CFormatStackItem& cItem, size_t nPos);
  bool RemoveTopItem();
  bool RemoveItem(size_t nIndex);

  void RemoveAllItems();


protected:
  xtstring  m_sStackName;
  CFormatStackItems m_cItems;
};





///////////////////////////////////////////////////////////////////////////////
// CBridgeToStyleManager
////////////////////////////////////////////////////////////////////////////////
// Instanz dieser Klasse ist in ConvertWorkerTo in dem Moment gesetzt,
// wenn ConvertWorkerTo uber "AttachConvertTo" zu ConvertWorkerFrom
// zugeordnet ist.
// Bei "DetachConvertTo" ist es rueckgesetzt.
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CBridgeToStyleManager
{
public:
  CBridgeToStyleManager(){};
  ~CBridgeToStyleManager(){};

  virtual CXPubParagraphStyle* GetActualFormatInStack() = 0;

  virtual CXPubParagraphStyle* GetActualStyle() = 0;
  virtual xtstring GetSelectedStyleName() = 0;
  virtual CXPubParagraphStyle* GetDefaultStyle() = 0;
};





///////////////////////////////////////////////////////////////////////////////
// CXPubStyleManager
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CXPubStyleManager : public CBridgeToStyleManager
{

typedef map<xtstring, CXPubParagraphStyle*>   CStyles;
typedef CStyles::iterator                     CStylesIterator;
typedef CStyles::const_iterator               CStylesConstIterator;
typedef CStyles::reverse_iterator             CStylesReverseIterator;
typedef pair<xtstring, CXPubParagraphStyle*>  CStylesPair;
typedef pair<CStylesIterator, bool>           CStylesInsert;

public:
  CXPubStyleManager();
  ~CXPubStyleManager();

  void SetDefaultStyleAsActualStyle();
  void SetActualStyle(const xtstring& sStyleName);
  virtual CXPubParagraphStyle* GetActualStyle();
  virtual xtstring GetSelectedStyleName(){return m_sSelectedStyleName;};

  void SetDefaultStyleAsActualStyleToSet();
  void SetActualStyleToSet(const xtstring& sStyleName);
  void SetNoActualStyleToSet();
  CXPubParagraphStyle* GetActualStyleToSet();

  bool BackUpStyle(){m_sSelectedStyleName_BackUp = m_sSelectedStyleName; m_sRealSelectedStyleName_BackUp = GetActualStyle()->GetStyleName(); return true;};
  bool RestoreStyle(){m_sSelectedStyleName = m_sSelectedStyleName_BackUp; SetActualStyle(m_sRealSelectedStyleName_BackUp); return true;};

  CXPubParagraphStyle* CreateNewStyle(const xtstring& sStyleName, const xtstring& sBasedOnStyle);

  virtual CXPubParagraphStyle* GetDefaultStyle();
  CXPubParagraphStyle* GetStyle(const xtstring& sStyleName);
  void RemoveStyle(const xtstring& sStyleName);

  // from CBridgeToStyleManager
  virtual CXPubParagraphStyle* GetActualFormatInStack(){return m_cFormatStack.GetActualFormatInStack();};
  size_t CountOfFormatsInFormatStack(){return m_cFormatStack.CountOfItems();};
  CXPubParagraphStyle* GetFormatInStack(size_t nPos){return m_cFormatStack.GetItemInFormatStack(nPos);};

  ///////////////
  // Format Stack
  bool CreateNewItemInFormatStack(size_t nPos)
          {return m_cFormatStack.CreateNewItemInFormatStack(nPos);};
  bool RecopyProperties(size_t nPos)
          {return m_cFormatStack.RecopyProperties(nPos);};
  CXPubParagraphStyle* GetItemInFormatStack(size_t nPos)
          {return m_cFormatStack.GetItemInFormatStack(nPos);};
  CXPubParagraphStyle* GetItemForRelease(CXPubParagraphStyle::TChangedProperty nChangedProperty)
          {return m_cFormatStack.GetItemForRelease(nChangedProperty);};
  bool ReleaseItemInFormatStack(CXPubParagraphStyle::TChangedProperty nChangedProperty, size_t& nPos)
          {return m_cFormatStack.ReleaseItemInFormatStack(nChangedProperty, nPos);};
  bool CreateNewSegmentInFormatStack()
          {return m_cFormatStack.CreateNewSegmentInFormatStack(GetDefaultStyle());};
  bool ReleaseSegmentInFormatStack()
          {return m_cFormatStack.ReleaseSegmentInFormatStack();};
  size_t GetCountOfSegmentsInStack()
          {return m_cFormatStack.GetCountOfSegmentsInStack();};

  bool SetPageBreakBefore(int nPageBreakBefore)
          {return m_cFormatStack.SetPageBreakBefore(nPageBreakBefore);};
  bool SetParagraphKeepNext(int nKeepNext)
          {return m_cFormatStack.SetParagraphKeepNext(nKeepNext);};
  bool SetParagraphKeep(int nKeep)
          {return m_cFormatStack.SetParagraphKeep(nKeep);};
  bool SetParagraphWindowOrphan(int nWindowOrphan)
          {return m_cFormatStack.SetParagraphWindowOrphan(nWindowOrphan);};
  bool SetParagraphIndents(int nLeft, int nRight, int nIndent)
          {return m_cFormatStack.SetParagraphIndents(nLeft, nRight, nIndent);};
  bool SetTextAlignment(int nAlignment)
          {return m_cFormatStack.SetTextAlignment(nAlignment);};
  bool SetParagraphSpacing(int nSpaceBefore, int nSpaceAfter, int nSpaceBetween, int nLineSpacing)
          {return m_cFormatStack.SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);};
  bool SetTabDefaults(int nTabDefaultPos, int nTabDefaultDecimalChar)
          {return m_cFormatStack.SetTabDefaults(nTabDefaultPos, nTabDefaultDecimalChar);};
  bool AddTabStop(int nTabFlag, int nTabPos, const xtstring& sTabDecChar, const xtstring& sTabLeader)
          {return m_cFormatStack.AddTabStop(nTabFlag, nTabPos, sTabDecChar, sTabLeader);};
  bool ClearTabStops()
          {return m_cFormatStack.ClearTabStops();};

protected:
  void RemoveAllStyles();

protected:
  xtstring              m_sSelectedStyleName;
  CXPubParagraphStyle*  m_pActualStyle;
  CXPubParagraphStyle*  m_pActualStyleToSet;
  CStyles               m_cStyles;
  CFormatStack          m_cFormatStack;

  xtstring              m_sSelectedStyleName_BackUp;
  xtstring              m_sRealSelectedStyleName_BackUp;
};



#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_STYLEMANAGER_H_)

