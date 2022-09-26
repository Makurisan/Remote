/***************************************************************************
                   INTERNAL FUNCTION PROTOTYPES
****************************************************************************/

/***************************************************************************
                             FILE: TER.C
****************************************************************************/
BOOL    TerAppendText2(HWND,BYTE huge *,LPWORD,int,int,int,int,BOOL);
BOOL    TerPostProcessing(PTERWND,UINT,WPARAM,LPARAM);
BOOL    TerTranslateAccelerator(PTERWND,UINT,WPARAM,LPARAM);
LRESULT TerEraseBackground(PTERWND,HDC);
LRESULT CALLBACK _export TerWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT TerDefWindowProc(HWND,UINT,WPARAM,LPARAM);
int     OverrideStyles(PTERWND);
int     ParamIdToSID(PTERWND,int);
int     ResetInitVariables(PTERWND);
int     SendActionMessage(PTERWND,UINT,WPARAM,LPARAM);
int     SendPreprocessMessage(PTERWND,UINT,WPARAM,LPARAM);
int     TerInitMenu(PTERWND,HMENU);
int     CALLBACK _export WEP(int);
int     WINAPI _export TerExit(int);
int     XlateCommandId(PTERWND,int);
void    LineAlloc(PTERWND,long,UINT,UINT);


/***************************************************************************
                             FILE: TER1.C
****************************************************************************/
int     CALLBACK ScrEnhMetaFileProc(HDC,HANDLETABLE *,CONST ENHMETARECORD *,int,LPARAM);
BOOL    CALLBACK _export ScrEnumMetafile(HDC,HANDLETABLE FAR *,METARECORD FAR *,int,LPARAM);
BOOL    FrameToDraw(PTERWND,int,long);
BOOL    IsFontBreak(PTERWND,int,int);
int     CalcBorderSpill(PTERWND,int,int,int,int);
int     ClearScrForXparentWrite(PTERWND,HDC);
int     CopyCtlImage(PTERWND,int);
int     DisplayText(PTERWND,HDC,int,int,int,int,LPBYTE,int,long,BOOL,BYTE);
int     PaintTer(PTERWND);
int     PictOut(PTERWND,HDC,int,int,int,int);
int     PosControls(PTERWND);
int     RepaintTer(PTERWND);
int     ResetBufBM(PTERWND);
int     TerInvalidateRect(PTERWND,LPRECT,BOOL);
int     TerPictOut(PTERWND,HDC,int,int,RECT far *,int,int);
int     TerTextOut(PTERWND,HDC,int,int,RECT far *,LPBYTE,LPBYTE,int,LPWORD,LPWORD,int);
void    GetWinDimension(PTERWND);
void    PaintFrames(PTERWND,HDC,long);
void    PaintRows(PTERWND,HDC,long,long);

/***************************************************************************
                             FILE: TER2.C
****************************************************************************/
BOOL    AdjustInputPos(PTERWND, BYTE, BYTE, WORD);
BOOL    DoAutoComp(PTERWND, BYTE,BYTE,WORD);
int     FreeClonedLine(PTERWND,struct StrLinePtr far *);
int     FreeClonedLinePtr(PTERWND,struct StrLinePtr far **, long);
struct StrLinePtr far **CloneLinePtr(PTERWND);
struct StrLinePtr far *CloneLine(PTERWND,long);
BOOL    CanInsertInInputField(PTERWND,int,long,int);
BOOL    CanInsertBreakChar(PTERWND,long,int);
BOOL    CanInsertObject(PTERWND,long,int);
BOOL    CanInsertPageBreak(PTERWND,long,int);
BOOL    CanInsertTable(PTERWND,long,int);
BOOL    CanInsertTextObject(PTERWND,long,int);
BOOL    CharMessagePending(PTERWND);
BOOL    CanInsert(PTERWND,long,int);
BOOL    CursorOnFirstWord(PTERWND);
BOOL    GetCursDirection(PTERWND);
BOOL    HiddenText(PTERWND,int);
BOOL    IsHiddenLine(PTERWND,long);
BOOL    IsProtectedChar(PTERWND,long,int);
BOOL    MoveCursor(PTERWND,long,int);
BOOL    MessagePending(PTERWND);
int     AllocDB(PTERWND,BYTE huge *,LPBYTE far *,LPBYTE far *);
int     ImeChar(PTERWND,WPARAM);
int     InitLine(PTERWND,long);
int     FreeLine(PTERWND,long);
int     ScrollText(PTERWND);
int     SetLineText(PTERWND,LPBYTE,long,int);
int     SplitLine(PTERWND,long,int,int);
int     TerAscii(PTERWND,BYTE,BYTE);
int     TerBackSpace(PTERWND);
int     TerBackTab(PTERWND);
int     TerBeginFile(PTERWND);
int     TerBeginLine(PTERWND);
int     TerCtrlDown(PTERWND);
int     TerCtrlUp(PTERWND);
int     TerDel(PTERWND);
int     TerDeleteLine(PTERWND);
int     TerDelPrevWord(PTERWND);
int     TerDoubleClick(PTERWND);
int     TerDown(PTERWND);
int     TerEndFile(PTERWND);
int     TerEndLine(PTERWND);
int     TerInsert(PTERWND);
int     TerInsertLineAft(PTERWND);
int     TerInsertLineBef(PTERWND);
int     TerJoinLine(PTERWND);
int     TerLeft(PTERWND);
int     TerNextWord(PTERWND);
int     TerPageDn(PTERWND,BOOL);
int     TerPageHorz(PTERWND,BYTE,int);
int     TerPageLeft(PTERWND,BOOL);
int     TerPageRight(PTERWND,BOOL);
int     TerPageUp(PTERWND,BOOL);
int     TerPosLine(PTERWND,long);
int     TerPrevWord(PTERWND,BOOL);
int     TerReturn(PTERWND);
int     TerRight(PTERWND);
int     TerSplitLine(PTERWND,int,BOOL,BOOL);
int     TerInsertTab(PTERWND);
int     TerUp(PTERWND);
int     TerWinDown(PTERWND);
int     TerWinLeft(PTERWND);
int     TerWinRight(PTERWND);
int     TerWinUp(PTERWND);
long    TerSplitMbcs2(PTERWND,BYTE TER_HUGE *,BYTE TER_HUGE *,BYTE TER_HUGE *,int);
long    TerSplitMbcs3(PTERWND,BYTE TER_HUGE *,WORD TER_HUGE *, BYTE TER_HUGE *,BYTE TER_HUGE *,WORD TER_HUGE *,int);
void    AdjustHiddenPos(PTERWND);
void    CopyLineData(PTERWND,long,long);
void    GetLineData(PTERWND,long,int,int,LPBYTE,LPBYTE,LPWORD,LPWORD);
void    MoveCharInfo(PTERWND,long,int,long,int,int);
void    MoveLineArrays(PTERWND,long,long,BYTE);
void    MoveLineData(PTERWND,long,int,int,BYTE);
void    SetLineData(PTERWND,long,int,int,LPBYTE,LPBYTE,LPWORD,LPWORD);

/***************************************************************************
                             FILE: TER3.C
****************************************************************************/
BOOL    SaveUndoMarkedCells(PTERWND,struct StrUndo *);
BOOL    SaveUndoTableAttrib(PTERWND,struct StrUndo *,long,int,long,int);
BOOL    SaveUndoPict(PTERWND,int,LPLONG,LPLONG);
int     FreeOneUndo(PTERWND, int);
BOOL    SaveUndoFrame(PTERWND,int,LPLONG,LPLONG,int);
BOOL    SaveUndoRowIns(PTERWND,int,LPLONG,LPLONG);
BOOL    SaveUndoRowDel(PTERWND,int,LPLONG,LPLONG);
BOOL    GetUndoRowRange(PTERWND,LPLONG,LPLONG);
BOOL    CanDragText(PTERWND);
BOOL    GetHypertextEnd(PTERWND,LPLONG,LPINT);
BOOL    GetHypertextStart(PTERWND,LPLONG,LPINT);
BOOL    HasSameParaBorder(PTERWND,long,long);
BOOL    HasSameParaShading(PTERWND,long,long);
BOOL    IsAnchorName(PTERWND,LPBYTE);
BOOL    IsHypertext(PTERWND,int);
BOOL    IsHypertext2(PTERWND,int,BOOL);
BOOL    IsHypertext3(PTERWND,int,BOOL,BOOL);
BOOL    IsLoneHypertextChar(PTERWND,long,int);
BOOL    JustifySpace(PTERWND,int);
BOOL    LineSelected(PTERWND,long);
BOOL    PosAfterHiddenText(PTERWND);
BOOL    SaveUndoAlloc(PTERWND,BYTE,int,long,int,long,int,LPVOID far *,LPVOID far *, LPVOID far *, LPVOID far *);
BOOL    SaveUndoDel(PTERWND,int,long,int,long,int);
BOOL    SaveUndoFont(PTERWND,int,long,int,long,int);
BOOL    SaveUndoPara(PTERWND,int,long,long);
BOOL    SaveUndoRep(PTERWND,int,long,int,long,int);
BOOL    SendLinkMessage(PTERWND,BOOL);
BOOL    TerResetClipRgn(PTERWND);
int     ColToUnits(PTERWND,int,long,int);
int     CreateCellDragObj(PTERWND,int,int,int,int,int);
int     CreateRowDragObj(PTERWND,int,int,int,int,int);
int     GetCharSeg(PTERWND,long,int,int,struct StrLineSeg far *);
int     GetCurPage(PTERWND,long);
int     GetFlatX(PTERWND,int,int,long);
int     GetFrame(PTERWND,long);
int     GetHotSpotHit(PTERWND,LPPOINT,LPINT,LPINT);
int     GetLineHeight(PTERWND,long,LPINT,LPINT);
int     GetLineHeight2(PTERWND,long,LPINT,LPINT,BOOL);
int     GetLinePoints(PTERWND,int,LPINT,LPINT,LPINT,LPINT);
int     GetLineWidth(PTERWND,long,BOOL,BOOL);
int     GetLineSpacing(PTERWND,long,int,int far *,int far *,BOOL);
int     GetLineSpacing2(PTERWND,long,int,int far *,int far *,LPINT,LPINT,BOOL);
int     GetLineSpacingAlt(PTERWND,long,int,int far *,int far *,LPINT,LPINT,BOOL);
int     GetRowHeight(PTERWND,long);
int     GetRowX(PTERWND,long);
int     GetRowY(PTERWND,long);
int     GetSpaceAdj(PTERWND,long,int);
int     GetTabPos(PTERWND,int, struct StrTab far *,int,int far *,int far *,LPBYTE,BOOL);
int     GetTabWidth(PTERWND,long,int,int);
int     GetTextHeight(PTERWND,LPBYTE,LPWORD,int,BOOL,LPINT,LPINT);
int     LineToUnits(PTERWND,long);
int     PosToCol(PTERWND,int,int,long);
int     ReleaseRedo(PTERWND);
int     ReleaseUndo(PTERWND);
int     ScrLineHeight(PTERWND,long,BOOL,BOOL);
int     ScrollUndo(PTERWND);
int     SetParaBorder(PTERWND,long);
int     TerMousePos(PTERWND,DWORD,BOOL);
int     TerSetCursorShape(PTERWND,DWORD,BOOL);
int     SetYOrigin(PTERWND,long);
int     TerWrapWidth(PTERWND,long,int);
int     TerWrapWidth2(PTERWND,long,int,BOOL);
int     UnitsToCol(PTERWND,int,long);
long    AddCrLf(PTERWND,long,BYTE huge *,WORD huge *, BYTE huge *,WORD huge *);
long    RowColToAbs(PTERWND,long,int);
long    UnitsToLine(PTERWND,int,int);
long    UnitsToLine2(PTERWND,int,int,int);
LPWORD  GetLineCharWidth(PTERWND,long);
LRESULT SendMessageToParent(PTERWND,UINT,WPARAM,LPARAM,BOOL);
struct  StrLineSeg far *GetLineSeg(PTERWND,long,LPWORD,LPINT);
struct  StrLineSeg far *GetLineSeg2(PTERWND,long,LPWORD,LPINT,int,int,LPBYTE,LPWORD);
void    AbsToRowCol(PTERWND,long,long far *,int far *);
void    SaveUndo(PTERWND,long,int,long,int,BYTE);
void    SetTerWindowOrg(PTERWND);


