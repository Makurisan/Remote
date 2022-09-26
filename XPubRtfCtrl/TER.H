#ifndef TERH
#define TERH

/******************************************************************************
               TER Constants
*******************************************************************************/
#if defined (_WIN32)
   #if !defined(WIN32)
     #define WIN32
   #endif
#endif
   
#define TER_CLASS      "Ter13Class"

#define TER_DATA_VER       101

#define CROP_LEFT           1
#define CROP_RIGHT          2
#define CROP_TOP            3
#define CROP_BOT            4

#define PERM_PRINT            0x4     // these values are specified by the pdf spec - permission flags
#define PERM_MOD              0x28
#define PERM_COPY             0x10

#define PERM_ALL              (PERM_PRINT|PERM_MOD|PERM_COPY)

#define PDFFLAG_COMPRESS_TEXT  0x1    // the flag used by the TerPdcStart function

#define MAX_WIDTH          1000
#define TE_MAX_TAB_STOPS   20
#define CELL_MARGIN        60      // cell margin in twips

#define OLEPTR_OBJECT      0
#define OLEPTR_STORAGE     1

#define HILIGHT_OFF        0
#define HILIGHT_LINE       1
#define HILIGHT_CHAR       2

#define FLOW_DEF           0
#define FLOW_LTR           1
#define FLOW_RTL           2

#define FONTINFO_UCBASE      1
#define FONTINFO_CHARSET     2
#define FONTINFO_PICT_WIDTH  3
#define FONTINFO_PICT_HEIGHT 4
#define FONTINFO_ULINE_COLOR 5
#define FONTINFO_AUX_ID      6
#define FONTINFO_FLAGS       7
#define FONTINFO_FRAME_TYPE  8
#define FONTINFO_FRAME_ID    9
#define FONTINFO_OFFSET      10
#define FONTINFO_IS_PICT     11
#define FONTINFO_IS_OLE      12
#define FONTINFO_IS_OCX      13

#define PARAINFO_TEXT_FLOW    1
#define PARAINFO_BORDER_COLOR 2
#define PARAINFO_BK_COLOR     3   // background color

#define PARAINFO_ERROR     -999999

#define SWRAP_NO_WRAP      1
#define SWRAP_AROUND       2
#define SWRAP_IGNORE       3
#define SWRAP_TIGHT        4
#define SWRAP_THRU         5

#define SSINFO_NAME        1
#define SSINFO_CHAR_SPACE  2
#define SSINFO_CHAR_OFFSET 3
#define SSINFO_NEXT        4

#define INFO_TITLE         0
#define INFO_SUBJECT       1
#define INFO_AUTHOR        2
#define INFO_MANAGER       3
#define INFO_COMPANY       4
#define INFO_OPERATOR      5
#define INFO_CATEGORY      6
#define INFO_KEYWORDS      7
#define INFO_COMMENT       8
#define INFO_DOCCOMM       9
#define INFO_MAX          10

#define TEXT_HORZ          0
#define TEXT_TOP_TO_BOT    1
#define TEXT_BOT_TO_TOP    2

#define TP_TOTAL_LINES     1
#define TP_TOTAL_PFMTS     2
#define TP_TOTAL_FONTS     3
#define TP_TOTAL_STYLES    4
#define TP_TOTAL_PAGES     5
#define TP_TOTAL_PARA_FRAMES 6
#define TP_TOTAL_CHAR_TAGS 7
#define TP_TOTAL_IMAGE_MAPS 8
#define TP_TOTAL_BLTS      9
#define TP_TOTAL_LISTS     10
#define TP_TOTAL_LIST_OR   11
#define TP_TOTAL_TABS      12
#define TP_TOTAL_TABLE_ROWS 13
#define TP_TOTAL_CELLS     14
#define TP_TOTAL_SECTS     15
#define TP_CUR_LINE        16
#define TP_CUR_COL         17
#define TP_CUR_SECT        18
#define TP_SELECTION_TYPE  19
#define TP_SELECTION_START_LINE  20
#define TP_SELECTION_START_COL   21
#define TP_SELECTION_END_LINE    22
#define TP_SELECTION_END_COL     23
#define TP_WATERMARK_WASH        24
#define TP_WATERMARK_PICT        25
#define TP_PAGE_BK_COLOR         26

#define CP_FIX_WIDTH       0        // fix cell width
#define CP_PARENT_CELL     1        // parent cell id
#define CP_LEVEL           2        // nesting level
#define CP_TEXT_ROTATION   3        // text rotation constants

#define FP_TEXT_ROTATION   1        // text rotation constant
#define FP_WRAP_STYLE      2        // text wrap style
#define FP_YBASE           3        // BASE_ constants
#define FP_FILL_PATTERN    4        // fill pattern
#define FP_TEXT_DIST       5        // text distance from frame

#define FP_ERROR        9999        // error return by the TerGetFrameParam function

#define SP_FLAGS           1        // text rotation constant


#define LP_LINE_FLAGS      1        // line flags LFLAG_
#define LP_LINE_FLAGS2     2        // line flags LFLAG2_

#define LP_ERROR        9999        // error return by the TerGetLineParam function

#define WRAP_OFF           0        // suspend wrapping temporarily
#define WRAP_MIN           1        // re-wrap minimum number of lines
#define WRAP_PARA          2        // re-wrap all lines in the current paragraph
#define WRAP_WIN           3        // re-wrap all lines visible in the current window
#define WRAP_PAGE          4        // wrap a page

#define APPLY_IGNORE       0        // ignore text data
#define APPLY_MOD_CUR_LINE 1        // apply the text data to the current line
#define APPLY_BEF_CUR_LINE 2        // insert after the current line
#define APPLY_AFT_CUR_LINE 3        // insert after the current line

#define SAVE_TER           0        // save in TER format
#define SAVE_TEXT          1        // save in text format
#define SAVE_TEXT_LINES    2        // text format with line breaks
#define SAVE_RTF           3        // save in Rich Text Format
#define SAVE_DEFAULT       4        // save in the format of the input file
#define SAVE_HTML          5        // save in html format, needs HTML add-on to work
#define SAVE_UTEXT         6        // save in the unicode text format


#define ULINE              0x1
#define BOLD               0x2
#define ITALIC             0x4
#define STRIKE             0x8
#define SUPSCR             0x10
#define SUBSCR             0x20
#define HIDDEN             0x40
#define PICT               0x80
#define ULINED             0x100
#define PROTECT            0x200
#define FNOTE              0x400
#define FNOTETEXT          0x800
#define FNOTEREST          0x1000
#define CHAR_BOX           0x2000
#define HLINK              0x4000
#define ENOTETEXT          0x8000   // endnote text

#if defined(WIN32)                  // extended character styles for win32
  #define CAPS            0x10000
  #define SCAPS           0x20000
#else                               // extended character styles not supported in 16 bit
  #define CAPS            0x0
  #define SCAPS           0x0
#endif

#define SID_NORMAL         -9999     // normal style id (index 0)
#define SID_CUR            -9998     // current style being created

#define FNOTEALL           (FNOTE|FNOTETEXT|FNOTEREST|ENOTETEXT)
#define FNOTENONE          (~((UINT)(FNOTEALL)))

#define DEFAULT_PFMT       0x0

#define CENTER             0x1
#define RIGHT_JUSTIFY      0x2
#define DOUBLE_SPACE       0x4
#define BULLET             0x8
#define PARA_BOX_TOP       0x10
#define PARA_BOX_BOT       0x20
#define PARA_BOX_LEFT      0x40
#define PARA_BOX_RIGHT     0x80
#define PARA_BOX_DOUBLE    0x100
#define PARA_BOX_THICK     0x200
#define PARA_BOX_ATTRIB    0x3F0
#define LEFT               0x400
#define JUSTIFY            0x800
#define PAGE_HDR           0x1000
#define PAGE_FTR           0x2000
#define PAGE_HDR_FTR       0x3000
#define PARA_KEEP          0x4000
#define PARA_KEEP_NEXT     0x8000
#define PARA_BOX_BETWEEN   0x10000


#define ROWFLAG_HDR        0x4     // header row, 0x1, 0x2, 0x400, 0x800 taken by CENTER,RIGHT_JUSTIFY,LEFT and JUSTIFY
#define ROWFLAG_HAS_SUBTABLE 0x8   // this row contains subtables 
#define ROWFLAG_SPLIT      0x10    // this row is split at page break
#define ROWFLAG_KEEP       0x2000  // keep this row in one page
#define ROWFLAG_PREEXIST   0x4000  // this row existed before an RTF read began
#define ROWFLAG_NEW_TABLE  0x8000  // row begins a new table
#define ROWFLAG_RTL        0x10000  // rtl row
#define ROWFLAG_BREAK      0x20000 // break the table at this row for calculating the cell widths

#define PARA_BOX           (0xF0|PARA_BOX_BETWEEN)

#define PFLAG_FRAME        0x1     // used only during RTF read
#define PFLAG_CELL         0x2     // used only during RTF read
#define PFLAG_RESERVED1    0x4     // for internal use
#define PFLAG_RESERVED2    0x4     // for internal use

#define PFLAG_NO_WRAP      0x10     // turn-off word wrapping
#define PFLAG_WIDOW        0x20     // Window orphan control
#define PFLAG_PAGE_BREAK   0x40     // page break before the paragraph
#define PFLAG_ENDNOTE      0x80     // endnote paragraph

#define FFLAG_ITALIC_OH     0x1
#define FFLAG_OCX_SRV_ERROR 0x2
#define FFLAG_CTL_VISIBLE   0x4
#define FFLAG_CTL_DRAWN     0x8
#define FFLAG_DRAW_PENDING  0x10    // drawing of this item pending
#define FFLAG_SEL           0x20    // temporary selection flag
#define FFLAG_PICT_DRAWN    0x40    // picture visible on the screen
#define FFLAG_HIDDEN_INFO   0x80    // hidden information is active for a hidden font
#define FFLAG_USER          0x100   // font created using TerCreateFont, not subject to reclaiming of resources
#define FFLAG_SCAPS         0x200   // size reduced for scaps adjustment
#define FFLAG_AUTO_SPELL    0x400
#define FFLAG_EQ_OBJECT     0x800   // denotes an equation object
#define FFLAG_RESIZED       0x1000  // ole object resized by the user
#define FFLAG_CHECKED       0x2000  // temporary font id 'checked' flag used by locate function

#define TAB_LEFT            0
#define TAB_RIGHT           1
#define TAB_CENTER          2
#define TAB_DECIMAL         3

#define TAB_NONE            0         // only one value applicable
#define TAB_DOT             1
#define TAB_HYPH            2
#define TAB_ULINE           4

#define CLR_WHITE          0xFFFFFF  // white color
#define CLR_WHITE_FORCE    0xFEFEFE  // white color to be enforced
#define CLR_BLACK          0x0       // black color
#define CLR_AUTO           0x010101  // auto text color

#define TER_WORD_WRAP       0x1        // TER custom control styles
#define TER_PRINT_VIEW      0x2
#define TER_HSCROLL         0x4
#define TER_VSCROLL         0x8
#define TER_SHOW_STATUS     0x10
#define TER_SHOW_RULER      0x20
#define TER_BORDER_MARGIN   0x40
#define TER_OUTPUT_RTF      0x80
#define TER_USE_PARAMS      0x100
#define TER_READ_ONLY       0x200
#define TER_PAGE_MODE       0x400
#define TER_SHOW_TOOLBAR    0x800
#define TER_INVISIBLE       0x1000
#define TER_FITTED_VIEW     0x2000
#define TER_DESIGN_MODE     0x4000
#define TER_NUM_STYLES      15

#define SRCH_SCROLL         0x1
#define SRCH_SEARCH         0x2
#define SRCH_REPLACE        0x4
#define SRCH_RETRIEVE       0x8
#define SRCH_CASE           0x10
#define SRCH_WORD           0x20
#define SRCH_SKIP_HIDDEN_TEXT 0x40
#define SRCH_BACK           0x80
#define SRCH_NO_REPLACE_PROT_TEXT 0x100

#define ALIGN_BOT            0        // align picture bottom to base line
#define ALIGN_MIDDLE         1        // center picture vertically to baseline
#define ALIGN_TOP            2        // align picture top to base line

#define PFRAME_NONE          0        // not a picture frame
#define PFRAME_LEFT          1        // left alignment picture frame
#define PFRAME_RIGHT         2        // right alignment picture frame
#define PFRAME_FLOAT         3        // not left or right aligned frame

#define POS_BEG              0        // position at the beginning of text
#define POS_END              1        // position at the end of text

#define DOB_TEXT_BOX         1        // text box
#define DOB_RECT             2        // rectangle
#define DOB_LINE             3        // line

#define DOB_LINE_NONE        0        // no line or border
#define DOB_LINE_SOLID       1        // solid line
#define DOB_LINE_DOTTED      2        // dotted line

#define SSTYPE_CHAR        1          // character type style sheet
#define SSTYPE_PARA        2          // paragraph type style sheet

#define BASE_PAGE      0              // object y position relative to the top of the page
#define BASE_MARG      1              // object y position relative to the top margin
#define BASE_PARA      2              // object y position relative to the anchor paragraph

#define WINDOW_EXT   250              // window extension to be used before playing a TER metafile

#define SEL_CELLS      1
#define SEL_COLS       2
#define SEL_ROWS       3
#define SEL_ALL        4

#define BKPICT_STRETCH 1
#define BKPICT_TILE    2

#define ZWNJ_CHAR           4        // zero-width non-joiner - used to unligate
#define HPARA_CHAR          5        // hidden para character
#define HYPH_CHAR           6        // optional hyphen
#define PAGE_CHAR           12       // new page character
#define NBSPACE_CHAR        14       // non-breaking space character
#define LINE_CHAR           15       // line break character
#define FTR_CHAR            16       // page footer begin/end char
#define HDR_CHAR            17       // page header begin/end char
#define ROW_CHAR            18       // new table row character
#define CELL_CHAR           19       // new table cell character
#define SECT_CHAR           20       // new section character
#define PARA_CHAR           21       // new paragraph character
#define COL_CHAR            22       // new column character
#define NBDASH_CHAR         23       // non-breaking dash character
#define PICT_CHAR           24       // picture holder charcter
#define FHDR_CHAR           25       // first page header character
#define FFTR_CHAR           26       // first page footer character
#define HIDDEN_CHAR         28       // invisible character
#define LAST_CTRL_CHAR      28       // last control character

