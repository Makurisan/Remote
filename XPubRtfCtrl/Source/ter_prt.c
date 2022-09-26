/*===============================================================================
   TER_PRT.C
   TER file and buffer I/O routines.

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


/******************************************************************************
    TerPageOptions:
    This routine define the layout of the printed page.
******************************************************************************/
TerPageOptions(PTERWND w)
{
    CallDialogBox(w,"PageParam",PageParam,0L);

    // recalculate the wrap width and display
    if (TerArg.PrintView) {
       RepaintRuler=true;
       PaintTer(w);           // repaint
    }

    return TRUE;
}

/******************************************************************************
    TerPrintOptions:
    This routine printer specific options.
******************************************************************************/
TerPrintOptions(PTERWND w, BOOL print)
{
    int SaveOrient,SavePaperSize;
    int len;
    HGLOBAL hDevNames;
    PRINTDLG pd;
    LPBYTE ptr;
    BOOL result,repage=FALSE,RedoFonts=FALSE;
    BYTE SaveName[MAX_WIDTH+1],SaveDriver[MAX_WIDTH+1];
    int  SaveResX,SaveResY,ResX,ResY,SaveBin,bin,sect;
    int FirstPage;
    float SavePageWidth,SavePageHeight;
    HANDLE hDriver;
    int  SaveModified=TerArg.modified;

    struct StrDevNames {      // the devnames structure
       WORD wDriverOffset,wNameOffset,wOutputOffset,wDefault;
       BYTE driver[MAX_WIDTH+1],name[MAX_WIDTH+1],port[MAX_WIDTH+1];
    } far *pDevNames;

    
    if (True(TerFlags5&TFLAG5_SHOW_PAGE_SETUP)) print=true;

    // Apply current section parameters to the DevMode structure
    sect=GetSection(w,CurLine);
    if (pDeviceMode->dmOrientation!=TerSect[sect].orient) {
       pDeviceMode->dmOrientation=TerSect[sect].orient;
       pDeviceMode->dmFields|=DM_ORIENTATION;    // creates problem on Win 2000 - can't change paper-size any more using the dialog box
    }
    if (pDeviceMode->dmPaperSize!=TerSect[sect].PprSize 
       || (pDeviceMode->dmPaperWidth!=(int)InchesToMm(TerSect[sect].PprWidth)*10)
       || (pDeviceMode->dmPaperLength!=(int)InchesToMm(TerSect[sect].PprHeight)*10) ) {
       pDeviceMode->dmPaperSize=TerSect[sect].PprSize;
       pDeviceMode->dmPaperWidth=(int)InchesToMm(TerSect[sect].PprWidth)*10;
       pDeviceMode->dmPaperLength=(int)InchesToMm(TerSect[sect].PprHeight)*10;
       pDeviceMode->dmFields|=DM_PAPERSIZE;
       if (TerSect[sect].PprSize==DMPAPER_USER || TerSect[sect].PprSize==0) pDeviceMode->dmFields|=(DM_PAPERWIDTH|DM_PAPERLENGTH);   
    }
    PaperOrient=pDeviceMode->dmOrientation;
    PaperSize=pDeviceMode->dmPaperSize;
    
    if (OpenPrinter(PrinterName,&hDriver,NULL)) {
       if (!DocumentProperties(hTerWnd,hDriver,PrinterName,pDeviceMode,pDeviceMode,DM_MODIFY|DM_COPY)) {
          PrintError(w,MSG_ERR_INIT_PRINTER,"TerSetPaper");
       }
       ClosePrinter(hDriver);
    }

    // save current settings
    SaveOrient=pDeviceMode->dmOrientation;
    SavePaperSize=PaperSize;
    SavePageWidth=PageWidth;
    SavePageHeight=PageHeight;
    SaveResX=ResX=pDeviceMode->dmPrintQuality;
    SaveResY=ResY=pDeviceMode->dmYResolution;
    SaveBin=bin=pDeviceMode->dmDefaultSource;
    lstrcpy(SaveName,PrinterName);
    lstrcpy(SaveDriver,PrinterDriver);

    PREPARE_FOR_DLG:
    // preparation to call the PrintDlg function
    // fill the devname structure with the current printer name
    if (  NULL==(hDevNames=GlobalAlloc(GMEM_MOVEABLE,sizeof(struct StrDevNames)))
       || NULL==(pDevNames=GlobalLock(hDevNames)) ) {
       return PrintError(w,MSG_ERR_MEM_LOCK,"TerPrintOptions");
    }
    pDevNames->wDriverOffset=(WORD) ((DWORD)(pDevNames->driver)-(DWORD)(LPINT)&(pDevNames->wDriverOffset));
    pDevNames->wNameOffset=(WORD) ((DWORD)(pDevNames->name)-(DWORD)(LPINT)&(pDevNames->wDriverOffset));
    pDevNames->wOutputOffset=(WORD) ((DWORD)(pDevNames->port)-(DWORD)(LPINT)&(pDevNames->wDriverOffset));
    pDevNames->wDefault=0;
    lstrcpy(pDevNames->driver,PrinterDriver);
    len=lstrlen(pDevNames->driver);
    if (len>4) pDevNames->driver[len-4]=0;  // remove the file extension
    lstrcpy(pDevNames->name,PrinterName);
    lstrcpy(pDevNames->port,PrinterPort);

    GlobalUnlock(hDevNames);

    // fill the PRINTDLG structure
    FarMemSet(&pd,0,sizeof(pd));
    pd.lStructSize=sizeof(pd);
    pd.hwndOwner=hTerWnd;
    pd.hDevNames=hDevNames;
    pd.Flags=0;
    if (print) {                        // show the print button
       if (HilightType==HILIGHT_OFF) {
          pd.Flags|=(PD_ALLPAGES|PD_NOSELECTION);
       }
       else  pd.Flags|=PD_SELECTION;
       pd.nMinPage=pd.nFromPage=1;
       pd.nMaxPage=pd.nToPage=TotalPages;
    }
    else pd.Flags|=PD_PRINTSETUP;

    // make a copy of hDeviceMode to pass
    if (hDeviceMode) {                // make a copy to pass
       DWORD bytes=GlobalSize(hDeviceMode);
       LPBYTE pDevMode;

       // allocate memory for the structure
       if ( (NULL==(pd.hDevMode=GlobalAlloc(GMEM_MOVEABLE,bytes+1)))
           || (NULL==(pDevMode=GlobalLock(pd.hDevMode)))  ) {
           return PrintError(w,MSG_OUT_OF_MEM,"TerPrintOptions");
       }
       FarMove(pDeviceMode,pDevMode,(UINT)bytes);
       GlobalUnlock(pd.hDevMode);
    }


    // call the printer dialog box
    result=PrintDlg(&pd);

    // extract the printer name, driver, port
    hDevNames=pd.hDevNames;
    if (!hDevNames || (NULL==(pDevNames=GlobalLock(hDevNames)))) return FALSE;

    ptr=(LPBYTE)&pDevNames->wDriverOffset;
    lstrcpy(PrinterName,&(ptr[pDevNames->wNameOffset]));
    lstrcpy(PrinterDriver,&(ptr[pDevNames->wDriverOffset]));
    len=lstrlen(PrinterDriver);    // add .drv extension
    if (len<4 || lstrcmpi(&(PrinterDriver[len-4]),".drv")!=0) lstrcat(PrinterDriver,".drv");
    lstrcpy(PrinterPort,&(ptr[pDevNames->wOutputOffset]));
    GlobalUnlock(hDevNames);
    GlobalFree(hDevNames);

    // get the new resolution
    if (pd.hDevMode) {
       DEVMODE far *pNewDevMode=GlobalLock(pd.hDevMode);
       if (pNewDevMode) {
          ResX=pNewDevMode->dmPrintQuality;
          ResY=pNewDevMode->dmYResolution;
          bin=pNewDevMode->dmDefaultSource;
          GlobalUnlock(pd.hDevMode);
       }
    }

    // check if the printer has changed
    if (lstrcmpi(PrinterName,SaveName)!=0 || lstrcmpi(PrinterDriver,SaveDriver)!=0
        || ResX!=SaveResX || ResY!=SaveResY) {
       //BOOL SaveFlag=(BOOL)(TerFlags&TFLAG_APPLY_PRT_PROPS);
       //TerFlags|=TFLAG_APPLY_PRT_PROPS;
       TerSetPrinter(hTerWnd,PrinterName,PrinterDriver,PrinterPort,pd.hDevMode);
       //if (!SaveFlag) TerFlags=ResetUintFlag(TerFlags,TFLAG_APPLY_PRT_PROPS);
       //if (pd.hDevMode) GlobalFree(pd.hDevMode);
       //return TRUE;
    }

    if (hDeviceMode && pd.hDevMode) {    // use the new structure
       GlobalUnlock(hDeviceMode);
       GlobalFree(hDeviceMode);
    }
    hDeviceMode=pd.hDevMode;              // lock the new structure
    pDeviceMode=GlobalLock(hDeviceMode);
    
    if (!result) {
       if (print && GoPrintPreview) {            // go back to print preview
          GoPrintPreview=FALSE;         // reset this flag
          PostMessage(hTerWnd,WM_COMMAND,ID_PRINT_PREVIEW,0L);  // terminate print preview
       }
       return FALSE;
    }
    
    if (!PrinterAvailable) goto PREPARE_FOR_DLG;  // let the user select another printer

    // update our paper size/orientation
    PaperSize=pDeviceMode->dmPaperSize;
    PaperOrient=pDeviceMode->dmOrientation;

    // set the orientation in the section
    if ((TerFlags&TFLAG_APPLY_PRT_PROPS || !MultiOrient(w)) && (SaveOrient!=PaperOrient || SaveBin!=bin)) {
       ApplyPrinterSetting(w,(SaveOrient==PaperOrient)?(-1):PaperOrient,(SaveBin==bin)?(-1):bin,-1,-1,-1);
       repage=TRUE;
       if (PrtDiffRes) RedoFonts=TRUE;
    }


    if (!OpenCurPrinter(w,FALSE)) return FALSE;

    // equate section page sizes 
    if ((TRUE || TerFlags&TFLAG_APPLY_PRT_PROPS || !MultiOrient(w)) && (SavePaperSize!=PaperSize || SavePageWidth!=PageWidth || SavePageHeight!=PageHeight)) {
       ApplyPrinterSetting(w,-1,-1,PaperSize,PageWidth,PageHeight);
       SetSectPageSize(w);
       repage=TRUE;
    }


    if (RedoFonts) RecreateFonts(w,hTerDC);

    SetSectPageSize(w);          // set the page sizes for the sections

    if (!print) TerArg.modified++;

    if (repage) RequestPagination(w,TRUE);

    // recalculate the wrap width
    if (TerArg.PrintView) PaintTer(w);   // repaint

    if (!print) return TRUE;

    // print the document
    FirstPage=pd.nFromPage-1;
    if (!(pd.Flags&(PD_PAGENUMS|PD_SELECTION))) FirstPage=-1;   // print selection
    if ((pd.Flags&PD_SELECTION)) FirstPage=-3;                  // print selection

    TerPrint2(hTerWnd,FALSE,FirstPage,pd.nToPage-1,1/*pDeviceMode->dmCopies*/,pd.Flags&PD_COLLATE);  // specify one copy becuase the printer driver takes cares of copies and collation

    TerArg.modified=SaveModified;     // reset any modification flag set during printing

    return TRUE;
}

/******************************************************************************
    TerGetPrinter:
    Get the current printer name/driver
*******************************************************************************/
BOOL WINAPI _export TerGetPrinter(HWND hWnd,LPBYTE name, LPBYTE driver, LPBYTE port)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return false;  // get the pointer to window data
    if (!PrinterAvailable) return false;

    if (name) lstrcpy(name,PrinterName);
    if (driver) lstrcpy(driver,PrinterDriver);
    if (port) lstrcpy(port,PrinterPort);

    return true;
}

/******************************************************************************
    TerGetDevmode:
    Get the DEVMODE structure for the current priunter
*******************************************************************************/
HGLOBAL WINAPI _export TerGetDevmode(HWND hWnd)
{
    PTERWND w;
    HGLOBAL hMem=NULL;
    DWORD size=0;
    LPBYTE pMem;

    if (NULL==(w=GetWindowPointer(hWnd))) return NULL;  // get the pointer to window data
    if (!PrinterAvailable) return NULL;

    size=pDeviceMode->dmSize+pDeviceMode->dmDriverExtra;
    if (NULL==(hMem=GlobalAlloc(GMEM_MOVEABLE,size))) return NULL;
    
    pMem=GlobalLock(hMem);
    FarMove(pDeviceMode,pMem,size);
    GlobalUnlock(hMem);

    return hMem;
}


/******************************************************************************
    TerIsPrinting:
    This function returns TRUE when the editor is printing a document.
*******************************************************************************/
BOOL WINAPI _export TerIsPrinting(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return (InPrinting);
}

/******************************************************************************
    TerPrint:
    Print a block of text or the entire document.  When the dialog
*******************************************************************************/
BOOL WINAPI _export TerPrint(HWND hWnd, BOOL dialog)
{

   return TerPrintEx(hWnd,dialog,-1,-1);
}

/******************************************************************************
    TerPrintEx:
    Print a block of text or the entire document.
*******************************************************************************/
BOOL WINAPI _export TerPrintEx(HWND hWnd, BOOL dialog, int FirstPage, int LastPage)
{
    return TerPrint2(hWnd,dialog,FirstPage,LastPage,1,TRUE);
}

/******************************************************************************
    TerPrint2:
    Print a block of text or the entire document.
*******************************************************************************/
BOOL WINAPI _export TerPrint2(HWND hWnd, BOOL dialog, int FirstPage, int LastPage, int copies, BOOL colate)
{
    int        i,FromCol,ToCol,ColBeg,ColEnd,ColLength,sel,copy,UncolateCopy;
    long       FromLine,ToLine,lin,PageBeginLine;
    int        PagePixWidth,PagePixHeight,BeginX,BeginY,NextY,LastY,
               LineHeight,BaseHeight,ExtLead,ColateCopies,UncolateCopies,
               PageBeginCol,ErrCode,SaveCopies=1;
    FARPROC    lpProc=NULL,lpAbortDlg=NULL;
    LPBYTE     ptr,lead;
    LPWORD     fmt;
    PTERWND    w;
    BOOL       TempPrintViewMode=FALSE,SavePaintEnabled,SavePageBorder;
    BOOL       TempPageMode=FALSE,SaveFittedView=FALSE;
    BOOL       TempViewPageHdrFtr=FALSE;
    BOOL       SaveEditPageHdrFtr,PrintCurPage=FALSE;
    DOCINFO    doc;
    BOOL       result=TRUE,ResetCopies=FALSE;
    BOOL       ShowCancelDialog;
    BYTE       ErrString[100];

    // Get the window pointer
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (restrict) return FALSE;

    if (!PrinterAvailable) {
       TerLastMsg=MSG_NO_PRINTER;
       return FALSE;                 // printer not available
    }

    if (HilightType!=HILIGHT_OFF && !NormalizeBlock(w)) return TRUE; // normalize the hilghted block

    if (EditFootnoteText) ToggleFootnoteEdit(w,TRUE);         // hide footnotes
    if (EditEndnoteText) ToggleFootnoteEdit(w,FALSE);         // hide endnotes

    // recalculate the section boundaries
    RecreateSections(w);

    //*** Get Print Specifications From the User
    if (dialog) {                          // ask user the range to print
       sel=CallDialogBox(w,"RangeParam",RangeParam,0L);
       if (!sel) return FALSE;
       if (sel==IDC_CUR_PAGE) {
          PrintCurPage=TRUE;
          FirstPage=LastPage=-2;
          sel=IDC_RANGE_PAGES;
       }
       else if (sel==IDC_RANGE_PAGES) {
          FirstPage=DlgInt1-1;
          LastPage=DlgInt2-1;
       }
       copies=DlgInt3;           // number of copies
       colate=DlgInt4;           // colate
    }
    else {
       if (FirstPage>=0 && FirstPage>LastPage) return FALSE;

       if (FirstPage>=0 || FirstPage==-2) {
          sel=IDC_RANGE_PAGES;
          if (FirstPage==-2) PrintCurPage=TRUE;
       }
       else if (FirstPage==-3 && HilightType!=HILIGHT_OFF) 
             sel=IDC_RANGE_SELECTED;
       else  sel=IDC_RANGE_ALL;

       if (copies==0) copies=pDeviceMode->dmCopies;
    }

    if (sel==IDC_RANGE_SELECTED && TerArg.WordWrap && EditPageHdrFtr) {
       return PrintError(w,MSG_NO_RANGE_PRINTING,NULL);
    }
    if (copies==1) colate=TRUE;

    // check for selective printing
    if (sel==IDC_RANGE_SELECTED && HilightType!=HILIGHT_OFF) {
       for (i=0;i<TotalPages;i++) if (HilightBegRow==PageInfo[i].FirstLine) break;  // does the selection start on the beginning of a page
       if (i==TotalPages && HilightEndRow<(HilightBegRow+MAX_SELECT_PRINT_LINES)) {
          return TerSelectPrint2(hTerWnd,copies);
       }
    }
    
    //************ create device context and a print font ******************
    if (UseScrMetrics) {                     // cretae the printer device context
       BYTE driver[300]="";
       if (PrinterDriver) {
         lstrcpy(driver,PrinterDriver);
         if (lstrlen(driver)>4) driver[lstrlen(driver)-4]=0;             // strip the .drv extension
       }
       hPr=CreateDC(driver,PrinterName,PrinterPort,pDeviceMode);
       if (!hPr) {
           BYTE string[100];
           wsprintf(string,"OpenCurPrinter: printer: %s, port: %s, driver: %s",(LPBYTE)PrinterName,(LPBYTE)PrinterPort,(LPBYTE)driver);
           PrintError(w,MSG_ERR_INIT_PRINTER,string);
           return false;
       }
    } 
    else if (!OpenCurPrinter(w,TRUE)) return FALSE;         // reopen current printer as private DC
    
    if (copies<1) copies=1;
    ColateCopies=UncolateCopies=1;
    if (colate) ColateCopies=copies;
    else        UncolateCopies=copies;

    //*** Being Printing ***
    ShowCancelDialog=!(TerFlags2&TFLAG2_NO_PRT_CANCEL_DLG);
    if (ShowCancelDialog) {
       lpProc = MakeProcInstance((FARPROC)PrintProc, hTerInst);
       SetAbortProc(hPr,(ABORTPROC)lpProc);      // set the abort process
    }

    // start the job
    FarMemSet(&doc,0,sizeof(doc));
    doc.cbSize=sizeof(doc);
    doc.lpszDocName=(strlen(DocName)==0)?"Unnamed":DocName;   // Adobe Acrobat does not like an empty DocName
    doc.lpszOutput=NULL;
    if (StartDoc(hPr,&doc)==SP_ERROR) {
        wsprintf(ErrString,"TerPrint2, Error Code: %lx",TerGetLastError());
        return PrintError(w,MSG_ERR_START_PRINTER,ErrString);
    }

    AbortPrint=FALSE;                    // activate the abort window

    if (ShowCancelDialog) {
       lpAbortDlg = MakeProcInstance((FARPROC)PrintAbortParam, hTerInst);
       hAbortWnd=CreateDialog(hTerInst,"PrintAbortParam",hTerWnd,(DLGPROC)lpAbortDlg);
       SetWindowLong(hAbortWnd,DWL_USER,(LONG)w);  // set the window data area
       PosDialog(w,hAbortWnd);
       ShowWindow(hAbortWnd,SW_NORMAL);
       UpdateWindow(hAbortWnd);
    }
    else hAbortWnd=hCurAbortWnd=NULL;

    EnableWindow(hTerWnd,FALSE);       // disable the main window

    SavePaintEnabled=PaintEnabled;     // disable the painting function
    PaintEnabled=FALSE;

    InPrinting=TRUE;
    HoldMessages=TRUE;                 // disables the window message processing
    PrtTextBkColor=PrtParaBkColor=PrtFrameBkColor=CLR_WHITE;
    if (UseScrMetrics)  TerOpFlags2|=TOFLAG2_SCALE_PRT_DC;

    // enable print view mode temporariy
    SaveEditPageHdrFtr=EditPageHdrFtr; // restore this flag when print is over
    SavePvFirstFramePage=FirstFramePage;
    SavePvLastFramePage=LastFramePage;

    SavePageBorder=ShowPageBorder;     // turnoff page border temporarily
    ShowPageBorder=FALSE;

    if (TerArg.WordWrap && !TerArg.PrintView) {
       TerArg.PrintView=TRUE;
       TempPrintViewMode=TRUE;
       if (!RecreateFonts(w,hTerDC)) goto END; // recreate the fonts
    }
    // Preparation for PrintView mode editing
    if (TerArg.PrintView) {
       // enable page mode temporarily
       if (!TerArg.PageMode) {
          TerArg.PageMode=TRUE;
          TempPageMode=TRUE;
       }

       SaveFittedView=TerArg.FittedView;  // turn-off fitted view temporarily
       TerArg.FittedView=FALSE;

       // disable page header/footer editgin
       if (!TempPageMode && EditPageHdrFtr) ToggleEditHdrFtr(w);

       // enable page header/footer temporarily
       if (!TempPrintViewMode && !ViewPageHdrFtr) {
          for (i=0;i<TotalSects;i++) {
             if (TerSect1[i].hdr.FirstLine>=0) {
                TempViewPageHdrFtr=TRUE;
                ToggleViewHdrFtr(w);
                break;
             }
          }
       }
    }

    // repaginate
    if (TerArg.PrintView) {
       if (HtmlMode) TerAdjustHtmlTable(hTerWnd);

       //FromAbs=RowColToAbs(w,FromLine,FromCol); // calculate absolute range
       //ToAbs=RowColToAbs(w,ToLine,ToCol);
       Repaginate(w,FALSE,FALSE,0,TRUE);
       if (TotalParaFrames>1) Repaginate(w,FALSE,FALSE,0,TRUE);  // another wrap to take care of para frames
       RecreateSections(w);
       //AbsToRowCol(w,FromAbs,&FromLine,&FromCol);
       //AbsToRowCol(w,ToAbs,&ToLine,&ToCol);
    }


    // define the range of text to be printed
    if (sel==IDC_RANGE_ALL) {           // print the entire file
       FromLine=0;
       FromCol=0;
       ToLine=TotalLines-1;
       ToCol=LineLen(ToLine)-1;
    }
    else if (sel==IDC_RANGE_PAGES) {       // range of pages to print
       if (TerArg.PrintView) {
          if (PrintCurPage) {              // print the current page
             for (FirstPage=1;FirstPage<TotalPages;FirstPage++) if (PageInfo[FirstPage].FirstLine>CurLine) break;
             FirstPage--;
             LastPage=FirstPage;
          }
          if (FirstPage>=TotalPages) FirstPage=TotalPages-1;
          if (LastPage>=TotalPages) LastPage=TotalPages-1;
          FromLine=PageInfo[FirstPage].FirstLine;
          if (LastPage==(TotalPages-1)) ToLine=TotalLines-1;
          else                          ToLine=PageInfo[LastPage+1].FirstLine-1;
       }
       else {                              // non-word wrap mode
          FromLine=0;
          ToLine=TotalLines-1;
       }
       FromCol=0;
       ToCol=LineLen(ToLine)-1;
    }
    else {                               // print the selected text
       FromLine=HilightBegRow;           // set begin and end range
       if (HilightType==HILIGHT_CHAR) FromCol=HilightBegCol;
       else                           FromCol=0;
       ToLine=HilightEndRow;
       if (HilightType==HILIGHT_CHAR) ToCol=HilightEndCol-1;
       else                           ToCol=LineLen(HilightEndRow)-1;
    }

    if (ToCol<0) ToCol=0;


    // Recreate fonts
    if (!RecreateFonts(w,hPr)) goto END; // create printer fonts (MUST be after disabling the hTerWnd)

    CalcPageDim(w,FromLine,&PagePixWidth,&PagePixHeight,&BeginX,&BeginY,&LastY); // calculate printing area dimensions
    NextY=BeginY;

    hPrtDataDC=hPr;       // printer data DC same as the attribute DC


    // set the printer for multiple copies
    if (FALSE && UncolateCopies>1) {
        SaveCopies=pDeviceMode->dmCopies;
        pDeviceMode->dmCopies=UncolateCopies;
        ResetDC(hPr,pDeviceMode);
        ResetCopies=TRUE;
        UncolateCopies=1;
    }

    for (copy=0;copy<ColateCopies;copy++) {
       if (TerArg.WordWrap) PrintViewMode(w,hPr,FromLine,ToLine,UncolateCopies,copy);
       else {                             // print the text mode document
           BOOL print=TRUE,NextCopy=FALSE;
           UncolateCopy=PageBeginCol=0;
           PageBeginLine=0;
           NextY=BeginY;

           if (sel==IDC_RANGE_PAGES && (FirstPage>0 || PrintCurPage)) print=FALSE;

           if (StartPage(hPr)<=0) return PrintError(w,MSG_ERR_START_PRINTER,"TerPrint");

           CurPage=0;
           for (lin=FromLine;lin<=ToLine;lin++) {
               LineHeight=GetLineHeight(w,lin,&BaseHeight,&ExtLead);

               if (LineLen(lin)==0) {        // blank line
                  if ((NextY+LineHeight)>LastY) {
                     if (print && sel==IDC_RANGE_PAGES && CurPage==LastPage && (UncolateCopy+1)==UncolateCopies) goto PRINTED;
                     if (print) SpoolIt(w,hPr); // spool this page
                     if (print) StartPage(hPr);       // start the next page

                     UncolateCopy++;
                     if ((UncolateCopy>=UncolateCopies) || !print) {
                        CurPage++;
                        PageBeginLine=lin;
                        PageBeginCol=0;
                        UncolateCopy=0;
                     }
                     else lin=PageBeginLine-1;

                     NextY=BeginY;
                     if (AbortPrint) break;
                     if (!print && CurPage==FirstPage && !PrintCurPage) print=TRUE;
                  }
                  NextY+=LineHeight;
                  continue;
               }

               if (lin==FromLine) ColBeg=FromCol;else ColBeg=0;    // determine the part of line to print
               if (NextCopy)      ColBeg=PageBeginCol;
               if (lin==ToLine)   ColEnd=ToCol;else ColEnd=LineLen(lin)-1;
               if (ColEnd<ColBeg) ColEnd=ColBeg;

               ptr=pText(lin);
               lead=pLead(lin);
               fmt=OpenCfmt(w,lin);
               NextCopy=FALSE;

               while (ColBeg<=ColEnd) {
                  ColLength=GetLineLen(w,ptr,fmt,lead,ColBeg,ColEnd,PagePixWidth);
                  if ((ColBeg+ColLength-1)>ColEnd) ColLength=ColEnd-ColBeg+1;

                  if ((NextY+LineHeight)>LastY) {
                     if (print && sel==IDC_RANGE_PAGES && CurPage==LastPage && (UncolateCopy+1)==UncolateCopies) goto PRINTED;
                     if (print) SpoolIt(w,hPr);       // spool this page
                     if (print) StartPage(hPr);       // start the next page

                     UncolateCopy++;
                     if ((UncolateCopy>=UncolateCopies) || !print) {
                        CurPage++;
                        PageBeginLine=lin;
                        PageBeginCol=ColBeg;
                        UncolateCopy=0;
                     }
                     else NextCopy=TRUE;

                     NextY=BeginY;
                     if (AbortPrint) break;
                     if (!print && CurPage==FirstPage && !PrintCurPage) print=TRUE;
                  }
                  if (NextCopy) break;   // start the next copy of this page

                  // print the line
                  if (ColLength==1 && ptr[ColBeg]==PAGE_CHAR) NextY=LastY; // force page break
                  else if (print) PrintNonWrapLine(w,lin,hPrtDataDC,hPr,BeginX,NextY,&ptr[ColBeg],&fmt[ColBeg],LeadPtr(lead,ColBeg),ColLength,LineHeight,BaseHeight,NULL,pfmt(lin),0,0,ExtLead,0,0,NULL);

                  NextY+=LineHeight;
                  ColBeg+=ColLength;
               }

               CloseCfmt(w,lin);
               if (AbortPrint) break;

               // begin next copy
               if (lin==ToLine && (UncolateCopy+1)<UncolateCopies) {
                  if (print) SpoolIt(w,hPr);       // spool this page
                  if (print) StartPage(hPr);       // start the next page
                  if (AbortPrint) break;
                  UncolateCopy++;
                  NextCopy=TRUE;
               }
               if (NextCopy) {
                  lin=PageBeginLine-1;
                  NextY=BeginY;
                  continue;
               }

               // enable printing of the current page
               if (!print && sel==IDC_RANGE_PAGES && PrintCurPage && lin==CurLine) {
                  NextY=BeginY;
                  print=TRUE;
                  FirstPage=LastPage=CurPage;
                  lin=PageBeginLine-1;
                  PageBeginCol=0;
                  continue;
               }
           }
           CurPage=0;
       }

       PRINTED:
       if (AbortPrint) break;
       if ((copy+1)<ColateCopies) {
         if ((ErrCode=EndPage(hPr))<0) {
            wsprintf(ErrString,"TerPrint2, Error Code: %d",ErrCode);
            PrintError(w,MSG_ERR_END_PAGE,ErrString);
         }
       }
    }

    if (AbortPrint) {
        AbortDoc(hPr);
        result=FALSE;
        TerLastMsg=MSG_PRINTING_CANCELLED;
    }
    else {
        if ((ErrCode=EndPage(hPr))<0) {
           wsprintf(ErrString,"TerPrint2(a), Error Code: %d",ErrCode);
           PrintError(w,MSG_ERR_END_PAGE,ErrString);
           result=FALSE;
        }
        if (EndDoc(hPr)<0)  PrintError(w,MSG_ERR_END_PRINTER,NULL);
    }

    END:

    // reset number of copies
    if (ResetCopies) {
        pDeviceMode->dmCopies=SaveCopies;
        ResetDC(hPr,pDeviceMode);
    }

    TerArg.FittedView=SaveFittedView;       // restore the original FittedView
    ShowPageBorder=SavePageBorder;          // restore this flag

    // Toggle off temporary view of page header/footer
    if (TempViewPageHdrFtr) ToggleViewHdrFtr(w);
    EditPageHdrFtr=SaveEditPageHdrFtr;      // restore flag

    // Toggle off temporary page mode
    if (TempPageMode) TogglePageMode(w);

    // disable any temporary PrintView mode
    if (TempPrintViewMode)  TerArg.PrintView=TerArg.PageMode=FALSE;

    SelectObject(hPr,GetStockObject(SYSTEM_FONT));  // deselect our font

    InPrinting=FALSE;
    HoldMessages=FALSE;              // enables window message processing
    if (UseScrMetrics)  ResetUintFlag(TerOpFlags2,TOFLAG2_SCALE_PRT_DC);

    RecreateFonts(w,hTerDC);         // recreate the display fonts
    PaintEnabled=SavePaintEnabled;

    EnableWindow(hTerWnd,TRUE);      // activate the main window

    if (ShowCancelDialog) {
       DestroyWindow(hAbortWnd);        // destroy the abort window
       FreeProcInstance(lpProc);        // free the process instances
       FreeProcInstance(lpAbortDlg);
    }

    TextBorder=ParaShading=0;          // reset text border

    hPrtDataDC=NULL;                   // reset the printer data DC
    if (UseScrMetrics) {
       DeleteDC(hPr);                  // delete the printer DC
       hPr=hTerDC;                     // reassign it back to the printer
    }

    if (HtmlMode) TerAdjustHtmlTable(hTerWnd);
       
    if (TerArg.FittedView) TerRepaginate(hTerWnd,FALSE);  // to length page sizes to fitted-view page sizes
    
    if(SavePvFirstFramePage != FirstFramePage && TerArg.PageMode) 
         CreateFrames(w,FALSE,SavePvFirstFramePage,SavePvLastFramePage);

    PaintTer(w);

    if (GoPrintPreview) {            // go back to print preview
       GoPrintPreview=FALSE;         // reset this flag
       PostMessage(hTerWnd,WM_COMMAND,ID_PRINT_PREVIEW,0L);  // terminate print preview
    }

    return result;
}

