#if !defined(_CONVERTWORKER_H_)
#define _CONVERTWORKER_H_


#include "AddOnManagerExpImp.h"
#include "SErrors/ErrorNumbers.h"
#include "SOpSys/types.h"
#include "TagStructs.h"
#include "StyleManager.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CXPubAddOnManager;
class CConvertWorkerFrom;
class CConvertWorkerTo;

typedef std::vector<CConvertWorkerFrom*>      CConvertWorkersFrom;
typedef CConvertWorkersFrom::iterator         CConvertWorkersFromIterator;
typedef CConvertWorkersFrom::const_iterator   CConvertWorkersFromConstIterator;
typedef CConvertWorkersFrom::reverse_iterator CConvertWorkersFromReverseIterator;

typedef std::vector<CConvertWorkerTo*>        CConvertWorkersTo;
typedef CConvertWorkersTo::iterator           CConvertWorkersToIterator;
typedef CConvertWorkersTo::const_iterator     CConvertWorkersToConstIterator;
typedef CConvertWorkersTo::reverse_iterator   CConvertWorkersToReverseIterator;

class CTableStackItem
{
public:
  CTableStackItem(){};
  virtual ~CTableStackItem(){};

public:
  int m_nTableID;
  int m_nTableRow;
  int m_nTableCol;
  int m_nTableHdrRows;

};

class XPUBADDONMANAGER_EXPORTIMPORT CConvertFromAddOn
{
public:
  CConvertFromAddOn(CXPubAddOnManager* pAddOnManager);
  virtual ~CConvertFromAddOn();

  virtual CConvertWorkerFrom* CreateConvertWorkerFrom() = 0;
  virtual CConvertWorkerFrom* CreateConvertWorkerFromClone(CConvertWorkerFrom* pConvertWorker) = 0;
  virtual bool ReleaseConvertWorkerFrom(CConvertWorkerFrom* pConvertWorker) = 0;

  CXPubAddOnManager* GetAddOnManager(){return m_pAddOnManager;};

  bool DestroyAndRemoveAllConvertWorkers();
protected:
  bool AddConvertWorkerFrom(CConvertWorkerFrom* pWorker);
  bool RemoveConvertWorkerFrom(CConvertWorkerFrom* pWorker);

  CXPubAddOnManager*  m_pAddOnManager;
  CConvertWorkersFrom m_cWorkers;
};

class XPUBADDONMANAGER_EXPORTIMPORT CConvertToAddOn
{
public:
  CConvertToAddOn(CXPubAddOnManager* pAddOnManager);
  virtual ~CConvertToAddOn();

  virtual CConvertWorkerTo* CreateConvertWorkerTo(HWND hWndParent) = 0;
  virtual CConvertWorkerTo* CreateConvertWorkerToClone(CConvertWorkerTo* pConvertWorker) = 0;
  virtual bool ReleaseConvertWorkerTo(CConvertWorkerTo* pConvertWorker) = 0;

  CXPubAddOnManager* GetAddOnManager(){return m_pAddOnManager;};

  bool DestroyAndRemoveAllConvertWorkers();
protected:
  bool AddConvertWorkerTo(CConvertWorkerTo* pWorker);
  bool RemoveConvertWorkerTo(CConvertWorkerTo* pWorker);

  CXPubAddOnManager*  m_pAddOnManager;
  CConvertWorkersTo   m_cWorkers;
};

// Textformat
#define CNV_TEXTFORMAT_ANSI   1   // Textformat Ansizeichensatz
#define CNV_TEXTFORMAT_TEXT   2   // OEM Windowszeichensatz

typedef enum TTypeOfFormater
{
	PDF_Formater,
	RTF_Formater,
	WORD_Formater,
  INDESIGN_Formater,
  FDF_Formater,
};