#define CHARID_CLEAR_LEFT   1        // clear-left
#define CHARID_CLEAR_RIGHT  2        // clear-right
#define CHARID_CLEAR_ALL    3        // clear-all


#define BRDRTYPE_SINGLE           0
#define BRDRTYPE_DBL              1
#define BRDRTYPE_TRIPLE           2
#define BRDRTYPE_SHADOW           3
#define BRDRTYPE_THICK_THIN       4
#define BRDRTYPE_THICK_THIN_THICK 5
#define BRDRTYPE_THIN_THICK       6
#define BRDRTYPE_THIN_THICK_THIN  7
#define BRDRTYPE_NONE             8
#define BRDRTYPE_LAST             8

#define TFLAG_RESIZE_BITMAP      0x1    // resize bitmap when inserted in a frame
#define TFLAG_METRIC             0x2    // show ruler in cm
#define TFLAG_RETURN_MSG_ID      0x4    // saves the msg ids to return to the user program.
#define TFLAG_APPLY_PRT_ORIENT   0x8    // apply current printer orientation to the document
#define TFLAG_APPLY_PRT_PROPS    0x8    // renamed - apply current printer orientation to the document
#define TFLAG_IGNORE_PICT_LINK   0x10   // ignore picture link names for writing to the disk
#define TFLAG_DISABLE_ACCEL      0x20   // Disable the accelerators
#define TFLAG_USE_NEXT_ID        0x40   // use next id for cell,row, etc.
#define TFLAG_SHOW_CARET         0x80   // display cater even in the read-only mode
#define TFLAG_UNPROTECTED_DEL    0x100  // unprotected block deletes
#define TFLAG_NO_HOUR_GLASS      0x200  // don't display hour glass cursor
#define TFLAG_NO_CHILD_TOP       0x400  // don't bring child (TER as child) to top
#define TFLAG_NO_WRAP            0x800  // to temporarily turnoff word wrapping
#define TFLAG_ACTIVATE_MDI_CHILD 0x1000 // explicity activate the current MDI child when TER gains focus
#define TFLAG_COMP_WORD97        0x2000 // follow word 97 compatible behavior
#define TFLAG_EXCLUDE_HIDDEN_SEL 0x4000 // exclude hidden text from selection
#define TFLAG_AUTO_VSCROLL_BAR   0x8000 // automatic vertical scroll bar
#define TFLAG_NO_PALETTE         0x10000L // do not use palettes
#define TFLAG_KEEP_PICT_ASPECT   0x20000L // maintain the picture aspect ration on resize
#define TFLAG_KEEP_FRAME_ASPECT  0x40000L // maintain the frame aspect ration on resize
#define TFLAG_PICT_IN_FRAME      0x80000L // insert picture in a frame
#define TFLAG_NO_PRINTER         0x100000L // don't open any printer
#define TFLAG_NO_DRAG_TEXT       0x200000L // disable dragging of text
#define TFLAG_NO_EDIT_OLE        0x400000L // disable editing of ole objects
#define TFLAG_BETTER_256         0x800000L // better 256 color pallet at the cost of picture loading performance
#define TFLAG_NO_EDIT_PICT       0x1000000L // disable editing of picture objects
#define TFLAG_SHOW_BREAKS        0x2000000L // show break lines even in the ReadOnly mode
#define TFLAG_SELECT_FULL_HLINK  0x4000000L // select hyperlink completely
#define TFLAG_BUF_DISP           0x8000000L // do display through a memory buffer
#define TFLAG_NO_OLE             0x10000000L // disable ole
#define TFLAG_ROW_PASTE          0x20000000L // enable table row pasting
#define TFLAG_NO_AUTO_FULL_CELL_SEL 0x40000000L // do not automatically select the cell marker
#define TFLAG_SWAP_DECIMAL       0x80000000L // swap decimal/comma

#define TFLAG2_COMP_WPF          0x1         // follow word perfect compatible behavior
#define TFLAG2_RETAIN_BKND       0x2         // retain the background
#define TFLAG2_USE_PAL_FOR_TEXT  0x4         // use the current pallet to draw the text
#define TFLAG2_CAN_MERGE_PROT_TEXT     0x8   // can merge protected text
#define TFLAG2_BKPICT_OVER_PAGE_BORDER 0x10  // write background picture over the frame border
#define TFLAG2_XPARENT_WRITE     0x20        // write transparently to the screen
#define TFLAG2_NO_CARET          0x40        // don't show caret even in the edit mode
#define TFLAG2_SHOW_SECT_PAGE_NO 0x80        // show the section page numbers on the status bar, instead of the sequential page numbers
#define TFLAG2_NO_CURSOR_CHANGE  0x100       // do not change cursor over the editor
#define TFLAG2_VERT_THUMB_TRACK  0x200       // update screen during thumbtracking
#define TFLAG2_NO_AUTO_REPAGE    0x400       // do not allow automatic repagination
#define TFLAG2_NO_BKP_FILE       0x800       // don't create the backup file when saving
#define TFLAG2_SELECT_FRAME_PICT 0x1000      // select the frame picture instead of the frame when clicked on a frame with a picture.
#define TFLAG2_HIDE_PAGE_BREAK   0x2000      // don't display the page break line
#define TFLAG2_PROTECT_FORMAT    0x4000      // protect the format of the protected text
#define TFLAG2_NO_HIDDEN_RTF_TEXT 0x8000     // don't save hidden text to the rtf file
#define TFLAG2_NO_SHADE_FIELD_TEXT 0x10000   // don't shade the field text
#define TFLAG2_IGNORE_TIMER      0x20000     // ignore timer
#define TFLAG2_NO_AUTO_HDR_FTR   0x40000     // do not diplay hdr/footer automatically at file input or paste
#define TFLAG2_WRITE_FIRST_RTF_COLOR 0x80000 // write the first rtf color
#define TFLAG2_FULL_REPAINT      0x100000    // upgarde line painting to full painting
#define TFLAG2_KEEP_PRINTER_OPEN 0x200000    // keep the printer open after the last TE window is closed
#define TFLAG2_ALT_PARA_SYM      0x400000    // display alternate paragraph symbol
#define TFLAG2_ALT_LINE_SYM      0x800000    // display alternate line break symbol
#define TFLAG2_NO_LINE_FITTING   0x1000000   // don't attempt to fit longer screen lines to printer line length
#define TFLAG2_NO_PRT_CANCEL_DLG 0x2000000   // don't show printing cancel dialog box
#define TFLAG2_INDENT_FRAMES     0x4000000   // indent the frames along with the text
#define TFLAG2_INDENT_TABLES     0x8000000   // indent the table rows along with the text
#define TFLAG2_NO_ADJUST_CURSOR  0x10000000  // do not adjust cursor for the hidden text
#define TFLAG2_CURSOR_BEF_HIDDEN 0x20000000  // when adjusting cursor, place it before the hidden text
#define TFLAG2_NO_CURSOR_ON_PROTECT 0x40000000  // do not let the cursor stay on the hidden text

#define TFLAG3_WRAP_SPACES       0x1         // wrapp additional spaces to the next line.
#define TFLAG3_NO_EDIT_TABLE_COL 0x2         // do not allow table column width/indentation editing using mouse
#define TFLAG3_TABLE_STATUS_LINE 0x4         // modify the status lines when tables are encountered
#define TFLAG3_PLAIN_TABLE_BORDER 0x8        // no 3d table border for the HTML tables
#define TFLAG3_GRAY_READ_ONLY    0x10        // gray the control in the read only mode
#define TFLAG3_CURSOR_IN_CELL    0x20        // restrict the cursor to current cell
#define TFLAG3_EXACT_CELL_WIDTH  0x40        // strictly honor exact cell width for the HTML table
#define TFLAG3_NO_SCROLL         0x80        // display only the top of the document
#define TFLAG3_NO_FULL_CELL_COPY 0x100       // do not copy the cell marker when only one cell selected
#define TFLAG3_HTML_CONT_TABLE   0x200       // write two contiguous rows as one table
#define TFLAG3_SELECT_FIRST_FIELD 0x400      // select first form field when the document is open
#define TFLAG3_MULTIPLE_RTF_GROUPS 0x800     // allow multiple RTF groups
#define TFLAG3_OLD_WORD_FORMAT   0x1000      // write old word format rtf syntax
#define TFLAG3_EMBEDDED_TABLES   0x2000      // allow embedded tables
#define TFLAG3_DATA_FIELD_INPUT  0x4000      // data field input mode
#define TFLAG3_GET_BUF_HDR_FTR   0x8000      // get header/footer from rtf buffer insertion if hdr/ftr does not exist in the document 
#define TFLAG3_HTML_READ         0x10000     // in html read
#define TFLAG3_NO_RTF_BKND_COLOR 0x20000     // do not read/write background color to the RTF file
#define TFLAG3_NO_SAVE_UNDO      0x40000     // do not save for undo
#define TFLAG3_LARGE_PARA_BORDER 0x80000     // paragraph border includes the paragraph space bef/aft
#define TFLAG3_STYLES_ON_TOOLBAR 0x100000    // enable style sheet selection on the toolbar
#define TFLAG3_SHOW_FRAMES       0x200000    // debug tool - show frame borders
#define TFLAG3_LINE_SCROLL       0x400000    // scroll one line at a time on arrow up/down
#define TFLAG3_PRINT_BKND_PICT   0x800000    // print background picture
#define TFLAG3_PASTE_RTF_CODE    0x1000000   // paste rtf codes in text format
#define TFLAG3_CLIP_CELL_OVERFLOW 0x2000000  // clip the text that does not fit in the cell (during printing)
#define TFLAG3_EXACT_SCREEN_FONT 0x4000000   // create exact screen font
#define TFLAG3_ZERO_CELL_HEIGHT  0x8000000   // set zero height for empty cells
#define TFLAG3_READ_PNG          0x10000000  // read png from the rtf file
#define TFLAG3_NO_TEXT_COLOR_ADJ 0x20000000  // don't adjust text color for the background color
#define TFLAG3_NO_MOUSE_SEL      0x40000000 // disable text selection by mouse

#define TFLAG4_COUNT_PCHAR_AS_CRLF  0x1         // count para char as two character in the the absolute to row/col conversion
#define TFLAG4_SKIP_PROT_TEXT       0x2         // do not allow the cussor on the protected text
#define TFLAG4_HIDE_HIDDEN_PARA_MARK 0x4        // hide the paragraph marker which has hidden attribute
#define TFLAG4_READONLY_CONTROLS    0x8         // disable embedded controls in TerSetReadOnly function
#define TFLAG4_NO_REPAGINATE        0x10        // hold repagination
#define TFLAG4_SMOOTH_SCROLL        0x20        // smooth scrolling on page boundaries during text selection using mouse
#define TFLAG4_NO_BUFFERED_SCROLL   0x40        // disable buffered scrolling
#define TFLAG4_TEXT_HIDDEN          0x80        // change the hyperlink format to text followed by hidden code
#define TFLAG4_AUTO_SPELL          0x100        // turn-on the auto speller feature
#define TFLAG4_BINARY_RTF_PICT     0x200        // write rtf pictures in the binary format
#define TFLAG4_UNDO_WINDOW_OVERFLOW 0x400       // undo the previous insertion/replace function if text overflows the window
#define TFLAG4_IME_UNICODE         0x800        // convert dbcs characters in IME to unicode
#define TFLAG4_MOD_END_MARK_FONT   0x1000       // allow deletion of end marker 
#define TFLAG4_ONE_ROW_TOOLBAR     0x2000       // use only the second row of the toolbar
#define TFLAG4_NO_OLE_DROP         0x4000       // no ole drop
#define TFLAG4_REPORT_EASE_MODE    0x8000       // being access by ReportEase
#define TFLAG4_ALWAYS_INVOKE_OLE   0x10000      // allow editing of ole objects in read-only mode as well.
#define TFLAG4_DISABLE_DATE_UPDATE 0x20000      // Diable updating of date
#define TFLAG4_SAVE_BMP_AS_PNG     0x40000      // save bmp as png
#define TFLAG4_SAVE_SHAPE_WITH_DRAW_OBJECT 0x80000 // save shape oject with drawing object 
#define TFLAG4_HTML_INPUT          0x100000     // the input is in the html format
#define TFLAG4_NO_DRAG_PROT_TEXT   0x200000     // do not allow drag/drop of protected text.
#define TFLAG4_FULL_DRAG_PROT_TEXT 0x400000     // allow full drag/drop of protected text.
#define TFLAG4_NO_MERGE_TABLE      0x800000     // do not allow deletion of paramarker before the table
#define TFLAG4_NO_TOC_UPDATE       0x1000000    // do not update table of contents
#define TFLAG4_NO_RESET_DC         0x2000000    // do not call ResetDC for printing - some fax/printing drivers do not handle ReestDC properly
#define TFLAG4_NO_SHARE_BORDER     0x4000000    // do not share cell borders
#define TFLAG4_ADJ_LEFT_TABLE_COL  0x8000000    // Adjust only the left column of the table
#define TFLAG4_DONT_FIX_NEG_INDENT 0x10000000   // Do not provide fix for negative indentation
#define TFLAG4_PASTE_LAST_PARA_PROP 0x20000000   // apply last paragraph properties from the rtf paste to an empty paragrapy
#define TFLAG4_PRINT_WMF_AS_BMP    0x40000000   // print the metafile as bmp

