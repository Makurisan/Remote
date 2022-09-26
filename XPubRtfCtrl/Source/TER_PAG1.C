/*==============================================================================
   TER_PAG1.C
   TER Page mode display functions.

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
   CreateEndnote:
   Create endnotes
*******************************************************************************/
CreateEndnote(PTERWND w)
{
    long l,StartLine,BegLine,count,FirstSectLine,LastSectLine;
    int i,len,CurPara,SkipPara,FirstCol,LastCol,EndnotePara=-1,StartCol;
    int SaveModified=TerArg.modified;
    LPWORD fmt;
    LPBYTE ptr;
    LPWORD pCt;
    WORD OldFmt,NewFmt;
    BOOL AdjustCurLine,AdjustHilightBeg,AdjustHilightEnd,LastEndnote,TerminatePara;
    BOOL FirstEndnote;

    dm("CreateEndnote");

    // check if ENOTETEXT style is in use
    for (i=0;i<TotalFonts;i++) if (TerFont[i].InUse && TerFont[i].style&ENOTETEXT) break;
    if (i==TotalFonts) return TRUE;          // endnote not used


    // remove the existing Endnote (generated) lines
    SkipPara=-1;
    for (l=0;l<TotalLines;l++) {
       CurPara=pfmt(l);
       if (CurPara==SkipPara) continue;  // non-endnote line

       if (PfmtId[CurPara].pflags&PFLAG_ENDNOTE) {
          MoveLineArrays(w,l,1,'D');

          if (CurLine>l) CurLine--;
          if (HilightType!=HILIGHT_OFF && HilightBegRow>l) HilightBegRow--;
          if (HilightType!=HILIGHT_OFF && HilightEndRow>l) HilightEndRow--;

          l--;                             // to check the current line again
       }
       else SkipPara=CurPara; 
    } 

  

    // start looking for too
    StartLine=0;
    if (!EndnoteAtSect) StartLine=TotalLines-1;  // endnote at the end of the document

    FirstSectLine=0;
    LastEndnote=FALSE;

    NEXT_SECT:

    // locate next section break
    for (l=StartLine;l<TotalLines;l++) if (LineFlags(l)&LFLAG_SECT) break;     // look for section break line
    if (l==TotalLines) {
       LastEndnote=TRUE;
       l--;
    } 
    
    BegLine=l;
    LastSectLine=BegLine;   // The second break line (or the last line) might have end notes too
    
    FirstEndnote=TRUE;     // flags first endnote of the section

    // check if CurLine and hilight positions need to be adjusted
    AdjustCurLine=(CurLine>BegLine);
    AdjustHilightBeg=(HilightType!=HILIGHT_OFF && HilightBegRow>BegLine);
    AdjustHilightEnd=(HilightType!=HILIGHT_OFF && HilightEndRow>BegLine);


    for (l=FirstSectLine;l<=LastSectLine;l++) {
       if (!(LineFlags2(l)&LFLAG2_ENOTETEXT)) {
          if (TerOpFlags2&TOFLAG2_SSE_READ) {     // check line font
             fmt=OpenCfmt(w,l);
             for (i=0;i<LineLen(l);i++) {
                if (TerFont[fmt[i]].style&ENOTETEXT) {
                   LineFlags2(l)|=LFLAG2_ENOTETEXT;
                   break;
                }
             }
             CloseCfmt(w,l);
          } 
          if (!(LineFlags2(l)&LFLAG2_ENOTETEXT)) continue;
       }
       
       if (LineLen(l)==0) continue;

       count=0;                   // number of lines addeed
       
       // ensure that the section break is on its own line
       if (FirstEndnote) {
          if (EndnotePara<0) EndnotePara=TerCreateParaId(hTerWnd,-1,TRUE,360,0,-360,0,0,0,0,PFLAG_ENDNOTE,0,0,0,0);
          count+=BeginEndnote(w,LastEndnote,&BegLine,EndnotePara);
          FirstEndnote=FALSE;
       }

       StartCol=0;                // start scanning from the first column

       while (StartCol<LineLen(l)) {
          // find the first and the last column to extract
          fmt=OpenCfmt(w,l);
          FirstCol=-1;
          for (i=StartCol;i<LineLen(l);i++) {
             if (FirstCol==-1 && TerFont[fmt[i]].style&ENOTETEXT) {
                FirstCol=i;
             }
             if (FirstCol>=0 && !(TerFont[fmt[i]].style&ENOTETEXT)) break;
             LastCol=i;
          }
          StartCol=i;                      // start column for the next endnote on the same line

          CloseCfmt(w,l);
          if (FirstCol<0) continue;

          // copy this line to the endnote area
          if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
          MoveLineArrays(w,BegLine,1,'B');
          count++;

          // adjust curline and hilight positions
          if (AdjustCurLine)    CurLine+=count;       // adjust the line position
          if (AdjustHilightBeg) HilightBegRow+=count;
          if (AdjustHilightEnd) HilightEndRow+=count;

       
          // check if the current endnote para being build will need to be terminated
          TerminatePara=TRUE;
          if ((LastCol+1)==LineLen(l) && (l+1)<TotalLines) {
             int NextLineFont=GetCurCfmt(w,l+1,0);
             if ((TerFont[NextLineFont].style&ENOTETEXT)) TerminatePara=FALSE;
          }  
       
          len=LastCol-FirstCol+1;
          if (TerminatePara) len++;

          LineAlloc(w,BegLine,0,len);
          MoveCharInfo(w,l,FirstCol,BegLine,0,(LastCol-FirstCol+1));
          pfmt(BegLine)=EndnotePara;
          cid(BegLine)=0;
          fid(BegLine)=0;
          len=LineLen(BegLine);
          LineFlags(BegLine)=0;
          LineFlags2(BegLine)=0;

          if (TerminatePara && len>1) {
             ptr=pText(BegLine);
             fmt=OpenCfmt(w,BegLine);

             ptr[len-1]=ParaChar;
             fmt[len-1]=fmt[len-2];
          } 


          // check the last character for any control chracter
          len=LineLen(BegLine);
          if (len>0 && LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK)) {
             BYTE LastChar;
             ptr=pText(BegLine);
             LastChar=ptr[len-1];
             if (LastChar==CellChar || lstrchr(BreakChars,LastChar)!=NULL) ptr[len-1]=ParaChar;  // replace break character by para char
          } 
       
          // translate any tab characters to spaces
          ptr=pText(BegLine);
          len=LineLen(BegLine);
          for (i=0;i<len;i++) if (ptr[i]==TAB) ptr[i]=' ';
          for (i=1;i<LineLen(BegLine);i++) {   // remove repettive white spaces
             if (ptr[i-1]==' ' && ptr[i]==' ') {
                MoveLineData(w,BegLine,i,1,'D');      // delete additiona white space
                ptr=pText(BegLine);
                i--;                                  // check this position again
             } 
          }
          len=LineLen(BegLine); 


          // delete any hidden characters
          for (i=0;i<LineLen(BegLine);i++) {
             int CurFont=GetCurCfmt(w,BegLine,i);
             if (True(TerFont[CurFont].style&HIDDEN) || (TerFont[CurFont].FieldId==FIELD_NAME)) {
                MoveLineData(w,BegLine,i,1,'D');
                i--;             // this position deleted
             }
          }
       
          //reset any tags in the copied line
          if (pCtid(BegLine)) {
             pCt=pCtid(BegLine);
             for (i=0;i<len;i++) pCt[i]=0;
             CloseCtid(w,BegLine);
          }

          // reset the FNOTEALL styles in the font
          fmt=OpenCfmt(w,BegLine);
          OldFmt=0xFFFF;
          for (i=0;i<len;i++) {
            if (fmt[i]!=OldFmt) {
              OldFmt=fmt[i];
              NewFmt=SetFontStyle(w,OldFmt,FNOTEALL,FALSE);  // reset the FNOTEALL bit
              NewFmt=SetFontStyle(w,NewFmt,PROTECT,TRUE);    // protect style
            
              if (TerFont[NewFmt].ParaStyId!=0 || TerFont[NewFmt].CharStyId!=1) 
                  NewFmt=SetFontStyleId(w,NewFmt,1,0);
            }
            fmt[i]=NewFmt;
          }


          // set LFLAG_PARA if applicable
          ptr=pText(BegLine);
          len=LineLen(BegLine);
          if (len>0 && ptr[len-1]==ParaChar) LineFlags(BegLine)|=LFLAG_PARA;

          BegLine++;
       }
    } 


    if (!LastEndnote) {
       StartLine=FirstSectLine=(BegLine+1);   // skipover the section break line
       goto NEXT_SECT;
    }


    //END:


    // adjust the line positions
    if (CurLine>=TotalLines) CurLine=TotalLines-1;
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
    if (CurCol<0) CurCol=0;


    if (HilightType!=HILIGHT_OFF) {
       if (HilightBegRow>=TotalLines) HilightBegRow=TotalLines-1;
       if (HilightBegCol>=LineLen(HilightBegRow)) HilightBegCol=LineLen(HilightBegRow)-1;
       if (HilightBegCol<0) HilightBegCol=0;

       if (HilightEndRow>=TotalLines) HilightEndRow=TotalLines-1;
       if (HilightEndCol>LineLen(HilightEndRow)) HilightEndCol=LineLen(HilightEndRow);
    }


    TerArg.modified=SaveModified;   // restore the modification status

    return TRUE;
}

/******************************************************************************
   BeginEndnote:
   This function inserts the separation line for the endnotes.  This function
   returns the number of lines added
*******************************************************************************/
long BeginEndnote(PTERWND w, BOOL LastEndnote, LPLONG pBegLine, int EndnotePara)
{
     long BegLine=(*pBegLine);
     long count=0;
     LPBYTE ptr;
     LPWORD fmt;
     int i,len,SepLineLen=25;

     if (LineLen(BegLine)>1) {
        if (LastEndnote) {    // no section break
           if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
             
           MoveLineArrays(w,BegLine,1,'A');
           count++;
           BegLine++;
             
           LineAlloc(w,BegLine,LineLen(BegLine),1);
           ptr=pText(BegLine);
           ptr[0]=ParaChar;
           LineFlags(BegLine)|=LFLAG_PARA;
        }
        else {
           // remove section break from the current line
           len=LineLen(BegLine)-1;
           LineAlloc(w,BegLine,len,len-1);   // this stips the section mark
           ResetLongFlag(LineFlags(BegLine),LFLAG_SECT);
           if (tabw(BegLine)) ResetUintFlag(tabw(BegLine)->type,INFO_SECT);

           // ensure that the previous line is delimeted by a para marker
           ptr=pText(BegLine);
           len=LineLen(BegLine);
           if (len==0 || (len>0 && ptr[len-1]!=ParaChar)) {
              LineAlloc(w,BegLine,len,len+1);
              ptr=pText(BegLine);
              ptr[len]=ParaChar;
              LineFlags(BegLine)|=LFLAG_PARA;
           } 

           // now place the section break on its own line
           if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
             
           MoveLineArrays(w,BegLine,1,'A');
           count++;
           BegLine++;
             
           LineAlloc(w,BegLine,LineLen(BegLine),2);     // space for paramark and section
           ptr=pText(BegLine);
           ptr[0]=ParaChar;
           ptr[1]=SECT_CHAR;
           LineFlags(BegLine)|=(LFLAG_PARA|LFLAG_SECT);
        }  
     }

     // insert a blank line
     if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
             
     MoveLineArrays(w,BegLine,1,'B');
     count++;
     
     pfmt(BegLine)=EndnotePara;        
     LineAlloc(w,BegLine,LineLen(BegLine),1);
     ptr=pText(BegLine);
     ptr[0]=ParaChar;
     LineFlags(BegLine)=LFLAG_PARA;
     LineFlags2(BegLine)=0;

     BegLine++;

     // insert a separator line
     if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
             
     MoveLineArrays(w,BegLine,1,'B');
     count++;
             
     pfmt(BegLine)=EndnotePara;        
     if (IsDefLangRtl(w)) {       // display the status line right-justified
        int CurPara=pfmt(BegLine);
        if (PfmtId[CurPara].flow!=FLOW_RTL) {    // right-justify the line dislay
           pfmt(BegLine)=SetParaParam(w,CurPara,PPARAM_FLOW,FLOW_RTL);
        }
     }
       
     LineAlloc(w,BegLine,LineLen(BegLine),SepLineLen);
     ptr=pText(BegLine);
     for (i=0;i<(SepLineLen-1);i++) ptr[i]='_';
     ptr[SepLineLen-1]=ParaChar;
     LineFlags(BegLine)=LFLAG_PARA;
     LineFlags2(BegLine)=0;

     // apply the protect font
     fmt=OpenCfmt(w,BegLine);
     for (i=0;i<SepLineLen;i++) fmt[i]=SetFontStyle(w,0,PROTECT,TRUE);
     CloseCfmt(w,BegLine);

     BegLine++;    // endnote inserted before this line

     (*pBegLine)=BegLine;
 
     return count;
}
 
