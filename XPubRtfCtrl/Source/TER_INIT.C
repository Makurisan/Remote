/*==============================================================================
   TER_INIT.C
   TER initialization and exit functions.

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1991)
   ----------------------------------
   This license agreement allows the purchaserp[he right to modify the
   source code to incorporate in an application larger than the editor itself.
   The target application must not be a programmer's utility 'like' a text editor.
   Sub Systems, Inc. reserves the right to prosecute anybody found to be
   making illegal copies of the executable software or compiling the source
   code for the purpose of selling there after.
===============================================================================*/

#define  PREFIX extern
#include "ter_hdr.h"

/*****************************************************************************
   InitTer:
     a. Construct a window class for TER and create a window.
     b. Initialize pointer and line length arrays.
     c. Read the input file or parse the input buffer.
     d. Initialize cursor, etc.

     The argument specifies the parameter passed by the WM_CREATE message.
*****************************************************************************/
BOOL InitTer (HWND hWnd, CREATESTRUCT far *pWin)
{
    PTERWND w;
    struct arg_list arg;                    // input parameter structure
    struct arg_list far *ArgPtr;
    BYTE class[20];
    int  i;

    TerOpenCount++;                         // increment number of open windows

    // create the window context structure
    if (NULL==(w=(PTERWND)OurAlloc(sizeof(struct StrTer)))) return FALSE;
    FarMemSet(w,0,sizeof(struct StrTer));

    GetClassName(hWnd,class,sizeof(class)-1);
    if (lstrcmpi(class,TER_CLASS)==0) {
       SetWindowLong(hWnd,0,(LONG)w);  // store it with the window
       VbxControl=FALSE;               // standard TER window
    }
    else {                             // store it in WinPtrs array
       for (i=0;i<TotalWinPtrs;i++) if (!(WinPtr[i].InUse)) break;
       if (i==TotalWinPtrs) {
          if (TotalWinPtrs>=MAX_WIN_PTRS) return PrintError(w,MSG_OUT_OF_WIN_SLOTS,"InitTer");
          TotalWinPtrs++;
       }
       WinPtr[i].InUse=TRUE;
       WinPtr[i].hWnd=hWnd;
       WinPtr[i].data=w;
       VbxControl=TRUE;
    }

    // Copy or create the user argument structure
    if (pWin->style&TER_USE_PARAMS) ArgPtr=pWin->lpCreateParams;  // The window created by the DLL
    else{  // The window created by your application or the dialog manager
        CreateUserParam(&arg,pWin);          // create the parameter structure
        ArgPtr=&arg;
    }

    TerArg=(*ArgPtr);                        // keep a copy of argument structure
    if (TerArg.FontTypeFace[0]==0) InitTypeface(w);  // set the a default font
    if (TerStyles) OverrideStyles(w);

    if (!TerArg.WordWrap) TerArg.PrintView=TerArg.PageMode=TerArg.FittedView=FALSE;
    if (TerArg.FittedView) TerArg.PageMode=TRUE;  // implies also the page mode
    if (TerArg.PageMode)  TerArg.PrintView=TRUE; // Print view implied
    if (TerArg.PageMode && !(TerArg.FittedView)) PagesShowing=TRUE;
    else                                         PagesShowing=FALSE;
    TerArg.hTextWnd=hWnd;                    // pass the window handle to the user structure

    w->hMpDC=(HDC)TerArg.modified;
    TerArg.modified=0;

    //LogPrintf("%x InitTer1",w->hMpDC);

    // Initialize variable
    if (!InitVariables(w)) return FALSE;     // initialize the global variables
    hTerWnd=hWnd;

    if (!AllocArrayMem(w)) return FALSE;     // allocate memory for line data arrays
    InitLine(w,0);                           // initialize the first line
    pfmt(0)=0;
    FarMemSet(&(PageInfo[0]),0,sizeof(struct StrPage));
    PageInfo[0].FirstLine=0;                 // line number where first page begins
    PageInfo[0].DispNbr=1;                   // first page number for display
    PageInfo[0].ScrHt=100;                   // some initial non-zero number
    PageInfo[0].flags=PAGE_FIRST_SECT_PAGE;  // first section page

    // create the default section
    TotalSects=1;
    InitSect(w,0);              // initialize the first section

    // create the default tab, para id
    TotalTabs=1;
    TerTab[0].count=0;
    TotalPfmts=1;
    FarMemSet(&(PfmtId[0]),0,sizeof(struct StrPfmt));
    PfmtId[0].flags=LEFT;
    PfmtId[0].BkColor=CLR_WHITE;
    PfmtId[0].BorderColor=CLR_AUTO;

    // create the default bullet structure
    TotalBlts=1;
    FarMemSet(&(TerBlt[0]),0,sizeof(struct StrBlt));
    TerBlt[0].IsBullet=TRUE;
    TerBlt[0].BulletChar=BULLET_CHAR;
    TerBlt[0].font=BFONT_SYMBOL;            // symbol

    // create the first list table
    TotalLists=1;
    FarMemSet(&(list[0]),0,sizeof(struct StrList));
    list[0].InUse=TRUE;

    TotalListOr=1;                         //number of overrides
    FarMemSet(&(ListOr[0]),0,sizeof(struct StrListOr));
    ListOr[0].InUse=TRUE;
     

    // create the default text frame
    TotalFrames=1;
    InitFrame(w,0);                          // initialize the first frame

    // create default table row/cell
    FarMemSet(&(TableRow[0]),0,sizeof(struct StrTableRow));
    FarMemSet(&(TableAux[0]),0,sizeof(struct StrTableAux));
    TableRow[0].InUse=TRUE;
    TotalTableRows=1;
    FarMemSet(&(cell[0]),0,sizeof(struct StrCell));
    FarMemSet(&(CellAux[0]),0,sizeof(struct StrCellAux));
    cell[0].InUse=TRUE;
    TotalCells=1;

    // create the default paragraph frame table
    TotalParaFrames=1;
    FarMemSet(&(ParaFrame[0]),0,sizeof(struct StrParaFrame));
    ParaFrame[0].InUse=TRUE;

    // create first two default stylesheet styles
    TotalSID=2;
    FarMemSet(&(StyleId[0]),0,sizeof(struct StrStyleId));
    StyleId[0].InUse=TRUE;
    StyleId[0].type=SSTYPE_PARA;
    StyleId[0].TextBkColor=CLR_WHITE;
    StyleId[0].ParaBkColor=CLR_WHITE;
    StyleId[0].TwipsSize=TerArg.PointSize*20;
    StyleId[0].OutlineLevel=-1;
    lstrcpy(StyleId[0].TypeFace,TerArg.FontTypeFace);
    lstrcpy(StyleId[0].name,"Normal");

    FarMemSet(&(StyleId[1]),0,sizeof(struct StrStyleId));
    StyleId[1].InUse=TRUE;
    StyleId[1].type=SSTYPE_CHAR;
    StyleId[1].TextBkColor=CLR_WHITE;
    StyleId[1].ParaBkColor=CLR_WHITE;
    StyleId[1].OutlineLevel=-1;
    lstrcpy(StyleId[1].name,"Default Paragraph Font");

    // initialize the character tag table
    FarMemSet(&(CharTag[0]),0,sizeof(struct StrCharTag));
    TotalCharTags=1;
    CharTag[0].InUse=TRUE;

    // create the block file information block
    FarMemSet(&(FileInfo),0,sizeof(struct StrFileInfo));


    //***********************************************************************
    WindowBeingCreated=TRUE;               // avoid message processing during a window creation
    ValidateRect(hTerWnd,NULL);            // No need to paint right now
    if (!StSearched) SearchSpellTime(w);   // search for spelltime product
    if (!InitPrinter(w))  return FALSE;    // initialize the printer variables
    if (!InitWinProp(w))  return FALSE;    // initialize window properties
    if (!InitOle(w))      return FALSE;    // initialize the ole client structure
    if (TerArg.WordWrap)  AllocWrapBuf(w,0); // allocate the word warp buffer
    WindowBeingCreated=FALSE;              // window creating complete

    if (TerFlags&TFLAG_APPLY_PRT_PROPS && pDeviceMode)  // apply the printer orientation
       ApplyPrinterSetting(w,pDeviceMode->dmOrientation,pDeviceMode->dmDefaultSource, PaperSize, PageWidth, PageHeight);

    lstrcpy(DocName,TerArg.file);
    if (!TerRead(w,DocName)) return FALSE;

    if (TerArg.ShowHorBar || TerArg.ShowVerBar) SetScrollBars(w);

    TerArg.open=TRUE;                   // mark this window as open

    // update the input structure
    ArgPtr->hTextWnd=TerArg.hTextWnd;
    ArgPtr->open=TerArg.open;
   
    PostMessage(hTerWnd,TER_CREATED,0,0L);
    //SendMessageToParent(w,TER_CREATED,(WPARAM)hWnd,0L,FALSE);
 
    //LogPrintf("%x InitTer7",w->hMpDC);

    return TRUE;
}