#define TFLAG5_NO_EXT_DROP         0x1          // do not allow dropping of external files
#define TFLAG5_GROUP_UNDO          0x2          // group all opeations in one undo
#define TFLAG5_NO_EXACT_ROW_HEIGHT 0x4          // translate the exact row height to 'minmum row height' during rtf read
#define TFLAG5_RTL_CURSOR          0x8          // reverse the behavior of left and right cursor keys
#define TFLAG5_NO_KB_SEL           0x10         // no text selection using keyboard
#define TFLAG5_WRITE_DOB           0x20         // write the drawing objects in the old format
#define TFLAG5_NO_OBJ_IN_STATUS_LINE 0x40       // do not count object when dislaying the status lines
#define TFLAG5_NO_CLEAR_SPL_HIST   0x80         // do not clear spell-checking history buffer between the sessions
#define TFLAG5_NO_NORMALIZE_FNOTE  0x100        // do not adjust block to fully contain the footnote
#define TFLAG5_INPUT_TO_UNICODE    0x200        // convert user-input or rtf input text to unicode
#define TFLAG5_NO_NORMALIZE_FIELD  0x400        // do not adjust block to fully contain the data field
#define TFLAG5_BEF_AND_AFT_HIDDEN  0x800        // allow cursor on the first hidden character and after the hidden text
#define TFLAG5_NO_DRAW_IMAGE       0x1000       // disable the TerDrawImage function.
#define TFLAG5_SHOW_PAGE_SETUP     0x2000       // show page-setup instead of print-setup in the printer-setup dialog
#define TFLAG5_FULL_REPAGINATE     0x4000       // fully repaginate the document inthe beginning irrespective of the size of the document
#define TFLAG5_PNG_RGB             0x8000       // save pictures in higher quality (RGB) 
#define TFLAG5_NO_SHOW_SPACE_SYM  0x10000       // do not show the space symbol when ShowParaMark is turned on
#define TFLAG5_OLD_HLINK          0x20000       // support old style hyperlink
#define TFLAG5_NO_DRAG_ROW_LINE   0x40000       // do not allow the dragging of the row line to resize the row height
#define TFLAG5_NO_SHARE           0x80000       // no share resources
#define TFLAG5_SHOW_PAGE_BREAK    0x100000      // show page break all the time
#define TFLAG5_VARIABLE_PAGE_SIZE 0x200000      // Variable page size
#define TFLAG5_SAVE_PICT_AS_WMF   0x400000      // save png, jpg, gif as wmf for compatibility with older RTF readers
#define TFLAG5_NO_DRAG_CELL_LINE  0x800000      // do not allow the dragging of the cell divider line to resize the cell width and row indentation
#define TFLAG5_RULER_INDENT_FIXED 0x1000000     // apply fix indentation when the ruler indentation marks are moved
#define TFLAG5_TOP_ROW_TOOLBAR    0x2000000     // display only the top row of the toolbar
#define TFLAG5_NO_ADJ_FOR_TABLE   0x4000000     // do not adjust the selection for table
#define TFLAG5_UNLIMITED_OLE_SPACE 0x8000000    // provide for unlimited ole object space
#define TFLAG5_OLD_RULER           0x10000000   // use the old ruler
#define TFLAG5_FRAME_TEXT_ONLY     0x20000000   // write frame text only without the frame definition
#define TFLAG5_PROTECT_DATA_FIELD  0x40000000   // protect data field

#define TFLAG6_USE_PEN_FOR_BORDER  0x1          // use pen for frame border drawing during printing
#define TFLAG6_WRITE_DBCS          0x2          // write dbcs characters for the unicode character in the RTF file
#define TFLAG6_WRITE_DEFAULT_COLOR 0x4          // write the default color for the first color
#define TFLAG6_HI_RES              0x8          // use high resolutinon for font creation and disable the printer
#define TFLAG6_DONT_WRITE_PICT_PATH 0x10        // do not write picture path when writingh out linked pictures 
#define TFLAG6_DONT_PROCESS_TAB    0x20         // this flags stops the processing of the tab key
#define TFLAG6_SHOW_PICT_IN_PIXEL_SIZE 0x40     // display picture in real pixel sizehttps://www.mapdinc.net/webte.aspx
#define TFLAG6_DONT_PROCESS_BULLET_KEYS 0x80    // do not use enter/tab/backtab keys for bullet processing
#define TFLAG6_XLATE_UNICODE       0x100        // do unicode translation, useful when the langauge does not tranlate to single byte character set, such as Hinde, Tamil
#define TFLAG6_IN_TEST_MODE      0x200          // in test mode
#define TFLAG6_IN_PLACE_ACTIVATE   0x400        // activate ole objects in-place
#define TFLAG6_ALLOW_CELL_OVERFLOW 0x800        // allow the excess cell text to overflow the cell
#define TFLAG6_INSERT_DROP_PICT_AS_LINK 0x1000  // link to the dropped picture files (instead of embedding it)
#define TFLAG6_WRAP_AT_WINDOW_WIDTH 0x2000      // wrap the text at window width - applicable to PageMode only (not fitted view)
#define TFLAG6_SAVE_TEXT_BK_COLOR   0x4000      // save text background color (TextDefBkColor) to the rtf file.
#define TFLAG6_LIST_TO_TEXT_IN_HTML 0x8000      // convert list to text while saving to the html format
#define TFLAG6_SUPPORT_DRAGON_SPEECH 0x10000    // enable use of Dragon speech library
#define TFLAG6_NO_LINK_MSG          0x20000     // do not display message when mouse hovers overs a text line
#define TFLAG6_NO_TRACK_MSG         0x40000     // do not display a message when mouse hovers over a track-change text
#define TFLAG6_OLE_DROP_SOURCE      0x80000     // use ole DropSource when dragging TE text
#define TFLAG6_CENTER_DLGS_ON_SCR  0x100000     // center the dialog boxes on the screen
#define TFLAG6_PRINT_EMF_AS_BMP    0x200000     // print the metafile as bmp
#define TFLAG6_PASTE_UNICODE       0x400000     // try CF_TEXT first as CF_UNICODE text
#define TFLAG6_ALT_UNI_CHARWIDTH   0x800000     // alternate (faster) way of calculating unicode character width

#define CFLAG_SEL1            0x1      // cell selection flag
#define CFLAG_SEL2            0x2      // another cell selection flag
#define CFLAG_MERGED          0x4      // to be merged with the previous cell
#define CFLAG_AUTO_WIDTH      0x8      // width will be adjusted in the fitted view
#define CFLAG_ROW_SPANNED     0x10     // this cell is spanned by a row above
#define CFLAG_HTML            0x20     // this cell belongs to an html table
#define CFLAG_COL_SPANNED     0x40     // this cell is spanned by a column before
#define CFLAG_HTML_HDR        0x80     // html header cell
#define CFLAG_FIX_WIDTH       0x100    // fixed width specified
#define CFLAG_FIX_WIDTH_PCT   0x200    // fixed width percentage specified
#define CFLAG_PREEXIST        0x400    // preexisting cells (used only during rtf read)
#define CFLAG_RTF_SEL         0x800    // use to mark the cells during rtf write
#define CFLAG_VALIGN_CTR      0x1000   // Vertical center align cell
#define CFLAG_VALIGN_BOT      0x2000   // vertical bottom align cell
#define CFLAG_FORCE_BKND_CLR  0x4000   // use bknd color even when it is same as the default background color
#define CFLAG_MARGIN_OVERRIDE 0x8000   // this cell has different margin than specified by the row
#define CFLAG_VALIGN_BASE     0x10000  // align the base of the top line with other cell which have this flag
#define CFLAG_3D              0x20000  // draw 3d border in html mode

#define LFLAG_PARA           0x1      // paragraph or cell break line
#define LFLAG_BREAK          0x2      // any other break line
#define LFLAG_PARA_FIRST     0x4      // first para line
#define LFLAG_CONTROL        0x8      // line has embedded control
#define LFLAG_FNOTE          0x10     // contains a footnote
#define LFLAG_SOFT_COL       0x20     // soft column break
#define LFLAG_NBSPACE        0x40     // line has non-break space characters
#define LFLAG_LINE           0x80     // line break
#define LFLAG_HTML_RULE      0x100    // is html horizontal rule
#define LFLAG_BOX_TOP        0x200    // draw top para border before this line
#define LFLAG_BOX_BOT        0x400    // draw bot para border before this line
#define LFLAG_SECT           0x800    // section break line
#define LFLAG_FRAME_TOP      0x1000   // first line of a screen frame
#define LFLAG_NBDASH         0x2000   // line has non-break dash characters
#define LFLAG_PICT           0x4000   // line contains an aligned picture
#define LFLAG_PICT_SPACE     0x8000   // contains a picture frame on the left or the right side
#define LFLAG_FNOTETEXT      0x10000  // contains a footnote text
#define LFLAG_FHDR           0x20000  // fist page hdr
#define LFLAG_FFTR           0x40000  // fist page ftr
#define LFLAG_HDR            0x80000  // regular page hdr
#define LFLAG_FTR            0x100000 // regular page ftr
#define LFLAG_HYPH           0x200000 // regular page ftr
#define LFLAG_SHADE_BEGIN    0x400000 // shading begins at this line
#define LFLAG_SHADE_END      0x800000 // shading ends at this line
#define LFLAG_TOC            0x1000000 // has table of contents
#define LFLAG_LIST           0x2000000 // line contains the list number to display
#define LFLAG_HPARA          0x4000000 // line contains a hidden para marker
#define LFLAG_LISTNUM        0x8000000 // has list number field in it
#define LFLAG_SELECTED      0x10000000 // result of LineSelected function
#define LFLAG_ASSUMED_TAB   0x20000000 // this cell line assumes a decimal tab in the beginning of the line
#define LFLAG_AUTONUMLGL    0x40000000 // has an autonumlgl field in the line


#define LFLAG_HDR_FTR   (LFLAG_HDR|LFLAG_FTR)
#define LFLAG_FHDR_FFTR (LFLAG_FHDR|LFLAG_FFTR)
#define LFLAG_HDRS_FTRS (LFLAG_HDR|LFLAG_FTR|LFLAG_FHDR|LFLAG_FFTR)
#define LFLAG_HDRS      (LFLAG_HDR|LFLAG_FHDR)
#define LFLAG_FTRS      (LFLAG_FTR|LFLAG_FFTR)

#define LFLAG2_SPELL_CHECKED    0x1    // this line has been spell checked
#define LFLAG2_ENOTETEXT        0x2    // contains a enotetext line
#define LFLAG2_TC               0x4    // HAS tc field
#define LFLAG2_HIDDEN_CHAR      0x8    // has a HIDDEN_CHAR
#define LFLAG2_BOX_BETWEEN      0x10   // draw 'between' para border after this line
#define LFLAG2_RTL              0x20   // right to left line
#define LFLAG2_DEL_REV          0x40   // line contain text deleted by a reviewer
#define LFLAG2_INS_REV          0x80   // line contain text deleted by a reviewer

#define INFO_TAB            0x1
#define INFO_SECT           0x2
#define INFO_PAGE           0x4
#define INFO_COL            0x8
#define INFO_CELL           0x10
#define INFO_ROW            0x20
#define INFO_TABLE          0x40
#define INFO_JUST           0x80
#define INFO_FRAME          0x400
#define INFO_SPACE_LINE     0x800
#define INFO_PAGE_NUMBER    0x1000

#define PARA_FRAME_HPAGE      0x1        // align relative to the left edge of the page
#define PARA_FRAME_USED       0x2
#define PARA_FRAME_RIGHT_JUST 0x4
#define PARA_FRAME_CENTER     0x8
#define PARA_FRAME_SET_WIDTH  0x10
#define PARA_FRAME_VPAGE      0x20       // align relative to the top edge of the page
#define PARA_FRAME_VMARG      0x40       // align relative to the top margin of the page
#define PARA_FRAME_TEXT_BOX   0x80       // is a text box drawing object
#define PARA_FRAME_LINE       0x100      // is a line object
#define PARA_FRAME_RECT       0x200      // is a rectangle object
#define PARA_FRAME_BOXED      0x400      // object with borders or solid line
#define PARA_FRAME_DOTTED     0x800      // dotted lines
#define PARA_FRAME_SELECT     0x1000     // used for temporary selection
#define PARA_FRAME_NO_WRAP    0x2000     // don't wrap text around this frame
#define PARA_FRAME_WRAP_THRU  0x4000     // wrap through the frame
#define PARA_FRAME_POSITIONED 0x8000     // TRUE after a paragraph frame is positioned
#define PARA_FRAME_PAGE_BREAK 0x10000    // this picture frame causes a page break
#define PARA_FRAME_CONV       0x20000    // this frame/text box was converted during rtf read
#define PARA_FRAME_NO_LINE    0x40000    // do not draw the line object
#define PARA_FRAME_IGNORE_X   0x80000    // ignore the x position
#define PARA_FRAME_IGNORE_Y   0x100000   // ignore the y position
#define PARA_FRAME_SHPGRP     0x200000   // part of a shape group
#define PARA_FRAME_WATERMARK  0x400000   // is watermark pictur frame
#define PARA_FRAME_FILL_PICT  0x800000   // Fill the background with picture
#define PARA_FRAME_IN_CELL    0x1000000  // shape placed relative to the containing cell
#define PARA_FRAME_VCENTER    0x2000000  // vertically centered frame
#define PARA_FRAME_FIXED_HEIGHT 0x4000000  // fixed height


#define PARA_FRAME_OBJECT     (PARA_FRAME_TEXT_BOX|PARA_FRAME_LINE|PARA_FRAME_RECT)

#define REL_SCREEN            0
#define REL_WINDOW            1
#define REL_TEXT_BOX          2
#define REL_TEXT_ORIG         3

#define ACTION_COMMAND        1
#define ACTION_VSCROLL        2
#define ACTION_HSCROLL        3
#define ACTION_CHAR           4
#define ACTION_LBUTTONDOWN    5
#define ACTION_RBUTTONDOWN    6
#define ACTION_LBUTTONUP      7
#define ACTION_RBUTTONUP      8
#define ACTION_LBUTTONDBLCLK  9
#define ACTION_RBUTTONDBLCLK  10
#define ACTION_SIZE           11
#define ACTION_SETFOCUS       12
#define ACTION_KILLFOCUS      13
#define ACTION_MOUSEMOVE      14
#define ACTION_QUERYENDSESSION 15

#define BLT_ROUND             0
#define BLT_DIAMOND           1
#define BLT_SQUARE            2
#define BLT_HOLLOW_SQUARE     3
#define BLT_4_DIAMONDS        4
#define BLT_ARROW             5
#define BLT_CHECK             6

#define BLTFLAG_HIDDEN        0x1