/******************************************************************************
    CalcPageDim:
    Calculate the page dimensions
******************************************************************************/
CalcPageDim(PTERWND w,long FromLine, int far *PagePixWidth,int far *PagePixHeight,int far *BeginX,int far *BeginY,int far *LastY)
{
    int sect;
    POINT pt;

    sect=GetSection(w,FromLine);           // get the beginnng section
    *PagePixWidth=(int)(PrtResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin));  // number of pixels in X direction for printing
    *PagePixHeight=(int)(PrtResY*(TerSect1[sect].PgHeight-TerSect[sect].TopMargin-TerSect[sect].BotMargin));   // number of pixels in Y direction for printing

    if (GetDeviceCaps(hPr,TECHNOLOGY)==DT_RASDISPLAY) pt.x=pt.y=0;
    else  Escape(hPr,GETPRINTINGOFFSET,0,NULL,&pt);// actual offset where printing begins

    *BeginX=PageX=(int)(TerSect[sect].LeftMargin*PrtResX)-pt.x;// beginning left position
    *BeginY=(int)(TerSect[sect].TopMargin*PrtResY)-pt.y; // beginning top position
    *LastY=(*BeginY)+(*PagePixHeight);                   // page end position

    return TRUE;
}

/******************************************************************************
    SpoolIt:
    Spool the current page to the printer.  This function can return a new
    printer device context for the next page.
******************************************************************************/
SpoolIt(PTERWND w,HDC hPrtDC)
{
    int result;

    result=EndPage(hPrtDC);

    // for Win32 the abort processdure does not get called automatically after EndPage
    if (Win32 && hAbortWnd) {
       BOOL ShowCancelDialog=!(TerFlags2&TFLAG2_NO_PRT_CANCEL_DLG);
       if (ShowCancelDialog) PrintProc(hPrtDC,0);
    }

    if (hCurAbortWnd) SetDlgItemInt(hCurAbortWnd,IDC_PAGE_NO,CurPage+1,FALSE);

    if (result<0) AbortPrint=TRUE;               // AbortPrint is also set by user cancel

    return TRUE;
}

/******************************************************************************
    GetLineLen:
    The length of the line that can be printed in one line.  This routine is
    called in non-word wrap mode only.  Therefore it assumes tabs at default
    positions.
******************************************************************************/
GetLineLen(PTERWND w,LPBYTE ptr,LPWORD fmt,LPBYTE lead, int BegCol,int EndCol,int PagePixWidth)
{
    int len=0,width,LinePixWidth=0;
    WORD CurFont;
    BYTE CurChar;

    while(PagePixWidth>0 && BegCol<=EndCol) {
        CurFont=fmt[BegCol];
        CurChar=ptr[BegCol];
        if (CurChar==TAB) width=GetPrtTabWidth(w,LinePixWidth,0,NULL);
        else width=DblCharWidth(w,CurFont,TRUE,CurChar,LeadByte(lead,BegCol));
        BegCol++;
        len++;
        PagePixWidth-=width;
        LinePixWidth+=width;
    }
    return len;
}

/******************************************************************************
    PrintOneLine:
    Print a line using appropriate fonts.
    The argument specify the pixel coordinates of the text, text pointer,
    font pointer, length of the text to print, the line height, and the
    height of the base line.
******************************************************************************/
PrintOneLine(PTERWND w,long LineNo, HDC hPrtDC,HDC hAttrDC, int BeginX,int BeginY,
             LPBYTE ptr,LPWORD fmt,LPBYTE lead,int len,int height, int BaseHeight,
             struct StrTabw far *CurTabw,int CurPara,int SpcBef, int SpcAft,int ExtLead, 
             int FrameNo,int LeftMargin, int RowBeginX, LPWORD pWidth, BOOL HasBullet,
             int BulletX,int SectLineX)
{
    WORD PrevFont,CurFont;
    BYTE PrevChar=0,PrevLead=0;
    int  TabNo=0,TabId,BaseY;
    int CurLen,TextLen,i,CurPixLen=0,PrintedLen,width;
    BOOL justify=FALSE,IsFramePict;
    int  SpaceNo=0,BoxAdj=0,BoxBeginX=-1,BoxEndX=0,TabX,NextSegStart;
    RECT rect;
    HBRUSH hBr;
    BOOL CurSegRtl;
    BOOL DrawingFootnote=(TerOpFlags2&TOFLAG2_DRAW_FNOTE);
    WORD UcBase[MAX_WIDTH];
    BOOL HasUcBase=FALSE,FontBreak=FALSE;


    if (len==0) goto PRINT_BORDER;            // nothing to print

    PrevFont=CurFont=0;
    CurLen=CurPixLen=PrintedLen=0;
    if (CurTabw && CurTabw->type&INFO_JUST) justify=TRUE;

    // check for rtl
    if (!DrawingFootnote) {       // already calculated
       CurScrSeg=0;
       ScrRtl=LineFlags2(LineNo)&LFLAG2_RTL;
       if (ScrRtl) pScrSeg=GetLineSeg(w,LineNo,pWidth,&TotalScrSeg);
       if (!pScrSeg) ScrRtl=FALSE;
    }

    if (ScrRtl) {
       if (LineInfo(w,LineNo,INFO_FRAME) && tabw(LineNo)->FrameCharPos==0 && !(TerOpFlags&TOFLAG2_DRAW_FNOTE)) RowBeginX-=tabw(LineNo)->FrameSpaceWidth;

       if (RowBeginX>0) for (i=0;i<TotalScrSeg;i++) pScrSeg[i].x+=RowBeginX;
       CurScrSeg=GetCharSeg(w,LineNo,0,TotalScrSeg,pScrSeg);
       NextSegStart=pScrSeg[CurScrSeg].col+pScrSeg[CurScrSeg].count;
       CurSegRtl=pScrSeg[CurScrSeg].rtl;
    }
    else {
       CurScrSeg=0;
       CurSegRtl=FALSE;
       NextSegStart=9999;
    }  

    // Extract and print line segments
    for (i=0;i<=len;i++) {
       if (i<len) CurFont=fmt[i];

       if (i<len) UcBase[i]=TerFont[CurFont].UcBase;

       if (i<len) {
         FontBreak=(PrevFont!=CurFont);
         if (TerFont[CurFont].UcBase && TerFont[PrevFont].UcBase) FontBreak=IsFontBreak(w,CurFont,PrevFont);
       } 

       if (   i==len || FontBreak || (PrevChar==TAB && TerFont[PrevFont].height!=0) 
          || PrevChar==HYPH_CHAR || PrevChar==HIDDEN_CHAR || PrevChar==ZWNJ_CHAR
          || i>=NextSegStart
          || (ScrRtl && CurSegRtl && (ptr[i-1]==' ' || ptr[i]==' ')) 
          || (justify && ptr[i]==' ' && JustifySpace(w,PrevFont))
          || (i>0 && CurTabw && CurTabw->type&INFO_FRAME && CurTabw->FrameCharPos==i) ) {// respond to format change or tab character

          TextLen=CurLen;                      // number of characters to print
          if (PrevChar==TAB) TextLen--;
          if (i==len && PrevChar==HYPH_CHAR) PrevChar='-';  // convert to regular dash
          if (PrevChar==HYPH_CHAR || PrevChar==HIDDEN_CHAR || PrevChar==ZWNJ_CHAR) TextLen--;

          if (TextLen>0) {
             // draw the background rectangle
             PrtTextBkColor=TerFont[PrevFont].TextBkColor;
             if (TerFont[PrevFont].TextBkColor!=CLR_WHITE) {
                 rect.left=BeginX;
                 rect.right=rect.left+CurPixLen;
                 rect.top=BeginY+SpcBef;
                 rect.bottom=rect.top+height;
                 if (ScrRtl) RtlRect(w,&rect,CurFrame,&(pScrSeg[CurScrSeg]));
                 hBr=CreateSolidBrush(TerFont[PrevFont].TextBkColor);
                 FillRect(hPrtDC,&rect,hBr);
                 DeleteObject(hBr);
             }

             // print this line segment
             IsFramePict=(TerFont[PrevFont].style&PICT && TerFont[PrevFont].ParaFID>0);
             if (!IsFramePict) PrintOneSegment(w,hPrtDC,hAttrDC,PrevFont,BeginX+BoxAdj,BeginY+SpcBef,&ptr[PrintedLen],LeadPtr(lead,PrintedLen),TextLen,BaseHeight,ExtLead,0,&pWidth[PrintedLen],HasUcBase?&(UcBase[PrintedLen]):NULL);

             // draw the character box rectangle
             if (BoxEndX>0) {
                 int  delta=PrtResY/ScrResY;  // one screen pixel width
                 int  BoxY=BeginY+SpcBef;

                 DrawShadowLine(w,hPrtDC,BoxBeginX+delta,BoxY+delta,BoxEndX-delta,BoxY+delta,GetStockObject(BLACK_PEN),NULL); //top line
                 DrawShadowLine(w,hPrtDC,BoxBeginX+delta,BoxY+height-2*delta,BoxEndX-delta,BoxY+height-2*delta,GetStockObject(BLACK_PEN),NULL); //bottom line
                 DrawShadowLine(w,hPrtDC,BoxBeginX+delta,BoxY+delta,BoxBeginX+delta,BoxY+height-2*delta,GetStockObject(BLACK_PEN),NULL); //left line
                 DrawShadowLine(w,hPrtDC,BoxEndX-delta,BoxY+delta,BoxEndX-delta,BoxY+height-2*delta,GetStockObject(BLACK_PEN),NULL); //right line
                 BoxBeginX=-1;                 // reset
                 BoxEndX=0;                   // reset the box attribute
             }
             HasUcBase=FALSE;
          }

          PrintedLen+=CurLen;
          BeginX+=CurPixLen;          // add the width of the text just printed

          // insert any frame reserved space
          if (i>0 && CurTabw && CurTabw->type&INFO_FRAME && CurTabw->FrameCharPos==i) BeginX+=ScrToPrtX(CurTabw->FrameScrWidth);

          if (PrevChar==TAB) {                  // add the tab width
             width=pWidth[i-1];
             if (TerFont[PrevFont].TextBkColor!=CLR_WHITE) {  // print any shading for the tab
                 rect.left=BeginX;
                 rect.right=rect.left+width;
                 rect.top=BeginY+SpcBef;
                 rect.bottom=rect.top+height;
                 hBr=CreateSolidBrush(TerFont[PrevFont].TextBkColor);
                 FillRect(hPrtDC,&rect,hBr);
                 DeleteObject(hBr);
             }

             // draw dot leaders and underlines
             TabId=PfmtId[CurPara].TabId;
             TabX=BeginX;
             if (TerArg.PageMode) TabX-=(LeftMargin+frame[FrameNo].x+frame[FrameNo].SpaceLeft);  // the entire frame set is created relative to the left margin
             GetTabPos(w,pfmt(LineNo),&TerTab[TabId],TabX,NULL,NULL,&TabFlags,FALSE);
             
             BaseY=BeginY+SpcBef+BaseHeight+ExtLead;

             if (TabFlags==TAB_DOT)
                DrawDottedLine(w,hPrtDC,BeginX,BaseY,BeginX+width, BaseY,0);
             else if (TabFlags==TAB_HYPH) {
                HPEN hPen=CreatePen(PS_DOT,0,0);
                DrawShadowLine(w,hPrtDC,BeginX,BaseY-2,BeginX+width,BaseY-2,hPen,NULL);
                DeleteObject(hPen);
             }
             else if (TabFlags==TAB_ULINE || TerFont[PrevFont].style&(ULINE|ULINED)) {
                HPEN hPen;
                COLORREF color=0;
                int thickness=PointsToPrtY(1)/2;
                //if (TerFont[PrevFont].style&BOLD) thickness=thickness*2;

                if (TerFont[PrevFont].style&(ULINE&ULINED) && TerFont[PrevFont].UlineColor!=CLR_AUTO)
                    color=TerFont[PrevFont].UlineColor;

                if (thickness<1) thickness=1;
                if (NULL!=(hPen=CreatePen(PS_SOLID,thickness,0))) {
                   int y=BaseY+2*thickness;
                   if (TerFont[PrevFont].style&ULINED) y-=thickness;
                   DrawShadowLine(w,hPrtDC,BeginX,y,BeginX+width,y,hPen,NULL);
                   if (TerFont[PrevFont].style&ULINED) {
                      y+=(2*thickness);
                      DrawShadowLine(w,hPrtDC,BeginX,y,BeginX+width,y,hPen,NULL);
                   } 
                   DeleteObject(hPen);
                }
             }

             TabFlags=0;                       // reset the global variables

             BeginX=BeginX+width;              // add tab width
             TabNo++;
          }
          CurLen=CurPixLen=0;
          BoxAdj=0;
          CharBoxLeft=CharBoxRight=FALSE;

          // make uc base array
          if (i<len && UcBase[i]>0) HasUcBase=TRUE;
          PrevFont=CurFont;

          if (ScrRtl && pScrSeg && i>=NextSegStart) {     // check next rtf/ltr seg start
             CurScrSeg++;
             NextSegStart=pScrSeg[CurScrSeg].col+pScrSeg[CurScrSeg].count;
             CurSegRtl=pScrSeg[CurScrSeg].rtl;
          } 
       }

       if (i==len) break;                   // end of the line

       // update the line pixel width
       PrevChar=ptr[PrintedLen+CurLen];
       PrevLead=LeadByte(lead,PrintedLen+CurLen);

       if (PrevChar!=TAB) {
          BOOL BeginBox=FALSE,EndBox=FALSE;
          width=0;   // current character width

          // check if character box begsin or ends
          if (CurTabw && (PrintedLen+CurLen)<CurTabw->CharFlagsLen && CurTabw->CharFlags[PrintedLen+CurLen]&BFLAG_BOX_FIRST) BeginBox=TRUE;
          if ((CurTabw && (PrintedLen+CurLen)<CurTabw->CharFlagsLen && CurTabw->CharFlags[PrintedLen+CurLen]&BFLAG_BOX_LAST)
              || (i==(len-1) && BoxBeginX!=-1)) EndBox=TRUE;

          if (BeginBox) {
              BoxBeginX=BeginX+CurPixLen;
              if (CurLen==0) BoxAdj=ExtraSpacePrtX;  // adjustment to print x position
          }

          // add actual character width
          IsFramePict=(TerFont[CurFont].style&PICT && TerFont[CurFont].ParaFID>0);
          if (!IsFramePict) width+=pWidth[i];

          CurPixLen+=width;

          // add extra character spaces before the character
          if (BeginBox) pWidth[i]-=ExtraSpacePrtX;
          if (EndBox) pWidth[i]-=ExtraSpacePrtX;

          // add extra character spaces after the character
          if (EndBox) BoxEndX=BeginX+CurPixLen;
       }

       CurLen++;
    }

    PRINT_BORDER:
    // Draw paragraph border
    if (TextBorder&PARA_BOX) {
       rect.left=BoxLeft;
       rect.right=BoxRight;   // enought to cover the right side


       if (LineNo>=0 && cid(LineNo)==0 && fid(LineNo)==0) {
          int adj=20;
          if (True(PfmtId[pfmt(LineNo)].flags&(PARA_BOX_THICK))) adj+=20;
          if (True(PfmtId[pfmt(LineNo)].flags&(PARA_BOX_DOUBLE))) adj+=40;
          rect.left-=PointsToPrtX(adj);  // enough to cover the right side
          rect.right+=PointsToPrtX(adj);
       } 
       if (LineNo>=0 && cid(LineNo)>0) {
          rect.right-=PointsToPrtX(1);  // try to bring it within the cell border
       }
       
       rect.top=BeginY+ExtLead;
       rect.bottom=BeginY/*+ExtLead*/+height+SpcBef+SpcAft;    // ExtLead causes overlap with next line with HP 4 printer on WIN NT

       ParaBoxColor=PfmtId[pfmt(LineNo)].BorderColor;
       DrawParaBorder(w,hPrtDC,&rect,TRUE);

       TextBorder=ParaShading=0;
       ParaBoxColor=CLR_AUTO;
    }

    // draw the bullet character for the paragraph
    if (HasBullet) {
       //int x=BeginX+LeftIndent+AdjX-BulletWidth;
       //DrawBullet(w,hPrtDC,hPrtDataDC,line,CurParaId,x,NextY+SpcBef+ExtLead,BaseHeight,TRUE);
       DrawBullet(w,hPrtDC,hPrtDataDC,LineNo,pfmt(LineNo),BulletX,BeginY+SpcBef+ExtLead,BaseHeight,TRUE);
    }

    // draw the section line numbering
    if (SectLine>=0) DrawSectLineNbr(w,hPrtDC,hPrtDataDC,LineNo-frame[FrameNo].PageFirstLine,SectLineX,BeginY+SpcBef+ExtLead,BaseHeight);
        
    // draw reviewed text indicator line
    if (True(LineFlags2(LineNo)&(LFLAG2_DEL_REV|LFLAG2_INS_REV))) DrawTrackingLine(w,hPrtDC,hPrtDataDC,SectLineX,BeginY,height);


    CharBoxLeft=CharBoxRight=FALSE;     // reset character box flags

    if (!(TerOpFlags2&TOFLAG2_DRAW_FNOTE)) {       // handled byte the calling function
       if (pScrSeg) MemFree(pScrSeg);
       pScrSeg=NULL;
       ScrRtl=FALSE;
    }


    return TRUE;
}



/******************************************************************************
    PrintOneSegment:
    This routine prints a segment of the line that uses just
    one font.  This routine will create the needed font, and delete it
    after it is done.
*******************************************************************************/
PrintOneSegment(PTERWND w,HDC hPrtDC,HDC hAttrDC,int CurFont,int BeginX,int BeginY,LPBYTE ptr,LPBYTE lead,int len,int BaseHeight,int ExtLead, int JustAdj, LPWORD pWidth, LPWORD pUcBase)
{
    int i,PixLen,TextLen,width,height,LineYPos,LastX,thickness;
    int SaveFmtId=CurFmtId;
    COLORREF SaveForeColor=CurForeColor;
    COLORREF SaveBackColor=CurBackColor;
    HPEN hPen,hOldPen;
    HFONT hOldFont;
    DWORD TextColor;
    DWORD shade;
    BYTE  string[MAX_WIDTH+2];
    LPINT dx;
    LPBYTE pText;
    LPWORD pUText;
    RECT rect;

    if (len>0) {
       BeginY=BeginY+BaseHeight-TerFont[CurFont].BaseHeight+ExtLead;

       if (TerFont[CurFont].style&PICT) {             // draw CurFontures
          width=TerFont[CurFont].CharWidth[PICT_CHAR];     // width and height of the CurFonture
          height=TerFont[CurFont].height;
          PrintPicture(w,hPrtDC,hAttrDC,CurFont,BeginX,BeginY,width,height,len);
       }
       else {
          if (!HiddenText(w,CurFont)) {
             hOldFont=SelectObject(hPrtDC,TerFont[CurFont].hFont);     // select the font

             SetBkMode(hPrtDC,TRANSPARENT);

             // set text color
             TextColor=TerFont[CurFont].TextColor; // set text color
             if (True(TerFont[CurFont].InsRev)) TextColor=reviewer[TerFont[CurFont].InsRev].InsColor;
             if (True(TerFont[CurFont].DelRev)) TextColor=reviewer[TerFont[CurFont].DelRev].DelColor;

             if (TextColor==CLR_AUTO || (HtmlMode && TextColor==0xFFFFFF)) {   // automatic color
                COLORREF BkColor=PrtTextBkColor;
                if (BkColor==CLR_WHITE) BkColor=PrtParaBkColor;
                if (BkColor==CLR_WHITE) BkColor=PrtFrameBkColor;
                TextColor=(~BkColor)&0xFFFFFF;
             }
             if (ParaShading) {
                shade=MulDiv(ParaShading,0xFF,10000);
                if (shade>0xFF) shade=0xFF;
                shade=(shade<<16)|(shade<<8)|shade; // all comonents the same
                if (TextColor==TerFont[0].TextColor) TextColor^=shade;
             }
             CurForeColor=TextColor;    // for OurExtTextOut function
             SetTextColor(hPrtDC,TextColor); // set text color

             // prepare the text string to print
             pText=OurAlloc(2*MAX_WIDTH+2);
             
             if (pUcBase) {     // unicode font
                pUText=OurAlloc(sizeof(WORD)*(MAX_WIDTH+1));
                for (i=0;i<len;i++) {
                   pUText[i]=MakeUnicode(w,pUcBase[i],ptr[i]);
                   pText[i]=string[i]=ptr[i];
                }
                pText[i]=string[i]=0;
                pUText[i]=0;
                TextLen=len;
             }
             else { 
                // copy the text to the temporary string
                for (i=0;i<len;i++) {
                   string[i]=ptr[i];
                   if (ptr[i]==NBSPACE_CHAR) string[i]=' '; // translate the nonbreaking space character
                   if (ptr[i]==NBDASH_CHAR) string[i]='-';  // translate the nonbreaking dash character
                   if (ptr[i]==HYPH_CHAR) string[i]='-';    // translate the optional hyphen character
                }
                string[i]=0;

                // translate '(' and ')' for rtl display
                //if (ScrRtl && len==1 && TerFont[CurFont].rtl) {
                //   if      (ptr[0]=='(') string[0]=')';
                //   else if (ptr[0]==')') string[0]='(';
                //}
                
                // create the mbcs text
                TextLen=TerMakeMbcs(hTerWnd,pText,string,lead,strlen(string));
             }
             
             // do scap translation
             if (pUcBase==NULL && TerFont[CurFont].style&(SCAPS|CAPS)) {
                for (i=0;i<TextLen;i++) if ((lead==NULL || lead[i]==0) && IsLcChar(pText[i])) pText[i]+=(BYTE)('A'-'a');  // convert to upper case
             } 

             // draw the text
             if (NULL==(dx=OurAlloc(sizeof(int)*(TextLen+1)))) return PrintError(w,MSG_OUT_OF_MEM,"PrintOneSegment");

             
             BuildMbcsCharWidth(w,dx,CurFont,pText,TextLen,string,lead,strlen(string),pWidth);
             for (i=0,PixLen=0;i<TextLen;i++) PixLen+=dx[i];
             
             rect.left=BeginX;             // rectangle needed for rtl text output
             rect.right=rect.left+PixLen;
             rect.top=BeginY+TerFont[CurFont].BaseHeightAdj;      // top/bottom values are ignored
             rect.bottom=rect.top+TerFont[CurFont].height;

             CurFmtId=CurFont;            // used by the OurExtTextOut function
             if (pUcBase) OurExtTextOutW(w,hPrtDC,BeginX,BeginY+TerFont[CurFont].BaseHeightAdj,0,&rect,pUText,TextLen,dx);
             else         OurExtTextOut(w,hPrtDC,BeginX,BeginY+TerFont[CurFont].BaseHeightAdj,0,&rect,pText,TextLen,dx);

             OurFree(dx);
             OurFree(pText);
             if (pUcBase) OurFree(pUText);

             SelectObject(hPrtDC,hOldFont);           // release font

             // display double underline format
             if (TerFont[CurFont].style&(ULINED|ULINE) /*&& !(TerFont[CurFont].style&(SUPSCR|SUBSCR))*/) {
                COLORREF color=TerFont[CurFont].TextColor;

                thickness=PointsToPrtY(1)/2;          // line thick ness
                //if (TerFont[CurFont].style&ULINE && TerFont[CurFont].style&BOLD) thickness=thickness*2;
                if (thickness<1) thickness=1;
                
                if (TerFont[CurFont].UlineColor!=CLR_AUTO) color=TerFont[CurFont].UlineColor;
                
                if (NULL!=(hPen=CreatePen(PS_SOLID,thickness,color))) {
                   hOldPen=SelectObject(hPrtDC,hPen);

                   LineYPos=BeginY+TerFont[CurFont].BaseHeight+thickness;
                   if (TerFont[CurFont].style&ULINE) LineYPos+=thickness;
                   LastX=BeginX+PixLen;
                   OurMoveToEx(w,hPrtDC,BeginX-JustAdj,LineYPos,NULL);// draw first line
                   OurLineTo(w,hPrtDC,LastX,LineYPos);

                   if (TerFont[CurFont].style&ULINED) {  // draw the second line
                      LineYPos=LineYPos+2*thickness;           // draw second line 2 points down
                      OurMoveToEx(w,hPrtDC,BeginX-JustAdj,LineYPos,NULL);// draw first line
                      OurLineTo(w,hPrtDC,LastX,LineYPos);
                   }

                   SelectObject(hPrtDC,hOldPen);
                   DeleteObject(hPen);
                }
             }
          }
       }
    }
    
    CurFmtId=SaveFmtId;
    CurForeColor=SaveForeColor;
    CurBackColor=SaveBackColor;

    return TRUE;
}

