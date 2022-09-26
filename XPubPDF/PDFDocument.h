#ifndef __PDFDOCUMENT_H__
#define __PDFDOCUMENT_H__

#include "XPubPDFExpImp.h"
#include <map>

#include "PDFPage.h"
#include "PDFFrame.h"
#include "PDFTextBox.h"
#include "PDFTable.h"
#include "PDFOutlineEntry.h"
#include "PDFImage.h"
#include "PDFAnnotation.h"
#include "PDFTemplate.h"

#if defined(WIN32)

#pragma warning(disable : 4251)

#include <windows.h>

#else // WIN32

/* orientation selections */
#define DMORIENT_PORTRAIT    1
#define DMORIENT_LANDSCAPE   2

/* paper selections */
#define DMPAPER_FIRST                DMPAPER_LETTER
#define DMPAPER_LETTER               1  /* Letter 8 1/2 x 11 in               */
#define DMPAPER_LETTERSMALL          2  /* Letter Small 8 1/2 x 11 in         */
#define DMPAPER_TABLOID              3  /* Tabloid 11 x 17 in                 */
#define DMPAPER_LEDGER               4  /* Ledger 17 x 11 in                  */
#define DMPAPER_LEGAL                5  /* Legal 8 1/2 x 14 in                */
#define DMPAPER_STATEMENT            6  /* Statement 5 1/2 x 8 1/2 in         */
#define DMPAPER_EXECUTIVE            7  /* Executive 7 1/4 x 10 1/2 in        */
#define DMPAPER_A3                   8  /* A3 297 x 420 mm                    */
#define DMPAPER_A4                   9  /* A4 210 x 297 mm                    */
#define DMPAPER_A4SMALL             10  /* A4 Small 210 x 297 mm              */
#define DMPAPER_A5                  11  /* A5 148 x 210 mm                    */
#define DMPAPER_B4                  12  /* B4 (JIS) 250 x 354                 */
#define DMPAPER_B5                  13  /* B5 (JIS) 182 x 257 mm              */
#define DMPAPER_FOLIO               14  /* Folio 8 1/2 x 13 in                */
#define DMPAPER_QUARTO              15  /* Quarto 215 x 275 mm                */
#define DMPAPER_10X14               16  /* 10x14 in                           */
#define DMPAPER_11X17               17  /* 11x17 in                           */
#define DMPAPER_NOTE                18  /* Note 8 1/2 x 11 in                 */
#define DMPAPER_ENV_9               19  /* Envelope #9 3 7/8 x 8 7/8          */
#define DMPAPER_ENV_10              20  /* Envelope #10 4 1/8 x 9 1/2         */
#define DMPAPER_ENV_11              21  /* Envelope #11 4 1/2 x 10 3/8        */
#define DMPAPER_ENV_12              22  /* Envelope #12 4 \276 x 11           */
#define DMPAPER_ENV_14              23  /* Envelope #14 5 x 11 1/2            */
#define DMPAPER_CSHEET              24  /* C size sheet                       */
#define DMPAPER_DSHEET              25  /* D size sheet                       */
#define DMPAPER_ESHEET              26  /* E size sheet                       */
#define DMPAPER_ENV_DL              27  /* Envelope DL 110 x 220mm            */
#define DMPAPER_ENV_C5              28  /* Envelope C5 162 x 229 mm           */
#define DMPAPER_ENV_C3              29  /* Envelope C3  324 x 458 mm          */
#define DMPAPER_ENV_C4              30  /* Envelope C4  229 x 324 mm          */
#define DMPAPER_ENV_C6              31  /* Envelope C6  114 x 162 mm          */
#define DMPAPER_ENV_C65             32  /* Envelope C65 114 x 229 mm          */
#define DMPAPER_ENV_B4              33  /* Envelope B4  250 x 353 mm          */
#define DMPAPER_ENV_B5              34  /* Envelope B5  176 x 250 mm          */
#define DMPAPER_ENV_B6              35  /* Envelope B6  176 x 125 mm          */
#define DMPAPER_ENV_ITALY           36  /* Envelope 110 x 230 mm              */
#define DMPAPER_ENV_MONARCH         37  /* Envelope Monarch 3.875 x 7.5 in    */
#define DMPAPER_ENV_PERSONAL        38  /* 6 3/4 Envelope 3 5/8 x 6 1/2 in    */
#define DMPAPER_FANFOLD_US          39  /* US Std Fanfold 14 7/8 x 11 in      */
#define DMPAPER_FANFOLD_STD_GERMAN  40  /* German Std Fanfold 8 1/2 x 12 in   */
#define DMPAPER_FANFOLD_LGL_GERMAN  41  /* German Legal Fanfold 8 1/2 x 13 in */
//#if(WINVER >= 0x0400)
#define DMPAPER_ISO_B4              42  /* B4 (ISO) 250 x 353 mm              */
#define DMPAPER_JAPANESE_POSTCARD   43  /* Japanese Postcard 100 x 148 mm     */
#define DMPAPER_9X11                44  /* 9 x 11 in                          */
#define DMPAPER_10X11               45  /* 10 x 11 in                         */
#define DMPAPER_15X11               46  /* 15 x 11 in                         */
#define DMPAPER_ENV_INVITE          47  /* Envelope Invite 220 x 220 mm       */
#define DMPAPER_RESERVED_48         48  /* RESERVED--DO NOT USE               */
#define DMPAPER_RESERVED_49         49  /* RESERVED--DO NOT USE               */
#define DMPAPER_LETTER_EXTRA        50  /* Letter Extra 9 \275 x 12 in        */
#define DMPAPER_LEGAL_EXTRA         51  /* Legal Extra 9 \275 x 15 in         */
#define DMPAPER_TABLOID_EXTRA       52  /* Tabloid Extra 11.69 x 18 in        */
#define DMPAPER_A4_EXTRA            53  /* A4 Extra 9.27 x 12.69 in           */
#define DMPAPER_LETTER_TRANSVERSE   54  /* Letter Transverse 8 \275 x 11 in   */
#define DMPAPER_A4_TRANSVERSE       55  /* A4 Transverse 210 x 297 mm         */
#define DMPAPER_LETTER_EXTRA_TRANSVERSE 56 /* Letter Extra Transverse 9\275 x 12 in */
#define DMPAPER_A_PLUS              57  /* SuperA/SuperA/A4 227 x 356 mm      */
#define DMPAPER_B_PLUS              58  /* SuperB/SuperB/A3 305 x 487 mm      */
#define DMPAPER_LETTER_PLUS         59  /* Letter Plus 8.5 x 12.69 in         */
#define DMPAPER_A4_PLUS             60  /* A4 Plus 210 x 330 mm               */
#define DMPAPER_A5_TRANSVERSE       61  /* A5 Transverse 148 x 210 mm         */
#define DMPAPER_B5_TRANSVERSE       62  /* B5 (JIS) Transverse 182 x 257 mm   */
#define DMPAPER_A3_EXTRA            63  /* A3 Extra 322 x 445 mm              */
#define DMPAPER_A5_EXTRA            64  /* A5 Extra 174 x 235 mm              */
#define DMPAPER_B5_EXTRA            65  /* B5 (ISO) Extra 201 x 276 mm        */
#define DMPAPER_A2                  66  /* A2 420 x 594 mm                    */
#define DMPAPER_A3_TRANSVERSE       67  /* A3 Transverse 297 x 420 mm         */
#define DMPAPER_A3_EXTRA_TRANSVERSE 68  /* A3 Extra Transverse 322 x 445 mm   */
//#endif /* WINVER >= 0x0400 */
//#if(WINVER >= 0x0500)
#define DMPAPER_DBL_JAPANESE_POSTCARD 69 /* Japanese Double Postcard 200 x 148 mm */
#define DMPAPER_A6                  70  /* A6 105 x 148 mm                 */
#define DMPAPER_JENV_KAKU2          71  /* Japanese Envelope Kaku #2       */
#define DMPAPER_JENV_KAKU3          72  /* Japanese Envelope Kaku #3       */
#define DMPAPER_JENV_CHOU3          73  /* Japanese Envelope Chou #3       */
#define DMPAPER_JENV_CHOU4          74  /* Japanese Envelope Chou #4       */
#define DMPAPER_LETTER_ROTATED      75  /* Letter Rotated 11 x 8 1/2 11 in */
#define DMPAPER_A3_ROTATED          76  /* A3 Rotated 420 x 297 mm         */
#define DMPAPER_A4_ROTATED          77  /* A4 Rotated 297 x 210 mm         */
#define DMPAPER_A5_ROTATED          78  /* A5 Rotated 210 x 148 mm         */
#define DMPAPER_B4_JIS_ROTATED      79  /* B4 (JIS) Rotated 364 x 257 mm   */
#define DMPAPER_B5_JIS_ROTATED      80  /* B5 (JIS) Rotated 257 x 182 mm   */
#define DMPAPER_JAPANESE_POSTCARD_ROTATED 81 /* Japanese Postcard Rotated 148 x 100 mm */
#define DMPAPER_DBL_JAPANESE_POSTCARD_ROTATED 82 /* Double Japanese Postcard Rotated 148 x 200 mm */
#define DMPAPER_A6_ROTATED          83  /* A6 Rotated 148 x 105 mm         */
#define DMPAPER_JENV_KAKU2_ROTATED  84  /* Japanese Envelope Kaku #2 Rotated */
#define DMPAPER_JENV_KAKU3_ROTATED  85  /* Japanese Envelope Kaku #3 Rotated */
#define DMPAPER_JENV_CHOU3_ROTATED  86  /* Japanese Envelope Chou #3 Rotated */
#define DMPAPER_JENV_CHOU4_ROTATED  87  /* Japanese Envelope Chou #4 Rotated */
#define DMPAPER_B6_JIS              88  /* B6 (JIS) 128 x 182 mm           */
#define DMPAPER_B6_JIS_ROTATED      89  /* B6 (JIS) Rotated 182 x 128 mm   */
#define DMPAPER_12X11               90  /* 12 x 11 in                      */
#define DMPAPER_JENV_YOU4           91  /* Japanese Envelope You #4        */
#define DMPAPER_JENV_YOU4_ROTATED   92  /* Japanese Envelope You #4 Rotated*/
#define DMPAPER_P16K                93  /* PRC 16K 146 x 215 mm            */
#define DMPAPER_P32K                94  /* PRC 32K 97 x 151 mm             */
#define DMPAPER_P32KBIG             95  /* PRC 32K(Big) 97 x 151 mm        */
#define DMPAPER_PENV_1              96  /* PRC Envelope #1 102 x 165 mm    */
#define DMPAPER_PENV_2              97  /* PRC Envelope #2 102 x 176 mm    */
#define DMPAPER_PENV_3              98  /* PRC Envelope #3 125 x 176 mm    */
#define DMPAPER_PENV_4              99  /* PRC Envelope #4 110 x 208 mm    */
#define DMPAPER_PENV_5              100 /* PRC Envelope #5 110 x 220 mm    */
#define DMPAPER_PENV_6              101 /* PRC Envelope #6 120 x 230 mm    */
#define DMPAPER_PENV_7              102 /* PRC Envelope #7 160 x 230 mm    */
#define DMPAPER_PENV_8              103 /* PRC Envelope #8 120 x 309 mm    */
#define DMPAPER_PENV_9              104 /* PRC Envelope #9 229 x 324 mm    */
#define DMPAPER_PENV_10             105 /* PRC Envelope #10 324 x 458 mm   */
#define DMPAPER_P16K_ROTATED        106 /* PRC 16K Rotated                 */
#define DMPAPER_P32K_ROTATED        107 /* PRC 32K Rotated                 */
#define DMPAPER_P32KBIG_ROTATED     108 /* PRC 32K(Big) Rotated            */
#define DMPAPER_PENV_1_ROTATED      109 /* PRC Envelope #1 Rotated 165 x 102 mm */
#define DMPAPER_PENV_2_ROTATED      110 /* PRC Envelope #2 Rotated 176 x 102 mm */
#define DMPAPER_PENV_3_ROTATED      111 /* PRC Envelope #3 Rotated 176 x 125 mm */
#define DMPAPER_PENV_4_ROTATED      112 /* PRC Envelope #4 Rotated 208 x 110 mm */
#define DMPAPER_PENV_5_ROTATED      113 /* PRC Envelope #5 Rotated 220 x 110 mm */
#define DMPAPER_PENV_6_ROTATED      114 /* PRC Envelope #6 Rotated 230 x 120 mm */
#define DMPAPER_PENV_7_ROTATED      115 /* PRC Envelope #7 Rotated 230 x 160 mm */
#define DMPAPER_PENV_8_ROTATED      116 /* PRC Envelope #8 Rotated 309 x 120 mm */
#define DMPAPER_PENV_9_ROTATED      117 /* PRC Envelope #9 Rotated 324 x 229 mm */
#define DMPAPER_PENV_10_ROTATED     118 /* PRC Envelope #10 Rotated 458 x 324 mm */
//#endif /* WINVER >= 0x0500 */

