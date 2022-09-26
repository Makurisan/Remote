#if !defined(_CONVERTWORKERTPDF_H_)
#define _CONVERTWORKERTPDF_H_

#define ConvertWorkerTPdf_Version   0

#include "CnvTPdfExpImp.h"


#define DEF_LEFT_DIST             0//points
#define DEF_RIGHT_DIST            0//points
#define DEF_TOP_DIST              0//points
#define DEF_BOTTOM_DIST           0//points

class CConvertWorkerToPdf;


class CConvertToPdfAddOn : public CConvertToAddOn
{
  friend class CConvertWorkerToPdf;
public:
  CConvertToPdfAddOn(CXPubAddOnManager* pAddOnManager);
  ~CConvertToPdfAddOn();

  virtual CConvertWorkerTo* CreateConvertWorkerTo(HWND hWndParen);
  virtual CConvertWorkerTo* CreateConvertWorkerToClone(CConvertWorkerTo* pConvertWorker);
  virtual bool ReleaseConvertWorkerTo(CConvertWorkerTo* pConvertWorker);
};


#include <vector>

typedef std::vector<CTableStackItem>         TableStackVector;


#include "PDFManager.h"

class CPDFDocument;
class CPDFTextBox;

class CConvertWorkerToPdf : public CConvertWorkerTo
{
  friend class CConvertToPdfAddOn;
public:
  CConvertWorkerToPdf(CConvertToPdfAddOn* pAddOn, HWND hWndParent);
  ~CConvertWorkerToPdf();

  //if sDoc is not set then use current document
  bool PDFSetDocInfo(const xtstring& sDoc, eDOCINFO eType, const xtstring& sText);