/******************************************************************************
   CreateToc:
   Create table of contents
*******************************************************************************/
CreateToc(PTERWND w)
{
    long l,StartLine,BegLine,EndLine,count;
    LPWORD pCt;
    int i,j,len,CurPara,CurStyle,idx,PageStrLen,PrevLen,HeadingTag;
    int SaveModified=TerArg.modified,PageNo,TocTabId=0;
    int NextHeadingTag,NextTocTag,TocStyle,PrevStyId,PageStrPos;
    BYTE HeadingLevel;
    LPWORD fmt;
    LPBYTE ptr;
    WORD OldFmt,NewFmt;
    struct StrPfmt para;
    BYTE PageStr[10],string[MAX_WIDTH+1],FirstStyle,LastStyle,FirstStyleHdng,LastStyleHdng;
    BOOL AdjustCurLine,AdjustHilightBeg,AdjustHilightEnd,ParaEnd;
    BOOL UseHeadingStyles,UseTC;
    BOOL IsHeading,IsTC,IsHiddenLine,TclFound;
    BYTE TocFmt[MAX_WIDTH+1];  // toc format
    int  CurFont;

    dm("CreateToc");

    if (TerFlags4&TFLAG4_NO_TOC_UPDATE) return TRUE;

    // remove the existing TOC and HEADING tags
    for (l=0;l<TotalLines;l++) {
       if (!pCtid(l)) continue;
       pCt=pCtid(l);
       len=LineLen(l);
       for (i=0;i<len;i++) {
          if (pCt[i]) {
             DeleteTag(w,l,i,TERTAG_TOC,NULL);
             if (pCtid(l)) DeleteTag(w,l,i,TERTAG_HEADING,NULL);
             if (!pCtid(l)) break;     // no tags left on this line
          } 
       } 
    } 

  
    // start looking for too
    DocHasToc=MultipleToc=FALSE;
    NextTocTag=0;
    StartLine=0;

    NEXT_TOC:

    NextHeadingTag=0;

    // locate next toc 
    for (l=StartLine;l<TotalLines;l++) if (LineFlags(l)&LFLAG_TOC) break;
    if (l==TotalLines) goto END;
    BegLine=l;
    
    // get the toc format string
    TocFmt[0]=0;
    FirstStyle=FirstStyleHdng='1';      // first heading style
    LastStyle=LastStyleHdng='9';       // last heading style
    UseHeadingStyles=UseTC=FALSE;
    fmt=OpenCfmt(w,BegLine);
    for (i=0;i<LineLen(BegLine);i++) {
       WORD CurFont=fmt[i];
       if (TerFont[CurFont].FieldId==FIELD_TOC) {
          if (TerFont[CurFont].FieldCode) {
             lstrcpy(TocFmt,TerFont[CurFont].FieldCode);

             // look for the applicable heading style ids
             for (j=1;j<lstrlen(TocFmt);j++) {
                if (TocFmt[j-1]=='\\') {
                   if (TocFmt[j]=='o') UseHeadingStyles=TRUE;
                   if (TocFmt[j]=='f') UseTC=TRUE;
                }
                if (TocFmt[j]=='-' && UseHeadingStyles) {
                  FirstStyle=TocFmt[j-1];
                  LastStyle=TocFmt[j+1];
                } 
             }
             if (FirstStyle<'1') FirstStyle='1';
             if (LastStyle>'9') LastStyle='9';
             if (FirstStyle>LastStyle) FirstStyle=LastStyle;
             if (UseHeadingStyles) {
                 FirstStyleHdng=FirstStyle;
                 LastStyleHdng=LastStyle;
             } 

             // check for the "Heading" specification
             strlwr(TocFmt);
             lstrcpy(string,"heading ");
             if ((ptr=strstr(TocFmt,string))!=null) {
                ptr=&(ptr[strlen(string)]);
                FirstStyleHdng=ptr[0];
                LastStyleHdng=ptr[2];   // there is a comma in-between
                if (FirstStyleHdng<'1') FirstStyleHdng='1';
                if (LastStyleHdng>'9') LastStyleHdng='9';
                if (FirstStyleHdng>LastStyleHdng) FirstStyleHdng=LastStyleHdng;
             }
             
             lstrcpy(TocFmt,TerFont[CurFont].FieldCode);  // Restore TocFmt

          } 
          break;
       } 
    } 

    if (!UseTC) UseHeadingStyles=TRUE;

    for (l=BegLine;l<TotalLines;l++) {
       if (!(LineFlags(l)&LFLAG_TOC)) {
          if (LineLen(l)==0) continue;
          break;
       }
       if (True(LineFlags(l)&LFLAG_SECT)) break;
    }
    EndLine=l-1;
    if (EndLine<BegLine) {
       StartLine=BegLine+1;
       goto NEXT_TOC;
    } 
    
    count=EndLine-BegLine+1;     // number of lines to delete
    MoveLineArrays(w,BegLine,count,'D');  //delete the exiting toc lines

    // check if CurLinea and hilight positions need to be adjusted
    AdjustCurLine=(CurLine>EndLine);
    AdjustHilightBeg=(HilightType!=HILIGHT_OFF && HilightBegRow>EndLine);
    AdjustHilightEnd=(HilightType!=HILIGHT_OFF && HilightEndRow>EndLine);
    if (AdjustCurLine)    CurLine-=count;
    if (AdjustHilightBeg) HilightBegRow-=count;
    if (AdjustHilightEnd) HilightEndRow-=count;


    if (!DocHasToc) {
       DocHasToc=TRUE;
       FirstTocPos=RowColToAbs(w,BegLine,0);
    }
    else MultipleToc=TRUE;                       // document has multiple toc

    // scan the document for the headings
    ParaEnd=FALSE;

    for (l=0;l<TotalLines;l++) {
       
       if (LineLen(l)==0 || (LineLen(l)==1 && LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK) && (LineFlags(l)&LFLAG_PARA_FIRST))) continue;  // ignore blank lines
       if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;  // don't get the lines from header/footer

       // check if it is a blank line
       if (LineLen(l)>1 && (LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK) && (LineFlags(l)&LFLAG_PARA_FIRST)) && !mbcs) {
          len=LineLen(l)-1;
          ptr=pText(l);
          for (i=0;i<len;i++) if (ptr[i]!=' ' && ptr[i]!=TAB) break;
          if (i==len) continue;   // ignore blank lines
      } 
       

       // skip to the end of the paragraph?
       if (ParaEnd) {
          if (LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK)) ParaEnd=FALSE;
          continue;
       } 

       // check if the line has nothing but hidden characters
       IsHiddenLine=FALSE;
       if (l==0 || True(LineFlags(l-1)&(LFLAG_PARA|LFLAG_BREAK))) {
          fmt=OpenCfmt(w,l);
          len=0;
          for (i=0;i<LineLen(l);i++) if (TerFont[fmt[i]].style&HIDDEN) len++;
          CloseCfmt(w,l);
          if (LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK) && len>0) len++;   // consider parachar only if atleast one hidden character found
          if (len>=LineLen(l)) IsHiddenLine=TRUE;
       }

       if (LineFlags(l)&LFLAG_TOC) continue;
       
       CurPara=pfmt(l);
       CurStyle=PfmtId[CurPara].StyId;
       if (CurStyle==0) continue;
       
       IsHeading=IsTC=FALSE;

       if (CurStyle && UseHeadingStyles) {
          IsHeading=TRUE;
          
          len=lstrlen("heading ");
          lstrcpy(string,StyleId[CurStyle].name);
          string[len]=0;
          if (lstrcmpi(string,"heading ")!=0) IsHeading=FALSE;
          
          //if (strstr(StyleId[CurStyle].name,"heading ")!=StyleId[CurStyle].name) IsHeading=FALSE;  // not a heading style
          
          if (IsHeading) {
             len=lstrlen("heading ");
             if (lstrlen(StyleId[CurStyle].name)>(len+1)) {
                BYTE LastChar=StyleId[CurStyle].name[len+1];
                if (LastChar!=' ' && LastChar!=',') IsHeading=FALSE;  // only 1 to 9 supported
             }
             if (IsHeading) HeadingLevel=StyleId[CurStyle].name[len];
          }
          if (!IsHeading) {                // check the outilne level
             if (StyleId[CurStyle].OutlineLevel>=0) {
                IsHeading=true;
                HeadingLevel=(char)(StyleId[CurStyle].OutlineLevel+(int)'1');   // 1 based
             } 
          } 

          if (IsHeading) {
             if ( (HeadingLevel<FirstStyle || HeadingLevel>LastStyle)
                &&(HeadingLevel<FirstStyleHdng || HeadingLevel>LastStyleHdng)) IsHeading=false;
          }
          if (IsHeading && IsHiddenLine) IsHeading=FALSE;
       }
       if (!IsHeading && UseTC && LineFlags2(l)&LFLAG2_TC) IsTC=TRUE;
       if (!IsHeading && !IsTC) continue;

       // copy this line to the toc
       if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
       MoveLineArrays(w,BegLine,1,'B');

       if (l>=BegLine) l++;                    // this line scrolled down
       if (AdjustCurLine)    CurLine++;       // adjust the line position
       if (AdjustHilightBeg) HilightBegRow++;
       if (AdjustHilightEnd) HilightEndRow++;
       
       if (IsHeading) {
          int PrevFont,NewFont;

          LineAlloc(w,BegLine,0,LineLen(l));
          MoveCharInfo(w,l,0,BegLine,0,LineLen(l));
          
          pfmt(BegLine)=pfmt(l);
          cid(BegLine)=cid(BegLine+1);
          fid(BegLine)=fid(BegLine+1);
          len=LineLen(BegLine);

          // check if the line includes a hidden paragraph marker
          if (LineFlags(l)&LFLAG_HPARA) {
             ptr=pText(l);
             for (i=0;i<len;i++) if (ptr[i]==HPARA_CHAR) break;
             if (i<len) {
                LineAlloc(w,BegLine,LineLen(l),i+1);  // truncate the line
                len=LineLen(BegLine);
                ptr=pText(BegLine);
                ptr[len-1]=ParaChar;
                ParaEnd=TRUE;                        // skip rest of the paragraph
             }
          } 

          // check if this line includes a section break
          if (LineFlags(l)&LFLAG_SECT) {
             len=LineLen(BegLine);
             ptr=pText(BegLine);
             if (len>0 && ptr[len-1]==SECT_CHAR) {
                if (len==1 || ptr[len-2]!=ParaChar) ptr[len-1]=ParaChar;   // replace SectChar with ParaChar
                else LineAlloc(w,BegLine,len,len-1);   // SectChar follows ParaChar - discard SectChar
             } 
          }

          // check the last character for any control chracter
          len=LineLen(BegLine);
          if (len>0 && LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK)) {
             BYTE LastChar;
             ptr=pText(BegLine);
             LastChar=ptr[len-1];
             if (LastChar==CellChar || lstrchr(BreakChars,LastChar)!=NULL) ptr[len-1]=ParaChar;  // replace break character by para char

             // reset any hidden style for the paragraph
             fmt=OpenCfmt(w,BegLine);
             CurFont=fmt[len-1];
             if (True(TerFont[CurFont].style&HIDDEN)) {
                 if (len>=2) fmt[len-1]=fmt[len-2];
                 else        fmt[len-1]=SetFontStyle(w,fmt[len-1],HIDDEN,FALSE);
             }
             CloseCfmt(w,BegLine);
          }

          // apply default point-size
          fmt=OpenCfmt(w,BegLine);
          CurFont=-1;
          for (i=0;i<LineLen(BegLine);i++) {
             PrevFont=CurFont;
             CurFont=fmt[i];
             if (i==0 || PrevFont!=CurFont) {
                NewFont=CurFont;
                NewFont=GetNewPointSize(w,(WORD)NewFont,(DWORD)(TerArg.PointSize*20),0L,BegLine,0);
                fmt[i]=NewFont;
             }
             else fmt[i]=fmt[i-1];
          }
          
       }
       else if (IsTC) {     // extract TC from the line
          int FirstCol=-1,LastCol=-1,PrevFont,CurFont,NewFont;
          
          fmt=OpenCfmt(w,l);
          ptr=pText(l);
          TclFound=ParaEnd=FALSE;
          HeadingLevel='1';
          for (i=0;i<LineLen(l);i++) {
             if (FirstCol==-1 && TerFont[fmt[i]].FieldId==FIELD_TC) FirstCol=i;
             if (FirstCol!=-1) {
                if (!TclFound && FirstCol==i) {
                   LPBYTE code=TerFont[fmt[i]].FieldCode;
                   if (code!=null && lstrlen(code)>=4) TclFound=(code[0]=='\\' && code[1]=='t' && code[2]=='c' && code[3]=='l');
                   if (TclFound && lstrlen(code)>=5) HeadingLevel=code[4];
                }
                LastCol=i;
                if (TerFont[fmt[i]].FieldId!=FIELD_TC) {
                  LastCol=i-1;
                  break;
                }
             }
          }
          if (FirstCol==-1) continue;  // tc field not found
          if (LastCol==-1) LastCol=LineLen(l)-1;

          // move the tc text to TOC
          LineAlloc(w,BegLine,0,LastCol-FirstCol+1);
          MoveCharInfo(w,l,FirstCol,BegLine,0,LineLen(BegLine));
          pfmt(BegLine)=0;
          cid(BegLine)=cid(BegLine+1);
          fid(BegLine)=fid(BegLine+1);
          len=LineLen(BegLine);


          // turn-off hidden style and TC field
          fmt=OpenCfmt(w,BegLine);
          CurFont=-1;
          for (i=0;i<LineLen(BegLine);i++) {
             PrevFont=CurFont;
             CurFont=fmt[i];
             if (i==0 || PrevFont!=CurFont) {
                NewFont=CurFont;
                if (TerFont[NewFont].style&HIDDEN) NewFont=SetFontStyle(w,NewFont,HIDDEN,FALSE);  // reset the hidden bit
                NewFont=SetFontFieldId(w,NewFont,0,NULL);
                NewFont=GetNewPointSize(w,(WORD)NewFont,(DWORD)(TerArg.PointSize*20),0L,BegLine,0);
                fmt[i]=NewFont;
             }
             else fmt[i]=fmt[i-1];
          }

          // check if the line ends
          if (ParaEnd) {                    // terminate with para end character
             len=LineLen(BegLine);
             LineAlloc(w,BegLine,len,len+1);
             ptr=pText(BegLine);
             fmt=OpenCfmt(w,BegLine);
             ptr[len]=ParaChar;
             fmt[len]=fmt[len-1];

             LineFlags(BegLine)|=LFLAG_PARA;
          } 
       }
       
       // translate any tab characters to spaces
       ptr=pText(BegLine);
       len=LineLen(BegLine);
       for (i=0;i<len;i++) if (ptr[i]==TAB) ptr[i]=' ';
       for (i=1;i<LineLen(BegLine);i++) {   // remove repettive white spaces
          if (ptr[i-1]==' ' && ptr[i]==' ') {
             MoveLineData(w,BegLine,i,1,'D');      // delete additiona white space
             ptr=pText(BegLine);
             i--;                                  // check this position again
          } 
       }
       len=LineLen(BegLine); 

       // delete any hidden characters
       for (i=0;i<LineLen(BegLine);i++) {
          int CurFont=GetCurCfmt(w,BegLine,i);
          if (True(TerFont[CurFont].style&(HIDDEN|FNOTEALL)) || (TerFont[CurFont].FieldId==FIELD_NAME)) {
             MoveLineData(w,BegLine,i,1,'D');
             i--;             // this position deleted
          }
       }
       len=LineLen(BegLine); 
       
       //reset any tags in the copied line
       if (pCtid(BegLine)) {
          pCt=pCtid(BegLine);
          for (i=0;i<len;i++) pCt[i]=0;
          CloseCtid(w,BegLine);
       }

       // copy list numbering information
       if (IsHeading && tabw(l) && tabw(l)->ListText && (LineFlags(l)&LFLAG_LIST)) {
          BYTE ListText[100];
          int count=lstrlen(tabw(l)->ListText);
          lstrcpy(ListText,tabw(l)->ListText);
          if (count>0 && ListText[count-1]!=' ') {      // assume tab separator for this list bullet
             lstrcat(ListText,"    ");
             count=lstrlen(ListText);
          } 
          MoveLineData(w,BegLine,0,count,'B');   // insert this bullet here

          ptr=pText(BegLine);
          fmt=OpenCfmt(w,BegLine);
          for (i=0;i<count;i++) {
             ptr[i]=ListText[i];
             fmt[i]=fmt[count];  // tabw(l)->ListFontId;
          } 
       } 

       // set the field id for the copied line
       len=LineLen(BegLine); 
       fmt=OpenCfmt(w,BegLine);
       OldFmt=0xFFFF;
       for (i=0;i<len;i++) {
         if (fmt[i]!=OldFmt) {
           OldFmt=fmt[i];
           if (TerFont[OldFmt].style&PICT) TerFont[OldFmt].FieldId=FIELD_TOC;
           else {
              NewFmt=(int)GetNewFieldId(w,OldFmt,(DWORD)FIELD_TOC
                       ,((lstrlen(TocFmt)==0)?(DWORD)0L:(DWORD)TocFmt),BegLine,i);  // get the toc font id
              if (TerFont[NewFmt].CharStyId!=1) NewFmt=GetNewCharStyle(w,NewFmt,1,0,BegLine,0);  // reset any character style id
           }
         }
         fmt[i]=NewFmt;
       }

       // get toc style
       TocStyle=GetTocStyle(w,HeadingLevel);
       if (IsHeading) PrevStyId=PfmtId[pfmt(l)].StyId;
       else           PrevStyId=0;   // normal style
       
       FarMove(&(PfmtId[pfmt(l)]),&para,sizeof(para));   // copy the previous information
       para.pflags=0;
       SetParaStyleId(w,&para,&(StyleId[para.StyId]),&(StyleId[TocStyle]),TRUE);

       // set the tab id
       if (para.TabId==0) {
          if (TocTabId==0) {    // create tab id to offset the page number
             struct StrTab tab;
             int width;


             FarMemSet(&tab,0,sizeof(struct StrTab));
             tab.count=1;
             width=TerWrapWidth(w,BegLine,-1);
             width=ScrToTwipsX(width)-90;
             tab.pos[0]=width;
             tab.type[0]=TocTabAlign;
             tab.flags[0]=TocTabLeader;

             TocTabId=TerCreateTabId(hTerWnd,&tab);
          }
          para.TabId=TocTabId; 
       } 

       // set the para id
       pfmt(BegLine)=NewParaId(w,pfmt(l),
                         para.LeftIndentTwips,
                         para.RightIndentTwips,
                         para.FirstIndentTwips,
                         para.TabId,
                         para.BltId,
                         para.AuxId,
                         para.Aux1Id,
                         TocStyle,
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
       
       ApplyLineTextStyle(w,BegLine,TocStyle,PrevStyId);

       // set line flags
       LineFlags(BegLine)|=LFLAG_TOC;
       if (LineFlags(l)&(LFLAG_PARA|LFLAG_SECT) || ParaEnd) LineFlags(BegLine)|=LFLAG_PARA;

       // do processing for paragraph end line
       if (LineFlags(BegLine)&LFLAG_PARA && LineLen(l)>0 && TocShowPageNo) {
          PageNo=LinePage(l);

          // place the heading tag on the heading line
          len=LineLen(l);
          if (MultipleToc) HeadingTag=GetTag(w,l,len-1,TERTAG_HEADING,NULL,NULL,NULL);
          else {
             wsprintf(string,"Heading%d",NextHeadingTag);
             NextHeadingTag++;
             HeadingTag=SetTag(w,l,len-1,TERTAG_HEADING,string,NULL,PageNo);
          }
          
          // append the page number string
          PageNo=PageInfo[PageNo].DispNbr;
          PageStr[0]=TAB;
          idx=1;
          wsprintf(&(PageStr[idx]),"%d",PageNo);
          PageStrLen=lstrlen(PageStr);

          PrevLen=LineLen(BegLine);
          PageStrPos=PrevLen-1;       // before the last paragraph marker
          MoveLineData(w,BegLine,PageStrPos,PageStrLen,'B');

          ptr=pText(BegLine);
          for (i=0;i<PageStrLen;i++) ptr[PageStrPos+i]=PageStr[i];

          // place the toc tag at the first page number character of the toc line
          wsprintf(string,"Toc%d",NextHeadingTag);
          HeadingTag=SetTag(w,BegLine,PageStrPos+idx,TERTAG_TOC,string,NULL,HeadingTag);
       }
               
       BegLine++;
    } 

    StartLine=BegLine;   // to start looking for next toc
    goto NEXT_TOC;

    END:

    // adjust the line positions
    if (CurLine>=TotalLines) CurLine=TotalLines-1;
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
    if (CurCol<0) CurCol=0;

    if (HilightType!=HILIGHT_OFF) {
       if (HilightBegRow>=TotalLines) HilightBegRow=TotalLines-1;
       if (HilightBegCol>=LineLen(HilightBegRow)) HilightBegCol=LineLen(HilightBegRow)-1;
       if (HilightBegCol<0) HilightBegCol=0;

       if (HilightEndRow>=TotalLines) HilightEndRow=TotalLines-1;
       if (HilightEndCol>LineLen(HilightEndRow)) HilightEndCol=LineLen(HilightEndRow);
    }

    TerArg.modified=SaveModified;   // restore the modification status

    return TRUE;
}