#ifndef _ASSERT
#define _ASSERT(expr) ((void)0)
#endif 

#endif // WIN32


typedef std::map <long, void *>           PDFPtrMap;
typedef std::map <long, void *>::iterator PDFPtrIterator;
typedef std::map <long, void *>::reverse_iterator PDFPtrIteratorR;


#define DEF_LEFT_MARGIN           1134    //in twips
#define DEF_RIGHT_MARGIN          1134    //in twips
#define DEF_TOP_MARGIN            1134    //in twips
#define DEF_BOTTOM_MARGIN         1134    //in twips
#define DEF_COLUMNS               1
#define DEF_COLUMN_SPACE          0
#define DEF_ORIENTATION           DMORIENT_PORTRAIT
#define DEF_FIRST_PAGE            -1
#define DEF_LAST_PAGE             -1
#define DEF_HDR_MARGIN            680    //in twips
#define DEF_FTR_MARGIN            680    //in twips
#define DEF_PAPER_WIDTH           11905   //in twips
#define DEF_PAPER_HEIGHT          16837   //in twips

static short DEF_PAPER_SIZE_COUNT = 15;
static short DefPaperSize[][3] = 
{
  {DMPAPER_LETTER, 12240, 15840},
  {DMPAPER_LEGAL, 12240, 20160},
  {DMPAPER_LEDGER, 24480, 15840},
  {DMPAPER_TABLOID, 15840, 24480},
  {DMPAPER_STATEMENT, 7920, 12240},
  {DMPAPER_EXECUTIVE, 10440, 15120},
  {DMPAPER_A3, 16837, 23811},
  {DMPAPER_A4, 11905, 16837},
  {DMPAPER_A5, 8390, 11905},
  {DMPAPER_B4, 14173, 20069},
  {DMPAPER_B5, 10318, 14570},
  {DMPAPER_ENV_10, 6480, 13680},
  {DMPAPER_ENV_DL, 6236, 12472},
  {DMPAPER_ENV_C5, 9184, 12982},
  {DMPAPER_ENV_MONARCH, 5580, 10800}
};