  // from CConvertWorkerTo
  virtual bool IsDataFormatAvailable(TDataFormatForStoneAndTemplate nOutputDataFormat);
  virtual xtstring GetDataStream(TDataFormatForStoneAndTemplate nOutputDataFormat);
  virtual bool GetDataStream(TDataFormatForStoneAndTemplate nOutputDataFormat, char *pOutput, int &nSize);
  virtual bool SaveDataStream(const xtstring& sfilename){ return false;};
  virtual bool PositionCursor(const xtstring& sXPubConst, TWorkPaperCopyType eWorkPaperCopyType);
  virtual bool PrintDataStream(const xtstring& sfilename, bool bPrintOrSelect){ return false;};

public:
  // from CConvertWorkerTo
  virtual TTypeOfFormater GetFormaterType(){ return PDF_Formater;};
  virtual bool SetVersion(const int nVersion){ return false;};
  virtual bool SetMetric(const int nType){ return false;};
virtual bool Spread(const int nFrom, const int nTo, const int nType, const int nCount){ return false;};
virtual bool Module(const xtstring& id, const xtstring& sFunktion, const xtstring& sParameter, const xtstring& sLayer){ return false;};
virtual bool Asset(const xtstring& sID, const xtstring& sName, const xtstring& sFileName){ return false;};
virtual bool Page(const xtstring& sJoin, const xtstring& sAsset, const xtstring& sLib, const xtstring& sOverset,
const xtstring& sRepeat, const int nFrom, const int nTo, const int nType){ return false;};
  virtual bool SetDocumentInfo(const TDocumentProperty nTypeOfProperty, const xtstring& sValue);
  virtual bool SetPassword(const xtstring& sOwner, const xtstring& sUser, int nPermission);
  virtual bool SetFontEmbedded(bool i_bEmbed = true);
  virtual bool InsertTextFromMemory(const xtstring& sInput, int nFormat/* = CNV_TEXTFORMAT_TEXT*/);
  virtual bool InsertWideTextFromMemory(const xwstring& sInput, int nFormat/* = CNV_TEXTFORMAT_TEXT*/);
  virtual bool FillPageBreak();
  virtual bool BeginPageBody(const int nPos);
  virtual bool InsertParagraph();
  virtual bool SetDefaults();
  virtual bool InsertLine();
  virtual bool SetParagraphSpacing(int nSpaceBefore, int nSpaceAfter, int nSpaceBetween, int nLineSpacing);
  virtual bool SetParagraphIndents(int nLinks, int nRechts, int nFirstIndent);
  virtual bool SetParagraphControl(const int nKeep, const int nKeepNext, const int nWidowOrphon);
  virtual bool InsertTabulator(int nTabFlag, int nTabPos, const xtstring& sTabDecChar, const xtstring& sTabLeader);
  bool ClearTabulators();
  virtual bool SetDefTabulator(int nNewWidth, int nDecDel);
  virtual bool SetFont(const xtstring& sFontName, int nFontSize, int nCodePage = -1);
  virtual bool SetFontName(const xtstring& sFontName);
  virtual bool SetFontNameDefault(const xtstring& sFontName);
  virtual int  GetFontSizeDefault(){ return m_nFontSize;};
  virtual xtstring& GetFontNameDefault(){ return m_sFontName;};
  virtual bool SetCodePage(const xtstring& sCodePage);
  virtual bool SetCodePageDefault(const xtstring& sCodePage);
  virtual xtstring GetCodePageDefault(){ return m_sCodePage;};
  virtual bool SetFontSize(int nFontSize);
  virtual bool SetFontSizeDefault(int nFontSize);
  virtual bool InsertTab();
	virtual bool Inline(const xtstring& id, const xtstring& sName, const xtstring& cPath,  float nWidht, float nHeight,
		const xtstring& native, const double dXScale=0.0, const double dYScale=0.0, const int type=0, int nFitting=0){return true;};
	virtual bool Import(const xtstring& id, const xtstring& sName, const xtstring& cPath,  float nWidht, float nHeight,
		const xtstring& native, const int type=0, int nFitting=0){return true;};
  virtual bool InsertImage(const xtstring& id, const xtstring& sName, const xtstring& cPath,  float nWidht, float nHeight,
    int nZoom, bool bLinked=true,  bool bShrink=false, int nFitting=0);
  virtual bool SetFontAttribute(int nAttrib);
  virtual bool SetTextColor(_ColorDef color);
  virtual bool SetHeaderFooterPos(int nCase, int nPosition);
  virtual bool SwitchHeaderFooter(int nCase, int nPos/*=CURSOR_END*/);
  virtual bool ToggleEditHdrFtr();
  virtual bool TextAlignment(int nAlign);
  virtual bool SetCursorPosition(int nCase, int nSection /*= 0*/);
  virtual bool InsertTextFrameColumns(const xtstring& sName, const int nColumns, const double nWidth, const double nGutter);
	virtual int  CreateRectangle(const xtstring& id, const xtstring& sName, int nLeft, int nTop, int nWidth, int nHeight,
		int LineThickness, const xtstring& LineColor, const xtstring& FillColor){return false;};
  virtual int CreateTextFrame(const xtstring& id, const xtstring& sName, const xtstring& sJoin, const xtstring& sLibrary, int nLeft, int nTop, int nWidth, int nHeight, int nVertAlign, int nHorzAlign, int nType,
    int LineThickness=-1, _ColorDef LineColor=(COLORDEF)-1, _ColorDef FillColor=(COLORDEF)-1, int nWrap=TAG_TEXTFRAME_WRAP_C_NOTEXT, int nTextDirection=0);
  virtual bool CheckTextFrameOverset(const xtstring& sName);
  virtual bool Line(int nX1, int nY1, int nX2, int nY2, int nWidth, _ColorDef LineColor);
  virtual bool Margins(int nLeft, int nRight, int Top, int Button);
  virtual bool SetSectionOrientation(int nOrient/*=SECTION_PORTRAIT*/);
  virtual bool SetPaperSize(int nDmConstant, int nWidth/*=0*/, int nLength/*=0*/);
  virtual bool DocumentInfo(const xtstring& cTag);
  virtual bool MergeField(const xtstring& cTag);
  virtual bool Field(const xtstring& cTag);
  virtual bool MoveFormField(const xtstring& cName, long i_lDeltaX, long i_lDeltaY);
  virtual bool FormField(const xtstring& sName, const xtstring& sValue, const xtstring& sType,
    const xtstring& sMask, const int nLength, const xtstring& sFontName, const int nFontSize, const int nCodePage=0);
  virtual bool CheckFormFieldOverset(const xtstring& cName, const xtstring& cValue);
  virtual bool GetFormFieldValue(const xtstring& cName, xtstring& cValue);
  virtual bool SetPageCount(int nCount);
  virtual bool SetPageNumberFormat(int nFormat){return false;};
  virtual bool SetCurrentPage(int nCount);
  virtual bool InsertPageBreak();
  virtual bool ParagraphPageBreak(const int nInsert);
  virtual bool InsertPosFrameBreak();
  virtual bool InsertSectionBreak();
  virtual bool InsertColumnBreak();
  virtual bool KeepParTogether(bool bBegin){return false;};
  virtual bool CanPageBreak(bool bDelete);
  virtual bool CheckCanPageBreak();
  virtual bool EditSection(int NumCols, int ColSpace, int FirstPageNo);
  virtual bool InsertBookmark(xtstring& sLevel, xtstring& sText, xtstring& sSymbol, int nZoom, bool bIgnorePos, int nColor, int nStyle);
  virtual bool InsertLink(const xtstring& sReferenzName, const xtstring& sSymbolName, int nPage, int nZoom);
  virtual bool InsertAnchor(const xtstring& sAnchorName);
  virtual bool LoadTemplate(const xtstring& cFileName, const xtstring& sLayer, const xtstring& sOwnerPwd, int nModus);
  virtual bool Layer(const xtstring& sName, const xtstring& sGroup, const xtstring& sParent, const int nIntent,
    const int nDefState, const int nLocked, const int nVisible, const int nPrint, const int nExport);
  virtual bool LayerEnd();
  virtual bool Layer_CS(const xtstring& sName, const xtstring& sSwatch, const xtstring sCpyFrom, const int& nShow, 
		const int& nGuide, const int& nLock, const int& nActive, const int& nDelete, const int& nMove){ return false;};

