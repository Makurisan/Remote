#ifndef __PDF_DEF_H__
#define __PDF_DEF_H__


#include <stdio.h>
#include <time.h>
#include <map>
#include <string>
#include <vector>
#include "PDFUtil.h"


class CPDFBaseFont;
class CPDFImage;



#define NOT_USED      -1

#define DEFFONTHEIGHT           10

#define DEFPOINTSPERINCH        72.f
#define POINTSPERINCH           1440.f//72
#define POINTSPERMM             56.692913385826f//2,8346456692913f
#define INCHINCM                2.54f

#define DATE_SIZE                         30
#define NAME_SIZE                         100
#define FILE_NAME_SIZE                    200
#define DICTIONARY_NAME_SIZE              20
#define ID_NAME_SIZE                      20

#define OBJECTS_ALLOC_STEP                10000

#define PDF_DEFAULT_FIRST_INDENT          0
#define PDF_DEFAULT_BLUE                  0
#define PDF_DEFAULT_GREEN                 0
#define PDF_DEFAULT_RED                   0
#define PDF_DEFAULT_BG_BLUE               1
#define PDF_DEFAULT_BG_GREEN              1
#define PDF_DEFAULT_BG_RED                1
#define PDF_DEFAULT_LINE_SPACE            0
#define PDF_DEFAULT_LETTER_SPACE          0


#define PDF_DEFAULT_TAB_DECIMAL_COMMA     ','
#define PDF_DEFAULT_TAB_DECIMAL_POINT     '.'
#define PDF_DEFAULT_TAB_SPACE             35

//text alignment
#define TF_LEFT                           0
#define TF_CENTER                         1
#define TF_RIGHT                          2
#define TF_JUSTIFY                        3
//vertical text alignment
#define TF_UP                             3
#define TF_MIDDLE                         4
#define TF_DOWN                           5

//tabs alignment
#define TAB_LEFT                          0
#define TAB_RIGHT                         1
#define TAB_CENTER                        2
#define TAB_DECIMAL                       3

//font attributes
#define FA_STANDARD                       0
#define FA_BOLD                           1
#define FA_NOBOLD                         2
#define FA_ITALIC                         3
#define FA_NOITALIC                       4
#define FA_UNDERLINE                      5
#define FA_NOUNDERLINE                    6
#define FA_DOUBLE_UNDERLINE               7
#define FA_NODOUBLE_UNDERLINE             8
#define FA_STRIKEOUT                      9
#define FA_NOSTRIKEOUT                    10


#define czBACKGROUNDTEXT   "XPubTagLib"


static const long lADOBE_GLYPH_LIST_COUNT = 4281;
extern const char *pczADOBE_GLYPH_LIST[lADOBE_GLYPH_LIST_COUNT][2];


static const char czOBJECTBEGIN[]               = "%ld 0 obj\n<<\n";
static const char czOBJECTEND[]                 = ">>\nendobj\n";
static const char czSTRINGTJ[]                  = "Tj";
static const char czFONTNAMESIZE[]              = "Tf";
static const char czMODIFYCTM[]                 = "cm";
static const char czOPERATORDO[]                = "Do";
static const char czImageName[]                 = "XpubI";

static const char czFALSE_U[]                   = "FALSE";
static const char czFALSE_L[]                   = "false";
static const char czTRUE_U[]                    = "TRUE";
static const char czTRUE_L[]                    = "true";
static const char czTRAILER[]                   = "trailer";
static const char czXREF[]                      = "xref";
static const char czENCRYPT[]                   = "Encrypt";
static const char czBGNDIC[]                    = "<<";
static const char czENDDIC[]                    = ">>";
static const char czROOT[]                      = "/Root";
static const char czPREV[]                      = "/Prev";
static const char czSIZE[]                      = "/Size";
static const char czEOF[]                       = "%%EOF";
static const char czXPUB[]                      = "/XPub";
//encrypt attributes
static const char czFilter[]                    = "Filter";
static const char czR[]                         = "R";
static const char czO[]                         = "O";
static const char czU[]                         = "U";
static const char czP[]                         = "P";
static const char czV[]                         = "V";
static const char czLength[]                    = "Length";
//linearized
static const char czLINEAR[]                    = "/Linearized";
static const char czL[]                         = "/L";
static const char czH[]                         = "/H";
static const char czE[]                         = "/E";
static const char czT[]                         = "/T";
//Catalog attributes
static const char czTYPECATALOG[]               = "/Type /Catalog";
static const char czDESTS[]                     = "/Dests";
static const char czOUTLINES[]                  = "/Outlines";
static const char czPAGES[]                     = "/Pages";
static const char czTHREADS[]                   = "/Threads";
static const char czACROFORM[]                  = "/AcroForm";
static const char czNAMES[]                     = "/Names";
static const char czOPENACTION[]                = "/OpenAction";
static const char czPAGEMODE[]                  = "/PageMode";
static const char czURI[]                       = "/URI";
static const char czVIEWERPRF[]                 = "/ViewerPreferences";
static const char czPAGELABELS[]                = "/PageLabels";
static const char czPAGELAYOUT[]                = "/PageLayout";
static const char czJAVASCRIPT[]                = "/JavaScript";
static const char czSTRUCTTREEROOT[]            = "/StructTreeRoot";
static const char czSPIDERINFO[]                = "/SpiderInfo";
static const char czFIELDS[]                    = "/Fields";
//Info attributes
static const char czAUTHOR[]                    = "/Author";
static const char czCREATIONDATE[]              = "/CreationDate";
static const char czMODDATE[]                   = "/ModDate";
static const char czCREATOR[]                   = "/Creator";
static const char czPRODUCER[]                  = "/Producer";
static const char czTITLE[]                     = "/Title";
static const char czSUBJECT[]                   = "/Subject";
static const char czKEYWORDS[]                  = "/Keywords";
//Viewer preferences attributes
static const char czHIDETOOLBAR[]               = "/HideToolbar";
static const char czHIDEMENUBAR[]               = "/HideMenubar";
static const char czHIDEWINDOWUI[]              = "/HideWindowUI";
static const char czFITWINDOW[]                 = "/FitWindow";
static const char czCENTERWINDOW[]              = "/CenterWindow";
static const char czNONFULLSCREENPAGEMODE[]     = "/NonFullScreenPageMode";
static const char czDIRECTION[]                 = "/Direction";
//Pages tree attributes
static const char czTYPEPAGESTREE[]             = "/Type /Pages";
static const char czKIDS[]                      = "/Kids";
static const char czCOUNT[]                     = "/Count";
static const char czPARENT[]                    = "/Parent";
//Page object attributes
static const char czTYPEPAGE[]                  = "/Type /Page";
static const char czMEDIABOX[]                  = "/MediaBox";
static const char czRESOURCES[]                 = "/Resources";
static const char czFONT[]                      = "/Font";
static const char czCONTENTS[]                  = "/Contents";
static const char czCROPBOX[]                   = "/CropBox";
static const char czROTATE[]                    = "/Rotate";
static const char czTHUMB[]                     = "/Thumb";
static const char czANNOTS[]                    = "/Annots";
static const char czBEADS[]                     = "/B";
static const char czDURATION[]                  = "/Duration";
static const char czHIDDEN[]                    = "/Hid";
static const char czTRANSITION[]                = "/Trans";
static const char czPIECEINFO[]                 = "/PriceInfo";
static const char czLASTMODIFIED[]              = "/LastModified";
static const char czSEPARTIONINFO[]             = "/SeparationInfo";
static const char czARTBOX[]                    = "/ArtBox";
static const char czTRIMBOX[]                   = "/TrimBox";
static const char czBLEEDBOX[]                  = "/BleedBox";