static const long lDefCodePage = 1252;
static const char czLayerName[] = "XPubLayer";

class CPDFLayer;


class XPUBPDF_EXPORTIMPORT CPDFSection
{
public:
  ~CPDFSection();

  CPDFDocument *GetDocument(){return m_pDoc;};

  long GetLeftMargin(){return m_lLeftMargin;};
  long GetRightMargin(){return m_lRightMargin;};
  long GetTopMargin(){return m_lTopMargin;};
  long GetBottomMargin(){return m_lBottomMargin;};
  long GetColumns(){return m_lColumns;};
  long GetColumnSpace(){return m_lColumnSpace;};
  long GetOrientation(){return m_lOrientation;};
  long GetFirstPage(){return m_lFirstPage;};
  long GetLastPage(){return m_lLastPage;};
  long GetHdrMargin(){return m_lHdrMargin;};
  long GetFtrMargin(){return m_lFtrMargin;};
  long GetPaperWidth(){return m_lPaperWidth;};
  long GetPaperHeight(){return m_lPaperHeight;};

  void SetLeftMargin(long i_lLeftMargin){m_lLeftMargin = i_lLeftMargin;};
  void SetRightMargin(long i_lRightMargin){m_lRightMargin = i_lRightMargin;};
  void SetTopMargin(long i_lTopMargin){m_lTopMargin = i_lTopMargin;};
  void SetBottomMargin(long i_lBottomMargin){m_lBottomMargin = i_lBottomMargin;};
  void SetMargins(long i_lLeft, long i_lRight, long i_lTop, long i_lBottom);
  void SetColumns(long i_lColumns){m_lColumns = i_lColumns;};
  void SetColumnSpace(long i_lColumnSpace){m_lColumnSpace = i_lColumnSpace;};
  void SetOrientation(long i_lOrientation){m_lOrientation = i_lOrientation;};
  void SetFirstPage(long i_lFirstPage){m_lFirstPage = i_lFirstPage;};
  void SetLastPage(long i_lLastPage){m_lLastPage = i_lLastPage;};
  void SetHdrMargin(long i_lHdrMargin){m_lHdrMargin = i_lHdrMargin;};
  void SetFtrMargin(long i_lFtrMargin){m_lFtrMargin = i_lFtrMargin;};
  void SetPaperWidth(long i_lPaperWidth){m_lPaperWidth = i_lPaperWidth;};
  void SetPaperHeight(long i_lPaperHeight){m_lPaperHeight = i_lPaperHeight;};
  void SetPaperSize(long i_lPaperWidth, long i_lPaperHeight);
/*
  void SetLeftMargin(CPDFTextBox *i_pTextArea, long i_lLeftMargin){m_lLeftMargin = i_lLeftMargin;};
  void SetRightMargin(CPDFTextBox *i_pTextArea, long i_lRightMargin){m_lRightMargin = i_lRightMargin;};
  void SetTopMargin(CPDFTextBox *i_pTextArea, long i_lTopMargin){m_lTopMargin = i_lTopMargin;};
  void SetBottomMargin(CPDFTextBox *i_pTextArea, long i_lBottomMargin){m_lBottomMargin = i_lBottomMargin;};
  void SetColumns(CPDFTextBox *i_pTextArea, long i_lColumns){m_lColumns = i_lColumns;};
  void SetColumnSpace(CPDFTextBox *i_pTextArea, long i_lColumnSpace){m_lColumnSpace = i_lColumnSpace;};
  void SetOrientation(CPDFTextBox *i_pTextArea, long i_lOrientation){m_lOrientation = i_lOrientation;};
  void SetFirstPage(CPDFTextBox *i_pTextArea, long i_lFirstPage){m_lFirstPage = i_lFirstPage;};
  void SetLastPage(CPDFTextBox *i_pTextArea, long i_lLastPage){m_lLastPage = i_lLastPage;};
  void SetHdrMargin(CPDFTextBox *i_pTextArea, long i_lHdrMargin){m_lHdrMargin = i_lHdrMargin;};
  void SetFtrMargin(CPDFTextBox *i_pTextArea, long i_lFtrMargin){m_lFtrMargin = i_lFtrMargin;};
  void SetPaperWidth(CPDFTextBox *i_pTextArea, long i_lPaperWidth){m_lPaperWidth = i_lPaperWidth;};
  void SetPaperHeight(CPDFTextBox *i_pTextArea, long i_lPaperHeight){m_lPaperHeight = i_lPaperHeight;};
*/
  CPDFHeaderFooter *GetHdrExemplar();
  CPDFHeaderFooter *GetFtrExemplar();

