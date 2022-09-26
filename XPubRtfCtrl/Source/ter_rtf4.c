/*==============================================================================
   TER_RTF3.C
   RTF write for TER files/buffers

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1992)
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
    WriteRtfWatermark:
    Write the watermark.
******************************************************************************/
BOOL WriteRtfWatermark(PTERWND w,struct StrRtfOut far *rtf, BOOL WriteHeader)
{
    int pict;

    rtf->WatermarkWritten=true;  // set this flag in the beginning
    
    pict=ParaFrame[WmParaFID].pict;    
    
    if (WriteHeader) {            // write the header group 
       if (!BeginRtfGroup(w,rtf)) return false;
       if (!WriteRtfControl(w,rtf,"header",PARAM_NONE,0)) return false; // begin the header group
    } 

    WriteRtfShape(w,rtf,pict);

    if (WriteHeader) {            // write the header group 
       if (!EndRtfGroup(w,rtf)) return false;
    }

    return true;
}
 
/*****************************************************************************
    WriteRtfParaStyle:
    Write the character type stylesheet item
******************************************************************************/
BOOL WriteRtfParaStyle(PTERWND w,struct StrRtfOut far *rtf, int id)
{
    UINT CurFlags=StyleId[id].ParaFlags;
    struct StrStyleId far *style=&(StyleId[id]);
    int i;

    // check if tab id needs to be written
    if (style->TabId>0 && !WriteRtfTab(w,rtf,style->TabId)) return FALSE;

    // check if para border specification need to be written
    if (CurFlags&(PARA_BOX|PARA_BOX_THICK|PARA_BOX_DOUBLE) ) {
       if (!WriteRtfParaBorder(w,rtf,CurFlags,style->ParaBorderColor,0)) return FALSE;
    }

    // compare and write indents
    if (style->FirstIndentTwips!=0 && !WriteRtfControl(w,rtf,"fi",PARAM_INT,style->FirstIndentTwips)) return FALSE;
    if (style->LeftIndentTwips>0 && !WriteRtfControl(w,rtf,"li",PARAM_INT,style->LeftIndentTwips)) return FALSE;
    if (style->RightIndentTwips>0 && !WriteRtfControl(w,rtf,"ri",PARAM_INT,style->RightIndentTwips)) return FALSE;
    if (style->SpaceBefore>0 && !WriteRtfControl(w,rtf,"sb",PARAM_INT,style->SpaceBefore)) return FALSE;
    if (style->SpaceAfter>0 && !WriteRtfControl(w,rtf,"sa",PARAM_INT,style->SpaceAfter)) return FALSE;
    
    if (style->LineSpacing!=0) {
       if (!WriteRtfControl(w,rtf,"sl",PARAM_INT,MulDiv(style->LineSpacing,240,100)+240)) return FALSE;
       if (!WriteRtfControl(w,rtf,"slmult",PARAM_INT,1)) return FALSE;
    }
    else if (style->SpaceBetween!=0 && !WriteRtfControl(w,rtf,"sl",PARAM_INT,style->SpaceBetween)) return FALSE;
    
    if (style->shading>0 && !WriteRtfControl(w,rtf,"shading",PARAM_INT,style->shading)) return FALSE;
    if (style->OutlineLevel>0 && !WriteRtfControl(w,rtf,"outlinelevel",PARAM_INT,style->OutlineLevel)) return FALSE;

    // write flags bullets
    if (CurFlags&BULLET && !WriteRtfBullet(w,rtf,style->BltId)) return FALSE;
    if (CurFlags&RIGHT_JUSTIFY && !WriteRtfControl(w,rtf,"qr",PARAM_NONE,0)) return FALSE;
    if (CurFlags&CENTER && !WriteRtfControl(w,rtf,"qc",PARAM_NONE,0)) return FALSE;
    if (CurFlags&JUSTIFY && !WriteRtfControl(w,rtf,"qj",PARAM_NONE,0)) return FALSE;
    if (CurFlags&PARA_KEEP && !WriteRtfControl(w,rtf,"keep",PARAM_NONE,0)) return FALSE;
    if (CurFlags&PARA_KEEP_NEXT && !WriteRtfControl(w,rtf,"keepn",PARAM_NONE,0)) return FALSE;

    // compare and write double spacing
    if (CurFlags&DOUBLE_SPACE)   {
        if (!WriteRtfControl(w,rtf,"sl",PARAM_INT,480)) return FALSE;
        if (!WriteRtfControl(w,rtf,"slmult",PARAM_INT,1)) return FALSE;
    }

    if (style->ParaBkColor!=CLR_WHITE && style->ParaBkColor!=CLR_AUTO) {
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==style->ParaBkColor) break;
       if (i==rtf->TotalColors) i=0;
       if (!WriteRtfControl(w,rtf,"cbpat",PARAM_INT,i)) return FALSE;        // write the color index
    }

    if (style->shading!=0 && !WriteRtfControl(w,rtf,"shading",PARAM_INT,style->shading)) return FALSE;

    return TRUE;
}

