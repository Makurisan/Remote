
#include "custcntl.h"          // custom control structure definitions
#include "ter_def.h"           // TER macro definitions
#include "ter_cmd.h"           // TER command ids
#include "ter_dlg.h"           // TER dialog control ids

/******************************************************************************
    Sub structure which needs to be defined before the main StrTer structure.
*******************************************************************************/
struct StrPdfBkm {    // pdf bookmark structure
    BYTE sign[2];     // signature byes 'TE'
    int level;        // bookmark level
    int page;         // bookmark page
    int y;            // y position of the bookmark entry
    BYTE name[100];   // name
};

struct StrLineSeg {
    int col;          // chracter column position
    int count;        // number of characters in the segment
    int x;            // pixel x position of the segment
    int width;        // pixel width of the segment.
    BOOL rtl;         // TRUE if rtl
};
 
struct StrImageMapRect {
    LPBYTE name;                         // name of the image map rectangle
    LPBYTE link;                         // link information in 'href=xyz' format
    LPBYTE target;                       // target for the link
    RECT   rect;                         // rectangle covered by this link
};

typedef struct StrStResult {
       UINT code;                        // result codes
       char replace[ST_MAX_WORD_LEN+1];  // word to substitute for the current word
       char reserved[1];                 // pad to 4 byte boundary - needed by VB 32 bit
       int  TotalAltWords;               // number of alternate words
       char AltWord[ST_MAX_SUG_WORDS][ST_MAX_WORD_LEN+1];  // each alternate word
       }STRESULT;


typedef struct StrFrameSet {
    int y;                        // y for this column of text
    int CurColHeight;             // total column height before the current frame
    int MaxColHeight;             // max column height before the current frame
    int ColumnX;                  // column x position
    int ColumnWidth;              // column width
    int ColumnSpace;              // column space
    int CellX;                    // cell begin x position
    int FrameHt;                  // current frame height

    int TopSect;                  // top page section
    int sect;                     // current section
    int TopLeftMargin;            // top left mragin

    int HiddenY;                  // hidden y
    int HdrMargin;
    int HdrHeight;
    int FtrMargin;
    int FtrHeight;

} FRAMESET; 

typedef FRAMESET far *PFRAMESET;             // pointer to the TER window variable structure

struct StrHdnFont {      // font information for hidden styles
   int     height;     // point size converted to pixel height for display
   int     BaseHeight; // baseline to the top of the character/picture
   int     BaseHeightAdj; // adjustment to base height before writing
   int     ExtLead;    // external leading space for the device
   int CharWidth[256];
};

struct StrHdrFtr {
      long  FirstLine;               // first header/ftr line
      long  LastLine;                // last header/ftr line
      int   height;                  // height of the header/ftr in printer units
      int   TextHeight;              // height of the header/footer text lines in printer units
      long LimitFtrLine;             // last footer line that can be contained on the page
      };

struct StrShape {                    // shape information
      int       x;                   // shape x
      int       y;                   // shape y
      int       width;               // shape width
      int       height;              // shape height
      int       FrmFlags;            // same as para frame flags PARA_FRAME_
      int       WrapType;            // shape SWRAP_ constants
      int       WrapSide;            // shape WRAPSIDE_ constants
      int       type;                // shape type constants SHPTYPE_
      int       align;               // LEFT, RIGHT_JUSTIFY or 0
      int       ZOrder;              // Z order of the shape
      COLORREF  LineColor;           // line color
      COLORREF  BackColor;           // fill color
      int       FillPattern;         // fill patten
      int       LineWdth;            // line width
      int       DistFromText;        // distance from text
      int       FillPict;            // picture id to fill the shape with
      };

struct StrShpGrp {                   // shape group information
      int       left;                // shape left location
      int       width;               // shape right
      int       top;                 // shape top
      int       height;              // shape bottom
      int       GroupLeft;           // left value relative to which the nested shape left is calculate
      int       GroupTop;            // top value relative to which the nested shape top is calculate
      int       GroupWidth;          // the nested shp are sized relative to the GroupWidth
      int       GroupHeight;         // group height
      };

struct StrRtfParaFrameInfo {            // para frame id information
      int  x;                           // x position in twips
      int  y;                           // y position in twips
      int  width;                       // frame width in twips
      int  height;                      // frame height in twips
      int  DistFromText;                // frame distance from text
      int  ZOrder;                      // Z order of the frame
      };

struct StrRgb {                         // most used color table
      BYTE red,green,blue;              // color components
      long count;                       // number of pixels using this color
      };

struct StrTabw {             // stores tab or section information for a line
      UINT type;           // information type: INFO_TAB, INFO_SECT
      int count;          // number of tabs used in the line
      int width[TE_MAX_TAB_STOPS];  // screen width of each tab
      int section;        // section number when type==INFO_SECT
      int JustCount;      // justification - number of spaces allocated one extra pixel
      int JustAdj;        // justification - extra pixels for each space
      int JustSpaceCount; // justification - number of spaces to modify
      int JustSpaceIgnore;// justification - number of spaces to ignore
      int FrameCharPos;   // character number before which the space is created
      int FrameX;         // frame space begin position
      int FrameWidth;     // width of the frame space - printer pixels
      int FrameSpaceWidth;// total width of the frame space including the distance from text in printer pixels
      int FrameScrWidth;  // total width of the frame space including distance from text - screen pixels
      int height;         // height of a space line in printer units
      int CharFlagsLen;   // length of the char flags array
      LPBYTE CharFlags;   // character flags
      LPBYTE ListText;    // list text to print for the line
      int ListTextWidth;  // list text width (prt unit when in PageMode-no-fitted view, else screen unit)
      int ListFontId;     // font id used for printing a list text
      long ListNbr;       // list number
      int ListnumCount;   // number of listnum fields in the line
      struct StrListnum far *pListnum;    // listnum structure
      LPBYTE pAutoNumLgl; // auto number gl field text
      };

union UnionCfmt {
     LPWORD pFmt;              // handle to the character formatting information
     struct {
        unsigned short type;   // = UNIFORM if one format used through out the line
        WORD fmt;              // format if type = UNIFORM
     }info;
};                             // character formatting data for the line

struct StrAnim {
      int    FirstPict;   // first picture in the animation list
      int    NextPict;    // next picture in the animation sequence
      long   LoopCount;   // current loops remaining
      long   OrigLoopCount;// Orignal number of loops or animation
      int    delay;       // delay (1/100 of second) before display this picture in the animation sequence
      int    CurAnim;     // currently animated picture  - used only for the initial picture
      int    NextAnim;    // next picture to be displayed - used only for the initial picture
      int    TimerId;     // animation timer id - used only for the initial picture
      BYTE   reserved[20];// reserved for future use
      };

typedef struct StrGifAnim {       // animation picture collection while reading the gif file
   HGLOBAL hDIB[MAX_ANIM_GIFS];   // bitmaps for animation picture
   long    AnimLoops;             // number of loops in the animation
   int     AnimPicts;             // number of animation pictures
   int     delay[MAX_ANIM_GIFS];  // delay to display a picture
   int     InitDelay;             // delay for the initial picture
};

struct StrClipInfo {
   short   size;                  // size of the block
   short   TblLevel;              // table level (0=not a table)     
   short   EmbTable;              // TRUE if the paste buffer contains embedded table
}; 

struct StrListnum {
   BYTE text[100];                // list text
   long ListNbr;
   int  ls;                       // an index into ListOr table
   int  lvl;                      // 0 or an index into ListOr->level table 
   BOOL IsHPARA;                  // Is HPARA
};

#if !defined(WIN32) 
  typedef struct StrSystemTime {
     WORD wYear;                  // year
     WORD wMonth;                 // month, January = 1
     WORD wDayOfWeek;             // day of the week, Sunday=0, Monday=1, etc
     WORD wDay;                   // Current day of the month
     WORD wHour;                  // hour
     WORD wMinute;                // minute
     WORD wSecond;                // second
     WORD wMilliseconds;          // milliseconds
  } SYSTEMTIME; 

#endif

