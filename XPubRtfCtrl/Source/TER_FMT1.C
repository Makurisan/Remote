/*==============================================================================
   TER_FMT1.C
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
    SetTerDefaultFont:
    This routine sets the default font and point size for the editor.
    This routine can be called in the beginning to change the default font.
*****************************************************************************/
BOOL WINAPI _export SetTerDefaultFont(HWND hWnd, LPBYTE TypeFace,int PointSize, UINT style, COLORREF TextColor, BOOL repaint)
{
    PTERWND w;
    int TwipsSize;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    ReleaseUndo(w);

    if (PointSize>=0) TwipsSize=PointSize*20;
    else              TwipsSize=-PointSize;        // specified in twips

    // Deselect and delete the default font
    hTerCurFont=hTerRegFont=GetStockObject(SYSTEM_FONT);
    SelectObject(hTerDC,hTerCurFont);
    DeleteTerObject(w,DEFAULT_CFMT);

    // create the new default font
    InitTerObject(w,DEFAULT_CFMT);
    TerFont[0].InUse=TRUE;                            // mark as in-use
    lstrcpy(TerFont[0].TypeFace,TypeFace);            // font type face
    TerFont[0].TwipsSize=TwipsSize;                   // point size
    TerFont[0].style=style;                           // character style

    if (!CreateOneFont(w,hTerDC,DEFAULT_CFMT,TRUE)) {
       return PrintError(w,MSG_ERR_FONT_CREATE,"SetTerDefaultFont");
    }

    hTerRegFont=hTerCurFont=TerFont[DEFAULT_CFMT].hFont;

    TerFont[DEFAULT_CFMT].TextColor=TextColor;     // text color

    lstrcpy(TerArg.FontTypeFace,TypeFace);    // save to the default variables
    TerArg.PointSize=TwipsSize/20;

    // set the normal para style
    lstrcpy(StyleId[0].TypeFace,TypeFace);
    StyleId[0].TwipsSize=TwipsSize;
    StyleId[0].style=style;

    // Select the default font
    SelectObject(hTerDC,hTerRegFont);
    GetTextMetrics(hTerDC,&TerTextMet);       // get new text metrics
    GetWinDimension(w);                       // recalulate our window dimension

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    ToolBarCfmt=-1;                           // reset the toolbar

    if (TerArg.ShowStatus && !InRtfRead) DisplayStatus(w);  // paint the status line
    if (repaint) PaintTer(w);

    return TRUE;
}

/*****************************************************************************
    SetTerDefTabType:
    Set the default tab type when the user clicks on the ruler
*****************************************************************************/
BOOL WINAPI _export TerSetDefTabType(HWND hWnd, int DefTab)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (DefTab!=TAB_LEFT && DefTab!=TAB_RIGHT && DefTab!=TAB_DECIMAL && DefTab!=TAB_CENTER) return false;
    DefTabType=DefTab;

    return TRUE;
}

/******************************************************************************
    TerCreateFont:
    Create a new font id.
******************************************************************************/
int WINAPI _export TerCreateFont(HWND hWnd,int ReuseId, BOOL shared, LPBYTE NewTypeFace,int NewPointSize,UINT NewStyle,COLORREF NewTextColor, COLORREF NewTextBkColor, int NewFieldId, int NewAuxId)
{
    return TerCreateFont2(hWnd,ReuseId,shared,NewTypeFace,NewPointSize,NewStyle,NewTextColor,NewTextBkColor,NewFieldId,NewAuxId,1,0,0);
}

/******************************************************************************
    TerCreateFont2:
    Create a new font id.
******************************************************************************/
int WINAPI _export TerCreateFont2(HWND hWnd,int ReuseId, BOOL shared, LPBYTE NewTypeFace,int NewPointSize,UINT NewStyle,COLORREF NewTextColor, COLORREF NewTextBkColor, int NewFieldId, int NewAuxId, int NewCharStyId, int NewParaStyId, int NewExpand)
{
    return TerCreateFont3(hWnd,ReuseId,shared,NewTypeFace,NewPointSize,NewStyle,NewTextColor,NewTextBkColor,NewFieldId,NewAuxId,NewCharStyId,NewParaStyId,NewExpand,DEFAULT_CHARSET);
}
 
