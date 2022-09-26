#ifndef __PDFFRAME_H__
#define __PDFFRAME_H__

#include "XPubPDFExpImp.h"
#include "PDFTextBox.h"

#define ADDITION_BORDER         20
#define FRAME_MARGIN            0
#define MIN_FRAME_COL_WIDTH     500
#define MIN_FRAME_GUTTER        100

class XPUBPDF_EXPORTIMPORT CPDFFrame :public CPDFTextBox
{
public:

  typedef enum eYPosType
  {
     ePosPage = 0,
     ePosMarg = 1,
     ePosPara = 2
  };

  typedef enum eResizeType
  {
    eFullResize = 0,
    eGrowResize = 1,
    eNoResize = 2
  };

  typedef enum eOverlapType
  {
    eNoOverlap = 0,
    eAroundOverlap = 1
  };

  CPDFFrame(CPDFDocument *i_pDoc, rec_a i_recPosSize, const char *i_pID,
            long i_lPage = -1, eYPosType i_eType = ePosPage);
  ~CPDFFrame();
  virtual bool_a SetBorder(long i_lLeft, long i_lTop, long i_lRight, long i_lBottom);
  virtual bool_a GetBorder(float &o_fLeft, float &o_fTop, float &o_fRight, float &o_fBottom);
  virtual bool_a GetBorder(long &o_lLeft, long &o_lTop, long &o_lRight, long &o_lBottom);
  CPDFColor& GetBorderColor();
  virtual bool_a SetBorderColor(float i_fRed, float i_fGreen, float i_fBlue);
  virtual bool_a Flush();
  bool_a FlushBorder();
  bool_a SetFrameColumns(long i_lColCount, long i_lColSpace, long i_lColGutter);

  virtual bool_a MoveTo(long i_lX, long i_lY);
  virtual bool_a CopyOverflowLines();
  //virtual bool_a CopyOverflowLines(CPDFTextBox *i_pTextBox, bool_a i_bCheckBreak = false_a);
  //virtual bool_a SetHeight(long i_lHeight);
  virtual bool_a SetDrawHeight(long i_lHeight);
  virtual bool_a AddFrame(CPDFFrame *i_pFrame, bool_a i_bUseSize = false_a);
  //virtual bool_a SetWidth(long i_lWidth);
  //virtual bool_a SetDrawWidth(long i_lWidth);
  //virtual bool_a AddPageMark(ePAGEMARKTYPE i_eType);
  //virtual long AddText(char *i_pText);
  //virtual bool_a AddImage(char *i_pImageName, long i_lX, long i_lY, long i_lWidth, long i_lHeight,
  //                        long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
  //                        bool_a i_bAddToLine);
  //virtual bool_a AddScaledImage(char *i_pImageName, long i_lX, long i_lY, long i_lScaleX, long i_lScaleY,
  //                              long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
  //                              bool_a i_bAddToLine);

  virtual bool_a CheckResize();
  char *GetFrameID();
  void SetFrameID(char *i_pID);
  bool_a MoveToYPos();
  bool_a InsertBreak();
  void SetResizeType(eResizeType i_eResize);
  CPDFFrame::eResizeType GetResizeType();
  void SetOverlapType(eOverlapType i_eOverlapType);
  CPDFFrame::eOverlapType GetOverlapType();
  long GetRotate(){return m_lRotate;};
  void SetRotate(long i_lRotate){m_lRotate = i_lRotate;};

protected:
  bool_a MakeColumns();
  long CalcColumnHeight();
  CPDFTextBox *GetTextCol(){return m_pTextCol;};


  char *m_pFrameID;
  eYPosType m_ePosType;
  long m_lYPos;
  CPDFColor m_BorderColor;//TODO: move it to textbox
  eResizeType m_eResizeType;
  eOverlapType m_eOverlapType;
  bool_a m_bCanResize;

  long m_lColGutter;
  long m_lColCount;
  long m_lColWidth;
  CPDFTextBox *m_pTextCol;
  long m_lRotate;
};


#endif  /*  __PDFFRAME_H__  */