/*****************************************************************************
    WriteRtfParaBorder:
    Write paragraph border controls
******************************************************************************/
BOOL WriteRtfParaBorder(PTERWND w,struct StrRtfOut far *rtf, UINT CurFlags, COLORREF BorderColor, int CurParaFID)
{
    if ((CurFlags&PARA_BOX)==PARA_BOX) {
       if (!WriteRtfControl(w,rtf,"box",PARAM_NONE,0)) return FALSE;
       if (CurFlags&PARA_BOX_THICK && !WriteRtfControl(w,rtf,"brdrw",PARAM_INT,30)) return FALSE;
       if (CurFlags&PARA_BOX_DOUBLE && !WriteRtfControl(w,rtf,"brdrdb",PARAM_NONE,0)) return FALSE;
       if (!(CurFlags&PARA_BOX_DOUBLE) && !WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
       if (BorderColor!=CLR_AUTO) WriteRtfParaBorderColor(w,rtf,BorderColor);
    }
    else {
       if (CurFlags&PARA_BOX_TOP) {
          if (!WriteRtfControl(w,rtf,"brdrt",PARAM_NONE,0)) return FALSE;
          if (CurFlags&PARA_BOX_THICK && !WriteRtfControl(w,rtf,"brdrw",PARAM_INT,30)) return FALSE;
          if (CurFlags&PARA_BOX_DOUBLE && !WriteRtfControl(w,rtf,"brdrdb",PARAM_NONE,0)) return FALSE;
          if (!(CurFlags&PARA_BOX_DOUBLE) && !WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
          if (BorderColor!=CLR_AUTO) WriteRtfParaBorderColor(w,rtf,BorderColor);
       }
       if (CurFlags&PARA_BOX_BOT) {
          if (!WriteRtfControl(w,rtf,"brdrb",PARAM_NONE,0)) return FALSE;
          if (CurFlags&PARA_BOX_THICK && !WriteRtfControl(w,rtf,"brdrw",PARAM_INT,30)) return FALSE;
          if (CurFlags&PARA_BOX_DOUBLE && !WriteRtfControl(w,rtf,"brdrdb",PARAM_NONE,0)) return FALSE;
          if (!(CurFlags&PARA_BOX_DOUBLE) && !WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
          if (BorderColor!=CLR_AUTO) WriteRtfParaBorderColor(w,rtf,BorderColor);
       }
       if (CurFlags&PARA_BOX_BETWEEN) {
          if (!WriteRtfControl(w,rtf,"brdrbtw",PARAM_NONE,0)) return FALSE;
          if (CurFlags&PARA_BOX_THICK && !WriteRtfControl(w,rtf,"brdrw",PARAM_INT,30)) return FALSE;
          if (CurFlags&PARA_BOX_DOUBLE && !WriteRtfControl(w,rtf,"brdrdb",PARAM_NONE,0)) return FALSE;
          if (!(CurFlags&PARA_BOX_DOUBLE) && !WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
          if (BorderColor!=CLR_AUTO) WriteRtfParaBorderColor(w,rtf,BorderColor);
       }
       if (CurFlags&PARA_BOX_LEFT) {
          if (!WriteRtfControl(w,rtf,"brdrl",PARAM_NONE,0)) return FALSE;
          if (CurFlags&PARA_BOX_THICK && !WriteRtfControl(w,rtf,"brdrw",PARAM_INT,30)) return FALSE;
          if (CurFlags&PARA_BOX_DOUBLE && !WriteRtfControl(w,rtf,"brdrdb",PARAM_NONE,0)) return FALSE;
          if (!(CurFlags&PARA_BOX_DOUBLE) && !WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
          if (BorderColor!=CLR_AUTO) WriteRtfParaBorderColor(w,rtf,BorderColor);
       }
       if (CurFlags&PARA_BOX_RIGHT) {
          if (!WriteRtfControl(w,rtf,"brdrr",PARAM_NONE,0)) return FALSE;
          if (CurFlags&PARA_BOX_THICK && !WriteRtfControl(w,rtf,"brdrw",PARAM_INT,30)) return FALSE;
          if (CurFlags&PARA_BOX_DOUBLE && !WriteRtfControl(w,rtf,"brdrdb",PARAM_NONE,0)) return FALSE;
          if (!(CurFlags&PARA_BOX_DOUBLE) && !WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
          if (BorderColor!=CLR_AUTO) WriteRtfParaBorderColor(w,rtf,BorderColor);
       }
    }

    // write the border margin for the frame type para
    if (CurFlags&PARA_BOX && CurParaFID>0 && !(ParaFrame[CurParaFID].flags&PARA_FRAME_OBJECT)) {
       int margin=ParaFrame[CurParaFID].margin;
       if (margin<0) margin=0;
       if (!WriteRtfControl(w,rtf,"brsp",PARAM_INT,margin)) return FALSE;
    }


    return TRUE;
}

/*****************************************************************************
    WriteRtfParaBorderColor:
    Write paragraph border color
******************************************************************************/
BOOL WriteRtfParaBorderColor(PTERWND w,struct StrRtfOut far *rtf, COLORREF BorderColor)
{
    int j;
    if (BorderColor!=CLR_AUTO) {
       for (j=0;j<rtf->TotalColors;j++) if (rtf->color[j].color==BorderColor) break;
       if (j==rtf->TotalColors) j=0;
       if (!WriteRtfControl(w,rtf,"brdrcf",PARAM_INT,j)) return FALSE;
    }

    return TRUE;
}

/*****************************************************************************
    WriteRtfTab:
    Write the tab information
******************************************************************************/
BOOL WriteRtfTab(PTERWND w,struct StrRtfOut far *rtf, int CurTabId)
{
    int i,count;
    
    count=TerTab[CurTabId].count;
    if (count>TE_MAX_TAB_STOPS) count=0;

    // write tab position
    for (i=0;i<count;i++) {
       if (TerTab[CurTabId].type[i]==TAB_RIGHT)  if (!WriteRtfControl(w,rtf,"tqr",PARAM_NONE,0)) return FALSE;   // right tab stop
       if (TerTab[CurTabId].type[i]==TAB_DECIMAL)if (!WriteRtfControl(w,rtf,"tqdec",PARAM_NONE,0)) return FALSE; // decimal tab stop
       if (TerTab[CurTabId].type[i]==TAB_CENTER) if (!WriteRtfControl(w,rtf,"tqc",PARAM_NONE,0)) return FALSE;   // center tab stop

       if (TerTab[CurTabId].flags[i]==TAB_DOT)   if (!WriteRtfControl(w,rtf,"tldot",PARAM_NONE,0)) return FALSE;   // dot leader
       if (TerTab[CurTabId].flags[i]==TAB_HYPH)  if (!WriteRtfControl(w,rtf,"tlhyph",PARAM_NONE,0)) return FALSE;  // hyphen leader
       if (TerTab[CurTabId].flags[i]==TAB_ULINE) if (!WriteRtfControl(w,rtf,"tlul",PARAM_NONE,0)) return FALSE;    // underline leaders
       if (!WriteRtfControl(w,rtf,"tx",PARAM_INT,TerTab[CurTabId].pos[i])) return FALSE;   // tab stop position
    }

    return TRUE;
}

/*****************************************************************************
    WriteRtfList:
    Write rtf list table and list override tables
******************************************************************************/
BOOL WriteRtfList(PTERWND w,struct StrRtfOut far *rtf)
{
    int i,j,len,RtfLs,ListIdx;
    LPBYTE pString;
    long ListId;

    rtf->XlateLs[0]=0;
     
    if (TotalLists==1 && TotalListOr==1) return TRUE;   // lists not used

    // write the list table
    FlushRtfLine(w,rtf);             // Create line berak for visual aid
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin listtable group

    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"listtable",PARAM_NONE,0)) return FALSE;

    for (i=1;i<TotalLists;i++) {    // write each list
       if (!list[i].InUse) continue;

       FlushRtfLine(w,rtf);             // Create line berak for visual aid
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin list group
       if (!WriteRtfControl(w,rtf,"list",PARAM_NONE,0)) return FALSE;
    
       if (!WriteRtfControl(w,rtf,"listtemplateid",PARAM_INT,list[i].TmplId)) return FALSE;
       if (list[i].LevelCount==1&& !WriteRtfControl(w,rtf,"listsimple",PARAM_NONE,0L)) return FALSE;
       
       if (list[i].flags&LISTFLAG_RESTART_SEC && !WriteRtfControl(w,rtf,"listrestarthdn",PARAM_NONE,0L)) return FALSE;

       // write each list level
       for (j=0;j<list[i].LevelCount;j++) {
          if (!WriteRtfListLevel(w,rtf,&(list[i].level[j]))) return FALSE;
       } 
       
       // write list name
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin listname group
       if (!WriteRtfControl(w,rtf,"listname",PARAM_NONE,0)) return FALSE;
    
       len=1;                                        // write the name with ; suffix
       if (list[i].name) len+=lstrlen(list[i].name);
       pString=MemAlloc(len+1);
       pString[0]=0;
       if (list[i].name) lstrcat(pString,list[i].name);
       lstrcat(pString,";");
       
       if (!WriteRtfText(w,rtf,pString,len)) return FALSE;
       MemFree(pString);

       if (!EndRtfGroup(w,rtf)) return FALSE;        // end listname group

       // write list id
       if (!WriteRtfControl(w,rtf,"listid",PARAM_INT,list[i].id)) return FALSE;
       FlushRtfLine(w,rtf);                          // Create line berak for visual aid

       // end this list group
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end list group
       FlushRtfLine(w,rtf);                          // Create line berak for visual aid
    } 

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end listtable group
    FlushRtfLine(w,rtf);                          // Create line berak for visual aid
    

    // Write list override table
    FlushRtfLine(w,rtf);             // Create line berak for visual aid
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin listoverridetable group

    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"listoverridetable",PARAM_NONE,0)) return FALSE;

    RtfLs=1;                         // rtf list number
    for (i=1;i<TotalListOr;i++) {    // write each list
       if (!ListOr[i].InUse) continue;

       // check if the list-override id used in the bullet table
       for (j=0;j<TotalBlts;j++) if (TerBlt[j].ls==i) break;
       if (j>=TotalBlts) continue;


       FlushRtfLine(w,rtf);             // Create line berak for visual aid
       if (!BeginRtfGroup(w,rtf)) return FALSE;               // begin listoverride group
       if (!WriteRtfControl(w,rtf,"listoverride",PARAM_NONE,0)) return FALSE;
    
       ListIdx=ListOr[i].ListIdx;
       ListId=list[ListIdx].id;
       if (!WriteRtfControl(w,rtf,"listid",PARAM_INT,ListId)) return FALSE;
       if (!WriteRtfControl(w,rtf,"listoverridecount",PARAM_INT,ListOr[i].LevelCount)) return FALSE;

       for (j=0;j<ListOr[i].LevelCount;j++) {
          FlushRtfLine(w,rtf);                          // Create line berak for visual aid
          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin lfolevel group
          if (!WriteRtfControl(w,rtf,"lfolevel",PARAM_NONE,0)) return FALSE;
          
          if (ListOr[i].level[j].flags&LISTLEVEL_REFORMAT && !WriteRtfControl(w,rtf,"listoverrideformat",PARAM_NONE,0L)) return FALSE;
          if (ListOr[i].level[j].flags&LISTLEVEL_RESTART && !WriteRtfControl(w,rtf,"listoverridestartat",PARAM_NONE,0L)) return FALSE;

          // write the list level group
          if (!WriteRtfListLevel(w,rtf,&(ListOr[i].level[j]))) return FALSE;

          if (!EndRtfGroup(w,rtf)) return FALSE;        // end lfolevel group
          FlushRtfLine(w,rtf);                          // Create line berak for visual aid
       } 
       
       if (!WriteRtfControl(w,rtf,"ls",PARAM_INT,RtfLs)) return FALSE;

       // end this list group
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end listoverride group
       FlushRtfLine(w,rtf);                          // Create line berak for visual aid

       rtf->XlateLs[i]=RtfLs;
       RtfLs++;                                      // increment rtf ls number
    } 

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end listoverridetable group
    FlushRtfLine(w,rtf);                          // Create line berak for visual aid
    

    return TRUE;
}

/*****************************************************************************
    WriteRtfListLevel:
    Write rtf list level information
******************************************************************************/
BOOL WriteRtfListLevel(PTERWND w,struct StrRtfOut far *rtf, struct StrListLevel far *pLevel)
{
    int i,len,CurFont,BltFontId=-1;
    BOOL SpecChar,LevelNumber;

    FlushRtfLine(w,rtf);                          // Create line berak for visual aid
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin listlevel group
    if (!WriteRtfControl(w,rtf,"listlevel",PARAM_NONE,0)) return FALSE;
    
    if (!WriteRtfControl(w,rtf,"levelnfc",PARAM_INT,pLevel->NumType)) return FALSE;
    if (!WriteRtfControl(w,rtf,"levelfollow",PARAM_INT,pLevel->CharAft)) return FALSE;
    if (!WriteRtfControl(w,rtf,"levelstartat",PARAM_INT,pLevel->start)) return FALSE;
    if ((pLevel->flags&LISTLEVEL_NO_RESET) && !WriteRtfControl(w,rtf,"levelnorestart",PARAM_NONE,0L)) return FALSE;
    if (pLevel->MinIndent>0 && !WriteRtfControl(w,rtf,"levelindent",PARAM_INT,pLevel->MinIndent)) return FALSE;
    if ((pLevel->flags&LISTLEVEL_OLD) && !WriteRtfControl(w,rtf,"levelold",PARAM_NONE,0L)) return FALSE; // turn-off levelold
    if ((pLevel->flags&LISTLEVEL_LEGAL) && !WriteRtfControl(w,rtf,"levellegal",PARAM_NONE,0L)) return FALSE; 
 
    // write leveltext group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin leveltext group
    if (!WriteRtfControl(w,rtf,"leveltext",PARAM_NONE,0)) return FALSE;

    // write the list text
    len=pLevel->text[0]+1;
    if (pLevel->NumType==LIST_BLT && pLevel->text[0]==0) {   // bullet char not provided, write default
       BYTE typeface[33];
       rtf->flags|=ROFLAG_NO_FLUSH;
       
       // write length
       if (!PutRtfSpecChar(w,rtf,1)) return FALSE;   // write length
       if (!WriteRtfControl(w,rtf,"u",PARAM_INT,-3913)) return FALSE;
       if (!WriteRtfText(w,rtf,"?",1)) return FALSE;

       ResetUintFlag(rtf->flags,ROFLAG_NO_FLUSH);

       lstrcpy(typeface,"Symbol");
       for (i=0;i<TotalFonts;i++) if (TerFont[i].InUse && !(TerFont[i].style&PICT) && lstrcmpi(TerFont[i].TypeFace,typeface)==0) break;
       if (i<TotalFonts) BltFontId=i;
    } 
    else {
       for (i=0;i<len;i++) {
          SpecChar=(i==0 || pLevel->text[i]<9);  // write in \'xx format

          rtf->flags|=ROFLAG_NO_FLUSH;
          if (SpecChar && !PutRtfSpecChar(w,rtf,pLevel->text[i])) return FALSE;
          if (!SpecChar) {
             BYTE chr=pLevel->text[i];
             if (pLevel->NumType==LIST_BLT && chr=='?') chr=0xb7;
             rtf->flags|=ROFLAG_NO_FLUSH;
             if (!WriteRtfText(w,rtf,&chr,1)) return FALSE;
          }
    
       } 
       ResetUintFlag(rtf->flags,ROFLAG_NO_FLUSH);
    }

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end listtext group
    
    // write levelnumbers group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin levelnumbers group
    if (!WriteRtfControl(w,rtf,"levelnumbers",PARAM_NONE,0)) return FALSE;

    for (i=0;i<len;i++) {
       LevelNumber=(i!=0 && pLevel->text[i]<9);  //  level number plaec holder

       if (LevelNumber && !PutRtfSpecChar(w,rtf,(BYTE)i)) return FALSE;
    } 
    if (!EndRtfGroup(w,rtf)) return FALSE;        // end levelnumbers group


    // write font properties
    CurFont=pLevel->FontId;
    if (BltFontId>=0) CurFont=BltFontId;
    if (CurFont>0) {
       COLORREF clr;

       if (lstrcmpi(TerFont[CurFont].TypeFace,TerFont[0].TypeFace)!=0) {
          if (!WriteRtfControl(w,rtf,"f",PARAM_INT,TerFont[CurFont].RtfIndex)) return FALSE;  // write font number
       } 
    
       if (TerFont[CurFont].TwipsSize!=TerFont[0].TwipsSize) {
           if (!WriteRtfControl(w,rtf,"fs",PARAM_INT,TerFont[CurFont].TwipsSize/10)) return FALSE;   // write point size
       } 

       if (!WriteRtfFontStyle(w,rtf,TerFont[CurFont].style,0)) return FALSE;

       clr=TerFont[CurFont].TextColor;
       if (clr!=CLR_AUTO && clr!=CLR_BLACK) {
          for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==clr) break;
          if (i==rtf->TotalColors) i=0;
          if (!WriteRtfControl(w,rtf,"cf",PARAM_INT,i)) return FALSE;        // write the color index
       }

    } 


    // write indentation
    if (pLevel->FirstIndent>0 && !WriteRtfControl(w,rtf,"fi",PARAM_INT,pLevel->FirstIndent)) return FALSE;
    if (pLevel->LeftIndent>0 && !WriteRtfControl(w,rtf,"li",PARAM_INT,pLevel->LeftIndent)) return FALSE;
    if (pLevel->RightIndent>0 && !WriteRtfControl(w,rtf,"ri",PARAM_INT,pLevel->RightIndent)) return FALSE;

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end listlevel group
    FlushRtfLine(w,rtf);                          // Create line berak for visual aid

    return TRUE;
}
 
/*****************************************************************************
    WriteRtfMargin:
    Write default document margins
******************************************************************************/
BOOL WriteRtfMargin(PTERWND w,struct StrRtfOut far *rtf)
{
    float PaperHeight,PaperWidth;
    int i;
    BYTE string[20];

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    // write the information group
    if (rtf->output<RTF_CB) {
       for (i=0;i<INFO_MAX;i++) if (pRtfInfo[i]) break;
       if (i<INFO_MAX) {
          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin info group
          if (!WriteRtfControl(w,rtf,"info",PARAM_NONE,0)) return FALSE;
          for (i=0;i<INFO_MAX;i++) {
             if (!pRtfInfo[i]) continue;
             if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin info item group
             if (!WriteRtfControl(w,rtf,RtfInfo[i],PARAM_NONE,0)) return FALSE;
             WriteRtfText(w,rtf,pRtfInfo[i],lstrlen(pRtfInfo[i]));
             if (!EndRtfGroup(w,rtf)) return FALSE;        // end info item group
          }
          if (!EndRtfGroup(w,rtf)) return FALSE;        // end info group
       }
    }

    // write the paper information
    PaperWidth=(float)8.5;
    PaperHeight=(float)11;
    
    if (hDeviceMode && pDeviceMode && pDeviceMode->dmPaperSize!=DMPAPER_LETTER) {
       if (pDeviceMode->dmPaperSize==DMPAPER_LEGAL) PaperHeight=(float)14;
       else if (pDeviceMode->dmPaperSize==DMPAPER_A4) {
          PaperWidth=(float)8.27;
          PaperHeight=(float)11.69;
       }
       else if (pDeviceMode->dmPaperSize==DMPAPER_TABLOID) {
          PaperWidth=(float)11;
          PaperHeight=(float)17;
       }
       else if (pDeviceMode->dmPaperSize==DMPAPER_ENV_10) {
          PaperHeight=(float)9.5;
          PaperWidth=(float)4.125;
       }

    }
    if (!WriteRtfControl(w,rtf,"paperw",PARAM_INT,InchesToTwips(PaperWidth))) return FALSE;
    if (!WriteRtfControl(w,rtf,"paperh",PARAM_INT,InchesToTwips(PaperHeight))) return FALSE;

    if (!WriteRtfControl(w,rtf,"margl",PARAM_INT,InchesToTwips(TerSect[0].LeftMargin))) return FALSE;
    if (!WriteRtfControl(w,rtf,"margr",PARAM_INT,InchesToTwips(TerSect[0].RightMargin))) return FALSE;
    
    if (!WriteRtfControl(w,rtf,"margt",PARAM_INT,InchesToTwips(TerSect[0].TopMargin*((TerSect[0].flags&SECT_EXACT_MARGT)?-1:1)))) return FALSE;
    if (!WriteRtfControl(w,rtf,"margb",PARAM_INT,InchesToTwips(TerSect[0].BotMargin*((TerSect[0].flags&SECT_EXACT_MARGB)?-1:1)))) return FALSE;

    if (!WriteRtfControl(w,rtf,"headery",PARAM_INT,InchesToTwips(TerSect[0].HdrMargin))) return FALSE;
    if (!WriteRtfControl(w,rtf,"footery",PARAM_INT,InchesToTwips(TerSect[0].FtrMargin))) return FALSE;

    // write other document properties
    if (!WriteRtfControl(w,rtf,"deftab",PARAM_INT,DefTabWidth)) return FALSE;
    if (!WriteRtfControl(w,rtf,"formshade",PARAM_NONE,0L)) return FALSE;

    if (ProtectForm && !WriteRtfControl(w,rtf,"formprot",PARAM_NONE,0L)) return FALSE;   // so tab works to move to the next field - but this disables the editing of the document
    
    // write the footnote number format
    if (FootnoteNumFmt) {
       string[0]=0;
       if      (FootnoteNumFmt==NBR_UPR_ROMAN) lstrcpy(string,"ftnnruc");
       else if (FootnoteNumFmt==NBR_LWR_ROMAN) lstrcpy(string,"ftnnrlc");
       else if (FootnoteNumFmt==NBR_UPR_ALPHA) lstrcpy(string,"ftnnauc");
       else if (FootnoteNumFmt==NBR_LWR_ALPHA) lstrcpy(string,"ftnnalc");
       if (lstrlen(string)>0 && !WriteRtfControl(w,rtf,string,PARAM_NONE,0L)) return FALSE;
    } 

    // write endnote specification
    if (!WriteRtfControl(w,rtf,EndnoteAtSect?"aendnotes":"aenddoc",PARAM_NONE,0L)) return FALSE;
    
    // write the footnote number format
    if (EndnoteNumFmt) {
       string[0]=0;
       if      (EndnoteNumFmt==NBR_UPR_ROMAN) lstrcpy(string,"aftnnruc");
       else if (EndnoteNumFmt==NBR_LWR_ROMAN) lstrcpy(string,"aftnnrlc");
       else if (EndnoteNumFmt==NBR_UPR_ALPHA) lstrcpy(string,"aftnnauc");
       else if (EndnoteNumFmt==NBR_LWR_ALPHA) lstrcpy(string,"aftnnalc");
       if (lstrlen(string)>0 && !WriteRtfControl(w,rtf,string,PARAM_NONE,0L)) return FALSE;
    } 
    
    // write the page border tags
    if (!WriteRtfControl(w,rtf,"pgbrdrhead",PARAM_NONE,0L)) return FALSE;
    if (!WriteRtfControl(w,rtf,"pgbrdrfoot",PARAM_NONE,0L)) return FALSE;
    
    if (NoTabIndent && !WriteRtfControl(w,rtf,"notabind",PARAM_NONE,0L)) return FALSE;   // do not treat left indent as tab stop
    
    if (DocTextFlow==FLOW_RTL && !WriteRtfControl(w,rtf,"rtldoc",PARAM_NONE,0L)) return FALSE;   // do not treat left indent as tab stop
    if (DocTextFlow==FLOW_LTR && !WriteRtfControl(w,rtf,"ltrdoc",PARAM_NONE,0L)) return FALSE;   // do not treat left indent as tab stop

    // write any page background color
    if (false && PageBkColor!=CLR_WHITE) {   // 5/17/2050 - using background group now
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==PageBkColor) break;
       if (i<rtf->TotalColors && !WriteRtfControl(w,rtf,"sspagecb",PARAM_INT,i)) return FALSE; 
    } 

    if (TrackChanges && !WriteRtfControl(w,rtf,"revisions",PARAM_NONE,0L)) return FALSE;   // track-changes enabled

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    return TRUE;
}