/******************************************************************************
    The following structure is allocated for each window.  The structure
    contains the global variables required for a window.  This structure
    is deallocated when the window is closed.
*******************************************************************************/
typedef struct StrTer {
     struct arg_list TerArgW;          // keep a copy of the user parametres

  /******************************************************************************
     Text line data arrays
  *******************************************************************************/

     struct StrLinePtr {
        LPBYTE pText;                 // pointer to the line text
        LPBYTE pLead;                 // pointer to the lead byte of the line text
        union UnionCfmt cfmt;         // character formatting data for the line
        int    LineLen;               // length of the line
        struct StrTabw far *tabw;     // additional line information structure
        int    pfmt;                  // paragraph format id
        int    LineHt;                // line height in printer units
        int    BaseLineHt;            // base line height in printer units - also includes space before the line
        int    ScrLineHt;             // line height in screen units
        int    LineY;                 // Y position (printer units) relative to the top of the current frame page (will be different when hdr/ftr or borders are toggled)
        int    LineX;                 // Page X position in printer units
        int    cid;                   // cell id
        int    fid;                   // paragraph frame id
        int    LinePage;              // page number for the line
        DWORD  LineFlags;             // line flags LFLAG_ constant
        DWORD  LineFlags2;            // line flags LFLAG2_ constant
        int    JustAdjX;              // Adjust to x indentation because of centering/right-justification (printer unit)
        LPWORD pCtid;                 // pointer to the character tag id array
        LPWORD pCharWidth;            // character width array
        int    LineFrame;             // screen frame number
     } far * (huge *LinePtrW);        // this 'huge' array contains 'far' line pointers

     struct StrLinePtr far *LinePtrCacheW[MAX_LINE_PTRS+1]; // line pointer cache

     int LinePtrCountW;               // number of line pointers in the cache

  /******************************************************************************
     Screen display control variables
  *******************************************************************************/
     HWND           hTerWndW;     // handle to TER Window
     HDC            hTerDCW,      // handle to TER class DC
                    hPrW,         // current printer device context
                    hBufDCW;      // used for buffered display

     HFONT          hTerRegFontW, // regular fixed pitched font
                    hTerCurFontW, // current font in the display context
                    hRulerFontW,  // Ruler font handle
                    hRulerFontBoldW, // Ruler bold font handle
                    hStatusFontW; // status display font
     TEXTMETRIC     TerTextMetW;  // TER text metric
     HCURSOR        hWaitCursorW; // handle to Hour glass cursor
     RECT           TerRectW,     // entire client window rectangal
                    TerWinRectW,  // text window rectangal
                    RulerRectW,   // ruler rectangle
                    ClipRectW,    // current clipping rectangle
                    DirtyRectW;   // screen rectangle that needs to be updated
     HBITMAP        hScrollBMW,   // stores scroll bits
                    hBkPictBMW,   // background picture bitmap
                    hBufBMW,      // used for buffered display
                    hOrigBufBMW,  // original bitmap in the buffered DC
                    hInitBkBMW;   // initial window background image

     int     CurColW,             // current window column position
             EditColW,            // column being edited
             EditWordIndexW,      // index of the word being edited
             EditWordLenW,        // length of the word being edited
             WinWidthW,           // current window in columns
             WinHeightW,          // current window height in number of rows
             LineWidthW,          // maximum line width in number of characters
             PaintFlagW,          // indicates scope of painting
             TabWidthW,           // tab width in number of characters
             HorScrollPosW,       // horizontal scroll position
             VerScrollPosW,       // vertical scroll position
             VerThumbSizeW,       // size of the vertical scroll bar thumb
             HorThumbSizeW,       // size of the horizontal scroll bar thumb
             MinThumbHtW,         // minimum thumb size in the SCROLL_RANGE page units
             TerWinWidthW,        // window width in units
             TerWinHeightW,       // window height in units
             TerWinOrgXW,         // window origin x co-ordinates
             TerWinOrgYW,         // window origin y co-ordinates
             NextXW,              // logical X co-ordinate where next painting will begin
             NextTabW,            // next tab number while displaying a text line
             NextSpaceW,          // next space number in a justified text
             PaintEnabledW,       // When FALSE, disables the window painting
             FrameRefreshEnabledW,// TRUE when the frame refresh is enabled
             WrapPendingW,        // wrapping postponed
             RulerPendingW,       // ruler painting postponed
             RulerLineXW,         // LineX for the ruler line
             RulerSectionW,       // section for which the ruler is displayed
             PagingMarginW,       // number of lines to retail while PGUP and PGDN
             RulerFontHeightW,    // height of the ruler font
             StatusFontHeightW,   // height of the status font
             StatusLineWidthW,    // width of the box in the status area
             ScrResXW,            // screen resolution/inch in X direction - adjusted for zoom
             ScrResYW,            // screen resolution/inch in Y direction - adjusted for zoom
             OrigScrResXW,        // original screen resolution/inch in X direction
             OrigScrResYW,        // original screen resolution/inch in Y direction
             PrtResXW,            // printer resolution/inch in X direction
             PrtResYW,            // printer resolution/inch in Y direction
             CurLineYW,           // Y position of the current line in PrintView mode
             CurPageWidthW,       // Height of the current page
             CurPageHeightW,      // Height of all pages in the frame set
             PageHeightAdjW,      // page height adjustment use in variable height page mode 
             FirstPageHeightW,    // Height of the first page in the frame set
             FirstPage2FrameW,    // first frame for the second page in the frame set
             FirstFramePageW,     // page number of the first page in the frame set
             LastFramePageW,      // page number of the last page in the frame set
             CurTextHeightW,      // Height of the text in the frame set
             CursHorzPosW,        // Horizontal cursor position or up/down key processing
             OverhangFontW,       // font to cover the previous overhang
             WinYOffsetW,         // offset of the first line with respect to top of the window
             CommandIdW,          // current Command Id
             PrevCmdIdW,          // previous commond id
             CmdModifiedW,        // tracking of modification count when the previous command was issued
             MessageIdW,          // current message Id
             RepaintNoW,          // incremental number of this repaint
             BorderSpillW,        // Spillover of the controls in any border direction, uses SPILL_ constants
             LeftBorderWidthW,    // left/right border width (in prt unit) when the page borders are displayed
             TopBorderHeightW,     // top border height (in prt unit) when the page borders are displayed
             BotBorderHeightW,     // bottom border height (in prt unit) when the page borders are displayed
             transparentW,        // transparent painting
             EnableTransparentW,  // enable transparency for text with no background color
             ScrYOffsetW,         // Y offset to be added during screen writing to protect decenders
             StatusBarHeightW,    // height of the status bar
             DispTextHtW,         // text height of the line being displayed
             GlbPrtDCIdW,         // id of the global font
             CurInputFieldW,      // current input field
             ClipTblLevelW,       // table level of the clipboard text
             ViewKindW,           // RTF document view kind
             WrapSpellCheckedW,   // TRUE when a paragraph is spell checked
             PrtLeftMargW,        // left margin of the page being printed
             PrtTopMargW,         // top margin of the page being printed
             ScrFrameAngleW,      // text angle of the frame being displayed
             FrameDistFromMarginW, // minimum frame distance from margin before text allows around the frame
             DocTextFlowW,         // FLOW_ constants
             SpellColW,           // column where autospell popped
             WrapTextFlowW,       // text flow direction for the paragraph being wrapped
             ImgDenXW,            // pixel density in the x direction of the image being imported
             ImgDenYW;            // pixel density in the x direction of the image being imported


     long    CurRowW,             // current window row position
             CurLineW,            // current line number in the file
             EditLineW,           // line being edited
             BeginLineW,          // line number of the first window line
             TotalLinesW,         // total lines in the file
             MaxLinesW,           // maximum number of line space allocated
             PaintBegLineW,       // first line to be painted
             PaintEndLineW,       // last line to be painted
             WinYOffsetLineW,     // First line with offset
             FrameRefreshLineCountW,// line count at which the frame was last refreshed.
             DocHeightW,          // total text height
             SpellLineW;          // line where the autospell popped

     BOOL    CrNewLineW,          // create a new line when <CR> hit at the end of line
             CrSplitLineW,        // split the current line if <CR> was hit
             TabAlignW,           // Align tabs
             JoinLinesW,          // join the current line to the previous line when BKSP is hit
             NotifiedW,           // TRUE when modification message is sent to your application
             InAcceleratorW,      // TRUE when processing an accelerator key
             CommandSkippedW,     // TRUE accelerator command skipped
             RulerClickedW,       // TRUE when user clicks the ruler
             FontsReleasedW,      // TRUE when the fonts are released
             WindowDestroyedW,    // TRUE when processing WM_DESTROY message
             ReclaimResourcesW,   // reuse unused font and para ids
             PageHasControlsW,    // TRUE when the current page has controls embedded into it
             ContinuousScrollW,   // TRUE to begin bitmap scrolling
             MovingControlW,      // TRUE when a control is being moved
             HtmlModeW,           // TRUE when html mode adjustments in effect
             HoldMessagesW,       // TRUE not to process messages
             FirstPaletteW,       // TRUE when the first palette is to be realised for the screen painting
             BorderShowingW,      // TRUE when the page border is showing
             PageBreakShowingW,   // TRUE when the page break line is showing
             ShowPageBreakW,      // TRUE to show the page break line
             PagesShowingW,       // TRUE when in page mode but not in fitted view mode
             OnWmPaintW,          // TRUE when the paint message is received
             MetaPalFoundW,       // pallet used in the metafile
             DraggingTextW,       // TRUE when text is being dragged
             InOleDragW,          // TRUE when using OLE DropSource/DataObject interfaces to drag text
             InDragDropW,         // TRUE when text is being dropped from another application
             Clr256DisplayW,      // TRUE if working on a 256 color display
             SkipCommandW,        // TRUE to skip a preprocessed command
             InPreprocessW,       // TRUE when in the preprocess event
             RedrawBorderW,       // TRUE to redraw the border area
             PrivatePrtDCW,       // private printer DC
             DrawBkndW,           // draw background
             HasOverlayingFramesW,// TRUE when a frameset has overlaying frames
             SpellPendingW,       // TRUE when auto-spellcheckig pending
             SpellCheckerPoppedW, // TRUE when the spellchecker menu popped
             mbcsW,               // TRUE if operating on an mbcs system.
             ScrRtlW,             // TRUE to apply rtl flow during screen painting
             ClipEmbTableW,       // TRUE if the paste buffer has embedded table
             DelRulerFontW,       // TRUE if Ruler font needs to be deleted explicitly
             DelRulerFontBoldW,   // TRUE if Ruler font needs to be deleted explicitly
             DelStatusFontW,      // TRUE if status font needs to be deleted explicitly
             EnterHitW,           // Enter key hit
             UseScrMetricsW;      // TRUE to use screen metrics instead of the printer to format the text


     int     StatusRowPosW,       // logical unit position where status row number is printed
             StatusColPosW,       // logical unit position where status col number is printed
             StatusLinePosW;      // logical unit position where status line number is printed

     HRGN    CurClipRgnW;         // current clip region handle

     LOGPALETTE far *pTerPalW;    // current color palette data
     HPALETTE hTerPalW;           // current palette handle

     LPBYTE  TempLeadW;           // temporary display pointer to the lead byte
     LPBYTE  pFontInUseW;         // temporary table to record fonts in use while reading an sse file
     int     FontInUseSizeW;      // size of the FontInUse buffer

     struct StrLineSeg far *pScrSegW; // screen line segment table - use for rtl display
     int    TotalScrSegW,        // number of segments in the segment table                        
            CurScrSegW;          // current scr segment

     WORD   KeyDownUniCharW;     // unicode character derived at keydown
     WORD   DeadCharW;           // dead character

  /******************************************************************************
                    Text Frame variables
  *******************************************************************************/
      struct StrFrame {
        BOOL   empty;            // TRUE if the frame does not contain text
        long   PageFirstLine,    // first line in the frame
               PageLastLine,     // last line of the frame
               ScrFirstLine,     // first line visible in the window
               ScrLastLine;      // last line visible in the window
        int    x,                // frame X location relative to page
               y,                // frame Y location relative to page
               ScrX,             // X location of WinBeginLine
               ScrY;             // Y location of WinBeginLine
        int    width,            // width of the frame
               height,           // height of the frame
               LimitY,           // text limitation on the y access, otherwise -1.
               TextHeight,       // height where the text ends
               ScrWidth,         // width of the frame visible in the window
               ScrHeight;        // height of the frame visible in the window
        int    RowOffset;        // offset into the RowY,RowHeight & RowX arrays
        int    sect;             // section to which this frame belongs
        UINT   border;           // frame border, uses BORDER_ constants
        int    BorderWidth[4];   // border width of each border
        COLORREF BorderColor[4]; // border color
        BOOL   LastColumnFrame;  // TRUE for the last frame of a column
        int    RowId;            // for cell frames the row id of the table row
        int    CellId;           // for cell frames the cell id of the frame
        int    SpaceTop;         // space before the first line
        int    SpaceBot;         // space after the last line
        int    SpaceLeft;        // left margin
        int    SpaceRight;       // right margin
        int    shading;          // frame shading percent 0 to 100
        int    ParaFrameId;      // para frame id
        UINT   flags;            // FRAME_? set of flags
        UINT   flags1;           // FRAME1_ set of flags
        COLORREF BackColor;      // background color
        int    LeftBorderWdth;   // left border width used by the top/bottom border frames
        int    ZOrder;           // Z order of the frame
        int    DispFrame;        // The frame to be painted at the current index
        int    level;            // subtable level
        int    BoxFrame;         // box frame containing this para-frame
        int    ParentFrame;      // parent frame
        int    OrigX;            // original value of x before rtl application
     } huge *frameW;


     HANDLE    hFrameW;          // handle for the above table
     int       CurFrameW,        // current frame index
               TotalFramesW,     // total number of frames
               MaxFramesW,       // maximum number of frames allocated
               NewFrameWidthW,   // width to use to create a new frame
               NewFrameHeightW,  // height to use to create a new frame.
               NewFrameXW,       // x position of the new frame
               NewFrameYW,       // y positin of the new frame from the top of the page, (from the top margin if header/footer not displayed)
               NewFrameVPageW;   // TRUE if new frame is to be relative to the top of the page

     BOOL      FramesSortedW;    // The frames are produced in the sorted order

  /******************************************************************************
                    Paragraph Frame variables
  *******************************************************************************/
      struct StrParaFrame {
        BOOL   InUse;            // TRUE when in use
        int    x;                // x position (twips) relative to left margin
        int    y;                // y position (twips) relative to top margin
        int    ParaY;            // y (twips) relative to the beginning of the next para
        int    width;            // width in twips
        int    margin;           // left and right border margin
        int    height;           // current height in twips
        int    MinHeight;        // minimum height in twips
        int    DistFromText;     // distance from the text text in twips
        int    PageNo;           // page number to which this frame belongs
        UINT   flags;            // PARA_FRAME_ flags
        long   TextLine;         // used temporarily to store the corresponding text line
        int    LineType;         // Line object type: DOL_ constants
        COLORREF BackColor;      // background color for the objects
        int    FillPattern;      // fill pattern number for the objects
        int    LineWdth;         // line width
        int    PageY;            // used to store the page y for para relative frame
        COLORREF LineColor;      // border line color
        int    pict;             // picture id when associated with an aligned picture
        int    ZOrder;           // Z order 0 for the most in the background, higher number for the one on the top
        int    CellId;           // cell id for the picture frame, if the picture is located inside a table
        int    TextAngle;        // text flow angle
        int    GroupX;           // used when PARA_FFAME_SHPGRP is set - this allows 'x' to become variable
        int    GroupY;           // used when PARA_FFAME_SHPGRP is set - this allows 'ParaY' to become variable
        int    ShapeType;        // SHPTYPE_ constants
        int    FillPict;         // picture id used to fill the shape
        int    OrgX;             // (twips) to be used with PARA_FRAME_INGORE_X type of frames
        int    OrgY;             // (twips) to be used with PARA_FRAME_INGORE_X type of frames
        
        BYTE   reserved32[36];   // reserved byets

     } huge *ParaFrameW;

     HANDLE    hParaFrameW;      // handle for the above table
     int       CurParaFrameW,    // current para frame index
               TotalParaFramesW, // total number of para frames
               MaxParaFramesW,   // current maximum para frames
               CurWrapParaFIDW,  // ParaFID for the paragraph being wrapped currently
               WmParaFIDW;       // Watermark paraframe id

     BOOL      ContainsParaFramesW; // TRUE when a text frame contain paraframes

  /******************************************************************************
                    Screen Text Line Position Table
  *******************************************************************************/
        LPINT  RowYW,                // Y position of each row
               RowHeightW,           // pixel height of each screen row
               RowXW;                // Starting X position of each row

  /******************************************************************************
                    Track-changes variables
  *******************************************************************************/
       struct StrReviewer {
          BYTE name[50];                // reviewer name
          COLORREF InsColor;            // insert color
          DWORD    InsStyle;            // insert character style
          COLORREF DelColor;            // delete color
          DWORD    DelStyle;            // delete style
          int      RtfId;               // id assigned using RTF read
          BYTE     reserved[20];
       }*reviewerW;
    
       int MaxReviewersW,               // maximum reviewer allocated
           TotalReviewersW;             // total reviewers in the table

  /******************************************************************************
                    Auto-complete variables
  *******************************************************************************/
    BOOL  InAutoCompW;              // collection auto comp string
    int   TotalAutoCompsW;          // number of auto comp strings
    long   AutoCompPosW;            // auto comp start pos
    LPBYTE AutoCompWordW[MAX_AUTO_COMPS+1];  // auto comp word
    LPBYTE AutoCompPhraseW[MAX_AUTO_COMPS+1];  // auto comp phrase


  /******************************************************************************
                       Text segment structure
  *******************************************************************************/
       struct StrTextSeg {
        BOOL    drawn;               // TRUE if the text segment is drawn
        LPBYTE   string;              // pointer to the text string
        HANDLE  hString;             // handle to the string
        int     len;                 // length of the text string
        int     x;                   // starting X coordinate of the text
        int     y;                   // starting Y coordinate of the text
        RECT    rect;                // coordinate of the text box
        COLORREF color;              // forground color
        COLORREF BackColor;          // background color
        int     fmt;                 // format id used for the segment
        UINT    border;              // BORDER_TOP,BORDER_BOT,BORDER_LEFT,BORDER_RIGHT
        BYTE    TabFlagsW;           // tab flags
        int     CurPfmtW;            // current para id
        UINT    ParShading;          // paragraph shading
        int     FrmShading;          // frame shading
        int     BoxLeftPos;          // position of left side of the box
        int     BoxRightPos;         // position of the right side of the box
        int     bulleted;            // TRUE when the bullet is drawn for the line
        int     CellId;              // frame cell id
        int     RowId;               // frame row id
        int     FrameHt;             // frame height
        int     FrameY;
        BOOL    ParFrameSpace;       // TRUE is space reserved for a paragraph frame
        int     RepaintNoW;          // incremental number of this repaint
        int     ParaFIDW;            // para frame id
        int     DevX;                // x position in physical units.
        int     DevY;                // y position in physical units.
        HBITMAP hBmp;                // background bitmap
     }far *TextSegW;

     int TotalSegmentsW,            // total text segments
          MaxSegsW;                 // current maximum number of segments
     BOOL UseTextMapW;              // use text map to increase painting efficiency
     UINT TextBorderW,              // current text border to be used
          ParaShadingW,             // current shading for the paragraph (0 to 10000)
          PaintFrameFlagsW;         // frame flags used during painting
     BOOL ParaFrameSpaceW,          // TRUE if paraframe space
          ShadeBeginW,              // TRUE if shading begins at this line
          ShadeEndW;                // TRUE if shading ends at this line

     int  FrameCellIdW,             // current frame cell id
          FrameRowIdW,              // current frame Row id
          TsFrameYW,                // top of current frame for text segment
          FrameHeightW;             // current frame height
     BYTE TabFlagsW;                // tab flags
     COLORREF FrameBkColorW,        // background color of a frame
              ParaBackColorW;       // background color of a paragrah

     int  ParaBoxSpaceW;            // para box space
  /******************************************************************************
     MBCS variables
  *******************************************************************************/
     BYTE    LeadWaitingW;        // lead byte waiting to be processed

  /******************************************************************************
     Mouse control variables
  *******************************************************************************/
     long    MouseLineW;          // text line position of mouse

     int     MouseColW,           // text column position of mouse
             MouseXW,             // current x position of mouse
             MouseYW,             // current y position of mouse
             MousePictFrameW;     // frame number where mouse was located
     BYTE    MouseOverShootW;     // stores L,R,T,or B when mouse goes past the text area of the screen
     int     MouseOverShootDistW; // distance (pixels) by which mouse went out

     BOOL    IgnoreMouseMoveW,    // ignore mouse move until a mouse button is depressed
             VerySmallMovementW,  // record if the mouse movement is very small
             MouseOnTextLineW;    // TRUE when the mouse is on text line

     BOOL    WheelShowingW,       // mouse wheen is showing
             WheelTimerUpOnW,     // TRUE when the up wheel timer is on
             WheelTimerDownOnW;   // TRUE when the down wheel timer is on

     int     WheelXW,             // X position of the center of the wheel
             WheelYW;             // Y position of the center of the wheel

     HCURSOR hWheelCurW;          // current wheel cursor

  /******************************************************************************
     Mouse bubble variables
  *******************************************************************************/
     int     MouseStopXW,         // x location where mouse rested
             MouseStopYW,         // y location where mouse rested
             MouseStopLineW,      // line number where mouse rested
             MouseStopColW;       // Column number where mouse rested
     BOOL    MouseStopTimerOnW,   // TRUE mouse stop timer is on
             MouseStopMsgOnW,     // mouse stop message is displaying
             MouseStopDoneW;      // mouse stop activity done of this location

  /******************************************************************************
     Word Wrapping related variables
  *******************************************************************************/
     WORD    DocCodeW,           // used to identify a word wrap file
             DocBeginW;          // Document header size or text begin location

     long    WrapMaxLinesW,      // Maximum lines for the wrap buffer
             WrapAddLinesW;      // lines to add to the array during wrap

     int     WrapBufferSizeW;    // wrap buffer size

     BYTE    ParaCharW;          // a character to mark end of a paragraph
     BYTE    OldParaCharW;       // a character to mark end of a paragraph (old)
     BYTE    DocExtW[5];         // extension of a document file

     LPBYTE   wrapW;              // pointer to the wrap buffer

     HANDLE  hWrapW;             // handle to the wrap buffer

     int     BufferLengthW,      // current length of a wrap buffer
             MaxBufferLengthW;   // max length of a wrap buffer

     int     WrapFlagW;          // indicates the extent of word wrapping for the current operation

     long    LastBufferedLineW,  // last line in the wrap buffer
             LastWrappedLineW;   // lines used up the wrap buffer

     int     WrapWidthCharsW,    // number of characters to wrap at
             WrapWidthTwipsW;    // number of twips to wrap at

     BOOL    DoExtraPassW;       // do an extra pass during reapgination

  /*****************************************************************************
                   Character Tag Id variables
  ******************************************************************************/
     struct StrCharTag {
        BOOL   InUse;            // TRUE if in use
        LPBYTE name;             // tag name
        long   type;             // tag type TAG_
        LPBYTE HtmlInfo;         // information used by html add-on
        LPBYTE AuxText;          // auxiliary text information
        DWORD  AuxInt;           // auxiliary numeric infor
        UINT   next;             // id of the next tag, 0 if it is the last tag in the sequence
        long   line;             // line number where the tag appears
        BYTE   reserved[16];     // reserved bytes
     } huge *CharTagW;

     LPWORD  WrapCtidW;          // pointer to the character miscellaneous id wrap buffer
     LPWORD  WrapCharWidthW;     // pointer to the character width wrap buffer
     LPINT   WrapCharWidthOrderW;  // pointer to the character width order buffer
     LPINT   WrapCharWidthDXW;   // pointer to the character width DX buffer used by GetCharacterPlacement function
     LPWORD  WrapCharWidthTextW; // pointer to the character width unicode text buffer used by GetCharacterPlacement function
     LPBYTE  WrapCharWidthClassW; // pointer to the text type buffer used by GetCharacterPlacement function
     LPBYTE  WrapLeadW;          // pointer to the character miscellaneous id wrap buffer
     int     TotalCharTagsW,     // number of entries in the character tag information table
             MaxCharTagsW;       // max number of allocated char tags
     BOOL    TagsWrappedW,       // TRUE if the tags are wrapped during wordwrapping
             LeadWrappedW,       // TRUE if the lead bytes are wrapped during word wrapping
             CharWidthWrappedW;  // TRUE if the charactet width are wrapped during word wrapping

  /*****************************************************************************
                   Character Formatting Variables
   The character formatting option provide underline, bold and italic attributes.
  ******************************************************************************/
     BYTE CfmtSignW[30];             // indicates the beginning of the formatting information

     LPWORD  WrapCfmtW;              // pointer to the character formatting wrap buffer
     LPWORD  TempCfmtW;              // pointer to temporary formatting info
     LPWORD  pLineCharWidthW;        // Line character width
     WORD    LineCfmtW[2*MAX_WIDTH+1]; // temp line cfmt
     BYTE    LineTextW[2*MAX_WIDTH+1]; // temp line text
     BYTE    LineLeadW[2*MAX_WIDTH+1]; // temp line lead text

     HANDLE hWrapCfmtW;              // handle to the character formatting wrap buffer
     int    CurFmtIdW;               // current format id in the device context
     int    InputFontIdW;            // font selected for next input

  /******************************************************************************
                    Paragraph formatting variables
  ******************************************************************************/
     HANDLE   hPfmtW;             // handle to the paragraph formatting info
     int      huge *pfmtW;          // paragraph formatting data for each line

     HANDLE   hPfmtIdW;           // handle to the paragraph formatting table

     struct StrPfmt {
           int LeftIndent;       // left indentation in screen units
           int RightIndent;      // Right indentation in screen units
           int FirstIndent;      // First line indentation in screen units
           int LeftIndentTwips;  // left indent in twips
           int RightIndentTwips; // Right indent in twips
           int FirstIndentTwips; // First line indent in twips
           UINT flags;           // includes such flags as double spacing,centering, right justify etc
           int TabId;            // tab id, index into the tab table
           int BltId;            // bullet id
           int StyId;            // style id associated with the para
           UINT shading;         // shading (0 to 10000)
           UINT pflags;          // more para flags: PFLAG_?
           int SpaceBefore;      // space before the paragraph in twips
           int SpaceAfter;       // space after the paragraph in twips
           int SpaceBetween;     // space between the paragraph lines in twips
           int AuxId;            // id for use by external application
           DWORD Aux1Id;         // id for use by external application
           COLORREF BkColor;     // background color
           int BorderSpace;      // border space for boxed paragrah in twips
           int LineSpacing;      // variable line spacing (0 single line spacing).
           int flow;             // FLOW_ constants
           COLORREF BorderColor; // border color (CLR_AUTO by default)
     } far *PfmtIdW;

     int TotalPfmtsW,            // total number of para ids
         MaxPfmtsW;              // current max pfmt allocated

     int CurPfmtW;               // current para id

     int CurWrapPfmtW;           // para id of para being wrapped
     int WrapParaFontW;          // paragraph font for the paragraph being wrapped

  /******************************************************************************
                       Paragraph Bullet/numbering parameters
  ******************************************************************************/
     struct StrBlt {                // bullet/page numbering
          BOOL IsBullet;            // TRUE if it is a bullet
          UINT flags;               // BLTFLAG_? flags
          int  start;               // starting number
          int  level;               // paragraph level
          int  NumberType;          // decimal BLTNBR_ variables
          int  font;                // BFONT_ font constants
          BYTE BulletChar;          // bullet character
          BYTE BefChar;             // character before the number
          BYTE AftChar;             // character after the number
          BYTE BefText[MAX_NUM_TEXT_LEN+1];  // before text
          int  ls;                  // an index into ListOr table
          int  lvl;                 // 0 or an index into ListOr->level table 

          #if defined(WIN32)
             BYTE reserved[6];
          #else    
             BYTE reserved[10];     // reserved bytes
          #endif

     } far *TerBltW;                // bullet structure
     int    TotalBltsW,             // total entries in the TerBlt structure
            MaxBltsW;               // max entries allocated

  /******************************************************************************
                       Paragraph List tables
  ******************************************************************************/
     struct StrList {
       BOOL InUse;                 // TRUE when the slot is in use
       long id;                    // list id
       long TmplId;                // list templaet id
       int  FontId;                // font id used to display this information
       int  LevelCount;            // number of levels (1 or 9)
       DWORD flags;                // LISTFLAG_ flags
       LPBYTE name;                // list name (MemAlloc)
       struct StrListLevel far *level;  // an array of list level to accomodate ListCount levels
       int reserved[10];          // reserve 20 bytes
     } far *listW;                 // list table pointer

     int  TotalListsW,             // total entries in the list table
          MaxListsW;               // max entries in the list table

     struct StrListOr {
       BOOL InUse;                  // TRUE when the slot is in use
       int  ListIdx;                // index into the list table
       int  LevelCount;             // 0 or 9 (9 only when the corresponding list has 9 levels)
       struct StrListLevel far *level;  // an array of list level to accomodate ListCount levels
       int  RtfLs;                 // ls value during rtf read
       UINT flags;                 // LISTOR_ flags
       int  reserved[9];           // reserve 18 bytes
     } far *ListOrW;               // list override table pointer

     int  TotalListOrW,            // total entries in the list override table
          MaxListOrW;              // max entries in the list override table

     struct StrListLevel {         // list level data structure
        int start;                 // start-at value
        int NumType;               // LIST_ constants
        int CharAft;               // LISTAFt_ constants
        int LeftIndent;            // left indent in twips
        int RightIndent;           // righr indent in twips
        int FirstIndent;           // first indent in twips
        UINT ParaFlags;            // paragraph flags
        UINT FontStyles;           // font styles
        int  FontId;               // font id
        BYTE text[50];             // bullet text template
        DWORD flags;               // LISTLEVEL_ flags
        int  MinIndent;            // minimum distance from number to text
        UINT FontStylesOff;        // turned off font styles
        int reserved[8];           // reserve 20 bytes
     } LISTLEVEL;                            

  /******************************************************************************
                       Paragraph Border and spacing parameters
  ******************************************************************************/
     int  BoxLeftW,                    // position of the left edge of the paragraph box
          BoxRightW;                   // position of the right edge of the paragraph box

     UINT DlgOnFlagsW,                 // box parameters turned on by the dialog box
          DlgOffFlagsW;                // box parameters turned off by the dialog box

     DWORD DlgLongW,                   // temporary variable to pass a long value
           DlgLong1W,                  // second temporary variable
           DlgLong2W,                  // second temporary variable
           DlgLong3W;                  // second temporary variable

     COLORREF DlgColor1W,              // temporaray color variables
              DlgColor2W,
              DlgColor3W,
              DlgColor4W;

     BYTE  DlgText1W[MAX_WIDTH+1],     // temporary holding place to pass dialog field values
           DlgText2W[MAX_WIDTH+1];     // temporary holding place to pass dialog field values

     int   DlgInt1W,                   // temporary integer variables to pass result
           DlgInt2W,
           DlgInt3W,
           DlgInt4W,
           DlgInt5W,
           DlgInt6W;

     LPVOID DlgPtr1W,                  // first pointer
            DlgPtr2W;                  // second parameter passing pointer

     BOOL  DlgCancelW;                 // cancel result from the dialog box

     int   SpaceBefW,                  // space before the paragraph
           SpaceAftW,                  // space after the paragraph
           ParaSpcBefW,                // paragraph space before
           ParaSpcAftW;                // paragraph space after

  /******************************************************************************
                       Font and Picture Control Data
  ******************************************************************************/
     BYTE   FmtSignW,         // indicates the beginning of a format data section
            FmtSignOldW;      // old value (version 3.5)

     struct StrFont {         // main font table
              //******* Font Variables *********
              HFONT   hFont;      // font handle
              int     GlbFontId;  // global font id, index into GlbFont table
              BYTE    TypeFace[32];//font typeface
              BYTE    FontFamily; // font famiy (optional)
              BYTE    CharSet;    // character set
              int     TwipsSize;  // display point size of the font in twips units
              DWORD   TextColor;  // Color of the text
              DWORD   TextBkColor;// text background color
              DWORD   UlineColor; // underline color
              int     CharStyId;  // Character Style sheet id associated with the font
              int     ParaStyId;  // Paragraph Style sheet id associated with the font
              int     expand;     // expansion or compression (negative) in twips
              int     lang;       // language id
              int     offset;     // character offset from the base in twips
              int     OffsetVal;  // character offset in device units
              LPBYTE  FieldCode;  // Field format information 
              int     TextAngle;  // text flow angle
              //******* Object Variables
              #if defined(NO_OLE)
                LPVOID  pObject;
              #else
                LPOLEOBJECT pObject;// pointer to the object
              #endif
              int     ObjectType; // OBJ_EMBED, OBJ_LINK, OBJ_NONE
              DWORD   ObjectSize; // size of the object storage data
              WORD    OleNotice;  // notification from the callback funciton
              int     ObjectAspect; // OASPECT_ constants
              DWORD   CacheConnectId;  // ole cache connection id
              BOOL    ObjectUpdate; // Update on initial update
              LPBYTE  OleStgName;  // ole storage name
              HGLOBAL hObject;    // temporary memory containing object data
              #if !defined(NO_OLE)
                #if !defined(INC_OLE1)
                  LPOLECLIENTSITE pObjSite; // Client site pointer
                  LPSTORAGE pObjStg;        // object storage
                #endif
              #endif
              HWND    hWnd;       // control window handle

              //****** Picture Variables ********
              int     PictType;   // PICT_DIBITMAP (device ind. bitmap) or PICT_METAFILE
              HBITMAP hBM;        // bitmap handle if style&PICT is TRUE and PictType=PICT_DIBITMAP
              HMETAFILE hMeta;    // bitmap handle if style&PICT is TRUE and PictType=PICT_METAFILE
              HENHMETAFILE hEnhMeta; // bitmap handle if style&PICT is TRUE and PictType=PICT_ENHMETAFILE
              DWORD   ImageSize;  // size of the device independent bitmap image
              DWORD   InfoSize;   // size of the device independent bitmap information structure
              LPBYTE  pImage;     // pointer to the device independent bitmap image
              LPBYTE  pInfo;      // pointer (OurAlloc) to the device independent bitmap info structure
              int     bmHeight;   // actual height of the stored bitmap
              int     bmWidth;    // actual width of the stored bitmap
              int     PictHeight; // display height in twips
              int     PictWidth;  // display width in twips
              DWORD   ColorsUsed; // number of colors used in the bitmap
              int     PictAlign;  // picture vertical alignment value, uses ALIGN_ constants
              int     FrameType;  // picture frame type, PFRAME_ constants
              int     ParaFID;    // para frame id when FrameType is not PFRAME_NONE
              int     PictX;      // latest x location on the screen
              int     PictY;      // latest y location on the screen
              struct StrRgb far *rgb;  // most used color table for 24 bit RGB bitmaps
              LPBYTE  LinkFile;   // linked file name
              LPBYTE  PictAlt;    // alternate picture name for html output
              int     TotalRgbs;  // number of colrs in the rgb table above
              HPALETTE hPal;      // palette if any used by the bitmap
              int     PctWidth;    // picture width in percentage of window width
              struct  StrAnim far *anim;  // picture animation information
              int     DispFrame;  // current dspay frame number
              int     MapId;      // picture map id
              int     OrigPictType; // original picture type
              DWORD   OrigImageSize;  // size of the original image
              LPBYTE  pOrigImage;  // image data for the original image
              int     CropLeft;    // left cropping - twips (not affected by resizing)
              int     CropRight;   // right cropping - twips
              int     CropTop;     // top cropping - twips
              int     CropBot;     // bottom cropping - twips
              int     OrigPictWidth;  // original picture width in twips
              int     OrigPictHeight; // original picture height in twips

              //****** Track-changes Variables *********
              int     InsRev;     // Reviewer id for the inserted text
              DWORD   InsTime;    // time inserted
              int     DelRev;     // Reviewer id for the deleted text
              DWORD   DelTime;    // time deleted

              //****** Common Variables *********
              int     height;     // point size converted to pixel height for display
              int     BaseHeight; // baseline to the top of the character/picture
              int     BaseHeightAdj; // adjustment to base height before writing
              UINT    style;      // font styles:  bold,underline,italic,strikeout, or PICTURE
              UINT    TempStyle;  // temporary runtime styles
              BOOL    InUse;      // TRUE indicates that this font/picture is in use in the current document
              LPINT   CharWidth;  // stores the character width of each style
              int     FieldId;    // field id, 0 for default
              int     AuxId;      // auxiliary id for sole use of outside application
              int     Aux1Id;     // another auxiliar id.  This is not saved in the output file
              UINT    flags;      // FFLAG_ constants
              int     ExtLead;    // external leading space for the device
              int     RtfIndex;   // rtf output index
              struct StrHdnFont far *hidden;  // information for hidden styles
              WORD    UcBase;     // Unicode base, actual character value is UcBase+Char
              BOOL    VarWidth;   // TRUE if this font uses context-sensitive width for the characters
              BOOL    rtl;        // TRUE if this font uses right-to-left character flow
              int     CharId;     // CHARID_ special character id
              } far *TerFontW;    // pointer to the font structure

     struct StrPrtFont {          // printer font metrics translation structure
              HFONT   hFont;      // font handle
              int     GlbFontId;  // global font id, index into GlbFont table
              int     height;     // point size converted to pixel height for display
              int     BaseHeight; // baseline to the top of the character/picture
              LPINT   CharWidth;  // stores the character width of each style
              int     ExtLead;    // External Lead space for the device
              int     OffsetVal;  // character offset in device units
              struct StrHdnFont far *hidden;  // information for hidden styles
              } far *PrtFontW;    // pointer to the font structure

     int      TotalFontsW,        // Total fonts in use by the current window
              TotalTypesW,        // number of enumerated font face types
              TotalPointSizesW;   // number of enumerated pointsizes
     BYTE     ReqTypeFaceW[32];   // requested type face
     int      ReqTwipsSizeW;      // requested font size

     BOOL     UsingZoomFontsW,    // use and create zoom fonts
              HasVarWidthFontW;   // document has context-sensitive fonts

     int      NextFontIdW,        // next picture id to use
              MaxFontsW;          // maximum number of allocated fonts
     int      NextFontAux1IdW;    // next font aux1 id to be used with TerCreateFont function
     DWORD    NextParaAux1IdW;    // next para aux1 id to be used with TerCreateParaId function

     WORD huge *CharScrFontIdW;   // stores the font id for the character cache
     WORD huge *CharPrtFontIdW;   // stores the font id for the character cache
     WORD huge *CharScrWidthW;    // stores the screen width for the character cache
     WORD huge *CharPrtWidthW;    // stores the printer width for the character cache
     BYTE     LinkPictDirW[MAX_WIDTH]; // linked picture directory

  /******************************************************************************
     Tab control control variables
  *******************************************************************************/
     struct StrTab far *TerTabW;  // pointer to the tab table

     HANDLE   hTerTabW,           // handle to the StrTab table
              hTabwW;             // handle to the StrTabw table

     int      TotalTabsW,         // total tab entries used in the tab table
              DefTabWidthW,       // default tab width in twips
              DefTabTypeW;        // default tab type

  /******************************************************************************
                       Style sheet parameters
  ******************************************************************************/
     struct StrStyleId {
        BOOL InUse;              // TRUE when this slot is in use
        BYTE name[MAX_SS_NAME_WIDTH+1];  // style name
        int  type;               // type SSTYPE_CHAR or SSTYPE_PARA
        BYTE OldTypeFace[31];    // font type face
        BYTE FontFamily;         // font family
        int  TwipsSize;          // twips size of the font
        UINT style;              // character style bits
        COLORREF TextColor;      // text color
        COLORREF TextBkColor;    // text background color
        int LeftIndentTwips;     // left indent in twips
        int RightIndentTwips;    // Right indent in twips
        int FirstIndentTwips;    // First line indent in twips
        UINT ParaFlags;          // includes such flags as double spacing,centering, right justify etc
        UINT shading;            // shading (0 to 10000)
        int SpaceBefore;         // space before the paragraph in twips
        int SpaceAfter;          // space after the paragraph in twips
        int SpaceBetween;        // space between the paragraph lines in twips
        int TabId;               // tab id
        UINT flags;              // SSFLAG_ constants
        int  LineSpacing;        // line spacing (0 for single line spacing)
        int  next;               // style for the next paragraph
        int  RtfIndex;           // Used temporarily during rtf output
        UINT pflags;             // includes para PFLAG_ constants
        int  BltId;              // bullet id
        COLORREF ParaBkColor;    // background color for the paragraph
        int  expand;             // character space expansion/contraction
        BYTE TypeFace[32];       // typeface
        int  HeadingNo;          // temporary variable - heading number 1 to 9 for heading styles, 0 for non-heading styles
        int  OutlineLevel;       // outline level
        COLORREF UlineColor;     // underline color
        COLORREF ParaBorderColor;// paragrpah border color
        int  offset;             // character offset
        BYTE reserved[28];       // reserved for future use
     } far * StyleIdW, PrevStyleIdW;

     HGLOBAL hStyleIdW;             // handle to the above table

     int CurSIDW,                // current stylesheet id being edited, (-1 when a style sheet not being edited)
         TotalSIDW,              // total ids used in the table
         MaxSIDW;                // current maximum ids allcated in the table

     BOOL EditingParaStyleW;     // TRUE when a paragraph style is being edited

  /******************************************************************************
                       Change Tacking varaibels
  ******************************************************************************/
      bool      TrackChangesW;     // True to track changes
      int       TrackRevW;         // tracking revierwer id
      int       TrackTimeW;        // tracking begin time

  /******************************************************************************
     Section control variables
  *******************************************************************************/
     struct StrSect {             // section table
              BOOL  InUse;        // TRUE when this slot is in use
              float LeftMargin;   // left margin in inches
              float RightMargin;  // right margin in inches
              float TopMargin;    // top margin in inches
              float BotMargin;    // bottom margin in inches
              int columns;        // number of columns
              float ColumnSpace;  // space between column in inches
              long FirstLine;     // first line of the section
              long LastLine;      // last line of the section
              int  orient;        // paper orientation: DMORIENT_ constant
              UINT flags;         // flags SECT_?
              int  LastPage;      // last page number of the section
              int  PageNumFmt;    // page number format, NBR_ constants
              short FirstPageNo;   // first display page number
              float HdrMargin;    // header margin
              float FtrMargin;    // footer margin
              short FirstPageBin; // bin for the first page, 0 or DMBIN_ constant
              short bin;          // bin for the remaining pages, 0 or DMBIN_ constant
              UINT  border;       // BORDER_ constants
              int   BorderWidth[4]; // border width in twips
              int   BorderSpace[4]; // space around the border in twips 
              UINT  BorderOpts;   // page border options PGBORDER_ flags
              COLORREF BorderColor; // border color
              int   BorderType;   // Border type BRDRTYPE_ constants
              int   PprSize;      // one of DMPAPER_ constants
              float PprWidth;     // paper width in portrait orientation
              float PprHeight;    // paper height in portrait orientation
              int   flow;         // FLOW_ constants
              int   AuxId;        // Auxiliary information used by the calling application
              BYTE  reserved[16]; // reserved btyes
              } far *TerSectW;    // pointer to the section table

     struct StrSect1 {            // temporary section table
              int   PrevSect;     // previous section number
              int   NextSect;     // next section number
              struct StrHdrFtr hdr;  // header information
              struct StrHdrFtr ftr;  // footer information
              struct StrHdrFtr fhdr; // first page header information
              struct StrHdrFtr fftr; // first page footer information
              int   HiddenX;      // hidden x space for the current printer
              int   HiddenY;      // hidden y space for the current printer
              float PgHeight;     // page height in inches after considering orientation
              float PgWidth;      // page height in inches after considering orientation
              int  LastPageHeight;// height of the text on the last page
              } far *TerSect1W;   // pointer to the section table

     int      TotalSectsW,        // total section entries used in the section table
              WrapSectW,          // section being wrapped currently
              MaxSectsW;          // maximum section allocated

     BOOL     SectModifiedW;      // TRUE when section boundary may have been modified

     int      KnownSectW;         // for optimization in GetSection
     long     KnownSectBegLineW,  // begin line for the know section
              KnownSectEndLineW;  // end line for the known section

     long      SectLineW;          // current section line number

  /******************************************************************************
     Screen page break and repagination variables
  *******************************************************************************/
     int      TotalPagesW,        // total number of pages
              MaxPagesW,          // size of the page array
              PrevTotalPagesW,    // Previous total pages used for determining the length of the page number and page count fields
              PageModifyCountW,   // number of times document modified
              CurPageW,           // current page number
              OverrideTotalPagesW;// override for the PageCount field

     long     RepageBeginLineW;   // repagination begin line
     BOOL     repaginatingW,      // TRUE during the repagination process
              LastPageCreatedW,   // TRUE when pagination creates the last page
              RepagePendingW;     // TRUE when repagination was suspending because of pending keystrokes

     struct StrPage {
        long  FirstLine;          // first line of the page
        long  LastLine;           // last line of the page
        //int   TopRow;             // row id of the top partial row
        //int   BotRow;             // row id of the bottom partial row
        //int   TopRowHt;           // height of the top partial row in printer units
        //int   BotRowHt;           // height of the bottom partial row in printer units
        int   FirstRow;           // first row in the page
        int   LastRow;            // last row in the page
        short DispNbr;            // page number for display
        int   ScrHt;              // screen height of the page
        int   TextHt;             // text height of the page in screen units
        int   BodyHt;             // height of the body text in printer units
        int   BodyTextHt;         // text height of the body text in printer units
        int   FnoteHt;            // height (in prt unit) of the footnote area
        UINT  flags;              // PAGE_ constants
        int   TopSect;            // top section of the page
        int   TblHdrHt;           // table header height in prt unit, 0 if no table header
        long  TblHdrFirstLine;    // first line of the table header rows
        long  TblHdrLastLine;     // last line of the table header rows
        int   FrameCount;         // number of positionable frames found in the page
     } huge *PageInfoW;

  /******************************************************************************
     Page Number Box display variables
  *******************************************************************************/
     BOOL     PageBoxShowingW;    // TRUE when the page box is showing during vertical scroll
     int      BoxPageW;           // page number in the page box
     RECT     PageBoxRectW;       // page box rectangle

  /******************************************************************************
     Print control variables
  *******************************************************************************/
     float   PageWidthW,         // width of the page in inches
             PageHeightW,        // height of the page in inches
             PvWidthW,           // width of the preview area in inches
             PvHeightW;          // height of the preview area in inches

     int     PageXW,             // X position where the text begin on the page
             PortraitHXW,        // hidden x value in portrait orient
             PortraitHYW,        // hidden y value in portrait orient
             LandscapeHXW,       // hidden x value in landscape orient
             LandscapeHYW;       // hidden y value in landscape orient

     HDC     hPrtDataDCW;        // DC to send the printer data to

     BOOL    AbortPrintW,        // set to TRUE to abort printing
             InPrintingW,        // TRUE when the editor is in printing process
             InPrintPreviewW,    // TRUE when the editor is in print preview process
             ExtPrintPreviewW,   // TRUE when external print preview on
             GoPrintPreviewW,    // TRUE to go back to print preview
             PrinterAvailableW,  // TRUE when a printer is avaialbe
             PrtDiffResW;        // TRUE if the printer has different resolutin for x and y

     int     PaperSizeW,         // DMPAPER_ WIN SDK constants
             PaperOrientW;       // DMORIENT_ WIN SDK constants

     BYTE    PrinterNameW[MAX_WIDTH+1],   // name of the printer
             PrinterDriverW[MAX_WIDTH+1], // printer driver
             PrinterPortW[MAX_WIDTH+1];   // printer port

     LPDEVMODE pDeviceModeW;     // pointer to the device mode storage
     HANDLE    hDeviceModeW;     // handle to device mode memory

     HWND      hAbortWndW;       // abort window handle

     int     TotalPreviewPagesW; // number of preview pages to display
     int     PreviewZoomW,       // zoom percent for print preview
             PreviewHeightW,     // height of the preview area
             PreviewWidthW;      // width of the preview area

     BOOL    SavePvStatusW,      // save the show status flag
             SavePvRulerW,       // save the ruler status
             SavePvToolbarW,     // save the tool bar status
             SavePvWordWrapW,    // save the word wrap flag
             SavePvPrintViewW,   // save the print view mode flag
             SavePvPageModeW,    // save the page mode flag
             SavePvPageBorderW,  // save the page border flag
             SavePvFittedViewW,  // save the fitted mode flag
             SavePvShowHorBarW,  // horrizontal scroll bar display flag
             SavePvOrgXW,        // save the x window origin
             SavePvOrgYW,        // save the y window origin
             SavePvViewPageHdrFtrW, // save the hdr/ftr view status
             ShowPvToolbarW,     // show the tool bar in the print preview mode
             SavePvToolBarHeightW; // save the toolbar height
     
     int     SavePvFirstFramePageW,  // first frame page
             SavePvLastFramePageW;   // last frame page

     WNDPROC ZoomSubclassProcW,  // handle to zoom bar subclass process
             OrigZoomSubclassProcW;// original zoom bar control process

     SIZE    PvViewportW;        // view port dimension during preview

     int     SavePrtFontHeightW, // font height of the printer font just created
             SavePrtPitchFamilyW, // Pitch/family for the printer font just created
             SavePvScrResXW,     // save the screen resolution
             SavePvScrResYW;     // save the screen resolution

     BOOL    SavePrtIsTrueTypeW;  // Truetype flag for the printer font just created

     COLORREF PrtTextBkColorW;    // text background color during printing
     COLORREF PrtParaBkColorW;    // para background color during printing
     COLORREF PrtFrameBkColorW;   // frame background color during printing

     RECT     MPRectW;             // merge print rectangle

     int      OverrideBinCopyW;   // copy to override bin, set to 0 for no overide
     int      OverrideBinW;       // override bin number
          
  /******************************************************************************
     Cursor Adjustment variables
  ******************************************************************************/
     long      PrevCursLineW;   // previous cursor line

     int       PrevCursColW,    // previous cursor column
               PrevCursPageW,   // previous cursor page
               PrevCursLineYW,  // previous cursor Y position (including TerWinOrgX)
               CursDirectionW;  // direction of cursor movement

  /******************************************************************************
     OLE variables
  ******************************************************************************/
     #if !defined(NO_OLE)
        #if defined (INC_OLE1)
           struct StrClient {
              OLECLIENT     client;      // ole client
              void far *    TerData;
           } TerClientW;

           OLECLIENTVTBL TerClientVtblW; // contains the pointer to call back function
           LHCLIENTDOC   lhDocW;         // handle to current document
           OLE_RELEASE_METHOD OleReleaseMethodW; // Ole release method
        #else
           LPSTORAGE     pDocStorageW;   // top level storage
        #endif

        struct StrOleStream {
           OLESTREAM     stream;      // ole stream
           void far *    TerData;
           DWORD         offset;      // offset into buffer
           DWORD         MaxSize;
        } OleStreamW;

        OLESTREAMVTBL OleStreamVtblW; // virtual table containing ole streams

        OLECLIPFORMAT NativeClipFormatW,      // Native format
                      ObjectLinkClipFormatW,  // Object link format
                      OwnerLinkClipFormatW;   // Owner link format
     #else
        UINT          NativeClipFormatW,      // Native format
                      ObjectLinkClipFormatW,  // Object link format
                      OwnerLinkClipFormatW;   // Owner link format
     #endif

     BOOL          WaitForOleW;    // Wait for OLE asynch task to be over
     HGLOBAL       hOleDataW;      // Handle to the OLE file io buffer
     BYTE huge *   pOleDataW;      // pointer to the OLE file io buffer
     int           CurObjectW,     // current object index
                   ActiveOleObjW;  // active ole object
     UINT          RtfClipFormatW, // Rich text format clipboard format
                   SSClipInfoW,    // clipboard data information block
                   CfEnhMetafileW; // enhanced metafile format

     BYTE          NoDropListW[MAX_WIDTH+1];   // list of file extensions that should not be dropped in to TE

  /******************************************************************************
     Color control variables
  ******************************************************************************/
     COLORREF   TextDefBkColorW, // default background color for text
                TextDefColorW,   // default text color
                StatusBkColorW,  // background color of the status line
                StatusColorW,    // foreground color of the status line
                PageBorderColorW,// page border color
                PageBkColorW,    // page background color
                TextBorderColorW,// color of the border around the text window
                ParaBoxColorW,   // color of the current paragraph border

                CurForeColorW,   // current foreground color
                CurBackColorW;   // currnet background color

     int        FrameShadingW;   // Current shading percent 0 to 100
     int        MaxRtfColorsW;   // maximum rtf colors allocated

     HBRUSH     hToolbarBrushW;  // toolbar painting brush

  /******************************************************************************
     Block highlighting variables
  ******************************************************************************/
     long       HilightBegRowW, // beginning row
                HilightEndRowW, // ending row
                DblClickEndRowW; // end row of double click word

     int        HilightTypeW,   // line/col or character block
                HilightBegColW, // beginning col
                HilightEndColW, // ending col
                DblClickEndColW, // end col of double clicked word
                MaxColBlockW;   // biggest column block allowed

     BOOL       StretchHilightW,// stretch hilighting mode
                FullCellHilightW, // TRUE when the cell being painted is fully hilighted
                HilightWithColCursorW,  // hilight using the column cursor
                HilightTimerOnW,   // resume hilight scrolling
                HilightAtCurPosW,// TRUE to exactly match hilight end pos to cur pos when the stretching hilighting through keystroke
                DblClickHilightW; // hilight begun with double click
  
  /******************************************************************************
     Tool Bar variables
  *******************************************************************************/
     int        ToolBarHeightW;            // height of the tool bar
     HWND       hToolBarWndW;              // handle to the tool bar window
     HWND       hPvToolBarWndW;            // handle to the print preview tool bar window
     FARPROC    lpToolBarW;                // handle to the tool bar dialog process


     int        ToolBarCfmtW;              // tool bar font id
     int        ToolBarPfmtW;              // tool bar para id
     int        ToolBarSIDW;               // tool bar style id

     WNDPROC    ToolBarComboProcW,         // handle to font subclass process
                OrigBarFontProcW,          // original font process
                OrigBarPointProcW,         // original point size edit control process
                OrigBarStyleProcW,         // original point size edit control process
                OrigBarZoomProcW;          // original point size edit control process

  /******************************************************************************
     Search/Replace Variables
  *******************************************************************************/
     BYTE         SearchStringW[MAX_WIDTH+1], // last string searched
                  SearchDirectionW,      // F'orward, B'ackward, E'ntire file
                  ReplaceStringW[MAX_WIDTH+1],// last string changed
                  ReplaceWithW[MAX_WIDTH+1];  // string to be changed with

     BOOL         ReplaceVerifyW,        // verify before replace
                  ReplaceBlockW;         // Replace range: block or file

     UINT         SearchFlagsW;          // Search flags

  /******************************************************************************
     Undo variables
  *******************************************************************************/
     struct StrUndo {
       HANDLE       hText,               // handle to the undo text information
                    hCfmt,               // handle to the undo formatting info
                    hPfmt,               // handle to the undo paragraph info
                    hLead;               // undo lead buffer

       long         beg,                 // undo block absolute coordinates
                    end;
       BYTE         type;                // type of undo: delete,insert or replace
       int          id;                  // undo operation id to tie more than one undo to one operation
       int          TblLevel;            // table level of saved data
       BOOL         EmbTable;            // undo has embedded tables
       
       int          ObjId;               // an id of an object to be undone
       int          width;               // width of an object
       int          height;              // height of an object
       struct StrParaFrame *pFrame;      // paraframe to be undone

       struct StrLinePtr **LinePtrU;     // line array object
       long         TotalLinesU;         // total lines in the line array
       long         MaxLinesU;           // max lines in the line array
       long         CursPos;             // cursor position in the line array

       int          RowCount;            // number of rows in the undo row table
       int          CellCount;           // number of cells in the undo row table
       LPINT        RowId;               // row ids saved
       LPINT        CellId;              // cell ids saved
       struct StrTableRow *pRow;         // undo row table - stores the modified row (MemAlloc)
       struct StrCell     *pCell;        // undo cell table                          (MemAlloc)

     } far *undoW;

     int          UndoCountW;             // text exist for undoing
     int          UndoTblSizeW;           // size of the undo table
     int          UndoRefW;               // undo reference count
     BOOL         InUndoW;                // TRUE when undoing
     int          CurUndoTypeW;           // current type being undone
     int          MaxUndosW;              // max undos allocated
     int          MaxUndoLimitW;          // max undos allowed - scrolling begins when undo exceed this number
     int          UndoSkipRefW;           // undo reference to be skipped (not saved)
     int          UndoInt1W;              // auxiliary undo variable
     int          UndoInt2W;              // auxiliary undo variable
     struct StrParaFrame UndoParaFrameW;  // a copy paraframe to be used with next SaveUndo

  /******************************************************************************
     Hidden/footnote/field text variables
  *******************************************************************************/
     BOOL         ShowHiddenTextW,       // show hidden text
                  ShowFieldNamesW,       // show field names
                  EditFootnoteTextW,     // Show footnote text
                  EditEndnoteTextW,      // Show footnote text
                  InFootnoteW,           // true when positioned within footnote text
                  EndnoteAtSectW;        // end note at the end of the section

     BYTE         FootnoteRestW[20];     // remaining part of the footnote marker

     int          FootnoteRestFontW;     // font id associated with footnote rest
     int          FootnoteNumFmtW;       // footnote number format, NBR_ constants
     int          EndnoteNumFmtW;        // endnote number format, NBR_ constants

  /******************************************************************************
     VBX/OCX Variables
  *******************************************************************************/
     BOOL         VbxControlW,           // TRUE if it is a VBX control
                  VbxMessageProcessedW;  // TRUE when VBX message processed successfully.

     VBX_CALLBACK VbxCallbackW;          // routine to recieve TER messages in a VBX
     MSG_CALLBACK MsgCallbackW;          // routine to recieve TER messages in non-vbx application
     void far *   ParentPtrW;            // parent pointer

  /******************************************************************************
     Object drag Variables
  *******************************************************************************/
     struct StrDragObj {                // drag object table
        BOOL      InUse;                // TRUE if the entry in use
        BOOL      drawn;                // object drawn
        int       type;                 // object type
        int       id1;                  // primary object identification
        int       id2;                  // secondary object identification
        int       id3;                  // another object identification

        POINT     ObjPoint[6];          // polygon to be dragged. Other than fist two points are static when strech = FALSE
        int       ObjPointCount;        // number of points in the drag polygon
        RECT      ObjRect;              // some object migh use rect istead of polygon

        RECT      HotRect[4];           // hotspot rectangles
        int       HotRectCount;         // number of hot rects in use
        BOOL      IsHotPolygon;         // left/top points in the HotRect are actually a polygon points
        BOOL      TextHilighted;        // TRUE if the text was selected when the opject was being moved
        double    AspectRatio;          // original aspect ratio (width/height)
     } far * DragObjW;                  // table pointer

     HGLOBAL      hDragObjW;            // drag object table handle

     int          CurDragObjW;          // current drag object
     int          CurHotSpotW;          // current hot spot within the drag object
     int          TotalDragObjsW;       // total drag objects on the screen
     int          MaxDragObjsW;         // current maximum drag objects

     BOOL         PictureHilightedW;    // TRUE when a picutre object is highlighted
     BOOL         FrameTabsHilightedW;  // TRUE when a frame size tabs are highlighted
     BOOL         FrameRectHilightedW;  // TRUE when a frame rectangle is highlighted
     BOOL         PictureClickedW;      // TRUE when a picutre object is clicked
     BOOL         FrameClickedW;        // TRUE when a frame object is clicked

  /******************************************************************************
     Table Variables
  *******************************************************************************/
     struct StrTableRow {               // Table structure
        BOOL      InUse;                // TRUE if the entry in use
        int       FixWidth;             // Used by html to create a table with fixed width, minus values specify the percent of the current window width
        int       FirstCell;            // first cell id for this row
        int       LastCell;             // last cell id for this row
        int       FrmSpcBef;            // Frame Space before the row in printer units (runtime use only)
        int       height;               // Row height during display or printing
        BOOL      PrevRow;              // Previous row of the table, -1 if the first row
        BOOL      NextRow;              // Next row of the table, -1 if the last row
        int       indent;               // left indentation in twips
        UINT      flags;                // RIGHT, CENTER or ROWFLAG_? flags
        int       CellMargin;           // space between the cells
        UINT      border;               // uses BORDER_ constants
        int       BorderWidth[4];       // border width for each border
        int       FirstFrame;           // first frame for the row
        int       LastFrame;            // last frame for the row
        int       AddedIndent;          // The adjustment by our rtf reader to the indentation
        int       MinHeight;            // minimum height of the row in twips
        int       CurIndent;            // current indentation including the frame displacement
        int       MinPictHeight;        // min row height for the picture frames
        int       PageNo;               // one of the page number where the row is located
        int       id;                   // table id - default to 0
        int       flow;                 // FLOW_ constants
        BYTE      reserved[4];
     } huge *TableRowW;                  // pointer to the table row table

     struct StrTableAux {               // Table run-time auxiliary information
        int       BotRowHt;             // Height (printer unit) of the split row at the bottom of the page
        int       TopRowHt;             // Height (printer unit) of the split row at the top of the page
        int       FirstPage;            // first page for the split row
        int       LastPage;             // last page for the split row
        UINT      flags;                // TAUX_ constants
        int       FrmBegX;              // beginning x for the screen frame set containing the row
        int       FrmEndX;              // last x for the screen frame set containing the row
     } huge *TableAuxW;

     struct StrCell {                   // Cell structure
        BOOL      InUse;                // TRUE if the entry in use
        int       row;                  // row id to which this cell belongs
        int       NextCell;             // next cell in the row, -1 if last
        int       PrevCell;             // previous cell in the row, -1 if last
        int       x;                    // calculated x (twips) position
        int       width;                // width (twips) including the margin
        UINT      border;               // uses BORDER_ constants
        int       BorderWidth[4];       // border width for each border
        int       margin;               // left/right margin in twips
        int       height;               // cell height (pixels) during display
        long      FirstLine;            // first line in the cell
        long      LastLine;             // last line in the cell including the cell marker line
        int       shading;              // shading percent 0 to 100
        UINT      flags;                // CFLAG_ constants
        int       RowSpan;              // number of rows this cell spans - used only for HTML
        int       ColSpan;              // number of columns this cell spans - used only for HTML
        COLORREF  BackColor;            // background color
        int       FixWidth;             // used by HTML add-to specify the width, -1 to specify the percentage
        int       level;                // table level
        int       ParentCell;           // parent cell for an embedded table cell
        COLORREF  BorderColor[4];       // border color
        int       TextAngle;            // 0,90,270
        BYTE      reserved[20];
     } huge *cellW;                      // pointer to the cell table

     struct StrCellAux {                // Cell auxilirary information that does not get saved
        int       PrevColCell;          // cell id of the prev cell in the column
        int       NextColCell;          // cell if of the next cell in the column
        int       SpanningCell;         // cell that spans this cell
        int       height;               // height (printer unit) of the cell on the first page (incase cells spans pages)
        UINT      flags;                // CAUX_ constants
        int       FirstPage;            // cell begin page
        int       LastPage;             // cell end page
        int       FirstPageHt;          // cell height (printer units) on the first page
        int       LastPageHt;           // cell height (printer units) on the last page - intermediate cells will have PageInfo[page].BodyHt
        int       BaseHeight;           // base height (printer unit) of the first line - calculate only if CFLAG_ALIGN_BASE is set
        int       SpaceBefore;          // Space before (prt unit) the first line - applicable only if CFLAG_ALIGN_BASE flag is set
        int       FrameId;              // one of the screen frames used  to display this cell
        int       TempPfmt;             // temporary pfmt id used duing table row insertion
     } huge *CellAuxW;                  // pointer to the aux cell table


     HGLOBAL      hTableRowW;           // handle to the row table
     HGLOBAL      hCellW;               // handle to the cell table

     int          TotalTableRowsW,      // number of row entries in the table
                  TotalCellsW,          // number of cells entries in the table
                  MaxTableRowsW,        // current max table row size
                  MaxCellsW,            // max current cell array size
                  FirstFreeCellIdW,     // first free cell id
                  CursorCellW;          // cell to which the cursor is limited

     int          TableRowsW,           // new table parameter
                  TableColsW;           // new table parameter

     BYTE         CellCharW;            // a character to mark end of a cell
     BYTE         OldCellCharW;         // a character to mark end of a cell (old)

     BOOL         ShowTableGridLinesW;  // show table grid lines
     BOOL         HasNestedTablesW;     // TRUE when the document has nested tables
     int          CurWrapCellW,         // cell id of the paragraph being currently wrapped
                  DefCellMarginW;       // default cell margin

  /******************************************************************************
     File info structure
  *******************************************************************************/
     struct StrFileInfo {               // Table structure
        BYTE      sign;                 // is set to SIGN_EOF
        BYTE      VerId;                // x.x is stored as xx (4.0 -> 40)
        BYTE      RevId;                // revision id
        int       PpHeight;             // paper height
        int       PpWidth;              // paper width
        int       PpSize;               // paper size constant
        int       PpOrient;             // orientation constant DMORIENT
        #if defined(WIN32)
           BYTE   reserved[583];        // reserved for future use
        #else
           BYTE   reserved[591];        // reserved for future use
        #endif
     }FileInfoW;

  /******************************************************************************
     Rtf Control Variables
  *******************************************************************************/
      int         RtfCurRowIdW;           // current table row id
      int         RtfCurCellIdW;          // current table cell id
      int         RtfInitCellIdW;         // initial table cell id
      int         RtfLastCellXW;          // last cell id for this row
      int         RtfParaFIDW;            // current paragraph frame id
      int         RtfPrevLevelW;          // previous line level
      int         RtfCurLevelW;           // cur line level
      int         RtfInitLevelW;          // initial level
      BOOL        RtfInTableW;            // text in table
      BOOL        RtfInEquationW;         // TRUE when in a field equation
      BOOL        InRtfReadW;             // TRUE when reading rtf file
      UINT        RtfInHdrFtrW;           // PAGE_HDR || PAGE_FTR
      struct StrRtfParaFrameInfo RtfParaFrameInfoW;  // paraframe information
      struct StrRtfParaFrameInfo PrevRtfParaFrameInfoW;  // paraframe information

  /******************************************************************************
     Page header/footer variables
  *******************************************************************************/
      BOOL        ViewPageHdrFtrW,        // TRUE to show page header footer
                  EditPageHdrFtrW,        // TRUE when editing page header/footer
                  PosPageHdrFtrW;         // TRUE when page header/footer need to be repositioned

      long        HdrFirstPageLineW,       // first header line number on the current page
                  HdrLastPageLineW,        // last header line number on the current page
                  FtrFirstPageLineW,       // first footer line number on the current page
                  FtrLastPageLineW;        // last footer line number on the current page

  /******************************************************************************
     Caret Variables
  *******************************************************************************/
     HWND         hCaretWndW;            // window that owns the caret
     BOOL         CaretEngagedW,         // caret engaged to scrolling text
                  CaretEnabledW,         // TRUE when ready to create a caret
                  CaretHiddenW,          // TRUE when the caret is hidden and destroyed, has meaning only when the caret is enabled
                  ShowProtectCaretW,     // TRUE to show caret on the protected text
                  CaretPositionedW,      // TRUE when caret already positioned
                  RepaintRulerW,         // repaint ruler unconditionally
                  CaretVertW;            // TRUE when caret is vertical 
     int          CaretPageW,            // page number of the caret
                  CaretHeightW;          // height of the caret
     long         CaretPosW;             // position of the caret

  /******************************************************************************
     Ime variables
  *******************************************************************************/
     long         ImeStartPosW;          // starting position of the composit string
     int          ImeByteLenW;           // previous byte length of the composit string
     int          ImeCharLenW;           // previous byte length of the composit string
     int          ImeResultBytesW,       // bytes saved as result
                  ImeResultCharsW;       // character saved as result
     BOOL         ImeEnabledW;           // TRUE when ime is enabled
     BOOL         InlineImeW;            // TRUE when inline ime is enabled
     int          ImeDiscardsW;          // number of WM_IME_CHARs to discard

  /******************************************************************************
     TOC Variables
  *******************************************************************************/
     BOOL         DocHasTocW,           // TRUE if the document has table of contents
                  MultipleTocW,         // TRUE if the document has multiple toc
                  TocShowPageNoW,       // display page number
                  DocHasHeadingsW;      // TRUE if the document has headings
     DWORD        FirstTocPosW;         // absolute position of the first toc
     int          TocTabAlignW,         // tab alignment TAB_ constant
                  TocTabLeaderW;        // tab leader TAB_ constant
  /******************************************************************************
     Internet access variables
  *******************************************************************************/
     HINTERNET    hInternetW;           // internet handle

  /******************************************************************************
     Toolbar Variables
  *******************************************************************************/
     int TlbItemCountW[MAX_TLB_LINES];            // number of toolbar elements for each toolbar line
     struct StrTlb {
        int id;                          // item ids
        int CmdId;                       // corresponding command id
        HWND hWnd;                       // window handle for combo box or other window related item
        LPBYTE  pBalloon;                // balloon text - allocated by MemAlloc
        UINT    flags;                   // TLBFLAG_ flags
        int     x;                       // x (pixel) relative to the top of the toolbar
        int     y;                       // y (pixel) relative to the top of the toolbar
        int     width;                   // width in pixels
        int     height;                  // height in pixels
        int     IconX;                   // x position of the icon in the toolbar bitmap
        int     IconY;                   // y position of the icon in the toolbar bitmap
        LPBYTE  pInfo;                   // custom icon image info
        LPBYTE  pImage;                  // custom icon image data
        int     ImageHeight;             // image height
     } TlbIdW[MAX_TLB_LINES][MAX_TLB_PER_LINE];   // elements

     struct StrTlb far *pBlnTlbW;        // current ballon item
     
     struct StrTlbCustId {               // custom icons
        BOOL InUse;                      // in use flag
        int id;                          // item ids  TLB_ constant.
        LPBYTE pInfo;                    // image info for the toolbar icon
        LPBYTE pImage;                   // image data
        int    height;                   // image height
        int line;                        // line = 0, first toolbar line, line=1 second toolbar line
        LPBYTE  pBalloon;                // balloon text
        int CmdId;                       // command id 
     } TlbCustIdW[MAX_CUST_ICONS];

     int TotalTlbCustIdsW;              // number of cust ids used

     RECT   BlnRectW;                    // balloon rectangle
     HBITMAP hBlnBMW;                    // bitmap that saves the balloon background
     int    TlbIdClickedW;               // toolbar icon clicked

  /******************************************************************************
     Image map Variables
  *******************************************************************************/
     struct StrImageMap {
        LPBYTE  name;                      // name of the image map
        int  TotalRects;                   // number of rectangles in the image map
        struct StrImageMapRect far *pMapRect; // pointer to an array of image image rectangles for this map
     } ImageMapW[MAX_IMAGE_MAPS];
     
     int TotalImageMapsW,                  // number of image maps in use in the above structure
         CurMapPictW,                      // picture id of the current picture with map
         CurMapIdW,                        // current map id
         CurMapRectW;                      // rectangle id 

  /******************************************************************************
     Spell checking Variables
  *******************************************************************************/
     BYTE StCharSetW;                      // character set for spell-checking
     WORD StToUniCharW[256];               // byte to unicode conversion
     LPBYTE pStFromUniCharW;               // unicode to byte conversion
     int  StFirstUniCharW,                 // first unicode character in the pStFromUniChar array
          StLastUniCharW;                  // last unicode character in the pStFromUniChar array

  /******************************************************************************
     Other Variables
  *******************************************************************************/
     BOOL         InsertModeW,           // indicates if insert or replace typing mode
                  TerHelpWantedW,        // TRUE when help is requested
                  WindowBeingCreatedW,   // TRUE when a window is being created
                  CrLfUsedW,             // TRUE when cr/lf pair used in the buffer
                  ProtectionLockW,       // Protection lock
                  ShowParaMarkW,         // show para mark
                  ShowPageBorderW,       // show page border
                  BatchModeW,            // TRUE when in an invisible mode
                  RepageTimerOnW,        // TRUE when the repagination timer is on
                  NoTabIndentW,          // Do not treat left indent as tab stop
                  restrictW,             // TRUE to create a restricted version
                  InIEW,                 // TRUW when hosted within IE
                  WmWashedW;             // watermark picture washed

     BYTE         TerHelpFileW[13];      // Help file for the editor
     BYTE         BreakCharsW[20];       // break characters
     BYTE         ReqCharSetW;           // requested char set
     DWORD        ReqLangW;              // language selected by keyboard
     HKL          CurHKLW;               // current keyboard layout
     BYTE         DocNameW[300];         // doc name

     int          FileFormatW,           // input format, SAVE_TER, SAVE_RTF, SAVE_TEXT
                  RtfInputW,             // type of rtf input: RTF_FILE, RTF_BUF, RTF_CB
                  ZoomPercentW;          // zoom percent

     BYTE         SpeedKeyEnabledW[MAX_SPEED_KEYS];  // TRUE to enable a speed key

     BOOL         ShowHyperlinkCursorW,  // TRUE to show the hyperlink cursor
                  ModifyProtectColorW,   // modify the color or the protected text
                  LinkDblClickW,         // TRUE to activate hyperlink on double click, otherwise use single click
                  MatchIdsW,             // match font and para ids when creating a new one
                  InDialogBoxW,          // TRUE when running a dialog box
                  SnapToGridW,           // snap to ruler grid
                  TblSelCursShowingW,    // TRUE when the tbl selection cursor is showing
                  LinkCursShowingW,      // TRUE when the hyperlink cursor is showing
                  ProtectFormW;          // TRUE when form protection on

     HGLOBAL      hAppMemoryW;           // data block reserved to use by your application
     LPVOID       pAppMemoryW;           // data pointer for hAppMemory

     UINT         LinkStyleW;            // character style of the hyperlink phrase
     COLORREF     LinkColorW;            // color of the linked phrase

     int          ExtraSpacePrtXW,       // unit of extra space in the printer units
                  ExtraSpaceScrXW,       // unit of extra space in the screen units
                  BulletXW,              // bullet x position
                  CurCtlIdW;             // current clicked control id

     BOOL         CharBoxLeftW,          // TRUE to draw the left character box border
                  CharBoxRightW,         // TRUE to draw the right character box border
                  bulletW,               // TRUE when a bullet drawn for the line
                  ExpandLineFmtW;        // TRUE when the line format has been temporarily expanded

     UINT         MessageDisplayedW;     // list of already displayed messages

     int          BkPictIdW;             // background picture id
     UINT         BkPictFlagW;           // BKPICT_? constants

     DWORD        TerFlagsW;             // TFLAG_ constants
     DWORD        TerFlags2W;            // TFLAG2_ constants
     DWORD        TerFlags3W;            // TFLAG3_ constants
     DWORD        TerFlags4W;            // TFLAG4_ constants
     DWORD        TerFlags5W;            // TFLAG5_ constants
     DWORD        TerFlags6W;            // TFLAG6_ constants
     DWORD        TerOpFlagsW,           // TOFLAG_ internal flags
                  TerOpFlags2W;          // TOFLAG2_ constant

     HWND         hMdiClientW,           // MDI client when TER is embedded in an MDI control
                  hMdiChildW,            // current MDI child when TER is embedded in an MDI control
                  hCurCtlWndW;           // current control window handle

     HANDLE       hUserAccTableW;        // custom accelerator table

     LPBYTE       RtfInfoW[INFO_MAX];    // rtf file information name string
     LPBYTE       pRtfInfoW[INFO_MAX];   // rtf file information data string

     BYTE         ToolbarFlagTextW[10];  // toolbar flag text
     int          EvalTimerCountW;       // eval timer count

     WNDPROC      FormFieldSubclassProcW;  // handle to zoom bar subclass process

     BYTE         UserDirW[MAX_WIDTH+1];  // user directory
     int          UserFileTypeW;         //  default file type to open: SAVE_ constants

     int          DefLangW;              // default language
     HMENU        hPopupW;               // popup menu

     struct       StrStResult SplResultW;  // result from 'SpellWord'

     RECT         ParamRectW;            // function parameter rectangle

     HDC          hMpDC;                 // merge-print dc

     HHOOK        hMsgHookW;             // message hook

     DWORD        EventMaskW;            // event mask for standard EN_ notification messages.
     HGLOBAL      hGetTextMemW;          // previous WM_GETTEXT memory handle
     long         GetTextLenW;           // previous GetTextLen
     int          GetTextModW;           // previous GetTextLen mod count

}TERWND;