/******************************************************************************
    InitVariables:
    Initialize the global variables.
*******************************************************************************/
BOOL InitVariables(PTERWND w)
{
    int i;

    mbcs          =InitMbcs; // initial value for mbcs

    CurRow        =0;     // current window row position
    CurCol        =0;     // current window column position
    CurLine       =0;     // current line number in the file
    CurLineY      =0;     // Y position of the current line
    BeginLine     =0;     // line number of the first window line
    TotalLines    =1;     // total lines in the file
    MaxLines      =INITIAL_MAX_LINES; // Initial line space to be allocated
    TerWinOrgX    =0;     // current horizontal window x co-ordinate in the logical space
    TerWinOrgY    =0;     // current vertical window y co-ordinate in the logical space
    CaretHeight   =0;     // current height of the caret
    PrevCursLine  =0;     // previous cursor line number
    PrevCursCol   =0;     // previous cursor column number
    PrevCursPage  =0;     // previous cursor page number
    PrevCursLineY =0;     // previous cursor Y position
    CursHorzPos   =-1;    // cursor horizontal position for up/down key positioning
    CursDirection = CURS_RESET; // cursor direction
    PaintFlag     =PAINT_WIN;  // paint the whole window by default
    WinYOffset    =0;     // Y offset of the first line
    WinYOffsetLine=-1;    // first line with offset
    hScrollBM     =NULL;  // stores used for scrolling
    hBkPictBM     =NULL;  // stores used for storing the background picture
    hBufBM        =NULL;  // used for buffered display
    hOrigBufBM    =NULL;  // used for buffered display
    hInitBkBM     =NULL;  // initial background bitmap
    hBufDC        =NULL;  // used for buffered display
    RepaintNo     =0;     // repaint number
    BorderSpill   =0;     // border spill in any direction
    transparent   =FALSE; // opaque writing to screen
    EnableTransparent =FALSE; // opaque writing to screen
    HtmlMode      =FALSE; // TRUE when HTML mode adjustments in effect
    TerFlags      =TerInitFlags;     // initialize flags TFLAG_
    TerFlags2     =TerInitFlags2;    // initialize flags TFLAG2_
    TerFlags3     =TerInitFlags3;    // initialize flags TFLAG2_
    TerFlags4     =TerInitFlags4;    // initialize flags TFLAG4_
    TerFlags5     =TerInitFlags5;    // initialize flags TFLAG5_
    TerFlags6     =TerInitFlags6;    // initialize flags TFLAG6_
    UseScrMetrics =True(TerFlags6&TFLAG6_HI_RES);
    TerOpFlags    =0;     // Internal operation flags
    LinePtrCount  =0;     // number of line pointers in the cache
    CurCtlId      =-1;    // currently selected control
    ScrYOffset    =0;     // offset to be added when writing to screen
    StatusBarHeight=0;    // height of the status bar
    DispTextHt=0;         // text height of the line being displayed
    GlbPrtDCId=0;         // global printer device context id
    hMdiClient    =NULL;  // MDI client when TER is embedded in an MDI environment
    hMdiChild     =NULL;  // MDI child when TER is embedded in an MDI environment
    MaxRtfColors  =200;   // allocate 200 colors initially
    MaxSects      =100;   // allocate space for 100 sections initially
    ExpandLineFmt =FALSE; // TRUE to temporarily expand the line formats
    DraggingText  =FALSE; // TRUE when dragging text
    InOleDrag     =FALSE; // TRUE when using ole to do dragging
    InDragDrop    =FALSE; // TRUE when dragging text from another application
    Clr256Display =FALSE; // TRUE when using a 256 color display
    hUserAccTable =NULL;   // user accelerator table
    HilightAtCurPos=FALSE;// highligt does not need to match exactly at the current pos
    RedrawBorder  =FALSE; // TRUE to redraw the background
    PrivatePrtDC  =TRUE;  // TRUE when the printer device context is private
    DrawBknd=TRUE;        // TRUE to draw background when doing the text output to the screen
    VerThumbSize  =0;     // size of vertical scrollbar thumb
    HorThumbSize  =0;     // size of horizontal scrollbar thumb
    MinThumbHt  =0;       // minimum size of the thumb in the SCROLL_RANGE page units
    LeadWaiting =0;       // Lead byte waiting to be processed
    pLineCharWidth=NULL;  // character width for the current line
    DefCellMargin=CELL_MARGIN;  // default cell margin
    ToolbarFlagText[0]=0; // toolbar flag text
    EvalTimerCount=0;     // eval timer count
    FormFieldSubclassProc=NULL;  // subclass process
    ViewKind=0;           // rtf view kind
    hInternet=NULL;       // internet handle
    FootnoteNumFmt=NBR_DEC; // default footnote number format
    EndnoteNumFmt=NBR_LWR_ROMAN;  // default endnote number format
    KnownSect=-1;         // known section for optimization
    ScrFrameAngle=0;       // frame angle
    FrameDistFromMargin=FRAME_DIST_FROM_MARGIN;
    DocTextFlow=FLOW_DEF;  // FLOW_ constants
    ScrRtl=FALSE;          // TRUE to apply rtl flow during screen painting
    pScrSeg=NULL;          // segment table pointer
    TotalScrSeg=0;         // total scr segments
    MousePictFrame=-1;     // mouse picture frame
    hCurCtlWnd=NULL;       // current control window handle
    CurCtlId=0;            // current control id
    DocName[0]=0;          // document name
    OverrideTotalPages=0;  // Override for the page count field
    TotalTlbCustIds=0;     // total custom toolbar icon ids
    PageHeightAdj=0;       // page height adjustment for a variable height page
    hMsgHook=null;         // message hook
    EnterHit=false;        // TRUE when the enter key is hit
    ProtectForm=false;     // TRUE to turn on protection
    WmParaFID=0;           // Watermark paraframe id
    InPreprocess=false;    // TRUE when in preprocess
    SectLine=-1;           // section line number to print
    ReqCharSet=DEFAULT_CHARSET;  // default requested character set
    ActiveOleObj=-1;       // active ole object
    InIE=false;            // hosted in IE?
    DeadChar=0;            // dead character
    MouseStopTimerOn=false;// TRUE mouse stop timer is on
    MouseStopMsgOn=false;  // TRUW when mouse stop message is displaying
    MouseStopDone=false;   // TRUW when mouse stop activity done for the current location
    MaxReviewers=MAX_REVIEWERS;  // initial max reviewers
    NoDropList[0]=0;       // list of file extensions that should not be dropped into the editor

    EventMask=0;           // Event mask used with standard EN_ messages
    hGetTextMem=null;      // text data for the previous WM_GETTEXT message
    GetTextLen=0;          // length of previous WM_GETTEXT data
    GetTextMod=0;          // modified count when the previous WM_GETTEXT data was provided

    InAutoComp=false;      // true when in auto-comp
    TotalAutoComps=0;      // number of auto-comp words

    TrackChanges=false;  
    TrackRev=0;            // current reviewer
    TrackTime=0;           // review start date/time

    CurMapPict=0;          // map picture
    CurMapId=0;
    CurMapRect=0;

    DocHasToc=FALSE;       // TRUE if doc has toc
    DocHasHeadings=FALSE;  // TRUE if doc has heading styles
    MultipleToc=FALSE;     // TRUE if doc has multiple toc
    TocShowPageNo=TRUE;    // TRUE to show page number for a toc
    TocTabAlign=TAB_RIGHT; // default tab alignment for toc page number
    TocTabLeader=TAB_DOT;  // default tab leader for toc page number

    SpellPending=FALSE;    // TRUE when auto spell checking pending
    SpellCheckerPopped=FALSE;  // TRUE when the spell-checker menu popped
    EditCol=-1;            // line being edited
    hPopup=NULL;           // popup menu handle

    UserDir[0]=0;                // default directory
    UserFileType=SAVE_TER;    // native file

    ImeStartPos=0;        // start position of ime input
    ImeEnabled=FALSE;     // true when ime is enabled
    InlineIme=Win32;      // initially true in Win32 environment
    ImeDiscards=0;        // number of WM_IME_CHARs to discard

    DefLang=ENGLISH;      // US English
    #if defined(WIN32)
       if (GetACP()==950 || GetACP()==936 || GetACP()==949 || GetACP()==1361) InlineIme=FALSE;   // disable for chinese and korean
       DefLang=(int)GetUserDefaultLangID();
    #endif

    ReqLang        =DefLang;   // current language


    LeftBorderWidth=0;    // page borders
    TopBorderHeight=0;

    TerArg.modified =0;          // indicates if the file needs saving
    HilightType    =HILIGHT_OFF; // turn off highlighting
    StretchHilight =FALSE;       // turn off highlight stretching mode
    HilightWithColCursor=FALSE;  // hilight using column cursor
    IgnoreMouseMove=TRUE;        // ignore mouse move until a mouse button is depressed
    MouseOnTextLine=TRUE;        // mouse is on text
    Notified       =FALSE;       // TRUE when the modification message is sent to your application
    MaxUndoLimit   =MAX_UNDOS;   // max undos limit 
    MaxUndos       =MAX_INIT_UNDOS;   // max undos allocated
    UndoCount      =0;           // number of undos
    UndoTblSize    =0;           // number of undo and redo elements in the undo table
    UndoRef        =0;           // undo reference count
    UndoSkipRef    =-1;          // undo reference to be skipped (partial undo)
    InUndo         =FALSE;       // True when in an undo operation
    InAccelerator  =FALSE;       // TRUE when processing an accelerator key
    SectModified   =FALSE;       // TRUE to recalculate section boundaries
    DocHeight      =0;           // total document height
    WheelShowing   =FALSE;       // TRUE when the mousewheel pressed
    WheelTimerUpOn   =FALSE;     // TRUE when the up wheel timer is on
    WheelTimerDownOn =FALSE;     // TRUE when the down wheel timer is on
    hWheelCur      =NULL;        // current wheel cursor

    CurClipRgn     =NULL;        // current clipping region
    pTerPal        =NULL;        // current palette data
    hTerPal        =NULL;        // current palette

    TotalPages     =1;           // total number of pages
    PrevTotalPages =1;           // used for determining the length of the page fields
    MaxPages       =0;
    CurPage        =0;
    PageModifyCount=-1;          // to force repagination after reading
    RepageBeginLine=0;           // repagination begin line
    RepagePending=FALSE;         // TRUE when the repagination is suspended because of pending keystrokes
    FirstFramePage=LastFramePage=0;  // the pages in the frame set
    repaginating =FALSE;         // TRUE during the repagination process

    NewFrameX     =-1;           // Use the current position
    NewFrameY     =-1;           // use the current position
    NewFrameVPage =FALSE;        // relative to the paragraph
    NewFrameWidth = 2160;        // width of the new frame
    NewFrameHeight=1440;         // height of the new frame

    PageInfo      =NULL;
    hStyleId      =NULL;
    StyleId       =NULL;

    hFrame        =NULL;         // handle to frame table
    hParaFrame    =NULL;         // handle to para frame table
    frame         =NULL;
    TerFont       =NULL;
    PrtFont       =NULL;
    hTerTab       =NULL;         // initialize the tab table variables
    TerTab        =NULL;
    TerBlt        =NULL;
    TerSect       =NULL;
    TerSect1      =NULL;
    hAppMemory    =NULL;         // data block reserved to use by your application
    pAppMemory    =NULL;         // data block reserved to use by your application
    CharScrFontId =NULL;         // character font id cache
    CharPrtFontId =NULL;         // character font id cache
    CharScrWidth  =NULL;         // character screen width width cache
    CharPrtWidth  =NULL;         // character printer width width cache

    TotalFonts    =0;            // not fonts created yet
    TotalTabs     =0;            // tab table not created yet
    NextFontId    =-1;            // next picture id to use
    NextFontAux1Id=0;            // next aux1 id to be used with TerCreateFont function
    NextParaAux1Id=0;            // next aux1 id to be used with TerCreatePara function
    DefTabWidth   =DEF_TAB_WIDTH;// default tab width
    DefTabType    =TAB_LEFT;     // the default tab type when the user clicks on the ruler
    TotalSects     =0;           // section table not created yet
    FontsReleased =FALSE;
    InRtfRead     =FALSE;        // TRUE when reading rtf
    MessageDisplayed=0;          // message display bits
    BkPictId=0;                  // background picture id

    hPfmtId       =NULL;         // handle to paragraph id table
    PfmtId        =NULL;
    MaxPfmts      =200;          // initial maximum para ids
    CurPfmt       =0;            // current paragraph id
    InputFontId   =-1;           // no font selected for input yet

    WindowBeingCreated=FALSE;    // TRUE when a window is being created
    WindowDestroyed=FALSE;       // TRUE when processing the WM_DESTROY window
    ReclaimResources=FALSE;      // reuse font and para ids - can be set to TRUE using GetTerField/SetTerFields functions
    PageHasControls=FALSE;       // TRUE when the current page contain controls
    ContinuousScroll=FALSE;      // TRUE to initiate bitmap scrolling
    CrLfUsed      =TRUE;         // TRUE when cr/lf pair used in the input buffer
    ProtectionLock=TRUE;         // protection lock on
    ShowParaMark  =FALSE;        // don't show para marker
    ShowPageBorder  =FALSE;      // don't show page border
    CaretEnabled  =FALSE;        // TRUE when caret is enabled
    CaretHidden   =TRUE;         // TRUE when caret is hidden
    CaretEngaged  =TRUE;         // TRUE when the caret is engaged to the current position
    CaretPos      =0;            // initial caret position
    ShowProtectCaret=TRUE;       // show caret in protected text
    ModifyProtectColor=TRUE;     // modify the color of the protected text
    LinkStyle     =ULINE;       // style of the hypertext phrase
    LinkColor     =0xFF0000;            // color of the hypertext phrase
    LinkDblClick  =FALSE;         // invoke hyperlink on double click
    VbxCallback   =NULL;         // vbx call back function
    MsgCallback   =NULL;         // message call back function
    ParentPtr     =NULL;         // parent pointer
    BatchMode     =((TerArg.style&TER_INVISIBLE)?TRUE:FALSE);  // TRUE when running in an invisible mode
    RepageTimerOn =FALSE;        // TRUE when the timer is on
    HilightTimerOn =FALSE;       // TRUE when the timer is on
    ClipTblLevel   =1;           // table level of the clipboard buffer, -1=unknown level, 0=not a table
    ClipEmbTable   =TRUE;       // TRUE when the paste buffer has embedded table
    NoTabIndent    =FALSE;       // TRUE - do not treat left indent as tab stop
    restrict       =FALSE;       // TRUE to create the restricted version

    WrapBufferSize=0;            // wrap buffer size
    hWrap         =0;            // wrap buffer for text wrap
    hWrapCfmt     =0;            // wrap buffer for format wrap
    WrapCtid      =NULL;         // wtap tag buffer
    WrapCharWidth =NULL;         // wrap char width buffer
    WrapCharWidthOrder =NULL;    // wrap char width order buffer
    WrapCharWidthDX =NULL;       // wrap char width DX buffer
    WrapCharWidthText =NULL;     // wrap char width text buffer
    WrapCharWidthClass =NULL;     // wrap char width text type buffer
    WrapLead      =NULL;         // wtap tag buffer
    WrapFlag      =WRAP_WIN;     // wrap the whole window by default
    WrapAddLines  =1;            // lines to add when lines needed during wrapping - more than 1 causes problem: DisplacePointers->AdjustSection->HdrLastLine
    MaxFonts      =25;           // maximum number of fonts slots to be allocated in the beginning
    MaxCharTags   =5;            // number of tags to be allocated in the beginning
    WrapWidthChars=0;            // number of characters to wrap at
    WrapWidthTwips=0;            // number of twips to wrap at

    MaxBlts       =50;           // number of bullet entires allocated in the beginning
    MaxLists      =10;           // initial number of lists
    MaxListOr     =10;           // initial number of list overrides

    FileFormat    =SAVE_TER;     // default file format

    CurSID       =-1;           // current style sheet being edited
    TotalSID    =0;             // total ids in the ss table
    MaxSID      =5;             // max ids to be allocated in the beginning
    EditingParaStyle=FALSE;     // TRUE when a paragraph style is being edited

    ScrResX=ScrResY=PrtResX=PrtResY=0;
    ZoomPercent =100;           // full zoom to start with
    ExtraSpacePrtX=ExtraSpaceScrX=0;

    OverrideBinCopy=0;          // do not override bin for printing
    OverrideBin=0;              // override bin id

    //************** Tool Bar variables *************
    ToolBarHeight =0;            // height of the tool bar
    hToolBarWnd=0;               // handle to the tool bar window
    hPvToolBarWnd=0;             // handle to the print preview tool bar window
    ToolBarCfmt=-1;              // reset
    ToolBarPfmt=-1;
    ToolBarSID=-1;

    //************** color variables *************
    TextDefBkColor=CLR_WHITE;    // default background color
    TextBorderColor=CLR_WHITE;   // default text border color
    FrameBkColor  =CLR_WHITE;    // frame background color
    ParaBackColor =CLR_WHITE;    // paragraph background color
    ParaBoxSpace  =PARA_BOX_SPACE; // default paragraph box space
    TextDefColor  =0;            // default text color

    StatusColor   =0x00080808;   // Status line foreground color
    StatusBkColor =0x00AFAFAF;   // Status line background color
    PageBorderColor =0x008F8F8F; // Status line background color
    PageBkColor=0xFFFFFF;        // page background color (page area between the page-borders)

    CurForeColor  =0x10000000;   // current foreground color
    CurBackColor  =0x10000000;   // current background color
    FrameShading  =0;            // current frame shading percent 0 to 100
    ParaShading   =0;            // current paragraph shading percent 0 to 10000
    PaintFrameFlags=0;           // frame flags used during painting
    ParaFrameSpace=FALSE;        // TRUE if a space is reserved for frame background

    PrtTextBkColor=CLR_WHITE;    // text background color during printing
    PrtParaBkColor=CLR_WHITE;    // para background color during printing
    PrtFrameBkColor=CLR_WHITE;   // frame background color during printing

    hToolbarBrush=NULL;          // toolbar brush

    //***************** text map variables *****************
    TotalSegments =0;            // total number of segments in the text map
    MaxSegs       =50;           // currently allocated screen segments
    UseTextMap    =TRUE;         // enable text map
    TextBorder    =0;            // reset border for printing text
    BoxLeft       =0;            // position of the left side of the para box
    BoxRight      =0;            // position of the right side of the para box

    PagingMargin  =1;        // number of lines to retail while PGUP and PGDN
    LineWidth     =MAX_WIDTH-2; // maximum line width
    TabWidth      =4;        // tab width in number of characters

    CrNewLine     =TRUE;     // create a new line when <CR> hit at the end of line
    CrSplitLine   =TRUE;     // split the current line if <CR> was hit
    TabAlign      =TRUE;     // Align tabs
    JoinLines     =TRUE;     // join the current line to the previous line when BKSP is hit
    HoldMessages  =FALSE;    // TRUE to suspend message processing
    FirstPalette  =TRUE;     // first palette to be realized
    BorderShowing =FALSE;    // TRUEN when the page border is showing

    DocCode       =(WORD)55555; // the first word in a file if equals to 55555 identifies
    DocBegin      =256;      // Document header size or text begin location

    ParaChar       =PARA_CHAR;// a character to mark end of a paragraph
    OldParaChar    =(BYTE)182;// a character to mark end of a paragraph
    lstrcpy(DocExt,".DOC");  // extension of a document file

    hPr           =0;         // printer device context
    PageWidth     =(float)8.5;// Default page width
    PageHeight    =(float)11; // Default page height

    InsertMode    =TRUE; // Turn insert mode ON

    MaxColBlock   =200;         // Biggest column block

    lstrcpy(SearchString,"");   // string to be searched
    lstrcpy(ReplaceString,"");  // string to be replaced
    lstrcpy(ReplaceWith,"");    // a string to be replaced with
    SearchFlags   =SRCH_CASE;   // case sensitive search by default

    TerHelpWanted =FALSE;       // TRUE when help is requested
    lstrcpy(TerHelpFile,"xpubtagnet.hlp"); // TER help file

    lstrcpy(CfmtSign,"~`!@#$%^&*()-+|=-TeCfMt");

    FmtSign=0xBF;              // indicates the beginning of a format data section
    FmtSignOld=0xBE;           // indicates the beginning of a format data section (version 3.5)

    PrinterName[0]=0;
    PrinterDriver[0]=0;
    PrinterPort[0]=0;
    hDeviceMode=0;
    hPrtDataDC=NULL;           // DC to send the printer data to
    InPrinting=FALSE;          // TRUE when the editor is printing
    PrinterAvailable=FALSE;    // TRUE when a printer is available
    PrtDiffRes=FALSE;          // TRUE when the printer has different resolution for x and y
    PaperSize=DMPAPER_LETTER;  // letter size paper
    PaperOrient=DMORIENT_PORTRAIT; // protrait orientation
    InPrintPreview=FALSE;      // TRUE when the editor is in print preview
    ExtPrintPreview=FALSE;     // TRUE when the editor is in external print preview
    GoPrintPreview=FALSE;      // TRUE to go back to print preview after printing.
    UsingZoomFonts=FALSE;      // TRUE when using the zoom fonts
    HasVarWidthFont=FALSE;     // Document has variable width font
    LinkPictDir[0]=0;          // Linked picture directory - when directory not specified explicitly
    TotalPreviewPages=1;       // number of preview pages to show
    PreviewZoom=0;             // best fit
    ShowPvToolbar=TRUE;        // show toolbar in the print preview mode

    RulerSection=0;            // section for which the ruler is displayed
    RulerPending=TRUE;         // Draw the ruler in the beginning
    PaintEnabled=TRUE;         // painting enabled
    FrameRefreshEnabled=TRUE;  // frame refresh enabled
    ShowHiddenText=FALSE;      // don't show hidden text
    ShowFieldNames=FALSE;      // don't show field names
    EditFootnoteText=FALSE;    // don't show footnote text
    EditEndnoteText=FALSE;     // don't show endnote text
    EndnoteAtSect=TRUE;        // endnote at the end of the section
    InFootnote=FALSE;          // TRUE when positioned on a footnote text
    ShowHyperlinkCursor=FALSE; // show hyperlink cursor
    MatchIds=TRUE;             // match font and para ids when creating a new one
    InDialogBox=FALSE;         // TRUE when displaying a dialog box
    SnapToGrid=FALSE;           // snap to ruler items to an invisible grid
    TblSelCursShowing=FALSE;   // TRUE when the table hilight cursor is showing

    // ole variables
    WaitForOle=0;               // OLE in progress message

    // drag object variables
    hDragObj=NULL;              // drag object handle table
    CurDragObj=-1;              // current drag object
    TotalDragObjs=0;            // number of drag objects on the screen
    MaxDragObjs=50;             // initial drag object size
    PictureHilighted=FALSE;     // TRUE when a picture object is highlighted
    FrameTabsHilighted=FALSE;   // TRUE when a frame size object is highlighted
    FrameRectHilighted=FALSE;   // TRUE when a frame move object is highlighted
    PictureClicked=FALSE;       // TRUE when a picture object is clicked
    FrameClicked=FALSE;         // TRUE when a frame object is clicked
    FirstFreeCellId=0;          // first free cell id
    CursorCell=0;               // cell to which the cursor is limited

    // table variables
    hTableRow=NULL;             // table row array
    hCell=NULL;                 // table cell array
    TotalTableRows=0;           // number of rows in the table
    TotalCells=0;               // number of cells in the table
    MaxTableRows=100;           // initial table row array size
    MaxCells=200;               // initial table cell array size
    MaxParaFrames=50;           // initial table cell array size

    CellChar=CELL_CHAR;         // cell break character
    OldCellChar=(BYTE)164;      // old cell break character
    FrameCellId=0;              // id of the current frame cell
    FrameRowId=0;               // id of the current frame row
    ShowTableGridLines=TRUE;    // Show table grid lines

    ViewPageHdrFtr=FALSE;       // don't view page header footer by default
    EditPageHdrFtr=FALSE;       // TRUE when editing page header/footer
    PosPageHdrFtr=FALSE;        // TRUE when reposition page header/footer

    SetCurLang(w,GetKeyboardLayout(0)); // set current language and character set
    
    // initialize the spell-time variables
    StCharSet=ANSI_CHARSET;     // default character set for spell-checking
    pStFromUniChar=NULL;        // unicode to byte conversion table pointer
    StFirstUniChar=0;           // first unicode character in the table
    StLastUniChar=0;           // first unicode character in the table


    // set the image map variables
    TotalImageMaps=1;          // initialize to 1
    FarMemSet(&(ImageMap[0]),0,sizeof(struct StrImageMap));

    // set break characters
    i=-1;
    i++;BreakChars[i]=PAGE_CHAR; // page break
    i++;BreakChars[i]=SECT_CHAR; // section break
    i++;BreakChars[i]=COL_CHAR;  // column break
    i++;BreakChars[i]=ROW_CHAR;  // table row break
    i++;BreakChars[i]=HDR_CHAR;  // page header
    i++;BreakChars[i]=FTR_CHAR;  // page footer
    i++;BreakChars[i]=FHDR_CHAR;  // first page header
    i++;BreakChars[i]=FFTR_CHAR;  // first page footer
    i++;BreakChars[i]=0;

    // set the rtf information name strings
    RtfInfo[INFO_TITLE]    ="title";
    RtfInfo[INFO_SUBJECT]  ="subject";
    RtfInfo[INFO_AUTHOR]   ="author";
    RtfInfo[INFO_MANAGER]  ="manager";
    RtfInfo[INFO_COMPANY]  ="company";
    RtfInfo[INFO_OPERATOR] ="operator";
    RtfInfo[INFO_CATEGORY] ="category";
    RtfInfo[INFO_KEYWORDS] ="keywords";
    RtfInfo[INFO_COMMENT]  ="comment";
    RtfInfo[INFO_DOCCOMM]  ="doccomm";
    for (i=0;i<INFO_MAX;i++) pRtfInfo[i]=NULL;

    // enable speed key
    FarMemSet(SpeedKeyEnabled,1,MAX_SPEED_KEYS);

    //************ DLL and non-DLL specific initialization *******
    //hTerInst=TerArg.hInst;      //uncomment this statment when statically linking the TER library

    return TRUE;
}

