/*==============================================================================
   TER_FMT2.C
   TER Format Handling additional functions.

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1991-97)
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

/*****************************************************************************
    FmtAlloc:
    This routine allocates space for storing the formatting
    information.  If the the length of the formatting information is 1,
    the formatting information will be stored in the array (cfmt) itself,
    instead of the pointer to the formatting information.
******************************************************************************/
FmtAlloc(PTERWND w,long line,int OldSize,int NewSize)
{
    WORD CurFmt;
    int i;
    LPWORD fmt;

    TerArg.modified++;

    if (cfmt(line).info.type==UNIFORM) {        // expand it
       if (OldSize==0) cfmt(line).pFmt=NULL;
       else {
          CurFmt=cfmt(line).info.fmt;
          if (NULL==(cfmt(line).pFmt=sAlloc(sizeof(WORD)*(OldSize+1)))) {
             AbortTer(w,"RAN OUT OF MEMORY (FmtAlloc B)",60);
          }
          fmt=cfmt(line).pFmt;
          for (i=0;i<OldSize;i++) fmt[i]=CurFmt;
       }
    }

    if (OldSize==0 && NewSize==0) {
       cfmt(line).pFmt=NULL;
       return TRUE;
    }

    if (OldSize==NewSize) return TRUE;

    if (NewSize==0){
       sFree(cfmt(line).pFmt);
       cfmt(line).pFmt=0;
       return TRUE;
    }

    if (OldSize>0) {
       if (NULL==(cfmt(line).pFmt=sReAlloc(cfmt(line).pFmt,sizeof(WORD)*(NewSize+1)))) {
          AbortTer(w,"RAN OUT OF MEMORY(FmtAlloc)",61);
       }
       for (i=OldSize;i<NewSize;i++) cfmt(line).pFmt[i]=0;
       return TRUE;
    }

    if (OldSize==0) {
       if (NULL==(cfmt(line).pFmt=sAlloc(sizeof(WORD)*(NewSize+1)))) {
          AbortTer(w,"RAN OUT OF MEMORY (FmtAlloc A)",62);
       }
       for (i=0;i<NewSize;i++) cfmt(line).pFmt[i]=0;
       return TRUE;
    }

    return TRUE;
}

/*****************************************************************************
    CharWidthAlloc:
    This routine allocates space for storing the character width information.
    The array is assigned to -1.
******************************************************************************/
CharWidthAlloc(PTERWND w,long line,int OldSize,int NewSize)
{
    int i;

    TerArg.modified++;

    if (pCharWidth(line)==NULL) {        // expand it
       if (NewSize==0) return TRUE;
       if (NULL==(pCharWidth(line)=sAlloc(sizeof(WORD)*(NewSize+1)))) {
          AbortTer(w,"RAN OUT OF MEMORY (CharWidthAlloc)",60);
       }
       for (i=0;i<NewSize;i++) CWidth(line,i)=-1;
       return TRUE;
    }

    if (OldSize==0 && NewSize==0) {
       pCharWidth(line)=NULL;
       return TRUE;
    }

    if (OldSize==NewSize) return TRUE;

    if (NewSize==0){
       if (pCharWidth(line)) sFree(pCharWidth(line));
       pCharWidth(line)=0;
       return TRUE;
    }

    if (OldSize>0) {
       if (NULL==(pCharWidth(line)=sReAlloc(pCharWidth(line),sizeof(WORD)*(NewSize+1)))) {
          AbortTer(w,"RAN OUT OF MEMORY(CharWidthAlloc)",61);
       }
       for (i=OldSize;i<NewSize;i++) CWidth(line,i)=-1;
       return TRUE;
    }

    return TRUE;
}

/*****************************************************************************
    CtidAlloc:
    This routine allocates space for storing the character miscellaneous id information
******************************************************************************/
CtidAlloc(PTERWND w,long line,int OldSize,int NewSize)
{
    int i;

    TerArg.modified++;

    if (pCtid(line)==NULL) {        // expand it
       if (NewSize==0) return TRUE;
       if (NULL==(pCtid(line)=sAlloc(sizeof(WORD)*(NewSize+1)))) {
          AbortTer(w,"RAN OUT OF MEMORY (CtidAlloc)",60);
       }
       for (i=0;i<NewSize;i++) ctid(line,i)=0;
       return TRUE;
    }

    if (OldSize==0 && NewSize==0) {
       pCtid(line)=NULL;
       return TRUE;
    }

    if (OldSize==NewSize) return TRUE;

    if (NewSize==0){
       sFree(pCtid(line));
       pCtid(line)=0;
       return TRUE;
    }

    if (OldSize>0) {
       if (NULL==(pCtid(line)=sReAlloc(pCtid(line),sizeof(WORD)*(NewSize+1)))) {
          AbortTer(w,"RAN OUT OF MEMORY(CtidAlloc)",61);
       }
       for (i=OldSize;i<NewSize;i++) ctid(line,i)=0;
       return TRUE;
    }

    return TRUE;
}

/*****************************************************************************
    LeadAlloc:
    This routine allocates space for storing the character miscellaneous id information
******************************************************************************/
LeadAlloc(PTERWND w,long line,int OldSize,int NewSize)
{
    int i;

    TerArg.modified++;

    if (pLead(line)==NULL) {        // expand it
       if (NewSize==0) return TRUE;
       if (NULL==(pLead(line)=sAlloc(sizeof(BYTE)*(NewSize+1)))) {
          AbortTer(w,"RAN OUT OF MEMORY (LeadAlloc)",60);
       }
       for (i=0;i<NewSize;i++) pLead(line)[i]=0;
       return TRUE;
    }

    if (OldSize==0 && NewSize==0) {
       pLead(line)=NULL;
       return TRUE;
    }

    if (OldSize==NewSize) return TRUE;

    if (NewSize==0){
       sFree(pLead(line));
       pLead(line)=0;
       return TRUE;
    }

    if (OldSize>0) {
       if (NULL==(pLead(line)=sReAlloc(pLead(line),sizeof(BYTE)*(NewSize+1)))) {
          AbortTer(w,"RAN OUT OF MEMORY(LeadAlloc)",61);
       }
       for (i=OldSize;i<NewSize;i++) pLead(line)[i]=0;
       return TRUE;
    }

    return TRUE;
}

/******************************************************************************
    TerSetDocTextFlow:
    Set the rtl/ltr text flow for the document.
******************************************************************************/
BOOL WINAPI _export TerSetDocTextFlow(HWND hWnd,BOOL dialog,int flow, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // get the list override and level
    if (dialog) {
       DlgInt1=DocTextFlow;
       lstrcpy(DlgText1,"Document Text Flow");

       if (!CallDialogBox(w,"ParaTextFlowParam",ParaTextFlowParam,0L)) return TRUE;

       flow=DlgInt1;
    }
    if (flow!=FLOW_DEF && flow!=FLOW_RTL && flow!=FLOW_LTR) return TRUE;

    DocTextFlow=flow;


    //END:
    if (repaint) {
      RequestPagination(w,TRUE);
      PaintTer(w);
    }

    return TRUE;
}

/******************************************************************************
    IsLineRtl:
    TRUE if given line direction is rtl
******************************************************************************/
BOOL IsLineRtl(PTERWND w, long LineNo)
{
    int ParaFlow,CellFlow,SectFlow;
    int sect,cl,row;

    ParaFlow=PfmtId[pfmt(LineNo)].flow;

    if (ParaFlow!=FLOW_DEF) return (ParaFlow==FLOW_RTL);

    cl=cid(LineNo);
    row=cell[cl].row;
    if (cl>0) CellFlow=TableRow[row].flow;
    else      CellFlow=FLOW_DEF;
    if (CellFlow!=FLOW_DEF) return (CellFlow==FLOW_RTL);

    sect=GetSection(w,LineNo);
    SectFlow=TerSect[sect].flow;
    if (SectFlow!=FLOW_DEF) return (SectFlow==FLOW_RTL);

    return (DocTextFlow==FLOW_RTL);
}
 
/******************************************************************************
    IsParaRtl:
    TRUE if paragraph direction is rtl
******************************************************************************/
BOOL IsParaRtl(PTERWND w, int ParaFlow, int CellFlow, int SectFlow, int DocFlow)
{
    if (ParaFlow==FLOW_DEF) {
       if (CellFlow==FLOW_DEF) {
          if (SectFlow==FLOW_DEF) return (DocFlow==FLOW_RTL);
          else                    return (SectFlow==FLOW_RTL);
       }
       else return (CellFlow==FLOW_RTL); 
    }
    else return (ParaFlow==FLOW_RTL); 
}
 
/******************************************************************************
    EnableMbcs:
    Enable mbcs environment if the give character set belongs to an mbcs character
******************************************************************************/
EnableMbcs(PTERWND w, int CharSet)
{
    if (mbcs) return TRUE;   // mbcs already enabled

    if (!IsMbcsCharSet(w,CharSet,NULL)) return TRUE;  // not an mbcs character set
 
    mbcs=TRUE;

    // allocate mbcs memory
    CharScrFontId=OurAlloc(CHAR_WIDTH_CACHE_SIZE);
    CharPrtFontId=OurAlloc(CHAR_WIDTH_CACHE_SIZE);
    CharScrWidth=OurAlloc(CHAR_WIDTH_CACHE_SIZE);
    CharPrtWidth=OurAlloc(CHAR_WIDTH_CACHE_SIZE);

    InitCharWidthCache(w);          // initialize the character width cache

    return TRUE;
}

/******************************************************************************
    IsMbcsCharSet:
    Returns TRUE if the given character set is an mbcs character set.
******************************************************************************/
BOOL IsMbcsCharSet(PTERWND w, int CharSet, LPINT pCodePage)
{
    CPINFO cp;
    CHARSETINFO cs;
 
    if (pCodePage) (*pCodePage)=0;

    if (TranslateCharsetInfo ((DWORD*)CharSet, &cs,TCI_SRCCHARSET)) {
       if (pCodePage) (*pCodePage)=cs.ciACP;
       GetCPInfo(cs.ciACP,&cp);
       return (cp.MaxCharSize>1);
    }

    return FALSE;
}
 
/******************************************************************************
    GetEffectiveCfmt:
    Get the font id in effect for new character input
******************************************************************************/
int GetEffectiveCfmt(PTERWND w)
{
    int i,NewCfmt,col,PrevFont,len,CurFont,FieldId;
    LPWORD fmt;
    long l;
    BOOL GetFromPrevLine=FALSE;
    BYTE CurChar,FontFound;
    LPBYTE ptr;

    NewCfmt=DEFAULT_CFMT;                     // default id

    if (InputFontId>=0 && TerFont[InputFontId].style&PICT) InputFontId=-1;

    if (InputFontId>=0) return InputFontId; // NewCfmt=InputFontId;
    else if (HilightType==HILIGHT_CHAR) {     // font of the highlighted block
       if (HilightEndRow<HilightBegRow || (HilightEndRow==HilightBegRow && HilightEndCol<HilightBegCol)) {
          l=HilightEndRow;
          col=HilightEndCol;
       }
       else {
          l=HilightBegRow;
          col=HilightBegCol;
       }
       if (col>=0 && LineLen(l)>col) {
          fmt=OpenCfmt(w,l);
          NewCfmt=fmt[col];
          CloseCfmt(w,l);
       }
    }
    else {                                    // font from the previous character
       // check if font needs to be retrieved from the previous lines
       if (CurLine>0) {
          if (LineLen(CurLine)==0) GetFromPrevLine=TRUE;
          else if (CurCol==0) {
             if (HtmlMode) {
                if (pfmt(CurLine)==pfmt(CurLine-1) && LineLen(CurLine)==1) GetFromPrevLine=TRUE;
             }
             else {
               //GetFromPrevLine=TRUE;
                if (/*!(LineFlags(CurLine)&LFLAG_PARA_FIRST) &&*/ LineLen(CurLine)<1) GetFromPrevLine=TRUE;
                else if (!(LineFlags(CurLine-1)&(LFLAG_PARA|LFLAG_BREAK))) GetFromPrevLine=TRUE;
                else if ((CurLine+1)==TotalLines && LineFlags(CurLine)&LFLAG_PARA && LineLen(CurLine)==1) GetFromPrevLine=TRUE;
                else if (EnterHit && LineFlags(CurLine)&LFLAG_PARA && LineLen(CurLine)==1) GetFromPrevLine=TRUE;
                if (GetFromPrevLine && CurLine>0 && cid(CurLine-1)!=cid(CurLine)) GetFromPrevLine=false;
                if (GetFromPrevLine && CurLine>0 && fid(CurLine-1)!=fid(CurLine)) GetFromPrevLine=false;
             }
          }
       }

       GET_PREV_FONT:
       if (GetFromPrevLine) {  // get the font from the previous line
          len=0;
          for (l=CurLine-1;l>=0;l--) {
            if (LineLen(l)==0) continue;
            ptr=pText(l);
            CurChar=ptr[LineLen(l)-1];

            len=LineLen(l)-(LineFlags(l)&LFLAG_BREAK?1:0);  // exclude the break characters
            if (len>0) break;
          }

          if (l>=0 && len>0) {
            if (IsBreakChar(w,CurChar) && CurChar!=ParaChar && CurChar!=LINE_CHAR)  NewCfmt=GetCurCfmt(w,CurLine,0);
            else {
               fmt=OpenCfmt(w,l);
               NewCfmt=fmt[len-1];
               CloseCfmt(w,l);
            }
          }
          else {
             GetFromPrevLine=FALSE;
             goto GET_PREV_FONT;
          } 
       }
       else if (TotalLines==1 && LineLen(0)==1) NewCfmt=GetCurCfmt(w,0,0);  // DEFAULT_CFMT;  // empty file
       else if (cfmt(CurLine).info.type==UNIFORM) NewCfmt=cfmt(CurLine).info.fmt;
       else if (CurCol>=LineLen(CurLine) && LineLen(CurLine)>0) {    // use the font of the last character of the line
          fmt=OpenCfmt(w,CurLine);
          NewCfmt=fmt[LineLen(CurLine)-1];
          CloseCfmt(w,CurLine);
       }
       else {                                  // use the font of the previous character
          fmt=OpenCfmt(w,CurLine);
          if (CurCol>0 && (CurCol-1)<LineLen(CurLine))   PrevFont=fmt[CurCol-1];
          else                                           PrevFont=0;
          if (HtmlMode && TerFont[PrevFont].style&HLINK) NewCfmt=fmt[CurCol];
          else {
             if (CurCol==0) {
                NewCfmt=fmt[CurCol];
                if (HtmlMode && TerFont[NewCfmt].style&HIDDEN && !ShowHiddenText) { //go past the hidden text
                   for (i=CurCol+1;i<LineLen(CurLine);i++) if (!(TerFont[fmt[i]].style&(HIDDEN|HLINK))) break;
                   if (i<LineLen(CurLine)) NewCfmt=fmt[i];
                }
                else if (TerFlags5&TFLAG5_BEF_AND_AFT_HIDDEN && HiddenText(w,NewCfmt)) NewCfmt=0;
             }
             else NewCfmt=fmt[CurCol-1];
          }
          CloseCfmt(w,CurLine);
       }
    }

    // exclude certain types
    FieldId=TerFont[NewCfmt].FieldId;
    if (FieldId>0  && FieldId!=FIELD_TEXTBOX
        /*&& FieldId!=FIELD_NAME && FieldId!=FIELD_DATA && FieldId!=FIELD_HLINK*/
        && HilightType!=HILIGHT_CHAR) {
       int CurCfmt=GetCurCfmt(w,CurLine,CurCol);
       if (TerFont[CurCfmt].FieldId==0) NewCfmt=GetTextFont(w,NewCfmt); // create a non-field font 
       //else if (CurCol==0) NewCfmt=0;
    }
    
    if (HilightType==HILIGHT_OFF) CurFont=GetCurCfmt(w,CurLine,CurCol);
    else                          CurFont=NewCfmt;
    
    // Text input field adjustments
    if (TerFont[CurFont].FieldId==FIELD_TEXTBOX && !TerArg.ReadOnly) { // in edit mode typing on the first field text should insert a regular character
       int PrevFont=GetPrevCfmt(w,CurLine,CurCol);  // check if we are on the field starting character
       if (TerFont[PrevFont].FieldId!=FIELD_TEXTBOX || (CurLine==0 && CurCol==0)) {
          NewCfmt=SetFontFieldId(w,CurFont,0,NULL);
          goto ADJUST;
       }
    } 
    
    if (TerFont[NewCfmt].FieldId==FIELD_TEXTBOX && TerFont[CurFont].FieldId!=FIELD_TEXTBOX) {
       if (!TerArg.ReadOnly) NewCfmt=CurFont;  // in document edit mode, let non-field character be inserted after the field
       goto ADJUST;  // let the field extent
    }
    if (TerFont[NewCfmt].FieldId!=FIELD_TEXTBOX && TerFont[CurFont].FieldId==FIELD_TEXTBOX) {
       if (TerArg.ReadOnly) NewCfmt=CurFont;  // this allow to enter text in the beginning of the textbox field in the read-only mode
       goto ADJUST;  // let the field extent
    }

    // do further validity check
    if (!IsValidInputFont(w,&NewCfmt,CurFont,CurLine,CurCol)) {
       if (TerFont[CurFont].FieldId>0) {
          NewCfmt=CurFont;
          goto ADJUST;
       } 
       CurFont=NewCfmt;
       NewCfmt=0;
       col=CurCol-1;
       l=CurLine;
       FontFound=FALSE;
       while (col>0 && PrevTextPos(w,&l,&col)) {      // check current line only
          PrevFont=GetCurCfmt(w,l,col);
          if (IsValidInputFont(w,&PrevFont,CurFont,l,col+1)) {
             NewCfmt=PrevFont;
             FontFound=TRUE;
             break;
          }
          CurFont=PrevFont;
       }
       if (!FontFound) {           // pick the first valid font on the line
          fmt=OpenCfmt(w,CurLine);
          for (i=0;i<LineLen(CurLine);i++) {
             CurFont=fmt[i];
             if (  !(TerFont[CurFont].style&(PICT|HIDDEN|FNOTEALL)) 
                && TerFont[CurFont].FieldId==0) {
                NewCfmt=CurFont;
                break;
             }
          }
          CloseCfmt(w,CurLine); 
       } 
    }

    // do font adjustment
    ADJUST:
    if (TerFont[NewCfmt].style&PICT) NewCfmt=DEFAULT_CFMT;
    PrevFont=GetPrevCfmt(w,CurLine,CurCol);
    if (HilightType==HILIGHT_OFF) {
      if (TerFont[PrevFont].FieldId==FIELD_HLINK && TerFont[NewCfmt].FieldId!=FIELD_HLINK) {
         NewCfmt=SetFontStyle(w,NewCfmt,TerFont[PrevFont].style,false);  // set normal font style
         CurFont=GetCurCfmt(w,CurLine,CurCol);
         if (TerFont[CurFont].TextColor!=TerFont[NewCfmt].TextColor) 
             NewCfmt=(int)GetNewColor(w,(WORD)NewCfmt,(DWORD)TerFont[CurFont].TextColor,0L,CurLine,CurCol);
           
      }
      
      if (TerFont[NewCfmt].FieldId==FIELD_HLINK && CurCol==0 && False(TerFont[NewCfmt].style&PROTECT)) {
         if (CurLine==0) NewCfmt=0;
         else NewCfmt=PrevFont;
      }
    }

    // check if the whole line is protected, otherwise turn-off protection
    if (CurCol==0 && True(TerFont[NewCfmt].style&PROTECT)) {  
       BOOL MayBe=false;
       if (CurLine==0) MayBe=true;   // may be unprotected, further test needed
       else {
          PrevFont=GetPrevCfmt(w,CurLine,CurCol);
          if (False(TerFont[PrevFont].style&PROTECT)) MayBe=true;
       }
       
       if (MayBe) {   // may be unprotected, do futher testing
          fmt=OpenCfmt(w,CurLine);
          len=LineLen(CurLine);
          for (i=0;i<len;i++) {
             CurFont=fmt[i];
             if (False(TerFont[CurFont].style&PROTECT)) {
                 NewCfmt=SetFontStyle(w,NewCfmt,PROTECT,false);  // turn-off protect
                 break;
             } 
          } 
          CloseCfmt(w,CurLine);
       }
    } 

    return ResetUniFont(w,NewCfmt);   // reset any unicode font specification
}

/******************************************************************************
    IsValidInputFont:
    Returns TRUE if the font id is valid for text input.
******************************************************************************/
BOOL IsValidInputFont(PTERWND w,LPINT pPrevFont,int CurFont, long LineNo, int col)
{
    UINT PrevStyle,CurStyle;
    int  PrevFont=(*pPrevFont),PrevField,CurField;

    if (col>=LineLen(LineNo)) FixPos(w,&LineNo,&col);


    START:
    PrevStyle=TerFont[PrevFont].style;
    PrevField=TerFont[PrevFont].FieldId;
    CurStyle=TerFont[CurFont].style;
    CurField=TerFont[CurFont].FieldId;


    if (PrevStyle&PICT && !(CurStyle&PICT)) {
       PrevFont=CurFont;
       goto START;
    }

    if (PrevStyle&PROTECT && !(CurStyle&PROTECT) && HilightType!=HILIGHT_CHAR) {
       PrevFont=CurFont;
       goto START;
    }
    if (PrevStyle&PROTECT && CurStyle&PROTECT) {
        if (ShowFieldNames && PrevField==FIELD_NAME && CurField==FIELD_NAME && GetCurChar(w,LineNo,col)=='{') PrevFont=0;
        //if (PrevStyle&HIDDEN) return FALSE;  // scan further backward    
        goto END;   // return PROTECT font, the text input/paste operation will stop the input
    }

    if (PrevField>0 && CurField==0) {
        BOOL ok;
        ok=(PrevField==FIELD_DATA && TerFlags3&TFLAG3_DATA_FIELD_INPUT && !ShowFieldNames);
        if (!ok) {
          PrevFont=CurFont;
          goto START;
        }
    }

    if (PrevStyle&HIDDEN) {
       if (IsHypertext(w,CurFont)) {
          PrevFont=CurFont;
          goto END;
       } 
       if (!ShowHiddenText) return FALSE;
    }
    if (ShowHyperlinkCursor && IsHypertext(w,PrevFont) && !IsHypertext(w,CurFont)) {
       PrevFont=CurFont;
       goto END;
    }    

    if (PrevStyle&FNOTETEXT && !EditFootnoteText) return FALSE;
    if (PrevStyle&ENOTETEXT && !EditEndnoteText) return FALSE;
    if (PrevStyle&(FNOTE|FNOTEREST)) return FALSE;

    if (TerFont[PrevFont].FieldId>0 && PrevField!=FIELD_NAME 
        && PrevField!=FIELD_DATA && PrevField!=FIELD_HLINK && PrevField!=FIELD_TOC) return FALSE;
    if (!ShowFieldNames && PrevField==FIELD_NAME) return FALSE;
    if (ShowFieldNames && PrevField==FIELD_DATA) {
        if (CurField==FIELD_NAME) PrevFont=0;     // use the default font to insert a character between two fields
        else return FALSE;
    }
    if (ShowFieldNames && CurField==FIELD_NAME && PrevField==FIELD_NAME) {  // check if we are on the second field name when the second field name is located right after the previous field name with any field data
       LPBYTE ptr=pText(LineNo);
       BYTE CurChar=ptr[col];
       if (CurStyle&PROTECT && CurChar=='{') PrevFont=0;   // use the default font
    }

    END:

    (*pPrevFont)=PrevFont;

    return TRUE;
}

/*****************************************************************************
    SetTerFont:
    This routine applies the requested font to the selected
    text.
*****************************************************************************/
BOOL WINAPI _export SetTerFont(HWND hWnd, LPBYTE TypeFace,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurSID>=0) {
       lstrcpy(StyleId[CurSID].TypeFace,TypeFace);   // apply to stylesheet
       return TRUE;
    }
    else return CharFmt(w,GetNewTypeFace,(DWORD)TypeFace,0L,repaint);
}