typedef TERWND far *PTERWND;             // pointer to the TER window variable structure


/******************************************************************************
    Common Variables
*******************************************************************************/
PREFIX struct StrGlbPrtDC {
     int        UseCount;               // number of uses
     HDC        hDC;                    // global printer device context
     BYTE       name[60],               // name of the printer
                drv[60],                // printer driver
                port[60];               // printer port
     DEVMODE    DevMode;                // device information used to create hDC
  } far *GlbPrtDC;

PREFIX struct   StrGlbFont {
     int        UseCount;               // when 0, the font is a candidate for deletion when running of resources
     HFONT      hFont;                  // font handle
     LOGFONT    lFont;                  // logical font structure
     HDC        hDC;                    // DC for which the font is created
     BOOL       IsScrDC;                // TRUE if it is a screen DC
  } far *GlbFont;

PREFIX BYTE     TempString[MAX_WIDTH+1], // temporary holding place
                TempString1[MAX_WIDTH+1];// temporary holding place

PREFIX int      TotalGlbFonts;          // total number of global fonts
PREFIX int      GlbPrtDCCount;          // total number of global printer handles
PREFIX HGLOBAL  hGlbFont;               // global font table handle

PREFIX HANDLE   hTerInst;               // Ter instance id
PREFIX BOOL     Win32;                  // TRUE if compiled as a WIN32 application
PREFIX HWND     hCurAbortWnd;           // Current abort window
PREFIX HMENU    hTerMenu;               // TER menu
PREFIX HDC      hMemDC;                 // comaptible display context
PREFIX HANDLE   hTerAccTable;           // accelerator table handle
PREFIX HPEN     hPagePen;               // pen to draw soft page break
PREFIX HPEN     hFocusPen;              // handle to the focus pen
PREFIX int      TerOpenCount;           // number of open windows
PREFIX int      TotalWinPtrs,           // number of non-standard windows open
                TotalWinTasks;          // total tasks using this DLL
