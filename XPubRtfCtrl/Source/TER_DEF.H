/******************************************************************************
                Constant Definitions
*******************************************************************************/

#define MAX_TYPES          300
#define MAX_POINTS         16
#define MAX_FONTS          320
#define MAX_GLB_FONTS      500
#define MAX_GLB_DC         100
#define MAX_SEGS           100
#define INITIAL_MAX_LINES  200
#define MAX_RTF_FONTS      5000
#define MAX_RTF_SEQ_FONTS  500
#define MAX_RTF_GROUPS     50
#define MAX_TABS           600
#define MAX_SPACES         20
#define MAX_SECTS          1100
#define MAX_FRAMES         1500
#define MAX_PARA_FRAMES    2000
#define MAX_FIELDS         100
#define MAX_WIN_PTRS       200
#define MAX_TASKS          50
#define MAX_DRAG_OBJS      850
#define MAX_SPEED_KEYS     300
#define MAX_PAGES          4500
#define MAX_AUTO_COMPS     100
#define MAX_PREVIEW_PAGES  2
#define MAX_ZOOMS          8
#define MAX_RGB            236   // in windows environment max 236 logical colors are available in the system palette
#define MAX_INIT_UNDOS     40
#define MAX_UNDOS          100
#define MAX_SS_NAME_WIDTH  63
#define MAX_SS_IDS         225
#define MAX_LINE_PTRS      150   // max number of cached line pointers
#define MAX_PFMT           2000
#define MAX_SELECT_PRINT_LINES 1000
#define MAX_PAL_COLORS     100
#define MAX_BLTS           1500   // maximum type of bullets
#define MAX_JPEG_HUFF_TBLS  8
#define MAX_JPEG_IN_BUF     3
#define MAX_CHAR_TAGS      2000
#define MAX_ANIM_GIFS       100        // number of animated pictures in a gif
#define MAX_EXPAND         4320        // 3 inches max character expansion
#define MAX_NUM_TEXT_LEN   13          // max paragraph numbering text len
#define MAX_FONT_HANDLES   10
#define MAX_TBL_LEVELS     20
#define MAX_TBL_COLS       300         // maximum number of columns in the table
#define MAX_LISTS         1200         
#define MAX_LIST_OR       5000
#define MAX_RTF_LS        5000
#define MAX_TLB_LINES      2
#define MAX_TLB_PER_LINE   100
#define MAX_CUST_ICONS     100
#define MAX_IMAGE_MAPS     50
#define MAX_PAPERS         16
#define MAX_DP_GROUPS      10
#define MAX_HTML_GROUPS     100
#define MAX_HUFF_CODES      104
#define MAX_HUFF_UNIQUE_CODES 91
#define MERGEPRINT_NO_EVAL_MSG 0x1
#define MAX_REVIEWERS       3

#define FILE_OPEN_IN_IE     99999

#define TRACK_NONE          0
#define TRACK_INS           1
#define TRACK_DEL           2

#define SECT_LINE_DIST     360

#define OURDMPAPER_A6      70        // DMPAPER_A6 available only if Win version >=5000, so define our own

#define MIN_VTEXT_CELL_HEIGHT 720

#define UNDO_LINE_LIMIT    5000

#define UNITS_PER_INCH     1440

#define PDF_RES_X          1440
#define PDF_RES_Y          1440

#define PPARAM_LEFT_INDENT    0
#define PPARAM_RIGHT_INDENT   1
#define PPARAM_FIRST_INDENT   2
#define PPARAM_TAB            3
#define PPARAM_BLT            4
#define PPARAM_AUX            5
#define PPARAM_AUX1           6
#define PPARAM_STY            7
#define PPARAM_SHADING        8
#define PPARAM_PFLAGS         9
#define PPARAM_SPACE_BEF      10
#define PPARAM_SPACE_AFT      11
#define PPARAM_SPACE_BET      12
#define PPARAM_LINE_SPACING   13
#define PPARAM_BK_COLOR       14
#define PPARAM_BORDER_SPACE   15
#define PPARAM_FLOW           16
#define PPARAM_FLAGS          17

#define UNDO_INSERT        'I'
#define UNDO_OBJECT_INSERT 'O'
#define UNDO_DELETE        'D'
#define UNDO_FONT          'F'
#define UNDO_PARA          'P'
#define UNDO_REPLACE       'R'
#define UNDO_ROW_DEL       'T'
#define UNDO_ROW_INS       'S'
#define UNDO_CELL_DEL      'U'
#define UNDO_CELL_INS      'V'
#define UNDO_FRAME         '1'
#define UNDO_FRAME_LIMITED '2'
#define UNDO_PICT_SIZE     '3'
#define UNDO_TABLE_ATTRIB  '4'


#define ENGLISH            0x409       // English language id
#define TER_LEFT           0
#define TER_CENTER         1
#define TER_RIGHT          2

#define CWIDTH_WIDTH       0x3FFF
#define CWIDTH_USED        0x4000     // in use 
#define CWIDTH_RTL         0x8000     // last bit 

#define WMTER_MOUSEWHEEL   0x20a
#define WMTER_MOUSEHOVER   0x2a1
#define SMTER_MOUSEWHEELPRESENT  75
#define WHEEL_RADIUS       14

#define TLB_ICON_HEIGHT    20
#define TLB_MARGIN         3 
#define TLB_SPACER_WIDTH   3 

#define TLBFLAG_MOUSE      0x1         // check mouse click
#define TLBFLAG_ENABLED    0x2         // icon enabled
#define TLBFLAG_CHECKED    0x4         // icon checked
#define TLBFLAG_HILIGHTED  0x8         // hilighted - moues over
#define TLBFLAG_COMBO      0x10        // uses combo box
#define TLBFLAG_EDITABLE   0x20        // include an edit box in the comb

#define TEXTFLOW_T2B       1
#define TEXTFLOW_T2B_NON_ANSII  3
#define TEXTFLOW_B2T       2

#define PNGCMP_NONE        0           // no compression
#define PNGCMP_FIXED       1           // fixed huffman code compression
#define PNGCMP_DYNAMIC     2           // dynamic huffman code compression
#define PNGCMP_ERROR       3           // compression error

#define PNGCLR_GRAY          0
#define PNGCLR_TRUE          2
#define PNGCLR_PAL           3
#define PNGCLR_GRAY_ALPHA    4
#define PNGCLR_TRUE_ALPHA    6

#define  PNG_CIRC_BUF_LO 500              // lo point of circular buffer
#define  PNG_CIRC_BUF_HI 1000             // hi point of circular buffer
#define  PNG_MAX_RUN_LEN 258              // max run length

#define CHAR_WIDTH_CACHE_SIZE 0x20000L // char width cache size (128K)

#define  OASPECT_NONE         0         // aspect not yet defined
#define  OASPECT_CONTENT      1         // content
#define  OASPECT_ICON         2         // icon display

#define PARA_BLOCKQUOTE       1        // html misc types
#define PARA_ADDRESS          2
#define PARA_HR               3
#define PARA_PRE              4

#define PASS_HDR              1        // header pass in CreateFrames function
#define PASS_TBL_HDR          2        // table header
#define PASS_BODY             3        // pass body
#define PASS_FTR              4        // footer pass

#define MAX_PASSES            4

#define PSPACE_SINGLE         0        // single line spacing
#define PSPACE_15             1        // one and half
#define PSPACE_20             2        // double
#define PSPACE_ATLEAST        3        // atleast
#define PSPACE_EXACTLY        4        // exactly
#define PSPACE_MULTIPLE       5        // multiple

#define HTML_MISC_FLAG_MASK   0xe00   // miscellaneous flag

#define SSFLAG_ADD         0x1   // additive character style

#define PFLAG_STYLES       0xFFF0;  // style bit masks in PFLAG_
#define SMALL_FILE         5000

#define TSTYLE_IME         0x1      // style to indicate character in ime input
#define TSTYLE_ULINEF      0x2      // fonts create with ULINE turned on

#define PI                  ((double)3.1415926535)

#define FITTED_VIEW_DISP_PAGES 2    // multiply page height by 2 in fitted view

#define FFLAG_MASK         (FFLAG_SCAPS|FFLAG_AUTO_SPELL)  // FFLAG_ used by GetNewFont function

#define TOFLAG_DEL_CELL          0x1  // in table cell deletion
#define TOFLAG_CURS_ON_TABLE_TOP 0x2  // cursor on a table top border
#define TOFLAG_INSERT_CHAR       0x4  // true when inserting character in the TerAscii function
#define TOFLAG_IN_MERGE_PRINT    0x8  // in merge print
#define TOFLAG_EXTERNAL_DC       0x10  // TerMergePrint print to an external device context
#define TOFLAG_WRITING_BKND      0x20 // writing the line background
#define TOFLAG_NO_RESET_FONT     0x40 // don't reset the effective font
#define TOFLAG_FULL_PAINT        0x80 // in TerRepaint
#define TOFLAG_WRAPPING          0x100  // wrapping in progress
#define TOFLAG_DRAWING_FRAME_PICT 0x200  // when drawing the aligned pictures
#define TOFLAG_FNOTE_FRAME       0x400  // drawing the footnote frame
#define TOFLAG_PAINT_WAITING     0x800  // waiting for a paint message
#define TOFLAG_DEBUG             0x1000 // debugging
#define TOFLAG_HILIGHT_OFF       0x2000 // turnoff highlighting after a formatting operation
#define TOFLAG_NO_TOL            0x4000 // no tolerance for frame
#define TOFLAG_NO_FIELD_ADJ      0x8000 // do not adjust the hilight block to include the field completely
#define TOFLAG_NO_ADJUST_POS     0x10000 //  do not adjust the cursor position
#define TOFLAG_MERGE_PRINT_RECT  0x20000 // merge print in a given rectangle MPRect
#define TOFLAG_SET_FOCUS         0x40000 // called from TerSetFocus function
#define TOFLAG_X_BEYOND_LINE     0x80000  // in UnitsToCol the x is beyond the line width
#define TOFLAG_IME_DISABLE       0x100000 // ime disable in progress
#define TOFLAG_REPAINT_MSG       0x200000 // internal repaint message
#define TOFLAG_SET_MOUSE_ON_TEXT_LINE 0x400000 // check mouse on text line
#define TOFLAG_SEL_CELL_CONTENT  0x800000 // do not delete full cell when deleting single cell
#define TOFLAG_NO_RECOVE_ID     0x1000000 // don't recover ids
#define TOFLAG_CONTINUOUS_SCROLL 0x2000000 // continuous vertical scroll
#define TOFLAG_NO_MORE_TAB_MSG   0x4000000 // don't display anymore tab messages
#define TOFLAG_X_BEFORE_LINE     0x8000000  // in UnitsToCol the x is before the line text
#define TOFLAG_EXT_SPELLER      0x10000000  // allow wavy line for external speller
#define TOFLAG_CLEAR_EOL        0x20000000  // clear eol call
#define TOFLAG_UPDATE_ENOTE     0x40000000  // update end note
#define TOFLAG_BREAK_CHAR       0x80000000  // break character during text input