/******************************************************************************
    TerCreateFont3:
    This routine examines the font table to check if a font exists that
    matches the new specification.  If it does,  the index of that font
    is returned.  Otherwise,  the routine will create a new font in an
    open slot in the font table.

    if ReuseId >=0 then then the new information is assined to ReuseId.

    If successful, the routine returns the index of the font in the table.
    Otherwise it returns (-1).
******************************************************************************/
int WINAPI _export TerCreateFont3(HWND hWnd,int ReuseId, BOOL shared, LPBYTE NewTypeFace,int NewPointSize,UINT NewStyle,COLORREF NewTextColor, COLORREF NewTextBkColor, int NewFieldId, int NewAuxId, int NewCharStyId, int NewParaStyId, int NewExpand, UINT NewCharSet)
{
    PTERWND w;
    int i,NewId,OldId=0,NewTwipsSize;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    if (ReuseId>=MAX_FONTS)  return -1;

    if (NewPointSize>=0) NewTwipsSize=NewPointSize*20;
    else                 NewTwipsSize=-NewPointSize;       // already specified in twips.

    // use a specific id
    if (ReuseId>=0) shared=FALSE;    // mutually exclusive

    // initialize intermedite ids
    if (ReuseId>=MaxFonts) ExpandFontTable(w,ReuseId+1);
    if (ReuseId>=TotalFonts) {    // initialize intermediate ids
       for (i=TotalFonts;i<=ReuseId;i++) InitTerObject(w,i);
       TotalFonts=ReuseId+1;
    }

    // create a font id
    if (!shared) MatchIds=FALSE;     // do not match with an exiting id
    if (ReuseId>=0) OldId=ReuseId;

    // argument checking
    if (NewCharStyId<0 || NewCharStyId>=TotalSID || !StyleId[NewCharStyId].InUse || StyleId[NewCharStyId].type!=SSTYPE_CHAR) NewCharStyId=1;  // default value
    if (NewParaStyId<0 || NewParaStyId>=TotalSID || !StyleId[NewParaStyId].InUse || StyleId[NewParaStyId].type!=SSTYPE_PARA) NewParaStyId=0;  // default value

    NewId=GetNewFont(w,hTerDC,OldId,NewTypeFace,NewTwipsSize,NewStyle,NewTextColor,NewTextBkColor,CLR_AUTO,NewFieldId,NewAuxId,NextFontAux1Id,NewCharStyId,NewParaStyId,NewExpand,0,0,NULL,0,0,(BYTE)NewCharSet,0,0);
    NextFontAux1Id=0;               // reset global transfer variables
    if (NewId>=0) TerFont[NewId].flags|=FFLAG_USER;    // protect from reclaiming process

    // copy the new font id to the old id if requested
    if (ReuseId>=0 && NewId!=ReuseId) {
       LPINT pSaveScrCharWidth,pSavePrtCharWidth;
       // Deselect the current font
       hTerCurFont=hTerRegFont=GetStockObject(SYSTEM_FONT);
       SelectObject(hTerDC,hTerCurFont);
       if (TerFont[ReuseId].InUse) DeleteTerObject(w,ReuseId);    // delete the old id and reuse the slot

       pSaveScrCharWidth=TerFont[ReuseId].CharWidth;  // save the char width pointers
       pSavePrtCharWidth=PrtFont[ReuseId].CharWidth;  // save the char width pointers

       FarMove(&(TerFont[NewId]),&(TerFont[ReuseId]),sizeof(TerFont[NewId]));
       FarMove(&(PrtFont[NewId]),&(PrtFont[ReuseId]),sizeof(PrtFont[NewId]));

       TerFont[NewId].CharWidth=pSaveScrCharWidth;    // move the pointers to the empty location
       PrtFont[NewId].CharWidth=pSavePrtCharWidth;

       InitTerObject(w,NewId);

       NewId=ReuseId;
    }

    // Select the default font
    if (NewId==DEFAULT_CFMT) {
       hTerRegFont=hTerCurFont=TerFont[DEFAULT_CFMT].hFont;
       SelectObject(hTerDC,hTerRegFont);

       GetTextMetrics(hTerDC,&TerTextMet);       // get new text metrics
       GetWinDimension(w);                       // recalulate our window dimension
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    if (TerArg.ShowStatus && NewId==DEFAULT_CFMT) DisplayStatus(w);  // paint the status line

    return NewId;
}

/*****************************************************************************
   RemoveDeletedObjects:
   Reset the InUse flag for the deleted objects
*****************************************************************************/
RemoveDeletedObjects(PTERWND w)
{
   int i,len;
   long l;
   int font;
   LPWORD fmt;

   if (!TerArg.modified) return TRUE;

   for (i=0;i<TotalFonts;i++) if (TerFont[i].style&PICT) break;
   if (i==TotalFonts) return TRUE;     // no objects found


   for (i=0;i<TotalFonts;i++) if (TerFont[i].style&PICT) ResetUintFlag(TerFont[i].flags,FFLAG_SEL);
   if (BkPictId>0) TerFont[BkPictId].flags|=FFLAG_SEL;

   // scan the document now
   for (l=0;l<TotalLines;l++) {
      if (cfmt(l).info.type==UNIFORM) {
        font=cfmt(l).info.fmt;
        if (TerFont[font].style&PICT) TerFont[font].flags|=FFLAG_SEL;
      }
      else {
        fmt=cfmt(l).pFmt;
        len=LineLen(l);
        for (i=0;i<len;i++) {
           font=fmt[i];
           if (TerFont[font].style&PICT) TerFont[font].flags|=FFLAG_SEL;
        }
      }

   }

      // set the use flag for all animated objects
   for (i=0;i<TotalFonts;i++) {
      if (TerFont[i].flags&FFLAG_SEL && IsFirstAnimPict(w,i)) {
         int NextPict=TerFont[i].anim->NextPict;
         while (NextPict>0 && NextPict<TotalFonts && TerFont[NextPict].InUse && (TerFont[NextPict].style&PICT) && TerFont[NextPict].anim) {
            TerFont[NextPict].flags|=FFLAG_SEL;
            NextPict=TerFont[NextPict].anim->NextPict;
         }
      }
   }

   // delete the unselected objects
   for (i=0;i<TotalFonts;i++) {
      if (TerFont[i].InUse && (TerFont[i].style&PICT) && !(TerFont[i].flags&FFLAG_SEL))
        DeleteTerObject(w,i);
   }

   return TRUE;
}

/*****************************************************************************
    TransferFontId:
    Transfer in or out the font id to a buffer.
*****************************************************************************/
TransferFontId(PTERWND w, BOOL in, int FontId, struct StrFont far *font, struct StrPrtFont far *pfont)
{
    if (in)  {  // move from buffer to the slot
       if (TerFont[FontId].CharWidth) OurFree(TerFont[FontId].CharWidth);  // free any existing memory
       if (PrtFont[FontId].CharWidth) OurFree(PrtFont[FontId].CharWidth);
       
       if (TerFont[FontId].hidden) OurFree(TerFont[FontId].hidden);  // free any existing memory 
       if (PrtFont[FontId].hidden) OurFree(PrtFont[FontId].hidden); 

       FarMove(font,&(TerFont[FontId]),sizeof(struct StrFont));
       FarMove(pfont,&(PrtFont[FontId]),sizeof(struct StrPrtFont));
    }
    else {      // transfer to buffers
       FarMove(&(TerFont[FontId]),font,sizeof(struct StrFont));
       FarMove(&(PrtFont[FontId]),pfont,sizeof(struct StrPrtFont));
       TerFont[FontId].CharWidth=NULL;   // pointer transferred
       PrtFont[FontId].CharWidth=NULL;
       
       TerFont[FontId].hidden=NULL;
       PrtFont[FontId].hidden=NULL;

       TerFont[FontId].LinkFile=NULL;
       TerFont[FontId].InUse=FALSE;
    }
    return TRUE;
}

/******************************************************************************
    TerEditStyle:
    This function starts (when 'start' equal to TRUE) or ends a stylesheet
    style editing. The name of the style is specified by the 'name' parameter.
    The 'new' should be set to TRUE if this is a new style.  For a new style,
    also specify the style type using the 'type' variable by setting it to
    SSTYPE_CHAR or SSTYPE_PARA constants.

    When the 'name' parameter is NULL, this function will show a dialog box
    for the user to select the parameters.

    If successful, this function returns the style id, otherwise it returns
    -1.
******************************************************************************/
int WINAPI _export TerEditStyle(HWND hWnd,BOOL start, LPBYTE name, BOOL new, int type, BOOL repaint)
{
    PTERWND w;
    BYTE StyleName[MAX_SS_NAME_WIDTH+1];
    BYTE HeadingStyle[MAX_SS_NAME_WIDTH+1]="heading ";
    int  i,len,CurPara,font;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data


    // check if the name provided
    if (name==NULL || name[0]==0) {
       if (CurSID<0) start=TRUE;
       else          start=FALSE;
    }

    if (start && CurSID>=0) return -1;  // editing session already in progress
    if (!start && CurSID<0) return -1;  // no session to end

    // end the current editing session
    if (!start) {
       int SaveCurSID=CurSID;

       if (StyleId[CurSID].type==SSTYPE_PARA) ApplyParaStyles(w,CurSID);
       else                                   ApplyCharStyles(w,CurSID);

       CurSID=-1;
       EditingParaStyle=FALSE;   // turnoff para style editing flag

       TerArg.modified++;
       if (repaint)  PaintTer(w);
       else          WordWrap(w,0,TotalLines);

       return SaveCurSID;
    }

    // Get the style name if not already provided
    if (name==NULL || name[0]==0) {
       if (!CallDialogBox(w,"EditStyleParam",EditStyleParam,0L)) return -1;
       // copy the information from the dialog box
       lstrcpy(StyleName,TempString);
       name=StyleName;
       new=DlgInt1;      // new style
       type=DlgInt2;     // style type
    }

    // check if new style has a duplicate name
    if (new) {
       for (i=0;i<TotalSID;i++) {
          if (StyleId[i].InUse && lstrcmpi(StyleId[i].name,name)==0) {
             new=FALSE;
             break;
          }
       }
    }

    // create a new style
    if (new) {
       CurSID=GetStyleIdSlot(w);
       if (CurSID<0) return -1;

       // initialize the style structure
       FarMemSet(&(StyleId[CurSID]),0,sizeof(struct StrStyleId));

       // validate the information
       len=lstrlen(name);
       if (len>MAX_SS_NAME_WIDTH) name[MAX_SS_NAME_WIDTH]=0;
       if (type!=SSTYPE_CHAR && type!=SSTYPE_PARA) type=SSTYPE_PARA;

       lstrcpy(StyleId[CurSID].name,name);    // copy the name
       StyleId[CurSID].InUse=TRUE;            // set the TRUE flag
       StyleId[CurSID].type=type;             // type SSTYPE_CHAR or SSTYPE_PARA
       StyleId[CurSID].OutlineLevel=-1;       // not a heading style by default

       if (type==SSTYPE_PARA && strlen(name)>strlen(HeadingStyle)) {
          BYTE TempName[MAX_SS_NAME_WIDTH+1];
          int  len=strlen(HeadingStyle),OutlineLevel=-1;
          strcpy(TempName,name);
          TempName[len]=0;
          if (strcmpi(TempName,HeadingStyle)==0) OutlineLevel=name[len]-'1';  // outline level
          if (OutlineLevel<0 || OutlineLevel>8) OutlineLevel=-1;    // not an outline level
          StyleId[CurSID].OutlineLevel=OutlineLevel;
       }

       // set the character properties
       font=GetEffectiveCfmt(w);

       if (lstrcmpi(TerFont[font].TypeFace,TerArg.FontTypeFace)!=0 || type==SSTYPE_PARA) {
          lstrcpy(StyleId[CurSID].TypeFace,TerFont[font].TypeFace);
          StyleId[CurSID].FontFamily=TerFont[font].FontFamily;
       }
       else  StyleId[CurSID].FontFamily=TerFont[0].FontFamily;
       if ((TerFont[font].TwipsSize/20)!=TerArg.PointSize || type==SSTYPE_PARA) StyleId[CurSID].TwipsSize=TerFont[font].TwipsSize;
       StyleId[CurSID].style=ResetUintFlag(TerFont[font].style,(PICT|CHAR_BOX|FNOTEALL));
       StyleId[CurSID].TextColor=TerFont[font].TextColor;
       StyleId[CurSID].TextBkColor=TerFont[font].TextBkColor;
       StyleId[CurSID].UlineColor=TerFont[font].UlineColor;
       StyleId[CurSID].expand=TerFont[font].expand;

       // set the paragraph properties
       if (type==SSTYPE_PARA) {
          CurPara=pfmt(CurLine);
          StyleId[CurSID].LeftIndentTwips=PfmtId[CurPara].LeftIndentTwips;
          StyleId[CurSID].RightIndentTwips=PfmtId[CurPara].RightIndentTwips;
          StyleId[CurSID].FirstIndentTwips=PfmtId[CurPara].FirstIndentTwips;
          StyleId[CurSID].ParaFlags=ResetUintFlag(PfmtId[CurPara].flags,(PAGE_HDR_FTR));
          StyleId[CurSID].pflags=PfmtId[CurPara].pflags&PFLAG_STYLES;
          StyleId[CurSID].shading=PfmtId[CurPara].shading;
          StyleId[CurSID].SpaceBefore=PfmtId[CurPara].SpaceBefore;
          StyleId[CurSID].SpaceAfter=PfmtId[CurPara].SpaceAfter;
          StyleId[CurSID].SpaceBetween=PfmtId[CurPara].SpaceBetween;
          StyleId[CurSID].LineSpacing=PfmtId[CurPara].LineSpacing;
          StyleId[CurSID].ParaBkColor=PfmtId[CurPara].BkColor;
          StyleId[CurSID].ParaBorderColor=PfmtId[CurPara].BorderColor;
          StyleId[CurSID].TabId=PfmtId[CurPara].TabId;
       }

       StyleId[CurSID].flags|=SSFLAG_ADD;   // additive character style
    }
    else {                // find the id to edit
       for (i=0;i<TotalSID;i++) {
          if (StyleId[i].InUse && lstrcmpi(StyleId[i].name,name)==0) {
             CurSID=i;
             break;
          }
       }
       if (CurSID==-1) return CurSID;
    }

    // save the original copy of the style
    FarMove(&(StyleId[CurSID]),&PrevStyleId,sizeof(struct StrStyleId));

    if (StyleId[CurSID].type==SSTYPE_PARA) EditingParaStyle=TRUE;  // a paragraph style is being edited
    HilightType=HILIGHT_OFF;      // turn-off anyhilighting

    PaintTer(w);

    return CurSID;
}

/******************************************************************************
    TerCancelEditStyle:
    Cancel the editing of the current
******************************************************************************/
BOOL WINAPI _export TerCancelEditStyle(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return false;  // get the pointer to window data

    if (CurSID==-1) return false;
    
    // restore the original copy of the style
    FarMove(&PrevStyleId,&(StyleId[CurSID]),sizeof(struct StrStyleId));
    
    CurSID=-1;    // turn-off style editing
    EditingParaStyle=FALSE;   // turnoff para style editing flag
    PaintTer(w);

    return true;
}

/******************************************************************************
    TerGetStyleParam:
    get style parameters
******************************************************************************/
int WINAPI _export TerGetStyleParam(HWND hWnd,int id, int type)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (id<0 || id>=TotalSID || !(StyleId[id].InUse)) return -1;

    if (type==SSINFO_CHAR_SPACE) {
       return StyleId[id].expand;
    }
    else if (type==SSINFO_CHAR_OFFSET) {
       return StyleId[id].offset;
    }
    else if (type==SSINFO_NEXT) {
       return StyleId[id].next;
    }
    else return -1; 
}
 