#define NBR_DEC               0     // decimal numbering
#define NBR_UPR_ALPHA         1
#define NBR_LWR_ALPHA         2
#define NBR_UPR_ROMAN         3
#define NBR_LWR_ROMAN         4
#define NBR_DEFAULT           5

#define LIST_DEC              0
#define LIST_UPR_ROMAN        1
#define LIST_LWR_ROMAN        2
#define LIST_UPR_ALPHA        3
#define LIST_LWR_ALPHA        4
#define LIST_ORDINAL          5
#define LIST_CARDINAL         6
#define LIST_ORDINAL_TEXT     7
#define LIST_DEC_PAD         22
#define LIST_BLT             23
#define LIST_NO_NUM         255

#define LISTFLAG_RESTART_SEC 0x1

#define LISTAFT_TAB           0
#define LISTAFT_SPACE         1
#define LISTAFT_NONE          2

#define LISTLEVEL_RESTART     0x1   // restart numbering when a different ListOr is encountered for the same list - restarting happens only once for each ListOr
#define LISTLEVEL_OLD         0x2   // converted from word 6.0
#define LISTLEVEL_RESTARTED   0x4   // used temporarily during list number calculation
#define LISTLEVEL_LEGAL       0x8   // convert previous levels to arabic
#define LISTLEVEL_REFORMAT    0x10  // format override
#define LISTLEVEL_NO_RESET    0x20  // don't reset numbering when lowerlevel (parent) level encountered 

#define PID_LINE              0
#define PID_PARA              1
#define PID_BULLET            2
#define PID_TAB               3
#define PID_STYLE             4      // style id

#define TER_FILE              1
#define TER_BUF               2

#define SCOPE_BEGIN           0
#define SCOPE_FORWARD         1
#define SCOPE_BACKWARD        2
#define SCOPE_ANY             3

#define ANIM_CONTINUOUS       -2

#define SECT_ALL              -1
#define SECT_CUR              -2

#define SECT_NEW_PAGE        0x1
#define SECT_RESTART_PAGE_NO 0x2
#define SECT_HAS_TITLE_PAGE  0x4
#define SECT_EXACT_MARGT     0x8    // exact top margin
#define SECT_EXACT_MARGB     0x10   // exact bottom margin
#define SECT_BRDR_HEADER     0x20   // border around header
#define SECT_BRDR_FOOTER     0x40   // border around footer
#define SECT_VALIGN_CTR      0x80   // center the page
#define SECT_VALIGN_BOT     0x100   // bottom align the page
#define SECT_LINE           0x200   // show line numbering

#define PARAM_IGNORE          -31234

#define FIELD_PAGE_NUMBER    1        // field id for the page field
#define FIELD_TEXTBOX        2        // textbox field
#define FIELD_CHECKBOX       3        // checkbox field
#define FIELD_LISTBOX        4        // list box field
#define FIELD_PAGE_COUNT     5        // total number of pages in the document
#define FIELD_NAME           6        // field name
#define FIELD_DATA           7        // field data
#define FIELD_DATE           8        // date/time field
#define FIELD_TOC            9        // table of content
#define FIELD_PRINTDATE     10        // printdate field
#define FIELD_LISTNUM       11        // list number field
#define FIELD_AUTONUMLGL    12        // autonumgl field
#define FIELD_TC            13        // TC tag for TOC content
#define FIELD_HLINK         14        // hyperlink field
#define FIELD_XE            15        // index entry field
#define FIELD_PAGE_REF      16        //  page reference field


#define TER_FIRST            0
#define TER_LAST             1
#define TER_NEXT             2
#define TER_PREV             4
#define TER_CUR              5

#define WC_SELECTION         0x1
#define WC_INCLUDE_HDR_FTR   0x2
#define WC_INCLUDE_HIDDEN    0x4

#define TERTAG_USER              0         // placed by the calling application
#define TERTAG_BKM               1         // bookmark tag
#define TERTAG_TOC               2         // used to tag the table of contents
#define TERTAG_HEADING           3         // used to tag the heading lines
#define TERTAG_SECT              4         // saves the section id
#define TERTAG_HPARA             5         // Stores the hidden para id
#define TERTAG_HPARA2            6         // Stores the para id in the hidden field data and other items

#define OCX_METHOD           0
#define OCX_SET_PROP         1
#define OCX_GET_PROP         2
#define OCX_SET_PROP_REF     3

#define OCX_NONE             0
#define OCX_BOOL             1
#define OCX_LONG             2
#define OCX_STRING           3
#define OCX_DISPATCH         4

#define TLB_LINE             1              // vertical line
#define TLB_TYPEFACE         2
#define TLB_POINTSIZE        3
#define TLB_BOLD             4
#define TLB_ITALIC           5
#define TLB_ULINE            6
#define TLB_ALIGN_LEFT       7
#define TLB_ALIGN_RIGHT      8
#define TLB_ALIGN_CENTER     9
#define TLB_ALIGN_JUSTIFY    10
#define TLB_INC_INDENT       11
#define TLB_DEC_INDENT       12
#define TLB_STYLE            13
#define TLB_ZOOM             14
#define TLB_CUT              15
#define TLB_COPY             16
#define TLB_PASTE            17
#define TLB_SPACER           18              // icon separator
#define TLB_NEW              19
#define TLB_OPEN             20
#define TLB_SAVE             21
#define TLB_PRINT            22
#define TLB_HELP             23
#define TLB_PAR              24
#define TLB_PREVIEW          25
#define TLB_NUMBER           26
#define TLB_BULLET           27
#define TLB_UNDO             28
#define TLB_REDO             29
#define TLB_FIND             30
#define TLB_DATE             31
#define TLB_PAGE_NUM         32
#define TLB_PAGE_COUNT       33
#define MAX_TLB              34     // one more than the last

/******************************************************************************
    Message ids
*******************************************************************************/
#define MSG_NO_ERROR              0
#define MSG_BAD_BMP_FILE          1
#define MSG_BAD_FNOTE_MARKER      2
#define MSG_BAD_OBJ_SIZE          3
#define MSG_BAD_PRINTER_DRV       4
#define MSG_CELL_DATA_MISSING     5
#define MSG_CELL_WITHOUT_ROW      6
#define MSG_CHARSTYLE_IN_USE      7
#define MSG_CHAR_FMT_MISSING      8
#define MSG_CLIPBOARD_BUSY        9
#define MSG_CLIPBOARD_NO_DATA     10
#define MSG_CLIPBOARD_NO_FMT      11
#define MSG_COUNT_MISMATCH        12
#define MSG_CREATE_NEW_FILE       13
#define MSG_END_BACKWARD_SEARCH   14
#define MSG_END_FORWARD_SEARCH    15
#define MSG_ERR_ADDING_STRING     16
#define MSG_ERR_ADDING_VALUE      17
#define MSG_ERR_ADVISE_CONNECT    18
#define MSG_ERR_ADV_CREATE        19
#define MSG_ERR_BLOCK_SIZE        20
#define MSG_ERR_BMP_CREATE        21
#define MSG_ERR_CTL_CREATE        22
#define MSG_ERR_DATA_RETRIEVE     23
#define MSG_ERR_DIB_CREATE        24
#define MSG_ERR_END_PAGE          25
#define MSG_ERR_END_PRINTER       26
#define MSG_ERR_FILE_LENGTH       27
#define MSG_ERR_FILE_OPEN         28
#define MSG_ERR_FILE_READ         29
#define MSG_ERR_FILE_SEEK         30
#define MSG_ERR_FILE_WRITE        31
#define MSG_SEL_PROTECTED         32
#define MSG_SPELL_OVER            33
#define MSG_STYLESHEET_MISSING    34
#define MSG_TAB_TBL_MISSING       35
#define MSG_TBL_DATA_MISSING      36
#define MSG_TYPE_STYLE_NAME       37
#define MSG_UNDO_EMPTY            38
#define MSG_UNEXPECTED_EOF        39
#define MSG_SEL_SPAN_HDRFTR       40
#define MSG_SECT_TBL_MISSING      41
#define MSG_ERR_FONT_CREATE       42
#define MSG_ERR_FONT_SELECT       43
#define MSG_ERR_INIT_BMP          44
#define MSG_ERR_INIT_OLE          45
#define MSG_ERR_INIT_PRINTER      46
#define MSG_ERR_INSTANCE          47
#define MSG_ERR_LOAD_ACCEL        48
#define MSG_ERR_MAIL_MERGE        49
#define MSG_ERR_MEM_LOCK          50
#define MSG_ERR_META_ACCESS       51
#define MSG_ERR_META_BAD_FILE     52
#define MSG_ERR_META_COPY         53
#define MSG_ERR_META_CREATE       54
#define MSG_ERR_META_READ         55
#define MSG_ERR_META_RECREATE     56
#define MSG_ERR_OBJ_DATA          57
#define MSG_ERR_OBJ_SAVE          58
#define MSG_ERR_OBJ_SIZE          59
#define MSG_ERR_OLE_CLSID         60
#define MSG_ERR_OLE_DATA_CONV     61
#define MSG_ERR_OLE_LOAD          62
#define MSG_ERR_OLE_SAVE          63
#define MSG_ERR_OLE_USER          64
#define MSG_ERR_OLE_VERSION       65
#define MSG_ERR_PEN               66
#define MSG_ERR_PICT_FILE_FMT     67
#define MSG_ERR_PICT_FILE_OPEN    68
#define MSG_ERR_PICT_FILE_READ    69
#define MSG_ERR_PRESENT_DATA      70
#define MSG_ERR_SITE_CREATE       71
#define MSG_ERR_START_PRINTER     72
#define MSG_ERR_STORAGE_CREATE    73
#define MSG_ERR_STORAGE_RETRIEVE  74
#define MSG_ERR_TEXT_METRIC       75
#define MSG_ERR_VIEW_RETRIEVE     76
#define MSG_FILE_INFO_MISSING     77
#define MSG_FILE_SIGN_MISSING     78
#define MSG_FONT_TBL_INCOMPL      79
#define MSG_INVALID_NUMBER        80
#define MSG_INVALID_PRINT_RECT    81
#define MSG_INVALID_SECT_DATA     82
#define MSG_INVALID_SELECT        83
#define MSG_KEEP_HDRFTR_LINE      84
#define MSG_LINE_TOO_LONG         85
#define MSG_LINE_TRUNCATED        86
#define MSG_MARGIN_TOO_LARGE      87
#define MSG_MAX_LINES_EXCEEDED    88
#define MSG_MERGE_DATA_TOO_LONG   89
#define MSG_MISSING_ROW_MARKER    90
#define MSG_NODEL_HDRFTR_BODY     91
#define MSG_NOT_TE_FILE           92
#define MSG_NO_HILIGHT            93
#define MSG_NO_OLE_CLASS          94
#define MSG_NO_RANGE_PRINTING     95
#define MSG_NO_SERVER             96
#define MSG_NO_STRING             97
#define MSG_NULL_FIELD            98
#define MSG_OLEERR_NEED_SHARE     99
#define MSG_OLEERR_NO_CLASS      100
#define MSG_OUT_OF_CELL_SLOT     101
#define MSG_OUT_OF_CHAR_STACK    102
#define MSG_OUT_OF_COMP_DC       103
#define MSG_OUT_OF_DC            104
#define MSG_OUT_OF_DRAG_OBJ      105
#define MSG_OUT_OF_FONT_SLOT     106
#define MSG_OUT_OF_GROUP         107
#define MSG_OUT_OF_MEM           108
#define MSG_OUT_OF_PARA_SLOT     109
#define MSG_OUT_OF_PARAFID_SLOT  110
#define MSG_OUT_OF_RANGE         111
#define MSG_OUT_OF_ROW_SLOT      112
#define MSG_OUT_OF_STYLESHEET    113
#define MSG_OUT_OF_TAB_POS       114
#define MSG_OUT_OF_TAB_TABLE     115
#define MSG_OUT_OF_TASK          116
#define MSG_OUT_OF_WIN_SLOTS     117
#define MSG_PARASTYLE_IN_USE     118
#define MSG_PARA_FMT_MISSING     119
#define MSG_PARA_TBL_INCOMPL     120
#define MSG_PFRAME_DATA_MISSING  121
#define MSG_PRINT_AREA_SMALL     122
#define MSG_PROTECTED            123
#define MSG_READ_ABORTED         124
#define MSG_REPLACE_STRING       125
#define MSG_SAVE_INCOMPL         126
#define MSG_SAVE_MODS            127
#define MSG_SECTION_FULL         128
#define MSG_BAD_WIN_HANDLE       129
#define MSG_BAD_VERSION          130
#define MSG_NODEL_MIX_HTML       131
#define MSG_ERR_TOO_MANY_FILES   132
#define MSG_ERR_FILE_ACCESS      133
#define MSG_REDO_EMPTY           134
#define MSG_OUT_OF_BLT_TABLE     135
#define MSG_BULLET_MISSING       136
#define MSG_BAD_JPG_FILE         137
#define MSG_ERR_JPG_HUFF         138
#define MSG_ERR_JPG_MARKER       139
#define MSG_ERR_JPG_NO_HUFF      140
#define MSG_TAG_TABLE_MISSING    141
#define MSG_ERR_ENH_META_HDR     142
#define MSG_GET_ENH_META         143
#define MSG_MAX_WIN_LINES        144
#define MSG_SOFT_PAGE_BREAK      145
#define MSG_SECT_BREAK           146
#define MSG_PAGE_BREAK           147
#define MSG_COL_BREAK            148
#define MSG_PAGE_HDR             149
#define MSG_PAGE_FTR             150
#define MSG_ROW_BREAK            151
#define MSG_DEL_DRAW_OBJ         152
#define MSG_DEL_FRAME            153
#define MSG_IRREVERSIBLE_DEL     154
#define MSG_ERR_PRINT            155
#define MSG_FIRST_HDR            156
#define MSG_FIRST_FTR            157
#define MSG_DEL_FIRST_HDR        158
#define MSG_DEL_FIRST_FTR        159
#define MSG_BAD_PNG_DATA         160
#define MSG_PRINTING_CANCELLED   161
#define MSG_SMALL_CELL_MARGIN    162
#define MSG_READ_ONLY            163
#define MSG_NO_PRINTER           164
#define MSG_DEL_TOC              165
#define MSG_NO_RTF_INFO_BLOCK    166
#define MSG_NO_LIST_BLOCK        167
#define MSG_NO_LIST_OR_BLOCK     168
#define MSG_DEL_HDR              169
#define MSG_DEL_FTR              170
#define MSG_ERR_JPG_MCU          171
#define MSG_OUT_OF_IMAGE_MAP_TABLE 172
#define MSG_COLUMNS              173
#define MSG_GIF_FILES            174
#define MSG_ENH_METAFILES        175
#define MSG_BMP_FILES            176 
#define MSG_METAFILES            177 
#define MSG_JPG_FILES            178 
#define MSG_PNG_FILES            179 
#define MSG_PICTURE_SIZE         180 
#define MSG_SINGLE               181 
#define MSG_15LINES              182 
#define MSG_DBL_LINE             183 
#define MSG_EXACTLY              184 
#define MSG_MULTIPLE             185 
#define MSG_LINES                186 
#define MSG_POINTS               187 
#define MSG_TAB_POS              188 
#define MSG_MARGIN               189 
#define MSG_PNG_ABORT            190 
#define MSG_FATAL_ERROR          191 
#define MSG_FILE_OPEN            192 
#define MSG_ERROR                193 
#define MSG_SPL_DONE             194 
#define MSG_BAD_RTF              195 
#define MSG_UNKNOWN              196
#define MSG_ATLEAST              197 
#define MSG_PAGE_BOX             198 
#define MSG_IGNORE_ALL           199 
#define MSG_ADD                  200 
#define MSG_SB_INS               201
#define MSG_SB_OT                202
#define MSG_SB_PAGE              203
#define MSG_SB_LINE              204
#define MSG_SB_ROW               205
#define MSG_SB_COL               206
#define MSG_PP_ONE_PAGE          207
#define MSG_PP_TWO_PAGES         208
#define MSG_PP_PRINT             209
#define MSG_PP_EDIT              210
#define MSG_PP_PG                211
#define MSG_PP_FITTED            212
#define MSG_TIFF                 213
#define MSG_TIF_FILES            214
#define MSG_INVOKE_TEXT_LINK     215
#define MSG_NO_REVIEWER_BLOCK    216
#define MSG_INSERTED_BY          217
#define MSG_DELETED_BY           218
#define MSG_NO_CHANGED_TEXT      219
#define MSG_ACCEPT_CHANGES       220
#define MSG_CHANGES_COUNT        221