static unsigned char PDFPasswordPad[32] =
{
  0x28, 0xbf, 0x4e, 0x5e, 0x4e, 0x75, 0x8a, 0x41,
  0x64, 0x00, 0x4e, 0x56, 0xff, 0xfa, 0x01, 0x08, 
  0x2e, 0x2e, 0x00, 0xb6, 0xd0, 0x68, 0x3e, 0x80, 
  0x2f, 0x0c, 0xa9, 0xfe, 0x64, 0x53, 0x69, 0x7a
};

static const char czID[]                        = "/ID";
static const unsigned char ucFILE_ID_1[]        = "4f4b544c5055504c4259504f4d51444d";
static const unsigned char ucFILE_ID_2[]        = "aa46e39668f854ca17107a7d40361f5b";

static const char czPreferredZoom[]             = "/PZ";

static const char czNEWPARAGRAPH_1[]              = "\r";
static const char czNEWPARAGRAPH_2[]              = "\n";
static const char czTABSTOP[]                     = "\t";

static const char czDELIMITER_CHAR1               = '(';
static const char czDELIMITER_CHAR2               = ')';
static const char czDELIMITER_CHAR3               = '<';
static const char czDELIMITER_CHAR4               = '>';
static const char czDELIMITER_CHAR5               = '[';
static const char czDELIMITER_CHAR6               = ']';
static const char czDELIMITER_CHAR7               = '{';
static const char czDELIMITER_CHAR8               = '}';
static const char czDELIMITER_CHAR9               = '/';
static const char czDELIMITER_CHAR10              = '%';
static const std::string DELIMITERS("()<>[]{}/%");

static const char czESCAPE_CHAR                   = '\\';

static const char czWHITE_SPACE_CHAR1             = 0x00; // Null (NUL)
static const char czWHITE_SPACE_CHAR2             = 0x09; // Tab (HT)
static const char czWHITE_SPACE_CHAR3             = 0x0a; // Line feed (LF)
static const char czWHITE_SPACE_CHAR4             = 0x0c; // Form feed (FF)
static const char czWHITE_SPACE_CHAR5             = 0x0d; // Carriage return (CR)
static const char czWHITE_SPACE_CHAR6             = 0x20; // Space (SP)
static const std::string WHITE_SPACES("\x00\x09\x0a\x0c\x0d\x20", 6);




#define COPY_REC(recD, recS) recD.lLeft = recS.lLeft; recD.lRight = recS.lRight; \
                                recD.lTop = recS.lTop; recD.lBottom = recS.lBottom;
#define COPY_FREC(recD, recS) recD.fLeft = recS.fLeft; recD.fRight = recS.fRight; \
                                recD.fTop = recS.fTop; recD.fBottom = recS.fBottom;

#define COPY_TABS(tabD, tabS) tabD.sTabFlag = tabS.sTabFlag; tabD.lTabPos = tabS.lTabPos;

#define COPY_TB(tbD, tbS) tbD->m_Color = tbS->m_Color; tbD->m_BgColor = tbS->m_BgColor; \
              tbD->bTransparent = tbS->bTransparent; tbD->m_TBBgColor = tbS->m_TBBgColor; \
              tbD->lTBBgImage = tbS->lTBBgImage;COPY_REC(tbD->recIndents, tbS->recIndents); \
              tbD->lIndent = tbS->lIndent; tbD->sAlignment = tbS->sAlignment; tbD->sVAlignment = tbS->sVAlignment; tbD->lLineSpace = tbS->lLineSpace; \
              tbD->lLetterSpace = tbS->lLetterSpace; tbD->lTabSize = tbS->lTabSize; tbD->cTabDecimalType = tbS->cTabDecimalType; \
              tbD->pTBTabStops = tbS->pTBTabStops; tbD->sTabsCount = tbS->sTabsCount; tbD->pTBNext = tbS->pTBNext; tbD->pTBPrev = tbS->pTBPrev;

#define COPY_PARA_TO_TEXTBOX(tb, para) COPY_REC(tb->recIndents, para->recIndents); tb->lIndent = para->lIndent; \
              tb->sAlignment = para->sAlignment; tb->lLineSpace = para->lLineSpace; tb->pTBTabStops = para->pParaTabStops; \
              tb->sTabsCount = para->sParaTabsCount; tb->lTabSize = para->lParaTabSize; tb->cTabDecimalType = para->cParaTabDecimalType;

#define COPY_PARA_TO_PARA(paraD, paraS) COPY_REC(paraD->recIndents, paraS->recIndents); paraD->lIndent = paraS->lIndent; \
              paraD->sAlignment = paraS->sAlignment; paraD->lLineSpace = paraS->lLineSpace; \
              paraD->m_bKeepLinesTogether = paraS->m_bKeepLinesTogether; \
              paraD->m_bKeepWithNext = paraS->m_bKeepWithNext; paraD->lParaTabSize = paraS->lParaTabSize; \
              paraD->cParaTabDecimalType = paraS->cParaTabDecimalType; paraD->m_bWidowOrphan = paraS->m_bWidowOrphan; \
              paraD->m_bParagraphPageBreak = paraS->m_bParagraphPageBreak;

#define COPY_ELEM_TO_TEXTBOX(tb, elem) tb->m_Color = elem->m_Color; tb->m_BgColor = elem->m_BgColor; \
              tb->lLetterSpace = elem->lLetterSpace;


typedef struct PDFDoc*                PDFDocPTR;
typedef struct _pdf_list                PDFList;
typedef struct _pdf_outline_entry       PDFOutlineEntry;
typedef struct _pdf_tab_stop            PDFTabStop;
typedef struct _pdf_line                PDFLine;
typedef struct _pdf_paragraph           PDFParagraph;
typedef struct _pdf_tb_image            PDFTBImage;
typedef struct _pdf_tb_space            PDFTBSpace;




typedef struct
{
  long lLeft;
  long lTop;
  long lBottom;
  long lRight;
} rec_a;

typedef struct
{
  float fLeft;
  float fTop;
  float fBottom;
  float fRight;
} frec_a;