  void AddTextToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, char *i_pText);
  void AddTextToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, wchar_t *i_pText);
  void AddImageToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, char *i_pImageName, long i_lX, long i_lY, long i_lWidth, long i_lHeight,
                          long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist, bool_a i_bAddToLine);
  void AddScaledImageToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, char *i_pImageName, long i_lX, long i_lY, long i_lScaledX, long i_lScaledY,
                               long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist, bool_a i_bAddToLine);
  void InsertLineToHdrFtr(CPDFHeaderFooter *i_pHdrFtr);
  void InsertTabToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, PDFTabStop i_Tab);
  void SetTextColorToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, float i_fRed, float i_fGreen, float i_fBlue);
  void SetAlignmentToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, short i_sAlignment);
  void SetLeftRightIndentsToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, long i_lLeft, long i_lRight, long i_lFirstIndent);
  void AddPageMarkToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, ePAGEMARKTYPE i_eType);

  bool_a PageBelong(long i_lPage);

protected:
  CPDFSection(CPDFDocument *i_pDoc, long i_lFirst, long i_lLast);
  CPDFSection(CPDFSection *i_pSection);

  friend class CPDFDocument;
private:

  long            m_lLeftMargin;   // left margin in points
  long            m_lRightMargin;  // right margin in points
  long            m_lTopMargin;    // top margin in points
  long            m_lBottomMargin; // bottom margin in points
  long            m_lOrientation;  // paper orientation: EPageType
  long            m_lFirstPage;    // first
  long            m_lLastPage;

  long            m_lColumns;      // number of columns
  long            m_lColumnSpace;  // space between column in points

  long            m_lHdrMargin;    // header margin

  long            m_lFtrMargin;    // footer margin

  long            m_lPaperWidth;   // paper width in portrait orientation
  long            m_lPaperHeight;  // paper height in portrait orientation

  bool_a          m_bForward;

  CPDFDocument    *m_pDoc;
};