/******************************************************************************
    InitSect:
    Initialize the section variable for given section
*******************************************************************************/
BOOL InitSect(PTERWND w,int sect)
{

    FarMemSet(&(TerSect[sect]),0,sizeof(struct StrSect));
    FarMemSet(&(TerSect1[sect]),0,sizeof(struct StrSect1));
    TerSect[sect].InUse=TRUE;
    TerSect[sect].LeftMargin=TerSect[sect].RightMargin=(float)1.25; // inches
    TerSect[sect].TopMargin=TerSect[sect].BotMargin=(float)1.0;    // 1.0 inches
    TerSect[sect].HdrMargin=TerSect[sect].FtrMargin=(float).5;
    TerSect[sect].columns=1;                    // number of columns
    TerSect[sect].ColumnSpace=(float).5;        // .5 inches
    TerSect[sect].FirstLine=0;                  // all lines
    TerSect[sect].LastLine=TotalLines-1;        // all lines
    TerSect[sect].flags=SECT_NEW_PAGE;          // section starts new page
    TerSect[sect].orient=PaperOrient;           // default orientation
    TerSect[sect].FirstPageNo=1;                // first page number for display
    TerSect[sect].BorderColor=CLR_AUTO;         // border color
    TerSect[sect].PprSize=PaperSize;            // paper size
    TerSect[sect].PprWidth=PageWidth;           // paper width
    TerSect[sect].PprHeight=PageHeight;         // paper height

    TerSect1[sect].hdr.FirstLine=-1;            // page header line
    TerSect1[sect].ftr.FirstLine=-1;            // page footer line
    TerSect1[sect].fhdr.FirstLine=-1;           // first page header line
    TerSect1[sect].fftr.FirstLine=-1;           // first page footer line
    TerSect1[sect].PgWidth=PageWidth;           // page width
    TerSect1[sect].PgHeight=PageHeight;         // page height
    TerSect1[sect].hdr.height=TerSect1[sect].ftr.height=0;
    TerSect1[sect].fhdr.height=TerSect1[sect].fftr.height=0;
    TerSect1[sect].PrevSect=-1;

    return TRUE;
}