/******************************************************************************
    TerSetStyleParam:
    set style parameters
******************************************************************************/
BOOL WINAPI _export TerSetStyleParam(HWND hWnd,int id, int type, int IntParam, LPBYTE TextParam, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (id<0 || id>=TotalSID || !(StyleId[id].InUse)) return FALSE;

    if (type==SSINFO_NAME) {
       lstrcpy(StyleId[id].name,TextParam);
    }
    else if (type==SSINFO_NEXT) {
       int NextSty=IntParam;
       if (NextSty<0 || NextSty>=TotalSID || !(StyleId[NextSty].InUse)) return FALSE;

       StyleId[id].next=NextSty;
    }
    else return FALSE; 

    TerArg.modified++;
    if (repaint) UpdateToolBar(w,TRUE);

    return TRUE;
}
 
/******************************************************************************
    TerGetStyleId:
    Get styleid for a style name.  The function returns -1 if unsuccessful.
******************************************************************************/
int WINAPI _export TerGetStyleId(HWND hWnd, LPBYTE name)
{
    PTERWND w;
    int i;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (name==NULL || lstrlen(name)==0) return -1;
    for (i=0;i<TotalSID;i++) if (lstrcmp(name,StyleId[i].name)==0) break;

    if (i>=TotalSID) return -1;
    else             return i;
}

/******************************************************************************
    TerGetStyleInfo:
    Get style item information
******************************************************************************/
int WINAPI _export TerGetStyleInfo(HWND hWnd, int id, LPBYTE name,LPINT pType)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (id<0 || id>=TotalSID || !(StyleId[id].InUse)) return -1;

    if (name) lstrcpy(name,StyleId[id].name);
    if (pType) (*pType)=StyleId[id].type;

    return TotalSID;
}


/******************************************************************************
    TerDeleteStyle:
    Delete a style id.  The style name can be specified by setting the style id to
    -1.  Style ids 0 and 1 can not be deleted.
******************************************************************************/
BOOL WINAPI _export TerDeleteStyle(HWND hWnd, int CurStyle, LPBYTE name)
{
    PTERWND w;
    int i;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (CurStyle<0) {
       if (name==NULL || lstrlen(name)==0) return FALSE;
       for (i=2;i<TotalSID;i++) if (lstrcmp(name,StyleId[i].name)==0) break;
       if (i>=TotalSID) return FALSE;
       CurStyle=i;
    }
    if (CurStyle<2) return FALSE;    // default styles can not be deleted

    // remove the style reference from the paragraph table
    if (StyleId[CurStyle].type==SSTYPE_PARA) {
       for (i=0;i<TotalPfmts;i++) {
          if (PfmtId[i].StyId==CurStyle) PfmtId[i].StyId=0;    // assign the default id
       }
    }

    // remove the style reference from the font table
    for (i=0;i<TotalFonts;i++) {
        if (TerFont[i].ParaStyId==CurStyle) TerFont[i].ParaStyId=0;    // assign the default paragraph style id
        if (TerFont[i].CharStyId==CurStyle) TerFont[i].CharStyId=1;    // assign the default character style id
    }


    StyleId[CurStyle].InUse=FALSE;

    TerArg.modified++;

    return TRUE;
}

