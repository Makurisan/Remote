// TER_PDF.H This file is to be included in ter_pdf.c file.

#ifndef WPCONTROLDEF

// Tag to identify the PDF enviroment
typedef int WPDFEnviroment;
typedef char* pchar;

// Function definition for callbacks  
typedef void (__stdcall TWPDF_Callback)(WPDFEnviroment PdfEnv, int number, char *buffer, int bufsize);

// Structur to set up the PDF Engine with WPDF_InitializeEx
typedef struct 
  {
     int SizeOfStruct;
     // Encoding
     int Encoding; // 0 = none, 1=ASCII85, 2=Hex
     // Compression 0=none, deflate, runlength
     int Compression;
     // BitmapCompression
     int BitCompression; // 0=auto, 1=deflat, 2=jpeg
     // Thumbnailes
     int ThumbNails; // 0=none, 1=color, 2=monchrome
     // UseFonts
     int FontMode;
     // PageMode
     int PageMode;
     // InputFileMode
     int InputFileMode;
     // JPEGCompress 0=off, - = value, 1..5 for 10, 25, 50, 75, 100
     int JPEGCompress;
     int EnhancedOptions;
     // PDF Resolution
     int PDFXRes, PDFYRes; // 0->72
     // Default PDF Size (for StartPageA)
     int PDFWidth, PDFHeight; // Default DINa4
     // Read PDF Filename
     char InputPDFFile[128];
     // Several Callback functions
     // For Stream Output
     TWPDF_Callback* OnStreamOpenr;  // Stream Function
     TWPDF_Callback* OnStreamWrite;
     TWPDF_Callback* OnStreamClose;
     // Messages
     TWPDF_Callback* OnError;
     TWPDF_Callback* OnMessage;
     // Mailmerge, etc
     char MailMergeStart[8];
     TWPDF_Callback* OnGetText;
     // Encryption
     int Permission;
     char UserPassword[20], OwnerPassword[20]; // actually only 5 chars long!
     // Info
     char Date[256],
          ModDate[256],
          Author[256],
          Producer[256],
          Title[256],
          Subject[256],
          Keywords[256];
     // Reserved, must be 0
     int Reserved;
} WPDFInfoRecord;

// Methods defined in the DLL
WPDFEnviroment (WINAPI _export *WpInitializeEx)(WPDFInfoRecord *Info); // WPDFInfoRecord
WPDFEnviroment (WINAPI _export *WpInitialize)(void);
void (WINAPI _export *WpFinalize)(WPDFEnviroment PdfEnv);
void (WINAPI _export *WpFinalizeAll)(void);
void (WINAPI _export *WpSetResult)(WPDFEnviroment PdfEnv, int buffertype, char *buffer, int bufsize);

int (WINAPI _export *WpBeginDoc)(WPDFEnviroment PdfEnv, char *FileName, int UseStream);
void (WINAPI _export *WpEndDoc)(WPDFEnviroment PdfEnv);
void (WINAPI _export *WpStartPage)(WPDFEnviroment PdfEnv);
void (WINAPI _export *WpStartPageEx)(WPDFEnviroment PdfEnv, int Width, int Height, int Rotation);
void (WINAPI _export *WpEndPage)(WPDFEnviroment PdfEnv);
void (WINAPI _export *WpStartWatermark)(WPDFEnviroment PdfEnv, char *Name);
void (WINAPI _export *WpStartWatermarkEx)(WPDFEnviroment PdfEnv, char *Name, int Width, int Height);
void (WINAPI _export *WpEndWatermark)(WPDFEnviroment PdfEnv);
void (WINAPI _export *WpDrawWatermark)(WPDFEnviroment PdfEnv, char *Name, int Rotation);

// Property and command
void (WINAPI _export *WpSetSProp)(WPDFEnviroment PdfEnv, int id, char *Value);
void (WINAPI _export *WpSetIProp)(WPDFEnviroment PdfEnv, int id, int Value);
int (WINAPI _export *WpExecCommand)(WPDFEnviroment PdfEnv, int id, int Value,char *buffer,int buflen);

// PDF Output Functions
void (WINAPI _export *WpDrawMetafile)(WPDFEnviroment PdfEnv, unsigned int meta, int x, int y, int w, int h);
int (WINAPI _export *WpDrawDIB)(WPDFEnviroment PdfEnv, int x, int y, int w, int h, void *BitmapInfo, void *BitmapBits);
int (WINAPI _export *WpDrawBMP)(WPDFEnviroment PdfEnv, int x, int y, int w, int h, HBITMAP Bitmap);
int (WINAPI _export *WpDrawJPEG)(WPDFEnviroment PdfEnv, int x, int y, int w, int h, int jpeg_w, int jpeg_h, void *buffer, int buflen);
int (WINAPI _export *WpDrawBitmapFile)(WPDFEnviroment PdfEnv, int x, int y, int w, int h, char *FileName);
int (WINAPI _export *WpDrawBitmapClone)(WPDFEnviroment PdfEnv, int x, int y, int w, int h, int BitmapID);

// HDC Output function
HDC (WINAPI _export *WpDC)(WPDFEnviroment PdfEnv); //** Get the DC of the PDF Canvas! **//
void (WINAPI _export *WpTextOut)(WPDFEnviroment PdfEnv, int x, int y, char * Text);
char* (WINAPI _export *WpTextRect)(WPDFEnviroment PdfEnv, int x, int y, int w, int h, char *Text, int Alignment);