/***************************************************************************
                             FILE: TER4.C
****************************************************************************/
BOOL    MouseStopAction(PTERWND);
BOOL    SetMouseStopTimer(PTERWND,LPARAM);
BOOL    InvokeTextLink(PTERWND,BOOL,long,int);
BOOL    DisableIme(PTERWND,BOOL);
BOOL    DocFitsInWindow(PTERWND);
BOOL    DoPopupSelection(PTERWND,int);
BOOL    FieldFound(PTERWND,long,int,LPBYTE,BOOL);
BOOL    GetCaretXY(PTERWND,int,int,int,LPINT,LPINT);
BOOL    GetFieldLoc(PTERWND,long,int,BOOL,LPLONG,LPINT);
BOOL    GetFieldScope(PTERWND,long,int,int,LPLONG,LPINT,LPLONG,LPINT);
BOOL    GetDateString(PTERWND,LPBYTE,LPBYTE,int);
BOOL    GetDateStringAlt(PTERWND,LPBYTE,LPBYTE);
BOOL    HtmlAddOnFound(PTERWND);
BOOL    ImeEndString(PTERWND);
BOOL    ImeStartString(PTERWND);
BOOL    IsDefLangRtl(PTERWND);
BOOL    IsHtmlList(PTERWND,int);
BOOL    IsHtmlRule(PTERWND,int);
BOOL    IsImeMessage(PTERWND,UINT,WPARAM,LPARAM);
BOOL    IsTextPosVisible(PTERWND,long,int);
BOOL    NextTextPos(PTERWND,LPLONG,LPINT);
BOOL    ParseDateToken(PTERWND,LPBYTE,LPINT,LPBYTE,LPBYTE);
BOOL    PrepForObject(PTERWND);
BOOL    PrevTextPos(PTERWND,LPLONG,LPINT);
BOOL    ReplaceTextInPlace(PTERWND,LPLONG,LPINT,int,LPBYTE,LPBYTE);
BOOL    ResetImeStyle(PTERWND,BOOL);
BOOL    OurSetCaretPos(PTERWND);
BOOL    TerSetClipRgn(PTERWND);
BYTE    GetShadedColorComp(PTERWND,BYTE,BYTE,int);
COLORREF GetShadedColor(PTERWND,COLORREF,COLORREF,int);
int     ActivateWheel(PTERWND,LPARAM);
int     CrackAutoNumLgl(PTERWND,LPBYTE,LPBYTE,LPINT);
int     CrackUnicode(PTERWND,WORD,LPWORD,LPBYTE);
int     CrackUnicodeString(PTERWND,LPWORD,LPWORD,LPBYTE);
int     CreateDefList(PTERWND,LPBYTE);
int     CreateDefListOr(PTERWND,int,int,BOOL,int);
int     DispMetaRecName(UINT);
int     GetFieldList(PTERWND,int,long,LPINT,LPINT);
int     GetHeadingNo(PTERWND,int);
int     GetObjSpcBef(PTERWND,long,BOOL);
int     FixPos(PTERWND,LPLONG,LPINT);
int     FreeHtmlAddOn(PTERWND);
int     HtmlListLevel(PTERWND,int);
int     InsertMarkerLine(PTERWND,long,BYTE,int,int,UINT,int);
int     LinePointsToRect(PTERWND,int,int,int,int,int);
int     LineRectToPoints(PTERWND,int,LPINT,LPINT,LPINT,LPINT);
int     LoadHtmlAddOn(PTERWND);
int     MakeAutoNumLgl(PTERWND,LPBYTE,LPBYTE,int,int,int);
int     ResetWheel(PTERWND);
int     SetAutoNumLgl(PTERWND,long,BOOL);
int     SetListnum(PTERWND,long,BOOL);
int     SetPageFromY(PTERWND,int);
int     SetWheelTimer(PTERWND,LPARAM,BOOL);
int     TerLButtonDown(PTERWND,BOOL,UINT,WPARAM,LPARAM);
long    GetFieldSwitchLong(PTERWND,LPBYTE,LPBYTE,long,LPINT);
long    GetStatusLineNo(PTERWND);
long    GetStatusLineNoAdj(PTERWND,long,long);
void    GetSystemDateTime(PTERWND,SYSTEMTIME far *);
WORD    MakeUnicode(PTERWND,WORD,BYTE);

/***************************************************************************
                             FILE: TER_BAR.C
****************************************************************************/
BOOL CALLBACK _export BarComboSubclass(HWND,UINT,WPARAM,LPARAM);
BOOL CheckToolbarIcon(PTERWND,HDC,int,int,int,int);
BOOL ColorToolbarIcon(PTERWND,HDC,int,int,int,int);
BOOL CreateToolBar(PTERWND);
BOOL DestroyToolBar(PTERWND);
BOOL EnableToolbarIcons(PTERWND,BOOL);
BOOL EraseBalloon(PTERWND);
BOOL FreeToolbar(PTERWND);
BOOL GrayToolbarIcon(PTERWND,HDC,int,int,int,int);
BOOL InitToolbar(PTERWND);
BOOL InvalidateToolbarComboItems(HWND);
BOOL CALLBACK _export ToolBarProc(HWND,UINT,WPARAM,LPARAM);
BOOL PaintBalloon(PTERWND);
BOOL PaintToolbar(HWND,HDC);
BOOL PaintToolbarIcon(PTERWND,HDC,HDC,struct StrTlb far *);
BOOL ToggleToolBar(PTERWND);
BOOL TlbLButtonDown(HWND,LPARAM);
BOOL TlbMouseMove(HWND,LPARAM);
BOOL TlbTimer(HWND,int);
BOOL UpdateToolBar(PTERWND,BOOL);
int  PosToolbarIcon(PTERWND,int,int,int);
LRESULT CALLBACK _export TlbWndProc(HWND,UINT,WPARAM,LPARAM);
HWND GetComboEditWindow(HWND,int);
struct StrTlb far *TlbMousePos(PTERWND,LPARAM);

/***************************************************************************
                             FILE: TER_BLK.C
****************************************************************************/
int     OleDragText(PTERWND);
BOOL    ApplyPrevPictProp(PTERWND,int,struct StrFont *);
BOOL    AdjustHtmlPictWidth(PTERWND);
BOOL    AdjustHtmlRulerWidth(PTERWND);
BOOL    AllSelected(PTERWND);
BOOL    AllSelected2(PTERWND,BOOL *);
BOOL    IsControl(PTERWND,int);
BOOL    IsFramePict(PTERWND,int);
int     BlockHasProtectOn(PTERWND,BOOL,BOOL);
BOOL    IsFirstAnimPict(PTERWND,int);
BOOL    IsProtected(PTERWND,BOOL,BOOL);
HWND    RealizeControl(PTERWND,int);
int     BlockCopy(PTERWND);
int     BlockDelete(PTERWND,BOOL);
int     BlockMove(PTERWND);
int     CreateAnimInfo(PTERWND,int);
int     CopyCharBlock(PTERWND,int);
int     CopyFromClipboard(PTERWND,UINT);
int     CopyLineBlock(PTERWND,BOOL);
int     CopyLineToClipBuf(PTERWND,long,int,int,BYTE huge *,WORD huge *,LPDWORD);
int     CopyToClipboard(PTERWND,WPARAM,BOOL);
int     DeleteAnimInfo(PTERWND,int);
int     DeleteCharBlock(PTERWND,BOOL,BOOL);
int     DeleteLineBlock(PTERWND,BOOL);
int     DragText(PTERWND,long);
int     EditPicture(PTERWND);
int     GetPictFileType(PTERWND,LPBYTE,BOOL);
int     HideControl(PTERWND,int);
int     NextBufferLine(PTERWND,struct StructTextBuf far *);
int     NormalizeBlock(PTERWND);
int     NormalizeForFootnote(PTERWND);
int     SetAnimTimer(PTERWND,int);
int     SetDragCaret(PTERWND,long);
int     SetTextInputFieldWnd(PTERWND,int,int,BOOL);
int     TerCreateMetafile(PTERWND,HGLOBAL,int);
LPBYTE  FileToMemPtr(LPBYTE,DWORD far *);
void    InsertBuffer(PTERWND,BYTE huge *,LPWORD,BYTE huge *,WORD huge *,int huge *,BOOL);

#if defined(WIN32)
BOOL    CalcEnhMetafileDim(PTERWND,int,LPENHMETAHEADER,LPINT,LPINT);
int     TerCreateEnhMetafile(PTERWND,HENHMETAFILE,int);
#endif

/***************************************************************************
                             FILE: TER_BLK1.C
****************************************************************************/
int     UndoTableAttrib(PTERWND, int);
BOOL    EditingInputField(PTERWND, BOOL, BOOL);
BOOL    TabOnControl(PTERWND,int,int);
BOOL    FindTextInputField(PTERWND,int);
BOOL    SelectTextInputField(PTERWND,BOOL);
BOOL    SelectTextInputField2(PTERWND,long,int,BOOL);
LRESULT CALLBACK _export FormFieldSubclass(HWND,UINT,WPARAM,LPARAM);
BOOL    CheckImageMapHit(PTERWND,int);
BOOL    IsFormField(PTERWND,int,int);
HBRUSH  SetControlColor(PTERWND,HDC,HWND);
int     CheckWindowOverflow(PTERWND);
int     EditInputField(PTERWND);
int     FreeImageMapTable(PTERWND);
int     GetTextRange(PTERWND,LPLONG,LPINT,LPLONG,LPINT);
int     SelectFormField(PTERWND,int);
int     SelectFirstFormField(PTERWND);
int     TerUndo(PTERWND,BOOL);
long    GetDocHeight(PTERWND);