/******************************************************************************
    TerSelectCharStyle:
    This function assigns a new character stylesheet id to the selected text.
    The 'CurStyle' must be a valid character style id or -1.  When CurStyle
    is -1, this function displays a user dialog box to select a character style.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSelectCharStyle(HWND hWnd, int CurStyle, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (CurStyle<0) {
       if ((CurStyle=CallDialogBox(w,"SelectStyleParam",SelectStyleParam,SSTYPE_CHAR))<0) return -1;
    }

    if (CurStyle>=TotalSID || StyleId[CurStyle].type!=SSTYPE_CHAR) return FALSE;  // not a valid style

    return CharFmt(w,GetNewCharStyle,(DWORD)CurStyle,0L,repaint);
}

/******************************************************************************
    TerSelectParaStyle:
    This function assigns a new paragraph stylesheet id to the selected text.
    The 'CurStyle' must be a valid paragraph style id or -1.  When CurStyle
    is -1, this function displays a user dialog box to select a character style.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSelectParaStyle(HWND hWnd, int CurStyle, BOOL repaint)
{
    PTERWND w;
    long FirstLine,LastLine,l;
    struct StrPfmt para;
    int   PrevStyId;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    if (CurStyle<0) {
       if ((CurStyle=CallDialogBox(w,"SelectStyleParam",SelectStyleParam,SSTYPE_PARA))<0) return -1;
    }

    if (CurStyle>=TotalSID || StyleId[CurStyle].type!=SSTYPE_PARA) return FALSE;  // not a valid style


    // get the range of lines to modify
    GetParaRange(w,&FirstLine,&LastLine);     // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {     // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       PrevStyId=PfmtId[pfmt(l)].StyId;

       FarMove(&(PfmtId[pfmt(l)]),&para,sizeof(para));   // copy the previous information

       SetParaStyleId(w,&para,&(StyleId[para.StyId]),&(StyleId[CurStyle]),TRUE);

       pfmt(l)=NewParaId(w,pfmt(l),
                         para.LeftIndentTwips,
                         para.RightIndentTwips,
                         para.FirstIndentTwips,
                         para.TabId,
                         para.BltId,
                         para.AuxId,
                         para.Aux1Id,
                         CurStyle,
                         para.shading,
                         para.pflags,
                         para.SpaceBefore,
                         para.SpaceAfter,
                         para.SpaceBetween,
                         para.LineSpacing,
                         para.BkColor,
                         para.BorderSpace,
                         para.flow,
                         para.flags);

       ApplyLineTextStyle(w,l,CurStyle,PrevStyId);
    }

    TerArg.modified++;
    RepageBeginLine=FirstLine;
    InputFontId=-1;          // so that the input fonts are recalculated

    if (repaint)  PaintTer(w);
    else          WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    ApplyLineTextStyle:
    Update line text with a new paragraph style.
******************************************************************************/
ApplyLineTextStyle(PTERWND w,long LineNo, int CurParaStyId, int PrevParaStyId)
{
    int i,len,CurFont=0,PrevFont=0,NewFont;
    LPWORD fmt;
    struct StrFont font;

    len=LineLen(LineNo);
    if (len==0) return TRUE;

    fmt=OpenCfmt(w,LineNo);

    for (i=0;i<len;i++) {
       PrevFont=CurFont;
       CurFont=fmt[i];

       if (TerFont[CurFont].style&PICT) continue;

       if (CurFont==PrevFont && i>0) {
          fmt[i]=NewFont;           // translation already done
          continue;
       }

       FarMove(&(TerFont[CurFont]),&font,sizeof(font));

       // reverse any previous style attributes
       if (lstrcmpi(font.TypeFace,StyleId[PrevParaStyId].TypeFace)==0) lstrcpy(font.TypeFace,TerArg.FontTypeFace);
       if (font.TwipsSize==StyleId[PrevParaStyId].TwipsSize) font.TwipsSize=TerArg.PointSize*20;
       ResetUintFlag(font.style,StyleId[PrevParaStyId].style);
       if (font.TextColor==StyleId[PrevParaStyId].TextColor) font.TextColor=0;
       if (font.TextBkColor==StyleId[PrevParaStyId].TextBkColor) font.TextBkColor=CLR_WHITE;
       if (font.UlineColor==StyleId[PrevParaStyId].UlineColor) font.UlineColor=0;
       if (font.expand==StyleId[PrevParaStyId].expand) font.expand=0;
       if (font.offset==StyleId[PrevParaStyId].offset) font.offset=0;

       // apply the new style
       if (lstrlen(StyleId[CurParaStyId].TypeFace)>0) lstrcpy(font.TypeFace,StyleId[CurParaStyId].TypeFace);
       if (StyleId[CurParaStyId].TwipsSize!=(TerArg.PointSize*20) &&
            StyleId[CurParaStyId].TwipsSize>0) font.TwipsSize=StyleId[CurParaStyId].TwipsSize;
       font.style|=StyleId[CurParaStyId].style;
       if (StyleId[CurParaStyId].TextColor!=CLR_AUTO) font.TextColor=StyleId[CurParaStyId].TextColor;
       if (StyleId[CurParaStyId].TextBkColor!=CLR_WHITE) font.TextBkColor=StyleId[CurParaStyId].TextBkColor;
       if (StyleId[CurParaStyId].UlineColor!=CLR_AUTO) font.UlineColor=StyleId[CurParaStyId].UlineColor;
       if (StyleId[CurParaStyId].expand) font.expand=StyleId[CurParaStyId].expand;
       if (StyleId[CurParaStyId].offset) font.offset=StyleId[CurParaStyId].offset;

       NewFont=GetNewFont(w,hTerDC,CurFont,font.TypeFace,font.TwipsSize,font.style,
                 font.TextColor,font.TextBkColor,font.UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,CurParaStyId,
                 font.expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 font.offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle);
       if (NewFont<0) NewFont=CurFont;

       if (TerFont[NewFont].CharStyId!=1) NewFont=ForceCharStyle(w,NewFont);
       fmt[i]=NewFont;
    }

    return TRUE;
}

/******************************************************************************
    ChangeLineTextStyle:
    Change line text with a new paragraph style.
******************************************************************************/
ChangeLineTextStyle(PTERWND w,long LineNo, int CurParaStyId)
{
    int i,len,CurFont=0,PrevFont=0,NewFont;
    LPWORD fmt;

    len=LineLen(LineNo);
    if (len==0) return TRUE;

    fmt=OpenCfmt(w,LineNo);

    for (i=0;i<len;i++) {
       PrevFont=CurFont;
       CurFont=fmt[i];

       if (TerFont[CurFont].style&PICT) continue;

       if (CurFont==PrevFont && i>0) {
          fmt[i]=NewFont;           // translation already done
          continue;
       }

       NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,
                 TerFont[CurFont].style,
                 TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 TerFont[CurFont].CharStyId,CurParaStyId,
                 TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle);
       if (NewFont<0) NewFont=CurFont;

       fmt[i]=NewFont;
    }

    return TRUE;
}


/******************************************************************************
    GetNewCharStyle:
    This routine accept the exiting font index and returns a new font
    index that supplies the given character style id.
******************************************************************************/
WORD GetNewCharStyle(PTERWND w,WORD OldFmt,DWORD data1, DWORD data2, long line, int col)
{
    int CurFont,NewFont;
    int  NewStyId,PrevStyId,CurParaStyId;
    struct StrFont font;

    CurFont=OldFmt;                       // current font number

    NewStyId=(int)data1;                  // new style id to apply
    PrevStyId=TerFont[CurFont].CharStyId;
    CurParaStyId=PfmtId[pfmt(line)].StyId;

    FarMove(&(TerFont[CurFont]),&font,sizeof(font));

    if (TerFont[CurFont].style&PICT) return OldFmt;        // do not change the picutre format

    // reverse any para style attributes
    if (lstrcmpi(font.TypeFace,StyleId[CurParaStyId].TypeFace)==0) lstrcpy(font.TypeFace,TerArg.FontTypeFace);
    if (font.TwipsSize==StyleId[CurParaStyId].TwipsSize) font.TwipsSize=(TerArg.PointSize*20);
    font.style=ResetUintFlag(font.style,StyleId[CurParaStyId].style);
    if (font.TextColor==StyleId[CurParaStyId].TextColor) font.TextColor=0;
    if (font.TextBkColor==StyleId[CurParaStyId].TextBkColor) font.TextBkColor=CLR_WHITE;
    if (font.UlineColor==StyleId[CurParaStyId].UlineColor) font.UlineColor=0;
    if (font.expand==StyleId[CurParaStyId].expand) font.expand=0;
    if (font.offset==StyleId[CurParaStyId].offset) font.offset=0;

    // apply the character style attributes
    SetCharStyleId(w,&font,&(StyleId[PrevStyId]),&(StyleId[NewStyId]),TRUE);

    // apply the paragraph style attributes
    SetCharStyleId(w,&font,&(StyleId[font.ParaStyId]),&(StyleId[CurParaStyId]),FALSE);

    // create the new font id
    if ((NewFont=GetNewFont(w,hTerDC,CurFont,font.TypeFace,font.TwipsSize,font.style,
                 font.TextColor,font.TextBkColor,font.UlineColor,
                 TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                 NewStyId,TerFont[CurFont].ParaStyId,
                 font.expand,TerFont[CurFont].TempStyle,
                 TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                 font.offset,TerFont[CurFont].UcBase,
                 TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                 TerFont[CurFont].TextAngle))>=0) {
        return (WORD)NewFont; // return the new font
    }
    else return OldFmt;
}

/******************************************************************************
    ForceCharStyle:
    Force the character style attribute
******************************************************************************/
int ForceCharStyle(PTERWND w,int OldFmt)
{
    int PrevStyle,CurStyle,NewFont;
    struct StrFont font;

    FarMove(&(TerFont[OldFmt]),&font,sizeof(font));

    PrevStyle=1;  // default character style
    CurStyle=TerFont[OldFmt].CharStyId;

    SetCharStyleId(w,&font,&StyleId[PrevStyle],&StyleId[CurStyle],TRUE);

    if ((NewFont=GetNewFont(w,hTerDC,OldFmt,font.TypeFace,font.TwipsSize,font.style,
                 font.TextColor,font.TextBkColor,font.UlineColor,
                 font.FieldId,font.AuxId,font.Aux1Id,
                 CurStyle,font.ParaStyId,
                 font.expand,font.TempStyle,
                 font.lang,font.FieldCode,
                 font.offset,font.UcBase,
                 font.CharSet,font.flags,
                 font.TextAngle))>=0) {
        return (WORD)NewFont; // return the new font
    }
    else return OldFmt;
} 