/******************************************************************************
   AbortTer:
   Handles an abnormal termination of TER routine.
*******************************************************************************/
void AbortTer(PTERWND w,LPBYTE desc,int code)
{
   if (lstrlen(desc)>0) MessageBox(NULL,desc,"Fatal Error",MB_OK);
   abort();
}

/******************************************************************************
   AllocArrayMem:
   Allocate fixed global memory of the following types:
      1.  Memory to store the handle to the line text data.
      2.  Memory to store the handle to the line formatting data.
      3.  Memory to store the fonts structure for the current window.
      4.  Memory to store the line length of each line.
*******************************************************************************/
AllocArrayMem(PTERWND w)
{
    int i;
    long l;

    MaxPages=5;          // allocate space for 5 page structures

    if (TerArg.PrintView || TerArg.PageMode) MaxFrames=50;
    else                                     MaxFrames=1;

    // allocate the line pointer array
    if (NULL==(LinePtr=OurAlloc((MaxLines+1)*sizeof(struct StrLinePtr far *)))) {
       return PrintError(w,MSG_OUT_OF_MEM,"AllocArrayMem");
    }
    for (l=0;l<MaxLines;l++) LinePtr[l]=NULL;  // initialize the pointers

    // allocate the attribute id arrays
    if (  (NULL==(TerFont=OurAlloc((long)(MaxFonts+1)*sizeof(struct StrFont))))
       || (NULL==(PrtFont=OurAlloc((long)(MaxFonts+1)*sizeof(struct StrPrtFont))))
       || (NULL==(TextSeg=OurAlloc((long)(MaxSegs+1)*sizeof(struct StrTextSeg))))
       || (NULL==(PageInfo=OurAlloc((long)(MaxPages+1)*sizeof(struct StrPage))))
       || (NULL==(reviewer=OurAlloc((long)(MaxReviewers+1)*sizeof(struct StrReviewer))))
       || (NULL==(TerBlt=OurAlloc((long)(MaxBlts+1)*sizeof(struct StrBlt))))
       || (NULL==(list=OurAlloc((long)(MaxLists+1)*sizeof(struct StrList))))
       || (NULL==(ListOr=OurAlloc((long)(MaxListOr+1)*sizeof(struct StrListOr))))
       || (NULL==(TerSect=OurAlloc((long)(MaxSects+1)*sizeof(struct StrSect))))
       || (NULL==(TerSect1=OurAlloc((long) (MaxSects+1)*sizeof(struct StrSect1))))
       || (NULL==(TableAux=OurAlloc((long)(MaxTableRows+1)*sizeof(struct StrTableAux))))
       || (NULL==(CellAux=OurAlloc((long)(MaxCells+1)*sizeof(struct StrCellAux))))
       || (NULL==(CharTag=OurAlloc((long)(MaxCharTags+1)*sizeof(struct StrCharTag))))
       || (NULL==(RowX=OurAlloc((long)(MAX_LINES_PER_WIN+1)*sizeof(int))))
       || (NULL==(RowY=OurAlloc((long)(MAX_LINES_PER_WIN+1)*sizeof(int))))
       || (NULL==(RowHeight=OurAlloc((long)(MAX_LINES_PER_WIN+1)*sizeof(int))))
       || (NULL==(undo=OurAlloc((long)(MaxUndos+1)*sizeof(struct StrUndo))))
       || (mbcs && NULL==(CharScrFontId=OurAlloc(CHAR_WIDTH_CACHE_SIZE)))
       || (mbcs && NULL==(CharPrtFontId=OurAlloc(CHAR_WIDTH_CACHE_SIZE)))
       || (mbcs && NULL==(CharScrWidth=OurAlloc(CHAR_WIDTH_CACHE_SIZE)))
       || (mbcs && NULL==(CharPrtWidth=OurAlloc(CHAR_WIDTH_CACHE_SIZE)))
       || (GlbPrtDC==NULL && NULL==(GlbPrtDC=OurAlloc((long)MAX_GLB_DC*sizeof(struct StrGlbPrtDC))))

       || (NULL==(hTerTab=GlobalAlloc(GMEM_MOVEABLE,(long)(MAX_TABS)*sizeof(struct StrTab))))
       || (NULL==(TerTab=(struct StrTab far *)GlobalLock(hTerTab)))
       || (NULL==(hPfmtId=GlobalAlloc(GMEM_MOVEABLE,(long)(MaxPfmts+1)*sizeof(struct StrPfmt))))
       || (NULL==(PfmtId=(struct StrPfmt far *)GlobalLock(hPfmtId)))
       || (NULL==(hStyleId=GlobalAlloc(GMEM_MOVEABLE,(long)(MaxSID+1)*sizeof(struct StrStyleId))))
       || (NULL==(StyleId=(struct StrStyleId far *)GlobalLock(hStyleId)))
       || (NULL==(hFrame=GlobalAlloc(GMEM_MOVEABLE,(long)(MaxFrames+1)*sizeof(struct StrFrame))))
       || (NULL==(frame=(struct StrFrame far *)GlobalLock(hFrame)))
       || (NULL==(hParaFrame=GlobalAlloc(GMEM_MOVEABLE,(long)(MaxParaFrames+1)*sizeof(struct StrParaFrame))))
       || (NULL==(ParaFrame=(struct StrParaFrame far *)GlobalLock(hParaFrame)))
       || (NULL==(hDragObj=GlobalAlloc(GMEM_MOVEABLE,(long)(MaxDragObjs+1)*sizeof(struct StrDragObj))))
       || (NULL==(DragObj=(struct StrDragObj far *)GlobalLock(hDragObj)))
       || (NULL==(hTableRow=GlobalAlloc(GMEM_MOVEABLE,(long)(MaxTableRows+1)*sizeof(struct StrTableRow))))
       || (NULL==(TableRow=(struct StrTableRow far *)GlobalLock(hTableRow)))
       || (NULL==(hCell=GlobalAlloc(GMEM_MOVEABLE,(long)(MaxCells+1)*sizeof(struct StrCell))))
       || (NULL==(cell=(struct StrCell far *)GlobalLock(hCell))) ) {
       return PrintError(w,MSG_OUT_OF_MEM,"AllocArrayMem(a)");
    }

    // Allocate the global font table, if not already allocated
    if (hGlbFont==NULL) {
       if ((NULL==(hGlbFont=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,(long)(MAX_GLB_FONTS)*sizeof(struct StrGlbFont))))
       || (NULL==(GlbFont=(struct StrGlbFont far *)GlobalLock(hGlbFont))) ) {
           return PrintError(w,MSG_OUT_OF_MEM,"AllocArrayMem");
       }
       TotalGlbFonts=0;             // initialze the total number of global fonts
    }

    // initialize the font character width table pointers
    for (i=0;i<MaxFonts;i++) TerFont[i].CharWidth=PrtFont[i].CharWidth=NULL;

    for (i=0;i<MaxPages;i++) FarMemSet(&(PageInfo[i]),0,sizeof(struct StrPage));

    if (mbcs) InitCharWidthCache(w);          // initialize the character width cache

    // initialize the reviewer table
    FarMemSet(&(reviewer[0]),0,sizeof(struct StrReviewer));
    TotalReviewers=1;

    // initialize the screen line arrays
    for (i=0;i<MAX_LINES_PER_WIN;i++) { // initialize the row position table
       RowX[i]=RowY[i]=RowHeight[i]=0;
    }
    
    return TRUE;
}

