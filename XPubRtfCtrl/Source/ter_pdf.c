/*==============================================================================
   TER_BLK.C
   TER Block Copy/Move/Delete functions.

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1991)
   ----------------------------------
   This license agreement allows the purchaser the right to modify the
   source code to incorporate in an application larger than the editor itself.
   The target application must not be a programmer's utility 'like' a text editor.
   Sub Systems, Inc. reserves the right to prosecute anybody found to be
   making illegal copies of the executable software or compiling the source
   code for the purpose of selling there after.
===============================================================================*/

#define  PREFIX extern
#include "ter_hdr.h"
#include "ter_pdf.h"

//#define  LINK_PDC

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interace with SubSystems' PDF converter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HINSTANCE hPdc=NULL;

#if defined(LINK_PDC)
  #include "pdc.h"
#else
  DWORD (WINAPI _export *PdcStartDoc2)(LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,DWORD,DWORD,LPBYTE,LPBYTE);
  BOOL  (WINAPI _export *PdcEndDoc)(DWORD);
  HDC   (WINAPI _export *PdcStartPage)(DWORD,int,int);
  BOOL  (WINAPI _export *PdcEndPage)(DWORD);
#endif


/********************************************************************************************
    TerPdcStart:
    Load the pdf libaray, initialize enviroment and start the document
*********************************************************************************************/
DWORD WINAPI _export TerPdcStart(LPBYTE LicKey, LPBYTE OutFile, 
    LPBYTE author, LPBYTE producer, LPBYTE title, LPBYTE subject, LPBYTE keywords,
    LPBYTE date, LPBYTE ModDate, DWORD flags)
{
    return TerPdcStart2(LicKey,OutFile,author,producer,title,subject,keywords,
                        date,ModDate,flags,0,"","");
}
 
DWORD WINAPI _export TerPdcStart2(LPBYTE LicKey, LPBYTE OutFile, 
    LPBYTE author, LPBYTE producer, LPBYTE title, LPBYTE subject, LPBYTE keywords,
    LPBYTE date, LPBYTE ModDate, DWORD flags, DWORD PermFlags, LPBYTE OwnerPass, LPBYTE UserPass)
{
    // load libarary if not already loaded
    #if !defined(LINK_PDC) 
      if (hPdc==NULL) {
        hPdc=TerLoadDll("pdc32.dll");

        if(hPdc==0) return 0;

        PdcStartDoc2=(LPVOID)GetProcAddress(hPdc, "PdcStartDoc2");
        PdcEndDoc=(LPVOID)GetProcAddress(hPdc, "PdcEndDoc");
        PdcStartPage=(LPVOID)GetProcAddress(hPdc, "PdcStartPage");
        PdcEndPage=(LPVOID)GetProcAddress(hPdc, "PdcEndPage");

        if(PdcStartDoc2==NULL || PdcEndDoc==NULL || PdcStartPage==NULL || PdcEndPage==NULL) {
           FreeLibrary(hPdc);
           hPdc=NULL;
            return 0;
        }
      }
    #endif

    return PdcStartDoc2(LicKey,OutFile,author,producer,title,subject,keywords,date,ModDate,flags,PermFlags,OwnerPass,UserPass);
} 

/********************************************************************************************
    TerPdcEnd:
    Load the pdf libaray, initialize enviroment and start the document
*********************************************************************************************/
BOOL WINAPI _export TerPdcEnd(DWORD id)
{
    return PdcEndDoc(id);
} 

