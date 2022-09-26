#ifndef __PDFTEXTBOX_H__
#define __PDFTEXTBOX_H__

#include "XPubPDFExpImp.h"
#include "pdfdef.h"

#include <map>

class CPDFTable;
class CPDFFrame;
class CPDFDocument;

class XPUBPDF_EXPORTIMPORT CPDFTextBox
{
public:
  //deprecated
  CPDFTextBox();//don't use it!!!
  void TBCpy(CPDFTextBox& tb);//don't use it!!!




  CPDFTextBox(CPDFDocument *i_pDoc, rec_a i_recPosSize,
              long i_lPage = -1, bool_a i_bPage = false_a, CPDFTextBox *i_pParent = NULL);
  CPDFTextBox(CPDFDocument *i_pDoc, long i_lWidth, long i_lHeight,
              long i_lPage = -1, bool_a i_bPage = false_a, CPDFTextBox *i_pParent = NULL);
  virtual ~CPDFTextBox();

  CPDFTextBox *Clone();
  CPDFTextBox *CreateIdentical();

  long GetElemListWidth(PDFParagraph *i_pParagraph, PDFLineElemList *i_pElem);


  virtual bool_a MoveTo(long i_lX, long i_lY);
  virtual bool_a SetHeight(long i_lHeight);
  virtual bool_a SetDrawHeight(long i_lHeight);
  long GetHeight();
  long GetDrawHeight(bool_a i_bDirection = false_a);
  long GetDividedHeight(long i_lHeight);
  virtual bool_a SetWidth(long i_lWidth);
  virtual bool_a SetDrawWidth(long i_lWidth);
  long GetWidth();
  long GetDrawWidth(bool_a i_bDirection = false_a);

  long GetDrawWidthInd(PDFParagraph *i_pParagraph, bool_a i_bDirection = false_a);
  long GetRightDrawPosInd(PDFParagraph *i_pParagraph, bool_a i_bDirection = false_a);
  long GetLeftDrawPosInd(PDFParagraph *i_pParagraph, bool_a i_bDirection = false_a);

  bool_a Join(CPDFTextBox *i_pParent);
  CPDFTextBox *UnJoin();
  virtual bool_a AddFillBreak(long i_lBreakCount);
  virtual bool_a AddAnchor(std::string i_strAnchor);
  bool_a ReplaceBookmarks();
  virtual bool_a AddPageMark(ePAGEMARKTYPE i_eType);
  bool_a ReplacePageMarks();



  bool_a BeginLink(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                  long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                  const char *i_pLocation);

  bool_a BeginLink(const char *i_pczName, const char *i_pLocation);

  bool_a BeginLink(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                  long i_lTop, long i_lRight, long i_lBottom, float i_fZoom);

  bool_a BeginLink(const char *i_pczName);

  bool_a ChangeLinkDest(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                        long i_lTop, long i_lRight, long i_lBottom, float i_fZoom);

  bool_a EndLink();



  virtual long AddText(char *i_pText);
  virtual long AddText(wchar_t *i_pText);
  virtual bool_a AddImage(char *i_pImageName, long i_lX, long i_lY, long i_lWidth, long i_lHeight,
                          long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                          bool_a i_bAddToLine);
  virtual bool_a AddScaledImage(char *i_pImageName, long i_lX, long i_lY, long i_lScaleX, long i_lScaleY,
                                long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                                bool_a i_bAddToLine);
  bool_a IsTableLast(CPDFTable *i_pTable = NULL);
  bool_a IsTableFirst(CPDFTable *i_pTable);
  bool_a IsPage(){return m_bPage;};
  virtual bool_a AddEmptySpace(long i_lHeight);
  virtual bool_a AddFrame(CPDFFrame *i_pFrame, bool_a i_bUseSize = false_a);
  virtual bool_a AddTable(CPDFTable *i_pTable);
  virtual bool_a AddTable(CPDFTable *i_pTable, long i_lYPos, long i_lHeight);
  virtual bool_a RemoveEmptyLineAfterTable();
  virtual bool_a UpdateTable(CPDFTable *i_pTable);
  virtual bool_a UpdateYPosSpaces();
  bool_a HasLastLineOnlyWhiteSigns();
  bool_a HasOnlyTable(CPDFTable *i_pTable);
  bool_a HasOnlyTable();