class XPUBPDF_EXPORTIMPORT CPDFDocument :public PDFDoc
{
public:
  CPDFDocument();
  ~CPDFDocument();

  static bool_a CompressStream(std::string &io_Stream);
  static bool_a UncompressStream(std::string &io_Stream);
  static size_t PDFStrStrKey(std::string& i_strObj, const char *i_pczKey);
  static size_t PDFStrStrKey(std::string& i_strObj, size_t i_index, const char *i_pczKey);
  static size_t PDFStrStrNumKey(std::string& i_strObj, const char *i_pczKey);
  static size_t PDFStrStrArrayKey(std::string& i_strObj, const char *i_pczKey);
  //static bool_a PDFGetColorValue(std::string& i_strObj, const char *i_pczKey, CPDFColor& io_Color);

  void FlushToFile(bool_a i_bFlush);
  //allocate and initialize members of document 
  bool_a Initialize(const char *i_pczDocName);
  //returns name of document (name of file which will be created)
  const char *GetName();
  void SetPassword(const char *i_pOwner, const char *i_pUser, int i_iPermission);
  const char *GetOwnerPassword();
  const char *GetUserPassword();
  void SetFontEmbedded(bool_a i_bEmbed = true_a);
  bool_a GetFontEmbedded();
  //enables/disables /FlateDecode compression
  void EnableCompression(bool_a i_bEnable = true_a);
  long GetDocumentStream(const char **o_pczStream);

  //convert millimetres to points
  long ConvertMMToPoints(float i_lMM);
  //convert points to twips
  long ConvertPointsToTwips(long i_lPoints);
  //convert twips to points
  long ConvertTwipsToPoints(long i_lTwips);
  long ConvertTwipsToPoints(float i_fTwips);
  long ConvertTwipsToPoints(int i_iTwips);

  //creates and initialize new page
  //if i_lNum page exists, returns pointer to that page
  //if the function fails, the return value is NULL
  //i_lWidth, i_lHeight - width and height of page in points
  //pointer is saved in document
  CPDFPage *CreatePage(long i_lNum, long i_lWidth, long i_lHeight,
                        CPDFPage::EPageType i_eType = CPDFPage::eDisplayPortrait);
  CPDFPage *CreatePage(long i_lNum, CPDFSection *i_pSection,
                        CPDFPage::EPageType i_eType = CPDFPage::eDisplayPortrait);
  CPDFPage *InsertPage(long i_lNum, CPDFSection *i_pSection,
                        CPDFPage::EPageType i_eType = CPDFPage::eDisplayPortrait);
  //returns pointers to page i_lNum
  CPDFPage *GetPage(long i_lNum);
  //returns count of pages created by this class
  long GetPageCount();
  //deletes page i_lNum
  void DeletePage(long i_lNum);
  long GetLastPage();
  long GetCurrentPage();
  bool_a SetCurrentPage(long i_lPage);

