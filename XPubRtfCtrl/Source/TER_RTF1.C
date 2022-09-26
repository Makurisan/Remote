/*============================================================================
   TER_RTF1.C
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
    ImportRtfTextLine:
    This routine is called by the rtf reader (RtfRead) to process the rtf
    data as it reads from the file.  It is also called to initialize and
    end the reading process.
******************************************************************************/
BOOL ImportRtfTextLine(PTERWND w,struct StrRtfGroup far *group,LPBYTE text)
{
    int   len,CurFont,FieldId;
    LPBYTE ptr,lead;
    BYTE   LastChar;
    struct StrRtf far *rtf;
    long  l,InsLine;                            // line number to insert
    BOOL  HasLead=FALSE,SetSectTag=FALSE;
    BYTE  LeadText[MAX_WIDTH+1];
    int   i,adj;

    rtf=group->rtf;                           // pointer to the rtf data structure
    
    if (-1==(InsLine=GetRtfInsertionLine(w,rtf))) return FALSE;

    // set the paraframe id
    SetRtfParaFID(w,InsLine,group);

    // mark footnote reference
    if (group->style&FNOTETEXT) SetRtfFootnote(w,group,InsLine-1);

    // adjust length for any unicode characters processed
    if (group->IgnoreCount>0 /*&& group->UcBase==0*/) {
       len=rtf->OutBufLen;
       adj=(len>group->IgnoreCount)?group->IgnoreCount:len;
       for (i=adj;i<len;i++) {
         rtf->OutBuf[i-adj]=rtf->OutBuf[i];
       }
       rtf->OutBuf[i-adj]=0; 
       rtf->OutBufLen=i-adj;
       group->IgnoreCount-=adj; 
    } 

    // decode mbcs characters
    if (group->gflags&GFLAG_CAPS && rtf->OutBufLen>0) lstrupr(w,rtf->OutBuf);
    if (mbcs && group->UcBase==0 && False(group->gflags2&GFLAG2_NO_MBCS)) HasLead=SplitRtfMbcsText(w,rtf,text,LeadText,group);

    if (mbcs && HasLead && (group->CharSet==ANSI_CHARSET || group->CharSet==DEFAULT_CHARSET)) group->CharSet=rtf->CharSet;

    // adjust length for any unicode characters processed
    if (FALSE && group->IgnoreCount>0 && group->UcBase==0) {
       len=lstrlen(text);
       adj=(len>group->IgnoreCount)?group->IgnoreCount:len;
       for (i=adj;i<len;i++) {
         text[i-adj]=text[i];
         LeadText[i-adj]=LeadText[i];
       }
       text[i-adj]=0; 
       LeadText[i-adj]=0;
       group->IgnoreCount-=adj; 
    } 

    if (group->caps) strupr(text);
    len=lstrlen(text);

    // check if line break follows lbr
    if (len==1 && text[0]==LINE_CHAR && group->UcBase==0 && group->CharId==0 && InsLine>0 && LineLen(InsLine-1)>0) {
       int PrevLen=LineLen(InsLine-1);
       LPBYTE PrevPtr=pText(InsLine-1);
       if (PrevPtr[PrevLen-1]==LINE_CHAR) {
          LPWORD PrevFmt=OpenCfmt(w,InsLine-1);
          int PrevCharId=TerFont[PrevFmt[PrevLen-1]].CharId;
          if (PrevCharId>0) {
             text[0]=0;    // ignore this line break
             len=lstrlen(text);
          } 
       } 
    } 

    //if (len==0) return TRUE;        // a blank line might already have been ineserted in the GetRtfInsertionLine function, so continue through the process

    cfmt(InsLine).info.type=UNIFORM;   // to stop LineAlloc to call FmtAlloc
    cfmt(InsLine).info.fmt=DEFAULT_CFMT;
    pLead(InsLine)=NULL;
    LineAlloc(w,InsLine,0,len);       // allocate space for the line

    ptr=pText(InsLine);
    if (len>0) FarMove(text,ptr,len);

    if (HasLead) {
       LeadAlloc(w,InsLine,0,len);
       lead=pLead(InsLine);
       FarMove(LeadText,lead,len);
    }

    // get the font id
    CurFont=GetRtfFontId(w,rtf,group); 
    FieldId=TerFont[CurFont].FieldId;

    // check if picture id specified to use as a font
    if (group->PictId>0 && TerFont[group->PictId].InUse && TerFont[group->PictId].style&PICT) {
       CurFont=group->PictId;
    }

    cfmt(InsLine).info.type=UNIFORM;
    cfmt(InsLine).info.fmt=CurFont; // current font


    LineY(InsLine)=0;               // initialize line y position
    LineX(InsLine)=0;               // initialize line x position
    LineHt(InsLine)=PrtFont[CurFont].height;  // initial height of the line
    cid(InsLine)=RtfCurCellId;     // assign the cell id


    // set the para id
    SetRtfParaId(w,InsLine,group);

    // section section marker for section breaks
    if (lstrlen(text)==1) {
       if (text[0]==SECT_CHAR && AllocTabw(w,InsLine)) {
           tabw(InsLine)->type=INFO_SECT;
           tabw(InsLine)->section=WrapSect;
           tabw(InsLine)->count=0;                 // tabs not used

           SetSectTag=TRUE;
       }
       if (text[0]==SECT_CHAR && InsLine>0) pfmt(InsLine)=pfmt(InsLine-1);  // borrow it from the previous line
       
       if (text[0]==SECT_CHAR || text[0]==PAGE_CHAR) fid(InsLine)=0;

       if (text[0]==ROW_CHAR && InsLine>0 && fid(InsLine)!=fid(InsLine-1)) {
          fid(InsLine)=RtfParaFID=fid(InsLine-1);
          if (RtfParaFID>0) FarMove(&PrevRtfParaFrameInfo,&RtfParaFrameInfo,sizeof(RtfParaFrameInfo));
       }

       // add a space character to an empty hdr/ftr area
       if (text[0]==HDR_CHAR || text[0]==FTR_CHAR) {
          if ((InsLine-2)>=0 && LineLen(InsLine-2)==1 && LineLen(InsLine-1)==1) {
             BYTE chr=pText(InsLine-2)[0];
             if (chr==HDR_CHAR || chr==FTR_CHAR) {  // this header/footer has only a \par, add a space
                LPBYTE ptr=pText(InsLine-1);
                BYTE chr=ptr[0];
                if (chr==PARA_CHAR) {
                   MoveLineData(w,InsLine-1,0,1,'B');  // scroll to make a space for a space character
                   ptr=pText(InsLine-1);
                   ptr[0]=' ';
                } 
             }  
          }     
       } 

       SetHdrFtrLineFlags(w,InsLine,text[0]);
    }

    // set break flags and normalize the para ids
    len=lstrlen(text);
    ResetUintFlag(rtf->flags1,RFLAG1_PARA_ENDED);

    if (len>0) {
       LastChar=text[len-1];
       if (LastChar==ParaChar) rtf->flags1|=RFLAG1_PARA_ENDED;

       if (LastChar==ROW_CHAR  || LastChar==CellChar) {
          if (tabw(InsLine)==0) AllocTabw(w,InsLine);
          if (tabw(InsLine)) {
              if (LastChar==ROW_CHAR)      tabw(InsLine)->type|=INFO_ROW;
              else if (LastChar==CellChar) tabw(InsLine)->type|=INFO_CELL;
          }
       }
       SetHdrFtrLineFlags(w,InsLine,LastChar);

       // set the paraflags
       if (LastChar==ParaChar || LastChar==CellChar) LineFlags(InsLine)|=LFLAG_PARA;
       if (FieldId==FIELD_TOC)        LineFlags(InsLine)|=LFLAG_TOC;
       if (FieldId==FIELD_LISTNUM)    LineFlags(InsLine)|=LFLAG_LISTNUM;
       if (FieldId==FIELD_AUTONUMLGL) LineFlags(InsLine)|=LFLAG_AUTONUMLGL;
       if (FieldId==FIELD_TC)         LineFlags2(InsLine)|=LFLAG2_TC;
       
       rtf->PrevField=FieldId;

       if ( LastChar==PAGE_CHAR || LastChar==SECT_CHAR || LastChar==COL_CHAR
         || LastChar==ROW_CHAR || IsHdrFtrChar(LastChar)) LineFlags(InsLine)|=LFLAG_BREAK;
       if (LastChar==SECT_CHAR) LineFlags(InsLine)|=LFLAG_SECT;

       if (TerFont[CurFont].style&ENOTETEXT) {
          LineFlags2(InsLine)|=LFLAG2_ENOTETEXT;
          TerOpFlags|=TOFLAG_UPDATE_ENOTE;
       }
       else if (TerFont[CurFont].style&FNOTETEXT) LineFlags(InsLine)|=LFLAG_FNOTETEXT;

       // for footnote text para, inherrit the para id from the preceeding paragraph
       // this is done so that a para marker in footnote text does not change the paramarker for the entire para
       if (TerFont[CurFont].style&FNOTETEXT && LineFlags(InsLine)&LFLAG_PARA && InsLine>0) {
          for (l=InsLine-1;l>=0;l--) {
             if (LineFlags(l)&LFLAG_PARA) {
                pfmt(InsLine)=pfmt(l);
                break;
             } 
          } 
       } 

       // assign the uniform para ids for all lines in the paragraph
       if (LineFlags(InsLine)&LFLAG_PARA) {
          l=InsLine-1;
          while (l>=0 && !(LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK))) {
             pfmt(l)=pfmt(InsLine);
             l--;
          }
       }

       // supply missing cell ids
       if (InsLine>0 && LastChar==CellChar && cid(InsLine)>0 && cell[cid(InsLine)].PrevCell>0) {
          int CurCell=cid(InsLine),CurPara;
          for (l=InsLine-1;l>=0;l--) {
             if (tabw(l) && tabw(l)->type&INFO_CELL) break;  // previous cell found
             if (cid(l)==0) {
                cid(l)=CurCell;
                CurPara=pfmt(l);
                if (!(PfmtId[CurPara].pflags&PFLAG_CELL)) pfmt(l)=SetParaParam(w,CurPara,PPARAM_PFLAGS,PfmtId[CurPara].pflags|PFLAG_CELL);
             }
          } 
       } 
       
       // assign correct parent cell id for nested cells
       if (InsLine>0 && LastChar==CellChar && cid(InsLine)>0) {
          int CurCell=cid(InsLine),cl,lvl;
          int CurLevel=cell[CurCell].level;
          for (l=InsLine-1;l>=0 && cid(l)>0;l--) {
             cl=cid(l);
             lvl=cell[cl].level;
             if (cl==CurCell) continue;
             if (lvl<=CurLevel) break;  // into parent cell or a new cell at this level
             if (lvl==(CurLevel+1) && CurLevel>=0) cell[cl].ParentCell=CurCell;   // immediate child found
          } 
       } 

       if (InsLine>0 && LastChar==ROW_CHAR)  {
          int CurFID=0,row,cl;
          pfmt(InsLine)=pfmt(InsLine-1); // assign the paragraph id from the last cell

          // find  fid for all lines in this cell
          row=cell[cid(InsLine)].row;
          for (l=InsLine;l>=0;l--) {     // find the frame id
             if (!(cl=cid(l))) break;
             if (cell[cl].row!=row) {
                if (TableRow[row].PrevRow!=cell[cl].row) break;
                row=cell[cl].row;
             }
             if ((CurFID=fid(l))>0) break;  // frame found
          }
          // apply this fid uniformly for the table
          if (CurFID>0) {
             row=cell[cid(InsLine)].row;
             for (l=InsLine;l>=0;l--) {     // find the frame id
                if (!(cl=cid(l))) break;
                if (cell[cl].row!=row) {
                   if (TableRow[row].PrevRow!=cell[cl].row) break;
                   row=cell[cl].row;
                }
                fid(l)=CurFID;
             }
          }  
       }
    }

    // Set line tags
    if (rtf->TagId) ApplyRtfTagId(w,rtf,InsLine);
    if (SetSectTag) {                                          // set the section tag
       BYTE string[20];

       wsprintf(string,"Section%d",WrapSect);
       SetTag(w,InsLine,0,TERTAG_SECT,string,NULL,WrapSect);   // insert the section tag
    }

    // convert to unicode
    if (TerFlags5&TFLAG5_INPUT_TO_UNICODE) {
       if (mbcs && HasLead) MbcsToUnicode(w);
       else TextToUnicode(w);
    }

    rtf->SomeTextRead=TRUE;     // some rtf text read
    group->UcBase=0;            // text already processed, reset the unicode base


    CurLine++;
    return TRUE;
}

/*****************************************************************************
    MbcsToUnicode:
    Convert mbcs line to unicode.
******************************************************************************/
BOOL MbcsToUnicode(PTERWND w)
{
    LPBYTE ptr=pText(CurLine);
    LPBYTE lead=OpenLead(w,CurLine);
    BYTE   txt,mb[3];      // multi-byte string
    WORD   UcBase,UText[3]; // unicode string
    LPWORD fmt=OpenCfmt(w,CurLine);
    int i,CodePage,count,CharSet;

    for (i=0;i<LineLen(CurLine);i++) {
      if (TerFont[fmt[i]].UcBase!=0) continue;
      if (lead[i] || ptr[i]>=128) {
         CharSet=TerFont[fmt[i]].CharSet;
         if (IsMbcsCharSet(w,CharSet,&CodePage)) {
            if (lead[i]) {
               mb[0]=lead[i];
               mb[1]=ptr[i];
               count=2;
            }
            else {
               mb[0]=ptr[i];
               count=1;
            } 
            if (MultiByteToWideChar(CodePage,0,mb,count,UText,1)==1) {
               CrackUnicode(w,UText[0],&UcBase,&txt);
               fmt[i]=SetUniFont(w,fmt[i],UcBase);
               ptr[i]=txt;
               lead[i]=0;
            } 
         } 
      } 
    }
    CloseLead(w,CurLine);
    CloseCfmt(w,CurLine); 

    return TRUE;
} 

/*****************************************************************************
    TextToUnicode:
    Convert regular text of line to unicode.
******************************************************************************/
BOOL TextToUnicode(PTERWND w)
{
    LPBYTE ptr=pText(CurLine);
    BYTE   txt,mb[3];      // multi-byte string
    WORD   UcBase,UText[3]; // unicode string
    LPWORD fmt=OpenCfmt(w,CurLine);
    int i,CodePage,CharSet;

    for (i=0;i<LineLen(CurLine);i++) {
       if (TerFont[fmt[i]].UcBase!=0) continue;
       CharSet=TerFont[fmt[i]].CharSet;
       if (!mbcs) {    // allow mbcs, because when mbcs is on, certain character with value as as lead char can confure ExtTextOut, so let them be converted unicode
          if (CharSet==2) continue;
          if (CharSet<2) {
             if (ptr[i]<0x80 || ptr[i]>0xA1) continue;     // characters above 0xA1 of ANSI charset set are mapped to indentical unicode values of LATIN-1 Supplement group unicode characters
          }
       }

       IsMbcsCharSet(w,CharSet,&CodePage);  // get the code page
       if (CharSet==77) CodePage=10000;     // mac character set corresponds to code page 10000
       mb[0]=ptr[i];
       if (MultiByteToWideChar(CodePage,0,mb,1,UText,1)==1) {
          CrackUnicode(w,UText[0],&UcBase,&txt);
          fmt[i]=SetUniFont(w,fmt[i],UcBase);
          ptr[i]=txt;
       }
    }
    CloseCfmt(w,CurLine); 

    return TRUE;
} 

/*****************************************************************************
    UnicodeToMutiByte:
    Convert regular text of line to unicode.
******************************************************************************/
BOOL UnicodeToMultiByte(PTERWND w,WORD uc, LPBYTE pLead, LPBYTE pChar,int CurFont)
{
    BYTE  mb[10];      // multi-byte string
    int   count,CodePage,CharSet;

    CharSet=TerFont[CurFont].CharSet;

    if (CharSet==0 || CharSet==1) CodePage=GetACP();
    else                          IsMbcsCharSet(w,CharSet,&CodePage);  // get the code page
    //LogPrintf("  uc: %x, cs: %d, cp: %d",uc,CharSet,CodePage);
    count=WideCharToMultiByte(CodePage,0x400/*WC_NO_BEST_FIT_CHARS*/,&uc,1,mb,10,NULL,NULL);
    if (count==1 || count==2) {
       (*pLead)=0;
       if (count==1) (*pChar)=mb[0];
       else if (count==2) {
         (*pLead)=mb[0];
         (*pChar)=mb[1];
       }
       
       //LogPrintf("  count: %d, chr: %d",count,(*pChar));
       return TRUE;
    }

    return FALSE;
} 