  virtual bool_a SetAlignment(short i_sAlignment);
  bool_a SetVerticalAlignment(short i_sVAlignment);
  bool_a GetSize(long &o_lWidth, long &o_lHeight);
  bool_a SetLineSpace(long i_lLineSpace);
  bool_a SetKeepLinesTogether(bool_a i_bKeepLinesTogether = true_a);
  bool_a SetKeepWithNext(bool_a i_bKeepWithNext = true_a);
  bool_a SetWidowOrphan(bool_a i_bWidowOrphon = true_a);
  bool_a SetParagraphPageBreak(bool_a i_bParagraphPageBreak = true_a);

  bool_a SetTextDirection(eTEXTDIRECTION i_eDirection);
  eTEXTDIRECTION GetTextDirection();

  virtual bool_a SetIndents(long i_lLeft, long i_lTop, long i_lRight, long i_lBottom, long i_lFirstIndent);
  virtual bool_a SetLeftRightIndents(long i_lLeft, long i_lRight, long i_lFirstIndent);
  virtual bool_a SetLeftRightIndents(long i_lLeft, long i_lRight);
  virtual bool_a SetTopBottomIndents(long i_lTop, long i_lBottom);

  virtual bool_a GetIndents(long &o_lLeft, long &o_lTop, long &o_lRight,
                            long &o_lBottom, long &o_lFirstIndent);

  virtual bool_a SetBorder(long i_lLeft, long i_lTop, long i_lRight, long i_lBottom);
  virtual bool_a SetBorder(float i_fLeft, float i_fTop, float i_fRight, float i_fBottom);
  virtual bool_a GetBorder(float &o_fLeft, float &o_fTop, float &o_fRight, float &o_fBottom);
  virtual bool_a GetBorder(long &o_lLeft, long &o_lTop, long &o_lRight, long &o_lBottom);

  bool_a SetMargins(long i_lLeft, long i_lTop, long i_lRight, long i_lBottom);
  bool_a GetMargins(long &o_lLeft, long &o_lTop, long &o_lRight, long &o_lBottom);
  long GetBottomMargin(){return this->recMargins.lBottom;};
  long GetLeftMargin(){return this->recMargins.lLeft;};
  long GetRightMargin(){return this->recMargins.lRight;};
  long GetTopMargin(){return this->recMargins.lTop;};


  bool_a SetTabs(PDFTabStop *i_pTabs, short i_sTabsCount);
  virtual bool_a InsertTab(PDFTabStop i_Tab);
  bool_a SetTabDefault(long i_lTabSize, char i_cTabDec);
  virtual bool_a InsertLine();
  bool_a HasTransparentBg();
  bool_a SetTransparentBg(bool_a i_bTransparent = true_a);
  CPDFColor& GetBgColor();
  bool_a SetBgColor(float i_fRed, float i_fGreen, float i_fBlue);
  bool_a SetBgImage(const char *i_pczImageName);
  bool_a ClearBgImage();
  CPDFColor& GetTextColor();
  virtual bool_a SetTextColor(float i_fRed, float i_fGreen, float i_fBlue);
  CPDFColor& GetTextBgColor();
  bool_a SetTextBgColor(float i_fRed, float i_fGreen, float i_fBlue);

  bool_a DeleteLastParagraph();

  virtual bool_a Flush();
  virtual bool_a IsFlush();
  bool_a IsEmpty();
  long GetLineCount();

  CPDFTextBox *GetNextTextBox();
  void SetNextTextBox(CPDFTextBox *i_pTextBox);
  CPDFTextBox *GetPrevTextBox();
  void SetPrevTextBox(CPDFTextBox *i_pTextBox);

  bool_a ChangeImageID(std::map <long, long> *i_pImageMap);
  bool_a ChangeFontID(std::map <long, long> *i_pFontMap);