typedef enum TTypeOfCase
{
  tCASE_NONE = 0,

  tCASE_NOPAGEBREAK,
  tCASE_NOPAGEBREAK_END,

  tCASE_TABLE,
  tCASE_TABLE_END,

  // Paragraph
  tCASE_PARABREAK, //	 1 // <p> 
  tCASE_LINE,			 //	 2 // <l>
	tCASE_PARAGRAPH, //  3 // <Links/Rechts>
  tCASE_PARAGRAPH_END,
  tCASE_PARAGRAPH_PAGEBREAK,

  // Ausrichtung
  tCASE_TEXTZ, //  1 // <TextZ> 
  tCASE_TEXTL, //  2 // <TextL>
  tCASE_TEXTR, //  3 // <TextR>
  tCASE_TEXTB, //  4 // <TextB>

  // Tabulator
  tCASE_TAB,	//       0 // dezimaler Tabulator nach Komma
  tCASE_TABCLEAR, //   1 // dezimaler Tabulator nach Komma
	tCASE_TABSIMPLE,//   2 // <Tab> 
	tCASE_TABDEFAULTS,// 3 // Defaults für Tabulator

  // Format
  tCASE_BOLD, //           1   // Fett an
  tCASE_BOLD_END, //       2   // Fett aus
  tCASE_ITALIC, //         3   // Kursiv an
  tCASE_ITALIC_END, //     4   // Kursiv aus
  tCASE_UNDERLINE, //      5   // Unterstrichen an
  tCASE_UNDERLINE_END, //  6   // Unterstrichen aus
	tCASE_UL_DOUBLE, //      7   // Doppelt an
	tCASE_UL_DOUBLE_END, //  8   // Doppelt aus
	tCASE_STRIKE,		 //      9   // Durchgestrichen an
	tCASE_STRIKE_END,	   //  10  // Durchgestrichen aus

  // Font
  tCASE_FONT_END,							// FontHoehe
  tCASE_FONT,									// FontName

  // Fontcolor
  tCASE_STYLE,									// Paragraphstyle
	tCASE_STYLE_END,							// Paragraphstyle end
	tCASE_STYLE_TEXTCOLOR_END,		// Textfarbe auf Default stellen

  // Grafik
  tCASE_GRAFIC_NORMAL, //    1   // normale Grafik
  tCASE_GRAFIC_BAUSTEIN, //  2   // Grafik über PVS-Baustein

  // Kopf/Fuss Positionen
  tCASE_HEADER,									// in den Header
  tCASE_HEADER_END,							// zurück in Text
  tCASE_FOOTER,									// in den Fuß
  tCASE_FOOTER_END,							// zurück in Text
	tCASE_FOOTER_MARGIN,
	tCASE_HEADER_MARGIN,

  // Box
  tCASE_BOX, 
  tCASE_BOXEND,

  // Textrahmen 
  tCASE_POSFRAME,
  tCASE_POSFRAME_END,
  tCASE_POSFRAME_BREAK,

  // Cursor Movements 
  tCASE_CURSOR_HOME,
  tCASE_CURSOR_END,

  // Cursor Movements 
  tCASE_LINK,
  tCASE_LINK_END,

  // Layer 
  tCASE_LAYER,
  tCASE_LAYER_END,
    
  // spezieller Pagebreak
  tCASE_PAGEBREAK,
  tCASE_FILLPAGEBREAK,
  tCASE_PARAGRAPHPAGEBREAK,

  // Library
  tCASE_LIBRARY_END,

};

typedef struct _ConvertDefaults
{
	int  DefLang;					    // Default language
  char DefFontName[100];
  char DefCodePage[100];    // Default Codepage
	int  DefFontSize;
	char DefDecimal[5];		// Tabulatorausrichtungszeichen bei "TABD"

	int DefCellMargin;		// Cellmargin in twips
  int DefTabWidth;			// Default Tabposition

	int PaperOrient;			// protrait orientation Werte: DMORIENT_PORTRAIT oder DMORIENT_LANDSCAPE
	int PageWidth;				// Default page width
	int PageHeight;				// Default page height
	int LeftMargin;				// linker Rand
	int RightMargin;			// rechter Rand
	int TopMargin;				// oberer Rand
	int BotMargin;	      // unterer Rand
	int HdrMargin;
	int FtrMargin;	
// section
	int ColumnSpace;			// bei TER ".5" inches
	int columns;					// bei TER "1"

}ConvertDefaults;

#define TAG_POS_BEG 0 // position at the beginning of text
#define TAG_POS_END 1 // position at the end of text

class XPUBADDONMANAGER_EXPORTIMPORT CConvertWorker
{
public:
  CConvertWorker();
  virtual ~CConvertWorker();

  virtual TDataFormatForStoneAndTemplate GetDataFormat() = 0;

  void SetRootTemplatePath(const xtstring& sRootTemplatePath){ m_sRootTemplatePath = sRootTemplatePath;};
  void SetRootPicturePath(const xtstring& sRootPicturePath){ m_sRootPicturePath = sRootPicturePath;};

protected:

  // mini help
  xtstring GetKeyStrFromIni(const xtstring& sSection, const xtstring& sKey, const xtstring& sFileName);
  void GetModulePath(xtstring& sPath);
  bool GetTemporaryFileName(const xtstring& sInPicture, xtstring& sOutPicture, xtstring& sExt);
  bool IsPathFile(const xtstring& sFilePathName);
  bool SetPathCombine(xtstring& sDestFilePathName, const xtstring& sDirectory, const xtstring& sFileName);
	bool SplitPath(const xtstring& sFileName, xtstring& sDrive, xtstring& sDir, xtstring& sName, xtstring& sExt);
	void GetRootTemplatePath(xtstring& sRootTemplatePath){ sRootTemplatePath = m_sRootTemplatePath;};
  void GetRootPicturePath(xtstring& sRootPicturePath){ sRootPicturePath = m_sRootPicturePath;};

public:
  virtual void SetChannelToExecutionModule(CChannelToExecutedEntity* pChannelCallBack,
                                           CChanelToExecutionModule* pExecModuleCallBack);
protected:
  
	CChannelToExecutedEntity* m_pChannelCallBack;
  CChanelToExecutionModule* m_pExecModuleCallBack;
	ConvertDefaults m_sDefaults;

  CErrorObject  m_cError;

 // globale Pfade für Template und Bilder
  xtstring m_sRootTemplatePath;
  xtstring m_sRootPicturePath;

};

class XPUBADDONMANAGER_EXPORTIMPORT CConvertWorkerProgress
{
public:
  CConvertWorkerProgress();
  virtual ~CConvertWorkerProgress();

  virtual void StartProgress() = 0;
  virtual void StepProgress(int nPos) = 0;
  virtual void EndProgress() = 0;
};

class XPUBADDONMANAGER_EXPORTIMPORT CConvertWorkerFrom : public CConvertWorker
{
  friend class CConvertWorkerTo;
public:
  CConvertWorkerFrom(TDataFormatForStoneAndTemplate nDataFormat);
  virtual ~CConvertWorkerFrom();

  virtual TDataFormatForStoneAndTemplate GetDataFormat(){return m_nDataFormat;};

  bool AttachConvertTo(CConvertWorkerTo* pConvertTo);
  CConvertWorkerTo* DetachConvertTo();

  void SetConvertWorkerProgress(CConvertWorkerProgress *pConvertProgress){m_pConvertProgress = pConvertProgress;};

  // functions to overload
  virtual bool ConvertAndAdd(const xtstring& sDataStream, const xtstring& sXPubConst, TWorkPaperCopyType eWorkPaperCopyType) = 0;
	virtual bool GetLibraryStream(const xtstring& sLibraryName, xtstring& sDataStream){ return false;};

  void SetKeywords(TagKeywords* pKeywords){m_pKeywords = pKeywords;};
  bool GetOnlyFreewareKeywordsUsed(){return m_bOnlyFreewareKeywordsUsed;};
  bool KeywordIsFreeware(const xtstring& sKeyword);
  bool ParameterIsFreeware(const xtstring& sKeyword, const xtstring& sParameter);

  void SetRestrictFeatures(bool bRestrictFeatures){m_bRestrictFeatures = bRestrictFeatures;};
protected:
  virtual CConvertWorkerFrom* Clone() = 0;

  TDataFormatForStoneAndTemplate  m_nDataFormat;
  CConvertWorkerTo*               m_pConvertTo;
  CConvertWorkerProgress*         m_pConvertProgress;
  CXPubStyleManager m_cStyleManager;
  TagKeywords*      m_pKeywords;
  bool              m_bOnlyFreewareKeywordsUsed;
  bool              m_bRestrictFeatures;
};

//Dokumentattribute von RTF und PDF zusammengefasst
typedef enum TDocumentProperty
{
	eDoc_Title = 1,
	eDoc_Comment,
	eDoc_Category,
	eDoc_Operator,
	eDoc_Manager,
	eDoc_Author,
	eDoc_Subject,
	eDoc_Keywords,
	eDoc_Creator,  
	eDoc_Producer, 
	eDoc_CreationDate,
	eDoc_ModDate,
};

class XPUBADDONMANAGER_EXPORTIMPORT CConvertWorkerTo : public CConvertWorker
{
  friend class CConvertWorkerFrom;
public:
  CConvertWorkerTo(TDataFormatForStoneAndTemplate nDataFormat, HWND hWndParent);
  virtual ~CConvertWorkerTo();