/*****************************************************************************
    WriteRtfSection:
    Write the section properties for a given section
******************************************************************************/
BOOL WriteRtfSection(PTERWND w,struct StrRtfOut far *rtf, int sect)
{
    int i,bin;
    BYTE string[20];

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    if (!WriteRtfControl(w,rtf,"sectd",PARAM_NONE,0)) return FALSE;  // set to default

    // write the section border controls
    if (rtf->output<RTF_CB && TerSect[sect].border) {
       if (TerSect[sect].BorderOpts>0 && !WriteRtfControl(w,rtf,"pgbrdropt",PARAM_INT,TerSect[sect].BorderOpts)) return FALSE;  // set to default
       
       for (i=0;i<4;i++) {
          if (i==BORDER_INDEX_TOP && !WriteRtfControl(w,rtf,"pgbrdrt",PARAM_NONE,0)) return FALSE;
          else if (i==BORDER_INDEX_BOT && !WriteRtfControl(w,rtf,"pgbrdrb",PARAM_NONE,0)) return FALSE;
          else if (i==BORDER_INDEX_LEFT && !WriteRtfControl(w,rtf,"pgbrdrl",PARAM_NONE,0)) return FALSE;
          else if (i==BORDER_INDEX_RIGHT && !WriteRtfControl(w,rtf,"pgbrdrr",PARAM_NONE,0)) return FALSE;

          if      (TerSect[sect].BorderType==BRDRTYPE_NONE && !WriteRtfControl(w,rtf,"brdrnone",PARAM_NONE,0L)) return FALSE;  // set to default
          else if (TerSect[sect].BorderType==BRDRTYPE_SINGLE && !WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0L)) return FALSE;  // set to default
          else if (TerSect[sect].BorderType==BRDRTYPE_DBL && !WriteRtfControl(w,rtf,"brdrdb",PARAM_NONE,0L)) return FALSE;  // set to default
          else if (TerSect[sect].BorderType==BRDRTYPE_TRIPLE && !WriteRtfControl(w,rtf,"brdrtriple",PARAM_NONE,0L)) return FALSE;  // set to default
          else if (TerSect[sect].BorderType==BRDRTYPE_SHADOW && !WriteRtfControl(w,rtf,"brdrsh",PARAM_NONE,0L)) return FALSE;  // set to default
          else if (TerSect[sect].BorderType==BRDRTYPE_THICK_THIN && !WriteRtfControl(w,rtf,"brdrthtnmg",PARAM_NONE,0L)) return FALSE;  // set to default
          else if (TerSect[sect].BorderType==BRDRTYPE_THICK_THIN_THICK && !WriteRtfControl(w,rtf,"brdrthtnthmg",PARAM_NONE,0L)) return FALSE;  // set to default
          else if (TerSect[sect].BorderType==BRDRTYPE_THIN_THICK && !WriteRtfControl(w,rtf,"brdrtnthmg",PARAM_NONE,0L)) return FALSE;  // set to default
          else if (TerSect[sect].BorderType==BRDRTYPE_THIN_THICK_THIN && !WriteRtfControl(w,rtf,"brdrtnthtnmg",PARAM_NONE,0L)) return FALSE;  // set to default
       
          if (!WriteRtfControl(w,rtf,"brdrw",PARAM_INT,TerSect[sect].BorderWidth[i])) return FALSE;  // set to default
          if (!WriteRtfControl(w,rtf,"brsp",PARAM_INT,TerSect[sect].BorderSpace[i])) return FALSE;  // set to default
          if (TerSect[sect].BorderColor!=CLR_AUTO) {
              int i;
              for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==TerSect[sect].BorderColor) break;
              if (i==rtf->TotalColors) i=0;
              if (!WriteRtfControl(w,rtf,"brdrcf",PARAM_INT,i)) return FALSE;  // set to default
          }
       } 
    } 

    // write the landscape and paper dimensions
    //if (hDeviceMode && pDeviceMode && (TRUE || TerSect[sect].orient==DMORIENT_LANDSCAPE || MultiOrient(w))) {
    if (true) {
       float PaperWidth=(float)8.5;
       float PaperHeight=(float)11;
       int   PprSize=TerSect[sect].PprSize;

       if (TerSect[sect].orient==DMORIENT_LANDSCAPE && !WriteRtfControl(w,rtf,"lndscpsxn",PARAM_NONE,0)) return FALSE;

       if (PprSize==DMPAPER_LEGAL) PaperHeight=(float)14;
       else if (PprSize==DMPAPER_A4) {
          PaperWidth=(float)8.27;
          PaperHeight=(float)11.69;
       }
       else if (PprSize==DMPAPER_ENV_10) {
          PaperHeight=(float)9.5;
          PaperWidth=(float)4.125;
       }
       else if (PprSize!=DMPAPER_LETTER) {
          PaperHeight=TerSect[sect].PprHeight;
          PaperWidth=TerSect[sect].PprWidth;
       } 

       if (TerSect[sect].orient==DMORIENT_LANDSCAPE) {  // swap width and height
          float temp=PaperWidth;
          PaperWidth=PaperHeight;
          PaperHeight=temp;
       } 


       if (!WriteRtfControl(w,rtf,"pgwsxn",PARAM_INT,InchesToTwips(PaperWidth))) return FALSE;
       if (!WriteRtfControl(w,rtf,"pghsxn",PARAM_INT,InchesToTwips(PaperHeight))) return FALSE;
    }

    // write the bin information
    bin=TerSect[sect].FirstPageBin;
    if (bin && bin!=DMBIN_AUTO && !WriteRtfControl(w,rtf,"binfsxn",PARAM_INT,bin)) return FALSE;
    bin=TerSect[sect].bin;
    if (bin && bin!=DMBIN_AUTO && !WriteRtfControl(w,rtf,"binsxn",PARAM_INT,bin)) return FALSE;

    // write titlepg when using first page header/footer
    if (TerSect1[sect].fhdr.FirstLine>=0 || TerSect1[sect].fftr.FirstLine>=0 || TerSect[sect].flags&SECT_HAS_TITLE_PAGE) {
       if (!WriteRtfControl(w,rtf,"titlepg",PARAM_NONE,0)) return FALSE;
    }

    // write margins
    if (!WriteRtfControl(w,rtf,"marglsxn",PARAM_INT,InchesToTwips(TerSect[sect].LeftMargin))) return FALSE;
    if (!WriteRtfControl(w,rtf,"margrsxn",PARAM_INT,InchesToTwips(TerSect[sect].RightMargin))) return FALSE;
    
    if (!WriteRtfControl(w,rtf,"margtsxn",PARAM_INT,InchesToTwips(TerSect[sect].TopMargin*((TerSect[sect].flags&SECT_EXACT_MARGT)?-1:1)))) return FALSE;
    if (!WriteRtfControl(w,rtf,"margbsxn",PARAM_INT,InchesToTwips(TerSect[sect].BotMargin*((TerSect[sect].flags&SECT_EXACT_MARGB)?-1:1)))) return FALSE;

    if (!WriteRtfControl(w,rtf,"headery",PARAM_INT,InchesToTwips(TerSect[sect].HdrMargin))) return FALSE;
    if (!WriteRtfControl(w,rtf,"footery",PARAM_INT,InchesToTwips(TerSect[sect].FtrMargin))) return FALSE;

    // write break
    if (!(TerSect[sect].flags&SECT_NEW_PAGE)) if (!WriteRtfControl(w,rtf,"sbknone",PARAM_NONE,0)) return FALSE;
    if (TerSect[sect].flags&SECT_NEW_PAGE) if (!WriteRtfControl(w,rtf,"sbkpage",PARAM_NONE,0)) return FALSE;

    if (TerSect[sect].flags&SECT_RESTART_PAGE_NO) {
       if (!WriteRtfControl(w,rtf,"pgnstarts",PARAM_INT,TerSect[sect].FirstPageNo)) return FALSE;
       if (!WriteRtfControl(w,rtf,"pgnrestart",PARAM_NONE,0)) return FALSE;
    }
    else if (!WriteRtfControl(w,rtf,"pgncont",PARAM_NONE,0)) return FALSE;

    // write page alignment
    if      (TerSect[sect].flags&SECT_VALIGN_CTR && !WriteRtfControl(w,rtf,"vertalc",PARAM_NONE,0)) return FALSE;
    else if (TerSect[sect].flags&SECT_VALIGN_BOT && !WriteRtfControl(w,rtf,"vertal",PARAM_NONE,0)) return FALSE;

    // write columns
    if (TerSect[sect].columns>1) {
       if (!WriteRtfControl(w,rtf,"cols",PARAM_INT,TerSect[sect].columns)) return FALSE;
       if (!WriteRtfControl(w,rtf,"colsx",PARAM_INT,InchesToTwips(TerSect[sect].ColumnSpace))) return FALSE;
    }

    // write default page number format
    if      (TerSect[sect].PageNumFmt==NBR_UPR_ROMAN) lstrcpy(string,"pgnucrm");
    else if (TerSect[sect].PageNumFmt==NBR_LWR_ROMAN) lstrcpy(string,"pgnlcrm");
    else if (TerSect[sect].PageNumFmt==NBR_UPR_ALPHA) lstrcpy(string,"pgnucltr");
    else if (TerSect[sect].PageNumFmt==NBR_LWR_ALPHA) lstrcpy(string,"pgnlcltr");
    else                                              lstrcpy(string,"pgndec");
    if (!WriteRtfControl(w,rtf,string,PARAM_NONE,0L)) return FALSE;
    
    // write rtl/ltr text flow
    if (TerSect[sect].flow==FLOW_RTL && !WriteRtfControl(w,rtf,"rtlsect",PARAM_NONE,0L)) return FALSE;   // do not treat left indent as tab stop
    if (TerSect[sect].flow==FLOW_LTR && !WriteRtfControl(w,rtf,"ltrsect",PARAM_NONE,0L)) return FALSE;   // do not treat left indent as tab stop

    // write line numbering tag
    if (True(TerSect[sect].flags&SECT_LINE) && !WriteRtfControl(w,rtf,"linemod",PARAM_INT,1L)) return FALSE;

    rtf->sect=sect;
    
    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    return TRUE;
}

/*****************************************************************************
    WriteRtfObject:
    Write the OLE object.
******************************************************************************/
BOOL WriteRtfObject(PTERWND w,struct StrRtfOut far *rtf,int obj)
{
   DWORD ObjectSize,l;

   if (TerFlags&TFLAG_NO_OLE) return TRUE;      // ole objects not supported

   FlushRtfLine(w,rtf);             // Create line berak for visual aid

   if (!BeginRtfGroup(w,rtf)) return FALSE;     // begin current object
   if (!WriteRtfControl(w,rtf,"object",PARAM_NONE,0)) return FALSE;      // write object group control
   if (TerFont[obj].ObjectType==OBJ_EMBED) {
      if (!WriteRtfControl(w,rtf,"objemb",PARAM_NONE,0)) return FALSE;
   }
   else if (TerFont[obj].ObjectType==OBJ_OCX) {
      if (!WriteRtfControl(w,rtf,"objocx",PARAM_NONE,0)) return FALSE;
   }
   else {
      if (!WriteRtfControl(w,rtf,"objautlink",PARAM_NONE,0)) return FALSE;
      if (!WriteRtfControl(w,rtf,"rsltpict",PARAM_NONE,0))   return FALSE;
   }

   if (TerFont[obj].ObjectUpdate && !WriteRtfControl(w,rtf,"objupdate",PARAM_NONE,0))   return FALSE;

   // write icon/content display
   if (TerFont[obj].ObjectAspect==OASPECT_ICON && !WriteRtfControl(w,rtf,"ssicon",PARAM_NONE,0))   return FALSE;
   if (TerFont[obj].ObjectAspect==OASPECT_CONTENT && !WriteRtfControl(w,rtf,"sscontent",PARAM_NONE,0))   return FALSE;

   // write the object data
   if (!BeginRtfGroup(w,rtf)) return FALSE;     // begin object data
   if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
   if (!WriteRtfControl(w,rtf,"objdata",PARAM_NONE,0)) return FALSE;

   if (!GetOleStorageData(w,obj)) return FALSE;
   if (!TerOleQuerySize(w,obj,&ObjectSize)) return FALSE;

   for (l=0;l<ObjectSize;l++) {
      if (!PutRtfHexChar(w,rtf,pOleData[l])) break;
   }
   GlobalUnlock(hOleData);
   GlobalFree(hOleData);

   if (!EndRtfGroup(w,rtf)) return FALSE;       // end the objdata group

   // write the result
   if (!BeginRtfGroup(w,rtf)) return FALSE;     // begin result
   if (!WriteRtfControl(w,rtf,"result",PARAM_NONE,0)) return FALSE;
   //if (!WriteRtfMetafile(w,rtf,obj)) return FALSE;
   if (!WriteRtfPicture(w,rtf,obj)) return FALSE;

   if (!EndRtfGroup(w,rtf)) return FALSE;       // end result

   // end the object
   if (!EndRtfGroup(w,rtf)) return FALSE;

   FlushRtfLine(w,rtf);             // Create line berak for visual aid

   return TRUE;
}

/*****************************************************************************
    EndInterParaGroups
    Terminates those character attributes which are limited to the paragraph
******************************************************************************/
BOOL EndInterParaGroups(PTERWND w,struct StrRtfOut far *rtf, int NewFont)
{
    int CurFieldId=rtf->group[rtf->GroupLevel].FieldId;

    if (rtf->flags&ROFLAG_IN_SUPSCR) {
       if (!EndRtfGroup(w,rtf)) return FALSE;      // end the group
       rtf->flags=ResetUintFlag(rtf->flags,ROFLAG_IN_SUPSCR);  // out of superscript group
    }
    if (rtf->flags&ROFLAG_IN_SUBSCR) {
       if (!EndRtfGroup(w,rtf)) return FALSE;      // end the group
       rtf->flags=ResetUintFlag(rtf->flags,ROFLAG_IN_SUBSCR);  // out of subscript group
    }
    if (rtf->flags&ROFLAG_IN_FIELD_NAME) {
       if (!EndRtfGroup(w,rtf)) return FALSE;      // end the fldinst group
       if (!EndRtfGroup(w,rtf)) return FALSE;      // end the field group
       ResetUintFlag(rtf->flags,ROFLAG_IN_FIELD_NAME);  // out of subscript group
    }
    if (CurFieldId==FIELD_DATA && TerFont[NewFont].FieldId!=FIELD_DATA) {
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end fldrslt group
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end current field group
    }
    return TRUE;
}