/******************************************************************************
    PrintPicture:
    This routine prints picture.
*******************************************************************************/
PrintPicture(PTERWND w,HDC hPrtDC,HDC hAttrDC,int CurFont,int BeginX,int BeginY,int width, int height, int count)
{
    void far *pImage;
    BITMAPINFO far *pInfo;
    POINT CurVpOrg,CurWinOrg;
    SIZE  CurVpExt,CurWinExt;
    MFENUMPROC lpProc;
    int PrevStretchMode;
    int DrawX=BeginX;
    int DrawY=BeginY;
    int DrawWidth=width;
    int DrawHeight=height;
    BOOL crop,UseOleDraw;
    HRGN rgn;

    // apply CurFonture cropping to the start pos
    crop=(TerFont[CurFont].CropLeft || TerFont[CurFont].CropRight || TerFont[CurFont].CropTop || TerFont[CurFont].CropBot);  // Is cropping used
    if (crop && TerFont[CurFont].OrigPictWidth!=0) {  // convert twips to Pixel
       float scale=(float)TerFont[CurFont].PictWidth/(TerFont[CurFont].OrigPictWidth-TerFont[CurFont].CropLeft-TerFont[CurFont].CropRight);  // non-zero scale implies that the CurFonture was resized
       int CropLeft=(int)(TerFont[CurFont].CropLeft*scale);
       DrawX-=TwipsToPrtY(CropLeft);
       DrawWidth=(int)(TerFont[CurFont].OrigPictWidth*scale);
       DrawWidth=TwipsToPrtX(DrawWidth);
    } 
    if (crop && TerFont[CurFont].OrigPictHeight!=0) {  // convert twips to Pixel
       float scale=(float)TerFont[CurFont].PictHeight/(TerFont[CurFont].OrigPictHeight-TerFont[CurFont].CropTop-TerFont[CurFont].CropBot);  // non-zero scale implies that the CurFonture was resized
       int CropTop=(int)(TerFont[CurFont].CropTop*scale);
       DrawY-=TwipsToPrtY(CropTop);
       DrawHeight=(int)(TerFont[CurFont].OrigPictHeight*scale);
       DrawHeight=TwipsToPrtY(DrawHeight);
    } 

    
    // reset rotation
    if (ScrFrameAngle>0 && OSCanRotate) FrameNoRotateDC(w,hPrtDC);
    
    // check if ole draw method should be used
    UseOleDraw=false;
    if (True(TerFlags6&TFLAG6_IN_PLACE_ACTIVATE) && True(TerOpFlags2&TOFLAG2_PDF)) UseOleDraw=true;

    if ((UseOleDraw || TerFont[CurFont].ObjectType==OBJ_OCX) && TerFont[CurFont].pObject!=NULL) {
       LPOLEOBJECT pObject;
       RECT PictRect;
       HRESULT result;

       pObject=TerFont[CurFont].pObject;
          
       PictRect.left=DrawX;
       PictRect.top=DrawY;
       PictRect.right=PictRect.left+DrawWidth;
       PictRect.bottom=PictRect.top+DrawHeight;
          
       result=OleDraw((LPUNKNOWN)pObject,DVASPECT_CONTENT,hPrtDC,&PictRect);
       if (result==S_OK) goto END;
    } 
    
    if (TerFont[CurFont].PictType==PICT_DIBITMAP) {
       pInfo=(LPVOID)TerFont[CurFont].pInfo;
       pImage=(LPVOID)TerFont[CurFont].pImage;

       PrevStretchMode=SetStretchBltMode(hPrtDC,STRETCH_DELETESCANS);  // can't afford any more colors generated

       if (crop) {      // set the clipping region
          POINT pt[2];
          
          SaveDC(hPrtDC);
          pt[0].x=BeginX;
          pt[0].y=BeginY;
          pt[1].x=BeginX+width;
          pt[1].y=BeginY+height;
          LPtoDP(hPrtDC,pt,2);
          rgn=CreateRectRgn(pt[0].x,pt[0].y,pt[1].x,pt[1].y);
          SelectClipRgn(hPrtDC,rgn);
          DeleteObject(rgn);
       }

       while(count>0) {                           // draw bitmaps
          OurStretchDIBits(w,hPrtDC,DrawX,DrawY,DrawWidth,DrawHeight,0,0,TerFont[CurFont].bmWidth,TerFont[CurFont].bmHeight,pImage,pInfo,DIB_RGB_COLORS,SRCCOPY);
          BeginX=BeginX+TerFont[CurFont].CharWidth[PICT_CHAR];
          DrawX=DrawX+TerFont[CurFont].CharWidth[PICT_CHAR];
          count--;
       }

       if (crop) RestoreDC(hPrtDC,-1);

       SetStretchBltMode(hPrtDC,PrevStretchMode);

    }
    else if (TerFont[CurFont].PictType==PICT_METAFILE
          || TerFont[CurFont].PictType==PICT_META7) {
       BOOL PrintToMetafile=(GetDeviceCaps(hPrtDC,TECHNOLOGY)==DT_METAFILE);
       HRGN rgn;
       POINT pt[2];
       RECT rect;

       SaveDC(hPrtDC);                  // save the current status of the printer device context

       // adjust for text rotation
       if (ScrFrameAngle>0) {
          SetRect(&rect,BeginX,BeginY,BeginX+width,BeginY+height);
          FrameRotateRect(w,&rect,CurFrame);
          BeginX=(rect.left<rect.right?rect.left:rect.right);
          BeginY=(rect.top<rect.bottom?rect.top:rect.bottom);
          SwapInts(&width,&height);
          
          SetRect(&rect,DrawX,DrawY,DrawX+DrawWidth,DrawY+DrawHeight);
          FrameRotateRect(w,&rect,CurFrame);
          DrawX=(rect.left<rect.right?rect.left:rect.right);
          DrawY=(rect.top<rect.bottom?rect.top:rect.bottom);
          SwapInts(&DrawWidth,&DrawHeight);
       }

       // set clipping for metafile
       pt[0].x=BeginX;
       pt[0].y=BeginY;
       pt[1].x=BeginX+width;
       pt[1].y=BeginY+height;
       LPtoDP(hPrtDC,pt,2);
       if (hPrtDC==hTerDC && InPrintPreview) {
          if (pt[0].y<TerWinRect.top) pt[0].y=TerWinRect.top;
          if (pt[0].x<TerWinRect.left) pt[0].x=TerWinRect.left;
          if (pt[1].y>TerWinRect.bottom) pt[1].y=TerWinRect.bottom;
          if (pt[1].x>TerWinRect.right) pt[1].x=TerWinRect.right;
       }
       else if ((TerOpFlags&TOFLAG_MERGE_PRINT_RECT) && !PrintToMetafile) { // merge print to a rectangle
          if (pt[0].y<MPRect.top)    pt[0].y=MPRect.top;
          if (pt[0].x<MPRect.left)   pt[0].x=MPRect.left;
          if (pt[1].y>MPRect.bottom) pt[1].y=MPRect.bottom;
          if (pt[1].x>MPRect.right)  pt[1].x=MPRect.right;
       }
       if (!PrintToMetafile && ScrFrameAngle==0) {
          rgn=CreateRectRgn(pt[0].x,pt[0].y,pt[1].x,pt[1].y);
          SelectClipRgn(hPrtDC,rgn);
          DeleteObject(rgn);
       }

       GetViewportOrgEx(hAttrDC,&CurVpOrg);     // get the current origin
       GetWindowOrgEx(hAttrDC,&CurWinOrg);      // get the current origin
       GetWindowExtEx(hAttrDC,&CurWinExt);
       GetViewportExtEx(hAttrDC,&CurVpExt);

       if (TerFont[CurFont].PictType==PICT_META7)
             SetMapMode(hPrtDC,MM_ISOTROPIC);
       else  SetMapMode(hPrtDC,MM_ANISOTROPIC); // use customized units

       SelectObject(hPrtDC,GetStockObject(SYSTEM_FONT));

       while(count>0) {                         // draw bitmaps
          if (PrintToMetafile || TerFlags4&TFLAG4_PRINT_WMF_AS_BMP) {
             HDC hTempDC=CreateCompatibleDC(hTerDC/*hAttrDC*/);      // use hTerDC instead of hAttrDC to preserve colors
             HBITMAP hTempBM,hPrevBM;
             RECT rect;
             int  TempWidth=DrawWidth,TempHeight=DrawHeight;

             if (TerFlags4&TFLAG4_REPORT_EASE_MODE) {           // ReportEase unit is printer, use screen unit to limit the size of the resulting bitmap
                TempWidth=PrtToScrX(DrawWidth);
                TempHeight=PrtToScrX(DrawHeight);
             } 

             hTempBM=CreateCompatibleBitmap(hTerDC/*hAttrDC*/,TempWidth,TempHeight);
             hPrevBM=SelectObject(hTempDC,hTempBM);

             // clear the background
             SetRect(&rect,0,0,DrawWidth,DrawHeight);
             FillRect(hTempDC,&rect,GetStockObject(WHITE_BRUSH));

             SaveDC(hTempDC);
             SetMapMode(hTempDC,MM_ANISOTROPIC);
             if (TerFont[CurFont].PictType==PICT_METAFILE) {
                 if (GetDeviceCaps(hAttrDC,TECHNOLOGY)==DT_RASDISPLAY) 
                      SetWindowExtEx(hTempDC,TwipsToScrX(TerFont[CurFont].bmWidth),TwipsToScrY(TerFont[CurFont].bmHeight),NULL); // logical dimension
                 else SetWindowExtEx(hTempDC,TwipsToPrtX(TerFont[CurFont].bmWidth),TwipsToPrtY(TerFont[CurFont].bmHeight),NULL); // logical dimension
             }
             else SetWindowExtEx(hTempDC,TerFont[CurFont].bmWidth,TerFont[CurFont].bmHeight,NULL); // logical dimension

             SetViewportExtEx(hTempDC,TempWidth,TempHeight,NULL); // equivalent device dimension
             SetViewportOrgEx(hTempDC,0,0,NULL);
             SetWindowOrgEx(hTempDC,0,0,NULL);

             if (NULL!=(lpProc=(MFENUMPROC)MakeProcInstance((FARPROC)PrtEnumMetafile, hTerInst))) {
                 EnumMetaFile(hTempDC,TerFont[CurFont].hMeta,lpProc,(LPARAM)w);
                 FreeProcInstance((FARPROC)lpProc);        // free the process instances
             }

             RestoreDC(hTempDC,-1);

             // copy from bitmap to metafile
             StretchBlt(hPrtDC,DrawX,DrawY,DrawWidth,DrawHeight,hTempDC,0,0,TempWidth,TempHeight,SRCCOPY);

             SelectObject(hTempDC,hPrevBM);
             DeleteObject(hTempBM);
             DeleteDC(hTempDC);
             
          }
          else {
             // most metafile would override the SetWindowExtEx to set it to the actual drawing height
             // Here we are setting the values when a metafile does not have an override.  In this case
             // we assume that the metafile was created in screen units
             SetWindowExtEx(hPrtDC,MulDiv(TerFont[CurFont].bmWidth,OrigScrResX,1440),MulDiv(TerFont[CurFont].bmHeight,OrigScrResY,1440),NULL); // logical dimension
             
             SetViewportExtEx(hPrtDC,
                  MulDiv(DrawWidth,CurVpExt.cx,CurWinExt.cx),
                  MulDiv(DrawHeight,CurVpExt.cy,CurWinExt.cy),NULL); // equivalent device dimension
             SetViewportOrgEx(hPrtDC,
                  CurVpOrg.x+MulDiv(DrawX-CurWinOrg.x,CurVpExt.cx,CurWinExt.cx),
                  CurVpOrg.y+MulDiv(DrawY-CurWinOrg.y,CurVpExt.cy,CurWinExt.cy),NULL);  // set view port origins

            if (NULL!=(lpProc=(MFENUMPROC)MakeProcInstance((FARPROC)PrtEnumMetafile2, hTerInst))) {
                 EnumMetaFile(hPrtDC,TerFont[CurFont].hMeta,lpProc,(LPARAM)w);
                 FreeProcInstance((FARPROC)lpProc);        // free the process instances
            }
          }

          BeginX=BeginX+TerFont[CurFont].CharWidth[PICT_CHAR];
          count--;
       }

       RestoreDC(hPrtDC,-1);
    }
    #if defined(WIN32)
    else if( TerFont[CurFont].PictType==PICT_ENHMETAFILE ) {
        RECT rect;
        while(count>0) {                         // draw bitmaps
          if (ScrFrameAngle>0) {
             SetRect(&rect,BeginX,BeginY,BeginX+width,BeginY+height);
             FrameRotateRect(w,&rect,CurFrame);
             BeginX=(rect.left<rect.right?rect.left:rect.right);
             BeginY=(rect.top<rect.bottom?rect.top:rect.bottom);
             SwapInts(&width,&height);

             SetRect(&rect,DrawX,DrawY,DrawX+DrawWidth,DrawY+DrawHeight);
             FrameRotateRect(w,&rect,CurFrame);
             DrawX=(rect.left<rect.right?rect.left:rect.right);
             DrawY=(rect.top<rect.bottom?rect.top:rect.bottom);
             SwapInts(&DrawWidth,&DrawHeight);
          }
            
          if (crop) {      // set the clipping region
             POINT pt[2];
          
             SaveDC(hPrtDC);
             pt[0].x=BeginX;
             pt[0].y=BeginY;
             pt[1].x=BeginX+width;
             pt[1].y=BeginY+height;
             LPtoDP(hPrtDC,pt,2);
             rgn=CreateRectRgn(pt[0].x,pt[0].y,pt[1].x,pt[1].y);
             SelectClipRgn(hPrtDC,rgn);
             DeleteObject(rgn);
          }

          if (True(TerFlags6&TFLAG6_PRINT_EMF_AS_BMP)) {
             HDC hTempDC=CreateCompatibleDC(hTerDC/*hAttrDC*/);      // use hTerDC instead of hAttrDC to preserve colors
             HBITMAP hTempBM,hPrevBM;
             RECT rect;
             int  TempWidth=DrawWidth,TempHeight=DrawHeight;

             if (TerFlags4&TFLAG4_REPORT_EASE_MODE) {           // ReportEase unit is printer, use screen unit to limit the size of the resulting bitmap
                TempWidth=PrtToScrX(DrawWidth);
                TempHeight=PrtToScrX(DrawHeight);
             } 

             hTempBM=CreateCompatibleBitmap(hTerDC/*hAttrDC*/,TempWidth,TempHeight);
             hPrevBM=SelectObject(hTempDC,hTempBM);

             // clear the background
             SetRect(&rect,0,0,DrawWidth,DrawHeight);
             FillRect(hTempDC,&rect,GetStockObject(WHITE_BRUSH));

             rect.left = 0;
             rect.right = rect.left + DrawWidth;
             rect.top = 0;
             rect.bottom = rect.top + DrawHeight;
            
             //PlayEnhMetaFile(hPrtDC, TerFont[CurFont].hEnhMeta, &rect);
             EnumEnhMetaFile(hTempDC, TerFont[CurFont].hEnhMeta, &ScrEnhMetaFileProc, (LPVOID) w, &rect);

             // copy from bitmap to metafile
             StretchBlt(hPrtDC,DrawX,DrawY,DrawWidth,DrawHeight,hTempDC,0,0,TempWidth,TempHeight,SRCCOPY);

             SelectObject(hTempDC,hPrevBM);
             DeleteObject(hTempBM);
             DeleteDC(hTempDC);
          }
          else {  

             rect.left = DrawX;
             rect.right = rect.left + DrawWidth;
             rect.top = DrawY;
             rect.bottom = rect.top + DrawHeight;
            
             //PlayEnhMetaFile(hPrtDC, TerFont[CurFont].hEnhMeta, &rect);
             EnumEnhMetaFile(hPrtDC, TerFont[CurFont].hEnhMeta, &ScrEnhMetaFileProc, (LPVOID) w, &rect);
            
             if (crop) RestoreDC(hPrtDC,-1);
            
          }
          BeginX=BeginX+TerFont[CurFont].CharWidth[PICT_CHAR];
          count--;
        }
    }
    #endif
    else if (IsControl(w,CurFont)) {  // print for field
       PrintControl(w,hPrtDC,hAttrDC,CurFont,BeginX,BeginY,width,height);
    }

    END:
    // restart rotation if stopped
    if (ScrFrameAngle>0 && OSCanRotate) FrameRotateDC(w,hPrtDC,CurFrame);

    return TRUE;
}

/******************************************************************************
    PrintPageBknd:
    Print the backgrond for the page
*******************************************************************************/
PrintPageBknd(PTERWND w,HDC hPrtDC,HDC hAttrDC, int PageNo)
{
   int sect,frm,x,y,HiddenX,HiddenY,width,height;
   RECT rect;
   HBRUSH hBr;
   POINT pt;

   for (frm=0;frm<TotalFrames;frm++) if (frame[frm].flags&FRAME_PAGE_BOX) break;
   if (frm<TotalFrames) {    // color the area between the page border
      // calculate beginning Y and X positions position
      sect=frame[frm].sect;

      if (GetDeviceCaps(hPrtDC,TECHNOLOGY)==DT_RASDISPLAY)  {
         if (ExtPrintPreview) {
            pt.x=TerSect1[sect].HiddenX;
            pt.y=TerSect1[sect].HiddenY;
         }
         else pt.x=pt.y=0;
      }
      else {
         Escape(hPr,GETPRINTINGOFFSET,0,NULL,&pt);        // actual offset where printing begins
         if (UseScrMetrics && True(TerOpFlags2&TOFLAG2_SCALE_PRT_DC)) DPtoLP(hPr,&pt,1); // drawing is done in hi-res
      }

      x=frame[frm].x-pt.x;
      y=frame[frm].y-pt.y;

      width=frame[frm].width;
      height=frame[frm].height;
   }
   else {   // color the whole page
      TerGetPageOrient2(hTerWnd,PageNo,&width,&height,&HiddenX,&HiddenY);  // get page width/height in twips
      width=TwipsToPrtX(width-2*HiddenX);
      height=TwipsToPrtY(height-2*HiddenY);

      if (GetDeviceCaps(hAttrDC,TECHNOLOGY)==DT_RASDISPLAY)  {
         x=TwipsToPrtX(HiddenX);   // Use printer translation because the mapping scales it down
         y=TwipsToPrtY(HiddenY);
      }
      else x=y=0;
   } 

   hBr=CreateSolidBrush(PageBkColor);
   SetRect(&rect,x,y,x+width,y+height);
   FillRect(hPrtDC,&rect,hBr);

   DeleteObject(hBr);

   return TRUE;
}  

/******************************************************************************
    PrintBkndPicture:
    Print the backgrond picture if any
*******************************************************************************/
PrintBkndPicture(PTERWND w,HDC hPrtDC,HDC hAttrDC, int PageNo)
{
   int x,y,width=TerFont[BkPictId].CharWidth[PICT_CHAR];
   int height=TerFont[BkPictId].height;
   int AreaWidth,AreaHeight,HiddenX,HiddenY;

   if (BkPictId<=0) return  TRUE;

   // get the area rectangle
   TerGetPageOrient2(hTerWnd,PageNo,&AreaWidth,&AreaHeight,&HiddenX,&HiddenY);  // get page width/height in twips
   AreaWidth=TwipsToPrtX(AreaWidth-2*HiddenX);
   AreaHeight=TwipsToPrtY(AreaHeight-2*HiddenY);

   if (GetDeviceCaps(hAttrDC,TECHNOLOGY)==DT_RASDISPLAY)  {
      x=TwipsToPrtX(HiddenX);   // Use printer translation because the mapping scales it down
      y=TwipsToPrtY(HiddenY);
   }
   else x=y=0;
    
   if (BkPictFlag==BKPICT_STRETCH) PrintPicture(w,hPrtDC,hAttrDC,BkPictId,x,y,AreaWidth,AreaHeight,1);
   else if (BkPictFlag==BKPICT_TILE) {
      int EndX=x+AreaWidth;
      int EndY=y+AreaHeight;
      int x1,y1;

      // draw horizontally
      y1=y;
      while(y1<EndY) {
         x1=x;
         while (x1<EndX) {
            PrintPicture(w,hPrtDC,hAttrDC,BkPictId,x1,y1,width,height,1);
            x1+=width;
            if (InPrintPreview && (x1+width)>EndX) break;   // the next tile does not fit compeletely
         }
         y1+=height;
      }
   }
   else PrintPicture(w,hPrtDC,hAttrDC,BkPictId,x,y,width,height,1);

   return TRUE;
} 

/******************************************************************************
    PrintControl:
    Print an embedded control.
*******************************************************************************/
BOOL PrintControl(PTERWND w, HDC hPrtDC, HDC hAttrDC, int pict, int x, int y, int width, int height)
{
    HPEN hOldPen,hPen=NULL;
    struct StrControl far *pInfo;
    HWND hCtlWnd=TerFont[pict].hWnd;
    BYTE string[255];
    int  i,len,count,x1,y1,width1,height1;
    HFONT hOldFont=NULL;
    int FontHeight=TerFont[0].height;
    struct StrForm far *pForm=NULL;
    BOOL DrawBorder=TRUE,NewPen=FALSE;

    // lock the class information
    pInfo=(LPVOID)TerFont[pict].pInfo;


    // get the form data pointer
    if (IsFormField(w,pict,FIELD_TEXTBOX)) {
       pForm=(LPVOID)TerFont[pict].pInfo;
       if (!(pForm->style&WS_BORDER)) DrawBorder=FALSE;
    }

    if (lstrcmpi(pInfo->class,"Edit")==0) hPen=GetStockObject(BLACK_PEN);
    else {      // create thicker pen
        hPen=CreatePen(PS_SOLID,TwipsToPrtX(15),0);
        NewPen=TRUE;
    }
    hOldPen=SelectObject(hPrtDC,hPen);

    if (pForm) hOldFont=SelectObject(hPrtDC,TerFont[pForm->FontId].hFont);
    else       hOldFont=SelectObject(hPrtDC,TerFont[0].hFont);

    // draw the control outline
    if (lstrcmpi(pInfo->class,"Button")==0 && (pInfo->style&0xFF)==BS_RADIOBUTTON) {
       int AdjX=(width*9)/10;
       int AdjY=(height*9)/10;
       x1=x+AdjX;
       y1=y+AdjY;
       width1=width-2*AdjX;
       height1=height-2*AdjY;
       Ellipse(hPrtDC,x1,y1,x1+width1,y1+height1);
       if (SendMessage(hCtlWnd,BM_GETCHECK,0,0L)) {
          HBRUSH hOldBrush=SelectObject(hPrtDC,GetStockObject(BLACK_BRUSH));
          Ellipse(hPrtDC,x1+width1/3,y1+height1/3,x1+2*(width1/3),y1+2*(height1/3));
          SelectObject(hPrtDC,hOldBrush);
       }
    }
    else if (lstrcmpi(pInfo->class,"Button")==0 && (pInfo->style&0xFF)==BS_PUSHBUTTON) {
       RoundRect(hPrtDC,x,y,x+width,y+height,width/4,height/4);  // draw the outline

       GetWindowText(hCtlWnd,string,sizeof(string)-1);
       PrintControlString(w,hPrtDC,hAttrDC,string,x,y,width,height,(int)ES_CENTER,TRUE);
    }
    else if (DrawBorder) Rectangle(hPrtDC,x,y,x+width,y+height);

    // draw each control
    if (lstrcmpi(pInfo->class,"Button")==0) {
       if ((pInfo->style&0xFF)!=BS_RADIOBUTTON) {   // check box
          if (SendMessage(hCtlWnd,BM_GETCHECK,0,0L)) {
             DrawShadowLine(w,hPrtDC,x,y,x+width,y+height,hPen,NULL);
             DrawShadowLine(w,hPrtDC,x,y+height,x+width,y,hPen,NULL);
          }
       }
    }
    else if (lstrcmpi(pInfo->class,"Edit")==0) {
       if (pInfo->style&ES_MULTILINE) {
          y1=y;
          count=(int)SendMessage(hCtlWnd,EM_GETLINECOUNT,0,0L);  // line count
          i=(int)SendMessage(hCtlWnd,EM_GETFIRSTVISIBLELINE,0,0L);  // first visible line
          while (i<count && (y1+FontHeight)<(y+height)) {
             string[0]=(BYTE)(sizeof(string)-1);
             len=(int)SendMessage(hCtlWnd,EM_GETLINE,i,(LPARAM)(LPSTR)string);
             string[len]=0;
             PrintControlString(w,hPrtDC,hAttrDC,string,x,y1,width,FontHeight,(int)(pInfo->style&(ES_CENTER|ES_RIGHT)),DrawBorder);
             i++;
             y1+=FontHeight;
          }
       }
       else {                // single line edit control
          GetWindowText(hCtlWnd,string,sizeof(string)-1);  // get the text
          if (pInfo->style&ES_PASSWORD) for (i=0;i<lstrlen(string);i++) string[i]='*';
          PrintControlString(w,hPrtDC,hAttrDC,string,x,y,width,height,(int)(pInfo->style&(ES_CENTER|ES_RIGHT)),DrawBorder);
       }
    }
    else if (lstrcmpi(pInfo->class,"ComboBox")==0) {
       // get the text
       int sel=(int)SendMessage(hCtlWnd,CB_GETCURSEL,0,0L);
       SendMessage(hCtlWnd,CB_GETLBTEXT,(WPARAM)sel,(LPARAM)(LPSTR)string);

       PrintControlString(w,hPrtDC,hAttrDC,string,x,y,width,height,0,TRUE);
    }
    else if (lstrcmpi(pInfo->class,"ListBox")==0) {
       int ItemHeight;
       RECT rect;
       y1=y;
       count=(int)SendMessage(hCtlWnd,LB_GETCOUNT,0,0L);  // item count
       i=(int)SendMessage(hCtlWnd,LB_GETTOPINDEX,0,0L);      // first item
       SendMessage(hCtlWnd,LB_GETITEMRECT,0,(LPARAM)(LPRECT)&rect);
       ItemHeight=ScrToPrtY(rect.bottom-rect.top);
       while (i<count && (y1+ItemHeight)<(y+height)) {
          if (LB_ERR==(len=(int)SendMessage(hCtlWnd,LB_GETTEXT,i,(LPARAM)(LPSTR)string))) break;
          string[len]=0;
          PrintControlString(w,hPrtDC,hAttrDC,string,x,y1,width,ItemHeight,0,TRUE);
          i++;
          y1+=ItemHeight;
       }
    }



    SelectObject(hPrtDC,hOldPen);
    SelectObject(hPrtDC,hOldFont);

    if (NewPen) DeleteObject(hPen);


    return TRUE;
}

