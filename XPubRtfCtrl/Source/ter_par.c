/*==============================================================================
   TER_PAR.C
   TER paragraph Format Handling additional functions.

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
#include "time.h"


#include "ter_hdr.h"

/******************************************************************************
    SetParaTextFlow:
    Set the rtl/ltr text flow for the paragraph id.
******************************************************************************/
int SetParaTextFlow(PTERWND w,int ParaId,int flow)
{

       return  NewParaId(w,ParaId,
                         PfmtId[ParaId].LeftIndent,
                         PfmtId[ParaId].RightIndentTwips,
                         PfmtId[ParaId].FirstIndent,
                         PfmtId[ParaId].TabId,
                         PfmtId[ParaId].BltId,
                         PfmtId[ParaId].AuxId,
                         PfmtId[ParaId].Aux1Id,
                         PfmtId[ParaId].StyId,
                         PfmtId[ParaId].shading,
                         PfmtId[ParaId].pflags,
                         PfmtId[ParaId].SpaceBefore,
                         PfmtId[ParaId].SpaceAfter,
                         PfmtId[ParaId].SpaceBetween,
                         PfmtId[ParaId].LineSpacing,
                         PfmtId[ParaId].BkColor,
                         PfmtId[ParaId].BorderSpace,
                         flow,
                         PfmtId[ParaId].flags);
}


/******************************************************************************
    TerSetParaTextFlow:
    Set the rtl/ltr text flow for the paragraph.
******************************************************************************/
BOOL WINAPI _export TerSetParaTextFlow(HWND hWnd,BOOL dialog,int flow, BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;
    if (EditingParaStyle) return FALSE; // not supported to styles yet

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected

    // get the list override and level
    if (dialog) {
       DlgInt1=PfmtId[pfmt(FirstLine)].flow;
       lstrcpy(DlgText1,"Paragraph Text Flow");

       if (!CallDialogBox(w,"ParaTextFlowParam",ParaTextFlowParam,0L)) return TRUE;

       flow=DlgInt1;
    }
    if (flow!=FLOW_DEF && flow!=FLOW_RTL && flow!=FLOW_LTR) return TRUE;


    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndent,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndent,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         flow,
                         PfmtId[pfmt(l)].flags);
    }

    //END:
    if (repaint) {
      RequestPagination(w,TRUE);
      PaintTer(w);
    }

    return TRUE;
}

/******************************************************************************
    TerSetListBullet:
    Set a list bullet or numbering. Set the type parameter to LIST_ constant.
******************************************************************************/
BOOL WINAPI _export TerSetListBullet(HWND hWnd, bool set, int NumType, int level, int start, LPBYTE TextBef, LPBYTE TextAft, bool repaint)
{
    return TerSetListBullet2(hWnd,set,NumType,level,start,TextBef,TextAft,NULL,repaint);
}

/******************************************************************************
    TerSetListBullet:
    Set a list bullet or numbering. Set the type parameter to LIST_ constant.
******************************************************************************/
BOOL WINAPI _export TerSetListBullet2(HWND hWnd, bool set, int NumType, int level, int start, LPBYTE TextBef, LPBYTE TextAft, LPBYTE ListText, bool repaint)
{
    int l,i,FirstLine,LastLine;
    int flags;
    int  LeftIndent,FirstIndent,BltId;
    int  FirstBltId=0;
    PTERWND w;
    DWORD LevelFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

      
    if (!TerArg.WordWrap) return false;

    if (TextBef==null) TextBef="";
    if (TextAft==null) TextAft="";
    if (ListText==null) ListText="";

    if (level>10) level=10;

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // retrieve the old values
       if (EditingParaStyle) {
          flags=StyleId[CurSID].ParaFlags;
          LeftIndent=StyleId[CurSID].LeftIndentTwips;
          FirstIndent=StyleId[CurSID].FirstIndentTwips;
          BltId=StyleId[CurSID].BltId;
       }
       else {
          flags=PfmtId[pfmt(l)].flags;
          LeftIndent=PfmtId[pfmt(l)].LeftIndentTwips;
          FirstIndent=PfmtId[pfmt(l)].FirstIndentTwips;
          BltId=PfmtId[pfmt(l)].BltId;
       }
       if (level<0 && True(flags&BULLET)) level=TerBlt[BltId].lvl;   // use the current level
       if (level<0) level=0;

       // get new paraid
       if (set) {
          flags|=BULLET;
          if (FirstBltId==0) {
              int ListOrId=0,ListId=0,font;
              bool CreateBltId=false,CreateList=false,UseOverride;
              struct StrListLevel *pNewLevel;
              BYTE NumFmt[50];

              if (TerBlt[BltId].ls>0) {
                 ListOrId=TerBlt[BltId].ls;
                 ListId=ListOr[ListOrId].ListIdx;
              }
                
              CreateList=(TerBlt[BltId].ls==0 || !ListOr[ListOrId].InUse || list[ListId].LevelCount!=9);  // not a standard list
              if (CreateList) { 
                  BYTE name[100];
                  GetNewListName(w,"List",name);
                  ListId=TerEditList(hTerWnd,true,0,false,name,true,0);
                  ListOrId=TerEditListOr(hTerWnd,true,0,false,ListId,false,0);
                  CreateBltId=true;
              } 
              
              // check if new list-or needs to be created
              pNewLevel=null;
              if      (ListOr[ListOrId].LevelCount>level) pNewLevel=&(ListOr[ListOrId].level[level]);
              else if (list[ListId].LevelCount>level) pNewLevel=&(list[ListId].level[level]);
              if (pNewLevel && (pNewLevel->NumType!=NumType)) pNewLevel=null;    // this override can not be used
           
              if (pNewLevel==null) {     // check if other override available for this list
                 for (i=0;i<TotalListOr;i++) {
                    if (ListOr[i].ListIdx!=ListId) continue;
                    if (ListOr[i].LevelCount<=level) continue;
                    if (ListOr[i].level[level].NumType==NumType) {
                       ListOrId=i;
                       CreateBltId=true;
                       pNewLevel=&(ListOr[ListOrId].level[level]);
                       break;
                    } 
                 } 
              } 
           
              if (pNewLevel==null) {   // no valid list-or found, create a new list-or id
                 ListOrId=TerEditListOr(hTerWnd,true,0,false,ListId,true,0);
                 CreateBltId=true;
                 if (ListOr[ListOrId].LevelCount>level) {
                    pNewLevel=&(ListOr[ListOrId].level[level]);
                    if (NumType==LIST_BLT) {
                       pNewLevel->NumType=NumType;
                       pNewLevel->flags|=(LISTLEVEL_RESTART|LISTLEVEL_REFORMAT);
                    }
                 }
              } 
              if (pNewLevel==null) return true;


              // set level properties
              NumFmt[0]=0;
              font=GetCurCfmt(w,l,0);
              if (NumType==LIST_BLT) {
                  LPBYTE typeface="";
                  BYTE sym=ListText[0];
                  BYTE BulletChar=0;
                  int CharSet=ANSI_CHARSET;

                  if (sym==BLT_DIAMOND) {
                     BulletChar=0xa8;
                     typeface="Symbol";
                  }
                  else if (sym==BLT_SQUARE) {
                     BulletChar=0xa7;
                     typeface="Wingdings";
                  }
                  else if (sym==BLT_HOLLOW_SQUARE) {
                     BulletChar='q';
                     typeface="Wingdings";
                  }
                  else if (sym==BLT_4_DIAMONDS) {
                     BulletChar='v';
                     typeface="Wingdings";
                  }
                  else if (sym==BLT_ARROW) {
                     BulletChar=0xd8;
                     typeface="Wingdings";
                  }
                  else if (sym==BLT_CHECK) {
                     BulletChar=0xfc;
                     typeface="Wingdings";
                  }
                  else if (sym==BLT_ROUND || sym==0) { // default
                     BulletChar=0xb7;
                     typeface="Symbol";
                  }
                  else BulletChar=sym;

                  NumFmt[0]=BulletChar;
                  NumFmt[1]=0;
                  if (typeface[0]!=0) font=GetNewFont(w,hTerDC,font,typeface,TerFont[font].TwipsSize,0,TerFont[font].TextColor,CLR_WHITE,TerFont[font].UlineColor,0,0,0,1,0,0,0,0,NULL,0,0,SYMBOL_CHARSET,0,TerFont[font].TextAngle);
              }
              else {
                  wsprintf(NumFmt,"%s%s%d%s%s",TextBef,"~",(level+1),"~",TextAft);
                  if (lstrlen(ListText)>0) strcpy(NumFmt,ListText);
              }

              UseOverride=(ListOr[ListOrId].LevelCount>level);
              LevelFlags=UseOverride?ListOr[ListOrId].level[level].flags:list[ListId].level[level].flags;
              TerEditListLevel(hTerWnd,!UseOverride,UseOverride?ListOrId:ListId,level,start,NumType,LISTAFT_TAB, NumFmt,font,LevelFlags);
                
              if (CreateBltId || TerBlt[BltId].lvl!=level) BltId=TerCreateListBulletId(hTerWnd,ListOrId,level);
              FirstBltId=BltId;
          }
          else BltId=FirstBltId;   // use the same id for the whole list
       }
       else {
          flags=ResetUintFlag(flags,BULLET);
          BltId=0;
       }

       if (set) {
          int indent=DefTabWidth/2;
          if (indent==0) indent=270;
          while (indent<270) indent*=2;

          LeftIndent=indent*(level+1);
          FirstIndent=-indent;
          if (ListText!=NULL) {
             int AddIndent=(strlen(ListText)/6)*indent;
             if (AddIndent>1440) AddIndent=1440;
             LeftIndent+=AddIndent;
             FirstIndent-=AddIndent;
          }
       }
       else {
          LeftIndent=FirstIndent=0;
       }

       // update stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].ParaFlags=flags;
          StyleId[CurSID].LeftIndentTwips=LeftIndent;
          StyleId[CurSID].FirstIndentTwips=FirstIndent;
          StyleId[CurSID].BltId=BltId;
          DrawRuler(w,false);
          return true;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         LeftIndent,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         FirstIndent,
                         PfmtId[pfmt(l)].TabId,
                         BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         flags);
    }

    TerArg.modified++;

    if (repaint) {
       RequestPagination(w,true);
       PaintTer(w);
       if (RulerPending) DrawRuler(w,false);
    }

    return true;
} 

/******************************************************************************
    TerSetListLevel:
    increment/decrement or set level
******************************************************************************/
BOOL WINAPI _export TerSetListLevel(HWND hWnd, int level, int increment,bool repaint)
{
    int l,m,i,FirstLine,LastLine,indent;
    int flags;
    int  FirstLeftIndent,FirstFirstIndent,LeftIndent,FirstIndent,BltId;
    int  FirstBltId=0,PrevBltId;
    PTERWND w;
    BYTE ListText[50]="";
    BOOL IndentSet;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return false;

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // retrieve the old values
       if (EditingParaStyle) {
          flags=StyleId[CurSID].ParaFlags;
          LeftIndent=StyleId[CurSID].LeftIndentTwips;
          FirstIndent=StyleId[CurSID].FirstIndentTwips;
          BltId=StyleId[CurSID].BltId;
       }
       else {
          flags=PfmtId[pfmt(l)].flags;
          LeftIndent=PfmtId[pfmt(l)].LeftIndentTwips;
          FirstIndent=PfmtId[pfmt(l)].FirstIndentTwips;
          BltId=PfmtId[pfmt(l)].BltId;
       }

       if (False(flags&BULLET)) continue;
       if (TerBlt[BltId].ls==0) continue;

       // get new bullet id
       if (FirstBltId==0) {
           struct StrListLevel far *pLevel;
           struct StrListLevel far *pNewLevel=null;
           int ListOrId=0,ListId=0;
             
           pLevel=GetListLevelPtr(w,TerBlt[BltId].ls,TerBlt[BltId].lvl);

           if (level==-1) level=TerBlt[BltId].lvl+increment;  // new level
           if (level<0) level=0;
           if (level>8) level=8;

           // check if pLevel from the previous or the next line should be used.
           pLevel=GetListLevelPtr(w,TerBlt[BltId].ls,level);                       // default pLevel for the new level
           if (l>0) pLevel=CheckLinePLevel(w,l-1,level,pLevel,NULL);               // get the pLevel of the previous line
           if ((l+1)<TotalLines) pLevel=CheckLinePLevel(w,l+1,level,pLevel,NULL);  // get the pLevel of the previous line

           ListOrId=TerBlt[BltId].ls;
           ListId=ListOr[ListOrId].ListIdx;
           if      (ListOr[ListOrId].LevelCount>level) pNewLevel=&(ListOr[ListOrId].level[level]);
           else if (list[ListId].LevelCount>level) pNewLevel=&(list[ListId].level[level]);
           
           if (l>0) pNewLevel=CheckLinePLevel(w,l-1,level,pNewLevel,&ListOrId);               // get the pLevel of the previous line
           if ((l+1)<TotalLines) pNewLevel=CheckLinePLevel(w,l+1,level,pNewLevel,&ListOrId);  // get the pLevel of the next line

           if (pNewLevel && (pNewLevel->NumType!=pLevel->NumType)) pNewLevel=null;    // this override can not be used
           
           if (pNewLevel==null) {     // check if other override available for this list
              for (i=0;i<TotalListOr;i++) {
                 if (ListOr[i].ListIdx!=ListId) continue;
                 if (ListOr[i].LevelCount<=level) continue;
                 if (ListOr[i].level[level].NumType==pLevel->NumType) {
                    ListOrId=i;
                    pNewLevel=&(ListOr[ListOrId].level[level]);
                    break;
                 } 
              } 
           } 
           
           if (pNewLevel==null) {   // no valid list-or found, create a new list-or id
              ListOrId=TerEditListOr(hTerWnd,true,0,false,ListId,true,0);
              if (ListOr[ListOrId].LevelCount>level) {
                 pNewLevel=&(ListOr[ListOrId].level[level]);
                 if (pLevel->NumType==LIST_BLT) {
                    FarMove(pLevel,pNewLevel,sizeof(struct StrListLevel));  // copy the property of the current level
                    pNewLevel->flags|=(LISTLEVEL_RESTART|LISTLEVEL_REFORMAT);
                 }
              }
           } 
           if (pNewLevel==null) return true;

           DecodeListText(w,pNewLevel->text,ListText);

           BltId=TerCreateListBulletId(hTerWnd,ListOrId,level);

           // Set indentation - get indentation from previous lines
           IndentSet=false;
           for (m=l-1;m>=0;m--) {
              PrevBltId=PfmtId[pfmt(m)].BltId;
              if (TerBlt[PrevBltId].ls==0) break;
              if (TerBlt[PrevBltId].lvl==level) {
                  LeftIndent=PfmtId[pfmt(m)].LeftIndentTwips;
                  FirstIndent=PfmtId[pfmt(m)].FirstIndentTwips;
                  IndentSet=true;
                  break;
              } 
           } 

           if (!IndentSet) {           // indent using the values from the current line
               indent=DefTabWidth/2;
               if (indent==0) indent=270;
               while (indent<270) indent*=2;
               LeftIndent+=indent*increment;
               if (LeftIndent<0) LeftIndent=0;
           } 

           FirstBltId=BltId;
           FirstLeftIndent=LeftIndent;
           FirstFirstIndent=FirstIndent;
       }
       else {
           BltId=FirstBltId;   // use the same id for the whole list
           LeftIndent=FirstLeftIndent;
           FirstIndent=FirstFirstIndent;
       }



       // update stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].ParaFlags=flags;
          StyleId[CurSID].LeftIndentTwips=LeftIndent;
          StyleId[CurSID].FirstIndentTwips=FirstIndent;
          StyleId[CurSID].BltId=BltId;
          DrawRuler(w,false);
          return true;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         LeftIndent,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         FirstIndent,
                         PfmtId[pfmt(l)].TabId,
                         BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         flags);
    }

    TerArg.modified++;

    if (repaint) {
       RequestPagination(w,true);
       PaintTer(w);
       if (RulerPending) DrawRuler(w,false);
    }

    return true;

   
}

/******************************************************************************
    CheckLinePLevel:
    Get the pLevel of the specified line if it belongs to the given level.
    Otherwise return the default.
******************************************************************************/
struct StrListLevel *CheckLinePLevel(PTERWND w,long l, int level, struct StrListLevel *pDefLevel,LPINT pListOrId)
{
     struct StrListLevel far *pLevel;
     int BltId,flags;
             
       
     if (EditingParaStyle) {
        flags=StyleId[CurSID].ParaFlags;
        BltId=StyleId[CurSID].BltId;
     }
     else {
        flags=PfmtId[pfmt(l)].flags;
        BltId=PfmtId[pfmt(l)].BltId;
     }

     if (False(flags&BULLET)) return pDefLevel;
     if (TerBlt[BltId].ls==0) return pDefLevel;

     pLevel=GetListLevelPtr(w,TerBlt[BltId].ls,TerBlt[BltId].lvl);

     if (level!=TerBlt[BltId].lvl) return pDefLevel;   // the given line does not correspond to the requested level
 
     if (pListOrId) (*pListOrId)=TerBlt[BltId].ls;

     return pLevel;
}
 
/******************************************************************************
    GetNewListName:
    get a unique list nam
******************************************************************************/
GetNewListName(PTERWND w,LPBYTE prefix, LPBYTE name)
{
   int i,j;

   for (i=1;true;i++) {
      wsprintf(name,"%s%d_%d",prefix,(long)time(null),i);
      for (j=0;j<TotalLists;j++) if (list[j].name && strcmp(list[j].name,name)==0) break;
      if (j==TotalLists) return TRUE;
   }
}

/******************************************************************************
    TerGetListLine:
    This function checks if the given line is part of the list where the 
    numbering is suppressed.  If it is, then the function returns the 
    previous line where the line numbering is enabled.  Otherwise, it returns
    -1.
    The 'line' parameter can be set to -1 to specify the current line.
******************************************************************************/
long WINAPI _export TerGetListLine(HWND hWnd,long line)
{
    PTERWND w;
    long l,PrevLine;
    int  PrevListId=0,PrevLeftIndent=0,BltId,ls;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (line<0) line=CurLine;
    if (True(PfmtId[pfmt(line)].flags&BULLET)) return -1;  // line already belongs to a list

    // find the previous list
    for (l=line-1;l>=0;l--) {
       if (False(PfmtId[pfmt(l)].flags&BULLET)) continue;
       if ((BltId=PfmtId[pfmt(l)].BltId)==0) break;      // bullet found, but no list numbering
       if ((ls=TerBlt[BltId].ls)==0) break;  // numbering found, but no list numbering
       
       PrevListId=ListOr[ls].ListIdx;
       PrevLine=l;
       PrevLeftIndent=PfmtId[pfmt(l)].LeftIndentTwips;
       break;
    } 
    if (PrevListId==0) return -1;     // no previous list found
    if (PrevLeftIndent!=PfmtId[pfmt(line)].LeftIndentTwips) return -1;  // not a continued line

    // find the next list
    // find the previous list
    for (l=line+1;l<TotalLines;l++) {
       if (False(PfmtId[pfmt(l)].flags&BULLET)) continue;
       if ((BltId=PfmtId[pfmt(l)].BltId)==0) return -1;      // bullet found, but no list numbering
       if ((ls=TerBlt[BltId].ls)==0) return -1;  // numbering found, but no list numbering
       
       return (PrevListId==ListOr[ls].ListIdx)?PrevLine:-1;   
    } 
  
    return -1;
}