/******************************************************************************
   AllocWrapBuf:
   Allocate the wrap buffer memory.
*******************************************************************************/
AllocWrapBuf(PTERWND w, int size)
{
    int ScreenHeight,ScreenWidth;

    ScreenHeight=GetDeviceCaps(hTerDC,VERTRES)/(TerTextMet.tmHeight+TerTextMet.tmExternalLeading);
    ScreenWidth=GetDeviceCaps(hTerDC,HORZRES)/TerTextMet.tmAveCharWidth;

    if (size==0) {
       WrapMaxLines=ScreenHeight+5;    // maximum number of lines allowed to be wrapped in one call
       WrapBufferSize=(ScreenWidth+2)*(ScreenHeight+2);
       if (WrapBufferSize<0) WrapBufferSize=20000;   // use 20 K when goes beyond 32 k
    }
    else {                             // reallocate the buffer to the given size
       if (hWrap) {                    // free memory for the word wrap buffer
          GlobalUnlock(hWrap);
          GlobalFree(hWrap);
          GlobalUnlock(hWrapCfmt);
          GlobalFree(hWrapCfmt);
          OurFree(WrapCtid);
          OurFree(WrapLead);

          if (WrapCharWidth)      OurFree(WrapCharWidth);
          if (WrapCharWidthOrder) OurFree(WrapCharWidthOrder);
          if (WrapCharWidthDX)    OurFree(WrapCharWidthDX);
          if (WrapCharWidthText)  OurFree(WrapCharWidthText);
          if (WrapCharWidthClass) OurFree(WrapCharWidthClass);
          
          hWrap         =0;            // wrap buffer for text wrap
          hWrapCfmt     =0;            // wrap buffer for format wrap
          WrapCtid      =NULL;         // wtap tag buffer
          WrapCharWidth =NULL;         // wrap char width buffer
          WrapCharWidthOrder =NULL;    // wrap char width order buffer
          WrapCharWidthDX =NULL;       // wrap char width DX buffer
          WrapCharWidthText =NULL;     // wrap char width text buffer
          WrapCharWidthClass =NULL;     // wrap char width text type buffer
          WrapLead      =NULL;         // wtap tag buffer
       }
       
       WrapBufferSize=size;
    }

    if ( (NULL==(hWrap=GlobalAlloc(GMEM_MOVEABLE,WrapBufferSize)))
       || (NULL==(wrap=GlobalLock(hWrap)))  ) {
       return PrintError(w,MSG_OUT_OF_MEM,"AllocWrapBuf");
    }
    if ( (NULL==(hWrapCfmt=GlobalAlloc(GMEM_MOVEABLE,WrapBufferSize*sizeof(WORD))))
       || (NULL==(WrapCfmt=GlobalLock(hWrapCfmt)))  ) {
       return PrintError(w,MSG_OUT_OF_MEM,"AllocWrapBuf-A");
    }

    if ( (NULL==(WrapCtid=OurAlloc((long)WrapBufferSize*sizeof(WORD))))) {
       return PrintError(w,MSG_OUT_OF_MEM,"AllocWrapBuf-B");
    }

    if ( (NULL==(WrapLead=OurAlloc((long)WrapBufferSize*sizeof(BYTE))))) {
       return PrintError(w,MSG_OUT_OF_MEM,"AllocWrapBuf-C");
    }

    return (TRUE);
}