/******************************************************************************
    PrintControlString:
    Print a string inside a control space. HorzAlign is 0, ES_CENTER, ES_RIGHT
*******************************************************************************/
BOOL PrintControlString(PTERWND w,HDC hPrtDC, HDC hAttrDC,LPBYTE string,int x, int y, int width, int height,int HorzAlign, BOOL margin)
{
    int x1,y1,AvailWidth,PrevExtra;
    SIZE size;
    RECT rect;
    BOOL ExtraSet=FALSE;


    GetTextExtentPoint(hPrtDC,string,lstrlen(string),&size);
    if      (HorzAlign==ES_CENTER) x1=x+(width-size.cx)/2;  // centered
    else if (HorzAlign==ES_RIGHT)  x1=x+(width-size.cx)-DblCharWidth(w,0,TRUE,' ',0);  // right justify
    else                           x1=x+(margin?DblCharWidth(w,0,TRUE,' ',0):0);  // create a left offset
    y1=y+(height-size.cy)/2;  // center vertically

    AvailWidth=width-2*(x1-x);  // width available for the text
    if (size.cx>AvailWidth && lstrlen(string)>0) {   // need to shrink the text
       int adj=(size.cx-AvailWidth)/lstrlen(string) + 1;
       PrevExtra=SetTextCharacterExtra(hPrtDC,-adj);
       ExtraSet=TRUE;
    } 


    SetRect(&rect,x,y,x+width,y+height);
    //TextOut(hPrtDC,x1,y1,string,lstrlen(string));
    ExtTextOut(hPrtDC,x1,y1,ETO_CLIPPED,&rect,string,lstrlen(string),NULL);

    if (ExtraSet) SetTextCharacterExtra(hPrtDC,PrevExtra);

    return TRUE;
}

/******************************************************************************
    PrtEnumMetafile:
    Enumeration of metafile records while printing to filter out SetWindowOrg
    call.
*******************************************************************************/
BOOL CALLBACK _export PrtEnumMetafile(HDC hDC, HANDLETABLE FAR *pTable, METARECORD FAR *pMetaRec, int ObjCount, LPARAM lParam)
{
    //DispMetaRecName(pMetaRec->rdFunction);

    if (pMetaRec->rdFunction==0x41) return FALSE;  // unsupported mac metafile command, stop playing

    //if (pMetaRec->rdFunction!=0x20b)    // filter out SetWindowOrg function
       PlayMetaFileRecord(hDC,pTable,pMetaRec,ObjCount);

    return TRUE;
}

/******************************************************************************
    PrtEnumMetafile2:
    Enumeration of metafile records while printing to stop at unsupported
    metafile commands.
*******************************************************************************/
BOOL CALLBACK _export PrtEnumMetafile2(HDC hDC, HANDLETABLE FAR *pTable, METARECORD FAR *pMetaRec, int ObjCount, LPARAM lParam)
{
    //DispMetaRecName(pMetaRec->rdFunction);
    
    if (pMetaRec->rdFunction==0x41) return FALSE;  // unsupported mac metafile command, stop playing

    PlayMetaFileRecord(hDC,pTable,pMetaRec,ObjCount);

    return TRUE;
}

/******************************************************************************
    GetPrtTabWidth:
    Get the tab width for a tab while printing
*******************************************************************************/
GetPrtTabWidth(PTERWND w,int TextPos, int TabNo, struct StrTabw far * CurTabw)
{
    int width;

    if (TerArg.WordWrap && CurTabw && TabNo<CurTabw->count) width=CurTabw->width[TabNo];
    else {                     // use default tab positions
       TextPos-=PageX;         // text position relative to the left margin
       width=((TextPos/TwipsToPrtX(DefTabWidth)+1)*TwipsToPrtX(DefTabWidth))-TextPos;
    }

    if (width<0) width=0;      // tab width can not be negative


    return width;
}

/******************************************************************************
    PrintViewMode:
    Print in PrintView mode.  If the FromLine is less than 0 than the routine
    prints the page number contained in the ToLine variable.
*******************************************************************************/
PrintViewMode(PTERWND w,HDC hPrtDC,long FromLine,long ToLine, int copies, int CopyNo)
{
    int  FromPage,ToPage,page,pass,copy;
    int  i,BeginX,sect,FrameNo,NextY,PageNextY,TextLen,AdjX,FrameY,
         LeftMargin,TopMargin,LinePixLen,ColWidth,
         LineHeight,BaseHeight,ExtLead,DescentHeight,CurDescentHeight,
         LeftIndent,RightIndent,CurParaId,len,BorderMargin;
    BOOL FirstFrame,reset;
    BYTE LineTxt[MAX_WIDTH];
    WORD LineFmt[MAX_WIDTH];
    UINT flags;
    LPBYTE ptr,lead;
    LPWORD fmt;
    struct StrTabw CurTabw;
    POINT pt;
    long line;
    WORD CurFmt;
    BOOL HasBullet,OnePage=FALSE;
    int  ParaBeginX,ParaFID,frm,PassCount;
    int  SpcBef,SpcAft,TabNo,LenForLineHt;
    int  CurBin,LineFont,BulletWidth;
    BOOL TextFound,SpaceFound,BreakIgnored;
    int  CurCopy,BigVal=9999,PosOffsetY=BigVal,NegOffsetY=BigVal,CurOffset,RowBeginX;
    LPWORD pWidth;
     
    // calculate page range
    if (FromLine<0) {
       FromPage=ToPage=(int)ToLine;   // page number contained in this variable
       if (FromPage>=TotalPages) return FALSE;   // page does not exist
       OnePage=TRUE;
    }
    else {
       for (FromPage=0;FromPage<TotalPages;FromPage++) if (PageInfo[FromPage].FirstLine>FromLine) break;
       if (FromPage>0) FromPage--;
       for (ToPage=0;ToPage<TotalPages;ToPage++) if (PageInfo[ToPage].FirstLine>ToLine) break;
       if (ToPage>0) ToPage--;
       if (ToPage<FromPage) ToPage=FromPage;
    }

    PassCount=FramesSorted?1:2;

    if (copies<1) copies=1;


    // print each page
    for (page=FromPage;page<=ToPage;page++) {
      for (copy=0;copy<copies;copy++) {
        // Create page frames
        CurPage=page;
        CreateFrames(w,TRUE,CurPage,CurPage);
        FirstFrame=TRUE;
        PassCount=FramesSorted?1:2;
        PageNextY=0;
        reset=FALSE;

        // Start the printer page
        if (GetDeviceCaps(hPrtDC,TECHNOLOGY)!=DT_RASDISPLAY && !(TerOpFlags&TOFLAG_EXTERNAL_DC)) {
           if (TerArg.WordWrap) {   // set the printer orientation and bin
              int width,height;

              sect=TerGetPageSect(hTerWnd,page);
              if (pDeviceMode->dmOrientation!=TerSect[sect].orient) {
                  PaperOrient=pDeviceMode->dmOrientation=(short)TerSect[sect].orient;
                  reset=TRUE;
              }
              if (page==0 || PageInfo[page-1].TopSect!=PageInfo[page].TopSect)
                   CurBin=TerSect[sect].FirstPageBin;
              else CurBin=TerSect[sect].bin;
              
              // bin override while print collated copies
              if (copies>1) CurCopy=copy;   // printing uncollated copies
              else          CurCopy=CopyNo; // print a collated copy
              if (OverrideBinCopy>0 && (CurCopy+1)>=OverrideBinCopy) CurBin=OverrideBin; 

              if (CurBin==0) CurBin=DMBIN_AUTO;

              if (pDeviceMode->dmDefaultSource!=CurBin && (CurBin!=DMBIN_AUTO || OverrideBinCopy>0)) {
                  pDeviceMode->dmDefaultSource=(short)CurBin;
                  reset=TRUE;
              }
              
              // check any change in the paper size
              width=(int)(TerSect[sect].PprWidth*254+(float).5);  // in tenth of mm
              height=(int)(TerSect[sect].PprHeight*254+(float).5);  // in tenth of mm

              if (TerSect[sect].PprSize!=pDeviceMode->dmPaperSize ||
                  width!=pDeviceMode->dmPaperWidth || height!=pDeviceMode->dmPaperLength) {
                  
                  pDeviceMode->dmPaperSize=TerSect[sect].PprSize;
                  if (pDeviceMode->dmPaperSize==0) pDeviceMode->dmPaperSize=FindPaperSize(w,width,height);

                  pDeviceMode->dmPaperWidth=width;
                  pDeviceMode->dmPaperLength=height;
                  reset=TRUE;
              }
              
              if (reset && !(TerFlags4&TFLAG4_NO_RESET_DC)) ResetDC(hPrtDC,pDeviceMode);   // some drivers don't like it before StartPage
           }
        }
        if (GetDeviceCaps(hPrtDC,TECHNOLOGY)!=DT_RASDISPLAY) {
            BOOL CallStartPage=TRUE;
            if (TerOpFlags&TOFLAG_EXTERNAL_DC && copy==0 && page==FromPage) CallStartPage=FALSE;  // calling application starts the first page

            if (CallStartPage) StartPage(hPrtDC);
        }
        
        // scale from hi-res to printer resolution
        if (UseScrMetrics && True(TerOpFlags2&TOFLAG2_SCALE_PRT_DC)) {
            int ResX=GetDeviceCaps(hPrtDC,LOGPIXELSX);
            int ResY=GetDeviceCaps(hPrtDC,LOGPIXELSY);
            SetMapMode(hPrtDC,MM_ANISOTROPIC);
            SetWindowExtEx(hPrtDC,UNITS_PER_INCH,UNITS_PER_INCH,NULL);
            SetViewportExtEx(hPrtDC,ResX,ResY,NULL);
        } 

        // print the page background color
        if (PageBkColor!=CLR_WHITE) PrintPageBknd(w,hPrtDataDC,hPrtDC,page);

        // print the background image
        if (TerFlags3&TFLAG3_PRINT_BKND_PICT) PrintBkndPicture(w,hPrtDataDC,hPrtDC,page);
        
        // print each frame
        for (pass=0;pass<PassCount;pass++) {   // print in passes
           for (frm=0;frm<TotalFrames;frm++) {
              if (True(frame[frm].flags&FRAME_WATERMARK) && !FirstFrame) continue;  // skip the watermark frame

              // initialize color variables
              PrtTextBkColor=PrtParaBkColor=PrtFrameBkColor=CLR_WHITE;

              if (FramesSorted) FrameNo=frame[frm].DispFrame;
              else              FrameNo=frm;
              ParaFID=frame[FrameNo].ParaFrameId;


              // first print the regular text and non-line/non-rect drawing objects
              if (!FramesSorted) {
                 if (pass==0 && ParaFID>0) continue;
                 if (pass==1 && ParaFID==0) continue;
              }

              if (frame[FrameNo].CellId>0 && cell[frame[FrameNo].CellId].flags&CFLAG_ROW_SPANNED && !(frame[FrameNo].flags&FRAME_TOP_SPAN)) continue;

              // calculate beginning Y and X positions position
              sect=frame[FrameNo].sect;

              if (GetDeviceCaps(hPrtDC,TECHNOLOGY)==DT_RASDISPLAY)  {
                 if (ExtPrintPreview) {
                    pt.x=TerSect1[sect].HiddenX;
                    pt.y=TerSect1[sect].HiddenY;
                 }
                 else pt.x=pt.y=0;
              }
              else {
                 Escape(hPr,GETPRINTINGOFFSET,0,NULL,&pt);        // actual offset where printing begins
                 if (UseScrMetrics && True(TerOpFlags2&TOFLAG2_SCALE_PRT_DC)) DPtoLP(hPr,&pt,1); // drawing is done in hi-res
              }

              LeftMargin=(int)(TerSect[sect].LeftMargin*PrtResX)-pt.x;// beginning left position
              if (FirstFrame) {
                 if (ViewPageHdrFtr) TopMargin=-pt.y;
                 else                TopMargin=(int)(TerSect[sect].TopMargin*PrtResY)-pt.y; // beginning top position
              
                 // print watermark if any
                 if (WmParaFID>0) {
                   for (i=0;i<TotalFrames;i++) {
                      int pict,pfid;
                      if (False(frame[i].flags&FRAME_WATERMARK)) continue;
                      pfid=frame[i].ParaFrameId;
                      pict=ParaFrame[pfid].pict;
                      if (pict>0) {
                         int width=TerFont[pict].CharWidth[PICT_CHAR];     // width and height of the CurFonture
                         int height=TerFont[pict].height;
                         PrintPicture(w,hPrtDataDC,hPrtDC,pict,frame[i].x+LeftMargin,frame[i].y+TopMargin,width,height,1);
                         break;
                      }
                   }
                 }
              }
              FirstFrame=FALSE;
              

              // draw the non-text drawing objects and picture frames
              if (!(frame[FrameNo].empty) && frame[FrameNo].flags&(FRAME_NON_TEXT_DO|FRAME_PICT)) {
                 long line=frame[FrameNo].PageFirstLine;
                 BOOL draw;
                 draw=( (line>=FromLine && line<=ToLine)
                        || FromLine < 0
                        || PfmtId[pfmt(line)].flags&PAGE_HDR_FTR
                        || (cid(line) && (TableRow[cell[cid(line)].row].flags&ROWFLAG_HDR))
                        || OnePage);
                  

                  if (!draw) continue;

                  frame[FrameNo].x+=LeftMargin;    // relative to printing begin x
                  frame[FrameNo].y+=TopMargin;     // relative to printing begin y

                  if (True(ParaFrame[ParaFID].flags&PARA_FRAME_FILL_PICT) && ParaFrame[ParaFID].FillPict>0) {
                      PrintPicture(w,hPrtDataDC,hPrtDC,ParaFrame[ParaFID].FillPict,frame[FrameNo].x,frame[FrameNo].y,frame[FrameNo].width,frame[FrameNo].height,1);
                  }
                  if      (ParaFrame[ParaFID].flags&PARA_FRAME_LINE) DrawLineObject(w,hPrtDataDC,FrameNo);
                  else if (ParaFrame[ParaFID].flags&PARA_FRAME_RECT) DrawRectObject(w,hPrtDataDC,FrameNo);
                  else if (ParaFrame[ParaFID].pict>0) PrintPicture(w,hPrtDataDC,hPrtDC,ParaFrame[ParaFID].pict,frame[FrameNo].x,frame[FrameNo].y,frame[FrameNo].width,frame[FrameNo].height,1);
                  continue;
              }

              // save the margin information
              PrtLeftMarg=LeftMargin;    // this global information needed for frame rotation
              PrtTopMarg=TopMargin;

              // draw footnote frames  
              if (frame[FrameNo].flags&FRAME_FNOTE) {
                  CurFrame=FrameNo;
                  //frame[FrameNo].x+=LeftMargin;
                  frame[FrameNo].y+=TopMargin;
                  DrawFootnote(w,hPrtDC,FrameNo,FALSE);
                  continue;
              }

              // draw the page border box
              if (frame[FrameNo].flags&FRAME_PAGE_BOX) {
                  frame[FrameNo].x-=pt.x;
                  frame[FrameNo].y-=pt.y;
                  DrawPageBorderBox(w,hPrtDC,FrameNo);
                  continue;
              } 

              BeginX=frame[FrameNo].x+LeftMargin;
              NextY=FrameY=frame[FrameNo].y+TopMargin;

              // draw frame shading and border
              PrintFrameBorderShading(w,hPrtDataDC,FrameNo,BeginX,NextY);

              // now skip empty frames
              if (frame[FrameNo].empty) continue;

              // adjust the starting Y position
              NextY=NextY+frame[FrameNo].SpaceTop;  // add additional space at the top

              // get the column width
              ColWidth=frame[FrameNo].width;
              if (TerSect[sect].columns>1 && !(frame[FrameNo].flags&FRAME_LAST_COL)
                 && frame[FrameNo].CellId==0 && frame[FrameNo].ParaFrameId==0) {
                 int ParaId=pfmt(frame[FrameNo].PageFirstLine);
                 if (!(PfmtId[ParaId].flags&PAGE_HDR_FTR)) ColWidth-=(int)(TerSect[sect].ColumnSpace*PrtResX);
              }

              // set text rotation variables
              CurFrame=FrameNo;  // CurFrame global variable used by OurXXX drawing functions
              ScrFrameAngle=LineTextAngle(w,-FrameNo);
              if (ScrFrameAngle>0 && OSCanRotate) FrameRotateDC(w,hPrtDataDC,CurFrame);
              
              // set section line number
              SectLine=GetSectDisplayLine(w,FrameNo);

              // print each LineText in the frame
              for (line=frame[FrameNo].PageFirstLine;line<=frame[FrameNo].PageLastLine;line++) {
                 if (TableLevel(w,line)!=frame[FrameNo].level) continue;   // line belongs to a subtable
                 if (TerFlags3&TFLAG3_CLIP_CELL_OVERFLOW && frame[FrameNo].CellId>0 && (NextY+LineHt(line))>(FrameY+frame[FrameNo].height)) break;
                 
                 if (frame[FrameNo].LimitY!=-1 && (NextY+LineHt(line))>(frame[FrameNo].LimitY+TopMargin) 
                      && line>frame[FrameNo].PageFirstLine) {     // check if LineHt-SpaceAft exceeds LimitY
                     GetLineSpacing(w,line,LineHt(line),&SpcBef,&SpcAft,FALSE);  // calculate the paragraph spacing
                     if ((NextY+LineHt(line)-SpcAft)>(frame[FrameNo].LimitY+TopMargin)) break;
                 }

                 NextY+=GetObjSpcBef(w,line,FALSE);  // add any frame space before the line


                 // skip over the section break line
                 if (tabw(line) && tabw(line)->type&(INFO_PAGE|INFO_COL)) continue;

                 if (LineLen(line)==0) continue;  // skip zero length lines
                 if (LineHt(line)==0) continue;   // skip break lines


                 // get para properties
                 HasBullet=FALSE;
                 CurParaId=pfmt(line);
                 flags=PfmtId[CurParaId].flags;
                 RightIndent=TwipsToPrtX(PfmtId[CurParaId].RightIndentTwips);
                 LeftIndent=ParaBeginX=frame[FrameNo].SpaceLeft+TwipsToPrtX(PfmtId[CurParaId].LeftIndentTwips);
                 if (line==0 || LineFlags(line)&LFLAG_PARA_FIRST) 
                    LeftIndent+=GetFirstIndent(w,line,&BulletWidth,&HasBullet,FALSE);
                 

                 // variables for para shading and background color
                 if (PfmtId[CurParaId].FirstIndentTwips<0) ParaBeginX+=TwipsToPrtX(PfmtId[CurParaId].FirstIndentTwips);
                 ParaBeginX+=BeginX;
                 ParaShading=PfmtId[CurParaId].shading;
                 ParaBackColor=PfmtId[CurParaId].BkColor;
                 ParaBoxSpace=PfmtId[CurParaId].BorderSpace;

                 // specify the para borders
                 TextBorder=BoxLeft=BoxRight=0;
                 if (flags&PARA_BOX && fid(line)==0) {
                    if (flags&PARA_BOX_TOP && (line==frame[FrameNo].PageFirstLine || !HasSameParaBorder(w,line-1,line))) TextBorder|=BOX_TOP;
                    if (flags&PARA_BOX_BOT && (line>=frame[FrameNo].PageLastLine || !HasSameParaBorder(w,line+1,line))) TextBorder|=BOX_BOT;
                    if (flags&PARA_BOX_LEFT)   TextBorder|=BOX_LEFT;
                    if (flags&PARA_BOX_RIGHT)  TextBorder|=BOX_RIGHT;
                    if (flags&PARA_BOX_BETWEEN) TextBorder|=BOX_BETWEEN;  // 09142005
                    if (flags&PARA_BOX_DOUBLE) TextBorder|=BOX_DOUBLE;
                    if (flags&PARA_BOX_THICK)  TextBorder|=BOX_THICK;
                 }

                 BorderMargin=PARA_BORDER_MARGIN;
                 if (LineFlags(line)&LFLAG_HTML_RULE) BorderMargin=-PARA_BORDER_MARGIN/2; // should remain within margins
                 if (ParaFID && !(ParaFrame[ParaFID].flags&PARA_FRAME_OBJECT)) BorderMargin=0;
                 if (cid(line)) BorderMargin=0; //BorderMargin/2;
                 BoxLeft=BeginX+TwipsToPrtX(PfmtId[CurParaId].LeftIndentTwips-BorderMargin);
                 if (PfmtId[CurParaId].FirstIndentTwips<0) BoxLeft+=TwipsToPrtX(PfmtId[CurParaId].FirstIndentTwips);
                 
                 BoxRight=BeginX+ColWidth/*-frame[FrameNo].SpaceLeft-frame[FrameNo].SpaceRight*/
                          -TwipsToPrtX(PfmtId[CurParaId].RightIndentTwips-BorderMargin);
                 if (TextBorder && tabw(line) && tabw(line)->FrameX>0) {
                    if (abs((tabw(line)->FrameX+tabw(line)->FrameWidth)-BoxRight)<PrtResX) 
                       BoxRight=tabw(line)->FrameX;
                 } 

                 // lock the current line
                 len=LineLen(line);
                 ptr=pText(line);
                 lead=pLead(line);
                 fmt=OpenCfmt(w,line);

                 // copy tabw
                 if (tabw(line)) FarMove(tabw(line),&CurTabw,sizeof(CurTabw));
                 else            FarMemSet(&CurTabw,0,sizeof(CurTabw));


                 // Calculate the tab widths for the tabs in the current line
                 BreakIgnored=false;
                 TabNo=0;
                 LinePixLen=LeftIndent;
                 LineHeight=DescentHeight=BaseHeight=ExtLead=0;

                 if (len>0 && LineFlags(line)&LFLAG_SECT) len--;
                 TextLen=len;
                 if ( len>0 && (ptr[len-1]==ParaChar || ptr[len-1]==CellChar
                   || ptr[len-1]==ROW_CHAR || ptr[len-1]==COL_CHAR
                   || ptr[len-1]==SECT_CHAR || ptr[len-1]==LINE_CHAR) ) {
                      TextLen--;
                      BreakIgnored=true;
                 }

                 while (TextLen>0) {    // exclude ending spaces/tabs
                    if (ptr[TextLen-1]!=' ' /*&& ptr[TextLen-1]!=TAB*/) break;     // can't skip tab because they might be printing leaders
                    TextLen--;
                 }
                 if (TextLen>0) len=TextLen;

                 // calcualte the length of line height consideration
                 TextFound=SpaceFound=FALSE;
                 LineFont=0;
                 LenForLineHt=len;
                 if (false && len>1 && LineFlags(line)&(LFLAG_PARA|LFLAG_LINE)) {
                    LineFont=fmt[len-1];
                    BreakIgnored=TRUE;
                    LenForLineHt--;
                 }

                 NegOffsetY=PosOffsetY=BigVal;  // initialize the font offset values
                  
                 pWidth=GetLineCharWidth(w,line);

                 for (i=0;i<=len;i++) {
                    if (i<len) CurFmt=fmt[i];

                    // update the line height
                    if (i<LenForLineHt) {
                       if (!SpaceFound && (ptr[i]==' ' || ptr[i]==TAB) && TerFont[CurFmt].height>0) SpaceFound=TRUE;
                       if ((ptr[i]!=' ' /*&& ptr[i]!=TAB*/) || (i==(LenForLineHt-1) && !TextFound)) {
                          CurDescentHeight=TerFont[CurFmt].height-TerFont[CurFmt].BaseHeight-TerFont[CurFmt].ExtLead;
                          if (TerFont[CurFmt].OffsetVal>0) CurDescentHeight-=TerFont[CurFmt].OffsetVal;
                          if (CurDescentHeight>DescentHeight) DescentHeight=CurDescentHeight;
                          if (TerFont[CurFmt].BaseHeight>BaseHeight) BaseHeight=TerFont[CurFmt].BaseHeight;
                          if (TerFont[CurFmt].ExtLead>ExtLead) ExtLead=TerFont[CurFmt].ExtLead;

                          CurOffset=TerFont[CurFmt].OffsetVal;
                          if (CurOffset==0) NegOffsetY=PosOffsetY=0;
                          else if (CurOffset>0 && CurOffset<PosOffsetY) PosOffsetY=CurOffset;  // minimum positive offset 
                          else if (CurOffset<0 && (-CurOffset)<NegOffsetY) NegOffsetY=-CurOffset;  // minimum negative offset 

                          TextFound=TRUE;
                       }
                    }

                    if (i<TextLen) LinePixLen+=pWidth[i];
                 }

                 if (PosOffsetY==BigVal) PosOffsetY=0;  // not used
                 if (NegOffsetY==BigVal) NegOffsetY=0;  // not used
                 if (PosOffsetY && NegOffsetY) PosOffsetY=NegOffsetY=0;  // height adjustment not needed

                 LineHeight=BaseHeight+DescentHeight+ExtLead-PosOffsetY-NegOffsetY;

                 if (NegOffsetY) BaseHeight-=NegOffsetY;
                 
                 if (LineHeight==0 && (SpaceFound || BreakIgnored)) {
                    //if (SpaceFound) LineFont=0;
                    LineHeight=TerFont[LineFont].height;
                    BaseHeight=TerFont[LineFont].BaseHeight;
                    ExtLead=TerFont[LineFont].ExtLead;
                 }

                 // Copy the line to a temporary area
                 for (i=0;i<len;i++) {   // copy the LineText
                    LineTxt[i]=ptr[i];
                    LineFmt[i]=fmt[i];
                    LineLead[i]=LeadByte(lead,i);
                 }
                 LineTxt[i]=0;              // terminate the LineText

                 // Adjust the line
                 if (len>0 && (LineTxt[len-1]==ParaChar
                           || LineTxt[len-1]==CellChar
                           || LineTxt[len-1]==LINE_CHAR
                           || LineTxt[len-1]==COL_CHAR
                           || LineTxt[len-1]==ROW_CHAR) ) {
                    len--;
                    LineTxt[len]=0;
                 }

                 // adjustment for centering and justifiction and frames
                 AdjX=0;
                 if (len>0 || LinePixLen>0) {
                    int JustCX=0,AftFrameCX=0;
                    int WrapWidth=ColWidth;
                    if (frame[FrameNo].CellId>0 && cell[frame[FrameNo].CellId].TextAngle!=0) 
                        WrapWidth=frame[FrameNo].height;

                    if (LineFlags(line)&LFLAG_ASSUMED_TAB && tabw(line) && tabw(line)->count>0) JustCX=tabw(line)->width[0];
                    else if (flags&(CENTER)) JustCX=(WrapWidth-RightIndent-frame[FrameNo].SpaceRight-LinePixLen)/2;
                    else if (flags&(RIGHT_JUSTIFY)) JustCX=WrapWidth-RightIndent-frame[FrameNo].SpaceRight-LinePixLen;
                    //JustCX=JustAdjX(line);

                    if (CurTabw.type&INFO_FRAME && CurTabw.FrameCharPos==0) AftFrameCX=ScrToPrtX(CurTabw.FrameScrWidth);

                    AdjX=(JustCX>AftFrameCX)?JustCX:AftFrameCX;
                 }

                 // print the LineTxt
                 GetLineSpacing2(w,line,LineHeight,&SpcBef,&SpcAft,&ParaSpcBef,&ParaSpcAft,FALSE);  // calculate the paragraph spacing
                 SpaceBef=SpcBef;
                 SpaceAft=SpcAft;


                 if (  (line>=FromLine && line<=ToLine)
                    || FromLine < 0
                    || PfmtId[pfmt(line)].flags&PAGE_HDR_FTR
                    || (cid(line) && (TableRow[cell[cid(line)].row].flags&ROWFLAG_HDR)) ){
                     
                     int BulletX,SectLineX;


                     if (ParaShading || ParaBackColor!=CLR_WHITE) PrintParaShading(w,hPrtDataDC,line,FrameNo,BoxLeft,BoxRight,NextY+ExtLead,LineHeight+/*ExtLead+*/SpcBef+SpcAft-(TextBorder&BOX_BOT?PointsToPrtY(1):0));  // 1 point reserved for border
                     else                                         PrtParaBkColor=CLR_WHITE;

                     RowBeginX=LeftIndent+AdjX;
                     BulletX=BeginX+LeftIndent+AdjX-BulletWidth;
                     SectLineX=BeginX+frame[FrameNo].SpaceLeft;
                     
                     PrintOneLine(w,line,hPrtDataDC,hPrtDC,BeginX+RowBeginX,NextY,LineTxt,LineFmt,LineLead,lstrlen(LineTxt),LineHeight,BaseHeight,&CurTabw,CurParaId,SpcBef,SpcAft,ExtLead,FrameNo,LeftMargin,RowBeginX,pWidth,
                                  HasBullet,BulletX,SectLineX);

                     // pass any heading info to pdf
                     SetPdfBkm(w,hPrtDataDC,line,page,NextY+pt.y);    // add hidden y because our pdf converter assumes full page length
                 }


                 // calculate the next Y position
                 NextY+=LineHt(line); //(LineHeight+SpcBef+SpcAft);
                 if (NextY>PageNextY) PageNextY=NextY;

                 // unlock the current line
                 CloseCfmt(w,line);
                 
                 MemFree(pWidth);
              }
              
              // reset text rotation
              if (ScrFrameAngle>0 && OSCanRotate) FrameNoRotateDC(w,hPrtDataDC);  // reset DC rotation
              ScrFrameAngle=0;                  // reset text rotation angle

              SectLine=-1;                      // reset section line numbering
           }
        }
        
        // spool this copy of the page
        if ((copy+1)<copies || page<ToPage) {
          SpoolIt(w,hPrtDC);    // spool this page
          if (AbortPrint) goto END;
        }
      }
    }

    END:

    // reset text rotation
    if (ScrFrameAngle>0 && OSCanRotate) FrameNoRotateDC(w,hPrtDataDC);  // reset DC rotation
    ScrFrameAngle=0;                  // reset text rotation angle

    return PageNextY;                     // next Y position on the paper
}