#define TOFLAG2_NO_WRAP            0x1     // disable wrapping
#define TOFLAG2_NO_NORMALIZE_FNOTE 0x2     // do not normalized for fnote/enote
#define TOFLAG2_REPOS_PICT_FRAMES  0x4     // reposition picture frames
#define TOFLAG2_DRAW_FNOTE         0x8     // drawing footnote
#define TOFLAG2_SSE_READ           0x10    // sse file input
#define TOFLAG2_USE_PEN_FOR_BORDER 0x20    // draw border using pen
#define TOFLAG2_FROM_CB            0x40    // data being pasted from clipboard
#define TOFLAG2_PDF                0x80    // pdf being generated
#define TOFLAG2_NO_COMPRESS_CFMT   0x100    // pdf being generated
#define TOFLAG2_CHECK_DISABLED_FRAMES 0x200 // check disabled frames in unit to line conversion
#define TOFLAG2_NO_SET_LANG        0x400   // don't set current language in InsertBuffer
#define TOFLAG2_DEL_CELL_TEXT      0x800   // delete cell text - not cell structure
#define TOFLAG2_NO_NORMALIZE       0x1000  // do not normalized block
#define TOFLAG2_NO_SAVE_UNDO       0x2000  // suspend save undo
#define TOFLAG2_PRINT_TO_META_DC   0x4000  // TerMergePrint to meta dc
#define TOFLAG2_SCALE_PRT_DC       0x8000  // set extension to hPr while printing a page
#define TOFLAG2_USE_UNDO_FRAME     0x10000 // use saved undo frame information

#define WRAPSIDE_BOTH      0
#define WRAPSIDE_LEFT      1
#define WRAPSIDE_RIGHT     2
#define WRAPSIDE_LARGEST   3

#define PAGE_BOX_WIDTH     1100  // width of the page box

#define NUM_WIDTH_CHARS    256
#define BORDER_MARGIN      75
#define PARA_BORDER_MARGIN 50     // left and right para border margins
#define FNOTE_MARGIN       50     // top and bottom space


#define PAGE_BORDER_MARGIN 360    // minimum page border margin

#define ID_FIRST_COMMAND   600

#define MIN_CELL_WIDTH     180    // min cell width in twips

#define DEF_LINE_WIDTH     15     // twip width for status bar drawing

#define SIGN_EOF16         0xBC
#define SIGN_EOF32         0xBA

#define DIB_TO_METAFILE    1

#define PICT_DIBITMAP      0
#define PICT_METAFILE      1
#define PICT_CONTROL       2
#define PICT_META7         3
#define PICT_BITMAP        4
#define PICT_JPEG          5
#define PICT_FORM          6
#define PICT_GIF           7
#define PICT_TIFF          8
#define PICT_ENHMETAFILE   9
#define PICT_PNG          10
#define PICT_SHAPE        11      // shape object

#define SHPTYPE_NONE       -1
#define SHPTYPE_ELLIPSE    3
#define SHPTYPE_LINE       20
#define SHPTYPE_RECT       1
#define SHPTYPE_PICT_FRAME 75
#define SHPTYPE_OCX        201
#define SHPTYPE_TEXT_BOX   202

#define VKIND_NONE         0
#define VKIND_PAGE_LAYOUT  1
#define VKIND_OUTLINE      2
#define VKIND_MASTER       3
#define VKIND_NORMAL       4
#define VKIND_OUTLINE_LAYOUT 5

#define FTYPE_NONE         0
#define FTYPE_DIB          1
#define FTYPE_METAFILE     2
#define FTYPE_JPEG         3
#define FTYPE_GIF          4
#define FTYPE_ENHMETAFILE  5
#define FTYPE_PNG          6
#define FTYPE_TIF          7
#define FTYPE_ERR          8

#define OBJ_NONE           0
#define OBJ_EMBED          1
#define OBJ_LINK           2
#define OBJ_EMBED_TEMP     3
#define OBJ_EMBED_INSERT   4
#define OBJ_OCX            5
#define OBJ_OCX_EXT        6

#define DEFAULT_CFMT       0x0

#define BOLD_WEIGHT        700
#define NORM_WEIGHT        400

#define UNIFORM            0xFFFF

#define EACH_INDENT        720
#define MAX_INDENT         8640
#define MAX_PARA_PER_WIN   30
#define MAX_PARA_PER_PAGE  100

#define REDUCTION          2
#define MIN_FRAME_MARGIN   40       // minimum frame margin
#define FRAME_BORDER_WIDTH 15       // frame border width

#define BOX_BORDER_WIDTH   40

#define PAINT_MIN          1
#define PAINT_LINE         2
#define PAINT_RANGE        3
#define PAINT_WIN          4
#define PAINT_PARTIAL_WIN  5
#define PAINT_WIN_RESET    6

#define PAINT_DIRECT       1
#define PAINT_MEM          2

#define SCROLL_RANGE       1000

#define STATUS_ROW         -1
#define MAX_LINES_PER_WIN  7000

#define RTF_BUF_LEN        4096
#define RTF_LINE_LEN       80

#define RTF_BEGIN          1
#define RTF_END            2
#define RTF_CONTROL        3
#define RTF_TEXT           4
#define RTF_PROPERTY       5
#define RTF_PICT           6
#define RTF_SECT           7
#define RTF_OBJECT         8

#define RTF_FILE_INCOMPLETE 1
#define RTF_SYNTAX_ERROR    2
#define RTF_END_FILE        3

#define RTF_FILE           0
#define RTF_BUF            1
#define RTF_CB             2
#define RTF_CB_BUF         3
#define RTF_FRAME_BUF      4
#define RTF_CB_APPEND      5

#define PARAM_NONE          0
#define PARAM_INT           1
#define PARAM_DBL           2

#define TAB                 9
#define ESCAPE              27
#define QUOTE               34
#define RULER_TOLERANCE     60
#define DEF_TAB_WIDTH       720      // default tab width in twips

#define DISP_PARA_CHAR      182      // actual character to display ParaChar
#define DISP_CELL_CHAR      164      // actual character to display CellChar
#define DISP_LINE_CHAR      171      // actual character to display LINE_CHAR
#define DISP_HYPH_CHAR      172      // acutal character to display HYPH_CHAR
#define DISP_HIDDEN_CHAR    149      // acutal character to display HIDDEN_CHAR

#define DISP_PARA_CHAR_ALT  191      // actual character to display ParaChar
#define DISP_LINE_CHAR_ALT  175      // actual character to display LINE_CHAR

#define BULLET_CHAR         0xB7     // bullet character

#define BFONT_DEF             0     // use the default font for bullets
#define BFONT_SYMBOL          1     // symbol font
#define BFONT_WINGDINGS       2     // wingding font

#define MIN_LINES_PER_PAGE  20

#define TIMER_REPAGE         9182     // timer id to repaginate
#define TIMER_HILIGHT        9183     // to control scrolling
#define TIMER_EVAL           9184     // evaluation timer
#define TIMER_TLB_HILIGHT    9185
#define TIMER_TLB_BALLOON    9186
#define TIMER_TLB_BALLOON_OFF 9187
#define TIMER_WHEEL_UP       9188
#define TIMER_WHEEL_DOWN     9189
#define TIMER_MOUSE_STOP     9190
#define TIMER_DRAGON_FOCUS   9191

#define TIMER_ANIM           9199     // all ids above this id reserved for animation timers

#define BORDER_TOP          0x1
#define BORDER_BOT          0x2
#define BORDER_LEFT         0x4
#define BORDER_RIGHT        0x8
#define BORDER_ALL          0xF

#define BORDER_INDEX_TOP    0
#define BORDER_INDEX_BOT    1
#define BORDER_INDEX_LEFT   2
#define BORDER_INDEX_RIGHT  3

#define BORDER_PARA_TOP     0
#define BORDER_PARA_BOT     1
#define BORDER_PARA_LEFT    2
#define BORDER_PARA_RIGHT   3
#define BORDER_CELL_TOP     4
#define BORDER_CELL_BOT     5
#define BORDER_CELL_LEFT    6
#define BORDER_CELL_RIGHT   7
#define BORDER_ROW_TOP      8
#define BORDER_ROW_BOT      9
#define BORDER_ROW_LEFT    10
#define BORDER_ROW_RIGHT   11
#define BORDER_PARA_ALL    12
#define BORDER_CHAR        13
#define BORDER_SECT_TOP    14
#define BORDER_SECT_BOT    15
#define BORDER_SECT_LEFT   16
#define BORDER_SECT_RIGHT  17
#define BORDER_PARA_BETWEEN 18
#define MAX_BORDERS        19

#define BOX_TOP             0x10
#define BOX_BOT             0x20
#define BOX_LEFT            0x40
#define BOX_RIGHT           0x80
#define BOX_DOUBLE          0x100
#define BOX_THICK           0x200
#define BOX_BETWEEN         0x10000

#define INFO_TAB            0x1
#define INFO_SECT           0x2
#define INFO_PAGE           0x4
#define INFO_COL            0x8
#define INFO_CELL           0x10
#define INFO_ROW            0x20
#define INFO_TABLE          0x40
#define INFO_JUST           0x80
#define INFO_FRAME          0x400
#define INFO_FRM_SPC_BEF    0x800
#define INFO_DYN_FIELD      0x1000

#define HYPER_CODE          HIDDEN

#define FIELD_NAME_STYLE    ULINED

#define PROTECT_COLOR       0x4F4F4F       // color applied to protected text

#define BM_UNCHECKED        0
#define BM_CHECKED          1
#define BM_NONE             2

#define MAX_DATA_LEN        100

#define MAX_PASTE_FORMATS   6
#define PASTE_OBJECT        0
#define PASTE_RTF           1
#define PASTE_TEXT          2
#define PASTE_PICT          3
#define PASTE_BM            4
#define PASTE_DIB           5

#define PASTE_DESC_OBJECT   "Object"
#define PASTE_DESC_RTF      "Formatted Text (RTF)"
#define PASTE_DESC_TEXT     "Unformatted Text"
#define PASTE_DESC_PICT     "Picture"
#define PASTE_DESC_BM       "Bitmap"
#define PASTE_DESC_DIB      "Device Independent Bitmap"

#define OLENOT_CHANGED      0x1
#define OLENOT_SAVED        0x2
#define OLENOT_CLOSED       0x4

#define MSDRAW_CLASS        "MSDraw"

#define FRAME_DIST_FROM_TEXT 180
#define FRAME_DIST_FROM_MARGIN 1440        // minimum distance from the margin

#define TER_IDLE            (WM_USER+10)   // message posted to do idle activity

// drag object types
#define DRAG_TYPE_SCR_FIRST    1
#define DRAG_TYPE_PICT_SIZE    1
#define DRAG_TYPE_FRAME_SIZE   2
#define DRAG_TYPE_FRAME_MOVE   3
#define DRAG_TYPE_SCR_LAST     3

#define DRAG_TYPE_RULER_BEGIN  5
#define DRAG_TYPE_RULER_LEFT   5
#define DRAG_TYPE_RULER_RIGHT  6
#define DRAG_TYPE_RULER_FIRST  7
#define DRAG_TYPE_RULER_TAB    8
#define DRAG_TYPE_RULER_END    8

#define DRAG_TYPE_CELL         9
#define DRAG_TYPE_ROW_INDENT  10
#define DRAG_TYPE_ROW         11