/*****************************************************************************
    GetRtfFontId:
    Get the font id for the specfied group info
******************************************************************************/
#pragma optimize("g",off)  // turn-off global optimization
int GetRtfFontId(PTERWND w, struct StrRtf far *rtf, struct StrRtfGroup far *group) 
{
    int   i,TwipsSize,CurFont=0,FieldId;
    BYTE  TypeFace[32],FontFamily[32];
    BYTE  CurFamily;
    int   TextAngle,CellTextAngle;
    UINT  style;
    COLORREF TextBkColor;
    struct StrRtfGroup TempGroup;
 
    // make a copy of the group data
    FarMove(group,&TempGroup,sizeof(TempGroup));
    group=&TempGroup;


    // apply footnote fonts
    if (group->style&FNOTEREST && !(group->style&FNOTETEXT) 
        && FootnoteRestFont>0 && FootnoteRestFont<TotalFonts) {
       
       group->style = (group->style&FNOTEALL) | (TerFont[FootnoteRestFont].style&FNOTENONE);

       group->TextColor=TerFont[FootnoteRestFont].TextColor;
       group->TextBkColor=TerFont[FootnoteRestFont].TextBkColor;
       group->UlineColor=TerFont[FootnoteRestFont].UlineColor;
       group->PointSize2=TerFont[FootnoteRestFont].TwipsSize/10;
       lstrcpy(group->TypeFace,TerFont[FootnoteRestFont].TypeFace);

       FootnoteRestFont=0;
    } 

    // set text angle for cell
    TextAngle=group->TextAngle;      // capture frame text angle
    CellTextAngle=cell[RtfCurCellId].TextAngle;
    if (RtfCurCellId!=0 && CellTextAngle!=0) TextAngle=CellTextAngle;

    // set character formatting
    lstrcpy(TypeFace,group->TypeFace);
    if (lstrlen(TypeFace)==0) lstrcpy(TypeFace,TerArg.FontTypeFace);
    TwipsSize=group->PointSize2*10;      // convert PointSize2 to twips
    if (TwipsSize==0) TwipsSize=12*20;   // use 12 points for default to be compatible with MSWord, TerArg.PointSize*20;
    if (group->CharScaleX>0) TwipsSize=MulDiv(TwipsSize,group->CharScaleX,100);

    style=group->style;
    
    if (rtf->InitFieldId>0) FieldId=rtf->InitFieldId;  // being pasted inside a field name/data
    else                    FieldId=group->FieldId;

    if (group->lang==DefLang) group->lang=0;          // default langauge

    // calculate the text background color
    if (group->CharBkPat!=PAT_NONE && group->CharPatBC!=0xFFFFFF) 
          TextBkColor=group->CharPatBC;
    else  TextBkColor=group->TextBkColor;

    // check tracking varibles
    if (!group->revised) {
        group->InsRev=0;
        group->InsTime=0;
    }
    if (!group->deleted) {
        group->DelRev=0;
        group->DelTime=0;
    }
     
    // scan existing font
    for (i=0;i<TotalFonts;i++) {    // scan the font table
       if (!(TerFont[i].InUse)) continue;
       if (TerFont[i].TwipsSize!=TwipsSize) continue;
       if (TerFont[i].style!=style) continue;
       if (TerFont[i].TextColor!=group->TextColor) continue;
       if (TerFont[i].TextBkColor!=TextBkColor) continue;
       if (TerFont[i].UlineColor!=group->UlineColor) continue;
       if (TerFont[i].CharStyId!=group->CharStyId) continue;
       if (TerFont[i].ParaStyId!=group->ParaStyId) continue;
       if (TerFont[i].expand!=group->expand) continue;
       if (TerFont[i].FieldId!=FieldId) continue;
       if (TerFont[i].AuxId!=group->AuxId) continue;
       if (TerFont[i].CharId!=group->CharId) continue;
       if (TerFont[i].offset!=group->offset) continue;
       if (TerFont[i].TextAngle!=TextAngle) continue;
       if (TerFont[i].UcBase!=group->UcBase) continue;
       if (TerFont[i].TempStyle!=0) continue;
       if (FieldId>0 && !IsSameFieldCode(w,TerFont[i].FieldCode,rtf->FieldCode)) continue;
       if (TerFont[i].CharSet!=group->CharSet && group->CharSet!=DEFAULT_CHARSET) continue;
       if (lstrcmpi(TerFont[i].TypeFace,TypeFace)!=0) continue;

       if (TerFont[i].InsRev != group->InsRev) continue;
       if (TerFont[i].InsTime != group->InsTime) continue;
       if (TerFont[i].DelRev != group->DelRev) continue;
       if (TerFont[i].DelTime != group->DelTime) continue;

       break;
    }
    

    CurFont=i;    // current font
    if (CurFont==TotalFonts) {       // create a new font
       for (i=0;i<TotalFonts;i++) if (!(TerFont[i].InUse)) break;
       CurFont=i;
       if (CurFont==TotalFonts) {
          // display out-of-font message only once
          if ((TotalFonts+1)==MAX_FONTS && !Win32) PrintError(w,MSG_OUT_OF_FONT_SLOT,"Default Font Used");
          if (TotalFonts==MAX_FONTS && !Win32) CurFont=DEFAULT_CFMT;   // set to default font
          else {
             if (TotalFonts>=MaxFonts) {  // expand the font table
                int NewMaxFonts=MaxFonts+(MaxFonts/3)+1;
                if (NewMaxFonts<=TotalFonts) NewMaxFonts=TotalFonts+1;
                if (NewMaxFonts>MAX_FONTS && !Win32) NewMaxFonts=MAX_FONTS;
                ExpandFontTable(w,NewMaxFonts);  // expand the font table
             }
             CurFont=TotalFonts;
             TotalFonts++;
          }
       }
       // create a new font
       if (CurFont!=DEFAULT_CFMT) {
          InitTerObject(w,CurFont);   // initialize the font object
          TerFont[CurFont].InUse=TRUE;
          lstrcpy(TerFont[CurFont].TypeFace,TypeFace);
          TerFont[CurFont].TwipsSize=TwipsSize;
          TerFont[CurFont].TextColor=group->TextColor;
          TerFont[CurFont].TextBkColor=TextBkColor;
          TerFont[CurFont].UlineColor=group->UlineColor;
          TerFont[CurFont].style=style;
          TerFont[CurFont].FieldId=FieldId;
          TerFont[CurFont].AuxId=group->AuxId;
          TerFont[CurFont].CharId=group->CharId;
          TerFont[CurFont].lang=group->lang;
          TerFont[CurFont].offset=group->offset;
          TerFont[CurFont].TextAngle=TextAngle;
          TerFont[CurFont].UcBase=group->UcBase;
          TerFont[CurFont].CharStyId=group->CharStyId;
          TerFont[CurFont].ParaStyId=group->ParaStyId;
          TerFont[CurFont].expand=group->expand;
          TerFont[CurFont].CharSet=group->CharSet;

          if (FieldId && rtf->FieldCode) {
             TerFont[CurFont].FieldCode=OurAlloc(lstrlen(rtf->FieldCode)+1);
             lstrcpy(TerFont[CurFont].FieldCode,rtf->FieldCode);
          }
          else TerFont[CurFont].FieldCode=NULL;

          lstrcpy(FontFamily,group->FontFamily);
          if      (lstrcmpi(FontFamily,"Roman")==0)   CurFamily=FF_ROMAN;
          else if (lstrcmpi(FontFamily,"swiss")==0)   CurFamily=FF_SWISS;
          else if (lstrcmpi(FontFamily,"modern")==0)  CurFamily=FF_MODERN;
          else if (lstrcmpi(FontFamily,"script")==0)  CurFamily=FF_SCRIPT;
          else if (lstrcmpi(FontFamily,"decor")==0)   CurFamily=FF_DECORATIVE;
          else                                        CurFamily=FF_DONTCARE;    // set default font family
          TerFont[CurFont].FontFamily=CurFamily;

          TerFont[CurFont].InsRev=group->InsRev;
          TerFont[CurFont].InsTime=group->InsTime;
          TerFont[CurFont].DelRev=group->DelRev;
          TerFont[CurFont].DelTime=group->DelTime;

          if (!CreateOneFont(w,hTerDC,CurFont,TRUE)) CurFont=DEFAULT_CFMT; // set to default font
       }
    }
    
    return CurFont;
}
#pragma optimize("",off)  // restore optimization
 
/*****************************************************************************
    ImportRtfPicture:
    Imbed a given metafile into the text stream.
******************************************************************************/
BOOL ImportRtfPicture(PTERWND w,struct StrRtfGroup far *group, struct StrRtfPict far *pic)
{
    int  pict;
    LPBYTE ptr;
    LPWORD fmt;
    struct StrRtf far *rtf;
    long  InsLine;                            // line number to insert
    BOOL  CreatePictFrame;

    rtf=group->rtf;                           // pointer to the rtf data structure


    if (pic->PictId>0) CurObject=pict=pic->PictId;   // id already available
    else {
       if ((TotalFonts+1)==MAX_FONTS && !Win32) PrintError(w,MSG_OUT_OF_FONT_SLOT,NULL);
       if (TotalFonts==MAX_FONTS && !Win32) return FALSE;  // ran out of picture table
       if ((pict=FindOpenSlot(w))==-1) return FALSE;  // ran out of picture table
       CurObject=pict;                                // current object

       if (group->gflags&GFLAG_ANIM_SEQ) return BuildRtfAnimSeq(w,group,pic,pict);
    }

    if (-1==(InsLine=GetRtfInsertionLine(w,rtf))) return FALSE;

    // set the paraframe id
    SetRtfParaFID(w,InsLine,group);

    LineAlloc(w,InsLine,0,1);    // allocate space for the character representing a picture
    ptr=pText(InsLine);
    ptr[0]=PICT_CHAR;


    fmt=OpenCfmt(w,InsLine);
    fmt[0]=pict;                // picture index serves as the formatting attribute
    CloseCfmt(w,InsLine);

    LineY(InsLine)=0;           // initialize line y position
    LineX(InsLine)=0;           // initialize line x position
    cid(InsLine)=RtfCurCellId;  // assign the cell id

    // set the para id
    SetRtfParaId(w,InsLine,group);

    // set the font array
    if (pic->PictId<=0) BuildRtfPicture(w,group,pic,pict);

    LineHt(InsLine)=PrtFont[pict].height;  // initial height of the line

    // set the animation information if any
    if (group->gflags&GFLAG_ANIM_INFO) {
       if (TerSetAnimInfo(hTerWnd,pict,group->AnimLoops,group->AnimDelay))
          rtf->FirstAnimPict=pict;
    }
    
    // create a picture frame
    CreatePictFrame=false;
    if (False(group->gflags&GFLAG_IN_SHPRSLT) && 
           (group->shape.type==SHPTYPE_PICT_FRAME || group->shape.type==SHPTYPE_OCX) ) CreatePictFrame=true;
    if (TerFont[pict].PictType==PICT_SHAPE) CreatePictFrame=true;
    
    ResetUintFlag(rtf->flags2,RFLAG2_PICT_FRAME_INSERTED);
    if (CreatePictFrame) {
       BuildRtfPictFrame(w,group,pict);
       LineFrame(InsLine)=TerFont[pict].ParaFID;

       rtf->PictFrameLine=InsLine;
       rtf->PictFrameCol=0;
    }

    if (rtf->TagId) ApplyRtfTagId(w,rtf,InsLine);

    rtf->SomeTextRead=TRUE;     // some rtf text read
    ResetUintFlag(rtf->flags1,RFLAG1_PARA_ENDED);

    CurLine++;

    return TRUE;
}


/*****************************************************************************
    ReadRtfInfo:
    Retrive the rtf document information.
******************************************************************************/
ReadRtfInfo(PTERWND w, struct StrRtf far *rtf)
{
    int    i,type=-1,InfoLen,NewLen;
    int    ControlGroupLevel=0,CurGroupLevel;

    ControlGroupLevel=rtf->GroupLevel;

    while (TRUE) {
       // get color number
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // color group ended
          else                                   continue; // look for next color
       }

       if (rtf->IsControlWord) {                 // extract color component
          for (i=0;i<INFO_MAX;i++) if (lstrcmpi(RtfInfo[i],rtf->CurWord)==0) break;
          if (i==INFO_MAX) continue;             // string not supported

          type=i;
          CurGroupLevel=rtf->GroupLevel;
          if (pRtfInfo[type]) MemFree(pRtfInfo[type]);
          pRtfInfo[type]=NULL;
          InfoLen=0;                             // length of the string

          while (TRUE) {
             if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

             if (rtf->GroupBegin) continue;
             if (rtf->GroupEnd) {
                if (rtf->GroupLevel<CurGroupLevel) break;    // fldinst group ended
                else                              continue; // look for the next conponent
             }

             if (!(rtf->IsControlWord)) {
                NewLen=InfoLen+lstrlen(rtf->CurWord);
                if (!pRtfInfo[type]) pRtfInfo[type]=MemAlloc(NewLen+1);
                else                 pRtfInfo[type]=MemReAlloc(pRtfInfo[type],NewLen+1);
                pRtfInfo[type][InfoLen]=0;
                lstrcat(pRtfInfo[type],rtf->CurWord);
                InfoLen=NewLen;
             }
          }
       }
    }

    return TRUE;
}

/*****************************************************************************
    ReadRtfReviewers:
    Retrive the reviewer table
******************************************************************************/
ReadRtfReviewers(PTERWND w, struct StrRtf far *rtf)
{
    int    i,len,idx;
    int    ControlGroupLevel=0,CurGroupLevel;
    BYTE   name[MAX_WIDTH+1];

    ControlGroupLevel=rtf->GroupLevel;

    for (i=0;i<TotalReviewers;i++) reviewer[i].RtfId=i;  // initialize

    idx=-1;
    while (TRUE) {
       // get color number
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) {
          CurGroupLevel=rtf->GroupLevel;
          idx++;                   // next reviewer begins
          name[0]=0;
          continue;
       }
       
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // reviewer group ended
          else if (rtf->GroupLevel<CurGroupLevel) {        // add this reviewer to the table
             if (idx<0) continue;
             len=strlen(name);
             if (len>0 && name[len-1]==';') len--;
             name[len]=0;

             if (strcmpi(name,"unknown")==0) name[0]=0;

             // check if this review is already in the table
             for (i=0;i<TotalReviewers;i++) if (lstrcmpi(reviewer[i].name,name)==0) break;
             if (i==TotalReviewers) {
                i=GetReviewerSlot(w);   // add new reviewer
                lstrncpy(reviewer[i].name,name,49);  // copy name
                reviewer[i].name[49]=0;
             }
             
             reviewer[i].RtfId=idx;
             continue;
          } 
       }

       if (!(rtf->IsControlWord)) lstrcat(name,rtf->CurWord);
    }

    return TRUE;
}

/*****************************************************************************
    SplitRtfMbcsText:
    Split mbcs text into lead and tail text.  This function returns TRUE
    if the lead text is found in the string.
******************************************************************************/
BOOL SplitRtfMbcsText(PTERWND w, struct StrRtf far *rtf, LPBYTE text, LPBYTE lead, struct StrRtfGroup far *pGroup)
{
    BYTE string[MAX_WIDTH+2];
    int  i,j,count,len,OrigLen,CharType,CharSet,CodePage=0;
    BOOL HasLead;

    if (!mbcs) return FALSE;
    if (!rtf->PendingByte && lstrlen(text)==1 && text[0]==' ') return FALSE;  // to allow for space separator character to be inserted

    //if (CharType==CHAR_LO || CharType==CHAR_HI) return FALSE;  // single byte text

    CharType=pGroup->CharType;
    CharSet=pGroup->CharSet;
    if (CharSet==ANSI_CHARSET || CharSet==DEFAULT_CHARSET) CharSet=rtf->CharSet;

    //if (CharType==0) IsMbcsCharSet(w,CharSet,&CodePage);  // get the code page
    //else             CodePage=0;                          // use the default code page

    if (!IsMbcsCharSet(w,CharSet,&CodePage)) CharType=0;  // get the code page
    
    // add any pending byte
    string[0]=0;
    if (rtf->PendingByte) {
      string[0]=rtf->PendingByte;
      string[1]=0;
      rtf->PendingByte=0;
    }
    strcat(string,text);
    len=strlen(string);

    // process the double byte stream
    if (CharType==CHAR_DB) {
       j=count=0;
       for (i=0;i<len;i++) {
          count++;
          if (count==1 && string[i]<32) {   // one of our control characters
             text[j]=string[i];
             lead[j]=0;
             j++;
             count=0;
          }
          else {
             if (count==1) lead[j]=string[i];
             else          text[j]=string[i];

             if (count==2) {
                j++;
                count=0;
             }
          }
       }
       if (count==1) rtf->PendingByte=lead[j];   // only partial character found
       text[j]=lead[j]=0;
       return (BOOL) j;
    }

    // handle mbcs character
    OrigLen=len;
    len=(int)TerSplitMbcs2(w,string,text,lead,CodePage);
    HasLead=(OrigLen>len);

    if (len>0 && lead[len-1]==0 && IsDBCSLeadByteEx(CodePage,text[len-1]) && !(rtf->IsControlWord)) { // when next word is not a control word
       rtf->PendingByte=text[len-1];
       len--;
       text[len]=lead[len]=0;
    }

    return HasLead;
}

/*****************************************************************************
    ApplyRtfTagId:
    Set the current tag id to the insertion line.
******************************************************************************/
ApplyRtfTagId(PTERWND w, struct StrRtf far *rtf, long InsLine)
{
    LPWORD ct;
    
    if (LineLen(InsLine)==0) return true;  

    ct=OpenCtid(w,InsLine);

    ct[0]=rtf->TagId;
    CloseCtid(w,InsLine);
    rtf->TagId=0;

    return TRUE;
}

/*****************************************************************************
    SetRtfParaFID:
    Set the paraframe id for the current paragraph in the RtrParaFID variable.
******************************************************************************/
SetRtfParaFID(PTERWND w, long line, struct StrRtfGroup far *group)
{
    struct StrRtf far *rtf;
    struct StrRtfParaFrameInfo CurParaFrameInfo;
    int    CurGroup,HPageGroup,OrgX;
    BOOL   OrgXSaved=false;



    // get the rtf structure pointer
    rtf=group->rtf;

    if (True(group->gflags2&GFLAG2_PICT_SHAPE)) goto END;  // use the parent fid - shpae objects can be embedded into a paragrah frame

    // convert drawing object to/from frames when in shape object
    if ((group->gflags&GFLAG_IN_SHPRSLT || group->gflags&GFLAG_USE_SHP_INFO)) {
       // apply the wrap type constants
       if (group->shape.WrapType==SWRAP_NO_WRAP)      group->FrmFlags|=PARA_FRAME_NO_WRAP;
       else if (group->shape.WrapType==SWRAP_IGNORE)  group->FrmFlags|=PARA_FRAME_WRAP_THRU;
       else                                           ResetUintFlag(group->FrmFlags,(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU));

       if (group->shape.WrapType!=SWRAP_IGNORE && group->shape.WrapType!=SWRAP_THRU) {
          //if (group->FrmFlags&PARA_FRAME_TEXT_BOX && group->shape.ShapeType!=SHPTYPE_TEXT_BOX) {  // convert a text box in shaperesult to the frame
          //   group->gflags=ResetLongFlag(group->gflags,GFLAG_DRAW_OBJECT);
          //   group->FrmFlags=ResetUintFlag(group->FrmFlags,PARA_FRAME_OBJECT);
          //   group->FrmFlags|=PARA_FRAME_CONV;  // converted into frame
          //}
          //if (group->shape.WrapType==SWRAP_NO_WRAP) group->FrmFlags|=PARA_FRAME_NO_WRAP;
       }
       else if (!(group->FrmFlags&PARA_FRAME_OBJECT)) {  // convert a frame to a drawing object
          group->gflags|=GFLAG_DRAW_OBJECT;
          group->FrmFlags|=PARA_FRAME_TEXT_BOX;
          group->FrmFlags|=PARA_FRAME_CONV;    // converted into text box
          if (group->shape.WrapType==SWRAP_IGNORE && group->shape.ZOrder==0) group->ParaFrameInfo.ZOrder=group->shape.ZOrder=-1;  // behind the text
       }
       group->FrmFlags|=PARA_FRAME_USED;

       if (group->gflags&GFLAG_USE_SHP_INFO) {          // copy information to group->ParaFrameInfo
          FarMemSet(&(group->ParaFrameInfo),0,sizeof(struct StrRtfParaFrameInfo));
          group->ParaFrameInfo.x=group->shape.x;
          HPageGroup=group->HPageGroup;
          rtf->group[HPageGroup].ParaFrameInfo.x=group->shape.x;

          group->ParaFrameInfo.y=group->shape.y;
          group->ParaFrameInfo.width=group->shape.width;
          group->ParaFrameInfo.height=group->shape.height;
          group->ParaFrameInfo.ZOrder=group->shape.ZOrder;
          group->ParaFrameInfo.DistFromText=group->shape.DistFromText;
       } 
    }
    else if (group->FrmFlags&PARA_FRAME_OBJECT) group->FrmFlags|=PARA_FRAME_WRAP_THRU;

    if (group->FrmFlags&PARA_FRAME_USED && RtfParaFID==0) rtf->flags|=RFLAG_PARA_FRAME_FOUND;

    // set the defaults for the new frame
    FarMove(&(group->ParaFrameInfo),&CurParaFrameInfo,sizeof(RtfParaFrameInfo));

    if (group->FrmFlags&PARA_FRAME_USED && !(group->gflags&GFLAG_DRAW_OBJECT)) {
       float width;                    // text area width
       int   WidthTwips;


       // get update page size
       width=(rtf->sect.orient==DMORIENT_PORTRAIT?rtf->sect.PprWidth:rtf->sect.PprHeight)
             -rtf->sect.LeftMargin-rtf->sect.RightMargin;   // text area width
       WidthTwips=(int)InchesToTwips(width);  // width of the line

       if (group->FrmFlags&PARA_FRAME_HPAGE) {
          //CurParaFrameInfo.x-=((int)InchesToTwips(rtf->sect.LeftMargin));
          CurGroup=rtf->GroupLevel;
          HPageGroup=rtf->group[CurGroup].HPageGroup;

          OrgX=rtf->group[HPageGroup].ParaFrameInfo.x; // save original x
          OrgXSaved=true;
          CurParaFrameInfo.x=rtf->group[HPageGroup].ParaFrameInfo.x-((int)InchesToTwips(rtf->sect.LeftMargin));
       }
       if (CurParaFrameInfo.width==0 && !(group->FrmFlags&PARA_FRAME_LINE)) {   // set temporary width
           if (group->FrmFlags&(PARA_FRAME_CENTER|PARA_FRAME_RIGHT_JUST)) 
                 CurParaFrameInfo.width=WidthTwips;
           else  CurParaFrameInfo.width=WidthTwips+(int)InchesToTwips(rtf->sect.RightMargin)-CurParaFrameInfo.x;
           if (CurParaFrameInfo.width<PARA_FRAME_MIN_WIDTH) CurParaFrameInfo.width=PARA_FRAME_MIN_WIDTH;
           group->FrmFlags|=PARA_FRAME_SET_WIDTH;   // width will be reduced after wordwrapping
       }
       if (group->FrmFlags&(PARA_FRAME_CENTER|PARA_FRAME_RIGHT_JUST)) {
          if (FALSE && group->FrmFlags&PARA_FRAME_SET_WIDTH) CurParaFrameInfo.x=WidthTwips;
          else                                      CurParaFrameInfo.x=WidthTwips-CurParaFrameInfo.width;
          if (CurParaFrameInfo.width<=0) CurParaFrameInfo.x=WidthTwips;  // when frame width is not specified
          //if (CurParaFrameInfo.x<0) CurParaFrameInfo.x=0;               // do not allow negative positioning
          if (group->FrmFlags&PARA_FRAME_CENTER) (CurParaFrameInfo.x)/=2;
       }
    }

    // get new frame id
    if (  (!IsSameRtfParaFrame(&CurParaFrameInfo,&RtfParaFrameInfo) || rtf->flags&RFLAG_NEW_FRAME)
       && (rtf->InitialParaFID==0 || line==rtf->FirstLine) ){
       RtfParaFID=0;            // normal text or chane of Para frame
       rtf->InitialParaFID=0;   // new frames allowed now

       FarMove(&RtfParaFrameInfo,&PrevRtfParaFrameInfo,sizeof(RtfParaFrameInfo));  // save a copy
       FarMove(&CurParaFrameInfo,&RtfParaFrameInfo,sizeof(RtfParaFrameInfo));

       rtf->flags=ResetUintFlag(rtf->flags,RFLAG_NEW_FRAME);
    }
    if (!(group->FrmFlags&PARA_FRAME_USED)) RtfParaFID=0;   // normal paragraph

    if (!RtfParaFID && group->FrmFlags&PARA_FRAME_USED) {
       if ((RtfParaFID=GetParaFrameSlot(w))>0) {
           ParaFrame[RtfParaFID].InUse=TRUE;
           
           ParaFrame[RtfParaFID].x=RtfParaFrameInfo.x;
           ParaFrame[RtfParaFID].OrgX=OrgXSaved?OrgX:RtfParaFrameInfo.x;           // used when PARA_FRAME_IGNORE_X is used
           ParaFrame[RtfParaFID].GroupX=RtfParaFrameInfo.x;         // used when PARA_FRAME_SHPGRP is set

           ParaFrame[RtfParaFID].ParaY=RtfParaFrameInfo.y;
           ParaFrame[RtfParaFID].OrgY=RtfParaFrameInfo.y;           // used when PARA_FRAME_IGNORE_Y is used
           ParaFrame[RtfParaFID].GroupY=RtfParaFrameInfo.y;

           ParaFrame[RtfParaFID].width=RtfParaFrameInfo.width;
           ParaFrame[RtfParaFID].height=RtfParaFrameInfo.height;
           ParaFrame[RtfParaFID].MinHeight=RtfParaFrameInfo.height;
           ParaFrame[RtfParaFID].ZOrder=RtfParaFrameInfo.ZOrder;
           if (ParaFrame[RtfParaFID].ZOrder==0) ParaFrame[RtfParaFID].ZOrder=-1;  // in MSWord Zorder less than 8192 is behind the text

           ParaFrame[RtfParaFID].DistFromText=RtfParaFrameInfo.DistFromText;
           ParaFrame[RtfParaFID].BackColor=0xFFFFFF;
           ParaFrame[RtfParaFID].flags=group->FrmFlags;
           ParaFrame[RtfParaFID].TextLine=line;
           ParaFrame[RtfParaFID].LineColor=0;     // default line color
           ParaFrame[RtfParaFID].TextAngle=group->TextAngle;     // default line color
           if ((group->gflags&(GFLAG_DRAW_OBJECT|GFLAG_USE_SHP_INFO))) {
              if (ParaFrame[RtfParaFID].flags&PARA_FRAME_TEXT_BOX) {
                   ParaFrame[RtfParaFID].margin=(group->TextBoxMargin==-1)?0:group->TextBoxMargin;
              }
              else ParaFrame[RtfParaFID].margin=0;
           }
           else {
              ParaFrame[RtfParaFID].margin=group->BorderMargin;
              //if (ParaFrame[RtfParaFID].margin<MIN_FRAME_MARGIN && group->flags&PARA_BOX)
              //   ParaFrame[RtfParaFID].margin=MIN_FRAME_MARGIN;
           }

           if (group->gflags&GFLAG_USE_SHP_INFO) {
              ParaFrame[RtfParaFID].flags|=group->shape.FrmFlags;
              ParaFrame[RtfParaFID].LineWdth=group->shape.LineWdth;
              ParaFrame[RtfParaFID].LineColor=group->shape.LineColor;
              ParaFrame[RtfParaFID].BackColor=group->shape.BackColor;
              ParaFrame[RtfParaFID].FillPattern=group->shape.FillPattern;
           }
       }
    }
    
    END:
    group->ParaFID=RtfParaFID;       // save the group paraframe id
    if (RtfCurCellId>0) {
       BOOL FrameInCell=FALSE;
       if (CurLine>0 && cid(CurLine-1)>0 && fid(CurLine-1)==0) FrameInCell=TRUE;  // check if frame-inside-table or table-inside-frame
       if (FrameInCell && ParaFrame[RtfParaFID].pict==0) RtfParaFID=group->ParaFID=0;  // regular frames inside the table cell not supported
    }

    return TRUE;
}