/******************************************************************************
    FindPaperSize:
    pass bookmark data to pdf generator
*******************************************************************************/
FindPaperSize(PTERWND w,int width, int height)
{
    LPWORD pPaper=NULL;
    LPPOINT pPaperSize=NULL;
    int PaperCount,i,size=0;;

    if (0==(PaperCount=DeviceCapabilities(PrinterName,PrinterPort,DC_PAPERS,NULL,pDeviceMode))) goto PAPER_DONE;
    pPaper=OurAlloc((PaperCount+2)*sizeof(WORD));       // one extra for DMPAPER_USER
    if (0==(PaperCount=DeviceCapabilities(PrinterName,PrinterPort,DC_PAPERS,(LPSTR)pPaper,pDeviceMode))) goto PAPER_DONE;

    pPaperSize=OurAlloc((PaperCount+2)*sizeof(POINT));
    if (0==(PaperCount=DeviceCapabilities(PrinterName,PrinterPort,DC_PAPERSIZE,(LPSTR)pPaperSize,pDeviceMode))) goto PAPER_DONE;
                   
    PAPER_DONE:

    for (i=0;i<PaperCount;i++) {
       if (pPaperSize[i].x==width && pPaperSize[i].y==height) {
          size=pPaper[i];
          break;
       } 
    } 

    if (pPaper) OurFree(pPaper);
    if (pPaperSize) OurFree(pPaperSize);
 
    return size;
}
 
/******************************************************************************
    SetPdfBkm:
    pass bookmark data to pdf generator
*******************************************************************************/
SetPdfBkm(PTERWND w,HDC hPrtDC,long line, int page, int y)
{
    int i,j,len,CurPara,CurStyle;
    BOOL IsHeading;
    BYTE str[MAX_WIDTH];
    int HeadingLevel,ObjType;
    struct StrPdfBkm bkm;
    LPBYTE ptr;
    LPWORD fmt;
    
    if (False(TerOpFlags2&TOFLAG2_PDF))  return true;
      
    ObjType=GetObjectType(hPrtDC);
    if (ObjType!=OBJ_ENHMETADC) return true;
     
    CurPara=pfmt(line);
    CurStyle=PfmtId[CurPara].StyId;

    if (CurStyle==0) return true;
    
    IsHeading=TRUE;
          
    len=lstrlen("heading ");
    lstrcpy(str,StyleId[CurStyle].name);
    str[len]=0;
    if (lstrcmpi(str,"heading ")!=0) IsHeading=FALSE;
          
    if (IsHeading) {
       len=lstrlen("heading ");
       if (lstrlen(StyleId[CurStyle].name)>(len+1)) {
          BYTE LastChar=StyleId[CurStyle].name[len+1];
          if (LastChar!=' ' && LastChar!=',') IsHeading=FALSE;  // only 1 to 9 supported
       }
       if (IsHeading) HeadingLevel=StyleId[CurStyle].name[len]-'1';  // 0 based
    }
    if (!IsHeading) {                // check the outilne level
       if (StyleId[CurStyle].OutlineLevel>=0) {
          IsHeading=true;
          HeadingLevel=StyleId[CurStyle].OutlineLevel;   // 1 based
       } 
    } 

    if (!IsHeading) return true;

    // fill the bookmark structure
    FarMemSet(&bkm,0,sizeof(bkm));

    bkm.sign[0]='T';
    bkm.sign[1]='E';       // signature
    bkm.level=HeadingLevel;
    bkm.page=page;
    bkm.y=PrtToTwipsY(y)/20;   // pass in point units

    ptr=pText(line);
    fmt=OpenCfmt(w,line);

    len=LineLen(line);
    if (len>0 && ptr[len]<32) len--;   // discard control characters

    if (len>99) len=99;
    for (i=len-1;i>=0;i--) if (ptr[i]>=32) break;  // remove trailing spaces, tabs, control chars
    
    if (len>99) len=99;

    // copy from ptr to name string
    j=0;
    for (i=0;i<len;i++) {
      if (j==0 && (ptr[i]<32 || ptr[i]==' ')) continue;  // remove and spaces and control characters from the beginning
      if (HiddenText(w,fmt[i])) continue;
      
      bkm.name[j]=ptr[i];
      j++;
    }
     
    bkm.name[j]=0;

    CloseCfmt(w,line);

    if (j>0) GdiComment(hPrtDC,sizeof(bkm),(LPBYTE)&bkm);

    return true;
}
 
/******************************************************************************
    PrintParaShading:
    Print the shading for the paragraph.
*******************************************************************************/
PrintParaShading(PTERWND w,HDC hPrtDC,long line, int FrameNo,int ParaBeginX, int ParaLastX, int BeginY, int height)
{
    RECT rect;
    HBRUSH hBr=NULL;
    //int BoxSpace;


    // build the frame rectangle
    rect.left=ParaBeginX;
    rect.right=ParaLastX;
    rect.top=BeginY;
    rect.bottom=rect.top+height;
    
    //BoxSpace=TwipsToPrtY(ParaBoxSpace);
    //if (LineFlags(line)&LFLAG_BOX_TOP) rect.top-=BoxSpace;
    //if (LineFlags(line)&LFLAG_BOX_BOT) rect.bottom+=BoxSpace;

    rect.top+=ParaSpcBef;  // 20050919 - exclude the paragraph before/after space from shading
    rect.bottom-=ParaSpcAft;
    
    //if (!(TerFlags3&TFLAG3_LARGE_PARA_BORDER)) {
       //if (LineFlags(line)&LFLAG_SHADE_BEGIN) rect.top+=SpaceBef;       /09142005
       //if (LineFlags(line)&LFLAG_SHADE_END) rect.bottom-=SpaceAft;
    //}

    // print frame background color or shading
    PrtParaBkColor=CLR_WHITE;  // default color
    if (ParaBackColor!=CLR_WHITE) PrtParaBkColor=ParaBackColor;

    // apply the paragraph shading
    if (ParaShading && ParaBackColor==CLR_WHITE) {
       DWORD shade=((DWORD)ParaShading*0xFF)/10000L;
       if (shade>0xFF) shade=0xFF;
       shade=(shade<<16)|(shade<<8)|shade;    // all comonents the same
       PrtParaBkColor=PrtParaBkColor^shade;
    }
    else if (ParaShading!=10000 && ParaShading!=0 /* && ParaBackColor==0*/) {     // reduce the background color
       BYTE shade=(BYTE) (255*(long)ParaShading/10000);

       int red=GetRValue(PrtParaBkColor);
       int green=GetGValue(PrtParaBkColor);
       int blue=GetBValue(PrtParaBkColor);
       //red=MulDiv(ParaShading,255-red,10000);
       //green=MulDiv(ParaShading,255-green,10000);
       //blue=MulDiv(ParaShading,255-blue,10000);
       //PrtParaBkColor=RGB(255-red,255-green,255-blue);

       red=(red<shade)?0:red-shade;
       green=(green<shade)?0:green-shade;
       blue=(blue<shade)?0:blue-shade;
       PrtParaBkColor=RGB(red,green,blue);
    }

    if (PrtParaBkColor!=CLR_WHITE) hBr=CreateSolidBrush(PrtParaBkColor);

    if (hBr) {
       FillRect(hPrtDC,&rect,hBr);
       DeleteObject(hBr);
    }

    return TRUE;
}

/******************************************************************************
    PrintFrameBorderShading:
    Print the frame border and shading
*******************************************************************************/
PrintFrameBorderShading(PTERWND w,HDC hPrtDC,int FrameNo,int BeginX, int BeginY)
{
    RECT rect;
    DWORD shade;
    HBRUSH hBr;
    int ParaFID;
    COLORREF FillColor=CLR_WHITE;
    BOOL colored=FALSE;
    int CellId=frame[FrameNo].CellId;

    // build the frame rectangle
    rect.left=BeginX;
    rect.right=rect.left+frame[FrameNo].width;
    rect.top=BeginY;
    rect.bottom=rect.top+frame[FrameNo].height;

    // print frame shading and background
    ParaFID=frame[FrameNo].ParaFrameId;
    if (ParaFrame[ParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_CONV)) {
       if (ParaFrame[ParaFID].FillPattern>0) {
          FillColor=ParaFrame[ParaFID].BackColor;
          if (true || FillColor!=CLR_WHITE) colored=TRUE;
       }
    }
    else {
       COLORREF FrameBackColor=frame[FrameNo].BackColor;
       //if (HtmlMode && CellId>0) FrameBackColor=0xFFFFFF;  // cell colors not printed in HTML mode

       FrameShading=frame[FrameNo].shading;
       PaintFrameFlags=frame[FrameNo].flags;

       if (FrameBackColor!=PageColor(w) || PaintFrameFlags&FRAME_FORCE_BKND_CLR) {
          int shading=FrameShading>0?FrameShading:0;
          int red=(BYTE)((GetRValue(FrameBackColor)*(100-shading))/100);
          int green=(BYTE)((GetGValue(FrameBackColor)*(100-shading))/100);
          int blue=(BYTE)((GetBValue(FrameBackColor)*(100-shading))/100);
          FillColor=RGB(red,green,blue);
       }
       else {
          shade=((DWORD)FrameShading*0xFF)/100;
          if (shade>0xFF) shade=0xFF;
          shade=(~shade)&0xFF;             // convert to blackness
          FillColor=(shade<<16)|(shade<<8)|shade;    // all comonents the same
       }
       colored=(FillColor!=CLR_WHITE);

       //if (frame[FrameNo].BackColor!=CLR_WHITE || frame[FrameNo].flags&FRAME_FORCE_BKND_CLR) {
       //   FillColor=frame[FrameNo].BackColor;
       //   colored=TRUE;
       //}
       //else if (frame[FrameNo].shading>0) {
       //   shade=((DWORD)frame[FrameNo].shading*0xFF)/100;
       //   if (shade>0xFF) shade=0xFF;
       //   shade=(~shade)&0xFF;                // convert to blackness
       //   FillColor=(shade<<16)|(shade<<8)|shade; // all comonents the same
       //   colored=TRUE;
       //}
    }

    if (colored) {
       hBr=CreateSolidBrush((COLORREF)FillColor);
       FillRect(hPrtDC,&rect,hBr);
       DeleteObject(hBr);
       PrtFrameBkColor=(COLORREF)FillColor;
    }
    else PrtFrameBkColor=CLR_WHITE;
    
    // Draw Frame Borders
    if (HtmlMode && frame[FrameNo].CellId>0 && !(TerFlags3&TFLAG3_PLAIN_TABLE_BORDER)) {
       int BorderX1=BeginX;
       int BorderY1=BeginY;
       int BorderX2=BeginX+frame[FrameNo].width;   // stay within the frame
       int BorderY2=BeginY+frame[FrameNo].height; // stay within the frame
       CurFrame=FrameNo;
       if (frame[FrameNo].border) DrawHtmlCellBorder(w,hPrtDC,frame[FrameNo].CellId,BorderX1,BorderY1,BorderX2,BorderY2,frame[FrameNo].BorderWidth,FALSE);
    }
    else {
       if (frame[FrameNo].border&BORDER_TOP)  PrintFrameBorder(w,hPrtDC,FrameNo,BORDER_INDEX_TOP,BeginX,BeginY);
       if (frame[FrameNo].border&BORDER_BOT)  PrintFrameBorder(w,hPrtDC,FrameNo,BORDER_INDEX_BOT,BeginX,BeginY);
       if (frame[FrameNo].border&BORDER_LEFT) PrintFrameBorder(w,hPrtDC,FrameNo,BORDER_INDEX_LEFT,BeginX,BeginY);
       if (frame[FrameNo].border&BORDER_RIGHT)PrintFrameBorder(w,hPrtDC,FrameNo,BORDER_INDEX_RIGHT,BeginX,BeginY);
    }

    return TRUE;
}

/******************************************************************************
    PrintFrameBorder:
    Print the specified frame border.
    To eliminate edge rounding, this function uses fills a rectangle instead
    of drawing a line.
*******************************************************************************/
PrintFrameBorder(PTERWND w,HDC hPrtDC,int FrameNo,int BorderIndex,int BeginX,int BeginY)
{
    RECT rect;
    int thickness,MinThick,CellId,ParaFID;
    POINT pt[2];
    int PageNo=(FrameNo>=FirstPage2Frame?FirstFramePage+1:FirstFramePage);
    HBRUSH hBr;
    COLORREF BorderColor;
    BOOL DottedLine;

    // build the frame rectangle
    rect.left=BeginX;
    rect.right=rect.left+frame[FrameNo].width;
    rect.top=BeginY;
    rect.bottom=BeginY+frame[FrameNo].height;

    if (frame[FrameNo].CellId>0 && True(frame[FrameNo].flags&FRAME_RTL)) SwapInts(&rect.left,&rect.right);


    // calculate the border thickness
    thickness=frame[FrameNo].BorderWidth[BorderIndex];
    if (BorderIndex==BORDER_INDEX_TOP || BorderIndex==BORDER_INDEX_BOT) {
       thickness=TwipsToPrtY(thickness);
       if (InPrintPreview && thickness>0) {  // make sure that thickness does not reduce to 0 when translating to device units
          if ((CellId=frame[FrameNo].CellId)>0) {
             pt[0].x=pt[1].x=BeginX;         // convert thickness to screen pixel
             pt[0].y=BeginY;
             if (BorderIndex==BORDER_INDEX_BOT) pt[0].y+=(frame[FrameNo].height-thickness);
             pt[1].y=pt[0].y+thickness;
             LPtoDP(hPrtDC,pt,2);
             
             if (pt[1].y<=pt[0].y) {         // if non-zero border vanishes
                if (BorderIndex==BORDER_INDEX_TOP) pt[1].y=pt[0].y+1;
                else {
                   int NextRowCell=GetSameColumnCell(w,CellId,TRUE);
                   if (NextRowCell<=0 || IsPageLastRow(w,-CellId,PageNo)) pt[1].y=pt[0].y+1;
                }
                if (pt[1].y>pt[0].y) {
                   DPtoLP(hPrtDC,pt,2);
                   thickness=(pt[1].y-pt[0].y)*5/4;
                }
             }
          }
          else {
             pt[0].x=pt[1].x=pt[0].y=0;
             pt[1].y=1;
             DPtoLP(hPrtDC,pt,2);
             MinThick=abs((pt[1].y-pt[0].y)*5/4); // minimum thickness
             if (thickness<MinThick) thickness=MinThick;
          }
       }
    }
    else {
       thickness=TwipsToPrtX(thickness);
       if (InPrintPreview && thickness>0) {  // make sure that thickness does not reduce to 0 when translating to device units
          if ((CellId=frame[FrameNo].CellId)>0) {
             pt[0].y=pt[1].y=BeginY;         // convert thickness to screen pixel
             if (BorderIndex==BORDER_INDEX_LEFT) pt[0].x=rect.left;
             else                                pt[0].x=rect.right-thickness;
             pt[1].x=pt[0].x+thickness;
             LPtoDP(hPrtDC,pt,2);
             if (pt[1].x<=pt[0].x) {         // if non-zero border vanishes
                if (BorderIndex==BORDER_INDEX_LEFT) pt[1].x=pt[0].x+1;
                else if (cell[CellId].NextCell<=0)  pt[1].x=pt[0].x+1;
                if (pt[1].x>pt[0].x) {
                   DPtoLP(hPrtDC,pt,2);
                   thickness=(pt[1].x-pt[0].x)*5/4;
                }
             }
          }
          else {
             pt[0].y=pt[1].y=pt[0].x=0;
             pt[1].x=1;
             DPtoLP(hPrtDC,pt,2);
             MinThick=abs((pt[1].x-pt[0].x)*5/4); // minimum thickness
             if (thickness<MinThick) thickness=MinThick;
          }
       }
    }


    if (thickness>0) {
       int FillThickness=thickness;

       if (TerOpFlags2&TOFLAG2_USE_PEN_FOR_BORDER) FillThickness=0;   // line drawing does not need filling

       // adjust the rectangle to reflect the area to be filled
       if (BorderIndex==BORDER_INDEX_TOP) {
          rect.bottom=rect.top+FillThickness;
       }
       else if (BorderIndex==BORDER_INDEX_BOT) {
          rect.top=rect.bottom-FillThickness;
       }
       else if (BorderIndex==BORDER_INDEX_LEFT) {
          rect.right=rect.left+FillThickness+1;
       }
       else {
          rect.left=rect.right-FillThickness;
          rect.right++;      // cover last pixel
       }
       
       BorderColor=frame[FrameNo].BorderColor[BorderIndex];
       ParaFID=frame[FrameNo].ParaFrameId;
       DottedLine=0;
       if (ParaFID>0) {
          if (ParaFrame[ParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_CONV)) BorderColor=ParaFrame[ParaFID].LineColor;
          if (ParaFrame[ParaFID].flags&PARA_FRAME_DOTTED) DottedLine=TRUE;
       }


       if (DottedLine)  DrawDottedLine(w,hPrtDC,rect.left,rect.top,rect.right,rect.bottom,BorderColor);
       else if (TerOpFlags2&TOFLAG2_USE_PEN_FOR_BORDER) {
          HPEN hPen=CreatePen(DottedLine?PS_DOT:PS_SOLID,thickness,BorderColor);
          HPEN hOldPen=SelectObject(hPrtDC,hPen);
          
          MoveToEx(hPrtDC,rect.left,rect.top,NULL);
          if (BorderIndex==BORDER_INDEX_TOP || BorderIndex==BORDER_INDEX_BOT) 
               LineTo(hPrtDC,rect.right,rect.top);
          else LineTo(hPrtDC,rect.left,rect.bottom);
          
          SelectObject(hPrtDC,hOldPen);
          DeleteObject(hPen);
       }
       else { 
         hBr=CreateSolidBrush(BorderColor);
         FillRect(hPrtDC,&rect,hBr);
         DeleteObject(hBr);
       } 
    }

    return TRUE;
}

/******************************************************************************
    FillRectangle:
    Implement FillRect to use Rectangle function. FillRect is not written out to a metafile
******************************************************************************/
FillRectangle(HDC hDC, LPRECT rect, HBRUSH hBr)
{
   HBRUSH hOldBr;
   HPEN hOldPen,hPen;
   BOOL result;
   LOGBRUSH lBrush;

   // make pen
   GetObject(hBr,sizeof(LOGBRUSH),&lBrush);
   hPen=CreatePen(PS_SOLID,0,lBrush.lbColor);
   hOldPen=SelectObject(hDC,hPen);

   hOldBr=SelectObject(hDC,hBr);
   
   result=Rectangle(hDC,rect->left,rect->top,rect->right,rect->bottom);
   SelectObject(hDC,hOldBr);
   SelectObject(hDC,hOldPen);
   DeleteObject(hPen);

   return result;
}

/******************************************************************************
    TerSelectPrint:
    Print the selected text.
******************************************************************************/
BOOL WINAPI _export TerSelectPrint(HWND hWnd)
{
    return TerSelectPrint2(hWnd,1);
}
    
/******************************************************************************
    TerSelectPrint2:
    Print the selected text.
******************************************************************************/
BOOL WINAPI _export TerSelectPrint2(HWND hWnd,int copies)
{
    BOOL result;
    struct StrPrint prt;
    DOCINFO    doc;
    PTERWND    w;
    int i;

    // Get the window pointer
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!PrinterAvailable || HilightType==HILIGHT_OFF) return FALSE;                 // printer not available

    if (!NormalizeBlock(w)) return FALSE; // normalize the hilghted block

    // set the prt structure to call the TerMergePrint function
    FarMemSet(&prt,0,sizeof(prt));

    prt.InputType='B';
    prt.hBuffer=TerGetRtfSel(hWnd,&(prt.BufferLen));
    if (!prt.hBuffer) return FALSE;
    prt.delim=0xd;
    prt.hDC=hPr;

    // start the print job
    FarMemSet(&doc,0,sizeof(doc));
    doc.cbSize=sizeof(doc);
    doc.lpszDocName=DocName;
    doc.lpszOutput=NULL;
    if (StartDoc(hPr,&doc)==SP_ERROR) {
        return PrintError(w,MSG_ERR_START_PRINTER,NULL);
    }
    
    for (i=0;i<copies;i++) {
       StartPage(hPr);

       prt.StartPos=0;
       result=TerMergePrint(&prt);
       // end the print job
       EndPage(hPr);
    }

    EndDoc(hPr);

    GlobalFree(prt.hBuffer);       // free the buffer

    return result;
}

