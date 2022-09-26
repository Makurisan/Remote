#ifndef __PDFCELL_H__
#define __PDFCELL_H__

#include "XPubPDFExpImp.h"
#include "PDFTextBox.h"

class CPDFCell;
class CPDFTable;

#define   PDF_CELL_BORDER_DEF_WIDTH     10 //twips

struct XPUBPDF_EXPORTIMPORT cell
{
  cell(){m_lRow = -1; m_lCol = -1;};
  cell(long lRow, long lCol){m_lRow = lRow; m_lCol = lCol;};
  void operator=(const cell& i_Cell);
  bool operator<(const cell& i_Cell) const;
  bool operator==(const cell& i_Cell) const;
  bool operator!=(const cell& i_Cell) const;
  bool operator()(const cell& i_Cell1, const cell& i_Cell2) const;

  long m_lCol;
  long m_lRow;
};

#define LEFT_BORDER                         0
#define RIGHT_BORDER                        1
#define TOP_BORDER                          2
#define BOTTOM_BORDER                       3
#define LEFT_DIAG_BORDER                    4
#define RIGHT_DIAG_BORDER                   5
#define BORDER_COUNT                        6

class XPUBPDF_EXPORTIMPORT CPDFCell
{
public:
  CPDFCell(CPDFTable *i_pTable);
  virtual ~CPDFCell();

  typedef enum
  {
    eLeftBorder           = 0x01,
    eRightBorder          = 0x02,
    eTopBorder            = 0x04,
    eBottomBorder         = 0x08,

    eLeftDiagonalBorder   = 0x10,
    eRightDiagonalBorder  = 0x20,

    eLeftDoubleBorder     = 0x100,
    eRightDoubleBorder    = 0x200,
    eTopDoubleBorder      = 0x400,
    eBottomDoubleBorder   = 0x800,

    eNoBorder             = 0x00,

    eRecBorder            = eLeftBorder | eRightBorder | eTopBorder | eBottomBorder,
    eRecDoubleBorder      = eLeftDoubleBorder | eRightDoubleBorder | eTopDoubleBorder | eBottomDoubleBorder,
  } EBORDERTYPE;

  virtual void ChangeLeftIndents(long i_lWidth);
  virtual void ChangeRightIndents(long i_lWidth);
  virtual void ChangeTopIndents(long i_lWidth);
  virtual void ChangeBottomIndents(long i_lWidth);
  virtual void SetBorderWidth(long i_lBorder, long i_lWidth);
  virtual bool_a CopyAttributes(CPDFCell *i_pCell);
  long GetBorderWidth(EBORDERTYPE i_eBorder);
  long GetBorderIndents(EBORDERTYPE i_eBorder);
  virtual void SetBorderType(long i_lBorder);
  long GetBorderType();
  void SetBorderColor(long i_lBorder, float i_fR, float i_fG, float i_fB);
  void SetBorderColor(long i_lBorder, CPDFColor& i_rColor);
  CPDFColor *CPDFCell::GetBorderColor(EBORDERTYPE i_eBorder);
  cell GetCellPos();

  long GetOverRowCount();
  bool_a IsOverRowCell();
  void IncOverRowCount();
  void DecOverRowCount();
  virtual rec_a GetPosSize();
  long GetWidth();
  virtual bool_a IsTable() = 0;
  virtual bool_a IsFlush() = 0;

protected:
  void CopyAttr(CPDFCell *i_pCell);
  void SetCellPos(cell i_Cell);
  void SetCellPos(long i_lRow, long i_lCol);
  void SetOverRowCount(long i_lCount);
  virtual bool_a SetWidth(long i_lWidth);
  virtual bool_a SetHeight(long i_lHeight);
  virtual bool_a SetDrawHeight(long i_lHeight);
  virtual bool_a MoveTo(long i_lX, long i_lY);
  virtual bool_a ChangeFontID(std::map <long, long> *i_pFontMap) = 0;
  virtual bool_a ChangeImageID(std::map <long, long> *i_pImageMap) = 0;
  virtual void SetDocument(CPDFDocument *i_pDoc) = 0;
  virtual void SetPage(long i_lPage) = 0;
  virtual void SetTable(CPDFTable *i_pTable) {m_pTable = i_pTable;};
  CPDFTable *GetTable(){return m_pTable;};
  virtual bool_a Flush() = 0;
  virtual bool_a FlushBorder() = 0;
//  virtual CPDFCell *Clone() = 0;

  void GetHalfBorder(long i_lBorder, long i_lWidth, long &o_lHi, long &o_lLow);
  float GetHalfBorderForDraw(long i_lBorder, long i_lWidth);

  friend class CPDFTable;

  CPDFTable     *m_pTable;

private:
  cell          m_Cell;
  long          m_lOverRowCount;
  long          m_lBorderType;
  long          m_lBorderWidth[BORDER_COUNT];
  long          m_lBorderIndents[LEFT_DIAG_BORDER];
  CPDFColor     m_BorderColor[BORDER_COUNT]; //TODO: move it to textbox
  rec_a         m_recPosSize;
};





class XPUBPDF_EXPORTIMPORT CPDFTableCell :public CPDFTextBox, public CPDFCell
{
public:
  CPDFTableCell(CPDFDocument *i_pDoc, CPDFTable *i_pTable, rec_a i_recPosSize, long i_lRow = -1,
            long i_lCol = -1, long i_lPage = -1);
  ~CPDFTableCell();

  virtual void ChangeLeftIndents(long i_lWidth);
  virtual void ChangeRightIndents(long i_lWidth);
  virtual void ChangeTopIndents(long i_lWidth);
  virtual void ChangeBottomIndents(long i_lWidth);
  virtual void SetBorderWidth(long i_lBorder, long i_lWidth);
  virtual bool_a CopyAttributes(CPDFCell *i_pCell);
  virtual void SetBorderType(long i_lBorder);

  virtual rec_a GetPosSize();
  long GetWidth();
  virtual bool_a IsTable(){ return false_a; };
  virtual bool_a IsFlush();
  CPDFTableCell *Clone();

  bool_a CheckResize();
  bool_a FontSizeChanged();
  bool_a CanBeHeightChanged(long &i_lDif);


  bool_a m_bChangeSize;


protected:
  friend class CPDFTable;

  virtual bool_a SetWidth(long i_lWidth);
  virtual bool_a SetHeight(long i_lHeight);
  virtual bool_a SetDrawHeight(long i_lHeight);
  virtual bool_a MoveTo(long i_lX, long i_lY);
  virtual void SetDocument(CPDFDocument *i_pDoc);
  virtual void SetPage(long i_lPage);
  virtual bool_a ChangeFontID(std::map <long, long> *i_pFontMap);
  virtual bool_a ChangeImageID(std::map <long, long> *i_pImageMap);
  virtual bool_a Flush();
  virtual bool_a FlushBorder();
  void CopyAttr(CPDFTableCell *i_pTCell);



};

#endif  /*  __PDFCELL_H__  */
