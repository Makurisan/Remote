#ifndef __PDFPAGE_H__
#define __PDFPAGE_H__

#include "XPubPDFExpImp.h"
#include "pdfdef.h"
#include "PDFTextBox.h"

#define MIN_PAGE_COLUMN_WIDTH     30 //in mm
#define MIN_PAGE_COLUMN_SPACE     5 //in mm

class CPDFDocument;
class CPDFPage;

class CPDFHeaderFooter :public CPDFTextBox
{
public:
  ~CPDFHeaderFooter();

  typedef enum
  {
    eHeader = 0,
    eFooter
  } EHeaderFooterType;

  virtual long AddText(char *i_pText);
  virtual long AddText(wchar_t *i_pText);
  virtual bool_a AddImage(char *i_pImageName, long i_lX, long i_lY, long i_lWidth, long i_lHeight,
                          long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                          bool_a i_bAddToLine);
  virtual bool_a AddScaledImage(char *i_pImageName, long i_lX, long i_lY, long i_lScaledX, long i_lScaledY,
                                long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                                bool_a i_bAddToLine);
  virtual bool_a InsertLine();
  virtual bool_a InsertTab(PDFTabStop i_Tab);
  virtual bool_a SetTextColor(float i_fRed, float i_fGreen, float i_fBlue);
  virtual bool_a SetAlignment(short i_sAlignment);
  virtual bool_a SetLeftRightIndents(long i_lLeft, long i_lRight, long i_lFirstIndent);
  virtual bool_a AddPageMark(ePAGEMARKTYPE i_eType);
  virtual bool_a CheckResize();
  virtual bool_a SetHeight(long i_lHeight);
  virtual bool_a SetDrawHeight(long i_lHeight);

  CPDFHeaderFooter *Clone();
  EHeaderFooterType GetHdrFtrType() {return m_eHdrFtrType;};

protected:
  void CopyAttr(CPDFHeaderFooter *i_pHF);

  friend class CPDFPage;

  CPDFHeaderFooter(EHeaderFooterType i_eHdrFtrType, CPDFDocument *i_pDoc, rec_a i_recPosSize, long i_lPage = -1);

  EHeaderFooterType   m_eHdrFtrType;
};


class XPUBPDF_EXPORTIMPORT CPDFPage :private PDFPage
{
public:
  ~CPDFPage();

  typedef enum
  {
    eDisplayPortrait         = 0,
    eDisplayLandScape        = 270,
    eDisplayRotatePortrait   = 180,
    eDisplayRotateLandScape  = 90
  } EPageType;

  typedef enum
  {
    eTextArea         = 0,
    eHeader           = 1,
    eFooter           = 2
  } EActiveControl;

  rec_a GetMediaBox();
  rec_a GetCropBox();
  bool_a CheckResize();
  long GetWidth(); //in points
  long GetHeight(); //in points
  EPageType GetRotation();
  long GetImagesCount();
  long GetFontsCount();
  long GetAnnotsCount();
  long GetFontID(long i_lIndex);
  long GetImageID(long i_lIndex);
  long GetAnnotID(long i_lIndex);
  PDFStream *GetPageStream();
  bool_a CopyPageStream(PDFStream *i_pBuffer);
  long GetPageNumber();
  bool_a SetSize(long i_ldmPaperType, long i_lWidth, long i_lHeight);
  bool_a SetOrientation(long i_lOrient);
  bool_a SetMargins(long i_lLeft, long i_lRight, long i_lTop, long i_lBottom);
  bool_a SetHeaderPos(long i_lPos);
  bool_a SetFooterPos(long i_lPos);
  long GetMinHeaderBottom();
  long GetMaxFooterTop();

  bool_a SetColumns(long i_lColCount, long i_lColSpace);
  CPDFTextBox *CheckColumnOverflow();
  long GetColCount() {return m_lColCount;};

  bool_a ActivateControl(EActiveControl i_eControl);
  EActiveControl GetActiveControlType();
  CPDFTextBox *GetActiveControl();

  CPDFTextBox *GetLastColumn();
  CPDFTextBox *GetTextArea();
  bool_a HasHeader();
  CPDFHeaderFooter *GetHeader();
  bool_a HasFooter();
  CPDFHeaderFooter *GetFooter();

  bool_a Flush();
  bool_a IsFlush();

protected:
  friend class CPDFDocument;
  friend class CPDFLayer;
  friend class CPDFTemplate;
  
  CPDFPage(CPDFDocument *i_pDoc, long i_fWidth, long i_fHeight, long i_lPageNum, EPageType i_ePageType);
  bool_a CalcHeaderSize(rec_a &i_Rec);
  bool_a CalcFooterSize(rec_a &i_Rec);
  bool_a CalcTextAreaSize(rec_a &i_Rec);
  bool_a SetTextAreaSize(rec_a &i_Rec);
  const char *GetContent(long &o_lSize);
  void SetPageNumber(long i_lPage);

  PDFDocPTR GetBaseDoc();

private:

  bool_a            m_bFlush;
  CPDFHeaderFooter  *m_pHeader;
  CPDFTextBox       *m_pTextArea;
  long              m_lColCount;
  long              m_lColSpace;
  CPDFHeaderFooter  *m_pFooter;
  EActiveControl    m_eActCtrl;
  CPDFDocument      *m_pDoc;
};



#endif  /*  __PDFPAGE_H__  */