/******************************************************************************
    GetNewTypeFace:
    This routine accepts the current format data for a character.
    The routine returns new format byte that contains the new
    typeface.
******************************************************************************/
WORD GetNewTypeFace(PTERWND w,WORD OldFmt,DWORD data1,DWORD data2, long line,int col)
{
    int CurFont,NewFont;
    UINT style;
    LPBYTE TypeFace;
    BYTE CharSet;

    TypeFace=(LPBYTE)data1;                // pointer to the typeface string
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style
    CharSet=TerFont[CurFont].CharSet;
    if (CharSet==SYMBOL_CHARSET) CharSet=DEFAULT_CHARSET;  // let charset be decided in the CreateOneFont function

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TypeFace,TerFont[CurFont].TwipsSize,style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // apply the new font
    }
    else return OldFmt;
}


/*****************************************************************************
    SetTerPointSize:
    This routine applies the requested point size to the selected
    text. Specify a negative value to the font size in twips.
*****************************************************************************/
BOOL WINAPI _export SetTerPointSize(HWND hWnd, int PointSize,BOOL repaint)
{
    PTERWND w;
    int TwipsSize;
    long l;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (PointSize>=0) TwipsSize=PointSize*20;
    else              TwipsSize=-PointSize;

    if (CurSID>=0) {
        StyleId[CurSID].TwipsSize=TwipsSize;
        return TRUE;
    }
    else {
       BOOL result=CharFmt(w,GetNewPointSize,(DWORD)TwipsSize,0L,repaint);

       if (HilightType!=HILIGHT_OFF) {     // check if repagination needed
          for (l=HilightBegRow;l<=HilightEndRow;l++) {
             if (IsBaselineAlignedCellLine(w,l)) {
                if (l<RepageBeginLine) RepageBeginLine=l;
                RequestPagination(w,FALSE);
                break;
             } 
          } 
       } 

       return result;
    }
}

/******************************************************************************
    GetNewPointSize:
    This routine accepts the current format data for a character.
    The routine returns new format byte that contains the new
    point size.
******************************************************************************/
WORD GetNewPointSize(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2, long line,int col)
{
    int CurFont,NewFont;
    UINT style;
    int  TwipsSize;

    TwipsSize=(int)data1;                 // new point size to apply
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TwipsSize,style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // apply the new font
    }
    else return OldFmt;
}

/******************************************************************************
    GetNewFieldId:
    This routine accepts the current format data for a character.
    The routine returns new format byte that contains the new field id.
******************************************************************************/
WORD GetNewFieldId(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2, long line,int col)
{
    int CurFont,NewFont;
    UINT style;
    int  FieldId;
    LPBYTE FieldCode=(LPBYTE)data2;

    FieldId=(int)data1;                 // new point size to apply
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // apply the new font
    }
    else return OldFmt;
}

/******************************************************************************
    GetNewCharSet:
    This routine accepts the current format data for a character.
    The routine returns new format byte that contains the new field id.
******************************************************************************/
WORD GetNewCharSet(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2, long line,int col)
{
    int CurFont,NewFont;
    UINT style;
    BYTE  CharSet=(BYTE)data1;

    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // apply the new font
    }
    else return OldFmt;
}

/******************************************************************************
    SetPointSizes:
    This function retrieves the available point sizes for the given font
    type face.  The point sizes are inserted into the dialog list box.
    The returns the number of point sizes avaialble.
******************************************************************************/
int SetPointSizes(PTERWND w,HWND hDlg,LPBYTE typeface,int CurTwipsSize)
{
    int  TerFontPoints[16]={4,6,7,8,9,10,11,12,14,16,18,20,24,30,36,72};      // size for the current type faces
    struct StrEnumFonts far * font;
    int  i,idx,ResY,PointSize,CurPointSize=CurTwipsSize/20,id;
    OLDFONTENUMPROC lpProc;
    BYTE string[30];

    id=(hDlg==hToolBarWnd)?TLB_POINTSIZE:IDC_FONT_POINTS;

    // allocate the font structure
    if (NULL==(font=OurAlloc(sizeof(struct StrEnumFonts)))) {
       PrintError(w,MSG_OUT_OF_MEM,"SetPointSizes");
       return 0;
    }

    font->GetMultiple=TRUE;              // enumerate all
    font->EnumTypeFace=FALSE;            // enumerate pointsizes instead
    font->PointCount=font->TypeCount=TotalPointSizes=0;
    lpProc = (OLDFONTENUMPROC) MakeProcInstance((FARPROC)TerEnumFonts, hTerInst);
    if (lstrlen(typeface)>0) EnumFonts(hTerDC,typeface,lpProc,(LPARAM)(LPBYTE)font);
    else                     EnumFonts(hTerDC,TerFont[0].TypeFace,lpProc,(LPARAM)(LPBYTE)font);  // to handle stylesheet fonts
    FreeProcInstance((FARPROC)lpProc);   // set back the old window

    SendDlgItemMessage(hDlg,id,CB_RESETCONTENT, 0, 0L); // clear list box

    if (font->TrueType || font->PointCount==0) { // show our list of point sizes
       TotalPointSizes=MAX_POINTS;
       for (i=0;i<TotalPointSizes;i++) {// add names to the list box
          wsprintf(string,"%2d",TerFontPoints[i]);
          idx=(int)SendMessage(GetDlgItem(hDlg,id),CB_ADDSTRING, 0, (DWORD)(LPBYTE)string);
          if ((TerFontPoints[i]*20)==CurTwipsSize) 
             SendMessage(GetDlgItem(hDlg,id),CB_SETCURSEL, idx,0L); // default point size 12
       }
    }
    else {                              // show available list of point sizes
       TotalPointSizes=font->PointCount;
       ResY=GetDeviceCaps(hTerDC,LOGPIXELSY);

       for (i=0;i<TotalPointSizes;i++) {
          PointSize=MulDiv(font->height[i],72,ResY);
          wsprintf(string,"%2d",PointSize);
          idx=(int)SendMessage(GetDlgItem(hDlg,id),CB_ADDSTRING, 0, (DWORD)(LPBYTE)string);
          if ((PointSize*20)==CurTwipsSize) 
             SendMessage(GetDlgItem(hDlg,id),CB_SETCURSEL, idx,0L); // default point size 12
       }

    }

    if (CurPointSize==0) string[0]=0;
    else {
       wsprintf(string,"%2d",CurPointSize);
       if ((CurPointSize*20)<CurTwipsSize) lstrcat(string,".5");   // show fractional pointsize
       if (CurPointSize==0) string[0]=0;
       SetDlgItemText(hDlg,id,string);
    }

    OurFree(font);                      // freeup the font structure

    return TotalPointSizes;
}

/****************************************************************************
    TerSetCharSet:
    Set a character set to use for creating the fonts.  Use use the default
    character set, set the CharSet to DEFAULT_CHARSET (or 1).
****************************************************************************/
BOOL WINAPI _export TerSetCharSet(HWND hWnd,BYTE CharSet)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    ReqCharSet=CharSet;

    return TRUE;
}

/******************************************************************************
    TerEnumFontFamiliesEx:
    Callback function enumerate Windows font type faces.
    The lpData argument returns the output in the StrEnumFonts structure.
******************************************************************************/
int CALLBACK _export TerEnumFontFamiliesEx(LPENUMLOGFONTEX lpEnumLogFont,NEWTEXTMETRICEX far *lpNewTextMetricsEx,DWORD nFontType,LPARAM lpData)
{
   struct StrEnumFonts far *font;
   int i;
   LPLOGFONT lpLogFont;
   LPNEWTEXTMETRIC lpNewTextMetrics;

   font=(struct StrEnumFonts far *)lpData;
   
   lpLogFont=&(lpEnumLogFont->elfLogFont);
   lpNewTextMetrics=&(lpNewTextMetricsEx->ntmTm);

   if (!(font->GetMultiple)) {
      font->TypeCount=0;  // get only first
      font->PointCount=0; // get only first
   }

   if (font->TypeCount>=MAX_TYPES
    || font->PointCount>=MAX_POINTS) {
      font->result=FALSE;
      return 0;  // end enumeration
   }
   else font->result=TRUE;                     // set success flag

   // fill font data
   font->CharSet=lpLogFont->lfCharSet;        // return character set
   if (lstrcmpi(lpLogFont->lfFaceName,"Symbol")==0) font->CharSet=SYMBOL_CHARSET;
   font->PitchAndFamily=lpLogFont->lfPitchAndFamily;

   if (nFontType&RASTER_FONTTYPE) font->TrueType=FALSE;
   else                           font->TrueType=TRUE;

   if (font->GetMultiple) {
      if (font->EnumTypeFace) {
         for (i=0;i<font->TypeCount;i++) if (lstrcmpi(font->TypeNames[i],lpLogFont->lfFaceName)==0) break;
         if (i==font->TypeCount) {    // new typeface
            lstrcpy(font->TypeNames[font->TypeCount],lpLogFont->lfFaceName);
            font->TypeCount++;
         }
      }
      else {
         // check if this point already exists
         for (i=0;i<font->PointCount;i++) {
            if (font->height[i]==(lpNewTextMetrics->tmHeight-lpNewTextMetrics->tmInternalLeading)) break;  // compare pointsize
         }
         if (i==font->PointCount) {
            font->height[font->PointCount]=lpNewTextMetrics->tmHeight-lpNewTextMetrics->tmInternalLeading;
            font->PointCount++;
         }
      }
   }
   else {
      lstrcpy(font->TypeNames[font->TypeCount],lpLogFont->lfFaceName);  // return the typeface actually found
      font->TypeCount++;
   } 

   if (font->GetMultiple) return 1;   // continue enumeration
   else                   return 0;   // end enumeration
}


/******************************************************************************
    TerEnumFonts:
    Callback function enumerate Windows font type faces.
    The lpData argument returns the output in the StrEnumFonts structure.
******************************************************************************/
int CALLBACK _export TerEnumFonts(LPLOGFONT lpLogFont,LPTEXTMETRIC lpTextMetrics,int nFontType,LPBYTE lpData)
{
   struct StrEnumFonts far *font;
   int i;

   font=(struct StrEnumFonts far *)lpData;


   if (!(font->GetMultiple)) {
      font->TypeCount=0;  // get only first
      font->PointCount=0; // get only first
   }

   if (font->TypeCount>=MAX_TYPES
    || font->PointCount>=MAX_POINTS) {
      font->result=FALSE;
      return 0;  // end enumeration
   }
   else font->result=TRUE;                     // set success flag

   // fill font data
   font->CharSet=lpLogFont->lfCharSet;        // return character set
   if (lstrcmpi(lpLogFont->lfFaceName,"Symbol")==0) font->CharSet=SYMBOL_CHARSET;

   font->PitchAndFamily=lpLogFont->lfPitchAndFamily;

   if (nFontType&RASTER_FONTTYPE) font->TrueType=FALSE;
   else                           font->TrueType=TRUE;

   if (font->GetMultiple) {
      if (font->EnumTypeFace) {
         for (i=0;i<font->TypeCount;i++) if (lstrcmpi(font->TypeNames[i],lpLogFont->lfFaceName)==0) break;
         if (i==font->TypeCount) {    // new typeface
            lstrcpy(font->TypeNames[font->TypeCount],lpLogFont->lfFaceName);
            font->TypeCount++;
         }
      }
      else {
         // check if this point already exists
         for (i=0;i<font->PointCount;i++) {
            if (font->height[i]==(lpTextMetrics->tmHeight-lpTextMetrics->tmInternalLeading)) break;  // compare pointsize
         }
         if (i==font->PointCount) {
            font->height[font->PointCount]=lpTextMetrics->tmHeight-lpTextMetrics->tmInternalLeading;
            font->PointCount++;
         }
      }
   }
   else {
      lstrcpy(font->TypeNames[font->TypeCount],lpLogFont->lfFaceName);  // return the typeface actually found
      font->TypeCount++;
   } 

   if (font->GetMultiple) return 1;   // continue enumeration
   else                   return 0;   // end enumeration
}


/******************************************************************************
    GetCharSet:
    Get the character set for a type face.
    The routine also returns the pitch and family through the second
    argument.
******************************************************************************/
BYTE GetCharSet(PTERWND w,HDC hDC,LPBYTE TypeFace,BYTE far *PitchFamily, BOOL far *TrueType, BOOL far *pFontExists)
{
    OLDFONTENUMPROC lpProc;
    BYTE CharSet,pf;
    struct StrEnumFonts far * font;

    // allocate the font structure
    if (NULL==(font=OurAlloc(sizeof(struct StrEnumFonts)))) {
       PrintError(w,MSG_OUT_OF_MEM,"GetCharSet");
       return 0;
    }

    // get the character set, pitch, family
    font->GetMultiple=FALSE;  // enumerate the first matching font
    font->result=FALSE;
    if (PitchFamily) pf=(*PitchFamily); // initialize our variable


    lpProc = (OLDFONTENUMPROC) MakeProcInstance((FARPROC)TerEnumFonts, hTerInst);
    EnumFonts(hDC,TypeFace,lpProc,(LPARAM)(LPBYTE)font);

    if (font->result) {       // enumeration succeeded
       CharSet=font->CharSet;
       pf=font->PitchAndFamily;
    }
    else if (hDC!=hTerDC) {  // use the display fonts
       EnumFonts(hTerDC,TypeFace,lpProc,(LPARAM)(LPBYTE)font);
       if (font->result) {    // enumeration succeeded
          CharSet=font->CharSet;
          pf=font->PitchAndFamily;
       }
       else {                // use defaults
          if (lstrcmpi(TypeFace,"Symbol")==0) CharSet=SYMBOL_CHARSET;
          else                                CharSet=ANSI_CHARSET;
          pf=DEFAULT_PITCH|FF_DONTCARE;
       }
    }
    else {                     // use defaults
       if (lstrcmpi(TypeFace,"Symbol")==0) CharSet=SYMBOL_CHARSET;
       else                                CharSet=ANSI_CHARSET;
       pf=DEFAULT_PITCH|FF_DONTCARE;
    }

    if (CharSet!=SYMBOL_CHARSET && ReqCharSet!=DEFAULT_CHARSET) CharSet=ReqCharSet;

    if (PitchFamily) (*PitchFamily)=pf;

    // pass TrueType result
    if (TrueType) {
       if (font->result && font->TrueType) (*TrueType)=TRUE;
       else                                (*TrueType)=FALSE;
    }

    FreeProcInstance((FARPROC)lpProc);  // set back the old window

    if (pFontExists) (*pFontExists)=(strcmpi(TypeFace,font->TypeNames[0])==0);

    OurFree(font);                      // freeup the font structure

    return CharSet;
}

/******************************************************************************
    IsDynField:
    This function returns TRUE if a field is a dynamic field such as page number, 
    page count, date/time, etc.
******************************************************************************/
BOOL IsDynField(PTERWND w,int FieldId)
{
    return (FieldId==FIELD_PAGE_NUMBER || FieldId==FIELD_PAGE_COUNT || FieldId==FIELD_PAGE_REF
            || FieldId==FIELD_DATE   || FieldId==FIELD_PRINTDATE || FieldId==FIELD_LISTNUM 
            || FieldId==FIELD_AUTONUMLGL);
}
    
/******************************************************************************
    TerGetCharWidth:
    Retrieve the character width table for a font.
******************************************************************************/
BOOL TerGetCharWidth(PTERWND w,HDC hDC, int NewFont, BOOL ScreenFont, long overhang, BYTE PitchAndFamily)
{
    int i,UcBase;
    HFONT hOldFont;
    int  adj,expand,width;

    if (NULL==(hOldFont=SelectObject(hDC,TerFont[NewFont].hFont))) return FALSE;

    UcBase=TerFont[NewFont].UcBase;
    
    if (UcBase!=0 && Win32) {    // get char width for Unicode characters
       BOOL done=false;
       FarMemSet(TerFont[NewFont].CharWidth,0,256*sizeof(int));  // lower 125 characters get 0 width
       
       if ((GetDeviceCaps(hDC,TECHNOLOGY)!=DT_RASDISPLAY || PitchAndFamily&TMPF_TRUETYPE)) {
          int FirstChar=MakeUnicode(w,(WORD)UcBase,128);
          int LastChar=MakeUnicode(w,(WORD)UcBase,255);
          if (GetCharWidthW(hDC,FirstChar,LastChar,&(TerFont[NewFont].CharWidth[128]))) done=true;
          if (done) { // Check size against GetTextExtentPointW and adjust.
             SIZE  size;
             int extra;
             int FirstChar=MakeUnicode(w,(WORD)UcBase,128);
             GetTextExtentPointW(hDC,(LPCWSTR)&FirstChar,1,&size);
             extra = size.cx - TerFont[NewFont].CharWidth[128]-overhang;
             for (i=128;i<256;i++) TerFont[NewFont].CharWidth[i] += extra;  // approximate fix - precise width is calculated at wrap time when TFLAG6_ALT_UNI_CHARWIDTh flag is set
          } 
       } 
         
       if (False(TerFlags6&TFLAG6_ALT_UNI_CHARWIDTH) && !done) {
          for (i=128;i<256;i++) {
             TerFont[NewFont].CharWidth[i]=TerGetCharWidthAltW(w,hDC,NewFont,MakeUnicode(w,(WORD)UcBase,(BYTE)i),overhang);
          }
       }
    } 
    // use GetCharWidth for screen true type fonts or any printer font
    else if (   (GetDeviceCaps(hDC,TECHNOLOGY)!=DT_RASDISPLAY || PitchAndFamily&TMPF_TRUETYPE)
        && GetCharWidth(hDC,0,255,TerFont[NewFont].CharWidth)) {
        for (i=0;i<256;i++) {                             // adjust the character width table
            TerFont[NewFont].CharWidth[i]=TerFont[NewFont].CharWidth[i]-overhang;
        }
    }
    else {      // try alternate method now for screen bitmap font or when GetDeviceCaps fails
        int WidthA,WidthB;

        // check if the width already calculated
        WidthA=TerGetCharWidthAlt(w,hDC,NewFont,(BYTE)'A',overhang);
        WidthB=TerGetCharWidthAlt(w,hDC,NewFont,(BYTE)'B',overhang);

        if (WidthA!=TerFont[NewFont].CharWidth['A'] || WidthB!=TerFont[NewFont].CharWidth['B']) {
            TerFont[NewFont].CharWidth[0]=0;
            for (i=1;i<256;i++) {
               TerFont[NewFont].CharWidth[i]=TerGetCharWidthAlt(w,hDC,NewFont,(BYTE)i,overhang);
            }
        }
    }

    // apply font expansion
    expand=TerFont[NewFont].expand;
    if (expand) {
       if (expand>MAX_EXPAND) expand=MAX_EXPAND;

       if (/*ScreenFont*/ GetDeviceCaps(hDC,TECHNOLOGY)==DT_RASDISPLAY)
             expand=TwipsToScrX(expand);
       else  expand=TwipsToPrtX(expand);

       for (i=0;i<256;i++) {
          adj=expand;
          width=TerFont[NewFont].CharWidth[i];
          if (adj<0 && (-adj)>width/2) adj=-width/2;

          TerFont[NewFont].CharWidth[i]+=adj;
       }
    }

    // translate the character width for para and cell markers, and non-breaking space character
    TerFont[NewFont].CharWidth[0]=0;   // to work with zero lead bytes
    TerFont[NewFont].CharWidth[(BYTE)ParaChar]=TerFont[NewFont].CharWidth[(BYTE)((TerFlags2&TFLAG2_ALT_PARA_SYM)?DISP_PARA_CHAR_ALT:DISP_PARA_CHAR)];
    TerFont[NewFont].CharWidth[(BYTE)LINE_CHAR]=TerFont[NewFont].CharWidth[(BYTE)((TerFlags2&TFLAG2_ALT_LINE_SYM)?DISP_LINE_CHAR_ALT:DISP_LINE_CHAR)];
    TerFont[NewFont].CharWidth[(BYTE)CellChar]=TerFont[NewFont].CharWidth[(BYTE)DISP_CELL_CHAR];
    TerFont[NewFont].CharWidth[(BYTE)NBSPACE_CHAR]=TerFont[NewFont].CharWidth[(BYTE)' '];
    TerFont[NewFont].CharWidth[(BYTE)NBDASH_CHAR]=TerFont[NewFont].CharWidth[(BYTE)'-'];

    if (ShowParaMark && !InPrinting) TerFont[NewFont].CharWidth[(BYTE)HYPH_CHAR]=TerFont[NewFont].CharWidth[(BYTE)DISP_HYPH_CHAR];
    else  TerFont[NewFont].CharWidth[(BYTE)HYPH_CHAR]=0;

    if (ShowParaMark && !InPrinting) TerFont[NewFont].CharWidth[(BYTE)HIDDEN_CHAR]=TerFont[NewFont].CharWidth[(BYTE)DISP_HIDDEN_CHAR];
    else  TerFont[NewFont].CharWidth[(BYTE)HIDDEN_CHAR]=0;


    SelectObject(hDC,hOldFont);                  // restore the previous font

    return TRUE;
}

/******************************************************************************
    TerGetCharWidthAltW:
    Retrieve the character width table for a font using GetTextExtPoint method
******************************************************************************/
int TerGetCharWidthAltW(PTERWND w,HDC hDC, int NewFont, WORD chr, int overhang)
{
#if defined (WIN32) 
    int j, width,SingleWidth,MultiWidth,count=10;
    SIZE  size;
    WORD  string[11];
    BOOL  UseSingleWidth=FALSE;

    // get the width of one character
    for (j=0;j<1;j++) string[j]=chr;  // get 10 characters to average
    string[j]=0;
    GetTextExtentPointW(hDC,string,j,&size);
    SingleWidth=size.cx;

    // get the width of one character
    for (j=0;j<count;j++) string[j]=chr;  // get 10 characters to average
    string[j]=0;
    GetTextExtentPointW(hDC,string,j,&size);
    MultiWidth=size.cx;

    if (abs(MultiWidth-SingleWidth*count)>SingleWidth) UseSingleWidth=TRUE;  // context sensitive width is affecting multi width, so use single width

    if (UseSingleWidth) return (SingleWidth-overhang);

    size.cx-=overhang;          // reduce by the overhang
    
    width=size.cx/j;
    if ((size.cx -(width*j))>=(j/2)) width++;

    return width;
#else
    return 0;
#endif
}

/******************************************************************************
    TerGetCharWidthAlt:
    Retrieve the character width table for a font using GetTextExtPoint method
******************************************************************************/
int TerGetCharWidthAlt(PTERWND w,HDC hDC, int NewFont, BYTE chr, int overhang)
{
    int j, width;
    SIZE  size;

    for (j=0;j<10;j++) TempString[j]=(BYTE)chr;  // get 10 characters to average
    TempString[j]=0;
    GetTextExtentPoint(hDC,TempString,j,&size);
    size.cx-=overhang;          // reduce by the overhang
    width=size.cx/j;
    if ((size.cx -(width*j))>=(j/2)) width++;

    return width;
}

/******************************************************************************
    IsValidBookmarkk:
    This function returns TRUE if the bookmark name is valid.  If the 'exists'
    parameter is TRUE then it check that the bookmark is an existing bookmark.
******************************************************************************/
BOOL IsValidBookmark(PTERWND w, LPBYTE name, BOOL exists)
{
    int i,len;

    len=lstrlen(name);
    if (len==0) return FALSE;
    for (i=0;i<len;i++) if (name[i]==' ') return FALSE;  // invalid character in the bookmark
    if (!exists) return TRUE;

    // check if the bookmark exists in the table
    for (i=1;i<TotalCharTags;i++) {
       if (CharTag[i].InUse && CharTag[i].type==TERTAG_BKM && lstrcmp(CharTag[i].name,name)==0) return TRUE;
    }

    return FALSE;
}