/******************************************************************************
    TerPlayMergeMeta:
    Play the metafile created by the TerMergePrintMeta function to the user DC.
    The x/y location is specified in the user DC' logical units.
******************************************************************************/
BOOL WINAPI _export TerPlayMergeMeta(HDC hDC, HDC hAttribDC, HMETAFILE hMeta, int x, int y, int zoom)
{
   int OutResX,OutResY;
   int SaveId,map,UnitsPerInch,LogResX,LogResY;
   double PhyResX,PhyResY;
   HDC hScrDC;
   POINT pt[2];
   MFENUMPROC lpProc=NULL;
   
   if (hMeta==NULL) return false;


   // get the x/y in device units to draw the metafile 
   pt[0].x=x;
   pt[0].y=y;
   LPtoDP(hDC, &pt[0],1 );   
   x=pt[0].x;
   y=pt[0].y;

   // get the current resolution of the device (per inch)
   map=GetMapMode(hAttribDC);

   if (map==MM_LOMETRIC)       UnitsPerInch=254;    //res = .1 mm
   else if (map==MM_HIMETRIC)  UnitsPerInch=2540;   //res = .01 mm
   else if (map==MM_HIENGLISH) UnitsPerInch=1000;   //res = .001 inch
   else if (map==MM_LOENGLISH) UnitsPerInch=100;    //res = .01 inch
   else if (map==MM_TWIPS)     UnitsPerInch=1440;   //res = 1440 twips
   else return FALSE;                        // unsupported mappng mode 

   // get current resolution
   pt[0].x=pt[0].y=0;
   pt[1].x=pt[1].y=UnitsPerInch;   // one inch in current mapping mode 
   LPtoDP(hDC, pt,2 );   
   OutResX=pt[1].x-pt[0].x;
   OutResY=-(pt[1].y-pt[0].y);  // LOMETRIC uses opposit y direction
 
   // convert OutRes from logical to physical unit relative to current screen (hMeta is relative to screen DC at twips resolution)
   hScrDC=GetDC(NULL);
   LogResX=GetDeviceCaps(hScrDC,LOGPIXELSX);
   LogResY=GetDeviceCaps(hScrDC,LOGPIXELSY);
   PhyResX=(((long)GetDeviceCaps(hScrDC,HORZRES)*25.4)/GetDeviceCaps(hScrDC,HORZSIZE));
   PhyResY=(((long)GetDeviceCaps(hScrDC,VERTRES)*25.4)/GetDeviceCaps(hScrDC,VERTSIZE));
   ReleaseDC(NULL,hScrDC);

   OutResX=(int)(OutResX*LogResX/PhyResX);   // convert to logical resolution since hMeta is created in twips logical resolution
   OutResY=(int)(OutResY*LogResY/PhyResY);   // convert to logical resolution since hMeta is created in twips logical resolution

   // apply zoom percentage
   if (zoom!=100) {
      OutResX=MulDiv(OutResX,zoom,100);
      OutResY=MulDiv(OutResY,zoom,100);
   } 

   // set the units for the target device
   SaveId=SaveDC(hDC);                 // save uesr DC status
   SetMapMode(hDC,MM_ANISOTROPIC);     // use customized units
        
   
   SetWindowExtEx(hDC,1440,1440,NULL);   // metafile created with 1440 twips resolution
   SetViewportExtEx(hDC,OutResX,OutResY,NULL);   // apply any scaling to the view port

   SetWindowOrgEx(hDC,0,0,NULL);        // set window origin

   SetViewportOrgEx(hDC,x,y,NULL); 

   if (NULL!=(lpProc=(MFENUMPROC)MakeProcInstance((FARPROC)MergeEnumMetafile, hTerInst))) {
       EnumMetaFile(hDC,hMeta,lpProc,0L);        // do Enum instead of Play, because Play is buggy and fails for no reason sometimes
       FreeProcInstance((FARPROC)lpProc);        // free the process instances
   }

   RestoreDC(hDC,SaveId);
 
   return TRUE;
}
 
/******************************************************************************
    MergeEnumMetafile:
*******************************************************************************/
BOOL CALLBACK _export MergeEnumMetafile(HDC hDC, HANDLETABLE FAR *pTable, METARECORD FAR *pMetaRec, int ObjCount, LPARAM lParam)
{
    //DispMetaRecName(pMetaRec->rdFunction);

    if (pMetaRec->rdFunction==0x41) return FALSE;  // unsupported mac metafile command, stop playing

    PlayMetaFileRecord(hDC,pTable,pMetaRec,ObjCount);

    return TRUE;
}

/******************************************************************************
    TerMergePrintMeta:
    Create a metafile containing document content.  The rectangle passed in 
    the logical unit of the DC (prt.hDC).  prt.hDC must be the attribute DC 
    for the target device.
******************************************************************************/
HMETAFILE WINAPI _export TerMergePrintMeta(struct StrPrint far *prt)
{
   struct StrPrint print;
   int width,height;
   int LogResX,LogResY;
   double PhyResX,PhyResY;
   RECT PrtRect;
   BOOL result;
   HDC  hUserDC=prt->hDC;
   HMETAFILE hMeta=NULL;
   int map;

   if (prt->rect==NULL) return NULL;   // rectantgle must be specified
   PrtRect=*(prt->rect);

   print=(*prt);   // make a copy
   print.rect=&PrtRect;

   width=(print.rect->right-print.rect->left);   // width/height in logical unit of hUserDC
   height=(print.rect->bottom-print.rect->top);  // height might be negative for certain mapping mode

   // convert width/height from user logical units to twips
   map=GetMapMode(hUserDC);

   if (map==MM_LOMETRIC) {                //res = .1 mm
      width = MulDiv(width,1440,254);     
      height = -MulDiv(height,1440,254);  // positive y is up
   } 
   else if (map==MM_HIMETRIC) {           //res = .01 mm
      width = MulDiv(width,1440,2540);     
      height = -MulDiv(height,1440,2540); // positive y is up
   } 
   else if (map==MM_HIENGLISH) {          //res = .001 inch
      width = MulDiv(width,1440,1000);     
      height = -MulDiv(height,1440,1000); // positive y is up
   } 
   else if (map==MM_LOENGLISH) {          //res = .01 inch
      width = MulDiv(width,1440,100);     
      height = -MulDiv(height,1440,100);  // positive y is up
   } 
   else if (map==MM_TWIPS) {              //res = 1440 twips
      height = -height;                   // positive y is up
   }
   else return NULL;                      // unsupported mappng mode 

   // convert width/height to physical resolution - needed to maintain wysiwyg relative to redering in a control of thes same data
   print.hDC=GetDC(NULL);             // use screen for attribute dc

   LogResX=GetDeviceCaps(print.hDC,LOGPIXELSX);
   LogResY=GetDeviceCaps(print.hDC,LOGPIXELSY);
   PhyResX=(((long)GetDeviceCaps(print.hDC,HORZRES)*25.4)/GetDeviceCaps(print.hDC,HORZSIZE));
   PhyResY=(((long)GetDeviceCaps(print.hDC,VERTRES)*25.4)/GetDeviceCaps(print.hDC,VERTSIZE));
   
   width=(int)(width*PhyResX/LogResX);
   height=(int)(height*PhyResY/LogResY);

   print.rect->left=print.rect->top=0;
   print.rect->right=-width;      // the negative sign indicates twips units in TerMergePrint
   print.rect->bottom=-height;

   print.hMetaDC=CreateMetaFile(NULL);               // get DC

   result=TerMergePrint2(&print,true,0);
   if (!result) return NULL;

   hMeta=CloseMetaFile(print.hMetaDC);

   prt->NextPos=print.NextPos;           // pass the next position

   ReleaseDC(NULL,print.hDC);
   
   return hMeta;
}

/******************************************************************************
    TerMergePrint:
    This API function is used to print a file or buffer to the speicifed
    printer device context.  The print specification is provided in the
    StrPrint structure.

    This routine returns TRUE if successful.  On successful return, it also
    updates the NextPos member variable (StrPrint) to the first character on
    the next page.  If the entire file has been printed, this variable is
    set to 0.
******************************************************************************/
BOOL WINAPI _export TerMergePrint(struct StrPrint far *prt) 
{
   return TerMergePrint2(prt, false,0);
} 

BOOL TerMergePrint2(struct StrPrint far *prt, BOOL IsHiResMeta, DWORD flags)
{
    struct arg_list arg;                        // window creation parameters
    PTERWND    w;
    POINT      pt;
    HDC        hPrSave=NULL;
    int        i,FromCol,ToCol,SaveId=0,SaveMetaId=0;
    long       FromLine,ToLine,l;
    int        PagePixWidth,PagePixHeight,BeginX,BeginY,LastY;
    int        PageNo;
    BOOL       result=FALSE,PrinterDevice=TRUE;
    HCURSOR    hSaveCursor=NULL;
    HRGN       rgn;
    POINT      RectPt[2];
    RECT       PageRect;
    BOOL       PrevTextDeleted=FALSE,PageBreakInserted=FALSE;
    BOOL       ScrDCUsed=FALSE,WinDCUsed=FALSE;

    if (False(flags&MERGEPRINT_NO_EVAL_MSG) && !DrawEval(NULL)) return FALSE;

    //LogPrintf("%x, tmp, 1",prt->hDC);

    if (false && !(prt->hDC) && prt->hParentWnd) {    // print to the parent window
       prt->hDC=GetDC(prt->hParentWnd);
       WinDCUsed=true;
    } 
    
    // check if output to screen
    if (prt->hDC && GetDeviceCaps(prt->hDC,TECHNOLOGY)==DT_RASDISPLAY) {
       PrinterDevice=FALSE;
       if (prt->rect==NULL) {
          PrintError(NULL,MSG_INVALID_PRINT_RECT,"TerMergePrint");
          return FALSE;
       }
       prt->OnePage=TRUE;
    }

    // check for output to a metafile
    if (prt->hMetaDC) {
       if (!(prt->hDC)) {
          prt->hDC=GetDC(NULL);
          ScrDCUsed=true;
          //return FALSE;  // the attribute device context must be specified
       }
       prt->OnePage=TRUE;   // can print only one page to a metafile
    }

    // check if the input file exists
    lstrcpy(TempString,prt->file);
    if (prt->InputType=='F' && lstrlen(prt->file)>0 && access(TempString,4)==-1) return FALSE;  // file not found


    // open an invisible TER window
    FarMemSet(&arg,0,sizeof(struct arg_list));   // initialie the structure

    arg.x=CW_USEDEFAULT;      // use default window position
    arg.y=CW_USEDEFAULT;
    arg.width=CW_USEDEFAULT;  // Initial edit window width
    arg.height=CW_USEDEFAULT; // Initial edit window height
    arg.InitLine=1;           // Initial line number to position on
    arg.hInst=prt->hInst;     // Current application instant handle
    arg.hParentWnd=prt->hParentWnd;// let this be the parent of the editor window
    arg.style=WS_OVERLAPPEDWINDOW|TER_INVISIBLE; //  invisible window
    lstrcpy(arg.FontTypeFace,"");  // default font type faces
    arg.PointSize=12;         //  point size of the default font
    arg.WordWrap=TRUE;        // turn on Word
    arg.PrintView=TRUE;       // turn on print view
    arg.PageMode=TRUE;        // turn on page mode
    arg.FittedView=FALSE;     // turn off fitted view
    arg.modified=(int)(long)prt->hDC;    // tracking code

    arg.InputType=prt->InputType; // Input type: (F)ile or (B)uffer
    lstrcpy(arg.file,prt->file);  // file name
    arg.hBuffer=prt->hBuffer;     // handle to the input/output buffer
    arg.BufferLen=prt->BufferLen; // buffer length
    arg.delim=prt->delim;         // line delimiter for the buffer input

    if (IsHiResMeta) TerSetFlags6(NULL,TRUE,TFLAG6_HI_RES);   // create a global hi res flag
    
    if (!CreateTerWindow(&arg)) return FALSE;
    
    if (IsHiResMeta) TerSetFlags6(NULL,FALSE,TFLAG6_HI_RES);   // disable the global hi res flag, the window flag remain set

    w=GetWindowPointer(arg.hTextWnd); // get the window data area
    TerArg.hBuffer=0;             // do not allow the buffer to be freed
    TerArg.BufferLen=0;           // do not allow the buffer to be freed
    ShowHiddenText=prt->PrintHiddenText;  // print hidden text?

    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while reading

    //LogPrintf("%x, tmp, 3",prt->hDC);

    // call the callback function if specified
    if (PrintCallback) PrintCallback(prt->hParentWnd,hTerWnd);
    InPrinting=TRUE;
    HoldMessages=TRUE;            // disable window message processing
    TerOpFlags|=TOFLAG_IN_MERGE_PRINT;

    if (prt->rect) {
       int RectInTwips=(prt->rect->left<0 || prt->rect->right<0 || prt->rect->top<0 || prt->rect->bottom<0);
       PageRect=*(prt->rect);
       if (RectInTwips) {
          PageRect.left=-PageRect.left;
          PageRect.right=-PageRect.right;
          PageRect.top=-PageRect.top;
          PageRect.bottom=-PageRect.bottom;
       }
       else {
          PageRect.left=(int)(MmToInches(PageRect.left)*1440);
          PageRect.right=(int)(MmToInches(PageRect.right)*1440);
          PageRect.top=(int)(MmToInches(PageRect.top)*1440);
          PageRect.bottom=(int)(MmToInches(PageRect.bottom)*1440);
       }
    }    

    //LogPrintf("%x, tmp, 4",prt->hDC);

    //Create the printer device context
    if (prt->hDC) {               // use the specified device context
       hPrSave=hPr;                  // save the current device context
       hPr=prt->hDC;              // use the specified device context

       SaveId=SaveDC(hPr);           // save the current status of the printer device context
       if (!IsHiResMeta) SetMapMode(hPr,MM_TEXT);      // use pixel units

       PrtResX=GetDeviceCaps(hPr,LOGPIXELSX);    // number of pixels per inch of X direction
       PrtResY=GetDeviceCaps(hPr,LOGPIXELSY);    // number of pixels per inch of Y direction
       
       if (UseScrMetrics) PrtResX=PrtResY=UNITS_PER_INCH;  // draw in twips units
                                                                        
       if (GetDeviceCaps(hPr,TECHNOLOGY)==DT_RASDISPLAY) {
          PortraitHX=PortraitHY=0;
          LandscapeHX=LandscapeHY=0;
       } 


       if (PrinterDevice) {                      // printer device context
          Escape(hPr,GETPHYSPAGESIZE,0,NULL,&pt);
          PageWidth=(float)pt.x/PrtResX;            // actual page size
          PageHeight=(float)pt.y/PrtResY;
          TerOpFlags|=TOFLAG_EXTERNAL_DC;           // print to the external printer device context
       }
       else {                                       // window device context
          SetViewportOrgEx(hPr,0,0,NULL);           // set view port origins
          SetWindowOrgEx(hPr,0,0,NULL);             // set the logical window origin

          PageWidth=TwipsToInches(PageRect.right);   // assume the page end at the rectangle
          PageHeight=TwipsToInches(PageRect.bottom);
       }

       if (!RecreateFonts(w,hTerDC)) goto END;   // recreate the fonts
    }
    else if (!hPr) OpenCurPrinter(w,TRUE);          // reopen our current printer as private


    //LogPrintf("%x, tmp, 5",prt->hDC);

    // set the metafile DC if provided
    if (prt->hMetaDC) {
       hPrtDataDC=prt->hMetaDC;
       TerOpFlags2|=TOFLAG2_PRINT_TO_META_DC;
    }
    else              hPrtDataDC=hPr;

    // Enable header/footer if necessary
    if (prt->rect==NULL && !ViewPageHdrFtr) {    // hdr/ftr valid only for default margin
       for (l=0;l<TotalLines;l++) if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) break;
       if (l<TotalLines) ToggleViewHdrFtr(w); // display header/footers
    }

    // set the margin
    if (prt->rect) {                // print in the specified area
       BOOL DeletePrevText=FALSE;

       TerOpFlags|=TOFLAG_MERGE_PRINT_RECT;

       // convert the rectangle in to device units.
       RectPt[0].x=TwipsToPrtX(PageRect.left);
       RectPt[1].x=TwipsToPrtX(PageRect.right);
       RectPt[0].y=TwipsToPrtY(PageRect.top);
       RectPt[1].y=TwipsToPrtY(PageRect.bottom);
       LPtoDP(hPrtDataDC,RectPt,2);

       MPRect.left=RectPt[0].x;
       MPRect.top=RectPt[0].y;
       MPRect.right=RectPt[1].x;
       MPRect.bottom=RectPt[1].y;

       if (GetDeviceCaps(hPrtDataDC,TECHNOLOGY)!=DT_METAFILE) {
          if (hPrtDataDC==prt->hMetaDC) SaveMetaId=SaveDC(prt->hMetaDC);

          // set the clipping region on the rectangle on the paint dc
          rgn=CreateRectRgn(RectPt[0].x-1,RectPt[0].y-1,RectPt[1].x,RectPt[1].y);
          SelectClipRgn(hPrtDataDC,rgn);
          DeleteObject(rgn);
       }

       pt.x=pt.y=0;
       if (PrinterDevice) Escape(hPr,GETPRINTINGOFFSET,0,NULL,&pt);// actual offset where printing begins

       // change any page relative frame to margin related frames
       if (TotalParaFrames>1) {
         int ParaFID,marg;
         for (ParaFID=1;ParaFID<TotalParaFrames;ParaFID++) {
           if (!ParaFrame[ParaFID].InUse || !(ParaFrame[ParaFID].flags&PARA_FRAME_VPAGE)) continue;
         
           marg=(int)(TerSect[0].TopMargin*1440);

           ParaFrame[ParaFID].y-=marg;
           ParaFrame[ParaFID].ParaY-=marg;
         
           ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_VPAGE);
           ParaFrame[ParaFID].flags|=PARA_FRAME_VMARG;
         } 
       }

       // set margins
       for (i=0;i<TotalSects;i++) { // set margins
          TerSect1[i].PgWidth=TerSect[i].PprWidth=PageWidth;      // use the uniform height and width
          TerSect1[i].PgHeight=TerSect[i].PprHeight=PageHeight;
          if (pDeviceMode) TerSect[i].orient=pDeviceMode->dmOrientation;

          TerSect[i].LeftMargin=TwipsToInches(PageRect.left)+PrtToInchesX(pt.x);
          TerSect[i].TopMargin=TwipsToInches(PageRect.top)+PrtToInchesY(pt.y);
          if (PrinterDevice) {
             TerSect[i].RightMargin=PageWidth-TwipsToInches(PageRect.right)-PrtToInchesX(pt.x);
             TerSect[i].BotMargin=PageHeight-TwipsToInches(PageRect.bottom)-PrtToInchesY(pt.y);
          }
          else TerSect[i].RightMargin=TerSect[i].BotMargin=(float)0;
          TerSect[i].HdrMargin=TerSect[i].TopMargin;
          TerSect[i].FtrMargin=TerSect[i].BotMargin;
       }

       // delete the text before start pos.  We can do this
       // because we don't use header/footer when printing to a rectangle
       // This logic facilitate variable size rectangle
       DeletePrevText=(prt->StartPos>0);
       if (DeletePrevText) {
          long l,BltId;
          AbsToRowCol(w,prt->StartPos,&HilightEndRow,&HilightEndCol);
       
          for (l=0;l<HilightEndRow;l++) {       // check if previous text uses paragraph numbering
              BltId=PfmtId[pfmt(l)].BltId;
              if (BltId>0 && (!TerBlt[BltId].IsBullet || TerBlt[BltId].ls>0)) {
                 // put a page break before the StartPos to accommodate variable size rectangles
                 if (prt->StartPos>0) {
                    int col=-1;
                    SetTerCursorPos(hTerWnd,HilightEndRow,HilightEndCol,TRUE);
                    TerPageBreak(hTerWnd,TRUE);
                    GetTerCursorPos(hTerWnd,&(prt->StartPos),&col);
                    PageBreakInserted=TRUE;
                 }   
                 
                 DeletePrevText=FALSE;
                 break;
              } 
          } 
       } 

       if (DeletePrevText) {   // don't do for specific page (StartPos<0) because pages not yet formed for rectangle printing
          HilightBegRow=0;
          HilightBegCol=0;
          AbsToRowCol(w,prt->StartPos,&HilightEndRow,&HilightEndCol);
          HilightType=HILIGHT_CHAR;
          DeleteCharBlock(w,FALSE,FALSE);
          PrevTextDeleted=TRUE;
       }
    }


    //LogPrintf("%x, tmp, 6",prt->hDC);

    // Merge data fields if any
    RecreateSections(w);          // recalulate the section boundaries

    if (HtmlMode) TerAdjustHtmlTable(hTerWnd);

    Repaginate(w,FALSE,FALSE,0,TRUE); // repaginate to connect the lines
    if (!TerMergeFields(hTerWnd,prt->MergeFields,prt->MergeData,FALSE)) {
       PrintError(w,MSG_ERR_MAIL_MERGE,NULL);
       goto END;
    }

    // repaginate
    RecreateSections(w);          // recalulate the section boundaries
    Repaginate(w,FALSE,FALSE,0,TRUE); // repaginate to connect the lines
    RecreateSections(w);
    if (TotalParaFrames>1) Repaginate(w,FALSE,FALSE,0,TRUE);  // another wrap to take care of para frames

    // Calculate the page number to print
    if (prt->StartPos<0) {      // printing specific page
       int page=(int)(-prt->StartPos);
       if (page>=TotalPages) page=TotalPages-1;
       prt->StartPos=RowColToAbs(w,PageInfo[page].FirstLine,0);
       AbsToRowCol(w,prt->StartPos,&FromLine,&FromCol);  // printing beginning position
       PageNo=page; // LinePage(FromLine);  // FromLine might belong the previous page incase of page-breaks within tables, so use the page variable instead
    }
    else if (prt->rect && PrevTextDeleted) {
       PageNo=0;
       FromLine=0;
       FromCol=0;
    }
    else {
       AbsToRowCol(w,prt->StartPos,&FromLine,&FromCol);  // printing beginning position
       PageNo=LinePage(FromLine);
       //for (PageNo=0;PageNo<TotalPages;PageNo++) if (PageInfo[PageNo].FirstLine>FromLine) break;
       //if (PageNo>0) PageNo--;
    }


    // get printing area dimensions
    CalcPageDim(w,FromLine,&PagePixWidth,&PagePixHeight,&BeginX,&BeginY,&LastY); // calculate printing area dimensions

    if (PagePixWidth<=0 || PagePixHeight<=0) {
       PrintError(w,MSG_PRINT_AREA_SMALL,NULL);
       goto END;
    }

    //LogPrintf("%x, tmp, 7",prt->hDC);

    // Mark the text range end
    if (prt->OnePage) {
       // find the last line and col of the first page
       CurPage=PageNo;                      // start printing from the first page
       CreateFrames(w,TRUE,CurPage,CurPage);// create the frames for the current page
       ToLine=PageInfo[CurPage].LastLine;
       //ToLine=0;
       //for (i=0;i<TotalFrames;i++) {
       //   int CurPara;
       //   if (frame[i].empty) continue;
       //   CurPara=pfmt(frame[i].PageLastLine);
       //   if (frame[i].PageLastLine>ToLine && !(PfmtId[CurPara].flags&PAGE_HDR_FTR)) ToLine=frame[i].PageLastLine;
       //}
    }
    else ToLine=TotalLines-1;

    ToCol=LineLen(ToLine)-1;
    if (ToCol<0) ToCol=0;

    //*** Beging Printing ***
    if (!prt->hDC) {                     // we are responsible for our device context
       DOCINFO    doc;

       // start the job
       FarMemSet(&doc,0,sizeof(doc));
       doc.cbSize=sizeof(doc);
       doc.lpszDocName=DocName;
       doc.lpszOutput=NULL;
       if (StartDoc(hPr,&doc)==SP_ERROR) {
           return PrintError(w,MSG_ERR_START_PRINTER,NULL);
       }
    }

    EnableWindow(hTerWnd,FALSE);         // disable the main window
    if (!RecreateFonts(w,hPr)) goto END; // create printer fonts (MUST be after disabling the hTerWnd)

    // print the text
    //LogPrintf("%x, tmp, 8, before PrintViewMode",prt->hDC);
    

    if (prt->OnePage) prt->NextY=PrintViewMode(w,hPr,-1,PageNo,1,0);
    else              prt->NextY=PrintViewMode(w,hPr,FromLine,ToLine,1,0);
    //LogPrintf("%x, tmp, 9",prt->hDC);

    if (!prt->hDC) {
        EndPage(hPr);
        EndDoc(hPr);
    }

    // set the last position
    if ((ToLine+1)>=TotalLines || CurPage>=(TotalPages-1)) prt->NextPos=0; // end of file
    else {
       //prt->NextPos=RowColToAbs(w,ToLine+1,0);     // next page to begin here
       prt->NextPos=RowColToAbs(w,PageInfo[PageNo+1].FirstLine,0);
       if (PrevTextDeleted) prt->NextPos+=prt->StartPos; // add the delete text size
       else if (PageBreakInserted) prt->NextPos-=1;   // adjust for inserted page break
    }
    result=TRUE;

    // End the print job
    END:
    SelectObject(hPr,GetStockObject(SYSTEM_FONT));  // deselect our font
    RecreateFonts(w,hTerDC);           // recreate the display fonts
    EnableWindow(hTerWnd,TRUE);        // activate the main window

    if (prt->hDC && SaveId) RestoreDC(prt->hDC,SaveId);  // restore the user printer device context
    if (prt->hMetaDC && SaveMetaId) RestoreDC(prt->hMetaDC,SaveMetaId);

    if (hPrSave) hPr=hPrSave;          // reinstate the our printer device context

    // close the window
    TerArg.modified=0;                 // don't allow file modification
    CloseTer(hTerWnd,TRUE);            // close the TER window

    if (hSaveCursor) SetCursor(hSaveCursor);
    
    if (ScrDCUsed) {
       ReleaseDC(NULL,prt->hDC);
       prt->hDC=0;
    } 

    if (WinDCUsed) {
       ReleaseDC(prt->hParentWnd,prt->hDC);
       prt->hDC=0;
    } 

    //LogPrintf("%x, tmp, 10",prt->hDC);

    return result;
}

/******************************************************************************
    TerSetPrintPreview:
    External print preview begins or ends. This function saves the fonts until and
    temporary editor modes until the external print preview ends.
******************************************************************************/
BOOL WINAPI _export TerSetPrintPreview(HWND hWnd, BOOL begin)
{
    PTERWND    w;

    // Get the window pointer
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (begin && ExtPrintPreview) return FALSE;   // print preview already in progress
    if (!begin && !ExtPrintPreview) return FALSE; // print preview not in progress

    if (begin) {                                  // save screen position variables
       SavePvFirstFramePage=FirstFramePage;
    } 
    
    if (!begin && InPrinting) {                   // exit print preview
       PostPreview(w);                            // post printing functions
       
       if (TerArg.FittedView) TerRepaginate(hTerWnd,FALSE);  // to length page sizes to fitted-view page sizes

       if(SavePvFirstFramePage != FirstFramePage && TerArg.PageMode) 
           CreateFrames(w,FALSE,SavePvFirstFramePage,SavePvLastFramePage);

       SetTerWindowOrg(w);                     // set the window origin
       InitCaret(w);

       PaintFlag=PAINT_WIN;
       RequestPagination(w,TRUE);

       PaintTer(w);
    }

    ExtPrintPreview=begin;                        // set the flag

    return TRUE;
}