typedef enum
{
  ePrint        = 0x01,
  eModify       = 0x02,
  eCopy         = 0x04,
  eAnnotation   = 0x08,
  eForm         = 0x10,
  eExtract      = 0x20,
  eAssemble     = 0x40,
  ePrint2       = 0x80
} ePDFPERMISSION;
//Page mode
static const char *pczPAGEMODEATTR[] = 
{
  "/UseNone",
  "/UseOutlines",
  "/UseThumbs",
  "/FullScreen"
};
typedef enum 
{
  eUseNone = 0,
  eUseOutlines,
  eUseThumbs,
  eFullScreen
} ePAGEMODEATTR;
//Page layout
static const char *pczPAGELAYOUTATTR[]= 
{
  "/SinglePage",
  "/OneColumn",
  "/TwoColumnLeft",
  "/TwoColumnRight"
};
typedef enum
{
  eNotUsed = -1,
  eSinglePage = 0,
  eOneColumn,
  eTwoCoumnLeft,
  eTwoColumnRight
} ePAGELAYOUTATTR;
//Destination
static const char *pczDestSpecification[] =
{
  "/XYZ null null null",
  "/XYZ %.0f %.0f %.3f",
  "/Fit",
  "/FitH %.0f",
  "/FitV %.0f",
  "/FitR %.0f %.0f %.0f %.0f",
  "/FitB",
  "/FitBH %.0f",
  "/FitBV %.0f"
};
typedef enum
{
  eDestNull = 0,
  eDestLeftTopZoom,
  eDestFit,
  eDestFitH,
  eDestFitV,
  eDestFitR,
  eDestFitB,
  eDestFitBH,
  eDestFitBV,
} eDESTSPEC;

typedef enum
{
  eNoPageMark       = 0x00,
  ePageNum          = 0x01,
  ePageCount        = 0x02,
} ePAGEMARKTYPE;

typedef enum
{
  eTitle = 1,
  eAuthor,
  eSubject,
  eKeywords,
  eCreator,
  eProducer,
  eCreationDate,
  eModDate
} eDOCINFO;

typedef struct
{
  char        czTitle[NAME_SIZE];                   //The document's title.
  char        czAuthor[NAME_SIZE];                  //The name of the person who created the document.
  char        czSubject[NAME_SIZE];                 //The subject of the document.
  char        czKeywords[NAME_SIZE];                //Keywords associated with the document.
  char        czCreator[NAME_SIZE];                 //If the document was converted into a PDF document from
                                                    //another form, this is name of the application that created
                                                    //the original document.
  char        czProducer[NAME_SIZE];                //The name of the application that converted the document
                                                    //from its native format to PDF.
  char        czCreationDate[DATE_SIZE];            //The date the document was created.
  char        czModDate[DATE_SIZE];                 //The date the document was last modified.
} PDFInfo, *PPDFInfo;

typedef struct
{
  bool_a          bHideToolbar;
  bool_a          bHideMenubar;
  bool_a          bHideWindowUI;
  bool_a          bFitWindow;
  bool_a          bCenterWindow;
  ePAGEMODEATTR   eNonFullScreenPageMode;
  bool_a          bDirection;
} PDFViewerPreferences, *PPDFViewerPreferences;

typedef struct
{
  long lElement;
} PDFOutlineTree, *PPDFOutlineTree;

typedef struct
{
//  PPDFNamedDestinations   pNamedDestinations;
  PPDFOutlineTree         pOutlineTree;
//  PPDFArticleThreads      pArcicleThreads;
//  PPDFAcroForm            pAcroForm;
//  PPDFNames               pNames;
//  PPDFOpenAction          pOpenAction;
  ePAGEMODEATTR           ePageMode;
//  PPDFURI                 pURI;
  PDFViewerPreferences    strViewerPreferences;
//  PPDFPageLabels          pPageLabels;
  ePAGELAYOUTATTR         ePageLayout;
//  float                   fJavaScript;
} PDFCatalog, *PPDFCatalog;

typedef struct
{
  long lLength;
  long lHintOff;
  long lHintLen;
  long lPageObjNum;
  long lPageEndOff;
  long lPagesNum;
  long lWSOff;
} PDFLinearized;



#define M_SOF0  0xC0    /* Start Of Frame N */
#define M_SOF1  0xC1    /* N indicates which compression process */
#define M_SOF2  0xC2    /* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5    /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8    /* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9    /* End Of Image (end of datastream) */
#define M_SOS   0xDA    /* Start Of Scan (begins compressed data) */
#define M_APP0  0xE0    /* Application-specific marker, type N */
#define M_APP12  0xEC    /* (we don't bother to list all 16 APPn's) */
#define M_COM   0xFE    /* COMment */



#define PI    3.141592654


#define DISPLAY_PORTRAIT                          0
#define DISPLAY_LANDSCAPE                         270
#define DISPLAY_PORTRAIT_ROT                      180
#define DISPLAY_LANDSCAPE_ROT                     90


static char *pczColorSpaces[] =
{
  "DeviceGray",
  "DeviceGray",
  "DeviceGray",
  "DeviceRGB",
  "DeviceCMYK"
};

static char *pczInvArray[] =
{
  "[1 0]",
  "[1 0]",
  "[1 0]",
  "[1 0 1 0 1 0]",
  "[1 0 1 0 1 0]"
};


#define JPEG_IMG    0
#define PNG_IMG     1

#define BINARY_READ      "rb"
#define BINARY_WRITE      "wb"

#define MIN_BUF_SIZE     25600
#define MAX_BUF_BUMP     600000


typedef enum
{
  PDF_Root,
  PDF_Catalog,
  PDF_Outlines,
  PDF_Pages,
  PDF_Page,
  PDF_Contents,
  PDF_ProcSet,
  PDF_Annots,
  PDF_Info,
  PDF_Encrypt,
  PDF_Linearized,
  PDF_Hint,
} CPDFobjTypes;

//this is representation of matrix 3x3:
//
//    fA  fB  0
//    fC  fD  0
//    fE  fF  1
//

class CPDFVector
{
public:
  CPDFVector(float i_fX, float i_fY)
  {
    m_fX = i_fX;
    m_fY = i_fY;
  };
  //bool operator()(const CPDFVector vct1, const CPDFVector vct2) const
  //{
  //  if ((vct1.fX / vct2.fX) == (vct1.fY / vct2.fY))
  //  {
  //    if (vct1.fY > vct2.fY)
  //      return true;
  //    else if (vct1.fY < vct2.fY)
  //      return false;
  //    if (vct1.fX > vct2.fX)
  //      return true;
  //    else if (vct1.fX < vct2.fX)
  //      return false;
  //  }

  //  return false;
  //}

  float GetX() const {return m_fX;};
  float GetY() const {return m_fY;};
  void SetX(float fX) {m_fX = fX;};
  void SetY(float fY) {m_fY = fY;};