#define DRAG_HOT_LEFT          0
#define DRAG_HOT_RIGHT         1
#define DRAG_HOT_TOP           2
#define DRAG_HOT_BOT           3

#define DRAG_TAB_WIDTH      100

#define FMFLAG_ACTIVE          0x1        // TRUE when mouse is click in the control
#define FMFLAG_SSFLDW_FOUND    0x2      // custom ssfldw tag read from the rtf file

#define RFLAG_CELL_MERGED     0x1      // cell merged with the previous cell
#define RFLAG_END_TABLE       0x2      // end of table - discard other table constructs
#define RFLAG_CURS_IN_TABLE   0x4      // cursor in table
#define RFLAG_OUT_OF_TABLE_ROW  0x8    // out of table row array.
#define RFLAG_OUT_OF_TABLE_CELL 0x10   // out of table cell array.
#define RFLAG_HDRFTR_TEXT_FOUND 0x20     // some text found
#define RFLAG_NEW_FRAME       0x40     // force a new frame.
#define RFLAG_HDR_FTR_FOUND   0x80     // header/footer found in the file
                                       // available bits
#define RFLAG_SECT_HDR_FOUND  0x400    // section header found
#define RFLAG_SECT_FTR_FOUND  0x800    // section footer found
#define RFLAG_PARA_FRAME_FOUND 0x1000  // para frame found
#define RFLAG_IGNORE_SLASH    0x2000   // do not treat backslash as a special character
#define RFLAG_READ_SHAPE_PICT 0x4000   // shape picture read pending
#define RFLAG_INSERT_BEF_PARA 0x8000   // insert text before current paragraph

#define RFLAG1_TROWD_FOUND     0x0001   // TRUE when trowd found
#define RFLAG1_TEXT_FOUND      0x0002   // some text found
#define RFLAG1_PNG_FOUND       0x0004   // png picture format found
#define RFLAG1_SECT_FHDR_FOUND 0x0008  // section first header found
#define RFLAG1_SECT_FFTR_FOUND 0x0010  // section first header found
#define RFLAG1_PARA_ENDED      0x0020   // para ended
#define RFLAG1_FIRST_TABLE     0x0040   // first table row when pasting
#define RFLAG1_SSSUBTABLE_USED 0x0080   // sub table control word used
#define RFLAG1_ROW_SPANNED     0x1000   // row spanned cell
#define RFLAG1_IGNORE_SPACE    0x2000   // ignore space as the word spearator
#define RFLAG1_PASTING_IN_PARA_FRAME 0x4000  // true when pasting inside a para frame
#define RFLAG1_NEW_HDR_FTR    0x8000   // new header footer found in the file

#define RFLAG2_LEFT_MARG_FOUND 0x1     // left margin tag found in the rtf file
#define RFLAG2_CHFTN_FOUND     0x2     // chftn tag found
#define RFLAG2_PAPER_DIM_FOUND 0x4     // TRUE when paper dimension found
#define RFLAG2_PICT_FRAME_INSERTED 0x8 // picture frame inserted

#define ROFLAG_IGNORE_SLASH   0x2      // do not treat backslash as a special character
#define ROFLAG_IN_SUPSCR      0x4      // in superscript group
#define ROFLAG_IN_SUBSCR      0x8      // in subscript group
#define ROFLAG_WRITE_TROWD    0x10     // write the first trowd
#define ROFLAG_NO_FLUSH       0x20     // do not flush
#define ROFLAG_IN_ANIM        0x40     // writing the animation sequence
#define ROFLAG_NO_ANIM        0x80     // ignore animation
#define ROFLAG_HIDDEN_TEXT    0x100    // in hidden text
#define ROFLAG_IN_FIELD_NAME  0x200    // writing the field name
#define ROFLAG_CELL_CLOSED    0x400    // TRUE after writing the \cell tag
#define ROFLAG_NO_CRLF        0x800    // no cr/lf in the rtf output
#define ROFLAG_IN_FIELD_TC   0x1000    // writing the tc field group
#define ROFLAG_IN_FIELD_XE   0x2000    // writing the xe field group
#define ROFLAG_IN_MERGE_FIELD 0x4000   // in mail-merge field group
#define ROFLAG_WRITE_FONT     0x8000   // write full font
#define ROFLAG_IN_STYLESHEET  0x10000  // writing stylesheet items

#define ROGFLAG_WRITE_FONT    0x1      // write full font

#define FRAME_RIGHTMOST      0x1      // rightmost frame
#define FRAME_DIVIDER_LINE   0x2      // draw a divider line at the bottom
#define FRAME_ON_SCREEN      0x4      // frame displayed on the screen
#define FRAME_NON_TEXT_DO    0x8      // line, rect objects
#define FRAME_TRANSPARENT    0x10     // do not clear the background
#define FRAME_BORDER_LEFT    0x20     // frame for left page border
#define FRAME_BORDER_RIGHT   0x40     // frame for right page border
#define FRAME_BORDER_TOP     0x80     // frame for top page border
#define FRAME_BORDER_BOT     0x100    // frame for bottom page border
#define FRAME_LAST_COL       0x200    // last column frame
#define FRAME_FORCE_BKND_CLR 0x400    // force the background color even if same as the default background color
#define FRAME_PICT           0x800    // picture frame
#define FRAME_DISABLED       0x1000   // frames that does not allow cursor positioning
#define FRAME_FNOTE          0x2000   // footnote frame
#define FRAME_GRAY           0x4000   // display text in this frame in grayed out color
#define FRAME_TOP_SPAN       0x8000   // page top spanned row
#define FRAME_PAGE_BOX       0x10000  // page border for the section
#define FRAME_RTL            0x20000  // this frame rtl mapped
#define FRAME_RTL_CHECKED    0x40000  // this frame checked for rtl mapping
#define FRAME_FIRST_ROW_FRAME 0x80000 // first frame for a row
#define FRAME_LAST_ROW_FRAME 0x100000 // lastt frame for a row
#define FRAME_WATERMARK      0x200000 // watermark frame

#define FRAME_BORDER (FRAME_BORDER_LEFT|FRAME_BORDER_RIGHT|FRAME_BORDER_TOP|FRAME_BORDER_BOT)

#define FRAME1_PART_EMPTY_CELL 0x1    // partially empty cell
#define FRAME1_PARA_FRAME_BOX  0x2    // box frame containing para-frame text

#define CURS_RESET           0        // used for cursor adjustment for hidden text
#define CURS_FORWARD         1
#define CURS_BACKWARD        2
#define CURS_UP              3
#define CURS_DOWN            4

#define PV_PAGE_DIST         1        // print preview page distance

#define PARA_FRAME_MIN_WIDTH 360     // minimum width of a para frame

#define WIN_BOT_MARGIN       (ScrResY/4)

#define BFLAG_BOX_FIRST      0x1      // first character of the boxed format
#define BFLAG_BOX_LAST       0x2      // last character of the boxed format

#define SPILL_LEFT           0x1
#define SPILL_RIGHT          0x2
#define SPILL_TOP            0x4
#define SPILL_BOT            0x8

#define PARA_BOX_SPACE       20   // space before/after the para border
#define PARA_BORDER_WIDTH    30   // space reserved for a single line paragraph border

#define MSG_MAX_DRAG         0x1      // out of drag table
#define MSG_MAX_PFMT         0x2      // out of para table

#define GFLAG_DRAW_OBJECT    0x1      // group flag - is a drawing object
#define GFLAG_DO_TEXT_READ   0x4      // draw object text box is being read
#define GFLAG_CHAR_STYLE     0x8      // current group has character style information
#define GFLAG_IN_STYLESHEET  0x10     // denotes the stylesheet group
#define GFLAG_IN_STYLE_ITEM  0x20     // denotes the stylesheet item group
#define GFLAG_NEW_FRAME      0x40     // group that begins a new drawing object frame
#define GFLAG_IN_FRAME       0x80     // group belongs to a para frame
#define GFLAG_IN_SHPRSLT     0x100    // in shape result
#define GFLAG_DPLINEHOLLOW   0x200    // dplinehollow tag encountered
#define GFLAG_ANIM_SEQ       0x400    // in ssanimpict sequence group
#define GFLAG_ANIM_INFO      0x800    // animation information found
#define GFLAG_CAPS           0x1000   // convert text to caps
#define GFLAG_IN_FIELD       0x2000   // processing a field
#define GFLAG_INSERT_BEF_PARA 0x4000   // insert before the current paragraph
#define GFLAG_INPUT_FIELD    0x8000   // text input field encountered
#define GFLAG_BACKGROUND     0x10000  // in backgrond group
#define GFLAG_IN_MERGE_FIELD 0x20000  // in merge field
#define GFLAG_IN_TOC_FIELD   0x40000  // processing a TOC field
#define GFLAG_IN_NESTTABLEPROPS 0x80000 // in nesttableprops group
#define GFLAG_CELL_MARGIN_OVERRIDE 0x100000 // cell margin being overridden
#define GFLAG_GOT_WIDTH      0x200000 // got width during rtf read
#define GFLAG_GOT_HEIGHT     0x400000 // got height during rtf read
#define GFLAG_USE_SHP_INFO   0x800000 // use shape info to construct para frames
#define GFLAG_NO_UNICODE     0x1000000 // do not process unicode text
#define GFLAG_IN_SHP         0x2000000 // in shp group
#define GFLAG_IN_DPGROUP     0x4000000 // in dp group header
#define GFLAG_IN_LEVELTEXT   0x8000000 // in leveltext group header
#define GFLAG_IN_LEVELNUM   0x10000000 // in levelnumbers group header
#define GFLAG_IN_SHPGRP     0x20000000 // in shapegroup
#define GFLAG_IN_LIST       0x40000000 // in list definition
#define GFLAG_IN_LIST_OR    0x80000000 // in list-or definition

#define GFLAG2_PICT_SHAPE        0x1   // creating picture shape
#define GFLAG2_NO_MBCS           0x2   // do not treat the current text as mbcs text

#define CHAR_MB              0        // multiple byte character
#define CHAR_LO              1        // single byte low character
#define CHAR_HI              2        // single byte high character
#define CHAR_DB              3        // double byte characters

#define DOL_HORZ             0        // drawing object line type
#define DOL_VERT             1        // vertical line
#define DOL_DIAG             2        // diagnal line
#define DOL_BDIAG            3        // backward diagnal line

#define SNAP_DIST           90       // ruler snaps at 90 twips = 1/16 inch
#define SNAP_DIST_METRIC    71       // ruler snaps at 71 twips = 1/8 cm

#define ST_MAX_WORD_LEN     40
#define ST_MAX_SUG_WORDS    12

#define MIN_TEXT_WIDTH      .5
#define MIN_TEXT_HEIGHT     .05      // .5

#define PAGE_HARD            0x1      // hard page break
#define PAGE_FIRST_SECT_PAGE 0x2      // first page of the section (top section)
#define PAGE_REPAGE          0x4      // repaginate this page
#define PAGE_BOXED           0x8      // this page has section border

#define CAUX_PAGINATION_DONE 0x1      // used during pagination to indicate that the cell is paginated for a the current page
#define CAUX_SET_TOP_SPACE   0x2      // reserve space on the top of the cell - mainly to separate the subtable at the top
#define CAUX_CELLX_WAITING   0x4      // cellx not encounterd yet for the cell during rtf read
#define CAUX_CLOSED          0x8      // \cell encountered
#define CAUX_HIDDEN          0x10     // cell hidden