/*****************************************************************************
    WriteRtfBackground:
    Write the rtf background information
******************************************************************************/
BOOL WriteRtfBackground(PTERWND w,struct StrRtfOut far *rtf,COLORREF BkColor)
{
    BYTE string[128];
    
    if (TerFlags3&TFLAG3_NO_RTF_BKND_COLOR) return TRUE;

    if (!WriteRtfControl(w,rtf,"viewkind",PARAM_INT,5L)) return FALSE;    // needed to show the color in MSWord

    // being the 'background' destination group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shpinst destination group
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"background",PARAM_NONE,0)) return FALSE;

    // write the shape group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shp group
    if (!WriteRtfControl(w,rtf,"shp",PARAM_NONE,0)) return FALSE;

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shpinst destination group
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"shpinst",PARAM_NONE,0)) return FALSE;

    if (!WriteRtfControl(w,rtf,"shpleft",PARAM_INT,0L)) return FALSE;
    if (!WriteRtfControl(w,rtf,"shpright",PARAM_INT,0L)) return FALSE;
    if (!WriteRtfControl(w,rtf,"shptop",PARAM_INT,0L)) return FALSE;
    if (!WriteRtfControl(w,rtf,"shpbottom",PARAM_INT,0L)) return FALSE;

    if (!WriteRtfControl(w,rtf,"shpbymargin",PARAM_NONE,0)) return FALSE;

    if (!WriteRtfControl(w,rtf,"shpwr",PARAM_INT,SWRAP_NO_WRAP)) return FALSE;
    if (!WriteRtfControl(w,rtf,"shpwrk",PARAM_INT,WRAPSIDE_BOTH)) return FALSE;

    // write shape type
    wsprintf(string,"%d",SHPTYPE_RECT);
    if (!WriteRtfShapeProp(w,rtf,"shapeType",string)) return FALSE;

    // write fillcolor
    wsprintf(string,"%lu",(DWORD)BkColor);
    if (!WriteRtfShapeProp(w,rtf,"fillColor",string)) return FALSE;


    if (!EndRtfGroup(w,rtf)) return FALSE;        // end shpinst group

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end shp group
    
    if (!EndRtfGroup(w,rtf)) return FALSE;        // end background group

    return TRUE;
}

/*****************************************************************************
    WriteRtfShape:
    Write the shape group.
******************************************************************************/
BOOL WriteRtfShape(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
    int  i,ParaFID=TerFont[pict].ParaFID;
    struct StrParaFrame far *pFrame=&(ParaFrame[ParaFID]);
    int ShapeType;
    BYTE string[128];
    BOOL result;
    struct StrAnim far *anim;
    BOOL LinkedGif,FirstAnimPict=FALSE;
    BOOL IsShpGrp;
    int x,y;

    // decide if the picture is animated and write enclosing animation group
    anim=TerFont[pict].anim;
    if (anim && (TerFont[pict].anim->FirstPict==0 || TerFont[pict].anim->FirstPict==pict))
       FirstAnimPict=TRUE;

    LinkedGif=TerFont[pict].LinkFile && strstr(TerFont[pict].LinkFile,".GIF") && FirstAnimPict;
    if (LinkedGif) FirstAnimPict=FALSE;    // do not write gif animation when doing clipboard copy of a gif file

    if (FirstAnimPict) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin a destination group
       if (!WriteRtfControl(w,rtf,"ssanimdelay",PARAM_INT,anim->delay)) return FALSE;  // write picture format
       if (!WriteRtfControl(w,rtf,"ssanimloops",PARAM_INT,anim->OrigLoopCount)) return FALSE;  // write picture format
    }

    // Check if drawing object also present in the document - Word can not handle drawing object and shape in the same document
    if (false && !(TerFlags4&TFLAG4_SAVE_SHAPE_WITH_DRAW_OBJECT)) {   // we write drawing objects as shape now
       for (i=0;i<TotalParaFrames;i++) {
         if (i!=ParaFID && ParaFrame[i].InUse && ParaFrame[i].flags&PARA_FRAME_OBJECT && ParaFrame[i].pict==0) break;
       } 

       if (i<TotalParaFrames) {   // drawing object present, do not write the shape object
          if (TerFont[pict].ObjectType==OBJ_NONE || TerFont[pict].ObjectType==OBJ_EMBED_TEMP)
               result=WriteRtfPicture(w,rtf,pict);
          else result=WriteRtfObject(w,rtf,pict);
          if (!result) return FALSE;
          goto END;
       }
    }


    // write the outer shape or shpgrp group
    IsShpGrp=True(ParaFrame[ParaFID].flags&PARA_FRAME_SHPGRP);

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shp group
    if (!WriteRtfControl(w,rtf,IsShpGrp?"shpgrp":"shp",PARAM_NONE,0)) return FALSE;

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shpinst destination group
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"shpinst",PARAM_NONE,0)) return FALSE;

    // write the location and size
    if (True(pFrame->flags&PARA_FRAME_IGNORE_X)) {  // use the OrgX value
       x=pFrame->OrgX;
       if (True(pFrame->flags&PARA_FRAME_HPAGE)) {  // convert OrgX to margin relative value
           int sect=GetSection(w,rtf->line);
           x-=(int)(TerSect[sect].LeftMargin*1440);
       }
    }
    else x=pFrame->x;
     
    if (!WriteRtfControl(w,rtf,"shpleft",PARAM_INT,x)) return false;
    if (!WriteRtfControl(w,rtf,"shpright",PARAM_INT,x+pFrame->width)) return false;
        
    y=True(pFrame->flags&PARA_FRAME_IGNORE_Y)?pFrame->OrgY:pFrame->ParaY;
    if (!WriteRtfControl(w,rtf,"shptop",PARAM_INT,y)) return false;
    if (!WriteRtfControl(w,rtf,"shpbottom",PARAM_INT,y+pFrame->height)) return false;

    if (!WriteRtfControl(w,rtf,"shpbxmarg",PARAM_NONE,0)) return FALSE;
    if (pFrame->flags&PARA_FRAME_VMARG && !WriteRtfControl(w,rtf,"shpbymargin",PARAM_NONE,0)) return FALSE;
    if (pFrame->flags&PARA_FRAME_VPAGE && !WriteRtfControl(w,rtf,"shpbypage",PARAM_NONE,0)) return FALSE;
    if (!(pFrame->flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG)) && !WriteRtfControl(w,rtf,"shpbypara",PARAM_NONE,0)) return FALSE;


    if (True(pFrame->flags&PARA_FRAME_IGNORE_X) && !WriteRtfControl(w,rtf,"shpbxignore",PARAM_NONE,0)) return FALSE;
    if (True(pFrame->flags&PARA_FRAME_IGNORE_Y) && !WriteRtfControl(w,rtf,"shpbyignore",PARAM_NONE,0)) return FALSE;

    if (pFrame->flags&PARA_FRAME_NO_WRAP && !WriteRtfControl(w,rtf,"shpwr",PARAM_INT,SWRAP_NO_WRAP)) return FALSE;
    if (pFrame->flags&PARA_FRAME_WRAP_THRU && !WriteRtfControl(w,rtf,"shpwr",PARAM_INT,SWRAP_IGNORE)) return FALSE;
    if (!(pFrame->flags&(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU))) {
       if (!WriteRtfControl(w,rtf,"shpwr",PARAM_INT,SWRAP_AROUND)) return FALSE;
       if (!WriteRtfControl(w,rtf,"shpwrk",PARAM_INT,WRAPSIDE_BOTH)) return FALSE;
    }
    
    if (pFrame->ZOrder<0 || ParaFID==WmParaFID) {
       if (!WriteRtfShapeProp(w,rtf,"fBehindDocument","1")) return FALSE;   // 14 means linked image
    }
    else if (!WriteRtfControl(w,rtf,"shpz",PARAM_INT,(pFrame->ZOrder<0?0:pFrame->ZOrder))) return FALSE;   // Word does not support negative Zorder

    // write left or right alignment
    if (TerFont[pict].FrameType==PFRAME_LEFT && !WriteRtfControl(w,rtf,"ssshpalignleft",PARAM_NONE,0L)) return FALSE;
    if (TerFont[pict].FrameType==PFRAME_RIGHT && !WriteRtfControl(w,rtf,"ssshpalignright",PARAM_NONE,0L)) return FALSE;

    if (IsShpGrp) {  // write the relative positioning information for a shape group
       if (!WriteRtfShapeProp(w,rtf,"groupLeft","0")) return false;
       if (!WriteRtfShapeProp(w,rtf,"groupTop","0")) return false;
       if (!WriteRtfShapeProp2(w,rtf,"groupRight",pFrame->width)) return false;
       if (!WriteRtfShapeProp2(w,rtf,"groupBottom",pFrame->height)) return false;

       if (!WriteRtfShapeProp2(w,rtf,"posh",0)) return false;
       if (!WriteRtfShapeProp2(w,rtf,"posrelh",3)) return false;
       if (!WriteRtfShapeProp2(w,rtf,"posv",0)) return false;
       if (!WriteRtfShapeProp2(w,rtf,"posrelv",3)) return false;
    }
    else {    // write the shape type
       ShapeType=pFrame->ShapeType;
       if (TerFont[pict].ObjectType==OBJ_OCX) ShapeType=SHPTYPE_OCX;
       if (ShapeType==0) ShapeType=SHPTYPE_PICT_FRAME;

       wsprintf(string,"%d",ShapeType);
       if (!WriteRtfShapeProp(w,rtf,"shapeType",string)) return FALSE;
    
       // write the attributes
       if (ShapeType==SHPTYPE_LINE) {
          if (pFrame->flags&PARA_FRAME_NO_LINE) {
            if (!WriteRtfShapeProp(w,rtf,"fLine","0")) return FALSE;
          }
          if (pFrame->LineType==DOL_DIAG && !WriteRtfShapeProp(w,rtf,"fFlipV","1")) return FALSE;
       } 
       else {
          wsprintf(string,"%d",((pFrame->flags&PARA_FRAME_BOXED) && pFrame->LineWdth>0)?1:0);
          if (!WriteRtfShapeProp(w,rtf,"fLine",string)) return FALSE;
       }
       
       if (ShapeType==SHPTYPE_LINE || pFrame->flags&PARA_FRAME_BOXED) {
           wsprintf(string,"%d",TwipsToEmu(pFrame->LineWdth));
           if (!WriteRtfShapeProp(w,rtf,"lineWidth",string)) return FALSE;

           wsprintf(string,"%d",pFrame->LineColor);
           if (!WriteRtfShapeProp(w,rtf,"lineColor",string)) return FALSE;

           if (pFrame->flags&PARA_FRAME_DOTTED) {
              wsprintf(string,"%d",6);
              if (!WriteRtfShapeProp(w,rtf,"lineDashing",string)) return FALSE;
           } 
       }
      
       if (ShapeType==SHPTYPE_RECT) {
           wsprintf(string,"%d",pFrame->FillPattern==0?0:1);
           if (!WriteRtfShapeProp(w,rtf,"fFilled",string)) return FALSE;
          
           if (pFrame->FillPattern>0) {
              wsprintf(string,"%d",pFrame->BackColor);
              if (!WriteRtfShapeProp(w,rtf,"fillColor",string)) return FALSE;
           }
       }  
       if (pFrame->flags&PARA_FRAME_IN_CELL) {
          wsprintf(string,"%d",1);
          if (!WriteRtfShapeProp(w,rtf,"fLayoutInCell",string)) return FALSE;
       } 

    }

    // write the shape property for the picture group
    if (TerFont[pict].LinkFile && TerFont[pict].ObjectType==OBJ_NONE
        && !(TerFlags&TFLAG_IGNORE_PICT_LINK)) {

       AddSlashes(w,TerFont[pict].LinkFile,string,2);   // replace one backslash by 2

       rtf->flags|=ROFLAG_IGNORE_SLASH;  // do not tread backslash as a special character for the next write
       if (!WriteRtfShapeProp(w,rtf,"pibName",string)) return FALSE;
       ResetUintFlag(rtf->flags,ROFLAG_IGNORE_SLASH);

       if (!WriteRtfShapeProp(w,rtf,"pibFlags","14")) return FALSE;   // 14 means linked image
    }
    else {
       if (TerFont[pict].PictType!=PICT_SHAPE) {
          if (!WriteRtfShapeProp(w,rtf,"pib",NULL)) return FALSE;
          rtf->flags|=ROFLAG_NO_ANIM;  // do not write animation pictures
          if (!WriteRtfPicture(w,rtf,pict)) return FALSE;
          ResetUintFlag(rtf->flags,ROFLAG_NO_ANIM);

          if (!EndRtfGroup(w,rtf)) return FALSE;        // end the sv group iniitated by the WriteRtfShapeProp function
          if (!EndRtfGroup(w,rtf)) return FALSE;        // end the sp group initiated by the WriteRtfShapeProp function
       }
    }

    // write the ole object data if any
    if (TerFont[pict].ObjectType!=OBJ_NONE && TerFont[pict].ObjectType!=OBJ_EMBED_TEMP) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shptxt group
       if (!WriteRtfControl(w,rtf,"shptxt",PARAM_NONE,0)) return FALSE;
       WriteRtfObject(w,rtf,pict);
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end shptxt group
    }

    // write the inner shp gruop for the ShpGrp group
    if (IsShpGrp) {

       if (!BeginRtfGroup(w,rtf)) return false;      // begin shp group
       if (!WriteRtfControl(w,rtf,"shp",PARAM_NONE,0)) return false;

       if (!BeginRtfGroup(w,rtf)) return false;      // begin shpinst destination group
       if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return false;
       if (!WriteRtfControl(w,rtf,"shpinst",PARAM_NONE,0)) return false;
     
       // write relative positioning information
       x=pFrame->GroupX;
       y=pFrame->GroupY;
       
       wsprintf(string,"%d",x);
       if (!WriteRtfShapeProp(w,rtf,"relLeft",string)) return false;
       wsprintf(string,"%d",y);
       if (!WriteRtfShapeProp(w,rtf,"relTop",string)) return false;

       x+=pFrame->width;           // right
       y+=pFrame->height;          // bottom

       wsprintf(string,"%d",x);
       if (!WriteRtfShapeProp(w,rtf,"relRight",string)) return false;
       wsprintf(string,"%d",y);
       if (!WriteRtfShapeProp(w,rtf,"relBottom",string)) return false;

       // write shape type
       ShapeType=ParaFrame[ParaFID].ShapeType;
       wsprintf(string,"%d",ShapeType);
       if (!WriteRtfShapeProp(w,rtf,"shapeType",string)) return false;

       if (!EndRtfGroup(w,rtf)) return false;        // end shpinst group
       if (!EndRtfGroup(w,rtf)) return false;        // end shp group
    } 

    // write any watermark related properties
    if (ParaFID==WmParaFID) {
       if (!WriteRtfShapeProp(w,rtf,"wzName","WordPictureWatermark3")) return false;
       if (!WriteRtfShapeProp(w,rtf,"posh","2")) return false;     // centering
       if (!WriteRtfShapeProp(w,rtf,"posrelh","0")) return false;  // relative to margin
       if (!WriteRtfShapeProp(w,rtf,"posv","2")) return false;     // centering
       if (!WriteRtfShapeProp(w,rtf,"posrelv","0")) return false;  // relative to margin
    } 

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the outer shpinst group


    // write shape result
    if (!IsShpGrp && TerFont[pict].PictType!=PICT_SHAPE) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shprslt group
       if (!WriteRtfControl(w,rtf,"shprslt",PARAM_NONE,0)) return FALSE;

       if (TerFont[pict].ObjectType==OBJ_NONE || TerFont[pict].ObjectType==OBJ_EMBED_TEMP)
            result=WriteRtfPicture(w,rtf,pict);
       else result=WriteRtfObject(w,rtf,pict);
       if (!result) return FALSE;

       if (!EndRtfGroup(w,rtf)) return FALSE;        // end shprslt group
    }

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the outer shp or shpgrp group

    END:
    // write animation sequence if any
    if (FirstAnimPict) {
       if (!EndRtfGroup(w,rtf)) return FALSE;  // end the animation group
       WriteRtfAnimSeq(w,rtf,pict);            // write picture in the animation sequence
    }

    return TRUE;
}