#define MAX_MSGS                 250
 

/******************************************************************************
    WIN32 specific defines
*******************************************************************************/
#if defined (WIN32)
   #define TER_HUGE
   #define _export
#else
   #define TER_HUGE huge
#endif

/******************************************************************************
    More constant definitions
*******************************************************************************/
typedef LRESULT (FAR PASCAL _export *VBX_CALLBACK)(HWND, UINT,WPARAM,LPARAM);
typedef LRESULT (FAR PASCAL _export *MSG_CALLBACK)(HWND, UINT,WPARAM,LPARAM);
typedef int     (FAR PASCAL _export *PRINT_CALLBACK)(HWND,HWND);

/****************************************************************************
   Instructions for packing the following structure
*****************************************************************************/
#ifndef RC_INVOKED
   #pragma pack(1)
#endif

/******************************************************************************
   Argument Structure:
   A set of control variables are passed by the user program to the
   'CreateTerWindow' routine. The variables are passed using the structure
   'arg_list',  as described below. The user program passes the POINTER to
   this structure.
   This file must be included by your application module that calls the
   TER routine.
*******************************************************************************/
struct arg_list {
    int    x;         /* Initial x position of the editing window,
                         you may specify CW_USEDEFAULT to use default values.*/
    int    y;         // Initial y position of the editing window.
    int    width;     /* Initial width of the window in device units,
                         you may specify CW_USEDEFAULT to use default values.*/
    int    height;    /* Initial height of the editing window.  When you
                         use CW_USEDEFAULT for 'width', the 'height' is
                         ingnored.*/
    long   LineLimit; /* Number of lines allowed in the editor window.
                         Set to 0 to have unlimited number of lines*/

    BOOL   WordWrap;  /* Set this flag to true (1),  if you wish to enable the
                         word wrapping feature. */
    BOOL   PrintView; /* Set this flag to true if you wish the lines on
                         the screen to be wrapped current page margins.
                         Otherwise, the lines are wrapped at the end of
                         the window. This variable is ignored if 'WordWrap'
                         is set to FALSE */
    BOOL   PageMode;  /* Set this flag to true if you wish to edit one page
                         at a time */
    BOOL   FittedView;// Set this flag to enable special PageMode where the
                      // text wraps within the window width
    BOOL   ShowStatus;// show status line ?
    BOOL   ShowMenu;  // show the menu bar ?
    BOOL   ShowHorBar;// show the horizonatal scroll bar
    BOOL   ShowVerBar;// show the vertical scroll bar
    BOOL   ruler;     // True to show the ruler
    BOOL   ToolBar;   // True to show the tool bar
    BOOL   UserCanClose;// TRUE allows the user to close the window
    BOOL   BorderMargins;// display margins around the window borders
    BOOL   ReadOnly;    // TRUE to create a read only session

    long   InitLine;  // Line number to position at the beginning.
    BYTE   InputType; /* This flag specifies the input type. If you wish to
                         edit a file, set the input_type to 'F'. Conversely,
                         if you wish to pass the text for editing in a buffer,
                         set this field to 'B'.*/

    BYTE   file[131];/*  If the input type is set to 'F',  specify the file
                         name for editing in this field. */

                     // ===== Buffer input fields only =======
    HGLOBAL hBuffer; /*  Specify the global memory handle containing the
                         input text data.  This handle becomes the property
                         of the editor.  Your program must never try to lock
                         or free this handle */
    long BufferLen;  //  Buffer length in number of characters.
    BYTE delim;      /*  Line delimiter character.  This character must be
                         used to delimit individual lines within the
                         text buffer.  This character is normally typically set
                         to 13 (carriage return) */

    BYTE   padding1[3]; // This field is not used

    int    SaveFormat;// The format of the output file: SAVE_TER, SAVE_TEXT,
                      // SAVE_RTF and SAVE_AS_INPUT

    HINSTANCE hInst;  //  Handle of the current instanaces.
    HWND   hParentWnd;// Handle to the parent window
    HWND   hTextWnd; //  Ter main window handle, will be filled by TER later
    DWORD  style;    //  Editor window style
    BYTE   FontTypeFace[28]; // Default type face, example: TmwRmn Courier, etc
    int    PointSize;// point size of the default font face

    BOOL       open;     // TRUE indicates an open window
    int        modified; // For internal use
    };


/******************************************************************************
   Field Data Structure:
   This structure is used to retrieve (GetTerFields()) and set (SetTerFields())
   control fields for a TER window.
*******************************************************************************/
struct StrTerField {
   /* The following fields are read/write fields.  To update a field,
      you must retrieve the current values by calling the GetTerFields function.
      Modify the fields that you wish to, and then call the SetTerFields
      function to make the new value effective
   **************************************************************************/
      int      size;              // size of this structure, used for version matching

      int      CurColW,           // current window column position
               PaintEnabledW,     // FALSE=disables the painting and word wrapping
               WrapFlagW;         // wrap control, see WRAP_ constants

      long     CurRowW,           // current window row position
               BeginLineW,        // line number of the first window line
               CurLineW;          // current line number in the file

      COLORREF TextBkColorW,      // background color for text
               StatusBkColorW,    // background color of the status line
               StatusColorW;      // foreground color of the status line

      int      HilightTypeW,      // line or character block, see HILIGHT_ constants
               HilightBegColW,    // beginning col
               HilightEndColW;    // ending col

      long     HilightBegRowW,    // beginning row
               HilightEndRowW;    // ending row

      BOOL     StretchHilightW;   // TRUE = highlight being stretch

      // please do not use following two fields as they are being phased out.
      // To get the text and font information, use TerGetLine function instead.
      BYTE     text[MAX_WIDTH];  // line text data, NULL terminated
      BYTE     font[MAX_WIDTH];  // font id for every character in the 'text' array

      int      pfmtW;             // paragraph id of the current line
      int      LineLenW;          // length of the 'text' array

      int      TextApply;        /* use this variable to specify how the 'text'
                                    and 'font' data should be applied to the
                                    current TER window, see APPLY_ constants */

      BOOL     ReclaimResourcesW;  // reuse unused font and para ids

      BOOL     ModifyProtectColorW,// modify the color or the protected text
               LinkDblClickW,      // TRUE to activate hyperlink on double click, otherwise use single click
               ShowProtectCaretW;  // TRUE to show caret on the protected text

      UINT     LinkStyleW;         // character style of the hyperlink phrase
      COLORREF LinkColorW;         // color of the linked phrase

      BOOL     SnapToGridW,        // TRUE to snap ruler items to an invisble grid
               HtmlModeW,          // TRUE when html mode in effect
               ShowTableGridLinesW;// TRUE to show table grid lines

   /* The following are the read only fields.  TER will ignore any
      modification to these fields.
   **************************************************************************/
      HWND     hTerWndW;          // handle to the editor window
      HDC      hTerDCW;           // handle to TER class DC
      RECT     TerRectW,          // entire client window rectangal
               TerWinRectW;       // text window rectangal

      long     TotalLinesW,       // total lines in the file
               MouseLineW,        // text line position of mouse
               MaxColBlockW;      // biggest column block allowed

      int      TotalPfmtsW,       // total paragraph ids in use by the current window
               TotalFontsW,       // Total font objects in use by the current window
               TotalStylesW,      // Total number of styles

               WinWidthW,         // current window in columns
               WinHeightW,        // current window height in number of rows
               TerWinOrgXW,       // window origin x co-ordinates
               MouseColW;         // text column position of mouse

      BOOL     modified;          // data modified, user needs to select the 'save' option to save data
      BOOL     WordWrapW;         // TRUE when wordwrap turned on

      int      ParaLeftIndentW;   // paragraph left indent in twips
      int      ParaRightIndentW;  // paragraph right indent in twips
      int      ParaFirstIndentW;  // paragraph first line indent in twips
      UINT     ParaFlagsW;        // paragraph flags,includes such flags as double spacing,centering, right justify etc
      int      ParaTabIdW;        // paragraph tab id, index into the tab table
      int      ParaCellIdW;       // paragraph cell id that the paragraph belongs to
      UINT     ParaShadingW;      // paragraph shading (0 to 10000)
      int      ParaFrameIdW;      // paragraph frame id
      int      ParaSpaceBeforeW;  // paragraph space before the paragraph in twips
      int      ParaSpaceAfterW;   // paragraph space after the paragraph in twips
      int      ParaSpaceBetweenW; // paragraph space between the paragraph in twips
      int      ParaStyleIdW;      // paragraph style id
      int      ParaAuxIdW;        // paragraph auxiliary id
      int      pflagsW;           // paragraph PFLAG_ constants

      int      CurSectW;          // current section number

      int      LeftMarginW;       // section left margin in twips
      int      RightMarginW;      // section right margin in twips
      int      TopMarginW;        // section top margin in twips
      int      BotMarginW;        // section bottom margin in twips
      int      columnsW;          // number of columns in the current section

      int      CurPageW;          // current page number
      int      TotalPagesW;       // total pages

      int      MouseXW;           // last mouse x position
      int      MouseYW;           // last mouse y position

      BOOL     PrintViewW;        // TRUE when the print view mode is turned on
      BOOL     PageModeW;         // TRUE when the page mode is turned on
      BOOL     FittedViewW;       // TRUE when the fitted view is turned on
      BOOL     ShowParaMarkW;     // TRUE when showing the paramarkers
      BOOL     ShowHiddenTextW;   // TRUE when showing the hidden text

      int      CurCtlIdW;         // currently selected control id
      UINT     ParaFrameFlagsW;   // Para frame flags
    };

/******************************************************************************
   Hyperlink Data Structure:
   This structure is used to pass hyperlink data to your application.
*******************************************************************************/
struct StrHyperLink {
      HWND    hWnd;               // Message windows
      BYTE    code[MAX_WIDTH+2];  // hyperlink code
      BYTE    text[MAX_WIDTH+2];  // hyperlink text
      BOOL    DoubleClick;        // TRUE if mouse double clicked, otherwise single click
    };

/******************************************************************************
   Print request parameter structure
   This structure is used to call the 'TerMergePrint' function to print
   a buffer or file containing text data in supported formats (RTF, native TER,
   and Ascii).
*******************************************************************************/
struct StrPrint {
    BYTE   InputType; /* This flag specifies the input type. If you wish to
                         specify a disk file name, set the input_type to 'F'.
                         Conversely, if you wish to pass the text for in a buffer,
                         set this field to 'B'.*/
    BYTE   file[131];/*  If the input type is set to 'F',  specify the input
                         file name in this field. */

                     // ===== Buffer input fields only =======
    HANDLE  hBuffer; /*  Specify the global memory handle containing the
                         input text data. */
    long BufferLen;  //  Buffer length in number of characters.
    BYTE delim;      /*  Line delimiter character used to delimit the lines
                         in the input buffer.  This character can be a CR or
                         any other character. Not used for an RTF buffer */
    BYTE   padding1[3];// This field is not used


    HDC    hDC;       // Printer or Window device context to print to.
                      // If NULL, the default printer
                      // device is opened.  The device context must be set to
                      // the pixel mapping mode.
    LPRECT rect;      // Printing area specified in mm (millimeter) units.
                      // If NULL, the entire page with default margins is used.
                      // The NULL value is not valid when printing to a
                      // window device context
    long   StartPos;  // Starting character position.  Set to 0 to begin from
                      // the first page.
    BOOL   OnePage;   // Set to TRUE to print one (initial) page only.  The
                      // printed page is NOT spooled to the printer.
    long   NextPos;   // (OUTPUT) When OnePage is TRUE, the TerMergePrint function
                      // returns the character where the next page should begin.
                      // This value is set to 0 after the entire buffer is printed