  virtual int CreateTable(const xtstring& id, const xtstring& sName, int wRows, int wColumns,
								int numHeaderRows, int numFooterRows, double spacebefore, double spaceafter);
  virtual bool CellBackgroundColor(int nTableID, int wRow, int wCol, _ColorDef crBkColor);
  virtual bool SetCellBorderWidth(int nTableID, int wRow, int wCol, int nLeft, int nTop, int nRight, int nButton);
  virtual bool SetCursorAfterTable(int nTableID);
  virtual bool SetTableHeaderRow(int nTableID, int wFromRow, int wToCol, int nSet);
  virtual bool SetTableRowKeep(int nTableID, int wRow, int wCol, bool bSet=true);
  virtual bool SetCellRotate(int nTableID, int wRow, int wCol, int nDirection);
	virtual bool SetTableClear(const xtstring& id, const xtstring& sName, const int nModus){ return false;};

  virtual int GetTableID();
  virtual bool SetTableID(int nTableID);
  virtual bool SetTableRow(int nTableRow);
  virtual bool SetTableCol(int nTableCol);
  virtual int GetTableRow();
  virtual int GetTableCol();

  virtual bool GetRowAndCols(int nTableID, int& wRows, int& wCols);
  virtual bool InsertTableRow(int nTableID, int nInsert, int wLines/*=1*/);
  virtual bool SetTableCellActivate(int nTableID, int wRow, int wCol, int nPos/*=TAG_POS_END*/);
  virtual bool SetRowHeight(int nTableID, int wRow, int wCol, double fRowHeight, long nPos/*=-1*/);
  virtual bool SetCellWidth(int nTableID, int wRow, int wCol, double fWidth, long nPos/*=-1*/);
  virtual bool SetCellBorderColor(int nTableID, int wRow, int wCol, _ColorDef cClrfLeft,
    _ColorDef cClrfTop, _ColorDef cClrfRight, _ColorDef cClrfButtom);
  virtual bool CellVerticalAlign(int nTableID, int wRow, int wCol, unsigned int nAlign);
  virtual bool CellMargin(int nTableID, int wRow, int wCol, int nLeft, int nTop, int nRight, int nButton);
  virtual bool DeleteColumn(int nTableID, int wRow, int wCol);
  virtual bool SplitColumn(int nTableID, int wRow, int wCol){ return false;};
	virtual bool MergeColumns(int nTableID, int wFromRow, int wFromCol, int wToRow, int wToCol){return false;};
  virtual bool InsertColumn(int nTableID, int wRow, int wCol);
  virtual bool SetRowPosition(int nTableID, int wRow, int indent);
  virtual bool SetTableBackgroundPicture(int nTableID, int wRow, int wCol, const xtstring& cFile);
  virtual bool ClearParagraphEndofDocument(){ return false;};
  virtual bool RemoveEmptyPages(){ return false;};
  virtual bool GetDocumentStrings(CxtstringVector& vStringArray);