/******************************************************************************
    TerPrintPreview:
    This function is used to draw a text image of the specified page in
    the current document to the specified rectangle at the specified device
    context.

    This function return TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerPrintPreview(HWND hWnd, HDC hDC, RECT far *rect, int page, BOOL scale)
{
    PTERWND    w;
    SIZE size;
    int  SaveTotalPages;

    // Get the window pointer
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!rect) rect=&ParamRect;              // use the argument rectangle passed previously

    if (!ExtPrintPreview || !InPrinting) {          // initialize not in external print preview
       if (!PrePreview(w,hDC)) {
          InPrintPreview=FALSE;
          return FALSE;  // tasks to perform before the printing begins
       }
       if (ExtPrintPreview) GetViewportExtEx(hDC,&PvViewport);
       HoldMessages=TRUE; // disable the message queue
    }

    // check if the size of the external viewport has changed
    if (ExtPrintPreview) {
       if (GetViewportExtEx(hDC,&size) && memcmp(&size,&PvViewport,sizeof(size))!=0) {  // reset print preview
          PostPreview(w);     // post printing functions

          if (!TerArg.WordWrap) SaveTotalPages=TotalPages;   // PrePreview initializes this variable in non-wordwrap mode
          PrePreview(w,hDC);  // restart
          if (!TerArg.WordWrap) TotalPages=SaveTotalPages;

          PvViewport=size;
          HoldMessages=TRUE;  // disable the message queue
       }
       else hPrtDataDC=hDC;   // set again hDC may have changed from the previous call to this function
    }

    // get the current page number
    if (TerArg.PrintView && page<0) page=GetCurPage(w,CurLine);

    PreviewOnePage(w,hDC,rect,page,scale); // preview this page

    if (!ExtPrintPreview) {                // perform exit logic when not in external print preview
       PostPreview(w);                     // post printing functions
       PaintTer(w);
    }

    // remove our resources from an external DC
    SelectObject(hDC,GetStockObject(SYSTEM_FONT));  // deselect our font
    SelectObject(hDC,GetStockObject(BLACK_PEN));
    SelectObject(hDC,GetStockObject(BLACK_BRUSH));

    return TRUE;
}

/******************************************************************************
    TerSetPreview:
    Set preview parameters such as number of pages and preview zoom percent.
    To show full fitted pages, set the zoom to 0.

    This function return TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetPreview(HWND hWnd, int pages, int zoom, BOOL ShowToolbar)
{
    PTERWND    w;

    dm("TerSetPreview");

    // Get the window pointer
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (pages<1) pages=1;
    if (pages>2) pages=2;
    if (zoom<0) zoom=0;
    if (zoom>200) zoom=200;

    TotalPreviewPages=pages;
    PreviewZoom=zoom;
    ShowPvToolbar=ShowToolbar;    // this setting is effective during the next print preview session

    if (InPrintPreview) {         // refresh
       RECT rect=TerRect;
       rect.top=TerWinRect.top;
       TerInvalidateRect(w,&rect,TRUE);   // force WM_PAINT with erase background
    }

    return TRUE;
}

/******************************************************************************
    TerMergePrintVB:
    This is an alternate API functions to call the TerMergePrint.  This
    function can be used to call the TerMergePrint function from a Visual
    Basic application.  This function simply applies the printing rectangle,
    and mail merge data to the 'prt' structure and calls the TerMergePrint
    function.
******************************************************************************/
BOOL WINAPI _export TerMergePrintVB(struct StrPrint far *prt, LPRECT rect, LPBYTE MergeFields, LPBYTE MergeData)
{
    BOOL result;

    prt->rect=rect;
    prt->MergeFields=MergeFields;
    prt->MergeData=MergeData;

    result=TerMergePrint(prt);

    return result;
}

/******************************************************************************
    TerMergePrintRep:
    This function prints given file or the buffer to the given rectangle.
    This functions returns the character position for the next printing. It returns
    0 when the last page is printed.  It returns -1 if an error occurs.
******************************************************************************/
long WINAPI _export TerMergePrintRep(HDC hPrintDC,LPBYTE FileName,HGLOBAL hBuf,DWORD BufLen,RECT far *pPrintRect,LPBYTE FieldNames,LPBYTE FieldValues, long start, LPINT NextY)
{
    return TerMergePrintRep2(hPrintDC,FileName,hBuf,BufLen,pPrintRect,FieldNames,FieldValues,start,NextY,NULL);
}
 
/******************************************************************************
    TerMergePrintRE:
    Special function to be called by ReportEase.
******************************************************************************/
long WINAPI _export TerMergePrintRE(HDC hPrintDC,LPBYTE FileName,HGLOBAL hBuf,DWORD BufLen,RECT far *pPrintRect,LPBYTE FieldNames,LPBYTE FieldValues, long start, LPINT NextY, HDC hAttribDC)
{
    long result;

    TerSetFlags4(NULL,TRUE,TFLAG4_REPORT_EASE_MODE);  // turn-on reportease mode
    result=TerMergePrintRep2(hPrintDC,FileName,hBuf,BufLen,pPrintRect,FieldNames,FieldValues,start,NextY,hAttribDC);
    TerSetFlags4(NULL,FALSE,TFLAG4_REPORT_EASE_MODE);

    return result;
}

/******************************************************************************
    TerMergePrintRep2:
    This function prints given file or the buffer to the given rectangle.
    This functions returns the character position for the next printing. It returns
    0 when the last page is printed.  It returns -1 if an error occurs.
******************************************************************************/
long WINAPI _export TerMergePrintRep2(HDC hPrintDC,LPBYTE FileName,HGLOBAL hBuf,DWORD BufLen,RECT far *pPrintRect,LPBYTE FieldNames,LPBYTE FieldValues, long start, LPINT NextY, HDC hAttribDC)
{
    return TerMergePrintRep3(hPrintDC,FileName,hBuf,BufLen,pPrintRect,FieldNames,FieldValues,start,NextY,hAttribDC,TRUE);
}

/******************************************************************************
    TerMergePrintRep3:
    This function prints given file or the buffer to the given rectangle.
    This functions returns the character position for the next printing. It returns
    0 when the last page is printed.  It returns -1 if an error occurs.
******************************************************************************/
long WINAPI _export TerMergePrintRep3(HDC hPrintDC,LPBYTE FileName,HGLOBAL hBuf,DWORD BufLen,RECT far *pPrintRect,LPBYTE FieldNames,LPBYTE FieldValues, long start, LPINT NextY, HDC hAttribDC, BOOL OnePage)
{ 
    return TerMergePrintRep4(hPrintDC,FileName,hBuf,BufLen,pPrintRect,FieldNames,FieldValues,start,NextY,hAttribDC,OnePage,0);
}

long TerMergePrintRep4(HDC hPrintDC,LPBYTE FileName,HGLOBAL hBuf,DWORD BufLen,RECT far *pPrintRect,LPBYTE FieldNames,LPBYTE FieldValues, long start, LPINT NextY, HDC hAttribDC, BOOL OnePage, DWORD flags)
{
    struct StrPrint prt;
    HDC hScrDC=NULL;
    BOOL result;

    //LogPrintf("%x, TerMergePrintRep 1",hPrintDC);
    
    FarMemSet(&prt,0,sizeof(prt));

    // fill the print structure
    if (FileName && lstrlen(FileName)>0) {
       prt.InputType='F';
       lstrcpy(prt.file,FileName);
    }
    else {
       prt.InputType='B';
       prt.hBuffer=hBuf;
       prt.BufferLen=BufLen;
       prt.delim=0xd;
    }
    prt.rect=pPrintRect;
    prt.OnePage=OnePage;
    prt.StartPos=start;          // start character position
    prt.MergeFields=FieldNames;  // merge field names
    prt.MergeData=FieldValues;   // merge field values

    // assign the device context
    if ((GetDeviceCaps(hPrintDC,TECHNOLOGY)==DT_METAFILE)) {
       if (hAttribDC) prt.hDC=hAttribDC;
       else           prt.hDC=hScrDC=GetDC(NULL);
       prt.hMetaDC=hPrintDC;
    }
    else {
       prt.hDC=hPrintDC;
       if (hAttribDC) {          // attribute dc specified
          prt.hDC=hAttribDC;
          prt.hMetaDC=hPrintDC;
       }
    }

    //LogPrintf("%x, TerMergePrintRep 2",hPrintDC);

    result=TerMergePrint2(&prt,false,flags);

    //LogPrintf("%x, TerMergePrintRep 3",hPrintDC);

    if (hScrDC) ReleaseDC(NULL,hScrDC);   // delete the temporary screen DC

    if (result) {
        if (NextY) (*NextY)=prt.NextY;    // y position where the painting halted
        return prt.NextPos;         // successful
    }
    else  return -1;
}