/******************************************************************************
    TerSetParaList:
    Set the list override and list level for a paragraph. Set the ParaId to 
    -1 to apply the list to the current paragraph or currently selected paragraph.
    This funtion returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetParaList(HWND hWnd,BOOL dialog,int ParaId, int CurListOr, int level, BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;
    BOOL SetParaId=(ParaId>=0);
    int  LevelCount,BltId;
    struct StrBlt blt;
    UINT flags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // get the current properties
    if (SetParaId) {
       if (ParaId>=TotalPfmts) return FALSE;
       BltId=PfmtId[ParaId].BltId;
    }
    else {
       GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
       if (EditingParaStyle) BltId=StyleId[CurSID].BltId;
       else {
          ParaId=pfmt(FirstLine);
          BltId=PfmtId[ParaId].BltId;
       } 
    }  

    // get the list override and level
    if (dialog) {
       DlgInt1=DlgInt2=0;
       if (BltId>0) {
          DlgInt1=TerBlt[BltId].ls;
          DlgInt2=TerBlt[BltId].lvl;
       }

       if (!CallDialogBox(w,"ListParaParam",ListParaParam,0L)) return TRUE;

       CurListOr=DlgInt1;
       level=DlgInt2;
    }

    // validate parameters
    if (CurListOr<0 || CurListOr>=TotalListOr) return FALSE;
    if (CurListOr>0) {
       LevelCount=ListOr[CurListOr].LevelCount;
       if (LevelCount==0) {
          int CurList=ListOr[CurListOr].ListIdx;
          LevelCount=list[CurList].LevelCount;
       } 
       if (level<0 || level>=LevelCount) return FALSE;
    }

    TerArg.modified++;

    // create a tab id with the specified override and level
    if (CurListOr==0) BltId=0;
    else {
       FarMove(&(TerBlt[0]),&blt,sizeof(struct StrBlt));
       blt.IsBullet=FALSE;
       blt.ls=CurListOr;
       blt.lvl=level;
       BltId=NewBltId(w,0,&blt);
    }

    // apply the new bullet id
    if (SetParaId) {
       PfmtId[ParaId].BltId=BltId;
       ResetUintFlag(PfmtId[ParaId].flags,BULLET);
       if (CurListOr) PfmtId[ParaId].flags|=BULLET;

       if (repaint) goto END;
       else         return TRUE;
    } 

    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // retrieve the old values
       if (EditingParaStyle) flags=StyleId[CurSID].ParaFlags;
       else                  flags=PfmtId[pfmt(l)].flags;

       // get new paraid
       if (CurListOr>0) flags|=BULLET;
       else             flags=ResetUintFlag(flags,BULLET);

       // update stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].ParaFlags=flags;
          StyleId[CurSID].BltId=BltId;
          DrawRuler(w,FALSE);
          return TRUE;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndent,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndent,
                         PfmtId[pfmt(l)].TabId,
                         BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         flags);
    }

    END:
    if (repaint) {
      RequestPagination(w,TRUE);
      PaintTer(w);
    }

    return TRUE;
}

/******************************************************************************
    TerEditListLevel:
    Edit the specified level for a list or list orverride. Set 'id' to -1 to show a 
    property dialog box. The level number within the text string must be surrounded by
    ~. Example:  (~1~) would indicate first level number within paranthesis.

    This funtion returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerEditListLevel(HWND hWnd, BOOL IsList, int id, int level, int StartAt, int NumType, int CharAft, LPBYTE text, int FontId, DWORD flags)
{
    PTERWND w;
    struct StrListLevel far *pLevel;
    

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (id<0) {
       if (!CallDialogBox(w,"ListLevelParam",ListLevelParam,0L)) return TRUE;
    } 
    else {
       if (id<=0 || level<0) return FALSE;    // invalid id
       if (FontId<0 || !(TerFont[FontId].InUse) || (TerFont[FontId].style&PICT)) return FALSE;

       if (IsList) {
          if (id>=TotalLists || !list[id].InUse) return FALSE;
          if (level>=list[id].LevelCount) return FALSE;
          pLevel=&(list[id].level[level]);
       }
       else {
          if (id>=TotalListOr || !ListOr[id].InUse) return FALSE;
          if (level>=ListOr[id].LevelCount) return FALSE;
          pLevel=&(ListOr[id].level[level]);
       }
       
       // set the standard bullet characters and font ids
       if (NumType==LIST_BLT) {
           LPBYTE typeface="";
           BYTE sym=text[0];
           BYTE BulletChar=0;
           int CharSet=ANSI_CHARSET;

           if (sym==BLT_DIAMOND) {
              BulletChar=0xa8;
              typeface="Symbol";
           }
           else if (sym==BLT_SQUARE) {
              BulletChar=0xa7;
              typeface="Wingdings";
           }
           else if (sym==BLT_HOLLOW_SQUARE) {
              BulletChar='q';
              typeface="Wingdings";
           }
           else if (sym==BLT_4_DIAMONDS) {
              BulletChar='v';
              typeface="Wingdings";
           }
           else if (sym==BLT_ARROW) {
              BulletChar=0xd8;
              typeface="Wingdings";
           }
           else if (sym==BLT_CHECK) {
              BulletChar=0xfc;
              typeface="Wingdings";
           }
           else if (sym==BLT_ROUND || sym==0) { // default
              BulletChar=0xb7;
              typeface="Symbol";
           }
           else BulletChar=sym;

           text[0]=BulletChar;
           text[1]=0;
           if (typeface[0]!=0) FontId=GetNewFont(w,hTerDC,FontId,typeface,TerFont[FontId].TwipsSize,0,TerFont[FontId].TextColor,CLR_WHITE,TerFont[FontId].UlineColor,0,0,0,1,0,0,0,0,NULL,0,0,SYMBOL_CHARSET,0,TerFont[FontId].TextAngle);
       } 

       // apply the new values
       pLevel->start=StartAt;
       pLevel->NumType=NumType;  
       pLevel->CharAft=CharAft;
       pLevel->FontId=FontId;
       pLevel->flags=flags;

       CodeListText(w,text,pLevel->text);
    } 

    TerArg.modified++;
    RequestPagination(w,true);

    return TRUE;
}

/******************************************************************************
    TerEditListOr:
    Edit or create a new list override.  The ListOrId can be set to -1 to show the
    list override selection dialog box.
    This function returns the current list override id.
******************************************************************************/
int WINAPI _export TerEditListOr(HWND hWnd, BOOL NewListOr, int ListOrId, BOOL PropDialog, int ListId, BOOL override, UINT flags)
{
    int i,FirstList,LevelCount;
    PTERWND w;
    struct StrListLevel far *pLevel;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    for (i=1;i<TotalLists;i++) if (list[i].InUse) break;
    if (i==TotalLists) return -1;     // no valid list found
    FirstList=i;

    if (NewListOr) {    // create a new list
       if ((ListOrId=GetListOrSlot(w))<0) return -1;  // slot not found

       // populate list variables
       ListOr[ListOrId].InUse=TRUE;
       //ListId=FirstList;                        // list being overridden
       ListOr[ListOrId].ListIdx=ListId;         // list being overridden
       LevelCount=(override?list[ListId].LevelCount:0);
       ListOr[ListOrId].LevelCount=LevelCount;
       ListOr[ListOrId].flags=flags;
       
       if (LevelCount>0) {
          ListOr[ListOrId].level=MemAlloc(LevelCount*sizeof(struct StrListLevel));
          FarMove(list[ListId].level,ListOr[ListOrId].level,LevelCount*sizeof(struct StrListLevel));

          // set default attributes
          for (i=0;i<LevelCount;i++) {
             pLevel=&(ListOr[ListOrId].level[i]);
             pLevel->flags|=(LISTLEVEL_RESTART|LISTLEVEL_REFORMAT);
          } 
       }
    }
    else if (ListOrId<0) {
       if ((ListOrId=CallDialogBox(w,"ListOrSelectParam",ListOrSelectParam,0L))<0) return -1;
    }  
    if (ListOrId<1 || ListOrId>=TotalListOr || !ListOr[ListOrId].InUse) return -1;

    // prompt for ListOr properties
    if (PropDialog) {        // use the property dialog
       DlgInt1=ListOr[ListOrId].ListIdx;                    // list index
       DlgInt2=(ListOr[ListOrId].LevelCount>0)?TRUE:FALSE;  // override levels
     
       if (!CallDialogBox(w,"ListOrPropParam",ListOrPropParam,0L)) return -1;
       ListId=DlgInt1;
       override=DlgInt2;
    } 

    // set ListOr properties

    ListOr[ListOrId].flags=flags;

    LevelCount=(override)?list[ListId].LevelCount:0;

    if (LevelCount==ListOr[ListOrId].LevelCount) {
       if (ListId!=ListOr[ListOrId].ListIdx && LevelCount>0) {   // listid changed, copy level information from list
          FarMove(list[ListId].level,ListOr[ListOrId].level,LevelCount*sizeof(struct StrListLevel));
       } 
    }
    else {   // different list selected
       if (ListOr[ListOrId].LevelCount>0) {        // free the current level information
          MemFree(ListOr[ListOrId].level);
          ListOr[ListOrId].level=NULL;
       }
       ListOr[ListOrId].LevelCount=LevelCount;
       ListOr[ListOrId].level=MemAlloc(LevelCount*sizeof(struct StrListLevel));
       FarMove(list[ListId].level,ListOr[ListOrId].level,LevelCount*sizeof(struct StrListLevel));
    }  
    
    // connect to the requested list
    ListOr[ListOrId].ListIdx=ListId;

    TerArg.modified++;

    return ListOrId;
}
 
/******************************************************************************
    TerEditList:
    Edit or create a new list.  The ListId can be set to -1 to show the
    list selection dialog box.
    This function returns the current list id.
******************************************************************************/
int WINAPI _export TerEditList(HWND hWnd, BOOL NewList, int ListId, BOOL PropDialog, LPBYTE name, BOOL nested, DWORD flags)
{
    int i,j,len,LevelCount;
    long TmplId,RtfId;
    struct StrListLevel far *pLevel;
    struct StrListLevel far *pLevelNew;
    LPBYTE pText;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data


    if (NewList) {    // create a new list
       if ((ListId=GetListSlot(w))<0) return -1;  // slot not found

       // populate list variables
       TmplId=1;                        // find the template value
       RtfId=(long)time(null);          // list id for the rtf file - seconds elapsed since midnight dec31,1899
       for (i=0;i<TotalLists;i++) {
          if (list[i].InUse && list[i].TmplId>=TmplId) TmplId=list[i].TmplId+1;  // use the next template id
          if (list[i].InUse && list[i].id>=RtfId) RtfId=list[i].id+1;  // use the next template id
       }
       list[ListId].InUse=TRUE;
       list[ListId].id=RtfId;
       list[ListId].TmplId=TmplId;
       list[ListId].FontId=0;
       LevelCount=(nested?9:1);
       list[ListId].LevelCount=LevelCount;
       list[ListId].flags=flags;
       if (name) list[ListId].name=MemAlloc(lstrlen(name)+1);
       lstrcpy(list[ListId].name,name);
       list[ListId].level=MemAlloc(LevelCount*sizeof(struct StrListLevel));
       FarMemSet(list[ListId].level,0,LevelCount*sizeof(struct StrListLevel));
    
       pLevel=list[ListId].level;

       // initialize the list levels
       for (i=0;i<LevelCount;i++) {
          pLevel[i].start=1;
          pLevel[i].CharAft=LISTAFT_TAB;
          pLevel[i].NumType=LIST_DEC;
       
          pText=pLevel[i].text;  
          pText[0]=2; pText[1]=i; pText[2]='.';
       } 
    }
    else if (ListId<0) {
       if (TotalLists<=1) return -1;  // nothing to select from
       if ((ListId=CallDialogBox(w,"ListSelectParam",ListSelectParam,0L))<0) return -1;
    }  
    if (ListId<1 || ListId>=TotalLists || !list[ListId].InUse) return -1;
 
    // prompt for list properties
    if (PropDialog) {        // use the property dialog
       DlgText1[0]=0;
       if (list[ListId].name) lstrcpy(DlgText1,list[ListId].name);
       DlgInt1=(list[ListId].LevelCount>1)?TRUE:FALSE;
       DlgLong=list[ListId].flags;
     
       if (!CallDialogBox(w,"ListPropParam",ListPropParam,0L)) return -1;
       name=DlgText1;
       nested=DlgInt1;
       flags=DlgLong;
    } 

    // set list properties
    if (name) {
       if (list[ListId].name) MemFree(list[ListId].name);
       list[ListId].name=MemAlloc(lstrlen(name)+1);
       lstrcpy(list[ListId].name,name);
    }

    list[ListId].flags=flags;

    LevelCount=(nested?9:1);
    if (LevelCount!=list[ListId].LevelCount) {
       list[ListId].level=MemReAlloc(list[ListId].level,LevelCount*sizeof(struct StrListLevel));
       
       if (LevelCount>list[ListId].LevelCount) {   // initialize the new levels
          int CurCount=list[ListId].LevelCount;
          
          pLevel=list[ListId].level;   // first element
          
          for (i=CurCount;i<LevelCount;i++) {
             pLevelNew=&(list[ListId].level[i]);
             FarMove(pLevel,pLevelNew,sizeof(struct StrListLevel));
             
             len=pLevel->text[0]+1;  // length of the number string
             
             for (j=1;j<len;j++) {
                if (pLevelNew->text[j]==0) pLevelNew->text[j]=i;  // print the number corresponding to the current level
             } 
          } 
       } 
       list[ListId].LevelCount=LevelCount;
    } 

    TerArg.modified++;

    return ListId;
}

/******************************************************************************
    TerGetListId:
    Get list-id for a list name. The funtion returns -1 if the list name is
    not found 
******************************************************************************/
int WINAPI _export TerGetListId(HWND hWnd, LPBYTE name)
{
    int i;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    for (i=0;i<TotalLists;i++) {
      if (list[i].InUse && lstrcmp(list[i].name,name)==0) return i;
    }
    
    return -1; 
}