/******************************************************************************
    OverrideStyles:
    Override styles by the styles give by the TerStyles variable.
    When the TER window is created by the dialog manager or by your application
    directly, this routine is called by the InitTer function to create
    a copy of the arg_list structure.  This is done to simulate the situation
    when the window is created by the DLL using the CreateTerWindow call.
*******************************************************************************/
OverrideStyles(PTERWND w)
{
   TerArg.WordWrap=(BOOL)(TerStyles&TER_WORD_WRAP);
   TerArg.PrintView=(BOOL)(TerStyles&TER_PRINT_VIEW);
   TerArg.ShowHorBar=(BOOL)(TerStyles&TER_HSCROLL);
   TerArg.ShowVerBar=(BOOL)(TerStyles&TER_VSCROLL);
   TerArg.ShowStatus=(BOOL)(TerStyles&TER_SHOW_STATUS);
   TerArg.ruler=(BOOL)(TerStyles&TER_SHOW_RULER);
   TerArg.BorderMargins=(BOOL)(TerStyles&TER_BORDER_MARGIN);
   TerArg.ReadOnly=(BOOL)(TerStyles&TER_READ_ONLY);
   TerArg.PageMode=(BOOL)(TerStyles&TER_PAGE_MODE);
   TerArg.ToolBar=(BOOL)(TerStyles&TER_SHOW_TOOLBAR);
   TerArg.FittedView=(BOOL)(TerStyles&TER_FITTED_VIEW);

   if (TerStyles&TER_OUTPUT_RTF) TerArg.SaveFormat=SAVE_RTF;

   return TRUE;
}

/******************************************************************************
    CreateUserParam:
    When the TER window is created by the dialog manager or by your application
    directly, this routine is called by the InitTer function to create
    a copy of the arg_list structure.  This is done to simulate the situation
    when the window is created by the DLL using the CreateTerWindow call.
*******************************************************************************/
CreateUserParam(struct arg_list far *arg,CREATESTRUCT far *pWin)
{
    DWORD style;

    FarMemSet(arg,0,sizeof(struct arg_list));   // initialie the structure

    style=pWin->style;

    arg->x=pWin->x;            // Initial X position of TER edit window
    arg->y=pWin->y;            // Initial Y position of TER edit window
    arg->width=pWin->cx;       // Initial edit window width
    arg->height=pWin->cy;      // Initial edit window height
    arg->LineLimit=0;          // set to 0 for unlimited number of lines
    arg->InitLine=1;           // Initial line number to position on
    arg->UserCanClose=(style&WS_CHILD)?FALSE:TRUE;   // user can not close this child window directly

    if (pWin->lpszName) lstrcpy(arg->file,pWin->lpszName); // Window title if used
    else                lstrcpy(arg->file,"");
    arg->hInst=pWin->hInstance;// Current application instant handle
    arg->hParentWnd=pWin->hwndParent; // let this be the parent of the editor window
    arg->style=pWin->style;    //  Editor window style
    lstrcpy(arg->FontTypeFace,"");  // default font type faces
    arg->PointSize=12;         //  point size of the default font
    arg->open=FALSE;           // all window closed in the beginning
    if (pWin->hMenu && !(style&WS_CHILD)) arg->ShowMenu=TRUE; // display menu

    if (style&TER_WORD_WRAP)  arg->WordWrap=TRUE;        // Word wrap on?
    if (style&TER_PRINT_VIEW) arg->PrintView=TRUE;       // Wrap at window width
    if (style&TER_PAGE_MODE)  arg->PageMode=TRUE;        // Page mode on
    if (style&TER_FITTED_VIEW)arg->FittedView=TRUE;      // Fitted Page mode
    if (style&TER_VSCROLL)    arg->ShowVerBar=TRUE;      // display vertical scroll bar
    if (style&TER_HSCROLL)    arg->ShowHorBar=TRUE;      // display horizontal scroll bar (N/A with word wrap)
    if (style&TER_SHOW_STATUS)arg->ShowStatus=TRUE;      // display the status
    if (style&TER_SHOW_RULER) arg->ruler=TRUE;           // show the ruler
    if (style&TER_SHOW_TOOLBAR)arg->ToolBar=TRUE;        // show the tool bar
    if (style&TER_BORDER_MARGIN) arg->BorderMargins=TRUE;   // display margins around border
    if (style&TER_READ_ONLY)  arg->ReadOnly=TRUE;        // Read only session
    arg->SaveFormat=SAVE_DEFAULT; // format of the output file (keep it save as input)
    if (style&TER_OUTPUT_RTF) arg->SaveFormat=SAVE_RTF;  // save in RTF format

    arg->InputType='B';        // Input type: (F)ile or (B)uffer
    arg->hBuffer=0;            // handle to the input/output buffer
    arg->BufferLen=0;          // empty buffer
    arg->delim=0xD;            // use carriage return as delimiters


    return TRUE;
}