/*****************************************************************************
    SetRtfParaId:
    During the text and picture import, set the para id for the line.
******************************************************************************/
SetRtfParaId(PTERWND w, long line, struct StrRtfGroup far *group)
{
    int    TabId,BltId,BorderSpace;
    UINT pflags;
    struct StrRtf far *rtf;
    struct StrPfmt NewPfmt;


    rtf=group->rtf;

    // get new tab id
    TabId=NewTabId(w,0,&(group->tab));
    
    // get the bullet id
    if (group->RtfLs>0/* && !(group->flags&BULLET)*/) {   // set a bullet for ls line
       group->flags|=BULLET;
       FarMemSet(&(group->blt),0,sizeof(struct StrBlt));
       group->blt.NumberType=NBR_DEFAULT;     // not yet assigned
    } 
    group->blt.ls=rtf->XlateLs[group->RtfLs];
    group->blt.lvl=group->ListLvl;
    BltId=NewBltId(w,0,&(group->blt));

    tabw(line)=0;

    fid(line)=RtfParaFID;
    if (RtfParaFID==0) rtf->TableInFrame=FALSE;   // end of any table-inside-frame

    pflags=group->pflags;
    if (RtfParaFID) pflags|=PFLAG_FRAME;
    if (cid(line)) pflags|=PFLAG_CELL;

    BorderSpace=(group->BorderMargin==0)?PARA_BOX_SPACE:group->BorderMargin;

    // set paragraph id
    FarMemSet(&(NewPfmt),0,sizeof(struct StrPfmt));

    NewPfmt.LeftIndentTwips=group->LeftIndent;
    NewPfmt.RightIndentTwips=group->RightIndent;
    NewPfmt.FirstIndentTwips=group->FirstIndent;
    NewPfmt.TabId           =TabId;
    NewPfmt.BltId           =BltId;
    NewPfmt.StyId           =group->ParaStyId;
    NewPfmt.shading         =group->ParShading;
    NewPfmt.pflags          =pflags;
    NewPfmt.SpaceBefore     =group->SpaceBefore;
    NewPfmt.SpaceAfter      =group->SpaceAfter;
    NewPfmt.SpaceBetween    =group->SpaceBetween;
    NewPfmt.LineSpacing     =group->LineSpacing;
    NewPfmt.BkColor         =group->ParaBkColor;
    NewPfmt.BorderColor     =group->ParaBorderColor;
    NewPfmt.BorderSpace     =BorderSpace;
    NewPfmt.flow            =group->flow;
    NewPfmt.flags           =group->flags;

    pfmt(line)=NewParaId2(w,0,&NewPfmt);

    return TRUE;
}

/*****************************************************************************
    IsSameRtfParaFrame:
    This routine returns TRUE when two para frames are indentical.
******************************************************************************/
BOOL IsSameRtfParaFrame(struct StrRtfParaFrameInfo far *frame1, struct StrRtfParaFrameInfo far *frame2)
{
    if (frame1->x!=frame2->x) return FALSE;
    if (frame1->y!=frame2->y) return FALSE;
    if (frame1->width!=frame2->width) return FALSE;
    if (frame1->height!=frame2->height) return FALSE;
    if (frame1->ZOrder!=frame2->ZOrder) return FALSE;
    if (frame1->DistFromText!=frame2->DistFromText) return FALSE;

    return TRUE;
}


/*****************************************************************************
    MergeRtfLinePieces:
    The rtf reader creates separate line for each text segment of identical
    font attributes.  This routines merges those pieces to reconstruct the
    lines.
******************************************************************************/
BOOL MergeRtfLinePieces(PTERWND w,long FirstLine, long LastLine)
{
   long l,DelLines,LineBegin,LineEnd,CursPos;
   BYTE text[MAX_WIDTH+1],BreakChar;
   WORD TextCfmt[MAX_WIDTH+1];
   int len,CurLen,i;
   LPBYTE ptr;
   LPWORD fmt;
   BOOL  BreakFound;


   // get the absolute cursor position regardless of the cr/lf or the para characters
   CursPos=0;
   for (l=0;l<=CurLine;l++) {
      if (l<CurLine) len=LineLen(l);
      else           len=CurCol;
      CursPos+=len;
      if (len>0 && l>=FirstLine) {   // don't count the para char created by rtf read
         ptr=pText(l);
         if (ptr[len-1]==ParaChar) CursPos--;

      }
   }

   // scan each line
   len=0;

   for (l=FirstLine;l<=LastLine;l++) {
      if (len==0) LineBegin=l;    // the combined line begins at this line

      // check if line break found
      CurLen=LineLen(l);
      ptr=pText(l);
      if (CurLen>0) BreakChar=ptr[CurLen-1];
      else          BreakChar=0;


      BreakFound=FALSE;
      if ( BreakChar==ParaChar || lstrchr(BreakChars,BreakChar)!=NULL) BreakFound=TRUE;

      if (BreakFound) {           // discard the break character
         LineAlloc(w,l,CurLen,CurLen-1);
         CurLen=LineLen(l);      // new line length
      }

      // check if current line exceed the max line width
      if (len==0 && CurLen>=MAX_WIDTH) continue;  // leave this line alone

      // check if combined need to be created
      LineEnd=-1;
      if (len>0 && (len+CurLen)>=MAX_WIDTH) LineEnd=l=l-1;  // position a line behind
      if (LineEnd==-1) {
         ptr=pText(l);
         fmt=OpenCfmt(w,l);
         for (i=0;i<CurLen;i++) {
            text[len+i]=ptr[i];
            TextCfmt[len+i]=fmt[i];
         }

         CloseCfmt(w,l);

         len+=CurLen;            // new length of the line
         if (BreakFound || l==LastLine) LineEnd=l;  // combine upto the current line
      }

      if (LineEnd==-1) continue; // get next line

      // process this buffered line
      if (LineEnd<LineBegin) LineEnd=LineBegin;

      // copy the buffered line to the line array
      if (LineEnd>LineBegin) {
         // assign the entire text to the first line
         LineAlloc(w,LineBegin,LineLen(LineBegin),len);
         ptr=pText(LineBegin);
         fmt=OpenCfmt(w,LineBegin);
         for (i=0;i<len;i++) {
            ptr[i]=text[i];
            fmt[i]=TextCfmt[i];
         }

         CloseCfmt(w,LineBegin);

         // delete the combinded lines
         DelLines=LineEnd-LineBegin;
         MoveLineArrays(w,LineBegin+1,DelLines,'D');

         l-=DelLines;
         LastLine-=DelLines;    // reduce the last line to process
      }
      len=0;                    // empty the line buffer for the next line
   }

   // Initialize other attributes
   for (l=FirstLine;l<=LastLine;l++) {
      pfmt(l)=0;               // use default para id
      TotalPfmts=1;
      if (tabw(l)) FreeTabw(w,l);
   }

   // get the CurLine/CurCol cursor position regardless of the cr/lf character
   CurLine=CurCol=0;
   while (CursPos>=0) {
      if (CursPos>=LineLen(CurLine)) CursPos=CursPos-LineLen(CurLine);
      else {
         CurCol=(int)CursPos;
         break;
      }
      if (CurLine>=(TotalLines-1)) {
         CurCol=LineLen(TotalLines-1);
         break;
      }
      CurLine++;
   }
   if (CurCol<0) CurCol=0;

   return TRUE;
}

/*****************************************************************************
    SetRtfSectPaperSize:
    Set the paper size
******************************************************************************/
BOOL SetRtfSectPaperSize(PTERWND w,struct StrRtf far *rtf)
{
    return SetRtfPaperSize(w,rtf,InchesToTwips(rtf->sect.PprWidth),InchesToTwips(rtf->sect.PprHeight),&rtf->sect.PprSize);
}

/*****************************************************************************
    SetRtfDocPaperSize:
    Set the paper size
******************************************************************************/
BOOL SetRtfDocPaperSize(PTERWND w,struct StrRtf far *rtf)
{
    return SetRtfPaperSize(w,rtf,rtf->PaperWidth,rtf->PaperHeight,&PaperSize);
}


/*****************************************************************************
    SetRtfPaperSize:
    Set the paper size
******************************************************************************/
BOOL SetRtfPaperSize(PTERWND w,struct StrRtf far *rtf, int width, int height, LPINT pPprSize)
{
    int PprSize;

    if (abs(width-12240)<10 && abs(height-15840)<10) PprSize=DMPAPER_LETTER;
    else if (abs(width-15840)<10 && abs(height-12240)<10) PprSize=DMPAPER_LETTER;

    else if (abs(width-12240)<10 && abs(height-20160)<10) PprSize=DMPAPER_LEGAL;
    else if (abs(width-20160)<10 && abs(height-12240)<10) PprSize=DMPAPER_LEGAL;

    else if (abs(width-11909)<10 && abs(height-16834)<10) PprSize=DMPAPER_A4;
    else if (abs(width-16834)<10 && abs(height-11909)<10) PprSize=DMPAPER_A4;

    else if (abs(width-24480)<10 && abs(height-15840)<10) PprSize=DMPAPER_TABLOID;
    else if (abs(width-15840)<10 && abs(height-24480)<10) PprSize=DMPAPER_TABLOID;

    else if (abs(width-5940)<10 && abs(height-13680)<10) PprSize=DMPAPER_ENV_10;
    else if (abs(width-13680)<10 && abs(height-5940)<10) PprSize=DMPAPER_ENV_10;

    else if (width>0 && height>0) {
       int i;
       for (i=0;i<DefPaperCount;i++) {
          if ( (abs(width-(int)(DefPaperWidth[i]*1440))<10 && abs(height-(int)(DefPaperHeight[i]*1440))<10) || 
               (abs(width-(int)(DefPaperHeight[i]*1440))<10 && abs(height-(int)(DefPaperWidth[i]*1440))<10)) {
               PprSize=DefPaperSize[i];
               break;
          }
       } 
       if (i==DefPaperCount) {
         PprSize=0;                // custom paper size
         rtf->ApplyPaperSize=TRUE;   // apply custom paper size to the printer
       }
    }

    if (pPprSize) (*pPprSize)=PprSize;

    return TRUE;
}

/*****************************************************************************
    SetRtfFontDefault:
    Set the default font properties
******************************************************************************/
BOOL SetRtfFontDefault(PTERWND w,struct StrRtf far *rtf, struct StrRtfGroup far *group)
{
    int CurGroup;

    CurGroup=rtf->GroupLevel;

    lstrcpy(group[CurGroup].TypeFace,"");
    group[CurGroup].PointSize2=0;
    group[CurGroup].TextColor=TerFont[0].TextColor;
    if (RtfInput<RTF_CB && group[CurGroup].TextColor==0) group[CurGroup].TextColor=CLR_AUTO;
    group[CurGroup].TextBkColor=CLR_WHITE;
    group[CurGroup].UlineColor=CLR_AUTO;
    group[CurGroup].style=group[CurGroup].style&(FNOTEALL); // reset all other styles
    group[CurGroup].StyleOff=0;    // style explictly turned off
    group[CurGroup].CharStyId=1; // default character style id
    group[CurGroup].CharSet=DEFAULT_CHARSET; // default character character set
    group[CurGroup].AuxId=0;
    group[CurGroup].lang=rtf->lang;       // default language
    group[CurGroup].offset=0;             // baseline offset
    group[CurGroup].expand=0;             // character spacing
    group[CurGroup].CharScaleX=0;         // character scaling
    group[CurGroup].caps=FALSE;           // capitalization
    ResetLongFlag(group[CurGroup].gflags,GFLAG_CAPS);  // additional character styles

    group[CurGroup].CharBkPat=PAT_NONE;   // character background pattern
    group[CurGroup].CharPatFC=group[CurGroup].TextColor;  // character background pattern color (foreground)
    group[CurGroup].CharPatBC=group[CurGroup].TextBkColor;  // character background pattern color (background)

    if (RtfInput<RTF_CB || !TrackChanges) {
      group[CurGroup].revised=false;             // tracking variables
      group[CurGroup].deleted=false;
    }

    return TRUE;
}

/*****************************************************************************
    IsRtfPlainFont:
    Returns TRUE if font parameter did not change from the default in the specified group
******************************************************************************/
BOOL IsRtfPlainFont(PTERWND w,struct StrRtf far *rtf, struct StrRtfGroup far *group)
{

    if (lstrlen(group->TypeFace)>0) return FALSE;
    if (group->PointSize2!=0) return FALSE;
    if (group->TextColor!=TerFont[0].TextColor && group->TextColor!=CLR_AUTO) return FALSE;
    if (group->TextBkColor!=CLR_WHITE) return FALSE;
    if (group->UlineColor!=CLR_AUTO) return FALSE;
    if (group->style&(~(FNOTEALL))) return FALSE;
    if (group->CharStyId!=1) return FALSE; // default character style id
    if (group->CharSet!=DEFAULT_CHARSET) return FALSE; // default character character set
    if (group->AuxId!=0) return FALSE;
    if (group->lang!=rtf->lang) return FALSE;       // default language
    if (group->offset!=0) return FALSE;             // baseline offset

    if (group->CharBkPat!=PAT_NONE) return FALSE;   // character background pattern
    if (group->CharPatFC!=group->TextColor) return FALSE;  // character background pattern color (foreground)
    if (group->CharPatBC!=group->TextBkColor) return FALSE;  // character background pattern color (background)

    return TRUE;
}


/*****************************************************************************
    SetRtfParaDefault:
    Set the default paragraph properties
******************************************************************************/
BOOL SetRtfParaDefault(PTERWND w,struct StrRtf far *rtf, struct StrRtfGroup far *group)
{
    int CurGroup,i;
    UINT UserStyles;
    BOOL InFnote;

    CurGroup=rtf->GroupLevel;
    InFnote=(group[CurGroup].style&(FNOTEALL));

    if (rtf->InitialCell==0 && !InFnote) group[CurGroup].InTable=FALSE; // turnoff table
    if (!group[CurGroup].InTable) rtf->CellFlow=FLOW_DEF;  // reset text flow direction

    group[CurGroup].LeftIndent=0;
    group[CurGroup].RightIndent=0;
    group[CurGroup].FirstIndent=0;          // indentation of the first line
    group[CurGroup].flags=RtfInHdrFtr;      // reset all flags except current status of page/header footer
    group[CurGroup].ParaBorderColor=CLR_AUTO;  // auto color

    group[CurGroup].flow=FLOW_DEF;
    //if (RtfInput<RTF_CB) {                  // set the default rtl direction
    //   if (IsParaRtl(w,FLOW_DEF,rtf->CellFlow,rtf->SectFlow,rtf->DocFlow)) group[CurGroup].flow=FLOW_RTL;
    //}
       
    group[CurGroup].tab.count=0;
    group[CurGroup].BorderMargin=0;
    group[CurGroup].ParShading=0;
    group[CurGroup].CellShading=0;
    group[CurGroup].CellPatBC=CLR_WHITE;   // background color pattern
    group[CurGroup].CellPatFC=0;           // foreground color pattern
    group[CurGroup].CellColSpan=1;
    group[CurGroup].SpaceBefore=0;
    group[CurGroup].SpaceAfter=0;
    group[CurGroup].SpaceBetween=0;
    group[CurGroup].LineSpacing=0;
    group[CurGroup].ParaBkColor=CLR_WHITE;
    group[CurGroup].RtfLs=0;
    group[CurGroup].ListLvl=0;

    if (!InFnote) group[CurGroup].ParaStyId=0;  // can't change style in the middle of the footnote
    
    if (!(rtf->flags1&RFLAG1_SSSUBTABLE_USED) && !InFnote) group[CurGroup].level=RtfInitLevel;

    UserStyles=PFLAG_STYLES;     // break into two step as workaround for compiler problem
    group[CurGroup].pflags&=(~((UINT)UserStyles));   // protect internal flags
    if (rtf->SetWidowOrphan) group[CurGroup].pflags|=PFLAG_WIDOW;

    if (rtf->InitialParaFID==0 && !(group[CurGroup].gflags&(GFLAG_DO_TEXT_READ|GFLAG_IN_SHP)) && !InFnote) {
       FarMemSet(&(group[CurGroup].ParaFrameInfo),0,sizeof(struct StrRtfParaFrameInfo));
       group[CurGroup].FrmFlags=0;
       group[CurGroup].TextBoxMargin=0;
       group[CurGroup].ParaFID=0;
       group[CurGroup].TextAngle=0;
       ResetLongFlag(group[CurGroup].gflags,(GFLAG_NEW_FRAME|GFLAG_IN_FRAME));
    }
    
    FarMemSet(group[CurGroup].BorderWidth,0,(sizeof(int)*MAX_BORDERS));
    for (i=0;i<MAX_BORDERS;i++) group[CurGroup].BorderColor[i]=CLR_AUTO;

    return TRUE;
}