/******************************************************************************
    TerGetListInfo
    Get information about a specifiee list id.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerGetListInfo(HWND hWnd, int ListId, LPBYTE name, LPINT pLevelCount, LPDWORD pFlags)
{

    return TerGetListInfo2(hWnd,ListId,name,pLevelCount,pFlags,NULL,NULL);
}
 
/******************************************************************************
    TerGetListInfo2
    Get information about a specifiee list id.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerGetListInfo2(HWND hWnd, int ListId, LPBYTE name, LPINT pLevelCount, LPDWORD pFlags, LPLONG pRtfId, LPLONG pTmplId)
{
    PTERWND w;
    struct StrList far *pList;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (ListId<0) {      // get it from the current line
       int ParaId=pfmt(CurLine);
       int BltId=PfmtId[ParaId].BltId;
       int ListOrId;
       if (BltId==0 || TerBlt[BltId].ls==0) return FALSE;
       ListOrId=TerBlt[BltId].ls;
       ListId=ListOr[ListOrId].ListIdx;
    } 

    if (ListId<0 || ListId>=TotalLists) return FALSE;

    pList=&(list[ListId]);

    if (name)        lstrcpy(name,pList->name);
    if (pLevelCount) (*pLevelCount)=pList->LevelCount;
    if (pFlags)      (*pFlags)=pList->flags;
    if (pRtfId)      (*pRtfId)=pList->id;
    if (pTmplId)     (*pTmplId)=pList->TmplId;

    return TRUE;
}
 
/******************************************************************************
    TerGetListOrInfo:
    Exract information about a list override id.
    This fucntions returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerGetListOrInfo(HWND hWnd, int ListOrId, LPINT pListId, LPINT pLevelCount, LPINT pFlags)
{
    PTERWND w;
    struct StrListOr far *pListOr;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (ListOrId<0) {      // get it from the current line
       int ParaId=pfmt(CurLine);
       int BltId=PfmtId[ParaId].BltId;
       if (BltId==0 || TerBlt[BltId].ls==0) return FALSE;
       ListOrId=TerBlt[BltId].ls;
    } 

    if (ListOrId<0 || ListOrId>=TotalListOr) return FALSE;

    pListOr=&(ListOr[ListOrId]);

    if (pListId)     (*pListId)=pListOr->ListIdx;
    if (pLevelCount) (*pLevelCount)=pListOr->LevelCount;
    if (pFlags)      (*pFlags)=pListOr->flags;

    return TRUE;
}

/******************************************************************************
    TerGetListLevelInfo:
    Get the list level information.
    This funtion returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerGetListLevelInfo(HWND hWnd, BOOL IsList, int id, int level, LPINT pStartAt, 
                            LPINT pNumType, LPINT pCharAft, LPBYTE text, LPINT pFontId, LPDWORD pFlags)
{
    PTERWND w;
    struct StrListLevel far *pLevel;
    LPBYTE pTypeface=NULL;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return TRUE;  // get the pointer to window data

    if (id<0) {      // get it from the current line
       int ParaId=pfmt(CurLine);
       int BltId=PfmtId[ParaId].BltId;
       int ListOrId;
       if (BltId==0 || TerBlt[BltId].ls==0) return FALSE;
       ListOrId=TerBlt[BltId].ls;
       if (IsList) id=ListOr[ListOrId].ListIdx;
       else        id=ListOrId;
       level=TerBlt[BltId].lvl;
    } 

    if (IsList) {
       if (id>=TotalLists || !list[id].InUse) return FALSE;
       if (level>=list[id].LevelCount) return FALSE;
       pLevel=&(list[id].level[level]);
    }
    else {
       if (id>=TotalListOr || !ListOr[id].InUse) return FALSE;
       if (level>=ListOr[id].LevelCount) return FALSE;
       pLevel=&(ListOr[id].level[level]);
    }
       
    // apply the new values
    if (pStartAt) (*pStartAt)=pLevel->start;
    if (pNumType) (*pNumType)=pLevel->NumType;  
    if (pCharAft) (*pCharAft)=pLevel->CharAft;
    if (pFontId)  (*pFontId) =pLevel->FontId;
    if (pFlags)   (*pFlags)  =pLevel->flags;

    DecodeListText(w,pLevel->text,text);

    // convert the bullet symbols
    pTypeface=TerFont[pLevel->FontId].TypeFace;

    if (pLevel->NumType==LIST_BLT && text!=null) {
        if (lstrlen(text)==0) {
           text[0]=BLT_ROUND;
           text[1]=0;
        } 
        else if (lstrlen(text)==1) {
           BYTE BulletChar=text[0];

           if      (BulletChar==0xa8 && lstrcmpi(pTypeface,"Symbol")==0)    text[0]=BLT_DIAMOND;
           else if (BulletChar==0xa7 && lstrcmpi(pTypeface,"Wingdings")==0) text[0]=BLT_SQUARE;
           else if (BulletChar=='q' && lstrcmpi(pTypeface,"Wingdings")==0)  text[0]=BLT_HOLLOW_SQUARE;
           else if (BulletChar=='v' && lstrcmpi(pTypeface,"Wingdings")==0)  text[0]=BLT_4_DIAMONDS;
           else if (BulletChar==0xd8 && lstrcmpi(pTypeface,"Wingdings")==0) text[0]=BLT_ARROW;
           else if (BulletChar==0xfc && lstrcmpi(pTypeface,"Wingdings")==0) text[0]=BLT_CHECK;
           else if (BulletChar==0xb7 && lstrcmpi(pTypeface,"Symbol")==0)    text[0]=BLT_ROUND;
        }
    } 

    return TRUE;
}

/******************************************************************************
    GetListText:
    Get list text.
******************************************************************************/
GetListText(PTERWND w,int ParaId,long LineNo,LPBYTE ListText,LPINT pListTextWidth,LPLONG pListNbr,LPINT pFontId, int CurParaFont, int FieldNbr, BOOL UsePrtUnits)
{
    int    NbrX,TabX,len,i,j,BltId,PixWidth=0,ListOrId,CurLevel,level,TabId,FontId=0;
    int    TabPos,TabType,CurTabWidth=0,MinWidth;
    long   nbr,ListNbr=0;
    BYTE   text[100],chr,string[30];
    LPBYTE pTemplate;
    struct StrListLevel far *pLevel;
    struct StrListLevel far *pCurLevel;
    HDC hDC;
    SIZE  size;
    HFONT hOldFont;
    BYTE  typeface[33];
    int   TwipsSize,override,NumType;
    UINT  style;
    COLORREF TextColor,TextBkColor;
    struct StrListnum far *pListnum;
    BOOL  IsRoman=FALSE;

    text[0]=0;
    MinWidth=0;           // minimum bullet width
    FontId=0;
    PixWidth=0;

    if (LineInfo(w,LineNo,INFO_ROW)) goto END;
    
    if (FieldNbr>=0) {
       if (!tabw(LineNo) || FieldNbr>=tabw(LineNo)->ListnumCount) goto END;
       pListnum=tabw(LineNo)->pListnum;
       ListOrId=pListnum[FieldNbr].ls;
       level=pListnum[FieldNbr].lvl;
    }
    else {
       if (!(PfmtId[ParaId].flags&BULLET)) goto END;
       BltId=PfmtId[ParaId].BltId;
       ListOrId=TerBlt[BltId].ls;
       level=TerBlt[BltId].lvl;
    }


    if ((pLevel=GetListLevelPtr(w,ListOrId,level))==NULL) goto END;  // get the level pointer
    
    if (pLevel->NumType==LIST_BLT) {        // check if bullet
       wsprintf(text,"%c",pLevel->text[1]);
       j=lstrlen(text);
       goto GOT_LIST_TEXT;
    }
    else if (FALSE && pLevel->NumType==LIST_NO_NUM) {   // no number
       text[0]=0;
       j=0;
       goto END;
    } 

    MinWidth=UsePrtUnits?TwipsToPrtX(pLevel->MinIndent):TwipsToScrX(pLevel->MinIndent);
    
    pTemplate=pLevel->text;
    len=pTemplate[0];         // length of the template
    if (len>=50) goto END;

    MinWidth=UsePrtUnits?TwipsToPrtX(pLevel->MinIndent):TwipsToScrX(pLevel->MinIndent);
    
    pTemplate=&(pTemplate[1]); // goto past the length byte

    j=0;                      // index into the string text being formed
    for (i=0;i<len;i++) {
       
       chr=pTemplate[i];
       if (chr>9) {           // regular character
          text[j]=pTemplate[i];
          j++;
          continue;
       }
       
       // get the para number for the current level
       CurLevel=pTemplate[i];
       nbr=GetNumberForLevel(w,LineNo,ListOrId,CurLevel,level,FieldNbr);
       if (level==CurLevel) ListNbr=nbr;  // keep track of number at the top level

       pCurLevel=GetListLevelPtr(w,ListOrId,CurLevel);   // add the starting number
       
       // format the number and append it to the text string
       NumType=pCurLevel->NumType;
       if (CurLevel<level && pLevel->flags&LISTLEVEL_LEGAL && NumType!=LIST_NO_NUM) NumType=LIST_DEC;

       if      (NumType==LIST_NO_NUM)    string[0]=0;
       else if (NumType==LIST_UPR_ALPHA) AlphaFormat(string,(int)nbr,TRUE);
       else if (NumType==LIST_LWR_ALPHA) AlphaFormat(string,(int)nbr,FALSE);
       else if (NumType==LIST_UPR_ROMAN) romanize(string,(int)nbr,TRUE);
       else if (NumType==LIST_LWR_ROMAN) romanize(string,(int)nbr,FALSE);
       else if (NumType==LIST_CARDINAL)  OrdinalString(string,(int)nbr,false,(TerFont[pCurLevel->FontId].style&(CAPS|SCAPS)));
       else if (NumType==LIST_ORDINAL_TEXT)  OrdinalString(string,(int)nbr,true,(TerFont[pCurLevel->FontId].style&(CAPS|SCAPS)));
       else if (NumType==LIST_DEC_PAD)   wsprintf(string,"%02ld",nbr);
       else                              wsprintf(string,"%ld",nbr);
       
       IsRoman=(NumType==LIST_UPR_ROMAN || NumType==LIST_LWR_ROMAN);

       text[j]=0;
       lstrcat(text,string); 

       j=lstrlen(text);
    }
    
    GOT_LIST_TEXT:

    text[j]=0;
    rTrim(text);      // remove any exact right spaces

    if (pLevel->CharAft==LISTAFT_SPACE) strcat(text," ");  // append the space character
    
    // get the list font
    FontId=pLevel->FontId;
    
    lstrcpy(typeface,TerFont[CurParaFont].TypeFace);     // default fonts
    TwipsSize=TerFont[CurParaFont].TwipsSize;
    style=TerFont[CurParaFont].style;
    TextColor=TerFont[CurParaFont].TextColor;
    TextBkColor=TerFont[CurParaFont].TextBkColor;

    // override with list font
    override=0;
    if (lstrcmpi(TerFont[FontId].TypeFace,TerFont[0].TypeFace)!=0) {
        lstrcpy(typeface,TerFont[FontId].TypeFace);
        override++;
    }
    if (TerFont[FontId].TwipsSize!=TerFont[0].TwipsSize) {
        TwipsSize=TerFont[FontId].TwipsSize;
        override++;
    }
    if (TerFont[FontId].style || pLevel->FontStylesOff) {
        style=TerFont[FontId].style;
        override++;
    }
    if (TerFont[FontId].TextColor!=TerFont[0].TextColor && TerFont[FontId].TextColor!=CLR_AUTO) {
        TextColor=TerFont[FontId].TextColor;
        override++;
    }
    if (TerFont[FontId].TextBkColor!=TerFont[0].TextBkColor && TerFont[FontId].TextBkColor!=TextDefBkColor) {
        TextBkColor=TerFont[FontId].TextBkColor;
        override++;
    }

    if (override==0)  FontId=CurParaFont;
    else if (override<5) {     // 5 items compared, some matched some did not - create a custom font
       int ParaFID=fid(LineNo);
       FontId=GetNewFont(w,hTerDC,FontId,typeface,TwipsSize,style,TextColor,TextBkColor,CLR_AUTO,0,0,0,1,0,0,0,0,NULL,0,0,TerFont[FontId].CharSet/*DEFAULT_CHARSET*/,0,ParaFrame[ParaFID].TextAngle);
    }   


    // get the width of the number string
    hDC=(UsePrtUnits?hPr:hTerDC);
    hOldFont=SelectObject(hDC,(UsePrtUnits?PrtFont[FontId].hFont:TerFont[FontId].hFont));
    GetTextExtentPoint(hDC,text,lstrlen(text),&size);
    
    PixWidth=size.cx;

    SelectObject(hDC,hOldFont);
    
    // add space for any tab character following the number
    if (pLevel->CharAft==LISTAFT_TAB) {
       if (IsRoman) PixWidth=0;   // roman letters print before the tab, so text does not take additional space
     
       NbrX=PfmtId[ParaId].LeftIndentTwips+PfmtId[ParaId].FirstIndentTwips;   // x where the number prints

       if (UsePrtUnits) NbrX=TwipsToPrtX(NbrX);
       else             NbrX=TwipsToScrX(NbrX);
    
       TabX=NbrX+PixWidth;                     // tab begin x

       // find the next tab stop after this location (TabX)
       TabId=PfmtId[ParaId].TabId;
       GetTabPos(w,ParaId,&(TerTab[TabId]),TabX,&TabPos,&TabType,NULL,!UsePrtUnits);   // base it on screen position
       CurTabWidth=TabPos-TabX;
       if (CurTabWidth<0) CurTabWidth=0;

       PixWidth+=CurTabWidth;
    }

    if (PixWidth<MinWidth) PixWidth=MinWidth;

    END:

    // pass values
    if (pListTextWidth) (*pListTextWidth)=PixWidth;
    if (ListText)        lstrcpy(ListText,text);
    if (pFontId)        (*pFontId)=FontId;
    if (pListNbr)       (*pListNbr)=ListNbr;

    return TRUE;
}
 
/******************************************************************************
     GetNumberForLevel:
     Get the paragraph number for a given level
******************************************************************************/
GetNumberForLevel(PTERWND w, long LineNo, int InitListOrId, int level,int LineLevel, int InitFieldNbr)
{
    int i,j,nbr=0,blt,CurListOrId,CurList,LineList,CurLevel=0,BltId,ListOrId=-1,FieldCount;
    int StartAt=1,CurStartAt,InitNbr,ParaFID;
    long l;
    BOOL CurRestart,CanReset,SectBreak,SectResetPending;
    struct StrListnum far *pListnum;
    struct StrListLevel far *pInitLevel;


    LineList=ListOr[InitListOrId].ListIdx;
    if (level<list[LineList].LevelCount) StartAt=list[LineList].level[level].start;
    InitNbr=nbr=StartAt-1;
    
    pInitLevel=GetListLevelPtr(w,InitListOrId,level);    // add the starting number
    CanReset=!(pInitLevel->flags&LISTLEVEL_NO_RESET);  // is level reset when a lower level (parent level) is encountered


    // check if we can get the list number from the previous paragraph
    if (InitFieldNbr<0) {
      for (l=LineNo-1;l>=0;l--) {
         if (LineInfo(w,l,(INFO_ROW|INFO_PAGE|INFO_COL|INFO_SECT))) break;   // no short-cut
         if (LineFlags(l)&LFLAG_LISTNUM) break;

         if (False(LineFlags(l)&LFLAG_PARA_FIRST)) continue;
         if (!(LineFlags(l)&LFLAG_LIST)) break;    // numbering suppressed

         BltId=PfmtId[pfmt(l)].BltId;
         CurListOrId=TerBlt[BltId].ls;
         if (ListOr[CurListOrId].ListIdx!=LineList) break;  // different list

         CurLevel=TerBlt[BltId].lvl;
         if (CurListOrId!=InitListOrId) {
            break;
            //if (level<ListOr[InitListOrId].LevelCount 
            //  && ListOr[InitListOrId].level[level].flags&LISTLEVEL_RESTART) break; // not a default override
         }
         if (CurLevel<level) break;
         if (CurLevel>level) continue;
         if (!tabw(l)) break;
         if (level<LineLevel) return (int)(tabw(l)->ListNbr);
         else                 return (int)(tabw(l)->ListNbr+1);  // return the next sequential number
      } 
    }

    // reset the LISTLEVEL_RESTARTED flag
    for (i=0;i<TotalListOr;i++) {
       for (j=0;j<ListOr[i].LevelCount;j++) ResetLongFlag(ListOr[i].level[j].flags,LISTLEVEL_RESTARTED);
    } 
    

    // calculate the number
    SectBreak=FALSE;
    for (l=0;l<=LineNo;l++) {
       if (LineInfo(w,l,(INFO_ROW|INFO_PAGE|INFO_COL))) continue;

       ParaFID=fid(l);
       if (ParaFID && ParaFrame[ParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT)) continue;

       // restart at new section?
       //if (list[LineList].flags&LISTFLAG_RESTART_SEC && LineInfo(w,l,INFO_SECT)) nbr=StartAt-1;
       if (list[LineList].flags&LISTFLAG_RESTART_SEC && LineInfo(w,l,INFO_SECT)) SectBreak=TRUE;

       // check the number of listnum fields on this line
       FieldCount=0;
       if (l==LineNo) FieldCount=InitFieldNbr+1;
       else if (tabw(l)) FieldCount=tabw(l)->ListnumCount;
       
       if (FieldCount<0) FieldCount=0;
       
       for (blt=-1;blt<FieldCount;blt++) {        // -1 for the paragraph list, 
          
          if (blt==-1) {                          // paragraph numbering
             //if (!(LineFlags(l)&LFLAG_LIST)) continue;
             BltId=PfmtId[pfmt(l)].BltId;
             CurListOrId=TerBlt[BltId].ls;
             CurLevel=TerBlt[BltId].lvl;
             
             if (False(LineFlags(l)&LFLAG_PARA_FIRST)) continue;
          }
          else {                                  // listnum numbering
             if (!tabw(l) || blt>=tabw(l)->ListnumCount) continue;
             pListnum=tabw(l)->pListnum;
             CurListOrId=pListnum[blt].ls;
             CurLevel=pListnum[blt].lvl;
             BltId=-1;
          } 
          
          CurList=ListOr[CurListOrId].ListIdx;
       

          if (CurListOrId==0) continue;         // non-list paragraph

          SectResetPending=SectBreak;           // reset due to section break is applicable to the next immediate list/level only
          SectBreak=FALSE;

          if (CurList!=LineList) continue;      // different list
          if (BltId>=0 && TerBlt[BltId].flags&BLTFLAG_HIDDEN) continue;
          if (BltId>=0 && TerBlt[BltId].IsBullet) continue;  //break;
       
          if (SectResetPending && CurLevel==level) nbr=StartAt-1;

          if (CurLevel>level) {
             if (nbr==(StartAt-1)) nbr++;    // list starts out with a height level
             continue;
          }


          if (CurListOrId!=ListOrId) {       // encountered a different override
             CurRestart=(level<ListOr[CurListOrId].LevelCount 
                       && (ListOr[CurListOrId].level[level].flags&LISTLEVEL_RESTART)  // liststartat override
                       && (!(ListOr[CurListOrId].level[level].flags&LISTLEVEL_RESTARTED)));  // already restarted
             if (CurRestart) {               // an override encuontered
                CurStartAt=ListOr[CurListOrId].level[level].start;
                StartAt=CurStartAt;
                nbr=StartAt-1;

                ListOr[CurListOrId].level[level].flags|=LISTLEVEL_RESTARTED;
                ListOrId=CurListOrId;
             }
          }

          if (CurLevel<level) {       // new parent encountered
             if (ListOr[InitListOrId].LevelCount==0) nbr=InitNbr;   // list-overrides has no levels so it is not affected by any preceeding list-overrides
             else if (CanReset) nbr=StartAt-1;           // reset the numbering
             continue;
          }

          nbr++;
       }
    }

    return nbr;
}

/******************************************************************************
     GetListLevelPtr:
     Get the level pointer for the given list overide id and level
******************************************************************************/
struct StrListLevel far *GetListLevelPtr(PTERWND w, int ListOrId, int level)
{
    int ListIdx;
    struct StrListLevel far *pLevel=NULL;
    struct StrListOr far *pListOr;
    struct StrList far *pList;

    if (ListOrId==0 || ListOrId>=TotalListOr) return pLevel;

    // get the list level structure pointer
    pListOr=&(ListOr[ListOrId]);           // list override
    
    ListIdx=pListOr->ListIdx;
    if (ListIdx<=0 || ListIdx>=TotalLists) return pLevel;
    pList=&(list[ListIdx]);     // corresponding list

    if (pListOr->LevelCount==0 || level>=pListOr->LevelCount) {        // no override
       if (level>=pList->LevelCount) {
          if (pList->LevelCount<=0) return pLevel;
          else level=pList->LevelCount-1;
       }
       pLevel=&(pList->level[level]);
    }
    else {
       if (level>=pListOr->LevelCount) return pLevel;
       pLevel=&(pListOr->level[level]);
    }  

    return pLevel;
}

/******************************************************************************
    DecodeListText:
    Decode list text to a readable form
******************************************************************************/
LPBYTE DecodeListText(PTERWND w,LPBYTE in, LPBYTE out)
{
    int i,j,len;

    j=0;
    len=in[0];
    for (i=1;i<=len;i++,j++) {
      if (in[i]<=8) {     // level indicator
         out[j]='~';j++;
         out[j]=in[i]+'1';j++;
         out[j]='~';
      }
      else out[j]=in[i]; 
    } 
    out[j]=0;
 
    return out;
}
  
/******************************************************************************
    CodeListText:
    Ccode list text to a readable form
******************************************************************************/
LPBYTE CodeListText(PTERWND w,LPBYTE in, LPBYTE out)
{
    int i,j,len,lvl;

    len=lstrlen(in);

    j=1;                 // first byte saves the length
    for (i=0;i<len;i++,j++) {
       if (in[i]=='~' && (i+2)<len && in[i+2]=='~') {
         lvl=in[i+1]-'1';
         if (lvl<0) lvl=0;
         if (lvl>8) lvl=8;
         out[j]=lvl;
         i+=2; 
       }
       else out[j]=in[i]; 
    } 
    out[j]=0;
    out[0]=j-1;        // length gets store in the first byte

    return out;
}
  
/******************************************************************************
    TerGetParaAux1Id:
    Get the value of the aux1 id for a given para id
******************************************************************************/
DWORD WINAPI _export TerGetParaAux1Id(HWND hWnd,int CurPara)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurPara<0 || CurPara>=TotalPfmts) return FALSE;

    return PfmtId[CurPara].Aux1Id;
}

/******************************************************************************
    TerSetNextParaAux1Id:
    Set the next aux1 id to be used by the TerCreatePara function
******************************************************************************/
BOOL WINAPI _export TerSetNextParaAux1Id(HWND hWnd,DWORD Aux1Id)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    NextParaAux1Id=Aux1Id;
    return TRUE;
}