  bool_a IsParallel(const CPDFVector& i_vct) const
  {
    float fT1 = 0.f;
    if (m_fX == 0.f)
    {
      if (i_vct.GetX() != 0.f)
        return false_a;
    }
    else
      fT1 = m_fX / i_vct.GetX();

    float fT2 = 0.f;
    if (m_fY == 0.f)
    {
      if (i_vct.GetY() != 0.f)
        return false_a;
    }
    else
      fT2 = m_fY / i_vct.GetY();

    if (fT1 != fT2 && fT1 != 0.f && fT2 != 0.f)
      return false_a;
    return true_a;
  };

private:

  float m_fX;
  float m_fY;
};

class PDFCTM
{
public:
  PDFCTM()
    :m_vct(0.f, 0.f)
  {};
  PDFCTM(float fA, float fB, float fC, float fD, float fE, float fF, float vctX, float vctY)
    :m_vct(vctX, vctY)
  {
    m_fA = fA; m_fB = fB;m_fC = fC;m_fD = fD;m_fE = fE;m_fF=fF;
  };
  bool operator()(const PDFCTM &ctm1, const PDFCTM &ctm2) const
  {
    ////vertical text
    //if (ctm1.m_vct.GetX() == 0.f && ctm2.m_vct.GetX() != 0.f)
    //  return true;
    //if (ctm1.m_vct.GetX() != 0.f && ctm2.m_vct.GetX() == 0.f)
    //  return false;
    //if (ctm1.m_vct.GetX() == 0.f && ctm2.m_vct.GetX() == 0.f)
    //{
    //  if (ctm1.m_fE < ctm2.m_fE)
    //    return true;
    //  else if (ctm1.m_fE > ctm2.m_fE)
    //    return false;
    //  if (ctm1.m_fF > ctm2.m_fF)
    //    return true;
    //  else if (ctm1.m_fF < ctm2.m_fF)
    //    return false;
    //  return false;
    //}
    //horizontal text
    if (ctm1.m_vct.GetY() == 0.f && ctm2.m_vct.GetY() != 0.f)
      return true;
    if (ctm1.m_vct.GetY() != 0.f && ctm2.m_vct.GetY() == 0.f)
      return false;
    if (ctm1.m_vct.GetY() == 0.f && ctm2.m_vct.GetY() == 0.f)
    {
      if (ctm1.m_fF > ctm2.m_fF)
        return true;
      else if (ctm1.m_fF < ctm2.m_fF)
        return false;
      if (ctm1.m_fE < ctm2.m_fE)
        return true;
      else if (ctm1.m_fE > ctm2.m_fE)
        return false;
      return false;
    }
    //vertical text
    if (ctm1.m_vct.GetX() == 0.f && ctm2.m_vct.GetX() != 0.f)
      return true;
    if (ctm1.m_vct.GetX() != 0.f && ctm2.m_vct.GetX() == 0.f)
      return false;
    if (ctm1.m_vct.GetX() == 0.f && ctm2.m_vct.GetX() == 0.f)
    {
      if (ctm1.m_fE < ctm2.m_fE)
        return true;
      else if (ctm1.m_fE > ctm2.m_fE)
        return false;
      if (ctm1.m_fF > ctm2.m_fF)
        return true;
      else if (ctm1.m_fF < ctm2.m_fF)
        return false;
      return false;
    }

    if (!ctm1.InLineVector(ctm2))
    {
      if (ctm1.m_vct.GetY() < ctm2.m_vct.GetY())
        return true;
      else if (ctm1.m_vct.GetY() > ctm2.m_vct.GetY())
        return false;
      if (ctm1.m_vct.GetX() > ctm2.m_vct.GetX())
        return true;
      else if (ctm1.m_vct.GetX() < ctm2.m_vct.GetX())
        return false;
      return false;
    }

    if (ctm1.m_fF > ctm2.m_fF)
      return true;
    else if (ctm1.m_fF < ctm2.m_fF)
      return false;
    if (ctm1.m_fE < ctm2.m_fE)
      return true;
    else if (ctm1.m_fE > ctm2.m_fE)
      return false;
    return false;
  };

  bool_a InLineVector(const PDFCTM& ctm) const
  {
    float fT1 = 0.f;
    if (m_vct.GetX() == 0.f)
    {
      if (ctm.m_vct.GetX() != 0.f)
        return false_a;
      if (m_fE != ctm.m_fE)
        return false_a;
    }
    else
    {
      fT1 = (ctm.m_fE - m_fE) / m_vct.GetX();
    }

    float fT2 = 0.f;
    if (m_vct.GetY() == 0.f)
    {
      if (ctm.m_vct.GetY() != 0.f)
        return false_a;
      if (m_fF != ctm.m_fF)
        return false_a;
    }
    else
    {
      fT2 = (ctm.m_fF - m_fF) / m_vct.GetY();
    }

    if (fT1 != fT2 && fT1 != 0.f && fT2 != 0.f)
      return false_a;
    return true_a;
  }

  CPDFVector m_vct;

  float m_fA;
  float m_fB;
  float m_fC;
  float m_fD;
  float m_fE;
  float m_fF;
};

struct PdfRgb
{
  float fR;
  float fG;
  float fB;
};


struct _pdf_list
{
  long     lValue;
  PDFList  *pNext;
};

struct _pdf_outline_entry
{
  long              lObjectIndex;
  long              lCount;
  long              lPage;
  bool_a            bOpen;
  char              *pDestSpec;
  char              *pTitle;
  long              lTitleLength;
  float             fCkeyR;
  float             fCkeyG;
  float             fCkeyB;
  short             sFkey;
  long              lFont;
  PDFOutlineEntry   *pParent;
  PDFOutlineEntry   *pPrev;
  PDFOutlineEntry   *pNext;
  PDFOutlineEntry   *pFirst;
  PDFOutlineEntry   *pLast;
};

typedef enum
{
  eAnnotText = 0,
  eAnnotLink,
  eAnnotFreeText,
  eAnnotLine,
  eAnnotSquare,
  eAnnotCircle,
  eAnnotPolygon,
  eAnnotPolyline,
  eAnnotHighlight,
  eAnnotUnderline,
  eAnnotSquiggly,
  eAnnotStrikeOut,
  eAnnotStamp,
  eAnnotCaret,
  eAnnotInk,
  eAnnotPopup,
  eAnnotFileAttachment,
  eAnnotSound,
  eAnnotMovie,
  eAnnotWidget,
  eAnnotScreen,
  eAnnotPrinterMark,
  eAnnotTrapNet
} eANNOTATIONTYPE;

static const char *pczANNOTATIONTYPE[] =
{
  ("Text"),
  ("Link"),
  ("FreeText"),
  ("Line"),
  ("Square"),
  ("Circle"),
  ("Polygon"),
  ("Polyline"),
  ("Highlight"),
  ("Underline"),
  ("Squiggly"),
  ("StrikeOut"),
  ("Stamp"),
  ("Caret"),
  ("Ink"),
  ("Popup"),
  ("FileAttachment"),
  ("Sound"),
  ("Movie"),
  ("Widget"),
  ("Screen"),
  ("PrinterMark"),
  ("TrapNet"),
};