  // Style functions
  virtual void StyleEvent(TStyleEvent nStyleEvent, CXPubParagraphStyle* pStyle);
  virtual bool CreateDefinedStyle(CXPubParagraphStyle* pStyle);

protected:
  // f�r Tabellensteuerung
  int _GET_ROW(int x){if (x == -1) return m_nTableRow; return x;};
  int _GET_COL(int x){if (x == -1) return m_nTableCol; return x;};
  void GetCellRange(CPDFTable *i_pTable, long i_lRow, long i_lCol, long &o_lBgnRow,
                    long &o_lEndRow, long &o_lBgnCol, long &o_lEndCol);
  long GetTable();
  long GetRow();
  long GetCol();
  CPDFTable *GetNestTable();
  long GetNestRow();
  long GetNestCol();

protected:

  TableStackVector  m_TableStack;

  int   m_nTableID;
  int   m_nTableRow;
  int   m_nTableCol;
  int   m_nTableNestRow;
  int   m_nTableNestCol;
  //bool  m_bInTable;
  int   m_nFrameID;//xtstring  m_sInFrame;
  int   m_nActiveTableID;
  int   m_nActiveRow;
  int   m_nActiveCol;
  int   m_nActiveNestTableID;
  int   m_nActiveNestTableRowPos;
  int   m_nActiveNestTableColPos;
  int   m_nActiveNestRow;
  int   m_nActiveNestCol;

  xtstring  m_sCodePage;
  xtstring  m_sFontName;
  int       m_nFontSize;

  int m_nLastStyleID;

  enum eActiveCtrlType
  {
    eNoCtrl           = -1,
    ePageTextAreaCtrl = 0,
    ePageHeaderCtrl   = 1,
    ePageFooterCtrl   = 2,
    eFrameCtrl        = 3,
    eTableCtrl        = 4,
//    eNestTableCtrl    = 5
  };
  eActiveCtrlType  m_eActiveCtrlType[3];
  void SetActiveCtrlType(eActiveCtrlType i_eActiveCtrlType);
  eActiveCtrlType GetActiveCtrlType();
  CPDFDocument *GetActiveDocumet();
  CPDFTextBox *GetActiveCtrl();

protected:

  CPDFManager   m_cPDFMan;
  CPDFDocument  *m_pCurDoc;
  CPDFTextBox   *m_pCurTextBox;

protected:
  virtual CConvertWorkerTo* Clone();

  CConvertToPdfAddOn* m_pAddOn;
};

#endif // !defined(_CONVERTWORKERTPDF_H_)