/******************************************************************************
    TerGetBookmark:
    Retrieve the name of the bookmark given its index. If the index is set to
    -1, this function returns the total number of bookmark after updating
    the bookmark table.
******************************************************************************/
int WINAPI _export TerGetBookmark(HWND hWnd, int index, LPBYTE name)
{
    PTERWND w;
    int count=0,i,j;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (index<0) UpdateTagTable(w); // update the bookmark table


    for (i=1;i<TotalCharTags;i++) if (CharTag[i].InUse && CharTag[i].type==TERTAG_BKM) count++;
    if (count==0 || index<0 || !name) return count;
    if (index>=count) return 0;

    // retrieve the bookmark name
    j=-1;
    name[0]=0;
    for (i=1;i<TotalCharTags;i++) {
      if (!CharTag[i].InUse || CharTag[i].type!=TERTAG_BKM) continue;
      j++;
      if (j==index) {
         lstrcpy(name,CharTag[i].name);
         break;
      }
    }

    return count;
}

/******************************************************************************
    InsertBookmark:
    Show dialog box and insert a bookmark.
******************************************************************************/
InsertBookmark(PTERWND w)
{
    BYTE name[MAX_WIDTH+1];

    if (!CallDialogBox(w,"BookmarkParam",BookmarkParam,0L)) return FALSE;

    lstrcpy(name,TempString);   // bookmark name

    if      (DlgInt1==IDC_INSERT) TerInsertBookmark(hTerWnd,-1,0,name);
    else if (DlgInt1==IDC_DELETE) TerDeleteBookmark(hTerWnd,name);
    else if (DlgInt1==IDC_GOTO)   TerPosBookmark(hTerWnd,name,TRUE);
    else if (DlgInt1==IDC_PAGE_REF) TerInsertPageRef(hTerWnd,name,true,false,true);

    return TRUE;
}

/******************************************************************************
    TerInsertBookmark:
    Insert a specified bookmark.  If a bookmark with the same name already
    exists, then it is deleted from the current location.  Set line to -1
    to use the current location.
    This function returns the bookmark id.  It returns 0 to indicate an error condition.
******************************************************************************/
int WINAPI _export TerInsertBookmark(HWND hWnd, long line, int col, LPBYTE name)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!IsValidBookmark(w,name,FALSE)) return 0;

    TerDeleteBookmark(hWnd,name);  // delete the existing bookmark

    return SetTag(w,line,col,TERTAG_BKM,name,NULL,0L);
}

/******************************************************************************
    TerPosBookmark:
    Position at the specified bookmark. This function returns TRUE if the given
    bookmark was found.  Otherwise it returns FALSE.
******************************************************************************/
BOOL WINAPI _export TerPosBookmark(HWND hWnd, LPBYTE name, BOOL repaint)
{
    PTERWND w;
    long SaveCurLine,PosLine;
    int  SaveCurCol;
    BOOL deleted=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // save the CurLine and CurCol which is modified by PosTag
    SaveCurLine=CurLine;
    SaveCurCol=CurCol;

    if (!PosTag(w,-1,name,TERTAG_BKM,SCOPE_ANY,FALSE)) return FALSE;

    if (repaint) {
      PosLine=CurLine;
      CurLine=SaveCurLine;
      TerPosLine(w,PosLine+1);
    }

    return TRUE;
}

/******************************************************************************
    TerDeleteBookmark:
    Delete the specified bookmark. This function returns TRUE if the given
    bookmark was found and deleted successfully.  Otherwise it returns 0.
******************************************************************************/
BOOL WINAPI _export TerDeleteBookmark(HWND hWnd, LPBYTE name)
{
    PTERWND w;
    long SaveCurLine;
    int  SaveCurCol;
    BOOL deleted=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // save the CurLine and CurCol which is modified by PosTag
    SaveCurLine=CurLine;
    SaveCurCol=CurCol;

    while (PosTag(w,-1,name,TERTAG_BKM,SCOPE_BEGIN,FALSE)) {
       deleted=DeleteTag(w,CurLine,CurCol,TERTAG_BKM,name);
    }

    CurLine=SaveCurLine;
    CurCol=SaveCurCol;

    return deleted;
}

/******************************************************************************
    TerSetInitTypeface:
    Set the initial type face.
******************************************************************************/
BOOL WINAPI _export TerSetInitTypeface(LPBYTE typeface)
{
    lstrcpy(InitFontFace,typeface);

    return TRUE;
}

/******************************************************************************
    TerPosTag:
    Postition the cursor at the specified tag.  If 'name' value is specified,
    then the name and type are used instead of the tag id field.  The 'scope' constant
    can be one of SCOPE_ values
******************************************************************************/
BOOL WINAPI _export TerPosTag(HWND hWnd,int TagId, LPBYTE name,int scope, BOOL repaint)
{

    return TerPosTagEx(hWnd,TERTAG_USER,TagId,name,scope,repaint);
}

/******************************************************************************
    TerPosTagEx:
    Postition the cursor at the specified tag.  If 'name' value is specified,
    then the name and type are used instead of the tag id field.  The 'scope' constant
    can be one of SCOPE_ values
******************************************************************************/
BOOL WINAPI _export TerPosTagEx(HWND hWnd,int TagType, int TagId, LPBYTE name,int scope, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return PosTag(w,TagId,name,TagType,scope,repaint);
}

/******************************************************************************
    PosTag:
    Postition the cursor at the specified tag.  If 'name' value is specified,
    then the name and type are used instead of the tag id field. If type is
    -1 then any type tag is matched.
    The 'scope' constant can be one of SCOPE_ values
******************************************************************************/
BOOL PosTag(PTERWND w,int TagId, LPBYTE name,int type, int scope, BOOL repaint)
{
    LPWORD ct;
    long l,StartLine;
    BOOL  SearchName=FALSE,forward;
    int i,len,tag,StartCol;

    if (scope==SCOPE_ANY) {
       if (PosTagQuick(w,TagId,name,type,repaint)) return true;
       else scope=SCOPE_BEGIN;   // start from the beginning of the file
    } 

    if (name && lstrlen(name)) SearchName=TRUE;

    // set the scope of the search
    if (scope==SCOPE_BEGIN) {  // beginning of the file
       StartLine=0;
       StartCol=0;
       scope=SCOPE_FORWARD;
    }
    else {
       StartLine=CurLine;
       StartCol=CurCol;
    }
    forward=(scope==SCOPE_FORWARD);

    if (forward) {
       for (l=StartLine;l<TotalLines;l++) {
         if (!pCtid(l)) continue;
         ct=pCtid(l);
         len=LineLen(l);
         for (i=((l==StartLine)?StartCol:0);i<len;i++) {
            tag=ct[i];
            while (TRUE) {
               if (!tag) break;            // no match
               if (SearchName) {
                 if ((type==-1 || (int)CharTag[tag].type==type)
                   && CharTag[tag].name && lstrcmp(CharTag[tag].name,name)==0) break;
               }
               else if (tag==TagId) break;
               tag=CharTag[tag].next;
            }
            if (tag) break;                // found
         }
         if (i<len) break;
       }
       if (l==TotalLines) return FALSE;      // tag not found
    }
    else {                                   // backward search
       for (l=StartLine;l>=0;l--) {
         if (!pCtid(l)) continue;
         ct=pCtid(l);
         len=LineLen(l);
         for (i=((l==StartLine)?StartCol:len-1);i>=0;i--) {
            tag=ct[i];
            while (TRUE) {
               if (!tag) break;    // no match
               if (SearchName) {
                 if ((type==-1 || (int)CharTag[tag].type==type)
                   && CharTag[tag].name && lstrcmp(CharTag[tag].name,name)==0) break;
               }
               else if (tag==TagId) break;
               tag=CharTag[tag].next;
            }
            if (tag) break;        // found
         }
         if (i>=0) break;
       }
       if (l<0) return FALSE;      // tag not found
    }

    CurCol=i;

    if (repaint) TerPosLine(w,l+1);
    else         CurLine=l;

    return TRUE;
}

/******************************************************************************
    PosTagQuick:
    Postition the cursor at the specified tag using the approxmate 'line' postion
    value available with the tag.
******************************************************************************/
BOOL PosTagQuick(PTERWND w,int TagId, LPBYTE name,int type, BOOL repaint)
{
    long line,TempLine;
    int i,col;

    if (TagId==-1 && name && lstrlen(name)) {   // find the tag id
       for (i=0;i<TotalCharTags;i++) if (CharTag[i].type==type && True(CharTag[i].name) && strcmp(CharTag[i].name,name)==0) break;
       if (i==TotalCharTags) return false;
       TagId=i;
    } 
    if (TagId<0 || TagId>=TotalCharTags) return false;

    line=CharTag[TagId].line;
    if (line<0 || line>=TotalLines) return false;

    TempLine=line;
    if ((col=PosTagLine(w,TagId,TempLine))>=0) goto FOUND;  // check the current line
    
    TempLine=line-1;
    if ((col=PosTagLine(w,TagId,TempLine))>=0) goto FOUND;  // check the line before

    TempLine=line+1;
    if ((col=PosTagLine(w,TagId,TempLine))>=0) goto FOUND;  // check the line after

    TempLine=line-2;
    if ((col=PosTagLine(w,TagId,TempLine))>=0) goto FOUND;  // check the line before

    TempLine=line+2;
    if ((col=PosTagLine(w,TagId,TempLine))>=0) goto FOUND;  // check the line after

    return false;

    FOUND:

    CurCol=col;

    if (repaint) TerPosLine(w,TempLine+1);
    else         CurLine=TempLine;

    return true;
}

/******************************************************************************
    PosTagLine:
    Check if a tag id exists in the reguested line.  If so, return the column  number
    else return -1.
******************************************************************************/
int PosTagLine(PTERWND w,int TagId, long line)
{
    LPWORD ct;
    int i,len,tag;

    if (line<0 || line>=TotalLines) return -1;

    if (!pCtid(line)) return -1;

    ct=pCtid(line);
    len=LineLen(line);
    for (i=0;i<len;i++) {
       tag=ct[i];
       while (TRUE) {
          if (!tag) break;    // no match
          if (tag==TagId) return i;    // return the column position
          tag=CharTag[tag].next;
       }
    }

    return -1;
}

/******************************************************************************
    JumpToPageRefBookmark:
    Check if a 'pageref' bookmark exists at the current location.  If so, jump
    to this location.
******************************************************************************/
BOOL JumpToPageRefBookmark(PTERWND w, BOOL repaint)
{
    int CurFont=GetCurCfmt(w,CurLine,CurCol);
    BYTE name[MAX_WIDTH+1];
    int i,len;

    if (TerFont[CurFont].FieldId!=FIELD_PAGE_REF || TerFont[CurFont].FieldCode==null) return false;

    len=lstrlen(TerFont[CurFont].FieldCode);

    for (i=0;i<len;i++) {
       name[i]=TerFont[CurFont].FieldCode[i];
       if (name[i]==' ') break;
    }
    name[i]=0;

    return PosTag(w,0,name,TERTAG_BKM,SCOPE_ANY,repaint);
}  

/******************************************************************************
    TerGetTag:
    Get the tag at the specified location.  Set 'line' to -1 to use the
    current position.  This function returns a tag id if successful, otherwise
    it return 0.
******************************************************************************/
int WINAPI _export TerGetTag(HWND hWnd,long line, int col, LPBYTE name, LPBYTE AuxText, LPLONG AuxInt, LPINT flags)
{
    return TerGetTagEx(hWnd,line,col,TERTAG_USER,name,AuxText,AuxInt,flags);
}

/******************************************************************************
    TerGetTagEx:
    Get the tag at the specified location.  Set 'line' to -1 to use the
    current position.  This function returns a tag id if successful, otherwise
    it return 0.
******************************************************************************/
int WINAPI _export TerGetTagEx(HWND hWnd,long line, int col, int TagType, LPBYTE name, LPBYTE AuxText, LPLONG AuxInt, LPINT flags)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (flags) (*flags)=0;

    return GetTag(w,line,col,TagType,name,AuxText,AuxInt);
}

/******************************************************************************
    GetTag:
    Get the tag at the specified location.  Set 'line' to -1 to use the
    current position.  Set col to -1 to specify a tag id using the line parameter.
    This function returns a tag id if successful, otherwise it return 0.
******************************************************************************/
int GetTag(PTERWND w,long line, int col, int type, LPBYTE name, LPBYTE AuxText, LPLONG AuxInt)
{
    int TagId;
    LPWORD ct;

    if (line<0) {
       line=CurLine;
       col=CurCol;
    }
    if (col>=0) {
      if (line<0 || line>=TotalLines) return 0;
      if (col>=LineLen(line)) return 0;
    }  


    if (col<0) TagId=(int)line;
    else {
      ct=OpenCtid(w,line);
      TagId=ct[col];
      CloseCtid(w,line);
    }

    if (TagId==0) return TagId;

    // traverse the tag sequence to get the tag of the given type
    while (TRUE) {
       if ((int)CharTag[TagId].type==type) break;
       TagId=CharTag[TagId].next;
       if (TagId==0) return 0;
    }

    if (name)    name[0]=0;
    if (AuxText) AuxText[0]=0;
    if (AuxInt)  (*AuxInt)=0;

    if (CharTag[TagId].name && name) lstrcpy(name,CharTag[TagId].name);
    if (CharTag[TagId].AuxText && AuxText)  lstrcpy(AuxText,CharTag[TagId].AuxText);
    if (AuxInt)  (*AuxInt)=CharTag[TagId].AuxInt;

    return TagId;
}

/******************************************************************************
    TerDeleteTag:
    Delete the specified bookmark. This function returns TRUE if the given
    bookmark was found and deleted successfully.  Otherwise it returns 0.
******************************************************************************/
int WINAPI _export TerDeleteTag(HWND hWnd, long line, int col, int type, LPBYTE name)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    return DeleteTag(w,line,col,type,name);
}
 
/******************************************************************************
    DeleteTag:
    Delete a tag at the specified location.  Set 'line' to -1 to use the
    current position.  This function returns a tag id of the deleted tag, otherwise
    it return 0.
******************************************************************************/
int DeleteTag(PTERWND w,long line, int col, int type, LPBYTE name)
{
    int TagId,PrevTag=0,NextTag;
    LPWORD ct;

    if (line<0) {
       line=CurLine;
       col=CurCol;
    }
    if (line<0 || line>=TotalLines) return 0;
    if (col<0 || col>=LineLen(line)) return 0;

    ct=OpenCtid(w,line);
    TagId=ct[col];

    if (TagId>=TotalCharTags) goto END;
    if (TagId==0) goto END;

    // traverse the tag sequence to get the tag of the given type
    PrevTag=0;
    while (TRUE) {
       if ( (type==-1 || (int)CharTag[TagId].type==type)
         && (!name || lstrcmp(CharTag[TagId].name,name)==0)) break;
       PrevTag=TagId;
       TagId=CharTag[TagId].next;
       if (TagId==0) goto END;
    }

    FreeTag(w,TagId);

    // remove from the chain
    NextTag=CharTag[TagId].next;
    if (PrevTag) CharTag[PrevTag].next=NextTag;
    else ct[col]=NextTag;

    END:

    CloseCtid(w,line);
    TerArg.modified++;

    return TagId;
}

/******************************************************************************
    TerSetTag:
    Set a tag at the specified location.  Set 'line' to -1 to use the
    current position.  This function returns a tag id if successful, otherwise
    it return 0.
******************************************************************************/
int WINAPI _export TerSetTag(HWND hWnd,long line, int col, LPBYTE name, LPBYTE AuxText, long AuxInt, UINT flags)
{
    PTERWND w;
    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return SetTag(w,line,col,TERTAG_USER,name,AuxText,AuxInt);
}

/******************************************************************************
    SetTag:
    Set a tag at the specified location.  Set 'line' to -1 to use the
    current position.  This function returns a tag id if successful, otherwise
    it return 0.
******************************************************************************/
int SetTag(PTERWND w,long line, int col, int type, LPBYTE name, LPBYTE AuxText, long AuxInt)
{
    int TagId=0,id;
    LPWORD ct;

    if (line<0) {
       line=CurLine;
       col=CurCol;
    }
    if (line<0 || line>=TotalLines) return 0;
    if (col<0 || col>=LineLen(line)) return 0;

    if (-1==(TagId=GetTagSlot(w))) return 0;
    
    ct=OpenCtid(w,line);
    if (ct[col]) {         // connect to existing tag sequence
       id=ct[col];
       while (TRUE) {
          if ((int)CharTag[id].type==type && lstrcmp(CharTag[id].name,name)==0) {  // use this id
             TagId=id;
             FreeTag(w,TagId); // release the existing pointers
             break;                           // use the existing tag id
          }
          if (CharTag[id].next==0) {       // reached the end of the sequence
             CharTag[id].next=TagId;
             break;
          }
          id=CharTag[id].next;  // find the last tag in the sequence
       }
    }
    else ct[col]=TagId;

    CloseCtid(w,line);

    // set the tag information
    CharTag[TagId].InUse=TRUE;
    CharTag[TagId].type=type;

    if (NULL!=(CharTag[TagId].name=MemAlloc(lstrlen(name)+1)))
        lstrcpy(CharTag[TagId].name,name);
    if (AuxText && NULL!=(CharTag[TagId].AuxText=MemAlloc(lstrlen(AuxText)+1)))
        lstrcpy(CharTag[TagId].AuxText,AuxText);
    CharTag[TagId].AuxInt=AuxInt;

    TerArg.modified++;
    return TagId;
}

/******************************************************************************
    TransferTags:
    Tranfer the tags from the specified character to the following character.
******************************************************************************/
TransferTags(PTERWND w,long line, int col)
{
    long NextLine;
    int  NextCol,CurCt,NextCt;
    LPWORD ct1,ct2;

    if (NULL==(ct1=pCtid(line))) return TRUE;  // nothing to transfer

    CurCt=ct1[col];
    if (CurCt==0) return TRUE;   // nothing to transfer.

    // get the location of the next character
    NextLine=line;
    NextCol=col+1;
    if (NextCol>=LineLen(NextLine)) {
       NextLine++;
       NextCol=0;
       if (NextLine>=TotalLines) return TRUE;
    }

    ct2=OpenCtid(w,NextLine);   // open the character tags for the next line
    NextCt=ct2[NextCol];

    if (NextCt==0) ct2[NextCol]=CurCt; // simply transfer the tag sequnce
    else {                             // append the previous sequence to the end of the sequence at NextLine2
       while (CharTag[NextCt].next>0) NextCt=CharTag[NextCt].next;
       CharTag[NextCt].next=CurCt;     // append to the end of the seqeunce
    }

    ct1[col]=0;
    CloseCtid(w,NextLine);

    return TRUE;
}

/******************************************************************************
    UpdateTagTable:
    Refresh the tag table from the text
******************************************************************************/
UpdateTagTable(PTERWND w)
{
    long l;
    int i,len,col,TagId;
    LPBYTE InUse;
    LPWORD ct;

    InUse=OurAlloc(TotalCharTags);     // create the flag table
    FarMemSet(InUse,0,TotalCharTags);

    for (l=0;l<TotalLines;l++) {
       if (!pCtid(l)) continue;
       ct=pCtid(l);
       len=LineLen(l);
       for (col=0;col<len;col++) {
          TagId=ct[col];
          while (TagId) {
             InUse[TagId]=TRUE;
             TagId=CharTag[TagId].next;
          }
       }
    }
    for (i=1;i<TotalCharTags;i++) {
       if (!InUse[i] && CharTag[i].InUse) FreeTag(w,i);
    }
    OurFree(InUse);

    return TRUE;
}

/******************************************************************************
    FreeTag:
    Free up a tag id.
******************************************************************************/
FreeTag(PTERWND w,int TagId)
{
    if (TagId<0 || TagId>=TotalCharTags || !CharTag[TagId].InUse) return TRUE;

    if (CharTag[TagId].name) MemFree(CharTag[TagId].name);
    if (CharTag[TagId].HtmlInfo) MemFree(CharTag[TagId].HtmlInfo);
    if (CharTag[TagId].AuxText)  MemFree(CharTag[TagId].AuxText);
    CharTag[TagId].name=CharTag[TagId].AuxText=CharTag[TagId].HtmlInfo=NULL;
    CharTag[TagId].type=0;
    CharTag[TagId].InUse=FALSE;

    return TRUE;
}

/******************************************************************************
    GetTagSlot:
    get the char tag slot from the CharTag table.  This function returns -1 if
    a slot not found.
******************************************************************************/
GetTagSlot(PTERWND w)
{
    int i;
    DWORD OldSize, NewSize;
    int   NewMaxCharTags;
    BYTE huge *pMem;

    for (i=1;i<TotalCharTags;i++) {
       if (!CharTag[i].InUse) {
          FarMemSet(&CharTag[i],0,sizeof(struct StrCharTag));
          return i;
       }
    }

    // use the next slot
    if (TotalCharTags>=MaxCharTags) {        // allocate more memory for the cell table
       NewMaxCharTags=MaxCharTags+MaxCharTags/2;

       // allocate the CharTag array
       OldSize=((DWORD)MaxCharTags+1)*sizeof(struct StrCharTag);
       NewSize=((DWORD)NewMaxCharTags+1)*sizeof(struct StrCharTag);
       if (NULL==(pMem=OurAlloc(NewSize))) return -1;

       HugeMove(CharTag,pMem,OldSize);
       OurFree(CharTag);
       CharTag=(struct StrCharTag huge *)pMem;

       MaxCharTags=NewMaxCharTags;    // new max cells
    }

    FarMemSet(&CharTag[TotalCharTags],0,sizeof(struct StrCharTag));
    TotalCharTags++;

    return (TotalCharTags-1);

}

/******************************************************************************
    GetListSlot:
    get a free list table slot.  This function returns -1 if
    a slot not found.
******************************************************************************/
GetListSlot(PTERWND w)
{
    int i;
    DWORD OldSize, NewSize;
    int   NewMaxLists;
    BYTE huge *pMem;

    for (i=1;i<TotalLists;i++) {
       if (!list[i].InUse) {
          FarMemSet(&list[i],0,sizeof(struct StrList));
          return i;
       }
    }

    // use the next slot
    if (TotalLists>=MaxLists) {        // allocate more memory for the cell table
       NewMaxLists=MaxLists+MaxLists/2;
       if (!Win32 && NewMaxLists>MAX_LISTS) NewMaxLists=MAX_LISTS;

       // allocate the list array
       OldSize=((DWORD)MaxLists+1)*sizeof(struct StrList);
       NewSize=((DWORD)NewMaxLists+1)*sizeof(struct StrList);
       if (NULL==(pMem=OurAlloc(NewSize))) return -1;

       HugeMove(list,pMem,OldSize);
       OurFree(list);
       list=(struct StrList huge *)pMem;

       MaxLists=NewMaxLists;    // new max cells
    }

    FarMemSet(&list[TotalLists],0,sizeof(struct StrList));
    TotalLists++;

    return (TotalLists-1);

}

/******************************************************************************
    GetListOrSlot:
    get a free list override table slot. This funtion returns -1 if 
    a slot not found.
******************************************************************************/
GetListOrSlot(PTERWND w)
{
    int i;
    DWORD OldSize, NewSize;
    int   NewMaxListOr;
    BYTE huge *pMem;

    for (i=1;i<TotalListOr;i++) {
       if (!ListOr[i].InUse) {
          FarMemSet(&ListOr[i],0,sizeof(struct StrListOr));
          return i;
       }
    }

    // use the next slot
    if (TotalListOr>=MaxListOr) {        // allocate more memory for the cell table
       NewMaxListOr=MaxListOr+MaxListOr/2;
       if (!Win32 && NewMaxListOr>MAX_LIST_OR) NewMaxListOr=MAX_LIST_OR;

       // allocate the ListOr array
       OldSize=((DWORD)MaxListOr+1)*sizeof(struct StrListOr);
       NewSize=((DWORD)NewMaxListOr+1)*sizeof(struct StrListOr);
       if (NULL==(pMem=OurAlloc(NewSize))) return -1;

       HugeMove(ListOr,pMem,OldSize);
       OurFree(ListOr);
       ListOr=(struct StrListOr huge *)pMem;

       MaxListOr=NewMaxListOr;    // new max cells
    }

    FarMemSet(&ListOr[TotalListOr],0,sizeof(struct StrListOr));
    TotalListOr++;

    return (TotalListOr-1);

}