  //creates and initialize new textbox
  //textbox is holded in memory
  //to write it to the file you must call function CPDFTextBox::Flush()
  //if the function fails, the return value is NULL
  //i_recPosSize.lLeft, i_recPosSize.lTop - upperleft corner position of textbox
  //i_recPosSize.lRight, i_recPosSize.lBottom - lowerright corner position of textbox
  //i_lPage is only index, it can be changed in everytime
  //pointer is saved in document
  CPDFTextBox *CreateTextBox(long i_lPage, rec_a i_recPosSize, CPDFTextBox *i_pParent = NULL);
  //delete textbox i_lID and remove it from document
  //in this case i_lID is sequence of textbox
  void DeleteTextBox(long i_lID);
  //delete textbox i_pTable and remove it from document
  void DeleteTextBox(CPDFTextBox *i_pTextBox);
  //remove textbox from document
  //in this case i_lID is sequence of textbox
  CPDFTextBox *RemoveTextBox(long i_lID);
  //remove textbox from document
  CPDFTextBox *RemoveTextBox(CPDFTextBox *i_pTextBox);
  //returns count of textboxes created by this class
  long GetTextBoxCount();
  //returns pointer to textbox i_lID
  //in this case i_lID is sequence of textbox
  CPDFTextBox *GetTextBox(long i_lID);

  long GetFrameCount();
  CPDFFrame *GetFrame(long i_lOrder);
  CPDFFrame *GetFrame(const char *i_pID, long &o_lOrder);
  CPDFFrame *CreateFrame(long i_lPage, rec_a i_recPosSize, CPDFFrame::eYPosType i_eType,
                         long &o_lID, const char *i_pID = NULL);
  bool_a JoinFrames(CPDFFrame *pFrame, const char *i_pID);

  //creates and initialize new table
  //table is holded in memory
  //to write it to the file, you must call CPDFTable::Flush()
  //if the function fails, the return value is NULL
  //i_lRowCount - row count
  //i_lColCount - column count
  //i_lXStep - width of cells
  //i_lYStep - height of cells
  //i_lPage is only index, it can be changed in everytime
  //pointer is saved in document
  CPDFTable *CreateTable(long i_lPage, long i_lRowCount, long i_lColCount,
                          long i_lXPos, long i_lYPos, long i_lXStep, long i_lYStep);
  CPDFTable *CreateTable(long i_lPage, long i_lRowCount, long i_lColCount,
                          long i_lXPos, long i_lYPos, long i_lXStep, long i_lYStep, long &o_lID);
  //delete table i_lID and remove it from document
  //in this case i_lID is sequence of table
  void DeleteTable(long i_lID);
  //delete table i_pTable and remove it from document
  void DeleteTable(CPDFTable *i_pTable);
  //remove table from document
  //in this case i_lID is sequence of table
  CPDFTable *RemoveTable(long i_lID);
  //remove table from document
  CPDFTable *RemoveTable(CPDFTable *i_pTable);
  //return count of tables
  long GetTableCount();
  //return pointer to table i_lID
  //in this case i_lID is sequence of table
  CPDFTable *GetTable(long i_lID);
  long GetTableID(CPDFTable *i_pTable);

  void SetDocInfo(eDOCINFO i_eType, const char *i_pText);
  const char *GetDocInfo(eDOCINFO i_eType);

  virtual CPDFBaseFont *SetActualFont(const char *i_pFontName, float i_fSize, long i_lCodePage = NOT_USED);
  virtual CPDFBaseFont *SetActualFont(const char *i_pFontName);
  virtual CPDFBaseFont *SetDefaultFont(const char *i_pFontName, float i_fSize);
  virtual CPDFBaseFont *SetDefaultFont(const char *i_pFontName);
  virtual bool_a SetDefaultFontSize(long i_fSize);
  virtual bool_a SetFontSize(float i_fSize);
  virtual float GetFontSize();
  virtual float GetConvertFontSize();
  virtual long GetActualFontHeight();
  virtual long GetFontHeight(long i_lID);
  virtual long GetActualFontAscentHeight();
  virtual long GetActualFontDescentHeight();
  virtual long GetFontsCount();
  virtual bool_a SetFontStyle(short i_sFlag);
  virtual CPDFBaseFont *GetActualFont();
  virtual long GetActualFontID();
  virtual void SetActualFontID(long i_lID);

  virtual float GetActualFontSize();
  virtual void SetActualFontSize(float i_fSize);
  virtual long GetActualFontStyle();
  virtual void SetActualFontStyle(long i_lStyle);
  virtual long GetActualCodePage();
  virtual bool_a SetActualCodePage(long i_lCodePage);

  virtual CPDFBaseFont *GetFont(long i_lID);

  CPDFFontDescriptor *AddFontDescriptor(CPDFFontDescriptor *i_pFontDesc, const char *i_pczFontName);
  CPDFFontDescriptor *GetFontDescriptor(const char *i_pczFontName);