  void SetCanPageBreak();
  void SetCanPageBreak(bool_a i_bDelete);
  bool_a HasPageBreak();
  bool_a HasBreak();
  bool_a HasCanPageBreak();
  bool_a InsertBreak();
  bool_a InsertColumnBreak();
  bool_a InsertBreak(CPDFTextBox *i_pTextBox);
  virtual bool_a CopyOverflowLines();
  virtual bool_a CopyOverflowLines(CPDFTextBox *i_pTextBox, bool_a i_bCheckBreak = false_a, long i_lHeight = NOT_USED);
  bool_a Overlap(CPDFTextBox *i_pTextBox);
  bool_a Overflow();
  long GetPageNumber();
  rec_a GetPosSize();
  bool_a SetPosSize(rec_a i_recPosSize);
  long GetTopDrawPos(bool_a i_bDirection = false_a);
  long GetBottomDrawPos(bool_a i_bDirection = false_a);
  long GetLeftDrawPos(bool_a i_bDirection = false_a);
  long GetRightDrawPos(bool_a i_bDirection = false_a);
  rec_a GetDrawPosSize();
  void GetActualPos(long &i_lLeft, long &i_lTop);
  virtual bool_a CheckResize();
  virtual bool_a FontSizeChanged();
  bool_a IsTableOverflow();
  bool_a CanDivide();
  bool_a CopyAttributtes(CPDFTextBox *i_pTextBox);

typedef enum
{
  eTBTextBox        = 0x01,
  eTBTableCell      = 0x02,
  eTBFrame          = 0x04,
  eTBHeaderFooter   = 0x08,
} eTBType;

  eTBType GetTBType(){return m_eTBType;};

  void SetDocument(CPDFDocument *i_pDoc);
  CPDFDocument *GetDocument(){return m_pDoc;};
  void SetPage(long i_lPage);

  void MakeDirection(rec_a &rec);

protected:
  void CopyAttr(CPDFTextBox *i_pTextBoxTo);
  bool_a SetImages(PDFTBImage *i_pTBImages);
  bool_a SetSpaces(PDFTBSpace *i_pTBSpaces);
  bool_a SetParagraph(PDFParagraph *i_pParagraph);
  bool_a SetParagraphLines(PDFParagraph *i_pParagraph, PDFLine *i_pLine);
  bool_a SetLineElements(PDFLine *i_pLine, PDFLineElemList *i_pElement);

  bool_a FlushElements();
  bool_a FlushBackground();
  bool_a FlushImages();
  long CalcAlignment();
  void MakeDirection();

  bool_a ShowBorder();
  bool_a ShowParagraph(PDFParagraph *i_pParagraph, long i_lY);
  bool_a ShowTab(PDFParagraph *i_pParagraph, long i_lY);

  PDFDocPTR GetBaseDoc();

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
public://TODO: finish moving ...

  bool_a        m_bDelete;


  CPDFColor     m_Color;
  CPDFColor     m_BgColor;
  CPDFColor     m_TBBgColor;
  bool_a        bTransparent;
  long          lTBBgImage;
  rec_a         recMargins;
  rec_a         recIndents;
  long          lIndent;
  frec_a        recBorder;
  short         sAlignment;
  short         sVAlignment;
  eTEXTDIRECTION m_eTextDirection;
  long          lPage;
  rec_a         recTB;
  long          lLineSpace;
  long          lLetterSpace;
  bool_a        bTBCanPageBreak;
  bool_a        bTBBreak;
  bool_a        bColumnBreak;
  bool_a        bPageMark;
  bool_a        bTBCopy;
  long          lTBWidth;
  long          lTBHeight;
  long          lTabSize;
  char          cTabDecimalType;
  PDFTabStop    *pTBTabStops;
  short         sTabsCount;
  PDFTBImage    *pTBImages;
  PDFTBSpace    *pTBSpaces;
  PDFParagraph  *pParagraph;
  PDFParagraph  *pActiveParagraph;
  CPDFTextBox   *pTBNext;
  CPDFTextBox   *pTBPrev;
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


protected:

  long          m_lLastID;
  long          m_lLastAnnot;
  bool_a        m_bFlush;
  bool_a        m_bPage;
  CPDFDocument  *m_pDoc;
  eTBType       m_eTBType;
};


#endif  /*  __PDFTEXTBOX_H__  */