                     // ===== Mail merge fields ==================
    LPBYTE  MergeFields;      // pointer to a list of mail merge field names.
                             // The fields must be separated  by a '|' character.
                             // The list must be terminated by a NULL character
    LPBYTE  MergeData;        // pointer to a list of mail merge data values.
                             // Each data vlue must be separated  by a '|' character.
                             // The list must be terminated by a NULL character
                             // The number of data elements in the MergeData
                             // MUST be same as the number of elements in the
                             // MergeFields array

                     // ===== Other fields ==================
    BOOL   PrintHiddenText;  // TRUE to print any hidden text
    HINSTANCE hInst;         // Instanaces handle of your program
    HWND   hParentWnd;       // A window handle in your application to receive mail merge messages

    int    NextY;            // (OUTPUT) The DLL returns the Y position on the paper

    HDC    hMetaDC;          // Metafile device context to print to.  Set to NULL
                             // print to the hDC specified above or to the default
                             // printer.  When hMetaDC is not NULL, the hDC is
                             // used as an attribute device context.

    };

struct StrTab {              // tab table
    int  count;              // number of tabs
    int  pos[TE_MAX_TAB_STOPS]; // tab stop position in twips
    int  type[TE_MAX_TAB_STOPS];// tab type each stop, TAB_LEFT, TAB_RIGHT
    BYTE flags[TE_MAX_TAB_STOPS]; // Other TAB_ constants
    };                       // pointer to the tab table

/****************************************************************************
   Instructions to resume the default structure packing.
*****************************************************************************/
#ifndef RC_INVOKED
   #pragma pack()
#endif

/******************************************************************************
               TER Messages
*******************************************************************************/

/*******************************************************************************
   TER_CLOSE:
   Ter sends this message to the parent window (your application window)
   before closing.  The wParam variables contains the handle of the TER
   window.  lParam is not used.

   If your application is using a GlobalBuffer, you can retrieve the updated
   buffer when receiving this message. Use the GetTerBuffer() function to
   retrived the global handle to updated text data.
*******************************************************************************/
#define TER_CLOSE    (WM_USER+1700)

/*******************************************************************************
   TER_MODIFIED:
   Ter sends this message to the parent window once when the editor text is
   modified. The wParam variable contains the handle of the TER window.
*******************************************************************************/
#define TER_MODIFIED (WM_USER+1701)

/*******************************************************************************
   TER_LINK:
   Ter sends this message to the parent window when the user clicks on
   the hyperlink text.
*******************************************************************************/
#define TER_LINK (WM_USER+1702)

/*******************************************************************************
   TER_MERGE:
   Ter sends this message to the parent window to get the mail merge field
   data for a field name.  The TerMergePrint function first checks the
   supplied list of mail merge code.  Only if the required code is not
   found in the input list, it sends this message.
   The 'wParam' parameter contains the handle to the TER window, and
   the 'lParam' parameter contains the pointer to the field name.
   Your applicaton should return the pointer to the field text, or NULL if text not available.
*******************************************************************************/
#define TER_MERGE (WM_USER+1703)

/*******************************************************************************
   TER_NOT_SAVED:
   Ter sends this message to the parent window (your application window)
   if the user abandons the modifications.
   The wParam variables contains the handle of the TER window.  lParam is not used.
   The TER_CLOSE message always follows this message.
*******************************************************************************/
#define TER_NOT_SAVED    (WM_USER+1704)

/*******************************************************************************
   TER_UPDATE_TOOLBAR:
   This messages notifies your application to update any external toolbar that
   your application might be using.
*******************************************************************************/
#define TER_UPDATE_TOOLBAR   (WM_USER+1705)
#define TER_UPDATE_STATUSBAR (WM_USER+1706)

#define TER_ACTION           (WM_USER+1707)
#define TER_PREPROCESS       (WM_USER+1708)
#define TER_XLATE_PICT       (WM_USER+1709)

#define TER_HTML_SIZE        (WM_USER+1710)

#define TER_ANIM_PICT        (WM_USER+1711)
#define TER_REPAINT          (WM_USER+1712)
#define TER_DROP             (WM_USER+1713)
#define TER_DRAG             (WM_USER+1714)
#define TER_BEFORE_DROP      (WM_USER+1715)
#define TER_PAGE_SIZE_CHANGING (WM_USER+1716)
#define TER_CREATED          (WM_USER+1717)
#define TER_SAVE_UNDO        (WM_USER+1718)

/******************************************************************************
                     Editor Function Prototype
   When calling TER from a C program, leave the TER_CPLUSPLUS symbol undefined.

   When calling TER from a C++ program, define the TER_CPLUSPLUS as following:

   #define TER_CPLUSPLUS extern "C"
   #include "ter.h"
*******************************************************************************/
#ifndef TER_CPLUSPLUS
   #if defined(__cplusplus)
      #define TER_CPLUSPLUS extern "C"
   #else
      #define TER_CPLUSPLUS
   #endif
#endif