/******************************************************************************
    PrintProc:
    Dispatches the messages to the PrintAbort routine, and checks
    if the user cancelled the printing
******************************************************************************/
int CALLBACK _export PrintProc(HDC hPrtDC, int code)
{
    MSG msg;
    PTERWND w;
    BOOL abort=FALSE;



    // get any cancel message
    while (!abort && PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {  // process message until abort
       if (hCurAbortWnd==NULL || !IsDialogMessage(hCurAbortWnd,&msg)) {  // dispatch modeless dialog messages
           TranslateMessage(&msg);   // translate/dispatch other window messages
           DispatchMessage(&msg);
       }
       if (hCurAbortWnd) {           // retrieve the updated abort flag
           w=(PTERWND)(DWORD)GetWindowLong(hCurAbortWnd,DWL_USER);
           if (w) abort=AbortPrint;
           else   abort = true;
       }
    }


    return (!abort);
}

/******************************************************************************
    TerPrintPreviewMode:
    Turn on the print preview mode.  The print preview tool bar is displayed
    when the 'toolbar' argument is true.  This function returns TRUE when
    successful.
*******************************************************************************/
BOOL WINAPI _export TerPrintPreviewMode(HWND hWnd, BOOL toolbar)
{
    PTERWND w;
    RECT rect;

    // Get the window pointer
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // toggle off print preview
    if (InPrintPreview) {
       // restore the screen resolution
       ScrResX=SavePvScrResX;
       ScrResY=SavePvScrResY;

       PostPreview(w);                  // post printing functions

       InPrintPreview=FALSE;

       // destory the print preview tool bar
       if (hPvToolBarWnd) {
          SetFocus(hTerWnd);             // remove the focus from the toolbar
          DestroyWindow(hPvToolBarWnd);  // destroy the tool bar
          // free the process instances
          #if !defined(WIN32)
             if (lpToolBar) FreeProcInstance(lpToolBar);
             if (ZoomSubclassProc) FreeProcInstance((FARPROC)ZoomSubclassProc);
          #endif

          hPvToolBarWnd=NULL;            // reset the tool bar handle
       }

       TerArg.ShowStatus=SavePvStatus;
       TerArg.ruler=SavePvRuler;
       TerArg.ToolBar=SavePvToolbar;
       ToolBarHeight=SavePvToolBarHeight;

       // destroy the horizontal scroll bar
       if (!SavePvShowHorBar && TerArg.ShowHorBar) {
          TerArg.ShowHorBar=SavePvShowHorBar;
          ShowScrollBar(hTerWnd,SB_HORZ,FALSE);
       }

       if (TerArg.ToolBar) ShowWindow(hToolBarWnd,SW_RESTORE); // redisplay the toolbar

       RulerPending=TRUE;
       RepaintRuler=TRUE;

       if (TerArg.WordWrap && CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
       if (CurCol<0) CurCol=0;
       PrevCursPage=CurPage;
       PrevCursLine=CurLine;
       PrevCursCol=CurCol;
       CursDirection=CURS_FORWARD;

       if (TerArg.FittedView || TotalParaFrames>1) TerRepaginate(hTerWnd,FALSE);  // to length page sizes to fitted-view page sizes

       if(SavePvFirstFramePage != FirstFramePage && TerArg.PageMode) 
           CreateFrames(w,FALSE,SavePvFirstFramePage,SavePvLastFramePage);

       SetTerWindowOrg(w);                     // set the window origin
       InitCaret(w);

       PaintFlag=PAINT_WIN;
       RequestPagination(w,TRUE);

       TerInvalidateRect(w,NULL,TRUE);     // force WM_PAINT with erase background

       return TRUE;
    }

    // ********** Turn on the print preview mode ************
    // turnoff status, ruler and tool bar
    if (!PrePreview(w,hTerDC)) {
       InPrintPreview=FALSE;
       return FALSE;  // tasks to perform before the printing begins
    }

    // reset the screen to 100 zoom
    SavePvScrResX=ScrResX;
    SavePvScrResY=ScrResY;
    //ScrResX=OrigScrResX;
    //ScrResY=OrigScrResY;

    TerArg.ShowStatus=TerArg.ruler=TerArg.ToolBar=FALSE;
    TerDestroyCaret(w);
    if (SavePvToolbar) ShowWindow(hToolBarWnd,SW_HIDE);   // hide the toolbar

    // create the print preview tool bar
    hPvToolBarWnd=NULL;
    ToolBarHeight=0;
    if (toolbar) {
       RECT rect;
       if (NULL==(lpToolBar=MakeProcInstance((FARPROC)PrintPreviewProc, hTerInst))) return FALSE;
       if (NULL==(hPvToolBarWnd=CreateDialogParam(hTerInst,"PrintPreview",hTerWnd,(DLGPROC)lpToolBar,(DWORD)w))) return FALSE;
       SetWindowLong(hPvToolBarWnd,DWL_USER,(LONG)w);  // set the window data area

       // Get the tool bar height
       GetWindowRect(hPvToolBarWnd,&rect);
       ToolBarHeight=rect.bottom-rect.top;
       InvalidateRect(hPvToolBarWnd,NULL,TRUE);

       // Subclass the zoom control to access the carriage return key
       if (NULL!=(hWnd=GetComboEditWindow(hPvToolBarWnd,IDC_PV_ZOOM))) {
          ZoomSubclassProc=(WNDPROC)MakeProcInstance((FARPROC)ZoomSubclass, hTerInst);
          OrigZoomSubclassProc=(WNDPROC)(DWORD)GetWindowLong(hWnd,GWL_WNDPROC); // original control proceedure
          SetWindowLong(hWnd,GWL_WNDPROC,(DWORD)ZoomSubclassProc);
       }
       else ZoomSubclassProc=NULL;
    }

    // display the horrizontal scroll bar
    if (!TerArg.ShowHorBar) {
       TerArg.ShowHorBar=TRUE;
       ShowScrollBar(hTerWnd,SB_HORZ,TRUE);
       SetScrollRange(hTerWnd,SB_HORZ,0,SCROLL_RANGE,FALSE);
    }

    // refresh the screen
    rect=TerRect;
    rect.top+=ToolBarHeight;

    TerInvalidateRect(w,&rect,TRUE);   // force WM_PAINT with erase background

    return TRUE;
}


/******************************************************************************
    PrePreview:
    Tasks to perform before the printing begins
*******************************************************************************/
PrePreview(PTERWND w,HDC hDC)
{
    int i;
    BOOL result=FALSE,SavePaintEnabled;
    HCURSOR hSaveCursor=NULL;

    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(LoadCursor(NULL,IDC_WAIT));

    InPrinting=TRUE;
    HoldMessages=TRUE;                    // disable message loop

    InPrintPreview=TRUE;
    SavePaintEnabled=PaintEnabled;        // disable painting
    PaintEnabled=FALSE;

    HideOcx(w,0);                          // hide any visible ocx

    if (EditFootnoteText) ToggleFootnoteEdit(w,TRUE);   // hide footnote editing
    if (EditEndnoteText) ToggleFootnoteEdit(w,FALSE);   // hide footnote editing

    SavePvWordWrap=TerArg.WordWrap;
    SavePvPrintView=TerArg.PrintView;
    SavePvPageMode=TerArg.PageMode;
    SavePvPageBorder=ShowPageBorder;
    SavePvFittedView=TerArg.FittedView;
    SavePvStatus=TerArg.ShowStatus;
    SavePvRuler=TerArg.ruler;
    SavePvToolbar=TerArg.ToolBar;
    SavePvToolBarHeight=ToolBarHeight;
    SavePvShowHorBar=TerArg.ShowHorBar;
    SavePvViewPageHdrFtr=ViewPageHdrFtr;
    SavePvOrgX=TerWinOrgX;
    SavePvOrgY=TerWinOrgY;
    SavePvFirstFramePage=FirstFramePage;
    SavePvLastFramePage=LastFramePage;

    RecreateSections(w);

    if (!OpenCurPrinter(w,FALSE)) goto END; // reopen current printer
    //if (PrinterAvailable) {
    //  if ((pDeviceMode->dmPaperSize!=PaperSize && PaperSize>0) || pDeviceMode->dmOrientation!=PaperOrient) TerSetPaper(hTerWnd,PaperSize,PaperOrient,FALSE);
    //}

    // enable print view mode temporariy
    if (TerArg.WordWrap && !TerArg.PrintView) {
       TerArg.PrintView=TRUE;
       if (!RecreateFonts(w,hTerDC)) goto END; // recreate the fonts
    }

    // Preparation for PrintView mode editing
    if (TerArg.PrintView) {
       ShowPageBorder=FALSE;                         // turnoff page border temporarily

       // enable page mode temporarily
       if (!TerArg.PageMode) TerArg.PageMode=TRUE;
       TerArg.FittedView=FALSE;                      // full page mode in effect

       // disable page header/footer editgin
       if (SavePvPageMode && EditPageHdrFtr) ToggleEditHdrFtr(w);

       // enable page header/footer temporarily
       if (SavePvPrintView && !ViewPageHdrFtr) {
          for (i=0;i<TotalSects;i++) {
             if (TerSect1[i].hdr.FirstLine>=0) {
                ToggleViewHdrFtr(w);
                break;
             }
          }
       }
    }

    // repaginate
    if (TerArg.PrintView) {
       if (HtmlMode) TerAdjustHtmlTable(hTerWnd);
       
       Repaginate(w,FALSE,FALSE,0,TRUE);
       RecreateSections(w);
    }

    // Recreate fonts
    if (!RecreateFonts(w,hPr)) goto END;   // create printer fonts (MUST be after disabling the hTerWnd)

    if (GetDeviceCaps(hDC,TECHNOLOGY)==DT_RASDISPLAY /*&& !(TerOpFlags2&TOFLAG2_PDF_PRINT)*/) {
       UsingZoomFonts=TRUE;                   // create approximate fonts
       if (!RecreateFonts(w,hPr)) goto END;   // create print preview fonts retaining the previous font information
    }
    else UsingZoomFonts=FALSE;

    if (!TerArg.WordWrap) TotalPages=CurPage=0;    // pages not build yet

    TerWinOrgX=TerWinOrgY=0;
    PrtTextBkColor=PrtParaBkColor=PrtFrameBkColor=CLR_WHITE;

    hPrtDataDC=hDC;                     // device context to send the printer data to

    result=TRUE;

    END:
    HoldMessages=FALSE;                // enable message loop
    PaintEnabled=SavePaintEnabled;

    if (hSaveCursor) SetCursor(hSaveCursor);

    return result;
}

/******************************************************************************
    PostPreview:
    Tasks to perform after the printing ends
*******************************************************************************/
PostPreview(PTERWND w)
{
    BOOL SavePaintEnabled;
    HCURSOR hSaveCursor=NULL;

    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(LoadCursor(NULL,IDC_WAIT));

    HoldMessages=TRUE;                  // disable message processing
    SavePaintEnabled=PaintEnabled;      // disable painting
    PaintEnabled=FALSE;

    TerWinOrgX=TerWinOrgY=0;

    TerArg.FittedView=SavePvFittedView;        // restore the fitted view
    ShowPageBorder=SavePvPageBorder;           // restore the page border

    // Toggle off temporary view of page header/footer
    if (!SavePvViewPageHdrFtr && ViewPageHdrFtr && SavePvPageMode) ToggleViewHdrFtr(w);

    // Toggle off temporary page mode
    if (!SavePvPageMode) TogglePageMode(w);

    // disable any temporary PrintView mode
    if (!SavePvPrintView)  TerArg.PrintView=TerArg.PageMode=FALSE;

    // enable our window
    UsingZoomFonts=FALSE;            // create exact fonts
    InPrinting=FALSE;
    HoldMessages=FALSE;              // enable message processing
    InPrintPreview=FALSE;            // not in print preview any more

    RecreateFonts(w,hTerDC);         // recreate the display fonts

    TextBorder=ParaShading=0;        // reset text border
    TerWinOrgX=SavePvOrgX;           // restore the window origin
    TerWinOrgY=SavePvOrgY;
    SetWindowOrgEx(hTerDC,TerWinOrgX,TerWinOrgY,NULL);   // set the logical window origin

    PaintEnabled=SavePaintEnabled;   // enabled painting

    hPrtDataDC=NULL;                 // reset the printer data DC

    WrapFlag=WRAP_PAGE;              // rewrap atleast one page
    
    if (HtmlMode) TerAdjustHtmlTable(hTerWnd);

    if (hSaveCursor) SetCursor(hSaveCursor);

    return TRUE;
}

/******************************************************************************
    PaintPrintPreview:
    Do the painting for print preview
*******************************************************************************/
PaintPrintPreview(PTERWND w)
{
    int   i,sect,SaveId,x,y,PageNo,PageDist,ScrPgWidth,ScrPgHeight;
    int   TempOrgX,TempOrgY,CurPgWidth,CurPgHeight;
    float PgWidth,PgHeight,width1,height1,width2,height2,width,height;
    RECT  rect,rect1;
    HPEN hOldPen;
    HBRUSH hOldBrush;
    int  PrtWinHeight,PrtPageHeight,ScrollPos;
    int  PrtWinWidth,PrtPageWidth,SaveCurPage;
    long CurPixLength,TotalPixLength;
    HDC  hDC;

    if (TotalPreviewPages==0) return TRUE;

    // get the page number
    if (TerArg.PrintView) CurPage=GetCurPage(w,CurLine);
    PageNo=SaveCurPage=CurPage;

    // set the buffer DC
    if (hBufDC) hDC=hPrtDataDC=hBufDC;
    else        hDC=hTerDC; 

    // width and height of all pages
    if (TotalPreviewPages>2) TotalPreviewPages=2;
    sect=TerGetPageSect(hTerWnd,PageNo);
    PgWidth=width1=TerSect1[sect].PgWidth;
    PgHeight=height1=TerSect1[sect].PgHeight;
    if (TotalPreviewPages>1) {
       if ((PageNo+1)<TotalPages) sect=TerGetPageSect(hTerWnd,PageNo+1);
       width2=TerSect1[sect].PgWidth;
       height2=TerSect1[sect].PgHeight;
       PgWidth+=(float)(PV_PAGE_DIST+width2);
       if (height2>height1) PgHeight=height2;
    }

    // clear the background if number or sizes of pages change
    if (TRUE || PgWidth!=PvWidth || PgHeight!=PvHeight) {
        HBRUSH hBr;
        SetTerWindowOrg(w);               // set the window origin to the current TerWinOrgX and TerWinOrgY

        hOldPen=SelectObject(hDC,GetStockObject(NULL_PEN));

        hBr=CreateSolidBrush(TextDefBkColor);
        hOldBrush=SelectObject(hDC,hBr);
        Rectangle(hDC,TerWinOrgX,TerWinOrgY,TerWinOrgX+TerWinWidth+1,TerWinOrgY+TerWinHeight+1);
        
        SelectObject(hDC,hOldPen);
        SelectObject(hDC,hOldBrush);
        DeleteObject(hBr);

        PvWidth=PgWidth;
        PvHeight=PgHeight;
    }


    // create the print rectangles
    if (PreviewZoom<=0) {                            // best fit
       ScrPgWidth=(int)((TerWinHeight*PgWidth)/PgHeight);// display TerWinWidth
       if (TerWinWidth>ScrPgWidth) {
          ScrPgHeight=TerWinHeight;
          x=(TerWinWidth-ScrPgWidth)/2;              // center horizontally
          y=0;
       }
       else {
          ScrPgHeight=(int)((TerWinWidth*PgHeight)/PgWidth);// display TerWinWidth
          ScrPgWidth=TerWinWidth;
          x=0;
          y=(TerWinHeight-ScrPgHeight)/2;            // center vertically
          if (y<0) y=0;
       }
       TerWinOrgX=TerWinOrgY=0;

       if (TerOpFlags&TOFLAG_FULL_PAINT) RecreateFonts(w,hPr); // win dim may have changed, empty the windows internal font cache
    }
    else {    // use the specified zoom value
       ScrPgWidth=(int)(((ScrResX*PgWidth)*PreviewZoom)/100);
       ScrPgHeight=(int)(((ScrResY*PgHeight)*PreviewZoom)/100);
       x=y=0;
       if (TerWinWidth>ScrPgWidth) {
           x=(TerWinWidth-ScrPgWidth)/2;    // center horizontally
           TerWinOrgX=0;
       }
       if (TerWinHeight>ScrPgHeight) {
           y=(TerWinHeight-ScrPgHeight)/2;  // center horizontally
           TerWinOrgY=0;
       }
    }

    // Save the Zoom height and width for scrolling
    PreviewHeight=ScrPgHeight;
    PreviewWidth=ScrPgWidth;
    if (PreviewWidth <= 0 || PreviewHeight<=0) return FALSE; 

    // calculate the pixel distance between pages
    PageDist=(int)(((float)PV_PAGE_DIST*ScrPgWidth)/PgWidth);

    // build and display the pages
    for (i=0;i<TotalPreviewPages;i++,PageNo++) {    // build the pages
        // get the pixel width and height of the current page
        if (i==0) {
           width=width1;
           height=height1;
        }
        else {
           width=width2;
           height=height2;
        }

        CurPgWidth=(int)((width*ScrPgWidth)/PgWidth);
        CurPgHeight=(int)((height*ScrPgHeight)/PgHeight);

        rect.left=x;
        rect.right=rect.left+CurPgWidth-1;          // leave a pixel for shadow
        rect.top=y;
        rect.bottom=rect.top+CurPgHeight-1;

        // draw the rectangle
        TerSetClipRgn(w);

        // switch to screen units
        SaveId=SaveDC(hDC);                 // save the current status of the printer device context

        TempOrgY=MulDiv(CurPgHeight,TerWinOrgY,(int)(height*PrtResY));
        if (TerWinHeight>CurPgHeight) TempOrgY=0;
        TempOrgX=MulDiv(CurPgWidth,TerWinOrgX,(int)(width*PrtResX));
        if (TerWinWidth>PreviewWidth) TempOrgX=0;
        SetWindowOrgEx(hDC,TempOrgX,TempOrgY,NULL);   // set the logical window origin

        // create the blank space between the previous page and current page
        if (i>0) {
           HBRUSH hBr;
           
           rect1=rect;
           rect1.right=rect.left;
           rect1.left=rect1.right-PageDist;
           
           hOldPen=SelectObject(hDC,GetStockObject(NULL_PEN));
           hBr=CreateSolidBrush(TextDefBkColor);
           hOldBrush=SelectObject(hDC,hBr);
           
           Rectangle(hDC,rect1.left,y,rect1.right,y+PreviewHeight-1);  // draw the frame
           
           SelectObject(hDC,hOldPen);
           SelectObject(hDC,hOldBrush);
           DeleteObject(hBr);
        }

        // create the blank rectangle for the page
        hOldPen=SelectObject(hDC,GetStockObject(BLACK_PEN));
        hOldBrush=SelectObject(hDC,GetStockObject(WHITE_BRUSH));

        // WinNt Fix - need to have the following ExtTextOut between
        // two rectangle filling using a white brush and a non-null pen
        lstrcpy(TempString," ");
        ExtTextOut(hDC,rect.left,rect.top,ETO_CLIPPED,&rect,TempString,1,NULL);

        Rectangle(hDC,rect.left,rect.top,rect.right,rect.bottom);  // draw the frame

        MoveToEx(hDC,rect.right,rect.top+1,NULL); // draw right shadow
        LineTo(hDC,rect.right,rect.bottom);
        MoveToEx(hDC,rect.left+1,rect.bottom,NULL); // draw bottom shadow
        LineTo(hDC,rect.right,rect.bottom);

        SelectObject(hDC,hOldPen);
        SelectObject(hDC,hOldBrush);

        RestoreDC(hDC,SaveId);

        // display the page
        rect.left+=TerWinRect.left;     // specify rectangle relative to the window top
        rect.right+=TerWinRect.left;
        rect.top+=TerWinRect.top;
        rect.bottom+=TerWinRect.top;

        //rgn=CreateRectRgn(rect.left,rect.top,rect.right-1,rect.bottom);
        //SelectClipRgn(hDC,rgn);
        //DeleteObject(rgn);

        PreviewOnePage(w,hDC,&rect,PageNo,TRUE); // preview the current page

        TerResetClipRgn(w);

        // set x for the next page
        x+=(CurPgWidth+PageDist);             // x for the next page
    }
    if (TerArg.PrintView) CurPage=SaveCurPage; // restoring from SaveCurPage to get CurPage even when CurLine is in header/footer

    // **************************************
    if (hBufDC) {
       BitBlt(hTerDC,TerWinOrgX,TerWinOrgY,TerWinWidth,TerWinHeight,hDC,TerWinOrgX,TerWinOrgY,SRCCOPY);
    } 


    // display the page number
    if (hPvToolBarWnd) {
       if (TotalPreviewPages==1 || (CurPage+1)==TotalPages)
            wsprintf(TempString,"%s%d",MsgString[MSG_PP_PG],CurPage+1);
       else wsprintf(TempString,"%s%d/%d",MsgString[MSG_PP_PG],CurPage+1,CurPage+2);
       SetDlgItemText(hPvToolBarWnd,IDC_PV_PAGE_NO,TempString);
    }

    // set vertical scroll bar
    PrtPageHeight=(int)(PgHeight*PrtResY);      // height of the page
    PrtWinHeight=MulDiv(TerWinHeight,PrtPageHeight,PreviewHeight);

    if (TerArg.ShowVerBar) {
       int ScrollRange=SCROLL_RANGE-VerThumbSize;
       
       TotalPixLength=(long)PrtPageHeight*TotalPages - (long)PrtWinHeight;
       if (TotalPixLength>0) {
          CurPixLength=(long)PrtPageHeight*CurPage + TerWinOrgY;
          ScrollPos=(int)(CurPixLength*ScrollRange/TotalPixLength);
          if (CurPixLength>=TotalPixLength && CurPixLength>0) ScrollPos=ScrollRange;
          if (ScrollPos<0) ScrollPos=0;
       }
       else ScrollPos=0;

       if (ScrollPos!=VerScrollPos) { // redraw
          VerScrollPos=ScrollPos;
          SetScrollPos(hTerWnd,SB_VERT,VerScrollPos,TRUE);
       }
    }

    // set horizontal scroll bar
    PrtPageWidth=(int)(PrtResX*width1);
    if (TotalPreviewPages>1) PrtPageWidth+=(int)(PrtResX*(width2+(float)PV_PAGE_DIST));
    PrtWinWidth=MulDiv(TerWinWidth,PrtPageWidth,PreviewWidth);

    if (TerArg.ShowHorBar) {
       TotalPixLength=PrtPageWidth - (long)PrtWinWidth;
       if (TotalPixLength>0) {
          CurPixLength=TerWinOrgX;
          ScrollPos=(int)(CurPixLength*SCROLL_RANGE/TotalPixLength);
          if (CurPixLength>=TotalPixLength && CurPixLength>0) ScrollPos=SCROLL_RANGE;
          if (ScrollPos<0) ScrollPos=0;
       }
       else ScrollPos=0;

       #if defined(WIN32)
       {
           SCROLLINFO ScrollInfo;

           int ThumbSize=(int)((long)TerWinWidth*SCROLL_RANGE/(PreviewWidth));
           if (ThumbSize<MinThumbHt) ThumbSize=MinThumbHt;
           if (ThumbSize>SCROLL_RANGE) ThumbSize=SCROLL_RANGE;

           FarMemSet(&ScrollInfo,0,sizeof(ScrollInfo));
           ScrollInfo.cbSize=sizeof(ScrollInfo);
           ScrollInfo.fMask=SIF_POS|SIF_DISABLENOSCROLL;
           if (ThumbSize>0) ScrollInfo.fMask|=SIF_PAGE;
           ScrollInfo.nPage=ThumbSize+1;    // add 1 so that nPage can go beyond SCROLL_RANGE to disable the scroll bar
           ScrollInfo.nPos=ScrollPos;
           SetScrollInfo(hTerWnd,SB_HORZ,&ScrollInfo,TRUE);
           HorThumbSize=ThumbSize;
       }
       #else
           if (ScrollPos!=HorScrollPos) SetScrollPos(hTerWnd,SB_HORZ,HorScrollPos,TRUE);
       #endif

       HorScrollPos=ScrollPos;
    }

    return TRUE;
}

/******************************************************************************
    PreviewOnePage:
    Display a page in the given rectangle
*******************************************************************************/
PreviewOnePage(PTERWND w, HDC hDC, RECT far *rect, int page, BOOL scale)
{
    int        ColBeg,ColEnd,ColLength,PageNo,sect;
    long       lin,FirstPageLine;
    int        PagePixWidth,PagePixHeight,BeginX,BeginY,NextY,LastY,
               LineHeight,BaseHeight,ExtLead,SaveId;
    LPBYTE     ptr,lead;
    LPWORD     fmt;
    HDC        hSavePr=NULL;
    BOOL       paginate=FALSE;

    // switch to the print preview DC
    SaveId=SaveDC(hDC);                 // save the current status of the printer device context

    if (scale) {
      SetMapMode(hDC,MM_ANISOTROPIC);        // use customized units
      sect=TerGetPageSect(hTerWnd,page);

      SetViewportOrgEx(hDC,rect->left,rect->top,NULL);  // set view port origins
      SetWindowOrgEx(hDC,TerWinOrgX,TerWinOrgY,NULL);   // set the logical window origin
      SetWindowExtEx(hDC,(int)(TerSect1[sect].PgWidth*PrtResX),(int)(TerSect1[sect].PgHeight*PrtResY),NULL); // logical dimension
      SetViewportExtEx(hDC,rect->right-rect->left,rect->bottom-rect->top,NULL); // equivalent device dimension
    }

    hSavePr=hPr;
    hPr=hDC;

    // Actual printing function
    if (TerArg.WordWrap) PrintViewMode(w,hPr,-1,page,1,0);
    else {                             // print the text mode document
        CalcPageDim(w,0,&PagePixWidth,&PagePixHeight,&BeginX,&BeginY,&LastY); // calculate printing area dimensions
        NextY=BeginY;
        FirstPageLine=0;
        if (TotalPages==0) paginate=TRUE;  // create page line table
        if (TotalPages>0) {
           if (page>=TotalPages) goto END;  // no more pages
           FirstPageLine=PageInfo[page].FirstLine;
           PageNo=page;                  // reset the total page count
        }
        else {
           page=-1;                      // go by current line
           TotalPages=1;
           PageInfo[0].FirstLine=0;      // initialize page line table
           ResetUintFlag(PageInfo[0].flags,PAGE_HARD);
           PageNo=0;                     // start from the first page
           paginate=TRUE;
        }

        for (lin=FirstPageLine;lin<TotalLines;lin++) {
            if (page<0 && lin==CurLine)  {// page to print not yet selected
               page=CurPage=PageNo;       // page found
               lin=FirstPageLine;         // restart this page
               NextY=BeginY;
            }

            LineHeight=GetLineHeight(w,lin,&BaseHeight,&ExtLead);

            if (LineLen(lin)==0) {        // blank line
               if ((NextY+LineHeight)>LastY) {
                  PageNo++;
                  if (PageNo>=MAX_PAGES) goto END;

                  CheckPageSpace(w,PageNo);  // ensure that page structure is large enougn

                  PageInfo[PageNo].FirstLine=lin; // build the page table
                  ResetUintFlag(PageInfo[PageNo].flags,PAGE_HARD);

                  if (PageNo>page && page>=0 && !paginate) break;
                  NextY=BeginY;
                  FirstPageLine=lin;      // first line of the next page
               }
               NextY+=LineHeight;
               continue;
            }

            ColBeg=0;    // determine the part of line to print
            ColEnd=LineLen(lin)-1;
            if (ColEnd<ColBeg) ColEnd=ColBeg;

            ptr=pText(lin);
            lead=pLead(lin);
            fmt=OpenCfmt(w,lin);
            while (ColBeg<=ColEnd) {
               ColLength=GetLineLen(w,ptr,fmt,lead,ColBeg,ColEnd,PagePixWidth);
               if ((ColBeg+ColLength-1)>ColEnd) ColLength=ColEnd-ColBeg+1;

               if ((NextY+LineHeight)>LastY) {
                  PageNo++;
                  if (PageNo>=MAX_PAGES) goto END;
                  CheckPageSpace(w,PageNo);

                  PageInfo[PageNo].FirstLine=lin; // build the page table
                  ResetUintFlag(PageInfo[PageNo].flags,PAGE_HARD);

                  if (PageNo>page && page>=0 && !paginate) break;
                  NextY=BeginY;
                  FirstPageLine=lin;      // first line of the next page
               }
               // print the line
               if (ColLength==1 && ptr[ColBeg]==PAGE_CHAR) NextY=LastY; // force page break
               else if (PageNo==page && page>=0) PrintNonWrapLine(w,lin,hPrtDataDC,hPr,BeginX,NextY,&ptr[ColBeg],&fmt[ColBeg],LeadPtr(lead,ColBeg),ColLength,LineHeight,BaseHeight,NULL,pfmt(lin),0,0,ExtLead,0,0,NULL);

               NextY+=LineHeight;
               ColBeg+=ColLength;
            }

            if (PageNo>page && page>=0 && !paginate) break;


            CloseCfmt(w,lin);
        }

        if ((PageNo+1)>TotalPages || paginate) TotalPages=PageNo+1;
    }

    END:

    if (hSavePr) hPr=hSavePr;        // switch back to the actual printer

    RestoreDC(hDC,SaveId);

    return TRUE;
}

/******************************************************************************
    PreviewDown:
    Do a page down and line down in preview mode
*******************************************************************************/
PreviewDown(PTERWND w, BOOL page)
{
    int  PrtWinHeight,PrtPageHeight;

    // convert to printer resolutions
    PrtPageHeight=(int)(PvHeight*PrtResY);      // height of the page
    PrtWinHeight=MulDiv(TerWinHeight,PrtPageHeight,PreviewHeight);

    // do we go to the next page now?
    if ((TerWinOrgY+PrtWinHeight)>=PrtPageHeight) {
       if ((CurPage+1)<TotalPages) {
          CurPage++;
          CurLine=PageInfo[CurPage].FirstLine;
          CurCol=0;
          TerWinOrgY=0;
       }
       else return TRUE;
    }
    else {       // scroll down by a fixed amount
       if (page) TerWinOrgY+=PrtWinHeight;
       else      TerWinOrgY+=(PrtWinHeight/6);
       if ((TerWinOrgY+PrtWinHeight)>PrtPageHeight) TerWinOrgY=PrtPageHeight-PrtWinHeight;
    }

    PaintPrintPreview(w);

    return TRUE;
}

/******************************************************************************
    PreviewUp:
    Do a page up and line up in preview mode
*******************************************************************************/
PreviewUp(PTERWND w, BOOL page)
{
    int  PrtWinHeight,PrtPageHeight;

    // convert to printer resolutions
    PrtPageHeight=(int)(PvHeight*PrtResY);      // height of the page
    PrtWinHeight=MulDiv(TerWinHeight,PrtPageHeight,PreviewHeight);

    // do we go to the next page now?
    if (TerWinOrgY==0) {
       if (CurPage>0) {
          CurPage--;
          CurLine=PageInfo[CurPage].FirstLine;
          CurCol=0;
          TerWinOrgY=PrtPageHeight-PrtWinHeight;
          if (TerWinOrgY<0) TerWinOrgY=0;
       }
       else return TRUE;
    }
    else {       // scroll down by a fixed amount
       if (page) TerWinOrgY-=PrtWinHeight;
       else      TerWinOrgY-=(PrtWinHeight/6);
       if (TerWinOrgY<0) TerWinOrgY=0;
    }

    PaintPrintPreview(w);

    return TRUE;
}

/******************************************************************************
    PreviewRight:
    Do a page right in preview mode
*******************************************************************************/
PreviewRight(PTERWND w, BOOL page)
{
    int  PrtWinWidth,PrtPageWidth;

    // convert to printer resolutions
    PrtPageWidth=(int)(PrtResX*PvWidth);
    PrtWinWidth=MulDiv(TerWinWidth,PrtPageWidth,PreviewWidth);

    // do we go to the next page now?
    if ((TerWinOrgX+PrtWinWidth)>=PrtPageWidth) return TRUE;
    else {       // scroll down by a fixed amount
       if (page) TerWinOrgX+=PrtWinWidth;
       else      TerWinOrgX+=(PrtWinWidth/6);
       if ((TerWinOrgX+PrtWinWidth)>PrtPageWidth) TerWinOrgX=PrtPageWidth-PrtWinWidth;
    }

    PaintPrintPreview(w);

    return TRUE;
}

/******************************************************************************
    PreviewLeft:
    Do a page left in preview mode
*******************************************************************************/
PreviewLeft(PTERWND w, BOOL page)
{
    int  PrtWinWidth,PrtPageWidth;

    // convert to printer resolutions
    PrtPageWidth=(int)(PrtResX*PvWidth);
    PrtWinWidth=MulDiv(TerWinWidth,PrtPageWidth,PreviewWidth);

    // do we go to the next page now?
    if (TerWinOrgX<=0) return TRUE;
    else {       // scroll down by a fixed amount
       if (page) TerWinOrgX-=PrtWinWidth;
       else      TerWinOrgX-=(PrtWinWidth/6);
       if (TerWinOrgX<0) TerWinOrgX=0;
    }

    PaintPrintPreview(w);

    return TRUE;
}

/******************************************************************************
   PreviewPageVert:
   Position at the specified vertical thumbtrack postion.
*******************************************************************************/
PreviewPageVert(PTERWND w,int pos)
{
   int NewPage;
   long TotalY,PageAbsY,PageY,height,CurHeight,SavePage;
   int ScrollRange=SCROLL_RANGE-VerThumbSize;
   int  PrtWinHeight,PrtPageHeight;

   SavePage=CurPage;

   // convert to printer resolutions
   PrtPageHeight=(int)(PvHeight*PrtResY);      // height of the page
   PrtWinHeight=MulDiv(TerWinHeight,PrtPageHeight,PreviewHeight);


   TotalY=PrtPageHeight*TotalPages;

   if (pos>=(ScrollRange-2)) {
      NewPage=TotalPages-1;
      PageY=PrtPageHeight-PrtWinHeight;
   }
   else { 
      PageAbsY=(TotalY*pos)/ScrollRange;
      height=0;
      
      for (NewPage=0;NewPage<(TotalPages-1);NewPage++) {
         CurHeight=PrtPageHeight;
         if ((height+CurHeight)>PageAbsY) break;
         height+=CurHeight;
      }
      PageY=PageAbsY-height;   // Y offset at the current page
   }

   if (PageY>(PrtPageHeight-PrtWinHeight)) PageY=PrtPageHeight-PrtWinHeight;
   if (PageY<0) PageY=0;
   
   CurPage=NewPage;
   if (CurPage!=SavePage) {
      CurLine=PageInfo[CurPage].FirstLine;
      CurCol=0;
   }

   TerWinOrgY=PageY;

   PaintPrintPreview(w);

   return TRUE;
}

/******************************************************************************
    PrintNonWrapLine:
    Print a line using appropriate fonts.
    The argument specify the pixel coordinates of the text, text pointer,
    font pointer, length of the text to print, the line height, and the
    height of the base line.
******************************************************************************/
PrintNonWrapLine(PTERWND w,long LineNo, HDC hPrtDC,HDC hAttrDC, int BeginX,int BeginY,
             LPBYTE ptr,LPWORD fmt,LPBYTE lead,int len,int height, int BaseHeight,
             struct StrTabw far *CurTabw,int CurPara,int SpcBef, int SpcAft,int ExtLead, 
             int FrameNo,int LeftMargin, LPWORD pWidth)
{
    WORD PrevFont,CurFont;
    BYTE PrevChar=0,PrevLead=0;
    int  TabNo=0,TabId,BaseY;
    int CurLen,TextLen,i,CurPixLen=0,PrintedLen,width;
    BOOL justify=FALSE,IsFramePict;
    int  JustAdj=0,SpaceNo=0,BoxAdj=0,BoxBeginX=-1,BoxEndX=0,TabX;
    RECT rect;
    HBRUSH hBr;
    WORD  CurLineWidth[MAX_WIDTH+1];

    if (len==0) goto PRINT_BORDER;            // nothing to print

    PrevFont=CurFont=0;
    CurLen=CurPixLen=PrintedLen=0;
    if (CurTabw && CurTabw->type&INFO_JUST) justify=TRUE;

    for (i=0;i<=len;i++) {
       if (i<len) CurFont=fmt[i];

       if (   i==len || CurFont!=PrevFont || (PrevChar==TAB && TerFont[PrevFont].height!=0) 
          || PrevChar==HYPH_CHAR || PrevChar==HIDDEN_CHAR
          || (justify && ptr[i]==' ' && JustifySpace(w,PrevFont))
          || (i>0 && CurTabw && CurTabw->type&INFO_FRAME && CurTabw->FrameCharPos==i) ) {// respond to format change or tab character

          TextLen=CurLen;                      // number of characters to print
          if (PrevChar==TAB) TextLen--;
          if (i==len && PrevChar==HYPH_CHAR) PrevChar='-';  // convert to regular dash
          if (PrevChar==HYPH_CHAR || PrevChar==HIDDEN_CHAR) TextLen--;

          // get extra space for justification
          if (justify && ptr[PrintedLen]==' ' && JustifySpace(w,PrevFont)) {
             if ( SpaceNo>=CurTabw->JustSpaceIgnore
               && SpaceNo<CurTabw->JustSpaceCount) {
                JustAdj=CurTabw->JustAdj;
                if ((SpaceNo-CurTabw->JustSpaceIgnore)<CurTabw->JustCount) JustAdj++;
             }
             SpaceNo++;
          }
          else JustAdj=0;

          if (TextLen>0) {
             // draw the background rectangle
             PrtTextBkColor=TerFont[PrevFont].TextBkColor;
             if (TerFont[PrevFont].TextBkColor!=CLR_WHITE) {
                 rect.left=BeginX;
                 rect.right=rect.left+CurPixLen+JustAdj;
                 rect.top=BeginY+SpcBef;
                 rect.bottom=rect.top+height;
                 hBr=CreateSolidBrush(TerFont[PrevFont].TextBkColor);
                 FillRect(hPrtDC,&rect,hBr);
                 DeleteObject(hBr);
             }

             // print this line segment
             IsFramePict=(TerFont[PrevFont].style&PICT && TerFont[PrevFont].ParaFID>0);
             if (!IsFramePict) PrintOneSegment(w,hPrtDC,hAttrDC,PrevFont,BeginX+JustAdj+BoxAdj,BeginY+SpcBef,&ptr[PrintedLen],LeadPtr(lead,PrintedLen),TextLen,BaseHeight,ExtLead,JustAdj,&CurLineWidth[PrintedLen],NULL);

             // draw the character box rectangle
             if (BoxEndX>0) {
                 int  delta=PrtResY/ScrResY;  // one screen pixel width

                 DrawShadowLine(w,hPrtDC,BoxBeginX+delta,BeginY+delta,BoxEndX-delta,BeginY+delta,GetStockObject(BLACK_PEN),NULL); //top line
                 DrawShadowLine(w,hPrtDC,BoxBeginX+delta,BeginY+height-2*delta,BoxEndX-delta,BeginY+height-2*delta,GetStockObject(BLACK_PEN),NULL); //bottom line
                 DrawShadowLine(w,hPrtDC,BoxBeginX+delta,BeginY+delta,BoxBeginX+delta,BeginY+height-2*delta,GetStockObject(BLACK_PEN),NULL); //left line
                 DrawShadowLine(w,hPrtDC,BoxEndX-delta,BeginY+delta,BoxEndX-delta,BeginY+height-2*delta,GetStockObject(BLACK_PEN),NULL); //right line
                 BoxBeginX=-1;                 // reset
                 BoxEndX=0;                   // reset the box attribute
             }

          }

          PrintedLen+=CurLen;
          BeginX+=(CurPixLen+JustAdj);

          // insert any frame reserved space
          if (i>0 && CurTabw && CurTabw->type&INFO_FRAME && CurTabw->FrameCharPos==i) BeginX+=ScrToPrtX(CurTabw->FrameScrWidth);

          if (PrevChar==TAB) {                  // add the tab width
             width=GetPrtTabWidth(w,BeginX,TabNo,CurTabw);
             if (TerFont[PrevFont].TextBkColor!=CLR_WHITE) {  // print any shading for the tab
                 rect.left=BeginX;
                 rect.right=rect.left+width;
                 rect.top=BeginY+SpcBef;
                 rect.bottom=rect.top+height;
                 hBr=CreateSolidBrush(TerFont[PrevFont].TextBkColor);
                 FillRect(hPrtDC,&rect,hBr);
                 DeleteObject(hBr);
             }

             // draw dot leaders and underlines
             TabId=PfmtId[CurPara].TabId;
             TabX=BeginX;
             if (TerArg.PageMode) TabX-=(LeftMargin+frame[FrameNo].x+frame[FrameNo].SpaceLeft);  // the entire frame set is created relative to the left margin
             GetTabPos(w,pfmt(LineNo),&TerTab[TabId],TabX,NULL,NULL,&TabFlags,FALSE);
             
             BaseY=BeginY+SpcBef+BaseHeight+TerFont[CurFont].BaseHeightAdj+ExtLead;

             if (TabFlags==TAB_DOT)
                DrawDottedLine(w,hPrtDC,BeginX,BaseY,BeginX+width, BaseY,0);
             else if (TabFlags==TAB_HYPH) {
                HPEN hPen=CreatePen(PS_DOT,0,0);
                DrawShadowLine(w,hPrtDC,BeginX,BaseY-2,BeginX+width,BaseY-2,hPen,NULL);
                DeleteObject(hPen);
             }
             else if (TabFlags==TAB_ULINE || TerFont[PrevFont].style&ULINE) {
                HPEN hPen;
                int thickness=PointsToPrtY(1)/2;
                if (TerFont[PrevFont].style&BOLD) thickness=thickness*2;
                if (thickness<1) thickness=1;
                if (NULL!=(hPen=CreatePen(PS_SOLID,thickness,0))) {
                   DrawShadowLine(w,hPrtDC,BeginX,BaseY+2*thickness,BeginX+width,BaseY+2*thickness,hPen,NULL);
                   DeleteObject(hPen);
                }
                //DrawShadowLine(w,hPrtDC,BeginX,BaseY+2,BeginX+width,BaseY+2,GetStockObject(BLACK_PEN),NULL);
             }

             TabFlags=0;                       // reset the global variables

             BeginX=BeginX+width;
             TabNo++;
          }
          CurLen=CurPixLen=0;
          BoxAdj=0;
          CharBoxLeft=CharBoxRight=FALSE;

          PrevFont=CurFont;

       }

       if (i==len) break;                   // end of the line

       // update the line pixel width
       PrevChar=ptr[PrintedLen+CurLen];
       PrevLead=LeadByte(lead,PrintedLen+CurLen);

       if (PrevChar!=TAB) {
          width=0;   // current character width

          // add extra character spaces before the character
          if (CurTabw && (PrintedLen+CurLen)<CurTabw->CharFlagsLen && CurTabw->CharFlags[PrintedLen+CurLen]&BFLAG_BOX_FIRST) {
              BoxBeginX=BeginX+CurPixLen;
              width+=ExtraSpacePrtX;
              if (CurLen==0) BoxAdj=ExtraSpacePrtX;  // adjustment to print x position
          }

          // add actual character width
          IsFramePict=(TerFont[CurFont].style&PICT && TerFont[CurFont].ParaFID>0);
          if (!IsFramePict) {
             BYTE chr=PrevChar;
             if (TerFont[CurFont].UcBase==0 && TerFont[CurFont].style&(SCAPS|CAPS) && PrevLead==0
                 && IsLcChar(chr)) chr+=(BYTE)('A'-'a');
             
             if (pWidth) width=pWidth[i];         // character width supplied
             else if (pCharWidth(LineNo) && CWidth(LineNo,i)!=-1) 
                   width+=CWidth(LineNo,i);  // context-sensitive width
             else  width+=DblCharWidth(w,CurFont,TRUE,chr,PrevLead);
          }
          
          // add extra character spaces after the character
          if ((CurTabw && (PrintedLen+CurLen)<CurTabw->CharFlagsLen && CurTabw->CharFlags[PrintedLen+CurLen]&BFLAG_BOX_LAST)
              || (i==(len-1) && BoxBeginX!=-1)) {
              CurPixLen+=ExtraSpacePrtX;
              BoxEndX=BeginX+CurPixLen;
          }
          CurPixLen+=width;
          CurLineWidth[i]=width;
       }

       CurLen++;
    }

    PRINT_BORDER:
    // Draw paragraph border
    if (TextBorder&PARA_BOX) {
       rect.left=BoxLeft;
       rect.right=BoxRight;   // enought to cover the right side

       if (LineNo && cid(LineNo)==0 && fid(LineNo)==0) {
          rect.left-=PointsToPrtX(20);  // enough to cover the right side
          rect.right+=PointsToPrtX(20);
       } 
       if (LineNo && cid(LineNo)>0) {
          rect.right-=PointsToPrtX(1);  // try to bring it within the cell border
       }
        
       rect.top=BeginY+ExtLead;
       rect.bottom=BeginY/*+ExtLead*/+height+SpcBef+SpcAft;    // ExtLead causes overlap with next line with HP 4 printer on WIN NT

       DrawParaBorder(w,hPrtDC,&rect,TRUE);

       TextBorder=ParaShading=0;
    }

    CharBoxLeft=CharBoxRight=FALSE;     // reset character box flags

    return TRUE;
}