/******************************************************************************
    IsListLine:
    This funtion returns TRUE if the specified line displays a list number.
******************************************************************************/
BOOL IsListLine(PTERWND w, long LineNo)
{
    int CurPara,BltId;

    if (LineNo<0 || LineNo>=TotalLines) return FALSE;
    if (LineInfo(w,LineNo,(INFO_PAGE|INFO_COL|INFO_ROW))) return FALSE;

    CurPara=pfmt(LineNo);
    if (!(PfmtId[CurPara].flags&BULLET)) return FALSE;

    BltId=PfmtId[CurPara].BltId;
    if (BltId==0) return FALSE;

    if (TerBlt[BltId].IsBullet) return FALSE;

    if (TerBlt[BltId].ls==0) return FALSE;   // not a list override

    // check if first line of the paragraph - don't rely on the LFLAG_FIRST_PARA flag as this function can be called during word wrapping 
    
    return IsFirstParaLine(w,LineNo);
} 
 
/******************************************************************************
    IsSameListOr:
    Check if two ListOr are the same
******************************************************************************/
BOOL IsSameListOr(PTERWND w, struct StrListOr far *pListOr1, struct StrListOr far *pListOr2)
{
    int i;

    if (pListOr1->InUse && !pListOr2->InUse) return FALSE;
    if (!pListOr1->InUse && pListOr2->InUse) return FALSE;
    
    if (pListOr1->ListIdx !=pListOr2->ListIdx) return FALSE;
    if (pListOr1->LevelCount !=pListOr2->LevelCount) return FALSE;
 
    for (i=0;i<pListOr1->LevelCount;i++) 
       if (!IsSameListLevel(w,&(pListOr1->level[i]),&(pListOr2->level[i]))) return FALSE;

    return TRUE;
}

/******************************************************************************
    IsSameListLevel:
    Check if two ListOr are the same
******************************************************************************/
BOOL IsSameListLevel(PTERWND w, struct StrListLevel far *pLevel1, struct StrListLevel far *pLevel2)
{
    int i,len;
    DWORD FlagMask=(LISTLEVEL_RESTART|LISTLEVEL_LEGAL|LISTLEVEL_REFORMAT|LISTLEVEL_NO_RESET);

    if (pLevel1->start!=pLevel2->start) return FALSE;
    if (pLevel1->NumType!=pLevel2->NumType) return FALSE;
    if (pLevel1->CharAft!=pLevel2->CharAft) return FALSE;
    if (pLevel1->LeftIndent!=pLevel2->LeftIndent) return FALSE;
    if (pLevel1->RightIndent!=pLevel2->RightIndent) return FALSE;
    if (pLevel1->FirstIndent!=pLevel2->FirstIndent) return FALSE;
    if (pLevel1->ParaFlags!=pLevel2->ParaFlags) return FALSE;
    if (pLevel1->FontStyles!=pLevel2->FontStyles) return FALSE;
    if ((pLevel1->flags&FlagMask)!=(pLevel2->flags&FlagMask)) return FALSE;
    if (pLevel1->MinIndent!=pLevel2->MinIndent) return FALSE;
    if (pLevel1->FontStylesOff!=pLevel2->FontStylesOff) return FALSE;
    if (pLevel1->text[0]!=pLevel2->text[0]) return FALSE;

    len=pLevel1->text[0];
    for (i=1;i<=len;i++) if (pLevel1->text[i]!=pLevel2->text[i]) return FALSE;
 
    return TRUE;
}
 
/******************************************************************************
    FreeListTable:
    Free the list table
******************************************************************************/
FreeListTable(PTERWND w)
{
    int i;

    for (i=1;i<TotalLists;i++) FreeList(w,i);    // free all entries except the first one
    TotalLists=1;

    for (i=1;i<TotalListOr;i++) FreeListOr(w,i);
    TotalListOr=1;  

    return TRUE;
} 

/******************************************************************************
    FreeList:
    Free a list table slot
******************************************************************************/
FreeList(PTERWND w, int ListId)
{
    if (list[ListId].InUse) {
       if (list[ListId].name) MemFree(list[ListId].name);
       if (list[ListId].level) MemFree(list[ListId].level);
    }

    FarMemSet(&list[ListId],0,sizeof(struct StrList));

    return TRUE;
} 

/******************************************************************************
    FreeListOr:
    Free a list override table slot
******************************************************************************/
FreeListOr(PTERWND w, int ListId)
{
    if (ListOr[ListId].InUse) {
       if (ListOr[ListId].level) MemFree(ListOr[ListId].level);
    }

    FarMemSet(&ListOr[ListId],0,sizeof(struct StrListOr));

    return TRUE;
} 

/****************************************************************************
    SetFnoteFontInfo:
    Set/reset the footnote text font info.
****************************************************************************/
SetFnoteFontInfo(PTERWND w,BOOL set)
{
    int i;
    int TempWidth[256];

    for (i=0;i<TotalFonts;i++) {
       if (!TerFont[i].InUse || TerFont[i].style&PICT || !TerFont[i].hidden) continue;
       if (!TerFont[i].style&FNOTETEXT) continue;
       if (set && TerFont[i].flags&FFLAG_HIDDEN_INFO) continue;  // already active
       if (!set && !(TerFont[i].flags&FFLAG_HIDDEN_INFO)) continue;  // already reset

       // swap the information
       SwapInts(&(TerFont[i].height),&(TerFont[i].hidden->height));
       SwapInts(&(TerFont[i].BaseHeight),&(TerFont[i].hidden->BaseHeight));
       SwapInts(&(TerFont[i].BaseHeightAdj),&(TerFont[i].hidden->BaseHeightAdj));
       SwapInts(&(TerFont[i].ExtLead),&(TerFont[i].hidden->ExtLead));

       FarMove(TerFont[i].CharWidth,TempWidth,sizeof(TempWidth));
       FarMove(TerFont[i].hidden->CharWidth,TerFont[i].CharWidth,sizeof(TempWidth));
       FarMove(TempWidth,TerFont[i].hidden->CharWidth,sizeof(TempWidth));

       if (PrtFont[i].hidden) {
          SwapInts(&(PrtFont[i].height),&(PrtFont[i].hidden->height));
          SwapInts(&(PrtFont[i].BaseHeight),&(PrtFont[i].hidden->BaseHeight));
          SwapInts(&(PrtFont[i].ExtLead),&(PrtFont[i].hidden->ExtLead));

          FarMove(PrtFont[i].CharWidth,TempWidth,sizeof(TempWidth));
          FarMove(PrtFont[i].hidden->CharWidth,PrtFont[i].CharWidth,sizeof(TempWidth));
          FarMove(TempWidth,PrtFont[i].hidden->CharWidth,sizeof(TempWidth));
       }

       if (set) TerFont[i].flags|=FFLAG_HIDDEN_INFO;
       else     ResetUintFlag(TerFont[i].flags,FFLAG_HIDDEN_INFO);

    }

    return TRUE;
}

/****************************************************************************
    TerColors:
    This dialog box allows the user to select the new color.  This routine
    calls the SetTerColor function to apply the chosen color to the text.
****************************************************************************/
BOOL TerColors(PTERWND w,BOOL foreground)
{
    CHOOSECOLOR cColor;
    DWORD CustColors[16];
    BYTE  clr;
    int i;
    float increment;
    WORD CurFont;
    LPWORD fmt;
    int result;

    // find the current color
    if (cfmt(CurLine).info.type==UNIFORM) CurFont=cfmt(CurLine).info.fmt;
    else {
       fmt=cfmt(CurLine).pFmt;
       if (CurCol<LineLen(CurLine)) CurFont=fmt[CurCol];
       else                         CurFont=DEFAULT_CFMT;
       if (TerFont[CurFont].style&PICT) CurFont=DEFAULT_CFMT;
    }


    // fill the CustColors array with 16 shades of gray
    increment=(float)255/(float)16;
    for (i=0;i<16;i++) {
       clr=(BYTE)((i+1)*increment);
       CustColors[i]=RGB(clr,clr,clr);
    }

    // fill the CHOOSECOLOR structure
    FarMemSet(&cColor,0,sizeof(CHOOSECOLOR));
    cColor.lStructSize=sizeof(CHOOSECOLOR);
    cColor.hwndOwner=hTerWnd;
    if (CurSID>=0) {
      if (foreground) cColor.rgbResult=StyleId[CurSID].TextColor;
      else            cColor.rgbResult=StyleId[CurSID].TextBkColor;
    }
    else {
      if (foreground) cColor.rgbResult=TerFont[CurFont].TextColor;
      else            cColor.rgbResult=TerFont[CurFont].TextBkColor;
    }
    
    if (cColor.rgbResult==CLR_AUTO) cColor.rgbResult=CLR_WHITE;  // show auto color as white
    
    cColor.lpCustColors=CustColors;
    cColor.Flags=CC_PREVENTFULLOPEN|CC_RGBINIT;

    result=ChooseColor(&cColor);

    SetFocus(hTerWnd);                  // set focus to our window

    if (!result) return FALSE;

    if (foreground) return SetTerColor(hTerWnd,cColor.rgbResult,TRUE);
    else            return SetTerBkColor(hTerWnd,cColor.rgbResult,TRUE);
}

/****************************************************************************
    TerSetCharSpace:
    This function applies character space adjustmemt.
****************************************************************************/
BOOL WINAPI _export TerSetCharSpace(HWND hWnd,BOOL dialog, int expand,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // accept the user values
    if (dialog) {
       if (CurSID>=0) DlgInt1=StyleId[CurSID].expand;  // value to pass to the dialog box
       else {
          int CurFont=GetEffectiveCfmt(w);
          DlgInt1=TerFont[CurFont].expand;
       }
       if (!CallDialogBox(w,"CharSpaceParam",CharSpaceParam,0L)) return FALSE;

       expand=DlgInt1;              // new value
    }

    if (CurSID>=0) {
       StyleId[CurSID].expand=expand;
       return TRUE;
    }
    else return CharFmt(w,GetNewExpand,(DWORD)expand,0L,repaint);
}

/****************************************************************************
    TerSetCharOffset:
    This function applies character offset (twips) adjustmemt.
****************************************************************************/
BOOL WINAPI _export TerSetCharOffset(HWND hWnd,int offset,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data


    if (CurSID>=0) {
       StyleId[CurSID].offset=offset;
       return TRUE;
    }
    else return CharFmt(w,GetNewCharOffset,(DWORD)offset,0L,repaint);
}

/****************************************************************************
    TerSetFontSpace:
    This function set the space adjustment for a font.
****************************************************************************/
BOOL WINAPI _export TerSetFontSpace(HWND hWnd,int font, int expand,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (font<0 || font>=TotalFonts || !TerFont[font].InUse || TerFont[font].style&PICT) return FALSE;

    TerFont[font].expand=expand;

    if (!CreateOneFont(w,hTerDC,font,TRUE)) return FALSE;

    if (mbcs) InitCharWidthCache(w);   // initialize the character width cache
    RequestPagination(w,TRUE);

    if (repaint) PaintTer(w);

    return TRUE;
}

/****************************************************************************
    TerGetFontSpace:
    This function retrieves the space adjustment for a font.
****************************************************************************/
int WINAPI _export TerGetFontSpace(HWND hWnd,int font)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (font<0 || font>=TotalFonts || !TerFont[font].InUse || TerFont[font].style&PICT) return 0;

    return TerFont[font].expand;
}

/****************************************************************************
    TerSetDefLang:
    This function sets the default language for the document. Language id
    must be a valid id as defined by RTF spec 1.5
****************************************************************************/
BOOL WINAPI _export TerSetDefLang(HWND hWnd,int lang)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    DefLang=lang;
    return TRUE;
}
/****************************************************************************
    TerSetCharLang:
    This function sets the language for the selected text. Language id
    must be a valid id as defined by RTF spec 1.5
****************************************************************************/
BOOL WINAPI _export TerSetCharLang(HWND hWnd,int lang, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (lang==DefLang) lang=0;                           // default language

    if (CurSID>=0) return TRUE;   // not valid for a style yet
    else return CharFmt(w,GetNewLang,(DWORD)lang,0L,repaint);
}

/****************************************************************************
    TerGetFontLang:
    This function retrieves the language id for a font.
****************************************************************************/
int WINAPI _export TerGetFontLang(HWND hWnd,int font)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (font<0 || font>=TotalFonts || !TerFont[font].InUse || TerFont[font].style&PICT) return 0;

    return (TerFont[font].lang?TerFont[font].lang:DefLang);
}

/******************************************************************************
    GetNewLang:
    This routine accept the exiting font index and returns a new font
    index that supplies the given language id.
******************************************************************************/
WORD GetNewLang(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2,long line, int col)
{
    int CurFont,NewFont;
    UINT style;
    int lang;

    lang=(int)data1;                // new color to apply
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if (style&PICT) return OldFmt;        // do not change the picutre format

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                           TerFont[CurFont].style,TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                           TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                           TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                           TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                           lang,TerFont[CurFont].FieldCode,
                           TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                           TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                           TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // return the new font
    }
    else return OldFmt;
}

/****************************************************************************
    TerGetFieldFont:
    Apply the given field id and field code on the given font and return the resulting
    font id.  This function returns -1 to indicate an error condtion.
****************************************************************************/
int WINAPI _export TerGetFieldFont(HWND hWnd,int font, int FieldId, LPBYTE FieldCode)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (font<0 || font>=TotalFonts || !TerFont[font].InUse) return -1;

    if (TerFont[font].FieldId==FieldId && IsSameFieldCode(w,TerFont[font].FieldCode,FieldCode)) return font;  // no chnage

    // is it a picture
    if (TerFont[font].style&PICT) {      // apply new field info
        TerFont[font].FieldId=FieldId;
        if (TerFont[font].FieldCode) OurFree(TerFont[font].FieldCode);
        TerFont[font].FieldCode=NULL;

        if (FieldCode) {
            TerFont[font].FieldCode=OurAlloc(lstrlen(FieldCode)+1);
            lstrcpy(TerFont[font].FieldCode,FieldCode);
            return font;
        } 
    } 

    // it is a regular font
    return (int)GetNewFieldId(w,(WORD)font,(DWORD)FieldId,(DWORD)FieldCode,0L,0);
}

/****************************************************************************
    TerGetFontFieldId:
    This function retrieves the field id for the font
****************************************************************************/
int WINAPI _export TerGetFontFieldId(HWND hWnd,int font)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (font<0 || font>=TotalFonts || !TerFont[font].InUse) return 0;

    return TerFont[font].FieldId;
}

/****************************************************************************
    TerGetFontFieldCode:
    This function retrieves the field code for the font
****************************************************************************/
BOOL WINAPI _export TerGetFontFieldCode(HWND hWnd,int font, LPBYTE FieldCode)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    FieldCode[0]=0;
    
    if (font<0 || font>=TotalFonts || !TerFont[font].InUse) return FALSE;

    if (TerFont[font].FieldCode) lstrcpy(FieldCode,TerFont[font].FieldCode);

    return TRUE;
}

/****************************************************************************
    TerSetFontField:
    Apply the given field id and field code on the given font id. This function returns
    TRUE when successful.
****************************************************************************/
BOOL WINAPI _export TerSetFontField(HWND hWnd,int font, int FieldId, LPBYTE FieldCode)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (font<0 || font>=TotalFonts || !TerFont[font].InUse) return FALSE;

    if (TerFont[font].FieldId==FieldId && IsSameFieldCode(w,TerFont[font].FieldCode,FieldCode)) return TRUE;  // no chnage

    // is it a picture
    TerFont[font].FieldId=FieldId;
    if (TerFont[font].FieldCode) OurFree(TerFont[font].FieldCode);
    TerFont[font].FieldCode=NULL;

    if (FieldCode) {
        TerFont[font].FieldCode=OurAlloc(lstrlen(FieldCode)+1);
        lstrcpy(TerFont[font].FieldCode,FieldCode);
    } 

    TerArg.modified++;

    return TRUE;
} 

/****************************************************************************
    InsertHuperlink:
    Insert a hyper link at the current location.
****************************************************************************/
InsertHyperlink(PTERWND w)
{
    if (!CallDialogBox(w,"HyperlinkParam",HyperlinkParam,0L)) return FALSE;
    if (lstrlen(DlgText1)==0 || lstrlen(DlgText2)==0) return FALSE;

    return TerInsertHyperlink(hTerWnd,DlgText1,DlgText2,0,TRUE);
}

/****************************************************************************
    TerInsertHuperlink:
    Insert a hyper link at the current location.  If PictId is not zero, then a picture
    is inserted. This function returns the font id or the picture id of the inserted link.
    It returns -1 when unsuccessful.
****************************************************************************/
int WINAPI _export TerInsertHyperlink(HWND hWnd, LPBYTE LinkText, LPBYTE LinkCode, int PictId, BOOL repaint)
{
    PTERWND w;
    int CurFont=-1;
    UINT CurLinkStyle;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (!LinkCode || lstrlen(LinkCode)==0) return -1;

    if (PictId>0) {                      // picture id specified
       if (PictId>=TotalFonts || !TerFont[PictId].InUse || !(TerFont[PictId].style&PICT)) return -1;
       TerGetFieldFont(hTerWnd,PictId,FIELD_HLINK,LinkCode);  // apply field code to the picture
       TerInsertObjectId(hTerWnd,PictId,repaint);
       return PictId;
    } 

    if (!LinkText || lstrlen(LinkText)==0) return -1;

    // apply style, color and field id
    CurLinkStyle=LinkStyle;
    ResetUintFlag(CurLinkStyle,HLINK);
    if (CurLinkStyle) SetTerCharStyle(hTerWnd,TRUE,CurLinkStyle,FALSE);
    SetTerColor(hTerWnd,LinkColor,FALSE);

    CurFont=GetEffectiveCfmt(w);
    InputFontId=CurFont=TerGetFieldFont(hTerWnd,CurFont,FIELD_HLINK,LinkCode);

    InsertTerText(hTerWnd,LinkText,repaint);
    
    InputFontId=-1;                             // reset input font id
    TerArg.modified++;

    return CurFont;
}

/****************************************************************************
    TerApplyHyperlink:
    Apply hyper link to the currently selected text.
    This function returns the font id of the inserted link.
    It returns TRUE when unsuccessful.
****************************************************************************/
BOOL WINAPI _export TerApplyHyperlink(HWND hWnd, LPBYTE LinkCode, BOOL repaint)
{
    PTERWND w;
    int CurFont=-1,i;
    long l;
    int StartCol,EndCol;
    LPWORD fmt;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!LinkCode || lstrlen(LinkCode)==0) return FALSE;
    if (HilightType==HILIGHT_OFF) return FALSE;
    
    if (!NormalizeBlock(w)) return FALSE;
    if (!NormalizeForFootnote(w)) return FALSE;    // make sure that footnotes are fully contained

    if (HilightType==HILIGHT_LINE) {      // set beginning and ending columns
        HilightBegCol=0;
        HilightEndCol=LineLen(HilightEndRow);
        if (HilightEndCol<0) HilightEndCol=0;
    }

    // save for undo
    SaveUndo(w,HilightBegRow,HilightBegCol,HilightEndRow,HilightEndCol-1,'F');
      
    for (l=HilightBegRow;l<=HilightEndRow;l++) {
       StartCol=(l==HilightBegRow)?HilightBegCol:0;
       EndCol=(l==HilightEndRow)?HilightEndCol:LineLen(l);

       fmt=OpenCfmt(w,l);
       for (i=StartCol;i<EndCol;i++) {
          fmt[i]=TerGetFieldFont(hTerWnd,fmt[i],FIELD_HLINK,LinkCode);
       }
       CloseCfmt(w,l); 
    } 
    
    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/****************************************************************************
    TerUpdateHyperlinkText:
    Update the text for the current hyperlink field.  This function can be
    used with the NewLinkText set to "" to delete the hyperlink completely.
****************************************************************************/
BOOL WINAPI _export TerUpdateHyperlinkText(HWND hWnd, LPBYTE NewLinkText, BOOL repaint)
{
    PTERWND w;
    int CurFont,BegCol,EndCol;
    BYTE CurFieldCode[MAX_WIDTH+1];
    long BegLine,EndLine,BegPos,EndPos;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!NewLinkText) return FALSE;

    CurFont=GetCurCfmt(w,CurLine,CurCol);
    if (TerFont[CurFont].FieldId!=FIELD_HLINK) return FALSE;

    if (TerFont[CurFont].FieldCode) lstrcpy(CurFieldCode,TerFont[CurFont].FieldCode);
    else                            CurFieldCode[0]=0;

    BegLine=EndLine=CurLine;
    BegCol=EndCol=CurCol;
    
    if (TerLocateFieldChar(hTerWnd,FIELD_HLINK,CurFieldCode,FALSE,&BegLine,&BegCol,FALSE)) 
       NextTextPos(w,&BegLine,&BegCol);    // first character of the hyperlink field
    else {
       BegLine=0;
       BegCol=0;
    }
    BegPos=RowColToAbs(w,BegLine,BegCol);
    
    TerLocateFieldChar(hTerWnd,FIELD_HLINK,CurFieldCode,FALSE,&EndLine,&EndCol,TRUE);
    PrevTextPos(w,&EndLine,&EndCol);    // last character of the hyperlink field
    EndPos=RowColToAbs(w,EndLine,EndCol);

    ReplaceTextString(w,NewLinkText,NULL,BegPos,EndPos);

    TerArg.modified++;

    if (repaint) PaintTer(w);
     
    return TRUE;
}

/****************************************************************************
    TerUpdateHyperlinkCode:
    Update the code for the current hyperlink field.
****************************************************************************/
BOOL WINAPI _export TerUpdateHyperlinkCode(HWND hWnd, LPBYTE NewLinkCode)
{
    PTERWND w;
    int CurFont,BegCol,EndCol,col1,col2,col,PrevFont,NewFont;
    BYTE CurFieldCode[MAX_WIDTH+1];
    long l,BegLine,EndLine;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!NewLinkCode || lstrlen(NewLinkCode)==0) return FALSE;

    CurFont=GetCurCfmt(w,CurLine,CurCol);
    
    if (TerFont[CurFont].FieldId!=FIELD_HLINK) return FALSE;
  
    if (TerFont[CurFont].FieldCode) lstrcpy(CurFieldCode,TerFont[CurFont].FieldCode);
    else                            CurFieldCode[0]=0;

    BegLine=EndLine=CurLine;
    BegCol=EndCol=CurCol;
    if (!TerLocateFieldChar(hTerWnd,FIELD_HLINK,CurFieldCode,FALSE,&BegLine,&BegCol,FALSE)) {
       BegLine=0;        // field begins at the beginning of the file
       BegCol=0;
    }
    else NextTextPos(w,&BegLine,&BegCol);    // first character of the hyperlink field
    TerLocateFieldChar(hTerWnd,FIELD_HLINK,CurFieldCode,FALSE,&EndLine,&EndCol,TRUE);
    PrevTextPos(w,&EndLine,&EndCol);    // first character of the hyperlink field
    
    PrevFont=-1;
    CurFont=0;
    for (l=BegLine;l<=EndLine;l++) {
       col1=(l==BegLine?BegCol:0);
       col2=(l==EndLine?EndCol:(LineLen(l)-1));

       fmt=OpenCfmt(w,l);
       for (col=col1;col<=col2;col++) {
          CurFont=fmt[col];
          if (CurFont==PrevFont) {
             fmt[col]=NewFont;
             continue;
          }
          
          if (TerFont[CurFont].FieldId!=FIELD_HLINK) goto END;
          NewFont=TerGetFieldFont(hTerWnd,CurFont,FIELD_HLINK,NewLinkCode);
          if (NewFont<0) goto END;
          
          PrevFont=CurFont;
          fmt[col]=NewFont;
       }
       
       CloseCfmt(w,l);
    } 

    END:
    TerArg.modified++;

    return TRUE;
}