/*****************************************************************************
    WriteRtfShapeProp2:
    Write a shape property.  if 'value' is NULL then the value text and two
    ending braces are not written out.
******************************************************************************/
BOOL WriteRtfShapeProp2(PTERWND w,struct StrRtfOut far *rtf,LPBYTE name, int value)
{
    BYTE str[10];

    wsprintf(str,"%d",value);
    return WriteRtfShapeProp(w,rtf,name,str);
}
 
/*****************************************************************************
    WriteRtfShapeProp:
    Write a shape property.  if 'value' is NULL then the value text and two
    ending braces are not written out.
******************************************************************************/
BOOL WriteRtfShapeProp(PTERWND w,struct StrRtfOut far *rtf,LPBYTE name, LPBYTE value)
{
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin sp group
    if (!WriteRtfControl(w,rtf,"sp",PARAM_NONE,0)) return FALSE;

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin sn group
    if (!WriteRtfControl(w,rtf,"sn",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfText(w,rtf,name,lstrlen(name))) return FALSE; // write text
    if (!EndRtfGroup(w,rtf)) return FALSE;        // end sn group

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin sv group
    if (!WriteRtfControl(w,rtf,"sv",PARAM_NONE,0)) return FALSE;

    if (value==NULL) return TRUE;

    if (!WriteRtfText(w,rtf,value,lstrlen(value))) return FALSE; // write text
    if (!EndRtfGroup(w,rtf)) return FALSE;        // end sv group

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end sp group

    return TRUE;
}

/*****************************************************************************
    WriteRtfPicture:
    Write the picture group.
******************************************************************************/
BOOL WriteRtfPicture(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
    BOOL FirstAnimPict=FALSE;       // is it a first animation picture?
    BOOL InAnimSequence=FALSE;      // is it part of the animation sequence?
    struct StrAnim far *anim;
    BOOL result=false;
    BOOL LinkedGif;

    // decide if the picture is animated
    anim=TerFont[pict].anim;
    if (anim) {
       if (!(rtf->flags&ROFLAG_IN_ANIM) && (TerFont[pict].anim->FirstPict==0 || TerFont[pict].anim->FirstPict==pict))
            FirstAnimPict=TRUE;
       else InAnimSequence=TRUE;
    }

    LinkedGif=TerFont[pict].LinkFile && (strstr(TerFont[pict].LinkFile,".GIF") && (FirstAnimPict || InAnimSequence));
    if (LinkedGif) FirstAnimPict=InAnimSequence=FALSE;    // do not write gif animation when doing clipboard copy of a gif file

    // if in animation sequence, write the enclosing group
    if (FirstAnimPict || InAnimSequence) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin a destination group
       if (InAnimSequence) {
          if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
          if (!WriteRtfControl(w,rtf,"ssanimseq",PARAM_NONE,0)) return FALSE;
       }
       if (!WriteRtfControl(w,rtf,"ssanimdelay",PARAM_INT,anim->delay)) return FALSE;  // write picture format
       if (FirstAnimPict && !WriteRtfControl(w,rtf,"ssanimloops",PARAM_INT,anim->OrigLoopCount)) return FALSE;  // write picture format
    }


    if (TerFont[pict].LinkFile && !(TerFlags&TFLAG_IGNORE_PICT_LINK))
       result=WriteRtfLinkedPicture(w,rtf,pict);
    else {
       if      (TerFont[pict].PictType==PICT_METAFILE) result=WriteRtfMetafile(w,rtf,pict);
       else if (TerFont[pict].PictType==PICT_META7)    result=WriteRtfMetafile(w,rtf,pict);
       #if defined(WIN32)
       else if (TerFont[pict].PictType==PICT_ENHMETAFILE) result=WriteRtfEnhMetafile(w,rtf,pict);
       #endif
       else if (TerFont[pict].PictType==PICT_DIBITMAP) {
          if ((TerFont[pict].OrigImageSize>0 &&  TerFont[pict].pOrigImage) && False(TerFlags5&TFLAG5_SAVE_PICT_AS_WMF) 
               && (TerFont[pict].OrigPictType!=PICT_GIF)) 
              result=WriteRtfOrigImage(w,rtf,pict,TerFont[pict].OrigPictType,TerFont[pict].pOrigImage,TerFont[pict].OrigImageSize);
          else if (TerFlags4&TFLAG4_SAVE_BMP_AS_PNG) {
              HGLOBAL hBuf;
              long BufLen;
              if (TerBmp2Png(hTerWnd,pict,TER_BUF,"",&hBuf,&BufLen)) {
                 LPBYTE pImage=GlobalLock(hBuf);
                 result=WriteRtfOrigImage(w,rtf,pict,PICT_PNG,pImage,BufLen);
                 GlobalUnlock(hBuf);
                 GlobalFree(hBuf);
              }
              else result=FALSE; 
          } 
          else result=WriteRtfDIB(w,rtf,pict);
       }
       else if (TerFont[pict].PictType==PICT_CONTROL)  result=WriteRtfCtl(w,rtf,pict);
       else if (TerFont[pict].PictType==PICT_FORM)     result=WriteRtfForm(w,rtf,pict);
    }

    if ((FirstAnimPict || InAnimSequence) && !EndRtfGroup(w,rtf)) return FALSE;  // end the animation group

    // write the subsequent animation pictures
    if (FirstAnimPict && !(rtf->flags&ROFLAG_NO_ANIM)) WriteRtfAnimSeq(w,rtf,pict);

    return result;
}

/*****************************************************************************
    WriteRtfAnimSeq:
    Write the picture in the animation sequnence starting from the second
    picture
******************************************************************************/
BOOL WriteRtfAnimSeq(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
    int CurPict;
    struct StrAnim far *anim;

    rtf->flags|=ROFLAG_IN_ANIM;     // write the animation sequence
    CurPict=pict;

    while (TRUE) {
       anim=TerFont[CurPict].anim;
       if (!anim) break;

       CurPict=anim->NextPict;
       if (CurPict==0 || TerFont[CurPict].anim==NULL) break;      // end of the sequence

       if (!WriteRtfPicture(w,rtf,CurPict)) break;
    }
    ResetUintFlag(rtf->flags,ROFLAG_IN_ANIM);

    return TRUE;
}

/*****************************************************************************
    WriteRtfEnhMetafile:
    Write the enh metafile group and picture data for the specified picture.
******************************************************************************/
BOOL WriteRtfEnhMetafile(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
   long l,bmHeight,bmWidth,OrigWidth,OrigHeight;
   BYTE huge *pMem;
   BOOL result=TRUE;
   int x,ScaleX,ScaleY;
   HGLOBAL hGlb=0;
   LPBYTE  pBuf=0;
   HDC hDC=0;
   DWORD OldSize,WinMetaSize;
   METAFILEPICT mfp;
   ENHMETAHEADER far *pEnhHdr;
   BOOL WriteOldMetafileFormat=false;


   FlushRtfLine(w,rtf);             // Create line berak for visual aid

   // retrieve picture handle
   if ( NULL==(pMem=TerGetEnhMetaFileBits(TerFont[pict].hEnhMeta))) {
     PrintError(w,MSG_ERR_META_ACCESS,"WriteRtfEnhMetafile");
     return FALSE;
   }


   // begin shppict
   if (!BeginRtfGroup(w,rtf)) return FALSE;
   if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
   if (!WriteRtfControl(w,rtf,"shppict",PARAM_NONE,0)) return FALSE;

   // begin pict
   if (!BeginRtfGroup(w,rtf)) return FALSE;
   if (!WriteRtfControl(w,rtf,"pict",PARAM_NONE,0)) return FALSE;

   // write picture height/width in HIMETRIC
   if (!WriteRtfControl(w,rtf,"emfblip",PARAM_NONE,0)) return FALSE;  // write picture format

   // get the original size from the metaheader
   pEnhHdr=(LPVOID)pMem;
   bmHeight=MulDiv(pEnhHdr->rclFrame.bottom-pEnhHdr->rclFrame.top,1440,2540);
   bmWidth=MulDiv(pEnhHdr->rclFrame.right-pEnhHdr->rclFrame.left,1440,2540);

   
   if (bmHeight==0) bmHeight=TerFont[pict].bmHeight;  // original picture height in twips
   if (bmWidth==0)  bmWidth=TerFont[pict].bmWidth;    // original picture width in twips

   if (bmHeight==0) bmHeight=TerFont[pict].PictHeight;
   if (bmWidth==0) bmWidth=TerFont[pict].PictWidth;

   // determine picture scaling
   OrigWidth=bmWidth;
   if (TerFont[pict].OrigPictWidth!=0) OrigWidth=TerFont[pict].OrigPictWidth;
   ScaleX=MulDiv(TerFont[pict].PictWidth,100,(OrigWidth-TerFont[pict].CropLeft-TerFont[pict].CropRight));
   
   OrigHeight=bmHeight;
   if (TerFont[pict].OrigPictHeight!=0) OrigHeight=TerFont[pict].OrigPictHeight;
   ScaleY=MulDiv(TerFont[pict].PictHeight,100,(OrigHeight-TerFont[pict].CropTop-TerFont[pict].CropBot));


   // wiite picture cropping information
   if (TerFont[pict].CropLeft!=0 && !WriteRtfControl(w,rtf,"piccropl",PARAM_INT,TerFont[pict].CropLeft)) return FALSE;  // write picture format
   if (TerFont[pict].CropRight!=0 && !WriteRtfControl(w,rtf,"piccropr",PARAM_INT,TerFont[pict].CropRight)) return FALSE;  // write picture format
   if (TerFont[pict].CropTop!=0 && !WriteRtfControl(w,rtf,"piccropt",PARAM_INT,TerFont[pict].CropTop)) return FALSE;  // write picture format
   if (TerFont[pict].CropBot!=0 && !WriteRtfControl(w,rtf,"piccropb",PARAM_INT,TerFont[pict].CropBot)) return FALSE;  // write picture format

   // width in HIMETRIC
   x = MulDiv(bmWidth,2540,1440);
   if (!WriteRtfControl(w,rtf,"picw",PARAM_INT,x)) return FALSE;  // write picture format
   // height in HIMETRIC
   x = MulDiv(bmHeight,2540,1440);
   if (!WriteRtfControl(w,rtf,"pich",PARAM_INT,x)) return FALSE;  // write picture format

   if (!WriteRtfControl(w,rtf,"picwgoal",PARAM_INT,OrigWidth)) return FALSE;  // write picture format
   if (!WriteRtfControl(w,rtf,"pichgoal",PARAM_INT,OrigHeight)) return FALSE;  // write picture format

   // Word97 width=picw*picscalex/100, Word6/TER: width=picwgoal*picscalex/100
   if (!WriteRtfControl(w,rtf,"picscalex",PARAM_INT,ScaleX)) return FALSE;  // write picture format
   if (!WriteRtfControl(w,rtf,"picscaley",PARAM_INT,ScaleY)) return FALSE;  // write picture format


   // write picture alignment
   if (!WriteRtfControl(w,rtf,"sspicalign",PARAM_INT,(int)(TerFont[pict].PictAlign) )) return FALSE;  // write picture format

   if (TerFlags4&TFLAG4_BINARY_RTF_PICT) {
      if (!WriteRtfControl(w,rtf,"bin",PARAM_INT,TerFont[pict].ImageSize )) return FALSE;  // write picture format
      rtf->flags|=ROFLAG_NO_CRLF;
      rtf->SpacePending=FALSE;
   }

   for (l=0;l<(long)TerFont[pict].ImageSize;l++) {
      if (TerFlags4&TFLAG4_BINARY_RTF_PICT) 
           result=PutRtfChar(w,rtf,pMem[l]);
      else result=PutRtfHexChar(w,rtf,pMem[l]);
      if (!result) break;
   }

   if (TerFlags4&TFLAG4_BINARY_RTF_PICT) ResetUintFlag(rtf->flags,ROFLAG_NO_CRLF);

   // recreate the metafile, this calls unlocks and releases the buffer
   if ( NULL==(TerFont[pict].hEnhMeta=TerSetEnhMetaFileBits(pMem)) ){
      return PrintError(w,MSG_ERR_META_RECREATE,"WriteRtfMetafile");
   }

   // end pict
   if (!EndRtfGroup(w,rtf)) return FALSE;
   // end shppict
   if (!EndRtfGroup(w,rtf)) return FALSE;


   // now, write the EnhMetaFile as a old Windows metafile
   if (WriteOldMetafileFormat) {
      OldSize=TerFont[pict].ImageSize;

      mfp.mm = MM_ANISOTROPIC;
      mfp.xExt = bmWidth;
      mfp.yExt = bmHeight;
      if (NULL==(hDC = CreateCompatibleDC(0))) return TRUE;
      if (0==(WinMetaSize = GetWinMetaFileBits(TerFont[pict].hEnhMeta, 0, 0, mfp.mm, hDC))) return TRUE;
      hGlb = GlobalAlloc(GHND, WinMetaSize);
      if (NULL==(pBuf = GlobalLock(hGlb))) return TRUE;
      WinMetaSize= GetWinMetaFileBits(TerFont[pict].hEnhMeta, WinMetaSize, pBuf, mfp.mm, hDC);
      TerFont[pict].ImageSize = WinMetaSize;
      if (NULL==(TerFont[pict].hMeta = SetMetaFileBitsEx(WinMetaSize, pBuf))) return TRUE;
   
      // begin nonshppict
      if (!BeginRtfGroup(w,rtf)) return FALSE;     // begin current picture
      if (!WriteRtfControl(w,rtf,"nonshppict",PARAM_NONE,0)) return FALSE;      // write picture group control

      TerFont[pict].PictType = PICT_METAFILE;
      WriteRtfMetafile(w, rtf, pict);

      // cleanup
      TerFont[pict].ImageSize = OldSize;
      TerFont[pict].PictType = PICT_ENHMETAFILE;
      if( TerFont[pict].hMeta ) {
          DeleteMetaFile(TerFont[pict].hMeta);
          TerFont[pict].hMeta =0;
      }
      if( hDC ) { DeleteDC(hDC); }
      if( hGlb ) {
          if( pBuf ) GlobalUnlock(hGlb);
          GlobalFree(hGlb);
      }

      // end nonshppict
      if (!EndRtfGroup(w,rtf)) return FALSE;     // begin current picture
   }

   FlushRtfLine(w,rtf);             // Create line berak for visual aid

   return result;
}