#define TAUX_TROWD_DONE        0x1      // trowd called for the row during rtf read
#define TAUX_REUSE_CELL_CHAIN  0x2      // reuse the cell chain created for this row (during rtf read)
#define TAUX_RTF_DEL           0x4      // deletge after rtf input
#define TAUX_HAS_NEW_TEXT      0x10     // this flag is set after encountering text for a row 
#define TAUX_COPIED            0x20     // row copied from previous row during rtf read

#define PAT_NONE               0
#define PAT_SOLID              1
#define PAT_OTHER              2


#define IDP_FIRST_POPUP      1500

#define IDP_FIRST_SPELL      1500
#define IDP_FIRST_WORD       1500
#define IDP_WORD1            1500
#define IDP_WORD2            1501
#define IDP_WORD3            1502
#define IDP_WORD4            1503
#define IDP_WORD5            1504
#define IDP_WORD6            1505
#define IDP_LAST_WORD        1505

#define IDP_IGNORE           1510
#define IDP_ADD              1511
#define IDP_LAST_SPELL       1511

#define IDP_LAST_POPUP       1511



// Short type defintions
#if defined (INC_OLE1)
   typedef int   (CALLBACK *CLIENT_CALLBACK)(LPOLECLIENT,OLE_NOTIFICATION,LPOLEOBJECT);
   typedef DWORD (CALLBACK *OLESTREAM_GET)(LPOLESTREAM,void FAR *,DWORD);
   typedef DWORD (CALLBACK *OLESTREAM_PUT)(LPOLESTREAM,OLE_CONST void FAR *,DWORD);
#else
   #define OLECLIPFORMAT WORD
   #if !defined(WIN32)
      typedef LPSTR LPOLESTR;
   #endif
#endif

// unit conversion macros
#define    PointsToScrX(x)  (MulDiv(x,ScrResX,72))
#define    ScrToPointsX(x)  (MulDiv(x,72,ScrResX))

#define    PointsToScrY(x)  (MulDiv(x,ScrResY,72))
#define    ScrToPointsY(x)  (MulDiv(x,72,ScrResY))

#define    PointsToPrtX(x)  (MulDiv(x,PrtResX,72))
#define    PrtToPointsX(x)  (MulDiv(x,72,PrtResX))

#define    PointsToPrtY(x)  (MulDiv(x,PrtResY,72))
#define    PrtToPointsY(x)  (MulDiv(x,72,PrtResY))

#define    TwipsToScrX(x)     (MulDiv(x,ScrResX,1440))
#define    TwipsToOrigScrX(x) (MulDiv(x,OrigScrResX,1440))
#define    ScrToTwipsX(x)     (MulDiv(x,1440,ScrResX))
#define    OrigScrToTwipsX(x)     (MulDiv(x,1440,OrigScrResX))

#define    TwipsToUnitX(x)     (MulDiv(x,UNITS_PER_INCH,1440))
#define    UnitToTwipsX(x)     (MulDiv(x,1440,UNITS_PER_INCH))

#define    TwipsToUnitY(x)     (MulDiv(x,UNITS_PER_INCH,1440))
#define    UnitToTwipsY(x)     (MulDiv(x,1440,UNITS_PER_INCH))

#define    TwipsToScrY(x)      (MulDiv(x,ScrResY,1440))
#define    TwipsToOrigScrY(x)  (MulDiv(x,OrigScrResY,1440))
#define    ScrToTwipsY(x)      (MulDiv(x,1440,ScrResY))
#define    OrigScrToTwipsY(x)  (MulDiv(x,1440,OrigScrResY))

#define    TwipsToPrtX(x)  (MulDiv(x,PrtResX,1440))
#define    PrtToTwipsX(x)  (MulDiv(x,1440,PrtResX))

#define    TwipsToPrtY(x)  (MulDiv(x,PrtResY,1440))
#define    PrtToTwipsY(x)  (MulDiv(x,1440,PrtResY))

#define    ScrToPrtX(x)    (MulDiv(x,PrtResX,ScrResX))
#define    PrtToScrX(x)    (MulDiv(x,ScrResX,PrtResX))

#define    ScrToPrtY(x)    (MulDiv(x,PrtResY,ScrResY))
#define    PrtToScrY(x)    (MulDiv(x,ScrResY,PrtResY))

#define    MmToPrtX(x)     (MulDiv(x,PrtResX*10,254))
#define    MmToPrtY(x)     (MulDiv(x,PrtResY*10,254))

#define    TwipsToInches(x)   (((float)(x))/1440)
#define    InchesToTwips(x)   ((long)((x)*1440))

#define    TwipsToCm(x)     (((float)(x)*254)/((float)1440*(float)(100)))
#define    CmToTwips(x)     ((long)((((float)x)*1440*100)/254))

#define    PointsToInches(x)   (((float)(x))/72)
#define    InchesToPoints(x)   ((long)((x)*72))

#define    TwipsToPoints(x)   ((x)/20)
#define    PointsToTwips(x)   ((x)*20)

#define    EmuToTwips(x)      ((x)/635)
#define    TwipsToEmu(x)      ((x)*635)

#define    MmToInches(x)       (((float)(x))/(float)25.4)
#define    InchesToMm(x)       (((float)(x))*(float)25.4)

#define    CmToInches(x)       (((float)(x))/(float)2.54)
#define    InchesToCm(x)       (((float)(x))*(float)2.54)

#define    PrtToInchesX(x)     (((float)x)/PrtResX)
#define    PrtToInchesY(x)     (((float)x)/PrtResY)

#define    ResetUintFlag(flags,flag)  (flags=(UINT)(flags)&(~((UINT)(flag))))
#define    ResetLongFlag(flags,flag)  (flags=(DWORD)(flags)&(~((DWORD)(flag))))
#define    ResetFlag(flags,flag)      ((UINT)(flags)&(~((UINT)(flag))))

#define    MakeWord(x,y)              ((((WORD)(BYTE)(x))<<8)|(y))
#define    WrapChar(x)                (LeadWrapped?MakeWord(WrapLead[(x)],wrap[(x)]):wrap[(x)])
#define    WideCharWidth(x1,x2,x3,y)  (DblCharWidth(x1,x2,x3,(BYTE)((y)&0xFF),(BYTE)((y)>>8)))

#define    IsHdrFtrChar(x)            (x==HDR_CHAR || x==FHDR_CHAR || x==FTR_CHAR || x==FFTR_CHAR)
#define    IsLcChar(x)                (x>='a' && x<='z')

#define    IsFootnoteStyle(x)         ((BOOL)((x&FNOTETEXT && !(x&ENOTETEXT))))
#define    IsSectionBorder(x)         (x>=BORDER_SECT_TOP && x<=BORDER_SECT_RIGHT)

/******************************************************************************
                         FAR/NEAR SELECTION
    Although TER is compiled in the medium model, in the Borland environment
    it is linked with the large model libraries.  The following variable
    defines the TERFAR constant accordingly.  This constant is used for
    modifying certain library functions.
*******************************************************************************/
#if defined(__TURBOC__)
   #ifdef __DLL__
      #define TERFAR far
      #define TERDLL
   #else
      #define TERFAR
   #endif
   #define DllMain DllEntryPoint
#else
   #ifdef _WINDLL
      #define TERDLL
   #endif
   #define TERFAR
   #define mktemp _mktemp
#endif

/******************************************************************************
    WIN32 specific defines
*******************************************************************************/
#define TER32
#if defined (TER32)
   #undef  huge
   #define huge
   #undef  FreeProcInstance
   #define FreeProcInstance(x)
   #define GET_WM_COMMAND_ID(wp,lp) LOWORD(wp)
   #define CONTROL_ID(wp,lp)        LOWORD(wp)
   #define NOTIFY_MSG(wp,lp)        HIWORD(wp)
   #define SCROLL_MSG(wp,lp)        LOWORD(wp)
   #define THUMB_POS(wp,lp)         HIWORD(wp)
   #undef  farmalloc
   #define farmalloc  malloc
   #undef  farrealloc
   #define farrealloc realloc
   #undef  farfree
   #define farfree    free
   #define SIGN_EOF   SIGN_EOF32
#else
   #define GET_WM_COMMAND_ID(wp,lp) (wp)
   #define CONTROL_ID(wp,lp)        (wp)
   #define NOTIFY_MSG(wp,lp)        HIWORD(lp)
   #define SCROLL_MSG(wp,lp)        (wp)
   #define THUMB_POS(wp,lp)         LOWORD(lp)
   #define CCSTYLEFLAG DWORD
   #define SIGN_EOF    SIGN_EOF16
   #define HENHMETAFILE             HANDLE
   #define BI_BITFIELDS             3
#endif

#if !defined(USE_WININET)
   #define HINTERNET                HANDLE
#endif

#define true  TRUE
#define false FALSE
#define null  NULL
#define True(x)                     (x)
#define False(x)                    (!(x))
#define bool                        BOOL

/******************************************************************************
     The following definitions are used to enhance the readability of the window
     specific global variables.  For example, instead of using ((w->TerArg),
     the program simply uses TerArg.
*******************************************************************************/

/******************************************************************************
     Text line data arrays
*******************************************************************************/
#define TerArg          (w->TerArgW)
#define LinePtr         (w->LinePtrW)
#define LinePtrCache    (w->LinePtrCacheW)
#define LinePtrCount    (w->LinePtrCountW)

#define pText(l)        (((w->LinePtrW)[l])->pText)
#define pLead(l)        (((w->LinePtrW)[l])->pLead)
#define pCtid(l)        (((w->LinePtrW)[l])->pCtid)
#define ctid(l,i)       ((((w->LinePtrW)[l])->pCtid)[i])
#define pCharWidth(l)   (((w->LinePtrW)[l])->pCharWidth)
#define CWidth(l,i)     ((((w->LinePtrW)[l])->pCharWidth)[i])
#define cfmt(l)         (((w->LinePtrW)[l])->cfmt)
#define tabw(l)         (((w->LinePtrW)[l])->tabw)
#define LineLen(l)      (((w->LinePtrW)[l])->LineLen)
#define pfmt(l)         (((w->LinePtrW)[l])->pfmt)
#define LineHt(l)       (((w->LinePtrW)[l])->LineHt)
#define BaseLineHt(l)   (((w->LinePtrW)[l])->BaseLineHt)
#define ScrLineHt(l)    (((w->LinePtrW)[l])->ScrLineHt)
#define LineY(l)        (((w->LinePtrW)[l])->LineY)
#define LineX(l)        (((w->LinePtrW)[l])->LineX)
#define cid(l)          (((w->LinePtrW)[l])->cid)
#define fid(l)          (((w->LinePtrW)[l])->fid)
#define LineFlags(l)    (((w->LinePtrW)[l])->LineFlags)
#define LineFlags2(l)   (((w->LinePtrW)[l])->LineFlags2)
#define LinePage(l)     (((w->LinePtrW)[l])->LinePage)
#define LineFrame(l)    (((w->LinePtrW)[l])->LineFrame)
#define JustAdjX(l)     (((w->LinePtrW)[l])->JustAdjX)