/*****************************************************************************
    CopyRtfRow:
    Create a new row by copying from the previous row
    This function is called when the 'row' control word
    is encountered.  This function copies the cells from the previous row
    to the newly created row.
******************************************************************************/
BOOL CopyRtfRow(PTERWND w,struct StrRtf far *rtf, struct StrRtfGroup far *group, int CurGroup)
{
    int CurRowId,PrevRowId,NextCell,PrevCell,PrevRowCell;
    int PrevRowPrevCell,PrevRowNextCell;
    long l;
    BOOL ExistingRow;

    if (rtf->flags&(RFLAG_OUT_OF_TABLE_CELL|RFLAG_OUT_OF_TABLE_ROW)) return TRUE;

    if (FALSE && rtf->InitialCell>0 && !rtf->PastingColumn) {   // outof initial cell now
       RtfCurCellId=RtfCurRowId=0;
       group->InTable=FALSE;
    }

    // the following condition arises when non-table text is found
    // inside a table. The editor will do the best it can do in this situation
    if (RtfCurRowId<=0) return SetRtfRowDefault(w,rtf,group,CurGroup);

    CurRowId=TableRow[RtfCurRowId].NextRow;
    if (rtf->PastingColumn && CurRowId<=0 && rtf->InitTblCol>=0) {   // ran out of existing table to copy into, so suspend reading
       rtf->SuspendReading=TRUE;
       return TRUE;
    }

    // create a new row
    if (CurRowId<=0 && (CurRowId=GetTableRowSlot(w))<0) {
       PrintError(w,MSG_OUT_OF_ROW_SLOT,"CopyRtfRow");
       rtf->flags|=RFLAG_OUT_OF_TABLE_ROW;
       return TRUE;                   // do not abort the read process
    }

    // set the group properties
    PrevRowId=RtfCurRowId;
    RtfCurRowId=CurRowId;

    // copy from the previous row
    ExistingRow=(rtf->PastingColumn && rtf->InitTblCol>=0 && TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST);
    if (!ExistingRow) {
       HugeMove(&(TableRow[PrevRowId]),&(TableRow[CurRowId]),sizeof(struct StrTableRow));
       HugeMove(&(TableAux[PrevRowId]),&(TableAux[CurRowId]),sizeof(struct StrTableAux));

       // set the table chain
       TableRow[PrevRowId].NextRow=CurRowId;
       TableRow[CurRowId].PrevRow=PrevRowId;
       TableRow[CurRowId].NextRow=-1;
       TableRow[CurRowId].flags=ResetUintFlag(TableRow[CurRowId].flags,(ROWFLAG_PREEXIST|ROWFLAG_NEW_TABLE));
       ResetUintFlag(TableAux[CurRowId].flags,TAUX_HAS_NEW_TEXT);
       TableAux[CurRowId].flags|=(TAUX_COPIED|TAUX_RTF_DEL);  // copied from the previous row, delete this row if data not encountered
    }

    
    // Find the first cell to copy from
    PrevRowCell=TableRow[PrevRowId].FirstCell;   // cell of the previous row
    while (PrevRowCell>0 && cell[PrevRowCell].flags&CFLAG_PREEXIST) PrevRowCell=cell[PrevRowCell].NextCell;
    if (PrevRowCell<=0) PrevRowCell=TableRow[PrevRowId].FirstCell;

    // find the first cell in the current row after which the copying begins
    PrevCell=-1;
    if (ExistingRow) {                           // position at the first new cell of the previous row
       if (rtf->InitTblCol==0) PrevCell=-1;
       else {
          int col=rtf->InitTblCol;               // find the cell on the current row after which the new cell is to be inserted
          PrevCell=TableRow[CurRowId].FirstCell;
          while (col>1 && cell[PrevCell].NextCell>0) {
             col-=cell[PrevCell].ColSpan;
             PrevCell=cell[PrevCell].NextCell;
          }
       }
    }
    else TableRow[CurRowId].FirstCell=TableRow[CurRowId].LastCell=-1;

    RtfCurCellId=RtfLastCellX=0;

    while (PrevRowCell>0 && !(cell[PrevRowCell].flags&CFLAG_PREEXIST)) {
       if ((NextCell=GetCellSlot(w,FALSE))<=0) {
          PrintError(w,MSG_OUT_OF_CELL_SLOT,"CopyRtfRow");
          rtf->flags|=RFLAG_OUT_OF_TABLE_CELL;
          return TRUE;                   // do not abort the read process
       }

       // copy the cell properties from the previous row
       CopyCell(w,PrevRowCell,NextCell);

       cell[NextCell].row=CurRowId;      // set current row id
       cell[NextCell].RowSpan=cell[NextCell].ColSpan=1;
       ResetUintFlag(CellAux[NextCell].flags,CAUX_CLOSED);  // a cell closed for data when \cell is encountered
       if (cell[NextCell].level>0) ResetUintFlag(cell[NextCell].flags,CFLAG_MERGED);  // for higher level table, row definition is always awailable, so don't copy this flag from the previous row

       InsertCell(w,NextCell,PrevCell,CurRowId,'A');

       PrevCell=NextCell;
       if (RtfCurCellId==0) RtfCurCellId=NextCell;  // cell at which the data is inserted
       RtfLastCellX=NextCell;                      // last cell defined in the cell chain


       // check if current cell in the previous row need to be merged with the previous cell
       if (cell[PrevRowCell].flags&CFLAG_MERGED) {
          PrevRowPrevCell=cell[PrevRowCell].PrevCell;
          PrevRowNextCell=cell[PrevRowCell].NextCell;

          if (PrevRowPrevCell>0) {
             cell[PrevRowPrevCell].NextCell=PrevRowNextCell;
             cell[PrevRowPrevCell].width+=cell[PrevRowCell].width;   // add the current cell width to the prev cell
             cell[PrevRowPrevCell].ColSpan++;                        // spans another column
             // transfer the right border specification
             ResetUintFlag(cell[PrevRowPrevCell].border,BORDER_RIGHT);
             cell[PrevRowPrevCell].border|=(cell[PrevRowCell].border&BORDER_RIGHT);
             cell[PrevRowPrevCell].BorderWidth[BORDER_INDEX_RIGHT]=cell[PrevRowCell].BorderWidth[BORDER_INDEX_RIGHT];
             cell[PrevRowPrevCell].BorderColor[BORDER_INDEX_RIGHT]=cell[PrevRowCell].BorderColor[BORDER_INDEX_RIGHT];
          }
          if (PrevRowNextCell>0) cell[PrevRowNextCell].PrevCell=PrevRowPrevCell;

          if (TableRow[PrevRowId].LastCell==PrevRowCell) TableRow[PrevRowId].LastCell=PrevRowPrevCell;

          // fix the cell id assignment
          for (l=0;l<TotalLines;l++) {
             if (cid(l)==PrevRowPrevCell && LineLen(l)>0 && pText(l)[LineLen(l)-1]==CellChar) {
                TransferTags(w,l,LineLen(l)-1);  // transfer any tags before deleting the chracter
                LineAlloc(w,l,LineLen(l),LineLen(l)-1);  // delete the cell break line
             }
             if (cid(l)==PrevRowCell) cid(l)=PrevRowPrevCell;
          }

          DelCell(w,PrevRowCell);

          PrevRowCell=PrevRowNextCell;      // advance to the next cell
       }
       else PrevRowCell=cell[PrevRowCell].NextCell;   // advance to the next cell
       
    }

    // set the cell after which new new will be inserted
    if (rtf->PastingColumn && rtf->InitTblCol>=0 && TableRow[PrevRowId].flags&ROWFLAG_PREEXIST) {
       if (RtfCurCellId>0 && cell[RtfCurCellId].PrevCell>0)
            rtf->InsertAftCell=cell[RtfCurCellId].PrevCell;
       else rtf->InsertAftCell=TableRow[PrevRowId].LastCell;
    }
    else rtf->InitTblCol=-1;                 // out of the initial table

    rtf->PrevCellX=0;

    return TRUE;
}

/*****************************************************************************
    SetRtfRowDefault
    Set the default for the table row.  This function is called to process
    the trowd control word.
******************************************************************************/
BOOL SetRtfRowDefault(PTERWND w,struct StrRtf far *rtf, struct StrRtfGroup far *group, int CurGroup)
{
    int CurRowId,PrevRowId,NextRowId,i;
    int CurCell;
    BOOL ExistingRow;

    if (rtf->flags&(RFLAG_OUT_OF_TABLE_CELL|RFLAG_OUT_OF_TABLE_ROW)) return TRUE;

    if (rtf->InitialCell>0 && !rtf->PastingColumn) {   // outof initial cell now
       int InitLevel=cell[rtf->InitialCell].level;
       int CurLevel=(RtfCurCellId>0?cell[RtfCurCellId].level:0);
       if (InitLevel==CurLevel) {
          RtfCurCellId=RtfCurRowId=0;
          group->InTable=FALSE;
       }
    }

    // delete the unused last row for the previous table
    if (rtf->OpenRowId>0) DeleteRtfRow(w,rtf,rtf->OpenRowId);
    rtf->OpenRowId=0;

    // check if this row is being initialized again?
    if (TableAux[RtfCurRowId].flags&TAUX_TROWD_DONE) {   // trowd already done for this row
       int cl=TableRow[RtfCurRowId].FirstCell;
       while (cl>0 && cell[cl].flags&CFLAG_PREEXIST) cl=cell[cl].NextCell;
       if (cl<=0 || cell[cl].PrevCell<=0) RtfLastCellX=0;
       else                               RtfLastCellX=cell[cl].PrevCell;  // reuse the cell chain from the beginning of the first new cell

       FarMemSet(group[CurGroup].BorderWidth,0,sizeof(int)*MAX_BORDERS);
       for (i=0;i<MAX_BORDERS;i++) group[CurGroup].BorderColor[i]=CLR_AUTO;

       group[CurGroup].CellShading=0;
       group[CurGroup].CellPatBC=CLR_WHITE;
       group[CurGroup].CellPatFC=0;
       group[CurGroup].CellColSpan=1;
       group[CurGroup].TextAngle=0;
       TableAux[RtfCurRowId].flags|=TAUX_REUSE_CELL_CHAIN;   // cell chains already formed during the previous trowd on this row
       return TRUE;
    }

    CurRowId=RtfCurRowId;

    if (CurRowId>0) {                          // initialize an existing row
        // delete any newly added cells if row data is not already started
        if (TableAux[CurRowId].flags&TAUX_HAS_NEW_TEXT) TableAux[CurRowId].flags|=TAUX_REUSE_CELL_CHAIN;
        else {                                 // delete the existing chain
           CurCell=TableRow[CurRowId].FirstCell;
           while (CurCell>0) {
              if (!(cell[CurCell].flags&CFLAG_PREEXIST)) RemoveCell(w,CurCell);
              CurCell=cell[CurCell].NextCell;
           }
        }

        // initialize if empty row
        if (TableRow[CurRowId].FirstCell<=0) {
           PrevRowId=TableRow[CurRowId].PrevRow;     // save prev row
           NextRowId=TableRow[CurRowId].NextRow;     // save next row
           FarMemSet(&(TableRow[CurRowId]),0,sizeof(struct StrTableRow));
           FarMemSet(&(TableAux[CurRowId]),0,sizeof(struct StrTableAux));

           TableRow[CurRowId].InUse=TRUE;
           TableRow[CurRowId].PrevRow=PrevRowId;     // restore prev row id
           TableRow[CurRowId].NextRow=NextRowId;     // restore next row id
           TableRow[CurRowId].flags=ResetUintFlag(TableRow[CurRowId].flags,ROWFLAG_PREEXIST);
           TableRow[CurRowId].CellMargin=0;  // DefCellMargin;  let it be 0 if not specified
        }
        TableRow[CurRowId].MinHeight=0;   // default height
    }
    else if ((CurRowId=GetTableRowSlot(w))>0) {// create a new cell
        TableRow[CurRowId].InUse=TRUE;
        PrevRowId=RtfCurRowId;
        RtfCurRowId=CurRowId;

        if (PrevRowId>0) {
           TableRow[PrevRowId].NextRow=CurRowId;
           TableRow[CurRowId].PrevRow=PrevRowId;
        }
        else TableRow[CurRowId].PrevRow=-1;
        TableRow[CurRowId].FirstCell=-1;   // first cell not yet set
        TableRow[CurRowId].NextRow=-1;

        TableRow[CurRowId].CellMargin=0;  // DefCellMargin;

        if (rtf->flags1&RFLAG1_FIRST_TABLE) {
           TableRow[CurRowId].flags|=ROWFLAG_NEW_TABLE;
           ResetUintFlag(rtf->flags1,RFLAG1_FIRST_TABLE);
        }
    }

    //  calculate the cell after which the new cells will be inserted
    ExistingRow=(rtf->PastingColumn && rtf->InitTblCol>=0 && TableRow[CurRowId].flags&ROWFLAG_PREEXIST);
    if (!(TableAux[CurRowId].flags&TAUX_HAS_NEW_TEXT))
        RtfCurCellId=0;                       // cell where the text is inserted - incremented on "cell" control word
    RtfLastCellX=0;                      // last cell during row construction - incremented on "cellx" control word

    if (ExistingRow) {                  // find the cell id at the insertion column
       int col=rtf->InitTblCol;
       if (col>0) {
          RtfLastCellX=TableRow[CurRowId].FirstCell;
          while (col>1 && cell[RtfLastCellX].NextCell>0) { // find the last cell before the new column gets inserted
             col-=cell[RtfLastCellX].ColSpan;
             RtfLastCellX=cell[RtfLastCellX].NextCell;
          }
       }
    }

    // initialize table info in the group table
    FarMemSet(group[CurGroup].BorderWidth,0,sizeof(int)*MAX_BORDERS);
    for (i=0;i<MAX_BORDERS;i++) group[CurGroup].BorderColor[i]=CLR_AUTO;

    group[CurGroup].CellShading=0;
    group[CurGroup].CellPatBC=CLR_WHITE;
    group[CurGroup].CellPatFC=0;
    group[CurGroup].CellColSpan=1;
    group[CurGroup].TextAngle=0;

    TableAux[RtfCurRowId].flags|=TAUX_TROWD_DONE;    // this row is initialized
    rtf->TableRead=TRUE;                  // table found

    return TRUE;
}

/*****************************************************************************
    CreateRtfCell:
    Create a new table cell.
******************************************************************************/
BOOL CreateRtfCell(PTERWND w,struct StrRtf far *rtf, struct StrRtfGroup far *group, int CurGroup)
{
    int i,NextCellId,CurX,NextCellX,margin,ParentCell,ParentRow;
    BOOL NewCellCreated=FALSE;
    UINT flags;

    if (rtf->flags&(RFLAG_OUT_OF_TABLE_CELL|RFLAG_OUT_OF_TABLE_ROW)) return TRUE;

    if (RtfCurRowId==0) {
       if (rtf->OpenRowId>0) {              // make use of the previously opened row
          RtfCurRowId=rtf->OpenRowId;
          RtfCurCellId=rtf->OpenCellId;
          RtfLastCellX=rtf->OpenLastCellX;
       }
       else {
          return PrintError(w,MSG_CELL_WITHOUT_ROW,NULL);
       }
    }

    // use existing cell chain of a reinitialized row
    NextCellId=-1;
    if (TableAux[RtfCurRowId].flags&TAUX_REUSE_CELL_CHAIN) {  // maintain the existing cell chain
       if (RtfLastCellX==0) NextCellId=TableRow[RtfCurRowId].FirstCell;
       else {
          NextCellId=cell[RtfLastCellX].NextCell;   // next cell might not have been defined - this would occur when more 'cell' than "cellx' encountered
          if (NextCellId>0 && cell[NextCellId].flags&CFLAG_PREEXIST) NextCellId=-1;  // end of newly created cell chain
       }
    }

    // create the new cell
    if (NextCellId<=0) {
       if ((NextCellId=GetCellSlot(w,FALSE))<=0) {
          PrintError(w,MSG_OUT_OF_CELL_SLOT,NULL);
          rtf->flags|=RFLAG_OUT_OF_TABLE_CELL;
          return TRUE;                   // do not abort the read process
       }
       NewCellCreated=TRUE;

       // set basic cell properties
       cell[NextCellId].InUse=TRUE;
       cell[NextCellId].row=RtfCurRowId;
       cell[NextCellId].FirstLine=cell[NextCellId].LastLine=-1;
       cell[NextCellId].level=rtf->CurTblLevel-1;    // zero based
       cell[NextCellId].ParentCell=ParentCell=rtf->TblLevel[rtf->CurTblLevel-1].CurCellId;
       if (HtmlMode) cell[NextCellId].flags|=CFLAG_FIX_WIDTH;

       if (ParentCell>0) {
          ParentRow=cell[ParentCell].row;
          TableRow[ParentRow].flags|=ROWFLAG_HAS_SUBTABLE;
       } 
       cell[NextCellId].PrevCell=cell[NextCellId].NextCell=-1;

    }

    // set margin   
    if (group[CurGroup].gflags&GFLAG_CELL_MARGIN_OVERRIDE) {    // check if margin specified at the row level is being overridden for this cell
       margin=group[CurGroup].CellMargin;
       cell[NextCellId].flags|=CFLAG_MARGIN_OVERRIDE;
    }
    else {
       margin=TableRow[RtfCurRowId].CellMargin;
    }
    margin=max(margin,group[CurGroup].BorderWidth[BORDER_CELL_LEFT]);
    margin=max(margin,group[CurGroup].BorderWidth[BORDER_CELL_RIGHT]);
    cell[NextCellId].margin=margin;

    // set borders
    cell[NextCellId].border=0;
    if (group[CurGroup].BorderWidth[BORDER_CELL_TOP])   cell[NextCellId].border|=BORDER_TOP;
    if (group[CurGroup].BorderWidth[BORDER_CELL_BOT])   cell[NextCellId].border|=BORDER_BOT;
    if (group[CurGroup].BorderWidth[BORDER_CELL_LEFT])  cell[NextCellId].border|=BORDER_LEFT;
    if (group[CurGroup].BorderWidth[BORDER_CELL_RIGHT]) cell[NextCellId].border|=BORDER_RIGHT;

    if (cell[NextCellId].border&BORDER_TOP)   cell[NextCellId].BorderWidth[BORDER_INDEX_TOP]  =group[CurGroup].BorderWidth[BORDER_CELL_TOP];
    if (cell[NextCellId].border&BORDER_BOT)   cell[NextCellId].BorderWidth[BORDER_INDEX_BOT]  =group[CurGroup].BorderWidth[BORDER_CELL_BOT];
    if (cell[NextCellId].border&BORDER_LEFT)  cell[NextCellId].BorderWidth[BORDER_INDEX_LEFT] =group[CurGroup].BorderWidth[BORDER_CELL_LEFT];
    if (cell[NextCellId].border&BORDER_RIGHT) cell[NextCellId].BorderWidth[BORDER_INDEX_RIGHT]=group[CurGroup].BorderWidth[BORDER_CELL_RIGHT];

    cell[NextCellId].BorderColor[BORDER_INDEX_TOP]  =group[CurGroup].BorderColor[BORDER_CELL_TOP];
    cell[NextCellId].BorderColor[BORDER_INDEX_BOT]  =group[CurGroup].BorderColor[BORDER_CELL_BOT];
    cell[NextCellId].BorderColor[BORDER_INDEX_LEFT] =group[CurGroup].BorderColor[BORDER_CELL_LEFT];
    cell[NextCellId].BorderColor[BORDER_INDEX_RIGHT]=group[CurGroup].BorderColor[BORDER_CELL_RIGHT];
    
    FarMemSet(&(group[CurGroup].BorderWidth[BORDER_CELL_TOP]),0,sizeof(int)*4);  // initialize the broder width
    for (i=0;i<4;i++) group[CurGroup].BorderColor[BORDER_CELL_TOP+i]=CLR_AUTO;   // initialize the group border color for cells

    // record the shading percent
    cell[NextCellId].shading=group[CurGroup].CellShading;
    cell[NextCellId].BackColor=group[CurGroup].CellPatBC;
    
    if (group[CurGroup].CellShading>0 && group[CurGroup].CellPatFC!=0) { // TE internally assumes black for foreground, so normalize it here, because we dont' store cell-foreground-pat color
       int shade=group[CurGroup].CellShading;
       COLORREF fc=group[CurGroup].CellPatFC;
       COLORREF bc=group[CurGroup].CellPatBC;
       cell[NextCellId].BackColor=GetShadedColor(w,fc,bc,shade);
       cell[NextCellId].shading=0;  // shading now applied
    } 
    
    if (group[CurGroup].CellColSpan>0) cell[NextCellId].ColSpan=group[CurGroup].CellColSpan;
    cell[NextCellId].TextAngle=group[CurGroup].TextAngle;

    group[CurGroup].CellShading=0;
    group[CurGroup].CellPatBC=CLR_WHITE;
    group[CurGroup].CellPatFC=0;
    group[CurGroup].CellColSpan=1;
    group[CurGroup].TextAngle=0;
    group[CurGroup].CellMargin=0;   // margin override
    ResetLongFlag(group[CurGroup].gflags,GFLAG_CELL_MARGIN_OVERRIDE);

    // record the cell flags
    flags=(CFLAG_VALIGN_CTR|CFLAG_VALIGN_BOT|CFLAG_VALIGN_BASE|CFLAG_FORCE_BKND_CLR);  // flags to record
    cell[NextCellId].flags=ResetUintFlag(cell[NextCellId].flags,flags);
    cell[NextCellId].flags|=(group[CurGroup].CellFlags&flags);
    group[CurGroup].CellFlags=ResetUintFlag(group[CurGroup].CellFlags,flags);

    // set the cell which will start receiving the data
    if (RtfCurCellId==0) RtfCurCellId=NextCellId;   // next cell where the text will be inserted

    // add new cell to the cell chain
    if (NewCellCreated) {
       InsertCell(w,NextCellId,RtfLastCellX,RtfCurRowId,'A'); // insert a cell after the last cell
    }

    // set cell width and postion
    if (RtfLastCellX>0) CurX=cell[RtfLastCellX].x+cell[RtfLastCellX].width;
    else                 CurX=TableRow[RtfCurRowId].indent;

    cell[NextCellId].x=CurX;
    NextCellX=(int)rtf->IntParam;   // starting x of the next cell

    if (TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST)
         cell[NextCellId].width=NextCellX-rtf->PrevCellX;
    else cell[NextCellId].width=NextCellX-CurX+TableRow[RtfCurRowId].AddedIndent;

    cell[NextCellId].FixWidth=cell[NextCellId].width;  // so the TerAjustHtmlTable will not change the table width

    rtf->PrevCellX=NextCellX;

    RtfLastCellX=NextCellId;                      // last cell added to the cell chain

    // is this cell to be merged with the previous cell
    if (rtf->flags&RFLAG_CELL_MERGED) {
       cell[NextCellId].flags|=CFLAG_MERGED;
       rtf->flags=ResetUintFlag(rtf->flags,RFLAG_CELL_MERGED);  // is set on clmrg control
    }
    else ResetUintFlag(cell[NextCellId].flags,CFLAG_MERGED);

    if (rtf->flags1&RFLAG1_ROW_SPANNED) {
       cell[NextCellId].flags|=CFLAG_ROW_SPANNED;
       rtf->flags1=ResetUintFlag(rtf->flags1,RFLAG1_ROW_SPANNED);  // is set on clmrg control
    }
    else ResetUintFlag(cell[NextCellId].flags,CFLAG_ROW_SPANNED);

    return TRUE;
}