TER_CPLUSPLUS HWND   WINAPI _export CreateTerWindow(struct arg_list far *);
TER_CPLUSPLUS BOOL   WINAPI _export CloseTer(HWND,BOOL);
TER_CPLUSPLUS HGLOBAL WINAPI _export GetTerBuffer(HWND, long far *);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerBuffer(HWND,HGLOBAL,long,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export GetTerFields(HWND,struct StrTerField far *);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerFields(HWND,struct StrTerField far *);
TER_CPLUSPLUS BOOL   WINAPI _export InsertRtfBuf(HWND,BYTE TER_HUGE *,long,long,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerFont(HWND,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerDefaultFont(HWND,LPBYTE,int,UINT,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerPointSize(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerCharStyle(HWND,UINT,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerColor(HWND,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerBkColor(HWND,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export InsertTerText(HWND,BYTE TER_HUGE *,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SelectTerText(HWND,long,int,long,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerCursorPos(HWND,long,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export GetTerCursorPos(HWND,long far *,int far *);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerParaFmt(HWND,UINT,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export ParaIndentTwips(HWND,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export ParaLeftIndent(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export ParaRightIndent(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export ParaHangingIndent(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export ParaNormal(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export DeselectTerText(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export SetTab(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export ClearTab(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export ClearAllTabs(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerMergeFields(HWND,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerMergePrint(struct StrPrint far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerMergePrintVB(struct StrPrint far *,LPRECT,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerPageBreak(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPrint(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPrintPreviewMode(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPrintPreview(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerColBreak(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerEnableRefresh(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerLocateStyle(HWND,UINT,long far *,int far *,int far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerSectBreak(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetModify(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetOutputFormat(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaShading(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export LoadTerControl(void);
TER_CPLUSPLUS int    WINAPI _export GetTerLine(HWND,long,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export SetTerLine(HWND,long,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export GetFontInfo(HWND,int,LPBYTE,LPINT,UINT far *);
TER_CPLUSPLUS BOOL   WINAPI _export SaveTerFile(HWND,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export ReadTerFile(HWND,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetTextColor(HWND,int,LPDWORD,LPDWORD);
TER_CPLUSPLUS UINT   WINAPI _export TerMenuEnable(HWND,int);
TER_CPLUSPLUS UINT   WINAPI _export TerMenuSelect(HWND,int);
TER_CPLUSPLUS long   WINAPI _export TerRowColToAbs(HWND,long,int);
TER_CPLUSPLUS void   WINAPI _export TerAbsToRowCol(HWND,long,long far *,int far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerPrintPreview(HWND,HDC,RECT far *,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetMargin(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetReadOnly(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerEnableSpeedKey(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerRewrap(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSearchReplace(HWND,LPBYTE,LPBYTE,UINT,long,long far *,long far *);
TER_CPLUSPLUS int    WINAPI _export TerPastePicture(HWND,UINT,HGLOBAL,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetHypertext(HWND,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerCreateTable(HWND,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerPictureFromFile(HWND,LPBYTE,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerInsertParaFrame(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerMoveParaFrame(HWND,int,int,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export GetTerFieldsAlt(HWND,struct StrTerField far *,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaSpacing(HWND,int,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerDataVersion(void);
TER_CPLUSPLUS BOOL   WINAPI _export TerAppendText(HWND,BYTE TER_HUGE *,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertText(HWND,BYTE TER_HUGE *,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerCreateFont(HWND,int,BOOL,LPBYTE,int,UINT,COLORREF,COLORREF,int,int);
TER_CPLUSPLUS int    WINAPI _export TerCreateParaId(HWND,int,BOOL,int,int,int,int,int,int,UINT,UINT,int,int,int,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetAppMemory(HWND,long);
TER_CPLUSPLUS LPVOID WINAPI _export TerGetAppMemory(HWND);
TER_CPLUSPLUS int    WINAPI _export TerSetDefTabWidth(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertObjectId(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerRegisterVbxCallback(HWND,VBX_CALLBACK);
TER_CPLUSPLUS VBX_CALLBACK WINAPI _export TerGetVbxCallback(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerRegisterMsgCallback(HWND,MSG_CALLBACK);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetPictInfo(HWND,int,UINT far *,LPRECT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPictInfo(HWND,int,UINT,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPictSize(HWND,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPictSizeTwips(HWND,int,int,int); // MAK
TER_CPLUSPLUS int    WINAPI _export TerPictureFromWmf(HWND,LPBYTE,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerInsertControl(HWND,LPBYTE,DWORD,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPrinter(HWND,LPBYTE,LPBYTE,LPBYTE,HGLOBAL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosHdrFtr(HWND,int,BOOL,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosTable(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerRepaint(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParentPtr(HWND,void far *);
TER_CPLUSPLUS LPVOID WINAPI _export TerGetParentPtr(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertFootnote(HWND,LPBYTE,LPBYTE,UINT,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerLocateStyleChar(HWND,UINT,BOOL,long far *,int far *,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerScrLineHeight(HWND,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPreview(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSect(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetFontId(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPaper(HWND,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerInsertDrawObject(HWND,int,int,int,int,int,int);
TER_CPLUSPLUS int    WINAPI _export TerGetCurFont(HWND,long,int);
TER_CPLUSPLUS int    WINAPI _export TerGetEffectiveFont(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetBullet(HWND,BOOL,BOOL);
TER_CPLUSPLUS HFONT  WINAPI _export TerGetEffectiveFontEx(HWND);
TER_CPLUSPLUS int    WINAPI _export TerEditStyle(HWND,BOOL,LPBYTE,BOOL,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSelectCharStyle(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSelectParaStyle(HWND,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetFontStyleId(HWND,int);
TER_CPLUSPLUS int    WINAPI _export TerSetZoom(HWND,int);
TER_CPLUSPLUS long   WINAPI _export TerGetPageFirstLine(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetTab(HWND,int,int,BYTE,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetSectFromLine(HWND,int);
TER_CPLUSPLUS int    WINAPI _export TerGetPageSect(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSectOrient(HWND,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetPageOrient(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerRowPosition(HWND,UINT,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerRowHeight(HWND,int,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetFrameYBase(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetObjectAttrib(HWND,int,int,int,COLORREF,BOOL,COLORREF);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetBkPictId(HWND,int,UINT,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerInsertPictureFile(HWND,LPBYTE,BOOL,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetTableColWidth(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertTableCol(HWND,BOOL,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertTableRow(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerCellBorder(HWND,int,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerCellShading(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosLineAtTop(HWND,long,BOOL);
TER_CPLUSPLUS DWORD  WINAPI _export TerSetFlags(HWND,BOOL,DWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetFrameSize(HWND,int,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS int    WINAPI _export TerGetTabStop(HWND,long,int,LPINT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerAppendTextEx(HWND,BYTE TER_HUGE *,int,int,int,int,BOOL);
TER_CPLUSPLUS HGLOBAL WINAPI _export TerGetRtfSel(HWND,LPLONG);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetParaInfo(HWND,long,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,UINT far *,LPINT,LPINT,LPINT,UINT far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetLineInfo(HWND,long,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPDWORD,UINT far *);
TER_CPLUSPLUS int    WINAPI _export TerGetLastMessage(LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerResetLastMessage();
TER_CPLUSPLUS BOOL   WINAPI _export TerPictLinkName(HWND,int,BOOL,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetCellInfo(HWND,int,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,UINT far *);
TER_CPLUSPLUS int    WINAPI _export TerGetControlId(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerRepaginate(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetReadOnly(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaAuxId(HWND,long,long,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSavePict(HWND,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaId(HWND,long,long,int);
TER_CPLUSPLUS int    WINAPI _export TerCreateCellId(HWND,BOOL,int,int,int,int,int,int,int,int,int,int,int,int,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCharAuxId(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertLine(HWND,BYTE TER_HUGE *,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteCells(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerShrinkFontTable(void);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPaperEx(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS long   WINAPI _export TerMergePrintRep(HDC,LPBYTE,HGLOBAL,DWORD,RECT far *,LPBYTE,LPBYTE,long,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteBlock(HWND,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetLine(HWND,long,LPBYTE,LPWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetLine(HWND,long,LPBYTE,LPWORD);
TER_CPLUSPLUS HGLOBAL WINAPI _export TerGetTextSel(HWND,LPLONG);
TER_CPLUSPLUS BOOL   WINAPI _export TerTextPosToPix(HWND,int,long,int,LPINT,LPINT);
TER_CPLUSPLUS HGLOBAL WINAPI _export TerFileToMem(LPBYTE,DWORD far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerSelectPrint(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosFrame(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosBodyText(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPixToTextPos(HWND,int,int,int,LPDWORD,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetNextFontAux1Id(HWND,int);
TER_CPLUSPLUS int    WINAPI _export TerGetFontAux1Id(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPflags(HWND,UINT,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerCommand(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDefPrinter(LPBYTE,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertRtfFile(HWND,LPBYTE,long,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerNormalizeBlock(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSearchString(HWND,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosPage(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCharSet(HWND,BYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetPagePos(HWND,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPagePos(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSectEx(HWND,int,int,BOOL,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetHypertextEx(HWND,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetPageOrientEx(HWND,int,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerHtmlCellWidthFlag(HWND,int,UINT,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerTableSplitCell(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetNewFrameDim(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetBorderColor(HWND,COLORREF);
TER_CPLUSPLUS BOOL   WINAPI _export TerCellColor(HWND,int,COLORREF,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerInsertPictureFileXY(HWND,LPBYTE,BOOL,int,BOOL,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetBulletEx(HWND,BOOL,BOOL,int,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetBulletInfo(HWND,int,long,LPINT,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS int    WINAPI _export TerCreateBulletId(HWND,BOOL,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetBulletId(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerPictureFromJpg(HWND,LPBYTE,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDefTextColor(HWND,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetWrapWidth(HWND,int,int,BOOL);
TER_CPLUSPLUS DWORD  WINAPI _export TerOverrideStyles(DWORD);
TER_CPLUSPLUS HWND   WINAPI _export TerCreateWindowAlt(int,int,int,int,BOOL,HWND,DWORD);
TER_CPLUSPLUS int    WINAPI _export TerInsertLineObject(HWND,int,int,int,int);
TER_CPLUSPLUS int    WINAPI _export TerCreateParaIdEx(HWND,int,BOOL,int,int,int,int,int,int,UINT,UINT,int,int,int,UINT,int,COLORREF);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaBkColor(HWND,BOOL,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerIgnoreCommand(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerFlushUndo(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetWinBorder(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerLineSelected(HWND,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerCellVertAlign(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSelectCol(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteHypertext(HWND,long,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetCellBorderWidth(HWND,int,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetAccelHandle(HWND,HANDLE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetSectInfo(HWND,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS int    WINAPI _export TerCreateTabId(HWND,struct StrTab far *);
TER_CPLUSPLUS DWORD  WINAPI _export TerSetFlags2(HWND,BOOL,DWORD);
TER_CPLUSPLUS int    WINAPI _export TerCreateParaFrameId(HWND,int,int,int,int);
TER_CPLUSPLUS int    WINAPI _export TerSetTag(HWND,long,int,LPBYTE,LPBYTE,long,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosTag(HWND,int,LPBYTE,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPrintEx(HWND,BOOL,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetDrawObjectInfo(HWND,int,LPINT,LPINT,LPINT,COLORREF far *,COLORREF far *,UINT far *);
TER_CPLUSPLUS int    WINAPI _export TerGetDispPageNo(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerIsPrinting(HWND);
TER_CPLUSPLUS int    WINAPI _export TerInsertOleObject(HWND,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerAdjustHtmlTable(HWND);
TER_CPLUSPLUS int    WINAPI _export TerXlateControlId(HWND,int);
TER_CPLUSPLUS HDC    WINAPI _export TerGetBufferDC(HWND);
TER_CPLUSPLUS long   WINAPI _export TerGetCaretPos(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetHypertext2(HWND,long,int,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetMarginEx(HWND,int,int,int,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerRowPositionEx(HWND,UINT,int,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerMovePictFrame(HWND,int,int,int);
TER_CPLUSPLUS int    WINAPI _export TerGetMarginEx(HWND,int,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS int    WINAPI _export TerGetTag(HWND,long,int,LPBYTE,LPBYTE,LPLONG,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetObjectAttribEx(HWND,int,int,int,COLORREF,BOOL,COLORREF,int);
TER_CPLUSPLUS int    WINAPI _export TerGetSeqSect(HWND,int);
TER_CPLUSPLUS long   WINAPI _export TerSplitMbcs(HWND,BYTE TER_HUGE *,BYTE TER_HUGE *,BYTE TER_HUGE *);
TER_CPLUSPLUS int    WINAPI _export TerMakeMbcs(HWND,LPBYTE,LPBYTE,LPBYTE,int);
TER_CPLUSPLUS int    WINAPI _export TerGetLineEx(HWND,long,LPBYTE,LPBYTE,LPWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetLineEx(HWND,long,LPBYTE,LPBYTE,LPWORD);
TER_CPLUSPLUS int    WINAPI _export TerPictureFromGif(HWND,LPBYTE,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerRegisterPrintCallback(PRINT_CALLBACK);
TER_CPLUSPLUS long   WINAPI _export TerMbcsLen(HWND,BYTE TER_HUGE *);
TER_CPLUSPLUS WORD   WINAPI _export TerMbcsChar(HWND,BYTE TER_HUGE *,long);
TER_CPLUSPLUS int    WINAPI _export TerInsertBookmark(HWND,long,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteBookmark(HWND,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosBookmark(HWND,LPBYTE,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetBookmark(HWND,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerCreateFirstHdrFtr(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteFirstHdrFtr(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosHdrFtrEx(HWND,int,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetRtfDocInfo(HWND,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetRtfDocInfo(HWND,int,LPBYTE);
TER_CPLUSPLUS int    WINAPI _export TerPictureFromEmf(HWND,LPBYTE,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertField(HWND,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS long   WINAPI _export TerGetField(HWND,long,int,int,BYTE TER_HUGE *);
TER_CPLUSPLUS BOOL   WINAPI _export TerLocateField(HWND,int,LPBYTE,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPrint2(HWND,BOOL,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerChangeField(HWND,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteField(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetTextCase(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCharSpace(HWND,BOOL,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetFontSpace(HWND,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetFontSpace(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerOverridePageSize(int,int);
TER_CPLUSPLUS long   WINAPI _export TerGetWordCount(HWND,UINT);
TER_CPLUSPLUS int    WINAPI _export TerPageFromLine(HWND,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetFontStyleId(HWND,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerClosePrinter(void);
TER_CPLUSPLUS int    WINAPI _export TerCreateFont2(HWND,int,BOOL,LPBYTE,int,UINT,COLORREF,COLORREF,int,int,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetHdrRow2(HWND,int,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetHdrRow(HWND,int,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetInitTypeface(LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerDocName(HWND,BOOL,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetRowKeep(HWND,int,BOOL,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetFontFieldId(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSect2(HWND,int,int,BOOL,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export GetFontInfo2(HWND,int,LPBYTE,LPINT,UINT far *);
TER_CPLUSPLUS DWORD  WINAPI _export TerSetFlags3(HWND,BOOL,DWORD);
TER_CPLUSPLUS int    WINAPI _export TerGetTagEx(HWND,long,int,int,LPBYTE,LPBYTE,LPLONG,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosTagEx(HWND,int,int,LPBYTE,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetTablePos(HWND,LPINT,LPINT,LPINT);
TER_CPLUSPLUS UINT   WINAPI _export TerAnd(UINT,UINT);
TER_CPLUSPLUS UINT   WINAPI _export TerOr(UINT,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSpellCheck(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetPageBorderDim(HWND,LPINT,LPINT);
//MAK TER_CPLUSPLUS int    WINAPI _export TerInsertTextInputField(HWND,LPBYTE,LPBYTE,int,BOOL,LPBYTE,int,UINT,COLORREF,BOOL,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerInsertTextInputField(HWND,LPBYTE,LPBYTE,int,BOOL,LPBYTE,int,
                                                            UINT,COLORREF,BOOL,UINT,HWND*,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerInsertTextInputFieldEx(HWND,LPBYTE,LPBYTE,int,BOOL,LPBYTE,int,
                                                            UINT,COLORREF,BOOL,UINT,HWND*,LPBYTE,LPBYTE,BOOL);

TER_CPLUSPLUS int    WINAPI _export TerInsertCheckBoxField(HWND,LPBYTE,int,BOOL,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetParaInfo2(HWND,long,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,UINT far *,LPINT,LPINT,LPINT,UINT far *,DWORD far *, COLORREF far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteStyle(HWND,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDefDir(HWND,LPBYTE,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCharLang(HWND,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetFontLang(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDefLang(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetFocus(HWND);
TER_CPLUSPLUS int    WINAPI _export TerGetStyleId(HWND,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerLocateFontId(HWND,int,LPLONG,LPINT);
TER_CPLUSPLUS int    WINAPI _export TerCreateBulletId2(HWND,BOOL,int,int,int,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetBullet2(HWND,BOOL,BOOL,int,int,int,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerLocateInputField(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetInputFieldInfo(HWND,int,LPBYTE,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetCheckboxInfo(HWND,int,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetTextFieldInfo(HWND,int,LPBYTE,LPINT,LPINT,LPBYTE,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCheckboxInfo(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetInputFieldInfo(HWND,int,LPBYTE,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetTextFieldInfo(HWND,int,LPBYTE,int,int,LPBYTE,int,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetSelection(HWND,LPLONG,LPINT,LPLONG,LPINT);
TER_CPLUSPLUS int    WINAPI _export TerGetStyleInfo(HWND,int,LPBYTE,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSelectRow(HWND,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerInsertOleFile(HWND,LPBYTE,BOOL,BOOL,long,int);
TER_CPLUSPLUS int    WINAPI _export TerGetVisibleCol(HWND,long,int);
TER_CPLUSPLUS DWORD  WINAPI _export TerSetFlags4(HWND,BOOL,DWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteHdrFtr(HWND,BYTE,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetTableLevel(HWND,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetRowInfo(HWND,int,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCaretPos(HWND,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerUpdateToolbar(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosAfterTable(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetParaInfo3(HWND,long,BOOL,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,UINT far *,LPINT,LPINT,LPINT,UINT far *,DWORD far *, COLORREF far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaSpacing2(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerQueryExit(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertFootnote2(HWND,LPBYTE,LPBYTE,UINT,BOOL,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerEditList(HWND,BOOL,int,BOOL,LPBYTE,BOOL,DWORD);
TER_CPLUSPLUS int    WINAPI _export TerEditListOr(HWND,BOOL,int,BOOL,int,BOOL,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerEditListLevel(HWND,BOOL,int,int,int,int,int,LPBYTE,int,DWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaList(HWND,BOOL,int,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerCreateListBulletId(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetStatusColor(HWND,COLORREF,COLORREF);
TER_CPLUSPLUS BOOL   WINAPI _export TerMenuSelect2(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerMenuEnable2(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCellInfo2(HWND,int,COLORREF,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerEditTooltip(HWND,int,LPBYTE);
TER_CPLUSPLUS int    WINAPI _export TerGetBulletInfo2(HWND,int,long,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaIndent(HWND,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetListInfo(HWND,int,LPBYTE,LPINT,LPDWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetListOrInfo(HWND,int,LPINT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetListLevelInfo(HWND,BOOL,int,int,LPINT,LPINT,LPINT,LPBYTE,LPINT,LPDWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteObject(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetListInfo2(HWND,int,LPBYTE,LPINT,LPDWORD,LPLONG,LPLONG);
TER_CPLUSPLUS BOOL   WINAPI _export TerSpellCheckerPopped(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerCellBorderColor(HWND,int,COLORREF,COLORREF,COLORREF,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetCellBorderColor(HWND,int,COLORREF far *,COLORREF far *,COLORREF far *,COLORREF far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetParaInfo4(HWND,long,BOOL,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,UINT far *,LPINT,LPINT,LPINT,UINT far *,DWORD far *, COLORREF far *, LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerRecreateToolbar(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerHideToolbarIcon(int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetPictOffset(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPictOffset(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerUpdateHyperlinkText(HWND,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerUpdateHyperlinkCode(HWND,LPBYTE);
TER_CPLUSPLUS int    WINAPI _export TerInsertHyperlink(HWND,LPBYTE,LPBYTE,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerFindHlinkField(HWND,LPBYTE,LPBYTE,LPLONG,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetPageCount(HWND,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerLocateFieldChar(HWND, int, LPBYTE, BOOL, long far *, int far *, BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosTable2(HWND,int,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetTablePos2(HWND,LPINT,LPINT,LPINT,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSectBorder(HWND,int,int,int,int,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSectPageSize(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerCellWidth(HWND,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosTable3(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetTableId(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetTableId(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerCreateOcxObject2(HWND,LPBYTE,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetOcxIntProp(HWND,int,LPBYTE,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetOcxTextProp(HWND,int,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerRotateFrameText(HWND,BOOL,long,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPictFrame2(HWND,int,int,int,int,BOOL);
TER_CPLUSPLUS long   WINAPI _export TerGetParam(HWND,int);
TER_CPLUSPLUS LPVOID WINAPI _export TerGetOlePtr(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetFrameMarginDist(HWND,int);
TER_CPLUSPLUS int    WINAPI _export TerSetUndoRef(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerLparam2String(long,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerString2Lparam(LPBYTE,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerScrToTwipsX(HWND,int,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerScrToTwipsY(HWND,int,LPINT);
TER_CPLUSPLUS int    WINAPI _export TerGetTabStop2(HWND,int,long,int,LPINT,LPINT,LPINT);
TER_CPLUSPLUS int    WINAPI _export TerSearchReplace2(HWND,LPBYTE,LPBYTE,UINT,long,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaTextFlow(HWND,BOOL,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSectTextFlow(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetRowTextFlow(HWND,BOOL,BOOL,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDocTextFlow(HWND,BOOL,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetWaveUnderline(HWND,long,int,int,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerApplyHyperlink(HWND,LPBYTE,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerCreateBulletId3(HWND,BOOL,int,int,int,LPBYTE,LPBYTE,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetBullet3(HWND,BOOL,BOOL,int,int,int,LPBYTE,LPBYTE,UINT,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPictAltInfo(HWND,int,BOOL,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPictCropping(HWND,int,int,int,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetPictCropping(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetMaxUndo(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSelectCellText(HWND,BOOL);
TER_CPLUSPLUS HINSTANCE WINAPI _export TerPdfStart(LPBYTE,LPINT,LPBYTE,LPBYTE,int,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL WINAPI _export   TerPdfEnd(HINSTANCE,int);
TER_CPLUSPLUS BOOL WINAPI _export   TerPdfPrintPage(HWND,int,int);
TER_CPLUSPLUS BOOL WINAPI _export   TerPdfSetDllName(LPBYTE);
TER_CPLUSPLUS BOOL WINAPI _export   TerSetParamRect(HWND,int,int,int,int);
TER_CPLUSPLUS DWORD  WINAPI _export TerSetFlags5(HWND,BOOL,DWORD);
TER_CPLUSPLUS BOOL WINAPI _export   TerGetCellInfo2(HWND,int,COLORREF far *,LPINT);
TER_CPLUSPLUS BOOL WINAPI _export   TerIsTableSelected(HWND);
TER_CPLUSPLUS int  WINAPI _export   TerCreateFont3(HWND,int,BOOL,LPBYTE,int,UINT,COLORREF,COLORREF,int,int,int,int,int,UINT);
TER_CPLUSPLUS BOOL WINAPI _export   TerSetCtlColor(HWND,COLORREF,BOOL);
TER_CPLUSPLUS BOOL WINAPI _export   TerInsertTextU(HWND,WORD TER_HUGE *,int,int,BOOL);
TER_CPLUSPLUS long WINAPI _export   TerGetFontParam(HWND,int,int);
TER_CPLUSPLUS BOOL WINAPI _export   TerSelectTable(HWND,int,BOOL);
TER_CPLUSPLUS BOOL WINAPI _export   TerSetSect3(HWND,int,int,BOOL,int,int,int,int);
TER_CPLUSPLUS int WINAPI _export    TerGetRowCellCount(HWND,BOOL);
TER_CPLUSPLUS HGLOBAL WINAPI _export TerGetTextSelU(HWND,LPLONG);
TER_CPLUSPLUS int WINAPI  _export   HandleToIntArray(LPWORD,long,HANDLE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetPrinter(HWND,LPBYTE,LPBYTE,LPBYTE);
TER_CPLUSPLUS int  WINAPI _export   TerGetHdrFtrPos(HWND,int);
TER_CPLUSPLUS int  WINAPI _export   TerHdrFtrExists(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteCellText(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPageNumFmt(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerGetPageNumFmt(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetUlineColor(HWND,BOOL,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerAddToolbarIcon(HWND,int,int,int,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetListBullet(HWND, BOOL, int, int, int, LPBYTE, LPBYTE, BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetListLevel(HWND, int, int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetListBullet2(HWND, BOOL, int, int, int, LPBYTE, LPBYTE, LPBYTE,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetBulletInfo3(HWND,int,long,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetSectBins(HWND, int, LPINT, LPINT);
TER_CPLUSPLUS int    WINAPI _export TerGetLineWidth(HWND,long);
TER_CPLUSPLUS int    WINAPI _export TerGetParaParam(HWND,long,BOOL,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetLinkPictDir(HWND, LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDefTabType(HWND,int);
TER_CPLUSPLUS DWORD  WINAPI _export TerSetFlags6(HWND,BOOL,DWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetStyleParam(HWND,int,int,int,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerCancelEditStyle(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetObjectWrapStyle(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerGetStyleParam(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerGetFrameParam(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetWatermarkPict(HWND,int,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertPageRef(HWND,LPBYTE,BOOL,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerCellRotateText(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetParaBorderColor(HWND,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCustomMessage(int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerCellBorder2(HWND,int,int,int,int,int,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetLinkDblClick(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetCellParam(HWND,int,int,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetStCharSet(HWND,int);
TER_CPLUSPLUS int    WINAPI _export TerGetLineU(HWND,long,LPWORD,LPWORD);
TER_CPLUSPLUS DWORD  WINAPI _export TerPdcStart(LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,DWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerPdcEnd(DWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerPdcPrintPage(HWND,DWORD,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetLicenseKey(LPBYTE);
TER_CPLUSPLUS DWORD  WINAPI _export TerPdcStart2(LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,DWORD,DWORD,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetFrameTextDist(HWND, int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerEnableTracking(HWND,BOOL,LPBYTE,BOOL,DWORD,COLORREF,DWORD,COLORREF);
TER_CPLUSPLUS BOOL   WINAPI _export TerFindNextChange(HWND,BOOL,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerAcceptChanges(HWND, BOOL, BOOL, BOOL);


TER_CPLUSPLUS HANDLE WINAPI    _export StrToHandle(BYTE TER_HUGE *,long);        // needed for Visual Basic interface only
TER_CPLUSPLUS BOOL   WINAPI    _export HandleToStr(BYTE TER_HUGE *,long,HANDLE); // needed for Visual Basic interface only
TER_CPLUSPLUS HANDLE WINAPI    _export StrToHandleInChunks(BYTE TER_HUGE *,long,HANDLE,long,long);
TER_CPLUSPLUS BOOL   WINAPI    _export HandleToStrInChunks(BYTE TER_HUGE *,long,HANDLE,long,long);
TER_CPLUSPLUS LRESULT CALLBACK _export VbxProcessMessage(HWND,UINT,WPARAM,LPARAM,LPINT);

// The following functions are for use by the HTML add-on
TER_CPLUSPLUS BOOL   WINAPI _export TerEquateHtmlTable(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetHtmlTblWidth(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCellInfo(HWND,int,COLORREF,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPictPctWidth(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerPictureFromOther(HWND,LPBYTE,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetAnimInfo(HWND,int,long,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerAddAnimPict(HWND,int,int,int);
TER_CPLUSPLUS int    WINAPI _export TerSetPictFrame(HWND,int,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetNextParaAux1Id(HWND,DWORD);
TER_CPLUSPLUS DWORD  WINAPI _export TerGetParaAux1Id(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSavePictDIB(HWND,int,LPBYTE);
TER_CPLUSPLUS int    WINAPI _export TerGetPictFrame(HWND,int);
TER_CPLUSPLUS long   WINAPI _export TerGetControlPos(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetEffectiveFont(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerDifTableRows(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerIsTableWider(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerShrinkTable(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerCommand2(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerIsModified(HWND);
TER_CPLUSPLUS int    WINAPI _export TerGetLevelCell(HWND,int,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerLineInfoFlags(HWND,long,UINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerIsSspAvail(void);
TER_CPLUSPLUS int    WINAPI _export TerCreateImageMap(HWND,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerAddImageMapRect(HWND,int,LPBYTE,LPBYTE,LPBYTE,int,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPictMapId(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerImageMapNameToId(HWND,LPBYTE);
TER_CPLUSPLUS int    WINAPI _export TerGetPictMapId(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetImageMapInfo(HWND,int,LPBYTE,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetImageMapRectInfo(HWND,int,int,LPBYTE,LPBYTE,LPBYTE,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetFontFieldCode(HWND,int,LPBYTE);
TER_CPLUSPLUS int    WINAPI _export TerGetFieldFont(HWND,int,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerEngageCaret(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCellBorderColor(HWND,int,COLORREF,COLORREF,COLORREF,COLORREF);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetFontField(HWND,int,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetMapRectInfo(HWND,int,int,LPBYTE,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetBkPictId(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSearchReplaceU(HWND,PWCHAR,PWCHAR,UINT,long,long far *,long far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerAppendTextU(HWND,LPWORD,int,int,int,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerTab2Spaces(HWND,long,int);
TER_CPLUSPLUS int    WINAPI _export TerCreateListBulletId2(HWND,int,int,int);
TER_CPLUSPLUS int    WINAPI _export TerGetListId(HWND,LPBYTE);
TER_CPLUSPLUS long   WINAPI _export TerGetListLine(HWND,long);
TER_CPLUSPLUS int    WINAPI _export TerOcxPropInfo(HWND,int,int,LPINT,LPBYTE,LPLONG,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetOleClass(HWND,int,LPBYTE);
TER_CPLUSPLUS long   WINAPI _export TerGetLineParam(HWND, long, int);
TER_CPLUSPLUS BOOL   WINAPI _export TerTableOutlineBorder(HWND,int,int,COLORREF,BOOL);


// Undocumented functions - do not use them without consultation with Sub Systems, Inc.
TER_CPLUSPLUS BOOL   WINAPI _export TerQuickRepaint(HWND);
TER_CPLUSPLUS LPBYTE WINAPI _export TerMbcsExt(HWND,LPBYTE,LPBYTE);
TER_CPLUSPLUS LPBYTE WINAPI _export TerMbcsDir(HWND,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDefCellMargin(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetCharDim(HWND,int,UINT,LPINT,LPINT,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetDeviceRes(HWND,LPINT,LPINT,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetTextAreaSize(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerPictureFromPng(HWND,LPBYTE,int,BOOL);
TER_CPLUSPLUS long   WINAPI _export TerGetPrevControlPos(HWND,long);
TER_CPLUSPLUS long   WINAPI _export TerGetNextControlPos(HWND,long);
TER_CPLUSPLUS int    WINAPI _export TerCreateOcxObject(HWND,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInvokeOcx(HWND,int,LPBYTE,UINT,int,DWORD,LPINT,LPLONG,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerDrawImage(HWND,HDC,HDC,LPRECT,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInit(HINSTANCE,DWORD);
TER_CPLUSPLUS BOOL   WINAPI _export TerShadeProtectedText(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerBmp2Png(HWND,int,int,LPBYTE,HGLOBAL far *,LPLONG);
TER_CPLUSPLUS BOOL   WINAPI _export TerInvokeOcx2(HWND,DWORD,LPBYTE,UINT,int,DWORD,LPINT,LPLONG,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSelectPrint2(HWND,int);
TER_CPLUSPLUS LPVOID WINAPI _export TerGetIDispatch(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerDeleteIDispatch(LPVOID);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertDateTime(HWND,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInsertToc(HWND,BOOL);
TER_CPLUSPLUS long   WINAPI _export TerMergePrintRep2(HDC,LPBYTE,HGLOBAL,DWORD,RECT far *,LPBYTE,LPBYTE,long,LPINT,HDC);
TER_CPLUSPLUS long   WINAPI _export TerMergePrintRE(HDC,LPBYTE,HGLOBAL,DWORD,RECT far *,LPBYTE,LPBYTE,long,LPINT,HDC);
TER_CPLUSPLUS BOOL   WINAPI _export TerCopyHeadersFooters(HWND,int,int,BOOL,BOOL,BOOL,BOOL,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetPageOrient2(HWND,int,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDebug(BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInternetOpen(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerInternetClose(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerInternetGet(HWND,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerInternetPut(HWND,LPBYTE,LPBYTE,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetDefBkColor(HWND,COLORREF,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerAddToolbarItem(HWND,int,int,LPBYTE,LPBYTE,LPBYTE,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSpellCheckCurWord(HWND,LPARAM);
TER_CPLUSPLUS long   WINAPI _export TerMergePrintRep3(HDC,LPBYTE,HGLOBAL,DWORD,RECT far *,LPBYTE,LPBYTE,long,LPINT,HDC,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosTable4(HWND,int,int,int,int,int,int,BOOL);
TER_CPLUSPLUS long   WINAPI _export TerGetFontParam(HWND,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerMovePictFrame2(HWND,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerLocateFontFlags(HWND,UINT,BOOL,long far *,int far *,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerPickBknd(HWND);
TER_CPLUSPLUS HGLOBAL WINAPI _export TerGetDevmode(HWND);
TER_CPLUSPLUS HINSTANCE WINAPI _export TerPdfStart2(LPBYTE,LPINT,LPBYTE,LPBYTE,int,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSectAuxId(HWND,int,int);
TER_CPLUSPLUS int    WINAPI _export TerGetSectAuxId(HWND,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSect4(HWND,int,int,BOOL,int,int,int,int,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerSavePictJpg(HWND,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetOverrideBin(HWND,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetOverrideBin(HWND,BOOL,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetOrigPictSize(HWND,int,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCellSpan(HWND, int, int, int);
TER_CPLUSPLUS BOOL   WINAPI _export TerOverridePageCount(HWND,int);
TER_CPLUSPLUS long   WINAPI _export TerGetCurParaNum(HWND);
TER_CPLUSPLUS long   WINAPI _export TerGetParaCount(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerPosAtParaNum(HWND,int,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerBulletToText(HWND,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerTocToText(HWND,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerFieldToText(HWND, BOOL,BOOL);
TER_CPLUSPLUS long   WINAPI _export TerGetTextHeight(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerRedirectArrowKeys(HWND,BOOL);
TER_CPLUSPLUS HINSTANCE WINAPI _export TerPdfStart3(LPBYTE,LPINT,LPBYTE,LPBYTE,int,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,int,int);
TER_CPLUSPLUS int    WINAPI _export TerGetBaseLineY(HWND, int,long);
TER_CPLUSPLUS BOOL   WINAPI _export TerSavePictEmf(HWND, int, LPBYTE);
TER_CPLUSPLUS HINSTANCE WINAPI _export TerPdfStart4(LPBYTE,LPINT,LPBYTE,LPBYTE,int,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,LPBYTE,int,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerMoveParaFrame2(HWND,int,int,int,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerMoveParaFrame3(HWND,int,int,int,int,int,int);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetPageOffset(HWND,int,int,LPINT,LPINT,LPINT,LPINT);
TER_CPLUSPLUS BOOL   WINAPI _export TerSpellCheck2(HWND,BOOL,BOOL,BOOL *);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSectLineNbr(HWND,int,BOOL,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerSavePictGif(HWND,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetPageBkColor(HWND,COLORREF);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetMergeDelim(LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerAddAutoCompWord(HWND,LPBYTE,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerGetVersion(LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetCharOffset(HWND,int,BOOL);
TER_CPLUSPLUS HMETAFILE WINAPI _export TerMergePrintMeta(struct StrPrint far *);
TER_CPLUSPLUS BOOL   WINAPI _export TerPlayMergeMeta(HDC,HDC,HMETAFILE,int,int,int);
TER_CPLUSPLUS int    WINAPI _export TerPictureFromTif(HWND,LPBYTE,int,BOOL);
TER_CPLUSPLUS int    WINAPI _export TerDeleteTag(HWND,long,int,int,LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerLocateChangedChar(HWND,int,int,BOOL,long far *,int far *,BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerLocateAuxIdChar(HWND,int,BOOL,long far *,int far *, BOOL);
TER_CPLUSPLUS BOOL   WINAPI _export TerTest(HWND, int, int, int, LPBYTE, LPBYTE, LPBYTE);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetSectVertAlign(HWND,int, int, BOOL);
TER_CPLUSPLUS int    WINAPI _export TerGetSectParam(HWND, int, int);
TER_CPLUSPLUS BOOL   WINAPI _export TerClearAutoCompList(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerSetNoDropList(HWND,LPBYTE);

// The following function are used to support the ocx
TER_CPLUSPLUS BOOL   WINAPI _export TerRepaintArea(HWND,LPRECT);
TER_CPLUSPLUS BOOL   WINAPI _export TerProcessTab(HWND);
TER_CPLUSPLUS BOOL   WINAPI _export TerIsInIE(HWND,BOOL);

#endif