  bool_a FillPageBreak();
  bool_a LoadTemplatePDF(const char *i_pFileName, long i_lPos, const char *pPassword = NULL);
  void SetTemplatePassword(long i_lIndex, const char *i_pczPassword);
  const char *GetTemplatePassword(long i_lIndex);
  long GetTemplateCount();
  long GetTemplatePageCount(long i_lIndex);
  long GetTemplateStrBlockCount(long i_lIndex, long i_lPage);
  const wchar_t *GetTemplateStrBlock(long i_lIndex, long i_lPage, long i_lBlock);
  const char *GetTemplateStrBlockFontName(long i_lIndex, long i_lPage, long i_lBlock);
  float GetTemplateStrBlockFontSize(long i_lIndex, long i_lPage, long i_lBlock);
  void GetTemplateStrBlockPosition(long i_lIndex, long i_lPage, long i_lBlock, float &o_fX, float &o_fY);
  long GetTemplateImageCount(long i_lIndex, long i_lPage);
  const char *GetTemplateImageName(long i_lIndex, long i_lPage, long i_lImageIndex);
  bool_a CreateFormOnImage(long i_lIndex, const char *i_pImageName);
  bool_a BuildStrBlock(long i_lIndex, CPDFTemplate::eStrBlockType i_eType, CPDFStrBlockControl *i_pStrBlockCtrl);

  long CalculatePageCount();
  bool_a IsTemplate();
  long GetFormFieldCount();
  const char *GetFormFieldName(long i_lIndex, bool_a i_bSorted = false_a);
  bool_a SetFormField(const char *i_pName, const char *i_pValue);
  bool_a SetFormFieldFont(const char *i_pName, const char *i_pFontName, float i_fFontSize, long i_lCodePage);
  bool_a CheckFormFieldOverflow(const char *i_pName, const char *i_pValue);
  const char *GetFormFieldValue(const char *i_pName);
  void SetFileID(char *i_pID);
  unsigned char *GetEncryptionOwnerKey() const;
  unsigned char *GetEncryptionUserKey() const;
  int GetEncryptionPermissionKey() const;
  bool_a Encrypt();

  virtual CPDFImage *CreateImage(const char *i_pImage);
  virtual long GetImagesCount();
  virtual bool_a AddImageToPage(long i_lPage, CPDFImage *i_pImage, long i_lX, long i_lY,
                        long i_lWidth, long i_lHeight, long i_lRotate = 0);
  virtual bool_a AddImageToPage(long i_lPage, long i_lIndex, long i_lX, long i_lY,
                        long i_lWidth, long i_lHeight, long i_lRotate = 0);
  virtual CPDFImage *GetImage(long i_lID);

  bool_a CreateLink(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                    long i_lTop, long i_lRight, long i_lBottom, float i_fZoom);

  bool_a CreateLink(const char *i_pczName, const char *i_pLocation);

  bool_a CreateLink(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                    long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                    const char *i_pLocation);

  bool_a ChangeLinkDest(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                        long i_lTop, long i_lRight, long i_lBottom, float i_fZoom);

  bool_a HasLink(const char *i_pczName);

  CPDFAnnotation *CreateForm(const char *i_pczName, eDICTFIELDTYPE i_eType, rec_a i_recPosSize, long i_lPage);
  long GetFormCount() {return m_vctAnnots.size();};
  CPDFAnnotation *GetForm(long i_lID);
  bool_a MoveForm(long i_lDeltaX, long i_lDeltaY);
  bool_a MoveForm(long i_lDeltaX, long i_lDeltaY, const char *i_pName);
  bool_a HasWidget();
  bool_a GetAcroFormObject(std::string &io_Stream);


  long GetOutlineEntryCount();
  CPDFOutlineEntry *GetOutlineEntry(long i_lIndex);
  CPDFOutlineEntry *CreateOutlineEntry(CPDFOutlineEntry *i_pFollowThis, bool_a i_bSubEntry,
                                       bool_a i_bOpen, long i_lPage, char *i_pTitle,
                                       eDESTSPEC i_eDest, long i_lLeft, long i_lTop,
                                       long i_lRight, long i_lBottom, float i_fZoom,
                                       float i_fR, float i_fG, float i_fB, short i_sFkey,
                                       long i_lFont = NOT_USED);

  CPDFOutlineEntry *CreateOutlineEntry(long i_lPage, char *i_pLevel, char *i_pTitle, long i_lLeft, long i_lTop,
                                       float i_fZoom, float i_fR, float i_fG, float i_fB, short i_sTitleStyle,
                                       long i_lFont = NOT_USED);

  //draw line from (i_lXFrom, i_lYFrom) to (i_lXTo, i_lYTo)
  //line is immediately write to page
  bool_a DrawLine(float i_fXFrom, float i_fYFrom, float i_fXTo, float i_fYTo);
  bool_a DrawLine(long i_lXFrom, long i_lYFrom, long i_lXTo, long i_lYTo);
  bool_a DrawDashedLine(float i_fXFrom, float i_fYFrom, float i_fXTo, float i_fYTo);
  bool_a DrawDashedLine(long i_lXFrom, long i_lYFrom, long i_lXTo, long i_lYTo);
  //draw rectangle
  //line is immediately write to page
  //i_bFill is true, rectangle is filled
  bool_a DrawRectangle(rec_a i_Rec, bool_a i_bFill);
  bool_a DrawRectangle(frec_a i_Rec, bool_a i_bFill);
  bool_a SetLineWidth(long i_lWidth);
  bool_a SetLineWidth(float i_fWidth);
  bool_a SetLineColor(float i_fRed, float i_fGreen, float i_fBlue);
  bool_a SetLineColor(CPDFColor& i_rColor);