/*****************************************************************************
    SendRtfText:
    Send currently buffered text to the text importer.  The text buffer
    is reinitialized.
******************************************************************************/
BOOL SendRtfText(PTERWND w,struct StrRtf far *rtf)
{
    BOOL result;
    struct StrRtfGroup far *group=&(rtf->group[rtf->GroupLevel]);

    if (rtf->OutBufLen==0) return TRUE;  // nothing to send

    if (rtf->GroupLevel==0) {            //  \rtf group not encountered yet
       rtf->OutBufLen=0;
       rtf->OutBuf[0]=0;
       return TRUE;
    }

    // do not allow nesting of fields
    if (rtf->InitFieldId>0 && rtf->group[rtf->GroupLevel].FieldId==FIELD_NAME) {
       rtf->OutBufLen=0;
       rtf->OutBuf[0]=0;
       return TRUE;
    } 

    if (rtf->group[rtf->GroupLevel].gflags&GFLAG_IN_STYLE_ITEM) return TRUE;  // this text actually is a style name

    // is this text going into a text box
    if (   group->gflags&GFLAG_INPUT_FIELD && rtf->pict>=0 && rtf->pict<TotalFonts
        && TerFont[rtf->pict].InUse && TerFont[rtf->pict].style&PICT) {
       struct StrForm far *pForm=(LPVOID)TerFont[rtf->pict].pInfo;
       int CurTextLen=lstrlen(pForm->InitText);
       if ((CurTextLen+rtf->OutBufLen)>MAX_WIDTH) rtf->OutBufLen=MAX_WIDTH-CurTextLen;
       if (rtf->OutBufLen<0) rtf->OutBufLen=0;
       rtf->OutBuf[rtf->OutBufLen]=0;
       lstrcat(pForm->InitText,rtf->OutBuf);
       rtf->OutBufLen=0;
       rtf->OutBuf[0]=0;
       return TRUE;
    }

    // use default font if necessary
    if (lstrlen(rtf->group[rtf->GroupLevel].TypeFace)==0) GetRtfDefaultFont(w,rtf);
    
    if (rtf->OutBufLen==1 && rtf->OutBuf[0]==SECT_CHAR)
           result=ImportRtfData(w,RTF_SECT,&(rtf->group[rtf->GroupLevel]),NULL,(LPBYTE)rtf);
    else   result=ImportRtfData(w,RTF_TEXT,&(rtf->group[rtf->GroupLevel]),NULL,rtf->OutBuf);

    if (rtf->OutBufLen>0) rtf->PrevChar=rtf->OutBuf[rtf->OutBufLen-1];

    rtf->OutBuf[0]=0;                    // intialize the text buffer
    rtf->OutBufLen=0;

    rtf->flags|=RFLAG_HDRFTR_TEXT_FOUND;  // some header/footer text found
    rtf->flags1|=RFLAG1_TEXT_FOUND;       // some regular text found

    return result;
}

/*****************************************************************************
    GetRtfDefaultFont:
    Get the default rtf font in the current group
******************************************************************************/
GetRtfDefaultFont(PTERWND w,struct StrRtf far *rtf)
{
    struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
    int DefFont;

    DefFont=rtf->group[rtf->GroupLevel].DefFont;
    lstrcpy(rtf->group[rtf->GroupLevel].TypeFace,pRtfGroup->font[DefFont].name);
    lstrcpy(rtf->group[rtf->GroupLevel].FontFamily,pRtfGroup->font[DefFont].family);
    rtf->group[rtf->GroupLevel].CharSet=pRtfGroup->font[DefFont].CharSet;
  
    return TRUE;
}
 
/*****************************************************************************
    SkipRtfGroup:
    Skip the current RTF group.  This function return 0 when sucessful, otherwise
    it returns an error code (RTF_FILE_INCOMPLETE or RTF_SYNTAX_ERROR)
******************************************************************************/
SkipRtfGroup(PTERWND w,struct StrRtf far *rtf)
{
    int  ControlGroupLevel=0;

    ControlGroupLevel=rtf->GroupLevel;

    while (TRUE) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;
       if (rtf->GroupEnd && rtf->GroupLevel<ControlGroupLevel) break;    // group ended
    }
    return 0;
}

/******************************************************************************
    ReadRtfFontTable:
    Read the RTF font table.  This function return 0 when sucessful, otherwise
    it returns an error code (RTF_FILE_INCOMPLETE or RTF_SYNTAX_ERROR)
******************************************************************************/
ReadRtfFontTable(PTERWND w,struct StrRtf far *rtf)
{
    struct StrRtfFont huge *font;
    struct StrRtfGroup far *pRtfGroup;
    int    i,len,FontId,CurFont,ControlGroupLevel=0,CurGroupLevel,WordLen,CharSet;
    BOOL   ExtractingName,NameWordRead;

    ControlGroupLevel=rtf->GroupLevel;

    pRtfGroup=GetRtfGroup(w,rtf);
    font=pRtfGroup->font;

    while (TRUE) {
       // get font number
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // font group ended
          else                                   continue; // look for next font
       }

       if (rtf->GroupLevel==ControlGroupLevel && !rtf->IsControlWord) continue;   // ignore anything between the fonts

       if (lstrcmpi(rtf->CurWord,"f")!=0) {
         if (!rtf->IsControlWord) return RTF_SYNTAX_ERROR;
         else {
           int ReturnCode=ProcessRtfControl(w,rtf);
           if (ReturnCode!=0) return ReturnCode;
           else               continue;
         }
       }
       FontId=(int)(rtf->IntParam);

       // get the rtf font table slot
       if (FontId>=0 && FontId<MAX_RTF_SEQ_FONTS) CurFont=FontId;  // lower numbers are stored sequentially
       else {
          if (pRtfGroup->MaxRtfFonts<MAX_RTF_SEQ_FONTS) CurFont=MAX_RTF_SEQ_FONTS;
          else for (CurFont=MAX_RTF_SEQ_FONTS;CurFont<pRtfGroup->MaxRtfFonts;CurFont++) if (!(font[CurFont].InUse)) break;
       }

       if (CurFont<0 || CurFont>=MAX_RTF_FONTS) return RTF_SYNTAX_ERROR;

       if (CurFont>=pRtfGroup->MaxRtfFonts) {    // allocate
          int NewMaxRtfFonts=CurFont+50;
          if (NewMaxRtfFonts>=MAX_RTF_FONTS) NewMaxRtfFonts=MAX_RTF_FONTS-1;
          if (NewMaxRtfFonts<(CurFont+1))    NewMaxRtfFonts=CurFont+1;

          if (NULL==(pRtfGroup->font=(struct StrRtfFont huge *)OurRealloc(pRtfGroup->font,sizeof(struct StrRtfFont)*(DWORD)NewMaxRtfFonts))) {
             PrintError(w,MSG_OUT_OF_MEM,"ReadRtfFontTable");
             return RTF_SYNTAX_ERROR;
          }
          for (i=pRtfGroup->MaxRtfFonts;i<NewMaxRtfFonts;i++) FarMemSet(&(pRtfGroup->font[i]),0,sizeof(struct StrRtfFont));  // initialize with zeros

          pRtfGroup->MaxRtfFonts=NewMaxRtfFonts;
          font=pRtfGroup->font;
       }

       // initialize the rtf font table slot
       font[CurFont].InUse=TRUE;
       font[CurFont].FontId=FontId;
       font[CurFont].CharSet=DEFAULT_CHARSET;
       if (lstrlen(font[CurFont].family)>0) font[CurFont].family[0]=0;  // erase previous specifiation if any
       if (lstrlen(font[CurFont].name)>0)   font[CurFont].name[0]=0;  // erase previous specifiation if any

       // get font family
       NameWordRead=FALSE;
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;
       if (rtf->IsControlWord) {  // font family found
          BOOL fUsed=TRUE;
          if (rtf->WordLen<=1) return RTF_SYNTAX_ERROR;
          if (lstrcmpi(rtf->CurWord,"fcharset")==0) {
             font[CurFont].CharSet=CharSet=(int)rtf->IntParam;
             if (!mbcs && CharSet!=DEFAULT_CHARSET && CharSet!=ANSI_CHARSET 
                  && CharSet!=SYMBOL_CHARSET  && IsMbcsCharSet(w,CharSet,NULL)) EnableMbcs(w,CharSet);
          }
          else {
            if (rtf->CurWord[0]!='f' && rtf->CurWord[0]!='F') fUsed=FALSE;
            rtf->CurWord[29]=0;   // truncate to max length
            if (fUsed) lstrcpy(font[CurFont].family,&(rtf->CurWord[1])); //exclude first 'f' character
            else  if (lstrlen(font[CurFont].family)==0) lstrcpy(font[CurFont].family,rtf->CurWord);
            StrTrim(font[CurFont].family);
          }
       }
       else NameWordRead=TRUE;  // first word of the font name read

       // get font name
       CurGroupLevel=rtf->GroupLevel;
       ExtractingName=TRUE;

       while (TRUE) {           // get each name framgment
         if (!NameWordRead && !GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;
         NameWordRead=FALSE;    // reset for the next read

         if (rtf->GroupEnd && rtf->GroupLevel<CurGroupLevel) {  // font name group ending without ';'
             StrTrim(font[CurFont].name);
             break;
         }

         if (rtf->WordLen==0)    continue;
         if (rtf->IsControlWord) {
            if (lstrcmpi(rtf->CurWord,"falt")==0) {
               BYTE FontName[32];
               BOOL FontExists;

               // check if the original font exists
               GetCharSet(w,hTerDC,font[CurFont].name,NULL,NULL,&FontExists);
               if (FontExists) SkipRtfGroup(w,rtf); // skip this group
               else {
                  lstrcpy(FontName,font[CurFont].name);
                  lstrlwr(w,FontName);   // check if one of the regular font found
                  if ((strcmp(FontName,"arial") == 0) ||
                      (strcmp(FontName,"courier") == 0) ||
                      (strcmp(FontName,"courier new") == 0) ||
                      (strcmp(FontName,"times new roman") == 0)) {
                     if (rtf->GroupLevel>CurGroupLevel) SkipRtfGroup(w,rtf);  // skip this group
                  }
                  else {
                     lstrcpy(font[CurFont].name2,font[CurFont].name);  // save the old name
                     font[CurFont].name[0]=0;  // alternate font name supplied
                     ExtractingName=TRUE;      // retextract the name
                  }
               }
            }
            else if (lstrcmpi(rtf->CurWord,"fcharset")==0) {
               font[CurFont].CharSet=CharSet=(int)rtf->IntParam;
               if (!mbcs && CharSet!=DEFAULT_CHARSET && CharSet!=ANSI_CHARSET 
                    && CharSet!=SYMBOL_CHARSET  && IsMbcsCharSet(w,CharSet,NULL)) EnableMbcs(w,CharSet);
            }
            else if (rtf->GroupLevel>CurGroupLevel) SkipRtfGroup(w,rtf);  // skip this group
            continue;          // ignore any other control word
         }

         if (ExtractingName && (strlen(font[CurFont].name)+strlen(rtf->CurWord))<32) lstrcat(font[CurFont].name,rtf->CurWord);
         WordLen=lstrlen(rtf->CurWord);

         if (WordLen>0 && rtf->CurWord[WordLen-1]==';') {
            len=lstrlen(font[CurFont].name);
            if (ExtractingName && len>0) {
                BOOL FontExists=FALSE;

                font[CurFont].name[len-1]=0; // exclude the semicolon
                StrTrim(font[CurFont].name);
                
                // check if this font exists in the system
                GetCharSet(w,hTerDC,font[CurFont].name,NULL,NULL,&FontExists);
                if (!FontExists) {  // check if it is a language specific font, extract the font base name
                   LPBYTE LangDelim="Western,Greek,Cyr,CE,(Hebrew),(Arabic),(Baltic)";
                   LPBYTE pDelim;
                   for (i=strlen(font[CurFont].name)-1;i>=0;i--) if (font[CurFont].name[i]==' ') break;
                   pDelim=&(font[CurFont].name[i+1]);
                   if (strstr(LangDelim,pDelim)) {
                      (*pDelim)=0;                       // use only the font base name
                      StrTrim(font[CurFont].name);
                   } 
                } 
                
                if (lstrlen(font[CurFont].name)==0) {
                  if (font[CurFont].CharSet==font[0].CharSet) {
                     lstrcpy(font[CurFont].name,font[0].name);
                     lstrcpy(font[CurFont].family,font[0].family);
                  }
                  else {
                     lstrcpy(font[CurFont].name,"Arial");
                     lstrcpy(font[CurFont].family,"swiss");
                  }
                }
            }
            if (rtf->GroupLevel==CurGroupLevel) break; // complete name derived
            else ExtractingName=FALSE;     // name extracted
         }
       }
    }

    // set the new default font
    GetRtfDefaultFont(w,rtf);
    if (RtfInput<RTF_CB) {
       if (   strcmpi(pRtfGroup[rtf->GroupLevel].TypeFace,TerFont[0].TypeFace)!=0 
          || (pRtfGroup[rtf->GroupLevel].PointSize2)!=(TerFont[0].TwipsSize/10)
          ||  pRtfGroup[rtf->GroupLevel].style!=TerFont[0].style)  {
          if (strlen(pRtfGroup[rtf->GroupLevel].TypeFace)!=0 && pRtfGroup[rtf->GroupLevel].PointSize2!=0) 
             SetTerDefaultFont(hTerWnd,pRtfGroup[rtf->GroupLevel].TypeFace,pRtfGroup[rtf->GroupLevel].PointSize2/2,pRtfGroup[rtf->GroupLevel].style,CLR_AUTO,false);
       }
    }

    return 0;                 // successful
}

/*****************************************************************************
    ReadRtfFormField:
    This routine reads the formfield information.
******************************************************************************/
ReadRtfFormField(PTERWND w,struct StrRtf far *rtf)
{
    int   ControlGroupLevel=0,NameGroup;
    struct StrRtfPict pic;
    struct StrForm far *pForm;
    struct StrRtfGroup far *pGroup;
    BOOL InName=FALSE;

    pGroup=rtf->group;
    pGroup=&(pGroup[rtf->GroupLevel]);
    
    if (lstrlen(rtf->group[rtf->GroupLevel].TypeFace)==0) GetRtfDefaultFont(w,rtf);  // set the font for the group

    ControlGroupLevel=rtf->GroupLevel;
    FarMemSet(&pic,0,sizeof(pic));  // initialize
    pic.type=PICT_FORM;             // picture type
    pic.FormId=-1;                  // set to no type
    pic.width=TerFont[0].TwipsSize/10; // default width
    pic.height=TerFont[0].TwipsSize/10; // default width

    pic.InfoSize=sizeof(struct StrForm);
    if ( (NULL==(pic.pInfo=OurAlloc(pic.InfoSize)))) {
        return PrintError(w,MSG_OUT_OF_MEM,"ReadRtfFormField");
    }
    pForm=(LPVOID)pic.pInfo;
    FarMemSet(pForm,0,sizeof(struct StrForm));
    pForm->FontId=-1;        // not yet created


    while (TRUE) {
       // get the next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (InName && rtf->GroupLevel<NameGroup) InName=FALSE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // field group ended
          else                                   continue; // look for the next conponent
       }

       if (rtf->IsControlWord) {
          if (lstrcmpi(rtf->CurWord,"fftype")==0) {
             if ((int)rtf->IntParam==0) {
                pic.FormId=FIELD_TEXTBOX;
             }
             else if ((int)rtf->IntParam==1) {
                pic.FormId=FIELD_CHECKBOX;
                lstrcpy(pForm->class,"Button");
                pForm->style=BS_AUTOCHECKBOX;
             }
             else if ((int)rtf->IntParam==2) {
                pic.FormId=FIELD_LISTBOX;
                lstrcpy(pForm->class,"ComboBox");
                pForm->style=WS_BORDER|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_AUTOHSCROLL;
             }
            pForm->style|=(WS_CHILD|WS_CLIPSIBLINGS); // force child window
          }
          else if (lstrcmpi(rtf->CurWord,"ssfldw")==0) {  // field width in twips
             pic.width=(int)rtf->IntParam;
             pForm->flags|=FMFLAG_SSFLDW_FOUND;
          }
          else if (lstrcmpi(rtf->CurWord,"ffhps")==0) {  // size in half points
             pic.height=pic.width=(int)rtf->IntParam*10;  // half-point to twips
             pic.bmWidth=TwipsToScrX((int)pic.width);
             pic.bmHeight=TwipsToScrY((int)pic.height);
          }
          else if (lstrcmpi(rtf->CurWord,"ffmaxlen")==0) {  // length of the text box in number of characters
             pForm->MaxLen=(int)rtf->IntParam;   // store the max len
          }
          else if (lstrcmpi(rtf->CurWord,"ssffborder")==0) {  // border around the text fied
             pForm->style|=WS_BORDER;
          }
          else if (lstrcmpi(rtf->CurWord,"ffres")==0
                 || lstrcmpi(rtf->CurWord,"ffdefres")==0) {  // result field
             pForm->InitNum=(int)rtf->IntParam;
             if (pic.FormId==FIELD_CHECKBOX && pForm->InitNum>1) pForm->InitNum=0;  // only 0 or 1 allowed
          }
          else if (lstrcmpi(rtf->CurWord,"ffname")==0) {  // forfield name
             InName=TRUE;
             NameGroup=rtf->GroupLevel;
          }
       }
       else {
          if (InName) lstrcat(pForm->name,rtf->CurWord);  // extract the field name
       }
    }


    if (pic.FormId<0) return 0;     // invalid picture

    // pass the picture to the application
    if (pic.FormId==FIELD_TEXTBOX) {
       int i,id,grp=pGroup->FieldGroup;    // save the information with the field group so itwill be passed to fldrslt
       BYTE LenStr[10];

       rtf->group[grp].FieldId=FIELD_TEXTBOX;
       
       // save the field code
       itoa(pForm->MaxLen,LenStr,10);
       rtf->FieldCode=OurAlloc(strlen(LenStr)+1+strlen(pForm->name)+1);
       lstrcpy(rtf->FieldCode,LenStr);
       lstrcat(rtf->FieldCode,"|");
       lstrcat(rtf->FieldCode,pForm->name);

       // assigne the field id
       id=70000;   // Aux id works like field id
       for (i=0;i<TotalFonts;i++) {
         if (TerFont[i].FieldId==FIELD_TEXTBOX && TerFont[i].AuxId>=id) id=TerFont[i].AuxId+1;
       } 
       rtf->group[grp].AuxId=id;   // field id

    }
    else ImportRtfData(w,RTF_PICT,&(rtf->group[rtf->GroupLevel]),NULL,(LPBYTE)&pic);

    return 0;
}