typedef enum
{
  eNone = -1,
  eButton = 0,
  eText,
  eChoice,
  eSignature
} eDICTFIELDTYPE;

static const char *pczDICTFIELDTYPE[] =
{
  ("Btn"),
  ("Tx"),
  ("Ch"),
  ("Sig")
};

class XPUBPDF_EXPORTIMPORT PDFAnnotation
{
public:
  PDFAnnotation();
  ~PDFAnnotation();

  long            lObjIndex[6]; //1 root obj and max 4 child obj
//  char            *pContents;
//  long            lPageObj;
  rec_a          recAnnot;                                   //      /Rect
  char            *pczAnnotName;                              //      /T
//  char            *pczAnnotDate;
  int             iAnnotFlags;                                //      /F
//  PDFBorderStyle  *pAnnotBorderStyle;
  char            *pczAnnotBorder;
  long            lFont;
  float           fFontSize;
// /AP
//    /AS
//  float           fAnnotR;
//  float           fAnnotG;
//  float           fAnnotB;
//  PDFAction       *pAnnotAction;
  char            *pczAction;
// /AA
// /StructParent
// /OC
  char            *pDestSpec;
  float           m_fZoom;
// /H
// /PA
  long            lDestPage;

  //MK dictionary
  char              *pczCA;                                   //      /MK - /CA
  CPDFColor         m_ColorBC;                                    //      /MK - /BC
  CPDFColor         m_ColorBG;                                    //      /MK - /BG

  // AP !!!
  // AS !!!
  // BS !!!
  short             sBorderWidth;                             //      /W

  CPDFColor         m_ColorDA;                                 //      /DA - color
//  char              *pczDA;                                   //      /DA
  long              lFieldFlags;                              //      /Ff
  long              lQ;                                       //      /Q
  char              *pczShortDesc;                            //      /TU
  char              *pczValue;                                //      /DV /V
  long              lMaxLen;                                  //      /MaxLen
  eANNOTATIONTYPE   eAnnotType;                               //      /Subtype
  eDICTFIELDTYPE    eDictFieldType;                           //      /FT
};

/* Type of image reference */
typedef enum {pdf_ref_direct, pdf_ref_file, pdf_ref_url} pdf_ref_type;

typedef unsigned char pdf_colormap[256][3];  /* colormap */
typedef enum { none, lzw, runlength, ccitt, dct, flate } pdf_compression;
typedef enum { pred_default = 1, pred_tiff = 2, pred_png = 15 } pdf_predictor;
typedef enum {
    DeviceGray = 0, DeviceRGB, DeviceCMYK, 
    CalGray, CalRGB, Lab, 
    Indexed, PatternCS, Separation,
    ImageMask, LastCS
} pdf_colorspace;
static const char *pdf_colorspace_names[] = {
    "DeviceGray", "DeviceRGB", "DeviceCMYK", "CalGray", "CalRGB", "Lab", 
    "Indexed", "Pattern", "Separation", "Invalid_CS"
};
static const char *pdf_filter_names[] = {
    "", "LZWDecode", "RunLengthDecode", "CCITTFaxDecode",
    "DCTDecode", "FlateDecode"
};


typedef struct
{
  long      lObjIndex;
  long      lColorSpaceObjIndex;
  char *name;    /* Im0, Im1, etc */
  int type;    /* image file type */
  int imagemask;    /* 1 if this image is /ImageMask, 0 if not */
  int invert;    /* invert B/W or gray */
  int process;    /* M_SOF# -- jpeg process or TIFF compression type  */
  int NumStrips;    /* number of strips (only for TIFF) */
  int photometric;  /* photometric interpretation for tiff */
  int orientation;  /* orientation tag in TIFF file (not always present, normally 1) */
  int width;    /* # of pixels horizontal */
  int height;    /* # of pixels vertical */
  int ncomponents;  /* # of color components */
  int bitspersample;  /* bits per sample */
  int Xdimension;
  int Ydimension;
  long filesize;    /* # of bytes in file or data */
  char *filepath;    /* path to image file */
  unsigned char *data;
  long lIndex;

  bool_a          bUsePNG;

  size_t                startpos;
  pdf_ref_type          reference;        /* kind of image data reference */
  int                   components;       /* number of color components */
  pdf_compression       compression;      /* image compression type */
  pdf_colorspace        colorspace;       /* image color space */
  bool_a                transparent;      /* image is transparent */
  unsigned char         transval[4];      /* transparent color values */
  int                   mask;             /* image number of image mask, or -1 */
  pdf_predictor         predictor;        /* predictor for lzw and flate */
  int                   palette_size;     /* # of palette entries (not bytes!) */
  pdf_colormap          *colormap;        /* pointer to color palette */
  int                   strips;           /* number of strips in image */
  int                   rowsperstrip;     /* number of rows per strip */
  size_t                nbytes;
  unsigned long         rowbytes;
  unsigned char         *raster;
  bool_a                use_raw;          /* use raw (compressed) image data */
} PDFImage;

typedef struct
{
  char    *pStream;
  size_t  curSize;
  size_t  allocSize;
} PDFStream;

typedef struct
{
  PDFStream     *pPageStream;
  long          lPageNum;
  long          lObjIndex;
  long          lParent;
  long          lContIndex;
  bool_a        bCompressed;
  long          lRotation;
  rec_a         recMediaBox;
  rec_a         recCropBox;
  long          lFontCount;
  long          lImageCount;
  long          lAnnotCount;
  PDFList       *pFontIdx;
  PDFList       *pImageIdx;
  PDFList       *pAnnotIdx;
  long          lCompLength;
  unsigned char *pucCompStream;

  long          lObjOffset;

//Resources;
//Thumb;
//Annots;
//Beads;
//lHidden;
//PieceInfo;
//LastModified;
//SeparationInfo;
//recArtBox;
//recTrimBox;
//recBleedBox;
//czID[ID_NAME_SIZE];
//fPZ;

} PDFPage;

struct _pdf_tab_stop
{
  short     sTabFlag;
  long      lTabPos;
};

class PDFLineElemList
{
public:

  PDFLineElemList();
  ~PDFLineElemList();
  void operator=(const PDFLineElemList& r);

  CPDFColor         m_Color;
  CPDFColor         m_BgColor;
  long              lFont;
  float             fFontSize;
  long              lElemWidth;
  long              lElemHeight;
  long              lLetterSpace;
  long              lSpace;
  long              lImage;
  long              lAnnot;
  long              lFillBreak;
  rec_a             recImageDist;
  bool_a            bBlockEnd;
  bool_a            bCanPageBreak;
  bool_a            bTBBreak;
  bool_a            bColumnBreak;
  std::string       m_strAnchor;
  ePAGEMARKTYPE     eElemPageMark;
  short             sTabStop;
//  char              *pStr;
  CPDFString        *m_pStr;
  PDFLineElemList   *pLCNext;
  PDFLineElemList   *pLCPrev;
};