PREFIX int      StorageId;              // id to make unique storage name

PREFIX BYTE     DefPrinterName[60],     // name of the printer
                DefPrinterDriver[60],   // printer driver
                DefPrinterPort[60];     // printer port

PREFIX HBITMAP  hToolbarBM;             // toolbar bitmaps

PREFIX HCURSOR  hTable1Cur,             // table cell width cursor
                hTable2Cur,             // table highlighting cursor
                hTable3Cur,             // table row height cursor
                hTab1Cur,               // tab marker cursor
                hHyperlinkCur,          // hyperlink cursor
                hPlusCur,               // plus cursor
                hDragInCur,             // dragging text inside
                hDragInCopyCur,         // dragging text inside in copy mode (ctrl pressed)
                hDragOutCur,            // cursor out while dragging
                hWheelFullCur,          // wheel up/down cursor
                hWheelUpCur,            // wheel up cursor
                hWheelDownCur,          // wheel down cursor
                hHBeamCur;              // horizontal I beam cursor

PREFIX struct StrWinPtr {               // Contains data pointer for non-standard windows
         BOOL InUse;                    // TRUE=slot in use
         HWND hWnd;                     // window handle
         PTERWND data;                  // data pointer
     }WinPtr[MAX_WIN_PTRS];             // max non-standard windows