/*****************************************************************************
    ReadRtfBookmark:
    This routine read the bookmark.
******************************************************************************/
ReadRtfBookmark(PTERWND w,struct StrRtf far *rtf)
{
    int   ControlGroupLevel=0,TagId;
    BYTE  name[MAX_WIDTH+1];

    ControlGroupLevel=rtf->GroupLevel;
    name[0]=0;

    while (TRUE) {
       // get the next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // field group ended
          else                                   continue; // look for the next conponent
       }
       if (rtf->IsControlWord) continue;

       if ((lstrlen(name)+lstrlen(rtf->CurWord))<MAX_WIDTH) lstrcat(name,rtf->CurWord);
    }

    StrTrim(name);
    if (strstr(name,"_Toc")==name) return 0;     // ignore the bookmarks used for table of contents
    
    if (lstrlen(name)>0) {
       if (-1==(TagId=GetTagSlot(w))) return 0;

       CharTag[TagId].InUse=TRUE;
       CharTag[TagId].type=TERTAG_BKM;

       if (NULL==(CharTag[TagId].name=MemAlloc(lstrlen(name)+1))) return 0;
       lstrcpy(CharTag[TagId].name,name);

       if (rtf->TagId) {                 // append to the chain
          int LastTag=rtf->TagId;
          while (CharTag[LastTag].next) LastTag=CharTag[LastTag].next;
          CharTag[LastTag].next=TagId;   // connect to the chain
       }
       else rtf->TagId=TagId;
    }

    return 0;                 // successful
}

/*****************************************************************************
    ReadRtfTag:
    This routine read the user tag.
******************************************************************************/
ReadRtfTag(PTERWND w,struct StrRtf far *rtf)
{
    int   ControlGroupLevel=0,TagId,AuxInt=0,len;
    BYTE  name[MAX_WIDTH];
    LPBYTE  AuxText=NULL;
    BOOL   ReadName=true,ReadText=false;

    ControlGroupLevel=rtf->GroupLevel;
    name[0]=0;

    while (true) {
       // get the next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (w,rtf->GroupBegin) continue;
       if (w,rtf->GroupEnd) {
          if (w,rtf->GroupLevel<ControlGroupLevel) break;    // field group ended
          else                                   continue; // look for the next conponent
       }
       if (rtf->IsControlWord) {
          if (lstrcmpi(rtf->CurWord,"sstagint")==0) {
             AuxInt=(int)rtf->IntParam;
             ReadName=false;    
             ReadText=true;  // AuxText follows AuxInt
          }
          continue;
       }

       if (ReadName && (lstrlen(name)+lstrlen(rtf->CurWord))<MAX_WIDTH) lstrcat(name,rtf->CurWord);
       if (ReadText) {
          len=lstrlen(rtf->CurWord);
          if (AuxText==null) {
             AuxText=MemAlloc(len+1);
             AuxText[0]=0;
          }
          else AuxText=MemReAlloc(AuxText,lstrlen(AuxText)+len+1);
          lstrcat(AuxText,rtf->CurWord);
       }
    }

    StrTrim(name);
    
    if (lstrlen(name)>0) {
       if (-1==(TagId=GetTagSlot(w))) return 0;

       CharTag[TagId].InUse=true;
       CharTag[TagId].type=TERTAG_USER;
       CharTag[TagId].AuxInt=AuxInt;
       CharTag[TagId].AuxText=AuxText;

       if (NULL==(CharTag[TagId].name=MemAlloc(lstrlen(name)+1))) return 0;
       lstrcpy(CharTag[TagId].name,name);

       if (True(rtf->TagId)) {                 // append to the chain
          int LastTag=(int)rtf->TagId;
          while (CharTag[LastTag].next>0) LastTag=CharTag[LastTag].next;
          CharTag[LastTag].next=(int)TagId;   // connect to the chain
       }
       else rtf->TagId=TagId;
    }

    return 0;                 // successful
}

/*****************************************************************************
    ReadRtfField:
    This routine currently simply reads special SYMBOL field output by
    Microsoft Word 6.0
******************************************************************************/
ReadRtfField(PTERWND w,struct StrRtf far *rtf)
{
    int   i,len,ControlGroupLevel=0,InstGroupLevel=0;
    BYTE  CurWord[MAX_WIDTH+1],FontName[50],FontSize[50],quote=34,symbol;
    BOOL  InstUsed=FALSE,IsSymbol=FALSE,InFontName=FALSE,InFontSize=FALSE;
    BOOL  SkipInst=FALSE,TextInputField=FALSE,EmbeddedField;
    BOOL  InstFound=FALSE,InMergeField=FALSE,InFieldCode=FALSE;
    UINT  SymStyle;
    COLORREF SymTextColor,SymTextBkColor;

    ControlGroupLevel=rtf->GroupLevel;
    
    EmbeddedField=(rtf->group[ControlGroupLevel].gflags&GFLAG_IN_FIELD)?TRUE:FALSE;

    rtf->group[ControlGroupLevel].gflags|=GFLAG_IN_FIELD;
    rtf->group[ControlGroupLevel].FieldGroup=ControlGroupLevel;
    
    if (rtf->FieldCode && !EmbeddedField) {
       OurFree(rtf->FieldCode);
       rtf->FieldCode=NULL;
    }


    while (TRUE) {
       // get the next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // field group ended
          else                                   continue; // look for the next conponent
       }

       // read the instruction group
       if (lstrcmpi(rtf->CurWord,"fldinst")==0 && rtf->IsControlWord) {
          if (rtf->InitFieldId>0) {
             SkipRtfGroup(w,rtf);
             InstFound=TRUE;
             continue;
          } 
          
          InstGroupLevel=rtf->GroupLevel;
          while (TRUE) {
             if (rtf->GroupEnd && rtf->GroupLevel<InstGroupLevel) break;    // this can happen when ReadRtfGroupText function reads the fldinst ending group braces character

             // get the next word
             if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

             if (rtf->GroupBegin) continue;
             if (rtf->GroupEnd) {
                if (rtf->GroupLevel<InstGroupLevel) break;    // fldinst group ended
                else                                continue; // look for the next conponent
             }

             if (SkipInst) continue;                 // skip this instruction

             lstrcpy(CurWord,rtf->CurWord);          // trim this word

             // process any rtf control
             if (rtf->IsControlWord) {
                int ReturnCode;

                if (InFieldCode && strcmp(CurWord,"u")==0) continue;  // ignore unicode characters in the field code
                ReturnCode=ProcessRtfControl(w,rtf);
                if (ReturnCode!=0) return ReturnCode;
                if (lstrcmpi(CurWord,"formfield")==0) InstUsed=TRUE;  // this control is processed
                continue;
             }

             // look for symbol word
             StrTrim(CurWord);
             if (lstrlen(CurWord)==0) continue;
             
             // only the INCLUDEPICTURE field is allowed to be nested because INCLUDEPICTURE is not 
             // really a field (simply a linked picture)
             if (EmbeddedField && lstrcmpi(CurWord,"INCLUDEPICTURE")!=0) {
                 SkipInst=TRUE;  // process only the result
                 continue;
             }

             // check if field code is being read
             if (InFieldCode) {
                rtf->FieldCode=OurRealloc(rtf->FieldCode,strlen(rtf->FieldCode)+strlen(CurWord)+1);
                strcat(rtf->FieldCode,CurWord);
                continue;
             } 

             InstFound=TRUE;        // some instruction found

             if (lstrcmpi(CurWord,"SYMBOL")==0) {
                if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;   // get the symbol id
                lstrcpy(CurWord,rtf->CurWord);        // trim this word
                StrTrim(CurWord);
                if (!rtf->IsControlWord) {
                  if (lstrcmpi(CurWord,"183")==0) {   // bullet
                    rtf->OutBuf[rtf->OutBufLen]=(BYTE)149;
                    rtf->OutBufLen++;
                    rtf->OutBuf[rtf->OutBufLen]=0;     // NULL terination
                    InstUsed=TRUE;                     // don't need the fldrsult
                  }
                  else {
                    IsSymbol=TRUE;                        // symbol encountered
                    InFontName=InFontSize=FALSE;
                    FontName[0]=FontSize[0]=0;            // font name/size attribute

                    len=lstrlen(CurWord);  // check if numeric
                    for (i=0;i<len;i++) if (CurWord[i]<'0' || CurWord[i]>'9') break;
                    if (i==len) {  // insert a character of the symbol font
                       int CurLevel=rtf->GroupLevel;

                       if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
                       lstrcpy(TempString,CurWord);    // copy to ds
                       symbol=(BYTE)(atoi(TempString));

                       // set the default symbol font
                       lstrcpy(rtf->group[CurLevel].TypeFace,"Symbol");
                       lstrcpy(rtf->group[CurLevel].FontFamily,"");
                       
                       // gather other character information
                       SymStyle=rtf->group[CurLevel].style;
                       SymTextColor=rtf->group[CurLevel].TextColor;
                       SymTextBkColor=rtf->group[CurLevel].TextBkColor;
                       
                       InstUsed=TRUE;       // don't need the fldrsult
                    }
                  }
                }
             }
             else if (IsSymbol) {
                if (lstrcmpi(CurWord,"\\")==0) InFontName=InFontSize=FALSE;

                if (InFontName) lstrcat(FontName,rtf->CurWord);
                if (InFontSize) lstrcat(FontSize,rtf->CurWord);

                if (lstrcmpi(CurWord,"f")==0)  InFontName=TRUE;
                if (lstrcmpi(CurWord,"s")==0)  InFontSize=TRUE;
             }
             else if (lstrcmpi(CurWord,"s")==0 && IsSymbol) {
                InFontName=FALSE;
                InFontSize=TRUE;
             }
             else if (lstrcmpi(CurWord,"PAGE")==0 && !HtmlMode) {
                // insert the page number
                int CurLevel=rtf->GroupLevel;

                if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
                rtf->OutBuf[0]='1';
                rtf->OutBuf[1]=0;
                rtf->OutBufLen=1;
                rtf->group[CurLevel].FieldId=FIELD_PAGE_NUMBER;
                if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE;    // send the page number
                InstUsed=TRUE;       // this instruct used, do not use the result control word now
             }
             else if (lstrcmpi(CurWord,"NUMPAGES")==0 && !HtmlMode) {
                // insert the page count
                int CurLevel=rtf->GroupLevel;

                if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
                rtf->OutBuf[0]='1';
                rtf->OutBuf[1]=0;
                rtf->OutBufLen=1;
                rtf->group[CurLevel].FieldId=FIELD_PAGE_COUNT;
                if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE;    // send the page number
                InstUsed=TRUE;       // this instruct used, do not use the result control word now
             }
             else if ((lstrcmpi(CurWord,"TIME")==0 || lstrcmpi(CurWord,"DATE")==0 
                        || lstrcmpi(CurWord,"PRINTDATE")==0) && !HtmlMode) {       // date/time
                BYTE DateFmt[100]="";                             // read date format
                int i,len,begin,DateGroupLevel=rtf->GroupLevel;
                LPBYTE pDateFmt;
                int FieldId=lstrcmpi(CurWord,"PRINTDATE")==0?FIELD_PRINTDATE:FIELD_DATE;
                
                while (GetRtfWord(w,rtf)) {           // extract the date format
                    if (rtf->GroupBegin) continue;
                    if (rtf->GroupEnd) {
                        if (rtf->GroupLevel<DateGroupLevel) break;
                        else  continue; // look for the next conponent
                    }
                    if (!rtf->IsControlWord) lstrcat(DateFmt,rtf->CurWord);
                }
                StrTrim(DateFmt);
                len=lstrlen(DateFmt);
                for (i=0;i<len;i++) if (DateFmt[i]=='"') break;     // beginning quotation
                begin=i+1;  // format begin 
                pDateFmt=&(DateFmt[begin]);
                for (i=begin;i<len;i++) if (DateFmt[i]=='"') break; // ending quotation
                DateFmt[i]=0;                                       // end the string here

                // save the date code
                rtf->FieldCode=OurAlloc(lstrlen(pDateFmt)+1);
                lstrcpy(rtf->FieldCode,pDateFmt);

                if (FieldId==FIELD_DATE) {
                   // transmit this field character
                   GetDateString(w,pDateFmt,DateFmt,-1);           // get the date string

                   if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
                   lstrcpy(rtf->OutBuf,DateFmt);
                   rtf->OutBufLen=lstrlen(rtf->OutBuf);
                   
                   rtf->GroupLevel++;  // reset to the fldinst group temporarily to capture any font specs
                   rtf->group[rtf->GroupLevel].FieldId=FieldId;
                   if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE;    // send the page number
                   rtf->GroupLevel--;  // reset to the current group

                   InstUsed=TRUE;
                }
                else {                    // the result will be read from the rtf file
                   for (i=ControlGroupLevel;i<=rtf->GroupLevel;i++) rtf->group[i].FieldId=FieldId;
                }
                 
                if (rtf->GroupLevel<InstGroupLevel) goto INST_DONE;    
             }   
             else if (    (lstrcmpi(CurWord,"TOC")==0 && !HtmlMode)
                       || lstrcmpi(CurWord,"LISTNUM")==0 
                       || lstrcmpi(CurWord,"PAGEREF")==0
                       || lstrcmpi(CurWord,"AUTONUMLGL")==0
                       || lstrcmpi(CurWord,"HYPERLINK")==0  ) {  // table of contents
                BYTE FldFmt[MAX_WIDTH+1];                      // read date format
                int i;
                BOOL IsToc=(lstrcmpi(CurWord,"TOC")==0);
                BOOL IsListnum=(lstrcmpi(CurWord,"LISTNUM")==0);
                BOOL IsAutoNumLgl=(lstrcmpi(CurWord,"AUTONUMLGL")==0);
                BOOL IsHyperlink=(lstrcmpi(CurWord,"HYPERLINK")==0);
                BOOL IsPageRef=(lstrcmpi(CurWord,"PAGEREF")==0);
                UINT style;
                
                // save current formaating
                style=rtf->group[rtf->GroupLevel].style;
                           
                ReadRtfGroupText(w,rtf,FldFmt);

                // save the field code
                rtf->FieldCode=OurAlloc(lstrlen(FldFmt)+1);
                lstrcpy(rtf->FieldCode,FldFmt);
                InFieldCode=TRUE;                      // more field code data might follow

                for (i=ControlGroupLevel;i<=rtf->GroupLevel;i++) {
                    if (IsToc)        rtf->group[i].FieldId=FIELD_TOC;
                    if (IsListnum)    rtf->group[i].FieldId=FIELD_LISTNUM;
                    if (IsAutoNumLgl) rtf->group[i].FieldId=FIELD_AUTONUMLGL;
                    if (IsHyperlink)  rtf->group[i].FieldId=FIELD_HLINK;
                    if (IsPageRef)    rtf->group[i].FieldId=FIELD_PAGE_REF;
                    rtf->group[i].style=style;
                }

                if (rtf->GroupLevel<InstGroupLevel) goto INST_DONE;    
             }   
             else if (lstrcmpi(CurWord,"EQ")==0) {
                RtfInEquation=TRUE;
                InstUsed=TRUE;       // this instruct used, do not use the result control word now
             }
             else if (lstrcmpi(CurWord,"INCLUDEPICTURE")==0) {
                BYTE str[MAX_WIDTH+1];
                LPBYTE ptr;
                BOOL InQuotes;
                
                //rtf->flags|=RFLAG_IGNORE_SLASH;  // don't tread backslash as control word or special character
                //rtf->flags1|=RFLAG1_IGNORE_SPACE;  // don't tread backslash as control word or special character
                //if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;   // get the picture file path name
                //ResetUintFlag(rtf->flags,RFLAG_IGNORE_SLASH);
                //ResetUintFlag(rtf->flags1,RFLAG1_IGNORE_SPACE);


                //InQuotes=(rtf->CurWord[0]==QUOTE);
                //StripSlashes(w,rtf->CurWord,CurWord);  // convert double slash to single slash in the string
                //XlateRtfHex(w,CurWord);
                //if (!InQuotes && NULL!=(ptr=strstr(CurWord," "))) (*ptr)=0;
                
                ReadRtfGroupText(w,rtf,str);
                InQuotes=(NULL!=(ptr=lstrchr(str,QUOTE)));
                ExtractQuotedText(str);  // if file name in quotes, extract the quoted etxt
                StripSlashes(w,str,CurWord);  // convert double slash to single slash in the string
                if (!InQuotes && NULL!=(ptr=strstr(CurWord," "))) (*ptr)=0;

                if (ReadRtfLinkedPicture(w,rtf,CurWord)>0) InstUsed=TRUE;  // read the linked picture
             }
             else if (lstrcmpi(CurWord,"SET")==0 && !InMergeField) {  // skip this instruction
                SkipInst=TRUE;
                InstUsed=TRUE;
             }
             else if (lstrcmpi(CurWord,"ASK")==0 && !InMergeField) {  // skip this instruction
                SkipInst=TRUE;
                InstUsed=TRUE;
             }
             else if (lstrcmpi(CurWord,"include")==0 && !InMergeField) {  // skip this instruction, use the result
                SkipInst=TRUE;
             }
             else if (lstrcmpi(CurWord,"shape")==0 && !InMergeField) {  // skip this instruction, use the result
                SkipInst=TRUE;
             }
             else if (RtfInEquation && lstrcmpi(CurWord,"X(")==0) {
                // insert the page number
                int CurLevel=rtf->GroupLevel;

                if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
                rtf->group[CurLevel].style|=CHAR_BOX;
                break;          // now field data will be read
             }
             else if (lstrcmpi(CurWord,"FORMTEXT")==0) {
                TextInputField=TRUE;
                continue;
             }
             else if (lstrcmpi(CurWord,"FORMCHECKBOX")==0
                   || lstrcmpi(CurWord,"FORMDROPDOWN")==0) {   // form input fields
                   continue;    // now look for the formfield destination group
             }
             else if (lstrcmpi(CurWord,"MERGEFIELD")==0) {
                   InMergeField=TRUE;
             } 
             else if (lstrcmpi(CurWord,"MERGEFIELD")!=0 && !InstUsed && !HtmlMode) {  // treat it as a field name/data pair
                UINT SaveStyle;
                if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
                for (i=InstGroupLevel;i<=rtf->GroupLevel;i++) rtf->group[i].FieldId=FIELD_NAME;

                // transmit the opening braces
                SaveStyle=rtf->group[rtf->GroupLevel].style;
                rtf->group[rtf->GroupLevel].style|=PROTECT;
                lstrcpy(rtf->OutBuf,"{");
                rtf->OutBufLen=lstrlen(rtf->OutBuf);
                if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
                rtf->group[rtf->GroupLevel].style=SaveStyle;
                for (i=ControlGroupLevel;i<=rtf->GroupLevel;i++) rtf->group[i].gflags|=GFLAG_IN_MERGE_FIELD;

                lstrcpy(rtf->OutBuf,rtf->CurWord);
                rtf->OutBufLen=lstrlen(rtf->CurWord);
                return 0;
             }
          }

          INST_DONE:

          // check if symbol encountered
          if (IsSymbol) {                 // if field was of symbol type
             int CurLevel=rtf->GroupLevel;
             StrTrim(FontName);
             StrTrim(FontSize);
             if (lstrlen(FontName)>0) {
                struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
                int CurFont;

                if (FontName[0]==quote) { // remove double quote around it
                   FontName[0]=' ';
                   FontName[lstrlen(FontName)-1]=' ';
                   StrTrim(FontName);
                }
                
                // get font charset
                for (CurFont=0;CurFont<pRtfGroup->MaxRtfFonts;CurFont++) {
                   if (pRtfGroup->font[CurFont].InUse) { 
                      if (lstrcmpi(pRtfGroup->font[CurFont].name,FontName)==0) break;
                      if (lstrcmpi(pRtfGroup->font[CurFont].name2,FontName)==0) {
                         lstrcpy(FontName,pRtfGroup->font[CurFont].name);  
                         break;
                      }
                   }
                } 
                lstrcpy(rtf->group[CurLevel].TypeFace,FontName);

                if (CurFont<pRtfGroup->MaxRtfFonts) rtf->group[CurLevel].CharSet=pRtfGroup->font[CurFont].CharSet;
                if (lstrcmpi(FontName,"Wingdings")==0) rtf->group[CurLevel].CharSet=2;
                if (lstrcmpi(FontName,"Monotype Sorts")==0) rtf->group[CurLevel].CharSet=2;
             }
             if (lstrlen(FontSize)>0) {
                int PointSize=atoi(FontSize);
                if (PointSize>0) rtf->group[CurLevel].PointSize2=PointSize*2;
             }
             // copy other character information from InstGroupLevel
             rtf->group[CurLevel].style=SymStyle;
             rtf->group[CurLevel].TextColor=SymTextColor;
             rtf->group[CurLevel].TextBkColor=SymTextBkColor;

             rtf->OutBuf[0]=symbol;
             rtf->OutBuf[1]=0;
             rtf->OutBufLen=1;
             if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send the symbol
          }

          if (RtfInEquation) break;  // now the field eqaution data will be read
          else               continue;
       }
       else if (lstrcmpi(rtf->CurWord,"fldrslt")==0 && rtf->IsControlWord) {
         if (!InstUsed) TextInputField=FALSE;
         if (TextInputField) {                    // text input field encountered
            int GroupLevel=rtf->GroupLevel;
            rtf->group[GroupLevel].gflags|=(GFLAG_INPUT_FIELD|GFLAG_NO_UNICODE);  // in input field result
            break;
         }
         else if ((InstUsed || !InstFound) && !EmbeddedField) SkipRtfGroup(w,rtf);  // skip the result group
         else break;                         // now field result will be read
       }
    }

    return 0;                 // successful
}