  virtual TDataFormatForStoneAndTemplate GetDataFormat(){return m_nDataFormat;};

  // functions to overload
  virtual bool IsDataFormatAvailable(TDataFormatForStoneAndTemplate nOutputDataFormat) = 0;
  virtual xtstring GetDataStream(TDataFormatForStoneAndTemplate nOutputDataFormat) = 0;
  virtual bool GetDataStream(TDataFormatForStoneAndTemplate nOutputDataFormat, char *pOutput, int &nSize) = 0;
  virtual bool SaveDataStream(const xtstring& sfilename) = 0;
  virtual bool PositionCursor(const xtstring& sXPubConst, TWorkPaperCopyType eWorkPaperCopyType) = 0;
  virtual bool PrintDataStream(const xtstring& sfilename, bool bSelectOrPrint) = 0;

public:
  // functions to overload
	virtual TTypeOfFormater GetFormaterType() = 0;

	virtual bool SetVersion(const int nVersion) = 0;
	virtual bool SetMetric(const int nType) = 0;
	virtual bool Spread(const int nFrom, const int nTo, const int nType, const int nCount) = 0;
	virtual bool Page(const xtstring& sJoin, const xtstring& sAsset, const xtstring& sLib, const xtstring& sOverset,
		 const xtstring& sRepeat, const int nFrom, const int nTo, const int nType) = 0;
	virtual bool Asset(const xtstring& id, const xtstring& sName, const xtstring& sFileName) = 0;
	virtual bool Module(const xtstring& id, const xtstring& sFunktion, const xtstring& sParameter, const xtstring& sLayer) = 0;
	virtual bool SetDocumentInfo(const TDocumentProperty nTypeOfProperty, const xtstring& sValue) = 0;
  virtual bool SetPassword(const xtstring& sOwner, const xtstring& sUser, int nPermission) = 0;
  virtual bool InsertTextFromMemory(const xtstring& sInput, int nFormat/* = CNV_TEXTFORMAT_TEXT*/) = 0;
  virtual bool InsertParagraph() = 0;
  virtual bool BeginPageBody(const int nPos) = 0;
  virtual bool SetDefaults() = 0;
  virtual bool InsertLine() = 0;
	virtual bool SetParagraphSpacing(int nSpaceBefore, int nSpaceAfter, int nSpaceBetween, int nLineSpacing) = 0;
	virtual bool SetParagraphControl(const int nKeep, const int nKeepNext, const int nWidowOrphan) = 0;
  virtual bool SetParagraphIndents(int nLinks, int nRechts, int nFirstIndent) = 0;
  virtual bool InsertTabulator(int nTabFlag, int nTabPos, const xtstring& sTabDecChar, const xtstring& sTabLeader) = 0;
	virtual bool SetDefTabulator(int nNewWidth, int nDecDel) = 0;
  virtual bool InsertTab() = 0;
  virtual bool SetFont(const xtstring& sFontName, int nFontSize, int nCodePage = -1) = 0;
  virtual bool SetFontName(const xtstring& sFontName) = 0;
  virtual bool SetFontNameDefault(const xtstring& sFontName) = 0;
  virtual bool SetCodePage(const xtstring& sCodePage)=0;
  virtual bool SetCodePageDefault(const xtstring& sCodePage)=0;
  virtual xtstring GetCodePageDefault()=0;
  virtual bool SetFontSize(int nFontSize) = 0;
	virtual bool SetFontSizeDefault(int nFontSize) = 0;
	virtual int  GetFontSizeDefault() = 0;
	virtual xtstring&  GetFontNameDefault() = 0;
	virtual bool Import(const xtstring& id, const xtstring& sName, const xtstring& cPath,  float nWidht, float nHeight,
		const xtstring& native, const int type=0, int nFitting=0) = 0;
	virtual bool Inline(const xtstring& id, const xtstring& sName, const xtstring& cPath,  float nWidht, float nHeight,
		const xtstring& native, const double dXScale=0.0, const double dYScale=0.0, const int type=0, int nFitting=0) = 0;
	virtual bool InsertImage(const xtstring& id, const xtstring& sName, const xtstring& cPath,  float nWidht, float nHeight,
		int nZoom, bool bLinked=true, bool bShrink=false, int nFitting=0) = 0;
  virtual bool SetFontAttribute(int nAttrib) = 0;
  virtual bool SetTextColor(_ColorDef color) = 0;
  virtual bool SetHeaderFooterPos(int nCase, int nPosition) = 0;
  virtual bool SwitchHeaderFooter(int nCase, int nPos/*=CURSOR_END*/) = 0;
  virtual bool ToggleEditHdrFtr() = 0;
  virtual bool TextAlignment(int nAlign) = 0;
  virtual bool SetCursorPosition(int nCase, int nSection /*= 0*/) = 0;
	virtual int  CreateRectangle(const xtstring& id, const xtstring& sName, int nLeft, int nTop, int nWidth, int nHeight,
								int LineThickness, const xtstring& LineColor, const xtstring& FillColor) = 0;
  virtual bool InsertTextFrameColumns(const xtstring& sName, const int nColumns, const double nWidth, const double nGutter) = 0;
  virtual int CreateTextFrame(const xtstring& id, const xtstring& sName, const xtstring& sJoin, const xtstring& sLibrary, int nLeft, int nTop, int nWidth, int nHeight, int nVertAlign, int nHorzAlign, int nType,
		int LineThickness=-1, _ColorDef LineColor=(COLORDEF)-1, _ColorDef FillColor=(COLORDEF)-1, int nWrap=0, int nTextDirection=0) = 0;
  virtual bool CheckTextFrameOverset(const xtstring& sName) = 0;
	virtual bool Line(int nX1, int nY1, int nX2, int nY2, int nWidth, _ColorDef LineColor) = 0;
  virtual bool Margins(int nLeft, int nRight, int Top, int Button) = 0;
  virtual bool SetSectionOrientation(int nOrient/*=SECTION_PORTRAIT*/) = 0;
  virtual bool SetPaperSize(int nDmConstant, int nWidth/*=0*/, int nLength/*=0*/) = 0;
  virtual bool DocumentInfo(const xtstring& cTag) = 0;
  virtual bool GetFormFieldValue(const xtstring& cName, xtstring& cValue) = 0;
  virtual bool MoveFormField(const xtstring& cName, long i_lDeltaX, long i_lDeltaY) = 0;
  virtual bool FormField(const xtstring& cName, const xtstring& sValue, const xtstring& sType,
    const xtstring& sMask, const int nLength, const xtstring& sFontName, const int nFontSize, const int nCodePage=0) = 0;
  virtual bool CheckFormFieldOverset(const xtstring& cName, const xtstring& cValue) = 0;
  virtual bool MergeField(const xtstring& cTag) = 0;
  virtual bool Field(const xtstring& cTag) = 0;
  virtual bool SetPageCount(int nCount) = 0;
  virtual bool SetCurrentPage(int nCount) = 0;
  virtual bool SetPageNumberFormat(int nFormat) = 0;
  virtual bool InsertPosFrameBreak() = 0;
  virtual bool InsertPageBreak() = 0;
  virtual bool ParagraphPageBreak(const int nInsert) = 0;
  virtual bool FillPageBreak() = 0;
  virtual bool InsertSectionBreak() = 0;
  virtual bool InsertColumnBreak() = 0;
  virtual bool KeepParTogether(bool bBegin) = 0;
  virtual bool CanPageBreak(bool bDelete) = 0;
  virtual bool CheckCanPageBreak() = 0;
	virtual bool EditSection(int NumCols, int ColSpace, int FirstPageNo) = 0;
	virtual bool InsertBookmark(xtstring& sLevel, xtstring& sText, xtstring& sSymbol, int nZoom, bool bIgnorePos, int nColor, int nStyle) = 0;
  virtual bool InsertLink(const xtstring& sReferenzName, const xtstring& sSymbolName, int nPage, int nZoom) = 0;
  virtual bool InsertAnchor(const xtstring& sAnchorName) = 0;
  virtual bool LoadTemplate(const xtstring& sFileName, const xtstring& sLayer, const xtstring& sOwnerPwd, int nModus=0) = 0;