PREFIX struct StrTask {                 // window task list
         HTASK task;                    // task id
         int   count;                   // number of open windows for this task
         #if !defined(INC_OLE1) && !defined(NO_OLE)
            LPSTORAGE     pDocStg;      // top level storage
         #endif
     } WinTask[MAX_TASKS];

PREFIX BOOL StSearched;                 // TRUE if SpellTime searched
PREFIX HINSTANCE hSpell;                // spell checker handle if SpellTime installed
PREFIX HINSTANCE hHts;                  // HTML add-on dll handle if html add-on is installed
PREFIX HINSTANCE hWininet;              // WININET dll handle
PREFIX BOOL SspSearched;                // TRUE if ssp liraray searched
PREFIX BOOL WininetSearched;            // TRUE if WININET liraray searched
PREFIX HINSTANCE hSsp;                  // ssp library handle

PREFIX DWORD TerInitFlags;              // initialization flags
PREFIX DWORD TerInitFlags2;             // initialization flags
PREFIX DWORD TerInitFlags3;             // initialization flags
PREFIX DWORD TerInitFlags4;             // initialization flags
PREFIX DWORD TerInitFlags5;             // initialization flags
PREFIX DWORD TerInitFlags6;             // initialization flags

PREFIX LPBYTE MsgString[MAX_MSGS];      // message pointers
PREFIX BYTE CustomMsg[MAX_MSGS];        // TRUE to indicate a custom message
PREFIX int  TerLastMsg;                 // last message
PREFIX BYTE TerLastDebugMsg[100];       // last debug message