/****************************************************************************
    TerFindHlinkField:
    Find a hyperlink field by scanning the field code. This function returns
    TRUE if any of the two strings are found in the field code. 
****************************************************************************/
BOOL WINAPI _export TerFindHlinkField(HWND hWnd, LPBYTE CodeString1, LPBYTE CodeString2, LPLONG pLine, LPINT pCol)
{
    PTERWND w;
    int CurFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!pLine || !pCol) return FALSE;

    while (TerLocateFieldChar(hWnd,FIELD_HLINK,NULL,TRUE,pLine,pCol,TRUE)) {  // scan for the next field
        CurFont=GetCurCfmt(w,(*pLine),(*pCol));

        if (TerFont[CurFont].FieldCode) {
           if (CodeString1 && strstr(TerFont[CurFont].FieldCode,CodeString1)) return TRUE;
           if (CodeString2 && strstr(TerFont[CurFont].FieldCode,CodeString2)) return TRUE;
        }
        
        if (!TerLocateFieldChar(hWnd,FIELD_HLINK,TerFont[CurFont].FieldCode,FALSE,pLine,pCol,TRUE)) break;  // go past the current field
    } 

    return FALSE;
}

/***************************************************************************
    TerLocateFieldChar:
    This routine scans the text starting from the given line and col position
    to search for the given FieldId.  The called specifies the direction of
    the search.  The caller also specifies to look for the presence of absence
    of such a style
    If the FieldId is located, the routine returns a TRUE value.
    It also updates the line and col parameters with the location of the style.
****************************************************************************/
BOOL WINAPI _export TerLocateFieldChar(HWND hWnd, int FieldId, LPBYTE FieldCode, BOOL present, long far *StartLine, int far *StartCol, BOOL forward)
{
    PTERWND w;
    long line;
    int  i,col,CurCfmt,CurFieldId;
    LPWORD fmt;
    LPBYTE CurFieldCode;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (FieldId==0) FieldCode=NULL;
    if (FieldCode && lstrlen(FieldCode)==0) FieldCode=NULL;

    // scan each line in the forward direction
    if (forward) {
       for (line=(*StartLine);line<TotalLines;line++) {
          if (line==(*StartLine)) col=*StartCol;
          else                    col=0;

          if (col>=LineLen(line)) continue;                   // go to next line
          if (LineLen(line)==0) continue;

          if (cfmt(line).info.type==UNIFORM) {
             CurCfmt=cfmt(line).info.fmt;
             
             CurFieldId=TerFont[CurCfmt].FieldId;
             CurFieldCode=TerFont[CurCfmt].FieldCode;
             if (!FieldCode) CurFieldCode=NULL;       // fieldcode not to be matched

             if (  (present && CurFieldId==FieldId && IsSameFieldCode(w,FieldCode,CurFieldCode)) 
                || (!present && (CurFieldId!=FieldId || !IsSameFieldCode(w,FieldCode,CurFieldCode))) ) {                // style found
                *StartLine=line;
                *StartCol=col;
                return TRUE;
             }
             else continue;                       // style not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i<LineLen(line);i++) {
            CurCfmt=fmt[i];

            CurFieldId=TerFont[CurCfmt].FieldId;
            CurFieldCode=TerFont[CurCfmt].FieldCode;
            if (!FieldCode) CurFieldCode=NULL;       // fieldcode not to be matched

            if (  (present && CurFieldId==FieldId && IsSameFieldCode(w,FieldCode,CurFieldCode)) 
                || (!present && (CurFieldId!=FieldId || !IsSameFieldCode(w,FieldCode,CurFieldCode))) ) {                // style found
            
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return TRUE;
            }
          }
          CloseCfmt(w,line);
       }
    }
    else { // scan each line in the backward direction
       for (line=(*StartLine);line>=0;line--) {
          if (line==(*StartLine)) col=*StartCol;
          else                    col=LineLen(line)-1;

          if (col<0) continue;
          if (LineLen(line)==0) continue;

          if (cfmt(line).info.type==UNIFORM) {
             CurCfmt=cfmt(line).info.fmt;
             
             CurFieldId=TerFont[CurCfmt].FieldId;
             CurFieldCode=TerFont[CurCfmt].FieldCode;
             if (!FieldCode) CurFieldCode=NULL;       // fieldcode not to be matched

             if (  (present && CurFieldId==FieldId && IsSameFieldCode(w,FieldCode,CurFieldCode)) 
                || (!present && (CurFieldId!=FieldId || !IsSameFieldCode(w,FieldCode,CurFieldCode))) ) {                // style found
                
                *StartLine=line;
                *StartCol=col;
                return TRUE;
             }
             else continue;                       // style not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i>=0;i--) {
            CurCfmt=fmt[i];
            
            CurFieldId=TerFont[CurCfmt].FieldId;
            CurFieldCode=TerFont[CurCfmt].FieldCode;
            if (!FieldCode) CurFieldCode=NULL;       // fieldcode not to be matched

            if (  (present && CurFieldId==FieldId && IsSameFieldCode(w,FieldCode,CurFieldCode)) 
                || (!present && (CurFieldId!=FieldId || !IsSameFieldCode(w,FieldCode,CurFieldCode))) ) {                // style found
               
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return TRUE;
            }
          }
          CloseCfmt(w,line);
       }
    }

    return FALSE;
}

/***************************************************************************
    TerLocateAuxIdChar:
    This routine scans the text starting from the given line and col position
    to search for the given FieldId.  The called specifies the direction of
    the search.  The caller also specifies to look for the presence of absence
    of such an aux id
    If the AuxId character is located, the routine returns a TRUE value.
    It also updates the line and col parameters with the location of the style.
****************************************************************************/
BOOL WINAPI _export TerLocateAuxIdChar(HWND hWnd, int AuxId, BOOL present, long far *StartLine, int far *StartCol, BOOL forward)
{
    PTERWND w;
    long line;
    int  i,col,CurCfmt,CurAuxId;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // scan each line in the forward direction
    if (forward) {
       for (line=(*StartLine);line<TotalLines;line++) {
          if (line==(*StartLine)) col=*StartCol;
          else                    col=0;

          if (col>=LineLen(line)) continue;                   // go to next line
          if (LineLen(line)==0) continue;

          if (cfmt(line).info.type==UNIFORM) {
             CurCfmt=cfmt(line).info.fmt;
             
             CurAuxId=TerFont[CurCfmt].AuxId;

             if (  (present && CurAuxId==AuxId) 
                || (!present && (CurAuxId!=AuxId))) {                // style found
                *StartLine=line;
                *StartCol=col;
                return TRUE;
             }
             else continue;                       // style not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i<LineLen(line);i++) {
            CurCfmt=fmt[i];

            CurAuxId=TerFont[CurCfmt].AuxId;

            if (  (present && CurAuxId==AuxId) 
                || (!present && (CurAuxId!=AuxId)) ) {                // style found
            
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return TRUE;
            }
          }
          CloseCfmt(w,line);
       }
    }
    else { // scan each line in the backward direction
       for (line=(*StartLine);line>=0;line--) {
          if (line==(*StartLine)) col=*StartCol;
          else                    col=LineLen(line)-1;

          if (col<0) continue;
          if (LineLen(line)==0) continue;

          if (cfmt(line).info.type==UNIFORM) {
             CurCfmt=cfmt(line).info.fmt;
             
             CurAuxId=TerFont[CurCfmt].AuxId;

             if (  (present && CurAuxId==AuxId) 
                || (!present && (CurAuxId!=AuxId)) ) {                // style found
                
                *StartLine=line;
                *StartCol=col;
                return TRUE;
             }
             else continue;                       // style not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i>=0;i--) {
            CurCfmt=fmt[i];
            
            CurAuxId=TerFont[CurCfmt].AuxId;

            if (  (present && CurAuxId==AuxId) 
                || (!present && (CurAuxId!=AuxId)) ) {                // style found
               
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return TRUE;
            }
          }
          CloseCfmt(w,line);
       }
    }

    return FALSE;
}


/******************************************************************************
    GetNewExpand:
    This routine accept the exiting font index and returns a new font
    index that supplies the given color.
******************************************************************************/
WORD GetNewExpand(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2,long line, int col)
{
    int CurFont,NewFont;
    UINT style;
    int expand;

    expand=(int)data1;                // new color to apply
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if (style&PICT) return OldFmt;        // do not change the picutre format

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                           TerFont[CurFont].style,TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                           TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                           TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                           expand,TerFont[CurFont].TempStyle,
                           TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                           TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                           TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                           TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // return the new font
    }
    else return OldFmt;
}

/******************************************************************************
    GetNewCharOffset:
    This routine accept the exiting font index and returns a new font
    index that supplies the given color.
******************************************************************************/
WORD GetNewCharOffset(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2,long line, int col)
{
    int CurFont,NewFont;
    UINT style;
    int offset;

    offset=(int)data1;                // new color to apply
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if (style&PICT) return OldFmt;        // do not change the picutre format

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                           TerFont[CurFont].style,TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                           TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                           TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                           TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                           TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                           offset,TerFont[CurFont].UcBase,
                           TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                           TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // return the new font
    }
    else return OldFmt;
}

/****************************************************************************
    SetFontTempStyle:
    This function sets or resets the font TempStyle.
****************************************************************************/
BOOL SetFontTempStyle(PTERWND w, UINT NewTempStyle,BOOL set,BOOL repaint)
{
    return CharFmt(w,GetNewTempStyle,NewTempStyle,(DWORD)set,repaint);
}

/******************************************************************************
    GetNewTempStyle:
    This routine applies the new temp style.
******************************************************************************/
WORD GetNewTempStyle(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2,long line, int col)
{
    int CurFont,NewFont;
    UINT style,TempStyle,NewTempStyle=(UINT)data1;
    BOOL set=(BOOL)data2;

    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style
    TempStyle=TerFont[CurFont].TempStyle;

    if (style&PICT) return OldFmt;        // do not change the picutre format

    if (set) TempStyle|=NewTempStyle;
    else     ResetUintFlag(TempStyle,NewTempStyle);

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                           TerFont[CurFont].style,TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                           TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                           TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                           TerFont[CurFont].expand,TempStyle,
                           TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                           TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                           TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                           TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // return the new font
    }
    else return OldFmt;
}



/****************************************************************************
    SetTerColor:
    This function applies the given color to the highlighted text or to the
    the current character
****************************************************************************/
BOOL WINAPI _export SetTerColor(HWND hWnd,COLORREF color,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurSID>=0) {
       StyleId[CurSID].TextColor=color;
       return TRUE;
    }
    else return CharFmt(w,GetNewColor,(DWORD)color,0L,repaint);
}

/****************************************************************************
    SetTerBkColor:
    This function applies the given background color to the highlighted text or to the
    the current character
****************************************************************************/
BOOL WINAPI _export SetTerBkColor(HWND hWnd,COLORREF color,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurSID>=0) {
       StyleId[CurSID].TextBkColor=color;
       return TRUE;
    }
    else return CharFmt(w,GetNewBkColor,(DWORD)color,0L,repaint);
}

/****************************************************************************
    TerSetUlineColor:
    Set underline color.
****************************************************************************/
BOOL WINAPI _export TerSetUlineColor(HWND hWnd,BOOL dialog, COLORREF color,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // get the color
    if (dialog) {
       if (CurSID>=0) color=StyleId[CurSID].UlineColor;
       else {
          if (HilightType!=HILIGHT_OFF) {
             NormalizeBlock(w);
             color=TerFont[GetCurCfmt(w,HilightBegRow,HilightBegCol)].UlineColor;
          }
          else  color=TerFont[GetCurCfmt(w,CurLine,CurCol)].UlineColor;
       }
       color=DlgEditColor(w,hTerWnd,color,false);
       if (DlgCancel) return false;
    }

    if (CurSID>=0) {
       StyleId[CurSID].UlineColor=color;
       return TRUE;
    }
    else return CharFmt(w,GetNewUlineColor,(DWORD)color,0L,repaint);
}


/******************************************************************************
    GetNewColor:
    This routine accept the exiting font index and returns a new font
    index that supplies the given color.
******************************************************************************/
WORD GetNewColor(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2,long line, int col)
{
    int CurFont,NewFont;
    UINT style;
    COLORREF color;

    color=(COLORREF)data1;                // new color to apply
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if (style&PICT) return OldFmt;        // do not change the picutre format

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                           TerFont[CurFont].style,
                           color,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                           TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                           TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                           TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                           TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                           TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                           TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                           TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // return the new font
    }
    else return OldFmt;
}

/******************************************************************************
    GetNewBkColor:
    This routine accept the exiting font index and returns a new font
    index that supplies the given background color.
******************************************************************************/
WORD GetNewBkColor(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2, long line, int col)
{
    int CurFont,NewFont;
    UINT style;
    COLORREF color;

    color=(COLORREF)data1;                // new color to apply
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if (style&PICT) return OldFmt;        // do not change the picutre format

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,color,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // return the new font
    }
    else return OldFmt;
}

/******************************************************************************
    GetNewUlineColor:
    This routine accept the exiting font index and returns a new font
    index that supplies the given color.
******************************************************************************/
WORD GetNewUlineColor(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2,long line, int col)
{
    int CurFont,NewFont;
    UINT style;
    COLORREF color;

    color=(COLORREF)data1;                // new color to apply
    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if (style&PICT) return OldFmt;        // do not change the picutre format

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                           TerFont[CurFont].style,
                           TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,color,
                           TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                           TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                           TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                           TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                           TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                           TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                           TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont;             // return the new font
    }
    else return OldFmt;
}


/******************************************************************************
   InitCharWidthCache:
   Initialize the character width cache.
*******************************************************************************/
InitCharWidthCache(PTERWND w)
{
   if (!mbcs) return TRUE;

   FarMemSet(CharScrFontId,0xFF,CHAR_WIDTH_CACHE_SIZE);
   FarMemSet(CharPrtFontId,0xFF,CHAR_WIDTH_CACHE_SIZE);
   FarMemSet(CharScrWidth,0xFF,CHAR_WIDTH_CACHE_SIZE);
   FarMemSet(CharPrtWidth,0xFF,CHAR_WIDTH_CACHE_SIZE);

   return TRUE;
}

/******************************************************************************
   BuildMbcsCharWidth:
   Build the char width array for an mbcs string.
*******************************************************************************/
BuildMbcsCharWidth(PTERWND w,LPINT CharWidth, int FontId, LPBYTE MbText, int MbLen, LPBYTE tail, LPBYTE lead, int LeadLen, LPWORD pWidth)
{
   int i,j,width,half;

   if (mbcs && lead) {
     for (i=j=0;j<LeadLen;j++,i++) {
        if (lead[j]) {
           if (pWidth) width=pWidth[j];
           else        width=DblCharWidth(w,FontId,TRUE,tail[j],lead[j]);
           half=width>>1;
           CharWidth[i]=half;
           i++;
           CharWidth[i]=width-half;
        }
        else {
           if (pWidth) CharWidth[i]=pWidth[j];
           else        CharWidth[i]=DblCharWidth(w,FontId,TRUE,tail[j],0);
        }
     }
   }
   else for (i=0;i<MbLen;i++) {
      if (pWidth) CharWidth[i]=pWidth[i];
      else        CharWidth[i]=DblCharWidth(w,FontId,TRUE,MbText[i],0);
   }

   return TRUE;
}

/*****************************************************************************
    DblCharWidth:
    This routine calculates the width combining lead and tail bytes
******************************************************************************/
DblCharWidth(PTERWND w, int font, BOOL screen, BYTE tail, BYTE lead)
{
    LPINT WidthTable;
    BYTE string[3];
    HDC hDC;
    HFONT hPrevFont,hFont;
    SIZE  size;
    WORD huge *pCacheWidth;
    WORD huge *pCharFontId;
    WORD chr,width;
    int  expand;

    if (mbcs && lead) {
       chr=MakeWord(lead,tail);
       pCacheWidth=screen?CharScrWidth:CharPrtWidth;    // cach width pointer
       pCharFontId=screen?CharScrFontId:CharPrtFontId;    // cach width pointer
       width=pCacheWidth[chr];                          // width from the cache
       if (pCharFontId[chr]==(WORD)font && width!=(WORD)0xFFFF) return (int)width;  // character width available in the cache

       string[0]=lead;          // calculate the character width
       string[1]=tail;
       string[2]=0;
       hDC=screen?hTerDC:hPr;
       hFont=screen?TerFont[font].hFont:PrtFont[font].hFont;
       hPrevFont=SelectObject(hDC,hFont);
       GetTextExtentPoint(hDC,string,2,&size);
       if (hPrevFont) SelectObject(hDC,hPrevFont);

       // apply character expansion
       expand=TerFont[font].expand;
       if (expand) {
          if (expand>MAX_EXPAND) expand=MAX_EXPAND;
          if (screen) expand=TwipsToScrX(expand);
          else        expand=TwipsToPrtX(expand);
          if (expand<0 && (-expand)>(size.cx/2)) expand=-size.cx/2;
          size.cx+=expand;
       }
       if (TerFont[font].CharWidth['A']==0) size.cx=0;   // hidden font

       pCharFontId[chr]=font;
       pCacheWidth[chr]=(WORD)size.cx;

       return size.cx;
    }
    else {
       if (!ShowParaMark && True(TerFont[font].style&PICT) && TerFont[font].FrameType!=PFRAME_NONE) return 1;  

       if (screen) WidthTable=TerFont[font].CharWidth;
       else        WidthTable=PrtFont[font].CharWidth;
       return (WidthTable!=NULL)?WidthTable[tail]:0;
    }
}

/******************************************************************************
   OldBuildMbcsCharWidth:
   Build the char width array for an mbcs string.
*******************************************************************************/
OldBuildMbcsCharWidth(PTERWND w,LPINT CharWidth, int FontId, LPBYTE MbText, int MbLen, LPBYTE tail, LPBYTE lead, int LeadLen)
{
   int i,j;

   if (mbcs && lead) {
     for (i=j=0;j<LeadLen;j++,i++) {
        if (lead[j]) {
           CharWidth[i]=CharWidth[i+1]=DblCharWidth(w,FontId,TRUE,lead[j],0);
           i++;
        }
        else CharWidth[i]=DblCharWidth(w,FontId,TRUE,tail[j],0);
     }
   }
   else for (i=0;i<MbLen;i++) CharWidth[i]=DblCharWidth(w,FontId,TRUE,MbText[i],0);

   return TRUE;
}


/*****************************************************************************
    OldDblCharWidth:
    This routine calculates the width combining lead and tail bytes
******************************************************************************/
OldDblCharWidth(PTERWND w, int font, BOOL screen, BYTE tail, BYTE lead)
{
    LPINT WidthTable;

    if (screen) WidthTable=TerFont[font].CharWidth;
    else        WidthTable=PrtFont[font].CharWidth;

    if (lead) return (WidthTable[lead]+WidthTable[lead]);  // double of lead
    else      return WidthTable[tail];
}

/******************************************************************************
    TerSetPageNumFmt:
    Set the page number fornmat.  The 'fmt' is set to one of the NBR_ constants
******************************************************************************/
bool WINAPI _export TerSetPageNumFmt(HWND hWnd, int sect, int fmt)
{
    int i;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (sect==SECT_CUR) {
       RecreateSections(w);               // to eliminate section boundary errors
       sect=GetSection(w,CurLine);
    }

    if (sect!=SECT_ALL && (sect<0 || sect>=TotalSects)) return false;

    for (i=0;i<TotalSects;i++) {
       if (sect!=SECT_ALL && sect!=i) continue;
       TerSect[i].PageNumFmt=fmt;
    }
    return true;
}

/******************************************************************************
    TerGetPageNumFmt:
    Retrieve the page number fornmat.
******************************************************************************/
int WINAPI _export TerGetPageNumFmt(HWND hWnd, int sect)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
 
      
    if (sect==SECT_CUR) {
       RecreateSections(w);               // to eliminate section boundary errors
       sect=GetSection(w,CurLine);
    }
    if (sect<0 || sect>=TotalSects) return -1;
    return TerSect[sect].PageNumFmt;
}


/******************************************************************************
    TerSetMargin:
    Set the document section margins. The margin values are passed using the TWIPS
    units. Pass a -1 for a margin value if you do not wish to modify that margin.
    This function return TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetMargin(HWND hWnd,int left, int right, int top, int bottom, BOOL refresh)
{
    return TerSetMarginEx(hWnd,SECT_CUR,left,right,top,bottom,-1,-1,refresh);
}

/******************************************************************************
    ForceSectOrient:
    Returns TRUE if a multiorient or multi-pagesize document is allowed to use only one orientation
******************************************************************************/
BOOL ForceSectOrient(PTERWND w)
{

    if (TerFlags&TFLAG_APPLY_PRT_PROPS) return true;

    if (pDeviceMode==NULL || !PrtDiffRes) return FALSE;

    //for (i=0;i<TotalSects;i++) {  // 05/06/05
    //    if (TerSect[i].orient!=pDeviceMode->dmOrientation) return TRUE;
    //    if (TerSect[i].PprSize!=PaperSize) return TRUE;
    //    if (TerSect[i].PprWidth!=PageWidth) return TRUE;
    //    if (TerSect[i].PprHeight!=PageHeight) return TRUE;
    //}

    return FALSE;
}

/******************************************************************************
    MultiOrient:
    This function returns TRUE if the document has multiple orientation sections.
******************************************************************************/
BOOL MultiOrient(PTERWND w)
{
   int orient=TerSect[0].orient;
   int i;

   for (i=1;i<TotalSects;i++) {
      if (TerSect[i].orient!=orient) return TRUE;
      if (TerSect[i].PprSize!=TerSect[0].PprSize) return TRUE;
      if (TerSect[i].PprWidth!=TerSect[0].PprWidth) return TRUE;
      if (TerSect[i].PprHeight!=TerSect[0].PprHeight) return TRUE;
   }

   return FALSE;
}

/******************************************************************************
    TerSetMarginEx:
    Set the document section margins. The margin values are passed using the TWIPS
    units. Pass a -1 for a margin value if you do not wish to modify that margin.
    This function return TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetMarginEx(HWND hWnd,int sect, int left, int right, int top, int bottom, int HeaderY, int FooterY, BOOL refresh)
{
    PTERWND w;
    int i;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (sect==SECT_CUR) {
       RecreateSections(w);               // to eliminate section boundary errors
       sect=GetSection(w,CurLine);
    }

    if (sect!=SECT_ALL && (sect<0 || sect>=TotalSects)) return FALSE;

    for (i=0;i<TotalSects;i++) {
       if (sect!=SECT_ALL && sect!=i) continue;

       if (left>=0) TerSect[i].LeftMargin=TwipsToInches(left);
       if (right>=0) TerSect[i].RightMargin=TwipsToInches(right);
       if (top!=-1) {
          if (top>=0) {
             TerSect[i].TopMargin=TwipsToInches(top);
             ResetUintFlag(TerSect[i].flags,SECT_EXACT_MARGT);
          }
          else {
             TerSect[i].TopMargin=TwipsToInches(-top);
             TerSect[i].flags|=SECT_EXACT_MARGT;
          } 
       }
       if (bottom!=-1) {
          if (bottom>=0) {
             TerSect[i].BotMargin=TwipsToInches(bottom);
             ResetUintFlag(TerSect[i].flags,SECT_EXACT_MARGB);
          }
          else {
             TerSect[i].BotMargin=TwipsToInches(-bottom);
             TerSect[i].flags|=SECT_EXACT_MARGB;
          } 
       }
       if (HeaderY>=0) TerSect[i].HdrMargin=TwipsToInches(HeaderY);
       if (FooterY>=0) TerSect[i].FtrMargin=TwipsToInches(FooterY);
    }

    TerArg.modified++;

    RequestPagination(w,true);

    if (refresh) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerGetMarginEx:
    Retrieve the margin information for the specified section.  Set the sect
    to SECT_CUR to use the current section.
******************************************************************************/
int WINAPI _export TerGetMarginEx(HWND hWnd,int sect, LPINT pLeft, LPINT pRight, LPINT pTop, LPINT pBottom, LPINT pHeaderY, LPINT pFooterY)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (sect==SECT_CUR) sect=GetSection(w,CurLine);
    if ((sect<0 || sect>=TotalSects)) return 0;

    if (pLeft)    (*pLeft)=(int)InchesToTwips(TerSect[sect].LeftMargin);
    if (pRight)   (*pRight)=(int)InchesToTwips(TerSect[sect].RightMargin);
    if (pTop)     {
       (*pTop)=(int)InchesToTwips(TerSect[sect].TopMargin);
       if (TerSect[sect].flags&SECT_EXACT_MARGT) (*pTop)=-(*pTop);
    }
    if (pBottom)  {
       (*pBottom)=(int)InchesToTwips(TerSect[sect].BotMargin);
       if (TerSect[sect].flags&SECT_EXACT_MARGB) (*pBottom)=-(*pBottom);
    }
    if (pHeaderY) (*pHeaderY)=(int)InchesToTwips(TerSect[sect].HdrMargin);
    if (pFooterY) (*pFooterY)=(int)InchesToTwips(TerSect[sect].FtrMargin);

    return TotalSects;
}