/******************************************************************************
     Screen display control variables
*******************************************************************************/
#define hTerWnd         (w->hTerWndW)
#define hTerDC          (w->hTerDCW)
#define hScrollBM       (w->hScrollBMW)
#define hBkPictBM       (w->hBkPictBMW)
#define hBufBM          (w->hBufBMW)
#define hOrigBufBM      (w->hOrigBufBMW)
#define hInitBkBM       (w->hInitBkBMW)
#define hBufDC          (w->hBufDCW)
#define hPr             (w->hPrW)
#define hTerRegFont     (w->hTerRegFontW)
#define hTerCurFont     (w->hTerCurFontW)
#define hRulerFont      (w->hRulerFontW)
#define hRulerFontBold  (w->hRulerFontBoldW)
#define hStatusFont     (w->hStatusFontW)
#define DelRulerFont    (w->DelRulerFontW)
#define DelRulerFontBold (w->DelRulerFontBoldW)
#define DelStatusFont   (w->DelStatusFontW)
#define UseScrMetrics   (w->UseScrMetricsW)
#define EnterHit        (w->EnterHitW)
#define TerTextMet      (w->TerTextMetW)
#define hWaitCursor     (w->hWaitCursorW)
#define TerRect         (w->TerRectW)
#define TerWinRect      (w->TerWinRectW)
#define RulerRect       (w->RulerRectW)
#define ClipRect        (w->ClipRectW)
#define DirtyRect       (w->DirtyRectW)

#define CurRow          (w->CurRowW)
#define CurCol          (w->CurColW)
#define EditCol         (w->EditColW)
#define EditWordIndex   (w->EditWordIndexW)
#define EditWordLen     (w->EditWordLenW)
#define CurLine         (w->CurLineW)
#define EditLine        (w->EditLineW)
#define CurLineY        (w->CurLineYW)
#define BeginLine       (w->BeginLineW)
#define TotalLines      (w->TotalLinesW)
#define MaxLines        (w->MaxLinesW)
#define PagingMargin    (w->PagingMarginW)
#define WinWidth        (w->WinWidthW)
#define WinHeight       (w->WinHeightW)
#define LineWidth       (w->LineWidthW)
#define PaintFlag       (w->PaintFlagW)
#define TabWidth        (w->TabWidthW)
#define HorScrollPos    (w->HorScrollPosW)
#define VerScrollPos    (w->VerScrollPosW)
#define VerThumbSize    (w->VerThumbSizeW)
#define HorThumbSize    (w->HorThumbSizeW)
#define MinThumbHt      (w->MinThumbHtW)
#define TerWinWidth     (w->TerWinWidthW)
#define TerWinHeight    (w->TerWinHeightW)
#define CurPageWidth    (w->CurPageWidthW)
#define CurPageHeight   (w->CurPageHeightW)
#define PageHeightAdj   (w->PageHeightAdjW)
#define FirstPageHeight (w->FirstPageHeightW)
#define FirstPage2Frame (w->FirstPage2FrameW)
#define FirstFramePage  (w->FirstFramePageW)
#define LastFramePage   (w->LastFramePageW)
#define CurTextHeight   (w->CurTextHeightW)
#define TerWinOrgX      (w->TerWinOrgXW)
#define TerWinOrgY      (w->TerWinOrgYW)
#define RulerClicked    (w->RulerClickedW)
#define FontsReleased   (w->FontsReleasedW)
#define WindowDestroyed (w->WindowDestroyedW)
#define ReclaimResources (w->ReclaimResourcesW)
#define PageHasControls (w->PageHasControlsW)
#define HasOverlayingFrames (w->HasOverlayingFramesW)
#define MovingControl   (w->MovingControlW)
#define HtmlMode        (w->HtmlModeW)
#define HoldMessages    (w->HoldMessagesW)
#define FirstPalette    (w->FirstPaletteW)
#define BorderShowing   (w->BorderShowingW)
#define OnWmPaint       (w->OnWmPaintW)
#define MetaPalFound    (w->MetaPalFoundW)
#define DraggingText    (w->DraggingTextW)
#define InOleDrag       (w->InOleDragW)
#define NoDropList      (w->NoDropListW)
#define InDragDrop      (w->InDragDropW)
#define Clr256Display   (w->Clr256DisplayW)
#define SkipCommand     (w->SkipCommandW)
#define InPreprocess    (w->InPreprocessW)
#define RedrawBorder    (w->RedrawBorderW)
#define PrivatePrtDC    (w->PrivatePrtDCW)
#define DrawBknd        (w->DrawBkndW)
#define PagesShowing    (w->PagesShowingW)
#define PageBreakShowing (w->PageBreakShowingW)
#define ShowPageBreak    (w->ShowPageBreakW)
#define ContinuousScroll (w->ContinuousScrollW)
#define NextX           (w->NextXW)
#define NextTab         (w->NextTabW)
#define NextSpace       (w->NextSpaceW)
#define RulerFontHeight (w->RulerFontHeightW)
#define StatusFontHeight (w->StatusFontHeightW)
#define StatusLineWidth (w->StatusLineWidthW)
#define ScrResX         (w->ScrResXW)
#define ScrResY         (w->ScrResYW)
#define OrigScrResX     (w->OrigScrResXW)
#define OrigScrResY     (w->OrigScrResYW)
#define PrtResX         (w->PrtResXW)
#define PrtResY         (w->PrtResYW)
#define PaintEnabled    (w->PaintEnabledW)
#define FrameRefreshEnabled (w->FrameRefreshEnabledW)
#define WrapPending     (w->WrapPendingW)
#define WrapSpellChecked (w->WrapSpellCheckedW)
#define RulerPending    (w->RulerPendingW)
#define RulerLineX      (w->RulerLineXW)
#define RulerSection    (w->RulerSectionW)
#define InAccelerator   (w->InAcceleratorW)
#define CommandSkipped  (w->CommandSkippedW)
#define CursHorzPos     (w->CursHorzPosW)
#define OverhangFont    (w->OverhangFontW)
#define PaintBegLine    (w->PaintBegLineW)
#define PaintEndLine    (w->PaintEndLineW)
#define WinYOffset      (w->WinYOffsetW)
#define WinYOffsetLine  (w->WinYOffsetLineW)
#define DocHeight       (w->DocHeightW)
#define CommandId       (w->CommandIdW)
#define PrevCmdId       (w->PrevCmdIdW)
#define CmdModified     (w->CmdModifiedW)
#define MessageId       (w->MessageIdW)
#define RepaintNo       (w->RepaintNoW)
#define BorderSpill     (w->BorderSpillW)
#define LeftBorderWidth (w->LeftBorderWidthW)
#define TopBorderHeight (w->TopBorderHeightW)
#define BotBorderHeight (w->BotBorderHeightW)
#define transparent     (w->transparentW)
#define EnableTransparent (w->EnableTransparentW)
#define ScrYOffset      (w->ScrYOffsetW)
#define StatusBarHeight (w->StatusBarHeightW)
#define DispTextHt      (w->DispTextHtW)
#define GlbPrtDCId      (w->GlbPrtDCIdW)
#define FrameRefreshLineCount (w->FrameRefreshLineCountW)
#define CurInputField   (w->CurInputFieldW)
#define ClipTblLevel    (w->ClipTblLevelW)
#define ClipEmbTable    (w->ClipEmbTableW)
#define ViewKind        (w->ViewKindW)
#define SpellPending    (w->SpellPendingW)
#define SpellCheckerPopped (w->SpellCheckerPoppedW)
#define PrtLeftMarg     (w->PrtLeftMargW)
#define PrtTopMarg      (w->PrtTopMargW)
#define ScrFrameAngle   (w->ScrFrameAngleW)
#define FrameDistFromMargin  (w->FrameDistFromMarginW)
#define DocTextFlow     (w->DocTextFlowW)
#define mbcs            (w->mbcsW)
#define ScrRtl          (w->ScrRtlW)
#define SpellLine       (w->SpellLineW)
#define SpellCol        (w->SpellColW)
#define WrapTextFlow    (w->WrapTextFlowW)
#define ImgDenX         (w->ImgDenXW)
#define ImgDenY         (w->ImgDenYW)

#define CrNewLine       (w->CrNewLineW)
#define CrSplitLine     (w->CrSplitLineW)
#define TabAlign        (w->TabAlignW)
#define JoinLines       (w->JoinLinesW)
#define Notified        (w->NotifiedW)

#define StatusRowPos    (w->StatusRowPosW)
#define StatusColPos    (w->StatusColPosW)
#define StatusLinePos   (w->StatusLinePosW)

#define CurClipRgn      (w->CurClipRgnW)

#define pTerPal         (w->pTerPalW)
#define hTerPal         (w->hTerPalW)
#define TempLead        (w->TempLeadW)
#define pFontInUse      (w->pFontInUseW)
#define FontInUseSize   (w->FontInUseSizeW)
#define pScrSeg         (w->pScrSegW)
#define TotalScrSeg     (w->TotalScrSegW)
#define CurScrSeg       (w->CurScrSegW)

#define KeyDownUniChar  (w->KeyDownUniCharW)
#define DeadChar        (w->DeadCharW)

/******************************************************************************
     Text Frame Variables
*******************************************************************************/
#define hFrame          (w->hFrameW)
#define frame           (w->frameW)
#define CurFrame        (w->CurFrameW)
#define TotalFrames     (w->TotalFramesW)
#define MaxFrames       (w->MaxFramesW)
#define NewFrameWidth   (w->NewFrameWidthW)
#define NewFrameHeight  (w->NewFrameHeightW)
#define NewFrameX       (w->NewFrameXW)
#define NewFrameY       (w->NewFrameYW)
#define NewFrameVPage   (w->NewFrameVPageW)
#define FramesSorted    (w->FramesSortedW)

/******************************************************************************
                    Text to Screen mapping variables
*******************************************************************************/
#define RowY            (w->RowYW)
#define RowHeight       (w->RowHeightW)
#define RowX            (w->RowXW)

/******************************************************************************
                    auto-complete variables
*******************************************************************************/
#define InAutoComp       (w->InAutoCompW)
#define TotalAutoComps   (w->TotalAutoCompsW)
#define AutoCompPos      (w->AutoCompPosW)
#define AutoCompWord     (w->AutoCompWordW)
#define AutoCompPhrase   (w->AutoCompPhraseW)

/******************************************************************************
                       Text segment structure
*******************************************************************************/
#define TextSeg         (w->TextSegW)
#define TotalSegments   (w->TotalSegmentsW)
#define MaxSegs         (w->MaxSegsW)
#define UseTextMap      (w->UseTextMapW)
#define TabFlags        (w->TabFlagsW)
#define TextBorder      (w->TextBorderW)
#define ParaShading     (w->ParaShadingW)
#define PaintFrameFlags (w->PaintFrameFlagsW)
#define ParaFrameSpace  (w->ParaFrameSpaceW)
#define FrameBkColor    (w->FrameBkColorW)
#define ParaBackColor   (w->ParaBackColorW)
#define ParaBoxSpace    (w->ParaBoxSpaceW)
#define ShadeBegin      (w->ShadeBeginW)
#define ShadeEnd        (w->ShadeEndW)