PREFIX COLORREF PalColor[MAX_PAL_COLORS]; // maximum colors in the online palette
PREFIX int TotalBaseColors,              // basic colors in the pallete
           TotalPalColors;               // total colors in the pallet table

PREFIX int      CosVal[361];             // cos values for 0 to 360 degrees multiplied by 16
PREFIX long     JpgFactor[8][8];         // reserve DCT coefficients

PREFIX int      ZRow[64];                // Jpeg row number for corresponding zig-zag index
PREFIX int      ZCol[64];                // Jpeg col number for corresponding zig-zag index

PREFIX DWORD    TerStyles;               // TER style bits to override the given styles at creation time
PREFIX BOOL     InitMbcs;                // The initial value of the mbcs variable
PREFIX PRINT_CALLBACK PrintCallback;     // routine that TerMergePrint calls
PREFIX int      InitPageWidth;           // initial page width in twips
PREFIX int      InitPageHeight;          // initial page height in twips
PREFIX BYTE     InitFontFace[32];        // initial type face
PREFIX BOOL     eval;                    // TRUE when for the eval version of the product
PREFIX BOOL     expired;                 // TRUE when eval expired.
PREFIX BYTE     LicenseKey[100];         // license key
PREFIX long     EvalDaysRemaining;       // number of days remaining in eval
PREFIX BOOL     disable;                 // TRUE to disable the control
PREFIX BOOL     extended;                // eval extended
PREFIX BYTE     DateFieldText[30][50];   // date field strings
PREFIX int      TotalDateFmts;           // number of default date formats
PREFIX LPBYTE   WeekDays[7];             // week days
PREFIX LPBYTE   MonthNames[12];          // month names
PREFIX BOOL     DebugMode;               // debug mode

PREFIX BYTE     TlbClass[40];            // toolbar window class

PREFIX LPBYTE   BalloonText[MAX_TLB];    // default 
PREFIX int      TlbItem[MAX_TLB_LINES][MAX_TLB_PER_LINE];  // tool bar sequence
PREFIX BOOL     TlbItemHide[MAX_TLB_LINES][MAX_TLB_PER_LINE];  // TRUE to hide this icon
PREFIX BOOL     IsWinNT2000;             // TRUE when running on WinNT or 2000
PREFIX BOOL     IsWin95_98_ME;           // TRUE when running on Win95, 98, or ME
PREFIX BOOL     OSCanRotate;             // OS supports DC rotation

PREFIX LPBYTE   DefPaperName[MAX_PAPERS]; // default paper names
PREFIX int      DefPaperSize[MAX_PAPERS]; // paper sizes
PREFIX float    DefPaperWidth[MAX_PAPERS]; // paper width in protrait mode
PREFIX float    DefPaperHeight[MAX_PAPERS]; // paper height in protrait mode
PREFIX int      DefPaperCount;            // default paper count

PREFIX BYTE     PdfDllName[MAX_WIDTH+1];  // Pdf Dll name
PREFIX BYTE     MergeDelim;               // mail-merge delimiter character

PREFIX int      HuffLen[MAX_HUFF_CODES+1];      // run length for tiff implementation
PREFIX BYTE     HuffWhite[MAX_HUFF_CODES+1];    // white run codes
PREFIX BYTE     HuffWhiteLen[MAX_HUFF_CODES+1]; // bit length of white run codes
PREFIX BYTE     HuffBlack[MAX_HUFF_CODES+1];    // black run codes
PREFIX BYTE     HuffBlackLen[MAX_HUFF_CODES+1]; // bit length of black run codes
PREFIX BOOL     HuffInitialized;                // TRUE when the huff table is initialized
/******************************************************************************
     WIN32 Custom Control Variables
*******************************************************************************/
PREFIX CCSTYLEFLAG CtlStyleFlag[TER_NUM_STYLES];  // style flags


/******************************************************************************
    JPG io structures
*******************************************************************************/
struct StrJpgHuffTbl {
     BOOL IsDC;                           // TRUE for dc, FALSE if AC
     int  id;                             // table id (0 or 1)
     BYTE CodeLen[16];                    // code lengths
     LPBYTE pSymbol;                      // upto 4096 (16*256) symbols
     LPBYTE pSymLen;                      // pointer to symbol length
     LPWORD pCode;                        // huff codes
     };

struct StrJpg {                           // Jpg io structure
      PTERWND w;                          // pointer to the current window
      HANDLE hFile;                       // file if file is used
      BYTE huge *buf;                     // input buffer pointer if using buffer
      long BufLen;                        // length of the buffer, -1 if not known
      long BufIndex;                      // index of current character in the buffer
      BYTE text[MAX_WIDTH+1];             // temporary buffer to read text from file
      int  TextLen;                       // length of the text in the 'text' buffer
      int  TextIndex;                     // index if current BYTE in the 'text' buffer
      BYTE CurChar;                       // last character read
      WORD CurWord;                       // current word value
      BYTE stack[MAX_WIDTH+1];            // characters returned from previous operations
      int  StackLen;                      // number of characters in the stack
      BOOL eof;                           // end of file
      long FilePos;                       // current file position
      
      BOOL IsProgressive;                 // is progressive jpeg file
      int  FirstIdx;                      // starting index in the scan
      int  LastIdx;                       // last idx available in the scan
      int  ShiftFactor;                   // number of bits shifted for successive approximation
      int  PrevShiftFactor;               // Shift factor for the previous scan

      int  BitsPerPixel;                  // calculated
      int  BytesPerPixel;                 // used for 24 bits/pixel only

      HGLOBAL hDIB;                       // memory for the DIB
      BYTE huge *pImage;                  // pointer to the image area

      long ScanLineSize;                  // size of one scan line

      int  width;                         // width of the image in pixels
      int  height;                        // height of the image in pixels

      DWORD ColorTableOffset;             // offset to the color table
      WORD red[256];                      // red color table
      WORD green[256];                    // green color table
      WORD blue[256];                     // blue color table

      BYTE QTable[4][64];                 // quantization table

      int  NumComps;                      // number of image components
      int  FrameCompId[4];                // component id in the frame
      int  HorzSamp[4];                   // horizontal sampling factor for each component
      int  VertSamp[4];                   // vertical sampling factor for each component
      int  QTableId[4];                   // quantization table id to use for each component
      int  HmDCId[4];                     // index of the huff table for DC
      int  HmACId[4];                     // index of the huff table for AC
      int  PrevDC[4];                     // previous DC value for the component

      LPBYTE pHist;                       // allocated pointer to store the scan dct history data
      LPBYTE pComp[4];                    // pointer to each component data
      short int far *pBlock;              // pointer to the prev scan data for the current block

      int  EobCount;                      // number of EOB (end of block) pending

      int  ScanCompCount;                 // number of components in the current scan
      int  ScanCompId[4];                 // scan ids of the components in the current scan
      int  ScanBlocksPerMcu[4];           // blocks per mcu for each component in the block
      int  ScanMcuPerRow;                 // mcu per row in the scan
      int  ScanMcuPerCol;                 // mcu per column in the scan
      int  ScanMaxMcu;                    // max mcus in the scan
      int  interleaved;                   // TRUE when the scan is interleaved

      int  HuffTblCount;                  // number of huffman tables
      struct StrJpgHuffTbl HuffTbl[MAX_JPEG_HUFF_TBLS];    // upto 4 huff tables

      DWORD InDword;                      // dword being processed
      int  BitIdx;                        // index of the first available bit in InBuf
      int  CurSym;                        // currently extracted symbol

      int  McuPerInterval;                // number of MCUs per restart interval
      BYTE MarkerInScan;                  // marker found during scanning
      int  McuPerRow;                     // number of MCUs per image row
      int  McuPerCol;                     // number of MCUs per image column
      int  RowsPerMcu;                    // number of pixel rows per mcu
      int  ColsPerMcu;                    // number of pixel columns per mcu
      int  BytesPerMcu;                   // mcu size in bytes
      int  BlocksPerRow[4];               // blocks per image row for each component
      int  BlocksPerMcu;                  // number of blocks in one mcu

      int  PixelsPerSampX[4];             // pixels per sample in the x direction for each component
      int  PixelsPerSampY[4];             // pixels per sample in the y direction for each component

      int CurMcu;                         // current mcu number
      int MaxMcu;                         // maximum mcus in the document
      int CurX;                           // current pixel x
      int CurY;                           // current pixel y

      BOOL AdobeMarkerFound;              // the marker used by adobe
      };

struct StrJpgIfd {                        // Jpg ifd entry
      WORD tag;                           // tag id
      WORD type;                          // type of data
      DWORD count;                        // number of data elements
      DWORD data;                         // data or data offset
     };

/******************************************************************************
    PNG io structures
*******************************************************************************/
struct StrPngHuffTbl {
   int count;            // number of codes
   int FirstCode;        // first non-zero length code
   LPINT pSymbol;        // pointer containing symbos
   LPINT pCode;          // pointer containing corresponding huffman codes
   LPINT pCodeLen;       // pointer containing code lengths for the corresponding codes
};

struct StrPng {                           // Gif io structure
      PTERWND w;                          // pointer to the current window
      BOOL input;                         // TRUE when reading png file, FALSE when creating png file
      HANDLE hFile;                       // handle if file is used
      BYTE huge *buf;                     // input buffer pointer if using buffer
      HGLOBAL hBuf;                       // output buffer handle (used for output only)
      long BufLen;                        // length of the buffer, -1 if not known
      long BufIndex;                      // index of current character in the buffer
      BYTE text[MAX_WIDTH+1];             // temporary buffer to read text from file
      int  TextLen;                       // length of the text in the 'text' buffer
      int  TextIndex;                     // index if current BYTE in the 'text' buffer
      BYTE CurChar;                       // last character read or being prepared to be written out
      WORD CurWord;                       // current word value
      BYTE stack[MAX_WIDTH+1];            // characters returned from previous operations
      int  StackLen;                      // number of characters in the stack
      BOOL eof;                           // end of file
      long FilePos;                       // current file position
      BOOL extracted;                     // TRUE when picture extracted
      long ChunkSize;                     // current chunk size

      HGLOBAL hDIB;                       // memory for the DIB
      BYTE huge *pImage;                  // pointer to the image area
      RGBQUAD far *pColor;                // color table pointer
      BYTE huge *pImageScan;              // pointer to the current image scan line

      BYTE huge *pData;                   // data pointer
      DWORD DataSize;                     // length of the data
      long  DataPos;                      // current byte position into the data steram
      int   DataBitPos;                   // Current bit position into the current byte

      LPBYTE pPrevScan;                   // previous filter buffer
      LPBYTE pScan;                       // filter buffer for the new line being constructed
      int  FilterType;                    // current filter type
      int  FilterBufSize;                 // filter buffer size
      int  CurFilterByte;                 // current filter byte number

      int  RunLenVal[35];                 // base values for the run lengths
      int  RunLenBits[35];                // extra bits values to read and add to the base values

      int  DistVal[35];                   // basic distance values
      int  DistBits[35];                  // extra bits values to read and add to the base distance values
      int  grid[8][8];                    // interlace grid
      int  GridX;                         // current x position in the interlace grid
      int  GridY;                         // current y position in the interlace grid
      int  GridPass;                      // current pass number in the grid

      int  width;                         // width of the image in pixels
      int  height;                        // height of the image in pixels
      int  BytesPerColor;                 // number of bytes per color
      int  BytesPerPixel;                 // bytes per pixel excluding alpha bytes
      int  BytesPerPixelAlpha;            // bytes per pixel including alpha bytes
      int  BitDepth;                      // number of bits in the pixel
      int  ColorComponents;               // number of color components
      int  PixelsPerByte;                 // for multiple pixel/byte images
      int  CurColorByte;                  // current color byte number
      int  CurColor;                      // current color component
      int  ClrTblSize;                    // number of colors in the color table
      int  ColorType;                     // PNGCLR_ constants
      int  AspectRatio;                   // Pixel aspect ratio of the source device
      int  interlaced;                    // this image data is interlaced
      int  InterlacePass;                 // current interlace pass number
      BOOL NewScanLine;                   // TRUE when starting a new scan line
      long ScanLineSize;                  // size of one scan line
      BOOL ReadAlpha;                     // read the alpha channel bits
      BOOL PalUsed;                       // TRUE when the palette is used

      BOOL AlphaPending;                  // TRUE when Alpha needs to be applied to the current pixel
      int  CurAlphaByte;                  // current alpha byte number
      BOOL xparent;                       // TRUE for a transparent image using tRMS chunk
      BYTE alpha[256];                    // alpha values for the palette
      int  AlphaCount;                    // number of entries used in the alpha array

      BYTE BkndColor[3];                  // RGB components of background color
      BYTE PixColor[3];                   // RGB components of the current pixel
      BYTE XparentColor[3];               // RGB components of the xparent color
      int  BkndIdx;                       // index of the background color in the palette

      COLORREF GlobalColor[256];          // global color table
      COLORREF LocalColor[256];           // local color table

      int CurX;                           // current pixel x
      int CurY;                           // current pixel y

      // output-only variables
      int   pict;                         // picture id

      DWORD CrcTable[256];                // crc table
      DWORD crc;                          // running crc
      DWORD Checksum;                     // adler checksum of the color bytes
      HDC   hDC;                          // memory device context
      HBITMAP hBM;                        // bitmap to hold the output image
      HBITMAP hOldBM;                     // to replace into hDC before deleting hBM

      LPBYTE CircData;                    // circular buffer data
      LPWORD CircNext;                    // index to the next run
      WORD RunLen;                        // length of the runs
      WORD CircBufLen;                    // length of the circular buffer in use
      WORD FirstRun;                      // index of the first run 

      LPBYTE pInfo;                       // pointer to the bitmap information and color table
      };


/******************************************************************************
    GIF io structures
*******************************************************************************/
struct StrGif {                           // Gif io structure
      HANDLE hFile;                        // handle if file is used
      BYTE huge *buf;                     // input buffer pointer if using buffer
      long BufLen;                        // length of the buffer, -1 if not known
      long BufIndex;                      // index of current character in the buffer
      BYTE text[MAX_WIDTH+1];             // temporary buffer to read text from file
      int  TextLen;                       // length of the text in the 'text' buffer
      int  TextIndex;                     // index if current BYTE in the 'text' buffer
      BYTE CurChar;                       // last character read
      WORD CurWord;                       // current word value
      BYTE stack[MAX_WIDTH+1];            // characters returned from previous operations
      int  StackLen;                      // number of characters in the stack
      BOOL eof;                           // end of file
      long FilePos;                       // current file position
      BOOL extracted;                     // TRUE when picture extracted
      BOOL IsTransparent;                 // TRUE when transparent
      BOOL XparentColor;                  // transparent color index
      int  DelayTime;                     // delay time in 1/100 of seconds
      long LoopCount;                     // number of loops in animation
      BOOL animating;                     // TRUE when getting animation pictures