  virtual int CreateTable(const xtstring& id, const xtstring& sName, int wRows, int wColumns,
							int numHeaderRows, int numFooterRows, double spacebefore, double spaceafter) = 0;
  virtual bool CellBackgroundColor(int nTableID, int wRow, int wCol, _ColorDef crBkColor) = 0;
  virtual bool SetCellBorderWidth(int nTableID, int wRow, int wCol, int nLeft, int nTop, int nRight, int nButton) = 0;
  virtual bool SetCursorAfterTable(int nTableID) = 0;
	virtual bool SetTableHeaderRow(int nTableID, int wFromRow, int wToCol, int nSet) = 0;
	virtual bool SetTableRowKeep(int nTableID, int wRow, int wCol, bool bSet=true) = 0;
	virtual bool SetTableClear(const xtstring& id, const xtstring& sName, const int nModus) = 0;
  virtual bool SetCellRotate(int nTableID, int wRow, int wCol, int nDirection) = 0;
  virtual bool Layer(const xtstring& sName, const xtstring& sGroup, const xtstring& sParent, const int nIntent,
    const int nDefState, const int nLocked, const int nVisible, const int nPrint, const int nExport) = 0;
  virtual bool LayerEnd() = 0;
  virtual bool Layer_CS(const xtstring& sName, const xtstring& sSwatch, const xtstring sCpyFrom, const int& nShow, 
		const int& nGuide, const int& nLock, const int& nActive, const int& nDelete, const int& nMove) = 0;