/***************************************************************************
                             FILE: TER_DLG.C
****************************************************************************/
int     CALLBACK _export CellRotationParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export OverrideBinParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export BookmarkParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export CellBorderParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export CellBorderColorParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export CellColorParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export CellShadingParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export CellVertAlignParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export CellWidthFlagParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export CellWidthParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export CharSpaceParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export CheckBoxFieldParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ClearTabParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export CtlDlgFn(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export DataFieldParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export DateParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export DeleteCellsParam(HWND,UINT,WPARAM,LPARAM);
COLORREF                 DlgEditColor(PTERWND,HWND,COLORREF,BOOL);
int                      DlgEditFont(PTERWND,HWND);
int                      DlgEditListFont(PTERWND,HWND,int);
int                      DlgListCharAft(PTERWND,HWND,int,int,BOOL);
int                      DlgListNumType(PTERWND,HWND,int,int,BOOL);
int                      DlgSetTabAttrib(PTERWND,HWND,int,int);
int     CALLBACK _export DrawObjectParam(HWND,UINT,WPARAM,LPARAM);
int     SetGuiFont(PTERWND,HWND,int);
BOOL    CALLBACK _export EditInputFieldParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export EditPictureParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export EditStyleParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export FileParam(HWND,UINT,WPARAM,LPARAM);
int                      FillListBox(PTERWND,HWND,int,BOOL,BOOL,int);
int                      FillListOrBox(PTERWND,HWND,int,BOOL,BOOL,int,BOOL);
int                      FillStyleBox(PTERWND,HWND,int,int,BOOL,BOOL);
BOOL    CALLBACK _export FontParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export FootnoteParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export HyperlinkParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export InputFieldParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export InsertObjectParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export JumpParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ListLevelParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ListParaParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ListPropParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ListOrPropParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export ListOrSelectParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export ListSelectParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ObjectAttribParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ObjectPosParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export PageParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ParaBoxParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ParaSpaceParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ParaTextFlowParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export PasteSpecialParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export PrintAbortParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export PrintPreviewProc(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export RangeParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ReplaceParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export RowHeightParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export RowPositionParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export RowTextFlowParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export SearchParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export SectionParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export SelectStyleParam(HWND,UINT,WPARAM,LPARAM);
int                      SetDlgListLevel(PTERWND,HWND);
int                      SetDlgListLevelProp(PTERWND,HWND);
int                      SetDlgListParaLevel(PTERWND,HWND,int);
BOOL    CALLBACK _export SetTabParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export TableParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export TextRotationParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export UndoParam(HWND,UINT,WPARAM,LPARAM);
int     CALLBACK _export ZoomParam(HWND,UINT,WPARAM,LPARAM);
BOOL    CALLBACK _export ZoomSubclass(HWND,UINT,WPARAM,LPARAM);
BOOL    SetDlgColor(WPARAM,LPARAM);
int     SetParaSpaceDlg(PTERWND,HWND,int,int,int);
int     PosDialog(PTERWND,HWND);
struct  StrListLevel far *GetDlgListLevelPtr(PTERWND,HWND);

/***************************************************************************
                             FILE: TER_DRAG.C
****************************************************************************/
BOOL    DeleteDragObjects(PTERWND,int,int);
BOOL    DragApply(PTERWND,int,int,int,int);
BOOL    DragApplyLineSize(PTERWND,int,int);
BOOL    DragCellSize(PTERWND,int);
BOOL    DragFrameMove(PTERWND,int,int);
BOOL    DragPictFrameSize(PTERWND,int,int,int);
BOOL    DragRowIndent(PTERWND,int);
BOOL    DragRowSize(PTERWND,int);
BOOL    DragRulerIndent(PTERWND,int);
BOOL    DragRulerTab(PTERWND,int,int);
BOOL    DrawDragCellLine(PTERWND);
BOOL    DrawDragHotSpots(PTERWND,int);
BOOL    DrawDragFrameRect(PTERWND);
BOOL    DrawDragFrameTabs(PTERWND);
BOOL    DrawDragPictRect(PTERWND);
BOOL    DrawDragRowLine(PTERWND);
BOOL    DrawDragRulerIndent(PTERWND);
BOOL    DrawDragRulerTab(PTERWND,int);
BOOL    ShowFrameDragObjects(PTERWND,int,int);
BOOL    ShowFrameMoveObjects(PTERWND,int,int);
BOOL    ShowPictureDragObjects(PTERWND,int);
BOOL    TerDragObject(PTERWND,DWORD);
int     TerDrawLine(PTERWND,int,int,int,int,HPEN,RECT far *,BOOL);
int     TerDrawPolygon(PTERWND,POINT far *,int,HPEN,HBRUSH,RECT far *,BOOL);
int     GetDragObjectSlot(PTERWND);
int     TerDrawRect(PTERWND,RECT far *,HPEN,BOOL,BOOL);

/***************************************************************************
                             FILE: TER_DRAW.C
****************************************************************************/
int     DrawTrackingLine(PTERWND,HDC,HDC,int,int,int);
BOOL    DrawMouseStopMsg(PTERWND,HDC,LPBYTE);
COLORREF PageColor(PTERWND);
int     DrawSectLineNbr(PTERWND,HDC,HDC,int,int,int,int);
long    GetSectDisplayLine(PTERWND,int);
int     GetFrameTextAngle(PTERWND,int);
BOOL    WriteSpaceSym(PTERWND,HDC,int,int,int,int,int,int);
BOOL    DrawBkPictRect(PTERWND,LPRECT,HDC);
BOOL    DrawDarkDottedLine(PTERWND,HDC,int,int,int,int);
BOOL    DrawDottedLine(PTERWND,HDC,int,int,int,int,COLORREF);
BOOL    DrawHtmlCellBorder(PTERWND,HDC,int,int,int,int,int,LPINT,BOOL);
BOOL    DrawLightDottedLine(PTERWND,HDC,int,int,int,int);
BOOL    DrawPageBorder(PTERWND,HDC,int);
BOOL    DrawPageBorderBox(PTERWND,HDC,int);
BOOL    DrawPageBox(PTERWND,HDC,int);
BOOL    DrawShadowBox(PTERWND,HDC,int,int,int,int,HPEN,HPEN);
BOOL    DrawShadowLine(PTERWND,HDC,int,int,int,int,HPEN,HPEN);
BOOL    DrawWigglyLine(PTERWND,HDC,int,int,int,int,COLORREF);
BOOL    HScrollAllowed(PTERWND);
BOOL    InRotatedFrame(PTERWND,int,int,int);
BOOL    OurLineTo(PTERWND,HDC,int,int);
BOOL    OurMoveToEx(PTERWND,HDC,int,int,LPPOINT);
BOOL    TextSegmentExists(PTERWND,int,int,RECT far *,int,LPBYTE);
COLORREF AdjustColor(PTERWND,COLORREF,BOOL);
HPALETTE TerSetPalette(PTERWND,HDC,HPALETTE,int,COLORREF far *,BOOL);
HPEN    OurCreatePen(PTERWND,int,COLORREF,LPINT);
int     AdjustHilight(PTERWND);
int     BkPictOut(PTERWND,HDC,int,int);
int     ClearEOL(PTERWND,HDC,int,int,int,int,BOOL);
int     DeleteTextMap(PTERWND,BOOL);
int     DeleteTextSeg(PTERWND,int,BOOL);
int     DisplayStatus(PTERWND);
int     DisplayStatusInfo(PTERWND);
int     DrawAnimPict(PTERWND,int);
int     DrawBullet(PTERWND,HDC,HDC,long,int,int,int,int,BOOL);
int     DrawEval(PTERWND);
int     DrawFootnote(PTERWND,HDC,int,BOOL);
int     DrawFrameBorder(PTERWND,HDC,int,int,int,int,int,int);
int     DrawLineObject(PTERWND,HDC,int);
int     DrawOneFootnote(PTERWND,HDC,int,int,long,int,int,BOOL,BOOL);
int     DrawParaBorder(PTERWND,HDC,RECT far *,BOOL);
int     DrawPictFrame(PTERWND,HDC,int);
int     DrawRectObject(PTERWND,HDC,int);
int     DrawWheel(PTERWND,HDC);
int     FrameNoRotateDC(PTERWND,HDC);
int     FrameRotateDC(PTERWND,HDC,int);
int     FrameRotateRect(PTERWND,LPRECT,int);
int     FrameRotateX(PTERWND,int,int,int);
int     FrameRotateY(PTERWND,int,int,int);
int     HilightTableCol(PTERWND,long,BOOL,BOOL);
int     KillHilightTimer(PTERWND);
int     NormalizeRect(PTERWND,LPRECT);
int     OurBitBlt(PTERWND,HDC,int,int,int,int,HDC,int,int,DWORD);
int     OurExtTextOut(PTERWND,HDC,int,int,UINT,LPRECT,LPBYTE,UINT,LPINT);
int     OurExtTextOutW(PTERWND,HDC,int,int,UINT,LPRECT,LPWORD,UINT,LPINT);
int     OurStretchDIBits(PTERWND,HDC,int,int,int,int,int,int,int,int,LPVOID,LPBITMAPINFO,UINT,DWORD);
int     PaintBkPict(PTERWND,HDC);
int     RedrawNonText(PTERWND);
int     RtlX(PTERWND,int,int,int,struct StrLineSeg far *);
int     RtlRect(PTERWND,LPRECT,int,struct StrLineSeg far *);
int     SetFont(PTERWND,HDC,WORD,BYTE);
int     SetColor(PTERWND,HDC,BYTE,WORD);
int     SetScrollBars(PTERWND);
int     SyncHilight(PTERWND);
int     TerDrawBorder(PTERWND);
int     TerSetHilight(PTERWND,WPARAM,DWORD,BOOL);
int     TerSetLineHilight(PTERWND);
int     TerSetCharHilight(PTERWND);
int     VerThumbPos(PTERWND,WPARAM,LPARAM);
int     WriteBreakLine(PTERWND,HDC,LPBYTE,int,int,int,HPEN);
int     WriteFrameSpace(PTERWND,HDC,long,int,int);
int     WriteSpace(PTERWND,HDC,long,int,int,int,int);
int     WriteSpaceRect(PTERWND,HDC,RECT far *);
int     WriteTab(PTERWND,HDC,long,int,int,int,int,int);
void    DrawRuler(PTERWND,BOOL);
void    OldDrawRuler(PTERWND,BOOL);
void    HorScrollCheck(PTERWND);

/***************************************************************************
                             FILE: TER_DRG.C
****************************************************************************/
BOOL    DoDragonMsg(PTERWND,HWND,UINT,WPARAM,LPARAM,LRESULT *);
void    DrgAbsToRowCol(PTERWND,long,long far *,int far *);
long    DrgRowColToAbs(PTERWND,long,int);


/***************************************************************************
                             FILE: TER_FMT.C
****************************************************************************/
int     DoFixStrForm(PTERWND,LPVOID);
BOOL    AdjustFontHeight(PTERWND,int,LPBYTE,int,BOOL);
BOOL    ConvertibleToTrueType(PTERWND,LPBYTE,LPBYTE);
BOOL    FreeFontResources(PTERWND,BOOL);
BOOL    IsSameField(PTERWND,int,int);
BOOL    IsSameFieldCode(PTERWND, LPBYTE,LPBYTE);
BOOL    ReadCfmt(PTERWND,LPBYTE,BYTE huge *,long,BYTE);
BOOL    SaveCfmt(PTERWND,LPBYTE,BYTE huge *,long,BYTE);
BYTE    GetCurChar(PTERWND,long,int);
BYTE    GetCurLead(PTERWND,long,int);
WORD    GetNewStyle(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    SetCharAuxId(PTERWND,WORD,DWORD,DWORD,long,int);
int     CreateGlbFont(PTERWND,LPLOGFONT,HDC);
int     CreateOneFont(PTERWND,HDC,int,BOOL);
int     ExpandFontTable(PTERWND,int);
int     FindOpenSlot(PTERWND);
int     GetCurCfmt(PTERWND,long,int);
int     GetNextCfmt(PTERWND,long,int);
int     GetPrevCfmt(PTERWND,long,int);
int     GetNewFont(PTERWND,HDC,int,LPBYTE,int,UINT,COLORREF,COLORREF,COLORREF,int,int,int,int,int,int,UINT,int,LPBYTE,int,int,BYTE,UINT,int);
int     GetNewFont2(PTERWND,HDC,int,struct StrFont *);
int     GetTextFont(PTERWND,int);
int     RecreateFonts(PTERWND,HDC);
int     ReleaseFonts(PTERWND);
int     SetPointSizes(PTERWND,HWND,LPBYTE,int);
int     TerReadRtfInfo(PTERWND,BYTE,HANDLE,BYTE huge *,long far *);
int     TerWriteRtfInfo(PTERWND,int,BOOL,BYTE,HANDLE,BYTE huge *,long far *);
int     TypeFaceChar(PTERWND,LPBYTE,int,BOOL);
int     TypeFaceLine(PTERWND,LPBYTE,int,BOOL);

/***************************************************************************
                             FILE: TER_FMT1.C
****************************************************************************/
int     SetNextStyle(PTERWND);
int     ForceCharStyle(PTERWND,int);
WORD    ApplyParaStyleOnFont(PTERWND,int,int);
BOOL    CharFmt(PTERWND,WORD (*GetNewFontId)(PTERWND,WORD,DWORD,DWORD,long,int), DWORD,DWORD,BOOL);
WORD    GetNewCharStyle(PTERWND,WORD,DWORD,DWORD,long,int);
int     ApplyCharStyles(PTERWND,int);
int     ApplyLineTextStyle(PTERWND,long,int,int);
int     ApplyParaStyles(PTERWND,int);
int     ApplyZoomPercent(PTERWND,int);
int     ChangeLineTextStyle(PTERWND,long,int);
int     CharFmtChr(PTERWND,WORD (*GetNewFontId)(PTERWND,WORD,DWORD,DWORD,long,int),DWORD,DWORD,BOOL);
int     CharFmtLine(PTERWND,WORD (*GetNewFontId)(PTERWND,WORD,DWORD,DWORD,long,int),DWORD,DWORD,BOOL);
int     CopyCharStyle(PTERWND,int,int);
int     DoRulerClick(PTERWND,UINT,DWORD);
int     GetStyleIdSlot(PTERWND);
int     RemoveDeletedObjects(PTERWND);
int     SetCharStyleId(PTERWND,struct StrFont far *,struct StrStyleId far *,struct StrStyleId far *,BOOL);
int     SetParaStyleId(PTERWND,struct StrPfmt far *,struct StrStyleId far *,struct StrStyleId far *,BOOL);
int     TransferFontId(PTERWND,BOOL,int,struct StrFont far *,struct StrPrtFont far *);
int     XlateSizeForPrt(PTERWND,int);

/***************************************************************************
                             FILE: TER_FMT2.C
****************************************************************************/
int     SetFontCharId(PTERWND, int,int);
int     PosTagLine(PTERWND,int,long);
BOOL    PosTagQuick(PTERWND,int, LPBYTE,int,BOOL);
BOOL    JumpToPageRefBookmark(PTERWND,BOOL);
BOOL    ExpandSectArray(PTERWND,int);
BOOL    GetHdrFtrRange(PTERWND,BYTE,long,LPLONG,LPLONG);
WORD    GetNewPointSize(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewTypeFace(PTERWND,WORD,DWORD,DWORD,long,int);
BOOL    IsDynField(PTERWND,int);
BOOL    IsListLine(PTERWND,long);
BOOL    IsMbcsCharSet(PTERWND,int,LPINT);
BOOL    IsEnglishChar(PTERWND,LPBYTE,int,int);
BOOL    IsLineRtl(PTERWND,long);
BOOL    IsParaRtl(PTERWND,int,int,int,int);
BOOL    IsSameListLevel(PTERWND,struct StrListLevel far *,struct StrListLevel far *);
BOOL    IsSameListOr(PTERWND,struct StrListOr far *,struct StrListOr far *);
BOOL    IsValidBookmark(PTERWND,LPBYTE,BOOL);
BOOL    IsValidInputFont(PTERWND,LPINT,int,long,int);
BOOL    MultiOrient(PTERWND);
BOOL    PosTag(PTERWND,int,LPBYTE,int,int,BOOL);
BOOL    SetFontTempStyle(PTERWND,UINT,BOOL,BOOL);
BOOL    TerColors(PTERWND,BOOL);
BOOL    TerGetCharWidth(PTERWND,HDC,int,BOOL,long,BYTE);
BOOL    ForceSectOrient(PTERWND);
BYTE    GetCharSet(PTERWND,HDC,LPBYTE,BYTE far *, BOOL far *, BOOL far *);
DWORD   GetHdrFtrFlag(PTERWND,long);
int     CALLBACK _export TerEnumFonts(LPLOGFONT,LPTEXTMETRIC,int,LPBYTE);
int     CALLBACK _export TerEnumFontFamiliesEx(LPENUMLOGFONTEX,NEWTEXTMETRICEX far *,DWORD,LPARAM);
int     AdjustSections(PTERWND,long,long);
int     BuildMbcsCharWidth(PTERWND,LPINT,int,LPBYTE,int,LPBYTE,LPBYTE,int,LPWORD);
int     CloseTextPtr(PTERWND,long);
int     CopyHeaderFooter(PTERWND,int,int,BYTE);
int     CreatePageHdrFtr(PTERWND,BYTE,int);
int     InitCharWidthCache(PTERWND);
int     InsertBookmark(PTERWND);
int     InsertHyperlink(PTERWND);
int     ChangeLetterCase(PTERWND,long,int,int,BOOL,LPDWORD);
int     CharWidthAlloc(PTERWND,long,int,int);
int     CtidAlloc(PTERWND,long,int,int);
int     DeleteTag(PTERWND,long,int,int,LPBYTE);
int     DeleteTerObject(PTERWND,int);
int     EnableMbcs(PTERWND,int);
int     FmtAlloc(PTERWND,long,int,int);
int     FreeList(PTERWND,int);
int     FreeListOr(PTERWND,int);
int     FreeListTable(PTERWND);
int     FreeTag(PTERWND,int);
int     GetEffectiveCfmt(PTERWND);
int     GetListOrSlot(PTERWND);
int     GetListSlot(PTERWND);
int     GetSection(PTERWND,long);
int     GetTag(PTERWND,long,int,int,LPBYTE,LPBYTE,LPLONG);
int     GetTagSlot(PTERWND);
int     DblCharWidth(PTERWND,int,BOOL,BYTE,BYTE);
int     InitTerObject(PTERWND,int);
int     LeadAlloc(PTERWND,long,int,int);
int     LineTextAngle(PTERWND,int);
int     OpenCharInfo(PTERWND,long,LPWORD far *,LPWORD far *);
int     OpenTextPtr(PTERWND,long,LPBYTE far *,LPBYTE far *);
int     RecreateSections(PTERWND);
int     ResetUniFont(PTERWND,int);
int     SetCurLang(PTERWND,HKL);
int     SetCurLangFont(PTERWND, int);
int     SetCurLangFont2(PTERWND, int,HKL);
int     SetFnoteFontInfo(PTERWND,BOOL);
int     SetFontFieldId(PTERWND,int,int,LPBYTE);
int     SetFontFlags(PTERWND,int,UINT,BOOL);
int     SetFontStyle(PTERWND,int,UINT,BOOL);
int     SetFontStyleId(PTERWND,int,int,int);
int     SetFontTextAngle(PTERWND,int,int);
int     SetScapFont(PTERWND,int,BOOL);
int     SetSectPageSize(PTERWND);
int     SetTag(PTERWND,long,int,int,LPBYTE,LPBYTE,long);
int     SetUniFont(PTERWND,int,WORD);
int     TerFonts(PTERWND);
int     TerGetCharWidthAlt(PTERWND,HDC,int,BYTE,int);
int     TerGetCharWidthAltW(PTERWND,HDC,int,WORD,int);
int     TransferTags(PTERWND,long,int);
int     UpdateTagTable(PTERWND);
long    GetFirstSectLine(PTERWND,int);
LPBYTE  OpenLead(PTERWND,long);
LPWORD  OpenCfmt(PTERWND,long);
LPWORD  OpenCtid(PTERWND,long);
void    CloseLead(PTERWND,long);
WORD    GetNewBkColor(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewCharSet(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewColor(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewUlineColor(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewExpand(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewCharOffset(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewFieldId(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewLang(PTERWND,WORD,DWORD,DWORD,long,int);
WORD    GetNewTempStyle(PTERWND,WORD,DWORD,DWORD,long,int);
void    CloseCfmt(PTERWND,long);
void    CloseCharInfo(PTERWND,long);
void    CloseCtid(PTERWND,long);
void    CompressCfmt(PTERWND,long);
void    CompressCtid(PTERWND,long);
void    ExpandCfmt(PTERWND,long);
void    ExpandCtid(PTERWND,long);

/***************************************************************************
                             FILE: TER_FRM.C
****************************************************************************/
int     CreateWatermarkFrame(PTERWND,int,int,LPINT,int,int,int,int,int);
int     ApplyParentCellLimitY(PTERWND,int,int,int,BOOL far *);
int     CreatePageBorderFrames(PTERWND,int,LPINT,LPINT,LPINT,LPINT,LPINT);
int     MapRtlCell(PTERWND,int);
int     MapRtlCol(PTERWND,LPINT,LPINT,LPINT,int,int,int);
int     SetBoxFrameHt(PTERWND,int,int,int,int);
int     SetFrameLineInfo(PTERWND,long,int,int,LPINT,LPINT,int,int,LPINT,int,int,int,int,int,int,int,LPINT,int,int);
int     SwitchParaFrames(PTERWND,int,PFRAMESET,PFRAMESET,PFRAMESET,PFRAMESET,LPINT,long,UINT,int);
void    CreateFrames(PTERWND,BOOL,int,int);
void    CreateFramesScr(PTERWND);

/***************************************************************************
                             FILE: TER_FRM1.C
****************************************************************************/
BOOL    AnchorParaFound(PTERWND,long,int);
BOOL    AnchorPictFrame(PTERWND,int,long,int);
BOOL    InitFrame(PTERWND,int);
int     CalcFrameSpace(PTERWND,long,RECT far *,int far *,int far *,int far *,int,BOOL,BOOL,BOOL,int);
int     CalcFrmSpcBef(PTERWND,long,int,BOOL,int);
int     CalcFrmSpcBefRow(PTERWND,long,int);
int     CalcFrmIndentBefRow(PTERWND,long,int);
int     CreateFnoteFrame(PTERWND,int,LPINT,LPINT,int,int,int);
int     CreatePageBorderBot(PTERWND,int,LPINT,LPINT,int,int);
int     CreatePageBox(PTERWND,int,int,int,int,int);
int     CreatePictFrames(PTERWND,int,int,int,int,int,int,long,long,UINT);
int     DeleteFrame(PTERWND);
int     GetAnchorY(PTERWND,long);
int     GetBorderCell(PTERWND,int,int,BOOL);
int     GetCellFrameLeftWidth(PTERWND,int,UINT far *, COLORREF far *);
int     GetCellFrameRightWidth(PTERWND,int,UINT far *, COLORREF far *);
int     GetCellFrameTopWidth(PTERWND,int,UINT far *,int, COLORREF far *);
int     GetCellFrameBotWidth(PTERWND,int,UINT far *,int, COLORREF far *);
int     GetFrameSpace(PTERWND,long,RECT far *,int far *,int far *,int far *);
int     GetFrmSpcBef(PTERWND,long,BOOL);
int     GetParaFrameSlot(PTERWND);
int     FitPictureInFrame(PTERWND,long,BOOL);
int     FrameEmptyCells(PTERWND,int,LPINT,int,LPINT,int,int,LPINT,int,int);
int     InsertParaFrame(PTERWND,int,int,int,int,BOOL,UINT,BOOL);
int     ParaIdForFrame(PTERWND,int,UINT);
int     PosPictFrames(PTERWND,long,int,int,int,int,int,int,int);
int     RefreshFrames(PTERWND,BOOL);
int     ReposPictFrames(PTERWND);
int     SetParaFrameY(PTERWND,int,int,int,int,int,int,int,long);
long    GetParaFrameLine(PTERWND,long);
UINT    GetCellFrameBorder(PTERWND,int,LPINT,LPINT,LPINT,LPINT,int,COLORREF far *);

/***************************************************************************
                             FILE: TER_FRM2.C
****************************************************************************/
int     BlankHdrFrameHeight(PTERWND,long,long,int);
int     CreateCellFrame(PTERWND,int,LPINT,int,int,LPINT,LPINT,int,int,LPINT,LPINT,LPINT,LPINT,int,int,int,int,int,int,LPINT,long,long,UINT,int,LPINT,int,int,int,int,int,int,int,int);
int     SortFrames(PTERWND);
long    CreateSubTableFrames(PTERWND,long,int,LPINT,LPINT,int,int,int,int,int,int,int,int,int,int,int,LPINT,int,int,int);

/***************************************************************************
                             FILE: TER_IMG.C
****************************************************************************/
BOOL    Gif2Bmp(PTERWND,int,LPBYTE,HGLOBAL,long,HGLOBAL far *,struct StrGifAnim far *);
BOOL    GifGetBytes(PTERWND, struct StrGif far *,LPVOID,int);
BOOL    GifGetChar(PTERWND, struct StrGif far *);
BOOL    GifGetWord(PTERWND, struct StrGif far *);
BOOL    GifPushChar(PTERWND, struct StrGif far *);
BOOL    GifReadExtension(PTERWND, struct StrGif far *);
BOOL    GifReadHdr(PTERWND, struct StrGif far *);
BOOL    GifReadImageGroup(PTERWND, struct StrGif far *);
BOOL    GifReadScrDesc(PTERWND, struct StrGif far *);
BOOL    GifData(struct StrLz far *);

BOOL    InitTifHuf();
BOOL    SetTifHuf(int,int,BYTE,BYTE,BYTE,BYTE);
BOOL    Tif2Bmp(PTERWND,int,LPBYTE,HGLOBAL,long, HGLOBAL far *,BOOL);
BOOL    TifGetBytes(PTERWND,struct StrTif far *,LPVOID,long);
BOOL    TifGetChar(PTERWND,struct StrTif far *);
BOOL    TifGetWord(PTERWND,struct StrTif far *);
BOOL    TifHufDecompress(PTERWND,struct StrTif far *,struct StrLz far *,LPBYTE,int);
BOOL    TifPushChar(PTERWND,struct StrTif far *);
DWORD   TifReadHdr(PTERWND,struct StrTif far *);
BOOL    TifReadStrips(PTERWND,struct StrTif far *);
BOOL    TifGetOffsetBytes(PTERWND,struct StrTif far *,DWORD,LPVOID,long);
BOOL    TifData(struct StrLz far *);
WORD    ToIntelInt(WORD);
DWORD   ToIntelLong(DWORD);

BOOL    LzAddChar(struct StrLz far *,int,int);
BOOL    LzExit(struct StrLz far *);
BOOL    LzExtractString(struct StrLz far *,int);
BOOL    LzDecompress(struct StrLz far *,LPBYTE,int);
BOOL    LzPlayString(struct StrLz far *,int);
BOOL    LzInit(PTERWND w, struct StrLz far *);
BOOL    LzInitMap(struct StrLz far *);
BOOL    LzInsertMap(struct StrLz far *,BYTE);
BOOL    LzLookupPair(struct StrLz far *,BYTE);
BOOL    LzReadCode(struct StrLz far *,LPINT);
BOOL    LzWrite(struct StrLz far *,int);


BOOL    IsSspAvail(void);

BOOL    PngReadRes(PTERWND,struct StrPng far *,DWORD);
BOOL    Png2Bmp(PTERWND,int,LPBYTE,HGLOBAL,long,HGLOBAL far *);
BOOL    PngApplyPalAlpha(PTERWND,struct StrPng far *);
BOOL    PngCollectData(PTERWND,struct StrPng far *,DWORD);
BOOL    PngData(PTERWND w, struct StrPng far *, BYTE);
BOOL    PngGetBytes(PTERWND,struct StrPng far *,void huge *,DWORD);
BOOL    PngGetWord(PTERWND,struct StrPng far *);
BOOL    PngGetChar(PTERWND,struct StrPng far *);
BOOL    PngInitVars(PTERWND,struct StrPng far *);
BOOL    PngProcessData(PTERWND,struct StrPng far *);
BOOL    PngPushChar(PTERWND,struct StrPng far *);
BOOL    PngReadHdr(PTERWND,struct StrPng far *,BOOL);
BOOL    PngReadPalette(PTERWND,struct StrPng far *,DWORD);
BOOL    PngXlateDWord(LPLONG);
BOOL    PngReadBknd(PTERWND,struct StrPng far *,DWORD);
BOOL    PngReadXparent(PTERWND,struct StrPng far *,DWORD);
int     PngAllocHuffTbl(PTERWND,struct StrPng far *,struct StrPngHuffTbl far *,int);
int     PngCreateHuffCode(PTERWND,struct StrPng far *,struct StrPngHuffTbl far *);
int     PngCreateFixedHuffTable(PTERWND,struct StrPng far *,struct StrPngHuffTbl far *, struct StrPngHuffTbl far *);
int     PngExtractHuffSym(PTERWND,struct StrPng far *,struct StrPngHuffTbl far *);
int     PngFreeHuffTbl(PTERWND,struct StrPng far *,struct StrPngHuffTbl far *);
int     PngGetBits(PTERWND,struct StrPng far *, int);
int     PngPushBits(PTERWND,struct StrPng far *,int);
int     PngReadHuffTables(PTERWND,struct StrPng far *,struct StrPngHuffTbl far *,struct StrPngHuffTbl far *);
int     PngReverseBits(PTERWND,struct StrPng far *,int,int);

BOOL    PngFlushBytes(PTERWND,struct StrPng far *);
BOOL    PngPutBits(PTERWND,struct StrPng far *,int,DWORD);
BOOL    PngPutByte(PTERWND,struct StrPng far *,BYTE);
BOOL    PngPutBytes(PTERWND,struct StrPng far *,LPBYTE,int);
BOOL    PngBeginChunk(PTERWND,struct StrPng far *,LPBYTE,DWORD);
BOOL    PngEndChunk(PTERWND,struct StrPng far *);
BOOL    PngExtractBitmap(PTERWND,struct StrPng far *);
BOOL    PngPutDWord(PTERWND,struct StrPng far *,DWORD);
BOOL    PngScrollCircBuf(PTERWND,struct StrPng far *);
BOOL    PngSetFilePos(PTERWND,struct StrPng far *,long);
BOOL    PngUpdateChecksum(PTERWND,struct StrPng far *,BYTE);
BOOL    PngWriteDataByte(PTERWND,struct StrPng far *,BYTE, struct StrPngHuffTbl far *,struct StrPngHuffTbl far *);
BOOL    PngWriteHdr(PTERWND,struct StrPng far *);
BOOL    PngWriteHuffCode(PTERWND,struct StrPng far *,WORD sym,struct StrPngHuffTbl far *);
BOOL    PngWriteImage(PTERWND,struct StrPng far *);
BOOL    PngWritePal(PTERWND,struct StrPng far *);
BOOL    PngWriteRun(PTERWND,struct StrPng far *,struct StrPngHuffTbl far *,struct StrPngHuffTbl far *);
BOOL    PngWriteScan256(PTERWND,struct StrPng far *,int,struct StrPngHuffTbl far *,struct StrPngHuffTbl far *);
BOOL    PngWriteScanRGB(PTERWND,struct StrPng far *,int,struct StrPngHuffTbl far *,struct StrPngHuffTbl far *);

int     InitJpgZTable(void);
int     JpgGetLsbForNonZeroItems(PTERWND,struct StrJpg far *,int,short int far *,int,int);
int     JpgSkip16Zeros(PTERWND,struct StrJpg far *,int,short int far *,int);
int     JpgSkipZeroItems(PTERWND,struct StrJpg far *,int,short int far *,int,int);
int     JpgStuffWidth(PTERWND,struct StrJpg far *,int,int,short int far *);
BOOL    Jpg2Bmp(PTERWND,int,LPBYTE,HGLOBAL,long, HGLOBAL far *,BOOL);
BOOL    JpgCopyMcu(PTERWND,struct StrJpg far *,int,int);
BOOL    JpgFillInBuf(PTERWND,struct StrJpg far *);
BOOL    JpgReadAppSeg(PTERWND,struct StrJpg far *);
BOOL    JpgGetBytes(PTERWND,struct StrJpg far *,LPVOID,long);
BOOL    JpgGetChar(PTERWND,struct StrJpg far *);
BOOL    JpgGetHmSymbol(PTERWND,struct StrJpg far *, struct StrJpgHuffTbl far *);
BOOL    JpgGetHuffTable(PTERWND,struct StrJpg far *);
BOOL    JpgGetImageInfo(PTERWND,struct StrJpg far *);
BOOL    JpgGetImageSymbol(PTERWND,struct StrJpg far *,int,BOOL);
BOOL    JpgGetMarker(PTERWND,struct StrJpg far *,LPBYTE);
BOOL    JpgGetQTable(PTERWND,struct StrJpg far *);
BOOL    JpgGetWord(PTERWND,struct StrJpg far *);
BOOL    JpgPushChar(PTERWND,struct StrJpg far *);
BOOL    JpgReadImage(PTERWND,struct StrJpg far *);
BOOL    JpgReadMcu(PTERWND,struct StrJpg far *);
BOOL    JpgReadSamp(PTERWND,struct StrJpg far *, int);
BOOL    JpgReverseDct(PTERWND,struct StrJpg far *,int);
BOOL    JpgGetOffsetBytes(PTERWND,struct StrJpg far *,DWORD,LPVOID,long);
BOOL    JpgSetPixels(PTERWND,struct StrJpg far *);
BOOL    JpgSkipBytes(PTERWND,struct StrJpg far *,WORD);
BOOL    JpgSkipMarker(PTERWND,struct StrJpg far *);
short   int far *JpgGetBlockPtr(PTERWND,struct StrJpg far *,int,int,int);

/***************************************************************************
                             FILE: TER_INIT.C
****************************************************************************/
BOOL    InitSect(PTERWND,int);
BOOL    InitTer(HWND,CREATESTRUCT far *);
BOOL    InitVariables(PTERWND);
BOOL    InitWinProp(PTERWND);
int     AllocArrayMem(PTERWND);
int     AllocWrapBuf(PTERWND,int);
int     CreateUserParam(struct arg_list far *,CREATESTRUCT far *);
int     EnableVScrollBar(PTERWND,BOOL);
int     InitDevMode(PTERWND,BOOL);
int     InitPrinter(PTERWND);
int     InitTypeface(PTERWND);
int     SaveBackground(PTERWND);
void    AbortTer(PTERWND,LPBYTE,int);

/***************************************************************************
                             FILE: TER_IO.C
****************************************************************************/
int     DispEnhMetaRecType(PTERWND,int);
BOOL    XlateTextInputField(PTERWND);
BOOL    AttachSectionData(PTERWND);
BOOL    GetFileName(PTERWND,BOOL,LPBYTE,int,LPBYTE);
BOOL    InitFileInfo(PTERWND);
BOOL    ReadFileInfo(PTERWND,LPBYTE,BYTE huge *,BYTE,BOOL);
BOOL    TerRead(PTERWND,LPBYTE);
BOOL    TerSave(PTERWND,LPBYTE,BOOL);
BOOL    TerSaveAs(PTERWND,LPBYTE);
BOOL    UnicodeRead(PTERWND,BYTE,LPBYTE,HANDLE,long,BOOL);
long    UnicodeWrite(PTERWND,LPBYTE,HANDLE,long);
int     AddExt(LPBYTE,LPBYTE);
int     DispMsg(UINT);
int     OurReadLine(PTERWND,LPBYTE,int,HANDLE);
int     ReadLine(LPBYTE,int,HANDLE);
int     RecordFontInUse(PTERWND,int);
int     SearchWininet(PTERWND);
int     TerFarRead(PTERWND,BYTE,HANDLE,BYTE huge *,long far *,long,long,BYTE huge *);
int     TerFarWrite(PTERWND,BYTE,HANDLE,BYTE huge *,long far *,long,long,BYTE huge *);
int     TerNew(PTERWND,LPBYTE);
int     TerOpen(PTERWND);
long    CfmtNeeded(PTERWND);
long    GetFileLength(LPSTR);


/***************************************************************************
                             FILE: TER_MISC.C
****************************************************************************/
BOOL    RecordMem(PTERWND,LPVOID,BOOL,LPBYTE);
int     SwapDbls(double *,double *);
DWORD   GetBitVal(DWORD,int,int);
int     strncmpi(LPBYTE,LPBYTE,int);
HINSTANCE TerLoadDll(LPBYTE);
void far *MemAllocCopy(LPBYTE,DWORD);
void far *OurAllocCopy(LPBYTE,DWORD);
BOOL    GetSearchUcBase(PTERWND,LPBYTE,int,LPWORD,LPWORD,LPBYTE,int,LPBYTE,LPWORD);
LPBYTE  StrLwrDBU(PTERWND,LPBYTE,LPBYTE,LPWORD);
LPBYTE  ExtractQuotedText(LPBYTE);
LPBYTE  OrdinalString(LPBYTE,int,BOOL,BOOL);
LPBYTE  OrdinalStringThousands(LPBYTE,int,bool);
LPBYTE  OrdinalString999(LPBYTE,int,bool);
LPBYTE  OrdinalString99(LPBYTE,int, bool);
LPBYTE  OrdinalString20(LPBYTE,int, bool);
LPBYTE  GetStringField(LPBYTE,int,BYTE,LPBYTE);
LPBYTE  Substring(LPBYTE,int,int,LPBYTE);
long    LineToParaNum(PTERWND,long);
long    ParaToLineNum(PTERWND,long);
BOOL    AllocLinePtr(PTERWND,long);
BOOL    AllocTabw(PTERWND,long);
BOOL    AllocTabwCharFlags(PTERWND,long);
BOOL    AssignLongVal(HWND,int,LPLONG);
BOOL    AssignNumVal(HWND,int,LPINT);
BOOL    CellCharIncluded(PTERWND,long,int,long,int,int);
BOOL    CheckDlgValue(HWND,BYTE,int,double,double);
BOOL    CopyTabw(PTERWND,long,long);
BOOL    CurrentPrinter(PTERWND,LPBYTE,LPBYTE,LPBYTE);
BOOL    ExpandArray(PTERWND,int,void far *(huge *),LPINT,int,int);
BOOL    FileExists(LPBYTE);
BOOL    FreeLinePtr(PTERWND,long);
BOOL    FreeTabw(PTERWND,long);
BOOL    FreeTabwMembers(PTERWND,struct StrTabw far *);
BOOL    IsBlankDlg(HWND,int);
BOOL    IsBreakChar(PTERWND,BYTE);
BOOL    IsCaretVisible(PTERWND,long,int);
BOOL    LineEndsInBreak(PTERWND,long);
BOOL    LineInfo(PTERWND,long,UINT);
BOOL    SearchDisplay(PTERWND,LPBYTE,LPWORD,BYTE,long,int,long,int);
BOOL    OpenCurPrinter(PTERWND,BOOL);
BOOL    OpenCurPrinterOrient(PTERWND,int,BOOL);
BOOL    ParaCharIncluded(PTERWND,long,int,long,int);
BOOL    PrintError(PTERWND,int,LPBYTE);
BOOL    RepairHdrFtrDelims(PTERWND,long,long);
BOOL    ReplaceTextString(PTERWND,LPBYTE,LPWORD,long,long);
BOOL    ReposPageHdrFtr(PTERWND,BOOL);
BOOL    SetDlgLong(HWND,int,long);
BOOL    SetHdrFtrLineFlags(PTERWND,long,BYTE);
BOOL    StuffListBox(HWND,int,LPBYTE,DWORD);
BOOL    ToggleEditHdrFtr(PTERWND);
BOOL    ToggleFieldNames(PTERWND);
BOOL    ToggleFootnoteEdit(PTERWND,BOOL);
BOOL    ToggleHiddenText(PTERWND);
BOOL    ToggleViewHdrFtr(PTERWND);
BOOL    ToggleParaMark(PTERWND);
BOOL    ToggleRuler(PTERWND);
BOOL    ToggleStatusRibbon(PTERWND);
BOOL    UseCaret(PTERWND);
BYTE    LeadByte(LPBYTE,int);
double  TerAtof(PTERWND,LPBYTE);
DWORD   GetWinStyle(HWND,DWORD);
DWORD   TerGetLastError(void);
HWND    GetTopParent(PTERWND,HWND);
HDC     CreateGlbPrtDC(PTERWND,LPBYTE,LPBYTE,LPBYTE,LPDEVMODE);
LPBYTE  TerGetMetaFileBits(HMETAFILE,LPLONG);
HMETAFILE TerSetMetaFileBits(LPBYTE);
int     dm(LPBYTE);
int     PageToFrameY(PTERWND,int);
int     SetWinStyle(HWND,BOOL,DWORD);
int     StrLenSplit(PTERWND,LPBYTE);
LPBYTE  AddChar(LPBYTE,BYTE);
LPBYTE  DoubleToStr(PTERWND,double,int,LPBYTE);
LPBYTE  LeadPtr(LPBYTE,int);
LPBYTE  lstrchr(LPBYTE,BYTE);
LPBYTE  lstrlwr(PTERWND,LPBYTE);
LPBYTE  lstrupr(PTERWND,LPBYTE);
LPBYTE  MakeCopy(LPBYTE,int);
LPBYTE  StrLwrDB(PTERWND,LPBYTE,LPBYTE);
LPVOID  sAlloc(UINT);
LPVOID  sReAlloc(LPVOID,UINT);
int     ApplyPrinterSetting(PTERWND,int,int,int,float,float);
int     Bitmap2DIB(PTERWND,HBITMAP);
int     CallDialogBox(PTERWND,LPBYTE,DLGPROC,DWORD);
int     OldCheckEval(void);
BOOL    CheckEval(BOOL,LPBYTE);
int     CheckLineLimit(PTERWND,long);
int     DebugFct(PTERWND);
int     DIB2Bitmap(PTERWND,HDC,int);
int     DisengageCaret(PTERWND);
int     ExtractDIB(PTERWND,HGLOBAL);
int     EngageCaret(PTERWND,int);
int     FrameToMargX(PTERWND,int);
int     FrameToPageY(PTERWND,int);
int     FreeRtfInfo(PTERWND);
int     InitCaret(PTERWND);
int     LogPrintf(LPBYTE,...);
int     MemFree(void far *ptr);
int     OurPrintf(LPBYTE,...);
int     ParseUserString(PTERWND,LPBYTE,LPBYTE);
int     StrQuote(LPBYTE);
int     RoundInt(int,int);
int     SetPictSize(PTERWND,int,int,int,BOOL);
int     sFree(LPVOID);
int     SwapInts(LPINT,LPINT);
int     SwapLongs(LPLONG,LPLONG);
int     TerDestroyCaret(PTERWND);
int     TerJump(PTERWND);
int     TerReplaceString(PTERWND);
int     TerSearchBackward(PTERWND);
int     TerSearchForward(PTERWND);
int     TerSearchString(PTERWND);
int     TerTextExtentX(HDC,LPBYTE,int);
int     ToggleFittedView(PTERWND);
int     TogglePageMode(PTERWND);
HENHMETAFILE TerSetEnhMetaFileBits(LPBYTE);
HMETAFILE DIB2Metafile(PTERWND,int);
LPBYTE  TerGetEnhMetaFileBits(HENHMETAFILE);
long    DateToDays(long);
long    lMulDiv(long,long,long);
long    hstrlen(BYTE huge *);
PTERWND GetWindowPointer(HWND);
void    AddSlashes(PTERWND,LPBYTE,LPBYTE,int);
void    AlphaFormat(LPBYTE,int,BOOL);
void    far *MemAlloc(UINT);
void    far *MemReAlloc(void far *,UINT);
void    FarMemSet(void huge *,BYTE,DWORD);
void    FarMove(void far *,void far *,UINT);
void    FarMoveOl(void far *,void far *,UINT);
void    far *GlbAlloc(DWORD);
void    far *GlbRealloc(void far *,DWORD);
void    far *OurAlloc(DWORD);
void    far *OurRealloc(void far *,DWORD);
void    HugeMove(void huge *,void huge *,long);
void    lstrncpy(LPBYTE,LPBYTE,int);
void    lTrim(LPBYTE);
void    GlbFree(void far *);
void    OurFree(void far *);
void    romanize(LPBYTE,int,BOOL);
void    rTrim(LPBYTE);
void    StripSlashes(PTERWND,LPBYTE,LPBYTE);
void    StrPrepend(LPBYTE,LPBYTE);
void    StrTrim(LPBYTE);
UINT    FarStringIndex(LPBYTE,LPBYTE);
UINT    FarStringRevIndex(LPBYTE,LPBYTE,UINT);
UINT    FarCharIndex(LPBYTE,BYTE,UINT);


/***************************************************************************
                             FILE: TER_OLE.C
****************************************************************************/
#if !defined(NO_OLE)
  #if defined (INC_OLE1)
     BOOL    PrintOleErrorMessage(PTERWND,LPBYTE,OLESTATUS);
     int     CALLBACK _export ClientCallback(LPOLECLIENT,OLE_NOTIFICATION,LPOLEOBJECT);
     int     TerOleWaitForRelease(PTERWND,LPOLEOBJECT);
  #else
     LPDATAOBJECT CreateDataObject(PTERWND);
     LPDROPSOURCE CreateDropSource(PTERWND);
     
     BOOL FixedPresentationData(PTERWND,int);

     BOOL DeactivateEditOle(PTERWND);
     
     BOOL    DropFile(PTERWND,LPBYTE);
     BOOL    TerOlePreCreate(PTERWND,int,LPOLECLIENTSITE far *,LPSTORAGE far *,FORMATETC far *);
     BOOL    TerOlePostCreate(PTERWND,int);
     int     PrintOleError(PTERWND,HRESULT);
     LPOLECLIENTSITE CreateOleClientSite(PTERWND,int,LPSTORAGE);
     LPOLESTR StrToOleStr(LPSTR,LPSTR);
     LPSTR   OleStrToStr(LPSTR,LPSTR,int);
     HRESULT STDMETHODCALLTYPE IocsQueryInterface(LPOLECLIENTSITE,REFIID,LPVOID far *);
     ULONG   STDMETHODCALLTYPE IocsAddRef(LPOLECLIENTSITE);
     ULONG   STDMETHODCALLTYPE IocsRelease(LPOLECLIENTSITE);
     HRESULT STDMETHODCALLTYPE IocsGetContainer(LPOLECLIENTSITE,LPOLECONTAINER far *);
     HRESULT STDMETHODCALLTYPE IocsOnShowWindow(LPOLECLIENTSITE,BOOL);
     HRESULT STDMETHODCALLTYPE IocsGetMoniker(LPOLECLIENTSITE,DWORD,DWORD,LPMONIKER far *);
     HRESULT STDMETHODCALLTYPE IocsRequestNewObjectLayout(LPOLECLIENTSITE);
     HRESULT STDMETHODCALLTYPE IocsSaveObject(LPOLECLIENTSITE);
     HRESULT STDMETHODCALLTYPE IocsShowObject(LPOLECLIENTSITE);

     LPADVISESINK CreateAdviseSink(PTERWND, int, LPSTORAGE, LPOLECLIENTSITE);
     HRESULT STDMETHODCALLTYPE IasQueryInterface(LPADVISESINK,REFIID,LPVOID far *);
     ULONG   STDMETHODCALLTYPE IasAddRef(LPADVISESINK);
     ULONG   STDMETHODCALLTYPE IasRelease(LPADVISESINK);
     void    STDMETHODCALLTYPE IasOnDataChange(LPADVISESINK,FORMATETC far *, STGMEDIUM far *);
     void    STDMETHODCALLTYPE IasOnViewChange(LPADVISESINK,DWORD, LONG);
     void    STDMETHODCALLTYPE IasOnRename(LPADVISESINK,LPMONIKER);
     void    STDMETHODCALLTYPE IasOnSave(LPADVISESINK);
     void    STDMETHODCALLTYPE IasOnClose(LPADVISESINK);

     int     SetOlePictOffset(PTERWND,int);
     BOOL    CALLBACK _export OleEnumMetafile(HDC,HANDLETABLE FAR *,METARECORD FAR *,int,LPARAM);

     void    UpdateOleObject(LPADVISESINK this);
     BOOL    UpdateOleObj(PTERWND,int);

     // activeX support interfaces
     #if defined(WIN32)

        BOOL    SetPropBool(PTERWND,int,DISPID,BOOL);
        BOOL    PumpCtlMessages(PTERWND,int);
        int     TerSetBool(VARIANTARG far *,BOOL);

        LPOLEINPLACESITE CreateInPlaceSite(PTERWND,int,LPOLECLIENTSITE);
        HRESULT STDMETHODCALLTYPE IipsQueryInterface(LPOLEINPLACESITE,REFIID,LPVOID far *);
        ULONG STDMETHODCALLTYPE IipsAddRef(LPOLEINPLACESITE);
        ULONG STDMETHODCALLTYPE IipsRelease(LPOLEINPLACESITE);
        HRESULT STDMETHODCALLTYPE IipsGetWindow(LPOLEINPLACESITE,HWND far *);
        HRESULT STDMETHODCALLTYPE IipsContextSensitiveHelp(LPOLEINPLACESITE,BOOL);
        HRESULT STDMETHODCALLTYPE IipsCanInPlaceActivate(LPOLEINPLACESITE);
        HRESULT STDMETHODCALLTYPE IipsOnInPlaceActivate(LPOLEINPLACESITE);
        HRESULT STDMETHODCALLTYPE IipsOnUIActivate(LPOLEINPLACESITE);
        HRESULT STDMETHODCALLTYPE IipsGetWindowContext(LPOLEINPLACESITE,LPOLEINPLACEFRAME far *,LPOLEINPLACEUIWINDOW FAR *,LPRECT,LPRECT,LPOLEINPLACEFRAMEINFO);
        HRESULT STDMETHODCALLTYPE IipsScroll(LPOLEINPLACESITE,SIZE);
        HRESULT STDMETHODCALLTYPE IipsOnUIDeactivate(LPOLEINPLACESITE,BOOL);
        HRESULT STDMETHODCALLTYPE IipsOnInPlaceDeactivate(LPOLEINPLACESITE);
        HRESULT STDMETHODCALLTYPE IipsDiscardUndoState(LPOLEINPLACESITE);
        HRESULT STDMETHODCALLTYPE IipsDeactivateAndUndo(LPOLEINPLACESITE);
        HRESULT STDMETHODCALLTYPE IipsOnPosRectChange(LPOLEINPLACESITE,LPCRECT);

        LPOLECONTROLSITE CreateOleControlSite(PTERWND,int,LPOLECLIENTSITE);
        HRESULT STDMETHODCALLTYPE IcsQueryInterface(LPOLECONTROLSITE,REFIID,LPVOID far *);
        ULONG STDMETHODCALLTYPE IcsAddRef(LPOLECONTROLSITE);
        ULONG STDMETHODCALLTYPE IcsRelease(LPOLECONTROLSITE);
        HRESULT STDMETHODCALLTYPE IcsOnControlInfoChanged(LPOLECONTROLSITE);
        HRESULT STDMETHODCALLTYPE IcsLockInPlaceActive(LPOLECONTROLSITE,BOOL);
        HRESULT STDMETHODCALLTYPE IcsGetExtendedControl(LPOLECONTROLSITE,IDispatch **);
        HRESULT STDMETHODCALLTYPE IcsTransformCoords(LPOLECONTROLSITE,POINTL *,POINTF *,DWORD);
        HRESULT STDMETHODCALLTYPE IcsTranslateAccelerator(LPOLECONTROLSITE,LPMSG,DWORD);
        HRESULT STDMETHODCALLTYPE IcsOnFocus(LPOLECONTROLSITE,BOOL);
        HRESULT STDMETHODCALLTYPE IcsShowPropertyFrame(LPOLECONTROLSITE);

        LPOLEINPLACEFRAME CreateInPlaceFrame(PTERWND,int,LPOLECLIENTSITE);
        HRESULT STDMETHODCALLTYPE IipfQueryInterface(LPOLEINPLACEFRAME,REFIID,LPVOID far *);
        ULONG STDMETHODCALLTYPE IipfAddRef(LPOLEINPLACEFRAME);
        ULONG STDMETHODCALLTYPE IipfRelease(LPOLEINPLACEFRAME);
        HRESULT STDMETHODCALLTYPE IipfGetWindow(LPOLEINPLACEFRAME,HWND far *);
        HRESULT STDMETHODCALLTYPE IipfContextSensitiveHelp(LPOLEINPLACEFRAME,BOOL);
        HRESULT STDMETHODCALLTYPE IipfGetBorder(LPOLEINPLACEFRAME,LPRECT);
        HRESULT STDMETHODCALLTYPE IipfRequestBorderSpace(LPOLEINPLACEFRAME,LPCBORDERWIDTHS);
        HRESULT STDMETHODCALLTYPE IipfSetBorderSpace(LPOLEINPLACEFRAME,LPCBORDERWIDTHS);
        HRESULT STDMETHODCALLTYPE IipfSetActiveObject(LPOLEINPLACEFRAME,LPOLEINPLACEACTIVEOBJECT,LPCSTR);
        HRESULT STDMETHODCALLTYPE IipfInsertMenus(LPOLEINPLACEFRAME,HMENU,LPOLEMENUGROUPWIDTHS);
        HRESULT STDMETHODCALLTYPE IipfSetMenu(LPOLEINPLACEFRAME,HMENU,HOLEMENU,HWND);
        HRESULT STDMETHODCALLTYPE IipfRemoveMenus(LPOLEINPLACEFRAME,HMENU);
        HRESULT STDMETHODCALLTYPE IipfSetStatusText(LPOLEINPLACEFRAME,LPCSTR);
        HRESULT STDMETHODCALLTYPE IipfEnableModeless(LPOLEINPLACEFRAME,BOOL);
        HRESULT STDMETHODCALLTYPE IipfTranslateAccelerator(LPOLEINPLACEFRAME,LPMSG,WORD);

        LPDISPATCH CreateDispatch(PTERWND,int,LPOLECLIENTSITE);
        HRESULT STDMETHODCALLTYPE IdQueryInterface(LPDISPATCH,REFIID,LPVOID far *);
        ULONG STDMETHODCALLTYPE IdAddRef(LPDISPATCH);
        ULONG STDMETHODCALLTYPE IdRelease(LPDISPATCH);
        HRESULT STDMETHODCALLTYPE IdInvoke(LPDISPATCH,DISPID,REFIID,LCID,WORD,DISPPARAMS far *,VARIANT far *,EXCEPINFO far *,unsigned int far *);
        HRESULT STDMETHODCALLTYPE IdGetIDsOfNames(LPDISPATCH,REFIID,OLECHAR FAR* FAR*,unsigned int,LCID,DISPID far *);
        HRESULT STDMETHODCALLTYPE IdGetTypeInfo(LPDISPATCH,UINT,LCID,ITypeInfo FAR* FAR*);
        HRESULT STDMETHODCALLTYPE IdGetTypeInfoCount(LPDISPATCH,unsigned int FAR*);

        DISPID DispIdFromName(PTERWND,int,LPBYTE);
        DISPID DispIdFromName2(PTERWND,LPDISPATCH,LPBYTE);
        BOOL   TerGetBool(VARIANTARG far *);

        LPDROPTARGET CreateDropTarget(PTERWND);
        HRESULT STDMETHODCALLTYPE IdtQueryInterface(LPDROPTARGET,REFIID,LPVOID far *);
        ULONG STDMETHODCALLTYPE IdtAddRef(LPDROPTARGET);
        ULONG STDMETHODCALLTYPE IdtRelease(LPDROPTARGET);
        HRESULT STDMETHODCALLTYPE IdtDrop(LPDROPTARGET,LPDATAOBJECT,DWORD,POINTL,DWORD *);
        HRESULT STDMETHODCALLTYPE IdtDragEnter(LPDROPTARGET,LPDATAOBJECT,DWORD,POINTL,DWORD *);
        HRESULT STDMETHODCALLTYPE IdtDragOver(LPDROPTARGET,DWORD,POINTL,DWORD *);
        HRESULT STDMETHODCALLTYPE IdtDragLeave(LPDROPTARGET);

        LPPROPERTYBAG CreatePropertyBag(PTERWND);
        HRESULT STDMETHODCALLTYPE IpbQueryInterface(LPPROPERTYBAG,REFIID,LPVOID far *);
        ULONG STDMETHODCALLTYPE IpbAddRef(LPPROPERTYBAG);
        ULONG STDMETHODCALLTYPE IpbRelease(LPPROPERTYBAG);
        HRESULT STDMETHODCALLTYPE IpbRead(LPPROPERTYBAG,LPCOLESTR,VARIANT *,LPERRORLOG *);
        HRESULT STDMETHODCALLTYPE IpbWrite(LPPROPERTYBAG,LPCOLESTR,VARIANT *);

     #endif

  #endif

  DWORD   CALLBACK _export TerOleGet(LPOLESTREAM,void FAR *,DWORD);
  DWORD   CALLBACK _export TerOlePut(LPOLESTREAM,void FAR *,DWORD);
  int     TerCreateTempOle(PTERWND,int);
  UINT    GetOleClipItem(PTERWND,LPBYTE,LPBYTE,LPBYTE);

#endif


int     OlePostProcessing(PTERWND);
int     TerInsertObject(PTERWND,int);
BOOL    TerInsertDragObject(PTERWND,HDROP,BOOL);
BOOL    ExitOle(PTERWND);
BOOL    InitOle(PTERWND);
BOOL    TerOleQuerySize(PTERWND,int,DWORD far *);
BOOL    ShowOcx(PTERWND,int);
BOOL    HideOcx(PTERWND,int);
BOOL    PosOcx(PTERWND,int,HDC);
int     GetOleStorageData(PTERWND,int);
int     TerEditOle(PTERWND,BOOL);
int     TerOleLoad(PTERWND,int);
int     TerOleFromClipboard(PTERWND,UINT,BOOL);
int     TerOleUnload(PTERWND,int);
int     TerPasteSpecial(PTERWND);


/***************************************************************************
                             FILE: TER_PAGE.C
****************************************************************************/
BOOL    IsKeepNextRow(PTERWND,long,LPLONG);
int     PageHdrHeight2(PTERWND,int,BOOL,BOOL);
int     UpdateBookmark(PTERWND,long,int);
int     PosWatermarkFrame(PTERWND,int);
BOOL    PageResized(PTERWND);
BOOL    IsPictPageBreak(PTERWND,long,int);
BOOL    SetPictPageBreak(PTERWND,long,int);
BOOL    SkipCellLine(PTERWND,long,int,int,LPINT,LPINT,int,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT,LPINT);
BOOL    HasUnpositionedPict(PTERWND,long,int);
BOOL    ResetLinePictPos(PTERWND,long);
long    AdjustForParaKeep(PTERWND,long,long,BOOL);
int     AdjustRowHeightForBaseAlign(PTERWND,int);
int     AdjustSectColHeight(PTERWND,int,int,long,LPLONG,int,BOOL);
int     CalcTblHdrHt(PTERWND,int);
int     CreateOnePage(PTERWND,int,BOOL);
int     EndCellOnPage(PTERWND,long,int,BOOL,BOOL,LPINT,LPINT,LPINT,LPINT,int,LPINT);
int     GetBorderLeftSpace(PTERWND,long);
int     GetPictLastY(PTERWND,long);
int     GetSectColWidthSpace(PTERWND,int,int,LPINT,LPINT,LPINT,LPINT);
int     GetSpannedRowHeight(PTERWND,int,LPINT,int);
int     HdrFtrExists(PTERWND, struct StrHdrFtr far *);
int     ResetHdrFtr(PTERWND, struct StrHdrFtr far *);
int     PageHdrSect(PTERWND,int,struct StrHdrFtr far *);
int     PageFtrSect(PTERWND,int,struct StrHdrFtr far *);
int     PageFtrHeight(PTERWND,int,BOOL);
int     PageFtrTextHeight(PTERWND,int);
int     PageHdrHeight(PTERWND,int,BOOL);
int     Repaginate(PTERWND,BOOL,BOOL,int,BOOL);
int     RequestPagination(PTERWND,BOOL);
int     WrapMoreLines(PTERWND,int);

/***************************************************************************
                             FILE: TER_PAG1.C
****************************************************************************/
BOOL    IsLastSpannedCell(PTERWND,int);
BOOL    IsPageLastRow(PTERWND,int,int);
BOOL    IsSpannedRow(PTERWND,int);
BOOL    IsSpanningRow(PTERWND,int);
int     AdjustPageNbr(PTERWND,int,long);
int     AdjustTableRowWidth(PTERWND,int);
int     CheckPageSpace(PTERWND,int);
int     CreateEndnote(PTERWND);
int     CreateToc(PTERWND);
int     ExtractFootnote(PTERWND,HDC,int,int,long,int,BOOL,BOOL);
int     GetPageMultiple(PTERWND);
int     GetRemainingCellSpans(PTERWND,int);
int     GetScrPageHt(PTERWND,int);
int     GetTocStyle(PTERWND, BYTE);
int     InsertDynField(PTERWND,int,LPBYTE);
int     PageFromLine(PTERWND,long,int);
int     PageTextWidth(PTERWND);
int     PgmDown(PTERWND);
int     PgmLeft(PTERWND);
int     PgmPageDn(PTERWND);
int     PgmPageHorz(PTERWND,int);
int     PgmPageLeft(PTERWND,BOOL);
int     PgmPageRight(PTERWND,BOOL);
int     PgmPageUp(PTERWND);
int     PgmPageVert(PTERWND,int);
int     PgmRight(PTERWND,BOOL);
int     PgmUp(PTERWND);
int     PgmWinDown(PTERWND);
int     PgmWinLeft(PTERWND);
int     PgmWinRight(PTERWND);
int     PgmWinScroll(PTERWND,int,int);
int     PgmWinUp(PTERWND);
int     TogglePageBorder(PTERWND);
int     TransBlt(PTERWND,HDC,int,int,int,int);
int     UpdateDynField(PTERWND,long,int);
int     UpdateToc(PTERWND);
long    BeginEndnote(PTERWND,BOOL,LPLONG,int);
long    SumPageScrHeight(PTERWND, int,int);

/***************************************************************************
                             FILE: TER_PDF.C
****************************************************************************/

/***************************************************************************
                             FILE: TER_PAR.C
****************************************************************************/
struct StrListLevel *CheckLinePLevel(PTERWND,long, int,struct StrListLevel *,LPINT);
int     NewParaId2(PTERWND,int,struct StrPfmt *);
int     GetNewListName(PTERWND,LPBYTE,LPBYTE);
int     ClearTabDlg(PTERWND);
int     GetFirstIndent(PTERWND,long,LPINT,BOOL far *,BOOL);
int     GetListText(PTERWND,int,long,LPBYTE,LPINT,LPLONG,LPINT,int,int,BOOL);
int     GetNumberForLevel(PTERWND,long,int,int,int,int);
int     GetParaRange(PTERWND,LPLONG,LPLONG);
int     NewBltId(PTERWND,int,struct StrBlt far *);
int     NewParaId(PTERWND,int,int,int,int,int,int,int,DWORD,int,UINT,UINT,int,int,int,int,COLORREF,int,int,unsigned);
int     NewTabId(PTERWND,int,struct StrTab far *);
int     SetParaParam(PTERWND,int,int,int);
int     SetParaTextFlow(PTERWND,int,int);
int     TerParaBorder(PTERWND);
int     TerParaSpacing(PTERWND);
LPBYTE  CodeListText(PTERWND,LPBYTE,LPBYTE);
LPBYTE  DecodeListText(PTERWND,LPBYTE,LPBYTE);

struct  StrListLevel far *GetListLevelPtr(PTERWND,int,int);


/***************************************************************************
                             FILE: TER_PRT.C
****************************************************************************/
int     FindPaperSize(PTERWND,int,int);
long    TerMergePrintRep4(HDC,LPBYTE,HGLOBAL,DWORD,RECT far *,LPBYTE,LPBYTE,long,LPINT,HDC,BOOL,DWORD);
BOOL    TerMergePrint2(struct StrPrint far *,BOOL,DWORD);
int     SetPdfBkm(PTERWND,HDC,long,int,int);
int     PrintPageBknd(PTERWND,HDC,HDC,int);
BOOL    CALLBACK _export MergeEnumMetafile(HDC,HANDLETABLE FAR *,METARECORD FAR *,int,LPARAM);
BOOL    CALLBACK _export PrtEnumMetafile(HDC,HANDLETABLE FAR *,METARECORD FAR *,int,LPARAM);
BOOL    CALLBACK _export PrtEnumMetafile2(HDC,HANDLETABLE FAR *,METARECORD FAR *,int,LPARAM);
BOOL    PrintControl(PTERWND,HDC,HDC,int,int,int,int,int);
BOOL    PrintControlString(PTERWND,HDC,HDC,LPBYTE,int,int,int,int,int,BOOL);
int     CalcPageDim(PTERWND,long,int far *,int far *,int far *,int far *,int far *);
int     CALLBACK _export PrintProc(HDC,int);
int     FillRectangle(HDC,LPRECT,HBRUSH);
int     GetLastSpannedCellHeight(PTERWND,int,LPINT,int);
int     GetLineLen(PTERWND,LPBYTE,LPWORD,LPBYTE,int,int,int);
int     GetPrtTabWidth(PTERWND,int,int,struct StrTabw far *);
int     PaintPrintPreview(PTERWND);
int     PrePreview(PTERWND,HDC);
int     PreviewOnePage(PTERWND,HDC,RECT far *rect,int,BOOL);
int     PreviewDown(PTERWND, BOOL);
int     PreviewRight(PTERWND, BOOL);
int     PreviewLeft(PTERWND, BOOL);
int     PreviewPageVert(PTERWND,int);
int     PreviewUp(PTERWND, BOOL);
int     PostPreview(PTERWND);
int     PrintBkndPicture(PTERWND,HDC,HDC,int);
int     PrintFrameBorder(PTERWND,HDC,int,int,int,int);
int     PrintFrameBorderShading(PTERWND,HDC,int,int,int);
int     PrintParaShading(PTERWND,HDC,long,int,int,int,int,int);
int     PrintPicture(PTERWND,HDC,HDC,int,int,int,int,int,int);
int     PrintViewMode(PTERWND,HDC,long,long,int,int);
int     PrintOneLine(PTERWND,long,HDC,HDC,int,int,LPBYTE,LPWORD,LPBYTE,int,int,int,struct StrTabw far *,int,int,int,int,int,int,int,LPWORD,BOOL,int,int);
int     PrintOneSegment(PTERWND,HDC,HDC,int,int,int,LPBYTE,LPBYTE,int,int,int,int,LPWORD,LPWORD);
int     PrintNonWrapLine(PTERWND,long,HDC,HDC,int,int,LPBYTE,LPWORD,LPBYTE,int,int,int,struct StrTabw far *,int,int,int,int,int,int,LPWORD);
int     SpoolIt(PTERWND,HDC);
int     TerPageOptions(PTERWND);
int     TerPrintOptions(PTERWND,BOOL);

/***************************************************************************
                             FILE: TER_RTF.C
****************************************************************************/
int     CopyToOutBuf(PTERWND,struct StrRtf far *);
BOOL    ImportRtfData(PTERWND,int,struct StrRtfGroup far *,struct StrRtfGroup far *,LPBYTE);
BOOL    RtfRead(PTERWND,int,LPBYTE,BYTE huge *,long);
int     DeleteRtfRow(PTERWND,struct StrRtf far *,int);
int     ProcessRtfControl(PTERWND,struct StrRtf far *);

/***************************************************************************
                             FILE: TER_RTF1.C
****************************************************************************/
int     ReadRtfReviewers(PTERWND,struct StrRtf far *);
BOOL    ResolveLinkFileName(PTERWND,LPBYTE);
BOOL    UnicodeToMultiByte(PTERWND,WORD,LPBYTE,LPBYTE,int);
BOOL    CopyRtfRow(PTERWND,struct StrRtf far *,struct StrRtfGroup far *,int);
BOOL    CreateRtfCell(PTERWND,struct StrRtf far *,struct StrRtfGroup far *,int);
BOOL    GetRtfChar(PTERWND,struct StrRtf far *);
BOOL    GetRtfHexChar(PTERWND,struct StrRtf far *);
BOOL    GetRtfWord(PTERWND,struct StrRtf far *);
BOOL    ImportRtfPicture(PTERWND,struct StrRtfGroup far *, struct StrRtfPict far *);
BOOL    ImportRtfTextLine(PTERWND,struct StrRtfGroup far *,LPBYTE);
BOOL    IsRtfPlainFont(PTERWND,struct StrRtf far *,struct StrRtfGroup far *);
BOOL    IsSameRtfParaFrame(struct StrRtfParaFrameInfo far *,struct StrRtfParaFrameInfo far *);
BOOL    MbcsToUnicode(PTERWND);
BOOL    TextToUnicode(PTERWND);
BOOL    MergeRtfLinePieces(PTERWND,long,long);
BOOL    PushRtfChar(PTERWND,struct StrRtf far *);
BOOL    SendRtfText(PTERWND,struct StrRtf far *);
BOOL    SetRtfDocPaperSize(PTERWND,struct StrRtf far *);
BOOL    SetRtfFontDefault(PTERWND,struct StrRtf far *,struct StrRtfGroup far *);
BOOL    SetRtfPaperSize(PTERWND,struct StrRtf far *, int, int, LPINT);
BOOL    SetRtfParaBorders(PTERWND,struct StrRtfGroup far *,int);
BOOL    SetRtfParaDefault(PTERWND,struct StrRtf far *,struct StrRtfGroup far *);
BOOL    SetRtfSectBorders(PTERWND,struct StrRtfGroup far *,int);
BOOL    SetRtfSectPaperSize(PTERWND,struct StrRtf far *);
BOOL    SplitRtfMbcsText(PTERWND,struct StrRtf far *,LPBYTE,LPBYTE,struct StrRtfGroup far *);
int     ApplyRtfTagId(PTERWND,struct StrRtf far *,long);
int     ExtractRtfPict(PTERWND,struct StrRtf far *);
int     GetRtfDefaultFont(PTERWND,struct StrRtf far *);
int     GetRtfFontId(PTERWND,struct StrRtf far *,struct StrRtfGroup far *); 
int     MakeRtfObject(PTERWND,struct StrRtf far *,BYTE huge *,int,long,int,BOOL);
int     ReadRtfBookmark(PTERWND,struct StrRtf far *);
int     ReadRtfTag(PTERWND,struct StrRtf far *);
int     ReadRtfColorTable(PTERWND,struct StrRtf far *);
int     ReadRtfField(PTERWND,struct StrRtf far *);
int     ReadRtfFormField(PTERWND,struct StrRtf far *);
int     ReadRtfFontTable(PTERWND,struct StrRtf far *);
int     ReadRtfLinkedPicture(PTERWND,struct StrRtf far *,LPBYTE);
int     ReadRtfObject(PTERWND,struct StrRtf far *rtf);
int     ReadRtfShapeParam(PTERWND,struct StrRtf far *,LPBYTE);
int     ReadRtfShapeProp(PTERWND,struct StrRtf far *,LPBYTE);
int     ReadRtfShapeText(PTERWND,struct StrRtf far *);
int     ReadRtfInfo(PTERWND,struct StrRtf far *);
int     SetRtfParaFID(PTERWND,long,struct StrRtfGroup far *);
int     SetRtfParaId(PTERWND,long,struct StrRtfGroup far *);
int     SkipRtfGroup(PTERWND,struct StrRtf far *);
LPBYTE  ReadRtfObjBytes(PTERWND,struct StrRtf far *,LPLONG);
LPBYTE  XlateRtfHex(PTERWND,LPBYTE);

/***************************************************************************
                             FILE: TER_RTF2.C
****************************************************************************/
BYTE    ApplyBrightnessContrast(PTERWND,BYTE,int,int);
BOOL    ApplyPictureBrightnessContrast(PTERWND,int,int,int);
int     UnicodeToRegBullet(PTERWND,struct StrRtf far *,LPBYTE,int,LPINT);
int     ReadRtfShpGrp(PTERWND , struct StrRtf far *);
BOOL    BuildRtfAnimSeq(PTERWND,struct StrRtfGroup far *,struct StrRtfPict far *,int);
BOOL    BuildRtfPictFrame(PTERWND,struct StrRtfGroup far *,int);
BOOL    BuildRtfPicture(PTERWND,struct StrRtfGroup far *,struct StrRtfPict far *,int);
BOOL    RtfHdrFtrExists(PTERWND,int);
int     ExitRtfGroup(PTERWND,struct StrRtf far *,int);
int     FixNegativeIndents(PTERWND,struct StrRtf far *);
int     FmtRtfFootnoteNbr(PTERWND,struct StrRtf far *,LPBYTE,int,int);
int     GetRtfLevelInfo(PTERWND,struct StrRtf far *,int);
int     HideHiddenParaMarkers(PTERWND);
int     InitGroupFromStyle(PTERWND,struct StrRtf far *,int,int,BOOL);
int     InitGroupForStyle(PTERWND,struct StrRtf far *);
int     InitRtfGroup(PTERWND,struct StrRtf far *,int);
int     ReadRtfBullet(PTERWND,struct StrRtf far *);
int     ReadRtfGroupText(PTERWND,struct StrRtf far *,LPBYTE);
int     ReadRtfList(PTERWND,struct StrRtf far *);
int     ReadRtfListOr(PTERWND,struct StrRtf far *);
int     ReadRtfPicture(PTERWND,struct StrRtf far *);
int     ReadRtfShape(PTERWND,struct StrRtf far *);
int     SetRtfFootnote(PTERWND,struct StrRtfGroup far *,long);
int     SetRtfLineOrient(PTERWND w, struct StrRtf far *rtf,BOOL,BOOL);
int     SaveRtfLevelInfo(PTERWND,struct StrRtf far *,int);
int     SetRtfTableInfo(PTERWND, struct StrRtfGroup far *);
int     SetRtfTblLevel(PTERWND,struct StrRtf far *,int, int,int);
int     UpdateRtfStylesheet(PTERWND,struct StrRtf far *);
long    GetRtfInsertionLine(PTERWND,struct StrRtf far *);
struct  StrRtfGroup far *GetRtfGroup(PTERWND,struct StrRtf far *);


/***************************************************************************
                             FILE: TER_RTF3.C
****************************************************************************/
BOOL    WriteRtfRev(PTERWND,struct StrRtfOut far *);
DWORD   GetRtfTrackingTime(PTERWND,DWORD);
BOOL    AppendRtfHlink(PTERWND,struct StrRtfOut far *,LPBYTE,LPBYTE);
BOOL    BeginRtfFieldName(PTERWND,struct StrRtfOut far *,int,int);
BOOL    ResetRtfFont(PTERWND,struct StrRtfOut far *);
BOOL    RtfWrite(PTERWND,int,LPBYTE);
BOOL    SetRtfRowDefault(PTERWND,struct StrRtf far *,struct StrRtfGroup far *,int);
BOOL    WritePfObjectTail(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfBullet(PTERWND w,struct StrRtfOut far * rtf,int);
BOOL    WriteRtfCharFmt(PTERWND,struct StrRtfOut far *,int,BOOL);
BOOL    WriteRtfCell(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfCellBorder(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfCharStyle(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfColor(PTERWND,struct StrRtfOut far *);
BOOL    WriteRtfDoInfo(PTERWND,struct StrRtfOut far *,int,UINT,int);
BOOL    WriteRtfFont(PTERWND,struct StrRtfOut far *);
BOOL    WriteRtfFontAttrib(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfFontStyle(PTERWND,struct StrRtfOut far *,UINT,UINT);
BOOL    WriteRtfFrameInfo(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfNoNestGroup(PTERWND,struct StrRtfOut far *);
BOOL    WriteRtfOneFont(PTERWND,struct StrRtfOut far *,int,LPBYTE,LPBYTE,int);
BOOL    WriteRtfParaFmt(PTERWND,struct StrRtfOut far *,int,int,int,int,int,int,long);
BOOL    WriteRtfRow(PTERWND,struct StrRtfOut far *,int,int,int);
BOOL    WriteRtfStylesheet(PTERWND,struct StrRtfOut far *);

/***************************************************************************
                             FILE: TER_RTF4.C
****************************************************************************/
BOOL    WriteRtfParaBorderColor(PTERWND,struct StrRtfOut far *,COLORREF);
BOOL    WriteRtfWatermark(PTERWND,struct StrRtfOut far *,BOOL);
BOOL    BeginRtfGroup(PTERWND,struct StrRtfOut far *);
BOOL    EndInterParaGroups(PTERWND,struct StrRtfOut far *,int);
BOOL    EndRtfGroup(PTERWND,struct StrRtfOut far *);
BOOL    FlushRtfLine(PTERWND,struct StrRtfOut far *);
BOOL    PutRtfChar(PTERWND,struct StrRtfOut far *,BYTE);
BOOL    PutRtfHexChar(PTERWND,struct StrRtfOut far *,BYTE);
BOOL    PutRtfSpecChar(PTERWND,struct StrRtfOut far *,BYTE);
BOOL    WriteRtfAnimSeq(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfBackground(PTERWND,struct StrRtfOut far *,COLORREF);
BOOL    WriteRtfControl(PTERWND,struct StrRtfOut far *,LPBYTE,int,double);
BOOL    WriteRtfCtl(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfDIB(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfForm(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfFormTextField(PTERWND,struct StrRtfOut far *,LPBYTE);
BOOL    WriteRtfLinkedPicture(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfList(PTERWND,struct StrRtfOut far *);
BOOL    WriteRtfListLevel(PTERWND,struct StrRtfOut far *,struct StrListLevel far *);
BOOL    WriteRtfMargin(PTERWND,struct StrRtfOut far *);
BOOL    WriteRtfMbcsText(PTERWND,struct StrRtfOut far *,LPBYTE,LPBYTE,WORD,int,int);
BOOL    WriteRtfMetafile(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfObject(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfOrigImage(PTERWND,struct StrRtfOut far *,int,int,LPBYTE,DWORD);
BOOL    WriteRtfParaBorder(PTERWND,struct StrRtfOut far *,UINT,COLORREF,int);
BOOL    WriteRtfParaStyle(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfPicture(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfSection(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfShape(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfShapeProp(PTERWND,struct StrRtfOut far *,LPBYTE,LPBYTE);
BOOL    WriteRtfShapeProp2(PTERWND,struct StrRtfOut far *,LPBYTE,int);
BOOL    WriteRtfTab(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfTag(PTERWND,struct StrRtfOut far *,int);
BOOL    WriteRtfText(PTERWND,struct StrRtfOut far *,LPBYTE,int);

#if defined(WIN32)
BOOL    WriteRtfEnhMetafile(PTERWND,struct StrRtfOut far *,int);
#endif

/***************************************************************************
                             FILE: TER_SPL.C
****************************************************************************/
BOOL    StUnicodeToText(PTERWND,long);
BOOL    StTextToUnicode(PTERWND,long);
BOOL    DoAutoSpellCheck(PTERWND);
BOOL    GetMisspelledWord(PTERWND,long,int,LPBYTE,LPINT,LPINT);
BOOL    SpellCheckCurWordPart1(PTERWND,LPARAM);
BOOL    SpellCheckCurWordPart2(PTERWND,int);
BOOL    WordBeingEdited(PTERWND);
int     AutoSpellCheck(PTERWND,long);
int     SearchSpellTime(PTERWND);
int     SplReplaceWord(PTERWND,LPBYTE,long,int,LPINT,LPINT,LPBYTE,int);
void    SplHilightWord(PTERWND);

/***************************************************************************
                             FILE: TBL_TBL.C
****************************************************************************/
BOOL    GetTableSelRange(PTERWND,LPLONG,LPLONG);
BOOL    GetTableMinMaxWidths(PTERWND,int,LPLONG,LPLONG,LPLONG,LPINT,long far * (far*),long far * (far *),long);
BOOL    SetRowBreakFlag(PTERWND);
BOOL    SetSubtableCellWidths(PTERWND,int,int);
BOOL    SetTableCellWidths(PTERWND,int,long,long,long,BOOL,LPLONG,LPLONG);
int     GetCellMinMaxWidth(PTERWND,int,LPLONG,LPLONG,int);
int     SetCellLines(PTERWND);
int     TerMergeCells(PTERWND);
int     TerSplitCell(PTERWND);

/***************************************************************************
                             FILE: TBL_TBL1.C
****************************************************************************/
BOOL    IsBaselineAlignedCellLine(PTERWND,long);
BOOL    InSameTable(PTERWND,int,int);
BOOL    IsFirstTableRow(PTERWND,int);
BOOL    IsLastTableRow(PTERWND,int);
BOOL    IsPartRow(PTERWND,BOOL,int,int);
BOOL    IsTableSplit(PTERWND,int,int);
BOOL    TableHilighted(PTERWND);
int     AdjustBlockForTable(PTERWND,LPLONG,LPINT,LPLONG,LPINT,BOOL);
int     CopyCell(PTERWND,int,int);
int     DelCell(PTERWND,int);
int     GetCellColumn(PTERWND,int,BOOL);
int     GetCellRightX(PTERWND,int);
int     GetCellSlot(PTERWND,BOOL);
int     GetColumnCell(PTERWND,int,int,BOOL);
int     GetNextCellInColumn(PTERWND,int,BOOL);
int     GetNextCellInColumnPos(PTERWND,int);
int     GetPrevCellInColumn(PTERWND,int,BOOL,BOOL);
int     GetPrevCellInColumnPos(PTERWND,int,BOOL);
int     GetRowCell(PTERWND,int,int);
int     GetRowWidth(PTERWND,int);
int     GetSameColumnCell(PTERWND,int,BOOL);
int     GetTableRowSlot(PTERWND);
int     GetTblSpcBef(PTERWND,long,BOOL);
int     InitCell(PTERWND,int);
int     InOuterLevels(PTERWND,int,long);
int     InsertCell(PTERWND,int,int,int,BYTE);
int     LevelCell(PTERWND,int,long);
int     LevelRow(PTERWND,int,int);
int     MarkCells(PTERWND,int);
int     MinTableLevel(PTERWND,long,long);
int     RecoverCellSlots(PTERWND);
int     RecoverTableRowSlots(PTERWND);
int     RemoveCell(PTERWND,int);
int     RepairTable(PTERWND);
int     SetRowIndent(PTERWND,long,int,int,int);
int     TblHilightLeft(PTERWND);
int     TableLevel(PTERWND,long);
int     TblHilightRight(PTERWND,BOOL);
int     TerBackTabCell(PTERWND);
int     TerTabCell(PTERWND);
int     TerToggleTableGrid(PTERWND);
int     UniformRowBorderCell(PTERWND,int,BOOL);
long    RepairOneTable(PTERWND,long,int);

/***************************************************************************
                             FILE: TBL_TRK.C
****************************************************************************/
BOOL    IsTrackChangeFont(PTERWND,int);
BOOL    AcceptChange(PTERWND,long,int);
BOOL    GetChangeBeginPos(PTERWND, LPLONG,LPINT);
BOOL    LocateRevMatched(PTERWND, BOOL,int,int,int,int);
BOOL    TrackDelBlock(PTERWND, long, int, long, int, BOOL, BOOL);
BOOL    TrackDel(PTERWND,long,int,BOOL);
BOOL    TrackingComment(PTERWND, long, int, LPBYTE);
int     SetTrackingFont(PTERWND,int,int);
int     FreeReviewer(PTERWND,int);
int     GetReviewerSlot(PTERWND);

/***************************************************************************
                             FILE: TBL_WRAP.C
****************************************************************************/
BOOL    DisplacePointers(PTERWND,long,long);
BOOL    GetWrapCharWidth(PTERWND);
BOOL    IsFirstParaLine(PTERWND,long);
int     CopyWrapLineData(PTERWND,int,int,WORD);
int     SaveWrapHilight(PTERWND,LPLONG,LPLONG,LPINT,LPINT,LPINT);
int     RestoreWrapHilight(PTERWND,long,long,BOOL,BOOL,BOOL);
int     WordWrap(PTERWND,long,long);
int     WrapMakeBuffer(PTERWND,long,long);
int     WrapParseBuffer(PTERWND,long);