/******************************************************************************
TerGetSectFromLine:
Get the section id from the given line
******************************************************************************/
int WINAPI _export TerGetSectFromLine(HWND hWnd, int line)
{
	PTERWND w;

	if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

	if(line < 0 && line >= TotalLines) return 0;

	// wieso oder wieso nicht?
	RecreateSections(w);

	return GetSection(w,line);

}

/******************************************************************************
    TerGetPageSect:
    Get the section number for the first line of the specified page.
******************************************************************************/
int WINAPI _export TerGetPageSect(HWND hWnd,int page)
{
    PTERWND w;
    long FirstLine;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (page<0 || page>=TotalPages) return 0;

    FirstLine=PageInfo[page].FirstLine;
    return GetSection(w,FirstLine);
}

/******************************************************************************
    GetSection:
    Return the section where a given line resides.
******************************************************************************/
int GetSection(PTERWND w,long lin)
{
    long l;
    LPBYTE ptr;
    int i,len,FirstLine,LastLine,PrevLine,NextLine,PrevSect,NextSect,sect=-1;
    BOOL count;

    if (!TerArg.WordWrap) return 0;

    if (TotalSects==1) return 0;

    // check for given information
    if (KnownSect>=0 && lin>=KnownSectBegLine && lin<=KnownSectEndLine) return KnownSect;

    // Check if section line range is accurate
    for (i=0;i<TotalSects;i++) {
       if (!TerSect[i].InUse) continue;
       FirstLine=TerSect[i].FirstLine;
       LastLine=TerSect[i].LastLine;
       if (FirstLine<0 || FirstLine>=TotalLines) break;
       if (LastLine<0 || LastLine>=TotalLines) break;
       if (FirstLine>LastLine) break;

       // check the link to the previous section
       PrevSect=TerSect1[i].PrevSect;
       if (PrevSect>=0) PrevLine=TerSect[PrevSect].LastLine;
       else             PrevLine=-1;
       if (FirstLine!=(PrevLine+1)) break;
       
       // check the link to the next section
       NextSect=TerSect1[i].NextSect;
       if (NextSect>=0) NextLine=TerSect[NextSect].FirstLine;
       else             NextLine=TotalLines;
       if (LastLine!=(NextLine-1)) break;

       if (lin>=FirstLine && lin<=LastLine) sect=i;   // section found
    } 
    
    if (i<TotalSects) goto FIX_SECTION;
    else if (sect>=0) return sect;                           // section found
    
    
    FIX_SECTION:
    count=0;
    for (l=0;l<TotalLines;l++) {
       len=LineLen(l);
       ptr=pText(l);
       if (len>0 && ptr[len-1]==SECT_CHAR) {
          long IntParam;
          if (GetTag(w,l,len-1,TERTAG_SECT,NULL,NULL,&IntParam)>0) {
             sect=(int)IntParam;
             
             TerSect[sect].LastLine=l;
             
             NextSect=TerSect1[sect].NextSect;    // set the next section first line
             if (NextSect>=0) TerSect[NextSect].FirstLine=l+1;

             if (count==0) TerSect[sect].FirstLine=0;   // first section of the document
             count++;
          }
       } 
    } 

    TerSect[0].LastLine=TotalLines-1;

    for (i=1;i<TotalSects;i++) {  // check other than the default sections
       if (TerSect[i].InUse && lin>=TerSect[i].FirstLine && lin<=TerSect[i].LastLine) return i;
    }

    return 0;                     // default section
}

/******************************************************************************
    RecreateSections:
    Recreate section boundaries
******************************************************************************/
RecreateSections(PTERWND w)
{
    long l,FirstLine;
    long HdrFirstLine,HdrLastLine=0,FtrFirstLine,FtrLastLine=0;
    long FHdrFirstLine,FHdrLastLine=0,FFtrFirstLine,FFtrLastLine=0;
    int  i,sect,SavePageModifyCount,PrevSect=-1,NewSect,len;
    BOOL SectMoved=FALSE;
    LPBYTE ptr;

    if (!TerArg.WordWrap) return FALSE;

    // reset all section boundaries
    for (i=0;i<TotalSects;i++) TerSect[i].InUse=FALSE;

    // examine each line for the section character
    FirstLine=0;
    SavePageModifyCount=PageModifyCount;
    HdrFirstLine=FtrFirstLine=-1;
    FHdrFirstLine=FFtrFirstLine=-1;

    for (l=0;l<TotalLines;l++) {
       if (LineFlags(l)&LFLAG_FHDR) {
          if (FHdrFirstLine==-1) FHdrFirstLine=l;
          FHdrLastLine=l;
       }
       else if (LineFlags(l)&LFLAG_HDR) {
          if (HdrFirstLine==-1) HdrFirstLine=l;
          HdrLastLine=l;
       }
       else if (LineFlags(l)&LFLAG_FFTR) {
          if (FFtrFirstLine==-1) FFtrFirstLine=l;
          FFtrLastLine=l;
       }
       else if (LineFlags(l)&LFLAG_FTR) {
          if (FtrFirstLine==-1) FtrFirstLine=l;
          FtrLastLine=l;
       }

       // check if a section break found on this line
       NewSect=-1;
       len=LineLen(l);
       ptr=pText(l);
       if (len>0 && ptr[len-1]==SECT_CHAR) {
          long IntParam;
          if (GetTag(w,l,len-1,TERTAG_SECT,NULL,NULL,&IntParam)>0) NewSect=(int)IntParam;
          else                                                     NewSect=-1;
       }

       if (NewSect==-1) {     // no section break
          if (tabw(l) && tabw(l)->type&INFO_SECT) ResetUintFlag(tabw(l)->type,INFO_SECT);
       }  
       else {                // section break
          // check if tabw section set
          if (!tabw(l)) AllocTabw(w,l);
          tabw(l)->type|=INFO_SECT;    // set the sect flag
          tabw(l)->section=NewSect;
           
          sect=tabw(l)->section;
          TerSect[sect].InUse=TRUE;

          if ( TerSect[sect].FirstLine!=FirstLine
            || TerSect[sect].LastLine!=l) {
               SectMoved=TRUE;
               //PageModifyCount=-1;  // force repagination
               //if (FirstLine<RepageBeginLine) RepageBeginLine=FirstLine;
          }

          TerSect[sect].FirstLine=FirstLine;
          TerSect[sect].LastLine=l;
          if (FirstLine==0) TerSect[sect].flags|=SECT_NEW_PAGE;

          //if (PrevSect>=0 && TerSect[PrevSect].orient!=TerSect[sect].orient) TerSect[sect].flags|=SECT_NEW_PAGE;

          TerSect1[sect].PrevSect=PrevSect;
          TerSect1[sect].NextSect=-1;
          if (PrevSect>=0) TerSect1[PrevSect].NextSect=sect;

          TerSect1[sect].hdr.FirstLine=HdrFirstLine;
          TerSect1[sect].hdr.LastLine=HdrLastLine;
          TerSect1[sect].ftr.FirstLine=FtrFirstLine;
          TerSect1[sect].ftr.LastLine=FtrLastLine;
          TerSect1[sect].fhdr.FirstLine=FHdrFirstLine;
          TerSect1[sect].fhdr.LastLine=FHdrLastLine;
          TerSect1[sect].fftr.FirstLine=FFtrFirstLine;
          TerSect1[sect].fftr.LastLine=FFtrLastLine;
          PrevSect=sect;

          HdrFirstLine=FtrFirstLine=-1;   // initialize for the next section
          FHdrFirstLine=FFtrFirstLine=-1;   // initialize for the next section

          FirstLine=l+1;           // set for the next section
       }
    }

    // set the default section
    if ( TerSect[0].FirstLine!=FirstLine) SectMoved=TRUE;
    //if ( TerSect[0].FirstLine!=FirstLine) PageModifyCount=-1; // force repagination

    TerSect[0].FirstLine=FirstLine;
    TerSect[0].LastLine=TotalLines-1;
    //if (PrevSect>=0 && TerSect[PrevSect].orient!=TerSect[0].orient) TerSect[0].flags|=SECT_NEW_PAGE;

    TerSect1[0].PrevSect=PrevSect;

    TerSect1[0].NextSect=-1;
    if (PrevSect>=0) TerSect1[PrevSect].NextSect=0;

    TerSect1[0].hdr.FirstLine=HdrFirstLine;
    TerSect1[0].hdr.LastLine=HdrLastLine;
    TerSect1[0].ftr.FirstLine=FtrFirstLine;
    TerSect1[0].ftr.LastLine=FtrLastLine;
    TerSect1[0].fhdr.FirstLine=FHdrFirstLine;
    TerSect1[0].fhdr.LastLine=FHdrLastLine;
    TerSect1[0].fftr.FirstLine=FFtrFirstLine;
    TerSect1[0].fftr.LastLine=FFtrLastLine;

    TerSect[0].InUse=TRUE;         // first section always in use

    SetSectPageSize(w);            // set the page sizes for the section

    SectModified=FALSE;            // Sections recreated

    if (PageModifyCount!=SavePageModifyCount) return TRUE;   // sections modified
    if (SectMoved) return TRUE;   // sections modified
    else           return FALSE;  // sections not modified
}

/******************************************************************************
    AdjustSections:
    This routine is called to adjust the section boundaries when text lines
    are inserted or deleted.
******************************************************************************/
AdjustSections(PTERWND w,long AfterLine, long count)
{
    int i;

    for (i=0;i<TotalSects;i++) {
       if (!TerSect[i].InUse) continue;

       if (TerSect[i].FirstLine>AfterLine && TerSect[i].FirstLine>0) TerSect[i].FirstLine+=count;
       if (TerSect[i].LastLine>AfterLine) TerSect[i].LastLine+=count;

       if (TerSect1[i].hdr.FirstLine>AfterLine) TerSect1[i].hdr.FirstLine+=count;
       if (TerSect1[i].hdr.LastLine>AfterLine) TerSect1[i].hdr.LastLine+=count;
       if (TerSect1[i].ftr.FirstLine>AfterLine) TerSect1[i].ftr.FirstLine+=count;
       if (TerSect1[i].ftr.LastLine>AfterLine) TerSect1[i].ftr.LastLine+=count;
    }

    TerSect[0].LastLine=TotalLines-1;    // end the default section

    SectModified=TRUE;

    return TRUE;
}

/*****************************************************************************
    ExpandSectArray:
    Expand section table arrays.  Set NewMax to -1 to expand by one third
******************************************************************************/
BOOL ExpandSectArray(PTERWND w,int NewMax)
{
   int SaveMaxSects=MaxSects;

   if (!ExpandArray(w,sizeof(struct StrSect),(void far *(huge *))&TerSect,&MaxSects,NewMax,-MAX_SECTS)) return FALSE;
   MaxSects=SaveMaxSects;
   if (!ExpandArray(w,sizeof(struct StrSect1),(void far *(huge*))&TerSect1,&MaxSects,NewMax,-MAX_SECTS)) return FALSE;

   return TRUE;
}

/******************************************************************************
    TerSetOverrideBin:
    Set the bin for subsequent copies while printing.
*******************************************************************************/
BOOL WINAPI _export TerSetOverrideBin(HWND hWnd,BOOL dialog, int StartCopy, int bin)
{
    PTERWND w;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!PrinterAvailable || !pDeviceMode) {
       OverrideBinCopy=0;  // disable
       dialog=FALSE;
    } 

    if (dialog) {
       if (!CallDialogBox(w,"OverrideBinParam",OverrideBinParam,0L)) return TRUE;
       StartCopy=DlgInt1;
       bin=DlgInt2;
    }
    
    OverrideBinCopy=StartCopy;
    OverrideBin=bin;
  
    return TRUE;      
}
 
/******************************************************************************
    TerGetOverrideBin:
    Retrieve the bin for subsequent copies while printing.
*******************************************************************************/
BOOL WINAPI _export TerGetOverrideBin(HWND hWnd,LPINT StartCopy, LPINT bin)
{
    PTERWND w;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (StartCopy) (*StartCopy)=OverrideBinCopy;
    if (bin)       (*bin)      =OverrideBin;

    return TRUE;
}

/******************************************************************************
    TerSetSectOrient:
    Set the current section orientation to DMORIENT_PORTRAIT or
    DMORIENT_LANDSCAPE.
*******************************************************************************/
BOOL WINAPI _export TerSetSectOrient(HWND hWnd,int orient, BOOL repaint)
{
    PTERWND w;
    int sect,i;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (orient!=DMORIENT_PORTRAIT && orient!=DMORIENT_LANDSCAPE) return FALSE;

    if (PrtDiffRes && pDeviceMode && pDeviceMode->dmOrientation!=orient) OpenCurPrinterOrient(w,orient,FALSE);

    sect=GetSection(w,CurLine);
    TerSect[sect].orient=orient;

    if (ForceSectOrient(w)) for (i=0;i<TotalSects;i++) TerSect[i].orient=orient;  // only one orientation supported

    SetSectPageSize(w);      // set the page sizes for the sections

    if (repaint) {           // no repagination when !repaint as input routine uses this function
       if (TerArg.PrintView) {
          Repaginate(w,FALSE,FALSE,0,TRUE);
          PageModifyCount=TerArg.modified++;
          RepageBeginLine=0;
       }
       PaintTer(w);
    }

    return TRUE;
}

/******************************************************************************
    TerSetSectTextFlow:
    Set the rtl text flow for a section.
*******************************************************************************/
BOOL WINAPI _export TerSetSectTextFlow(HWND hWnd,int sect, int flow, BOOL repaint)
{
    PTERWND w;
    int i;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (flow!=FLOW_DEF && flow!=FLOW_RTL && flow!=FLOW_LTR) return FALSE;

    if (sect==SECT_CUR)      sect=GetSection(w,CurLine);
    else if (sect==SECT_ALL) sect=-1;
    else if (sect<0 || sect>=TotalSects) return FALSE;

    for (i=0;i<TotalSects;i++) {
       if (sect>=0 && i!=sect) continue;

       TerSect[i].flow=flow;
       
       if (sect>=0) break;     // only one section changes
    } 


    TerArg.modified++;

    if (repaint) TerRepaginate(hWnd,TRUE);           // no repagination when !repaint as input routine uses this function
    else         RequestPagination(w,TRUE);

    return TRUE;
}

/******************************************************************************
    TerSetSectLineNbr:
    Set the section line numbering property.
*******************************************************************************/
BOOL WINAPI _export TerSetSectLineNbr(HWND hWnd,int sect, BOOL set, BOOL repaint)
{
    PTERWND w;
    int i;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (sect==SECT_CUR)      sect=GetSection(w,CurLine);
    else if (sect==SECT_ALL) sect=-1;
    else if (sect<0 || sect>=TotalSects) return FALSE;

    for (i=0;i<TotalSects;i++) {
       if (sect>=0 && i!=sect) continue;

       if (set) TerSect[i].flags|=SECT_LINE;
       else     ResetUintFlag(TerSect[i].flags,SECT_LINE);
       
       if (sect>=0) break;     // only one section changes
    } 


    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}


/******************************************************************************
    TerSetSectBorder:
    Set page border for the current section.  The 'sect' variable can be set to
    SECT_CUR, SECT_ALL or a section number
*******************************************************************************/
BOOL WINAPI _export TerSetSectBorder(HWND hWnd,int sect, int BorderType, int width, int space, COLORREF color, BOOL repaint)
{
    PTERWND w;
    int i,j;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (BorderType<0 || BorderType>BRDRTYPE_LAST) return FALSE;
    
    if (sect==SECT_CUR)      sect=GetSection(w,CurLine);
    else if (sect==SECT_ALL) sect=-1;
    else if (sect<0 || sect>=TotalSects) return FALSE;

    for (i=0;i<TotalSects;i++) {
       if (sect>=0 && i!=sect) continue;

       TerSect[i].BorderType=BorderType;
       
       if (BorderType==BRDRTYPE_NONE) TerSect[i].border=0; // no border
       else {
          TerSect[i].border=0xF;   // set all 4 borders
          for (j=0;j<4;j++) {
             TerSect[i].BorderWidth[j]=width;
             TerSect[i].BorderSpace[j]=space;
          }
          TerSect[i].BorderColor=color;
          TerSect[i].BorderOpts=32;     // always on top, relative to page
       }
       
       if (sect>=0) break;     // only one section changes
    } 

    TerArg.modified++;

    if (repaint) PaintTer(w);           // no repagination when !repaint as input routine uses this function

    return TRUE;
}

/******************************************************************************
    TerSetSectVertAlign:
    Set the serction vertical alignment. The 'sect' variable can be set to
    SECT_CUR, SECT_ALL or a section number
*******************************************************************************/
BOOL WINAPI _export TerSetSectVertAlign(HWND hWnd,int sect, int valign, BOOL repaint)
{
    PTERWND w;
    int i;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (valign!=0 && valign!=SECT_VALIGN_CTR && valign!=SECT_VALIGN_BOT) return false;

    if (sect==SECT_CUR)      sect=GetSection(w,CurLine);
    else if (sect==SECT_ALL) sect=-1;
    else if (sect<0 || sect>=TotalSects) return false;

    for (i=0;i<TotalSects;i++) {
       if (sect>=0 && i!=sect) continue;

       ResetUintFlag(TerSect[i].flags,(SECT_VALIGN_CTR|SECT_VALIGN_BOT));
       if      (valign==SECT_VALIGN_CTR) TerSect[i].flags|=SECT_VALIGN_CTR;
       else if (valign==SECT_VALIGN_BOT) TerSect[i].flags|=SECT_VALIGN_BOT;
       
       if (sect>=0) break;     // only one section changes
    } 

    TerArg.modified++;

    if (repaint) TerRepaginate(hTerWnd,true);           // no repagination when !repaint as input routine uses this function
    else         RequestPagination(w,true);

    return true;
}

/******************************************************************************
    TerGetSectParam:
    Get the section parameters
******************************************************************************/
int WINAPI _export TerGetSectParam(HWND hWnd, int sect, int type)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FP_ERROR;  // get the pointer to window data

    if (sect==SECT_CUR) {
       RecreateSections(w);               // to eliminate section boundary errors
       sect=GetSection(w,CurLine);
    }

    if (type==SP_FLAGS) return TerSect[sect].flags;

    return FP_ERROR;
}




/******************************************************************************
    TerSetSectPageSize:
    Set page size for the current section.  The 'sect' variable can be set to
    SECT_CUR, SECT_ALL or a section number
*******************************************************************************/
BOOL WINAPI _export TerSetSectPageSize(HWND hWnd,int sect, int size, int ParamWidth, int ParamHeight, BOOL repaint)
{
    PTERWND w;
    int i;
    float width,height;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (sect==SECT_CUR)      sect=GetSection(w,CurLine);
    else if (sect==SECT_ALL) sect=-1;
    else if (sect<0 || sect>=TotalSects) return FALSE;

    // calculate width and heights
    for (i=0;i<MAX_PAPERS;i++) if (DefPaperSize[i]==size) break;  // check if one of the defined paper size
    if (i<MAX_PAPERS) {
       width=DefPaperWidth[i];
       height=DefPaperHeight[i];
    } 
    else {
       width=(float)ParamWidth/1440;
       height=(float)ParamHeight/1440;
    } 
    

    for (i=0;i<TotalSects;i++) {
       if (sect>=0 && i!=sect) continue;

       TerSect[i].PprSize=size;
       TerSect[i].PprWidth=width;
       TerSect[i].PprHeight=height;
       
       if (sect>=0) break;     // only one section changes
    } 

    SetSectPageSize(w);

    if (repaint) {
       if (TerArg.PrintView) {
          Repaginate(w,FALSE,FALSE,0,TRUE);
          PageModifyCount=TerArg.modified++;
          RepageBeginLine=0;
       }
       PaintTer(w);           // no repagination when !repaint as input routine uses this function
    }

    return TRUE;
}

/******************************************************************************
    SetSectPageSize:
    Set the page sizes for the sections
*******************************************************************************/
SetSectPageSize(PTERWND w)
{
    int i;

    for (i=0;i<TotalSects;i++) {
       if (TerSect[i].orient==DMORIENT_PORTRAIT 
           || (TerOpFlags&TOFLAG_EXTERNAL_DC && !(TerFlags4&TFLAG4_REPORT_EASE_MODE) && !(TerOpFlags2&TOFLAG2_PDF))) {
          TerSect1[i].PgWidth=TerSect[i].PprWidth;
          TerSect1[i].PgHeight=TerSect[i].PprHeight;
          TerSect1[i].HiddenX=0; // PortraitHX;
          TerSect1[i].HiddenY=0; // PortraitHY;
       }
       else {
          TerSect1[i].PgWidth=TerSect[i].PprHeight;
          TerSect1[i].PgHeight=TerSect[i].PprWidth;
          TerSect1[i].HiddenX=0; //LandscapeHX;
          TerSect1[i].HiddenY=0; //LandscapeHY;
       }

       // make sure that margin don't exceed the page dimensions
       if (!(TerOpFlags&TOFLAG_IN_MERGE_PRINT)) {
          if (TerSect[i].LeftMargin+TerSect[i].RightMargin>(TerSect1[i].PgWidth-MIN_TEXT_WIDTH)) {
             TerSect[i].RightMargin=TerSect1[i].PgWidth-(float)MIN_TEXT_WIDTH-TerSect[i].LeftMargin;
             if (TerSect[i].RightMargin<0) {
                TerSect[i].LeftMargin+=TerSect[i].RightMargin;
                TerSect[i].RightMargin=(float)0;
             }
          }
          
          if ((TerSect[i].TopMargin+TerSect[i].BotMargin>(TerSect1[i].PgHeight-MIN_TEXT_HEIGHT)) 
               && False(TerFlags5&TFLAG5_VARIABLE_PAGE_SIZE)) {
                
             TerSect[i].BotMargin=TerSect1[i].PgHeight-(float)MIN_TEXT_HEIGHT-TerSect[i].TopMargin;
             if (TerSect[i].BotMargin<0) {
                TerSect[i].TopMargin+=TerSect[i].BotMargin;
                TerSect[i].BotMargin=(float)0;
             }
          }
       }
    }
    return TRUE;
}

/******************************************************************************
    TerSetSectAuxId:
    Set the section aux id.
*******************************************************************************/
BOOL WINAPI _export TerSetSectAuxId(HWND hWnd,int sect, int AuxId)
{
    PTERWND w;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (sect<0 || sect>=TotalSects) return FALSE;
 
    TerSect[sect].AuxId=AuxId;

    return TRUE;
}
 