  virtual int GetTableID() = 0;
  virtual bool SetTableID(int nTableID) = 0;
  virtual bool SetTableRow(int nTableRow) = 0;
  virtual bool SetTableCol(int nTableCol) = 0;
  virtual int GetTableRow() = 0;
  virtual int GetTableCol() = 0;
  virtual bool GetRowAndCols(int nTableID, int& wRows, int& wCols) = 0;
  virtual bool InsertTableRow(int nTableID, int nInsert, int wLines/*=1*/) = 0;
  virtual bool SetTableCellActivate(int nTableID, int wRow, int wCol, int nPos/*=TAG_POS_END*/) = 0;
	virtual bool SetRowHeight(int nTableID, int wRow, int wCol, double fRowHeight, long nPos/*=-1*/) = 0;
	virtual bool SetCellWidth(int nTableID, int wRow, int wCol, double fWidth, long nPos/*=-1*/) = 0;
  virtual bool SetCellBorderColor(int nTableID, int wRow, int wCol, _ColorDef cClrfLeft,
                                 _ColorDef cClrfTop, _ColorDef cClrfRight, _ColorDef cClrfButtom) = 0;
  virtual bool CellVerticalAlign(int nTableID, int wRow, int wCol, unsigned int nAlign) = 0;
  virtual bool CellMargin(int nTableID, int wRow, int wCol, int nLeft, int nTop, int nRight, int nButton) = 0;
  virtual bool DeleteColumn(int nTableID, int wRow, int wCol) = 0;
  virtual bool InsertColumn(int nTableID, int wRow, int wCol) = 0;
  virtual bool SplitColumn(int nTableID, int wRow, int wCol) = 0;
  virtual bool MergeColumns(int nTableID, int wFromRow, int wFromCol, int wToRow, int wToCol) = 0;
  virtual bool SetRowPosition(int nTableID, int wRow, int indent) = 0;
	virtual bool SetTableBackgroundPicture(int nTableID, int wRow, int wCol, const xtstring& cFile) = 0;
  virtual bool ClearParagraphEndofDocument() = 0;
  virtual bool RemoveEmptyPages() = 0;
  virtual bool GetDocumentStrings(CxtstringVector& vStringArray) = 0;

  // Style functions
  virtual void StyleEvent(TStyleEvent nStyleEvent, CXPubParagraphStyle* pStyle){};
  virtual bool CreateDefinedStyle(CXPubParagraphStyle* pStyle){return true;};

  void SetBridgeToStyleManager(CBridgeToStyleManager* pBridgeToStyleManager){m_pBridgeToStyleManager = pBridgeToStyleManager;};

protected:
  virtual CConvertWorkerTo* Clone() = 0;

  bool AttachConvertFrom(CConvertWorkerFrom* pConvertFrom);
  CConvertWorkerFrom* DetachConvertFrom();

  TDataFormatForStoneAndTemplate m_nDataFormat;
  CConvertWorkerFrom* m_pConvertFrom;
	HWND m_hWndParent;

  // Style Members
  CBridgeToStyleManager*  m_pBridgeToStyleManager;
//  CXPubParagraphStyle*    m_pDefaultStyle;
//  CXPubParagraphStyle*    m_pActualStyle;
//  xtstring                m_sSelectedStyleName;
};



#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_CONVERTWORKER_H_)