struct _pdf_line
{
  long              lCurrLineWidth;
  long              lCurrLineHeight;
  long              lCurrLineAsc;
  long              lCurrLineDesc;
  short             sLastBlock;
  short             sLastTab;
  bool_a            bNewLine;
  PDFLineElemList   *pElemList;
  PDFLine           *pLineNext;
  PDFLine           *pLinePrev;
};

struct _pdf_paragraph
{
  bool_a            m_bKeepLinesTogether;
  bool_a            m_bKeepWithNext;
  bool_a            m_bWidowOrphan;
  bool_a            m_bParagraphPageBreak;
  rec_a             recIndents;
  long              lIndent;
  short             sAlignment;
  long              lParaWidth;
  long              lParaHeight;
  long              lLineSpace;
  PDFTabStop        *pParaTabStops;
  short             sParaTabsCount;
  long              lParaTabSize;
  char              cParaTabDecimalType;
  bool_a            bParaContinue;
  PDFLine           *pPDFLine;
  PDFLine           *pPDFLastLine;
  PDFParagraph      *pParaNext;
  PDFParagraph      *pParaPrev;
};

struct _pdf_tb_image
{
  rec_a         recPos;
  rec_a         recDist;
  long          lImage;
  PDFTBImage    *pTBImageNext;
  PDFTBImage    *pTBImagePrev;
};

typedef enum
{
  eFixImage = 0,
  eFixFrame,
  eFrame,
  eFixTableWidth,
  eFixEmptySpace,
} eSPACETYPE;

typedef long (*PDFFUNC)(void *, eSPACETYPE, long, long, long, bool_a, void *);

struct _pdf_tb_space
{
  rec_a         recSpacePos;
  eSPACETYPE    eSpaceType;
  PDFTBSpace    *pTBSpaceNext;
  PDFTBSpace    *pTBSpacePrev;
  long          lSpaceID;
};

typedef enum
{
  eHorizontal   = 1,
  eVerticalUp   = 2,
  eVerticalDown = 3
} eTEXTDIRECTION;

struct PDFDoc
{
public:
  PDFDoc* GetStruct(){return this;};
  FILE                    *pOutputFile;
  PDFStream               *pDocStream;
  bool_a                  bFlushToFile;
  std::string             m_strFileName;
  long                    lMajorLevel;
  long                    lMinorLever;
  PDFInfo                 strInfo;
  PDFCatalog              strCatalog;
  PDFLinearized           strLinearized;
  long                    lActualSize;
  long                    *pObjOffset;
  long                    *pObjIndex;
  int                     iCurrAllocObjects;

  std::vector<PDFPage *>  m_vctPages;
  long                    m_lActualPage;


  PDFFUNC                 pDocFunc;

  std::vector<PDFAnnotation *>  m_vctAnnots; // TO DO: memory leak if annot is not eAnnotLink

//Images
  std::vector<CPDFImage *> m_vctImages;
  int imageFlagBCI;/* bit-0 (/ImageB), bit-1 (/ImageC), bit-2 (/ImageI) */


  long lOutlineEntriesCount;
  PDFOutlineEntry *pFirstOutlineEntry;
  PDFOutlineEntry *pLastOutlineEntry;


  PDFCTM      strTextCTM;
  float       fWordSpacing;
  long        lDefTabSpace;
  long        lLetterSpace;
  char        cTabDecimalType;
  bool_a    bTextObj;        /* flag indicating within Text block between BT ET */

  int compressionON;        /* compress stream */
  char *streamFilterList;      /* for PDF stream /Filter spec */
  int   hex_string;        /* current string mode: If non-zero HEX */
  long horiz_scaling;      /* text horizontal scaling in percent */
  long startXref;        /* offset of xref */

  unsigned char     ucOwnerPassw[33];
  unsigned char     ucUserPassw[33];
  unsigned char     ucEncrKey[33];
  unsigned char     ucO[33];
  unsigned char     ucU[33];
  unsigned char     ucFileID[33];
  int               iPermission;

};

static char *pczDEF_AFM_FONTS_SHORTCUT[] =
{
  "Helv", //Helvetica
  "HeBo", //Helvetica-Bold
  "HeOb", //Helvetica-Oblique
  "HeBO", //Helvetica-BoldOblique
  "TiRo", //Times-Roman
  "TiBo", //Times-Bold
  "TiIt", //Times-Italic
  "TiBI", //Times-BoldItalic
  "Cour", //Courier
  "CoBo", //Courier-Bold
  "CoOb", //Courier-Oblique
  "CoBO", //Courier-BoldOblique
  "ZaDb", //ZapfDingbats
  "Symb" //Symbol
};