/******************************************************************************
    InitWinProp:
    Initialize window properties such as display context, fonts etc.
******************************************************************************/
BOOL InitWinProp(PTERWND w)
{
    int i;
    BYTE class[20];

    if (NULL==(hTerDC=GetDC(hTerWnd))) {    // allocate a class DC
         return PrintError(w,MSG_OUT_OF_DC,"");
    }

    Clr256Display=(8==(GetDeviceCaps(hTerDC,BITSPIXEL)*GetDeviceCaps(hTerDC,PLANES)));  // 256 clr display

    // get screen resolution
    OrigScrResX=ScrResX=PrtResX=GetDeviceCaps(hTerDC,LOGPIXELSX);
    OrigScrResY=ScrResY=PrtResY=GetDeviceCaps(hTerDC,LOGPIXELSY);

    ExtraSpaceScrX=TwipsToScrX(BOX_BORDER_WIDTH);  // a unit of extra space
    StatusLineWidth=TwipsToScrY(DEF_LINE_WIDTH);   // width of the lines used in the status area
    if (StatusLineWidth<1) StatusLineWidth=1;

    // open the printer
    if (TerArg.PrintView) OpenCurPrinter(w,FALSE) ; // open the printer device context
    if (PrinterAvailable && pDeviceMode) {     // initialize the papersize/ orientation
       PaperSize=pDeviceMode->dmPaperSize;     // letter size paper
       PaperOrient=pDeviceMode->dmOrientation; // protrait orientation
    }

    //*********** Initialize the font table
    for (i=0;i<MaxFonts;i++) TerFont[i].InUse=FALSE;
    TotalFonts=1;
    InitTerObject(w,0);

    //*********** create the font to write ruler markers ******
    TerFont[0].InUse=TRUE;                            // mark as in-use
    lstrcpy(TerFont[0].TypeFace,TerArg.FontTypeFace); // font type face
    TerFont[0].TwipsSize=8*20;                        // use this pointsize for the ruler
    TerFont[0].style=0;                               // no particular style

    if (!CreateOneFont(w,hTerDC,0,TRUE)) {            // create the default font
       return PrintError(w,MSG_ERR_FONT_CREATE,"InitWinProp");
    }

    hRulerFont=TerFont[0].hFont;                       // use the newly created font
    RulerFontHeight=TerFont[0].height;                 // character height
    
    DelRulerFont=(TerFont[0].GlbFontId==-1);           // this font needs to be delete explicity
    if (PrtFont[0].GlbFontId==-1 && PrtFont[0].hFont) DeleteObject(PrtFont[0].hFont);
    InitTerObject(w,0);                                // reinitialize the slot

    //*********** create the font to write ruler markers ******
    TerFont[0].InUse=TRUE;                            // mark as in-use
    lstrcpy(TerFont[0].TypeFace,TerArg.FontTypeFace); // font type face
    TerFont[0].TwipsSize=8*20;                        // use this pointsize for the ruler
    TerFont[0].style=BOLD;                            // font style

    if (!CreateOneFont(w,hTerDC,0,TRUE)) {            // create the default font
       return PrintError(w,MSG_ERR_FONT_CREATE,"InitWinProp");
    }

    hRulerFontBold=TerFont[0].hFont;                       // use the newly created font
    //RulerFontBoldHeight=TerFont[0].height;               // character height - use the same height as RulerFont
    
    DelRulerFontBold=(TerFont[0].GlbFontId==-1);           // this font needs to be delete explicity
    if (PrtFont[0].GlbFontId==-1 && PrtFont[0].hFont) DeleteObject(PrtFont[0].hFont);
    InitTerObject(w,0);                                // reinitialize the slot

    //*********** create the font to write the status bar ******
    TerFont[0].InUse=TRUE;                            // mark as in-use
    lstrcpy(TerFont[0].TypeFace,TerArg.FontTypeFace); // font type face
    TerFont[0].TwipsSize=9*20;                        // use this pointsize for the ruler
    TerFont[0].style=0;                               // no particular style

    if (!CreateOneFont(w,hTerDC,0,TRUE)) {            // create the default font
       return PrintError(w,MSG_ERR_FONT_CREATE,"InitWinProp");
    }

    hStatusFont=TerFont[0].hFont;                      // use the newly created font
    StatusFontHeight=TerFont[0].height;                // character height

    DelStatusFont=(TerFont[0].GlbFontId==-1);           // this font needs to be delete explicity
    if (PrtFont[0].GlbFontId==-1 && PrtFont[0].hFont) DeleteObject(PrtFont[0].hFont);
    InitTerObject(w,0);                                // reinitialize the slot

    //************** establish the default font in the font table **********
    TerFont[0].InUse=TRUE;                            // mark as in-use
    lstrcpy(TerFont[0].TypeFace,TerArg.FontTypeFace);  // font type face
    TerFont[0].TwipsSize=TerArg.PointSize*20;         // point size
    TerFont[0].style=0;                               // no particular style
    
    if (!CreateOneFont(w,hTerDC,0,TRUE)) {            // create the default font
       return PrintError(w,MSG_ERR_FONT_CREATE,"InitWinProp");
    }

    hTerRegFont=hTerCurFont=TerFont[0].hFont;

    if (NULL==SelectObject(hTerDC,hTerRegFont)) {   // select the newly created font
        return PrintError(w,MSG_ERR_FONT_SELECT,"InitWinProp");
    }

    //********************* other initializations **************************
    if (!GetTextMetrics(hTerDC,&TerTextMet)) {      // get text metric for the newly created font
        return PrintError(w,MSG_ERR_TEXT_METRIC,"InitWinProp");
    }

    SetBkMode(hTerDC,OPAQUE);              // enable opaqe printing of text

    hWaitCursor=LoadCursor(NULL,IDC_WAIT); // load hour glass cursor

    // load tool bar bitmaps for character and paragraph styles
    if (!hToolbarBM)      hToolbarBM=LoadBitmap(hTerInst,"ToolbarBM");

    // load cursor resources
    if (!hTable1Cur)      hTable1Cur=LoadCursor(hTerInst,"Table1");
    if (!hTable2Cur)      hTable2Cur=LoadCursor(hTerInst,"Table2");
    if (!hTable3Cur)      hTable3Cur=LoadCursor(hTerInst,"Table3");
    if (!hTab1Cur)        hTab1Cur=LoadCursor(hTerInst,"Tab1");
    if (!hHyperlinkCur)   hHyperlinkCur=LoadCursor(hTerInst,"Hyperlink");
    if (!hPlusCur)        hPlusCur=LoadCursor(hTerInst,"Plus");
    if (!hDragInCur)      hDragInCur=LoadCursor(hTerInst,"DragIn");
    if (!hDragInCopyCur)  hDragInCopyCur=LoadCursor(hTerInst,"DragInCopy");
    if (!hDragOutCur)     hDragOutCur=LoadCursor(hTerInst,"DragOut");
    if (!hWheelFullCur)   hWheelFullCur=LoadCursor(hTerInst,"wheel1");
    if (!hWheelUpCur)     hWheelUpCur=LoadCursor(hTerInst,"wheel2");
    if (!hWheelDownCur)   hWheelDownCur=LoadCursor(hTerInst,"wheel3");
    if (!hHBeamCur)       hHBeamCur=LoadCursor(hTerInst,"HBeam");

    // create memory device context
    if (!hMemDC) {                         // resource shared by windows
      if (NULL==(hMemDC=CreateCompatibleDC(hTerDC))) { // to paint pictures
         return PrintError(w,MSG_OUT_OF_COMP_DC,"");
      }
    }

    if (!hTerAccTable) {
       if (NULL==(hTerAccTable=LoadAccelerators(hTerInst,"TerAccTable"))) {   // load the accelerator table
          return PrintError(w,MSG_ERR_LOAD_ACCEL,"");
       }
    }

    // create pens
    if (!hPagePen) {
       if ( NULL==(hPagePen=CreatePen(PS_DOT,0,0))) {
         return PrintError(w,MSG_ERR_PEN,"");
       }
    }
    if (!hFocusPen) {
       if ( NULL==(hFocusPen=CreatePen(PS_DOT,0,0))) {
         return PrintError(w,MSG_ERR_PEN,"");
       }
    }

    // Create the tool bar
    InitToolbar(w);
    if (TerArg.ToolBar) CreateToolBar(w);

    GetWinDimension(w);                   // get window dimensions

    //if (TerArg.WordWrap && !TerArg.PrintView) TerArg.ShowHorBar=FALSE; // horizontal bar not applicable in word wrap mode

    if (TerArg.ShowHorBar) {                   // display horizontal scroll bar
       ShowScrollBar(hTerWnd,SB_HORZ,TRUE);
       SetScrollRange(hTerWnd,SB_HORZ,0,SCROLL_RANGE,FALSE);
       HorScrollPos=0;
       SetScrollPos(hTerWnd,SB_HORZ,HorScrollPos,TRUE);
       GetWinDimension(w);                        // get window dimensions
    }

    if (TerArg.ShowVerBar) {
       TerArg.ShowVerBar=FALSE;  // will be set back in the following function
       EnableVScrollBar(w,TRUE); // prepare vertical scroll bar
    }

    // Register our clipboard format
    RtfClipFormat        =RegisterClipboardFormat("Rich Text Format");  // register a format for RTF
    SSClipInfo           =RegisterClipboardFormat("SS Object Info");    // register a clipboard information format
    NativeClipFormat    =(OLECLIPFORMAT)RegisterClipboardFormat("Native");            // register a format for RTF
    OwnerLinkClipFormat =(OLECLIPFORMAT)RegisterClipboardFormat("OwnerLink");         // register a format for RTF
    ObjectLinkClipFormat=(OLECLIPFORMAT)RegisterClipboardFormat("ObjectLink");        // register a format for RTF
    #if defined(WIN32)
       CfEnhMetafile    =CF_ENHMETAFILE;   // enhancemeta file format
    #else
       CfEnhMetafile    =0;                // enh metafile not available for 16 bit
    #endif

    // find the MDI client and child when TER is embedded in an MDI environment
    if (TerArg.style&WS_CHILD) {
       hMdiChild=hTerWnd;
       hMdiClient=GetParent(hMdiChild);
       while (hMdiChild && hMdiClient) {    // look for the MDIClient class window
          GetClassName(hMdiClient,class,sizeof(class)-1);
          if (lstrcmpi(class,"MDIClient")==0) break;
          hMdiChild=hMdiClient;
          hMdiClient=GetParent(hMdiChild);
       }
    }

    if (TerArg.ShowStatus) DisplayStatus(w);   // display the status line

    if (TerFlags2&TFLAG2_RETAIN_BKND) SaveBackground(w);

    ValidateRect(hTerWnd,NULL);                // No need to paint right now

    if (eval) SetTimer(hTerWnd,TIMER_EVAL,30000,NULL);          // 30000 ms 

    return TRUE;
}