/******************************************************************************
    SetCharStyleId:
    This function removes the previous style from the given font and applies
    the new style.
******************************************************************************/
SetCharStyleId(PTERWND w,struct StrFont far *font, struct StrStyleId far *PrevStyle, struct StrStyleId far *NewStyle, BOOL force)
{
    // apply the new style id
    if ( force || (lstrcmpi(PrevStyle->TypeFace,font->TypeFace)==0)
      || (lstrlen(PrevStyle->TypeFace)==0 && lstrcmpi(TerArg.FontTypeFace,font->TypeFace)==0)
      || (lstrcmpi(font->TypeFace,TerArg.FontTypeFace)==0) ) {
         if (lstrlen(NewStyle->TypeFace)>0)
               lstrcpy(font->TypeFace,NewStyle->TypeFace);
         //else  lstrcpy(font->TypeFace,TerArg.FontTypeFace);
    }

    if (  force || (PrevStyle->TwipsSize==font->TwipsSize)
       || (PrevStyle->TwipsSize==0 && (TerArg.PointSize*20)==font->TwipsSize)
       || (font->TwipsSize==(TerArg.PointSize*20)) ) {
        if (NewStyle->TwipsSize>0)
             font->TwipsSize=NewStyle->TwipsSize;
        //else font->TwipsSize=TerArg.PointSize*20;
    }

    if (force) {
       font->style=ResetUintFlag(font->style,PrevStyle->style);
       font->style=ResetUintFlag(font->style,(BOLD|ULINE|ULINED|ITALIC|STRIKE|SUPSCR|SUBSCR|SCAPS|CAPS));  // styles to turnoff
       font->style|=NewStyle->style;
    }
    else {                    // change only those bits which match with the previous style
       UINT StyBits=(BOLD|ULINE|ULINED|ITALIC|STRIKE|SUPSCR|SUBSCR|SCAPS|CAPS);
       UINT OtherBits=~StyBits;                    // non-relevant bits
       UINT CurSty=font->style&StyBits;
       UINT PrevSty=PrevStyle->style&StyBits;
       UINT NewSty=NewStyle->style&StyBits;
       UINT SameBits=(~(CurSty^PrevSty))&StyBits;  // matching bits set to 1
       CurSty&=(~SameBits);                        // turnoff the match bits in the current style
       NewSty&=SameBits;                           // select matching bits in the new style
       CurSty|=NewSty;                             // merge the matching bits
       CurSty|=(font->style&OtherBits);            // merge non-relevant bits
       font->style=CurSty;                         // new style
    }    

    if (force) {
       if (TRUE || NewStyle->TextColor!=0)           font->TextColor=NewStyle->TextColor;
       if (TRUE || NewStyle->TextBkColor!=CLR_WHITE) font->TextBkColor=NewStyle->TextBkColor;
       if (TRUE || NewStyle->UlineColor!=0)           font->UlineColor=NewStyle->UlineColor;
       font->expand=NewStyle->expand;
       font->offset=NewStyle->offset;
    }
    else {
       if ((PrevStyle->TextColor==font->TextColor)
          || (PrevStyle->TextColor==0 && font->TextColor==0)
          || (font->TextColor==0 || font->TextColor==CLR_AUTO) ) {
           font->TextColor=NewStyle->TextColor;
       }
       if ((PrevStyle->TextBkColor==font->TextBkColor)
          || (PrevStyle->TextBkColor==CLR_WHITE && font->TextBkColor==CLR_WHITE)
          || (font->TextBkColor==CLR_WHITE) ) {
           font->TextBkColor=NewStyle->TextBkColor;
       }
       if ((PrevStyle->UlineColor==font->UlineColor)
          || (PrevStyle->UlineColor==0 && font->UlineColor==0)
          || (font->UlineColor==0 || font->UlineColor==CLR_AUTO) ) {
           font->UlineColor=NewStyle->UlineColor;
       }
       if (PrevStyle->expand==font->expand || font->expand==0) {
           font->expand=NewStyle->expand;
       }
       if (PrevStyle->offset==font->offset || font->offset==0) {
           font->offset=NewStyle->offset;
       }
    }

    return TRUE;
}

/******************************************************************************
    SetParaStyleId:
    This function removes the previous style from the given PfmtId and applies
    the new style.
******************************************************************************/
SetParaStyleId(PTERWND w,struct StrPfmt far *para, struct StrStyleId far *PrevStyle, struct StrStyleId far *NewStyle, BOOL force)
{
    UINT SameBits;

    if ( force || (PrevStyle->LeftIndentTwips==para->LeftIndentTwips)
       || (PrevStyle->LeftIndentTwips==0 && PfmtId[0].LeftIndentTwips==para->LeftIndentTwips) )
       para->LeftIndentTwips=NewStyle->LeftIndentTwips;
       para->LeftIndent=TwipsToScrX(para->LeftIndentTwips);

    if ( force || (PrevStyle->RightIndentTwips==para->RightIndentTwips)
       || (PrevStyle->RightIndentTwips==0 && PfmtId[0].RightIndentTwips==para->RightIndentTwips) )
       para->RightIndentTwips=NewStyle->RightIndentTwips;
       para->RightIndent=TwipsToScrX(para->RightIndentTwips);

    if ( force || (PrevStyle->FirstIndentTwips==para->FirstIndentTwips)
       || (PrevStyle->FirstIndentTwips==0 && PfmtId[0].FirstIndentTwips==para->FirstIndentTwips) )
       para->FirstIndentTwips=NewStyle->FirstIndentTwips;
       para->FirstIndent=TwipsToScrX(para->FirstIndentTwips);

    if (force) {
       para->flags=ResetUintFlag(para->flags,(LEFT|CENTER|RIGHT_JUSTIFY|JUSTIFY|DOUBLE_SPACE|BULLET|PARA_BOX_ATTRIB|PARA_KEEP|PARA_KEEP_NEXT));
       para->flags|=NewStyle->ParaFlags;
       para->pflags=ResetUintFlag(para->pflags,(PFLAG_NO_WRAP|PFLAG_WIDOW));
       para->pflags|=NewStyle->pflags;
    }
    else {
       UINT AlignMask=(LEFT|CENTER|RIGHT_JUSTIFY|JUSTIFY);
       UINT flag1=para->flags,flag2=PrevStyle->ParaFlags,OtherMask=~AlignMask;
       if (!(flag1&(CENTER|RIGHT_JUSTIFY|JUSTIFY))) flag1|=LEFT;      // supply default
       if (!(flag2&(CENTER|RIGHT_JUSTIFY|JUSTIFY))) flag2|=LEFT;      // supply default
       if ((flag1&AlignMask)==(flag2&AlignMask)) {
          ResetUintFlag(para->flags,AlignMask);
          para->flags|=(NewStyle->ParaFlags&AlignMask);
       }
       // handle double spacing flag
       if ((flag1&DOUBLE_SPACE)==(flag2&DOUBLE_SPACE)) {
          ResetUintFlag(para->flags,DOUBLE_SPACE);
          para->flags|=(NewStyle->ParaFlags&DOUBLE_SPACE);
       }
       ResetUintFlag(OtherMask,DOUBLE_SPACE); 
       
       // handle the bullet flag
       if ((flag1&BULLET)==(flag2&BULLET)) {
          ResetUintFlag(para->flags,BULLET);
          para->flags|=(NewStyle->ParaFlags&BULLET);
       }
       ResetUintFlag(OtherMask,BULLET); 

       ResetUintFlag(OtherMask,PAGE_HDR_FTR);  
       ResetUintFlag(para->flags,OtherMask);  // reset existing non-align bits
       para->flags|=(NewStyle->ParaFlags&OtherMask);
       
       SameBits=~(para->pflags^PrevStyle->pflags);
       para->pflags=(NewStyle->pflags&SameBits)|(para->pflags&(~SameBits));
    }

    if ( force || (PrevStyle->shading==para->shading)
       || (PrevStyle->shading==0 && PfmtId[0].shading==para->shading) )
       para->shading=NewStyle->shading;

    if ( force || (PrevStyle->ParaBkColor==para->BkColor)
       || (PrevStyle->ParaBkColor==CLR_WHITE && PfmtId[0].BkColor==para->BkColor) )
       para->BkColor=NewStyle->ParaBkColor;

    if ( force || (PrevStyle->ParaBorderColor==para->BorderColor)
       || (PrevStyle->ParaBorderColor==CLR_AUTO && PfmtId[0].BorderColor==para->BorderColor) )
       para->BorderColor=NewStyle->ParaBorderColor;

    if ( force || (PrevStyle->SpaceBefore==para->SpaceBefore)
       || (PrevStyle->SpaceBefore==0 && PfmtId[0].SpaceBefore==para->SpaceBefore) )
       para->SpaceBefore=NewStyle->SpaceBefore;

    if ( force || (PrevStyle->SpaceAfter==para->SpaceAfter)
       || (PrevStyle->SpaceAfter==0 && PfmtId[0].SpaceAfter==para->SpaceAfter) )
       para->SpaceAfter=NewStyle->SpaceAfter;

    if ( force || (PrevStyle->SpaceBetween==para->SpaceBetween)
       || (PrevStyle->SpaceBetween==0 && PfmtId[0].SpaceBetween==para->SpaceBetween) )
       para->SpaceBetween=NewStyle->SpaceBetween;

    if ( force || (PrevStyle->LineSpacing==para->LineSpacing)
       || (PrevStyle->LineSpacing==0 && PfmtId[0].LineSpacing==para->LineSpacing) )
       para->LineSpacing=NewStyle->LineSpacing;

    if ( force || (PrevStyle->TabId==para->TabId)
       || (PrevStyle->TabId==0 && PfmtId[0].TabId==para->TabId) )
       para->TabId=NewStyle->TabId;

    if ( force || (PrevStyle->BltId==para->BltId)
       || (PrevStyle->BltId==0 && PfmtId[0].BltId==para->BltId) )
       para->BltId=NewStyle->BltId;

    return TRUE;
}