/******************************************************************************
    SetTerParaFmt:
    This routine toggle on/off certain paragraph attributes.
    Such attributes include paragraph centering, right justify.
    The paragraph centering and right justify is mutually exclusive.
******************************************************************************/
BOOL WINAPI _export SetTerParaFmt(HWND hWnd,UINT FmtType,BOOL OnOff, BOOL repaint)
{
    long l,FirstLine,LastLine;
    unsigned flags;
    PTERWND w;
    int LeftIndentTwips,RightIndentTwips,FirstIndentTwips,indent;
    int SpaceBefore,SpaceAfter,SpaceBetween,ParaFrameId,LineSpacing;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // get the range of lines to modify
    GetParaRange(w,&FirstLine,&LastLine);     // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {     // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       if (EditingParaStyle) {
          LeftIndentTwips=StyleId[CurSID].LeftIndentTwips;
          RightIndentTwips=StyleId[CurSID].RightIndentTwips;
          FirstIndentTwips=StyleId[CurSID].FirstIndentTwips;
          SpaceBefore=StyleId[CurSID].SpaceBefore;
          SpaceAfter=StyleId[CurSID].SpaceAfter;
          SpaceBetween=StyleId[CurSID].SpaceBetween;
          LineSpacing=StyleId[CurSID].LineSpacing;
          ParaFrameId=0;
          flags=StyleId[CurSID].ParaFlags;
       }
       else {
          LeftIndentTwips=PfmtId[pfmt(l)].LeftIndentTwips;
          RightIndentTwips=PfmtId[pfmt(l)].RightIndentTwips;
          FirstIndentTwips=PfmtId[pfmt(l)].FirstIndentTwips;
          SpaceBefore=PfmtId[pfmt(l)].SpaceBefore;
          SpaceAfter=PfmtId[pfmt(l)].SpaceAfter;
          SpaceBetween=PfmtId[pfmt(l)].SpaceBetween;
          LineSpacing=PfmtId[pfmt(l)].LineSpacing;
          ParaFrameId=fid(l);
          flags=PfmtId[pfmt(l)].flags;
       }

       if (OnOff) flags=flags|FmtType;      // toggle on
       else       flags=flags&(~FmtType);   // toggle off

       // set justification flags
       if (FmtType&LEFT)          flags=flags&(~((UINT)(CENTER|RIGHT_JUSTIFY|JUSTIFY)));
       if (FmtType&CENTER)        flags=flags&(~((UINT)(LEFT|RIGHT_JUSTIFY|JUSTIFY)));
       if (FmtType&RIGHT_JUSTIFY) flags=flags&(~((UINT)(LEFT|CENTER|JUSTIFY)));
       if (FmtType&JUSTIFY)       flags=flags&(~((UINT)(LEFT|CENTER|RIGHT_JUSTIFY)));

       // make page header and footer mutually exclusive
       if (FmtType&PAGE_HDR)    flags=flags&(~(UINT)PAGE_FTR);
       if (FmtType&PAGE_FTR)    flags=flags&(~(UINT)PAGE_HDR);
       if (CurSID<0 && tabw(l) && tabw(l)->type&(INFO_TABLE|INFO_SECT|INFO_PAGE|INFO_COL))
           flags=flags&(~((UINT)(PAGE_HDR|PAGE_FTR)));


       // Create/remove left indentation when left border specified
       if (FmtType&PARA_BOX_LEFT) {
          indent=LeftIndentTwips;          // find current para box indent
          if (FirstIndentTwips<0) indent+=FirstIndentTwips;  // add adjustment to the first line
          if (OnOff && indent<RULER_TOLERANCE) LeftIndentTwips+=(RULER_TOLERANCE-indent);
          else if (!OnOff && LeftIndentTwips==RULER_TOLERANCE) LeftIndentTwips-=RULER_TOLERANCE;
          if (LeftIndentTwips<0) LeftIndentTwips=0;
       }

       // Create/remove right indentation when right border specified
       if (FmtType&PARA_BOX_RIGHT) {
          if (OnOff && RightIndentTwips<RULER_TOLERANCE) RightIndentTwips=RULER_TOLERANCE;
          else if (!OnOff && RightIndentTwips==RULER_TOLERANCE) RightIndentTwips-=RULER_TOLERANCE;
          if (RightIndentTwips<0) RightIndentTwips=0;
       }

       // turnoff space between lines if DOUBLE_SPACE specified
       if (flags&DOUBLE_SPACE) SpaceBetween=LineSpacing=0;

       // Create/remove top space for top para border
       if (FmtType&PARA_BOX_TOP && ParaFrameId==0) {
          if      (OnOff && SpaceBefore<PARA_BOX_SPACE) SpaceBefore=PARA_BOX_SPACE;
          else if (!OnOff) SpaceBefore-=PARA_BOX_SPACE;
          if (SpaceBefore<0) SpaceBefore=0;
       }

       // Create/remove top space for top para border
       if (FmtType&PARA_BOX_BOT && ParaFrameId==0) {
          if      (OnOff && SpaceAfter<PARA_BOX_SPACE) SpaceAfter=PARA_BOX_SPACE;
          else if (!OnOff && SpaceAfter>=PARA_BOX_SPACE) SpaceAfter-=PARA_BOX_SPACE;
          if (SpaceAfter<0) SpaceAfter=0;
       }

       // update stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].LeftIndentTwips=LeftIndentTwips;
          StyleId[CurSID].RightIndentTwips=RightIndentTwips;
          StyleId[CurSID].FirstIndentTwips=FirstIndentTwips;
          StyleId[CurSID].SpaceBefore=SpaceBefore;
          StyleId[CurSID].SpaceAfter=SpaceAfter;
          StyleId[CurSID].SpaceBetween=SpaceBetween;
          StyleId[CurSID].LineSpacing=LineSpacing;
          StyleId[CurSID].ParaFlags=flags;
          return TRUE;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         LeftIndentTwips,
                         RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         SpaceBefore,
                         SpaceAfter,
                         SpaceBetween,
                         LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         flags);
    }

    TerArg.modified++;
    if (TerArg.PrintView && FmtType&(PAGE_HDR|PAGE_FTR) && TotalLines<SMALL_FILE)
          Repaginate(w,FALSE,FALSE,0,TRUE);  // repaginate now
    else  RepageBeginLine=FirstLine;

    if (repaint) {
      if (!OnOff && FmtType&(PARA_BOX|PARA_BOX_THICK|PARA_BOX_DOUBLE)) PaintFlag=PAINT_WIN_RESET;
      PaintTer(w);
    }
    else  WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;

}

/******************************************************************************
    SetParaParam:
    Set the new value for the specified paragraph parameter specified 
******************************************************************************/
int SetParaParam(PTERWND w, int CurPara, int type, int val)
{
    return NewParaId(w,CurPara,
             (type!=PPARAM_LEFT_INDENT)?PfmtId[CurPara].LeftIndentTwips:val,
             (type!=PPARAM_RIGHT_INDENT)?PfmtId[CurPara].RightIndentTwips:val,
             (type!=PPARAM_FIRST_INDENT)?PfmtId[CurPara].FirstIndentTwips:val,
             (type!=PPARAM_TAB)?PfmtId[CurPara].TabId:val,
             (type!=PPARAM_BLT)?PfmtId[CurPara].BltId:val,
             (type!=PPARAM_AUX)?PfmtId[CurPara].AuxId:val,
             (type!=PPARAM_AUX1)?PfmtId[CurPara].Aux1Id:val,
             (type!=PPARAM_STY)?PfmtId[CurPara].StyId:val,
             (type!=PPARAM_SHADING)?PfmtId[CurPara].shading:val,
             (type!=PPARAM_PFLAGS)?PfmtId[CurPara].pflags:(UINT)val,
             (type!=PPARAM_SPACE_BEF)?PfmtId[CurPara].SpaceBefore:val,
             (type!=PPARAM_SPACE_AFT)?PfmtId[CurPara].SpaceAfter:val,
             (type!=PPARAM_SPACE_BET)?PfmtId[CurPara].SpaceBetween:val,
             (type!=PPARAM_LINE_SPACING)?PfmtId[CurPara].LineSpacing:val,
             (type!=PPARAM_BK_COLOR)?PfmtId[CurPara].BkColor:(COLORREF)val,
             (type!=PPARAM_BORDER_SPACE)?PfmtId[CurPara].BorderSpace:val,
             (type!=PPARAM_FLOW)?PfmtId[CurPara].flow:val,
             (type!=PPARAM_FLAGS)?PfmtId[CurPara].flags:(UINT)val);
} 


/******************************************************************************
    ParaLeftIndent:
    Indent or unindent the highlighted or the current paragraph.
    The indentation is created from the left margin.
******************************************************************************/
BOOL WINAPI _export ParaLeftIndent(HWND hWnd,BOOL indent,BOOL repaint)
{
    long l,FirstLine,LastLine;
    int  i,TabId,count,LeftIndentTwips,FirstIndentTwips,CurStop,PrevFID=0,PrevRowId=0;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    GetParaRange(w,&FirstLine,&LastLine);   // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       if (EditingParaStyle) {
          LeftIndentTwips=StyleId[CurSID].LeftIndentTwips;
          FirstIndentTwips=StyleId[CurSID].FirstIndentTwips;
          TabId=StyleId[CurSID].TabId;
       }
       else {
          LeftIndentTwips=PfmtId[pfmt(l)].LeftIndentTwips;
          FirstIndentTwips=PfmtId[pfmt(l)].FirstIndentTwips;
          TabId=PfmtId[pfmt(l)].TabId;

          if (TerFlags2&TFLAG2_INDENT_FRAMES && fid(l)) {
             LeftIndentTwips=ParaFrame[fid(l)].x;
             TabId=0;
          }
          if (TerFlags2&TFLAG2_INDENT_TABLES && cid(l)) {
             int row=cell[cid(l)].row;
             LeftIndentTwips=TableRow[row].indent;
             TabId=0;
          }
       }

       count=TerTab[TabId].count;         // number of tab stops

       if (indent) {                      // calculate the new start col
          for (i=0;i<count;i++) {         // find the next tab stop
             if (TerTab[TabId].pos[i]>LeftIndentTwips) {
                LeftIndentTwips=TerTab[TabId].pos[i];
                break;
             }
          }
          if (i==count) {                 // use default stops
             CurStop=LeftIndentTwips/DefTabWidth;       // current stop
             LeftIndentTwips=(CurStop+1)*DefTabWidth;  // next stop
          }
       }
       else {
          BOOL AfterLastTab=false;
          if (count>0 && LeftIndentTwips>TerTab[TabId].pos[count-1]) AfterLastTab=true;

          if (AfterLastTab) {  // tab back using default tabs
             CurStop=LeftIndentTwips/DefTabWidth;       // current stop
             if (LeftIndentTwips==(CurStop*DefTabWidth)) CurStop--;
             LeftIndentTwips=CurStop*DefTabWidth;      // next stop
             
             if (count>0 && LeftIndentTwips<TerTab[TabId].pos[count-1]) LeftIndentTwips=TerTab[TabId].pos[count-1]; // position at the last tab stop
          } 
          else {  // now move back on tab stops
             for (i=count-1;i>=0;i--) {         // find the previous tab stop
                if (TerTab[TabId].pos[i]<LeftIndentTwips) {
                   LeftIndentTwips=TerTab[TabId].pos[i];
                   break;
                }
             }
             if (i<0) {
                CurStop=LeftIndentTwips/DefTabWidth;
                if (CurStop>0) LeftIndentTwips=(CurStop-1)*DefTabWidth;
                else           LeftIndentTwips=0;
                if (LeftIndentTwips<0) LeftIndentTwips=0;
             }
          }
       }

       // check if the frame needs to be indented
       if (TerFlags2&TFLAG2_INDENT_FRAMES && !EditingParaStyle && fid(l)>0) {
          if (fid(l)!=PrevFID) {
            int ParaFID=fid(l);
            int PrevX=ParaFrame[ParaFID].x;
            ParaFrame[ParaFID].x=LeftIndentTwips;
            if (PrevX>=0 && ParaFrame[ParaFID].x<0) ParaFrame[ParaFID].x=0;
            PrevFID=ParaFID;
          }
          continue;
       }

       // check if the table needs to be indented
       if (TerFlags2&TFLAG2_INDENT_TABLES && !EditingParaStyle && cid(l)>0) {
          int row=cell[cid(l)].row;
          if (row!=PrevRowId) {
            int PrevX=TableRow[row].indent,cl,x;
            TableRow[row].indent=LeftIndentTwips;
            if (PrevX>=0 && TableRow[row].indent<0) TableRow[row].indent=0;
            // apply the new x to all cells in the row
            cl=TableRow[row].FirstCell;
            x=TableRow[row].indent;
            while (cl>0) {
               cell[cl].x=x;
               x+=cell[cl].width;
               cl=cell[cl].NextCell;
            }
            PrevRowId=row;
          }
          continue;
       }

       if (LeftIndentTwips>MAX_INDENT) LeftIndentTwips=MAX_INDENT; //maximum indentaion
       if (LeftIndentTwips<0) LeftIndentTwips=0;

       if ((LeftIndentTwips+FirstIndentTwips)<0) LeftIndentTwips=-FirstIndentTwips;

       // update stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].LeftIndentTwips=LeftIndentTwips;
          DrawRuler(w,FALSE);
          return TRUE;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;
    if (!EditingParaStyle && FirstLine<RepageBeginLine) RepageBeginLine=FirstLine;

    if (repaint) PaintTer(w);
    else         WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    ParaRightIndent:
    Indent or unindent the highlighted or the current paragraph.
    The indentation is created from the right margin.