/******************************************************************************
   UpdateToc:
   Update page numbers for the table of contents
*******************************************************************************/
UpdateToc(PTERWND w)
{
    long l,StartLine,BegLine,EndLine;
    int i,len,CurPara,CurStyle,HeadingTag,PageNo;
    int SaveModified=TerArg.modified;
    int PageStrPos;
    BYTE suffix;
    BYTE PageStr[10],string[MAX_WIDTH+1];
    int StartCol,TocTag,OldLen;

    dm("UpdateToc");

    DocHasHeadings=FALSE;

    // update the page number for the heading lines
    for (l=0;l<TotalLines;l++) {
       if (LineFlags(l)&LFLAG_TOC) continue;
       if (!(LineFlags(l)&LFLAG_PARA)) continue;

       CurPara=pfmt(l);
       CurStyle=PfmtId[CurPara].StyId;
       if (CurStyle==0) continue;
       
       len=lstrlen("heading ");
       lstrcpy(string,StyleId[CurStyle].name);
       string[len]=0;
       if (lstrcmpi(string,"heading ")!=0) continue;
       //if (strstr(StyleId[CurStyle].name,"heading ")!=StyleId[CurStyle].name) continue;  // not a heading style
       
       if (lstrlen(StyleId[CurStyle].name)>(len+1)) continue;  // only 1 to 9 supported
       suffix=StyleId[CurStyle].name[len];
       if (suffix<'1' || suffix>'9') continue;
       DocHasHeadings=TRUE;

       // DocHasHeadings flag set, now exit if doc does not have toc or toc page number is not to be displayed
       if (!DocHasToc || !TocShowPageNo) return TRUE;

       // get the tag
       HeadingTag=GetTag(w,l,LineLen(l)-1,TERTAG_HEADING,NULL,NULL,NULL);
       if (HeadingTag<=0 || HeadingTag>=TotalCharTags) continue;

       CharTag[HeadingTag].AuxInt=LinePage(l);   // store the page number
    } 


    AbsToRowCol(w,FirstTocPos,&StartLine,&StartCol);

    NEXT_TOC:

    // locate next toc 
    for (l=StartLine;l<TotalLines;l++) if (LineFlags(l)&LFLAG_TOC) break;
    if (l==TotalLines) goto END;
    BegLine=l;
    for (;l<TotalLines;l++) if (!(LineFlags(l)&LFLAG_TOC)) break;
    EndLine=l-1;

    for (l=BegLine;l<=EndLine;l++) {
       if (!(LineFlags(l)&LFLAG_PARA)) continue;   // not a page number line
       if (!pCtid(l)) continue;

       // search for page number string position
       len=LineLen(l);
       for (i=0;i<len;i++) {
          if (0!=(TocTag=GetTag(w,l,i,TERTAG_TOC,NULL,NULL,NULL))) break;
       }
       if (i==len) continue; 
       PageStrPos=i;
       OldLen=len-PageStrPos-1;   // the last character is the aragraph marker character
    
       //get the new page number
       HeadingTag=(int)CharTag[TocTag].AuxInt;
       if (HeadingTag<=0 || HeadingTag>=TotalCharTags) continue;
       PageNo=(int)CharTag[HeadingTag].AuxInt;

       // set the page number 
       wsprintf(PageStr,"%d",PageInfo[PageNo].DispNbr);
       ReplaceTextInPlace(w,&l,&PageStrPos,OldLen,PageStr,NULL);
    } 

    if (MultipleToc) {
      StartLine=l;   // to start looking for next toc
      goto NEXT_TOC;
    }
 
    END:
    // adjust the line positions
    if (CurLine>=TotalLines) CurLine=TotalLines-1;
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
    if (CurCol<0) CurCol=0;

    if (HilightType!=HILIGHT_OFF) {
       if (HilightBegRow>=TotalLines) HilightBegRow=TotalLines-1;
       if (HilightBegCol>=LineLen(HilightBegRow)) HilightBegCol=LineLen(HilightBegRow)-1;
       if (HilightBegCol<0) HilightBegCol=0;

       if (HilightEndRow>=TotalLines) HilightEndRow=TotalLines-1;
       if (HilightEndCol>LineLen(HilightEndRow)) HilightEndCol=LineLen(HilightEndRow);
    }

    TerArg.modified=SaveModified;   // restore the modification status

    return TRUE;
} 

/******************************************************************************
   GetTocStyle:
   Get toc stle id
*******************************************************************************/
GetTocStyle(PTERWND w, BYTE level)
{
   BYTE name[20]="toc ";
   int i,CurSty;

   dm("GetTocStyle");

   AddChar(name,level);     // style name

   // check if the style exists
   for (i=0;i<TotalSID;i++) {
      if (StyleId[i].InUse && StyleId[i].type==SSTYPE_PARA && lstrcmpi(StyleId[i].name,name)==0) break;
   } 

   if (i<TotalSID) return i;

   CurSty=GetStyleIdSlot(w);
   if (CurSty<0) return 0;

   FarMemSet(&(StyleId[CurSty]),0,sizeof(struct StrStyleId));

   StyleId[CurSty].InUse=TRUE;
   StyleId[CurSty].type=SSTYPE_PARA;
   lstrcpy(StyleId[CurSty].name,name);
   lstrcpy(StyleId[CurSty].TypeFace,TerFont[0].TypeFace);
   StyleId[CurSty].FontFamily=TerFont[0].FontFamily;
   StyleId[CurSty].TwipsSize=TerFont[0].TwipsSize;
   StyleId[CurSty].style=0;
   StyleId[CurSty].TextColor=TerFont[0].TextColor;
   StyleId[CurSty].TextBkColor=TerFont[0].TextBkColor;
   
   // set the paragraph properties
   StyleId[CurSty].LeftIndentTwips=200*(level-'1');
   StyleId[CurSty].ParaBkColor=TextDefBkColor;
   StyleId[CurSty].TabId=0;

   StyleId[CurSty].flags|=SSFLAG_ADD;   // additive character style

   return CurSty;
}
 
/******************************************************************************
   AdjustTableRowWidth:
   Ensure that the subtable row is fully contained inside the parent cell.
*******************************************************************************/
AdjustTableRowWidth(PTERWND w, int row)
{
   int ParentCell,ParentWidth,FirstCell,width,indent,cl;
   
   dm("AdjustTableRowWidth");

   FirstCell=TableRow[row].FirstCell;
   ParentCell=cell[FirstCell].ParentCell;
   if (ParentCell<=0) return TRUE;
   
   ParentWidth=cell[ParentCell].width-2*cell[ParentCell].margin;

   // find the width of the subtable row
   width=indent=0;
   for (cl=FirstCell;cl>0;cl=cell[cl].NextCell) width+=cell[cl].width;
   if (!(TableRow[row].flags&(RIGHT_JUSTIFY|CENTER))) indent=TableRow[row].indent;

   if ((width+indent)>ParentWidth) indent=ParentWidth-width;
   if (indent>=0) {                       // just use the lesser indent to contain the subtable row within the parent cell
      TableRow[row].indent=indent;
      return TRUE;
   } 
   TableRow[row].indent=0;                // reduce the indent to 0

   // prorate the cell width 
   for (cl=FirstCell;cl>0;cl=cell[cl].NextCell) {
      cell[cl].width=MulDiv(cell[cl].width,ParentWidth,width);
      if (cell[cl].width<=(2*cell[cl].margin)) cell[cl].width=2*cell[cl].margin+10;  // add a tolerance amount
   }

   return TRUE;
}    