/*****************************************************************************
    WriteRtfMetafile:
    Write the metafile group and picture data for the specified picture.
******************************************************************************/
BOOL WriteRtfMetafile(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
   long l,ImageSize,bmHeight,bmWidth,limit,OrigWidth,OrigHeight;
   int ScaleX,ScaleY;
   BYTE huge *pMem;
   BOOL result=TRUE;
   
   if (TerFont[pict].hMeta==0) return true;   // picture not available

   FlushRtfLine(w,rtf);             // Create line berak for visual aid

   // get picture height/width
   if (!BeginRtfGroup(w,rtf)) return FALSE;     // begin current picture
   if (!WriteRtfControl(w,rtf,"pict",PARAM_NONE,0)) return FALSE;      // write picture group control

   // write picture height/width in HIMETRIC
   if (TerFont[pict].PictType==PICT_META7) {
      if (!WriteRtfControl(w,rtf,"wmetafile",PARAM_INT,7)) return FALSE;  // write picture format

      bmHeight=TerFont[pict].bmHeight;
      bmWidth=TerFont[pict].bmWidth;

      if (!WriteRtfControl(w,rtf,"picw",PARAM_INT,bmWidth)) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"pich",PARAM_INT,bmHeight)) return FALSE;  // write picture format
   }
   else {
      if (!WriteRtfControl(w,rtf,"wmetafile",PARAM_INT,8)) return FALSE;  // write picture format

      if (TerFont[pict].OrigPictWidth>0) {
         bmHeight=TerFont[pict].OrigPictHeight;   //TerFont[pict].bmHeight;  // picture height in twips
         bmWidth=TerFont[pict].OrigPictWidth;     //TerFont[pict].bmWidth;    // picture width in twips
      }
      else {
         bmHeight=TerFont[pict].bmHeight;  // picture height in twips
         bmWidth=TerFont[pict].bmWidth;    // picture width in twips
      } 

      
      // TE and WordXP don't use picw.  Word2000 uses it as target picture width/height
      //  TE and WordXP would use it as xExt, and yExt for the metafile, but this information is
      // already inserted into the metafile (see TerPictureFromWmf).
      limit=lMulDiv(32000,1440,2540);      // to make sure that picw/pich does not exceed 32000, a Word limit
      if (bmWidth>limit)  bmWidth=limit;    // ScaleX will supply proper adjustment for this change in value
      if (bmHeight>limit) bmHeight=limit;  // ScaleY will supply proper adjustment for this change in value
      if (!WriteRtfControl(w,rtf,"picw",PARAM_INT,(lMulDiv(bmWidth,2540,1440)))) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"pich",PARAM_INT,(lMulDiv(bmHeight,2540,1440)))) return FALSE;  // write picture format
   }

   // determine picture scaling
   OrigWidth=bmWidth;
   ScaleX=MulDiv(TerFont[pict].PictWidth,100,(OrigWidth-TerFont[pict].CropLeft-TerFont[pict].CropRight));
   
   OrigHeight=bmHeight;
   ScaleY=MulDiv(TerFont[pict].PictHeight,100,(OrigHeight-TerFont[pict].CropTop-TerFont[pict].CropBot));


   // wiite picture cropping information
   if (TerFont[pict].CropLeft!=0 && !WriteRtfControl(w,rtf,"piccropl",PARAM_INT,TerFont[pict].CropLeft)) return FALSE;  // write picture format
   if (TerFont[pict].CropRight!=0 && !WriteRtfControl(w,rtf,"piccropr",PARAM_INT,TerFont[pict].CropRight)) return FALSE;  // write picture format
   if (TerFont[pict].CropTop!=0 && !WriteRtfControl(w,rtf,"piccropt",PARAM_INT,TerFont[pict].CropTop)) return FALSE;  // write picture format
   if (TerFont[pict].CropBot!=0 && !WriteRtfControl(w,rtf,"piccropb",PARAM_INT,TerFont[pict].CropBot)) return FALSE;  // write picture format


   // write picture width
   if (TerFont[pict].PictType!=PICT_META7 && bmWidth>0) {
      // picwgoal used by Word6.0 and TER, but not by Word97
      //if (!WriteRtfControl(w,rtf,"picwgoal",PARAM_INT,bmWidth)) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"picwgoal",PARAM_INT,OrigWidth)) return FALSE;  // write picture format

      // Word97 width=picw*picscalex/100, Word6/TER: width=picwgoal*picscalex/100
      //scale=lMulDiv(TerFont[pict].PictWidth,100,bmWidth);
      if (!WriteRtfControl(w,rtf,"picscalex",PARAM_INT,ScaleX)) return FALSE;  // write picture format
   }
   else {
      if (!WriteRtfControl(w,rtf,"picwgoal",PARAM_INT,OrigWidth)) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"picscalex",PARAM_INT,ScaleX)) return FALSE;  // write picture format
   }

   // write picture height
   if (TerFont[pict].PictType!=PICT_META7 && bmHeight>0) {
      // picwgoal used by Word6.0 and TER but not by Word97
      //if (!WriteRtfControl(w,rtf,"pichgoal",PARAM_INT,bmHeight)) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"pichgoal",PARAM_INT,OrigHeight)) return FALSE;  // write picture format

      // Word97 height=pich*picscaley/100, Word6/TER: height=pichgoal*picscaley/100
      //scale=lMulDiv(TerFont[pict].PictHeight,100,bmHeight);
      if (!WriteRtfControl(w,rtf,"picscaley",PARAM_INT,ScaleY)) return FALSE;  // write picture format
   }
   else {
      if (!WriteRtfControl(w,rtf,"pichgoal",PARAM_INT,OrigHeight)) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"picscaley",PARAM_INT,ScaleY)) return FALSE;
   }


   // write picture alignment
   if (!WriteRtfControl(w,rtf,"sspicalign",PARAM_INT,(int)(TerFont[pict].PictAlign) )) return FALSE;  // write picture format


   // retrieve picture handle
   if ( NULL==(pMem=(LPVOID)TerGetMetaFileBits(TerFont[pict].hMeta,&ImageSize)) ){
     PrintError(w,MSG_ERR_META_ACCESS,"WriteRtfMetafile");
     return FALSE;
   }

   if (TerFlags4&TFLAG4_BINARY_RTF_PICT) {
      if (!WriteRtfControl(w,rtf,"bin",PARAM_INT,TerFont[pict].ImageSize )) return FALSE;  // write picture format
      rtf->flags|=ROFLAG_NO_CRLF;
      rtf->SpacePending=FALSE;
   }

   for (l=0;l<(long)ImageSize;l++) {
      if (TerFlags4&TFLAG4_BINARY_RTF_PICT) 
           result=PutRtfChar(w,rtf,pMem[l]);
      else result=PutRtfHexChar(w,rtf,pMem[l]);
      if (!result) break;
   }

   if (TerFlags4&TFLAG4_BINARY_RTF_PICT) ResetUintFlag(rtf->flags,ROFLAG_NO_CRLF);

   // recreate the metafile, this calls unlocks and releases the buffer
   if ( NULL==(TerFont[pict].hMeta=TerSetMetaFileBits((LPVOID)pMem)) ){
      return PrintError(w,MSG_ERR_META_RECREATE,"WriteRtfMetafile");
   }

   if (!EndRtfGroup(w,rtf)) return FALSE;       // end current picture
   
   FlushRtfLine(w,rtf);             // Create line berak for visual aid
   
   return result;
}

/*****************************************************************************
    WriteRtfDIB:
    Write the device independent bitmap group and picture data for the
    specified picture. When the DIB_TO_METAFILE global constant is defined
    (see ter_def.h), the function imbeds the DIB into a metafile, and
    then writes the metafile to the rtf file.
******************************************************************************/
BOOL WriteRtfDIB(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
   BOOL result=TRUE;

   if (DIB_TO_METAFILE) {
      // Convert the DIB to metafile to write to the rtf file
      struct StrFont SaveTerObject;

      SaveTerObject=TerFont[pict];
      TerFont[pict].hMeta=DIB2Metafile(w,pict);
      result=WriteRtfMetafile(w,rtf,pict);
      if (TerFont[pict].hMeta)  DeleteMetaFile(TerFont[pict].hMeta); // delete metafile
      TerFont[pict]=SaveTerObject;              // restore the object
      return result;
   }
   else {
      // Write the actual DIB to the rtf file

      long l,height,width,OrigHeight,OrigWidth;
      int ScaleX,ScaleY; 
      BYTE huge *pMem;

      FlushRtfLine(w,rtf);             // Create line berak for visual aid

      // get picture height/width
      height=TerFont[pict].PictHeight;             // picture height in twips
      width=TerFont[pict].PictWidth;               // picture width in twips

      if (!BeginRtfGroup(w,rtf)) return FALSE;     // begin current picture
      if (!WriteRtfControl(w,rtf,"pict",PARAM_NONE,0)) return FALSE;     // write picture group control

      if (!WriteRtfControl(w,rtf,"dibitmap",PARAM_INT,0)) return FALSE;  // write picture format

      // determine picture scaling
      OrigWidth=width;
      if (TerFont[pict].OrigPictWidth!=0) OrigWidth=TerFont[pict].OrigPictWidth;
      ScaleX=MulDiv(TerFont[pict].PictWidth,100,(OrigWidth-TerFont[pict].CropLeft-TerFont[pict].CropRight));
   
      OrigHeight=height;
      if (TerFont[pict].OrigPictHeight!=0) OrigHeight=TerFont[pict].OrigPictHeight;
      ScaleY=MulDiv(TerFont[pict].PictHeight,100,(OrigHeight-TerFont[pict].CropTop-TerFont[pict].CropBot));


      // wiite picture cropping information
      if (TerFont[pict].CropLeft!=0 && !WriteRtfControl(w,rtf,"piccropl",PARAM_INT,TerFont[pict].CropLeft)) return FALSE;  // write picture format
      if (TerFont[pict].CropRight!=0 && !WriteRtfControl(w,rtf,"piccropr",PARAM_INT,TerFont[pict].CropRight)) return FALSE;  // write picture format
      if (TerFont[pict].CropTop!=0 && !WriteRtfControl(w,rtf,"piccropt",PARAM_INT,TerFont[pict].CropTop)) return FALSE;  // write picture format
      if (TerFont[pict].CropBot!=0 && !WriteRtfControl(w,rtf,"piccropb",PARAM_INT,TerFont[pict].CropBot)) return FALSE;  // write picture format


      // write picture height/width in HIMETRIC
      if (!WriteRtfControl(w,rtf,"picw",PARAM_INT,TerFont[pict].bmWidth)) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"pich",PARAM_INT,TerFont[pict].bmHeight)) return FALSE;  // write picture format

      // write picture height/width in twips
      if (!WriteRtfControl(w,rtf,"picwgoal",PARAM_INT,OrigWidth)) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"pichgoal",PARAM_INT,OrigHeight)) return FALSE;  // write picture format

      // write the picture scaling
      if (!WriteRtfControl(w,rtf,"picscalex",PARAM_INT,ScaleX)) return FALSE;  // write picture format
      if (!WriteRtfControl(w,rtf,"picscaley",PARAM_INT,ScaleY)) return FALSE;  // write picture format

      // write picture alignment
      if (!WriteRtfControl(w,rtf,"sspicalign",PARAM_INT,(int)(TerFont[pict].PictAlign) )) return FALSE;  // write picture format

      if (TerFlags4&TFLAG4_BINARY_RTF_PICT) {
         if (!WriteRtfControl(w,rtf,"bin",PARAM_INT,TerFont[pict].InfoSize+TerFont[pict].ImageSize )) return FALSE;  // write picture format
         rtf->flags|=ROFLAG_NO_CRLF;
         rtf->SpacePending=FALSE;
      }

      // write the picture information
      pMem=(BYTE huge *)TerFont[pict].pInfo;

      for (l=0;l<(long)TerFont[pict].InfoSize;l++) {
         if (TerFlags4&TFLAG4_BINARY_RTF_PICT) 
              result=PutRtfChar(w,rtf,pMem[l]);
         else result=PutRtfHexChar(w,rtf,pMem[l]);
         if (!result) goto END;
      }

      // write the picture data
      pMem=(BYTE huge *)TerFont[pict].pImage;
      for (l=0;l<(long)TerFont[pict].ImageSize;l++) {
         if (TerFlags4&TFLAG4_BINARY_RTF_PICT) 
              result=PutRtfChar(w,rtf,pMem[l]);
         else result=PutRtfHexChar(w,rtf,pMem[l]);
         if (!result) goto END;
      }

      if (TerFlags4&TFLAG4_BINARY_RTF_PICT) ResetUintFlag(rtf->flags,ROFLAG_NO_CRLF);

   
      END:
      if (!EndRtfGroup(w,rtf)) return FALSE;       // end current picture
   
      FlushRtfLine(w,rtf);             // Create line berak for visual aid
   
      return result;
   }
}