/******************************************************************************
    TerGetSectAuxId:
    get the section aux id.
*******************************************************************************/
int WINAPI _export TerGetSectAuxId(HWND hWnd,int sect)
{
    PTERWND w;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (sect<0 || sect>=TotalSects) return 0;
 
    return TerSect[sect].AuxId;
}
 
/******************************************************************************
    TerSetSect:
    Set the section parameters.
******************************************************************************/
BOOL WINAPI _export TerSetSect(HWND hWnd, int NumCols, int ColSpace, BOOL StartPage)
{
    return TerSetSectEx(hWnd,NumCols,ColSpace,StartPage,0);
}

/******************************************************************************
    TerSetSectEx:
    Set the section parameters.
******************************************************************************/
BOOL WINAPI _export TerSetSectEx(HWND hWnd, int NumCols, int ColSpace, BOOL StartPage, int FirstPageNo)
{
    return TerSetSect2(hWnd,NumCols,ColSpace,StartPage,FirstPageNo,-1,-1);
}

/******************************************************************************
    TerSetSect2:
    Set the section parameters.
******************************************************************************/
BOOL WINAPI _export TerSetSect2(HWND hWnd, int NumCols, int ColSpace, BOOL StartPage, int FirstPageNo, int FirstPageBin, int NextPageBin)
{
    return TerSetSect3(hWnd,NumCols,ColSpace,StartPage,FirstPageNo,FirstPageBin,NextPageBin,-1);
}

/******************************************************************************
    TerSetSect3:
    Set the section parameters.
******************************************************************************/
BOOL WINAPI _export TerSetSect3(HWND hWnd, int NumCols, int ColSpace, BOOL StartPage, int FirstPageNo, int FirstPageBin, int NextPageBin, int sect)
{
   return TerSetSect4(hWnd,NumCols,ColSpace,StartPage,FirstPageNo,FirstPageBin,NextPageBin,sect,-1,0,0);
}

/******************************************************************************
    TerSetSect4:
    Set the section parameters.
    Set PaperType to -1 to leave the current value unchanged.  The PaperWidth and PaperHeight
    are specified in twips and are specified for the portrait orientaion.
******************************************************************************/
BOOL WINAPI _export TerSetSect4(HWND hWnd, int NumCols, int ColSpace, BOOL StartPage, int FirstPageNo, int FirstPageBin, int NextPageBin, int sect, int PaperType, int PaperWidth, int PaperHeight)
{
    PTERWND w;
    int PrevSect;
    BOOL result=TRUE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (NumCols!=0) {   // use the specified values
       RecreateSections(w);               // to eliminate section boundary errors
       if (sect<0 || sect>=TotalSects) sect=GetSection(w,CurLine);

       if (NumCols>0) TerSect[sect].columns=NumCols;
       TerSect[sect].ColumnSpace=TwipsToInches(ColSpace);
       if (StartPage) TerSect[sect].flags|=SECT_NEW_PAGE;
       else           TerSect[sect].flags=ResetUintFlag(TerSect[sect].flags,SECT_NEW_PAGE);

       if (FirstPageNo>0) {
          TerSect[sect].flags|=SECT_RESTART_PAGE_NO;
          TerSect[sect].FirstPageNo=FirstPageNo;
       }
       else TerSect[sect].flags=ResetUintFlag(TerSect[sect].flags,SECT_RESTART_PAGE_NO);

       if (FirstPageBin>=0) TerSect[sect].FirstPageBin=FirstPageBin;
       if (NextPageBin>=0) TerSect[sect].bin=NextPageBin;

       if (PaperType>=0) {
         TerSect[sect].PprSize=PaperType;
         TerSect[sect].PprWidth=TwipsToInches(PaperWidth);
         TerSect[sect].PprHeight=TwipsToInches(PaperHeight);
       } 
    }
    else if (NumCols==0) {
       if (result=CallDialogBox(w,"SectionParam",SectionParam,0L));
       sect=GetSection(w,CurLine);
    }

    // update the document
    if (result) {
       TerArg.modified++;
       PageModifyCount=0;      // force repagination

       PrevSect=TerSect1[sect].PrevSect;
       if (PrevSect>=0 && TerSect[sect].orient!=TerSect[PrevSect].orient) TerSect[sect].flags|=SECT_NEW_PAGE;

       SetSectPageSize(w);    // set the section page sizes

       SectModified=TRUE;

       // recalculate the wrap width and display
       if (TerArg.PrintView) {
          PaintTer(w);                         // repaint

          if (RepageBeginLine>(TerSect[sect].FirstLine-1)) RepageBeginLine=TerSect[sect].FirstLine-1;
          if (RepageBeginLine<0) RepageBeginLine=0;
          PostMessage(hTerWnd,TER_IDLE,0,0L);  // induce idle time activity
       }
    }

    return TRUE;
}

/******************************************************************************
    TerGetSectInfo:
    Get the section parameters.
******************************************************************************/
BOOL WINAPI _export TerGetSectInfo(HWND hWnd, LPINT NumCols, LPINT ColSpace, LPINT StartPage, LPINT FirstPageNo)
{
    PTERWND w;
    int sect;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    RecreateSections(w);               // to eliminate section boundary errors
    sect=GetSection(w,CurLine);

    if (NumCols)    (*NumCols)=TerSect[sect].columns;
    if (ColSpace)   (*ColSpace)=(int)(TerSect[sect].ColumnSpace*1440);  // in twips
    if (StartPage)  (*StartPage)=TerSect[sect].flags&SECT_NEW_PAGE;
    if (FirstPageNo){
       (*FirstPageNo)=0;
       if (TerSect[sect].flags&SECT_RESTART_PAGE_NO) (*FirstPageNo)=TerSect[sect].FirstPageNo;
    }

    return TRUE;
}

/******************************************************************************
    TerGetSectBins:
    Get the section parameters.
******************************************************************************/
BOOL WINAPI _export TerGetSectBins(HWND hWnd, int sect, LPINT FirstPageBin, LPINT NextPageBin)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (sect==SECT_CUR) {
       RecreateSections(w);               // to eliminate section boundary errors
       sect=GetSection(w,CurLine);
    }

    if (FirstPageBin) (*FirstPageBin)=TerSect[sect].FirstPageBin;
    if (NextPageBin)  (*NextPageBin)=TerSect[sect].bin;

    return true;
}


/******************************************************************************
    TerGetSeqSect:
    Translate a section id into a sequential section number.
******************************************************************************/
int WINAPI _export TerGetSeqSect(HWND hWnd, int sect)
{
    PTERWND w;
    int SectNo;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    if (sect>=TotalSects) return -1;

    RecreateSections(w);               // to eliminate section boundary errors

    SectNo=0;

    while(TerSect1[sect].PrevSect>=0) {
       SectNo++;
       sect=TerSect1[sect].PrevSect;
    }

    return SectNo;
}

/*******************************************************************************
   TerCopyHeadersFooters:
   Copy header footers from one section to another
*******************************************************************************/
BOOL WINAPI _export TerCopyHeadersFooters(HWND hWnd, int SrcSect, int DestSect, 
                    BOOL CopyHdr, BOOL CopyFtr, BOOL CopyFirstHdr, BOOL CopyFirstFtr,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE; // get the pointer to window data
    if (SrcSect==DestSect) return TRUE;                 // no need to copy

    if (GetFirstSectLine(w,SrcSect)<0) return FALSE;    // source section not found
    if (GetFirstSectLine(w,DestSect)<0) return FALSE;   // dest section not found
 
    if (CopyFtr && !CopyHeaderFooter(w,SrcSect,DestSect,FTR_CHAR)) return FALSE;
    if (CopyFirstFtr && !CopyHeaderFooter(w,SrcSect,DestSect,FFTR_CHAR)) return FALSE;
    if (CopyHdr && !CopyHeaderFooter(w,SrcSect,DestSect,HDR_CHAR)) return FALSE;
    if (CopyFirstHdr && !CopyHeaderFooter(w,SrcSect,DestSect,FHDR_CHAR)) return FALSE;

    TerRepaginate(hWnd,repaint);

    return TRUE;
}

/*******************************************************************************
    CopyHeaderFooter:
    Copy header or footer from one section (sequential section number) to another.
    The 'delim' character specfies header/footer delimiter characters.
    This function currently does NOT support header/footer with tables or frames.
*******************************************************************************/
CopyHeaderFooter(PTERWND w,int SrcSect, int DestSect, BYTE delim)
{
    long l,SrcLine,DestLine,count;

    // locate the source lines to copy
    if ((SrcLine=GetFirstSectLine(w,SrcSect))<0) return FALSE;
    
    if (!GetHdrFtrRange(w,delim,SrcLine,&SrcLine,&count)) return TRUE;  // hdr/ftr not found
    
    // check if the header/footer lines belong to a table or frame
    for (l=SrcLine;l<(SrcLine+count);l++) if (cid(l) || fid(l)) return FALSE;
    
    if ((DestLine=GetFirstSectLine(w,DestSect))<0) return FALSE;

    // make room for additional lines
    MoveLineArrays(w,DestLine,count,'B');

    // adjust other lines for the movement
    if (DestLine<SrcLine) SrcLine+=count;       // source line moved 

    if (HilightType!=HILIGHT_OFF && DestLine<HilightBegRow) HilightBegRow+=count;
    if (HilightType!=HILIGHT_OFF && DestLine<HilightEndRow) HilightEndRow+=count;

    // copy the source lines
    for (l=0;l<count;l++,SrcLine++,DestLine++) {
       CopyLineData(w,SrcLine,DestLine);
       
       cid(DestLine)=0;
       fid(DestLine)=0;
    }
     
    // delete any old hdr/ftr in the section
    if (GetHdrFtrRange(w,delim,DestLine,&DestLine,&count)) {
       MoveLineArrays(w,DestLine,count,'D');
       
       if (HilightType!=HILIGHT_OFF && DestLine<HilightBegRow) HilightBegRow+=count;
       if (HilightType!=HILIGHT_OFF && DestLine<HilightEndRow) HilightEndRow+=count;
    } 

    return TRUE;
}
 
/*******************************************************************************
    GetHdrFtrFlag:
    Get the LFLAG_ constant associated for given text line.
*******************************************************************************/
DWORD GetHdrFtrFlag(PTERWND w,long LineNo)
{
    long l;

    if (LineNo<0 || LineNo>=TotalLines) return 0;

    if (!(PfmtId[pfmt(LineNo)].flags&PAGE_HDR_FTR)) return 0;  // not in header footer

    for (l=LineNo;l<TotalLines;l++) if (LineFlags(l)&LFLAG_HDRS_FTRS) return (LineFlags(l)&LFLAG_HDRS_FTRS);
 
    return 0;
}
 
/*******************************************************************************
    GetHdrFtrRange:
    Get range of lines for the given header footer. The scanning starts at the given
    line and ends when out of header/footer area.
*******************************************************************************/
BOOL GetHdrFtrRange(PTERWND w,BYTE delim, long StartLine, LPLONG pFirstLine, LPLONG pCount)
{
    long l,FirstLine,count;
    BOOL found;
    int ParaId;

    found=FALSE;
    count=0;               // number of lines to copy
    for (l=StartLine;l<TotalLines;l++) {
       ParaId=pfmt(l);
       if (!(PfmtId[ParaId].flags&PAGE_HDR_FTR)) return FALSE;  // nothing to copy

       if (found) count++;

       if (LineLen(l)==1 && pText(l)[0]==delim) {
          if (!found) {
             found=TRUE;
             FirstLine=l;
             count++;
          }
          else break;
       } 
    }
    if (!found || l==TotalLines) return FALSE;   // nothing to copy 
 
    if (pFirstLine) (*pFirstLine)=FirstLine;
    if (pCount)     (*pCount)=count;
 
    return TRUE;
}
 
/*******************************************************************************
    GetFirstSectLine:
    Get the first line of a section (secquential section number).  This function
    returns -1 if a sequential section is not found.
*******************************************************************************/
long GetFirstSectLine(PTERWND w,int sec)
{
    long l;
    int len;

    if (sec==0) return 0L;
 
    for (l=0;l<TotalLines;l++) {
       len=LineLen(l);
       if (len>0 && pText(l)[len-1]==SECT_CHAR) sec--;  // section delimter found
       if (sec==0) {
         if ((l+1)<TotalLines) l++;   // first section line begins after the delimter
         return l;
       }  
    } 

    return -1;
} 

/******************************************************************************
    TerSetWaveUnderline:
    Set or reset squiggley underline for the give text.
******************************************************************************/
BOOL WINAPI _export TerSetWaveUnderline(HWND hWnd, long LineNo, int StartCol, int EndCol, BOOL set, BOOL repaint)
{
    PTERWND w;
    int CurFont,PrevFont,len,i,NewFont;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    len=LineLen(LineNo); 
    if (EndCol>(len-1)) EndCol=len-1;

    if (len<0) return TRUE;
    
    fmt=OpenCfmt(w,LineNo);
    PrevFont=-1;
    for (i=StartCol;i<=EndCol;i++) {
       CurFont=fmt[i];
       if (CurFont!=PrevFont) {
          NewFont=SetFontFlags(w,CurFont,FFLAG_AUTO_SPELL,set);  // turn-off the flag
          PrevFont=CurFont;
       } 
       fmt[i]=(WORD)NewFont; 
    } 

    TerOpFlags|=TOFLAG_EXT_SPELLER;      // to emable squigley underline during display

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    IsEnglishChar:
    Check if a given character in the line is to be assigned english font
******************************************************************************/
BOOL IsEnglishChar(PTERWND w, LPBYTE ptr, int col, int len)
{
    BYTE CurChar=ptr[col];
    int TempCol;

    if ((CurChar>='a' && CurChar<='z') || (CurChar>='A' && CurChar<='Z')) return TRUE;
    if (CurChar>='0' && CurChar<='9') return (DefLang==ENGLISH);

    if (CurChar==' ') {
      for (TempCol=col-1;TempCol>=0;TempCol--) if (ptr[TempCol]!=' ') break;   // look for non-space character backward
      if (TempCol>=0) {
         CurChar=ptr[TempCol];
         if ((CurChar>='a' && CurChar<='z') || (CurChar>='A' && CurChar<='Z')) return TRUE;
         if (CurChar>='0' && CurChar<='9') return (DefLang==ENGLISH);
      } 
      for (TempCol=col+1;TempCol<len;TempCol++) if (ptr[TempCol]!=' ') break;   // look for non-space character backward
      if (TempCol<len) {
         CurChar=ptr[TempCol];
         if ((CurChar>='a' && CurChar<='z') || (CurChar>='A' && CurChar<='Z')) return TRUE;
         if (CurChar>='0' && CurChar<='9') return (DefLang==ENGLISH);
      } 
    }

    return FALSE;
} 

/******************************************************************************
    SetCurLangFont:
    Set font for the curent language.
******************************************************************************/
int SetCurLangFont(PTERWND w, int CurFont)
{
   return SetCurLangFont2(w,CurFont,(HKL)0);
}

/******************************************************************************
    SetCurLangFont2:
    Set font for the curent language.
******************************************************************************/
int SetCurLangFont2(PTERWND w, int CurFont, HKL hkl)
{

    if ((UINT)hkl==0) hkl=GetKeyboardLayout(0);

    if (hkl!=CurHKL) SetCurLang(w,hkl);    // set the keyboard layout

    if (TerFont[CurFont].CharSet==SYMBOL_CHARSET) return CurFont;  // symbol char set does have any language association

    if (TerFont[CurFont].CharSet==ReqCharSet && TerFont[CurFont].lang==(int)ReqLang) return CurFont;

    return GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 (int)ReqLang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 ReqCharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle);
}


/******************************************************************************
    ResetUniFont:
    Reset any unicode font specification
******************************************************************************/
int ResetUniFont(PTERWND w, int CurFont)
{
    if (TerFont[CurFont].UcBase==0) return CurFont;   // unicode font not applied
 
    return GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,0,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle);
 
}

/******************************************************************************
    SetUniFont:
    Set UcBase for a font and return the new font id
******************************************************************************/
int SetUniFont(PTERWND w, int CurFont, WORD UcBase)
{
    if (TerFont[CurFont].UcBase==UcBase) return CurFont;   // unicode font not applied
 
    return GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle);
 
}

/******************************************************************************
    SetScapFont:
    Set or reset the SCAP font
******************************************************************************/
int SetScapFont(PTERWND w, int CurFont, BOOL set)
{
    UINT flags;

    if (TerFont[CurFont].style&PICT) return CurFont;   // do not modify a picture font
    if (TerFont[CurFont].UcBase)     return CurFont;   // scap not applicable to unicode characters
 
    flags=TerFont[CurFont].flags;
    if (set) flags|=FFLAG_SCAPS;
    else     ResetUintFlag(flags,FFLAG_SCAPS);

    return GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,flags,
                 TerFont[CurFont].TextAngle);
 
}

/******************************************************************************
    SetFontStyle:
    Set or reset the font style bits and return the new font
******************************************************************************/
int SetFontStyle(PTERWND w, int CurFont, UINT style, BOOL set)
{
    UINT NewStyle;

    if (TerFont[CurFont].style&PICT) return CurFont;   // do not modify a picture font
 
    NewStyle=TerFont[CurFont].style;
    if (set) NewStyle|=style;
    else     ResetUintFlag(NewStyle,style);

    return GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 NewStyle,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle);
 
}

/******************************************************************************
    SetFontCharId:
    Set or reset the CharId font
******************************************************************************/
int SetFontCharId(PTERWND w, int CurFont, int id)
{
    struct StrFont font;
      
    if (True(TerFont[CurFont].style&PICT)) return CurFont;   // do not modify a picture font
 
    font=TerFont[CurFont];  // deep copy not needed

    font.CharId=id;

    return GetNewFont2(w,hTerDC,CurFont,&font);
}


/******************************************************************************
    SetFontStyleId:
    Set or reset the font style bits and return the new font
******************************************************************************/
int SetFontStyleId(PTERWND w, int CurFont, int CharStyId, int ParaStyId)
{
    if (TerFont[CurFont].style&PICT) return CurFont;   // do not modify a picture font
 
    return GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 CharStyId,ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle);
 
}

/******************************************************************************
    SetFontFieldId:
    Set new field id for the font.
******************************************************************************/
int SetFontFieldId(PTERWND w, int CurFont, int FieldId, LPBYTE FieldCode)
{

    if (TerFont[CurFont].style&PICT) return CurFont;   // do not modify a picture font
 
    return GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 FieldId,
                 TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,
                 FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle);
 
}

/******************************************************************************
    SetFontFlags:
    Set or reset the font flag
******************************************************************************/
int SetFontFlags(PTERWND w, int CurFont, UINT flags, BOOL set)
{
    UINT FontFlags;

    if (TerFont[CurFont].style&PICT) return CurFont;   // do not modify a picture font
    if (TerFont[CurFont].UcBase)     return CurFont;   // scap not applicable to unicode characters
 
    FontFlags=TerFont[CurFont].flags;
    if (set) FontFlags|=flags;
    else     ResetUintFlag(FontFlags,flags);

    return GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,FontFlags,
                 TerFont[CurFont].TextAngle);
 
}