      HGLOBAL hDIB;                       // memory for the DIB
      BYTE huge *pImage;                  // pointer to the image area
      COLORREF PrevBkColor;               // save the color at the background index
      COLORREF PrevXparentColor;          // save the color at the transparent index


      int  width;                         // width of the image in pixels
      int  height;                        // height of the image in pixels
      BOOL GlbClrTblExists;               // TRUE if global color table exists
      int  GlbClrTblSize;                 // size of the global color table
      int  ColorBits;                     // number of bits per primary color
      int  BkColor;                       // index of the background color into Global color table
      int  AspectRatio;                   // Pixel aspect ratio of the source device
      int  interlaced;                    // this image data is interlaced
      int  InterlacePass;                 // current interlace pass number
      long ScanLineSize;                  // size of one scan line

      HGLOBAL hPrevData;                  // previous data

      COLORREF GlobalColor[256];          // global color table
      COLORREF LocalColor[256];           // local color table
      COLORREF far *CurColor;             // current color table

      struct GifImageDesc {
         short x;                          // x position of this image
         short y;                          // y position of this image
         short width;                      // width of the image
         short height;                     // height of the image
         BYTE packed;                     // packed fields
      } image;

      int CurX;                           // current pixel x
      int CurY;                           // current pixel y

      };

/******************************************************************************
    TIFF io structures
*******************************************************************************/
struct StrTif {                           // Tif io structure
      PTERWND w;                          // pointer to the current window
      HANDLE hFile;                        // handle if file is used
      BYTE huge *buf;                     // input buffer pointer if using buffer
      long BufLen;                        // length of the buffer, -1 if not known
      long BufIndex;                      // index of current character in the buffer
      BYTE text[MAX_WIDTH+1];             // temporary buffer to read text from file
      int  TextLen;                       // length of the text in the 'text' buffer
      int  TextIndex;                     // index if current BYTE in the 'text' buffer
      BYTE CurChar;                       // last character read
      WORD CurWord;                       // current word value
      BYTE stack[MAX_WIDTH+1];            // characters returned from previous operations
      int  StackLen;                      // number of characters in the stack
      BOOL eof;                           // end of file
      long FilePos;                       // current file position
      BOOL motorala;                      // TRUE if motorala format file

      int  BitsPerSample;                 // bits per sample, def = 1
      int  BitsPerPixel;                  // calculated
      int  SamplesPerPixel;               // plane per pixel, default = 1
      int  BytesPerPixel;                 // used for 24 bits/pixel only
      int  CurPixByte;                    // cur pixel byte

      int  RowsPerStrip;                  // scan lines held in one data strip, default = 1
      DWORD StripLenOffset;               // offset containing strip lengths
      int  StripLenType;                  // type of entry (int/long etc) in the strip length table
      DWORD StripOffset;                  // offset to the pointers to strips

      int  ImageType;                     // photometric interpretation
      WORD compression;                   // compression type, def=1

      HGLOBAL hDIB;                       // memory for the DIB
      BYTE huge *pImage;                  // pointer to the image area

      long ScanLineSize;                  // size of one scan line

      int  width;                         // width of the image in pixels
      int  height;                        // height of the image in pixels

      DWORD ColorTableOffset;             // offset to the color table
      WORD red[256];                      // red color table
      WORD green[256];                    // green color table
      WORD blue[256];                     // blue color table


      int CurX;                           // current pixel x
      int CurY;                           // current pixel y

      };

struct StrTifIfd {                        // Tif ifd entry
      WORD tag;                           // tag id
      WORD type;                          // type of data
      DWORD count;                        // number of data elements
      DWORD data;                         // data or data offset
     };


/******************************************************************************
    LZW decompression structures
*******************************************************************************/
struct StrLz {                        // LZW decompression structure
     // The variables to be initialized by the calling program
     int  LzLastCharL;                   // last character
     int  LzClearCodeL;                  // clear code
     int  LzEofCodeL;                    // end of file code
     int  LzFirstCodeL;                  // value of the first code
     int  LzBeginInBitsL;                // starting number of bits per code
     int  LzMaxInBitsL;                  // maximum bits per code
     int  LzMaxInBufSizeL;               // maximum input buffer stream size
     void far *LzUserDataL;              // additional information stored by the calling function
     BOOL (*LzOutputCallbackL)(struct StrLz far *);  // routine to accept the output items
     BOOL LzImageTypeL;                  // image type PICT_ constant
     int  LzBitsStuffedL;                // number of bits stuffed in the byte

     // The output variables
     int  LzOutValueL;                     // current output value

     // The internal variables used by the decompression routine
     int  LzCurInBitsL;                  // current number of bits per input item

     long LzMapSizeL;                    // size of lz map in bytes

     struct StrLzMap {                   // lz code map
          int prev;                      // pointer back
          int chr;                       // character
          } far *LzMapL;

     LPBYTE LzInBufL;                      // input buffer

     LPINT LzCharL;                        // character buffer
     int LzCharCountL;                     // current character count

     long LzBitsReadL;                     // number of bits read from the input buffer
     int  LzCurBufSizeL;                   // number of bytes read in the input buffer

     int  LzNextCodeL;                     // next free code
     int  LzPrevCodeL;                     // previous code
     int  LzPrevCharL;                     // previous char

     BOOL LzEofL;                          // end of input stream
     BOOL LzClearedL;                      // TRUE after the clear code

};

/******************************************************************************
    Other Structures
*******************************************************************************/
struct StrMergeData {
       LPBYTE pName;                    // name of the field
       LPBYTE pData;                    // data for the field
       int   NameLen;                  // length of the name
       int   DataLen;                  // length of the field data
       };

struct StructTextBuf {
       BYTE huge *pBuf;                   // pointer to the buffer containing text
       BYTE huge *lead;                   // pointer to the lead byte buffer
       WORD huge *pFmt;                   // pointer to the buffer containing the format data
       int   len;                         // current line length
       long  index;                       // index to the current line
       BOOL  eof,eol;                     // end of file and end of line flags
       BOOL  CrFound;                     // <cr> found
       int   MaxLineLen;                  // maximum line length allowed
       BOOL  ParaCharFound;               // indicates if paragraph character is detected
      };

struct StrEnumFonts {                     // result of font enumeration
      BOOL GetMultiple;                   // when TRUE all enumerated fonts are returned
      BOOL EnumTypeFace;                  // Enumerate type face, otherwise enumerate pointsizes
      BOOL result;                        // TRUE or FALSE
      BYTE CharSet;                       // character set
      BYTE PitchAndFamily;                // pitch and family
      int  TypeCount;                     // number of fonts enumerated
      BYTE TypeNames[MAX_TYPES][32];      // font type faces returned
      int  PointCount;                    // number of point sizes enumerated
      int  height[MAX_POINTS];            // font height in device units
      BOOL TrueType;                      // TRUE indicates true type font
      };

struct StrDlgParam {                      // Dialog box title and owner window control area
      PTERWND w;                          // owner window data pointer
      LPBYTE   data;                       // miscellaneous data pointer
      };

struct StrFontIO {                        // font structure to write to ter file
      UINT style;                         // character style
      int  TwipsSize;                     // font size
      BYTE OldTypeFace[31];               // font type face
      BYTE FontFamily;                    // font family
      int  PictType;                      // picture type when the font represents a picture
      DWORD TextColor;                    // color of the text
      int  bmWidth;                       // actual width of the picture
      int  bmHeight;                      // actual height of the picture
      int  ObjectType;                    // object type
      DWORD ObjectSize;                   // size of the object storage data
      DWORD TextBkColor;                  // text background color
      int  FieldId;                       // field id
      int  PictAlign;                     // picture alignment value
      int  AuxId;                         // id for external use
      int  CharStyId;                     // character style id
      int  ParaStyId;                     // paragraph style id
      int  LinkNameLen;                   // length of the linked picture file name
      int  FrameType;                     // picture frame type
      int  ParaFID;                       // paragraph formatting options
      int  expand;                        // expansion or compression in twips
      BYTE TypeFace[32];                  // typeface
      int  lang;                          // language id for the font
      int  offset;                        // offset from the base
      int  ObjectAspect;                  // OASPECT_ constant
      WORD UcBase;                        // UcBase
      BYTE CharSet;                       // character set
      int  OrigPictType;                  // original picture type
      DWORD OrigImageSize;                // original image size
      int  TextAngle;                     // text angle
      BOOL ObjectUpdate;                  // update on initial load
      int  CropLeft;                      // left cropping - twips
      int  CropRight;                     // right cropping - twips
      int  CropTop;                       // top cropping - twips
      int  CropBot;                       // bottom cropping - twips
      int  OrigPictWidth;                 // original picture width in twips
      int  OrigPictHeight;                // original picture height
      DWORD UlineColor;                   // underline color
      int  CharId;                        // special character id
      int  InsRev;                        // Reviewer id for the inserted text
      DWORD InsTime;                       // time inserted
      int  DelRev;                        // Reviewer id for the deleted text
      DWORD DelTime;                       // time deleted
      BYTE reserved[4];                   // reserved bytes
      };

struct StrParaIO {                        // para table structure to write to ter file
      int  LeftIndent;                    // left indent in twips
      int  RightIndent;                   // right indent in twips
      int  FirstIndent;                   // first line indent in twips
      UINT flags;                         // paragraph flags
      int  TabId;                         // tab id
      int  BltId;                         // bullet id
      UINT shading;                       // shading percentage
      UINT pflags;                        // no longer used
      int  SpaceBefore;                   // space before the paragraph-twips
      int  SpaceAfter;                    // space after the paragraph - twips
      int  SpaceBetween;                  // space between the paragraph lines
      int  AuxId;                         // id for external use
      int  StyId;                         // style id
      COLORREF BkColor;                   // paragraph background color
      int  BorderSpace;                   // border space for boxed paragrah in twips
      int  LineSpacing;                   // variable line spacing
      int  flow;                          // text flow
      COLORREF BorderColor;               // border color
      };

struct StrMetaFileHdr {
      DWORD sign;
      WORD  hmf;
      short left;
      short top;
      short right;
      short bottom;
      short UnitsPerInch;
      DWORD reserved;
      WORD  CheckSum;
      };

struct StrControl {                       // embedded control information structure
      BYTE class[60];                     // windows class
      LONG style;                         // control style bits
      long id;                            // id of the control (use long for 16/32 bit compatiblity)
      };

struct StrForm {                          // form field information structure
      // first 3 field should be exactly as StrControl fields so they can be access via pStrControl
      BYTE class[60];                     // windows class
      LONG style;                         // control style bits
      long id;                            // id of the control (use long for 16/32 bit compatiblity)

      int  TextType;                      // text field type
      int  CheckBoxType;                  // check box size, 0=auto, 1=exact
      BOOL protected;                     // 1 = protected, 0=not protected
      int  MaxLen;                        // max length of the text field
      int  CheckBoxSize;                  // checkbox size in twips
      BYTE name[MAX_WIDTH+1];             // field name
      BYTE InitText[MAX_WIDTH+1];         // initialize text data
      long InitNum;                       // initial numeric data
      BYTE typeface[32];                  // font typeface
      BYTE family[32];                    // font family
      int  TwipsSize;                     // font twips size
      UINT FontStyle;                     // font style
      int  CharSet;                       // font character set
      COLORREF TextColor;                 // text color
      COLORREF TextBkColor;               // text background color
      int  FontId;                        // current font id
      WNDPROC OrigWndProc;                // original window process
      DWORD flags;                        // FMFLAG_ constant
      int  reserved[22];                  // reserved for future use
      int  ListCount;                     // number of items in the list box
      BYTE ListItem[1][MAX_WIDTH+1];      // list box items
	// new from MAK	
			BYTE maskdata[150];									// mask data
			int typetxt;												// from rtf spec
      };

struct StrFormOld {                          // form field information structure
      // first 3 field should be exactly as StrControl fields so they can be access via pStrControl
      BYTE class[60];                     // windows class
      LONG style;                         // control style bits
      long id;                            // id of the control (use long for 16/32 bit compatiblity)

      int  TextType;                      // text field type
      int  CheckBoxType;                  // check box size, 0=auto, 1=exact
      BOOL protected;                     // 1 = protected, 0=not protected
      int  MaxLen;                        // max length of the text field
      int  CheckBoxSize;                  // checkbox size in twips
      BYTE name[300+1];             // field name
      BYTE InitText[300+1];         // initialize text data
      long InitNum;                       // initial numeric data
      BYTE typeface[32];                  // font typeface
      BYTE family[32];                    // font family
      int  TwipsSize;                     // font twips size
      UINT FontStyle;                     // font style
      int  CharSet;                       // font character set
      COLORREF TextColor;                 // text color
      COLORREF TextBkColor;               // text background color
      int  FontId;                        // current font id
      WNDPROC OrigWndProc;                // original window process
      DWORD flags;                        // FMFLAG_ constant
      int  reserved[22];                  // reserved for future use
      int  ListCount;                     // number of items in the list box
      BYTE ListItem[1][300+1];      // list box items
      };

struct StrRtf {                           // Rtf input file processing block
      HANDLE hFile;                        // stream if file is used
      BYTE huge *buf;                     // input buffer pointer if using buffer
      long BufLen;                        // length of the buffer, -1 if not known
      long BufIndex;                      // index of current character in the buffer
      BYTE OutBuf[MAX_WIDTH+1];           // output buffer
      int  OutBufLen;                     // output buffer length
      BYTE text[MAX_WIDTH+1];             // temporary buffer to read text from file
      int  TextLen;                       // length of the text in the 'text' buffer
      int  TextIndex;                     // index if current BYTE in the 'text' buffer
      BYTE CurChar;                       // last character read
      BYTE HdrFtrChar;                    // current header/footer character
      BYTE stack[MAX_WIDTH+1];            // characters returned from previous operations
      int  StackLen;                      // number of characters in the stack
      BYTE CurWord[MAX_WIDTH+1];          // current word
      int  WordLen;                       // length of the current word
      BYTE PendingByte;                   // pending byte from the previous string
      BOOL GroupBegin;                    // TRUE if the group begins
      BOOL GroupEnd;                      // TRUE if the group ends
      BOOL IsControlWord;                 // TRUE if control word
      BOOL IgnoreText;                    // TRUE to ignore text if not understood
      BOOL SubEntry;                      // TRUE if subentry in an index entry
      BOOL eof;                           // end of file
      BOOL IgnoreCrLfInControlWord;       // TRUE to ingore cr/lf in control word
      BYTE param[MAX_WIDTH/3];            // rtf control word parameter
      long IntParam;                      // rtf control word parameter in integer numeric format
      double DoubleParam;                 // rtf control word parameter in double numeric format
      int  GroupLevel;                    // group nesting level
      struct StrRtfGroup far *group;      // rtf group table pointer
      int  TotalColors;                   // total colors in the color table
      int  InitialCell;                   // value of the initial cell
      int  InitialParaFID;                // initial paraframe id
      int  OpenRowId;                     // id of the open row
      int  OpenCellId;                    // id of the open cell
      int  OpenLastCellX;                // id of the last open cell
      BOOL SetWidowOrphan;                // set widow/orpan property for the paragraphs

      // picture transfer information
      HMETAFILE hMeta;                    // handle to the metafile
      #if defined(WIN32)
         HENHMETAFILE hEnhMeta; // bitmap handle if style&PICT is TRUE and PictType=PICT_ENHMETAFILE
      #endif
      int  PictWidth;                     // width of the picture in twips
      int  PictHeight;                    // height of the picture in twips
      UINT flags;                         // RFLAG_ flags
      UINT flags1;                        // RFLAG1_ flags
      UINT flags2;                        // RFLAG2_ flags

      // current section properties
      struct StrSect sect;                // current section properties

      int  PaperWidth;                    // current 'paperw' in twips
      int  PaperHeight;                   // current 'paperh' in twips
      BOOL ApplyPaperSize;                // TRUE to set the paper size

      long    FirstLine;                  // first line where the text is inserted
      int     FootnoteNo;                 // footnote number
      int     EndnoteNo;                  // endnote number
      long    InsLine;                    // line where the text is inserted
      int     InitSect;                   // initial section number
      int     InitTblCol;                 // initial table column
      int     InsertBefCell;              // cell to insert data before
      int     InsertAftCell;              // cell to insert data after
      int     PrevCellX;                  // previous cell x
      BOOL    SomeTextRead;               // TRUE when some rtf data found
      BOOL    TableRead;                  // TRUE when text in the table found
      BOOL    SuspendReading;             // TRUE to suspend further reading of the rtf stream
      BOOL    PastingColumn;              // pasting in table
      BOOL    EmbedTable;                 // embed any table from the paste data into the existing table if any
      BOOL    EmptyDoc;                   // inserting in an empty document

      int     CurTabType;                 // TAB_LEFT or TAB_RIGHT
      BYTE    CurTabFlags;                // current tab flags

      int     FirstAnimPict;              // picture id of the first animation picture in the sequence

      BYTE    PrevChar;                   // previous character pumped by the SendRtfText function
      int     ShpGroup;                   // shape group
      int     TagId;                      // tag id to be applied to the next character

      int     pict;                       // picture id of the latest text input formfield
      int     InitFieldId;                // Initial Field Id
      int     lang;                       // default lanuage
      int     CharSet;                    // default character set
      LPBYTE  FieldCode;                  // field code
      int     PrevField;                  // field id of the previous character embedded into the document
      