/*****************************************************************************
    WriteRtfOrigImage:
    Write the device independent bitmap group and picture data for the
    specified picture. When the DIB_TO_METAFILE global constant is defined
    (see ter_def.h), the function imbeds the DIB into a metafile, and
    then writes the metafile to the rtf file.
******************************************************************************/
BOOL WriteRtfOrigImage(PTERWND w,struct StrRtfOut far *rtf,int pict,int PictType,LPBYTE pImage,DWORD ImageSize)
{   
   BOOL result=TRUE;
   long l,height,width,bmWidth,bmHeight,OrigWidth,OrigHeight;
   int ScaleX,ScaleY;

   FlushRtfLine(w,rtf);             // Create line berak for visual aid

   // get picture height/width
   height=TerFont[pict].PictHeight;             // display height in twips
   width=TerFont[pict].PictWidth;               // display width in twips
   bmWidth=ScrToTwipsX(TerFont[pict].bmWidth);   // original picture width
   bmHeight=ScrToTwipsY(TerFont[pict].bmHeight); // original picture height

   if (!BeginRtfGroup(w,rtf)) return FALSE;     // begin current picture
   if (!WriteRtfControl(w,rtf,"pict",PARAM_NONE,0)) return FALSE;     // write picture group control

   if (PictType==PICT_JPEG && !WriteRtfControl(w,rtf,"jpegblip",PARAM_INT,0)) return FALSE;  // write picture format
   if (PictType==PICT_PNG && !WriteRtfControl(w,rtf,"pngblip",PARAM_INT,0)) return FALSE;  // write picture format

   // determine picture scaling
   OrigWidth=bmWidth;
   if (TerFont[pict].OrigPictWidth!=0) OrigWidth=TerFont[pict].OrigPictWidth;
   if (OrigWidth>=32768) OrigWidth=20000;       // MSWord does not like units larger than 32768 - let scaling provide for the correct width
   ScaleX=MulDiv(TerFont[pict].PictWidth,100,(OrigWidth-TerFont[pict].CropLeft-TerFont[pict].CropRight));
   

   OrigHeight=bmHeight;
   if (TerFont[pict].OrigPictHeight!=0) OrigHeight=TerFont[pict].OrigPictHeight;
   if (OrigHeight>=32768) OrigHeight=20000;       // MSWord does not like units larger than 32768 - let scaling provide for the correct height
   ScaleY=MulDiv(TerFont[pict].PictHeight,100,(OrigHeight-TerFont[pict].CropTop-TerFont[pict].CropBot));

   // wiite picture cropping information
   if (TerFont[pict].CropLeft!=0 && !WriteRtfControl(w,rtf,"piccropl",PARAM_INT,TerFont[pict].CropLeft)) return FALSE;  // write picture format
   if (TerFont[pict].CropRight!=0 && !WriteRtfControl(w,rtf,"piccropr",PARAM_INT,TerFont[pict].CropRight)) return FALSE;  // write picture format
   if (TerFont[pict].CropTop!=0 && !WriteRtfControl(w,rtf,"piccropt",PARAM_INT,TerFont[pict].CropTop)) return FALSE;  // write picture format
   if (TerFont[pict].CropBot!=0 && !WriteRtfControl(w,rtf,"piccropb",PARAM_INT,TerFont[pict].CropBot)) return FALSE;  // write picture format


   // write picture height/width - Word ignores the following 4 tags
   if (!WriteRtfControl(w,rtf,"picw",PARAM_INT,TerFont[pict].bmWidth)) return FALSE;  // write picture format
   if (!WriteRtfControl(w,rtf,"pich",PARAM_INT,TerFont[pict].bmHeight)) return FALSE;  // write picture format
   if (!WriteRtfControl(w,rtf,"picwgoal",PARAM_INT,OrigWidth)) return FALSE;  // write original width in twips
   if (!WriteRtfControl(w,rtf,"pichgoal",PARAM_INT,OrigHeight)) return FALSE;  // write original height in twips
   
   // write the scaling factor - Word applies the scaling factor to the picture dimension embedded in the picture data to get the current size
   if (!WriteRtfControl(w,rtf,"picscalex",PARAM_INT,ScaleX)) return FALSE;
   if (!WriteRtfControl(w,rtf,"picscaley",PARAM_INT,ScaleY)) return FALSE;

   // write picture alignment
   if (!WriteRtfControl(w,rtf,"sspicalign",PARAM_INT,(int)(TerFont[pict].PictAlign) )) return FALSE;  // write picture format

   if (TerFlags4&TFLAG4_BINARY_RTF_PICT) {
      if (!WriteRtfControl(w,rtf,"bin",PARAM_INT,ImageSize )) return FALSE;  // write picture format
      rtf->flags|=ROFLAG_NO_CRLF;
      rtf->SpacePending=FALSE;
   }

   // write the picture data
   for (l=0;l<(long)ImageSize;l++) {
      if (TerFlags4&TFLAG4_BINARY_RTF_PICT) 
           result=PutRtfChar(w,rtf,pImage[l]);
      else result=PutRtfHexChar(w,rtf,pImage[l]);
      if (!result) goto END;
   }

   if (TerFlags4&TFLAG4_BINARY_RTF_PICT) ResetUintFlag(rtf->flags,ROFLAG_NO_CRLF);

   END:
   if (!EndRtfGroup(w,rtf)) return FALSE;       // end current picture
   
   FlushRtfLine(w,rtf);             // Create line berak for visual aid
   
   return result;
}

/*****************************************************************************
    WriteRtfForm:
    Write the form field control information.
******************************************************************************/
BOOL WriteRtfForm(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
    BYTE keyword[30]="";
    int id;
    struct StrForm far *pForm=(LPVOID)TerFont[pict].pInfo;

    // begin the field group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin field group
    if (!WriteRtfControl(w,rtf,"field",PARAM_NONE,0)) return FALSE;

    // begin instruction group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin instruction group
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"fldinst",PARAM_NONE,0)) return FALSE;

    // write the FORM keyword group
    if (TerFont[pict].FieldId==FIELD_CHECKBOX) lstrcpy(keyword,"FORMCHECKBOX");
    if (TerFont[pict].FieldId==FIELD_LISTBOX) lstrcpy(keyword,"FORMDROPDOWN");
    if (lstrlen(keyword)>0) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;
       if (!WriteRtfText(w,rtf,keyword,lstrlen(keyword))) return FALSE;
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end the instruction group
    }

    // begin the formfield group
    if (!BeginRtfGroup(w,rtf)) return FALSE;
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"formfield",PARAM_NONE,0)) return FALSE;

    // write the form field group
    id=0;
    if (TerFont[pict].FieldId==FIELD_CHECKBOX) id=1;
    if (TerFont[pict].FieldId==FIELD_LISTBOX)  id=2;
    if (id==0 || !WriteRtfControl(w,rtf,"fftype",PARAM_INT,id)) return FALSE;

    if (TerFont[pict].FieldId==FIELD_CHECKBOX) {
       HWND hWnd=NULL;
       if (pForm) hWnd=GetDlgItem(hTerWnd,(int)pForm->id);
       if ((BOOL)SendMessage(hWnd,BM_GETCHECK,0,0L)) {
          if (!WriteRtfControl(w,rtf,"ffres",PARAM_INT,1)) return FALSE;
       }
       if (!WriteRtfControl(w,rtf,"ffhps",PARAM_INT,(2*TerFont[pict].PictHeight)/20)) return FALSE;
    }

////////////////////////// new insert
//MAK
		if (!WriteRtfControl(w,rtf,"fftypetxt",PARAM_INT,pForm->typetxt)) return FALSE;
//////////////////////////

    // write the field name
    if (!BeginRtfGroup(w,rtf)) return FALSE;
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"ffname",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfText(w,rtf,pForm->name,lstrlen(pForm->name))) return FALSE; // write field name
    if (!EndRtfGroup(w,rtf)) return FALSE;   // end the field name group

////////////////////////// new insert
    // MAK: ffformat
    if(strlen(pForm->maskdata))
    {
      if (!BeginRtfGroup(w,rtf)) return FALSE;
      if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
      if (!WriteRtfControl(w,rtf,"ffformat",PARAM_NONE,0)) return FALSE;
      if (!WriteRtfText(w,rtf,pForm->maskdata,lstrlen(pForm->maskdata))) return FALSE; // write field name
      if (!EndRtfGroup(w,rtf)) return FALSE;   // end the ffformat group
    }
//////////////////////////


    if (!EndRtfGroup(w,rtf)) return FALSE;   // end the formfield

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the instruction group

    // write the result group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin the result group
    if (!WriteRtfControl(w,rtf,"fldrslt",PARAM_NONE,0)) return FALSE;

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the result group

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the field group

    return TRUE;
}

/*****************************************************************************
    WriteRtfFormTextField:
    Write the form text field control information. This function writes the
    contents of the fldinst group.  The opening and closing braces for
    the fldinst is provided by the calling function
******************************************************************************/
BOOL WriteRtfFormTextField(PTERWND w,struct StrRtfOut far *rtf,LPBYTE FieldCode)
{
    BYTE keyword[30]="";
    BYTE MaxLenStr[10],name[MAX_WIDTH+1];
    int id,MaxLen=0;

    if (FieldCode==NULL) return false;

    // write the FORM keyword group
    lstrcpy(keyword,"FORMTEXT");
    if (lstrlen(keyword)>0) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;
       if (!WriteRtfText(w,rtf,keyword,lstrlen(keyword))) return FALSE;
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end the instruction group
    }

    // begin the formfield group
    if (!BeginRtfGroup(w,rtf)) return FALSE;
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"formfield",PARAM_NONE,0)) return FALSE;

    // write the form field group
    id=0;
    if (!WriteRtfControl(w,rtf,"fftype",PARAM_INT,id)) return FALSE;

    GetStringField(FieldCode,0,'|',MaxLenStr);  // first field - max len
    MaxLen=atoi(MaxLenStr);
    if (MaxLen>0 && !WriteRtfControl(w,rtf,"ffmaxlen",PARAM_INT,MaxLen)) return FALSE;

    // write the field name
    if (!BeginRtfGroup(w,rtf)) return FALSE;
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"ffname",PARAM_NONE,0)) return FALSE;
    
    GetStringField(FieldCode,1,'|',name);  // second field - max len
    if (!WriteRtfText(w,rtf,name,lstrlen(name))) return FALSE; // write field name
    
    if (!EndRtfGroup(w,rtf)) return FALSE;   // end the field name group

    if (!EndRtfGroup(w,rtf)) return FALSE;   // end the formfield

    return TRUE;
}

/*****************************************************************************
    WriteRtfCtl:
    Write the embedded control information.
******************************************************************************/
BOOL WriteRtfCtl(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
   BOOL result=TRUE;
   long l,height,width;
   BYTE huge *pMem;

   // get picture height/width
   height=TerFont[pict].PictHeight;             // picture height in twips
   width=TerFont[pict].PictWidth;               // picture width in twips

   if (!BeginRtfGroup(w,rtf)) return FALSE;     // begin current picture
   if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
   if (!WriteRtfControl(w,rtf,"sscontrol",PARAM_NONE,0)) return FALSE;  // Sub Systems extension
   if (!WriteRtfControl(w,rtf,"ssctl",PARAM_NONE,0)) return FALSE;  // Sub Systems extension

   // write picture height/width in HIMETRIC
   if (!WriteRtfControl(w,rtf,"picw",PARAM_INT,TerFont[pict].bmWidth)) return FALSE;  // write picture format
   if (!WriteRtfControl(w,rtf,"pich",PARAM_INT,TerFont[pict].bmHeight)) return FALSE;  // write picture format

   // write picture height/width in twips
   if (!WriteRtfControl(w,rtf,"picwgoal",PARAM_INT,(int)width)) return FALSE;  // write picture format
   if (!WriteRtfControl(w,rtf,"pichgoal",PARAM_INT,(int)height)) return FALSE;  // write picture format

   // write picture alignment
   if (!WriteRtfControl(w,rtf,"sspicalign",PARAM_INT,(int)(TerFont[pict].PictAlign) )) return FALSE;  // write picture format

   if (TerFlags4&TFLAG4_BINARY_RTF_PICT) {
      if (!WriteRtfControl(w,rtf,"bin",PARAM_INT,TerFont[pict].InfoSize)) return FALSE;  // write picture format
      rtf->flags|=ROFLAG_NO_CRLF;
      rtf->SpacePending=FALSE;
   }

   // write the picture information
   pMem=(BYTE huge *)TerFont[pict].pInfo;
   for (l=0;l<(long)TerFont[pict].InfoSize;l++) {
      if (TerFlags4&TFLAG4_BINARY_RTF_PICT) 
           result=PutRtfChar(w,rtf,pMem[l]);
      else result=PutRtfHexChar(w,rtf,pMem[l]);
      if (!result) goto EOR;
   }

   if (TerFlags4&TFLAG4_BINARY_RTF_PICT) ResetUintFlag(rtf->flags,ROFLAG_NO_CRLF);

   EOR:

   if (!EndRtfGroup(w,rtf)) return FALSE;       // end current picture
   return result;
}

/*****************************************************************************
    WriteRtfLinkedPicture:
    Write the linked picture field.
******************************************************************************/
BOOL WriteRtfLinkedPicture(PTERWND w,struct StrRtfOut far *rtf,int pict)
{
    int i,len,idx;

    // write the picture size group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin field group
    if (!WriteRtfControl(w,rtf,"sslinkpictw",PARAM_INT,TerFont[pict].PictWidth)) return FALSE;
    if (!WriteRtfControl(w,rtf,"sslinkpicth",PARAM_INT,TerFont[pict].PictHeight)) return FALSE;

    // write the picture field group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin field group
    if (!WriteRtfControl(w,rtf,"field",PARAM_NONE,0)) return FALSE;

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin instruction group
    if (!WriteRtfControl(w,rtf,"fldinst",PARAM_NONE,0)) return FALSE;

    // write the field name
    lstrcpy(TempString,"INCLUDEPICTURE ");
    if (!WriteRtfText(w,rtf,TempString,lstrlen(TempString))) return FALSE;

    // check if picture file needs to be written out without path
    idx=0;
    if (True(TerFlags6&TFLAG6_DONT_WRITE_PICT_PATH)) {
       len=strlen(TerFont[pict].LinkFile);
       if (len>0) {
          for (i=(len-1);i>=0;i--) {
             if (TerFont[pict].LinkFile[i]=='\\' || TerFont[pict].LinkFile[i]==':') break;
          }
          idx=i+1; 
       }
    }

    // convert one slash into 4 slashes in the file name
    AddSlashes(w,&(TerFont[pict].LinkFile[idx]),TempString,4);
    if (NULL!=strstr(TempString," ")) StrQuote(TempString);

    lstrcat(TempString," \\\\d");  // add the \\d switch

    rtf->flags|=ROFLAG_IGNORE_SLASH;  // do not tread backslash as a special character for the next write
    if (!WriteRtfText(w,rtf,TempString,lstrlen(TempString))) return FALSE;  // copy the file pathname
    rtf->flags=ResetUintFlag(rtf->flags,ROFLAG_IGNORE_SLASH);

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the instruction group

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin the result group
    if (!WriteRtfControl(w,rtf,"fldrslt",PARAM_NONE,0)) return FALSE;
    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the result group
    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the field group
    if (!EndRtfGroup(w,rtf)) return FALSE;        // end the picture size group

    return TRUE;
}

/*****************************************************************************
    WriteRtfControl:
    Write the rtf control text to rtf output device.  The control text should
    be NULL terminated.  The next argument provide control parameter type.
******************************************************************************/
BOOL WriteRtfControl(PTERWND w,struct StrRtfOut far *rtf,LPBYTE control,int type, double val)
{
    BYTE string[20];

    rtf->SpacePending=FALSE;      // no need to write the space after the last control

    rtf->WritingControl=TRUE;     // beginning of a control word

    //write the control prefix
    if (!PutRtfChar(w,rtf,'\\')) return FALSE; // write prefix

    //write the control text
    if (!WriteRtfText(w,rtf,control,lstrlen(control))) return FALSE;

    //write the numeric parameter
    if (type==PARAM_INT) {         // write in integer format
       wsprintf(string,"%ld",(long)val);
       if (!WriteRtfText(w,rtf,string,lstrlen(string))) return FALSE;
    }
    else if (type==PARAM_DBL) {    // write in double format
       wsprintf(string,"%f",val);
       if (!WriteRtfText(w,rtf,string,lstrlen(string))) return FALSE;
    }

    // write the space delimiter
    rtf->SpacePending=TRUE;      // this space will be written out only if next word is not a control

    rtf->WritingControl=FALSE;   // end of a control word

    return TRUE;

}