******************************************************************************/
BOOL WINAPI _export ParaRightIndent(HWND hWnd,BOOL indent,BOOL repaint)
{
    long l,FirstLine,LastLine;
    int  RightIndentTwips;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    GetParaRange(w,&FirstLine,&LastLine);   // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       if (EditingParaStyle) RightIndentTwips=StyleId[CurSID].RightIndentTwips;
       else                  RightIndentTwips=PfmtId[pfmt(l)].RightIndentTwips;

       if (indent) {                      // calculate the new start col
          if ((RightIndentTwips+EACH_INDENT)<MAX_INDENT)
                   RightIndentTwips+=EACH_INDENT;
          else
                   RightIndentTwips=MAX_INDENT; //maximum indentaion
       }
       else {
         if ((RightIndentTwips-EACH_INDENT)>=0) RightIndentTwips-=EACH_INDENT;
         else                                   RightIndentTwips=0;
       }

       // Update stylesheet if active
       if (EditingParaStyle) {
            StyleId[CurSID].RightIndentTwips=RightIndentTwips;
            DrawRuler(w,FALSE);
            return TRUE;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;
    if (!EditingParaStyle && FirstLine<RepageBeginLine) RepageBeginLine=FirstLine;

    if (repaint) PaintTer(w);
    else         WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    ParaHangingIndent:
    Create hanging indent;
******************************************************************************/
BOOL WINAPI _export ParaHangingIndent(HWND hWnd,BOOL indent,BOOL repaint)
{
    long l,FirstLine,LastLine;
    int  i,CurStop,TabId,count,LeftIndentTwips,FirstIndentTwips,OrigLeftIndentTwips;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    GetParaRange(w,&FirstLine,&LastLine);   // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // extract the current values
       if (EditingParaStyle) {
          LeftIndentTwips=StyleId[CurSID].LeftIndentTwips;
          FirstIndentTwips=StyleId[CurSID].FirstIndentTwips;
          TabId=StyleId[CurSID].TabId;
       }
       else {
          LeftIndentTwips=PfmtId[pfmt(l)].LeftIndentTwips;
          FirstIndentTwips=PfmtId[pfmt(l)].FirstIndentTwips;
          TabId=PfmtId[pfmt(l)].TabId;
       }

       OrigLeftIndentTwips=LeftIndentTwips;  // save unmodified value
       count=TerTab[TabId].count;         // number of tab stops

       if (indent) {                      // calculate the new start col
          for (i=0;i<count;i++) {         // find the next tab stop
             if (TerTab[TabId].pos[i]>LeftIndentTwips) {
                LeftIndentTwips=TerTab[TabId].pos[i];
                break;
             }
          }
          if (i==count) {                 // use default stops
             CurStop=LeftIndentTwips/DefTabWidth;       // current stop
             LeftIndentTwips=(CurStop+1)*DefTabWidth;  // next stop
          }
       }
       else {
          for (i=count-1;i>=0;i--) {         // find the previous tab stop
             if (TerTab[TabId].pos[i]<LeftIndentTwips) {
                LeftIndentTwips=TerTab[TabId].pos[i];
                break;
             }
          }
          if (i<0) {                        // use default stops
             CurStop=LeftIndentTwips/DefTabWidth;       // current stop
             if (LeftIndentTwips==(CurStop*DefTabWidth)) CurStop--;
             LeftIndentTwips=CurStop*DefTabWidth;      // next stop
          }
       }
       if (LeftIndentTwips>MAX_INDENT) LeftIndentTwips=MAX_INDENT; //maximum indentaion
       if (LeftIndentTwips<0) LeftIndentTwips=0;

       // create offset for the first line
       FirstIndentTwips=FirstIndentTwips-(LeftIndentTwips-OrigLeftIndentTwips);

       // update the stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].LeftIndentTwips=LeftIndentTwips;
          StyleId[CurSID].FirstIndentTwips=FirstIndentTwips;
          DrawRuler(w,FALSE);
          return TRUE;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;
    if (!EditingParaStyle && FirstLine<RepageBeginLine) RepageBeginLine=FirstLine;

    if (repaint) PaintTer(w);
    else         WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    TerSetParaIndent:
    Set the paragraph indentation.  Set the indent value to -1 to leave it unchanged.
******************************************************************************/
BOOL WINAPI _export TerSetParaIndent(HWND hWnd,int left, int right, int first, BOOL repaint)
{
    long l,FirstLine,LastLine;
    int  SnapDist,LeftIndentTwips,RightIndentTwips,FirstIndentTwips,PrevFID=0,PrevRowId=0;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    GetParaRange(w,&FirstLine,&LastLine);   // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');


    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // get the modified values
       LeftIndentTwips=left;
       RightIndentTwips=right;
       FirstIndentTwips=first;

       // snap to invisible ruler grid
       if (SnapToGrid) {
          if (TerFlags&TFLAG_METRIC) SnapDist=SNAP_DIST_METRIC;
          else                       SnapDist=SNAP_DIST;
          LeftIndentTwips=RoundInt(LeftIndentTwips,SnapDist);
          RightIndentTwips=RoundInt(RightIndentTwips,SnapDist);
          FirstIndentTwips=RoundInt(FirstIndentTwips,SnapDist);
       }

       // update the stylesheet if active
       if (EditingParaStyle) {
          if (left!=-1)  StyleId[CurSID].LeftIndentTwips=LeftIndentTwips;
          if (right!=-1) StyleId[CurSID].RightIndentTwips=RightIndentTwips;
          if (first!=-1) StyleId[CurSID].FirstIndentTwips=FirstIndentTwips;
          DrawRuler(w,FALSE);
          return TRUE;
       }

       // check if the frame needs to be indented
       if (TerFlags2&TFLAG2_INDENT_FRAMES && left!=0 && left!=-1 && fid(l)>0) {
          if (fid(l)!=PrevFID) {
            int ParaFID=fid(l);
            int PrevX=ParaFrame[ParaFID].x;
            ParaFrame[ParaFID].x=left;
            if (PrevX>=0 && ParaFrame[ParaFID].x<0) ParaFrame[ParaFID].x=0;
            PrevFID=ParaFID;
          }
          continue;
       }

       // check if the table needs to be indented
       if (TerFlags2&TFLAG2_INDENT_TABLES && left!=0 && left!=-1 && cid(l)>0) {
          int row=cell[cid(l)].row;
          if (row!=PrevRowId) {
            int PrevX=TableRow[row].indent,cl,x;
            TableRow[row].indent=left;
            if (PrevX>=0 && TableRow[row].indent<0) TableRow[row].indent=0;
            // apply the new x to all cells in the row
            cl=TableRow[row].FirstCell;
            x=TableRow[row].indent;
            while (cl>0) {
               cell[cl].x=x;
               x+=cell[cl].width;
               cl=cell[cl].NextCell;
            }
            PrevRowId=row;
          }
          continue;
       }


       pfmt(l)=NewParaId(w,pfmt(l),
                         (left==-1)?PfmtId[pfmt(l)].LeftIndentTwips:LeftIndentTwips,
                         (right==-1)?PfmtId[pfmt(l)].RightIndentTwips:RightIndentTwips,
                         (first==-1)?PfmtId[pfmt(l)].FirstIndentTwips:FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;
    if (!EditingParaStyle && FirstLine<RepageBeginLine) RepageBeginLine=FirstLine;

    if (repaint) PaintTer(w);
    else         WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}


/******************************************************************************
    ParaIndentTwips:
    Increment or decrement the paragraph indentation by the given number of
    twips.
******************************************************************************/
BOOL WINAPI _export ParaIndentTwips(HWND hWnd,int DeltaLeft, int DeltaRight, int DeltaFirst, BOOL repaint)
{
    long l,FirstLine,LastLine;
    int  SnapDist,LeftIndentTwips,RightIndentTwips,FirstIndentTwips,PrevFID=0,PrevRowId=0;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    GetParaRange(w,&FirstLine,&LastLine);   // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    // get the modified values
    if (EditingParaStyle) {
       LeftIndentTwips=StyleId[CurSID].LeftIndentTwips+DeltaLeft;
       RightIndentTwips=StyleId[CurSID].RightIndentTwips+DeltaRight;
       FirstIndentTwips=StyleId[CurSID].FirstIndentTwips+DeltaFirst;
    }
    else {
       LeftIndentTwips=PfmtId[pfmt(CurLine)].LeftIndentTwips+DeltaLeft;
       RightIndentTwips=PfmtId[pfmt(CurLine)].RightIndentTwips+DeltaRight;
       FirstIndentTwips=PfmtId[pfmt(CurLine)].FirstIndentTwips+DeltaFirst;
    }

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // Use line values when cur line is not in the range
       if (!EditingParaStyle /*&& (CurLine<FirstLine || CurLine>LastLine)*/) {
          LeftIndentTwips=PfmtId[pfmt(l)].LeftIndentTwips+DeltaLeft;
          RightIndentTwips=PfmtId[pfmt(l)].RightIndentTwips+DeltaRight;
          FirstIndentTwips=PfmtId[pfmt(l)].FirstIndentTwips+DeltaFirst;
       }

       if (LeftIndentTwips>MAX_INDENT) LeftIndentTwips=MAX_INDENT; //maximum indentaion
       if (LeftIndentTwips<0) {
          FirstIndentTwips+=LeftIndentTwips;
          LeftIndentTwips=0;
       }
       if (RightIndentTwips>MAX_INDENT) RightIndentTwips=MAX_INDENT; //maximum indentaion
       if (RightIndentTwips<0) RightIndentTwips=0;
       if (FirstIndentTwips>MAX_INDENT) FirstIndentTwips=MAX_INDENT; //maximum indentaion
       if ((LeftIndentTwips+FirstIndentTwips)<0) FirstIndentTwips=-LeftIndentTwips;


       // snap to invisible ruler grid
       if (SnapToGrid) {
          if (TerFlags&TFLAG_METRIC) SnapDist=SNAP_DIST_METRIC;
          else                       SnapDist=SNAP_DIST;
          LeftIndentTwips=RoundInt(LeftIndentTwips,SnapDist);
          RightIndentTwips=RoundInt(RightIndentTwips,SnapDist);
          FirstIndentTwips=RoundInt(FirstIndentTwips,SnapDist);
       }

       // update the stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].LeftIndentTwips=LeftIndentTwips;
          StyleId[CurSID].RightIndentTwips=RightIndentTwips;
          StyleId[CurSID].FirstIndentTwips=FirstIndentTwips;
          DrawRuler(w,FALSE);
          return TRUE;
       }

       // check if the frame needs to be indented
       if (TerFlags2&TFLAG2_INDENT_FRAMES && DeltaLeft!=0 && fid(l)>0) {
          if (fid(l)!=PrevFID) {
            int ParaFID=fid(l);
            int PrevX=ParaFrame[ParaFID].x;
            ParaFrame[ParaFID].x+=DeltaLeft;
            if (PrevX>=0 && ParaFrame[ParaFID].x<0) ParaFrame[ParaFID].x=0;
            PrevFID=ParaFID;
          }
          continue;
       }

       // check if the table needs to be indented
       if (TerFlags2&TFLAG2_INDENT_TABLES && DeltaLeft!=0 && cid(l)>0) {
          int row=cell[cid(l)].row;
          if (row!=PrevRowId) {
            int PrevX=TableRow[row].indent,cl,x;
            TableRow[row].indent+=DeltaLeft;
            if (PrevX>=0 && TableRow[row].indent<0) TableRow[row].indent=0;
            // apply the new x to all cells in the row
            cl=TableRow[row].FirstCell;
            x=TableRow[row].indent;
            while (cl>0) {
               cell[cl].x=x;
               x+=cell[cl].width;
               cl=cell[cl].NextCell;
            }
            PrevRowId=row;
          }
          continue;
       }


       pfmt(l)=NewParaId(w,pfmt(l),
                         LeftIndentTwips,
                         RightIndentTwips,
                         FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;
    if (!EditingParaStyle && FirstLine<RepageBeginLine) RepageBeginLine=FirstLine;

    if (repaint) PaintTer(w);
    else         WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    ParaNormal:
    Turn off all paragraph formats for the highlighted or the current paragraph
******************************************************************************/
BOOL WINAPI _export ParaNormal(HWND hWnd,BOOL repaint)
{
    long l,FirstLine,LastLine;
    PTERWND w;
    UINT flags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // update stylesheet if active
    if (EditingParaStyle) {
        StyleId[CurSID].LeftIndentTwips=0;     // left indent in twips
        StyleId[CurSID].RightIndentTwips=0;    // Right indent in twips
        StyleId[CurSID].FirstIndentTwips=0;    // First line indent in twips
        StyleId[CurSID].ParaFlags=0;           // includes such flags as double spacing,centering, right justify etc
        StyleId[CurSID].pflags=0;              // PFLAG_ constants
        StyleId[CurSID].shading=0;             // shading (0 to 10000)
        StyleId[CurSID].SpaceBefore=0;         // space before the paragraph in twips
        StyleId[CurSID].SpaceAfter=0;          // space after the paragraph in twips
        StyleId[CurSID].SpaceBetween=0;        // space between the paragraph lines in twips
        StyleId[CurSID].LineSpacing=0;         // variable line spacing (0 for single line spacing)
        StyleId[CurSID].TabId=0;               // tab id
        StyleId[CurSID].ParaBkColor=CLR_WHITE; // background color
        DrawRuler(w,FALSE);
        return TRUE;
    }


    GetParaRange(w,&FirstLine,&LastLine);   // get the range of the lines that would be affected
    SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       flags=PfmtId[pfmt(l)].flags;
       flags=flags&(~(UINT)(CENTER|RIGHT_JUSTIFY|JUSTIFY|DOUBLE_SPACE|PARA_KEEP|PARA_KEEP_NEXT|BULLET));
       flags|=LEFT;
       pfmt(l)=NewParaId(w,pfmt(l),0,0,0,0,0,
               PfmtId[pfmt(l)].AuxId,PfmtId[pfmt(l)].Aux1Id,0,
               0,0,0,0,0,0,CLR_WHITE,PARA_BOX_SPACE,FLOW_DEF,flags);
    }

    TerArg.modified++;
    if (!EditingParaStyle && FirstLine<RepageBeginLine) RepageBeginLine=FirstLine;

    if (repaint) PaintTer(w);
    else         WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}


/******************************************************************************
    SetTab:
    Set a new tab position.  If the position is less than zero, then this
    function shows a dialog box to access user values.
******************************************************************************/
BOOL WINAPI _export SetTab(HWND hWnd,int type,int pos,BOOL repaint)
{
    return TerSetTab(hWnd,type,pos,0,repaint);
}

/******************************************************************************
    TerSetTab:
    Set a new tab position.
******************************************************************************/
BOOL WINAPI _export TerSetTab(HWND hWnd,int type,int pos,BYTE flags,BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;
    int i,idx,OldTabId,TabId,delta=RULER_TOLERANCE/2;
    struct StrTab TabRec;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // accept user values
    if (pos<0) {           // accept the values from the user
       if (!CallDialogBox(w,"SetTabParam",SetTabParam,0L)) return FALSE;
       pos=DlgInt1;
       type=DlgInt2;
       flags=(BYTE)DlgInt3;
    }

    // validate input
    if (pos<0) pos=0;
    if (type!=TAB_LEFT && type!=TAB_RIGHT && type!=TAB_CENTER && type!=TAB_DECIMAL) type=TAB_LEFT;

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // get new tab id
       if (EditingParaStyle) OldTabId=StyleId[CurSID].TabId;
       else                  OldTabId=PfmtId[pfmt(l)].TabId;


       TabRec=TerTab[OldTabId];
       if (TabRec.count==TE_MAX_TAB_STOPS) {
          return PrintError(w,MSG_OUT_OF_TAB_POS,"Set Tab");
       }

       for (i=0;i<TabRec.count;i++) {
          if (abs(TabRec.pos[i]-pos)<=delta) break;   // match found
          if (TabRec.pos[i]>pos) break;  // create a new tab
       }
       if (i==TabRec.count) {
          idx=TabRec.count;
          TabRec.count++;
       }
       else {
          idx=i;
          if (abs(TabRec.pos[idx]-pos)>delta) { // make room for the new tab
            for (i=TabRec.count-1;i>=idx;i--) {
               TabRec.pos[i+1]=TabRec.pos[i];
               TabRec.type[i+1]=TabRec.type[i];
               TabRec.flags[i+1]=TabRec.flags[i];
            }
            TabRec.count++;
          }
       }
       TabRec.pos[idx]=pos;
       TabRec.type[idx]=type;
       TabRec.flags[idx]=flags;
       TabId=NewTabId(w,OldTabId,&TabRec);    // get new tab id

       // update the stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].TabId=TabId;
          DrawRuler(w,FALSE);
          return TRUE;
       }

       // get new paraid
       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;
    if (!EditingParaStyle && FirstLine<RepageBeginLine) RepageBeginLine=FirstLine;

    if (repaint) {
       PaintTer(w);
       if (RulerPending) DrawRuler(w,FALSE);
    }
    else WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    TerCreateTabId:
    This function creates tab id.
******************************************************************************/
int WINAPI _export TerCreateTabId(HWND hWnd,struct StrTab far *pTabRec)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (!TerArg.WordWrap) return -1;

    return NewTabId(w,0,pTabRec);
}

/******************************************************************************
    TerGetTabStop:
    Get the position and type of a tab stop.  This function returns the number
    of tab stops for the given line.  It returns -1 when an error occurs.
******************************************************************************/
int WINAPI _export TerGetTabStop(HWND hWnd,long line, int TabNo, LPINT pPos, LPINT pType, LPINT pFlag)
{
   return TerGetTabStop2(hWnd,PID_LINE,line,TabNo,pPos,pType,pFlag);
}

/******************************************************************************
    TerGetTabStop2:
    Get the position and type of a tab stop.  This function returns the number
    of tab stops for the given line.  It returns -1 when an error occurs.
******************************************************************************/
int WINAPI _export TerGetTabStop2(HWND hWnd,int type, long line, int TabNo, LPINT pPos, LPINT pType, LPINT pFlag)
{
    PTERWND w;
    int TabId;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (!TerArg.WordWrap) return -1;
    
    if (type==PID_LINE || line<0) {
       if (line<0) line=CurLine;
       if (line>=TotalLines) return -1;
       TabId=PfmtId[pfmt(line)].TabId;
    }
    else if (type==PID_PARA) {
       if ((int)line>TotalPfmts) return -1;
       TabId=PfmtId[(int)line].TabId;
    }
    else {
       if ((int)line>TotalTabs) return -1;
       TabId=(int)line;
    }  
    
    if (TabNo<0) return TerTab[TabId].count;   // return the tab count

    if (TabNo>=TerTab[TabId].count) return -1;

    if (pPos)   (*pPos)=TerTab[TabId].pos[TabNo];
    if (pType)  (*pType)=TerTab[TabId].type[TabNo];
    if (pFlag)  (*pFlag)=TerTab[TabId].flags[TabNo];

    return TerTab[TabId].count;
}

/*****************************************************************************
    TerParaBorder:
    This routine calls the dialog box to request paragraph border parameters
    and applies them to the selected paragraph.
*****************************************************************************/
TerParaBorder(PTERWND w)
{
    int shading;
    BOOL refresh;

    if (HilightType!=HILIGHT_OFF) NormalizeBlock(w);  // normalize the hilighted block

    if (!CallDialogBox(w,"ParaBoxParam",ParaBoxParam,0)) return FALSE;

    // extract the shading percentage
    if (TempString[0]!=0) {
       shading=atoi(TempString);
       if (shading<0) shading=0;
       if (shading>100) shading=100;
       shading=shading*100;          // range is 0 to 10000

       // apply the shading
       if (DlgOnFlags || DlgOffFlags) refresh=FALSE;  // wait for refreshing
       else                           refresh=TRUE;

       TerSetParaShading(hTerWnd,shading,refresh);
    }

    // apply changes
    if (!DlgOffFlags && DlgOnFlags) SetTerParaFmt(hTerWnd,DlgOnFlags,TRUE,false);
    else if (DlgOffFlags && !DlgOnFlags) SetTerParaFmt(hTerWnd,DlgOffFlags,FALSE,false);
    else if (DlgOffFlags && DlgOnFlags) {
       SetTerParaFmt(hTerWnd,DlgOnFlags,TRUE,FALSE);
       SetTerParaFmt(hTerWnd,DlgOffFlags,FALSE,false);
    }
    TerSetParaBorderColor(hTerWnd,DlgColor1,TRUE);

    return TRUE;
}

/*****************************************************************************
    TerParaSpacing:
    This routine calls the dialog box to request paragraph spacing parameters
    and applies them to the selected paragraph.
*****************************************************************************/
TerParaSpacing(PTERWND w)
{
    if (HilightType!=HILIGHT_OFF) NormalizeBlock(w);  // normalize the hilighted block

    if (!CallDialogBox(w,"ParaSpaceParam",ParaSpaceParam,0)) return FALSE;

    TerSetParaSpacing2(hTerWnd,PointsToTwips(DlgInt1),PointsToTwips(DlgInt2),PointsToTwips(DlgInt3),DlgInt4,TRUE);

    return TRUE;
}

/******************************************************************************
    ClearTab:
    Clear a specified tab position. The tab position is specified in
    twips.
******************************************************************************/
BOOL WINAPI _export ClearTab(HWND hWnd,int pos,BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;
    int i,idx,OldTabId,TabId;
    struct StrTab TabRec;
    BOOL OneCleared;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // validate input
    if (pos<0) pos=0;

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // get new tab id
       if (EditingParaStyle) {
          OldTabId=StyleId[CurSID].TabId;
          if (TerTab[OldTabId].count==0) break; // no tabs to reset
       }
       else {
          OldTabId=PfmtId[pfmt(l)].TabId;
          if (TerTab[OldTabId].count==0) continue; // no tabs to reset
       }

       TabRec=TerTab[OldTabId];

       // remove the tabs at this position
       OneCleared=FALSE;
       while (TabRec.count>0) {
          for (i=0;i<TabRec.count;i++) if (abs(TabRec.pos[i]-pos)<(SNAP_DIST/2)) break;
          if (i==TabRec.count) break;     // no more positions found to reset
          idx=i;
          // scroll over this tab position
          for (i=idx+1;i<TabRec.count;i++) {
             TabRec.pos[i-1]=TabRec.pos[i];
             TabRec.type[i-1]=TabRec.type[i];
             TabRec.flags[i-1]=TabRec.flags[i];
          }
          TabRec.count--;
          OneCleared=TRUE;            // atleast one tab position cleared
       }
       if (!OneCleared) continue;     // no tabs to clear

       TabId=NewTabId(w,OldTabId,&TabRec);// get new tab id

       // update the stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].TabId=TabId;
          DrawRuler(w,FALSE);
          return TRUE;
       }

       // get new paraid
       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;

    if (repaint) {
       PaintTer(w);
       if (RulerPending) DrawRuler(w,FALSE);
    }
    else WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    TerSetParaBkColor:
    Set the paragraph background color.  Set the 'dialog' parameter to TRUE
    to show a user dialog box.
******************************************************************************/
BOOL WINAPI _export TerSetParaBkColor(HWND hWnd,BOOL dialog,COLORREF color,BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    // get the color
    if (dialog) {
       if (EditingParaStyle) color=StyleId[CurSID].ParaBkColor;
       else                  color=PfmtId[pfmt(FirstLine)].BkColor;
       color=DlgEditColor(w,hTerWnd,color,FALSE);
       if (DlgCancel) return FALSE;
    }

    TerArg.modified++;

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // update the style sheet
       if (EditingParaStyle) {
          StyleId[CurSID].ParaBkColor=color;
          return TRUE;
       }


       // get new paraid
       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         color,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerSetParaBorderColor:
    Set the paragraph border color.
******************************************************************************/
BOOL WINAPI _export TerSetParaBorderColor(HWND hWnd,COLORREF color,BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;
    struct StrPfmt NewPfmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    TerArg.modified++;

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // update the style sheet
       if (EditingParaStyle) {
          StyleId[CurSID].ParaBorderColor=color;
          return TRUE;
       }


       // get new paraid
       FarMove(&(PfmtId[pfmt(l)]),&NewPfmt,sizeof(struct StrPfmt));
       NewPfmt.BorderColor=color;
       pfmt(l)=NewParaId2(w,pfmt(l),&NewPfmt);
    }

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    ClearAllTabs:
    Reset all tabs for the paragraph.
******************************************************************************/
BOOL WINAPI _export ClearAllTabs(HWND hWnd,BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // update the stylesheet if active
    if (EditingParaStyle) {
       StyleId[CurSID].TabId=0;
       DrawRuler(w,FALSE);
       return TRUE;
    }

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // get new paraid with default tab table
       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         0,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;

    if (repaint) {
       PaintTer(w);
       if (RulerPending) DrawRuler(w,FALSE);
    }
    else WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}


/******************************************************************************
    GetParaRange:
    Get the range of lines that would be affected by the current paragraph
    format operation.
    The result is returned by the argument pointers.
******************************************************************************/
GetParaRange(PTERWND w,LPLONG StartLine,LPLONG EndLine)
{
    long l,FirstLine,LastLine;
    LPBYTE ptr;
    int len;

    if (!NormalizeBlock(w)) HilightType=HILIGHT_OFF;

    if (HilightType!=HILIGHT_OFF) {
        FirstLine=HilightBegRow;
        LastLine=HilightEndRow;
        if (HilightEndCol==0 && LastLine>FirstLine && cid(LastLine)==0) LastLine--;
    }
    else FirstLine=LastLine=CurLine;

    // locate the beginning of the paragraph
    l=FirstLine-1;
    while (l>=0) {
        len=LineLen(l);
        if (len>0) {
           ptr=pText(l);
           if (ptr[len-1]==ParaChar || ptr[len-1]==CellChar || ptr[len-1]==ROW_CHAR
           || IsHdrFtrChar(ptr[len-1])
           || ptr[len-1]==SECT_CHAR || ptr[len-1]==PAGE_CHAR || ptr[len-1]==COL_CHAR) {

              FirstLine=l+1;
              break;
           }

        }
        if (l==0) FirstLine=0;
        l--;
    }

    // locate the end of the paragraph
    l=LastLine;
    while (l<TotalLines) {
        len=LineLen(l);
        if (len>0) {
           ptr=pText(l);
           if (ptr[len-1]==ParaChar || ptr[len-1]==CellChar || ptr[len-1]==ROW_CHAR
           || IsHdrFtrChar(ptr[len-1])
           || ptr[len-1]==SECT_CHAR || ptr[len-1]==PAGE_CHAR || ptr[len-1]==COL_CHAR) {

              LastLine=l;
              break;
           }

        }
        if ((l+1)==TotalLines) LastLine=l;
        l++;
    }

    (*StartLine)=FirstLine;
    (*EndLine)=LastLine;

    return TRUE;
}

/******************************************************************************
    TerGetParaInfo:
    Get the paragraph information.  If LineNo is negative, it specifies the
    para id.
******************************************************************************/
BOOL WINAPI _export TerGetParaInfo(HWND hWnd, long LineNo, LPINT LeftIndent, LPINT RightIndent,
          LPINT FirstIndent, LPINT TabId, LPINT StyId, LPINT AuxId, LPINT shading,
          UINT far *pflags, LPINT SpaceBefore, LPINT SpaceAfter, LPINT SpaceBetween,
          UINT far *flags)
{

     return TerGetParaInfo2(hWnd,LineNo,LeftIndent,RightIndent,
          FirstIndent, TabId, StyId, AuxId, shading,
          pflags, SpaceBefore, SpaceAfter, SpaceBetween,
          flags,NULL,NULL);
}

/******************************************************************************
    TerGetParaInfo2:
    Get the paragraph information.  If LineNo is negative, it specifies the
    para id.
******************************************************************************/
BOOL WINAPI _export TerGetParaInfo2(HWND hWnd, long LineNo, LPINT LeftIndent, LPINT RightIndent,
          LPINT FirstIndent, LPINT TabId, LPINT StyId, LPINT AuxId, LPINT shading,
          UINT far *pflags, LPINT SpaceBefore, LPINT SpaceAfter, LPINT SpaceBetween,
          UINT far *flags, DWORD far *Aux1Id, COLORREF far *BkColor)
{

    return TerGetParaInfo3(hWnd,LineNo,FALSE,LeftIndent,RightIndent,
          FirstIndent,TabId,StyId,AuxId,shading,
          pflags,SpaceBefore,SpaceAfter,SpaceBetween,
          flags,Aux1Id,BkColor);
}

/******************************************************************************
    TerGetParaInfo3:
    Get the paragraph information.  If LineNo is negative, it specifies the
    para id.
******************************************************************************/
BOOL WINAPI _export TerGetParaInfo3(HWND hWnd, long LineNo, BOOL IsStyleItem, LPINT LeftIndent, LPINT RightIndent,
          LPINT FirstIndent, LPINT TabId, LPINT StyId, LPINT AuxId, LPINT shading,
          UINT far *pflags, LPINT SpaceBefore, LPINT SpaceAfter, LPINT SpaceBetween,
          UINT far *flags, DWORD far *Aux1Id, COLORREF far *BkColor)
{

    return TerGetParaInfo4(hWnd,LineNo,IsStyleItem,LeftIndent,RightIndent,
          FirstIndent,TabId,StyId,AuxId,shading,
          pflags,SpaceBefore,SpaceAfter,SpaceBetween,
          flags,Aux1Id,BkColor,NULL);

}

/******************************************************************************
    TerGetParaInfo4:
    Get the paragraph information.  If LineNo is negative, it specifies the
    para id.
******************************************************************************/
BOOL WINAPI _export TerGetParaInfo4(HWND hWnd, long LineNo, BOOL IsStyleItem, LPINT LeftIndent, LPINT RightIndent,
          LPINT FirstIndent, LPINT TabId, LPINT StyId, LPINT AuxId, LPINT shading,
          UINT far *pflags, LPINT SpaceBefore, LPINT SpaceAfter, LPINT SpaceBetween,
          UINT far *flags, DWORD far *Aux1Id, COLORREF far *BkColor, LPINT LineSpacing)
{
    PTERWND w;
    int ParaId,sid=-1;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (IsStyleItem) {
       if (LineNo==SID_CUR) sid=CurSID;
       else                 sid=(int)LineNo;
       if (sid<0 || sid>=TotalSID) return FALSE;
    }
    else { 
       if (LineNo<0) ParaId=(int)(-LineNo);
       else {
           if (LineNo>=TotalLines) return FALSE;
           ParaId=pfmt(LineNo);
       }
       if (ParaId<0 || ParaId>=TotalPfmts) return FALSE;
    }

    if (sid<0) {          // get regular paragraph info
       if (LeftIndent)   (*LeftIndent)=PfmtId[ParaId].LeftIndentTwips;
       if (RightIndent)  (*RightIndent)=PfmtId[ParaId].RightIndentTwips;
       if (FirstIndent)  (*FirstIndent)=PfmtId[ParaId].FirstIndentTwips;
       if (TabId)        (*TabId)=PfmtId[ParaId].TabId;
       if (StyId)        (*StyId)=PfmtId[ParaId].StyId;
       if (AuxId)        (*AuxId)=PfmtId[ParaId].AuxId;
       if (shading)      (*shading)=PfmtId[ParaId].shading;
       if (pflags)       (*pflags)=PfmtId[ParaId].pflags;
       if (SpaceBefore)  (*SpaceBefore)=PfmtId[ParaId].SpaceBefore;
       if (SpaceAfter)   (*SpaceAfter)=PfmtId[ParaId].SpaceAfter;
       if (SpaceBetween) (*SpaceBetween)=PfmtId[ParaId].SpaceBetween;
       if (LineSpacing)  (*LineSpacing)=PfmtId[ParaId].LineSpacing;
       if (flags)        (*flags)=PfmtId[ParaId].flags;
       if (Aux1Id)       (*Aux1Id)=PfmtId[ParaId].Aux1Id;
       if (BkColor)      (*BkColor)=PfmtId[ParaId].BkColor;
    }
    else {                // get paragraph info for a stylesheet item
       if (LeftIndent)   (*LeftIndent)=StyleId[sid].LeftIndentTwips;
       if (RightIndent)  (*RightIndent)=StyleId[sid].RightIndentTwips;
       if (FirstIndent)  (*FirstIndent)=StyleId[sid].FirstIndentTwips;
       if (TabId)        (*TabId)=StyleId[sid].TabId;
       if (StyId)        (*StyId)=sid;
       if (AuxId)        (*AuxId)=0;
       if (shading)      (*shading)=StyleId[sid].shading;
       if (pflags)       (*pflags)=StyleId[sid].pflags;
       if (SpaceBefore)  (*SpaceBefore)=StyleId[sid].SpaceBefore;
       if (SpaceAfter)   (*SpaceAfter)=StyleId[sid].SpaceAfter;
       if (SpaceBetween) (*SpaceBetween)=StyleId[sid].SpaceBetween;
       if (LineSpacing)  (*LineSpacing)=StyleId[sid].LineSpacing;
       if (flags)        (*flags)=StyleId[sid].ParaFlags;
       if (Aux1Id)       (*Aux1Id)=0;
       if (BkColor)      (*BkColor)=StyleId[sid].ParaBkColor;
    } 

    return TRUE;
}

/******************************************************************************
    TerGetParaParam:
    Get the paragraph information.  If LineNo is negative, it specifies the
    para id.
******************************************************************************/
int WINAPI _export TerGetParaParam(HWND hWnd, long LineNo, BOOL IsStyleItem, int type)
{
    PTERWND w;
    int ParaId,sid=-1;

    if (NULL==(w=GetWindowPointer(hWnd))) return PARAINFO_ERROR;  // get the pointer to window data

    if (IsStyleItem) {
       if (LineNo==SID_CUR) sid=CurSID;
       else                 sid=(int)LineNo;
       if (sid<0 || sid>=TotalSID) return PARAINFO_ERROR;
    }
    else { 
       if (LineNo<0) ParaId=(int)(-LineNo);
       else {
           if (LineNo>=TotalLines) return PARAINFO_ERROR;
           ParaId=pfmt(LineNo);
       }
       if (ParaId<0 || ParaId>=TotalPfmts) return PARAINFO_ERROR;
    }

    switch (type) {
      case PARAINFO_TEXT_FLOW:
         return (sid<0)?PfmtId[ParaId].flow:0;

      case PARAINFO_BORDER_COLOR:
         return (int)(DWORD)((sid<0)?PfmtId[ParaId].BorderColor:StyleId[sid].ParaBorderColor);
      
      case PARAINFO_BK_COLOR:
         return (int)(DWORD)((sid<0)?PfmtId[ParaId].BkColor:StyleId[sid].ParaBkColor);
    } 
      
    return PARAINFO_ERROR;
}

/******************************************************************************
    TerCreateParaId:
    Create a new para id.
    if ReuseId >=0 then then the new information is assined to ReuseId.
******************************************************************************/
int WINAPI _export TerCreateParaId(HWND hWnd, int ReuseId, int shared, int LeftIndentTwips, int RightIndentTwips,
          int FirstIndentTwips, int TabId, int StyId, int AuxId, UINT shading,
          UINT pflags, int SpaceBefore, int SpaceAfter, int SpaceBetween,
          unsigned flags) {

   return TerCreateParaIdEx(hWnd,ReuseId,shared,LeftIndentTwips,RightIndentTwips,
          FirstIndentTwips,TabId,StyId,AuxId,shading,
          pflags,SpaceBefore,SpaceAfter,SpaceBetween,flags,0,CLR_WHITE);
}

/******************************************************************************
    TerCreateParaIdEx:
    Create a new para id.
    if ReuseId >=0 then then the new information is assined to ReuseId.
******************************************************************************/
int WINAPI _export TerCreateParaIdEx(HWND hWnd, int ReuseId, int shared, int LeftIndentTwips, int RightIndentTwips,
          int FirstIndentTwips, int TabId, int StyId, int AuxId, UINT shading,
          UINT pflags, int SpaceBefore, int SpaceAfter, int SpaceBetween,
          unsigned flags,int BltId, COLORREF BkColor)
{
    PTERWND w;
    int i,OldId=0,NewId;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    if (ReuseId>=MaxPfmts)  return -1;

    if (ReuseId>=0) shared=FALSE;    // mutually exclusive

    // initialize intermedite ids
    if (ReuseId>=TotalPfmts) {    // initialize intermediate ids
       for (i=TotalPfmts;i<=ReuseId;i++) FarMemSet(&(PfmtId[i]),0,sizeof(PfmtId[i]));
       TotalPfmts=ReuseId+1;
    }
    if (!shared) MatchIds=FALSE;     // create an exclusive id

    if (ReuseId>=0) OldId=ReuseId;

    NewId=NewParaId(w,OldId,
                      LeftIndentTwips,
                      RightIndentTwips,
                      FirstIndentTwips,
                      TabId,
                      BltId,
                      AuxId,
                      NextParaAux1Id,
                      StyId,
                      shading,
                      pflags,
                      SpaceBefore,
                      SpaceAfter,
                      SpaceBetween,
                      0,
                      BkColor,
                      PARA_BOX_SPACE,
                      FLOW_DEF,
                      flags);

    // reset the next para aux1 id
    NextParaAux1Id=0;

    // copy the new para id to the old id if requested
    if (ReuseId>=0 && NewId!=ReuseId) {
       FarMove(&(PfmtId[NewId]),&(PfmtId[ReuseId]),sizeof(PfmtId[NewId]));
       NewId=ReuseId;
    }

    return NewId;
}

/******************************************************************************
    NewParaId:
    get the new para id by applying new specification to the old paragraph id
******************************************************************************/
NewParaId(PTERWND w,int old,int LeftIndentTwips, int RightIndentTwips,
          int FirstIndentTwips, int TabId, int BltId, int AuxId, DWORD Aux1Id, int StyId,
          UINT shading, UINT pflags, int SpaceBefore, int SpaceAfter,
          int SpaceBetween, int LineSpacing, COLORREF BkColor, int BorderSpace, int flow, unsigned flags)
{
   struct StrPfmt NewPara;
   BOOL UseOld=false;

   if (old>=0 && old<TotalPfmts) UseOld=true;

   if (UseOld) FarMove(&(PfmtId[old]),&NewPara,sizeof(struct StrPfmt));
   else        FarMemSet(&(PfmtId[old]),0,sizeof(struct StrPfmt));

   // fill the new structure
   NewPara.LeftIndentTwips=LeftIndentTwips;
   NewPara.RightIndentTwips=RightIndentTwips;
   NewPara.FirstIndentTwips=FirstIndentTwips;
   NewPara.TabId=TabId;
   NewPara.BltId=BltId;
   NewPara.AuxId=AuxId;
   NewPara.Aux1Id=Aux1Id;
   NewPara.StyId=StyId;
   NewPara.shading=shading;
   NewPara.pflags=pflags;
   NewPara.SpaceBefore=SpaceBefore;
   NewPara.SpaceAfter=SpaceAfter;
   NewPara.SpaceBetween=SpaceBetween;
   NewPara.LineSpacing=LineSpacing;
   NewPara.BkColor=BkColor;
   NewPara.BorderSpace=BorderSpace;
   NewPara.flow=flow;
   NewPara.flags=flags;

   //****** calculate the indentations in device units *****
   NewPara.LeftIndent=TwipsToScrX(NewPara.LeftIndentTwips);
   NewPara.RightIndent=TwipsToScrX(NewPara.RightIndentTwips);
   NewPara.FirstIndent=TwipsToScrX(NewPara.FirstIndentTwips);
   
   // set structure variables not provided by NewParaId function
   NewPara.BorderColor=UseOld?PfmtId[old].BorderColor:CLR_AUTO;
             
   return NewParaId2(w,old,&NewPara);
}

/******************************************************************************
    NewParaId2:
    get the new para id by applying new specification to the old paragraph id
******************************************************************************/
NewParaId2(PTERWND w, int old, struct StrPfmt *pNew)
{
    int i,j,UnusedId;
    long l;
    DWORD OldSize, NewSize;
    int   NewMaxPfmts;
    HGLOBAL hMem;
    BYTE huge * pMem;

    if (!(pNew->flags&BULLET)) pNew->BltId=0;

    if (MatchIds) {     // check if such a paragraph already exists
       if ( PfmtId[old].LeftIndentTwips==pNew->LeftIndentTwips
         && PfmtId[old].RightIndentTwips==pNew->RightIndentTwips
         && PfmtId[old].FirstIndentTwips==pNew->FirstIndentTwips
         && PfmtId[old].TabId==pNew->TabId
         && PfmtId[old].BltId==pNew->BltId
         && PfmtId[old].AuxId==pNew->AuxId
         && PfmtId[old].Aux1Id==pNew->Aux1Id
         && PfmtId[old].StyId==pNew->StyId
         && PfmtId[old].shading==pNew->shading
         && PfmtId[old].pflags==pNew->pflags
         && PfmtId[old].SpaceBefore==pNew->SpaceBefore
         && PfmtId[old].SpaceAfter==pNew->SpaceAfter
         && PfmtId[old].SpaceBetween==pNew->SpaceBetween
         && PfmtId[old].LineSpacing==pNew->LineSpacing
         && PfmtId[old].BkColor==pNew->BkColor
         && PfmtId[old].BorderSpace==pNew->BorderSpace
         && PfmtId[old].BorderColor==pNew->BorderColor
         && PfmtId[old].flow==pNew->flow
         && PfmtId[old].flags==pNew->flags ){
          return old;
       }

       //*** search the current paragraph id table ***
       for (i=0;i<TotalPfmts;i++) {
           if (PfmtId[i].LeftIndentTwips==pNew->LeftIndentTwips
            && PfmtId[i].RightIndentTwips==pNew->RightIndentTwips
            && PfmtId[i].FirstIndentTwips==pNew->FirstIndentTwips
            && PfmtId[i].TabId==pNew->TabId
            && PfmtId[i].BltId==pNew->BltId
            && PfmtId[i].AuxId==pNew->AuxId
            && PfmtId[i].Aux1Id==pNew->Aux1Id
            && PfmtId[i].StyId==pNew->StyId
            && PfmtId[i].shading==pNew->shading
            && PfmtId[i].pflags==pNew->pflags
            && PfmtId[i].SpaceBefore==pNew->SpaceBefore
            && PfmtId[i].SpaceAfter==pNew->SpaceAfter
            && PfmtId[i].SpaceBetween==pNew->SpaceBetween
            && PfmtId[i].LineSpacing==pNew->LineSpacing
            && PfmtId[i].BkColor==pNew->BkColor
            && PfmtId[i].BorderSpace==pNew->BorderSpace
            && PfmtId[i].BorderColor==pNew->BorderColor
            && PfmtId[i].flow==pNew->flow
            && PfmtId[i].flags==pNew->flags ){
              return i;
           }
       }
    }
    else MatchIds=TRUE;    // reset this flag

    // check if paratable exhausted
    UnusedId=-1;
    if (TotalPfmts>=MaxPfmts && MaxPfmts>=1000 && ReclaimResources) { // find an unused paraid when the array becomes large
       BYTE far *InUse=OurAlloc(MaxPfmts);          // allocate temporary flag array
       FarMemSet(InUse,0,MaxPfmts);
       for (l=0;l<TotalLines;l++) InUse[pfmt(l)]=TRUE;
       // now check the undo buffer
       for (i=0;i<UndoTblSize;i++) {
          if (undo[i].type=='P') {
             LPWORD pUndoPfmt=GlobalLock(undo[i].hPfmt);
             if (pUndoPfmt) {
                int TotalIds=pUndoPfmt[0];
                for (j=0;j<TotalIds;j++) InUse[pUndoPfmt[j+1]]=TRUE;
             }
             GlobalUnlock(undo[i].hPfmt);
          }
       }
       for (i=1;i<MaxPfmts;i++) if (!InUse[i]) break;
       if (i<MaxPfmts) UnusedId=i;                  // unused id found
       OurFree(InUse);
    }

    // out of max table size?
    if (!Win32 && TotalPfmts>=MAX_PFMT && UnusedId==-1)  {
       if (!(MessageDisplayed&MSG_MAX_PFMT)) PrintError(w,MSG_OUT_OF_PARA_SLOT,NULL);
       MessageDisplayed|=MSG_MAX_PFMT;
       return old; // ran out of cell table size
    }

    // enlarge the para id table
    if (TotalPfmts>=MaxPfmts && UnusedId==-1) {        // allocate more memory for the cell table
       NewMaxPfmts=MaxPfmts+MaxPfmts/2;

       if (!Win32 && NewMaxPfmts>MAX_PFMT) NewMaxPfmts=MAX_PFMT;

       OldSize=((DWORD)MaxPfmts+1)*sizeof(struct StrPfmt);
       NewSize=((DWORD)NewMaxPfmts+1)*sizeof(struct StrPfmt);

       if ( (NULL!=(hMem=GlobalAlloc(GMEM_MOVEABLE,NewSize)))
          && (NULL!=(pMem=GlobalLock(hMem))) ){

          // move data to new location
          HugeMove(PfmtId,pMem,OldSize);

          // free old handle
          GlobalUnlock(hPfmtId);
          GlobalFree(hPfmtId);

          // assign new handle
          hPfmtId=hMem;              // new array
          PfmtId=(struct StrPfmt far *)pMem;

          MaxPfmts=NewMaxPfmts;    // new max PfmtIds
       }
       else return -1;
    }

    if (TotalPfmts<MaxPfmts || UnusedId>=0) {
       if (TotalPfmts<MaxPfmts) {
          i=TotalPfmts;
          TotalPfmts++;
       }
       else  i=UnusedId;

       PfmtId[i].LeftIndentTwips=pNew->LeftIndentTwips;
       PfmtId[i].RightIndentTwips=pNew->RightIndentTwips;
       PfmtId[i].FirstIndentTwips=pNew->FirstIndentTwips;
       PfmtId[i].TabId=pNew->TabId;
       PfmtId[i].BltId=pNew->BltId;
       PfmtId[i].AuxId=pNew->AuxId;
       PfmtId[i].Aux1Id=pNew->Aux1Id;
       PfmtId[i].StyId=pNew->StyId;
       PfmtId[i].shading=pNew->shading;
       PfmtId[i].pflags=pNew->pflags;
       PfmtId[i].SpaceBefore=pNew->SpaceBefore;
       PfmtId[i].SpaceAfter=pNew->SpaceAfter;
       PfmtId[i].SpaceBetween=pNew->SpaceBetween;
       PfmtId[i].LineSpacing=pNew->LineSpacing;
       PfmtId[i].BkColor=pNew->BkColor;
       PfmtId[i].BorderSpace=pNew->BorderSpace;
       PfmtId[i].BorderColor=pNew->BorderColor;
       PfmtId[i].flow=pNew->flow;
       PfmtId[i].flags=pNew->flags;

       //****** calculate the indentations in device units *****
       PfmtId[i].LeftIndent=TwipsToScrX(PfmtId[i].LeftIndentTwips);
       PfmtId[i].RightIndent=TwipsToScrX(PfmtId[i].RightIndentTwips);
       PfmtId[i].FirstIndent=TwipsToScrX(PfmtId[i].FirstIndentTwips);

       return i;             // new paragraph id
    }

    if (!(MessageDisplayed&MSG_MAX_PFMT)) PrintError(w,MSG_OUT_OF_PARA_SLOT,NULL);
    MessageDisplayed|=MSG_MAX_PFMT;

    return old;
}

/******************************************************************************
    NewTabId:
    get the new tab id by applying new specification to the old tab id
******************************************************************************/
NewTabId(PTERWND w,int old,struct StrTab far *TabRec)
{
    int i,j;
    int  UnusedId;
    long l;
    BYTE TabInUse[MAX_TABS];
    BYTE far *PfmtInUse;          // allocate temporary flag array

    if (TabRec->count==0) return 0;   // return default tab id

    //*** search the current tab id table ***
    for (i=0;i<TotalTabs;i++) {
        if (TerTab[i].count==TabRec->count) {
           for (j=0;j<TabRec->count;j++) {     // check each tab position
              if (TerTab[i].pos[j]!=TabRec->pos[j]) break;
              if (TerTab[i].type[j]!=TabRec->type[j]) break;
              if (TerTab[i].flags[j]!=TabRec->flags[j]) break;
           }
           if (j==TabRec->count) return i;     // match found
        }
    }

    // check if tab table exhausted
    UnusedId=-1;
    if (TotalTabs>=MAX_TABS) {       // find an unused tab id
       // look for a tab id with 0 count
       for (i=1;i<TotalTabs;i++) {
          if (TerTab[i].count==0) {
             UnusedId=i;
             break;
          }
       }
       if (UnusedId==-1) {
          // reset the tab ids in the unused paragraphs
          PfmtInUse=OurAlloc(TotalPfmts);          // allocate temporary flag array
          FarMemSet(PfmtInUse,0,TotalPfmts);
          for (l=0;l<TotalLines;l++) PfmtInUse[pfmt(l)]=TRUE;
          for (i=1;i<TotalPfmts;i++) if (!PfmtInUse[i]) PfmtId[i].TabId=0;  // reset the tab id
          OurFree(PfmtInUse);

          for (i=1;i<MAX_TABS;i++)   TabInUse[i]=FALSE;     // don't touch the default slot
          for (j=0;j<TotalPfmts;j++) TabInUse[PfmtId[j].TabId]=TRUE;
          for (i=1;i<MAX_TABS;i++) {
             if (!TabInUse[i]) {
                if (UnusedId<0) UnusedId=i;
                TerTab[i].count=0;   // reset the count to 0
                break;
             }
          }
       }
    }

    if (TotalTabs<MAX_TABS || UnusedId>=0) {
       if (TotalTabs<MAX_TABS) {
          i=TotalTabs;
          TotalTabs++;
       }
       else  i=UnusedId;

       TerTab[i]=(*TabRec);                    // new tab id

       return i;                               // new tab id
    }

    if (!(TerOpFlags&TOFLAG_NO_MORE_TAB_MSG)) {
       PrintError(w,MSG_OUT_OF_TAB_TABLE,NULL);
       TerOpFlags|=TOFLAG_NO_MORE_TAB_MSG;
    }

    return old;
}

/******************************************************************************
    NewBltId:
    get the new bullet id by applying new specification to the old bullet id
******************************************************************************/
NewBltId(PTERWND w,int old,struct StrBlt far *BltRec)
{
    int i,j;
    int  UnusedId;
    long l;
    LPBYTE BltInUse;
    BYTE far *PfmtInUse;          // allocate temporary flag array

    //*** search the current tab id table ***
    for (i=1;i<TotalBlts;i++) {
        if (TerBlt[i].flags!=BltRec->flags) continue;
        if (TerBlt[i].start!=BltRec->start) continue;
        if (TerBlt[i].level!=BltRec->level) continue;
        if (TerBlt[i].NumberType!=BltRec->NumberType) continue;
        if (TerBlt[i].BulletChar!=BltRec->BulletChar) continue;
        if (TerBlt[i].AftChar!=BltRec->AftChar) continue;
        if (lstrcmpi(TerBlt[i].BefText,BltRec->BefText)!=0) continue;

        if (TerBlt[i].ls!=BltRec->ls) continue;         // list table reference
        if (TerBlt[i].lvl!=BltRec->lvl) continue;       // list override table reference


        return i;     // match found
    }

    // check if some bullets ids can be freed
    UnusedId=-1;
    if (TotalBlts>=MaxBlts) {       // find an unused tab id
       BltInUse=OurAlloc(TotalBlts+1);
       
       // reset the tab ids in the unused paragraphs
       PfmtInUse=OurAlloc(TotalPfmts);          // allocate temporary flag array
       FarMemSet(PfmtInUse,0,TotalPfmts);
       for (l=0;l<TotalLines;l++) PfmtInUse[pfmt(l)]=TRUE;
       for (i=1;i<TotalPfmts;i++) if (!PfmtInUse[i]) PfmtId[i].BltId=0;  // reset the tab id
       OurFree(PfmtInUse);

       for (i=1;i<TotalBlts;i++)   BltInUse[i]=FALSE;     // don't touch the default slot
       for (j=0;j<TotalPfmts;j++) BltInUse[PfmtId[j].BltId]=TRUE;
       for (i=1;i<TotalBlts;i++) {
          if (!BltInUse[i]) {
             if (UnusedId<0) UnusedId=i;
             break;
          }
       }

       OurFree(BltInUse);
    }

    // exapand the bullet table 
    if (UnusedId==-1 && TotalBlts>=MaxBlts) {        // allocate more memory for the cell table
       int NewMaxBlts=MaxBlts+MaxBlts/2;
       DWORD OldSize,NewSize;
       LPBYTE pMem;

       if (!Win32 && NewMaxBlts>MAX_BLTS) NewMaxBlts=MAX_BLTS;

       OldSize=((DWORD)MaxBlts+1)*sizeof(struct StrBlt);
       NewSize=((DWORD)NewMaxBlts+1)*sizeof(struct StrBlt);

       if ((NewSize>OldSize) && NULL!=(pMem=OurAlloc(NewSize))) {

          // move data to new location
          HugeMove(TerBlt,pMem,OldSize);

          // free old handle
          OurFree(TerBlt);
          TerBlt=(struct StrBlt far *)pMem;
          
          MaxBlts=NewMaxBlts;    // new max cells
       }
    }

    // assign the bullet id
    if (TotalBlts<MaxBlts || UnusedId>=0) {
       if (TotalBlts<MaxBlts) {
          i=TotalBlts;
          TotalBlts++;
       }
       else  i=UnusedId;

       TerBlt[i]=(*BltRec);                    // new tab id

       return i;                               // new tab id
    }

    PrintError(w,MSG_OUT_OF_BLT_TABLE,NULL);

    return old;
}

/******************************************************************************
    ClearTabDlg:
    This function invokes the dialog to accept a tab position to clear.
******************************************************************************/
ClearTabDlg(PTERWND w)
{
    int idx,TabId;

    if (EditingParaStyle) TabId=StyleId[CurSID].TabId;        // style sheet tab id
    else                  TabId=PfmtId[pfmt(CurLine)].TabId;  // current tab id

    idx=CallDialogBox(w,"ClearTabParam",ClearTabParam,TabId);

    if (idx>=0) {
       ClearTab(hTerWnd,TerTab[TabId].pos[idx],TRUE);
    }

    return TRUE;
}

/******************************************************************************
    TerSetBullet:
    Set or reset the paragraph bullet.
******************************************************************************/
BOOL WINAPI _export TerSetBullet(HWND hWnd,BOOL set,BOOL repaint)
{
    return TerSetBulletEx(hWnd,set,TRUE,1,0,0,repaint);
}

/******************************************************************************
    TerSetBulletEx:
    Set or reset the paragraph bullet/numbering.  The 'type' parameter is either
    one of BLT_ or NBR_ constants.
******************************************************************************/
BOOL WINAPI _export TerSetBulletEx(HWND hWnd,BOOL set,BOOL IsBullet, int start, int level, int type, BOOL repaint)
{
    return TerSetBullet2(hWnd,set,IsBullet,start,level,type,NULL,NULL,repaint);
}
    
/******************************************************************************
    TerSetBullet2:
    Set or reset the paragraph bullet/numbering.  The 'type' parameter is either
    one of BLT_ or NBR_ constants.
******************************************************************************/
BOOL WINAPI _export TerSetBullet2(HWND hWnd,BOOL set,BOOL IsBullet, int start, int level, int type, LPBYTE TextBef, LPBYTE TextAft, BOOL repaint)
{
    return TerSetBullet3(hWnd,set,IsBullet,start,level,type,TextBef,TextAft,0,repaint);
}
 
/******************************************************************************
    TerSetBullet3:
    Set or reset the paragraph bullet/numbering.  The 'type' parameter is either
    one of BLT_ or NBR_ constants.
******************************************************************************/
BOOL WINAPI _export TerSetBullet3(HWND hWnd,BOOL set,BOOL IsBullet, int start, int level, int type, LPBYTE TextBef, LPBYTE TextAft, UINT BltFlags, BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;
    UINT flags;
    int  LeftIndent,FirstIndent,BltId;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    if (level<0)  level=0;                // for future use
    if (level>10) level=10;

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // retrieve the old values
       if (EditingParaStyle) {
          flags=StyleId[CurSID].ParaFlags;
          LeftIndent=StyleId[CurSID].LeftIndentTwips;
          FirstIndent=StyleId[CurSID].FirstIndentTwips;
          BltId=StyleId[CurSID].BltId;
       }
       else {
          flags=PfmtId[pfmt(l)].flags;
          LeftIndent=PfmtId[pfmt(l)].LeftIndentTwips;
          FirstIndent=PfmtId[pfmt(l)].FirstIndentTwips;
          BltId=PfmtId[pfmt(l)].BltId;
       }

       // get new paraid
       if (set) {
          flags|=BULLET;
          BltId=TerCreateBulletId3(hWnd,IsBullet,start,level,type,TextBef,TextAft,BltFlags);
       }
       else {
          flags=ResetUintFlag(flags,BULLET);
          BltId=0;
       }

       if (set && !(PfmtId[pfmt(l)].flags&BULLET)) {
          LeftIndent+=360;
          FirstIndent-=360;
       }
       else if (!set && PfmtId[pfmt(l)].flags&BULLET) {
          LeftIndent-=360;
          FirstIndent+=360;
       }

       if (LeftIndent<0) LeftIndent=0;
       if ((LeftIndent+FirstIndent)<0) FirstIndent=-LeftIndent;

       // update stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].ParaFlags=flags;
          StyleId[CurSID].LeftIndentTwips=LeftIndent;
          StyleId[CurSID].FirstIndentTwips=FirstIndent;
          StyleId[CurSID].BltId=BltId;
          DrawRuler(w,FALSE);
          return TRUE;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         LeftIndent,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         FirstIndent,
                         PfmtId[pfmt(l)].TabId,
                         BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         flags);
    }

    TerArg.modified++;

    if (repaint) {
       PaintTer(w);
       if (RulerPending) DrawRuler(w,FALSE);
    }
    else WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    TerCreateBulletId:
    Create a new bullet id from the specified parameters.
******************************************************************************/
int WINAPI _export TerCreateBulletId(HWND hWnd,BOOL IsBullet, int start, int level, int type)
{
    return TerCreateBulletId2(hWnd,IsBullet,start,level,type,NULL,NULL);
}
    
/******************************************************************************
    TerCreateBulletId2:
    Create a new bullet id from the specified parameters.
******************************************************************************/
int WINAPI _export TerCreateBulletId2(HWND hWnd,BOOL IsBullet, int start, int level, int type, LPBYTE TextBef, LPBYTE TextAft)
{
   return TerCreateBulletId3(hWnd,IsBullet,start,level,type,TextBef,TextAft,0);
}

/******************************************************************************
    TerCreateBulletId3:
    Create a new bullet id from the specified parameters.
******************************************************************************/
int WINAPI _export TerCreateBulletId3(HWND hWnd,BOOL IsBullet, int start, int level, int type, LPBYTE TextBef, LPBYTE TextAft, UINT flags)
{
    PTERWND w;
    struct StrBlt blt;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.WordWrap) return 0;

    FarMove(&(TerBlt[0]),&blt,sizeof(struct StrBlt));

    if (IsBullet) {
       blt.IsBullet=TRUE;
       blt.level=11;                    // level constant for the bullets

       if (type==BLT_DIAMOND) {
          blt.BulletChar=0xa8;
          blt.font=BFONT_SYMBOL;
       }
       else if (type==BLT_SQUARE) {
          blt.BulletChar=0xa7;
          blt.font=BFONT_WINGDINGS;
       }
       else if (type==BLT_HOLLOW_SQUARE) {
          blt.BulletChar='q';
          blt.font=BFONT_WINGDINGS;
       }
       else if (type==BLT_4_DIAMONDS) {
          blt.BulletChar='v';
          blt.font=BFONT_WINGDINGS;
       }
       else if (type==BLT_ARROW) {
          blt.BulletChar=0xd8;
          blt.font=BFONT_WINGDINGS;
       }
       else if (type==BLT_CHECK) {
          blt.BulletChar=0xfc;
          blt.font=BFONT_WINGDINGS;
       }
       else {                         // default
          blt.BulletChar=0xb7;
          blt.font=BFONT_SYMBOL;
       }
    }
    else {                  // construct the new bullet id
       blt.IsBullet=FALSE;
       blt.start=start;
       blt.level=level;
       blt.NumberType=type;

       if (TextBef) lstrcpy(blt.BefText,TextBef);
       if (TextAft) blt.AftChar=TextAft[0];
    }

    blt.flags=flags;

    return NewBltId(w,0,&blt);
}

/******************************************************************************
    TerCreateListBulletId:
    Create a new bullet id using the list parametersspecified parameters.
******************************************************************************/
int WINAPI _export TerCreateListBulletId(HWND hWnd, int CurListOr, int level)
{
   return TerCreateListBulletId2(hWnd,CurListOr,level,-1);
}

/******************************************************************************
    TerCreateListBulletId:
    Create a new bullet id using the list parametersspecified parameters. Set the
    BltType to a BLT_ constant when creating a non-standard bullet.  Otherwise
    set to -1.
 ******************************************************************************/
int WINAPI _export TerCreateListBulletId2(HWND hWnd, int CurListOr, int level,int BltType)
{
    PTERWND w;
    struct StrBlt blt;
    int LevelCount;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.WordWrap) return 0;
    
    // validate parameters
    if (CurListOr<0 || CurListOr>=TotalListOr) return 0;
    LevelCount=ListOr[CurListOr].LevelCount;
    if (LevelCount==0) {
       int CurList=ListOr[CurListOr].ListIdx;
       LevelCount=list[CurList].LevelCount;
    } 
    if (level<0 || level>=LevelCount) return 0;


    FarMove(&(TerBlt[0]),&blt,sizeof(struct StrBlt));
    blt.IsBullet=FALSE;
    blt.ls=CurListOr;
    blt.lvl=level;

    if (BltType!=-1) {   // this list level represents a bullet, set the specified bullet character
       if (BltType==BLT_DIAMOND) {
          blt.BulletChar=0xa8;
          blt.font=BFONT_SYMBOL;
       }
       else if (BltType==BLT_SQUARE) {
          blt.BulletChar=0xa7;
          blt.font=BFONT_WINGDINGS;
       }
       else if (BltType==BLT_HOLLOW_SQUARE) {
          blt.BulletChar='q';
          blt.font=BFONT_WINGDINGS;
       }
       else if (BltType==BLT_4_DIAMONDS) {
          blt.BulletChar='v';
          blt.font=BFONT_WINGDINGS;
       }
       else if (BltType==BLT_ARROW) {
          blt.BulletChar=0xd8;
          blt.font=BFONT_WINGDINGS;
       }
       else if (BltType==BLT_CHECK) {
          blt.BulletChar=0xfc;
          blt.font=BFONT_WINGDINGS;
       }
       else {                         // default
          blt.BulletChar=0xb7;
          blt.font=BFONT_SYMBOL;
       }
    }

    return NewBltId(w,0,&blt);
}

/******************************************************************************
    TerSetBulletId:
    Assign a bullet id to a para id
******************************************************************************/
BOOL WINAPI _export TerSetBulletId(HWND hWnd,int BltId, int ParaId)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (ParaId<0 || ParaId>=TotalPfmts) return FALSE;
    if (BltId<0 || BltId>=TotalBlts) return FALSE;
    PfmtId[ParaId].BltId=BltId;
    return TRUE;
}