      // variables to support subtables
      struct StrRtfTableLevel {
         int CurRowId;                       // saved cur row id for the parent level
         int CurCellId;                      // saved cur cell id
         int LastCellX;                     // saved last cell id
         int OpenRowId;                      // saved open row id
         int OpenCellId;                     // saved open cell id
         int OpenLastCellX;                 // saved last cell id
         int level;                          // Table level
         int InitTblCol;                     // initial table column
         BOOL PastingColumn;                 // TRUE when pasting column
      } TblLevel[MAX_TBL_LEVELS];            // table level information

      int CurTblLevel;                       // current table level
      BOOL TableInFrame;                     // TRUE when the table is located in a frame

      int XlateLs[MAX_RTF_LS];              // translate RtfLs to ls

      int DocFlow;                         // document text flow direction
      int SectFlow;                        // section text flow direction
      int CellFlow;                        // cell text flow direction

      int  PictFID;                        // Picture frame id
      long PictFrameLine;                  // line where the last picture frame was inserted
      int  PictFrameCol;                   // column where the last picture frame was inserted

      long FilePos;                         // current file position
      BOOL mac;                             // mac file

      BOOL EnableTracking;                  // enable tracking
      };

struct StrRtfFont {                       // font table in an rtf file
      BOOL InUse;                         // TRUE when the slot is in use
      int  FontId;                        // font id
      int  CharSet;                       // character set
      BYTE family[28];                    // font family (use only 28 bytes so that the struct is 64 byte total)
      BYTE name[33];                      // font name
      BYTE name2[33];                     // another name
      };

struct StrRtfColor {                       // color table in an rtf file
      COLORREF color;                      // color
      };

struct StrRtfGroup {                      // RTF group properties
      struct StrRtf far *rtf;             // pointer to the rtf structure

      struct StrRtfFont  huge *font;      // rtf font table pointer
      int    MaxRtfFonts;                 // fonts allocated in the font table

      struct StrRtfColor far *color;      // rtf color table pointer

      LPINT  RtfSID;                      // holds the translation of the style id from rtf to StyleId table.
      int    MaxRtfSID;                   // allocated SID table size

      // character properties
      int     RtfGroup;                   // group that begins a new RTF segment.
      int     FieldGroup;                 // group that starts the \field group

      int     DefFont;                    // default font index into font table
      BYTE    FontFamily[32];             // font family
      BYTE    TypeFace[32];               // font typeface
      int     PointSize2;                 // double point size of the font
      COLORREF TextColor;                 // rtf text color
      COLORREF TextBkColor;               // rtf background text color
      COLORREF UlineColor;                // underline color
      UINT    style;                      // character style
      UINT    StyleOff;                   // styles explicitly turned-off
      int     CharStyId;                  // charcter stylesheet id
      int     FieldId;                    // field id
      int     PictId;                     // picture font id when picture stored as an id
      int     AuxId;                      // Aux id
      int     CharId;                     // special charcter id
      int     CharType;                   // character type CHAR_ constant
      int     expand;                     // character expansion
      int     CharSet;                    // character set
      int     lang;                       // language id
      int     offset;                     // baseline offset
      int     CharScaleX;                 // character scale x
      BOOL    caps;                       // capilatization turned on
      WORD    UcBase;                     // unicode base
      BYTE    EndnoteMarker;              // end-note marker

      // paragraph properties
      int LeftIndent;                     // left indentation in twips
      int RightIndent;                    // Right indentation in twips
      int FirstIndent;                    // left indentation of the first line in twips
      UINT flags;                         // includes such flags as double spacing,centering, right justify etc
      UINT pflags;                        // more paragraph flags
      struct StrTab tab;                  // tap position record
      struct StrBlt blt;                  // bullet type  record
      UINT ParShading;                    // paragraph shading
      int SpaceBefore;                    // space before paragraph
      int SpaceAfter;                     // space after paragraph
      int SpaceBetween;                   // space between paragraph lines
      int OutlineLevel;                   // outline level
      int LineSpacing;                    // variable line spacing (0 for single line spacing)
      int ParaStyId;                      // paragrph style id
      COLORREF ParaBkColor;               // paragraph background color
      int ParaBorderColor;                // paragraph border color
      int flow;                           // paragraph text flow FLOW_ constant
      UINT CellFlags;                     // cell flags
      int NextStyId;                      // next style id

      int InTable;                        // true when in a table paragraph

      // border information
      int BorderType;                     // current border type
      int BorderWidth[MAX_BORDERS];       // border width for each type
      int BorderSpace[MAX_BORDERS];       // border width for each type
      COLORREF BorderColor[MAX_BORDERS];  // border width for each type
      int BorderMargin;                   // space between text and border
      int CellShading;                    // cell shading percent 0 to 100
      int CellMargin;                     // individual cell margin - can be different from margin at the row level
      int CellColSpan;                    // column spanning for the cell
      COLORREF CellPatFC;                 // cell foreground pattern color
      COLORREF CellPatBC;                 // cell background pattern color

      // paragraph frame information
      struct StrRtfParaFrameInfo ParaFrameInfo;
      UINT   FrmFlags;                    // temporary frame flags
      int    TextBoxMargin;               // space between text and border for a text box drawing object
      int    ParaFID;                     // para FID associated with the group
      int     TextAngle;                  // text angle for the paragraph frame or cell text

      //  other group flags
      DWORD  gflags;                      // GFLAG_ flags
      DWORD  gflags2;                     // GFLAG2_ flags

      // control information
      BYTE ControlWord[MAX_WIDTH+1];      // control word for the group
      BOOL IgnoreText;                    // TRUE to ingore any text in this group
      int  ControlCount;                  // number of control words in the group

      // animation information
      long AnimLoops;                     // animation loop count
      int  AnimDelay;                     // animation delay (1/100 second)

      // linked picture size
      int LinkPictWidth;                  // width of the linked picture
      int LinkPictHeight;                 // height of the linked picture

      // Shape variables
      struct StrShape shape;              // shape information
      struct StrShpGrp ShpGrp;            // shape group information

      // variables to support subtables
      int level;                          // table level for the text

      int RtfLs;                          // ls found in rtf file
      int ListLvl;                        // level found in the rtf file

      // character background patten
      int CharBkPat;                      // character background pattern value PAT_ constant
      COLORREF CharPatFC;                 // character background pattern color (foreground)
      COLORREF CharPatBC;                 // character background pattern color (background)

      int IgnoreCount;                   // number of following characters to be ignored
      int UcIgnoreCount;                 // ignore count set by the previous uc tag

      int HPageGroup;                    // group where the PARA_FRAME_HPAGE constant was set
      //long InsLine;                       // next line number to be inserted
      
      int DpGroupCount;                  // number of embedded dp groups
      int DpGroupX[MAX_DP_GROUPS];       // x value for the dp groups
      int DpGroupY[MAX_DP_GROUPS];       // y value for the dp groups

      //****** Track-changes Variables *********
      int     InsRev;                   // Reviewer id for the inserted text
      DWORD   InsTime;                  // time inserted
      int     DelRev;                   // Reviewer id for the deleted text
      DWORD   DelTime;                  // time deleted
      BOOL    revised;                  // text inserted
      BOOL    deleted;                  // text deleted

      };

struct StrRtfPict {                       // RTF picture properties
      int       type;                     // PICT_METAFILE or PICT_DIBITMAP
      HMETAFILE hMeta;                    // metafile handle if a metafile
      #if defined(WIN32)
         HENHMETAFILE hEnhMeta; // bitmap handle if style&PICT is TRUE and PictType=PICT_ENHMETAFILE
      #endif
      LPBYTE    pInfo;                    // DI bitmap information block
      LPBYTE    pImage;                   // DI bitmap image block
      long      bmHeight;                 // picture height in pixels or yExt
      long      bmWidth;                  // picture width in pixels or xExt
      long      height;                   // picture height in twips
      long      width;                    // picture width in twips
      long      CropLeft;                 // left cropping
      long      CropRight;                // right cropping
      long      CropTop;                  // top cropping
      long      CropBot;                  // bottom cropping
      long      ScaleX;                   // horrizontal scaling for display
      long      ScaleY;                   // vertical scaling for display
      long      InfoSize;                 // picture information size
      long      ImageSize;                // image size of the picture
      
      long      OrigImageSize;            // original image size
      int       OrigPictType;             // original picture type
      LPBYTE    pOrigImage;               // orignal picture information - OurAlloc pointer

      int       align;                    // picture alignement ALIGN_BOT, ALIGN_TOP, ALIGN_MIDDLE
      int       PictId;                   // picture id
      int       BitsPerPixel;             // bits per pixel
      int       planes;                   // planes for device
      int       WidthBytes;               // bytes to contain one row of pixels
      int       FormId;                   // form id
      };

struct StrRtfObject {                     // RTF object properties
      int       ObjectType;               // object type OBJ_NONE, OBJ_EMBED, OBJ_LINK
      HGLOBAL   hObject;                  // object data handle
      DWORD     ObjectSize;               // storage size of the object data
      int       ObjectAspect;             // object aspect
      int       ObjectUpdate;             // object aspect
      int       pict;                     // picture index
      };

struct StrRtfOut {                        // Rtf output file processing block
      int  output;                        // rtr output type: RTF_FILE,RTF_BUF,RTF_CB
      HANDLE hFile;                        // stream if file is used
      HGLOBAL hBuf;                       // output buffer handle if using buffer output
      BYTE huge *buf;                     // output buffer pointer if using buffer
      long BufLen;                        // allcated length of buffer
      long BufIndex;                      // index of next character in the buffer
      BYTE text[MAX_WIDTH+1];             // temporary buffer to write text
      int  TextLen;                       // length of the text in the 'text' buffer
      BOOL SpacePending;                  // TRUE if space needs to be written out after the last control
      BOOL WritingControl;                // TRUE when writing a control word
      struct StrRtfColor far *color;      // rtf color table pointer
      int  TotalColors;                   // total colors in the color table
      int  GroupLevel;
      struct StrRtfOutGroup {
         int FontId;                      // group font
         UINT style;                      // style bits that are not affected by the 'plain' keyword
         int FieldId;                     // field id
         LPBYTE FieldCode;                // field code
         UINT flags;                      // ROGFLAG_ flags
      } group[MAX_RTF_GROUPS];
      UINT flags;                         // ROFLAG_ flags
      int  sect;                          // current section
      BOOL TblHilight;                    // TRUE when a table column is highlighted
      BOOL IsDbcs;                        // TRUE when writing double byte characters
      BOOL FieldHasPara;                  // TRUE when the field text has paragraph
      BOOL ParaFmtOnParaEnd;              // TRUE to write paragraph properties when the paragraph ends
      int  XlateLs[MAX_RTF_LS];           // translate ls to RtfLs
      BOOL WatermarkWritten;              // TRUE when the watermark is written
      long line;                          // current line number

      int  DelRevCount;                   // number of RevDel 
      };


#if !defined(INC_OLE1) && !defined(NO_OLE)  // OLE2 specific structure
      struct StrClientSite {              // client site
         IOleClientSite site;             // client site
         ULONG     count;                 // use count
         PTERWND   w;                     // window data pointer
         int       obj;                   // object id in the object table
         LPSTORAGE pObjStg;               // object storage
         LPADVISESINK pObjAdvise;         // object advise sink
         DWORD     ConnectToken;          // token used to call unadvise
         #if defined(WIN32)
            LPOLEINPLACESITE pInPlaceSite;   // in place site pointer
            LPOLEINPLACEFRAME pInPlaceFrame; // in place frame pointer
            LPOLECONTROLSITE pOleControlSite; // ole control site pointer
            LPOLEINPLACEACTIVEOBJECT pActiveObject;  // active object pointer
            LPDISPATCH pDispatch;            // dispatch object pointer
         #endif
      };

      typedef struct StrClientSite far *LPSTRCLIENTSITE;

      struct StrAdviseSink {              // advise sink interface data
         IAdviseSink advise;              // client site
         ULONG     count;                 // use count
         PTERWND   w;                     // window data pointer
         int       obj;                   // object id in the object table
         LPSTORAGE pObjStg;               // object storage
         LPOLECLIENTSITE pObjSite;        // client site interface
      };
      typedef struct StrAdviseSink far *LPSTRADVISESINK;

      #if defined(WIN32)                     // ActiveX support available for 32 bit only
         struct StrPropertyBag {             // Property bag structure
            IPropertyBag pPropertyBag;       // property bag
            ULONG     count;                 // use count
            PTERWND   w;                     // window data pointer
            int       PropCount;             // number of property found
            int       id;                    // id of the property to locate
            int       type;                  // PTYPE_  constants
            BYTE      name[100];             // property name
            LPBYTE    pText;                 // property text value
            int       val;                   // integer value
         };

         typedef struct StrPropertyBag far *LPSTRPROPERTYBAG;

         struct StrDropTarget {              // Drop Target
            IDropTarget pDrop;               // in drop target
            ULONG     count;                 // use count
            PTERWND   w;                     // window data pointer
         };

         typedef struct StrDropTarget far *LPSTRDROPTARGET;

         struct StrInPlaceFrame {            // in place frame
            IOleInPlaceFrame pFrame;         // in place frame
            ULONG     count;                 // use count
            PTERWND   w;                     // window data pointer
            int       obj;                   // object id in the object table
            LPOLECLIENTSITE pObjSite;        // client site interface
         };

         typedef struct StrInPlaceFrame far *LPSTRINPLACEFRAME;

         struct StrInPlaceSite {             // in place client site
            IOleInPlaceSite site;            // in place client site
            ULONG     count;                 // use count
            PTERWND   w;                     // window data pointer
            int       obj;                   // object id in the object table
            LPOLECLIENTSITE pObjSite;        // client site interface
         };

         typedef struct StrInPlaceSite far *LPSTRINPLACESITE;

         struct StrOleControlSite {          // ole control site
            IOleControlSite site;            // ole control site
            ULONG     count;                 // use count
            PTERWND   w;                     // window data pointer
            int       obj;                   // object id in the object table
            LPOLECLIENTSITE pObjSite;        // client site interface
         };

         typedef struct StrOleControlSite far *LPSTRCONTROLSITE;

         struct StrDispatch {                // advise sink interface data
            IDispatch dispatch;              // client site
            ULONG     count;                 // use count
            PTERWND   w;                     // window data pointer
            int       obj;                   // object id in the object table
            LPOLECLIENTSITE pObjSite;        // client site interface
         };
         typedef struct StrDispatch far *LPSTRDISPATCH;
      #endif
#endif

/*******************************************************************************
    SpellTime Function Prototypes
********************************************************************************/
PREFIX int  (WINAPI _export *StParseLine)(LPBYTE,LPBYTE,LPINT,LPINT,int);
PREFIX int  (WINAPI _export *StResetUserDict)(LPBYTE,LPBYTE);
PREFIX int  (WINAPI _export *StClearHist)(void);
PREFIX int  (WINAPI _export *SpellWord)(LPBYTE,UINT,HWND,struct StrStResult far *);
PREFIX BOOL (WINAPI _export *StSetDictName)(LPBYTE);
PREFIX int  (WINAPI _export *ToSpellHist)(LPSTR,char,LPSTR);
PREFIX int  (WINAPI _export *ToUserDict)(LPSTR);

/*******************************************************************************
    HTML add-on functions used by TER
********************************************************************************/
PREFIX BOOL (WINAPI _export *HtsSaveFromTer)(HWND,BOOL,LPBYTE,HGLOBAL far *,LPLONG);
PREFIX BOOL (WINAPI _export *HtsReadFromTer)(HWND,BOOL,LPBYTE,HGLOBAL far *,long);

/*******************************************************************************
    SSP library functions used by TER
********************************************************************************/
PREFIX BOOL (WINAPI _export *SspGif2Bmp)(int,LPBYTE,HGLOBAL,long,HGLOBAL far *,struct StrGifAnim far *);

/*******************************************************************************
    WININET library functions used by TER
********************************************************************************/
#if defined(WIN32)
  PREFIX HINTERNET (WINAPI _export *pInternetOpen)(LPBYTE,DWORD,LPBYTE,LPBYTE,DWORD);
  PREFIX BOOL      (WINAPI _export *pInternetCloseHandle)(HINTERNET);
  PREFIX BOOL      (WINAPI _export *pInternetReadFile)(HINTERNET,LPVOID,DWORD,LPDWORD);
  PREFIX BOOL      (WINAPI _export *pInternetWriteFile)(HINTERNET,LPVOID,DWORD,LPDWORD);
  PREFIX HINTERNET (WINAPI _export *pInternetOpenUrl)(HINTERNET,LPBYTE,LPBYTE,DWORD,DWORD,DWORD);
  PREFIX BOOL      (WINAPI _export *pInternetQueryDataAvailable)(HINTERNET,LPDWORD,DWORD,DWORD);
  PREFIX BOOL      (WINAPI _export *pInternetGetLastResponseInfo)(LPDWORD,LPBYTE,LPDWORD);
  PREFIX HINTERNET (WINAPI _export *pHttpOpenRequest)(HINTERNET,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE *,DWORD,DWORD);
  PREFIX BOOL      (WINAPI _export *pHttpEndRequest)(LPINTERNET_BUFFERS,LPBYTE,DWORD,DWORD);
  PREFIX BOOL      (WINAPI _export *pHttpSendRequestEx)(HINTERNET,LPVOID,LPVOID,DWORD,DWORD);
#endif

/*******************************************************************************
    Function Prototypes
********************************************************************************/
#include "ter_prot.h"