/*****************************************************************************
    ReadRtfLinkedPicture:
    Read the linked picture file.
******************************************************************************/
ReadRtfLinkedPicture(PTERWND w,struct StrRtf far *rtf, LPBYTE PictFile)
{
    int pict;
    struct StrRtfPict pic;
    struct StrRtfGroup far *group=rtf->group;
    int  CurGroup=rtf->GroupLevel;

    // check if the file exists
    ResolveLinkFileName(w,PictFile);
    if (!FileExists(PictFile)) return -1;

    // insert a linked picture
    pict=TerInsertPictureFile(hTerWnd,PictFile,FALSE,0,FALSE);

    if (pict<=0) return -1;

    TerFont[pict].style|=group[CurGroup].style;   // append the group styles
    if (group[CurGroup].FieldId) {
       TerFont[pict].FieldId=group[CurGroup].FieldId;
       if (rtf->FieldCode) {
           TerFont[pict].FieldCode=OurAlloc(lstrlen(rtf->FieldCode)+1);
           lstrcpy(TerFont[pict].FieldCode,rtf->FieldCode);
       } 
    } 

    // insert the picture into text
    FarMemSet(&pic,0,sizeof(pic));  // initialize
    pic.PictId=pict;

    // pass the picture to the application
    ImportRtfData(w,RTF_PICT,&(rtf->group[rtf->GroupLevel]),NULL,(LPBYTE)&pic);

    // apply the picture size
    if (group[CurGroup].LinkPictWidth>0 || group[CurGroup].LinkPictHeight>0)
       TerSetPictSize(hTerWnd,pict,TwipsToScrX(group[CurGroup].LinkPictWidth),TwipsToScrY(group[CurGroup].LinkPictHeight));

    return pict;
}

/*****************************************************************************
    ResolveLinkFileName:
    Resolve the link file name.
******************************************************************************/
ResolveLinkFileName(PTERWND w, LPBYTE PictFile)
{
    // check if the file exists
    BOOL FullPathSpecified=(lstrlen(PictFile)>1 && PictFile[1]==':') || (lstrlen(PictFile)>0 && PictFile[0]=='\\');
    if (!FullPathSpecified) {  
       BYTE path[300]="";
       if (strlen(LinkPictDir)>0) {
           lstrcpy(path,LinkPictDir);
           lstrcat(path,"\\");
           lstrcat(path,PictFile);
           lstrcpy(PictFile,path);
       }
       else { 
          BYTE LastChar;
          int len;
          if (strlen(UserDir)>0) lstrcpy(path,UserDir);
          else if ((strlen(DocName)>0 && DocName[0]=='\\') || (strlen(DocName)>1 && DocName[1]==':')) {  // the file name specifies full path
             int i,len=strlen(DocName);
             lstrcpy(path,DocName);
             for (i=(len-1);i>0;i--) if (path[i]=='\\') break;
             path[i]=0;
          } 
          if (lstrlen(path)>0) {
             len=strlen(path);
             LastChar=path[len-1];
             if (LastChar!='\\') lstrcat(path,"\\");
          
             lstrcat(path,PictFile);
             lstrcpy(PictFile,path);
          }
       }
    }

    return TRUE;
}

/*****************************************************************************
    ReadRtfColorTable:
    Read the RTF color table.  This function return 0 when sucessful, otherwise
    it returns an error code (RTF_FILE_INCOMPLETE or RTF_SYNTAX_ERROR)
******************************************************************************/
ReadRtfColorTable(PTERWND w,struct StrRtf far *rtf)
{
    struct StrRtfColor far *color;
    int    i,CurColor=0,ControlGroupLevel=0;
    BYTE   red,green,blue;
    struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);

    color=pRtfGroup->color;
    for (i=0;i<MaxRtfColors;i++) {
       color->color=TerFont[DEFAULT_CFMT].TextColor;   // initialize colors
       if (color->color==0) color->color=CLR_AUTO;
    }

    ControlGroupLevel=rtf->GroupLevel;
    rtf->IgnoreCrLfInControlWord=TRUE;     // ignore cr/lf in control word

    while (TRUE) {
       // get color number
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // color group ended
          else                                   continue; // look for next color
       }

       if (rtf->IsControlWord) {                 // extract color component
          red=GetRValue(color[CurColor].color);  // break existing color components
          green=GetGValue(color[CurColor].color);
          blue=GetBValue(color[CurColor].color);

          if      (lstrcmpi(rtf->CurWord,"red")==0)   red=(BYTE)(rtf->IntParam);
          else if (lstrcmpi(rtf->CurWord,"green")==0) green=(BYTE)(rtf->IntParam);
          else if (lstrcmpi(rtf->CurWord,"blue")==0)  blue=(BYTE)(rtf->IntParam);
          color[CurColor].color=RGB(red,green,blue);   // extracted color
       }
       else {
          if (lstrcmpi(rtf->CurWord,";")==0) {
             CurColor++;                   // next color begins
             if (CurColor>=MaxRtfColors) {
                int add=50;
                if (NULL==(color=pRtfGroup->color=(struct StrRtfColor far *)OurRealloc(color,sizeof(struct StrRtfColor)*(MaxRtfColors+add)))) return FALSE;
                pRtfGroup->color=color;
                MaxRtfColors+=add;
             }
          }
       }
    }

    rtf->IgnoreCrLfInControlWord=FALSE;       // reset this flag

    return 0;                 // successful
}

/*****************************************************************************
    SetRtfParaBorder:
    Set the pargraph border flags
******************************************************************************/
BOOL SetRtfParaBorders(PTERWND w,struct StrRtfGroup far *group, int BorderType)
{
    if (BorderType==BORDER_PARA_ALL)   group->flags|=PARA_BOX;
    if (BorderType==BORDER_PARA_LEFT)  group->flags|=PARA_BOX_LEFT;
    if (BorderType==BORDER_PARA_RIGHT) group->flags|=PARA_BOX_RIGHT;
    if (BorderType==BORDER_PARA_TOP)   group->flags|=PARA_BOX_TOP;
    if (BorderType==BORDER_PARA_BOT)   group->flags|=PARA_BOX_BOT;
    if (BorderType==BORDER_PARA_BETWEEN) group->flags|=PARA_BOX_BETWEEN;

    return TRUE;
}

/*****************************************************************************
    SetRtfSectBorder:
    Set the section border parameters
******************************************************************************/
BOOL SetRtfSectBorders(PTERWND w,struct StrRtfGroup far *group, int BorderType)
{
    int index=-1;
    struct StrRtf far *rtf=group->rtf;

    if (BorderType==BORDER_SECT_LEFT)  {
       rtf->sect.border|=BORDER_LEFT;
       index=BORDER_INDEX_LEFT;
    }
    else if (BorderType==BORDER_SECT_RIGHT) {
       rtf->sect.border|=BORDER_RIGHT;
       index=BORDER_INDEX_RIGHT;
    }
    else if (BorderType==BORDER_SECT_TOP) {
       rtf->sect.border|=BORDER_TOP;
       index=BORDER_INDEX_TOP;
    }
    else if (BorderType==BORDER_SECT_BOT) {
       rtf->sect.border|=BORDER_BOT;
       index=BORDER_INDEX_BOT;
    }

    if (index>=0) {
       rtf->sect.BorderWidth[index]=group->BorderWidth[BorderType];
       rtf->sect.BorderSpace[index]=group->BorderSpace[BorderType];
    } 

    return TRUE;
}

/*****************************************************************************
    GetRtfWord:
    Reads the next word from the input file or the input buffer.
******************************************************************************/
BOOL GetRtfWord(PTERWND w,struct StrRtf far *rtf)
{
    BYTE CurChar=0,PrevChar,TempByte;
    BYTE line[MAX_WIDTH+1];
    int  len,i,j,k,temp,GroupLevel,NewFieldId;
    BOOL SpecialChar,NumParamFound,NumFound;
    UINT NewRtfInHdrFtr;
    BOOL IgnoreSlash=rtf->flags&RFLAG_IGNORE_SLASH;


    rtf->IgnoreText=FALSE;

    NEXT_WORD:
    if (rtf->eof) return FALSE; // end of file

    rtf->GroupBegin=rtf->GroupEnd=rtf->SubEntry=FALSE;
    rtf->IsControlWord=FALSE;
    rtf->CurWord[0]=0;
    rtf->WordLen=0;

    // Extract a word delimited by {, }, \ or a space.  If space
    // is the delimiter, it is included in the extracted word.  All other
    // delimiters are pushed back on the character stream.

    len=0;
    while (GetRtfChar(w,rtf)) {
       PrevChar=CurChar;
       CurChar=rtf->CurChar;

       // check for delimiters before accepting the character
       if (len>0 && (line[len-1]!='\\' || IgnoreSlash)) {
          if (  CurChar=='{'
             || CurChar=='}'
             || (CurChar=='\\' && !IgnoreSlash) ){
             PushRtfChar(w,rtf);
             break;
          }
       }

       line[len]=CurChar;         // build string
       len++;

       // check for these delimiters after accepting the characters
       if (len==1) {              // first character
          if (CurChar=='{' || CurChar=='}') break;
          if (CurChar==')' && RtfInEquation) len--;    // discard the equation closing paranthesis
       }
       else if (len==2 && line[len-2]=='\\' && line[len-1]=='\\' && !IgnoreSlash) break;
       else if ((CurChar==' ' && !(rtf->flags1&RFLAG1_IGNORE_SPACE)) || (RtfInEquation && CurChar=='(')) break;
       else if (CurChar==')' && RtfInEquation) len--;  // discard the equation closing paranthesis
       else if (rtf->mac && len>1 && CurChar==10 && PrevChar!=13) break;

       if (len==MAX_WIDTH) break; // break this long word
    }


    if (rtf->eof) return FALSE;   // end of stream
    line[len]=0;

    // process group begin and end
    if (len==1 && line[0]=='{') { // new group
       if (!SendRtfText(w,rtf)) return FALSE;  // send the previous text if any

       if (rtf->GroupLevel>=MAX_RTF_GROUPS) return PrintError(w,MSG_OUT_OF_GROUP,"GetRtfWord");

       rtf->GroupBegin=TRUE;
       rtf->GroupLevel++;         // increase the group level

       // copy properties to the next group and initialize
       if (rtf->GroupLevel>0) FarMove(&(rtf->group[rtf->GroupLevel-1]),&(rtf->group[rtf->GroupLevel]),sizeof(struct StrRtfGroup));
       rtf->group[rtf->GroupLevel].ControlCount=0;
       ResetLongFlag(rtf->group[rtf->GroupLevel].gflags,GFLAG_NEW_FRAME);

       // start a new style item when the style sheet is active
       if (rtf->group[rtf->GroupLevel].gflags&GFLAG_IN_STYLESHEET) {
          rtf->group[rtf->GroupLevel].gflags|=GFLAG_IN_STYLE_ITEM;
          InitGroupForStyle(w,rtf);
       }

       return TRUE;
    }

    if (len==1 && line[0]=='}') {   // end group
       GroupLevel=rtf->GroupLevel;

       // do we need to append a para marker
       if (rtf->group[GroupLevel].gflags&GFLAG_DO_TEXT_READ
          && (GroupLevel==0 || !(rtf->group[GroupLevel-1].gflags&GFLAG_DO_TEXT_READ))
          //&& rtf->OutBufLen>0 && rtf->OutBuf[rtf->OutBufLen-1]!=ParaChar
          && !(rtf->flags1&RFLAG1_PARA_ENDED)) {
          rtf->OutBuf[rtf->OutBufLen]=ParaChar;
          rtf->OutBuf[rtf->OutBufLen+1]=0;
          rtf->OutBufLen++;
       }

       if (!SendRtfText(w,rtf)) return FALSE;     // send the previous text if any

       // realize any formfield control
       if (rtf->group[GroupLevel].gflags&GFLAG_INPUT_FIELD && rtf->pict>=0
          && (GroupLevel==0 || !(rtf->group[GroupLevel-1].gflags&GFLAG_INPUT_FIELD))) {
           if (rtf->group[GroupLevel].FieldId!=FIELD_TEXTBOX) {
              RealizeControl(w,rtf->pict);
              XlateSizeForPrt(w,rtf->pict);         // convert to` printer dimensions
           }
       }


       // check if a field name is ending
       NewFieldId=rtf->group[rtf->GroupLevel-1].FieldId;
       if (rtf->group[rtf->GroupLevel].FieldId==FIELD_NAME && NewFieldId!=FIELD_NAME) {
          // transmit the opening braces
          UINT SaveStyle=rtf->group[rtf->GroupLevel].style;
          rtf->group[rtf->GroupLevel].style|=PROTECT;
          //ResetUintFlag(rtf->group[rtf->GroupLevel].style,HIDDEN);
          lstrcpy(rtf->OutBuf,"}");
          rtf->OutBufLen=lstrlen(rtf->OutBuf);
          
          rtf->group[rtf->GroupLevel].gflags2|=GFLAG2_NO_MBCS;  // do not teat this braces as MBCS
          if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
          ResetUintFlag(rtf->group[rtf->GroupLevel].gflags2,GFLAG2_NO_MBCS);

          rtf->group[rtf->GroupLevel].style=SaveStyle;
       }

       // check if a page header/footer group is ending
       NewRtfInHdrFtr=rtf->group[rtf->GroupLevel-1].flags&PAGE_HDR_FTR;
       if (RtfInHdrFtr&PAGE_HDR_FTR && NewRtfInHdrFtr==0) {
          if (!(rtf->flags&RFLAG_HDRFTR_TEXT_FOUND)) {  // insert a paramarker
              rtf->OutBuf[0]=' ';          // add a space character so that the header/footer is retained
              rtf->OutBuf[1]=ParaChar;
              rtf->OutBuf[2]=0;
              rtf->OutBufLen=2;
              if (!SendRtfText(w,rtf)) return FALSE;  // send the para break character
          }
          SetRtfParaDefault(w,rtf,rtf->group);
          rtf->OutBuf[0]=rtf->HdrFtrChar;
          rtf->OutBuf[1]=0;
          rtf->OutBufLen=1;
          if (!SendRtfText(w,rtf)) return FALSE;  // send the section break character
          RtfInHdrFtr=0;                          // reset header/footer flag
       }

       // check if a subtable group is ending
       if (rtf->GroupLevel>0 && rtf->flags1&RFLAG1_SSSUBTABLE_USED 
          && rtf->group[rtf->GroupLevel].level!=rtf->group[rtf->GroupLevel-1].level) {
          int GroupLevel=rtf->GroupLevel;   
          SetRtfTblLevel(w,rtf,GroupLevel-1,rtf->group[GroupLevel-1].level,rtf->group[GroupLevel].level);
       } 

       rtf->GroupEnd=TRUE;

       // update a stylesheet item if active
       if (  rtf->group[rtf->GroupLevel].gflags&GFLAG_IN_STYLE_ITEM && rtf->GroupLevel>0
          && !(rtf->group[rtf->GroupLevel-1].gflags&GFLAG_IN_STYLE_ITEM)) {
          UpdateRtfStylesheet(w,rtf);
       }

       ExitRtfGroup(w,rtf,rtf->GroupLevel);    // free table if exiting an \rtf group

       rtf->GroupLevel--;

       // update the RtfInEquation flag
       if (!(rtf->group[rtf->GroupLevel].style&CHAR_BOX)) RtfInEquation=FALSE;

       // Stuff the ending part of an autonumbered footnote in progress
       if (rtf->group[rtf->GroupLevel].style&FNOTEREST && rtf->group[rtf->GroupLevel+1].style&FNOTETEXT 
           && lstrlen(FootnoteRest)>0) {
          rtf->OutBuf[rtf->OutBufLen]=0;
          lstrcpy(rtf->OutBuf,FootnoteRest);
          rtf->OutBufLen=lstrlen(rtf->OutBuf);
          if (!SendRtfText(w,rtf)) return FALSE;  // send the para break character
          
          FootnoteRest[0]=0;                  // empty the footnote marker string
          ResetUintFlag(rtf->group[rtf->GroupLevel].style,FNOTEREST);
       }

       // inform importer of the property changes
       //ImportRtfData(w,RTF_PROPERTY,&(rtf->group[rtf->GroupLevel+1]),&(rtf->group[rtf->GroupLevel]),NULL);
       if      (rtf->GroupLevel<0)  return FALSE;   // end of rtf data
       else if (rtf->GroupLevel==0 && (!(TerFlags3&TFLAG3_MULTIPLE_RTF_GROUPS) || (TerOpFlags2&TOFLAG2_FROM_CB)))  return FALSE;   // end of rtf data
       else                         return TRUE;
    }

    // Translate special characters
    SpecialChar=FALSE;
    j=0;
    for (i=0;i<len;i++) {
       PrevChar=CurChar;
       CurChar=line[i];

       if (SpecialChar) {
          switch (CurChar) {
            case '\\':
            case '{':
            case '}':
              rtf->CurWord[j]=CurChar;
              j++;
              break;

            case '~':       // non breaking space, treat as regular space
              rtf->CurWord[j]=NBSPACE_CHAR;
              j++;
              break;

            case '*':       // ignore text if control word not understood
              rtf->IgnoreText=TRUE;
              break;

            case ':':       // subentry in an index entry
              if (rtf->group[rtf->GroupLevel].FieldId==FIELD_XE) {
                rtf->CurWord[j]=CurChar;
                j++;
              } 
              rtf->SubEntry=TRUE;
              break;

            case '\'':      // hexadecimal value follows
              if (i+2<len) {
                 TempByte=(char)toupper(line[i+1]);      // convert to decimal
                 if (TempByte>='A') temp=10+TempByte-'A';
                 else               temp=TempByte-'0';
                 temp=temp<<4;
                 rtf->CurWord[j]=(char)temp;
                 TempByte=(char)toupper(line[i+2]);      // second byte
                 if (TempByte>='A') temp=10+TempByte-'A';
                 else               temp=TempByte-'0';
                 rtf->CurWord[j]+=(char)temp;
                 i=i+2;
                 
                 if (rtf->CurWord[j]==0xa && !(rtf->group[rtf->GroupLevel].gflags&(GFLAG_IN_LEVELTEXT|GFLAG_IN_LEVELNUM))) {
                    rtf->CurWord[j]='l';j++;
                    rtf->CurWord[j]='i';j++;
                    rtf->CurWord[j]='n';j++;
                    rtf->CurWord[j]='e';j++;
                    rtf->IsControlWord=TRUE;
                    NumParamFound=FALSE;
                    break;
                 }
                 else { 
                    if (rtf->CurWord[j]>LAST_CTRL_CHAR || rtf->CurWord[j]==0xa 
                        || rtf->CurWord[j]==0xd || rtf->CurWord[j]==TAB || rtf->CurWord[j]==PAGE_CHAR
                        || True(rtf->group[rtf->GroupLevel].gflags&(GFLAG_IN_LEVELTEXT|GFLAG_IN_LEVELNUM))) j++;
                 }
              }
              break;

            case '|':       // special characters to be ignored, Formula character
              break;

            case '-':       // optional hyphen - ignore for now
              rtf->CurWord[j]=HYPH_CHAR;
              j++;
              break;

            case '_':       // non-breaking hyphen
              rtf->CurWord[j]=NBDASH_CHAR;
              j++;
              break;

            default:        // control word found
              if (rtf->IsControlWord) { // a control word already active
                 MessageBox(NULL,line,MsgString[MSG_BAD_RTF],MB_OK);
                 rtf->eof=TRUE;
                 return FALSE;
              }
              rtf->IsControlWord=TRUE;
              NumParamFound=FALSE;  // will be set to TRUE when a numeric parameter is found
              if (CurChar==10 || CurChar==13) {  // treat as paragraph
                 rtf->CurWord[j]='p';j++;
                 rtf->CurWord[j]='a';j++;
                 rtf->CurWord[j]='r';j++;
                 break;
              }
              else {
                rtf->CurWord[j]=CurChar;
                j++;
              }
          }
          SpecialChar=FALSE;//Turnoff special character flag
       }
       else {               // if not under special character
          if (CurChar=='\\' && !IgnoreSlash) SpecialChar=TRUE;  // special character may follow
          else {            // ordinary character
             // check for the end of a control word
             if (rtf->IsControlWord) {
                // Handle cr/lf within control word
                if (CurChar==13) continue; // wait for CR
                if (CurChar==10) {
                   if (!rtf->IgnoreCrLfInControlWord/* && NumParamFound*/) {
                      for (k=len-1;k>i;k--) {
                         rtf->CurChar=line[k];
                         PushRtfChar(w,rtf);
                      }
                      break;
                   }
                   else continue;
                }
                
                // terminate the control character if current character is hyphen (minus) but the next character is not numeric
                if (CurChar=='-' && ((i+1)==len || line[i+1]<'0' || line[i+1]>'9') && line[i+1]!=' ') {     // allow for just - as a style off character, such as \b- = turn-off bold
                   for (k=len-1;k>i;k--) {
                      rtf->CurChar=line[k];
                      PushRtfChar(w,rtf);
                   }
                   break;
                } 

                // check if current character is a number
                if (CurChar=='.' || CurChar=='-' || (CurChar>='0' && CurChar<='9'))
                      NumFound=TRUE;
                else  NumFound=FALSE;

                // terminate this control word, if the next one starts
                if (!NumFound && !isalpha(CurChar) && CurChar!=' ') {// control word ends without a numeric parameter
                   // Push remaining characters including the current character
                   for (k=len-1;k>=i;k--) {
                      rtf->CurChar=line[k];
                      PushRtfChar(w,rtf);
                   }
                   break;
                }
                else if (NumParamFound && !NumFound) {
                   // Push back every character after the last numeric character
                   for (k=len-1;k>=i;k--) {
                      rtf->CurChar=line[k];
                      PushRtfChar(w,rtf);
                   }
                   if (CurChar==' ') GetRtfChar(w,rtf);  // space is considered part of the control character
                   //else {                   // next control word starts
                   //  rtf->CurChar='\\';     // begin next control word
                   //  PushRtfChar(w,rtf);
                   //}
                   break;                   // break outof main loop
                }
                // check if numeric parameter for a control word is found
                if (NumFound) NumParamFound=TRUE;
             }

             // incorporate current character into current word
             if (CurChar != 10 &&  CurChar !=13) {
               rtf->CurWord[j]=CurChar;  // ordinary character
               j++;
             }
             else if (rtf->mac && CurChar==10 && PrevChar!=13 && j>0) {
               rtf->CurWord[j]=ParaChar;
               j++;
             } 
          }
       }
    }
    rtf->WordLen=len=j;          //current word length
    rtf->CurWord[len]=0;

    if (!(rtf->IsControlWord)) {
        if (rtf->WordLen>0) return TRUE;     // an oridnary word extracted
        else                goto NEXT_WORD;  // suppress zero length words
    }

    // Separate numeric parameter for the control word
    StrTrim(rtf->CurWord);
    for (i=0;i<len;i++) {
       CurChar=rtf->CurWord[i];
       if (CurChar=='.' || CurChar=='-' || (CurChar>='0' && CurChar<='9')) break;
    }

    if (i==0) i=1;    // minimum length of a control word

    for (j=i;j<len;j++) rtf->param[j-i]=rtf->CurWord[j];  // extrac param
    rtf->param[j-i]=0;
    rtf->CurWord[i]=0;
    rtf->WordLen=i;

    // convert to double numeric
    lstrcpy(TempString,rtf->param);              // copy to data segment
    if (lstrlen(TempString)>0) {
       if (lstrlen(TempString)==1 && TempString[0]=='-') {
          rtf->IntParam=0;                         // a false value
          rtf->DoubleParam=0;
       }
       else {
          rtf->IntParam=atol(TempString);           // number in integer format
          rtf->DoubleParam=strtod(TempString,NULL); // number in double format
       }
    }
    else {
       rtf->IntParam=1;                         // a TRUE value
       rtf->DoubleParam=1;
    }

    // increment number of control words in the group
    (rtf->group[rtf->GroupLevel].ControlCount)++;

    return TRUE;
}