/****************************************************************************
    TerPickBknd:
    Pick the current background to be used as control's background 
*****************************************************************************/
BOOL WINAPI _export TerPickBknd(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    TerFlags2|=TFLAG2_RETAIN_BKND;    // turn-on this flag if not already set
    SaveBackground(w);

    return TRUE;
}

/******************************************************************************
    SaveBackground:
    Save the window background
*******************************************************************************/
SaveBackground(PTERWND w)
{
    HBITMAP hOldBM=NULL;

    // copy the image to a bitmap
    if (hInitBkBM) DeleteObject(hInitBkBM);
    if (NULL==(hInitBkBM=CreateCompatibleBitmap(hTerDC,TerWinWidth,TerWinHeight))) return FALSE;

    hOldBM=SelectObject(hMemDC,hInitBkBM);
    BitBlt(hMemDC,0,0,TerWinWidth,TerWinHeight,hTerDC,TerWinOrgX,TerWinOrgY,SRCCOPY);
    SelectObject(hMemDC,hOldBM);

    return TRUE;
}

/******************************************************************************
    EnableVScrollBar:
    Enable or disable vertical scroll bar.
*******************************************************************************/
EnableVScrollBar(PTERWND w,BOOL enable)
{
   if (TerArg.ShowVerBar==enable) return TRUE;  // no change needed

   TerArg.ShowVerBar=enable;

   if (enable) {
       ShowScrollBar(hTerWnd,SB_VERT,TRUE);
       SetScrollRange(hTerWnd,SB_VERT,0,SCROLL_RANGE,FALSE);
       GetWinDimension(w);                     // get window dimensions
   }
   else {
       ShowScrollBar(hTerWnd,SB_VERT,FALSE);
       PostMessage(hTerWnd,TER_REPAINT,0,0L);
   }

   return TRUE;
}

/******************************************************************************
    InitTypeface:
    Set the default typeface
*******************************************************************************/
InitTypeface(PTERWND w)
{
    HFONT hFont;
    LOGFONT lFont;

    if (!mbcs) lstrcpy(TerArg.FontTypeFace,"Arial");
    else { 
       #if defined(WIN32)
          hFont=GetStockObject(DEFAULT_GUI_FONT);
       #else
          hFont=GetStockObject(DEVICE_DEFAULT_FONT);
       #endif
       if (GetObject(hFont,sizeof(LOGFONT),&lFont))
            lstrcpy(TerArg.FontTypeFace,lFont.lfFaceName);
       else lstrcpy(TerArg.FontTypeFace,"System");

       if (lstrlen(TerArg.FontTypeFace)==0) lstrcpy(TerArg.FontTypeFace,"System");
    }

    if (lstrlen(InitFontFace)>0) lstrcpy(TerArg.FontTypeFace,InitFontFace);  // use the initial font face

    return TRUE;
}

/******************************************************************************
    InitPrinter:
    Initialize the current printer variables
*******************************************************************************/
InitPrinter(PTERWND w)
{
    hDeviceMode=0;
    pDeviceMode=NULL;

    if (True(TerFlags&TFLAG_NO_PRINTER) || True(TerArg.style&TER_DESIGN_MODE)) {
       PrinterAvailable=false;   // don't use printer
       return true;
    }

    // get the default printer
    CurrentPrinter(w,PrinterName,PrinterDriver,PrinterPort);

    if (!PrinterAvailable) return TRUE; // printer not available

    if (!InitDevMode(w,FALSE)) PrinterAvailable=FALSE;
    
    return TRUE;
}

/******************************************************************************
    InitDevMode:
    Initialize the DEVMODE structure for the printer
*******************************************************************************/
InitDevMode(PTERWND w, BOOL msg)
{
    int bytes;
    HANDLE hDriver;


    if (OpenPrinter(PrinterName,&hDriver,NULL)) {
       if ((bytes=DocumentProperties(hTerWnd,hDriver,PrinterName,NULL,NULL,0))<0) {
          return FALSE;  // PrintError(w,MSG_ERR_INIT_PRINTER,NULL);
       }
       if (bytes<sizeof(DEVMODE)) bytes=sizeof(DEVMODE);

       if ( (NULL==(hDeviceMode=GlobalAlloc(GMEM_MOVEABLE,bytes)))
          || (NULL==(pDeviceMode=(DEVMODE far *)GlobalLock(hDeviceMode)))  ) {
           if (msg) return PrintError(w,MSG_OUT_OF_MEM,"InitPrinter");
           else     return FALSE;
       }

       if (!DocumentProperties(hTerWnd,hDriver,PrinterName,pDeviceMode,NULL,DM_COPY)) {
          if (msg) return PrintError(w,MSG_ERR_INIT_PRINTER,"InitPrinter(a)");
          else     return FALSE;
       }

       ClosePrinter(hDriver);
       return TRUE;
    }
    else {
       //BYTE string[20];
       //wsprintf(string,"Printer: %s, Error: %lx",(LPBYTE)PrinterName,GetLastError());
       return FALSE; // PrintError(w,MSG_BAD_PRINTER_DRV,(LPBYTE)string);
    }


    return TRUE;
}