  bool_a AddTextToPage(long i_lPage, long i_lX, long i_lY,
                      long i_lRotate, char *i_pString);
  bool_a ContinueTextToPage(long i_lPage, char *i_pczString);
  bool_a SetTextColor(float i_fRed, float i_fGreen, float i_fBlue);
  bool_a SetTextColor(CPDFColor& i_rColor);
  bool_a SetLeading(float i_fLeading);
  bool_a NextLine();

  void SetCellMargins(long i_lLeft, long i_lRight, long i_lTop, long i_lBottom);
  long GetLeftMargin(){return m_lLeftCellMargin;};
  long GetRightMargin(){return m_lRightCellMargin;};
  long GetTopMargin(){return m_lTopCellMargin;};
  long GetBottomMargin(){return m_lBottomCellMargin;};
  long GetLetterSpace();
  long GetTabDefaultSize();
  char GetTabDefaultDecimal();
  bool_a SetTabDefault(long i_lTabSize, char i_cTabDec);
  CPDFSection *GetSection(long i_lPageNum);

  CPDFLayer *CreateLayer(const char *i_pName);
  void EndLayer();
  CPDFLayer *GetActiveLayer(bool_a i_bReturnThis = true_a);
  long GetLayerCount();
  virtual CPDFLayer *GetLayer(long i_lIndex);
  virtual CPDFLayer *GetLayer(const char *i_pName);
  virtual bool_a GetLayerOrderStream(std::string &o_stream);
  virtual bool_a GetOCPropertiesStream(std::string &o_stream);


  bool_a Flush();
  bool_a IsFlush();
  void DeleteAll();

  bool_a AppendDocument(CPDFDocument *i_pDoc);
  bool_a AppendPageFromDocument(CPDFDocument *i_pDoc, long i_lPage);

  static long SetPos(void *i_pPtr, eSPACETYPE i_eType, long i_lPage,
                    long i_lPosY, long i_lPosX, bool_a i_bDivide, void *i_pParent);

  bool_a InsertPageBreak();
  bool_a CanPageBreak(bool_a i_bDelete);
  bool_a CheckCanPageBreak();
  bool_a InsertSectionBreak();
  bool_a InsertColumnBreak();
  void SetCanCopyOverflowLines(bool_a i_bEnable = true_a);
  bool_a CanCopyOverflowLines();
  bool_a CheckPageOverflow(long i_lPage = -1);

protected:
  CPDFBaseFont *MakeFont(const char *i_pczFontName = NULL);
  long CheckExistFont(const char *i_pczFontName);
  bool_a CopyResourcesToTemplate();

  friend class CPDFTemplate;

private:

  bool_a CheckHeaderFooter();
  bool_a CheckFrameOverlap();
  bool_a CheckTable();

  bool_a FlushDoc();
  bool_a FlushLayers();

  long ProcessTable(CPDFTable *i_pTable, long i_lPage, long i_lPosX, long i_lPosY,
                    bool_a i_bDivide, CPDFTextBox *i_pParent);
  long ProcessFrame(CPDFFrame *i_pFrame, long i_lPage, long i_lPosX, long i_lPosY);
  void AdjustSections(long i_lPage);
  bool_a CloseAllObject();
  bool_a CopyObjectToTemplate();

  bool_a          m_bFlush;
  bool_a          m_bCanCopyOverflow;

  long            m_lLeftCellMargin;
  long            m_lRightCellMargin;
  long            m_lTopCellMargin;
  long            m_lBottomCellMargin;
//  CPDFTemplate    *m_pTemplatePDF;
  std::vector<CPDFTemplate *> m_TemplatesPDF;

  bool_a m_bFontEmbed;

  std::map<std::string, CPDFFontDescriptor *> m_mapFontDesc;

//Fonts
  std::vector<CPDFBaseFont *> m_vctFonts;
  long m_lActualFontID;
  float m_fActualFontSize;
  long m_lActualFontStyle;
  long m_lActualCodePage;


  PDFPtrMap       m_OEMap;
  PDFPtrMap       m_TextBoxMap;
  PDFPtrMap       m_TableMap;
  PDFPtrMap       m_FrameMap;
  PDFPtrMap       m_SectionMap;

  std::string     m_strLayer;
  std::vector<CPDFLayer *> m_vctLayers;
};

class CPDFInfo
{
public:
  CPDFInfo();
  ~CPDFInfo();
private:
};

#if defined(WIN32)
#pragma warning(default : 4251)
#endif // WIN32

#endif  /*  __PDFDOCUMENT_H__  */