/*******************************************************************************
   TerSetTextCase:
   Change the case of the text block.
*******************************************************************************/
BOOL WINAPI _export TerSetTextCase(HWND hWnd, BOOL CaseType, BOOL repaint)
{
    long l, index=0;

    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE; // get the pointer to window data

    if (mbcs) return FALSE;
    if (HilightType==HILIGHT_OFF) return FALSE;
    if (!NormalizeBlock(w)) return FALSE;
    if (!NormalizeForFootnote(w)) return FALSE;    // make sure that footnotes are fully contained

    if (HilightType==HILIGHT_LINE) {      // set beginning and ending columns
        HilightBegCol=0;
        HilightEndCol=LineLen(HilightEndRow);
        if (HilightEndCol<0) HilightEndCol=0;
    }

    // Proces a single line buffer
    if (HilightBegRow==HilightEndRow) {   // single line block
        ChangeLetterCase(w,HilightBegRow,HilightBegCol,HilightEndCol-1,CaseType,(LPDWORD)&index);
        goto CLOSE;                      // close the clipboard
    }

    //*************** handle the first line of the multiline blocks **********

    ChangeLetterCase(w,HilightBegRow,HilightBegCol,LineLen(HilightBegRow)-1,CaseType,(LPDWORD)&index);

    //*************** handle middle lines of the multiline blocks ************
    for (l=HilightBegRow+1;l<HilightEndRow;l++)
         ChangeLetterCase(w,l,0,LineLen(l)-1, CaseType,(LPDWORD)&index);

    //*************** handle the last line of the multiline blocks ************
    ChangeLetterCase(w,HilightEndRow,0,HilightEndCol-1,CaseType,(LPDWORD)&index);

    CLOSE:

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/*******************************************************************************
    ChangeLetterCase:
    Change the case of the specified text.
*******************************************************************************/
int ChangeLetterCase(PTERWND w,long line,int BegCol,int EndCol,BOOL CaseType, LPDWORD StartIndex)
{
    LPBYTE ptr;
    LPWORD fmt;
    int i,len,CurFont;
    DWORD index=(*StartIndex);
    UINT SkippedStyles=(HIDDEN|FNOTETEXT|FNOTEREST);
    BOOL CopyEntireLine=FALSE;

    // can we copy the entire line
    if (cfmt(line).info.type==UNIFORM) {
       CurFont=cfmt(line).info.fmt;
       if (!(TerFont[CurFont].style&SkippedStyles)) CopyEntireLine=TRUE;
    }

    ptr=pText(line);

    // copy to the buffer
    if (CopyEntireLine) {

      len=EndCol-BegCol+1;
      if (len>0) {
          for (i=BegCol;i<=EndCol && i<LineLen(line);i++) {
             #if defined(WIN32)
                if (CaseType) CharUpperBuff(&ptr[i], 1);
                else          CharLowerBuff(&ptr[i], 1);
             #else
                if (isalpha(ptr[i])) {
                    if (CaseType)
                        ptr[i] = (BYTE) toupper(ptr[i]);
                    else
                        ptr[i] = (BYTE) tolower(ptr[i]);
                }
             #endif
         }
      }
    }
    else {
      fmt=OpenCfmt(w,line);
      len=0;
      for (i=BegCol;i<=EndCol && i<LineLen(line);i++) {
         if (!(TerFont[fmt[i]].style&SkippedStyles)) {
             #if defined(WIN32)
                if (CaseType) CharUpperBuff(&ptr[i], 1);
                else          CharLowerBuff(&ptr[i], 1);
             #else
                if (isalpha(ptr[i])) {
                    if (CaseType) ptr[i] = (BYTE) toupper(ptr[i]);
                    else          ptr[i] = (BYTE) tolower(ptr[i]);
                }
             #endif
             len++;
         }
      }
      CloseCfmt(w,line);
    }

    index=index+len;

    (*StartIndex)=index;

    return TRUE;
}

/******************************************************************************
    OpenTextPtr:
    Open the text pointers.
******************************************************************************/
OpenTextPtr(PTERWND w,long line,LPBYTE far *ptr, LPBYTE far *lead)
{
    (*ptr)=pText(line);
    if (mbcs) (*lead)=OpenLead(w,line);
    else      (*lead)=NULL;

    return TRUE;
}

/******************************************************************************
    CloseTextPtr:
    close the text pointers.
******************************************************************************/
CloseTextPtr(PTERWND w,long line)
{
    if (mbcs) CloseLead(w,line);
    return TRUE;
}

/******************************************************************************
    OpenCharInfo:
    Open the cfmt and ctid arrays.
******************************************************************************/
OpenCharInfo(PTERWND w,long line,LPWORD far *fmt, LPWORD far *cmi)
{
    if (cfmt(line).info.type==UNIFORM) ExpandCfmt(w,line);
    if (!pCtid(line))                  ExpandCtid(w,line);

    if (fmt) (*fmt)=cfmt(line).pFmt;
    if (cmi) (*cmi)=pCtid(line);

    return TRUE;
}

/******************************************************************************
    OpenCfmt:
    This routine expands if the character formatting information if needed. It
    then locks the handle returns the far pointer to the character formatting
    information.
******************************************************************************/
LPWORD OpenCfmt(PTERWND w,long line)
{
    if (cfmt(line).info.type==UNIFORM) ExpandCfmt(w,line);

    return cfmt(line).pFmt;
}

/******************************************************************************
    CloseCharInfo:
    This routine unlocks the character information array. and compresses
    the character information.  This routine must be called in conjuction
    with the 'OpenCfmt' routine.
******************************************************************************/
void CloseCharInfo(PTERWND w,long line)
{
    if (cfmt(line).info.type!=UNIFORM) CompressCfmt(w,line);
    if (pCtid(line)) CompressCtid(w,line);
}

/******************************************************************************
    CloseCfmt:
    This routine unlocks the character information array. and compresses
    the character information.  This routine must be called in conjuction
    with the 'OpenCfmt' routine.
******************************************************************************/
void CloseCfmt(PTERWND w,long line)
{
    if (cfmt(line).info.type!=UNIFORM) CompressCfmt(w,line);
}

/******************************************************************************
    CompressCfmt:
    This routine compresses the formatting information for a line.  If a line
    has a pointer to store the formatting information,  and the formatting
    information for all characters in the line is identical,  this information
    will be stored as UNIFORM type (one character) information.  This saves
    the memory and disk space.
*******************************************************************************/
void CompressCfmt(PTERWND w,long line)
{
    int i,SaveModified;
    WORD FirstFmt;
    LPWORD fmt;
    LPBYTE ptr;


    if   (cfmt(line).info.type==UNIFORM || (TerOpFlags2&TOFLAG2_NO_COMPRESS_CFMT)) return;

    // check for a tab character
    if  (TerArg.WordWrap) {
       if (LineLen(line)==0) {
          cfmt(line).info.type=UNIFORM;
          cfmt(line).info.fmt=DEFAULT_CFMT;
          return;
       }
       if (tabw(line) || LineFlags(line)&(LFLAG_NBSPACE|LFLAG_NBDASH|LFLAG_HYPH)) return;  // lines with tab/section info  can not be compressed
       if (LineFlags(line)&LFLAG2_HIDDEN_CHAR) return;
    }
    else {
       if (LineLen(line)>0) {
          ptr=pText(line);
          for (i=0;i<LineLen(line);i++) if (ptr[i]==TAB) break;

          if (i<LineLen(line)) return; // tab found
       }
    }

    fmt=cfmt(line).pFmt;

    if (LineLen(line)>0) FirstFmt=fmt[0];
    else                 FirstFmt=DEFAULT_CFMT;

    for (i=0;i<LineLen(line);i++) {
       if (fmt[i]!=FirstFmt) return;  // this format can not be compressed
    }

    SaveModified=TerArg.modified;          // save the file modification status
    FmtAlloc(w,line,LineLen(line),0);
    TerArg.modified=SaveModified;          // restore the file modification status

    cfmt(line).info.type=UNIFORM;
    cfmt(line).info.fmt=FirstFmt;

    return;
}

/******************************************************************************
    ExpandCfmt:
    This routine expands the formatting information for a line.
*******************************************************************************/
void ExpandCfmt(PTERWND w,long line)
{
    int i,SaveModified;
    WORD CurFmt;
    LPWORD fmt;

    if   (cfmt(line).info.type!=UNIFORM) return;   // already decompressed
    if   (LineLen(line)==0) return;                // zero length line can not be expanded

    CurFmt=cfmt(line).info.fmt;

    SaveModified=TerArg.modified;          // save the file modification status
    FmtAlloc(w,line,0,LineLen(line));      // allocate memory for the formatting info
    TerArg.modified=SaveModified;          // restore the file modification status

    fmt=cfmt(line).pFmt;
    for (i=0;i<LineLen(line);i++) {      // apply the uniform format
       fmt[i]=CurFmt;
    }

    return;
}

/******************************************************************************
    OpenCtid:
    This routine expands if the character miscellaneous id information
    if needed.
******************************************************************************/
LPWORD OpenCtid(PTERWND w,long line)
{
    ExpandCtid(w,line);

    return pCtid(line);
}

/******************************************************************************
    CloseCtid:
    This routine unlocks the character miscellaneous id array.
******************************************************************************/
void CloseCtid(PTERWND w,long line)
{
    CompressCtid(w,line);
}

/******************************************************************************
    CompressCtid:
    This routine compresses the character miscellaneous id information for a line.
*******************************************************************************/
void CompressCtid(PTERWND w,long line)
{
    int i,SaveModified;


    if   (!pCtid(line)) return;

    for (i=0;i<LineLen(line);i++) if (ctid(line,i)) return;  // non-zero id array can not be compressed

    SaveModified=TerArg.modified;          // save the file modification status
    CtidAlloc(w,line,LineLen(line),0);
    TerArg.modified=SaveModified;          // restore the file modification status

    return;
}

/******************************************************************************
    ExpandCtid:
    This routine expands the formatting information for a line.
*******************************************************************************/
void ExpandCtid(PTERWND w,long line)
{
    int SaveModified;

    if   (pCtid(line)) return;                // already decompressed
    if   (LineLen(line)==0) return;          // zero length line can not be expanded

    SaveModified=TerArg.modified;            // save the file modification status
    CtidAlloc(w,line,0,LineLen(line));       // allocate memory for the formatting info
    TerArg.modified=SaveModified;            // restore the file modification status

    return;
}

/******************************************************************************
    CloseLead:
    This routine compresses the lead bytes for the line
*******************************************************************************/
void CloseLead(PTERWND w,long line)
{
    int i,SaveModified;

    if (!mbcs) return;

    if   (!pLead(line)) return;

    for (i=0;i<LineLen(line);i++) if ((pLead(line)[i])) return;  // non-zero id array can not be compressed

    SaveModified=TerArg.modified;          // save the file modification status
    LeadAlloc(w,line,LineLen(line),0);
    TerArg.modified=SaveModified;          // restore the file modification status

    return;
}

/******************************************************************************
    OpenLead:
    This routine expands the formatting information for a line.
*******************************************************************************/
LPBYTE OpenLead(PTERWND w,long line)
{
    int SaveModified;

    if (!mbcs) return NULL;

    if   (pLead(line)) return pLead(line);   // already decompressed
    if   (LineLen(line)==0) return NULL;     // zero length line can not be expanded

    SaveModified=TerArg.modified;            // save the file modification status
    LeadAlloc(w,line,0,LineLen(line));       // allocate memory for the formatting info
    TerArg.modified=SaveModified;            // restore the file modification status

    return pLead(line);
}

/*****************************************************************************
    TerFonts:
    This routine display the available fonts and point sizes
*****************************************************************************/
TerFonts(PTERWND w)
{
    WORD CurFont;
    long  line;
    int   col;

    // get the current font
    if (HilightType==HILIGHT_LINE) {    // get line/col to read old font
       line=HilightBegRow;
       col=0;
    }
    else if (HilightType==HILIGHT_CHAR) {
       line=HilightBegRow;
       col=HilightBegCol;
    }
    else {
       line=CurLine;
       col=CurCol;
    }
    
    if (InputFontId>=0) CurFont=InputFontId;
    else                CurFont=GetEffectiveCfmt(w);

    if (!CallDialogBox(w,"FontParam",FontParam,(long)CurFont)) return FALSE;

    SetTerFont(hTerWnd,ReqTypeFace,FALSE);         // apply font without screen refresh
    SetTerPointSize(hTerWnd,-ReqTwipsSize,TRUE);   // apply new point size, refresh screen

    return TRUE;
}

/*****************************************************************************
    TerCreateFirstHdrFtr:
    Create the first page header or footer for the section
******************************************************************************/
BOOL WINAPI _export TerCreateFirstHdrFtr(HWND hWnd,BOOL HdrFtr)
{
    PTERWND w;
    int sect;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (!TerArg.WordWrap || !TerArg.PageMode) return FALSE;

    if (!EditPageHdrFtr) ToggleEditHdrFtr(w);      // turnon the editing of header footer
    if (!EditPageHdrFtr) return FALSE;

    // check if first header/footer already exists
    RecreateSections(w);     // recreate sections
    sect=GetSection(w,CurLine);
    if (HdrFtr && TerSect1[sect].fhdr.FirstLine>=0) return FALSE;  // already exists
    if (!HdrFtr && TerSect1[sect].fftr.FirstLine>=0) return FALSE;  // already exists

    ReleaseUndo(w);     // release any existing undos

    TerSect[sect].flags|=SECT_HAS_TITLE_PAGE;
    CreatePageHdrFtr(w,(BYTE)(HdrFtr?FHDR_CHAR:FFTR_CHAR),sect);
    RecreateSections(w);

    if (HdrFtr) CurLine=TerSect1[sect].fhdr.FirstLine+1;   // position beyond the
    else        CurLine=TerSect1[sect].fftr.FirstLine+1;   // position beyond the
    if (CurLine<0) CurLine=0;
    CurCol=0;

    if (TotalLines<SMALL_FILE) Repaginate(w,FALSE,FALSE,0,TRUE);
    else                       PaintTer(w);

    return TRUE;
}

/*****************************************************************************
    TerDeleteFirstHdrFtr:
    Delete the first page header or footer for the section
******************************************************************************/
BOOL WINAPI _export TerDeleteFirstHdrFtr(HWND hWnd,BOOL HdrFtr,BOOL msg)
{

   return TerDeleteHdrFtr(hWnd,(BYTE)(HdrFtr?FHDR_CHAR:FFTR_CHAR),msg);
}

/*****************************************************************************
    TerDeleteHdrFtr:
    Delete the page header or footer for the section
******************************************************************************/
BOOL WINAPI _export TerDeleteHdrFtr(HWND hWnd,BYTE HdrFtr,BOOL msg)
{
    PTERWND w;
    int sect;
    long FirstLine,LastLine,count;
    LPBYTE pMsg;
    BOOL  CurLineInHdrFtr;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (!TerArg.WordWrap || !TerArg.PageMode) return FALSE;

    if (EditPageHdrFtr) ToggleEditHdrFtr(w);      // turnoff the editing of header footer
    if (EditPageHdrFtr) return FALSE;

    // check if first header/footer already exists
    RecreateSections(w);     // recreate sections
    sect=GetSection(w,CurLine);
    if (HdrFtr==HDR_CHAR) {
       FirstLine=TerSect1[sect].hdr.FirstLine;
       LastLine=TerSect1[sect].hdr.LastLine;
    }
    else if (HdrFtr==FHDR_CHAR) {
       FirstLine=TerSect1[sect].fhdr.FirstLine;
       LastLine=TerSect1[sect].fhdr.LastLine;
    }
    else if (HdrFtr==FTR_CHAR) {
       FirstLine=TerSect1[sect].ftr.FirstLine;
       LastLine=TerSect1[sect].ftr.LastLine;
    }
    else {
       FirstLine=TerSect1[sect].fftr.FirstLine;
       LastLine=TerSect1[sect].fftr.LastLine;
    }

    if (FirstLine<0) return FALSE;  // does not exist

    // user confirmation
    if (msg) {
       if      (HdrFtr==HDR_CHAR)  pMsg=(LPBYTE)MsgString[MSG_DEL_HDR];
       else if (HdrFtr==FHDR_CHAR) pMsg=(LPBYTE)MsgString[MSG_DEL_FIRST_HDR];
       else if (HdrFtr==FTR_CHAR)  pMsg=(LPBYTE)MsgString[MSG_DEL_FTR];
       else                        pMsg=(LPBYTE)MsgString[MSG_DEL_FIRST_FTR];
       if (IDNO==(MessageBox(hTerWnd,pMsg,MsgString[MSG_IRREVERSIBLE_DEL],MB_YESNO))) return FALSE;
    }

    ReleaseUndo(w);     // release any existing undos

    CurLineInHdrFtr=PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR;

    // delete the header footer lines
    count=LastLine-FirstLine+1;
    MoveLineArrays(w,FirstLine,count,'D');

    if (HdrFtr==FHDR_CHAR || HdrFtr==FFTR_CHAR) ResetUintFlag(TerSect[sect].flags,SECT_HAS_TITLE_PAGE);

    RecreateSections(w);

    // position the current line
    if (CurLineInHdrFtr) {   // position at the first body text
       for (;(CurLine+1)<TotalLines;CurLine++) if (!(PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR)) break;
    }
    else if (FirstLine<CurLine) CurLine-=count;

    if (CurLine<0) CurLine=0;
    CurCol=0;

    if (FirstLine<HilightBegRow) HilightBegRow-=count;
    if (FirstLine<HilightEndRow) HilightEndRow-=count;

    if (TotalLines<SMALL_FILE) Repaginate(w,FALSE,FALSE,0,TRUE);
    else                       PaintTer(w);

    return TRUE;
}

/*****************************************************************************
    CreatePageHdrFtr:
    Create page header or footer for the section
******************************************************************************/
CreatePageHdrFtr(PTERWND w,BYTE type,int sect)
{
   int  CurParaId,CurSect;
   long l,FirstLine;
   LPBYTE ptr;
   LPWORD fmt;
   UINT  flags;
   BOOL  DoHdr=(type==HDR_CHAR || type==FHDR_CHAR);

   if (!CheckLineLimit(w,TotalLines+3)) return FALSE;   // we will need to insert 3 lines
   
   CurSect=GetSection(w,CurLine);     // section for the current line

   FirstLine=TerSect[sect].FirstLine;

   // get the current para id
   CurParaId=0;
   flags=PfmtId[CurParaId].flags;
   if (DoHdr) flags=(flags&(~(UINT)PAGE_FTR))|PAGE_HDR;
   else       flags=(flags&(~(UINT)PAGE_HDR))|PAGE_FTR;

   CurParaId=NewParaId(w,CurParaId,
            PfmtId[CurParaId].LeftIndentTwips,
            PfmtId[CurParaId].RightIndentTwips,
            PfmtId[CurParaId].FirstIndentTwips,
            PfmtId[CurParaId].TabId,
            PfmtId[CurParaId].BltId,
            PfmtId[CurParaId].AuxId,
            PfmtId[CurParaId].Aux1Id,
            PfmtId[CurParaId].StyId,
            PfmtId[CurParaId].shading,
            PfmtId[CurParaId].pflags,
            PfmtId[CurParaId].SpaceBefore,
            PfmtId[CurParaId].SpaceAfter,
            PfmtId[CurParaId].SpaceBetween,
            PfmtId[CurParaId].LineSpacing,
            PfmtId[CurParaId].BkColor,
            PfmtId[CurParaId].BorderSpace,
            PfmtId[CurParaId].flow,
            flags);

   // insert the header lines
   MoveLineArrays(w,FirstLine,3,'B');   // insert 3 lines before the first line
   TerSect[sect].FirstLine=FirstLine;   // MoveLineArrays can change the FirstLine

   for (l=FirstLine;l<FirstLine+3;l++) {
     LineAlloc(w,l,0,1);

     ptr=pText(l);
     fmt=OpenCfmt(w,l);
     ptr[0]=type;
     if (l==(FirstLine+1)) ptr[0]=ParaChar;
     fmt[0]=DEFAULT_CFMT;

     CloseCfmt(w,l);

     pfmt(l)=CurParaId;

     if (l==FirstLine || l==(FirstLine+2)) SetHdrFtrLineFlags(w,l,type); // set the LineFlags for the delimiter line
   }

   // adjust CurLine and higlight lines
   if    (type==HDR_CHAR && sect==CurSect && !InRtfRead)
                                 CurLine=FirstLine+1;  // position at the paramarker
   else  if (CurLine>=FirstLine) CurLine=CurLine+3;    // shift downward

   if (FirstLine<HilightBegRow) HilightBegRow+=(long)3;
   if (FirstLine<HilightEndRow) HilightEndRow+=(long)3;

   TerArg.modified++;

   return TRUE;
}

/******************************************************************************
    SetCurLang:
    Set currnet language and character set.
******************************************************************************/
SetCurLang(PTERWND w, HKL hkl)
{
   CHARSETINFO cs;
   BYTE LocaleData[6],CharSet=0;
   DWORD CodePage;
   //LPBYTE endptr;
   
   CurHKL=hkl;
         
   // Find the Charset for the new kbd 
   GetLocaleInfo (LOWORD(hkl), LOCALE_IDEFAULTANSICODEPAGE, LocaleData, 6) ;
   CodePage = atol (LocaleData) ; 
        
   if (TranslateCharsetInfo ((LPVOID) CodePage, &cs,TCI_SRCCODEPAGE)) ReqCharSet=(BYTE)cs.ciCharset; 
   
   if (TRUE || ReqCharSet>0) {
      // Find the language for the keyboard
      //GetLocaleInfo (LOWORD(hkl), LOCALE_IDEFAULTLANGUAGE, LocaleData, 6) ;
      //ReqLang=strtoul(LocaleData,&endptr,16);     // convert hex string to long
      ReqLang=LOWORD(hkl);
   }
   else ReqLang=DefLang;
   
   return TRUE;
}

/******************************************************************************
    SetFontTextAngle:
    Set text angle for a font and return the the new font
******************************************************************************/
int SetFontTextAngle(PTERWND w, int CurFont, int TextAngle)
{
    int NewFont;

    if (TerFont[CurFont].style&PICT) {
       TerFont[CurFont].TextAngle=TextAngle;
       SetPictSize(w,CurFont,TwipsToScrY(TerFont[CurFont].PictHeight),TwipsToScrX(TerFont[CurFont].PictWidth),TRUE);
       XlateSizeForPrt(w,CurFont);              // convert size to printer resolution
       return CurFont;   // do not modify a picture font
    }

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TextAngle))>=0) {
        return NewFont;             // return the new font
    }
    else return CurFont;
}

/******************************************************************************
    LineTextAngle:
    Get the text angle for a line. Specify negative value for a frame number
******************************************************************************/
int LineTextAngle(PTERWND w, int LineNo)
{
   int ParaFID=0,CellId=0,TextAngle=0;

   if (LineNo<0) {
      int frm=(int)(-LineNo);
      return GetFrameTextAngle(w,frm);
   }
   else {
      if (LineNo>=TotalLines) return 0;
      else {
         ParaFID=fid(LineNo);
         CellId=cid(LineNo);
      }
      if (ParaFID!=0) TextAngle=ParaFrame[ParaFID].TextAngle;
      if (TextAngle==0 && CellId>0) TextAngle=cell[CellId].TextAngle;
   }  
   
   return TextAngle;
}
 
/******************************************************************************
    TerDeleteObject:
    This routine deletes the font or bitmap for a specified TerFont element.
******************************************************************************/
BOOL WINAPI _export TerDeleteObject(HWND hWnd,int idx)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (idx<0 || idx>=TotalFonts || !(TerFont[idx].InUse)) return FALSE;

    DeleteTerObject(w,idx);
 
    return TRUE;
}
 
/******************************************************************************
    DeleteTerObject:
    This routine deletes the font or bitmap for a specified TerFont element.
******************************************************************************/
DeleteTerObject(PTERWND w,int idx)
{

    if (TerFont[idx].InUse) {
       if (TerFont[idx].style&PICT) {
          if (TerFont[idx].ObjectType!=OBJ_NONE) TerOleUnload(w,idx);

          if (TerFont[idx].hMeta) DeleteMetaFile(TerFont[idx].hMeta); // delete metafile
          #if defined(WIN32)
          if (TerFont[idx].hEnhMeta) DeleteEnhMetaFile(TerFont[idx].hEnhMeta); // delete metafile
          #endif
          if (TerFont[idx].hBM)    DeleteObject(TerFont[idx].hBM);  // delete bitmap
          if (TerFont[idx].pImage) OurFree(TerFont[idx].pImage);    // delete DIB image
          if (TerFont[idx].pOrigImage) OurFree(TerFont[idx].pOrigImage);    // delete the original image information
          if (TerFont[idx].pInfo)  OurFree(TerFont[idx].pInfo);     // delete DIB info
          if (TerFont[idx].rgb)    OurFree(TerFont[idx].rgb);       // delete most used color table
          if (TerFont[idx].LinkFile) OurFree(TerFont[idx].LinkFile);// delete linked file name
          if (TerFont[idx].PictAlt) OurFree(TerFont[idx].PictAlt);  // delete alt picture name

          if (TerFont[idx].anim)   DeleteAnimInfo(w,idx);
          if (TerFont[idx].PictType==PICT_CONTROL || TerFont[idx].PictType==PICT_FORM) {
             TerFont[idx].InUse=FALSE;        // to bypass the subclass proceedure
             if (TerFont[idx].hWnd) DestroyWindow(TerFont[idx].hWnd);
             TerFont[idx].hWnd=NULL;
             ValidateRect(hTerWnd,NULL);     // discard any painting messages
          }
          if (TerFont[idx].hPal) {
             SelectPalette(hTerDC,GetStockObject(DEFAULT_PALETTE),TRUE);
             DeleteObject(TerFont[idx].hPal);
             TerFont[idx].hPal=NULL;
          }
       }
       else {
          if (TerFont[idx].hidden) OurFree(TerFont[idx].hidden);
          if (PrtFont[idx].hidden) OurFree(PrtFont[idx].hidden);
          if (TerFont[idx].hFont)  {
             if (TerFont[idx].GlbFontId>=0)         // global font used
                  GlbFont[TerFont[idx].GlbFontId].UseCount--;  // reduce the use font
             else DeleteObject(TerFont[idx].hFont); // private font used
          }
          if (PrtFont[idx].hFont)  {
             if (PrtFont[idx].GlbFontId>=0)         // global font used
                  GlbFont[PrtFont[idx].GlbFontId].UseCount--;  // reduce the use font
             else DeleteObject(PrtFont[idx].hFont); // private font used
          }
          if (TerFont[idx].FieldCode) OurFree(TerFont[idx].FieldCode);
       }
       InitTerObject(w,idx);
    }
    return TRUE;
}

/******************************************************************************
    InitTerObject:
    Initialize a Ter Object (font or picutre)
******************************************************************************/
InitTerObject(PTERWND w,int idx)
{
    LPINT CharWidth;

    // Allocate the character width table if not already allocated
    if (TerFont[idx].CharWidth==NULL) {
       TerFont[idx].CharWidth=OurAlloc(sizeof(int)*NUM_WIDTH_CHARS);
       PrtFont[idx].CharWidth=OurAlloc(sizeof(int)*NUM_WIDTH_CHARS);
    }

    CharWidth=TerFont[idx].CharWidth;          // save char width table
    FarMemSet(&(TerFont[idx]),0,sizeof(struct StrFont));
    TerFont[idx].CharWidth=CharWidth;          // restore the charwidth table

    TerFont[idx].TextBkColor=CLR_WHITE;  //  default background color
    TerFont[idx].TextColor=CLR_AUTO;     //  auto text color
    TerFont[idx].UlineColor=CLR_AUTO;    //  underline color
    TerFont[idx].FontFamily=FF_DONTCARE;
    TerFont[idx].CharSet=DEFAULT_CHARSET;
    TerFont[idx].ObjectType=OBJ_NONE;
    TerFont[idx].PictAlign=ALIGN_BOT;
    TerFont[idx].GlbFontId=-1;                 // index into the global font structure
    TerFont[idx].CharStyId=1;                  // index into the stylesheet table

    CharWidth=PrtFont[idx].CharWidth;          // save char width table
    FarMemSet(&(PrtFont[idx]),0,sizeof(struct StrPrtFont));
    PrtFont[idx].CharWidth=CharWidth;          // restore the charwidth table
    PrtFont[idx].GlbFontId=-1;                 // index into the global font structure

    return TRUE;
}