void (WINAPI _export *WpMoveTo)(WPDFEnviroment PdfEnv, int x, int y);
void (WINAPI _export *WpLineTo)(WPDFEnviroment PdfEnv, int x, int y);
void (WINAPI _export *WpRectangle)(WPDFEnviroment PdfEnv, int x, int y,int w,int h);

void (WINAPI _export *WpHyperlink)(WPDFEnviroment PdfEnv, int x, int y,int w,int h,char *Name);
void (WINAPI _export *WpBookmark)(WPDFEnviroment PdfEnv, int x, int y,char *Name);
void (WINAPI _export *WpOutline)(WPDFEnviroment PdfEnv, int level,  int x, int y,char *Name, char *Caption);

// Attribute funtions
void (WINAPI _export *WpSetTextDefaultAttr)(WPDFEnviroment PdfEnv, char *FontName, int Size);
void (WINAPI _export *WpSetTextAttr)(WPDFEnviroment PdfEnv, char *FontName, int Size, int Bold, int Italic,int Underline);
void (WINAPI _export *WpSetTextAttrEx)(WPDFEnviroment PdfEnv, char *FontName, int Charset,
int Size, int Bold, int Italic,int Underline, unsigned int Color);
void (WINAPI _export *WpSetPenAttr)(WPDFEnviroment PdfEnv, int Style, int Width, int Color);
void (WINAPI _export *WpSetBrushAttr)(WPDFEnviroment PdfEnv, int Style, int Color);

// Set License Key
void (WINAPI _export *WpSetLicenseKey)(unsigned long number, char *Name, char *Code);


  #define WPPDF_Author 1
  #define WPPDF_Date     2
  #define WPPDF_ModDate  3
  #define WPPDF_Producer 4
  #define WPPDF_Title    5
  #define WPPDF_Subject  6
  #define WPPDF_Keywords 7
  #define WPPDF_Creator  8
  #define WPPDF_IncludedFonts  9
  #define WPPDF_ExcludedFonts  10
  #define WPPDF_OwnerPassword  11
  #define WPPDF_UserPassword   12
  #define WPPDF_InputFile      13

  #define WPPDF_ENCODE  1
  #define WPPDF_COMPRESSION  2
  #define WPPDF_PAGEMODE  3
  #define WPPDF_USEFONTMODE  4
  #define WPPDF_Encryption  5
  #define WPPDF_InputFileMode   6
  #define WPPDF_MonochromeThumbnail  7
  #define WPPDF_JPEGCompress  8
  #define WPPDF_EnhancedOptions  9

 #define WPMSG_BeforeBeginDoc  1
 #define WPMSG_AfterEndDoc     2
 #define WPMSG_EmbedFont       3
 #define WPMSG_InputPDFFile    4
 #define WPMSG_AfterBeginDoc   24
 #define WPMSG_BeforeEndDoc    25
 #define WPMSG_AfterStartPage  26
 #define WPMSG_BeforeEndPage   27

 #define WPCOM_xxxx 0
 #define WPCOM_Version  1

 //Only used by RTF2PDF:
 #define WPCOM_RTFINIT       1000
 #define WPCOM_RTFVersion    1001
 #define WPCOM_RTFLOAD       1002
 #define WPCOM_RTFAPPEND     1003
 #define WPCOM_RTFMAKEFIELDS 1004
 #define WPCOM_RTFMERGE      1005
 #define WPCOM_RTFSAVE       1006
 #define WPCOM_RTFBACKUP     1007
 #define WPCOM_RTFRESTORE    1008

 #define WPCOM_RTF_PAGEWIDTH     1010
 #define WPCOM_RTF_PAGEHEIGHT    1011
 #define WPCOM_RTF_MARGINLEFT    1012
 #define WPCOM_RTF_MARGINRIGHT   1013
 #define WPCOM_RTF_MARGINTOP     1014
 #define WPCOM_RTF_MARGINBOTTOM  1015

 #define WPCOM_RTF_PAGECOLUMNS   1020
 #define WPCOM_RTF_PAGEROTATION  1021
 #define WPCOM_RTF_PAGEZOOM      1022
 #define WPCOM_RTFPRINT          1100


 #define WPMSG_RTFLOAD           1001
 #define WPMSG_RTFPRINTPAGECOUNT 1002
 #define WPMSG_RTFPRINTPAGE      1003

 // Message IDs for OnError
 #define WPERR_Bookmark  1 // Bookmark not found
 #define WPERR_Bitmap    2   // Bitmap Errpr
 #define WPERR_FileOpen  3 // Fileopen errr
 #define WPERR_Meta      4 // Metafile error
 #define WPERR_Font      5 // Font embedding error
 #define WPERR_InputPDF  6 // Not able to load PDF file (don't find it, wrong format)
 #define WPERR_BeginDocRequired 10 // Not inside of BeginDoc/EndDoc
 #define WPERR_StartPageRequired11 // Not inside of StartPage/EndPage or StartWatermark/EndWatermark
 #define WPERR_StreamMissing    12 // On OnStreamWrite method was specified



#define WPCONTROLDEF
#endif