/*****************************************************************************
    WriteRtfTag:
    Write the character tags such as bookmark.
******************************************************************************/
BOOL WriteRtfTag(PTERWND w,struct StrRtfOut far *rtf,int CurTag)
{
    if (CurTag<=0 || CurTag>=TotalCharTags || !CharTag[CurTag].InUse) return TRUE;

    // write the bookmark tags
    while (CurTag) {
       if (CurTag<=0 || CurTag>=TotalCharTags || !CharTag[CurTag].InUse) return TRUE;
       if (CharTag[CurTag].type==TERTAG_BKM) {
          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin bkmkstart destinationn group
          if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
          if (!WriteRtfControl(w,rtf,"bkmkstart",PARAM_NONE,0)) return FALSE;
          if (!WriteRtfText(w,rtf,CharTag[CurTag].name,lstrlen(CharTag[CurTag].name))) return FALSE;
          if (!EndRtfGroup(w,rtf)) return FALSE;        // end shpinst group

          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin bkmkend destinationn group
          if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
          if (!WriteRtfControl(w,rtf,"bkmkend",PARAM_NONE,0)) return FALSE;
          if (!WriteRtfText(w,rtf,CharTag[CurTag].name,lstrlen(CharTag[CurTag].name))) return FALSE;
          if (!EndRtfGroup(w,rtf)) return FALSE;        // end shpinst group
       }
       else if (CharTag[CurTag].type==TERTAG_USER) {
          LPBYTE AuxText;

          if (!BeginRtfGroup(w,rtf)) return false;      // begin bkmkstart destinationn group
          if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return false;
          if (!WriteRtfControl(w,rtf,"sstag",PARAM_NONE,0)) return false;
          if (!WriteRtfText(w,rtf,CharTag[CurTag].name,lstrlen(CharTag[CurTag].name))) return false;
           
          if (!WriteRtfControl(w,rtf,"sstagint",PARAM_INT,CharTag[CurTag].AuxInt)) return false;
           
          AuxText=CharTag[CurTag].AuxText;
          if (AuxText!=null && lstrlen(AuxText)>0 && !WriteRtfText(w,rtf,AuxText,lstrlen(AuxText))) return false;

          if (!EndRtfGroup(w,rtf)) return false;        // end shpinst group
       }
       CurTag=CharTag[CurTag].next;
    }

    return TRUE;
}

/*****************************************************************************
    WriteRtfMbcsText:
    Write the mbcs rtf text to output.
******************************************************************************/
BOOL WriteRtfMbcsText(PTERWND w,struct StrRtfOut far *rtf,LPBYTE text,LPBYTE lead, WORD UcBase, int TextLen, int CurFont)
{
    LPBYTE pMb;
    int len,i;

    if (UcBase) {                     // unicode text
       if (mbcs && !WriteRtfControl(w,rtf,"loch",PARAM_NONE,0)) return FALSE;
       for (i=0;i<TextLen;i++) {
          WORD uchar=(WORD)MakeUnicode(w,UcBase,text[i]);
          if (uchar>255) {
            int CharSet=(CurFont>=0?TerFont[CurFont].CharSet:0);
            int CodePage,count;
            BYTE chr[10];
            if (CharSet==0 || CharSet==1) CodePage=GetACP();
            else                          IsMbcsCharSet(w,CharSet,&CodePage);  // get the code page
            count=WideCharToMultiByte(CodePage,0x400/*WC_NO_BEST_FIT_CHARS*/,&uchar,1,chr,10,NULL,NULL);
            if (count==1) {
               chr[1]=0;
               if (True(TerFlags6&TFLAG6_WRITE_DBCS) && !WriteRtfControl(w,rtf,"uc",PARAM_INT,1)) return FALSE;
               if (!WriteRtfControl(w,rtf,"u",PARAM_INT,uchar)) return FALSE;
               if (!PutRtfSpecChar(w,rtf,chr[0])) return FALSE;  // to help rtf readers which do not process unicode
            }
            else if (count==2 && True(TerFlags6&TFLAG6_WRITE_DBCS)) {
               if (!WriteRtfControl(w,rtf,"uc",PARAM_INT,2)) return FALSE;
               if (!WriteRtfControl(w,rtf,"u",PARAM_INT,uchar)) return FALSE;
               if (!PutRtfSpecChar(w,rtf,chr[0])) return FALSE;  // high byte
               if (!PutRtfSpecChar(w,rtf,chr[1])) return FALSE;  // low byte
            }
            else  {
               if (True(TerFlags6&TFLAG6_WRITE_DBCS) && !WriteRtfControl(w,rtf,"uc",PARAM_INT,1)) return FALSE;
               if (!WriteRtfControl(w,rtf,"u",PARAM_INT,uchar)) return FALSE;
               if (!WriteRtfText(w,rtf,"?",1)) return FALSE;
            }
          }
          else {
             if (True(TerFlags6&TFLAG6_WRITE_DBCS) && !WriteRtfControl(w,rtf,"uc",PARAM_INT,1)) return FALSE;
             if (!WriteRtfControl(w,rtf,"u",PARAM_INT,uchar)) return FALSE;
             if (!PutRtfSpecChar(w,rtf,(BYTE) uchar)) return FALSE;  // to help rtf readers which do not process unicode
          }
       }
       return TRUE;
    } 
    
    if (!mbcs) return WriteRtfText(w,rtf,text,TextLen);
    if (!rtf->IsDbcs || lead==NULL) {
       if (!WriteRtfControl(w,rtf,"loch",PARAM_NONE,0)) return FALSE;
       return WriteRtfText(w,rtf,text,TextLen);
    }

    // write a dbcs string
    pMb=OurAlloc(2*TextLen+2);
    len=TerMakeMbcs(hTerWnd,pMb,text,lead,TextLen);

    if (!WriteRtfControl(w,rtf,"dbch",PARAM_NONE,0)) return FALSE;

    // write the text in the hex format
    for (i=0;i<len;i++) if (!PutRtfSpecChar(w,rtf,pMb[i])) return FALSE;

    if (!WriteRtfControl(w,rtf,"loch",PARAM_NONE,0)) return FALSE;

    OurFree(pMb);

    return TRUE;
}

/*****************************************************************************
    WriteRtfText:
    Write the rtf text to output.  This routine scans the text stream. If
    any special character are found, they are preceede with a '\' character.
******************************************************************************/
BOOL WriteRtfText(PTERWND w,struct StrRtfOut far *rtf,LPBYTE text,int TextLen)
{
    int i;
    BYTE CurChar;
    BYTE DblQuote=34;
    BOOL IgnoreSlash=rtf->flags&ROFLAG_IGNORE_SLASH;

    if (rtf->flags&ROFLAG_IN_FIELD_TC && !(rtf->WritingControl)) IgnoreSlash=TRUE;

    if (TextLen==0) return TRUE;

    for (i=0;i<TextLen;i++) {
        CurChar=text[i];
        if (CurChar>=0x80 || CurChar==0xd || CurChar==0xa || CurChar==HIDDEN_CHAR || (InIE &&  CurChar==DblQuote)) {   // write in the hex format
           if (!PutRtfSpecChar(w,rtf,CurChar)) return FALSE;
           continue;
        }

        if ((CurChar=='\\' && !IgnoreSlash) || CurChar=='{' || CurChar=='}'
             || CurChar==NBSPACE_CHAR || CurChar==NBDASH_CHAR
             || CurChar==HYPH_CHAR) {
            if (!PutRtfChar(w,rtf,'\\')) return FALSE; // write prefix
            rtf->flags|=ROFLAG_NO_FLUSH; // keep the second character next to it
        }
        if (CurChar==TAB) {
            if (!WriteRtfControl(w,rtf,"tab",PARAM_NONE,0)) return FALSE;
        }
        else if (CurChar==ZWNJ_CHAR) {
            if (!WriteRtfControl(w,rtf,"zwnj",PARAM_NONE,0)) return FALSE;
        }
        else if (CurChar==NBSPACE_CHAR) {
            if (!PutRtfChar(w,rtf,'~')) return FALSE; // write non-break char suffix
        }
        else if (CurChar==NBDASH_CHAR) {
            if (!PutRtfChar(w,rtf,'_')) return FALSE; // write non-break dash suffix
        }
        else if (CurChar==HYPH_CHAR) {
            if (!PutRtfChar(w,rtf,'-')) return FALSE; // write non-break dash suffix
        }
        else if (CurChar==PAGE_CHAR) {
            if (!WriteRtfControl(w,rtf,"page",PARAM_NONE,0)) return FALSE;
        }
        else if (CurChar==COL_CHAR) {
            if (!WriteRtfControl(w,rtf,"column",PARAM_NONE,0)) return FALSE;
        }
        else if (CurChar==HPARA_CHAR) {
            if (!WriteRtfControl(w,rtf,"par",PARAM_NONE,0)) return FALSE; // hidden paragraph
        }
        else if (!PutRtfChar(w,rtf,CurChar)) return FALSE;  // write character

        rtf->flags=ResetUintFlag(rtf->flags,ROFLAG_NO_FLUSH);  // reset no flush flag
    }
    return TRUE;
}

/*****************************************************************************
    BeginRtfGroup:
    Write the rtf 'begin group' character to output
******************************************************************************/
BOOL BeginRtfGroup(PTERWND w,struct StrRtfOut far *rtf)
{
    rtf->SpacePending=FALSE;          // delimit the last control by '{' character
    if ((rtf->GroupLevel+1)<MAX_RTF_GROUPS) {
       rtf->GroupLevel++;
       FarMove(&(rtf->group[rtf->GroupLevel-1]),&(rtf->group[rtf->GroupLevel]),sizeof(struct StrRtfOutGroup));
    }  
    return PutRtfChar(w,rtf,'{');
}

/*****************************************************************************
    EndRtfGroup:
    Write the rtf 'end group' character to output
******************************************************************************/
BOOL EndRtfGroup(PTERWND w,struct StrRtfOut far *rtf)
{
    rtf->SpacePending=FALSE;          // delimit the last control by '}' character
    if (rtf->GroupLevel>0) rtf->GroupLevel--;
    return PutRtfChar(w,rtf,'}');
}

/*****************************************************************************
    PutRtfSpecChar:
    Write a special character using \'xx representation
******************************************************************************/
BOOL PutRtfSpecChar(PTERWND w,struct StrRtfOut far *rtf,BYTE CurChar)
{
    if (!PutRtfChar(w,rtf,'\\')) return FALSE;
    rtf->flags|=ROFLAG_NO_FLUSH;
    if (!PutRtfChar(w,rtf,'\'')) return FALSE;
    if (!PutRtfHexChar(w,rtf,CurChar))  return FALSE;
    rtf->flags=ResetUintFlag(rtf->flags,ROFLAG_NO_FLUSH);

    return TRUE;
}

/*****************************************************************************
    PutRtfHexChar:
    Write a character to rtf output in hex format
******************************************************************************/
BOOL PutRtfHexChar(PTERWND w,struct StrRtfOut far *rtf,BYTE CurChar)
{
    BYTE HiChar,LoChar,HexChar;
    int SaveFlag;

    HiChar=(char)((CurChar&0xF0)>>4);
    LoChar=(char)(CurChar&0x0F);

    // write hi char
    if (HiChar<=9) HexChar=(char)('0'+HiChar);
    else           HexChar=(char)('a'+HiChar-10);

    if (!PutRtfChar(w,rtf,HexChar)) return FALSE;

    // write lo char
    if (LoChar<=9) HexChar=(char)('0'+LoChar);
    else           HexChar=(char)('a'+LoChar-10);

    SaveFlag=rtf->WritingControl;
    rtf->WritingControl=TRUE;      // disable line break

    if (!PutRtfChar(w,rtf,HexChar)) return FALSE;

    rtf->WritingControl=SaveFlag;  // restore the flag status

    return TRUE;
}

/*****************************************************************************
    PutRtfChar:
    Write a character to rtf output
******************************************************************************/
BOOL PutRtfChar(PTERWND w,struct StrRtfOut far *rtf,BYTE CurChar)
{
    BOOL IgnoreSlash=rtf->flags&ROFLAG_IGNORE_SLASH;

    if (rtf->SpacePending) {
       rtf->text[rtf->TextLen]=' ';       // append the pending space
       rtf->TextLen++;
       rtf->SpacePending=FALSE;
    }

    // check for line break
    if (!rtf->WritingControl && !(rtf->flags&ROFLAG_NO_FLUSH)) {
       if (rtf->TextLen>MAX_WIDTH/4 && rtf->text[rtf->TextLen-1]==' ' && !FlushRtfLine(w,rtf)) return FALSE;  // try not be break at a control word
       if (rtf->TextLen>MAX_WIDTH/3 && !FlushRtfLine(w,rtf)) return FALSE;  // try not be break at a control word
    }
    if (CurChar=='\\' && !IgnoreSlash && rtf->TextLen>MAX_WIDTH/2)  if (!FlushRtfLine(w,rtf)) return FALSE;  // flush the current line
    if ((rtf->TextLen+2)>MAX_WIDTH)  if (!FlushRtfLine(w,rtf)) return FALSE;  // flush the current line

    rtf->text[rtf->TextLen]=CurChar;      // store current character
    rtf->TextLen++;

    return TRUE;
}

/*****************************************************************************
    FlustRtfLine:
    Write the current line to the RTF file or output buffer
******************************************************************************/
BOOL FlushRtfLine(PTERWND w,struct StrRtfOut far *rtf)
{
    int BytesWritten;

    if (rtf->TextLen==0) return TRUE;    // nothing to flush

    // add cr/lf to the line
    if (!(rtf->flags&ROFLAG_NO_CRLF)) {
      rtf->text[rtf->TextLen]=0xd;
      rtf->TextLen++;
      rtf->text[rtf->TextLen]=0xa;
      rtf->TextLen++;
    }

    if (rtf->hFile!=INVALID_HANDLE_VALUE) {    // write to file
       if (!WriteFile(rtf->hFile,rtf->text,rtf->TextLen,&((DWORD)BytesWritten),NULL) 
           || BytesWritten!=rtf->TextLen) {
          return PrintError(w,MSG_ERR_FILE_WRITE,"FlushRtfLine");
       }
    }
    else {                   // write to buffer
       if (rtf->BufIndex+rtf->TextLen>rtf->BufLen) { // expand the buffer
          rtf->BufLen=rtf->BufLen + (rtf->BufLen/4); // expand 25 percent
          if (rtf->BufLen<(rtf->BufIndex+rtf->TextLen)) rtf->BufLen=rtf->BufIndex+rtf->TextLen;

          GlobalUnlock(rtf->hBuf);
          if ( NULL==(rtf->hBuf=GlobalReAlloc(rtf->hBuf,rtf->BufLen+1, GMEM_MOVEABLE))
            || NULL==(rtf->buf=(BYTE huge *)GlobalLock(rtf->hBuf)) ){
             return PrintError(w,MSG_OUT_OF_MEM,"FlushRtfLine");
          }
       }
       HugeMove(rtf->text,&(rtf->buf[rtf->BufIndex]),rtf->TextLen);
       rtf->BufIndex=rtf->BufIndex+rtf->TextLen;
    }

    rtf->TextLen=0;                                 // reset buffer
    rtf->SpacePending=FALSE;                        // line already delimited

    return TRUE;
}