/******************************************************************************
     MBCS variables
*******************************************************************************/
#define LeadWaiting     (w->LeadWaitingW)

/******************************************************************************
     Mouse control variables
*******************************************************************************/
#define MouseCol        (w->MouseColW)
#define MouseLine       (w->MouseLineW)
#define MousePictFrame  (w->MousePictFrameW)
#define MouseX          (w->MouseXW)
#define MouseY          (w->MouseYW)
#define MouseOverShoot  (w->MouseOverShootW)
#define MouseOverShootDist (w->MouseOverShootDistW)
#define IgnoreMouseMove (w->IgnoreMouseMoveW)
#define MouseOnTextLine  (w->MouseOnTextLineW)
#define VerySmallMovement  (w->VerySmallMovementW)
#define WheelX          (w->WheelXW)
#define WheelY          (w->WheelYW)
#define WheelShowing    (w->WheelShowingW)
#define WheelTimerUpOn   (w->WheelTimerUpOnW)
#define WheelTimerDownOn (w->WheelTimerDownOnW)
#define hWheelCur        (w->hWheelCurW)

/******************************************************************************
     Word Wrapping related variables
*******************************************************************************/
#define DocCode           (w->DocCodeW)
#define DocBegin          (w->DocBeginW)
#define WrapMaxLines      (w->WrapMaxLinesW)
#define WrapAddLines      (w->WrapAddLinesW)
#define WrapBufferSize    (w->WrapBufferSizeW)
#define ParaChar          (w->ParaCharW)
#define OldParaChar       (w->OldParaCharW)
#define DocExt            (w->DocExtW)
#define wrap              (w->wrapW)
#define hWrap             (w->hWrapW)
#define BufferLength      (w->BufferLengthW)
#define MaxBufferLength   (w->MaxBufferLengthW)
#define LastBufferedLine  (w->LastBufferedLineW)
#define LastWrappedLine   (w->LastWrappedLineW)
#define WrapFlag          (w->WrapFlagW)
#define WrapWidthChars    (w->WrapWidthCharsW)
#define WrapWidthTwips    (w->WrapWidthTwipsW)
#define DoExtraPass       (w->DoExtraPassW)
#define WrapCtid          (w->WrapCtidW)
#define WrapCharWidth     (w->WrapCharWidthW)
#define WrapCharWidthOrder (w->WrapCharWidthOrderW)
#define WrapCharWidthDX   (w->WrapCharWidthDXW)
#define WrapCharWidthText (w->WrapCharWidthTextW)
#define WrapCharWidthClass (w->WrapCharWidthClassW)
#define WrapLead          (w->WrapLeadW)
#define LeadWrapped       (w->LeadWrappedW)

/*****************************************************************************
                   Character Formatting Variables
******************************************************************************/
#define CfmtSign          (w->CfmtSignW)
#define WrapCfmt          (w->WrapCfmtW)
#define hWrapCfmt         (w->hWrapCfmtW)
#define TempCfmt          (w->TempCfmtW)
#define pLineCharWidth    (w->pLineCharWidthW)
#define LineCfmt          (w->LineCfmtW)
#define LineLead          (w->LineLeadW)
#define LineText          (w->LineTextW)
#define CurFmtId          (w->CurFmtIdW)
#define InputFontId       (w->InputFontIdW)

/*****************************************************************************
                   Character Tag Variables
******************************************************************************/
#define CharTag           (w->CharTagW)
#define TotalCharTags     (w->TotalCharTagsW)
#define MaxCharTags       (w->MaxCharTagsW)
#define TagsWrapped       (w->TagsWrappedW)
#define CharWidthWrapped  (w->CharWidthWrappedW)

/******************************************************************************
                    Paragraph formatting variables
******************************************************************************/
#define hPfmtId           (w->hPfmtIdW)
#define PfmtId            (w->PfmtIdW)
#define TotalPfmts        (w->TotalPfmtsW)
#define MaxPfmts          (w->MaxPfmtsW)
#define CurPfmt           (w->CurPfmtW)
#define CurWrapPfmt       (w->CurWrapPfmtW)
#define WrapParaFont      (w->WrapParaFontW)

/******************************************************************************
                    Paragraph bullet/numbering variables
******************************************************************************/
#define TerBlt            (w->TerBltW)
#define TotalBlts         (w->TotalBltsW)
#define MaxBlts           (w->MaxBltsW)

/******************************************************************************
                    Paragraph list table
******************************************************************************/
#define list              (w->listW)
#define TotalLists        (w->TotalListsW)
#define MaxLists          (w->MaxListsW)
#define ListOr            (w->ListOrW)
#define TotalListOr       (w->TotalListOrW)
#define MaxListOr         (w->MaxListOrW)


/******************************************************************************
                       Font and Picture Control Data
******************************************************************************/
#define FmtSign           (w->FmtSignW)
#define FmtSignOld        (w->FmtSignOldW)
#define TerFont           (w->TerFontW)
#define PrtFont           (w->PrtFontW)
#define TerTab            (w->TerTabW)
#define hTerTab           (w->hTerTabW)
#define TerSect           (w->TerSectW)
#define TerSect1          (w->TerSect1W)
#define TotalFonts        (w->TotalFontsW)
#define NextFontId        (w->NextFontIdW)
#define MaxFonts          (w->MaxFontsW)
#define NextFontAux1Id    (w->NextFontAux1IdW)
#define NextParaAux1Id    (w->NextParaAux1IdW)
#define UsingZoomFonts    (w->UsingZoomFontsW)
#define HasVarWidthFont   (w->HasVarWidthFontW)
#define LinkPictDir       (w->LinkPictDirW)
#define TotalTypes        (w->TotalTypesW)
#define TotalPointSizes   (w->TotalPointSizesW)
#define TotalTabs         (w->TotalTabsW)
#define DefTabWidth       (w->DefTabWidthW)
#define DefTabType        (w->DefTabTypeW)
#define TotalSects        (w->TotalSectsW)
#define WrapSect          (w->WrapSectW)
#define MaxSects          (w->MaxSectsW)
#define SectModified      (w->SectModifiedW)
#define ReqTypeFace       (w->ReqTypeFaceW)
#define ReqTwipsSize      (w->ReqTwipsSizeW)
#define CharScrFontId     (w->CharScrFontIdW)
#define CharPrtFontId     (w->CharPrtFontIdW)
#define CharScrWidth      (w->CharScrWidthW)
#define CharPrtWidth      (w->CharPrtWidthW)

/******************************************************************************
     Print control variables
*******************************************************************************/
#define PageWidth         (w->PageWidthW)
#define PageHeight        (w->PageHeightW)
#define PvWidth           (w->PvWidthW)
#define PvHeight          (w->PvHeightW)
#define PageX             (w->PageXW)
#define PortraitHX        (w->PortraitHXW)
#define PortraitHY        (w->PortraitHYW)
#define LandscapeHX       (w->LandscapeHXW)
#define LandscapeHY       (w->LandscapeHYW)
#define hPrtDataDC        (w->hPrtDataDCW)
#define AbortPrint        (w->AbortPrintW)
#define InPrinting        (w->InPrintingW)
#define InPrintPreview    (w->InPrintPreviewW)
#define ExtPrintPreview   (w->ExtPrintPreviewW)
#define GoPrintPreview    (w->GoPrintPreviewW)
#define PrinterAvailable  (w->PrinterAvailableW)
#define PrtDiffRes        (w->PrtDiffResW)
#define PaperSize         (w->PaperSizeW)
#define PaperOrient       (w->PaperOrientW)
#define PrinterName       (w->PrinterNameW)
#define PrinterDriver     (w->PrinterDriverW)
#define PrinterPort       (w->PrinterPortW)
#define hDeviceMode       (w->hDeviceModeW)
#define pDeviceMode       (w->pDeviceModeW)
#define hAbortWnd         (w->hAbortWndW)
#define TotalPreviewPages (w->TotalPreviewPagesW)
#define PreviewZoom       (w->PreviewZoomW)
#define PreviewHeight     (w->PreviewHeightW)
#define PreviewWidth      (w->PreviewWidthW)
#define SavePvStatus      (w->SavePvStatusW)
#define SavePvRuler       (w->SavePvRulerW)
#define SavePvToolbar     (w->SavePvToolbarW)
#define SavePvScrResX     (w->SavePvScrResXW)
#define SavePvScrResY     (w->SavePvScrResYW)
#define SavePvToolBarHeight (w->SavePvToolBarHeightW)
#define SavePvWordWrap    (w->SavePvWordWrapW)
#define SavePvPrintView   (w->SavePvPrintViewW)
#define SavePvPageMode    (w->SavePvPageModeW)
#define SavePvPageBorder  (w->SavePvPageBorderW)
#define SavePvFittedView  (w->SavePvFittedViewW)
#define SavePvShowHorBar  (w->SavePvShowHorBarW)
#define SavePvOrgX        (w->SavePvOrgXW)
#define SavePvOrgY        (w->SavePvOrgYW)
#define SavePvViewPageHdrFtr (w->SavePvViewPageHdrFtrW)
#define SavePvFirstFramePage (w->SavePvFirstFramePageW)
#define SavePvLastFramePage  (w->SavePvLastFramePageW)
#define ShowPvToolbar     (w->ShowPvToolbarW)
#define PvViewport        (w->PvViewportW)
#define ZoomSubclassProc  (w->ZoomSubclassProcW)
#define OrigZoomSubclassProc (w->OrigZoomSubclassProcW)
#define SavePrtFontHeight (w->SavePrtFontHeightW)
#define SavePrtPitchFamily (w->SavePrtPitchFamilyW)
#define SavePrtIsTrueType (w->SavePrtIsTrueTypeW)
#define PrtTextBkColor    (w->PrtTextBkColorW)
#define PrtParaBkColor    (w->PrtParaBkColorW)
#define PrtFrameBkColor   (w->PrtFrameBkColorW)
#define MPRect            (w->MPRectW)
#define OverrideBinCopy   (w->OverrideBinCopyW)
#define OverrideBin       (w->OverrideBinW)

/******************************************************************************
     Cursor adjustment variables
******************************************************************************/
#define PrevCursLine    (w->PrevCursLineW)
#define PrevCursCol     (w->PrevCursColW)
#define PrevCursPage    (w->PrevCursPageW)
#define PrevCursLineY   (w->PrevCursLineYW)
#define CursDirection   (w->CursDirectionW)

/******************************************************************************
     Color control variables
******************************************************************************/
#define TextDefBkColor    (w->TextDefBkColorW)
#define TextDefColor      (w->TextDefColorW)
#define TextBorderColor   (w->TextBorderColorW)
#define ParaBoxColor      (w->ParaBoxColorW)
#define StatusBkColor     (w->StatusBkColorW)
#define StatusColor       (w->StatusColorW)
#define PageBorderColor   (w->PageBorderColorW)
#define PageBkColor       (w->PageBkColorW)
#define CurForeColor      (w->CurForeColorW)
#define CurBackColor      (w->CurBackColorW)
#define FrameShading      (w->FrameShadingW)
#define MaxRtfColors      (w->MaxRtfColorsW)
#define hToolbarBrush     (w->hToolbarBrushW)
#define hTextBrush        (w->hTextBrushW)
#define hHilightBrush     (w->hHilightBrushW)
#define hStatusBrush      (w->hStatusBrushW)