/******************************************************************************
    ApplyCharStyles:
    This function is used to apply the given character style id to the document.
******************************************************************************/
ApplyCharStyles(PTERWND w, int CurStyle)
{
    int i,CurParaStyId;
    struct StrFont far *font;

    // edit each font id
    for (i=0;i<TotalFonts;i++) {
       if (!(TerFont[i].InUse) || TerFont[i].style&PICT || TerFont[i].CharStyId!=CurStyle) continue;

       // reverse any para style attributes
       font=&(TerFont[i]);
       CurParaStyId=font->ParaStyId;

       if (lstrcmpi(font->TypeFace,StyleId[CurParaStyId].TypeFace)==0) lstrcpy(font->TypeFace,TerArg.FontTypeFace);
       if (font->TwipsSize==StyleId[CurParaStyId].TwipsSize) font->TwipsSize=TerArg.PointSize*20;
       font->style=ResetUintFlag(font->style,StyleId[CurParaStyId].style);
       if (font->TextColor==StyleId[CurParaStyId].TextColor) font->TextColor=0;
       if (font->TextBkColor==StyleId[CurParaStyId].TextBkColor) font->TextBkColor=CLR_WHITE;
       if (font->UlineColor==StyleId[CurParaStyId].UlineColor) font->UlineColor=0;
       if (font->expand==StyleId[CurParaStyId].expand) font->expand=0;
       if (font->offset==StyleId[CurParaStyId].offset) font->offset=0;

       // apply the char style elements
       SetCharStyleId(w,font,&PrevStyleId,&(StyleId[CurStyle]),TRUE);

       // apply the para style elements
       SetCharStyleId(w,font,&(StyleId[CurParaStyId]),&(StyleId[CurParaStyId]),FALSE);

       CreateOneFont(w,hTerDC,i,TRUE);      // recreate this font
    }

    return TRUE;
}

/******************************************************************************
    SetNextStyle:
    Set next style when the user hits Enter at the end of a line which uses
    a paragraph style.
******************************************************************************/
SetNextStyle(PTERWND w)
{
    int sid,ParaId;

    if (LineLen(CurLine)!=1) return true;  // Enter not hit at the end of the line

    ParaId=pfmt(CurLine);
    sid=PfmtId[ParaId].StyId;

    if (StyleId[sid].next==sid) return true;  // next style is the same

    TerSelectParaStyle(hTerWnd,StyleId[sid].next,true);

    return true;
} 

/******************************************************************************
    ApplyParaStyleOnFont:
    This function applies the given paragraph style on the given font id.
    It returns a new font id which has the attribute of the ParaStyId character
    attributes.
******************************************************************************/
WORD ApplyParaStyleOnFont(PTERWND w, int CurFont, int ParaStyId)
{
    int NewFont;
    struct StrFont font;

    if (TerFont[CurFont].style&PICT) {
       TerFont[CurFont].ParaStyId=ParaStyId;
       return (WORD)CurFont;
    }
    
    if (TerFont[CurFont].style&HIDDEN) return (WORD)CurFont;  // do not modify this font because it can redisplay the hidden text

    // get the updated font structure
    FarMove(&(TerFont[CurFont]),&font,sizeof(font));  // make a copy of the font
    
    if (InRtfRead || font.CharStyId!=1) font.ParaStyId=ParaStyId;          // simply use the current para style id
    else SetCharStyleId(w,&font, &(StyleId[font.ParaStyId]), &(StyleId[ParaStyId]),TRUE);

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,font.TypeFace,font.TwipsSize,font.style,
                 font.TextColor,font.TextBkColor,font.UlineColor,
                 font.FieldId,font.AuxId,font.Aux1Id,
                 font.CharStyId,ParaStyId,font.expand,font.TempStyle,
                 font.lang,font.FieldCode,
                 font.offset,font.UcBase,
                 font.CharSet,font.flags,
                 font.TextAngle))>=0) {
        return (WORD)NewFont;             // return the new font
    }
    else return (WORD)CurFont;
}

/******************************************************************************
    ApplyParaStyles:
    This function is used to update all para ids which has the given paragraph
    style id.
******************************************************************************/
ApplyParaStyles(PTERWND w, int CurStyle)
{
    int i;
    struct StrStyleId far *CurCharStyle;
    BOOL force;

    // supply typeface and pointsize if not selected
    if (lstrlen(StyleId[CurStyle].TypeFace)==0) lstrcpy(StyleId[CurStyle].TypeFace,PrevStyleId.TypeFace);
    if (StyleId[CurStyle].TwipsSize==0) StyleId[CurStyle].TwipsSize=PrevStyleId.TwipsSize;

    // edit each font id
    for (i=1;i<TotalFonts;i++) {
       if (!(TerFont[i].InUse) || TerFont[i].style&PICT || TerFont[i].ParaStyId!=CurStyle) continue;

       // apply the para style elements
       SetCharStyleId(w,&(TerFont[i]),&PrevStyleId,&(StyleId[CurStyle]),FALSE);

       // reset those attributes which are common between previous para style and current character style
       CurCharStyle=&(StyleId[TerFont[i].CharStyId]);
       if (lstrcmpi(CurCharStyle->TypeFace,PrevStyleId.TypeFace)==0) lstrcpy(TerFont[i].TypeFace,CurCharStyle->TypeFace);
       if (CurCharStyle->TwipsSize==PrevStyleId.TwipsSize) TerFont[i].TwipsSize=CurCharStyle->TwipsSize;
       if (CurCharStyle->expand==PrevStyleId.expand) TerFont[i].expand=CurCharStyle->expand;
       if (CurCharStyle->offset==PrevStyleId.offset) TerFont[i].offset=CurCharStyle->offset;
       if (CurCharStyle->TextColor==PrevStyleId.TextColor && PrevStyleId.TextColor!=0) TerFont[i].TextColor=CurCharStyle->TextColor;
       if (CurCharStyle->TextBkColor==PrevStyleId.TextBkColor && PrevStyleId.TextBkColor!=CLR_WHITE) TerFont[i].TextBkColor=CurCharStyle->TextBkColor;
       if (CurCharStyle->UlineColor==PrevStyleId.UlineColor && PrevStyleId.UlineColor!=0) TerFont[i].UlineColor=CurCharStyle->UlineColor;
       //TerFont[i].style|=(CurCharStyle->style&PrevStyleId.style);

       CreateOneFont(w,hTerDC,i,TRUE);      // recreate this font
    }

    // modify the default font if CurStyle is 0
    if (CurStyle==0) {
       if (lstrlen(StyleId[CurStyle].TypeFace)>0) {
          lstrcpy(TerFont[0].TypeFace,StyleId[CurStyle].TypeFace);
          lstrcpy(TerArg.FontTypeFace,StyleId[CurStyle].TypeFace);
       }
       if (StyleId[CurStyle].TwipsSize>0) {
          TerFont[0].TwipsSize=StyleId[CurStyle].TwipsSize;
          TerArg.PointSize=StyleId[CurStyle].TwipsSize/20;
       }
       TerFont[0].style=StyleId[CurStyle].style;
       TerFont[0].TextColor=StyleId[CurStyle].TextColor;
       //TerFont[0].TextBkColor=TextDefBkColor=StyleId[CurStyle].TextBkColor;
       TerFont[0].TextBkColor=StyleId[CurStyle].TextBkColor;
       TerFont[0].UlineColor=StyleId[CurStyle].UlineColor;
       TerFont[0].expand=StyleId[CurStyle].expand;
       TerFont[0].offset=StyleId[CurStyle].offset;

       CreateOneFont(w,hTerDC,0,TRUE);      // recreate this font
    }

    // edit each para id
    force=FALSE;
    for (i=1;i<TotalPfmts;i++) {
       if (PfmtId[i].StyId!=CurStyle) continue;

       // set the properties of the new style
       SetParaStyleId(w,&(PfmtId[i]),&PrevStyleId,&(StyleId[CurStyle]),force);
    }

    // modify the default para id
    if (CurStyle==0) {
       PfmtId[0].LeftIndentTwips=StyleId[CurStyle].LeftIndentTwips;
       PfmtId[0].RightIndentTwips=StyleId[CurStyle].RightIndentTwips;
       PfmtId[0].FirstIndentTwips=StyleId[CurStyle].FirstIndentTwips;

       //****** calculate the indentations in device units *****
       PfmtId[0].LeftIndent=TwipsToScrX(PfmtId[0].LeftIndentTwips);
       PfmtId[0].RightIndent=TwipsToScrX(PfmtId[0].RightIndentTwips);
       PfmtId[0].FirstIndent=TwipsToScrX(PfmtId[0].FirstIndentTwips);

       PfmtId[0].flags=StyleId[CurStyle].ParaFlags;
       PfmtId[0].pflags=StyleId[CurStyle].pflags;
       
       PfmtId[0].shading=StyleId[CurStyle].shading;
       PfmtId[0].SpaceBefore=StyleId[CurStyle].SpaceBefore;
       PfmtId[0].SpaceAfter=StyleId[CurStyle].SpaceAfter;
       PfmtId[0].SpaceBetween=StyleId[CurStyle].SpaceBetween;
       PfmtId[0].LineSpacing=StyleId[CurStyle].LineSpacing;
       PfmtId[0].BkColor=StyleId[CurStyle].ParaBkColor;
       PfmtId[0].BorderColor=StyleId[CurStyle].ParaBorderColor;
       PfmtId[0].TabId=StyleId[CurStyle].TabId;
    }

    return TRUE;
}