/*****************************************************************************
    GetRtfChar:
    Reads the next characters from the input file or the input buffer.
    When the InSteam is NULL, the characters are read from the buffer.
    Returns FALSE when no more characters available.
******************************************************************************/
BOOL GetRtfChar(PTERWND w,struct StrRtf far *rtf)
{
    static long BytesRead=0;
    static long PrevBytesRead=0;

    // get the character from the character stack if available
    if (rtf->StackLen>0) {
       rtf->StackLen--;
       rtf->CurChar=rtf->stack[rtf->StackLen];
       rtf->FilePos++;       // advance file position
       return TRUE;
    }

    // read from the file
    rtf->eof=FALSE;
    if (rtf->hFile!=INVALID_HANDLE_VALUE) {
       if (rtf->TextIndex<rtf->TextLen) {  // read from the temporary buffer
          rtf->CurChar=rtf->text[rtf->TextIndex];
          (rtf->TextIndex)++;
          rtf->FilePos++;    // advance file position
          return TRUE;
       }

       // read more characters from the file
       ReadFile(rtf->hFile,rtf->text,MAX_WIDTH,&((DWORD)rtf->TextLen),NULL);

       BytesRead+=MAX_WIDTH;
       if (BytesRead>=(PrevBytesRead+100000)) {
          PrevBytesRead=BytesRead;
       } 



       if (rtf->TextLen==0) {
          rtf->eof=TRUE;
          return FALSE;      // end of file
       }
       rtf->TextIndex=0;

       rtf->CurChar=rtf->text[rtf->TextIndex];
       (rtf->TextIndex)++;
       rtf->FilePos++;      // advance file position
       return TRUE;
    }
    else {              // read from the buffer
       if (rtf->BufLen>=0 && rtf->BufIndex>=rtf->BufLen) {
          rtf->eof=TRUE;
          return FALSE; // end of buffer
       }
       rtf->CurChar=rtf->buf[rtf->BufIndex];
       (rtf->BufIndex)++;
       rtf->FilePos++;  // advance file position
       return TRUE;
    }
}

/*****************************************************************************
    GetRtfHexChar:
    Read 2 consequtive characters in hex form to form on a numeric value.
    The character is returned in the 'CurChar' variable of the rtf structure.
    The function returns FALSE on end of file or when a non-hex character is
    found.
******************************************************************************/
BOOL GetRtfHexChar(PTERWND w,struct StrRtf far *rtf)
{
    BYTE CurChar;

    // get high order hex character
    while (TRUE) {
       if (!GetRtfChar(w,rtf)) return FALSE;
       if (rtf->CurChar!=0xa && rtf->CurChar!=0xd && rtf->CurChar!=' ') break;  // skip cr/lf
    }

    if      (rtf->CurChar >='0' && rtf->CurChar <='9') rtf->CurChar=(char)(rtf->CurChar-'0');
    else if (rtf->CurChar >='A' && rtf->CurChar <='F') rtf->CurChar=(char)(rtf->CurChar-'A'+10);
    else if (rtf->CurChar >='a' && rtf->CurChar <='f') rtf->CurChar=(char)(rtf->CurChar-'a'+10);
    else return FALSE;

    CurChar=(BYTE)rtf->CurChar;
    CurChar=(BYTE)(CurChar<<4);            // high order hex value found

    // get low order hex character
    while (TRUE) {
       if (!GetRtfChar(w,rtf)) return FALSE;
       if (rtf->CurChar!=0xa && rtf->CurChar!=0xd && rtf->CurChar!=' ') break;  // skip cr/lf
    }
    if      (rtf->CurChar >='0' && rtf->CurChar <='9') rtf->CurChar=(char)(rtf->CurChar-'0');
    else if (rtf->CurChar >='A' && rtf->CurChar <='F') rtf->CurChar=(char)(rtf->CurChar-'A'+10);
    else if (rtf->CurChar >='a' && rtf->CurChar <='f') rtf->CurChar=(char)(rtf->CurChar-'a'+10);
    else return FALSE;

    CurChar=(BYTE)(CurChar+rtf->CurChar);

    rtf->CurChar=(char)CurChar;

    return TRUE;
}

/*****************************************************************************
    PushRtfChar:
    Push the lastly read character onto stack.
******************************************************************************/
BOOL PushRtfChar(PTERWND w,struct StrRtf far *rtf)
{
    if (rtf->StackLen>=MAX_WIDTH) return PrintError(w,MSG_OUT_OF_CHAR_STACK,"PushRtfChar");

    rtf->stack[rtf->StackLen]=rtf->CurChar;
    (rtf->StackLen)++;
    rtf->FilePos--;  // decrement file position
    return TRUE;
}

/*****************************************************************************
    XlateRtfHex:
    Reads the next word from the input file or the input buffer.
******************************************************************************/
LPBYTE XlateRtfHex(PTERWND w,LPBYTE line)
{
    int i,j,len;
    int temp;
    BYTE TempChar,CurChar,PrevChar=0,slash='\\';

    len=lstrlen(line);

    j=0;
    for (i=0;i<len;i++) {
       CurChar=line[i];

       if (PrevChar==slash) {        // after backslash
         if (CurChar=='\'' && (i+2)<len) {
            TempChar=(char)toupper(line[i+1]);      // convert to decimal
            if (TempChar>='A') temp=10+TempChar-'A';
            else               temp=TempChar-'0';
            temp=temp<<4;
            line[j]=(char)temp;
            TempChar=(char)toupper(line[i+2]);      // second byte
            if (TempChar>='A') temp=10+TempChar-'A';
            else               temp=TempChar-'0';
            line[j]+=(char)temp;

            i=i+2;
            j++;
         }
         else {
            line[j]=slash;     // put the previous unprocessed slash
            j++;
            if (CurChar!=slash) {
               line[j]=CurChar;
               j++;
            }
         }
       }
       else if (CurChar!=slash) {
         line[j]=CurChar;
         j++;
       }
       PrevChar=CurChar;
    }

    if (PrevChar==slash) {
       line[j]=slash;     // put the previous unprocessed slash
       j++;
    }

    line[j]=0;

    return line;
}

/*****************************************************************************
    ReadRtfObject:
    Read the RTF object data. This function return 0 when sucessful, otherwise
    it returns an error code (RTF_FILE_INCOMPLETE or RTF_SYNTAX_ERROR)
******************************************************************************/
ReadRtfObject(PTERWND w,struct StrRtf far *rtf)
{
    int    ControlGroupLevel=0;
    BOOL   ObjectDataRead=FALSE,SkipObject=FALSE,ResultGroup=FALSE;
    BYTE   huge *pMem=NULL;
    int    ObjectType=OBJ_NONE;
    int    ObjectAspect=OASPECT_NONE;
    int    ObjectUpdate=FALSE;
    DWORD  ObjectSize;
    int    ReturnCode=0,result;

    ControlGroupLevel=rtf->GroupLevel;
    CurObject=-1;             // initialized by the ImportRtfPicture function when inserting a picture

    while (TRUE) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if      (rtf->GroupLevel<ControlGroupLevel) break;    // object group ended
          continue;                                             // look for next group end
       }

       if (SkipObject) continue;

       // process the control word
       if (rtf->IsControlWord) {

          if      (lstrcmpi(rtf->CurWord,"objemb")==0) ObjectType=OBJ_EMBED;      // embedded object
          else if (lstrcmpi(rtf->CurWord,"objocx")==0) ObjectType=OBJ_OCX;        // embedded object
          else if (lstrcmpi(rtf->CurWord,"objautlink")==0) ObjectType=OBJ_LINK;   // linked object
          else if (lstrcmpi(rtf->CurWord,"objupdate")==0) ObjectUpdate=TRUE;      // display content
          else if (lstrcmpi(rtf->CurWord,"sscontent")==0) ObjectAspect=OASPECT_CONTENT;    // display content
          else if (lstrcmpi(rtf->CurWord,"ssicon")==0) ObjectAspect=OASPECT_ICON;    // display icon
          else if (lstrcmpi(rtf->CurWord,"result")==0)     ResultGroup=TRUE;   // result follows
          else if ( lstrcmpi(rtf->CurWord,"objclass")==0
                 || lstrcmpi(rtf->CurWord,"objname")==0  ){
             if (0!=(result=SkipRtfGroup(w,rtf))) return result;
          }

          else if (lstrcmpi(rtf->CurWord,"pict")==0) {// read picture result
             if (ResultGroup) {                       // picture within the result group
                result=ReadRtfPicture(w,rtf);
                if (result) ReturnCode=result;        // code to be returned
                continue;
             }
             else {
                SkipObject=TRUE;
                continue;
             }
          }
          else if (lstrcmpi(rtf->CurWord,"objdata")==0) {// picture data in binary
              if (ObjectDataRead) continue;              // object data already read
              if (NULL==(pMem=ReadRtfObjBytes(w,rtf,(LPLONG)&ObjectSize))) return RTF_SYNTAX_ERROR;
              if (ObjectSize>0) ObjectDataRead=TRUE;
          }
          else if (ResultGroup) {         // controls in the result group
              int ReturnCode=ProcessRtfControl(w,rtf);
              if (ReturnCode!=0) return ReturnCode;
          }
       }
       else if (ResultGroup) {     // data in the result group
          // send previous text if buffer full
          if (rtf->OutBufLen+rtf->WordLen>RTF_LINE_LEN) if (!SendRtfText(w,rtf)) return RTF_SYNTAX_ERROR;
          lstrcat(rtf->OutBuf,rtf->CurWord);
          rtf->OutBufLen+=rtf->WordLen;
       }
    }

    if (  ObjectDataRead && CurObject>0
       && (ObjectType==OBJ_EMBED || ObjectType==OBJ_LINK || ObjectType==OBJ_OCX)) {
       MakeRtfObject(w,rtf,pMem,ObjectType,ObjectSize,ObjectAspect,ObjectUpdate);
       return ReturnCode;                     // 0=successful
    }
    else {
       if (pMem) GlbFree(pMem);
       return 0;
    }
}

/*****************************************************************************
    MakeRtfObject:
    Build an OLE object.  This function returns TRUE when successful.
******************************************************************************/
MakeRtfObject(PTERWND w,struct StrRtf far *rtf,BYTE huge *pMem, int ObjectType,long ObjectSize, int ObjectAspect, int ObjectUpdate)
{
    struct StrRtfObject obj;

    // Create an OLE object
    obj.ObjectType=ObjectType;
    obj.ObjectSize=ObjectSize;
    obj.ObjectAspect=ObjectAspect;
    obj.ObjectUpdate=ObjectUpdate;
    obj.pict=CurObject;

    #if defined(WIN32)
       obj.hObject=GlobalHandle(pMem);      // convert pointer to handle
    #else
       {
       DWORD temp=GlobalHandle((UINT)(((DWORD)pMem)>>16));
       obj.hObject=(HGLOBAL)LOWORD(temp);
       }
    #endif
    GlobalUnlock(obj.hObject);

    // pass the object data to the application
    ImportRtfData(w,RTF_OBJECT,&(rtf->group[rtf->GroupLevel]),NULL,(LPBYTE)&obj);

    return 0;
}

/*****************************************************************************
    ReadRtfObjBytes:
    Read the object bytes (hex format)
******************************************************************************/
LPBYTE ReadRtfObjBytes(PTERWND w,struct StrRtf far *rtf,LPLONG size)
{
    BYTE   huge *pMem;
    long   ObjectSize,BufSize,IncSize=0x4000;

    ObjectSize=0;            // current size of the object
    BufSize=IncSize;         // allocate the initial buffer
    if (NULL==(pMem=GlbAlloc(BufSize))) return NULL;;

    while(TRUE) {
        if (!GetRtfHexChar(w,rtf)) {
            if (rtf->CurChar=='}') {    // end of hex data
               PushRtfChar(w,rtf);
               break;
            }
            else if (rtf->CurChar=='{') {// ignore intervening groups
               PushRtfChar(w,rtf);       // push back group beginner
               if (!GetRtfWord(w,rtf)) return NULL;  // read back the group beginner
               if (0!=SkipRtfGroup(w,rtf)) return NULL;
               continue;
            }
            else return NULL; // read in hex format
        }
        if (ObjectSize>=BufSize) {      // expand the
           BufSize+=IncSize;
           if (NULL==(pMem=GlbRealloc(pMem,BufSize))) return NULL;
        }
        pMem[ObjectSize]=rtf->CurChar;
        ObjectSize++;
    }

    if (size) (*size)=ObjectSize;
    return (LPBYTE)pMem;
}

/*****************************************************************************
    ReadRtfShapeProp:
    Read the current shape property
******************************************************************************/
ReadRtfShapeProp(PTERWND w,struct StrRtf far *rtf, LPBYTE prop)
{
    int    ControlGroupLevel=0,result;

    ControlGroupLevel=rtf->GroupLevel;

    while (TRUE) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // font group ended
          else                                   continue; // look for next font
       }

       // process the control word
       if (rtf->IsControlWord) {
          if (lstrcmpi(rtf->CurWord,prop)==0) {
             result=ReadRtfPicture(w,rtf);
             if (result) return result;       // error condition
          }
       }
    }

    return 0;
}

/*****************************************************************************
    ReadRtfShapeText:
    Read the shptxt group
******************************************************************************/
ReadRtfShapeText(PTERWND w,struct StrRtf far *rtf)
{
    int    ControlGroupLevel=0,ReturnCode;

    ControlGroupLevel=rtf->GroupLevel;

    rtf->group[ControlGroupLevel].gflags|=GFLAG_USE_SHP_INFO;  // use shp info to construct para frames

    while (TRUE) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupEnd && rtf->GroupLevel<ControlGroupLevel) break;    // shptxt group ended

       if (rtf->IsControlWord) {
          ReturnCode=ProcessRtfControl(w,rtf);
          if (ReturnCode!=0) return ReturnCode;
       }
       else if (rtf->WordLen>0) CopyToOutBuf(w,rtf);                // text encountered
    }
    
    if (rtf->OutBufLen>0) SendRtfText(w,rtf);

    return 0;
}

/*****************************************************************************
    ExtractRtfPict:
    Extract only the picture from this group
******************************************************************************/
ExtractRtfPict(PTERWND w,struct StrRtf far *rtf)
{
    int    ControlGroupLevel=0,result;

    ControlGroupLevel=rtf->GroupLevel;

    while (TRUE) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // font group ended
          else                                   continue; // look for next font
       }

       // process the control word
       if (rtf->IsControlWord) {
          if (lstrcmpi(rtf->CurWord,"pict")==0) {
             result=ReadRtfPicture(w,rtf);
             if (result) return result;       // error condition
          }
       }
    }

    return 0;
}

/*****************************************************************************
    ReadRtfShapeParam:
    Read the shape name or shape value parameter as a string
******************************************************************************/
ReadRtfShapeParam(PTERWND w,struct StrRtf far *rtf,LPBYTE out)
{
    int    len,WordLen,ControlGroupLevel=0;
    BOOL  append=TRUE;

    out[0]=0;
    len=0;

    ControlGroupLevel=rtf->GroupLevel;

    while (TRUE) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // font group ended
          else                                   continue; // look for next font
       }

       if (append) {
          WordLen=lstrlen(rtf->CurWord);
          if ((len+WordLen)>=MAX_WIDTH) append=FALSE;
          if (append) {
             lstrcat(out,rtf->CurWord);
             len+=WordLen;
          }
       }
    }

    return 0;
}