static short encode2WinAnsi[][256] = {
/* 0 MacRomanEncoding - OK */
  {
    /*   0 */    0,    1,    2,    3,    4,    5,    6,    7,    8,    9, 
    /*  10 */   10,   11,   12,   13,   14,   15,   16,   17,   18,   19, 
    /*  20 */   20,   21,   22,   23,   24,   25,   26,   27,   28,   29, 
    /*  30 */   30,   31,   32,   33,   34,   35,   36,   37,   38,   39, 
    /*  40 */   40,   41,   42,   43,   44,   45,   46,   47,   48,   49, 
    /*  50 */   50,   51,   52,   53,   54,   55,   56,   57,   58,   59, 
    /*  60 */   60,   61,   62,   63,   64,   65,   66,   67,   68,   69, 
    /*  70 */   70,   71,   72,   73,   74,   75,   76,   77,   78,   79, 
    /*  80 */   80,   81,   82,   83,   84,   85,   86,   87,   88,   89, 
    /*  90 */   90,   91,   92,   93,   94,   95,   96,   97,   98,   99, 
    /* 100 */  100,  101,  102,  103,  104,  105,  106,  107,  108,  109, 
    /* 110 */  110,  111,  112,  113,  114,  115,  116,  117,  118,  119, 
    /* 120 */  120,  121,  122,  123,  124,  125,  126,  127,  196,  197, 
    /* 130 */  199,  201,  209,  214,  220,  225,  224,  226,  228,  227, 
    /* 140 */  229,  231,  233,  232,  234,  235,  237,  236,  238,  239, 
    /* 150 */  241,  243,  242,  244,  246,  245,  250,  249,  251,  252, 
    /* 160 */  134,  268,  162,  163,  167,  127,  182,  223,  174,  169, 
    /* 170 */  153,  180,  168,    0,  198,  216,    0,  177,    0,    0, 
    /* 180 */  165,  181,    0,    0,    0,    0,    0,  170,  186,    0, 
    /* 190 */  230,  248,  191,  161,  172,    0,  131,    0,    0,  171, 
    /* 200 */  187,  133,    0,  192,  195,  213,  140,  156,  150,  151, 
    /* 210 */  147,  148,    0,  146,  247,    0,  255,  159,  263,  164, 
    /* 220 */  139,  155,  261,  262,  135,  183,  130,  132,  137,  194, 
    /* 230 */  202,  193,  203,  200,  205,  206,  207,  204,  211,  212, 
    /* 240 */    0,  210,  218,  219,  217,  260,  136,  152,  175,  257, 
    /* 250 */  259,  176,  184,  264,  267,  258
  },

/* 1 MacExpertEncoding */
/* DO NOT USE MacExpertEncoding -- Look-up table is not implemented yet because of buggy
   data (it seems) of PDF Spec.
   Page 459-460, PDF Spec v1.3 (C.3 MacExpert Encoding description seems extremely buggy)
   Giving up until better data are obtained.
*/

  {
    /*   0 */    0,    1,    2,    3,    4,    5,    6,    7,    8,    9, 
    /*  10 */   10,   11,   12,   13,   14,   15,   16,   17,   18,   19, 
    /*  20 */   20,   21,   22,   23,   24,   25,   26,   27,   28,   29, 
    /*  30 */   30,   31,   32,   33,   34,   35,   36,   37,   38,   39, 
    /*  40 */   40,   41,   42,   43,   44,   45,   46,   47,   48,   49, 
    /*  50 */   50,   51,   52,   53,   54,   55,   56,   57,   58,   59, 
    /*  60 */   60,   61,   62,   63,   64,   65,   66,   67,   68,   69, 
    /*  70 */   70,   71,   72,   73,   74,   75,   76,   77,   78,   79, 
    /*  80 */   80,   81,   82,   83,   84,   85,   86,   87,   88,   89, 
    /*  90 */   90,   91,   92,   93,   94,   95,   96,   97,   98,   99, 
    /* 100 */  100,  101,  102,  103,  104,  105,  106,  107,  108,  109, 
    /* 110 */  110,  111,  112,  113,  114,  115,  116,  117,  118,  119, 
    /* 120 */  120,  121,  122,  123,  124,  125,  126,  127,  128,  129, 
    /* 130 */  130,  131,  132,  133,  134,  135,  136,  137,  138,  139, 
    /* 140 */  140,  141,  142,  143,  144,  145,  146,  147,  148,  149, 
    /* 150 */  150,  151,  152,  153,  154,  155,  156,  157,  158,  159, 
    /* 160 */  160,  161,  162,  163,  164,  165,  166,  167,  168,  169, 
    /* 170 */  170,  171,  172,  173,  174,  175,  176,  177,  178,  179, 
    /* 180 */  180,  181,  182,  183,  184,  185,  186,  187,  188,  189, 
    /* 190 */  190,  191,  192,  193,  194,  195,  196,  197,  198,  199, 
    /* 200 */  200,  201,  202,  203,  204,  205,  206,  207,  208,  209, 
    /* 210 */  210,  211,  212,  213,  214,  215,  216,  217,  218,  219, 
    /* 220 */  220,  221,  222,  223,  224,  225,  226,  227,  228,  229, 
    /* 230 */  230,  231,  232,  233,  234,  235,  236,  237,  238,  239, 
    /* 240 */  240,  241,  242,  243,  244,  245,  246,  247,  248,  249, 
    /* 250 */  250,  251,  252,  253,  254,  255
  },

/* 2 StandardEncoding - OK */
  {
    /*   0 */    0,    1,    2,    3,    4,    5,    6,    7,    8,    9, 
    /*  10 */   10,   11,   12,   13,   14,   15,   16,   17,   18,   19, 
    /*  20 */   20,   21,   22,   23,   24,   25,   26,   27,   28,   29, 
    /*  30 */   30,   31,   32,   33,   34,   35,   36,   37,   38,  146, 
    /*  40 */   40,   41,   42,   43,   44,   45,   46,   47,   48,   49, 
    /*  50 */   50,   51,   52,   53,   54,   55,   56,   57,   58,   59, 
    /*  60 */   60,   61,   62,   63,   64,   65,   66,   67,   68,   69, 
    /*  70 */   70,   71,   72,   73,   74,   75,   76,   77,   78,   79, 
    /*  80 */   80,   81,   82,   83,   84,   85,   86,   87,   88,   89, 
    /*  90 */   90,   91,   92,   93,   94,   95,  145,   97,   98,   99, 
    /* 100 */  100,  101,  102,  103,  104,  105,  106,  107,  108,  109, 
    /* 110 */  110,  111,  112,  113,  114,  115,  116,  117,  118,  119, 
    /* 120 */  120,  121,  122,  123,  124,  125,  126,    0,    0,    0, 
    /* 130 */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
    /* 140 */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
    /* 150 */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
    /* 160 */    0,  161,  162,  163,  263,  165,  131,  167,  164,   39, 
    /* 170 */  147,  171,  139,  155,  261,  262,    0,  150,  134,  135, 
    /* 180 */  183,    0,  182,  127,  130,  132,  148,  187,  133,  137, 
    /* 190 */    0,  191,    0,   96,  180,  136,  152,  175,  257,  259, 
    /* 200 */  168,    0,  176,  184,    0,  264,  267,  258,  151,    0, 
    /* 210 */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
    /* 220 */    0,    0,    0,    0,    0,  198,    0,  170,    0,    0, 
    /* 230 */    0,    0,  256,  216,  140,  186,    0,    0,    0,    0, 
    /* 240 */    0,  230,    0,    0,    0,  260,    0,    0,  265,  248, 
    /* 250 */  156,  223,    0,    0,    0,    0
  },

/* 3 PDFDocEncoding -- Not Used */
  {
    /*   0 */    0,    1,    2,    3,    4,    5,    6,    7,    8,    9, 
    /*  10 */   10,   11,   12,   13,   14,   15,   16,   17,   18,   19, 
    /*  20 */   20,   21,   22,   23,   24,   25,   26,   27,   28,   29, 
    /*  30 */   30,   31,   32,   33,   34,   35,   36,   37,   38,   39, 
    /*  40 */   40,   41,   42,   43,   44,   45,   46,   47,   48,   49, 
    /*  50 */   50,   51,   52,   53,   54,   55,   56,   57,   58,   59, 
    /*  60 */   60,   61,   62,   63,   64,   65,   66,   67,   68,   69, 
    /*  70 */   70,   71,   72,   73,   74,   75,   76,   77,   78,   79, 
    /*  80 */   80,   81,   82,   83,   84,   85,   86,   87,   88,   89, 
    /*  90 */   90,   91,   92,   93,   94,   95,   96,   97,   98,   99, 
    /* 100 */  100,  101,  102,  103,  104,  105,  106,  107,  108,  109, 
    /* 110 */  110,  111,  112,  113,  114,  115,  116,  117,  118,  119, 
    /* 120 */  120,  121,  122,  123,  124,  125,  126,  127,  128,  129, 
    /* 130 */  130,  131,  132,  133,  134,  135,  136,  137,  138,  139, 
    /* 140 */  140,  141,  142,  143,  144,  145,  146,  147,  148,  149, 
    /* 150 */  150,  151,  152,  153,  154,  155,  156,  157,  158,  159, 
    /* 160 */  160,  161,  162,  163,  164,  165,  166,  167,  168,  169, 
    /* 170 */  170,  171,  172,  173,  174,  175,  176,  177,  178,  179, 
    /* 180 */  180,  181,  182,  183,  184,  185,  186,  187,  188,  189, 
    /* 190 */  190,  191,  192,  193,  194,  195,  196,  197,  198,  199, 
    /* 200 */  200,  201,  202,  203,  204,  205,  206,  207,  208,  209, 
    /* 210 */  210,  211,  212,  213,  214,  215,  216,  217,  218,  219, 
    /* 220 */  220,  221,  222,  223,  224,  225,  226,  227,  228,  229, 
    /* 230 */  230,  231,  232,  233,  234,  235,  236,  237,  238,  239, 
    /* 240 */  240,  241,  242,  243,  244,  245,  246,  247,  248,  249, 
    /* 250 */  250,  251,  252,  253,  254,  255
  }
};