/******************************************************************************
    CopyCharStyle:
    Copy the character style from one id to another.
******************************************************************************/
CopyCharStyle(PTERWND w, int src, int dest)
{
    lstrcpy(StyleId[dest].TypeFace,StyleId[src].TypeFace);
    StyleId[dest].TwipsSize=StyleId[src].TwipsSize;
    StyleId[dest].style=StyleId[src].style;
    StyleId[dest].TextColor=StyleId[src].TextColor;
    StyleId[dest].TextBkColor=StyleId[src].TextBkColor;
    StyleId[dest].UlineColor=StyleId[src].UlineColor;
    StyleId[dest].expand=StyleId[src].expand;
    StyleId[dest].offset=StyleId[src].offset;

    return TRUE;
}

/******************************************************************************
    DoRulerClick:
    This routine is called when user clicks the ruler to set a tab
*******************************************************************************/
DoRulerClick(PTERWND w,UINT wParam,DWORD lParam)
{
    int i,x,RulerPos,type,TabId,FrameNo;

    if (!TerArg.WordWrap) return TRUE;   // tab stops not available in non-wordwrap mode
    if (CurSID>=0 && !EditingParaStyle) {
       MessageBeep(0);
       return TRUE;  // editing character style
    }

    // calculate ruler position in twips
    x=(int)(short)LOWORD(lParam);
    RulerPos=x-TerWinRect.left+TerWinOrgX;  // ruler position in screen units
    FrameNo=GetFrame(w,CurLine);           // subtract column displacement
    if (LineFlags2(CurLine)&LFLAG2_RTL) 
         RulerPos=RtlX(w,RulerPos,0,CurFrame,NULL);
    else RulerPos-=(frame[FrameNo].x+frame[FrameNo].SpaceLeft);
    if (RulerPos<0) RulerPos=0;

    RulerPos=ScrToTwipsX(RulerPos);        // convert to twips unit
    if (SnapToGrid) {
       if (TerFlags&TFLAG_METRIC) RulerPos=RoundInt(RulerPos,SNAP_DIST_METRIC);  // apply snap
       else                       RulerPos=RoundInt(RulerPos,SNAP_DIST);  // apply snap
    }

    // set new tab position
    if (GetKeyState(VK_SHIFT)&0x8000) {    // shift pressed
       if (wParam==WM_LBUTTONDOWN) type=TAB_CENTER;
       else                        type=TAB_DECIMAL;
    }
    else {
       if (wParam==WM_LBUTTONDOWN) type=DefTabType;
       else                        type=TAB_RIGHT;
    }

    // check if a tab position exist at this location
    TabId=PfmtId[pfmt(CurLine)].TabId;

    for (i=0;i<TerTab[TabId].count;i++) {
       if (abs(TerTab[TabId].pos[i]-RulerPos)<(SNAP_DIST/2)) break;  // tolerate 50 twips difference
    }

    if (i<TerTab[TabId].count || RulerPos<=0)
          ClearTab(hTerWnd,TerTab[TabId].pos[i],TRUE); // clear this tab position
    else  SetTab(hTerWnd,type,RulerPos,TRUE);          // set new tab, refresh screen

    return TRUE;
}

/******************************************************************************
    TerSetZoom:
    Set the zoom percentage for display. If the percent is set to -1, this
    function will display a dialog box to the user to prompt for the zoom
    percent.
    This function returns the old zoom percentage if successful.  Otherwise
    it returns -1.
******************************************************************************/
int WINAPI _export TerSetZoom(HWND hWnd, int percent)
{
    PTERWND w;
    int i,OldZoomPercent,NewZoomPercent;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    
    if (percent==-2) return ZoomPercent;

    OldZoomPercent=ZoomPercent;

    if (percent<0) {
       if ((NewZoomPercent=CallDialogBox(w,"ZoomParam",ZoomParam,0L))<0) return -1;
       percent=NewZoomPercent;
    }

    // apply the zoom limits
    if (ZoomPercent==percent) return ZoomPercent;   // no change
    if (percent<10) percent=10;
    if (percent>500) percent=500;

    ApplyZoomPercent(w,percent);

    // recreate the fonts
    RecreateFonts(w,hTerDC);

    // resize any embedded control
    for (i=0;i<TotalFonts;i++) {
       if (IsControl(w,i)) {
          int width=TerFont[i].CharWidth[PICT_CHAR];
          int height=TerFont[i].height;
          
          // SetWindowPos sets the height of the window plus the drop down
          struct StrControl far *pInfo=(LPVOID)TerFont[i].pInfo;
          if (pInfo!=NULL && (lstrcmpi(pInfo->class,"ComboBox")==0)) {
              height=((4*(TwipsToScrY(TerFont[i].PictHeight)-5))/5)*8;
              SendMessage(TerFont[i].hWnd,CB_SETITEMHEIGHT,(WPARAM)-1,TwipsToScrY(TerFont[i].PictHeight)-5);
          }

          SetWindowPos(TerFont[i].hWnd,NULL,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER);
       }
    }

    if (TerArg.PageMode) TerRepaginate(hTerWnd,TRUE);

    TerRepaint(hTerWnd,TRUE);

    return OldZoomPercent;
}

/******************************************************************************
    ApplyZoomPercent:
    Apply the zoom percent.
******************************************************************************/
ApplyZoomPercent(PTERWND w, int NewZoom)
{
    int i;

    // calculate the new screen resolution
    ScrResX=MulDiv(OrigScrResX,NewZoom,100);
    ScrResY=MulDiv(OrigScrResY,NewZoom,100);

    // update the para ids
    for (i=0;i<TotalPfmts;i++) {
       PfmtId[i].LeftIndent=TwipsToScrX(PfmtId[i].LeftIndentTwips);
       PfmtId[i].RightIndent=TwipsToScrX(PfmtId[i].RightIndentTwips);
       PfmtId[i].FirstIndent=TwipsToScrX(PfmtId[i].FirstIndentTwips);
    }

    ZoomPercent=NewZoom;

    return TRUE;
}