/******************************************************************************
    TerGetBulletInfo:
    Retrieve the bullet info. The id type can be PID_LINE, PID_PARA or PID_BULLET.
    This function returns the bullet id if successful. Otherwise it returns -1.
    The bullet id of 0 signifies that the line or paragraph do not use bullet
    property.
******************************************************************************/
int WINAPI _export TerGetBulletInfo(HWND hWnd,int IdType, long id,LPINT IsBullet, LPINT start, LPINT level, LPINT symbol, LPINT flags)
{
    return TerGetBulletInfo2(hWnd,IdType,id,IsBullet,start,level,symbol, NULL,flags);
}

/******************************************************************************
    TerGetBulletInfo2:
    Retrieve the bullet info. The id type can be PID_LINE, PID_PARA or PID_BULLET.
    This function returns the bullet id if successful. Otherwise it returns -1.
    The bullet id of 0 signifies that the line or paragraph do not use bullet
    property.
******************************************************************************/
int WINAPI _export TerGetBulletInfo2(HWND hWnd,int IdType, long id,LPINT IsBullet, LPINT start, LPINT level, LPINT symbol, LPINT pListOr, LPINT flags)
{
    return TerGetBulletInfo3(hWnd,IdType,id,IsBullet,start,level,symbol,pListOr,flags,NULL);
}