/******************************************************************************
     Block highlighting variables
******************************************************************************/
#define HilightType       (w->HilightTypeW)
#define HilightWithColCursor (w->HilightWithColCursorW)
#define HilightBegRow     (w->HilightBegRowW)
#define HilightEndRow     (w->HilightEndRowW)
#define HilightBegCol     (w->HilightBegColW)
#define HilightEndCol     (w->HilightEndColW)
#define HilightAtCurPos   (w->HilightAtCurPosW)
#define MaxColBlock       (w->MaxColBlockW)
#define StretchHilight    (w->StretchHilightW)
#define HilightTimerOn    (w->HilightTimerOnW)
#define FullCellHilight   (w->FullCellHilightW)
#define DblClickEndRow     (w->DblClickEndRowW)
#define DblClickEndCol     (w->DblClickEndColW)
#define DblClickHilight   (w->DblClickHilightW)

/******************************************************************************
     Tool Bar Variables
*******************************************************************************/
#define ToolBarHeight     (w->ToolBarHeightW)
#define hToolBarWnd       (w->hToolBarWndW)
#define hPvToolBarWnd     (w->hPvToolBarWndW)
#define lpToolBar         (w->lpToolBarW)
#define ToolBarCfmt       (w->ToolBarCfmtW)
#define ToolBarPfmt       (w->ToolBarPfmtW)
#define ToolBarSID        (w->ToolBarSIDW)
#define ToolBarComboProc  (w->ToolBarComboProcW)
#define OrigBarFontProc   (w->OrigBarFontProcW)
#define OrigBarStyleProc  (w->OrigBarStyleProcW)
#define OrigBarPointProc  (w->OrigBarPointProcW)
#define OrigBarZoomProc   (w->OrigBarZoomProcW)

/******************************************************************************
     Search/Replace Variables
*******************************************************************************/
#define SearchString      (w->SearchStringW)
#define SearchDirection   (w->SearchDirectionW)
#define ReplaceString     (w->ReplaceStringW)
#define ReplaceWith       (w->ReplaceWithW)
#define SearchFlags       (w->SearchFlagsW)
#define ReplaceVerify     (w->ReplaceVerifyW)
#define ReplaceBlock      (w->ReplaceBlockW)

/******************************************************************************
     Undo variables
*******************************************************************************/
#define undo              (w->undoW)
#define UndoCount         (w->UndoCountW)
#define MaxUndos          (w->MaxUndosW)
#define MaxUndoLimit      (w->MaxUndoLimitW)
#define UndoTblSize       (w->UndoTblSizeW)
#define UndoRef           (w->UndoRefW)
#define UndoSkipRef       (w->UndoSkipRefW)
#define InUndo            (w->InUndoW)
#define CurUndoType       (w->CurUndoTypeW)
#define UndoParaFrame     (w->UndoParaFrameW)
#define UndoInt1          (w->UndoInt1W)
#define UndoInt2          (w->UndoInt2W)

/******************************************************************************
     Hidden/footnote text variables
*******************************************************************************/
#define ShowHiddenText    (w->ShowHiddenTextW)
#define ShowFieldNames    (w->ShowFieldNamesW)
#define EditFootnoteText  (w->EditFootnoteTextW)
#define EditEndnoteText   (w->EditEndnoteTextW)
#define EndnoteAtSect     (w->EndnoteAtSectW)
#define InFootnote        (w->InFootnoteW)
#define FootnoteRest      (w->FootnoteRestW)
#define FootnoteRestFont  (w->FootnoteRestFontW)
#define FootnoteNumFmt    (w->FootnoteNumFmtW)
#define EndnoteNumFmt     (w->EndnoteNumFmtW)

/******************************************************************************
     Paragraph Border and spacing variables
*******************************************************************************/
#define BoxLeft        (w->BoxLeftW)
#define BoxRight       (w->BoxRightW)
#define DlgOnFlags     (w->DlgOnFlagsW)
#define DlgOffFlags    (w->DlgOffFlagsW)
#define DlgLong        (w->DlgLongW)
#define DlgLong1       (w->DlgLong1W)
#define DlgLong2       (w->DlgLong2W)
#define DlgLong3       (w->DlgLong3W)
#define DlgColor1      (w->DlgColor1W)
#define DlgColor2      (w->DlgColor2W)
#define DlgColor3      (w->DlgColor3W)
#define DlgColor4      (w->DlgColor4W)
#define DlgText1       (w->DlgText1W)
#define DlgText2       (w->DlgText2W)
#define DlgInt1        (w->DlgInt1W)
#define DlgInt2        (w->DlgInt2W)
#define DlgInt3        (w->DlgInt3W)
#define DlgInt4        (w->DlgInt4W)
#define DlgInt5        (w->DlgInt5W)
#define DlgInt6        (w->DlgInt6W)
#define DlgPtr1        (w->DlgPtr1W)
#define DlgPtr2        (w->DlgPtr2W)
#define DlgCancel      (w->DlgCancelW)
#define SpaceBef       (w->SpaceBefW)
#define SpaceAft       (w->SpaceAftW)
#define ParaSpcBef     (w->ParaSpcBefW)
#define ParaSpcAft     (w->ParaSpcAftW)

/******************************************************************************
     Paragraph Frame variables
*******************************************************************************/
#define ParaFrame       (w->ParaFrameW)
#define hParaFrame      (w->hParaFrameW)
#define CurParaFrame    (w->CurParaFrameW)
#define TotalParaFrames (w->TotalParaFramesW)
#define MaxParaFrames   (w->MaxParaFramesW)
#define CurWrapParaFID  (w->CurWrapParaFIDW)
#define WmParaFID       (w->WmParaFIDW)
#define ContainsParaFrames (w->ContainsParaFramesW)

/******************************************************************************
     OLE Variables
*******************************************************************************/
#define TerClient      (w->TerClientW)
#define TerClientVtbl  (w->TerClientVtblW)
#define OleStream      (w->OleStreamW)
#define OleStreamVtbl  (w->OleStreamVtblW)
#define lhDoc          (w->lhDocW)
#define WaitForOle     (w->WaitForOleW)
#define OleReleaseMethod (w->OleReleaseMethodW)
#define hOleData       (w->hOleDataW)
#define pOleData       (w->pOleDataW)
#define CurObject      (w->CurObjectW)
#define ActiveOleObj   (w->ActiveOleObjW)
#define pDocStorage    (w->pDocStorageW)

/******************************************************************************
     VBX Variables
*******************************************************************************/
#define VbxControl          (w->VbxControlW)
#define VbxMessageProcessed (w->VbxMessageProcessedW)
#define VbxCallback         (w->VbxCallbackW)
#define MsgCallback         (w->MsgCallbackW)
#define ParentPtr           (w->ParentPtrW)

/******************************************************************************
     Drag object Variables
*******************************************************************************/
#define DragObj             (w->DragObjW)
#define hDragObj            (w->hDragObjW)
#define CurDragObj          (w->CurDragObjW)
#define CurHotSpot          (w->CurHotSpotW)
#define TotalDragObjs       (w->TotalDragObjsW)
#define MaxDragObjs         (w->MaxDragObjsW)
#define PictureHilighted    (w->PictureHilightedW)
#define FrameTabsHilighted  (w->FrameTabsHilightedW)
#define FrameRectHilighted  (w->FrameRectHilightedW)
#define PictureClicked      (w->PictureClickedW)
#define FrameClicked        (w->FrameClickedW)

/******************************************************************************
     Table Variables
*******************************************************************************/
#define TableRow             (w->TableRowW)
#define TableAux             (w->TableAuxW)
#define cell                 (w->cellW)
#define CellAux               (w->CellAuxW)
#define hTableRow            (w->hTableRowW)
#define hCell                (w->hCellW)
#define TotalTableRows       (w->TotalTableRowsW)
#define TotalCells           (w->TotalCellsW)
#define MaxTableRows         (w->MaxTableRowsW)
#define MaxCells             (w->MaxCellsW)
#define FirstFreeCellId      (w->FirstFreeCellIdW)
#define CursorCell           (w->CursorCellW)
#define TableRows            (w->TableRowsW)
#define TableCols            (w->TableColsW)
#define CellChar             (w->CellCharW)
#define OldCellChar          (w->OldCellCharW)
#define FrameCellId          (w->FrameCellIdW)
#define FrameRowId           (w->FrameRowIdW)
#define FrameHeight          (w->FrameHeightW)
#define TsFrameY             (w->TsFrameYW)
#define ShowTableGridLines   (w->ShowTableGridLinesW)
#define HasNestedTables      (w->HasNestedTablesW)
#define CurWrapCell          (w->CurWrapCellW)
#define DefCellMargin        (w->DefCellMarginW)

/******************************************************************************
     RTF Control Variables
******************************************************************************/
#define RtfCurRowId          (w->RtfCurRowIdW)
#define RtfCurCellId         (w->RtfCurCellIdW)
#define RtfInitCellId        (w->RtfInitCellIdW)
#define RtfParaFID           (w->RtfParaFIDW)
#define RtfInTable           (w->RtfInTableW)
#define RtfPrevLevel         (w->RtfPrevLevelW)
#define RtfCurLevel          (w->RtfCurLevelW)
#define RtfInitLevel         (w->RtfInitLevelW)
#define RtfParaFrameInfo     (w->RtfParaFrameInfoW)
#define PrevRtfParaFrameInfo (w->PrevRtfParaFrameInfoW)
#define RtfLastCellX         (w->RtfLastCellXW)
#define RtfInEquation        (w->RtfInEquationW)
#define InRtfRead            (w->InRtfReadW)
#define RtfInHdrFtr          (w->RtfInHdrFtrW)

/******************************************************************************
     Page Header/Footer Variables
******************************************************************************/
#define ViewPageHdrFtr       (w->ViewPageHdrFtrW)
#define EditPageHdrFtr       (w->EditPageHdrFtrW)
#define PosPageHdrFtr        (w->PosPageHdrFtrW)
#define HdrFirstPageLine     (w->HdrFirstPageLineW)
#define HdrLastPageLine      (w->HdrLastPageLineW)
#define FtrFirstPageLine     (w->FtrFirstPageLineW)
#define FtrLastPageLine      (w->FtrLastPageLineW)

/******************************************************************************
     Page Box variables
******************************************************************************/
#define PageBoxShowing       (w->PageBoxShowingW)
#define BoxPage              (w->BoxPageW)
#define PageBoxRect          (w->PageBoxRectW)

/******************************************************************************
     Caret Variables
*******************************************************************************/
#define CaretHeight      (w->CaretHeightW)
#define CaretEnabled     (w->CaretEnabledW)
#define CaretHidden      (w->CaretHiddenW)
#define CaretPositioned  (w->CaretPositionedW)
#define CaretEngaged     (w->CaretEngagedW)
#define CaretPos         (w->CaretPosW)
#define RepaintRuler     (w->RepaintRulerW)
#define CaretPage        (w->CaretPageW)
#define ShowProtectCaret (w->ShowProtectCaretW)
#define CaretVert        (w->CaretVertW)