#define RESOURES_KEY_NUM                 8
static char *pczRESOURES_KEY[][3] =
{
  {"/ExtGState", "<<", ">>"},
  {"/ColorSpace", "<<", ">>"},
  {"/Pattern", "<<", ">>"},
  {"/Shading", "<<", ">>"},
  {"/XObject", "<<", ">>"},
  {"/Font", "<<", ">>"},
  {"/Properties", "<<", ">>"},
  {"/ProcSet", "[", "]"}
};

#define ACROFORM_KEY_NUM                 3 //now is used only 3 keys
static char *pczACROFORM_KEY[][3] =
{
  {"/Fields", "[", "]"},
  {"/DR", "<<", ">>"},
  {"/DA", "(", ")"}
};


#define BT_OPERATOR_POS                 30
#define ET_OPERATOR_POS                 31
#define Td_OPERATOR_POS                 39
#define TD_OPERATOR_POS                 40
#define Tm_OPERATOR_POS                 41

#define T_STAR_OPERATOR_POS             42
#define APOSTROPHE_OPERATOR_POS         45
#define QUOTES_OPERATOR_POS             46

#define Do_OPERATOR_POS                 65


static const char *pczOperatorsList[] = 
{
  //General graphics state                                          ***
  ("w"),      //Set line width
  ("J"),      //Set line cap style
  ("j"),      //Set line join style
  ("M"),      //Set miter limit
  ("ri"),     //Set color rendering intent
  ("i"),      //Set flatness tolerance
  ("gs"),     //(PDF 1.2) Set parameters from graphics state parameter dictionary
  ("d"),      //Set line dash pattern
  //Special graphics state
  ("q"),      //Save graphics state
  ("Q"),      //Restore graphics state
  ("cm"),     //Concatenate matrix to current transformation matrix
  //Path construction
  ("m"),      //Begin new subpath
  ("l"),      //Append straight line segment to path
  ("c"),      //Append curved segment to path (three control points)
  ("v"),      //Append curved segment to path (initial point replicated)
  ("y"),      //Append curved segment to path (final point replicated)
  ("h"),      //Close subpath
  ("re"),     //Append rectangle to path
  //Path painting
  ("S"),      //Stroke path
  ("s"),      //Close and stroke path
  ("f"),      //Fill path using nonzero winding number rule
  ("F"),      //Fill path using nonzero winding number rule (obsolete)
  ("f*"),     //Fill path using even-odd rule
  ("B"),      //Fill and stroke path using nonzero winding number rule
  ("B*"),     //Fill and stroke path using even-odd rule
  ("b"),      //Close, fill, and stroke path using nonzero winding number rule
  ("b*"),     //Close, fill, and stroke path using even-odd rule
  ("n"),      //End path without filling or stroking
  //Clipping paths
  ("W"),      //Set clipping path using nonzero winding number rule
  ("W*"),     //Set clipping path using even-odd rule
  //Text objects
  ("BT"),     //Begin text object
  ("ET"),     //End text object
  //Text state                                                      ***
  ("Tc"),     //Set character spacing
  ("Tw"),     //Set word spacing
  ("Tz"),     //Set horizontal text scaling
  ("TL"),     //Set text leading
  ("Tf"),     //Set text font and size
  ("Tr"),     //Set text rendering mode
  ("Ts"),     //Set text rise
  //Text positioning                                                ***
  ("Td"),     //Move text position
  ("TD"),     //Move text position and set leading
  ("Tm"),     //Set text matrix and text line matrix
  ("T*"),     //Move to start of next text line
  //Text showing                                                    ***
  ("Tj"),     //Show text
  ("TJ"),     //Show text, allowing individual glyph positioning
  ("'"),      //Move to next line and show text
  ("\""),     //Set word and character spacing, move to next line, and show text
  //Type 3 fonts
  ("d0"),     //Set glyph width in Type 3 font
  ("d1"),     //Set glyph width and bounding box in Type 3 font
  //Color                                                           ***
  ("CS"),     //(PDF 1.1) Set color space for stroking operations
  ("cs"),     //(PDF 1.1) Set color space for nonstroking operations
  ("SC"),     //(PDF 1.1) Set color for stroking operations
  ("SCN"),    //(PDF 1.2) Set color for stroking operations (ICCBased and special color spaces)
  ("sc"),     //(PDF 1.1) Set color for nonstroking operations
  ("scn"),    //(PDF 1.2) Set color for nonstroking operations (ICCBased and special color spaces)
  ("G"),      //Set gray level for stroking operations
  ("g"),      //Set gray level for nonstroking operations
  ("RG"),     //Set RGB color for stroking operations
  ("rg"),     //Set RGB color for nonstroking operations
  ("K"),      //Set CMYK color for stroking operations
  ("k"),      //Set CMYK color for nonstroking operations
  //Shading patterns
  ("sh"),     //(PDF 1.3) Paint area defined by shading pattern
  //Inline images
  ("BI"),     //Begin inline image object
  ("ID"),     //Begin inline image data
  ("EI"),     //End inline image object
  //XObjects
  ("Do"),     //Invoke named XObject
  //Marked content                                                  ***
  ("MP"),     //(PDF 1.2) Define marked-content point
  ("DP"),     //(PDF 1.2) Define marked-content point with property list
  ("BDC"),    //(PDF 1.2) Begin marked-content sequence with property list
  ("BMC"),    //(PDF 1.2) Begin marked-content sequence
  ("EMC"),    //(PDF 1.2) End marked-content sequence
  //Compatibility
  ("BX"),     //(PDF 1.1) Begin compatibility section
  ("EX"),     //(PDF 1.1) End compatibility section

  ("")        // End
};



#endif  /*  __PDF_DEF_H__  */