/******************************************************************************
   PgmUp(w)
   process up arrow in page mode
*******************************************************************************/
PgmUp(PTERWND w)
{
   int i,FrameNo,LocateY,NearestY,LastY,TextAngle;
   long ScrFirstLine;
   BOOL MatchFound=FALSE,SkipCurFrame,FramesRefreshed=FALSE;
   int AdjX=0;

   if (CurLine<=0) return TRUE;

   TOP:

   if ((FrameNo=GetFrame(w,CurLine))<0) return TRUE;

   CursDirection=CURS_BACKWARD;          // cursor moving backward
   HilightAtCurPos=TRUE;               // lock hilight at current position

   if (CursHorzPos<0) CursHorzPos=ColToUnits(w,CurCol,CurLine,LEFT);
   
   TextAngle=LineTextAngle(w,-FrameNo);
   
   if (CursHorzPos>=(frame[FrameNo].x+frame[FrameNo].width)) {// this can happen when positioned at the end of a right-justified line
      int x=frame[FrameNo].x+frame[FrameNo].width-1;  // this can happen when positioned at the end of a right-justified line
      AdjX=CursHorzPos-x;
      CursHorzPos=x;
   }

   if (CurLine>frame[FrameNo].PageFirstLine) {
      LocateY=LineToUnits(w,CurLine)-1;
      LocateY-=GetObjSpcBef(w,CurLine,TRUE);
      CurLineY=LocateY;
      if (TextAngle>0) goto LINE_FOUND;
   }
   else {
      LocateY=frame[FrameNo].y-1; // locate this y position
      if (TextAngle>0) return TRUE;  // can't move any further
   }

   SkipCurFrame=(CurLine<=frame[FrameNo].PageFirstLine);

   // locate the line at LocateY position
   NearestY=-1;
   for (i=TotalFrames-1;i>=0;i--) {
      if (frame[i].empty || frame[i].flags&FRAME_DISABLED) continue;
      if (TerFlags3&TFLAG3_CURSOR_IN_CELL && frame[i].CellId!=cid(CurLine)) continue;

      if (SkipCurFrame && FrameNo==i) continue;
      if (CursHorzPos>=frame[i].x && CursHorzPos<(frame[i].x+frame[i].width)) {
         LastY=frame[i].y+frame[i].height-1;
         if (LocateY>=frame[i].y && LocateY<=LastY) MatchFound=TRUE;
         if (LastY<=LocateY && LastY>NearestY) NearestY=LastY;
      }
   }


   if (!MatchFound && NearestY<0) {
      if (CurPage==FirstFramePage && !FramesRefreshed && CurPage>0) { // refresh the frames so that the previous page is included in the frame
          CreateFrames(w,FALSE,CurPage-1,CurPage);                  // create the text frames
          FramesRefreshed=TRUE;
          goto TOP;
      }
      else return PgmPageUp(w);     // position at the previous page
   }

   if (!MatchFound) LocateY=NearestY;                      // use the nearest Y

   LINE_FOUND:
   TerOpFlags|=TOFLAG_NO_TOL;               // no lolerance for frame location
   CurLine=UnitsToLine2(w,CursHorzPos,LocateY,(TextAngle>0?FrameNo:-1));
   ResetLongFlag(TerOpFlags,TOFLAG_NO_TOL);

   if (CurPage>FirstFramePage && LocateY<FirstPageHeight) CurPage--;  // adjust the page number

   FrameNo=GetFrame(w,CurLine);

   // determine the scope of printing
   ScrFirstLine=frame[FrameNo].ScrFirstLine;
   if (frame[FrameNo].ScrY<TerWinOrgY) ScrFirstLine++;
   if (tabw(CurLine) && tabw(CurLine)->type&INFO_ROW) CurLine--;
   if (frame[FrameNo].flags&FRAME_ON_SCREEN && CurLine>=ScrFirstLine) {
      PaintFlag=PAINT_MIN;
      WrapFlag=WRAP_OFF;
   }
   else {
      if (TerFlags3&TFLAG3_LINE_SCROLL) CurLineY=0;
      else                              CurLineY=TerWinHeight/2; // position at middle of page
      
      TerWinOrgY=LineToUnits(w,CurLine)-CurLineY;
      if (TerWinOrgY<0) TerWinOrgY=0;
      SetTerWindowOrg(w);               // set the logical window origin
      
      PaintFlag=PAINT_WIN;
      UseTextMap=FALSE;
   }

   CurCol=UnitsToCol(w,CursHorzPos+AdjX,CurLine);

   // adjust cur pos when stretch hilightling
   if (HilightType==HILIGHT_CHAR && StretchHilight && cid(CurLine)) {
      long SaveLine=CurLine;
      for (;(CurLine-1)>=0;CurLine--) if (LineInfo(w,CurLine-1,INFO_CELL|INFO_ROW) || !cid(CurLine-1)) break;
      if (SaveLine!=CurLine) CurCol=0;
   }

   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   PgmDown(w)
   process down arrow in page mode
*******************************************************************************/
PgmDown(PTERWND w)
{
   int i,TextAngle,FrameNo,NextY,LocateY,NearestY,NewOrgY,FirstLineY;
   long PageLastLine,ScrLastLine,FirstLine;
   BOOL MatchFound=FALSE,SkipCurFrame,FramesRefreshed=FALSE;
   int AdjX=0;

   // adjust cursor to the end of any hightlighted line
   if (!StretchHilight && !DraggingText) {           // highlighting not in progress
      if (HilightType==HILIGHT_CHAR && HilightEndRow==(CurLine-1) && HilightEndCol==LineLen(HilightEndRow)) {
         CurLine--;
         CurCol=LineLen(CurLine)-1;
      } 
      TerSetCharHilight(w);                       // turnoff any highlighting
   }

   TOP:
   
   // cursor restricted to the current cell
   if ((FrameNo=GetFrame(w,CurLine))<0) return TRUE;

   CursDirection=CURS_FORWARD;         // cursor moving forward
   HilightAtCurPos=TRUE;               // lock hilight at current position

   if (CursHorzPos<0) CursHorzPos=ColToUnits(w,CurCol,CurLine,LEFT);
   if (CursHorzPos>=(frame[FrameNo].x+frame[FrameNo].width)) {// this can happen when positioned at the end of a right-justified line
      int x=frame[FrameNo].x+frame[FrameNo].width-1;  // this can happen when positioned at the end of a right-justified line
      AdjX=CursHorzPos-x;
      CursHorzPos=x;
   }

   // find the last text line of the frame
   PageLastLine=frame[FrameNo].PageLastLine;
   if (tabw(PageLastLine) && tabw(PageLastLine)->type&INFO_ROW) PageLastLine--;

   TextAngle=LineTextAngle(w,-FrameNo);

   if (CurLine<PageLastLine) {
      LocateY=LineToUnits(w,CurLine)+ScrLineHeight(w,CurLine,FALSE,FALSE)+1;
      if (TextAngle>0) goto LINE_FOUND;
   }
   else {
      if (TextAngle>0) return TRUE;  // don't exit the frame
      LocateY=frame[FrameNo].y+frame[FrameNo].height+1; // locate this y position
   }

   SkipCurFrame=(CurLine>=PageLastLine);

   // locate the line at LocateY position
   NearestY=CurPageHeight; // find the nearest frame if exact frame not found
   for (i=0;i<TotalFrames;i++) {
      if (frame[i].empty || frame[i].flags&FRAME_DISABLED) continue;
      if (TerFlags3&TFLAG3_CURSOR_IN_CELL && frame[i].CellId!=cid(CurLine)) continue;

      if (SkipCurFrame && FrameNo==i) continue;

      if (CursHorzPos>=frame[i].x && CursHorzPos<(frame[i].x+frame[i].width)) {
        if ( LocateY>=frame[i].y && LocateY<(frame[i].y+frame[i].height)) MatchFound=TRUE;
        if (frame[i].y>LocateY && frame[i].y<NearestY) NearestY=frame[i].y;
      }
   }

   if (!MatchFound && NearestY==CurPageHeight) {
      if ((CurLine+1)==TotalLines) {
         if (HilightType==HILIGHT_CHAR) {
            HilightEndCol=CurCol=LineLen(CurLine)-1;
            PaintTer(w);
         }
         return TRUE;
      }
      if (CurPage==LastFramePage && !FramesRefreshed) {         // refresh the frames so that current page and the next page
          CreateFrames(w,FALSE,CurPage,CurPage+1);              // create the text frames
          FramesRefreshed=TRUE;
          goto TOP;
      }
      else return PgmPageDn(w);  // position at the next page
   }


   if (!MatchFound) LocateY=NearestY;          // use the nearest y

   LINE_FOUND:
   TerOpFlags|=TOFLAG_NO_TOL;               // no lolerance for frame location
   CurLine=UnitsToLine2(w,CursHorzPos,LocateY,(TextAngle>0?FrameNo:-1));
   ResetLongFlag(TerOpFlags,TOFLAG_NO_TOL);

   if (CurPage==FirstFramePage && LocateY>=FirstPageHeight) CurPage++;  // adjust the page number

   FrameNo=GetFrame(w,CurLine);

   // decide the scope of painting
   ScrLastLine=frame[FrameNo].ScrLastLine;
   if (CurLine==ScrLastLine) {
      NextY=GetRowY(w,CurLine)+ScrLineHeight(w,CurLine,TRUE,FALSE);
      if (NextY>(TerWinOrgY+TerWinHeight)) ScrLastLine--;
   }

   if (frame[FrameNo].flags&FRAME_ON_SCREEN && CurLine<=ScrLastLine
     && ((TerWinOrgY+TerWinHeight)<CurPageHeight || LastFramePage==(TotalPages-1)) ){
      PaintFlag=PAINT_MIN;
      WrapFlag=WRAP_OFF;
   }
   else {
      if (TerFlags3&TFLAG3_LINE_SCROLL)  CurLineY=TerWinHeight-ScrLineHt(CurLine);
      else                               CurLineY=TerWinHeight/2;         // try position at middle of page
      
      TerWinOrgY=LineToUnits(w,CurLine)-CurLineY;
      if (TerWinOrgY<0) TerWinOrgY=0;
      SetTerWindowOrg(w);               // set the logical window origin
      
      NewOrgY=TerWinOrgY;
      FirstLine=UnitsToLine(w,CursHorzPos,NewOrgY);  // try to show the previous line completly
      if (FirstLine==(CurLine-1)) {
         FirstLineY=LineToUnits(w,FirstLine);
         if (FirstLineY<NewOrgY) { // previous line not complete in view
            int CurLineHt=ScrLineHeight(w,CurLine,FALSE,FALSE);
            CurLineY+=(NewOrgY-FirstLineY);  // show previous line completely
            if ((CurLineY+CurLineHt)>=TerWinHeight) CurLineY=TerWinHeight-CurLineHt-1;  // make sure current line is completely visible
            if (CurLineY<TerWinHeight/2) CurLineY=TerWinHeight/2;
         }
      }
      PaintFlag=PAINT_WIN;
      UseTextMap=FALSE;
   }

   CurCol=UnitsToCol(w,CursHorzPos+AdjX,CurLine);


   // adjust cur pos when stretch hilightling
   if (HilightType==HILIGHT_CHAR && StretchHilight && cid(CurLine)) {
      for (;(CurLine+1)<TotalLines;CurLine++) if (LineInfo(w,CurLine,INFO_CELL)) break;
      CurCol=LineLen(CurLine)-1;
   }


   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   PgmRight:
   process the right arrow in page mode
*******************************************************************************/
PgmRight(PTERWND w,BOOL HilightBegins)
{
   BYTE LastChar;
   LPBYTE ptr;
   BOOL InHdrFtr=FALSE;
   long line;

   CursDirection=CURS_FORWARD;          // cursor moving backward

   if (HilightType==HILIGHT_CHAR && cid(HilightEndRow)>0 && cid(CurLine)>0) return TblHilightRight(w,HilightBegins);

   if (CurCol==LineWidth-1) return TRUE;
   CurCol++;

   if (LineLen(CurLine)>0) {
       ptr=pText(CurLine);
       LastChar=ptr[LineLen(CurLine)-1];

   }

   if (CurCol>=LineLen(CurLine) &&
      ((CurLine+1)<TotalLines || LastChar==ParaChar)) {
       if ((CurLine+1)==TotalLines) {   // can't go any further
          CurCol--;
          return TRUE;
       }

       // limit cursor to the cell?
       if (TerFlags3&TFLAG3_CURSOR_IN_CELL && (cid(CurLine)!=cid(CurLine+1) || LineInfo(w,CurLine+1,INFO_ROW))) {
          CurCol--;
          return TRUE;
       }

       // don't let it go from page header/footer to a body frame
       InHdrFtr=PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR;
       if (InHdrFtr && LineFlags(CurLine+1)&LFLAG_HDRS_FTRS) {
          CurCol--;
          return TRUE;  // can't go any further
       }

       // don't let it go from body to header footer
       if (!InHdrFtr && PfmtId[pfmt(CurLine+1)].flags&PAGE_HDR_FTR && !EditPageHdrFtr) {
          line=CurLine;
          while ((line+1)<TotalLines && PfmtId[pfmt(line+1)].flags&PAGE_HDR_FTR) line++;  // can't go any further
          if ((line+1)>=TotalLines) return TRUE;
          CurLine=line+1;
       }
       else CurLine++;

       CurCol=0;

       if ((CurPage+1)<TotalPages && LinePage(CurLine)==CurPage) {// on the same page
          WrapFlag=WRAP_OFF;                  // also disables frame creation
          PaintFlag=PAINT_LINE;               // allows changes to TerWinOrgY
       }
   }
   else {
       WrapFlag=WRAP_OFF;
       PaintFlag=PAINT_MIN;        // minimum painting required
   }

   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   PgmLeft:
   process the left arrow in page mode
*******************************************************************************/
PgmLeft(PTERWND w)
{
   BOOL InHdrFtr;
   long line;

   if (HilightType==HILIGHT_CHAR && cid(HilightEndRow)>0) return TblHilightLeft(w);

   TerSetCharHilight(w);              // turnoff any highlighting

   CursDirection=CURS_BACKWARD;       // cursor moving backward

   if (CurCol>0) {
       CurCol--;
       WrapFlag=WRAP_OFF;
       PaintFlag=PAINT_MIN;
       PaintTer(w);
       return TRUE;
   }
   else {
       if (CurLine!=0) {
          if (TerFlags3&TFLAG3_CURSOR_IN_CELL && cid(CurLine)!=cid(CurLine-1)) return TRUE;

          // don't let it go from page header/footer to a body frame
          InHdrFtr=PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR;
          if  (InHdrFtr && LineFlags(CurLine-1)&LFLAG_HDRS_FTRS) return TRUE;  // can't go any further

          // don't let it go from body to header footer
          if (!InHdrFtr && PfmtId[pfmt(CurLine-1)].flags&PAGE_HDR_FTR && !EditPageHdrFtr) {
             line=CurLine;
             while (line>0 && PfmtId[pfmt(line-1)].flags&PAGE_HDR_FTR) line--;  // can't go any further
             if (line==0) return TRUE;
             CurLine=line-1;
          }
          else CurLine--;

          CurCol=LineLen(CurLine)-1;
          if (LinePage(CurLine)==CurPage) {// on the same page
             WrapFlag=WRAP_OFF;                  // also disables frame creation
             PaintFlag=PAINT_LINE;               // allows changes to TerWinOrgY
          }
          PaintTer(w);
          return TRUE;
       }
       else return TRUE;
   }
}

/*****************************************************************************
   PgmPageUp:
   Display the previous page in page mode
******************************************************************************/
PgmPageUp(PTERWND w)
{
   int i,y,NewY,FrameY=-1,TopFrame=-1,PrevFrame;

   CursDirection=CURS_BACKWARD;          // cursor moving backward

   if (TerWinOrgY==0 && CurPage==0) {
      for (i=TotalFrames-1;i>=0;i--) {
         if (frame[i].empty || frame[i].flags&FRAME_DISABLED) continue;
         if (TerFlags3&TFLAG3_CURSOR_IN_CELL && frame[i].CellId!=cid(CurLine)) continue;

         if (FrameY<0) FrameY=frame[i].y;
         if (frame[i].y<=FrameY) {
            FrameY=frame[i].y;
            TopFrame=i;
         }
      }
      if (TopFrame>=0 && CurLine!=frame[TopFrame].PageFirstLine) {
        CurLine=frame[TopFrame].PageFirstLine;  // to position at the top of the file
        CurCol=0;
        CursDirection=CURS_FORWARD;
        PaintTer(w);
      }
      return TRUE;
   }

   NewY=TerWinOrgY-TerWinHeight+(3*TerFont[0].height)/2;
   if (CursHorzPos<0) CursHorzPos=ColToUnits(w,CurCol,CurLine,LEFT);


   // POSITION:
   TerWinOrgY=NewY;
   if (TerWinOrgY<0) {                     // going to previous page
      if (FirstFramePage>0) {              // more pages available to scroll
         if (CurPage>0) CurPage--;         // position on the previous page
         FirstFramePage--;
         CreateFrames(w,FALSE,FirstFramePage,FirstFramePage+1);  // create frame for this page
         TerWinOrgY=FirstPageHeight+NewY;
         if (TerWinOrgY<0) TerWinOrgY=0;
         if (CommandId==ID_UP) CurLineY=TerWinHeight;
         else                  CurLineY=TerWinHeight/2;
      }
      else {
         TerWinOrgY=0;
         if (CurLineY<0) CurLineY=0;
      }
   }


   // END_PAGE_UP:

   // get the current line
   if (CurLineY>(TerWinHeight-3*TerFont[0].height)/2) CurLineY=TerWinHeight-3*TerFont[0].height/2;
   if (CurLineY<0) CurLineY=0;
   y=TerWinOrgY+CurLineY;
   PrevFrame=-1;
   for (i=(TotalFrames-1);i>=0;i--) {
      if (frame[i].empty || frame[i].flags&FRAME_DISABLED) continue;
      if (TerFlags3&TFLAG3_CURSOR_IN_CELL && frame[i].CellId!=cid(CurLine)) continue;

      if (y>=frame[i].y && y<(frame[i].y+frame[i].height)) {
        CurLine=UnitsToLine(w,CursHorzPos,y);
        break;
      }
      else if (frame[i].y<=y) {           // find the nearest frame
         if (PrevFrame!=-1 && frame[PrevFrame].y<(TerWinOrgY+TerWinHeight)) CurLine=frame[PrevFrame].PageFirstLine;  // try to remain with in the visible area
         else CurLine=frame[i].PageLastLine;   // This CurLine will do if the above UnitsToLine never executes
         //break;     // keep looping to possibly get CurLine from the above UnitsToLine function
      }
      PrevFrame=i;
   }
   if (i<0 && PrevFrame!=-1 && frame[PrevFrame].y<(TerWinOrgY+TerWinHeight)) CurLine=frame[PrevFrame].PageFirstLine;  // try to remain with in the visible area

   CurCol=UnitsToCol(w,CursHorzPos,CurLine);


   y=LineToUnits(w,CurLine);
   if (y<TerWinOrgY || y>=(TerWinOrgY+TerWinHeight)) DisengageCaret(w);

   SetTerWindowOrg(w);              // set the logical window origin
   UseTextMap=FALSE;                // no point building map, probably won't be used
   PaintTer(w);

   return TRUE;
}

/*****************************************************************************
   PgmPageDn:
   Display the next page in page mode
******************************************************************************/
PgmPageDn(PTERWND w)
{
   int i,y,NewY,PrevFrame;
   long l;

   CursDirection=CURS_FORWARD;          // cursor moving forward

   CALC_NEW_Y:
   NewY=TerWinOrgY+TerWinHeight-(3*TerFont[0].height)/2;
   if (NewY<TerWinOrgY) NewY=TerWinOrgY+TerWinHeight;

   if (CursHorzPos<0) CursHorzPos=ColToUnits(w,CurCol,CurLine,LEFT);


   // POSITION:
   // Going to next page?
   if ((TerWinOrgY+TerWinHeight)>=CurTextHeight) {   // end of page displayed
      if (CurPage<(TotalPages-1) && LastFramePage<(TotalPages-1)) {    // more pages available to scroll
         CurPage=LastFramePage+1;                    // position on the next page
         if (LastFramePage>FirstFramePage) TerWinOrgY-=FirstPageHeight;  // previous page being dropped
         CreateFrames(w,FALSE,CurPage-1,CurPage); // create frame for this page
         goto CALC_NEW_Y;
      }
      else {              // last pane of the last page showing
         if (CommandId!=ID_PGDN && CommandId!=ID_DOWN) { // for scroll down only
            TerWinOrgY=NewY;
            if ((TerWinOrgY+TerWinHeight)>=CurPageHeight) TerWinOrgY=CurPageHeight-TerWinHeight-1;
            if (TerWinOrgY<0) TerWinOrgY=0;
            CurLineY=0;
         }
         else if (CommandId==ID_PGDN) { // position at the last line last column
            BOOL LineSet=FALSE;
            if (TerFlags3&TFLAG3_CURSOR_IN_CELL && cid(CurLine)) {  // position at the last line of the cell
                for (l=CurLine+1;l<TotalLines;l++) if (cid(l)!=cid(CurLine) || LineInfo(w,l,INFO_ROW)) break;
                CurLine=l-1;
                LineSet=TRUE;
            }
            else if (LineToUnits(w,TotalLines-1)>=TerWinOrgY) {
               CurLine=TotalLines-1;
               LineSet=TRUE;
            }
            if (LineSet) {
               CurCol=LineLen(CurLine)-1;
               if (CurCol<0) CurCol=0;
               PaintTer(w);
            }
            return TRUE;
         }
         else return TRUE;             // keep showing the last page
      }
   }
   else {                              // show the next pane
      TerWinOrgY=NewY;
      if ((TerWinOrgY+TerWinHeight)>=CurPageHeight) TerWinOrgY=CurPageHeight-TerWinHeight/*-1*/;
      if (TerWinOrgY<0) TerWinOrgY=0;

      if (CurLineY<0) CurLineY=0;
   }

   // END_PAGE_DOWN:

   // get the current line
   y=TerWinOrgY+CurLineY;
   PrevFrame=-1;
   for (i=0;i<TotalFrames;i++) {
      if (frame[i].empty || frame[i].flags&FRAME_DISABLED) continue;
      if (TerFlags3&TFLAG3_CURSOR_IN_CELL && frame[i].CellId!=cid(CurLine)) continue;

      if (y>=frame[i].y && y<(frame[i].y+frame[i].height)) {
        CurLine=UnitsToLine(w,CursHorzPos,y);
        break;
      }
      else if (frame[i].y>=y) {           // find the nearest frame
         if (PrevFrame!=-1 && (frame[PrevFrame].y+frame[PrevFrame].height)>TerWinOrgY) CurLine=frame[PrevFrame].PageLastLine;  // try to remain with in the visible area
         else CurLine=frame[i].PageFirstLine;
         break;
      }
      PrevFrame=i;
   }
   if (i==TotalFrames && PrevFrame!=-1 && (frame[PrevFrame].y+frame[PrevFrame].height)>TerWinOrgY) CurLine=frame[PrevFrame].PageLastLine;  // try to remain with in the visible area

   // get the page number
   if (i<TotalFrames) {
      if (i<FirstPage2Frame) CurPage=FirstFramePage;
      else                   CurPage=LastFramePage;
   }
   CurCol=UnitsToCol(w,CursHorzPos,CurLine);

   y=LineToUnits(w,CurLine);
   if ((y<TerWinOrgY || y>=(TerWinOrgY+TerWinHeight)) && CommandId!=ID_DOWN) DisengageCaret(w);

   SetTerWindowOrg(w);              // set the logical window origin
   UseTextMap=FALSE;                // no point building map, probably won't be used
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   PgmWinUp:
   Scroll the window up by one line maintaining the same cursor position if
   possible. (in page mode)
*******************************************************************************/
PgmWinUp(PTERWND w)
{
   int SaveX,DeltaY,SaveWinOrgY;

   SaveWinOrgY=TerWinOrgY;

   // do we go to the next page now?
   DeltaY=TerFont[0].height;           // amount to scroll
   if (MessageId==WMTER_MOUSEWHEEL) DeltaY*=3;   // bigger jump

   SaveX=ColToUnits(w,CurCol,CurLine,LEFT);

   if (TerWinOrgY==0 && CurPage==0) return TRUE;         // on the top of the file

   if (FirstFramePage>0 && DeltaY>TerWinOrgY) {
       CreateFrames(w,FALSE,FirstFramePage-1,FirstFramePage);
       TerWinOrgY+=FirstPageHeight;
       if ((TerWinOrgY+TerWinHeight-DeltaY)>CurTextHeight) TerWinOrgY=CurTextHeight-TerWinHeight+DeltaY;
       //return PgmPageUp(w);  // frames for the previous page not yet built
   }

   TerWinOrgY-=DeltaY;
   if (TerWinOrgY<0) TerWinOrgY=0;   // this should happen only on the first page

   if (CurLineY<0) CurLineY=0;
   if (CurLineY>TerWinHeight) CurLineY=TerWinHeight;
   if ((TerWinOrgY+CurLineY)>CurTextHeight) CurLineY=CurTextHeight-TerWinOrgY;
   
   if (CurLineY<0) CurLineY=0;
   CurLine=UnitsToLine(w,SaveX,TerWinOrgY+CurLineY);
   CurCol=UnitsToCol(w,SaveX,CurLine);

   SetTerWindowOrg(w);

   // scroll the window and set the clip region
   PgmWinScroll(w,0,TerWinOrgY-SaveWinOrgY);

   return TRUE;
}

/******************************************************************************
   PgmWinDown:
   Scroll the window down by one line maintaining the same cursor position if
   possible. (in page mode)
*******************************************************************************/
PgmWinDown(PTERWND w)
{
   int SaveX,DeltaY,SaveWinOrgY,FullHeight,YAdj;
   BOOL FramesRefreshed=FALSE;

   // do we go to the next page now?
   SaveWinOrgY=TerWinOrgY;
   DeltaY=TerFont[0].height;           // amount to scroll
   if (MessageId==WMTER_MOUSEWHEEL) DeltaY*=3;   // bigger jump
   
   SaveX=ColToUnits(w,CurCol,CurLine,LEFT);

   TOP:
   //if ((TerWinOrgY+(TerWinHeight/2))>CurTextHeight && CurPage<(TotalPages-1)) return PgmPageDn(w);  // position at the next page
   if ((TerWinOrgY+TerWinHeight)>=CurPageHeight) {
      if (LastFramePage<(TotalPages-1)) {
         YAdj=(FirstFramePage<LastFramePage)?FirstPageHeight:0;
         CreateFrames(w,FALSE,LastFramePage,LastFramePage+1);              // create the text frames
         FramesRefreshed=TRUE;
         TerWinOrgY-=YAdj;
         goto TOP;
      }
      else return TRUE;   // at the end of the document
   } 
   //else if (   (TerWinOrgY+DeltaY)>=CurTextHeight
   //    || (TerWinOrgY+TerWinHeight)>=CurTextHeight ) {
   //    if (LastFramePage<(TotalPages-1)) {
   //       if (!FramesRefreshed) {         // refresh the frames so that current page and the next page
   //           CreateFrames(w,FALSE,LastFramePage,LastFramePage+1);              // create the text frames
   //           FramesRefreshed=TRUE;
   //           TerWinOrgY=0;
   //           goto TOP;
   //       }
   //       else return PgmPageDn(w);  // position at the next page
   //    } 
   //    else TerWinOrgY+=DeltaY;
   //}
   else TerWinOrgY+=DeltaY;

   FullHeight=(ShowPageBorder || (LastFramePage+1)<TotalPages)?CurPageHeight:CurTextHeight;
   if ((TerWinOrgY+TerWinHeight)>=FullHeight) TerWinOrgY=FullHeight-TerWinHeight;
   //if ((TerWinOrgY+TerWinHeight)>=CurPageHeight) TerWinOrgY=CurPageHeight-TerWinHeight/*-1*/;
   
   if (TerWinOrgY<0) TerWinOrgY=0;

   if (CurLineY<0) CurLineY=0;
   if ((TerWinOrgY+CurLineY)>CurTextHeight) CurLineY=CurTextHeight-TerWinOrgY;
   if (CurLineY<0) CurLineY=0;
   CurLine=UnitsToLine(w,SaveX,TerWinOrgY+CurLineY);
   CurCol=UnitsToCol(w,SaveX,CurLine);


   // scroll the window and set the clip region
   if (TerWinOrgY!=SaveWinOrgY || FramesRefreshed) {
      SetTerWindowOrg(w);
      PgmWinScroll(w,0,TerWinOrgY-SaveWinOrgY);
   }

   return TRUE;
}

/******************************************************************************
   PgmWinRight:
   Scroll the window right by one column maintaining the same cursor position if
   possible.
*******************************************************************************/
PgmWinRight(PTERWND w)
{
   int SaveX,SaveY,MaxTextX,DeltaX;
   LPBYTE ptr;
   LPWORD fmt;

   SaveX=ColToUnits(w,CurCol,CurLine,LEFT);
   SaveY=LineToUnits(w,CurLine);

   // set x origin
   if (CurCol<LineLen(CurLine)) {
      ptr=pText(CurLine);
      fmt=OpenCfmt(w,CurLine);
      DeltaX=TerFont[fmt[CurCol]].CharWidth[(BYTE)ptr[CurCol]];

      CloseCfmt(w,CurLine);
   }
   else DeltaX=DblCharWidth(w,0,TRUE,' ',0);
   TerWinOrgX+=DeltaX;
   MaxTextX=PageTextWidth(w);
   if ((TerWinOrgX+TerWinWidth)>MaxTextX) TerWinOrgX=MaxTextX-TerWinWidth;
   if (TerWinOrgX<0) TerWinOrgX=0;

   // set new column
   SaveX=SaveX+DeltaX;
   if (SaveX>(TerWinOrgX+TerWinWidth)) SaveX=TerWinOrgX+TerWinWidth;
   CurLine=UnitsToLine(w,SaveX,SaveY);
   CurCol=UnitsToCol(w,SaveX,CurLine);

   SetTerWindowOrg(w);    // set the logical window origin

   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   PgmWinLeft:
   Scroll the window left by one column maintaining the same cursor position if
   possible.
*******************************************************************************/
PgmWinLeft(PTERWND w)
{
   int SaveX,SaveY,DeltaX;
   LPBYTE ptr;
   LPWORD fmt;

   SaveX=ColToUnits(w,CurCol,CurLine,LEFT);
   SaveY=LineToUnits(w,CurLine);

   // set x origin
   if (CurCol>0) {
      ptr=pText(CurLine);
      fmt=OpenCfmt(w,CurLine);
      DeltaX=TerFont[fmt[CurCol-1]].CharWidth[(BYTE)ptr[CurCol-1]];

      CloseCfmt(w,CurLine);
   }
   else DeltaX=DblCharWidth(w,0,TRUE,' ',0);
   TerWinOrgX-=DeltaX;
   if (TerWinOrgX<0) TerWinOrgX=0;

   // set new column
   SaveX=SaveX-DeltaX;
   if (SaveX<0) SaveX=0;
   CurLine=UnitsToLine(w,SaveX,SaveY);
   CurCol=UnitsToCol(w,SaveX,CurLine);

   SetTerWindowOrg(w);    // set the logical window origin

   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   PgmWinScroll:
   This function makes an image of the page (if not already existing) and
   displays the scrolled portion of the image.
*******************************************************************************/
PgmWinScroll(PTERWND w, int ScrollX, int ScrollY)
{
   int SaveOrgX,SaveOrgY,SaveWinWidth,SaveWinHeight,SaveCurLineY,BMWidth,BMHeight;
   int NewFirstPage,NewLastPage,sect,OrigBkMode;
   float width1,width2,height1,height2;
   HDC  hScrollDC,SaveDC;
   RECT SaveRect;
   HBITMAP hOldBM;
   long FirstLine,LastLine;
   HBRUSH hBkBrush=NULL;
   int  SecondPageHalfY,WinHalfY;

   if (TerFlags4&TFLAG4_NO_BUFFERED_SCROLL || TerArg.FittedView) goto PAINT_METHOD;
   if (BkPictId>0 && Clr256Display) goto PAINT_METHOD;

   if (!hScrollBM && !ContinuousScroll) {  // next scrolling will be using the bitmap
      ContinuousScroll=TRUE;
      goto PAINT_METHOD;
   }

   // dimension of the bitmap
   sect=TerGetPageSect(hTerWnd,FirstFramePage);
   width1=TerSect1[sect].PgWidth;
   height1=TerSect1[sect].PgHeight;

   sect=TerGetPageSect(hTerWnd,LastFramePage);
   width2=TerSect1[sect].PgWidth;
   height2=TerSect1[sect].PgHeight;

   if (width1>width2) BMWidth=(int)(width1*ScrResX);
   else               BMWidth=(int)(width2*ScrResX);
   BMWidth+=2*PrtToScrX(LeftBorderWidth);      // add the border widths
   if (BMWidth<TerWinWidth) BMWidth=TerWinWidth;
   if (TerArg.FittedView) BMWidth=TerWinWidth;
   BMWidth++;

   BMHeight=(int)((height1+height2)*ScrResY);   // make upto 2 pages at a time
   if (TerArg.FittedView) BMHeight=BMHeight*FITTED_VIEW_DISP_PAGES;
   if (BorderShowing) {
      BMHeight+=PrtToScrY(4*TopBorderHeight);
   }

   if (BMHeight<TerWinHeight) BMHeight=TerWinHeight;
   BMHeight++;

   // create background brush
   if (TextDefBkColor!=0xFFFFFF) hBkBrush=CreateSolidBrush(TextDefBkColor);
   else                          hBkBrush=GetStockObject(WHITE_BRUSH);

   // check if previous bitmap is usable
   CurPage=GetCurPage(w,CurLine);
   NewFirstPage=FirstFramePage;  // calculate first and last page in the frame
   NewLastPage=LastFramePage;

   WinHalfY=TerWinOrgY+(TerWinHeight/2);  // half point of the window
   if (FirstFramePage>0 && WinHalfY<(FirstPageHeight/2)) NewFirstPage--;
   SecondPageHalfY=(CurPageHeight-FirstPageHeight)/2+FirstPageHeight;
   if ( (LastFramePage+1)<TotalPages && WinHalfY>SecondPageHalfY) NewLastPage++;
   if (FirstFramePage!=NewFirstPage || LastFramePage!=NewLastPage) {
      if (hScrollBM) DeleteObject(hScrollBM);  // delete the temporary bitmap used for scrolling
      hScrollBM=NULL;
   }


   // create the image of the page
   if (!hScrollBM) {
      // word wrap the current page
      FirstLine=PageInfo[NewFirstPage].FirstLine;
      if ((NewLastPage+1)<TotalPages) LastLine=PageInfo[NewLastPage].LastLine;
      else                            LastLine=TotalLines;
      WordWrap(w,FirstLine,LastLine-FirstLine);
      RefreshFrames(w,TRUE);

      // delete existing text segments
      DeleteTextMap(w,FALSE);

      // create scroll dc and bitmap
      if (NULL==(hScrollDC=CreateCompatibleDC(hTerDC))) goto PAINT_METHOD;
      if (NULL==(hScrollBM=CreateCompatibleBitmap(hTerDC,BMWidth,BMHeight))) goto PAINT_METHOD;  // follow full paint method
      hOldBM=SelectObject(hScrollDC,hScrollBM);

      // save the current window parameters and replace by temporary parameters
      SaveOrgX=TerWinOrgX;
      TerWinOrgX=0;
      SaveOrgY=TerWinOrgY;
      TerWinOrgY=0;
      SaveCurLineY=CurLineY;
      SaveWinWidth=TerWinWidth;
      TerWinWidth=BMWidth;
      SaveWinHeight=TerWinHeight;
      TerWinHeight=BMHeight;
      SaveRect=TerWinRect;
      SetRect(&TerWinRect,0,0,TerWinWidth,TerWinHeight);
      CurForeColor  =0x10000000;   // reset current color
      CurBackColor  =0x10000000;

      SelectObject(hTerDC,GetStockObject(SYSTEM_FONT));   // remove font from our DC
      SaveDC=hTerDC;
      hTerDC=hScrollDC;
      hTerCurFont   =TerFont[0].hFont;
      SelectObject(hTerDC,hTerCurFont);

      OrigBkMode=SetBkMode(hTerDC,TRANSPARENT);  // VGA 1024/768 has problem with OPAQUE (not really needed any way)

      // fill the device bitmap
      if (hBkBrush) FillRect(hTerDC,&TerWinRect,hBkBrush); // initialize the bitmap

      // paint the text into the memory device context
      PaintFlag=PAINT_WIN;
      UseTextMap=FALSE;                    // no point building map, probably won't be used
      PictureClicked=FrameClicked=FALSE;
      FrameRefreshEnabled=FALSE;           // to protect TerWinOrgY from changing
      PaintFrames(w,hScrollDC,-1);
      FrameRefreshEnabled=TRUE;
      SetBkMode(hTerDC,OrigBkMode);        // reset to opaque mode
      
      if (CaretEnabled && !CaretHidden) ShowCaret(hTerWnd); // show caret after clip reset

      // restore the current parameters;
      TerWinOrgX=SaveOrgX;
      TerWinOrgY=SaveOrgY;
      CurLineY=SaveCurLineY;
      TerWinWidth=SaveWinWidth;
      TerWinHeight=SaveWinHeight;
      TerWinRect=SaveRect;
      CurForeColor  =0x10000000;   // reset current color
      CurBackColor  =0x10000000;

      SelectObject(hTerDC,GetStockObject(SYSTEM_FONT));   // remove font from the memory DC
      hTerDC=SaveDC;
      hTerCurFont   =TerFont[0].hFont;
      SelectObject(hTerDC,hTerCurFont);
   }
   else {
      if (NULL==(hScrollDC=CreateCompatibleDC(hTerDC))) goto PAINT_METHOD;
      hOldBM=SelectObject(hScrollDC,hScrollBM);
   }

   // show the bits of on the window
   if (BkPictId==0) BitBlt(hTerDC,TerWinOrgX,TerWinOrgY,TerWinWidth,TerWinHeight,hScrollDC,TerWinOrgX,TerWinOrgY,SRCCOPY);
   else             TransBlt(w,hScrollDC,TerWinOrgX,TerWinOrgY,TerWinWidth,TerWinHeight);

   // set caret and scoll bars
   OurSetCaretPos(w);
   if (TerArg.ShowHorBar || TerArg.ShowVerBar) SetScrollBars(w);
   if (MessageId==WM_HSCROLL) DrawRuler(w,FALSE);


   // clear any space from the right
   if ((TerWinOrgX+TerWinWidth)>BMWidth) {
      RECT rect;
      SetRect(&rect,BMWidth,TerWinOrgY,TerWinOrgX+TerWinWidth,TerWinOrgY+TerWinHeight);
      FillRect(hTerDC,&rect,hBkBrush);
   }

   // clear any space from the bottom
   if ((TerWinOrgY+TerWinHeight)>BMHeight) {
      RECT rect;
      SetRect(&rect,TerWinOrgX,BMHeight,TerWinOrgX+TerWinWidth,TerWinOrgY+TerWinHeight);
      FillRect(hTerDC,&rect,hBkBrush);
   }

   // delete the background brush
   if (hBkBrush && TextDefBkColor!=0xFFFFFF) DeleteObject(hBkBrush);

   // delete the compatible DC
   SelectObject(hScrollDC,GetStockObject(BLACK_PEN));
   SelectObject(hScrollDC,GetStockObject(SYSTEM_FONT));
   SelectObject(hScrollDC,hOldBM);
   DeleteDC(hScrollDC);

   return TRUE;

   PAINT_METHOD:
   // delete the background brush
   if (hBkBrush && TextDefBkColor!=0xFFFFFF) DeleteObject(hBkBrush);

   UseTextMap=FALSE;                    // no point building map, probably won't be used
   PictureClicked=FrameClicked=FALSE;
   
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   TransBlt:
   Do transparent blasting of bitmap
*******************************************************************************/
TransBlt(PTERWND w,HDC hTextDC,int x,int y,int width,int height)
{
   HDC hMaskDC;
   HBITMAP hMaskBM,hOldMaskBM,hDestBM,hOldDestBM;
   HDC hDestDC;

   dm("TransBlt");

   // create the mask
   if (NULL==(hMaskDC=CreateCompatibleDC(hTerDC))) return FALSE;
   if (NULL==(hMaskBM=CreateBitmap(width,height,1,1,NULL))) return FALSE;
   hOldMaskBM=SelectObject(hMaskDC,hMaskBM);

   SetBkColor(hTextDC,TextDefBkColor);
   BitBlt(hMaskDC,0,0,width,height,hTextDC,x,y,SRCCOPY);

   // create the temporary destination bitmap
   if (NULL==(hDestDC=CreateCompatibleDC(hTerDC))) return FALSE;
   if (NULL==(hDestBM=CreateCompatibleBitmap(hTerDC,width,height))) return FALSE;
   hOldDestBM=SelectObject(hDestDC,hDestBM);

   // set color for the destination DC
   SetBkColor(hDestDC,0xFFFFFF);
   SetTextColor(hDestDC,0L);

   // draw the background
   PaintBkPict(w,hDestDC);

   // do blasting
   BitBlt(hDestDC,0,0,width,height,hTextDC,x,y,SRCINVERT);
   BitBlt(hDestDC,0,0,width,height,hMaskDC,0,0,SRCAND);
   BitBlt(hDestDC,0,0,width,height,hTextDC,x,y,SRCINVERT);

   BitBlt(hTerDC,x,y,width,height,hDestDC,0,0,SRCCOPY);

   // freeup the resources
   SelectObject(hMaskDC,hOldMaskBM);
   DeleteObject(hMaskBM);
   DeleteDC(hMaskDC);

   SelectObject(hDestDC,hOldDestBM);
   DeleteObject(hDestBM);
   DeleteDC(hDestDC);


   return TRUE;
}

/******************************************************************************
   PgmPageLeft:
   Scroll the window left by a page or a column
*******************************************************************************/
PgmPageLeft(PTERWND w,BOOL page)
{
   int SaveWinOrgX;

   SaveWinOrgX=TerWinOrgX;
   if (page) TerWinOrgX-=TerWinWidth/2;
   else      TerWinOrgX-=TerWinWidth/8;
   if (TerWinOrgX<0) TerWinOrgX=0;
   SetTerWindowOrg(w);    // set the window origin at TerWinOrgX value

   if (page) {
      UseTextMap=FALSE;      // no point building map, probably won't be used
      PaintTer(w);
   }
   else PgmWinScroll(w,TerWinOrgX-SaveWinOrgX,0);

   return TRUE;
}

/******************************************************************************
   PgmPageRight:
   Scroll the window right by page or a column
*******************************************************************************/
PgmPageRight(PTERWND w,BOOL page)
{
   int MaxTextX,SaveWinOrgX;

   SaveWinOrgX=TerWinOrgX;
   MaxTextX=PageTextWidth(w)+1;
   if (MaxTextX<TerWinWidth) return TRUE;  // no reason to scroll

   if (page) TerWinOrgX+=TerWinWidth/2;
   else      TerWinOrgX+=TerWinWidth/8;
   if ((TerWinOrgX+TerWinWidth)>MaxTextX) TerWinOrgX=MaxTextX-TerWinWidth;
   if (TerWinOrgX<0) TerWinOrgX=0;

   SetTerWindowOrg(w); // set the window origin at TerWinOrgX value

   if (page) {
      UseTextMap=FALSE;      // no point building map, probably won't be used
      PaintTer(w);
   }
   else PgmWinScroll(w,TerWinOrgX-SaveWinOrgX,0);

   return TRUE;
}

/******************************************************************************
   PgmPageHorz:
   Scroll the window horizontally to position at the specified thumbtrack
   position.
*******************************************************************************/
PgmPageHorz(PTERWND w,int pos)
{
   int MaxTextX;

   MaxTextX=PageTextWidth(w);
   if (MaxTextX<TerWinWidth) return TRUE;  // no reason to scroll

   TerWinOrgX=(int)(((long)(MaxTextX-TerWinWidth)*pos)/(SCROLL_RANGE-HorThumbSize));
   if (TerWinOrgX>(MaxTextX-TerWinWidth)) TerWinOrgX=MaxTextX-TerWinWidth;

   SetTerWindowOrg(w); // set the window origin at TerWinOrgX value

   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintTer(w);
   return TRUE;
}

/******************************************************************************
   PgmPageVert:
   Position at the specified vertical thumbtrack postion.
*******************************************************************************/
PgmPageVert(PTERWND w,int pos)
{
   int NewPage,CurY;
   int SavePage=CurPage;
   long TotalY,PageAbsY,PageY,height,CurHeight;
   BOOL PosAtPageTop=FALSE;
   BOOL SmoothScroll=(TerOpFlags&TOFLAG_CONTINUOUS_SCROLL);
   int ScrollRange=SCROLL_RANGE-VerThumbSize;

   TotalY=SumPageScrHeight(w,0,TotalPages)-TerWinHeight;
   
   if (pos>=(ScrollRange*99/100)) {
      NewPage=TotalPages-1;
      PageY=GetScrPageHt(w,NewPage)-TerWinHeight;
      if (PageY<0) PageY=0;
   }
   else { 
      PageAbsY=MulDiv(TotalY,pos,ScrollRange);
      height=0;
      for (NewPage=0;NewPage<(TotalPages-1);NewPage++) {
         CurHeight=GetScrPageHt(w,NewPage);
         if ((height+CurHeight)>PageAbsY) break;
         height+=CurHeight;
      }
      PageY=PageAbsY-height;   // Y offset at the current page
      if (PageY>=SumPageScrHeight(w,NewPage,1)) PageY=SumPageScrHeight(w,NewPage,1)-1;
   }

   // calculate the new page and new y
   if (NewPage!=CurPage && PagesShowing && !SmoothScroll) {
      if (NewPage<(TotalPages-1) || pos<(ScrollRange-2)) PosAtPageTop=TRUE;  // on the same page
   }

   // position at the page
   CurPage=NewPage;
   if (SavePage!=CurPage) RefreshFrames(w,TRUE);

   // set y origin
   if (PosAtPageTop) {
      if (CurPage==FirstFramePage) TerWinOrgY=0;
      else                         TerWinOrgY=FirstPageHeight;
   }
   else {                          // fitted view mode
      if (CurPage==FirstFramePage) TerWinOrgY=(int)PageY;
      else                         TerWinOrgY=FirstPageHeight+(int)PageY;
      if (TerWinOrgY>=(CurPageHeight-TerWinHeight)) TerWinOrgY=CurPageHeight-TerWinHeight;
      if (TerWinOrgY<0) TerWinOrgY=0;
   }

   SetTerWindowOrg(w);

   // Get the current line
   CurY=TerWinOrgY;
   if (CurY>=CurTextHeight) CurY=CurTextHeight-1;
   CurLine=UnitsToLine(w,0,CurY);
   CurCol=0;

   UseTextMap=FALSE;                    // no point building map, probably won't be used

   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   TerInsertPageRef:
   Insert a page ref field.
*******************************************************************************/
BOOL WINAPI _export TerInsertPageRef(HWND hWnd, LPBYTE bookmark, BOOL IsHyperlink, BOOL IsAlphabetic, BOOL repaint)
{
   PTERWND w;
   BYTE FieldCode[MAX_WIDTH+1];
   int CurFont;

   if (NULL==(w=GetWindowPointer(hWnd))) return false;  // get the pointer to window data

   if (!TerArg.PageMode) return false;
   if (!CanInsert(w,CurLine,CurCol)) return false;

   if (!IsValidBookmark(w,bookmark,true)) return false;
   
   // build the field code
   lstrcpy(FieldCode,bookmark);
   lstrcat(FieldCode," ");

   if (IsHyperlink)  lstrcat(FieldCode,"\\h ");
   if (IsAlphabetic) lstrcat(FieldCode,"\\* alphabetic ");
   lstrcat(FieldCode,"\\* MERGEFORMAT ");

   CurFont=GetEffectiveCfmt(w);
   InputFontId=CurFont=TerGetFieldFont(hTerWnd,CurFont,FIELD_PAGE_REF,FieldCode);

   InsertTerText(hTerWnd,"1",repaint);
    
   InputFontId=-1;                             // reset input font id
   TerArg.modified++;

   return true;
}

/******************************************************************************
   TerGetTextHeight:
   Get the total text height (twips) in all pages.  The height does not include
   the header footer text.
*******************************************************************************/
long WINAPI _export TerGetTextHeight(HWND hWnd)
{
   PTERWND w;
   long DocSize=0;
   int i;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   if (TerArg.PageMode) {
      long BodySize=0;   // body size in printer units
      for (i=0;i<TotalPages;i++) {
         BodySize+=PageInfo[i].BodyTextHt;
      }
      return PrtToTwipsY(BodySize);
   }
   else {
      long l;
      for (l=0;l<TotalLines;l++) DocSize+=ScrLineHeight(w,l,FALSE,FALSE);
      return ScrToTwipsY(DocSize); 
   }
   
}

/******************************************************************************
   SumPageScrHeight:
   Returns the sum of the screen height for specified number of pages.
*******************************************************************************/
long SumPageScrHeight(PTERWND w, int StartPage, int count)
{
   int i;
   long height=0;

   if (StartPage<0) count=0;
   if (StartPage>=TotalPages) StartPage=TotalPages-1;

   for (i=StartPage;i<(StartPage+count);i++) height+=(long)GetScrPageHt(w,i);
   return height;
}

/******************************************************************************
   GetScrPageHt:
   Get the screen height of the given page
*******************************************************************************/
int GetScrPageHt(PTERWND w, int PageNo)
{
   int sect,height,BorderHeight=0;
   long FirstLine;

   if (BorderShowing) BorderHeight=PrtToScrY(TopBorderHeight+BotBorderHeight);

   if (TerArg.FittedView) height=PageInfo[PageNo].ScrHt;
   else {
      if (!BorderShowing && PageNo==(TotalPages-1) && !ViewPageHdrFtr) height=PageInfo[PageNo].TextHt;  // just consider the height of the text on the last page
      else {                                          // consider the full height of the page
         FirstLine=PageInfo[PageNo].FirstLine;
         sect=GetSection(w,FirstLine);
         height=(int)(TerSect1[sect].PgHeight*ScrResY);
         if (!BorderShowing && !ViewPageHdrFtr) height-=(int)((TerSect[sect].TopMargin+TerSect[sect].BotMargin)*ScrResY);
      }
   }
   height+=BorderHeight;

   return height;
}

/******************************************************************************
   TerOverridePageCount:
   Overide the page count value for PageCount field. Set to 0 to remove override
*******************************************************************************/
BOOL WINAPI _export TerOverridePageCount(HWND hWnd,int PageCount)
{
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   OverrideTotalPages=PageCount;

   return TRUE;
}

/******************************************************************************
   TerGetPageCount:
   Get total number of pages in the doocument and current page number.
*******************************************************************************/
BOOL WINAPI _export TerGetPageCount(HWND hWnd,LPINT pTotalPages, LPINT pCurPage)
{
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
   if (!TerArg.PageMode) return FALSE;
   
   if (pTotalPages) (*pTotalPages)=TotalPages;
   if (pCurPage) (*pCurPage)=CurPage;            
                
   return TRUE;
}

/******************************************************************************
   TerGetPagePos:
   Get the current page number displayed at the top of the window and
   the offset of the window top relative to the page top.
   This function returns TRUE when successful.
*******************************************************************************/
BOOL WINAPI _export TerGetPagePos(HWND hWnd,LPINT pPage, LPINT pOff)
{
   int CurY,page;
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
   if (!TerArg.PageMode) return FALSE;

   page=FirstFramePage;
   CurY=TerWinOrgY;

   if (TerWinOrgY>FirstPageHeight && (page+1)<TotalPages) {
      page++;
      CurY-=FirstPageHeight;
   }

   if (CurY<0) CurY=0;
   CurY=ScrToTwipsY(CurY);

   if (pPage) (*pPage)=page;
   if (pOff)  (*pOff)=CurY;

   return TRUE;
}

/******************************************************************************
   TerPosPage:
   Position at the specified page number.
*******************************************************************************/
BOOL WINAPI _export TerPosPage(HWND hWnd,int NewPage)
{
   return TerSetPagePos(hWnd,NewPage,0);
}

/******************************************************************************
   TerSetPagePos:
   Position at the specified page number at the specified offset within the
   page. The offset is specified in twips.
*******************************************************************************/
BOOL WINAPI _export TerSetPagePos(HWND hWnd,int NewPage, int PageY)
{
   int CurY,SavePage;
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

   if (!TerArg.PageMode) return FALSE;

   // calculate the new y
   PageY=TwipsToScrY(PageY);

   // position at the page
   SavePage=CurPage;
   CurPage=NewPage;
   if (SavePage!=CurPage || CurPage!=FirstFramePage) {
      CreateFrames(w,FALSE,CurPage,CurPage+1);     // create the text frames
   }

   // set y origin
   if (CurPage==FirstFramePage) TerWinOrgY=(int)PageY;
   else                         TerWinOrgY=FirstPageHeight+(int)PageY;
   if (TerWinOrgY>=(CurPageHeight-TerWinHeight) && CurPageHeight>=TerWinHeight) TerWinOrgY=CurPageHeight-TerWinHeight;

   SetTerWindowOrg(w);

   // Get the current line
   CurY=TerWinOrgY;
   if (CurY>=CurTextHeight) CurY=CurTextHeight-1;
   CurLine=UnitsToLine(w,0,CurY);
   CurCol=0;

   if (CaretEngaged) DisengageCaret(w); // disengage caret to keep TerWinOrgY from changing during display

   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   TerPageFromLine:
   This function returns the page number that contains the given line number.
*******************************************************************************/
int WINAPI _export TerPageFromLine(HWND hWnd, long LineNo)
{
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

   return PageFromLine(w,LineNo,-1);
}

/******************************************************************************
   PageFromLine:
   This function returns the page number that contains the given line number.
   When the previous page >=0 then the PrevPage is returned if the LineNo is
   a hdr/ftr line and the PrevPage has the same section as the LineNo.
*******************************************************************************/
PageFromLine(PTERWND w, long LineNo, int PrevPage)
{
    int PageNo=0;

    if (LineNo<0) LineNo=0;
    if (LineNo>=TotalLines) LineNo=TotalLines-1;

    if (  PrevPage>=0 && PrevPage<TotalPages
       && PfmtId[pfmt(LineNo)].flags&PAGE_HDR_FTR
       && GetSection(w,LineNo)==GetSection(w,PageInfo[PrevPage].FirstLine)) PageNo=PrevPage;
    else {
       if (cid(LineNo)) PageNo=LinePage(LineNo);
       else {
          for (PageNo=0;PageNo<TotalPages;PageNo++) if (PageInfo[PageNo].FirstLine>LineNo) break;
          if (PageNo>0) PageNo--;
       }
    }

    // check for header/footer line when first page header footer is also available
    return AdjustPageNbr(w,PageNo,LineNo);
}

/******************************************************************************
   AdjustPageNbr:
   This function adjusts page number for header/footer
*******************************************************************************/
AdjustPageNbr(PTERWND w, int PageNo, long LineNo)
{
    if (LineNo<0 || LineNo>=0) return PageNo;
    if (PageNo<0 || PageNo>=TotalPages) return 0;

    if (PfmtId[pfmt(LineNo)].flags&PAGE_HDR_FTR && (PageNo+1)<TotalPages) {
       int sect=PageInfo[PageNo].TopSect;
       if (sect<0 || sect>=TotalSects) return PageNo;
       if (LineNo>=TerSect1[sect].hdr.FirstLine && LineNo<=TerSect1[sect].hdr.LastLine) {
          if (TerSect1[sect].fhdr.FirstLine>=0) PageNo++;
       }
       if (LineNo>=TerSect1[sect].ftr.FirstLine && LineNo<=TerSect1[sect].ftr.LastLine) {
          if (TerSect1[sect].fftr.FirstLine>=0) PageNo++;
       }
    }
    return PageNo;
}

/******************************************************************************
   PageTextWidth:
   This routine returns the width of the printed text on the current page. The
   value is returned in the screen units.
*******************************************************************************/
PageTextWidth(PTERWND w)
{
   int sect,MinWidth=0,width=0,i,LastX;

   if (!TerArg.FittedView) {
      sect=TerGetPageSect(hTerWnd,CurPage);
      if (BorderShowing) MinWidth=(int)(ScrResX*TerSect1[sect].PgWidth)+2*PrtToScrX(LeftBorderWidth);
      else               MinWidth=(int)(ScrResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin));
   }
   else MinWidth=TerWinWidth;

   // width the width of the frames in the visible area
   for (i=0;i<TotalFrames;i++) {
      if (frame[i].empty) continue;
      if (frame[i].y>(TerWinOrgY+TerWinHeight)) continue;
      if ((frame[i].y+frame[i].height)<TerWinOrgY) continue;
      LastX=frame[i].x+frame[i].width;
      if (LastX>width) width=LastX;
   }

   if (width<MinWidth) width=MinWidth;

   return width;
}

/******************************************************************************
   TogglePageBorder:
   Toggle the display of the page border in the page mode
*******************************************************************************/
TogglePageBorder(PTERWND w)
{
   if (!TerArg.WordWrap || !TerArg.PageMode || TerArg.FittedView) return FALSE;

   ShowPageBorder=!ShowPageBorder;
   if (!ShowPageBorder) BorderShowing=FALSE;

   TerRepaginate(hTerWnd,TRUE);
   return TRUE;
}

/*****************************************************************************
   Update Page Number:
   Update the page number string
******************************************************************************/
UpdateDynField(PTERWND w, long line, int PageNo)
{
   int CurFieldId,i,pos,NextFont,FieldFont,SaveModified,CurFont,PrevFont,ListNbr=0,NumType;
   int len,StartPos=0,CurLen,NewNumLen,NewCountLen,NewLen,NumLen,nbr,TopSect;
   LPBYTE ptr,FieldCode;
   LPWORD fmt;
   char NewNumString[50],NewCountString[50];
   LPBYTE NewString;
   long SaveLine=line;

   // get the current page number string
   nbr=PageInfo[PageNo].DispNbr;
   TopSect=PageInfo[PageNo].TopSect;
   NumType=TerSect[TopSect].PageNumFmt;

   if      (NumType==NBR_UPR_ALPHA) AlphaFormat(NewNumString,nbr,TRUE);
   else if (NumType==NBR_LWR_ALPHA) AlphaFormat(NewNumString,nbr,FALSE);
   else if (NumType==NBR_UPR_ROMAN) romanize(NewNumString,nbr,TRUE);
   else if (NumType==NBR_LWR_ROMAN) romanize(NewNumString,nbr,FALSE);
   else                             wsprintf(NewNumString,"%d",nbr);

   NewNumLen=lstrlen(NewNumString);
   
   nbr=(OverrideTotalPages>0?OverrideTotalPages:TotalPages);
   if      (NumType==NBR_UPR_ALPHA) AlphaFormat(NewCountString,nbr,TRUE);
   else if (NumType==NBR_LWR_ALPHA) AlphaFormat(NewCountString,nbr,FALSE);
   else if (NumType==NBR_UPR_ROMAN) romanize(NewCountString,nbr,TRUE);
   else if (NumType==NBR_LWR_ROMAN) romanize(NewCountString,nbr,FALSE);
   else                             wsprintf(NewCountString,"%d",nbr);
   NewCountLen=lstrlen(NewCountString);

   // size the page strings
   wsprintf(TempString,"%d",PrevTotalPages);     // to provide stable field length
   len=lstrlen(TempString);
   if (NewNumLen<len) {
      lstrcpy(TempString,NewNumString);
      for (i=NewNumLen;i<len;i++) NewNumString[i-NewNumLen]=' ';
      NewNumString[i-NewNumLen]=0;
      lstrcat(NewNumString,TempString);
      NewNumLen=lstrlen(NewNumString);
   }
   if (NewCountLen<len) {
      lstrcpy(TempString,NewCountString);
      for (i=NewCountLen;i<len;i++) NewCountString[i-NewCountLen]=' ';
      NewCountString[i-NewCountLen]=0;
      lstrcat(NewCountString,TempString);
      NewCountLen=lstrlen(NewCountString);
   }

   NEXT_FIELD:

   // check if at the end of the line
   len=LineLen(line);
   if (len==0) return TRUE;
   if (StartPos>=len) {      // this line ends with a page number character
      if ((line+1)>=TotalLines) return TRUE;

      // delete any page number string from the beginning of the next line
      NextFont=GetCurCfmt(w,line+1,0);
      if (TerFont[NextFont].FieldId!=FIELD_PAGE_NUMBER && TerFont[NextFont].FieldId!=FIELD_PAGE_COUNT) return TRUE;

      // first character is a page number
      fmt=OpenCfmt(w,line+1);
      NumLen=0;
      for (i=0;i<LineLen(line+1);i++) {
         if (TerFont[fmt[i]].FieldId!=FIELD_PAGE_NUMBER && TerFont[fmt[i]].FieldId!=FIELD_PAGE_COUNT) break;
         NumLen++;
      }
      CloseCfmt(w,line+1);

      SaveModified=TerArg.modified;                       // not user modification
      if (NumLen>0) MoveLineData(w,line+1,0,NumLen,'D');  // delete characters
      TerArg.modified=SaveModified;                       // restore modification flag

      return TRUE;
   }

   // find the position of the page number string
   fmt=OpenCfmt(w,line);
   for (i=StartPos;i<len;i++) {
      int FieldId=TerFont[fmt[i]].FieldId;
      if (IsDynField(w,FieldId)) {
         if (FieldId==FIELD_DATE && TerFlags4&TFLAG4_DISABLE_DATE_UPDATE) continue;

         if (FieldId!=FIELD_PRINTDATE) break;
         else if (InPrinting && !InPrintPreview) break;  // FIELD_PRINTDATE result changed only while printing
      }
   }
   if (i==len) {
      CloseCfmt(w,line);
      return TRUE;    // no more page number found
   }

   // find the length of the page number string
   FieldFont=fmt[i];
   CurFieldId=TerFont[fmt[i]].FieldId;
   FieldCode=TerFont[fmt[i]].FieldCode;
   pos=i;
   CurLen=1;
   for (i=pos+1;i<len;i++) {
      if (TerFont[fmt[i]].FieldId!=CurFieldId) break;
      CurLen++;
   }
   CloseCfmt(w,line);

   // process the date fields
   if (    CurFieldId==FIELD_DATE || CurFieldId==FIELD_PRINTDATE 
        || CurFieldId==FIELD_LISTNUM || CurFieldId==FIELD_AUTONUMLGL
        || CurFieldId==FIELD_PAGE_REF) {
      BYTE FieldString[40];
      if (i==len) {        // scan the next line also because date fields can wrap
         long l=line+1;
         int  col=0;
         while (TRUE) {
            CurFont=GetCurCfmt(w,l,col);
            if (TerFont[CurFont].FieldId!=CurFieldId) break;
            CurLen++;
            if (!NextTextPos(w,&l,&col)) break;
         }
      }
      
      // check if this field continues from the previous position
      PrevFont=GetPrevCfmt(w,line,pos);
      if (TerFont[PrevFont].FieldId==CurFieldId) {   // continuing field
         StartPos=pos+CurLen;
         if (StartPos>=len) return TRUE;             // line processed
         else goto NEXT_FIELD;
      } 

      // refresh the date string
      if (CurFieldId==FIELD_DATE || CurFieldId==FIELD_PRINTDATE) GetDateString(w,FieldCode,FieldString,FieldFont);
      else if (CurFieldId==FIELD_LISTNUM) {
         lstrcpy(FieldString,"List");
         if (tabw(line) && ListNbr<tabw(line)->ListnumCount) {
            lstrcpy(FieldString,tabw(line)->pListnum[ListNbr].text);
            ListNbr++;
         } 
      } 
      else if (CurFieldId==FIELD_AUTONUMLGL) {
         lstrcpy(FieldString,"1");
         if (tabw(line) && tabw(line)->pAutoNumLgl) {
            lstrcpy(FieldString,tabw(line)->pAutoNumLgl);
            if (!FieldCode || strstr(FieldCode,"\\e")==NULL) lstrcat(FieldString,".");
             
         } 
      }
      else if (CurFieldId==FIELD_PAGE_REF && TerArg.PageMode) {
         BOOL IsAlphabetic=(strstr(FieldCode,"\\* alphabetic")!=null);
         int  FieldCodeLen=lstrlen(FieldCode),BkmPage=0;
         BYTE BkmName[MAX_WIDTH+1];

         for (i=0;i<FieldCodeLen && FieldCode[i]!=' ';i++) BkmName[i]=FieldCode[i];
         BkmName[i]=0;     // bookmark
         for (i=1;i<TotalCharTags;i++) {
            if (CharTag[i].InUse && CharTag[i].type==TERTAG_BKM && lstrcmp(CharTag[i].name,BkmName)==0) break;
         }
         if (i<TotalCharTags) {
            long line=CharTag[i].line;
            if (line>=0 && line<TotalLines) BkmPage=LinePage(line);
         }
         if (BkmPage<0) BkmPage=0;
         if (BkmPage>=TotalPages) BkmPage=TotalPages-1;
         BkmPage=PageInfo[BkmPage].DispNbr;

         if (IsAlphabetic) AlphaFormat(FieldString,BkmPage,false);
         else              wsprintf(FieldString,"%d",BkmPage);
      } 
      
      if (lstrlen(FieldString)==0) StartPos=pos+CurLen;       // don't replace the existing string
      else {
         ReplaceTextInPlace(w,&line,&pos,CurLen,FieldString,NULL);        // replace text
         StartPos=pos;
      }
      if (line>SaveLine) return TRUE;                    // went to next line
      goto NEXT_FIELD;
   } 

   // adjust the string
   if (CurFieldId==FIELD_PAGE_NUMBER) NewLen=NewNumLen;
   else                           NewLen=NewCountLen;
   if (CurFieldId==FIELD_PAGE_NUMBER) NewString=NewNumString;
   else                           NewString=NewCountString;

   if (NewLen<=CurLen) {   // same length string, just update the text
      ptr=pText(line);
      for (i=pos;i<(pos+NewLen);i++) ptr[i]=NewString[i-pos];
      StartPos=i;             // position beyond the string
      if (NewLen<CurLen) { // delete extract characters
         SaveModified=TerArg.modified;      // not a user modification
         MoveLineData(w,line,StartPos,CurLen-NewLen,'D');
         TerArg.modified=SaveModified;
      }
   }
   else {                    // replace with bigger string
      ptr=pText(line);
      for (i=pos;i<(pos+CurLen);i++) ptr[i]=NewString[i-pos];   // replace the common length
      StartPos=i;


      // insert spaces
      SaveModified=TerArg.modified;      // not a user modification
      MoveLineData(w,line,StartPos,NewLen-CurLen,'B');
      TerArg.modified=SaveModified;

      ptr=pText(line);
      fmt=OpenCfmt(w,line);
      for (i=StartPos;i<(pos+NewLen);i++) {
         ptr[i]=NewString[i-pos];
         fmt[i]=fmt[i-1];
      }
      CloseCfmt(w,line);
      StartPos=i;
   }

   goto NEXT_FIELD;                // process next page number string
}

/****************************************************************************
    TerGetPageOrient:
    This function returns the orientation (DMORIENT_PORTRAIT or DMORIENT_LANDSCAPE)
    for the given page.
*****************************************************************************/
int WINAPI _export TerGetPageOrient(HWND hWnd,int page)
{
    return TerGetPageOrientEx(hWnd,page,NULL,NULL);
}

/****************************************************************************
    TerGetPageOrientEx:
    This function returns the orientation (DMORIENT_PORTRAIT or DMORIENT_LANDSCAPE)
    for the given page.  Width and height are returned in twips units.
*****************************************************************************/
int WINAPI _export TerGetPageOrientEx(HWND hWnd,int page, LPINT pWidth, LPINT pHeight)
{
   return TerGetPageOrient2(hWnd,page,pWidth,pHeight,NULL,NULL);
}

/****************************************************************************
    TerGetPageOrient2:
    This function returns the orientation (DMORIENT_PORTRAIT or DMORIENT_LANDSCAPE)
    for the given page.  Width and height are returned in twips units.
*****************************************************************************/
int WINAPI _export TerGetPageOrient2(HWND hWnd,int page, LPINT pWidth, LPINT pHeight, LPINT pHiddenX, LPINT pHiddenY)
{
    PTERWND w;
    int sect;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (page<0) page=0;
    if (page>=TotalPages) page=TotalPages-1;

    sect=TerGetPageSect(hWnd,page);

    if (pWidth) (*pWidth)=(int)(TerSect1[sect].PgWidth*1440);
    if (pHeight) (*pHeight)=(int)(TerSect1[sect].PgHeight*1440);

    if (pHiddenX) (*pHiddenX)=PrtToTwipsX(TerSect1[sect].HiddenX);
    if (pHiddenY) (*pHiddenY)=PrtToTwipsY(TerSect1[sect].HiddenY);

    return TerSect[sect].orient;
}


/****************************************************************************
    TerGetPageFirstLine:
    Get the first line of the specified page number.
*****************************************************************************/
long WINAPI _export TerGetPageFirstLine(HWND hWnd,int page)
{
    PTERWND w;
    long line;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (page<0) page=0;
    if (page>(TotalPages-1)) page=TotalPages-1;

    line=PageInfo[page].FirstLine;

    return line;
}

/****************************************************************************
    TerGetDispPageNo:
    Get the display page number.
*****************************************************************************/
int WINAPI _export TerGetDispPageNo(HWND hWnd,int page)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (page<0 || page>=TotalPages) return 0;
    return PageInfo[page].DispNbr;
}

/******************************************************************************
    IsLastSpannedCell:
    Returns TRUE if the given cell is the last spanned cell
******************************************************************************/
BOOL IsLastSpannedCell(PTERWND w, int cl)
{
    return (cell[cl].flags&CFLAG_ROW_SPANNED && GetRemainingCellSpans(w,cl)<=1);
}
    
/******************************************************************************
    GetRemainingCellSpans:
    Remaing cell spans for a spanned row
******************************************************************************/
int GetRemainingCellSpans(PTERWND w, int CurCell)
{
    int SpanCount,cl,CurRowId,row;

    CurRowId=cell[CurCell].row;

    cl=CellAux[CurCell].SpanningCell;    // top cell
    SpanCount=cell[cl].RowSpan;
    row=cell[cl].row;

    while(row!=CurRowId && row>0) {
       SpanCount--;
       row=TableRow[row].NextRow;
    }

    if (SpanCount<1) SpanCount=1;

    return SpanCount;
}

/******************************************************************************
    IsSpannedRow:
    This function returns TRUE if any cell in this row is spanned. To specify
    a cell in the row, set the second parameter to a negative value.
******************************************************************************/
BOOL IsSpannedRow(PTERWND w, int row)
{
    int TempCell;

    if (row<0) row=cell[-row].row;

    TempCell=TableRow[row].FirstCell;
    while (TempCell>0) {
       if (cell[TempCell].flags&CFLAG_ROW_SPANNED) return TRUE;
       TempCell=cell[TempCell].NextCell;
    }

    return FALSE;
}

/******************************************************************************
    IsSpanningRow:
    This function returns TRUE if any cell in this row spans other cells. To specify
    a cell in the row, set the second parameter to a negative value.
******************************************************************************/
BOOL IsSpanningRow(PTERWND w, int row)
{
    int TempCell;

    if (row<0) row=cell[-row].row;

    TempCell=TableRow[row].FirstCell;
    while (TempCell>0) {
       if (cell[TempCell].RowSpan>1) return TRUE;
       TempCell=cell[TempCell].NextCell;
    }

    return FALSE;
}

/******************************************************************************
    InsertDynField:
    Insert page number, page count, date/time in the text.
******************************************************************************/
InsertDynField(PTERWND w, int FieldId, LPBYTE FieldCode)
{
    char FieldString[50];
    int  i,FieldLen;
    int  NewFont;
    LPBYTE ptr;
    LPWORD fmt;

    if (LineLen(CurLine)>0 && CurCol>=LineLen(CurLine)) return TRUE;

    // get the new font id
    HilightType=HILIGHT_OFF;     // turn off highlight
    if (!CharFmt(w,GetNewFieldId,(DWORD)FieldId,(DWORD)FieldCode,FALSE)) return FALSE;
    
    NewFont=InputFontId;
    InputFontId=-1;
    
    if (NewFont<0 || TerFont[NewFont].FieldId!=FieldId) return FALSE;
    NewFont=SetCurLangFont(w,NewFont);   // set font for the currently selected language

    // make the current page number string
    if (FieldId==FIELD_DATE) GetDateString(w,FieldCode,FieldString,NewFont);
    else                     wsprintf(FieldString,"%d",CurPage+1);
    FieldLen=lstrlen(FieldString);

    // insert the string in the text
    MoveLineData(w,CurLine,CurCol,FieldLen,'B'); // insert space before the current column

    SaveUndo(w,CurLine,CurCol,CurLine,CurCol+FieldLen-1,'I');  // record for undo

    ptr=pText(CurLine);
    fmt=OpenCfmt(w,CurLine);

    for (i=CurCol;i<(CurCol+FieldLen);i++) {
       ptr[i]=FieldString[i-CurCol];
       fmt[i]=NewFont;
    }

    CurCol+=FieldLen;                            // go past the page number

    CloseCfmt(w,CurLine);

    PaintTer(w);                               // repaint the screen

    return TRUE;
}

/******************************************************************************
   TerGetPageOffset:
   Get the offset of the beginning of a page relative to the window or the 
   text box . Set PageNo to -1 to specify the current page. 
*******************************************************************************/
BOOL WINAPI _export TerGetPageOffset(HWND hWnd, int PageNo, int rel, LPINT pX, LPINT pY, LPINT pWidth, LPINT pHeight)
{
   PTERWND w;
   BOOL visible=true;
   int x,y,width,height,sect;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   // initialize the out variables
   x=y=width=height=0;

   if (!TerArg.PageMode) return false;
   if (rel!=REL_WINDOW && rel!=REL_TEXT_BOX) return false;


   if (PageNo<0) PageNo=CurPage;
   if (PageNo<FirstFramePage || PageNo>LastFramePage) return false;  // page not visible
     
   // calculate y
   y=0;  // page y relative to the frame set
   if (PageNo>FirstFramePage) y+=FirstPageHeight;
   if (BorderShowing) y+=PrtToScrY(TopBorderHeight);
   y-=TerWinOrgY;            // relative to the top of the text box
     
   if (y>TerWinHeight) visible=false;
   if (y<0 && visible) {            // check if page visible
      height=FirstPageHeight;
      if (BorderShowing) height-=PrtToScrY(TopBorderHeight+BotBorderHeight);
      if ((y+height)<0) visible=false;
   } 
   if (rel==REL_WINDOW)  y+=TerWinRect.top;
     
   // calculate x
   x=0;                 // relative to the frame set
   if (BorderShowing) x+=PrtToScrX(LeftBorderWidth);
   x-=TerWinOrgX;     // relative to the text box
   if (x>TerWinWidth) visible=false;
   if (rel==REL_WINDOW) x+=TerWinRect.left;

   // calculate height
   if (PageNo==FirstFramePage) height=FirstPageHeight;
   else height=frame[TotalFrames-1].y+frame[TotalFrames-1].height-frame[FirstPage2Frame].y;
   if (BorderShowing) {
      height-=PrtToScrY(TopBorderHeight);
      height-=((PageNo+1)==TotalPages?PrtToScrY(TopBorderHeight):PrtToScrY(BotBorderHeight));
   }

   // calcualte width;
   sect=PageInfo[PageNo].TopSect;
   width=(int)(TerSect1[sect].PgWidth*ScrResX);

   (*pX)=x;
   (*pY)=y;
   (*pWidth)=width;
   (*pHeight)=height;

   return visible;
}

/******************************************************************************
    CheckPageSpace:
    Enlarge page space if necessary.
******************************************************************************/
CheckPageSpace(PTERWND w, int LastPage)
{
    int NewMaxPages;
    int i,PrevMax=MaxPages;

    if ((LastPage+1)<MaxPages) return TRUE;

    NewMaxPages=MaxPages+5;
    if (NewMaxPages<=LastPage) NewMaxPages=LastPage+1;
    if (NewMaxPages>MAX_PAGES) NewMaxPages=MAX_PAGES;
    if (NewMaxPages<=MaxPages) return TRUE;            // no need to enlarge

    ExpandArray(w,sizeof(struct StrPage),(void far *(huge *))&PageInfo,&MaxPages,NewMaxPages,MAX_PAGES);

    for (i=PrevMax;i<MaxPages;i++) FarMemSet(&(PageInfo[i]),0,sizeof(struct StrPage));

    return TRUE;
}


/******************************************************************************
    IsPageLastRow:
    return sTRUE if this is the last row on the page.
******************************************************************************/
BOOL IsPageLastRow(PTERWND w, int row, int PageNo)
{
    int LastRow,CurCell;

    if (row<0) CurCell=-row;
    else       CurCell=TableRow[row].FirstCell;
    LastRow=LevelRow(w,cell[CurCell].level,PageInfo[PageNo].LastRow); // last row of the page at the current level
    
    if (row<0) {   // cell number specified
       int CellId=-row,span;
       row=cell[CellId].row;
       if (cell[CellId].flags&CFLAG_ROW_SPANNED) span=GetRemainingCellSpans(w,CellId);
       else                                      span=cell[CellId].RowSpan;
       while (span>1) {
          if  (row==LastRow) return TRUE;
          row=TableRow[row].NextRow;
          if (row<=0) return FALSE;
          span--;
       }
    }

    return (/*row==PageInfo[PageNo].BotRow || */row==LastRow);
}

/******************************************************************************
    GetPageMultiple:
    Get multiple by which to increase the page size
******************************************************************************/
int GetPageMultiple(PTERWND w)
{
    if (!TerArg.FittedView || InPrinting) return 1;

    //return (HtmlMode?(FITTED_VIEW_DISP_PAGES*2):FITTED_VIEW_DISP_PAGES);
    return (FITTED_VIEW_DISP_PAGES);
} 

/******************************************************************************
    ExtractFootnote:
    Retrieve the height of the footnote text.
******************************************************************************/
ExtractFootnote(PTERWND w, HDC hDC, int x, int y, long line, int sect, BOOL screen,BOOL draw)
{
    int FnoteHeight=0;
    int i,j,CurFont,PrevFont,len,WrapWidth;
    int CurWidth;
    int BufLen;
    UINT CurStyle,PrevStyle;
    BYTE CurLead,CurChar;
    BOOL   InFnoteText,SaveInPrinting=InPrinting,GetCharWidth;
    long l=line;
    LPWORD pWidth=NULL;

    dm("ExtractFootnote");

    if (TerArg.FittedView) return 0;
    if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) return 0;
    if (!WrapCharWidth && (NULL==(WrapCharWidth=OurAlloc((long)WrapBufferSize*sizeof(WORD))))) return 0;

    SetFnoteFontInfo(w,TRUE);    // activate the hidden height/width information for the hidden fonts

    // calculate the wrap width
    WrapWidth=(int)((TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin)*PrtResX);

    len=LineLen(line);

    for (i=0;i<len;i++) {
       if (l>line) break;

       CurFont=GetCurCfmt(w,l,i);
       CurStyle=TerFont[CurFont].style;

       if (i==0 && IsFootnoteStyle(CurStyle)) {   // check if it is a continuing footnote from the previous line
          PrevFont=GetPrevCfmt(w,l,i);
          PrevStyle=TerFont[PrevFont].style;
          if (IsFootnoteStyle(PrevStyle)) {       // continuing font
             while (i<len) {
                CurFont=GetCurCfmt(w,l,i);
                if (!IsFootnoteStyle(TerFont[CurFont].style)) break;
                i++;
             }
          }
          if (i==len) break;
       }

       // look for the beginning of the next footnote text
       while (i<len) {
          CurFont=GetCurCfmt(w,l,i);
          if (IsFootnoteStyle(TerFont[CurFont].style)) break;
          i++;
       }
       if (i==len) break;                 // no more footnote text found

       // a footnote text begins here
       j=0;
       GetCharWidth=TRUE;

       while (i<len) {                   // retrieve this footnote
          if (GetCharWidth) {
             InPrinting=!screen;  // TRUE;  // to get the character width in printing units
             pWidth=GetLineCharWidth(w,l);
             InPrinting=SaveInPrinting;
             GetCharWidth=FALSE;
          } 
          
          CurFont=GetCurCfmt(w,l,i);
          CurLead=GetCurLead(w,l,i);
          CurChar=GetCurChar(w,l,i);

          InFnoteText=IsFootnoteStyle(TerFont[CurFont].style);

          if (TRUE || (TerFont[CurFont].rtl && pWidth)) 
                CurWidth=pWidth[i];
          else  CurWidth=DblCharWidth(w,CurFont,screen/*FALSE*/,CurChar,CurLead);

          if (!InFnoteText || j>=WrapBufferSize) {
             BufLen=j;

             // parse the wrap buffer to extract the lines
             wrap[BufLen]=0;
             FnoteHeight+=DrawOneFootnote(w,hDC,x,y+FnoteHeight,line,BufLen,WrapWidth,screen,draw);

             if (!InFnoteText) break;  // end of this footnote
             j=0;                      // begin the next wrap buffer
          }

          wrap[j]=CurChar;
          if (wrap[j]==HPARA_CHAR) wrap[j]=ParaChar;
          WrapLead[j]=CurLead;
          WrapCfmt[j]=CurFont;
          WrapCharWidth[j]=CurWidth;
          j++;
          i++;

          if (i==len) {                   // fnote continues into the next line
             l++;
             if (l>=TotalLines) break;
             len=LineLen(l);              // process the next line now
             i=0;
             if (pWidth) MemFree(pWidth);
             pWidth=NULL;
             GetCharWidth=TRUE;
          }
       }
    }

    SetFnoteFontInfo(w,FALSE);

    if (pWidth) MemFree(pWidth);
    pWidth=NULL;

    return FnoteHeight;
}