/******************************************************************************
    GetStyleIdSlot:
    get the para frame slot from the para frame table.  This function returns -1 if
    a table slot not found.
******************************************************************************/
GetStyleIdSlot(PTERWND w)
{
    int i;
    DWORD OldSize, NewSize;
    int   NewMaxSID;
    HGLOBAL hMem;
    LPBYTE pMem;

    // find an open slot
    for (i=2;i<TotalSID;i++) {
       if (!StyleId[i].InUse) {
          FarMemSet(&StyleId[i],0,sizeof(struct StrStyleId));
          return i;
       }
    }

    // enlarge the table if needed
    if (TotalSID>=MaxSID) {        // allocate more memory for the StyleId table
       NewMaxSID=MaxSID+MaxSID/2;

       if (!Win32 && NewMaxSID>MAX_SS_IDS) NewMaxSID=MAX_SS_IDS;

       OldSize=((DWORD)MaxSID+1)*sizeof(struct StrStyleId);
       NewSize=((DWORD)NewMaxSID+1)*sizeof(struct StrStyleId);

       if ( (NULL!=(hMem=GlobalAlloc(GMEM_MOVEABLE,NewSize)))
          && (NULL!=(pMem=GlobalLock(hMem))) ){

          // move data to new location
          HugeMove(StyleId,pMem,OldSize);

          // free old handle
          GlobalUnlock(hStyleId);
          GlobalFree(hStyleId);

          // assign new handle
          hStyleId=hMem;              // new array
          StyleId=(struct StrStyleId far *)pMem;

          MaxSID=NewMaxSID;    // new max StyleIds
       }
       else {
          PrintError(w,MSG_OUT_OF_STYLESHEET,NULL);
          return -1;
       }
    }

    // use the next slot
    if (TotalSID<MaxSID) {
       TotalSID++;
       FarMemSet(&StyleId[TotalSID-1],0,sizeof(struct StrStyleId));
       return (TotalSID-1);
    }

    PrintError(w,MSG_OUT_OF_STYLESHEET,NULL);

    return -1;
}

/******************************************************************************
    CharFmt:
    Apply the give character format (data) to the current character or
    the highlighted block of text.  This routine extracts the exiting
    font index and calls the service routine (FormatFunction) to get
    the new font index.
******************************************************************************/
BOOL CharFmt(PTERWND w,WORD (*GetNewFontId)(PTERWND,WORD,DWORD,DWORD,long,int), DWORD data1,DWORD data2, BOOL repaint)
{
    WORD OldFmt;

    TerArg.modified++;

    if (mbcs) InitCharWidthCache(w);   // initialize the character width cache

    if (HilightType==HILIGHT_LINE)      {CharFmtLine(w,GetNewFontId,data1,data2,repaint);return TRUE;}
    if (HilightType==HILIGHT_CHAR)      {CharFmtChr(w,GetNewFontId,data1,data2,repaint);return TRUE;}

    OldFmt=(WORD)GetEffectiveCfmt(w);

    InputFontId=GetNewFontId(w,OldFmt,data1,data2,CurLine,CurCol);  // set the new font for input
    //if (repaint) UpdateToolBar(w);

    return TRUE;
}

/******************************************************************************
    CharFmtLine:
    Apply the requested font data to all character in a highlighted line
    block.
******************************************************************************/
CharFmtLine(PTERWND w,WORD (*GetNewFontId)(PTERWND,WORD,DWORD,DWORD,long,int),DWORD data1, DWORD data2, BOOL repaint)
{
    int j;
    long l;
    LPWORD fmt;

    if (!NormalizeBlock(w)) return TRUE;
    if (TerFlags2&TFLAG2_PROTECT_FORMAT && BlockHasProtectOn(w,TRUE,FALSE)) return TRUE;

    SaveUndo(w,HilightBegRow,0,HilightEndRow,LineLen(HilightEndRow)-1,'F');  // save undo

    for (l=HilightBegRow;l<=HilightEndRow;l++) {   // process each line in the block
        if (cfmt(l).info.type==UNIFORM) {
           cfmt(l).info.fmt=GetNewFontId(w,cfmt(l).info.fmt,data1,data2,l,0);
        }
        else {
           fmt=OpenCfmt(w,l);
           for (j=0;j<LineLen(l);j++) {
               fmt[j]=GetNewFontId(w,fmt[j],data1,data2,l,j);
           }
           CloseCfmt(w,l);
        }
    }

    if (TerOpFlags&TOFLAG_HILIGHT_OFF) HilightType=HILIGHT_OFF;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    CharFmtChr:
    Toggle on or off the request format on all characters in a character block
******************************************************************************/
CharFmtChr(PTERWND w,WORD (*GetNewFontId)(PTERWND,WORD,DWORD,DWORD,long,int),DWORD data1,DWORD data2,BOOL repaint)
{
    int  j;
    long l,BegLine,EndLine;
    LPWORD fmt;

    if (!NormalizeBlock(w)) return TRUE;
    if (TerFlags2&TFLAG2_PROTECT_FORMAT && BlockHasProtectOn(w,TRUE,FALSE)) return TRUE;

    SaveUndo(w,HilightBegRow,HilightBegCol,HilightEndRow,HilightEndCol-1,'F');  // save undo

    if (HilightBegRow==HilightEndRow) {  // only a part of one line highlighted
       fmt=OpenCfmt(w,HilightBegRow);
       for (j=HilightBegCol;j<HilightEndCol;j++) {
           if (j>=LineLen(HilightBegRow)) break;
           fmt[j]=GetNewFontId(w,fmt[j],data1,data2,HilightBegRow,j);
       }
       CloseCfmt(w,HilightBegRow);
       goto END;
    }

    //******* process the first line ************
    fmt=OpenCfmt(w,HilightBegRow);
    for (j=HilightBegCol;j<LineLen(HilightBegRow);j++) {
        fmt[j]=GetNewFontId(w,fmt[j],data1,data2,HilightBegRow,j);
    }
    CloseCfmt(w,HilightBegRow);

    //******* process the last line ************
    fmt=OpenCfmt(w,HilightEndRow);
    for (j=0;j<HilightEndCol;j++) {
        if (j>=LineLen(HilightEndRow)) break;
        fmt[j]=GetNewFontId(w,fmt[j],data1,data2,HilightEndRow,j);
    }
    CloseCfmt(w,HilightEndRow);

    //******* process the middle lines ***********
    BegLine=HilightBegRow+1;
    EndLine=HilightEndRow-1;
    for (l=BegLine;l<=EndLine;l++) {   // process each line in the block
        if (!LineSelected(w,l)) continue;

        if (cfmt(l).info.type==UNIFORM) {
           cfmt(l).info.fmt=GetNewFontId(w,cfmt(l).info.fmt,data1,data2,l,j);
        }
        else {
           fmt=OpenCfmt(w,l);
           for (j=0;j<LineLen(l);j++) {
               fmt[j]=GetNewFontId(w,fmt[j],data1,data2,l,j);
           }
           CloseCfmt(w,l);
        }
    }

    END:

    if (TerOpFlags&TOFLAG_HILIGHT_OFF) HilightType=HILIGHT_OFF;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    XlateSizeForPrt:
    Convert the font size for the given font to the printer resolution.
******************************************************************************/
XlateSizeForPrt(PTERWND w, int FontIdx)
{
    int i,PictResX=ScrResX,PictResY=ScrResY;

    if (!TerArg.PrintView) return TRUE;    // this information will not be used

    if (InPrinting && UseScrMetrics) PictResX=PictResY=UNITS_PER_INCH;  // in this mode, the picture dimensions are set in the twips units (instead of the screen units)
    
    PrtFont[FontIdx].height=MulDiv(TerFont[FontIdx].height,PrtResY,PictResY);
    PrtFont[FontIdx].BaseHeight=MulDiv(TerFont[FontIdx].BaseHeight,PrtResY,PictResY);
    for (i=0;i<256;i++) PrtFont[FontIdx].CharWidth[i]=(int)((TerFont[FontIdx].CharWidth[i]*(long)PrtResX)/PictResX);
    PrtFont[FontIdx].ExtLead=MulDiv(TerFont[FontIdx].ExtLead,PrtResY,PictResY);

    return TRUE;
}