/******************************************************************************
    TerGetBulletInfo2:
    Retrieve the bullet info. The id type can be PID_LINE, PID_PARA or PID_BULLET.
    This function returns the bullet id if successful. Otherwise it returns -1.
    The bullet id of 0 signifies that the line or paragraph do not use bullet
    property.
******************************************************************************/
int WINAPI _export TerGetBulletInfo3(HWND hWnd,int IdType, long id,LPINT IsBullet, LPINT start, LPINT level, LPINT symbol, LPINT pListOr, LPINT flags, LPBYTE pListText)
{
    PTERWND w;
    int BltId,CurPara;
    struct StrListLevel far *pLevel=NULL;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (IdType==PID_LINE) {
      if (id<-1 || id>=TotalLines) return -1;
      if (id==-1) id=CurLine;
      IdType=PID_PARA;
      id=pfmt((int)id);
    }
    if (IdType==PID_PARA) {
      if ((int)id<0 || (int)id>=TotalPfmts) return -1;
      IdType=PID_BULLET;
      CurPara=(int)id;
      id=PfmtId[CurPara].BltId;
      if (id==0 && !(PfmtId[CurPara].flags&BULLET)) return 0;   // not a bulleted para
    }
    else if (IdType==PID_STYLE) {
      if (id<0 || id>=TotalSID) return -1;
      id=StyleId[id].BltId;
      if (id==0 && !(StyleId[id].ParaFlags&BULLET)) return 0;   // not a bulleted para
    } 
    
    BltId=(int)id;
    if (BltId<0 || BltId>=TotalBlts) return -1;

    if (IsBullet) (*IsBullet)=TerBlt[BltId].IsBullet;
    
    if (pListOr)    (*pListOr)=TerBlt[BltId].ls;
    
    if (TerBlt[BltId].ls>0) pLevel=GetListLevelPtr(w,TerBlt[BltId].ls,TerBlt[BltId].lvl);
    
    if (pLevel && IsBullet) (*IsBullet)=(int)(pLevel->NumType==LIST_BLT);

    if (level) {
        if (pLevel) (*level)=TerBlt[BltId].lvl;
        else        (*level)=TerBlt[BltId].level;
    }
    if (start) {
       if (pLevel) (*start)=pLevel->start;
       else        (*start)=TerBlt[BltId].start;
    }
    if (flags) (*flags)=TerBlt[BltId].flags;

    if (symbol) {
       if (TerBlt[BltId].IsBullet) {
          BYTE chr=TerBlt[BltId].BulletChar;
          (*symbol)=BLT_ROUND;

          if (TerBlt[BltId].font==BFONT_SYMBOL) {
             if (chr==0xb7) (*symbol)=BLT_ROUND;
             if (chr==0xa8) (*symbol)=BLT_DIAMOND;
          }
          else if (TerBlt[BltId].font==BFONT_WINGDINGS) {
             if (chr==0xa7)  (*symbol)=BLT_SQUARE;
             if (chr=='q')   (*symbol)=BLT_HOLLOW_SQUARE;
             if (chr=='v')   (*symbol)=BLT_4_DIAMONDS;
             if (chr==0xd8)  (*symbol)=BLT_ARROW;
             if (chr==0xfc)  (*symbol)=BLT_CHECK;
          }
       }
       else {
          if (pLevel) {
             int NumType=pLevel->NumType;
             if      (NumType==LIST_DEC)       (*symbol)=NBR_DEC;
             else if (NumType==LIST_UPR_ALPHA) (*symbol)=NBR_UPR_ALPHA;
             else if (NumType==LIST_LWR_ALPHA) (*symbol)=NBR_LWR_ALPHA;
             else if (NumType==LIST_UPR_ROMAN) (*symbol)=NBR_UPR_ROMAN;
             else if (NumType==LIST_LWR_ROMAN) (*symbol)=NBR_LWR_ROMAN;
             else (*symbol)=NBR_DEFAULT;

             if (IsBullet) (*IsBullet)= (NumType==LIST_BLT);

             if (pListText!=null) DecodeListText(w,pLevel->text,pListText);
             if (IsBullet) {
                if (pListText==NULL) (*symbol)=BLT_ROUND;
                else {
                   BYTE chr=pListText[0];
                   if (chr==0xb7)  pListText[0]=(*symbol)=BLT_ROUND;
                   if (chr==0xa8)  pListText[0]=(*symbol)=BLT_DIAMOND;
                   if (chr==0xa7)  pListText[0]=(*symbol)=BLT_SQUARE;
                   if (chr=='q')   pListText[0]=(*symbol)=BLT_HOLLOW_SQUARE;
                   if (chr=='v')   pListText[0]=(*symbol)=BLT_4_DIAMONDS;
                   if (chr==0xd8)  pListText[0]=(*symbol)=BLT_ARROW;
                   if (chr==0xfc)  pListText[0]=(*symbol)=BLT_CHECK;
                }
             } 
          } 
          else (*symbol)=TerBlt[BltId].NumberType;
       }
    }
    return BltId;
}