/********************************************************************************************
    TerPdcPrintPage:
    End the document, finalize and unload the pdf library.
*********************************************************************************************/
BOOL WINAPI _export TerPdcPrintPage(HWND hWnd,DWORD id,int PageNo)
{
    HDC hPdfDC;
    PTERWND w;
    RECT rect;
    int pg,sect,width,height;
    int ResX,ResY;
    DWORD SaveTerFlags4;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    TerOpFlags|=TOFLAG_EXTERNAL_DC;
    TerOpFlags2|=(TOFLAG2_PDF|TOFLAG2_USE_PEN_FOR_BORDER);

    SaveTerFlags4=TerFlags4;
    TerFlags4|=TFLAG4_NO_SHARE_BORDER;
   
    TerSetPrintPreview(hWnd,TRUE);  // begin preview

    // print each page
    for (pg=0;pg<TotalPages;pg++) {
       if (PageNo>=0) pg=PageNo;            // print a specific page
       if (pg>=TotalPages) return FALSE;

       sect=PageInfo[pg].TopSect;
       width=(int)(1440*TerSect[sect].PprWidth);         // in twips
       height=(int)(1440*TerSect[sect].PprHeight);
       if (TerSect[sect].orient==DMORIENT_LANDSCAPE) SwapInts(&width,&height);
       
       hPdfDC=PdcStartPage(id,width,height);

       // get the device resolution
       //int PixWidth,XPixs;
       //PixWidth=GetDeviceCaps(hPdfDC,HORZSIZE);  // screen width in mm
       //XPixs=GetDeviceCaps(hPdfDC,HORZRES);      // horizontal pixel count

       //ResX=(int)((XPixs*25.4)/PixWidth);    // physical resolution
       //ResY=GetDeviceCaps(hPdfDC,LOGPIXELSY) *112/100 ;   // logical resolution - apply a correction factor
      
       ResX=GetDeviceCaps(hPdfDC,LOGPIXELSX);
       ResY=GetDeviceCaps(hPdfDC,LOGPIXELSY);


       // set the device mapping
       SetMapMode(hPdfDC,MM_ANISOTROPIC);
       SetWindowOrgEx(hPdfDC,0,0,NULL);   // set the logical window origin
       SetViewportOrgEx(hPdfDC,MulDiv(TerSect1[sect].HiddenX,ResX,PrtResX),MulDiv(TerSect1[sect].HiddenY,ResY,PrtResY),NULL);  // set view port origins
       SetWindowExtEx(hPdfDC,PrtResX,PrtResY,NULL); // logical dimension
       SetViewportExtEx(hPdfDC,ResX,ResY,NULL); // equivalent device dimension

       if (FALSE) {
          MoveToEx(hPdfDC,2*PrtResX,2*PrtResY,NULL);
          LineTo(hPdfDC,6*PrtResX,2*PrtResY);
         
          MoveToEx(hPdfDC,2*PrtResX,2*PrtResY,NULL);
          LineTo(hPdfDC,2*PrtResX,6*PrtResY);
       }
       else { 
          // Set the rectangle values - it is not really used by TerPrintPreview function when printing a pdf file
          rect.left=rect.top=0;
          rect.right=(int)(96*8.5);
          rect.bottom=(int)(96*11);

          TerPrintPreview(hWnd,hPdfDC,&rect,pg,FALSE);
       }
       
       PdcEndPage(id);    // end page

       if (PageNo>=0) break;
    }

    TerSetPrintPreview(hWnd,FALSE);  // end preview
    ResetUintFlag(TerOpFlags,TOFLAG_EXTERNAL_DC);
    ResetUintFlag(TerOpFlags2,TOFLAG2_USE_PEN_FOR_BORDER);
    ResetUintFlag(TerOpFlags2,TOFLAG2_PDF);

    TerFlags4=SaveTerFlags4;

    
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interace to WpTools PDF converter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _stdcall PdfMessage(WPDFEnviroment,int,char *,int);
void _stdcall PdfErrMessage(WPDFEnviroment,int,char *,int);
void _stdcall PdfStreamOpen(WPDFEnviroment,int,char *,int);
void _stdcall PdfStreamWrite(WPDFEnviroment,int,char *,int);
void _stdcall PdfStreamClose(WPDFEnviroment,int,char *,int);
void _stdcall PdfGetText(WPDFEnviroment,int,char *,int);
HINSTANCE LoadEngine(LPBYTE);
void UnloadEngine(HINSTANCE);

//#define RES_X 96
//#define RES_Y 96
#define RES_X 72
#define RES_Y 72

/********************************************************************************************
    TerPdfSetDllName:
    Set the PDF control dll name, if not the same as wPDFControl01.dll.
*********************************************************************************************/
BOOL WINAPI _export TerPdfSetDllName(LPBYTE DllName)
{
    lstrcpy(PdfDllName,DllName);
    return TRUE;
}

/********************************************************************************************
    TerPdfStart:
    Load the pdf libaray, initialize enviroment and start the document
*********************************************************************************************/
HINSTANCE WINAPI _export TerPdfStart(LPBYTE OutFile,LPINT pPdfId,LPBYTE LicName, 
    LPBYTE LicKey, int LicNbr,
    LPBYTE author, LPBYTE producer, LPBYTE title, LPBYTE subject, LPBYTE keywords,
    LPBYTE date, LPBYTE ModDate)
{
    return TerPdfStart2(OutFile,pPdfId,LicName,LicKey,LicNbr,author,producer,title,subject,keywords,date,ModDate,0);
} 

/********************************************************************************************
    TerPdfStart2:
    Load the pdf libaray, initialize enviroment and start the document
*********************************************************************************************/
HINSTANCE WINAPI _export TerPdfStart2(LPBYTE OutFile,LPINT pPdfId,LPBYTE LicName, 
    LPBYTE LicKey, int LicNbr,
    LPBYTE author, LPBYTE producer, LPBYTE title, LPBYTE subject, LPBYTE keywords,
    LPBYTE date, LPBYTE ModDate,int EnhOptions)
{
    return TerPdfStart3(OutFile,pPdfId,LicName,LicKey,LicNbr,author,producer,title,subject,keywords,date,ModDate,EnhOptions,0);
}

/********************************************************************************************
    TerPdfStart3:
    Load the pdf libaray, initialize enviroment and start the document
*********************************************************************************************/
HINSTANCE WINAPI _export TerPdfStart3(LPBYTE OutFile,LPINT pPdfId,LPBYTE LicName, 
    LPBYTE LicKey, int LicNbr,
    LPBYTE author, LPBYTE producer, LPBYTE title, LPBYTE subject, LPBYTE keywords,
    LPBYTE date, LPBYTE ModDate,int EnhOptions, int SecurityPermission)
{
   return TerPdfStart4(OutFile,pPdfId,LicName,LicKey,LicNbr,author,producer,title,subject,keywords,
                       date,ModDate,EnhOptions,SecurityPermission,"");
}


/********************************************************************************************
    TerPdfStart4:
    Load the pdf libaray, initialize enviroment and start the document
*********************************************************************************************/
HINSTANCE WINAPI _export TerPdfStart4(LPBYTE OutFile,LPINT pPdfId,LPBYTE LicName, 
    LPBYTE LicKey, int LicNbr,
    LPBYTE author, LPBYTE producer, LPBYTE title, LPBYTE subject, LPBYTE keywords,
    LPBYTE date, LPBYTE ModDate,int EnhOptions, int SecurityPermission, LPBYTE password)
{
    
   HINSTANCE hPdf;
   WPDFEnviroment pdf;
   static WPDFInfoRecord Info;

   FarMemSet(&Info,0,sizeof(Info));

   Info.SizeOfStruct=sizeof(WPDFInfoRecord); // used to check if this record is ok
   Info.Encoding=0; // int Encoding : Integer; // 0 = none, 1=ASCII85, 2=Hex
   Info.Compression=0;//1; // int Compression : Integer;
   Info.BitCompression=0; // int BitCompression : Integer; // 0=auto, 1=deflat, 2=jpeg
   Info.ThumbNails=1; // int ThumbNails : Integer; // 0=none, 1=color, 2=monchrome
   Info.FontMode=0; // int FontMode : Integer;
   Info.PageMode=1; // int PageMode : Integer;
   Info.InputFileMode=3; // int InputFileMode : Integer;
   Info.JPEGCompress=0; // int JPEGCompress  : Integer;
   Info.EnhancedOptions=EnhOptions; // int EnhancedOptions : Integer;
   Info.PDFXRes=RES_X;
   Info.PDFYRes=RES_Y; // int PDFXRes, PDFYRes;
   Info.PDFWidth=0;
   Info.PDFHeight=0; // int PDFWidth, PDFHeight;  Default DINa4
   Info.InputPDFFile[0]=0; // char InputPDFFile[127];
     
   Info.OnStreamOpenr=PdfStreamOpen; // TWPDF_Callback* OnStreamOpen;
   Info.OnStreamWrite=PdfStreamWrite; // TWPDF_Callback* OnStreamWrite;
   Info.OnStreamClose=PdfStreamClose; // TWPDF_Callback* OnStreamClose;
     
   // Messages
   Info.OnError=PdfErrMessage; // TWPDF_Callback* OnError;
   Info.OnMessage=PdfMessage; // TWPDF_Callback* OnMessage;

   // Mailmerge, etc
   lstrcpy(Info.MailMergeStart,"@@"); // MailMergeStart
   Info.OnGetText=PdfGetText; // OnGetText callback
   Info.Permission=SecurityPermission; //  int Permission;
   lstrcpy(Info.UserPassword,password);
   lstrcpy(Info.OwnerPassword,password);  // char UserPassword[20], OwnerPassword[20]; // actually only 5 chars long!
     
   lstrcpy(Info.Date,date);  // char Date[256],
   lstrcpy(Info.ModDate,ModDate); //      ModDate[256],
   lstrcpy(Info.Author,author); //     Author[256],
   lstrcpy(Info.Producer,producer); //     Producer[256],
   lstrcpy(Info.Title,title); //     Title[256],
   lstrcpy(Info.Subject,subject); //     Subject[256],
   lstrcpy(Info.Keywords,keywords); //     Keywords[256];
     
   // Reserved, must be 0
   Info.Reserved=0; // int Reserved;  
   
   hPdf=NULL;
   
   if((hPdf=LoadEngine(PdfDllName))) {
     if(WpSetLicenseKey!=NULL) WpSetLicenseKey(LicNbr,LicName,LicKey);  // The WpSetLicenseKey will be NULL when using the demo version of the control

     pdf=WpInitializeEx(&Info);
     WpBeginDoc(pdf,OutFile,0);    // begin doc
   
     if (pPdfId) (*pPdfId)=(int)pdf;
   }
   
   return hPdf;
}     

/********************************************************************************************
    TerPdfEnd:
    End the document, finalize and unload the pdf library.
*********************************************************************************************/
BOOL WINAPI _export TerPdfEnd(HINSTANCE hPdf,int PdfId)
{
   WPDFEnviroment pdf=(WPDFEnviroment)PdfId;
   
   WpEndDoc(pdf);
   WpFinalize(pdf);
   UnloadEngine(hPdf);

   return TRUE;
} 

/********************************************************************************************
    TerPdfPrintPage:
    End the document, finalize and unload the pdf library.
*********************************************************************************************/
BOOL WINAPI _export TerPdfPrintPage(HWND hWnd,int PdfId,int PageNo)
{
   HDC hPdfDC;
   WPDFEnviroment pdf=(WPDFEnviroment)PdfId;
   PTERWND w;
   RECT rect;
   int pg,sect,width,height;
   int ResX,ResY;
   DWORD SaveTerFlags4;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   TerOpFlags|=TOFLAG_EXTERNAL_DC;
   TerOpFlags2|=(TOFLAG2_PDF|TOFLAG2_USE_PEN_FOR_BORDER);

   SaveTerFlags4=TerFlags4;
   TerFlags4|=TFLAG4_NO_SHARE_BORDER;
   
   TerSetPrintPreview(hWnd,TRUE);  // begin preview

   // print each page
   for (pg=0;pg<TotalPages;pg++) {
      if (PageNo>=0) pg=PageNo;            // print a specific page
      if (pg>=TotalPages) return FALSE;

      sect=PageInfo[pg].TopSect;
      width=(int)(RES_X*TerSect[sect].PprWidth);
      height=(int)(RES_Y*TerSect[sect].PprHeight);
      if (TerSect[sect].orient==DMORIENT_LANDSCAPE) SwapInts(&width,&height);
      WpStartPageEx(pdf,width,height,0);
      
      hPdfDC=WpDC(pdf);               // get DC

      // get the device resolution
      //int PixWidth,XPixs;
      //PixWidth=GetDeviceCaps(hPdfDC,HORZSIZE);  // screen width in mm
      //XPixs=GetDeviceCaps(hPdfDC,HORZRES);      // horizontal pixel count

      //ResX=(int)((XPixs*25.4)/PixWidth);    // physical resolution
      //ResY=GetDeviceCaps(hPdfDC,LOGPIXELSY) *112/100 ;   // logical resolution - apply a correction factor
      
      ResX=GetDeviceCaps(hPdfDC,LOGPIXELSX);
      ResY=GetDeviceCaps(hPdfDC,LOGPIXELSY);

      // set the device mapping
      SetWindowOrgEx(hPdfDC,0,0,NULL);   // set the logical window origin
      SetViewportOrgEx(hPdfDC,MulDiv(TerSect1[sect].HiddenX,ResX,PrtResX),MulDiv(TerSect1[sect].HiddenY,ResY,PrtResY),NULL);  // set view port origins
         
      SetWindowExtEx(hPdfDC,PrtResX,PrtResY,NULL); // logical dimension
      SetViewportExtEx(hPdfDC,ResX,ResY,NULL); // equivalent device dimension

      if (FALSE) {
         MoveToEx(hPdfDC,2*PrtResX,2*PrtResY,NULL);
         LineTo(hPdfDC,6*PrtResX,2*PrtResY);
         
         MoveToEx(hPdfDC,2*PrtResX,2*PrtResY,NULL);
         LineTo(hPdfDC,2*PrtResX,6*PrtResY);
      }
      else { 


         // Set the rectangle values - it is not really used by TerPrintPreview function when printing a pdf file
         rect.left=rect.top=0;
         rect.right=(int)(RES_X*8.5);
         rect.bottom=(int)(RES_Y*11);

         TerPrintPreview(hWnd,hPdfDC,&rect,pg,FALSE);
      }
      WpEndPage(pdf);    // end page

      if (PageNo>=0) break;
   }

   TerSetPrintPreview(hWnd,FALSE);  // end preview
   ResetUintFlag(TerOpFlags,TOFLAG_EXTERNAL_DC);
   ResetUintFlag(TerOpFlags2,TOFLAG2_USE_PEN_FOR_BORDER);
   ResetUintFlag(TerOpFlags2,TOFLAG2_PDF);

   TerFlags4=SaveTerFlags4;

   return TRUE;
}


void _stdcall PdfMessage(WPDFEnviroment PdfEnv, int number, char *buffer, int bufsize)
{
}

void _stdcall PdfErrMessage(WPDFEnviroment PdfEnv, int number, char *buffer, int bufsize)
{
  //OurPrintf("Error: %s\n",buffer);
}

void _stdcall PdfStreamOpen(WPDFEnviroment PdfEnv, int number, char *buffer, int bufsize)
{
}

// Create the PDF file as stdout
void _stdcall PdfStreamWrite(WPDFEnviroment PdfEnv, int number, char *buffer, int BufSize)
{
}

void _stdcall PdfStreamClose(WPDFEnviroment PdfEnv, int number, char *buffer, int bufsize)
{
}

void _stdcall PdfGetText(WPDFEnviroment PdfEnv, int number, char *buffer, int bufsize)
{
}

HINSTANCE LoadEngine(LPBYTE dllname)
{
  HINSTANCE hPdf = LoadLibrary(dllname);
  if(hPdf==0) return NULL;
  else {
    WpInitializeEx=(LPVOID)GetProcAddress(hPdf, "WPDF_InitializeEx");
    WpInitialize=(LPVOID)GetProcAddress(hPdf, "WPDF_Initialize");
    WpFinalize=(LPVOID)GetProcAddress(hPdf, "WPDF_Finalize");
    WpFinalizeAll=(LPVOID)GetProcAddress(hPdf, "WPDF_FinalizeAll");
    WpSetResult=(LPVOID)GetProcAddress(hPdf, "WPDF_SetResult");
    WpBeginDoc=(LPVOID)GetProcAddress(hPdf, "WPDF_BeginDoc");
    WpEndDoc=(LPVOID)GetProcAddress(hPdf, "WPDF_EndDoc");
    WpStartPage=(LPVOID)GetProcAddress(hPdf, "WPDF_StartPage");
    WpStartPageEx=(LPVOID)GetProcAddress(hPdf, "WPDF_StartPageEx");
    WpEndPage=(LPVOID)GetProcAddress(hPdf, "WPDF_EndPage");
    WpStartWatermark=(LPVOID)GetProcAddress(hPdf, "WPDF_StartWatermark");
    WpStartWatermarkEx=(LPVOID)GetProcAddress(hPdf, "WPDF_StartWatermarkEx");
    WpEndWatermark=(LPVOID)GetProcAddress(hPdf, "WPDF_EndWatermark");
    WpDrawWatermark=(LPVOID)GetProcAddress(hPdf, "WPDF_DrawWatermark");

    // Optional functions
    WpSetSProp=(LPVOID)GetProcAddress(hPdf, "WPDF_SetSProp");
    WpSetIProp=(LPVOID)GetProcAddress(hPdf, "WPDF_SetIProp");
    WpExecCommand=(LPVOID)GetProcAddress(hPdf, "WPDF_ExecCommand");

    // Output
    WpDrawMetafile=(LPVOID)GetProcAddress(hPdf, "WPDF_DrawMetafile");
    WpDrawDIB=(LPVOID)GetProcAddress(hPdf, "WPDF_DrawDIB");
    WpDrawBMP=(LPVOID)GetProcAddress(hPdf, "WPDF_DrawBMP");
    WpDrawJPEG=(LPVOID)GetProcAddress(hPdf, "WPDF_DrawJPEG");
    WpDrawBitmapFile=(LPVOID)GetProcAddress(hPdf, "WPDF_DrawBitmapFile");
    WpDrawBitmapClone=(LPVOID)GetProcAddress(hPdf, "WPDF_DrawBitmapClone");
    // Get DC
    WpDC=(LPVOID)GetProcAddress(hPdf, "WPDF_DC");
    // Graphics API
    WpTextOut=(LPVOID)GetProcAddress(hPdf, "WPDF_TextOut");
    WpTextRect=(LPVOID)GetProcAddress(hPdf, "WPDF_TextRect");
    WpMoveTo=(LPVOID)GetProcAddress(hPdf, "WPDF_MoveTo");
    WpLineTo=(LPVOID)GetProcAddress(hPdf, "WPDF_LineTo");
    WpRectangle=(LPVOID)GetProcAddress(hPdf, "WPDF_Rectangle");
    WpHyperlink=(LPVOID)GetProcAddress(hPdf, "WPDF_Hyperlink");
    WpBookmark=(LPVOID)GetProcAddress(hPdf, "WPDF_Bookmark");
    WpOutline=(LPVOID)GetProcAddress(hPdf, "WPDF_Outline");
    // DC Attributes
    WpSetTextDefaultAttr=(LPVOID)GetProcAddress(hPdf, "WPDF_SetTextDefaultAttr");
    WpSetTextAttr=(LPVOID)GetProcAddress(hPdf, "WPDF_SetTextAttr");
    WpSetTextAttrEx=(LPVOID)GetProcAddress(hPdf, "WPDF_SetTextAttrEx");
    WpSetPenAttr=(LPVOID)GetProcAddress(hPdf, "WPDF_SetPenAttr");
    WpSetBrushAttr=(LPVOID)GetProcAddress(hPdf, "WPDF_SetBrushAttr");
    // License Key - this function can be nil (in demo version)
    WpSetLicenseKey=(LPVOID)GetProcAddress(hPdf, "WPDF_SetLicenseKey");


    // Check if we didn't found all functions
    if(
         (WpInitialize==NULL)||(WpInitializeEx==NULL)||
         (WpFinalize==NULL)||(WpFinalizeAll==NULL)||
         (WpSetResult==NULL)||
         (WpBeginDoc==NULL)||(WpEndDoc==NULL)||
         (WpStartPage==NULL)||(WpStartPageEx==NULL)|| (WpEndPage==NULL)||
         (WpStartWatermark==NULL)||(WpStartWatermarkEx==NULL)||(WpDrawWatermark==NULL)||(WpEndWatermark==NULL)||
         (WpSetSProp==NULL)||(WpSetIProp==NULL)||(WpExecCommand==NULL)||
         (WpDrawMetafile==NULL)||
         (WpDrawDIB==NULL)||(WpDrawBMP==NULL)||(WpDrawJPEG==NULL)||(WpDrawBitmapFile==NULL)||(WpDrawBitmapClone==NULL)||
         (WpDC==NULL)||
         (WpTextOut==NULL)||(WpTextRect==NULL)||(WpMoveTo==NULL)||(WpLineTo==NULL)||(WpRectangle==NULL)||
         (WpRectangle==NULL)||(WpMoveTo==NULL)||
         (WpHyperlink==NULL)||(WpBookmark==NULL)||(WpOutline==NULL)||
         (WpSetTextDefaultAttr==NULL)||(WpSetTextAttr==NULL)||
         (WpSetTextAttrEx==NULL)||(WpSetPenAttr==NULL)||
         (WpSetBrushAttr==NULL)
      )
      {
        FreeLibrary(hPdf);
        WpInitialize=NULL;
        WpFinalize=NULL;
        WpFinalizeAll=NULL;
        return NULL;
      }
    else return hPdf;
  };
}

void UnloadEngine(HINSTANCE hPdf)
{
    FreeLibrary(hPdf);
    WpInitialize=NULL;
    WpFinalize=NULL;
    WpFinalizeAll=NULL;
}


 