/******************************************************************************
     Stylesheet Variables
*******************************************************************************/
#define StyleId          (w->StyleIdW)
#define PrevStyleId      (w->PrevStyleIdW)
#define hStyleId         (w->hStyleIdW)
#define EditingParaStyle (w->EditingParaStyleW)
#define CurSID           (w->CurSIDW)
#define MaxSID           (w->MaxSIDW)
#define TotalSID         (w->TotalSIDW)

/******************************************************************************
     Ime Variables
*******************************************************************************/
#define ImeStartPos      (w->ImeStartPosW)
#define ImeByteLen       (w->ImeByteLenW)
#define ImeCharLen       (w->ImeCharLenW)
#define ImeResultBytes   (w->ImeResultBytesW)
#define ImeResultChars   (w->ImeResultCharsW)
#define ImeEnabled       (w->ImeEnabledW)
#define InlineIme        (w->InlineImeW)
#define ImeDiscards      (w->ImeDiscardsW)

/******************************************************************************
     TOC Variables
*******************************************************************************/
#define DocHasToc        (w->DocHasTocW)
#define DocHasHeadings   (w->DocHasHeadingsW)
#define MultipleToc      (w->MultipleTocW)
#define FirstTocPos      (w->FirstTocPosW)
#define TocShowPageNo    (w->TocShowPageNoW)
#define TocTabLeader     (w->TocTabLeaderW)
#define TocTabAlign      (w->TocTabAlignW)

/******************************************************************************
     Image Map Variables
*******************************************************************************/
#define ImageMap        (w->ImageMapW)
#define TotalImageMaps  (w->TotalImageMapsW)
#define CurMapPict      (w->CurMapPictW)
#define CurMapId        (w->CurMapIdW)
#define CurMapRect      (w->CurMapRectW)


/******************************************************************************
     Spell checking variables
*******************************************************************************/
#define StCharSet        (w->StCharSetW)
#define StToUniChar      (w->StToUniCharW)
#define pStFromUniChar   (w->pStFromUniCharW)
#define StFirstUniChar   (w->StFirstUniCharW)
#define StLastUniChar    (w->StLastUniCharW)

/******************************************************************************
     Other Variables
*******************************************************************************/
#define BreakChars        (w->BreakCharsW)
#define ReqCharSet        (w->ReqCharSetW)
#define ReqLang           (w->ReqLangW)
#define CurHKL            (w->CurHKLW)
#define DocName           (w->DocNameW)
#define FileInfo          (w->FileInfoW)
#define InsertMode        (w->InsertModeW)
#define TerHelpWanted     (w->TerHelpWantedW)
#define WindowBeingCreated (w->WindowBeingCreatedW)
#define CrLfUsed          (w->CrLfUsedW)
#define ProtectionLock    (w->ProtectionLockW)
#define ShowParaMark      (w->ShowParaMarkW)
#define ShowPageBorder    (w->ShowPageBorderW)
#define BatchMode         (w->BatchModeW)
#define RepageTimerOn     (w->RepageTimerOnW)
#define NoTabIndent       (w->NoTabIndentW)
#define restrict          (w->restrictW)
#define InIE              (w->InIEW)
#define WmWashed          (w->WmWashedW)

#define TerHelpFile       (w->TerHelpFileW)

#define RtfClipFormat        (w->RtfClipFormatW)
#define SSClipInfo           (w->SSClipInfoW)
#define CfEnhMetafile        (w->CfEnhMetafileW)
#define NativeClipFormat     (w->NativeClipFormatW)
#define ObjectLinkClipFormat (w->ObjectLinkClipFormatW)
#define OwnerLinkClipFormat  (w->OwnerLinkClipFormatW)

#define FileFormat        (w->FileFormatW)
#define RtfInput          (w->RtfInputW)
#define ZoomPercent       (w->ZoomPercentW)

#define hInternet         (w->hInternetW)

#define TotalPages        (w->TotalPagesW)
#define OverrideTotalPages (w->OverrideTotalPagesW)
#define PrevTotalPages    (w->PrevTotalPagesW)
#define MaxPages          (w->MaxPagesW)
#define PageModifyCount   (w->PageModifyCountW)
#define CurPage           (w->CurPageW)
#define PageInfo          (w->PageInfoW)
#define RepageBeginLine   (w->RepageBeginLineW)
#define RepagePending     (w->RepagePendingW)
#define repaginating      (w->repaginatingW)
#define LastPageCreated   (w->LastPageCreatedW)
#define SpeedKeyEnabled   (w->SpeedKeyEnabledW)
#define ShowHyperlinkCursor (w->ShowHyperlinkCursorW)
#define MatchIds          (w->MatchIdsW)
#define InDialogBox       (w->InDialogBoxW)
#define SnapToGrid        (w->SnapToGridW)
#define TblSelCursShowing (w->TblSelCursShowingW)
#define LinkCursShowing   (w->LinkCursShowingW)
#define ProtectForm       (w->ProtectFormW)
#define hAppMemory        (w->hAppMemoryW)
#define pAppMemory        (w->pAppMemoryW)
#define ModifyProtectColor (w->ModifyProtectColorW)
#define LinkStyle         (w->LinkStyleW)
#define LinkColor         (w->LinkColorW)
#define LinkDblClick      (w->LinkDblClickW)
#define ExtraSpacePrtX    (w->ExtraSpacePrtXW)
#define ExtraSpaceScrX    (w->ExtraSpaceScrXW)
#define CharBoxLeft       (w->CharBoxLeftW)
#define CharBoxRight      (w->CharBoxRightW)
#define bullet            (w->bulletW)
#define ExpandLineFmt     (w->ExpandLineFmtW)
#define CurCtlId          (w->CurCtlIdW)
#define hCurCtlWnd        (w->hCurCtlWndW)
#define MessageDisplayed  (w->MessageDisplayedW)
#define BkPictId          (w->BkPictIdW)
#define BkPictFlag        (w->BkPictFlagW)
#define TerFlags          (w->TerFlagsW)
#define TerFlags2         (w->TerFlags2W)
#define TerFlags3         (w->TerFlags3W)
#define TerFlags4         (w->TerFlags4W)
#define TerFlags5         (w->TerFlags5W)
#define TerFlags6         (w->TerFlags6W)
#define TerOpFlags        (w->TerOpFlagsW)
#define TerOpFlags2       (w->TerOpFlags2W)
#define hMdiClient        (w->hMdiClientW)
#define hMdiChild         (w->hMdiChildW)
#define hUserAccTable     (w->hUserAccTableW)
#define RtfInfo           (w->RtfInfoW)
#define pRtfInfo          (w->pRtfInfoW)
#define ToolbarFlagText   (w->ToolbarFlagTextW)
#define EvalTimerCount    (w->EvalTimerCountW)
#define FormFieldSubclassProc  (w->FormFieldSubclassProcW)
#define UserDir           (w->UserDirW)
#define UserFileType      (w->UserFileTypeW)
#define DefLang           (w->DefLangW)
#define hPopup            (w->hPopupW)
#define SplResult         (w->SplResultW)
#define ParamRect         (w->ParamRectW)
#define hMsgHook          (w->hMsgHookW)

#define EventMask         (w->EventMaskW)
#define hGetTextMem       (w->hGetTextMemW)
#define GetTextLen        (w->GetTextLenW)
#define GetTextMod        (w->GetTextModW)

// tracking variables
#define TrackChanges      (w->TrackChangesW)
#define TrackRev          (w->TrackRevW)
#define TrackTime         (w->TrackTimeW)

// section variables
#define KnownSect          (w->KnownSectW)
#define KnownSectBegLine   (w->KnownSectBegLineW)
#define KnownSectEndLine   (w->KnownSectEndLineW)
#define SectLine           (w->SectLineW)

// toolbar variables
#define TlbItemCount       (w->TlbItemCountW)
#define TlbId              (w->TlbIdW)
#define TlbCustId          (w->TlbCustIdW)
#define TotalTlbCustIds    (w->TotalTlbCustIdsW)
#define TlbIdClicked       (w->TlbIdClickedW)
#define pBlnTlb            (w->pBlnTlbW)
#define BlnRect            (w->BlnRectW)
#define hBlnBM             (w->hBlnBMW)

// mousestop bubble variables
#define reviewer           (w->reviewerW)
#define MaxReviewers       (w->MaxReviewersW)
#define TotalReviewers     (w->TotalReviewersW)

// mousestop bubble variables
#define MouseStopX         (w->MouseStopXW)
#define MouseStopY         (w->MouseStopYW)
#define MouseStopLine      (w->MouseStopLineW)
#define MouseStopCol       (w->MouseStopColW)
#define MouseStopTimerOn   (w->MouseStopTimerOnW)
#define MouseStopMsgOn     (w->MouseStopMsgOnW)
#define MouseStopDone      (w->MouseStopDoneW)

// LZW variables
#define LzClearCode       (lz->LzClearCodeL)
#define LzLastChar        (lz->LzLastCharL)
#define LzEofCode         (lz->LzEofCodeL)
#define LzFirstCode       (lz->LzFirstCodeL)
#define LzBeginInBits     (lz->LzBeginInBitsL)
#define LzMaxInBits       (lz->LzMaxInBitsL)
#define LzMaxInBufSize    (lz->LzMaxInBufSizeL)
#define LzUserData        (lz->LzUserDataL)
#define LzOutputCallback  (lz->LzOutputCallbackL)
#define LzOutValue        (lz->LzOutValueL)
#define LzCurInBits       (lz->LzCurInBitsL)
#define LzMapSize         (lz->LzMapSizeL)
#define LzMap             (lz->LzMapL)
#define LzChar            (lz->LzCharL)
#define LzCharCount       (lz->LzCharCountL)
#define LzInBuf           (lz->LzInBufL)
#define LzBitsRead        (lz->LzBitsReadL)
#define LzCurBufSize      (lz->LzCurBufSizeL)
#define LzNextCode        (lz->LzNextCodeL)
#define LzPrevCode        (lz->LzPrevCodeL)
#define LzPrevChar        (lz->LzPrevCharL)
#define LzEof             (lz->LzEofL)
#define LzCleared         (lz->LzClearedL)
#define LzImageType       (lz->LzImageTypeL)
#define LzBitsStuffed     (lz->LzBitsStuffedL)

// StrFrameSet structure
#define Py                (pPrt->y)
#define PCurColHeight     (pPrt->CurColHeight)
#define PMaxColHeight     (pPrt->MaxColHeight)
#define PFrameHt          (pPrt->FrameHt)
#define Sy                (pScr->y)
#define SCurColHeight     (pScr->CurColHeight)
#define SMaxColHeight     (pScr->MaxColHeight)
#define SFrameHt          (pScr->FrameHt)

#define PColumnX          (pPrt->ColumnX)
#define PColumnWidth      (pPrt->ColumnWidth)
#define PColumnSpace      (pPrt->ColumnSpace)
#define PCellX            (pPrt->CellX)
#define PTopSect          (pPrt->TopSect)
#define Psect             (pPrt->sect)
#define PTopLeftMargin    (pPrt->TopLeftMargin)
#define PHiddenY          (pPrt->HiddenY)
#define PHdrMargin        (pPrt->HdrMargin)
#define PHdrHeight        (pPrt->HdrHeight)
#define PFtrMargin        (pPrt->FtrMargin)
#define PFtrHeight        (pPrt->FtrHeight)