/******************************************************************************
    TerSetParaShading:
    Set the paragraph shading.  The shading value must be specified between
    0 and 10000 (max).
******************************************************************************/
BOOL WINAPI _export TerSetParaShading(HWND hWnd,int shading,BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // validate input
    if (shading<0) shading=0;
    if (shading>10000) shading=10000;

    // update stylesheet if active
    if (EditingParaStyle) {
       StyleId[CurSID].shading=shading;
       return TRUE;
    }

    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // get new paraid
       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;

    if (repaint) {
       PaintTer(w);
       if (RulerPending) DrawRuler(w,FALSE);
    }
    else WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    TerSetParaSpacing:
    Set the paragraph spacing.  The spacing value must be specified in twips.
    The before and after spacing variables must be between 0 and 5000.
    The negative values for SpaceBetween specify exact spacing.
******************************************************************************/
BOOL WINAPI _export TerSetParaSpacing(HWND hWnd,int SpaceBefore, int SpaceAfter, int SpaceBetween, BOOL repaint)
{

    return TerSetParaSpacing2(hWnd,SpaceBefore,SpaceAfter,SpaceBetween,0,repaint);
}
 
/******************************************************************************
    TerSetParaSpacing2:
    Set the paragraph spacing.  The spacing value must be specified in twips.
    The before and after spacing variables must be between 0 and 5000.
    The negative values for SpaceBetween specify exact spacing.
******************************************************************************/
BOOL WINAPI _export TerSetParaSpacing2(HWND hWnd,int SpaceBefore, int SpaceAfter, int SpaceBetween, int LineSpacing, BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;
    UINT flags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    // validate input
    if (SpaceBefore<0)    SpaceBefore=0;
    if (SpaceBefore>5000) SpaceBefore=5000;
    if (SpaceAfter<0)     SpaceAfter=0;
    if (SpaceAfter>5000)  SpaceAfter=5000;
    if (SpaceBetween<-2000)SpaceBetween=-2000;
    if (SpaceBetween>2000)SpaceBetween=2000;


    GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // turnoff double spacing if 'SpaceBetween spacified'
       if (EditingParaStyle) {
          flags=StyleId[CurSID].ParaFlags;
       }
       else  {
          flags=PfmtId[pfmt(l)].flags;
       }

       if (SpaceBetween!=0) LineSpacing=0;
       if (LineSpacing!=0)  SpaceBetween=0;
       if (SpaceBetween || LineSpacing) flags=ResetUintFlag(flags,DOUBLE_SPACE);

       // update stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].SpaceBefore=SpaceBefore;
          StyleId[CurSID].SpaceAfter=SpaceAfter;
          StyleId[CurSID].SpaceBetween=SpaceBetween;
          StyleId[CurSID].LineSpacing=LineSpacing;
          StyleId[CurSID].ParaFlags=flags;
          return TRUE;
       }

       // get new paraid
       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         SpaceBefore,
                         SpaceAfter,
                         SpaceBetween,
                         LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         flags);
    }

    TerArg.modified++;

    if (repaint) {
       PaintTer(w);
       if (RulerPending) DrawRuler(w,FALSE);
    }
    else WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;
}

/******************************************************************************
    TerSetPFlags:
    Set PFLAG_ styles for the paragraph.
    Set the paragraph spacing.  The spacing value must be specified in twips.
    The before and after spacing variables must be between 0 and 5000.
    The negative values for SpaceBetween specify exact spacing.
******************************************************************************/
BOOL WINAPI _export TerSetPflags(HWND hWnd,UINT FmtType,BOOL OnOff, BOOL repaint)
{
    long l,FirstLine,LastLine;
    unsigned pflags;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;


    FmtType=FmtType&PFLAG_STYLES;            // allow only user flags
    if (FmtType==0) return FALSE;

    // get the range of lines to modify
    GetParaRange(w,&FirstLine,&LastLine);     // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {     // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       if (EditingParaStyle) pflags=StyleId[CurSID].pflags;
       else                  pflags=PfmtId[pfmt(l)].pflags;

       if (OnOff) pflags=pflags|FmtType;      // toggle on
       else       pflags=pflags&(~FmtType);   // toggle off

       // update stylesheet if active
       if (EditingParaStyle) {
          StyleId[CurSID].pflags=pflags;
          return TRUE;
       }

       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         PfmtId[pfmt(l)].AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;
    if (TerArg.PrintView && FmtType&PFLAG_WIDOW && TotalLines<SMALL_FILE)
          Repaginate(w,FALSE,FALSE,0,TRUE);  // repaginate now
    else  RepageBeginLine=FirstLine;

    if (repaint) PaintTer(w);
    else         WordWrap(w,FirstLine,LastLine+10-FirstLine);

    return TRUE;

}


/******************************************************************************
    TerSetParaAuxId:
    Set the paragraph aux id. When FirstLine is -1, the routine selects
    the current paragraph or all selected paragraph (if text highlighted).
    This function does not repaint the screen.
******************************************************************************/
BOOL WINAPI _export TerSetParaAuxId(HWND hWnd,long FirstLine, long LastLine, UINT AuxId)
{
    PTERWND w;
    long l;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    if (FirstLine<0) GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) {   // apply new paragraph ids
       if (!LineSelected(w,l)) continue;

       // get new paraid
       pfmt(l)=NewParaId(w,pfmt(l),
                         PfmtId[pfmt(l)].LeftIndentTwips,
                         PfmtId[pfmt(l)].RightIndentTwips,
                         PfmtId[pfmt(l)].FirstIndentTwips,
                         PfmtId[pfmt(l)].TabId,
                         PfmtId[pfmt(l)].BltId,
                         AuxId,
                         PfmtId[pfmt(l)].Aux1Id,
                         PfmtId[pfmt(l)].StyId,
                         PfmtId[pfmt(l)].shading,
                         PfmtId[pfmt(l)].pflags,
                         PfmtId[pfmt(l)].SpaceBefore,
                         PfmtId[pfmt(l)].SpaceAfter,
                         PfmtId[pfmt(l)].SpaceBetween,
                         PfmtId[pfmt(l)].LineSpacing,
                         PfmtId[pfmt(l)].BkColor,
                         PfmtId[pfmt(l)].BorderSpace,
                         PfmtId[pfmt(l)].flow,
                         PfmtId[pfmt(l)].flags);
    }

    TerArg.modified++;

    return TRUE;
}

/******************************************************************************
    TerSetParaId:
    Set the paragraph id. When FirstLine is -1, the routine selects
    the current paragraph or all selected paragraph (if text highlighted).
    This function does not repaint the screen.
******************************************************************************/
BOOL WINAPI _export TerSetParaId(HWND hWnd,long FirstLine, long LastLine, int ParaId)
{
    PTERWND w;
    long l;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;
    if (ParaId<0 || ParaId>=TotalPfmts) return FALSE;

    if (FirstLine<0) GetParaRange(w,&FirstLine,&LastLine); // get the range of the lines that would be affected
    if (!EditingParaStyle) SaveUndo(w,FirstLine,0,LastLine,0,'P');

    for (l=FirstLine;l<=LastLine;l++) pfmt(l)=ParaId;

    TerArg.modified++;

    return TRUE;
}

/******************************************************************************
    TerBulleToText:
    Convert bullet, numbering and list numbering to regular text
******************************************************************************/
BOOL WINAPI _export TerBulletToText(HWND hWnd,BOOL all, BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine,m;
    int ParaId,BltId,font,NewPara,ListOrId;
    BYTE BltText[MAX_WIDTH+1];
    UINT flags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;

    if (!all && HilightType==HILIGHT_OFF) return FALSE;

    FirstLine=0;
    LastLine=TotalLines-1;
    if (!all) {
       NormalizeBlock(w);
       FirstLine=HilightBegRow;
       LastLine=HilightEndRow;
    } 

    for (l=LastLine;l>=FirstLine;l--) {          // go reverse so the that numbereing algorithm still work
       if (False(LineFlags(l)&LFLAG_PARA_FIRST)) continue;  // not the first line of a paragraph
       
       ParaId=pfmt(l);
       BltId=PfmtId[ParaId].BltId;
       if (BltId==0 && False(PfmtId[ParaId].flags&BULLET)) continue;

       // get the bullet string
       DrawBullet(w,hTerDC,hTerDC,l,ParaId,0,0,0,false);
       strcpy(BltText,DlgText1);
       font=DlgInt1;
       if (strlen(BltText)==0) continue;


       // check if a tab needs to be added
       BltId=PfmtId[ParaId].BltId;
       ListOrId=TerBlt[BltId].ls;
       if (ListOrId==0) {              // add a tab for the regular bullet and bumbering
          lstrcat(BltText,"\t");
       }
       else {
          struct StrListLevel far *pLevel;
          if ((pLevel=GetListLevelPtr(w,ListOrId,TerBlt[BltId].lvl))!=NULL) {
             if (pLevel->CharAft==LISTAFT_TAB) strcat(BltText,"\t");
          } 
       
       } 
          


       // insert the text string
       SetTerCursorPos(hTerWnd,l,0,false);
       TerInsertText(hTerWnd,BltText,font,ParaId,false);

       // reset the bullet id from the paragraph
       flags=ResetUintFlag(PfmtId[ParaId].flags,BULLET);

       NewPara=NewParaId(w,ParaId,
                         PfmtId[ParaId].LeftIndentTwips,
                         PfmtId[ParaId].RightIndentTwips,
                         PfmtId[ParaId].FirstIndentTwips,
                         PfmtId[ParaId].TabId,
                         0,
                         PfmtId[ParaId].AuxId,
                         PfmtId[ParaId].Aux1Id,
                         PfmtId[ParaId].StyId,
                         PfmtId[ParaId].shading,
                         PfmtId[ParaId].pflags,
                         PfmtId[ParaId].SpaceBefore,
                         PfmtId[ParaId].SpaceAfter,
                         PfmtId[ParaId].SpaceBetween,
                         PfmtId[ParaId].LineSpacing,
                         PfmtId[ParaId].BkColor,
                         PfmtId[ParaId].BorderSpace,
                         PfmtId[ParaId].flow,
                         flags);
       for (m=l;m<TotalLines;m++) {             // apply the new paragraph to all lines in the current paragraph
          if (pfmt(m)==ParaId) pfmt(m)=NewPara;
          if (LineFlags(m)&LFLAG_PARA) break;
       } 

    } 

    if (repaint) PaintTer(w);
    return TRUE;
}

/*******************************************************************************
    GetFirstIndent:
    Get first line indentation adjustment.
*******************************************************************************/
int GetFirstIndent(PTERWND w,long LineNo,LPINT pBulletWidth, BOOL far *pHasBullet, BOOL screen)
{
    BOOL HasBullet=FALSE;
    int  CurPara=pfmt(LineNo);
    int indent=0,BulletWidth=0;

    if (pHasBullet)     (*pHasBullet)=FALSE;
    if (pBulletWidth)   (*pBulletWidth)=0;
    
    if (screen) indent=TwipsToScrX(PfmtId[CurPara].FirstIndentTwips);          
    else        indent=TwipsToPrtX(PfmtId[CurPara].FirstIndentTwips);          

    if (LineFlags(LineNo)&LFLAG_LIST) {
       BulletWidth=(tabw(LineNo)?tabw(LineNo)->ListTextWidth:0);
       if (TerArg.PageMode && !TerArg.FittedView && screen) BulletWidth=PrtToScrX(BulletWidth);
       indent+=BulletWidth;
       HasBullet=TRUE;
    }
    else if (PfmtId[CurPara].flags&BULLET && PfmtId[CurPara].FirstIndent<0) {
       int BltId=PfmtId[CurPara].BltId;
       BOOL IsHidden=FALSE;
       if (TerBlt[BltId].flags&BLTFLAG_HIDDEN) IsHidden=TRUE;
       if (!IsHidden) {
         BulletWidth=-indent;
         indent=0;                              // negative indent space will be used to print the bullet
         HasBullet=TRUE; // bulleted line
       }
    }
 
    if (pHasBullet)     (*pHasBullet)=HasBullet;
    if (pBulletWidth)   (*pBulletWidth)=BulletWidth;
 
    return indent;
}
 

