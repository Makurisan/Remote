/*==============================================================================
   TER Editor Developer's Kit
   TER4.C
   Text editing functions

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
    ActivateWheel:
    Activate the mouse wheel
*******************************************************************************/
ActivateWheel(PTERWND w,LPARAM lParam)
{

    if (CaretEngaged) DisengageCaret(w); // disengage the caret
    if (PictureClicked) {            // disable any picture hilighting
       HilightType=HILIGHT_OFF;
       PictureClicked=FALSE;
    }
    
    WheelShowing=TRUE;
    WheelX=(short)LOWORD(lParam); // casting preserves sign
    WheelY=(short)HIWORD(lParam);
    
    hWheelCur=hWheelFullCur;      // show the full wheel cursor to start with
    TerSetCursorShape(w,lParam,FALSE);

    PaintTer(w);

    return TRUE;
}
 
/******************************************************************************
    SetWheelTimer:
    Set the wheel timer.
*******************************************************************************/
SetWheelTimer(PTERWND w, LPARAM lParam, BOOL reset)
{
   int y,UnitTime=100,time;
   
   if (WheelTimerUpOn) {                       // reset any up timer
      KillTimer(hTerWnd,TIMER_WHEEL_UP);
      WheelTimerUpOn=0;
   }
   if (WheelTimerDownOn) {                     // reset any down timer
      KillTimer(hTerWnd,TIMER_WHEEL_DOWN);
      WheelTimerDownOn=0;
   }
   if (reset) return TRUE;    // just had to reset

   hWheelCur=hWheelFullCur;      // show the full wheel cursor to start with

   // calculate the timer direction and speel speed
   y=(short)HIWORD(lParam);
   if (y<=(WheelY-WHEEL_RADIUS)) {
      hWheelCur=hWheelUpCur;               // wheel up cursor
      time=UnitTime/(((WheelY-y)/WHEEL_RADIUS));
      WheelTimerUpOn=SetTimer(hTerWnd,TIMER_WHEEL_UP,time,NULL);
   } 
   else if (y>=(WheelY+WHEEL_RADIUS)) {
      hWheelCur=hWheelDownCur;             // wheel down cursor
      time=UnitTime/(((y-WheelY)/WHEEL_RADIUS));
      WheelTimerDownOn=SetTimer(hTerWnd,TIMER_WHEEL_DOWN,time,NULL);
   } 

   return TRUE;
}
 
/******************************************************************************
    ResetWheel:
    Reset the mouse wheen.
*******************************************************************************/
ResetWheel(PTERWND w)
{
    if (!WheelShowing) return TRUE;

    SetWheelTimer(w,0,TRUE);       // reset the wheel timer

    WheelShowing=FALSE;
    
    if (hWheelCur) {
       hWheelCur=NULL;            // reset the wheel cursor
       TerSetCursorShape(w,0,FALSE);
    }
    
    PaintTer(w);

    return TRUE;
}
 
/******************************************************************************
    SetAutoNumLgl:
    Set autonumlgl text for the fields in the specified line
******************************************************************************/
SetAutoNumLgl(PTERWND w, long LineNo, BOOL UsePrtUnits)
{
   BYTE text[100]="",prefix[100];
   int LineHdng,CurHdng,nbr;     // current style heading
   int StyId;
   long l;

   // get the heading style for the current line
   LineHdng=0;
   StyId=PfmtId[pfmt(LineNo)].StyId;
   if (StyId) LineHdng=GetHeadingNo(w,StyId);

   // scan backward to find the previous number
   for (l=LineNo-1;l>=0;l--) {
      if (!(LineFlags(l)&LFLAG_AUTONUMLGL)) continue;
      if (!tabw(l) || !tabw(l)->pAutoNumLgl) continue;


      CurHdng=0;
      StyId=PfmtId[pfmt(l)].StyId;
      if (StyId) CurHdng=GetHeadingNo(w,StyId);
   
      if (LineHdng>0 && (CurHdng==0 || CurHdng>LineHdng)) continue;

      if (CurHdng==LineHdng) {   // advance to next number
         CrackAutoNumLgl(w,tabw(l)->pAutoNumLgl,prefix,&nbr);   // get prefix and nbr from the whole numbe
         MakeAutoNumLgl(w,text,prefix,nbr+1,CurHdng,LineHdng);
      }
      else MakeAutoNumLgl(w,text,tabw(l)->pAutoNumLgl,1,CurHdng,LineHdng);  // start a new series
      
      break;
   } 

   if (lstrlen(text)==0) MakeAutoNumLgl(w,text,"",1,0,LineHdng);  // first number


   // copy the number string
   if (tabw(LineNo)->pAutoNumLgl) MemFree(tabw(LineNo)->pAutoNumLgl);
   tabw(LineNo)->pAutoNumLgl=MemAlloc(lstrlen(text)+1);
   lstrcpy(tabw(LineNo)->pAutoNumLgl,text);
 
   return TRUE;
}
 

/******************************************************************************
    SetListnum:
    Set listnum for the fields in the specified line
******************************************************************************/
SetListnum(PTERWND w, long LineNo, BOOL UsePrtUnits)
{
   LPWORD fmt;
   int i,len,PrevFont,CurFont,FieldNbr,FieldId;
   BYTE text[100];
   long ListNbr;
   struct StrListnum far *pListnum;
   int ListOrId,level,BltId;
   LPBYTE ptr;
   BYTE CurChar;

   fmt=OpenCfmt(w,LineNo);
   ptr=pText(LineNo);
 
   FieldNbr=-1;
   CurFont=0;
   len=LineLen(LineNo);

   for (i=0;i<len;i++) {
      CurChar=ptr[i];
      PrevFont=CurFont; 
      CurFont=fmt[i];

      if (i==0 ||  PrevFont!=CurFont || CurChar==HPARA_CHAR) {
         if (CurChar==HPARA_CHAR) {
            int CurPara;
            if (0==GetTag(w,LineNo,i,TERTAG_HPARA,NULL,NULL,&CurPara)) continue;  // no tag found
            if (CurPara<0 || CurPara>=TotalPfmts) continue;
            if ((BltId=PfmtId[CurPara].BltId)==0) continue;
            if (TerBlt[BltId].ls==0) continue;
         }
         else { 
            FieldId=TerFont[CurFont].FieldId;
            if (FieldId!=FIELD_LISTNUM) continue;
         }

         FieldNbr++;   // next field number
         if (!tabw(LineNo)) AllocTabw(w,LineNo);
         if (tabw(LineNo)->ListnumCount==0) tabw(LineNo)->pListnum=MemAlloc((FieldNbr+1)*sizeof(struct StrListnum));
         else                               tabw(LineNo)->pListnum=MemReAlloc(tabw(LineNo)->pListnum,(FieldNbr+1)*sizeof(struct StrListnum));
         
         tabw(LineNo)->ListnumCount=FieldNbr;      // count before this field
         
         if (CurChar==HPARA_CHAR) {
            ListOrId=TerBlt[BltId].ls;
            level=TerBlt[BltId].lvl;
         }
         else GetFieldList(w,CurFont,LineNo,&ListOrId,&level);
         
         tabw(LineNo)->ListnumCount=FieldNbr+1;    // count including this field

         pListnum=tabw(LineNo)->pListnum;
         pListnum[FieldNbr].ls=ListOrId;
         pListnum[FieldNbr].lvl=level;
         pListnum[FieldNbr].IsHPARA=(CurChar==HPARA_CHAR);

         GetListText(w,pfmt(LineNo),LineNo,text,NULL,&ListNbr,NULL,0,FieldNbr,UsePrtUnits);

         // store the list info in the tabw structure
         pListnum=tabw(LineNo)->pListnum;
         lstrcpy(pListnum[FieldNbr].text,text);
         pListnum[FieldNbr].ListNbr=ListNbr;
      }
   } 
 
   return TRUE;
}
 
/******************************************************************************
     GetFieldList:
     Get the listOr id and level for the given field id.  This function
     builds the list and ListOd if they don't already exist.
******************************************************************************/
GetFieldList(PTERWND w,int FieldFont,long LineNo, LPINT pListOrId,LPINT pLevel)
{
    LPBYTE ListParam;
    int ListOrId=0,len,i,k;
    int level=0,ParaId,BltId=0;
    int StartAt=1,ListId;
    BYTE param[100],name[100];
    int FieldId=TerFont[FieldFont].FieldId;
    int OverrideStartAt;
    long l;
    BOOL ListSetForCurPara;
    
    ListSetForCurPara=true;                    
    for (l=LineNo;l>=0;l--) {              // scan backward
       ParaId=pfmt(l);
       BltId=PfmtId[ParaId].BltId;
       if (BltId) break;
       if (True(LineFlags(l)&LFLAG_PARA_FIRST)) ListSetForCurPara=false;   // going over to the previous paragraph
    }
    if (BltId==0) ListSetForCurPara=false;    

    if (BltId==0) {                          // scan forward
       for (l=LineNo;l<TotalLines;l++) {     // get the bulle id to use
         ParaId=pfmt(l);
         BltId=PfmtId[ParaId].BltId;
         if (BltId) break;
       }
    }
         
    ListParam=TerFont[FieldFont].FieldCode;
    
    if (ListParam) StrTrim(ListParam);

    // set the default level to the level of the previous listnum
    level=0;  // default level
    for (l=LineNo-1;l>=0;l--) {
       if (LineFlags(l)&LFLAG_LISTNUM && tabw(l) 
          && tabw(l)->ListnumCount>0 && tabw(l)->pListnum)  {
          level=tabw(l)->pListnum[tabw(l)->ListnumCount-1].lvl;  // continue with the list level of the previous list number
          break;
       }
        
       ParaId=pfmt(l);
       if (PfmtId[ParaId].BltId) {
          level=TerBlt[BltId].lvl;   // if another list is encountered before a list number, than continue with that level
          break;     // listnum not found
       }

    } 

    if (!ListParam || lstrlen(ListParam)==0) {         // no list parameter
       if (BltId>0) {
          ListOrId=TerBlt[BltId].ls;

          if (ListSetForCurPara) {  // use the next heigher level of the list on the current line
             ParaId=pfmt(LineNo);
             BltId=PfmtId[ParaId].BltId;
             level=TerBlt[BltId].lvl+1;
             if (level>8) level=8;
          }
          goto END;
       }
    }                               
 

    // extract the list name
    if (ListParam) lstrcpy(param,ListParam);
    else           param[0]=0;
    lstrupr(w,param);
    StrTrim(param);

    len=lstrlen(param);
    for (i=0;(i<len && param[i]!=' ' && param[i]!='\\');i++) name[i]=param[i];
    name[i]=0;
    StrTrim(name);

    // check if the field list name is same as the current line list name
    if (lstrlen(name)>0 && BltId && TerBlt[BltId].ls) {
       int CurListId=ListOr[TerBlt[BltId].ls].ListIdx;
       if (CurListId>=0 && CurListId<TotalLists && list[CurListId].name && lstrcmpi(list[CurListId].name,name)==0) {
          ListOrId=TerBlt[BltId].ls;
          level=TerBlt[BltId].lvl+1;
       }
    } 
    
    if (lstrlen(name)==0) {          // use the current list
       ListOrId=TerBlt[BltId].ls;
       level=TerBlt[BltId].lvl;
    } 

    StartAt=(int)GetFieldSwitchLong(w,param,"\\S",1,&OverrideStartAt);
    level=(int)GetFieldSwitchLong(w,param,"\\L",level+1,NULL)-1;
    

    if (lstrlen(name)==0) {
       if (BltId>0) {
          ListOrId=TerBlt[BltId].ls;
          goto END;
       }
       else { 
          lstrcpy(name,"NumberDefault");
          if (lstrlen(param)==0) {         // find the level 
              if (tabw(LineNo)) {
                 if (!tabw(LineNo)->pListnum || tabw(LineNo)->ListnumCount==0) level=0;
                 else {
                    int PrevList=tabw(LineNo)->ListnumCount-1;
                    struct StrListnum far *pListnum=tabw(LineNo)->pListnum;
                    level=pListnum[PrevList].lvl+1;
                 }
              }
          }
       }    
    }

    if (level<0) level=0;
    if (level>8) level=8;


    //if (lstrlen(name)==0) goto END;

    // locate the list id
    for (i=0;i<TotalLists;i++) {
      if (list[i].InUse && list[i].name && lstrcmpi(list[i].name,name)==0) break;
    } 

    if (i<TotalLists) ListId=i;
    else              ListId=CreateDefList(w,name);   // create a default list and return the list id

    // locate the ListOr id
    for (i=0;i<TotalListOr;i++) {
       if (ListOr[i].ListIdx!=ListId) continue;

       if (ListOr[i].LevelCount>0) {
          for (k=0;k<ListOr[i].LevelCount;k++) {  // check if format overridden
             if (ListOr[i].level[k].flags&LISTLEVEL_REFORMAT) break;
          }
          if (k<ListOr[i].LevelCount) continue; // reformatted ListOr not acceptable 
       }    

       if (OverrideStartAt) {
          if (level>=ListOr[i].LevelCount) continue;    // level must be provide by Listor
          if (ListOr[i].level[level].start==StartAt && ListOr[i].level[level].flags&LISTLEVEL_RESTART) break;
       }
       else {
          if (level<ListOr[i].LevelCount) {    // level provide by ListOr
            if (ListOr[i].level[level].flags&LISTLEVEL_RESTART) break;  
          }
          else if (level<list[ListId].LevelCount) break;  // level provide by list
       }
    } 

    if (i<TotalListOr) ListOrId=i;
    else               ListOrId=CreateDefListOr(w,ListId,StartAt,OverrideStartAt,level);

    END:

    if (pListOrId) (*pListOrId)=ListOrId;
    if (pLevel)  (*pLevel)=level;

    return TRUE;
}

/******************************************************************************
    CreateDefList:
    Create a default list and return the list id.
******************************************************************************/
int CreateDefList(PTERWND w,LPBYTE name)
{
    int CurList,i,j,k,LevelCount=9,PairCount;
    long ListId=0,ListTmplId=0;
    struct StrListLevel far *pLevel;
    LPBYTE pText;

    // generate list id and list template for the new list
    for (i=0;i<TotalLists;i++) {
       if (list[i].id>ListId) ListId=list[i].id;
       if (list[i].TmplId>ListTmplId) ListTmplId=list[i].TmplId;
    }
    ListId++;                  // make is unique
    ListTmplId++;
     
    if ((CurList=GetListSlot(w))<0) return 0;

    list[CurList].InUse=TRUE;
    list[CurList].id=ListId;
    list[CurList].TmplId=ListTmplId;
    list[CurList].FontId=0;
    list[CurList].LevelCount=LevelCount;
    list[CurList].flags=0;
    list[CurList].name=MemAlloc(lstrlen(name)+1);
    lstrcpy(list[CurList].name,name);
    list[CurList].level=MemAlloc(LevelCount*sizeof(struct StrListLevel));
    FarMemSet(list[CurList].level,0,LevelCount*sizeof(struct StrListLevel));

    // initialize the levels
    pLevel=list[CurList].level;

    for (i=0;i<LevelCount;i++) { // common fields
       pLevel[i].start=1;
       pLevel[i].CharAft=LISTAFT_NONE;
    } 
    
    if (lstrcmpi(name,"NumberDefault")==0) {
       pLevel[0].NumType=pLevel[3].NumType=pLevel[6].NumType=LIST_DEC;
       pLevel[1].NumType=pLevel[4].NumType=pLevel[7].NumType=LIST_LWR_ALPHA;
       pLevel[2].NumType=pLevel[5].NumType=pLevel[8].NumType=LIST_LWR_ROMAN;

       pText=pLevel[0].text;  pText[0]=2; pText[1]=0; pText[2]=')';
       pText=pLevel[1].text;  pText[0]=2; pText[1]=1; pText[2]=')';
       pText=pLevel[2].text;  pText[0]=2; pText[1]=2; pText[2]=')';
       pText=pLevel[3].text;  pText[0]=3; pText[1]='('; pText[2]=3; pText[3]=')';
       pText=pLevel[4].text;  pText[0]=3; pText[1]='('; pText[2]=4; pText[3]=')';
       pText=pLevel[5].text;  pText[0]=3; pText[1]='('; pText[2]=5; pText[3]=')';
       pText=pLevel[6].text;  pText[0]=2; pText[1]=0; pText[2]='.';
       pText=pLevel[7].text;  pText[0]=2; pText[1]=1; pText[2]='.';
       pText=pLevel[8].text;  pText[0]=2; pText[1]=2; pText[2]='.';
    } 
    else if (lstrcmpi(name,"OutlineDefault")==0) {
       pLevel[0].NumType=LIST_UPR_ROMAN;
       pLevel[1].NumType=LIST_UPR_ALPHA;
       pLevel[2].NumType=pLevel[4].NumType=LIST_DEC;
       pLevel[3].NumType=pLevel[5].NumType=pLevel[7].NumType=LIST_LWR_ALPHA;
       pLevel[6].NumType=pLevel[8].NumType=LIST_LWR_ROMAN;
    
       pText=pLevel[0].text;  pText[0]=2; pText[1]=0; pText[2]='.';
       pText=pLevel[1].text;  pText[0]=2; pText[1]=1; pText[2]='.';
       pText=pLevel[2].text;  pText[0]=2; pText[1]=2; pText[2]='.';
       pText=pLevel[3].text;  pText[0]=2; pText[1]=3; pText[2]=')';
       pText=pLevel[4].text;  pText[0]=3; pText[1]='('; pText[2]=4; pText[3]=')';
       pText=pLevel[5].text;  pText[0]=3; pText[1]='('; pText[2]=5; pText[3]=')';
       pText=pLevel[6].text;  pText[0]=3; pText[1]='('; pText[2]=6; pText[3]=')';
       pText=pLevel[7].text;  pText[0]=3; pText[1]='('; pText[2]=7; pText[3]=')';
       pText=pLevel[8].text;  pText[0]=3; pText[1]='('; pText[2]=8; pText[3]=')';

    }
    else {
       for (i=0;i<9;i++) pLevel[i].NumType=LIST_DEC;

       for (i=0;i<9;i++) {
          pText=pLevel[i].text;
          PairCount=i+1;             // pair consists of 1.
          pText[0]=PairCount*2;      // length of the field
          j=1;

          for (k=0;k<PairCount;k++,j++) {
             pText[j]=k;
             j++;
             pText[j]='.';
          } 
       } 
    }  


    return CurList;
}

/******************************************************************************
    CreateDefListOr:
    Create a default listor for the given list and returns the list id.
******************************************************************************/
int CreateDefListOr(PTERWND w,int ListId, int StartAt, BOOL OverrideStartAt,int level)
{
   int CurListOr,LevelCount,i,ParentLevel;
 
   if ((CurListOr=GetListOrSlot(w))<0) return 0;

   ListOr[CurListOr].InUse=TRUE;
   ListOr[CurListOr].ListIdx=ListId;

   LevelCount=(level==0 && !OverrideStartAt)?0:9;
   ListOr[CurListOr].LevelCount=LevelCount;

   if (LevelCount==0) return CurListOr;

   ListOr[CurListOr].level=MemAlloc(LevelCount*sizeof(struct StrListLevel));

   for (i=0;i<LevelCount;i++) {
      if (i<list[ListId].LevelCount) ParentLevel=i;
      else                           ParentLevel=list[ListId].LevelCount-1;
      FarMove(&(list[ListId].level[ParentLevel]),&(ListOr[CurListOr].level[i]),sizeof(struct StrListLevel));
   
      if (OverrideStartAt && level==i) {
         ListOr[CurListOr].level[i].flags|=LISTLEVEL_RESTART;
         ListOr[CurListOr].level[i].start=StartAt;
      } 
   } 
   
   return CurListOr;
}
 
/******************************************************************************
    GetFieldSwitchLong:
    Get the value of a numberic (long) parameter for a field switch.
******************************************************************************/
long GetFieldSwitchLong(PTERWND w,LPBYTE ParamList,LPBYTE param,long DefValue, LPINT pFound)
{
    long result=DefValue;
    LPBYTE ptr;
    int i,j;
    BYTE string[100];

    if (pFound) (*pFound)=FALSE;

    if (NULL==(ptr=strstr(ParamList,param))) return result;
    
    ptr=&(ptr[strlen(param)]);  // go past the param

    for (i=0;ptr[i]!=0;i++) if (ptr[i]!=' ') break;  // go past the spaces

    j=0;
    for (;ptr[i]!=0 && ptr[i]!=' ';i++,j++) string[j]=ptr[i];
    string[j]=0;
    
    if (pFound) (*pFound)=TRUE;


    return atol(string);
}
 
/******************************************************************************
    MakeAutoNumLgl:
    Build autonumlgl text from prefix and ending number
******************************************************************************/
MakeAutoNumLgl(PTERWND w, LPBYTE out, LPBYTE prefix, int nbr,int FromHdng,int ToHdng)
{
    int i,len;

    lstrcpy(out,prefix);
    len=lstrlen(out);
    if (len>0 && out[len-1]!='.') {
       lstrcat(out,".");  // append a delimiter
       len=lstrlen(out);
    }
      
    if ((FromHdng+1)<ToHdng) {
       for (i=(FromHdng+1);i<ToHdng;i++) lstrcat(out,"0.");    // provide for missing heading levels
       len=lstrlen(out);
    }
     
    wsprintf(&(out[len]),"%d",nbr);
 
 
    return TRUE;
}
 
/******************************************************************************
    CrackAutoNumLgl:
    Split autonumlgl text into prefix and ending number
******************************************************************************/
CrackAutoNumLgl(PTERWND w, LPBYTE NbrText, LPBYTE prefix, LPINT pNbr)
{
    int nbr=0,idx;
    BYTE text[100];

    prefix[0]=0;
    (*pNbr)=0;
    lstrcpy(text,NbrText);    // make a local copy

    idx=lstrlen(text)-1;   // index of the last letter
    if (idx<0) return TRUE;

    if (text[idx]=='.') {  // discard any ending '.'
       text[idx]=0;
       idx--;
       if (idx<0) return TRUE;
    } 

    // look for preceeding '.' character
    for (;text[idx]!='.' && idx>=0;idx--);
    
    nbr=atoi(&(text[idx+1]));
    text[idx+1]=0;
    
    lstrcpy(prefix,text);  
    (*pNbr)=nbr;

    return TRUE;
}
 
/******************************************************************************
    GetHeadingNo:
    Get the heading number for the style id.
******************************************************************************/
GetHeadingNo(PTERWND w, int CurStyle)
{
    BYTE suffix;
    int len;

    if (strstr(StyleId[CurStyle].name,"heading ")!=StyleId[CurStyle].name) return 0;  // not a heading style
    len=lstrlen("heading ");
    if (lstrlen(StyleId[CurStyle].name)>(len+1)) return 0;  // only 1 to 9 supported
    suffix=StyleId[CurStyle].name[len];

    return  suffix-'0';       // return heading 1 to 9
}

/******************************************************************************
     IsTextPosVisible:
     Check if a text position is on the screen.
******************************************************************************/
BOOL IsTextPosVisible(PTERWND w,long line, int col)
{
    int x,frm;

    if (!TerArg.PageMode) return (line>=BeginLine && line<=(BeginLine+WinHeight));
 
    if ((frm=GetFrame(w,line))==-1) return FALSE;   // line in the frame set

    if (line<frame[frm].ScrFirstLine || line>frame[frm].ScrLastLine) return FALSE;

    x=ColToUnits(w,col,line,TER_LEFT);

    return (x>=TerWinOrgX && x<=(TerWinOrgX+TerWinWidth));
}
 
/******************************************************************************
     CrackUnicodeString:
     Crack a unicode string into unicode base and regular character string
******************************************************************************/
CrackUnicodeString(PTERWND w,LPWORD UText, LPWORD UcBase, LPBYTE text)
{
     int i;

     for (i=0;UText[i];i++) CrackUnicode(w,UText[i],&(UcBase[i]),&(text[i]));
  
     text[i]=0;
     UcBase[i]=0;

     return i;              // return the length of the string
}
 
/******************************************************************************
     CrackUnicode:
     Crack a unicode character into unicode base and regular character
******************************************************************************/
CrackUnicode(PTERWND w,WORD UChar, LPWORD pUcBase, LPBYTE pByte)
{
     BYTE lo;
     WORD hi;

     lo=(BYTE)(UChar&0xFF);
     hi=((UChar>>8)<<8);
 
     if (hi>0 && lo<128)  {       // ensure that regular character is in the upper 128 area
       hi-=128;
       lo+=128;
     }
     
     if (hi==0 && lo>=128) hi=1;  // to flag a one byte unicode character
    
     if (pByte) (*pByte)=lo;
     if (pUcBase) (*pUcBase)=hi;
     
     return TRUE; 
}
 
/******************************************************************************
     MakeUnicode:
     Crack a unicode character into unicode base and regular character
******************************************************************************/
WORD MakeUnicode(PTERWND w,WORD UcBase, BYTE byte)
{
    if (UcBase==1) UcBase=0;  // 1 flags a one byte unicode character
    return (UcBase+byte);
} 

/******************************************************************************
     GetShadedColor:
     Get shaded color for the specified shading (0 to 100)
******************************************************************************/
COLORREF GetShadedColor(PTERWND w, COLORREF FcColor, COLORREF BcColor, int shade)
{
    BYTE red,green,blue;

    red=GetShadedColorComp(w,GetRValue(FcColor),GetRValue(BcColor),shade);
    green=GetShadedColorComp(w,GetGValue(FcColor),GetGValue(BcColor),shade);
    blue=GetShadedColorComp(w,GetBValue(FcColor),GetBValue(BcColor),shade);
 
    return RGB(red,green,blue);
}

/******************************************************************************
    GetShadedColorComp:
    Get shaded color component for the specified shading (0 to 100)
******************************************************************************/
BYTE GetShadedColorComp(PTERWND w,BYTE FcColor, BYTE BcColor, int shade)
{
    BYTE result;
    int diff;

    if (FcColor==BcColor) return FcColor;

    diff=(int)(WORD)BcColor-(int)(WORD)FcColor;
    result=(BYTE)(WORD)((int)(WORD)BcColor-MulDiv(diff,shade,100));

    return result;
} 

/******************************************************************************
     TerGetVisibleCol:
     Given the column number for a line, this function returns the visible 
     column number.
******************************************************************************/
int WINAPI _export TerGetVisibleCol(HWND hWnd, long line, int col)
{
    PTERWND w;
    LPWORD fmt;
    int DispCol=0,i,len;
    int PrevFont=-1;
    BOOL visible=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;      // get the pointer to window data

    if (line<0) line=CurLine;
    if (col<0) col=CurCol;

    if (line>=TotalLines) return -1;

    fmt=OpenCfmt(w,line);
    len=LineLen(line);
    if (col>=len) col=len-1;

    DispCol=0;
    for (i=0;i<col;i++) {
       if ((int)fmt[i]!=PrevFont) visible=!HiddenText(w,fmt[i]);
       PrevFont=fmt[i];
       if (visible) DispCol++;
    } 

    CloseCfmt(w,line);

    return DispCol;
}

/******************************************************************************
     TerGetHypertext:
     This function gets any hypertext phrase and hypertext code at the current
     cursor position.  The function return FALSE if hypertext is not found.
******************************************************************************/
BOOL WINAPI _export TerGetHypertext(HWND hWnd, LPBYTE text, LPBYTE code)
{
    PTERWND w;
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;      // get the pointer to window data

    if (RulerClicked) return FALSE;

    return TerGetHypertextEx(hWnd,text,code,FALSE);
}

/******************************************************************************
     TerGetHypertextEx:
     This function gets any hypertext phrase and hypertext code at the current
     cursor position.  The function return FALSE if hypertext is not found.
******************************************************************************/
BOOL WINAPI _export TerGetHypertextEx(HWND hWnd, LPBYTE text, LPBYTE code, BOOL select)
{
     return TerGetHypertext2(hWnd,-1,0,text,code,select);
}

/******************************************************************************
     TerGetHypertext2:
     This function gets any hypertext phrase and hypertext code at the current
     cursor position.  The function return FALSE if hypertext is not found.
******************************************************************************/
BOOL WINAPI _export TerGetHypertext2(HWND hWnd, long LineNo, int ColNo, LPBYTE text, LPBYTE code, BOOL select)
{
    PTERWND w;
    int i,j,len,CurLen,CurCfmt,BegCol,EndCol,StartCol,LastCol,TextCol,CodeCol;
    LPWORD fmt;
    LPBYTE ptr,lead;
    long l,BegLine,EndLine,TextLine,CodeLine;
    BOOL  CursBefHidden;
    BOOL  HiddenAfterText;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;      // get the pointer to window data

    if (LineNo<0) {          // supply the default
       LineNo=CurLine;
       ColNo=CurCol;
    }

    if (LineLen(LineNo)==0) return FALSE;

    CursBefHidden=(TerFlags2&TFLAG2_CURSOR_BEF_HIDDEN)?TRUE:FALSE;
    HiddenAfterText=(TerFlags4&TFLAG4_TEXT_HIDDEN)?TRUE:FALSE;

    // get current font id
    CurCfmt=GetCurCfmt(w,LineNo,ColNo);
    if (!IsHypertext(w,CurCfmt)) {          // not on hypertext
       if (CursBefHidden && (TerFont[CurCfmt].style&HIDDEN)) goto CHECK_HYPERTEXT;
       else return FALSE;
    }

    CHECK_HYPERTEXT:

    // initialize the hyperlink structure
    if (text) text[0]=0;
    if (code) code[0]=0;
    if (select) HilightType=HILIGHT_OFF;  // turn off hilighting to startwith

    // find the beginning of the hypertext
    TextLine=BegLine=LineNo;
    TextCol=BegCol=ColNo;
    
    // check if positioned on a image map
    if (CurCfmt==CurMapPict && CurMapId>0) {
       struct StrImageMap far *pMap=&(ImageMap[CurMapId]);
       struct StrImageMapRect far *pMapRect=&(pMap->pMapRect[CurMapRect]);
       int len;

       lstrcpy(text,"Image Map: ");
       if (pMap->name) lstrcat(text,pMap->name);

       if (code) {
          code[0]=0;
          if (pMapRect->link) wsprintf(code,"href=%c%s%c ",QUOTE,pMapRect->link,QUOTE);
          len=lstrlen(code);
          if (pMapRect->target) if (pMapRect->link) wsprintf(&(code[len]),"target=%c%s%c ",QUOTE,pMapRect->target,QUOTE);
       }

       if (select) {
          HilightBegRow=HilightEndRow=BegLine;
          HilightBegCol=BegCol;
          HilightEndCol=BegCol+1;
          HilightType=HILIGHT_CHAR;
          PaintTer(w);
       }
       return TRUE;
    } 

    // check if positioned over HYPERLINK field
    if (TerFont[CurCfmt].FieldId==FIELD_HLINK) {
       // get the field code
       if (code && TerFont[CurCfmt].FieldCode) lstrcat(code,TerFont[CurCfmt].FieldCode);
       
       GetFieldLoc(w,LineNo,ColNo,TRUE,&BegLine,&BegCol);
       GetFieldLoc(w,LineNo,ColNo,FALSE,&EndLine,&EndCol);  // EndCol is after the last field text

       if (text) {               // extract hyperlink text
          BYTE tail[MAX_WIDTH+1],lead[MAX_WIDTH+1],string[2*MAX_WIDTH+1];
    
          len=0;
          for (l=BegLine;l<=EndLine;l++) {         // extract the text
             StartCol=(l==BegLine)?BegCol:0;
             LastCol=(l==EndLine)?EndCol:LineLen(l);
             GetLineData(w,l,StartCol,LastCol-StartCol,tail,lead,NULL,NULL);
             CurLen=TerMakeMbcs(hTerWnd,string,tail,lead,LastCol-StartCol);
       
             if ((len+CurLen)>MAX_WIDTH) CurLen=MAX_WIDTH-len;
             string[CurLen]=0;
             lstrcat(text,string);
             len+=CurLen;
          }
       }
       
       if (select) {
          HilightBegRow=BegLine;
          HilightEndRow=EndLine;
          HilightBegCol=BegCol;
          HilightEndCol=EndCol;
          HilightType=HILIGHT_CHAR;
          PaintTer(w);
       }
       return TRUE;
    } 

    // regular hyperlink construct
    if (CursBefHidden && (TerFont[CurCfmt].style&HIDDEN)) {  // look forward to the beginning of the hypertext
      for (l=BegLine;l<TotalLines;l++) {
         len=LineLen(l);
         if (len==0) continue;

         fmt=OpenCfmt(w,l);
         if (l==BegLine) StartCol=BegCol+1;
         else            StartCol=0;

         for (i=StartCol;i<len;i++) {
            CurCfmt=fmt[i];
            if (IsHypertext(w,CurCfmt)) {  // hyper text found
               TextLine=l;         // marks the first hypertext position
               TextCol=i;
               break;
            }
            else {
               if (!(TerFont[CurCfmt].style&HIDDEN)) return FALSE;  // out of the hidden text
               BegLine=l;                  // marks the last hidden text position
               BegCol=i;
            }
         }
         CloseCfmt(w,l);
         if (i<len) break;        // previous loop ended
      }
    }
    
    else {                        // look backward to the beginning of the hypertext
      for (l=BegLine;l>=0;l--) {
         len=LineLen(l);
         if (len==0) continue;

         fmt=OpenCfmt(w,l);
         if (l==BegLine) StartCol=BegCol-1;
         else            StartCol=len-1;

         for (i=StartCol;i>=0;i--) {
            CurCfmt=fmt[i];
            if (!IsHypertext(w,CurCfmt)) {  // past the beginning of the text
               BegLine=l;         // look for hypercode here
               BegCol=i;
               break;
            }
            else {
               TextLine=l;
               TextCol=i;
            }
         }
         CloseCfmt(w,l);
         if (i>=0) break;        // previous loop ended
      }
    }

    // extract the hypetext
    j=0;                    // index into
    CodeLine=-1;
    for (l=TextLine;l<TotalLines;l++) {
       len=LineLen(l);
       if (len==0) continue;

       fmt=OpenCfmt(w,l);
       OpenTextPtr(w,l,&ptr,&lead);

       if (l==TextLine) StartCol=TextCol;
       else             StartCol=0;

       for (i=StartCol;i<len;i++) {
          CurCfmt=fmt[i];
          if (!IsHypertext(w,CurCfmt)) {
             CodeLine=l;                   // save it for the phrase/hidden format, because for this format hidden text starts right after the phrase
             CodeCol=i;
             break;  // past the end of hypertext
          }
          else {
             if (LeadByte(lead,i)) {
                if (text) text[j]=LeadByte(lead,i);
                j++;
             }
             if (text) text[j]=ptr[i];                      // extract the character
             j++;
             if (j>=(MAX_WIDTH-1)) break;
          }
       }
       CloseCfmt(w,l);
       CloseTextPtr(w,l);


       if (i<len) break;                               // previous loop ended
    }
    if (text) text[j]=0;                                    // terminate the string

    if (select) {                                 //mark the ending selection point
       if (HiddenAfterText) {                     // hidden code after hypertext phrase
          HilightBegRow=TextLine;
          HilightBegCol=TextCol;
       } 
       else {
          HilightEndRow=l;
          HilightEndCol=i;
       }
    }

    // check for the hyper text code
    if (HiddenAfterText) {
       if (CodeLine<0) return TRUE;    // code line not found
       CurCfmt=GetCurCfmt(w,CodeLine,CodeCol);
       if (!(TerFont[CurCfmt].style&HYPER_CODE)) return TRUE; // code not found
    }
    else { 
       if (TextLine==0 && TextCol==0) return TRUE;  // code not found
       CurCfmt=GetCurCfmt(w,BegLine,BegCol);
       if (!(TerFont[CurCfmt].style&HYPER_CODE)) return TRUE; // code not found


       // find the beginning of the hyper code
       CodeLine=BegLine;
       CodeCol=BegCol;
       for (l=BegLine;l>=0;l--) {
          len=LineLen(l);
          if (len==0) continue;

          fmt=OpenCfmt(w,l);
          if (l==BegLine) StartCol=BegCol-1;
          else            StartCol=len-1;

          for (i=StartCol;i>=0;i--) {
             CurCfmt=fmt[i];
             if (!(TerFont[CurCfmt].style&HYPER_CODE)) break;  // past the beginning of the code
             else {
                CodeLine=l;
                CodeCol=i;
             }
          }
          CloseCfmt(w,l);
          if (i>=0) break;     // previous loop ended
       }

       if (select) {
          HilightBegRow=CodeLine;
          HilightBegCol=CodeCol;
       }
    }

    // extract the hypecode
    j=0;                    // index into
    for (l=CodeLine;l<TotalLines;l++) {
       len=LineLen(l);
       if (len==0) continue;

       fmt=OpenCfmt(w,l);
       OpenTextPtr(w,l,&ptr,&lead);

       if (l==CodeLine) StartCol=CodeCol;
       else             StartCol=0;

       for (i=StartCol;i<len;i++) {
          CurCfmt=fmt[i];
          if (IsHypertext(w,CurCfmt) || !(TerFont[CurCfmt].style&HYPER_CODE)) {
             if (HiddenAfterText) {
                HilightEndRow=l;
                HilightEndCol=i;
             } 
             break;  // past the end of hypercode
          }
          else {
             if (LeadByte(lead,i)) {
                if (code) code[j]=LeadByte(lead,i);  // extract the lead byte
                j++;
             }
             if (code) code[j]=ptr[i];                      // extract the character
             j++;
             if (j>=MAX_WIDTH) break;
          }
       }

       CloseCfmt(w,l);
       CloseTextPtr(w,l);

       if (i<len) break;                              // previous loop ended

    }
    if (code) code[j]=0;                                   // terminate the string

    if (select) {
       HilightType=HILIGHT_CHAR;
       PaintTer(w);
    }

    return TRUE;
}

/******************************************************************************
     TerDeleteHypertext:
     Delete the hypertext phrase at the given location.  set the LineNo to -1
     to specify the current cursor location.
******************************************************************************/
BOOL WINAPI _export TerDeleteHypertext(HWND hWnd, long LineNo, int ColNo, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (LineNo<0) {
       LineNo=CurLine;
       ColNo=CurCol;
    }

    if (LineNo<0 || LineNo>=TotalLines) return FALSE;
    if (ColNo<0 || ColNo>=LineLen(LineNo)) return FALSE;

    if (!IsHypertext(w,GetCurCfmt(w,LineNo,ColNo))) return FALSE;    // hypertext not found at the current cursor location

    // hilight the area to be deleted
    HilightType=HILIGHT_OFF;

    // find the beginning of the hypertext phrase
    HilightBegRow=LineNo;
    HilightBegCol=ColNo;
    GetHypertextStart(w,&HilightBegRow,&HilightBegCol);


    // look for the position after the hypertext phrase
    HilightEndRow=LineNo;
    HilightEndCol=ColNo+1;
    if (HilightBegCol<HilightEndCol) HilightEndCol--;  // otherwise GetHypertextEnd can extend to the next hypertext
    GetHypertextEnd(w,&HilightEndRow,&HilightEndCol);

    HilightType=HILIGHT_CHAR;
    StretchHilight=FALSE;

    return TerDeleteBlock(hWnd,repaint);
}

/****************************************************************************
    TerInsertToc:
    Insert table of contents.
****************************************************************************/
BOOL WINAPI _export TerInsertToc(HWND hWnd, BOOL repaint)
{
    PTERWND w;
    char FieldString[50];
    int  i,FieldLen;
    int  NewFont;
    LPBYTE ptr;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    // create a line break if not on the first column
    if (!PrepForObject(w)) return FALSE;


    // get the new font id
    HilightType=HILIGHT_OFF;     // turn off highlight
    if (!CharFmt(w,GetNewFieldId,(DWORD)FIELD_TOC,0L,FALSE)) return FALSE;
    NewFont=InputFontId;
    InputFontId=-1;
    if (NewFont<0 || TerFont[NewFont].FieldId!=FIELD_TOC) return FALSE;

    // make the current page number string
    lstrcpy(FieldString,"Table of Contents");
    AddChar(FieldString,ParaChar);
    FieldLen=lstrlen(FieldString);

    // insert the string in the text
    MoveLineArrays(w,CurLine,1,'B'); // insert line before the current line

    LineAlloc(w,CurLine,0,FieldLen);
    
    ptr=pText(CurLine);
    fmt=OpenCfmt(w,CurLine);

    for (i=0;i<FieldLen;i++) {
       ptr[i]=FieldString[i];
       fmt[i]=NewFont;
    }
    LineFlags(CurLine)|=LFLAG_TOC;

    CloseCfmt(w,CurLine);
    
    ReleaseUndo(w);     // release any existing undos

    if (repaint) TerRepaginate(hTerWnd,TRUE);   // repaint the screen

    return TRUE;
}

/****************************************************************************
    TerInsertDateTime:
    Insert current date or time.  Set the second parameter to NULL to display a dialog box
****************************************************************************/
BOOL WINAPI _export TerInsertDateTime(HWND hWnd, LPBYTE pDateFmt, BOOL repaint)
{
    PTERWND w;
    BYTE DateFmt[50];

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!pDateFmt || lstrlen(pDateFmt)==0) {             // accept date/time format from a dialog box
       if (!CallDialogBox(w,"DateParam",DateParam,0L)) return FALSE;
       lstrcpy(DateFmt,DlgText1);
       pDateFmt=DateFmt;
    }

    return InsertDynField(w,FIELD_DATE,pDateFmt);
}

/******************************************************************************
    PrepForObject:
    prepare current line for the object (such as table, toc) that needs to be placed on their
    own line. After this function returns TRUE, the object can be inserted at CurLine/CurCol
******************************************************************************/
BOOL PrepForObject(PTERWND w)
{
    int len;
    LPBYTE ptr;
    LPWORD fmt;

    // check if on the protected area
    if (CurLine>0 && LineLen(CurLine)>0 && LineLen(CurLine-1)>0) {
       int CurCfmt=GetCurCfmt(w,CurLine,0);
       int PrevCfmt=GetCurCfmt(w,CurLine-1,LineLen(CurLine-1)-1);
       if (TerFont[CurCfmt].style&PROTECT && TerFont[PrevCfmt].style&PROTECT) {
          MessageBeep(0);
          return FALSE;
       }
    }

    // split the current line if not at the beginning of the line
    if (CurCol>0 && SplitLine(w,CurLine,CurCol,0)) {
       CurLine++;
       CurCol=0;
    }


    // split the current para if in the middle of a para
    if (CurLine>0 && (len=LineLen(CurLine-1))>0 && !(LineFlags(CurLine-1)&LFLAG_HDRS_FTRS)) {
      ptr=pText(CurLine-1);
      if (ptr[len-1]!=ParaChar && ptr[len-1]!=CellChar && ptr[len-1]!=ROW_CHAR 
                      && ptr[len-1]!=SECT_CHAR && ptr[len-1]!=COL_CHAR && ptr[len-1]!=PAGE_CHAR) {
         LineAlloc(w,CurLine-1,len,len+1);
         ptr=pText(CurLine-1);
         fmt=OpenCfmt(w,CurLine-1);
         ptr[len]=ParaChar;         // insert a para character
         fmt[len]=fmt[len-1];
         if (TerFont[fmt[len]].style&PICT) fmt[len]=0;
         SaveUndo(w,CurLine-1,len,CurLine-1,len,'O');   // use 'o' instead of 'I' so this undo does get combined with previous character input
      }
    }

    return TRUE;
}

/******************************************************************************
    ReplaceTextInPlace:
    Replace text of given length at the given line/col position.  The text can
    wrap multiple lines. The function returns the position after the replaced text.
******************************************************************************/
BOOL ReplaceTextInPlace(PTERWND w,LPLONG pLine,LPINT pCol,int len,LPBYTE ptr, LPBYTE lead)
{
    BOOL allocated=FALSE;
    int i,NewLen,SaveLen,del,copy,col=(*pCol);
    LPWORD fmt;
    long line=(*pLine);

    // extract lead bytes from ptr
    if (mbcs) {
       if (lead==NULL) {
         BYTE huge *TempPtr=ptr;
         long BufLen=hstrlen(ptr);

         ptr=OurAlloc(BufLen+2);
         lead=OurAlloc(BufLen+2);
         if (!ptr || !lead) return FALSE;

         TerSplitMbcs(hTerWnd,TempPtr,ptr,lead);
         allocated=TRUE;
       }
    }
    else lead=NULL;
    
    NewLen=SaveLen=lstrlen(ptr);    // length of the new text    

    while (len>0 || NewLen>0) {
        if (NewLen==0) {    // new text completely inserted, delete excess characters from the old string
           while (len>0) {
              del=len;        // delete the remaining characters
              if ((col+del)>LineLen(line)) del=LineLen(line)-col;
              MoveLineData(w,line,col,del,'D');
              len-=del;
              col+=del;
              if (col>=LineLen(line)) {
                 line++;
                 if (line>=TotalLines) goto END;   // end of file
                 col=0;
              } 
           }
        }
        else if (len==0) {  // existing text completely replaced, insert the excess new text
           int PrevCfmt=GetPrevCfmt(w,line,col);
           MoveLineData(w,line,col,NewLen,'B');  // create space before
           SetLineData(w,line,col,NewLen,&(ptr[SaveLen-NewLen]),lead?&(lead[SaveLen-NewLen]):NULL,NULL,NULL);

           // Assign the font ids
           fmt=OpenCfmt(w,line);
           for (i=0;i<NewLen;i++) fmt[col+i]=PrevCfmt;
           CloseCfmt(w,line);

           col+=NewLen;

           goto END;
        }        
    
        // copy characters
        copy=len;
        if (NewLen<copy) copy=NewLen;
        if ((LineLen(line)-col)<copy) copy=LineLen(line)-col;
        SetLineData(w,line,col,copy,&(ptr[SaveLen-NewLen]),lead?&(lead[SaveLen-NewLen]):NULL,NULL,NULL);

        NewLen-=copy;
        len-=copy;
        col+=copy;
        if (col>=LineLen(line)) {
           line++;
           col=0;
        } 
    }    

    END:
    if (allocated) OurFree(ptr);
    if (allocated) OurFree(lead);

    (*pLine)=line;
    (*pCol)=col;

    return TRUE;
}

/******************************************************************************
    GetDateString:
    Get date string for a specifield date format field.
******************************************************************************/
BOOL GetDateString(PTERWND w, LPBYTE pDateFmt, LPBYTE DateString, int FieldFont)
{
    SYSTEMTIME date;
    LCID lcid=LOCALE_USER_DEFAULT;
    int i,len,DateFmtLen;
    BYTE DateFmt[50],TimeFmt[50];
    LPBYTE pAMPM;
      
    GetSystemDateTime(w,&date);

    // get the font locale
    if (FieldFont>=0) lcid=(LCID)TerFont[FieldFont].lang;
    else if (FieldFont==-2) lcid=1033;   // use english language
    else {
       HKL hkl=GetKeyboardLayout(0);
       lcid=(LCID)LOWORD(hkl);
    } 

    //if ((int)lcid==1033) return GetDateStringAlt(w,pDateFmt,DateString);  // english language - use our function

    lstrcpy(DateString," ");  // initialize
    
    // find the beginning of the time format
    lstrcpy(DateFmt,pDateFmt);
    len=lstrlen(DateFmt);
    for (i=0;i<len;i++) if (DateFmt[i]=='H') DateFmt[i]='h'; // GetTimeFormat does not like 'H'
    for (i=0;i<len;i++) if (DateFmt[i]=='h' || DateFmt[i]=='m' || DateFmt[i]=='s' || DateFmt[i]=='a' || DateFmt[i]=='p') break;
    DateFmt[i]=0;
    DateFmtLen=i;

    // format date
    GetDateFormat(lcid,0,&date,DateFmt,DateString,30);

    if (DateFmtLen==lstrlen(pDateFmt)) return TRUE;


    // format time
    lstrcpy(TimeFmt,&(pDateFmt[DateFmtLen]));
    pAMPM=strstr(TimeFmt,"am/pm");  // convert to tt format
    if (pAMPM) {
       (*pAMPM)=0;
       lstrcat(TimeFmt,"tt");
    } 

    len=lstrlen(DateString);
    GetTimeFormat(lcid,0,&date,TimeFmt,&(DateString[len]),30);

    return TRUE;
}    

/******************************************************************************
    GetDateStringAlt:
    Get date string for a specifield date format field.
******************************************************************************/
BOOL GetDateStringAlt(PTERWND w, LPBYTE pDateFmt, LPBYTE DateString)
{
    SYSTEMTIME date;
    BYTE string[20],MonthShort[4],DayShort[4],token[40],prefix[40];
    LPBYTE MonthLong,AmPm,DayLong;
    int HourShort,idx=0,len;


    GetSystemDateTime(w,&date);
    
    // calculate additional values
    lstrcpy(string,MonthNames[date.wMonth-1]);
    string[3]=0;           // print first three letters of the month
    lstrcpy(MonthShort,string);  // month abbreviation
    MonthLong=MonthNames[date.wMonth-1];

    lstrcpy(string,WeekDays[date.wDayOfWeek]);
    string[3]=0;           // print first three letters of the weekday
    lstrcpy(DayShort,string);  // day abbreviation
    DayLong=WeekDays[date.wDayOfWeek];


    HourShort=date.wHour;
    AmPm="AM";
    if (HourShort>12) {
       HourShort-=12;
       AmPm="PM";
    }
    else if (HourShort==12) {
       AmPm="PM";
    }  

    if (!pDateFmt) {              // default date format
       wsprintf(DateString,"%d/%d/%02d",date.wMonth,date.wDay,date.wYear%100);
       return TRUE;
    }

    DateString[0]=0;
    while (ParseDateToken(w,pDateFmt,&idx,token,prefix)) {
        if (prefix[0]!=0) lstrcat(DateString,prefix);       // add the last prefix
    

        len=lstrlen(DateString);
        if      (strcmp(token,"yyyy")==0)  wsprintf(&(DateString[len]),"%04d",date.wYear);
        else if (strcmp(token,"yy")==0)    wsprintf(&(DateString[len]),"%02d",date.wYear%100);
        else if (strcmp(token,"MMMM")==0)  lstrcat(DateString,MonthLong);
        else if (strcmp(token,"MMM")==0)   lstrcat(DateString,MonthShort);
        else if (strcmp(token,"MM")==0)    wsprintf(&(DateString[len]),"%02d",date.wMonth);
        else if (strcmp(token,"M")==0)     wsprintf(&(DateString[len]),"%d",date.wMonth);
        else if (strcmp(token,"dddd")==0)  lstrcat(DateString,DayLong);
        else if (strcmp(token,"ddd")==0)   lstrcat(DateString,DayShort);
        else if (strcmp(token,"dd")==0)    wsprintf(&(DateString[len]),"%02d",date.wDay);
        else if (strcmp(token,"d")==0)     wsprintf(&(DateString[len]),"%d",date.wDay);
        else if (strcmp(token,"HH")==0)    wsprintf(&(DateString[len]),"%02d",date.wHour);
        else if (strcmp(token,"H")==0)     wsprintf(&(DateString[len]),"%d",date.wHour);
        else if (strcmp(token,"hh")==0)    wsprintf(&(DateString[len]),"%0wd",HourShort);
        else if (strcmp(token,"h")==0)     wsprintf(&(DateString[len]),"%d",HourShort);
        else if (strcmp(token,"mm")==0)    wsprintf(&(DateString[len]),"%02d",date.wMinute);
        else if (strcmp(token,"m")==0)     wsprintf(&(DateString[len]),"%d",date.wMinute);
        else if (strcmp(token,"ss")==0)    wsprintf(&(DateString[len]),"%02d",date.wSecond);
        else if (strcmp(token,"s")==0)     wsprintf(&(DateString[len]),"%d",date.wSecond);
        else if (strcmp(token,"am/pm")==0) lstrcat(DateString,AmPm);

        idx+=lstrlen(token);
    }
    if (prefix[0]!=0) lstrcat(DateString,prefix);       // add the last prefix
    
    if (lstrlen(DateString)==0) wsprintf(DateString,"%d/%d/%02d",date.wMonth,date.wDay,date.wYear%100);

    return TRUE;
}
    
/*****************************************************************************
    ParseDateToken:
    Retrieve the next date token and the suffix.  The 'pIdx' is updated to the location
    where the next token was found.
******************************************************************************/
BOOL ParseDateToken(PTERWND w, LPBYTE pDateFmt, LPINT pIdx, LPBYTE token,LPBYTE prefix)
{
   int idx=(*pIdx),len,i,pos;
   BYTE tokens[][7]={"yyyy","yy","MMMM","MMM","MM","M","dddd","ddd","dd","d","HH","H","hh","h","mm","m","ss","s","am/pm",""};
   LPBYTE pFmt,ptr;
   BOOL found=FALSE; 

   pFmt=&(pDateFmt[idx]);
   len=lstrlen(pFmt);
   token[0]=0;
   lstrcpy(prefix,pFmt);
   if (len==0) return FALSE;
   
   for (pos=0;pos<len;pos++) {
      ptr=&(pFmt[pos]);
      for (i=0;tokens[i][0]!=0;i++) {
         if (strstr(ptr,tokens[i])==ptr) {  // longest token found
            (*pIdx)+=pos;         // position of the token
            lstrcpy(token,tokens[i]);
            prefix[pos]=0;
            return TRUE;
         }   
      }  
   }

   return FALSE;
}
    
/*****************************************************************************
    GetSystemDateTime:
    Get the system date and time
******************************************************************************/
void GetSystemDateTime(PTERWND w, SYSTEMTIME far *pDateTime)
{
   #if defined (WIN32)
      GetLocalTime(pDateTime);
   #else
      union  REGS reg;
      union  REGS oreg;
      struct SREGS sreg;
      long days,date;

      //***************** get system date time **********************************
      // get date
      reg.h.ah=0x2a;
      segread(&sreg);
      sreg.es=sreg.ds;               // a valid value for ES
      int86x(0x21,&reg,&oreg,&sreg);

      pDateTime->wYear=oreg.x.cx;
      pDateTime->wMonth=oreg.h.dh;
      pDateTime->wDay=oreg.h.dl;

      // calcualte day of the week
      date=pDateTime->wYear*10000L+pDateTime->wMonth*100L+pDateTime->wDay;
      days=DateToDays(date);       // days since 12/31/1899
      days=days+6;                   // adjustment factor to make rem(days) to start on sunday.
      pDateTime->wDayOfWeek=(WORD)(days-((days/7)*7));        // week day 

      // get time
      reg.h.ah=0x2c;
      segread(&sreg);
      sreg.es=sreg.ds;               // a valid value for ES
      int86x(0x21,&reg,&oreg,&sreg);
      pDateTime->wHour=oreg.h.ch;
      pDateTime->wMinute=oreg.h.cl;
      pDateTime->wSecond=oreg.h.dh;
      pDateTime->wMilliseconds=0;
   #endif

}

/******************************************************************************
    PrevTextPos:
    This function returns the previous text position.  It returns FALSE if input position
    is at the beginning of the file.
******************************************************************************/
BOOL PrevTextPos(PTERWND w, LPLONG pLineNo, LPINT pCol)
{
    long line=(*pLineNo);
    int  col=(*pCol);
 
    if (line==0 && col==0) return FALSE;

    col--;
        
    if (col<0) {
       line--;
       while (line>=0 && LineLen(line)==0) line--;
       if (line<0) {
          line=0;
          col=0;
       }
       else col=LineLen(line)-1;
    }

    (*pLineNo)=line;
    (*pCol)=col;

    return TRUE;
}

/******************************************************************************
    NextTextPos:
    This function returns the next text position.  It returns FALSE if input position
    is at the end of the file
******************************************************************************/
BOOL NextTextPos(PTERWND w, LPLONG pLineNo, LPINT pCol)
{
    long line=(*pLineNo);
    int  col=(*pCol);
 
    if (line==(TotalLines-1) && col==(LineLen(line)-1)) return FALSE;

    col++;
    
    if (col>=LineLen(line)) {
       line++;
       while (line<TotalLines && LineLen(line)==0) line++;
       if (line>=TotalLines) {
          line=TotalLines-1;
          col=LineLen(line)-1;
          if (col<0) col=0;
       }
       else col=0;
    }

    (*pLineNo)=line;
    (*pCol)=col;

    return TRUE;
}

/******************************************************************************
    HtmlAddOnFound:
    Check if HTML add-on available.
******************************************************************************/
BOOL HtmlAddOnFound(PTERWND w)
{
    BOOL found;

    LoadHtmlAddOn(w);
    found=(BOOL)hHts;
    FreeHtmlAddOn(w);

    return found;
}

/******************************************************************************
    LoadHtmlAddOn:
    Search for the HtmlAddOn product and find the function pointers.
******************************************************************************/
LoadHtmlAddOn(PTERWND w)
{
    hHts=NULL;

    hHts=TerLoadDll("HTS8.DLL");

    if (!hHts) return FALSE;

    // find the function pointers
    HtsSaveFromTer=(LPVOID)GetProcAddress(hHts,"HtsSaveFromTer");
    HtsReadFromTer=(LPVOID)GetProcAddress(hHts,"HtsReadFromTer");

    if (!HtsSaveFromTer || !HtsReadFromTer) {
       FreeLibrary(hHts);
       hHts=NULL;
    }

    return TRUE;
}

/******************************************************************************
    FreeHtmlAddOn:
    Free the html add-on library
******************************************************************************/
FreeHtmlAddOn(PTERWND w)
{
    FreeLibrary(hHts);
    hHts=NULL;
    return TRUE;
}

/******************************************************************************
    TerSetFlags:
    Set or reset the ter flags.  This function returns the new flags.  To retrieve
    the current flags, call this function with flags set to 0.  If hWnd is zero,
    this function sets the initialization flags.
******************************************************************************/
DWORD WINAPI _export TerSetFlags(HWND hWnd, BOOL set, DWORD flags)
{
    PTERWND w;

    if (!hWnd) {
       if (set) TerInitFlags|=flags;
       else     TerInitFlags=ResetLongFlag(TerInitFlags,flags);
       return TerInitFlags;
    }
    else {
       if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

       if (set) TerFlags|=flags;
       else     TerFlags=ResetLongFlag(TerFlags,flags);

       return TerFlags;
    }
}

/******************************************************************************
    TerSetFlags2:
    Set or reset the ter flags.  This function returns the new flags.  To retrieve
    the current flags, call this function with flags set to 0.  If hWnd is zero,
    this function sets the initialization flags.
******************************************************************************/
DWORD WINAPI _export TerSetFlags2(HWND hWnd, BOOL set, DWORD flags)
{
    PTERWND w;

    if (!hWnd) {
       if (set) TerInitFlags2|=flags;
       else     TerInitFlags2=ResetLongFlag(TerInitFlags2,flags);
       return TerInitFlags2;
    }
    else {
       if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

       if (set) TerFlags2|=flags;
       else     TerFlags2=ResetLongFlag(TerFlags2,flags);

       return TerFlags2;
    }
}

/******************************************************************************
    TerSetFlags3:
    Set or reset the ter flags.  This function returns the new flags.  To retrieve
    the current flags, call this function with flags set to 0.  If hWnd is zero,
    this function sets the initialization flags.
******************************************************************************/
DWORD WINAPI _export TerSetFlags3(HWND hWnd, BOOL set, DWORD flags)
{
    PTERWND w;

    if (!hWnd) {
       if (set) TerInitFlags3|=flags;
       else     TerInitFlags3=ResetLongFlag(TerInitFlags3,flags);
       return TerInitFlags3;
    }
    else {
       if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

       if (set) {
          TerFlags3|=flags;
          if (flags&TFLAG3_CURSOR_IN_CELL) CursorCell=cid(CurLine);
       }
       else {
          TerFlags3=ResetLongFlag(TerFlags3,flags);
          if (flags&TFLAG3_CURSOR_IN_CELL) CursorCell=0;
       }

       return TerFlags3;
    }
}

/******************************************************************************
    TerSetFlags4:
    Set or reset the ter flags.  This function returns the new flags.  To retrieve
    the current flags, call this function with flags set to 0.  If hWnd is zero,
    this function sets the initialization flags.
******************************************************************************/
DWORD WINAPI _export TerSetFlags4(HWND hWnd, BOOL set, DWORD flags)
{
    PTERWND w;

    if (!hWnd) {
       if (set) TerInitFlags4|=flags;
       else     TerInitFlags4=ResetLongFlag(TerInitFlags4,flags);
       return TerInitFlags4;
    }
    else {
       if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

       if (set) TerFlags4|=flags;
       else TerFlags4=ResetLongFlag(TerFlags4,flags);

       return TerFlags4;
    }
}

/******************************************************************************
    TerSetFlags5:
    Set or reset the ter flags.  This function returns the new flags.  To retrieve
    the current flags, call this function with flags set to 0.  If hWnd is zero,
    this function sets the initialization flags.
******************************************************************************/
DWORD WINAPI _export TerSetFlags5(HWND hWnd, BOOL set, DWORD flags)
{
    PTERWND w;

    if (!hWnd) {
       if (set) TerInitFlags5|=flags;
       else     TerInitFlags5=ResetLongFlag(TerInitFlags5,flags);
       return TerInitFlags5;
    }
    else {
       if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

       if (set) TerFlags5|=flags;
       else TerFlags5=ResetLongFlag(TerFlags5,flags);

       return TerFlags5;
    }
}

/******************************************************************************
    TerSetFlags6:
    Set or reset the ter flags.  This function returns the new flags.  To retrieve
    the current flags, call this function with flags set to 0.  If hWnd is zero,
    this function sets the initialization flags.
******************************************************************************/
DWORD WINAPI _export TerSetFlags6(HWND hWnd, BOOL set, DWORD flags)
{
    PTERWND w;

    if (!hWnd) {
       if (set) TerInitFlags6|=flags;
       else     TerInitFlags6=ResetLongFlag(TerInitFlags6,flags);
       return TerInitFlags6;
    }
    else {
       if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

       if (set) TerFlags6|=flags;
       else TerFlags6=ResetLongFlag(TerFlags6,flags);

       return TerFlags6;
    }
}

/******************************************************************************
    GetObjSpcBef:
    Get the previously calculated frame space before this line.
******************************************************************************/
GetObjSpcBef(PTERWND w,long line, BOOL InScrUnits)
{
    return (GetFrmSpcBef(w,line,InScrUnits)+GetTblSpcBef(w,line,InScrUnits));
}

/****************************************************************************
    GetTerFields:
    This function provides a number of operational parameters about the
    current window.  You also need to call this function before calling
    the SetTerFields function.
    This first argument specifies the TER window handle for which the
    information is needed.  The second argument is the pointer to the information
    block (defined in TER.H file).  This function will fill all the
    fields of this structure.
****************************************************************************/
BOOL WINAPI _export GetTerFields(HWND hWnd, struct StrTerField far *fld)
{
    return GetTerFieldsAlt(hWnd,fld,-1);
}

/****************************************************************************
    TerGetSelection:
    Get the beginning and ending location of selected text.
****************************************************************************/
BOOL WINAPI _export TerGetSelection(HWND hWnd, LPLONG pBegLine, LPINT pBegCol, LPLONG pEndLine, LPINT pEndCol)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (HilightType==HILIGHT_OFF) return FALSE;

    if (pBegLine) (*pBegLine)=HilightBegRow;
    if (pBegCol) (*pBegCol)=HilightBegCol;
    if (pEndLine) (*pEndLine)=HilightEndRow;
    if (pEndCol)  (*pEndCol)=HilightEndCol;

    if (HilightType==HILIGHT_LINE) {
       if (pBegCol) (*pBegCol)=0;
       if (pEndCol) (*pEndCol)=LineLen(HilightEndRow);
    }    

    return TRUE;
}

/****************************************************************************
    TerShadeProtectedText:
    Set or resetting or protect text shading
****************************************************************************/
BOOL WINAPI _export TerShadeProtectedText(HWND hWnd, BOOL shade, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    ModifyProtectColor=shade;

    if (repaint) PaintTer(w);

    return TRUE;
}

/****************************************************************************
    GetTerFieldsAlt:
    This function is similar to GetTerFields.  In addition, the specified
    line number is used for the information that is depended on the line number.
****************************************************************************/
BOOL WINAPI _export GetTerFieldsAlt(HWND hWnd, struct StrTerField far *fld,long LineNo)
{
    LPBYTE ptr;
    LPWORD fmt;
    int   i,ParaId,sect;
    WORD  LastFmt;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (LineNo<0) LineNo=CurLine;        // use current line number instead


    //**** fill the read only fields ************
    fld->size         =sizeof(struct StrTerField);   // pass size for version checking

    fld->hTerWndW     =hTerWnd;          // handle to the editor window
    fld->hTerDCW      =hTerDC;           // handle to TER class DC
    fld->TerRectW     =TerRect;          // entire client window rectangal
    fld->TerWinRectW  =TerWinRect;       // text window rectangal

    fld->TotalLinesW  =TotalLines;       // total lines in the file
    fld->TotalPfmtsW  =TotalPfmts;       // total paragraph ids in use by the current window
    fld->TotalFontsW  =TotalFonts;       // Total font objects in use by the current window
    fld->TotalStylesW =TotalSID;         // Total number of styles

    fld->WinWidthW    =WinWidth;         // current window in columns
    fld->WinHeightW   =WinHeight;        // current window height in number of rows
    fld->TerWinOrgXW  =TerWinOrgX;       // window origin x co-ordinates

    fld->MouseColW    =MouseCol;         // text column position of mouse
    fld->MouseLineW   =MouseLine;        // text line position of mouse

    fld->MaxColBlockW =MaxColBlock;      // biggest column block allowed

    fld->modified    =TerArg.modified;  // data modified, user needs to select the 'save' option to save data
    fld->WordWrapW   =TerArg.WordWrap;  // word wrap on


    ParaId=pfmt(LineNo);

    fld->ParaLeftIndentW  =PfmtId[ParaId].LeftIndentTwips;   // paragraph left indent in twips
    fld->ParaRightIndentW =PfmtId[ParaId].RightIndentTwips;  // paragraph right indent in twips
    fld->ParaFirstIndentW =PfmtId[ParaId].FirstIndentTwips;  // paragraph first line indent in twips
    fld->ParaFlagsW       =PfmtId[ParaId].flags;             // paragraph flags,includes such flags as double spacing,centering, right justify etc
    fld->pflagsW          =PfmtId[ParaId].pflags;            // paragraph PFLAG_? constants
    fld->ParaTabIdW       =PfmtId[ParaId].TabId;             // paragraph tab id, index into the tab table
    fld->ParaCellIdW      =cid(LineNo);                      // paragraph cell id that the paragraph belongs to
    fld->ParaFrameIdW     =fid(LineNo);                      // paragraph frame id
    fld->ParaShadingW     =PfmtId[ParaId].shading;           // paragraph shading (0 to 10000)
    fld->ParaSpaceBeforeW =PfmtId[ParaId].SpaceBefore;       // space before paragraph
    fld->ParaSpaceAfterW  =PfmtId[ParaId].SpaceAfter;        // space after paragraph
    fld->ParaSpaceBetweenW =PfmtId[ParaId].SpaceBetween;     // space between paragraph lines
    fld->ParaStyleIdW     =PfmtId[ParaId].StyId;             // paragraph style id
    fld->ParaAuxIdW       =PfmtId[ParaId].AuxId;             // paragraph aux id

    fld->CurSectW=sect    =GetSection(w,LineNo);            // current section number

    fld->LeftMarginW      =(int)InchesToTwips(TerSect[sect].LeftMargin);  // section left margin in twips
    fld->RightMarginW     =(int)InchesToTwips(TerSect[sect].RightMargin); // section right margin in twips
    fld->TopMarginW       =(int)InchesToTwips(TerSect[sect].TopMargin);   // section top margin in twips
    fld->BotMarginW       =(int)InchesToTwips(TerSect[sect].BotMargin);   // section bottom margin in twips
    fld->columnsW         =TerSect[sect].columns;                    // number of columns in the current section

    fld->CurPageW         =CurPage;            // current page number
    fld->TotalPagesW      =TotalPages;         // number of pages

    fld->MouseXW          =MouseX;             // last mouse x position
    fld->MouseYW          =MouseY;             // last mouse x position

    fld->PrintViewW       =TerArg.PrintView;   // print view mode
    fld->PageModeW        =TerArg.PageMode;    // page mode
    fld->FittedViewW      =TerArg.FittedView;  // fitted view mode
    fld->ShowParaMarkW    =ShowParaMark;  // show para marks
    fld->ShowHiddenTextW  =ShowHiddenText;  // show hidden text

    fld->CurCtlIdW        =CurCtlId;           // currently selected control id
    fld->ParaFrameFlagsW  =ParaFrame[fld->ParaFrameIdW].flags;

    //*********** normalize some fields ***********
    if (FALSE && HilightType==HILIGHT_OFF) {
        HilightBegRow=HilightEndRow=0;
        HilightBegCol=HilightEndCol=0;
    }
    if (FALSE && HilightType==HILIGHT_LINE) {
        HilightBegCol=HilightEndCol=0;
    }

    //**** fill the read/write  fields ************
    fld->CurRowW         =CurRow;            // current window row position
    fld->CurColW         =CurCol;            // current window column position
    fld->CurLineW        =CurLine;           // current line number in the file
    fld->BeginLineW      =BeginLine;         // line number of the first window line
    fld->PaintEnabledW   =PaintEnabled;      // FALSE=disables the painting and word wrapping
    fld->WrapFlagW       =WrapFlag;          // Wrap control, see WRAP_ constants
    fld->ReclaimResourcesW =ReclaimResources;// reuse font and para ids
    if (!TerArg.WordWrap) fld->WrapFlagW=WRAP_OFF;

    fld->TextBkColorW    =TextDefBkColor;    // default background color for text
    fld->StatusBkColorW  =StatusBkColor;     // background color of the status line
    fld->StatusColorW    =StatusColor;       // foreground color of the status line
    fld->LinkColorW      =LinkColor;         // color of the linked text
    fld->SnapToGridW     =SnapToGrid;        // snap ruler items to an invisible grid
    fld->HtmlModeW       =HtmlMode;          // html mode adjustments
    fld->ShowTableGridLinesW=ShowTableGridLines; // show grid lines

    fld->ModifyProtectColorW=ModifyProtectColor; // modify the protected text color

    fld->HilightTypeW    =HilightType;       // line/col or character block
    fld->HilightBegRowW  =HilightBegRow;     // beginning row
    fld->HilightEndRowW  =HilightEndRow;     // ending row
    fld->HilightBegColW  =HilightBegCol;     // beginning col
    fld->HilightEndColW  =HilightEndCol;     // ending col

    fld->StretchHilightW =StretchHilight;    // TRUE = highlight being stretch

    fld->LinkStyleW      =LinkStyle;         // Link character style
    fld->LinkDblClickW   =LinkDblClick;      // Invoke link on double click
    fld->ShowProtectCaretW=ShowProtectCaret; // Show caret on protected text

    fld->LineLenW=LineLen(LineNo);          // length of the current line

    //************* fill in text data *************
    ptr=pText(LineNo);
    for (i=0;i<LineLen(LineNo);i++) fld->text[i]=ptr[i];
    fld->text[i]=0;


    //************* fill in font data *************
    fmt=OpenCfmt(w,LineNo);
    for (i=0;i<LineLen(LineNo);i++) fld->font[i]=(BYTE)fmt[i];  // maintain old variable here

    //******* extrapolate the format field to the end ***
    LastFmt=DEFAULT_CFMT;
    if (i>0 && !(TerFont[fmt[i-1]].style&PICT)) LastFmt=fmt[i-1];
    for (;i<MAX_WIDTH;i++) fld->font[i]=(BYTE)LastFmt;   // maintain old parameter
    CloseCfmt(w,LineNo);

    fld->pfmtW=pfmt(LineNo);               // paragraph id of the current line

    fld->TextApply=APPLY_IGNORE;           // initialize the TextApply flag

    return TRUE;
}

/****************************************************************************
    SetTerFields:
    You can use this function to control the TER window by changing
    the operational parameters.  You must call the GetTerFields to get
    the current parameters, and then change the required fields.

    This first argument specifies the TER window handle for which the
    information is provided.  The second argument is the pointer to the
    information block (defined in TER.H file).

    This function verifies and alters any invalid parameters.  You must call
    the GetTerFields to retrieve the updated values.
****************************************************************************/
BOOL WINAPI _export SetTerFields(HWND hWnd, struct StrTerField far *fld)
{
    LPBYTE ptr;
    LPWORD fmt;
    int   i;
    long  LineNo;
    BOOL  RefreshStatus=FALSE;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (fld->size!=sizeof(struct StrTerField)) return PrintError(w,MSG_BAD_VERSION,"SetTerField");

    //**** apply the read/write  fields ************
    PaintFlag=PAINT_LINE;

    //********* Set the position fields ************
    CurRow         =fld->CurRowW;            // current window row position
    CurCol         =fld->CurColW;            // current window column position
    CurLine        =fld->CurLineW;           // current line number in the file
    BeginLine      =fld->BeginLineW;         // line number of the first window line

       //************** validate data *************
    if (CurLine>=TotalLines) {
       CurLine=TotalLines-1;
       PaintFlag=PAINT_WIN;
    }
    if (CurLine<0) {
       CurLine=0;
       PaintFlag=PAINT_WIN;
    }
    if (CurLine-BeginLine>WinHeight) {
       BeginLine=CurLine-WinHeight;
       PaintFlag=PAINT_WIN;
    }
    if (BeginLine<0) {
       BeginLine=0;
       PaintFlag=PAINT_WIN;
    }
    if (CurRow!=(CurLine-BeginLine)) PaintFlag=PAINT_WIN;
    CurRow=CurLine-BeginLine;
    if (CurCol<0) {
       CurCol=0;
       PaintFlag=PAINT_WIN;
    }

    //**************** set color fields ************
    if ( StatusColor!=fld->StatusColorW
      || TextDefBkColor!=fld->TextBkColorW
      || StatusBkColor!=fld->StatusBkColorW) PaintFlag=PAINT_WIN;

    if ( StatusColor!=fld->StatusColorW
      || StatusBkColor!=fld->StatusBkColorW) {
         if (TerArg.ShowStatus) RefreshStatus=TRUE;
         if (StatusBkColor!=fld->StatusBkColorW) {    // create a new toolbar background brush
            if (hToolbarBrush) DeleteObject(hToolbarBrush);
            hToolbarBrush=CreateSolidBrush(fld->StatusBkColorW);
            if (hToolBarWnd) InvalidateRect(hToolBarWnd,NULL,TRUE);   // show the tool bar
            if (hPvToolBarWnd) InvalidateRect(hPvToolBarWnd,NULL,TRUE); // show the tool bar
            RulerPending=TRUE;
         }
    }

    // apply the background color
    if (TextDefBkColor!=fld->TextBkColorW) TerSetCtlColor(hTerWnd,fld->TextBkColorW,FALSE);

    StatusBkColor  =0xFFFFFF&fld->StatusBkColorW; // background color of the status line
    StatusColor    =0xFFFFFF&fld->StatusColorW;   // foreground color of the status line

    if (RefreshStatus) DisplayStatus(w);

    LinkColor      =fld->LinkColorW;         // color of the linked text
    SnapToGrid     =fld->SnapToGridW;        // snap ruler items to an invisible grid
    HtmlMode       =fld->HtmlModeW;          // html mode adjustments

    if (ShowTableGridLines!=fld->ShowTableGridLinesW) PaintFlag=PAINT_WIN;
    ShowTableGridLines=fld->ShowTableGridLinesW; // shwo grid lines

    ModifyProtectColor=fld->ModifyProtectColorW; // modify the protected text color

    //**************** set highlighting fields ************

    HilightType    =fld->HilightTypeW;       // line/col or character block
    HilightBegRow  =fld->HilightBegRowW;     // beginning row
    HilightEndRow  =fld->HilightEndRowW;     // ending row
    HilightBegCol  =fld->HilightBegColW;     // beginning col
    HilightEndCol  =fld->HilightEndColW;     // ending col

    StretchHilight =fld->StretchHilightW;    // TRUE = highlight being stretch

    // ***** other fields
    LinkStyle       =fld->LinkStyleW;        // Link character style
    LinkDblClick    =fld->LinkDblClickW;     // Invoke link on double click
    ShowProtectCaret=fld->ShowProtectCaretW; // Show caret on protected text

    //*************** set text data ************
    LineNo=-1;
    if (fld->TextApply==APPLY_MOD_CUR_LINE) {  // update current line
       LineNo=CurLine;
    }
    else if (fld->TextApply==APPLY_BEF_CUR_LINE) {  // insert before current line
       MoveLineArrays(w,CurLine,1,'B');               // create a line before Current Line
       LineNo=CurLine;
    }
    else if (fld->TextApply==APPLY_AFT_CUR_LINE) {  // insert before current line
       MoveLineArrays(w,CurLine,1,'A');               // create a line before Current Line
       LineNo=CurLine+1;
    }

       //************** apply the line data *************
    if (LineNo>=0) {
       if (fld->LineLenW>MAX_WIDTH-1) fld->LineLenW=MAX_WIDTH-1;
       if (fld->LineLenW<0) fld->LineLenW=0;

       LineAlloc(w,LineNo,LineLen(LineNo),fld->LineLenW); // allocate space
       //************* store in text data *************
       ptr=pText(LineNo);
       for (i=0;i<LineLen(LineNo);i++) ptr[i]=fld->text[i];


       //************* store in font data *************
       fmt=OpenCfmt(w,LineNo);
       for (i=0;i<LineLen(LineNo);i++) {
          fmt[i]=DEFAULT_CFMT;
          if (fld->font[i]<=TotalFonts && TerFont[fld->font[i]].InUse) fmt[i]=fld->font[i];
       }
       CloseCfmt(w,LineNo);

       pfmt(LineNo)=DEFAULT_PFMT;
       if (fld->pfmtW<=TotalPfmts) pfmt(LineNo)=fld->pfmtW;

       TerArg.modified++;
       PaintFlag=PAINT_WIN;
    }

    WrapFlag       =fld->WrapFlagW;          // Wrap control, see WRAP_ constants
    ReclaimResources =fld->ReclaimResourcesW;// reuse font and para ids

    if (!PaintEnabled && fld->PaintEnabledW) PaintFlag=PAINT_WIN;
    PaintEnabled   =fld->PaintEnabledW;      // FALSE=disables the painting and word wrapping
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
   DocFitsInWindow:
   Returs TRUE when all lines fit in the current window height
*******************************************************************************/
BOOL DocFitsInWindow(PTERWND w)
{
    int DocSize=0;
    long l;

    for (l=0;l<TotalLines;l++) {
       DocSize+=ScrLineHeight(w,l,FALSE,FALSE);
       if (DocSize>TerWinHeight) return FALSE;
    }

    return TRUE;
}

/******************************************************************************
   GetStatusLineNo:
   Get the line number for status display.
*******************************************************************************/
long GetStatusLineNo(PTERWND w)
{
    long line,l;
    BOOL InPageHdrFtr;

    if  (!TerArg.WordWrap || !TerArg.PrintView || TerArg.FittedView) {
       line=CurLine;
       if (TerArg.FittedView && (TerFlags3&TFLAG3_TABLE_STATUS_LINE)) line-=GetStatusLineNoAdj(w,0,CurLine);
       return line+1;
    }

    line=CurLine+1-PageInfo[CurPage].FirstLine;

    if (TerArg.PageMode) {
       long PageLines,HdrLines,FirstLine,FirstBodyLine;
       struct StrHdrFtr hdr;
       
       if  (CurPage<(TotalPages-1)) PageLines=PageInfo[CurPage+1].FirstLine-PageInfo[CurPage].FirstLine;
       else                         PageLines=TotalLines-PageInfo[CurPage].FirstLine;

       // subtract the hdr/ftr lines from the beginning of the page
       FirstLine=FirstBodyLine=PageInfo[CurPage].FirstLine;
       if (PfmtId[pfmt(FirstLine)].flags&PAGE_HDR_FTR) {
          for (l=FirstLine;l<TotalLines && PfmtId[pfmt(l)].flags&PAGE_HDR_FTR && PageLines>1;l++) PageLines--;
          FirstBodyLine=l;
       }
       
       if (ViewPageHdrFtr) {

           int sect=PageHdrSect(w,CurPage,&hdr);
           if (sect<0) hdr.FirstLine=hdr.LastLine=-1;

           HdrLines=hdr.LastLine-hdr.FirstLine+1-2;  // excluding the delimiter lines
           if (hdr.FirstLine<0) HdrLines=0;
           else if (HdrLines==1 && hdr.FirstLine>=0 && (hdr.FirstLine+1)<TotalLines 
               && LineLen(hdr.FirstLine+1)==1 && !EditPageHdrFtr) HdrLines=0;
            
           if  (PfmtId[pfmt(CurLine)].flags&PAGE_HDR) {
              line=0;
              for (l=CurLine;l>=0 && !(LineFlags(l)&LFLAG_HDRS_FTRS); l--) line++;  // line number within header section
           }
           else if (PfmtId[pfmt(CurLine)].flags&PAGE_FTR) {
              line=HdrLines+PageLines;
              for (l=CurLine;l>=0 && !(LineFlags(l)&LFLAG_HDRS_FTRS); l--) line++;  // line number within footer section
           }
           else    line=CurLine-FirstBodyLine+HdrLines+1;
       }
       else {
           line=CurLine-FirstBodyLine+1;

           if (TerFlags3&TFLAG3_TABLE_STATUS_LINE) line-=GetStatusLineNoAdj(w,PageInfo[CurPage].FirstLine,PageInfo[CurPage].LastLine);
       }
    }


    // subtract any intermediate section hdr/footer lines
    l=PageInfo[CurPage].FirstLine;
    InPageHdrFtr=(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR);

    for (;l<CurLine;l++) {
       if (InPageHdrFtr && PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;  // skip initial header footer
       InPageHdrFtr=FALSE;
       if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) line--;   // adjust for hidden intermediate header/footer
    }

    if (line<1) line=1;

    // ignore objects
    if ((TerFlags5&TFLAG5_NO_OBJ_IN_STATUS_LINE) && fid(CurLine)==0) {
      for (l=PageInfo[CurPage].FirstLine;l<CurLine;l++) {
         if (fid(l)!=0) line--;   // adjust for hidden intermediate header/footer
      }
      if (line<1) line=1;
    } 

    return line;
}

/******************************************************************************
   GetStatusLineNoAdj:
   Get the lines to subtract from the status line to adjust for table cell lines
*******************************************************************************/
long GetStatusLineNoAdj(PTERWND w, long FirstLine, long LastLine)
{
    long l,adj=0,CellLineCount=0,RowLineCount=0,MaxCellLines=0;

    if (CurLine<FirstLine) return 0L;

    for (l=FirstLine;l<=LastLine;l++) {
       if (l==CurLine) {                           // break now
          if (cid(CurLine)==0) break;
          adj+=(RowLineCount-CellLineCount);
          break;
       }

       if (cid(l)) {
          RowLineCount++;
          CellLineCount++;
       }

       if (LineInfo(w,l,INFO_CELL)) {  // end of a cell
          if (CellLineCount>MaxCellLines) MaxCellLines=CellLineCount;
          CellLineCount=0;
       }
       else if (LineInfo(w,l,INFO_ROW)) {  // end of a table row
          adj+=(RowLineCount-MaxCellLines);    // add one for the current line
          MaxCellLines=RowLineCount=CellLineCount=0;
       }
    }

    return adj;
}

/******************************************************************************
   LineRectToPoints:
   Convert the line rectangle to points (in twips) relative to page.
*******************************************************************************/
LineRectToPoints(PTERWND w, int ParaFID, LPINT x1, LPINT y1, LPINT x2, LPINT y2)
{
   if (!(ParaFrame[ParaFID].flags&PARA_FRAME_LINE)) return TRUE;

   // get the line coordinates
   (*x1)=ParaFrame[ParaFID].x;
   (*x2)=ParaFrame[ParaFID].x+ParaFrame[ParaFID].width;
   (*y1)=ParaFrame[ParaFID].y;
   (*y2)=ParaFrame[ParaFID].y+ParaFrame[ParaFID].MinHeight;
   if (ParaFrame[ParaFID].LineType==DOL_DIAG) SwapInts(y1,y2);  // swap the points
   if (ParaFrame[ParaFID].LineType==DOL_HORZ) (*y2)=(*y1);
   if (ParaFrame[ParaFID].LineType==DOL_VERT) (*x2)=(*x1);

   return TRUE;
}

/******************************************************************************
   LinePointsToRect:
   Convert the line points to the rectangle.
*******************************************************************************/
LinePointsToRect(PTERWND w, int ParaFID, int x1, int y1, int x2, int y2)
{
   if (!(ParaFrame[ParaFID].flags&PARA_FRAME_LINE)) return TRUE;

   if (x2<x1) {    // swap the points
      SwapInts(&x1,&x2);
      SwapInts(&y1,&y2);
   }

   ParaFrame[ParaFID].x=x1;
   ParaFrame[ParaFID].width=x2-x1;
   if (y2>y1) {
      ParaFrame[ParaFID].y=y1;
      ParaFrame[ParaFID].MinHeight=y2-y1;
      ParaFrame[ParaFID].LineType=DOL_BDIAG;
   }
   else {
      ParaFrame[ParaFID].y=y2;
      ParaFrame[ParaFID].MinHeight=y1-y2;
      ParaFrame[ParaFID].LineType=DOL_DIAG;
   }
   if (y1==y2) ParaFrame[ParaFID].LineType=DOL_HORZ;
   if (x1==x2) ParaFrame[ParaFID].LineType=DOL_VERT;

   ParaFrame[ParaFID].height=ParaFrame[ParaFID].MinHeight;

   return TRUE;
}

/******************************************************************************
    SetPageFromY:
    When two pages are created in PageMode, this function sets the CurPage to
    the page that contains the given y.
*******************************************************************************/
SetPageFromY(PTERWND w,int y)
{
    if (!TerArg.PageMode) return TRUE;

    CurPage=FirstFramePage;
    if (y>FirstPageHeight) CurPage++;
    if (CurPage>=TotalPages) CurPage--;

    return TRUE;
}

/******************************************************************************
    IsHtmlRule:
    Returns TRUE if the given para is an html horizontal rule.
*******************************************************************************/
BOOL IsHtmlRule(PTERWND w,int ParaId)
{
    int ShiftBits=9,RuleValue=3;
    UINT DataMask=0x7;    // 3bits
    UINT AuxId=PfmtId[ParaId].AuxId;

    AuxId=(AuxId>>ShiftBits)&DataMask;     // extract para misc type
    return ((AuxId==(UINT)RuleValue)?TRUE:FALSE);
}

/******************************************************************************
    IsHtmlList:
    Returns TRUE if the given para belongs to an html list.
*******************************************************************************/
BOOL IsHtmlList(PTERWND w,int ParaId)
{
    int ShiftBits=3,ListType;
    UINT DataMask=0x7;    // 3bits to store list types
    UINT AuxId=PfmtId[ParaId].AuxId;

    ListType=(AuxId>>ShiftBits)&DataMask;     // extract para list type
    return (BOOL)ListType;
}

/******************************************************************************
    HtmlListLevel:
    Returns the html list level
*******************************************************************************/
HtmlListLevel(PTERWND w,int ParaId)
{
    UINT DataMask=0x7;    // 3bits to store list levels
    UINT AuxId=PfmtId[ParaId].AuxId;

    return AuxId&DataMask;     // extract para list level
}

/******************************************************************************
     TerSetClipRgn:
     Set the clip region to the text area
******************************************************************************/
BOOL TerSetClipRgn(PTERWND w)
{
    HRGN rgn;

    if (CurClipRgn) SelectClipRgn(hTerDC,CurClipRgn);
    else {
       rgn=CreateRectRgn(TerWinRect.left,TerWinRect.top,TerWinRect.right,TerWinRect.bottom/*-1*/);
       SelectClipRgn(hTerDC,rgn);
       DeleteObject(rgn);
    }

    return TRUE;
}

/******************************************************************************
     TerResetClipRgn:
     Reset the clip region for the text area
******************************************************************************/
BOOL TerResetClipRgn(PTERWND w)
{
    SelectClipRgn(hTerDC,NULL);              // reset the clip region
    return TRUE;
}

/******************************************************************************
    TerLButtonDown:
    Processing for left button down
*******************************************************************************/
TerLButtonDown(PTERWND w,BOOL edit, UINT message, WPARAM wParam,LPARAM lParam)
{
    int FrameNo;
    int CurFont;
    HWND hSaveWnd;
    BOOL OnHiddenText;
    BOOL AllowMouseSel=!(TerFlags3&TFLAG3_NO_MOUSE_SEL);
    BOOL NewFocus=FALSE;

    if (ActiveOleObj>=0) DeactivateEditOle(w);   // deactive any current ole object

    if (hTerWnd!=GetFocus()) {
       SetFocus(hTerWnd);
       NewFocus=TRUE;                    // just got focus
    }

    InAutoComp=false;                    // reset autocomplete 

    TerMousePos(w,lParam,TRUE);          // get mouse position in MouseCol and MouseLine variables

    if (!NewFocus && (MouseLine!=CurLine || MouseCol!=CurCol)) InputFontId=-1;  // reset font

    if (RulerClicked) {
       if (edit && CurDragObj<0) DoRulerClick(w,message,lParam);
    }
    else if (MouseOverShoot==' ') {
       BOOL PrevPictClicked=PictureClicked;
       PictureClicked=FrameClicked=FALSE;
    
       if (AllowMouseSel) {
         if (TblSelCursShowing) HilightTableCol(w,MouseLine,TRUE,TRUE);  // highlight a table column
         else if (HilightType==HILIGHT_CHAR) {
             if (GetKeyState(VK_SHIFT)&0x8000)   // resume hilighting
                StretchHilight=TRUE;
             else  {
                if (CurDragObj<0) {
                   if (message==WM_LBUTTONDOWN) {
                      if (CanDragText(w)) {
                         SendMessageToParent(w,TER_DRAG,(WPARAM)hTerWnd,0L,FALSE);
                         DraggingText=TRUE;  // dragging text
                         if (True(TerFlags6&TFLAG6_OLE_DROP_SOURCE)) OleDragText(w);
                      }
                      if (!DraggingText) {
                         HilightType=HILIGHT_OFF; // turnoff character highlighting
                         DblClickHilight=FALSE;
                         if (!PrevPictClicked) PaintTer(w);  // suspend painting if drag object is selected
                      }
                   }
                }
                else {
                   if (DragObj[CurDragObj].type==DRAG_TYPE_CELL || DragObj[CurDragObj].type==DRAG_TYPE_ROW_INDENT)
                       MarkCells(w,IDC_TABLE_SEL_ROWS);
                   DragObj[CurDragObj].TextHilighted=(HilightType!=HILIGHT_OFF);
                   HilightType=HILIGHT_OFF; // turnoff character highlighting
                }
             }
         }
         else if (HilightType==HILIGHT_OFF && GetKeyState(VK_SHIFT)&0x8000) {
             HilightType=HILIGHT_CHAR;
             if (CaretEngaged) {
                HilightBegRow=CurLine;
                HilightBegCol=CurCol;
             }
             else AbsToRowCol(w,CaretPos,&HilightBegRow,&HilightBegCol);   // set the current position

             HilightEndRow=MouseLine;
             HilightEndCol=MouseCol;
             StretchHilight=TRUE;
         }
       }

       if (MouseLine>=TotalLines) TerMousePos(w,lParam,TRUE);  // painting above might have changed the line count

       // position at the line
       if (CurDragObj<0) {
          CurCol=MouseCol;
          CurFont=GetCurCfmt(w,MouseLine,CurCol);
          OnHiddenText=MoveCursor(w,MouseLine,CurCol);   //HiddenText(w,CurFont);
          if (OnHiddenText) CursDirection=(TerFlags2&TFLAG2_CURSOR_BEF_HIDDEN)?CURS_BACKWARD:CURS_FORWARD;

          if (CurLine!=MouseLine || hScrollBM || HilightType==HILIGHT_CHAR || OnHiddenText) { // hScrollBM checked to do the real painting
             if (hScrollBM || HilightType==HILIGHT_CHAR || OnHiddenText) {
                TerPosLine(w,MouseLine+1); // position cursor at the current mouse line
             }
             else {
                CurLine=MouseLine;
                CurRow=CurLine-BeginLine;
                DisplayStatusInfo(w);      // display status information
                DrawRuler(w,FALSE);
                // erase and delete the drag objects
                if (PictureHilighted) DrawDragPictRect(w);       // erase the picture rectangle
                if (FrameTabsHilighted) DrawDragFrameTabs(w);     // erase the frame size tabs
                DeleteDragObjects(w,DRAG_TYPE_SCR_FIRST,DRAG_TYPE_SCR_LAST);
             }
          }
          else DisplayStatusInfo(w);      // display status information

          // end stylesheet session
          if (CurSID>=0) TerEditStyle(hTerWnd,TRUE,NULL,TRUE,0,TRUE);
          if (SpellPending || PageBreakShowing || LineInfo(w,CurLine,INFO_PAGE)) PaintTer(w);  // to indicate the misspelled word
       }
       else DisplayStatusInfo(w);                            // display status information

       if ((FrameNo=GetFrame(w,CurLine))>=0 && frame[FrameNo].ParaFrameId>0) FrameClicked=TRUE;

       CurFont=GetCurCfmt(w,CurLine,CurCol);

       if (  TerFont[CurFont].style&PICT && !(TerFont[CurFont].style&PROTECT) && AllowMouseSel
          && HilightType==HILIGHT_OFF /*&& !TerArg.ReadOnly*/ /*&& !HtmlMode*/) {
          
          BOOL IsReadOnlyControl=(TerArg.ReadOnly && True(TerFlags4&TFLAG4_READONLY_CONTROLS) && IsControl(w,CurFont));
          
          if (FrameClicked && (TerFlags2&TFLAG2_SELECT_FRAME_PICT)) { // check if frame click need to be reset
             int PrevFID=(CurLine==0?0:fid(CurLine-1));
             int NextFID=(CurLine==(TotalLines-1)?0:fid(CurLine+1));
             if (PrevFID!=fid(CurLine) && NextFID!=fid(CurLine) && LineLen(CurLine)==2) FrameClicked=FALSE; // the frame contains nothing but the picture
          }

          if (!FrameClicked && !IsReadOnlyControl) {
             PictureClicked=TRUE;
             HilightType=HILIGHT_CHAR;
             HilightBegRow=HilightEndRow=CurLine;
             HilightBegCol=CurCol;
             HilightEndCol=CurCol+1;
             StretchHilight=FALSE;
             CursDirection=CURS_FORWARD;
          }
       }
    }

    // begin monitoring mouse movement
    if ((!RulerClicked && MouseOverShoot==' ') || CurDragObj>=0) {
        if (message==WM_LBUTTONDOWN || message==WM_MBUTTONDOWN) IgnoreMouseMove=FALSE;
        else if (!(TerArg.WordWrap)) IgnoreMouseMove=FALSE; // beginning of line block - available in non-wordwrap mode only
        if (NewFocus) IgnoreMouseMove=TRUE;
    }

    CaretEngaged=TRUE;                 // caret engaged to the current position
    CurCtlId=-1;                       // reset id for any selected control

    if (ImeEnabled) DisableIme(w,TRUE);    // disable ime processing

    // send link message
    if (!RulerClicked && MouseOverShoot==' ' && MouseOnTextLine && LinkCursShowing) {
       hSaveWnd=hTerWnd;
       
       if (!LinkDblClick && SendLinkMessage(w,FALSE)) {
          if (!IsWindow(hSaveWnd)) return FALSE;   // this window destroyed by the parent
          IgnoreMouseMove=TRUE;   // assume MOUSEUP
       }
       else JumpToPageRefBookmark(w,true);
    }

    // check if located on a text link
    if (GetKeyState(VK_CONTROL)&0x8000) {     // check if located on a text starting with http:// or www.
       InvokeTextLink(w,true,CurLine,CurCol);
    } 

    return TRUE;
}

/******************************************************************************
    SetMouseStopTimer:
    Set timer for mouse-stop activities.
******************************************************************************/
BOOL SetMouseStopTimer(PTERWND w,LPARAM lParam)
{
    int x,y;

    x=MouseX=(int)(short)LOWORD(lParam); // casting preserves sign
    y=MouseY=(int)(short)HIWORD(lParam);

    if (MouseStopTimerOn && x==MouseStopX && y==MouseStopY) return TRUE;  
    MouseStopX=x;
    MouseStopY=y;

    TerMousePos(w,lParam,TRUE);          // get mouse position in MouseCol and MouseLine variables
    if (MouseStopTimerOn && MouseLine==MouseStopLine && MouseCol==MouseStopCol) return true;  
    if (MouseStopDone && MouseLine==MouseStopLine && MouseCol==MouseStopCol) return true;  // mouse-stop activity done for this location

    MouseStopLine=MouseLine;
    MouseStopCol=MouseCol;

    if (MouseStopTimerOn) return true;  // new position set, let the timer continue

    SetTimer(hTerWnd,TIMER_MOUSE_STOP,500,NULL);          // in milliseconds 
    MouseStopTimerOn=true;
    MouseStopDone=false;                // true when mouse-stop activity is done for this location

    return true;
} 

/******************************************************************************
    MouseStopAction:
    Do mouse-stop activities.
******************************************************************************/
BOOL MouseStopAction(PTERWND w)
{
    BYTE str[MAX_WIDTH+1];

    KillTimer(hTerWnd,TIMER_MOUSE_STOP);     // kill the current timer
    MouseStopTimerOn=false;

    // clear any old mouse-stop message
    if (MouseStopMsgOn) PaintTer(w);
     
    // do all mouse-stop activities here
    if (TrackingComment(w,MouseStopLine,MouseStopCol,str)) {
       DrawMouseStopMsg(w,hTerDC,str);
    } 
    else if (InvokeTextLink(w,false,MouseStopLine,MouseStopCol)) {  // positioned on a text line
       DrawMouseStopMsg(w,hTerDC,MsgString[MSG_INVOKE_TEXT_LINK]);
    } 
    
    MouseStopDone=true;                     // mouse stop activity done for this location
    return true;
}
 
/******************************************************************************
     InvokeTextLink:
     Check if located on a text link.  invoke if requested
******************************************************************************/
BOOL InvokeTextLink(PTERWND w, BOOL invoke,long line, int col)
{
     LPBYTE ptr;
     int i,len,TokLen,StartCol,CurFont;
     BOOL found=false;
     BYTE tok[10];
     BYTE url[MAX_WIDTH+1];

     if (True(TerFlags6&TFLAG6_NO_LINK_MSG)) return false;

     if (line<0 || line>=TotalLines) return false;
     ptr=pText(line);

     len=LineLen(line);
     if (col<0 || col>=len) return false;

     CurFont=GetCurCfmt(w,line,col);
     if (TerFont[CurFont].FieldId!=0) return false;

     lstrcpy(tok,"http://");  // token to located
     TokLen=strlen(tok);
     if (TokLen<len) {
       StartCol=col;
       if ((StartCol+TokLen)>len) StartCol=len-TokLen;
       for (i=StartCol;i>=0;i--) {
          if (strncmpi(&(ptr[i]),tok,TokLen)==0) {
             found=true;
             break;
          }
          if (ptr[i]==' ') break;
       }
       if (found) goto FOUND;
     }

     lstrcpy(tok,"www.");  // token to located
     TokLen=strlen(tok);
     if (TokLen<len) {
       StartCol=col;
       if ((StartCol+TokLen)>len) StartCol=len-TokLen;
       for (i=StartCol;i>=0;i--) {
          if (strncmpi(&(ptr[i]),tok,TokLen)==0) {
             found=true;
             break;
          }
          if (ptr[i]==' ') break;
       } 
       if (found) goto FOUND;
     }

     return FALSE; // token not found

     FOUND:
     if (!invoke) return TRUE;  // token found

     StartCol=i;      // start loation

     // extract the url
     for (i=StartCol;i<len;i++) {
        if (ptr[i]==' ' || ptr[i]<32 || ptr[i]=='<' || ptr[i]=='>') break;
        url[i-StartCol]=ptr[i];
     } 
     url[i-StartCol]=0;     // url located

     // invoke the url
     ShellExecute(hTerWnd,"open",url,NULL,NULL,0);

     return true;
} 

/******************************************************************************
     OurSetCaretPos:
     Set the caret position
******************************************************************************/
BOOL OurSetCaretPos(PTERWND w)
{
    int CurFont=-1,y,height,CaretCol,SpcBef,NewCaretVert;
    long CaretLine=-1;

    if (CaretEnabled) {     // don't affect caret if intentionally hidden
      CaretPositioned=TRUE; // set to positioned

      // test for caret engagement
      if (CaretEngaged) {   // caret position same as current position
         CaretLine=CurLine;
         CaretCol=CurCol;
      }
      else AbsToRowCol(w,CaretPos,&CaretLine,&CaretCol);

      // get the Y position of the caret
      y=LineToUnits(w,CaretLine);

      // adjust the caret height if needed
      height=GetRowHeight(w,CaretLine);
      GetLineSpacing(w,CaretLine,height,&SpcBef,NULL,TRUE);
      y+=SpcBef;            // adjust for space before the paragraph

      if (y<TerWinOrgY) {   // handle the partial begin lines
         height=height-(TerWinOrgY-y);
         y=TerWinOrgY;
      }
      if ((height+y-TerWinOrgY)>TerWinHeight) height=TerWinHeight-y+TerWinOrgY;
      NewCaretVert=(LineTextAngle(w,CaretLine)==0);

      if (CaretHeight!=height || CaretVert!=NewCaretVert) InitCaret(w);

      // display the caret
      if (IsCaretVisible(w,CaretLine,CaretCol)) {
         int CaretX,CaretY;
         GetCaretXY(w,CaretLine,CaretCol,y,&CaretX,&CaretY);
         SetCaretPos(CaretX,CaretY); // position caret at current row/col position
      }

      // show picture rectangle
      CurFont=GetCurCfmt(w,CaretLine,CaretCol);
      if (TerFont[CurFont].style&PICT && PictureClicked) {
         int frm=TerFont[CurFont].ParaFID;
         if (frm>0 && True(ParaFrame[frm].flags&PARA_FRAME_LINE)) {
            int FrameNo=TerFont[CurFont].DispFrame;
            if (FrameNo>=0) ShowFrameDragObjects(w,FrameNo,frm);
         } 
         else if (!(TerFlags&TFLAG_NO_EDIT_PICT)) {
            if (HilightType==HILIGHT_CHAR) ShowPictureDragObjects(w,CurFont);
            else                           PictureClicked=FALSE;   // hilight was removed 
         }
      }

      // show the frame tabs
      if (FrameClicked) {
         int FrameNo=GetFrame(w,CaretLine);
         if (FrameNo>=0 && frame[FrameNo].ParaFrameId>0) ShowFrameDragObjects(w,frame[FrameNo].BoxFrame,-1);
      }
    }
    else {  
      // show picture rectangle
      CurFont=GetCurCfmt(w,CurLine,CurCol);
      if (TerFont[CurFont].style&PICT && PictureClicked && !(TerFlags&TFLAG_NO_EDIT_PICT)) ShowPictureDragObjects(w,CurFont);
    }

    // check if the caret positioned on a footnote text
    InFootnote=FALSE;
    if (CaretLine==-1) {    // calculate if not already calculated
      if (CaretEngaged) {   // caret position same as current position
         CaretLine=CurLine;
         CaretCol=CurCol;
      }
      else AbsToRowCol(w,CaretPos,&CaretLine,&CaretCol);
    }
    if (CurFont==-1) CurFont=GetCurCfmt(w,CaretLine,CaretCol);  // calculate if not already calculated
    if (TerFont[CurFont].style&(FNOTETEXT|FNOTEREST)) InFootnote=TRUE;

    return TRUE;
}

/******************************************************************************
    GetCaretXY:
    This function returns caret x/y position after rotation and rtl manipulation
******************************************************************************/
BOOL GetCaretXY(PTERWND w,int CaretLine, int CaretCol, int y, LPINT pX, LPINT pY)
{
    int x;
    int CaretX,CaretY=y,FrameNo;

    x=CaretX=ColToUnits(w,CaretCol,CaretLine,LEFT); // position caret at current row/col position
    
    // adjust x for RTL text
    if (LineFlags2(CaretLine)&LFLAG2_RTL) {
       pScrSeg=GetLineSeg(w,CaretLine,NULL,&TotalScrSeg);
       if (pScrSeg && TotalScrSeg>0) {
          if ((FrameNo=GetFrame(w,CaretLine))>=0) {
             CurScrSeg=GetCharSeg(w,CaretLine,CaretCol,TotalScrSeg,pScrSeg);
             if (CaretCol==0) {
                CaretX=x=RtlX(w,x,0,FrameNo,&(pScrSeg[CurScrSeg]));
                if (CaretX>=TerWinWidth) CaretX--;  // ensure that the caret remains visible
             }
             else {      // [posistion after the previous character
                int col=CaretCol-1;
                CurScrSeg=GetCharSeg(w,CaretLine,col,TotalScrSeg,pScrSeg); // position after the previous character
                x=ColToUnits(w,col,CaretLine,TER_RIGHT);
                CaretX=x=RtlX(w,x,0,FrameNo,&(pScrSeg[CurScrSeg]));
             } 
          }    
          MemFree(pScrSeg);
          pScrSeg=NULL;
          TotalScrSeg=0;
       }
    } 
         
    // adjust x/y for text rotation
    if (TerArg.PageMode) {
       int TextAngle=0,ParaFID,CellId;
       ParaFID=fid(CaretLine);
       CellId=cid(CaretLine);
       if (ParaFID!=0) TextAngle=ParaFrame[ParaFID].TextAngle;
       if (TextAngle==0 && CellId>0) TextAngle=cell[CellId].TextAngle;

       if (TextAngle>0 && (FrameNo=GetFrame(w,CaretLine))>=0) {
          CaretX=FrameRotateX(w,x,y,FrameNo);
          CaretY=FrameRotateY(w,x,y,FrameNo);
          if (TextAngle==270) CaretX-=CaretHeight;
       }
    } 
         
    if (pX) (*pX)=CaretX;
    if (pY) (*pY)=CaretY;

    return TRUE;
}
 
/******************************************************************************
    IsDefLangRtl:
    Is default language right-to-left?
******************************************************************************/
BOOL IsDefLangRtl(PTERWND w)
{
   int i;

   for (i=0;i<TotalFonts;i++) {
      if (TerFont[i].InUse && !(TerFont[i].style&PICT)) {
          if (TerFont[i].lang==DefLang) return TerFont[i].rtl;
          if (TerFont[i].lang==0 && TerFont[i].rtl) return TRUE;
      }
   }
 
   return FALSE;
}
 
/******************************************************************************
    IsImeMessage:
    This function returns TRUE if it encounters an ime message.
******************************************************************************/
#pragma optimize("g",off)  // turn-off global optimization
BOOL IsImeMessage(PTERWND w,UINT message, WPARAM wParam, LPARAM lParam)
{

    HIMC hImc;          // ime context
    int i,len,CurCharLen,count,CursPos,DeltaStart,CursPosChar,DeltaStartChar,StartCol;
    BYTE SaveByte,LByte,TByte;
    LPBYTE ptr,lead,pString;
    BOOL result,CursorSet=FALSE;
    WORD UcBase;
    LPWORD fmt;

    //if (!mbcs && (   message==WM_IME_STARTCOMPOSTION || message==WM_IME_ENDCOMPOSTION 
    //              || message==WM_IME_COMPOSITION || message==WM_IME_NOTIFY || message==WM_IME_COMPOSITIONFULL)) EnableMbcs(

    if (!InlineIme) return FALSE;    // inline ime not enabled

    if (!ImmIsUIMessage(NULL,message,wParam,lParam)) return FALSE;  // not an ime message

    //DispMsg(message);
    if (message==WM_IME_STARTCOMPOSITION) {
       ImeStartString(w);
    }
    else if (message==WM_IME_ENDCOMPOSITION) {
       ImeEndString(w);
    }
    else if (message==WM_IME_COMPOSITION) {
       BOOL IsUnicode=FALSE;
       WORD StringU[300];
       int  LenU,ResultCharLen,ResultLen;
       long HilightBeg,HilightEnd;


       //if (lParam&GCS_COMPATTR) OurPrintf("GCS_COMPATTR");
       //if (lParam&GCS_COMPCLAUSE) OurPrintf("GCS_COMPCLAUSE");
       //if (lParam&GCS_COMPREADATTR) OurPrintf("GCS_COMPREADATTR");
       //if (lParam&GCS_COMPREADCLAUSE) OurPrintf("GCS_COMPREADCLAUSE");
       //if (lParam&GCS_COMPREADSTR) OurPrintf("GCS_COMPREADSTR");
       //if (lParam&GCS_COMPSTR) OurPrintf("GCS_COMPSTR");
       //if (lParam&GCS_CURSORPOS) OurPrintf("GCS_CURSORPOS");
       //if (lParam&GCS_DELTASTART) OurPrintf("GCS_DELTASTART");
       //if (lParam&GCS_RESULTCLAUSE) OurPrintf("GCS_RESULTCLAUSE");
       //if (lParam&GCS_RESULTREADCLAUSE) OurPrintf("GCS_RESULTREADCLAUSE");
       //if (lParam&GCS_RESULTREADSTR) OurPrintf("GCS_RESULTREADSTR");
       //if (lParam&GCS_RESULTSTR) OurPrintf("GCS_RESULTSTR");
       //if (lParam&CS_INSERTCHAR) OurPrintf("CS_INSERTCHAR");
       //if (lParam&CS_NOMOVECARET) OurPrintf("CS_NOMOVECARET");
       
       if (!(lParam&(GCS_RESULTSTR|GCS_COMPSTR))) return TRUE;
       
       if (!ImeEnabled) ImeStartString(w);

       hImc=ImmGetContext(hTerWnd);

       HilightType=HILIGHT_OFF;  // reset by default

       CursPos=ImmGetCompositionString(hImc,GCS_CURSORPOS,NULL,0);
       DeltaStart=ImmGetCompositionString(hImc,GCS_DELTASTART,NULL,0);

       // get the result string
       ResultLen=ResultCharLen=0;
       if (lParam&GCS_RESULTSTR) {
         BOOL inserted=FALSE;

         ResultLen=ImmGetCompositionString(hImc,GCS_RESULTSTR,NULL,0);
         pString=OurAlloc(ResultLen+1);
         ResultLen=ImmGetCompositionString(hImc,GCS_RESULTSTR,pString,ResultLen);
         pString[ResultLen]=0;
         ResultCharLen=TerMbcsLen(hTerWnd,pString);

         LenU=ImmGetCompositionStringW(hImc,GCS_RESULTSTR,NULL,0);
         if (LenU>0 && (TerFlags5&TFLAG5_INPUT_TO_UNICODE)) {   
            ImmGetCompositionStringW(hImc,GCS_RESULTSTR,StringU,LenU);
            if (ResultCharLen==(int)(LenU/sizeof(WORD))) {     // replace the compsition string by the result string
               LPBYTE pDataTxt;
               LPWORD pDataUb;
               pDataTxt=OurAlloc(ResultCharLen+1);
               pDataUb=OurAlloc((ResultCharLen+1)*sizeof(WORD));
               StringU[ResultCharLen]=0;
               CrackUnicodeString(w,StringU,pDataUb,pDataTxt);
               ReplaceTextString(w,pDataTxt,pDataUb,ImeStartPos+ImeResultChars,ImeStartPos+ImeResultChars+ImeCharLen-1);
               OurFree(pDataTxt);
               OurFree(pDataUb);
               inserted=TRUE;
            } 
         }
         if (!inserted) {     // insert the ascci text instead
            ReplaceTextString(w,pString,NULL,ImeStartPos+ImeResultChars,ImeStartPos+ImeResultChars+ImeCharLen-1);
         } 
         OurFree(pString);


         ImeResultChars+=ResultCharLen; // increase the result chracetr count
         ImeResultBytes+=ResultLen;
         ImeCharLen=0;                  // reset the composition string
         ImeByteLen=0;
         
         if (true || !(lParam&GCS_COMPSTR)) {
           ImeEndString(w);
           ImmReleaseContext(hTerWnd,hImc);
           return TRUE;
         }
       }
       
       len=ImmGetCompositionString(hImc,GCS_COMPSTR,NULL,0);
       pString=OurAlloc(len+1);
       len=ImmGetCompositionString(hImc,GCS_COMPSTR,pString,len);
       pString[len]=0;
       CurCharLen=TerMbcsLen(hTerWnd,pString);

       // get the string in the unicode format   
       LenU=ImmGetCompositionStringW(hImc,GCS_COMPSTR,NULL,0);
       if (LenU>0) {   
          ImmGetCompositionStringW(hImc,GCS_COMPSTR,StringU,LenU);
          IsUnicode=TRUE;
       }
       
       // convert byte positions of CursPos to the character position
       SaveByte=pString[CursPos];
       pString[CursPos]=0;
       CursPosChar=TerMbcsLen(hTerWnd,pString);
       pString[CursPos]=SaveByte;

       // convert byte positions of DeltaStart to the character position
       SaveByte=pString[DeltaStart];
       pString[DeltaStart]=0;
       DeltaStartChar=TerMbcsLen(hTerWnd,pString);
       pString[DeltaStart]=SaveByte;

       AbsToRowCol(w,ImeStartPos+ImeResultChars,&CurLine,&CurCol);
       StartCol=CurCol;


       if (len>ImeByteLen) {        // insert characters
          AbsToRowCol(w,ImeStartPos+ImeResultChars+ImeCharLen,&CurLine,&CurCol);  // position at the end of the previous string

          count=0;
          for (i=0;i<len;i++) {
             LByte=TByte=0;
             TByte=pString[i];
             if (IsDBCSLeadByte(TByte)) {
                 LByte=TByte;
                 i++;
                 TByte=pString[i];
             }
             if (count>=ImeCharLen) {
                result=TerAscii(w,TByte,LByte);
             }
             count++;
          }

          ImeByteLen=len;
          if (!result) DisableIme(w,FALSE);
       }
       else if (len<ImeByteLen) {   //  delete characters
          AbsToRowCol(w,ImeStartPos+ImeResultChars+CurCharLen,&CurLine,&CurCol);
          for (i=CurCharLen;i<ImeCharLen;i++) TerDel(w);

          ImeByteLen=len;
       }

       if (DeltaStart<len) {   // modify the characters at this location
          AbsToRowCol(w,ImeStartPos+ImeResultChars+DeltaStartChar,&CurLine,&CurCol);

                        
          for (i=DeltaStart;i<len;i++) {
             UcBase=0;
             LByte=TByte=0;
             TByte=pString[i];
             if (IsDBCSLeadByte(TByte)) {
                 LByte=TByte;
                 i++;
                 TByte=pString[i];
             }
             
             if (TerFlags5&TFLAG5_INPUT_TO_UNICODE) {
               if (LByte) {  // convert to unicde
                   WORD UChar;
                   BYTE DbcsText[3];
                   int count;
                   DbcsText[0]=LByte;
                   DbcsText[1]=TByte;
                   DbcsText[2]=0;
                   if ((count=MultiByteToWideChar(GetACP(),MB_PRECOMPOSED,DbcsText,2,&UChar,1))==1) {
                       CrackUnicode(w,UChar,&UcBase,&TByte);
                       LByte=0;
                   }
               }
               else if (IsUnicode) {
                  WORD UChar=StringU[i];
                  CrackUnicode(w,UChar,&UcBase,&TByte);
                  LByte=0;
               }
             }

             OpenTextPtr(w,CurLine,&ptr,&lead);
             ptr[CurCol]=TByte;
             if (lead) lead[CurCol]=LByte;
             CloseTextPtr(w,CurLine);
                
             // set the unicode font
             fmt=OpenCfmt(w,CurLine);
             fmt[CurCol]=SetUniFont(w,fmt[CurCol],UcBase);
                
             CurCol++;
             if (CurCol>=LineLen(CurLine)) {
                CurCol=0;
                CurLine++;
                if (CurLine>=TotalLines) CurLine--;
             }
          }

          PaintFlag=PAINT_LINE;
          PaintTer(w);
       }

       if ((CurCol-StartCol)!=CursPosChar) {  // simply set the cursor postion
          AbsToRowCol(w,ImeStartPos+ImeResultChars+CursPosChar,&CurLine,&CurCol);
          PaintFlag=PAINT_MIN;
          PaintTer(w);
       }

       // check if the composition text needs to be selected
       LenU=ImmGetCompositionStringW(hImc,GCS_COMPATTR,NULL,0);
       pString=OurAlloc(LenU+1);   // attribute is one byte per character
       ImmGetCompositionStringW(hImc,GCS_COMPATTR,pString,LenU);
       HilightBeg=-1;
       for (i=0;i<CurCharLen && i<LenU;i++) {
          if (pString[i]&ATTR_TARGET_NOTCONVERTED) {
            if (HilightBeg==-1) HilightBeg=HilightEnd=i;
            else                HilightEnd=i;
          }
          if (false && HilightBeg!=-1) {
             AbsToRowCol(w,ImeStartPos+ImeResultChars+HilightBeg,&HilightBegRow,&HilightBegCol);
             AbsToRowCol(w,ImeStartPos+ImeResultChars+HilightEnd+1,&HilightEndRow,&HilightEndCol);
             StretchHilight=FALSE;
             HilightType=HILIGHT_CHAR;
             PaintTer(w);
          }  
       }  


       ImeCharLen=CurCharLen;             // length of the string being composed


       if (pString) OurFree(pString);

       ImmReleaseContext(hTerWnd,hImc);
    }
    else return FALSE;

    return TRUE;
}
#pragma optimize("",off)  // restore optimization


/******************************************************************************
    ImeStartString:
    Start IME string input.
******************************************************************************/
BOOL ImeStartString(PTERWND w)
{
   ImeStartPos=RowColToAbs(w,CurLine,CurCol);  // start position of the text
   ImeEnabled=TRUE;
   ImeByteLen=ImeCharLen=0;           // byte and character length of the composition strinsg
   ImeResultBytes=ImeResultChars=0;   // byte and character length of the result strings
   
   return TRUE;
}

/******************************************************************************
    ImeEndString:
    End IME string input.
******************************************************************************/
BOOL ImeEndString(PTERWND w)
{
   if (!ImeEnabled) return TRUE;

   if (!(TerOpFlags&TOFLAG_IME_DISABLE)) {
      AbsToRowCol(w,ImeStartPos+ImeResultChars+ImeCharLen-1,&CurLine,&CurCol);
      NextTextPos(w,&CurLine,&CurCol);
   }

   HilightType=HILIGHT_OFF;  // reset by default
   ResetImeStyle(w,TRUE);
   ImeEnabled=FALSE;
       
   ImeByteLen=ImeCharLen=0;           // byte and character length of the composition strinsg
   ImeResultBytes=ImeResultChars=0;   // byte and character length of the result strings

   return TRUE;
}
 
/******************************************************************************
    IsImeMessage:
    This function returns TRUE if it encounters an ime message.
******************************************************************************/
BOOL OldIsImeMessage(PTERWND w,UINT message, WPARAM wParam, LPARAM lParam)
{

    HIMC hImc;          // ime context
    int i,len,CurCharLen,count,CursPos,DeltaStart,CursPosChar,DeltaStartChar,StartCol;
    BYTE SaveByte,LByte,TByte;
    LPBYTE ptr,lead,pString;
    BOOL result,CursorSet=FALSE;
    WORD UcBase;
    LPWORD fmt;

    //if (!mbcs && (   message==WM_IME_STARTCOMPOSTION || message==WM_IME_ENDCOMPOSTION 
    //              || message==WM_IME_COMPOSITION || message==WM_IME_NOTIFY || message==WM_IME_COMPOSITIONFULL)) EnableMbcs(

    if (!InlineIme) return FALSE;    // inline ime not enabled

    if (!ImmIsUIMessage(NULL,message,wParam,lParam)) return FALSE;  // not an ime message

    if (message==WM_IME_STARTCOMPOSITION) {
       ImeStartPos=RowColToAbs(w,CurLine,CurCol);  // start position of the text
       ImeEnabled=TRUE;
       ImeByteLen=ImeCharLen=0;
    }
    else if (message==WM_IME_ENDCOMPOSITION) {
       if (!(TerOpFlags&TOFLAG_IME_DISABLE)) {
          AbsToRowCol(w,ImeStartPos+ImeCharLen-1,&CurLine,&CurCol);
          NextTextPos(w,&CurLine,&CurCol);
       }

       ResetImeStyle(w,TRUE);
       ImeEnabled=FALSE;
    }
    else if (message==WM_IME_COMPOSITION) {
       BOOL IsUnicode=FALSE,IsUndefined=FALSE;
       WORD StringU[300];
       int  LenU;

       if (!ImeEnabled || (lParam&GCS_RESULTSTR)) return TRUE;

       hImc=ImmGetContext(hTerWnd);

       CursPos=ImmGetCompositionString(hImc,GCS_CURSORPOS,NULL,0);
       DeltaStart=ImmGetCompositionString(hImc,GCS_DELTASTART,NULL,0);

       len=ImmGetCompositionString(hImc,GCS_COMPSTR,NULL,0);
       pString=OurAlloc(len+1);
       len=ImmGetCompositionString(hImc,GCS_COMPSTR,pString,len);
       pString[len]=0;
       CurCharLen=TerMbcsLen(hTerWnd,pString);

       // check if the mbcs string is undefined
       if (CurCharLen==len) {     // no trailing bytes
          IsUndefined=TRUE;
          for (i=0;i<CurCharLen;i++) if (pString[i]!='?') IsUndefined=FALSE;
       } 
       
       // get the string in the unicode format   
       if (IsUndefined || (TerFlags4&TFLAG4_IME_UNICODE)) {
          LenU=ImmGetCompositionStringW(hImc,GCS_COMPSTR,NULL,0);
          if (LenU>0) {   
             ImmGetCompositionStringW(hImc,GCS_COMPSTR,StringU,LenU);
             if (CurCharLen==(LenU/(int)sizeof(WORD))) IsUnicode=TRUE;
          }
       }

       // convert byte positions of CursPos to the character position
       SaveByte=pString[CursPos];
       pString[CursPos]=0;
       CursPosChar=TerMbcsLen(hTerWnd,pString);
       pString[CursPos]=SaveByte;

       // convert byte positions of DeltaStart to the character position
       SaveByte=pString[DeltaStart];
       pString[DeltaStart]=0;
       DeltaStartChar=TerMbcsLen(hTerWnd,pString);
       pString[DeltaStart]=SaveByte;

       AbsToRowCol(w,ImeStartPos,&CurLine,&CurCol);
       StartCol=CurCol;


       if (len>ImeByteLen) {        // insert characters
          AbsToRowCol(w,ImeStartPos+ImeCharLen,&CurLine,&CurCol);  // position at the end of the previous string

          count=0;
          for (i=0;i<len;i++) {
             LByte=TByte=0;
             TByte=pString[i];
             if (IsDBCSLeadByte(TByte)) {
                 LByte=TByte;
                 i++;
                 TByte=pString[i];
             }
             if (count>=ImeCharLen) {
               result=TerAscii(w,TByte,LByte);
             }
             count++;
          }

          ImeByteLen=len;
          if (!result) DisableIme(w,FALSE);
       }
       else if (len<ImeByteLen) {   //  delete characters
          AbsToRowCol(w,ImeStartPos+CurCharLen,&CurLine,&CurCol);
          for (i=CurCharLen;i<ImeCharLen;i++) TerDel(w);

          ImeByteLen=len;
       }

       if (DeltaStart<len) {   // modify the characters at this location
          AbsToRowCol(w,ImeStartPos+DeltaStartChar,&CurLine,&CurCol);


          for (i=DeltaStart;i<len;i++) {
             UcBase=0;
             LByte=TByte=0;
             TByte=pString[i];
             if (IsDBCSLeadByte(TByte)) {
                 LByte=TByte;
                 i++;
                 TByte=pString[i];
             }
                
             if (TerFlags4&TFLAG4_IME_UNICODE && LByte) {  // convert to unicde
                 WORD UChar;
                 BYTE DbcsText[3];
                 int count;
                 DbcsText[0]=LByte;
                 DbcsText[1]=TByte;
                 DbcsText[2]=0;
                 if ((count=MultiByteToWideChar(GetACP(),MB_PRECOMPOSED,DbcsText,2,&UChar,1))==1) {
                     CrackUnicode(w,UChar,&UcBase,&TByte);
                     LByte=0;
                 }
             }
             else if (IsUnicode) {
                WORD UChar=StringU[i];
                CrackUnicode(w,UChar,&UcBase,&TByte);
                LByte=0;
             }
               
             OpenTextPtr(w,CurLine,&ptr,&lead);
             ptr[CurCol]=TByte;
             lead[CurCol]=LByte;
             CloseTextPtr(w,CurLine);
                
             // set the unicode font
             if (UcBase) {
                fmt=OpenCfmt(w,CurLine);
                fmt[CurCol]=SetUniFont(w,fmt[CurCol],UcBase);
             } 
                
             CurCol++;
             if (CurCol>=LineLen(CurLine)) {
                CurCol=0;
                CurLine++;
                if (CurLine>=TotalLines) CurLine--;
             }
          }

          PaintFlag=PAINT_LINE;
          PaintTer(w);
       }

       if ((CurCol-StartCol)!=CursPosChar) {  // simply set the cursor postion
          AbsToRowCol(w,ImeStartPos+CursPosChar,&CurLine,&CurCol);
          PaintFlag=PAINT_MIN;
          PaintTer(w);
       }

       ImeCharLen=CurCharLen;

       if (pString) OurFree(pString);

       ImmReleaseContext(hTerWnd,hImc);
    }
    else return FALSE;

    return TRUE;
}

/******************************************************************************
    DisableIme:
    Disable ime if enabled.
******************************************************************************/
BOOL DisableIme(PTERWND w, BOOL repaint)
{
    #if defined(WIN32)
       HIMC hImc=ImmGetContext(hTerWnd);
       TerOpFlags|=TOFLAG_IME_DISABLE;  // ime disable in progress
       ResetImeStyle(w,repaint);
       ImeEnabled=FALSE;
       ImmNotifyIME(hImc,NI_COMPOSITIONSTR,CPS_CANCEL,0L);
       ImmReleaseContext(hTerWnd,hImc);
       ResetLongFlag(TerOpFlags,TOFLAG_IME_DISABLE);
    #endif

    return TRUE;
}

/******************************************************************************
    ResetImeStyle:
    Reset the ime style on the newly entered characters.
******************************************************************************/
BOOL ResetImeStyle(PTERWND w, BOOL repaint)
{
    long BegLine,EndLine;
    int  BegCol,EndCol;

    if (!ImeEnabled || (ImeResultChars+ImeCharLen)<=0) return TRUE;

    AbsToRowCol(w,ImeStartPos,&BegLine,&BegCol);
    AbsToRowCol(w,ImeStartPos+ImeResultChars+ImeCharLen-1,&EndLine,&EndCol);

    if (InsertMode) SaveUndo(w,BegLine,BegCol,EndLine,EndCol,'I');

    HilightBegRow=BegLine;
    HilightBegCol=BegCol;
    HilightEndRow=EndLine;
    HilightEndCol=EndCol+1;
    HilightType=HILIGHT_CHAR;

    StretchHilight=FALSE;
    TerOpFlags|=TOFLAG_HILIGHT_OFF;   // turnoff highlight after the operation

    SetFontTempStyle(w,TSTYLE_IME,FALSE,repaint);

    ResetLongFlag(TerOpFlags,TOFLAG_HILIGHT_OFF);

    return TRUE;
}

/******************************************************************************
    ResetImeStyle:
    Reset the ime style on the newly entered characters.
******************************************************************************/
BOOL OldResetImeStyle(PTERWND w, BOOL repaint)
{
    long BegLine,EndLine;
    int  BegCol,EndCol;

    if (!ImeEnabled || ImeCharLen<=0) return TRUE;

    AbsToRowCol(w,ImeStartPos,&BegLine,&BegCol);
    AbsToRowCol(w,ImeStartPos+ImeCharLen-1,&EndLine,&EndCol);

    if (InsertMode) SaveUndo(w,BegLine,BegCol,EndLine,EndCol,'I');

    HilightBegRow=BegLine;
    HilightBegCol=BegCol;
    HilightEndRow=EndLine;
    HilightEndCol=EndCol+1;
    HilightType=HILIGHT_CHAR;

    StretchHilight=FALSE;
    TerOpFlags|=TOFLAG_HILIGHT_OFF;   // turnoff highlight after the operation

    SetFontTempStyle(w,TSTYLE_IME,FALSE,repaint);

    ResetLongFlag(TerOpFlags,TOFLAG_HILIGHT_OFF);

    return TRUE;
}

/******************************************************************************
    TerInsertField:
    Insert a field.
******************************************************************************/
BOOL WINAPI _export TerInsertField(HWND hWnd,LPBYTE name, LPBYTE data, BOOL repaint)
{
    PTERWND w;
    int FldFont,ProtFldFont,SaveFontId,CharSet=0;
    long BegLine,BegCol,EndCol,EndLine;
    DWORD SaveFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!CanInsertTextObject(w,CurLine,CurCol)) return FALSE;

    if (!name || lstrlen(name)==0) {
       if (!CallDialogBox(w,"DataFieldParam",DataFieldParam,0L)) return FALSE;
       name=DlgText1;
       data=DlgText2;
       CharSet=(BYTE)DlgInt1;
       if (lstrlen(name)==0) return FALSE;
    }

    SaveFontId=GetEffectiveCfmt(w);
    
    BegLine=CurLine;           // save for undo
    BegCol=CurCol;

    SaveFlags=TerOpFlags2;
    TerOpFlags2|=TOFLAG2_NO_SAVE_UNDO;

    // insert the field name
    HilightType=HILIGHT_OFF;
    if (!CharFmt(w,GetNewFieldId,(DWORD)FIELD_NAME,0L,FALSE)) return FALSE;
    FldFont=InputFontId;
    ProtFldFont=GetNewStyle(w,(WORD)FldFont,PROTECT,TRUE,CurLine,-1);   // set col  to -1 so GetNewStyle does not check the field type
    InputFontId=ProtFldFont;
    InsertBuffer(w,"{",NULL,NULL,NULL,NULL,FALSE);  // insert the opening braces
    InputFontId=FldFont;
    InsertBuffer(w,name,NULL,NULL,NULL,NULL,FALSE); // insert the field name
    InputFontId=ProtFldFont;
    InsertBuffer(w,"}",NULL,NULL,NULL,NULL,FALSE);  // insert the closing braces
    InputFontId=SaveFontId;

    // insert the field data
    if (data && lstrlen(data)>0) {
       HilightType=HILIGHT_OFF;
       if (!CharFmt(w,GetNewFieldId,(DWORD)FIELD_DATA,0L,FALSE)) return FALSE;
       if (InputFontId>=0 && CharSet>0) InputFontId=(int)GetNewCharSet(w,(WORD)InputFontId,(DWORD)CharSet,0L,CurLine,CurCol);
       InsertBuffer(w,data,NULL,NULL,NULL,NULL,FALSE); // insert the field name
    }

    //ReleaseUndo(w);
    TerOpFlags2=SaveFlags;
    EndLine=CurLine;
    EndCol=CurCol;
    PrevTextPos(w,&EndLine,&EndCol);
    SaveUndo(w,BegLine,BegCol,EndLine,EndCol,'I'); // register undo

    InputFontId=-1;
    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
     TerGetField:
     This function gets the field name text or field data text at the specified
     cursor location. Set line to -1 to assume the current cursor location.
     Set the text to NULL to get the length of the text.
     The 'type' can be set to FIELD_NAME or FIELD_DATA.
******************************************************************************/
long WINAPI _export TerGetField(HWND hWnd, long LineNo, int ColNo, int type, BYTE huge *text)
{
    PTERWND w;
    int  i,BegCol,EndCol,StartCol,LastCol;
    LPBYTE ptr,lead;
    long l,BegLine,EndLine,len;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;      // get the pointer to window data

    if (type!=FIELD_NAME && type!=FIELD_DATA) return 0;

    if (text) text[0]=0;

    if (LineNo<0) {          // supply the default
       LineNo=CurLine;
       ColNo=CurCol;
    }

    // get the beginning and ending (exclusive) of the field component
    if (!GetFieldScope(w,LineNo,ColNo,type,&BegLine,&BegCol,&EndLine,&EndCol)) return 0;

    // exclude braces from the field name
    if (type==FIELD_NAME) {
       BegCol++;
       FixPos(w,&BegLine,&BegCol);
       EndCol--;
       FixPos(w,&EndLine,&EndCol);
       if (BegLine==EndLine && EndCol<=BegCol) return 0;
    }

    // Extract the length
    len=0;
    for (l=BegLine;l<=EndLine;l++) {
       StartCol=(l==BegLine)?BegCol:0;
       LastCol=(l==EndLine)?EndCol:LineLen(l);   // exclusive

       OpenTextPtr(w,l,&ptr,&lead);

       for (i=StartCol;i<LastCol;i++) {
          if (LeadByte(lead,i)) {
             if (text) text[len]=LeadByte(lead,i);
             len++;
          }
          if (text) text[len]=ptr[i];                      // extract the character
          len++;
       }
       CloseTextPtr(w,l);
    }

    if (text) text[len]=0;

    return len;     // simply return the
}

/******************************************************************************
     TerChangeField:
     This function sets new data for all occurences of a specified field name.
     This function returns TRUE if atleast one occurence was found.
******************************************************************************/
BOOL WINAPI _export TerChangeField(HWND hWnd, LPBYTE name, LPBYTE data,BOOL repaint)
{
    PTERWND w;
    int LocFlag=TER_FIRST,DataCol,FieldNameCol;
    long DataLine,FieldNameLine;
    int DataFont=-1;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;      // get the pointer to window data


    while(TRUE) {      // locate and replace
      if (!TerLocateField(hTerWnd,LocFlag,name,TRUE,FALSE)) break;

      // get the range of the data string
      FieldNameCol=CurCol;
      FieldNameLine=CurLine;
      if (GetFieldScope(w,CurLine,CurCol,FIELD_DATA,&HilightBegRow,&HilightBegCol,&HilightEndRow,&HilightEndCol)) {
         DataLine=HilightBegRow;  // line where the data begins
         DataCol=HilightBegCol;   // column where the data begins

         DataFont=GetCurCfmt(w,DataLine,DataCol);

         // delete the existing field data
         HilightType=HILIGHT_CHAR;
         StretchHilight=FALSE;
         TerOpFlags|=TOFLAG_NO_FIELD_ADJ;

         TerDeleteBlock(hWnd,FALSE);

         HilightType=HILIGHT_OFF;
         ResetUintFlag(TerOpFlags,TOFLAG_NO_FIELD_ADJ);
      }
      else {                      // field data not found
         if (!GetFieldLoc(w,CurLine,CurCol,FALSE,&DataLine,&DataCol)) return FALSE;  // find the position after the end of the name
      }

      // insert the new field
      CurLine=DataLine;
      CurCol=DataCol;

      if (lstrlen(data)>0) {
         HilightType=HILIGHT_OFF;
         //if (!CharFmt(w,GetNewFieldId,(DWORD)FIELD_DATA,0L,FALSE)) return FALSE;
         if (DataFont>=0) InputFontId=DataFont;
         else             InputFontId=(int)GetNewFieldId(w,(WORD)GetNextCfmt(w,FieldNameLine,FieldNameCol),(DWORD)FIELD_DATA,(DWORD)0L,CurLine,CurCol);  // GEt the font with the new field id
         
         TerOpFlags2|=TOFLAG2_NO_SET_LANG;               // do not modify the input font for current language
         InsertBuffer(w,data,NULL,NULL,NULL,NULL,FALSE); // insert the field name
         ResetUintFlag(TerOpFlags2,TOFLAG2_NO_SET_LANG);

         InputFontId=-1;
      }

      LocFlag=TER_NEXT;          // now look for the next occurence
      CurCol--;                  // so that the next field if it allows immediately will also be located
      FixPos(w,&CurLine,&CurCol);
    }

    CurCol++;                    // position after the last field
    FixPos(w,&CurLine,&CurCol);

    ReleaseUndo(w);

    if (repaint) PaintTer(w);

    return (LocFlag!=TER_FIRST);
}

/******************************************************************************
     TerDeleteField:
     This function deletes the current field.
******************************************************************************/
BOOL WINAPI _export TerDeleteField(HWND hWnd,BOOL repaint)
{
    PTERWND w;
    int CurFont,FieldId;
    DWORD SaveTerFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;      // get the pointer to window data


    CurFont=GetCurCfmt(w,CurLine,CurCol);
    FieldId=TerFont[CurFont].FieldId;
    if (FieldId!=FIELD_NAME && FieldId!=FIELD_DATA) return FALSE; // not located on a field

    if (!GetFieldScope(w,CurLine,CurCol,FIELD_NAME,&HilightBegRow,&HilightBegCol,&HilightEndRow,&HilightEndCol)) return FALSE;
    CurFont=GetCurCfmt(w,HilightEndRow,HilightEndCol);
    FieldId=TerFont[CurFont].FieldId;
    if (FieldId==FIELD_DATA) if (!GetFieldLoc(w,HilightEndRow,HilightEndCol,FALSE,&HilightEndRow,&HilightEndCol)) return FALSE;  // find the position after the field data

    HilightType=HILIGHT_CHAR;
    StretchHilight=FALSE;
    SaveTerFlags=TerFlags;
    TerFlags|=TFLAG_UNPROTECTED_DEL;
    TerOpFlags|=TOFLAG_NO_FIELD_ADJ;

    TerDeleteBlock(hWnd,repaint);

    TerFlags=SaveTerFlags;
    HilightType=HILIGHT_OFF;
    ResetUintFlag(TerOpFlags,TOFLAG_NO_FIELD_ADJ);

    //ReleaseUndo(w);

    return TRUE;
}


/******************************************************************************
     TerLocateField:
     This function locates the given field. The loc can be TER_FIRST, TER_LAST,
     TER_NEXT, or TER_PREV.  If 'exact' is TRUE, then the field name is matched
     exactly, otherwise the only the length of 'name' is matched.
     When the field is located, the cursor is placed on the first character of
     the field name and the function returns TRUE.  This function does not
     update the screen.
******************************************************************************/
BOOL WINAPI _export TerLocateField(HWND hWnd, int loc, LPBYTE name, BOOL exact, BOOL repaint)
{
    PTERWND w;
    int   ColNo,CurCfmt,FieldId;
    long  LineNo;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;      // get the pointer to window data

    if (loc!=TER_FIRST && loc!=TER_LAST && loc!=TER_NEXT && loc!=TER_PREV) return FALSE;

    LineNo=CurLine;
    ColNo=CurCol;


    if (loc==TER_FIRST) {
       LineNo=0;
       ColNo=0;
    }
    if (loc==TER_LAST) {
       LineNo=TotalLines-1;
       ColNo=LineLen(LineNo)-1;
       FixPos(w,&LineNo,&ColNo);
    }

    CurCfmt=GetCurCfmt(w,LineNo,ColNo);
    FieldId=TerFont[CurCfmt].FieldId;

    if (FieldId==FIELD_NAME || FieldId==FIELD_DATA) {     // already located on a field name
       if (loc==TER_FIRST || loc==TER_LAST) {
          if (FieldFound(w,LineNo,ColNo,name,exact)) goto END;
       }
    }
    if (loc==TER_FIRST) loc=TER_NEXT;
    if (loc==TER_LAST)  loc=TER_PREV;

    // find in loop
    while (TRUE) {
       // are we at the end of the document?
       if (loc==TER_NEXT && LineNo==(TotalLines-1) && ColNo==LineLen(LineNo)) return FALSE;
       if (loc==TER_PREV && LineNo==0 && ColNo==0) return FALSE;

       CurCfmt=GetCurCfmt(w,LineNo,ColNo);
       FieldId=TerFont[CurCfmt].FieldId;

       // go past the current field
       if (loc==TER_NEXT) {
          if (FieldId==FIELD_NAME) {     // position after name
             GetFieldLoc(w,LineNo,ColNo,FALSE,&LineNo,&ColNo);  // to position at the first character of the data
             CurCfmt=GetCurCfmt(w,LineNo,ColNo);
             FieldId=TerFont[CurCfmt].FieldId;
          }
          if (FieldId==FIELD_DATA) {     // position after data
             GetFieldLoc(w,LineNo,ColNo,FALSE,&LineNo,&ColNo);  // to position at the first character of the data
             CurCfmt=GetCurCfmt(w,LineNo,ColNo);
             FieldId=TerFont[CurCfmt].FieldId;
          }
       }
       else {
          if (FieldId==FIELD_DATA) {     // position before data
             GetFieldLoc(w,LineNo,ColNo,TRUE,&LineNo,&ColNo);  // to position at the first character of the data
             ColNo--;
             FixPos(w,&LineNo,&ColNo);
             CurCfmt=GetCurCfmt(w,LineNo,ColNo);
             FieldId=TerFont[CurCfmt].FieldId;
          }
          if (FieldId==FIELD_NAME) {     // position before name
             GetFieldLoc(w,LineNo,ColNo,TRUE,&LineNo,&ColNo);  // to position at the first character of the data
             ColNo--;
             FixPos(w,&LineNo,&ColNo);
             CurCfmt=GetCurCfmt(w,LineNo,ColNo);
             FieldId=TerFont[CurCfmt].FieldId;
          }
       }


       // locate the next/prev field
       while (FieldId!=FIELD_NAME && FieldId!=FIELD_DATA) {
          if (loc==TER_NEXT) {
             if (LineNo==(TotalLines-1) && ColNo==LineLen(LineNo)) return FALSE;

             GetFieldLoc(w,LineNo,ColNo,FALSE,&LineNo,&ColNo);  // to position at the first character of the data

             CurCfmt=GetCurCfmt(w,LineNo,ColNo);
             FieldId=TerFont[CurCfmt].FieldId;
          }
          else {
             if (LineNo==0 && ColNo==0) return FALSE;

             GetFieldLoc(w,LineNo,ColNo,TRUE,&LineNo,&ColNo);  // to position at the first character of the data
             ColNo--;
             FixPos(w,&LineNo,&ColNo);
             CurCfmt=GetCurCfmt(w,LineNo,ColNo);
             FieldId=TerFont[CurCfmt].FieldId;
          }
       }

       // match the field name
       if (FieldFound(w,LineNo,ColNo,name,exact)) goto END;
    }


    END:

    // position at the first character of the field name
    CurCfmt=GetCurCfmt(w,LineNo,ColNo);
    FieldId=TerFont[CurCfmt].FieldId;
    if (FieldId==FIELD_DATA) {
       GetFieldLoc(w,LineNo,ColNo,TRUE,&LineNo,&ColNo);  // to position at the first character of the data
       ColNo--;     // to position at the field name
       FixPos(w,&LineNo,&ColNo);
       CurCfmt=GetCurCfmt(w,LineNo,ColNo);
       FieldId=TerFont[CurCfmt].FieldId;
    }
    if (FieldId==FIELD_NAME) GetFieldLoc(w,LineNo,ColNo,TRUE,&LineNo,&ColNo);

    CurLine=LineNo;
    CurCol=ColNo;
    CursDirection=CURS_FORWARD;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    FieldFound:
    This function returns TRUE if the field at the given location matches with
    the given field, matches.
******************************************************************************/
BOOL FieldFound(PTERWND w,long LineNo, int ColNo, LPBYTE name, BOOL exact)
{
    LPBYTE pName=NULL;
    long len;
    int  MatchLen,i;
    BOOL result=FALSE;

    len=TerGetField(hTerWnd,LineNo,ColNo,FIELD_NAME,NULL);
    pName=OurAlloc((UINT)(len+1));
    TerGetField(hTerWnd,LineNo,ColNo,FIELD_NAME,pName);

    if (exact) {
       if (lstrcmp(name,pName)==0) result=TRUE;
    }
    else {
       if (name==NULL) return TRUE;   // match any field
       MatchLen=lstrlen(name);
       if ((long)MatchLen>len) MatchLen=(int)len;
       for (i=0;i<MatchLen;i++) if (pName[i]!=name[i]) break;
       if (i==MatchLen) result=TRUE;
    }

    OurFree(pName);

    return result;
}

/******************************************************************************
    FixPos:
    Adjust the position if the col is less than 0 or greater than the last column
    for the line.
******************************************************************************/
FixPos(PTERWND w,LPLONG pLine, LPINT pCol)
{
    long line=(*pLine);
    int  col=(*pCol);

    if (line<0) {
       line=0;
       col=0;
    }
    if (line>=TotalLines) {
       line=TotalLines-1;
       col=LineLen(line)-1;
    }

    if (!TerArg.WordWrap) return TRUE;

    if (col<0) {
       line--;
       while (line>=0 && LineLen(line)==0) line--;
       if (line<0) {
          line=0;
          col=0;
       }
       else col=LineLen(line)-1;
    }
    else if (col>=LineLen(line)) {
       line++;
       while (line<TotalLines && LineLen(line)==0) line++;
       if (line>=TotalLines) {
          line=TotalLines-1;
          col=LineLen(line)-1;
          if (col<0) col=0;
       }
       else col=0;
    }

    (*pLine)=line;
    (*pCol)=col;

    return TRUE;
}

/******************************************************************************
     GetFieldScope:
     Get the begininning and ending loation of the field.
******************************************************************************/
BOOL GetFieldScope(PTERWND w,long LineNo,int ColNo,int type,LPLONG pBegLine,LPINT pBegCol,LPLONG pEndLine,LPINT pEndCol)
{
    int  CurCfmt,CurType;

    // get current font id
    CurCfmt=GetCurCfmt(w,LineNo,ColNo);
    CurType=TerFont[CurCfmt].FieldId;
    if (CurType!=FIELD_NAME && CurType!=FIELD_DATA) return FALSE;

    // position at the request field component
    if (type==FIELD_NAME && CurType==FIELD_DATA) {
       GetFieldLoc(w,LineNo,ColNo,TRUE,&LineNo,&ColNo);  // position at the first character of field data
       ColNo--;                    // position on the last character of field name
       FixPos(w,&LineNo,&ColNo);
    }
    else if (type==FIELD_DATA && CurType==FIELD_NAME) {
       GetFieldLoc(w,LineNo,ColNo,FALSE,&LineNo,&ColNo);  // position past the field name
    }

    CurCfmt=GetCurCfmt(w,LineNo,ColNo);
    if (TerFont[CurCfmt].FieldId!=type) return FALSE;

    // get the beginning and ending of the current component
    GetFieldLoc(w,LineNo,ColNo,TRUE,pBegLine,pBegCol);
    GetFieldLoc(w,LineNo,ColNo,FALSE,pEndLine,pEndCol);

    return TRUE;
}

/******************************************************************************
    GetFieldLoc:
    Get the begininning or ending loation of the current field name or data.
    The ending location goes past the last character of that field component.
******************************************************************************/
BOOL GetFieldLoc(PTERWND w,long LineNo,int ColNo,BOOL begin,LPLONG pLine,LPINT pCol)
{
    long l;
    int  col,FieldId,StartCol,CurCfmt;
    BOOL FirstPos=TRUE,FieldBreak;
    LPBYTE FieldCode;

    CurCfmt=GetCurCfmt(w,LineNo,ColNo);
    FieldId=TerFont[CurCfmt].FieldId;
    FieldCode=TerFont[CurCfmt].FieldCode;

    if (begin) {                    // scan backward
       for (l=LineNo;l>=0;l--) {
         StartCol=(l==LineNo)?ColNo:(LineLen(l)-1);
         for (col=StartCol;col>=0;col--) {
            CurCfmt=GetCurCfmt(w,l,col);
            
            FieldBreak=(FieldId!=TerFont[CurCfmt].FieldId);  // end of field, or new field starts
            if (!FieldBreak && FieldCode!=TerFont[CurCfmt].FieldCode) {  // check if the field code is different
               FieldBreak=!IsSameFieldCode(w,FieldCode,TerFont[CurCfmt].FieldCode);
            } 
            
            if (FieldBreak) {
               col++;                   // go forward to position on the first character of the field component
               if (col>=LineLen(l)) {
                 l++;
                 col=0;
               }
               break;
            }

            if (   FieldId==FIELD_NAME && FieldId==TerFont[CurCfmt].FieldId
                && TerFont[CurCfmt].style&PROTECT && GetCurChar(w,l,col)=='{' /*&& !FirstPos*/) break;

            FirstPos=FALSE;
         }
         if (col>=0) break;
       }
       if (l<0) {
         l=0;
         col=0;
       }
    }
    else {
       for (l=LineNo;l<TotalLines;l++) {
         StartCol=(l==LineNo)?ColNo:0;
         for (col=StartCol;col<LineLen(l);col++) {
            CurCfmt=GetCurCfmt(w,l,col);
            
            FieldBreak=(FieldId!=TerFont[CurCfmt].FieldId);  // end of field, or new field starts
            if (!FieldBreak && FieldCode!=TerFont[CurCfmt].FieldCode) {  // check if the field code is different
               FieldBreak=!IsSameFieldCode(w,FieldCode,TerFont[CurCfmt].FieldCode);
            } 

            if (FieldBreak) break;

            if (   FieldId==FIELD_NAME && FieldId==TerFont[CurCfmt].FieldId
                && TerFont[CurCfmt].style&PROTECT && GetCurChar(w,l,col)=='{' && !FirstPos) break;

            FirstPos=FALSE;
         }
         if (col<LineLen(l)) break;
       }
       if (l==TotalLines) {
         l=TotalLines-1;
         col=LineLen(l);
       }
    }

    if (pLine) (*pLine)=l;
    if (pCol) (*pCol)=col;

    return TRUE;
}

/******************************************************************************
    DoPopupSelection:
    Process the popup menu item selection
******************************************************************************/
BOOL DoPopupSelection(PTERWND w,int CmdId)
{
    if (CmdId>=IDP_FIRST_SPELL && CmdId<=IDP_LAST_SPELL) return SpellCheckCurWordPart2(w,CmdId);
 
    return TRUE;
}

/******************************************************************************
    TerScrToTwipsX:
    Convert the client cooridnates into left-margin relative twips.
******************************************************************************/
BOOL WINAPI _export TerScrToTwipsX(HWND hWnd,int scrX,LPINT twipsX)
{
   PTERWND w;
   int x;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   scrX = scrX-TerWinRect.left;
   scrX += TerWinOrgX;
   x = ScrToTwipsX(scrX);

   if (TerArg.PageMode) x=FrameToMargX(w,x);   // convert frame relative value to left margin relative value

   (*twipsX)=x;

   return TRUE;
}

/******************************************************************************
    TerScrToTwipsY:
    Convert the client cooridnates into page relative twips.  The value returned
    is relative to the top of the page.
******************************************************************************/
BOOL  WINAPI _export TerScrToTwipsY(HWND hWnd,int scrY,LPINT twipsY)
{
   PTERWND w;
   int y;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   scrY = scrY - TerWinRect.top;
   scrY += TerWinOrgY;    ;
   y  = ScrToTwipsY( scrY );

   if (TerArg.PageMode) y=FrameToPageY(w,y);

   (*twipsY)=y;

   return TRUE;
}

/******************************************************************************
    InsertMarkerLine:
    Insert a break line before the specified line.
*******************************************************************************/
InsertMarkerLine(PTERWND w,long LineNo,BYTE BreakChar,int CurFont, int CurParaId, UINT TabwType, int CurCellId)
{
    LPBYTE ptr;
    LPWORD fmt;
    int ParaFID;

    if (!CheckLineLimit(w,TotalLines+1)) {
       PrintError(w,MSG_MAX_LINES_EXCEEDED,"InsertMarkerLine");
       return FALSE;
    }

    ParaFID=fid(LineNo);

    MoveLineArrays(w,LineNo,1,'B');            // create a line before Current Line
    LineAlloc(w,LineNo,0,1);
    
    fid(LineNo)=ParaFID;

    ptr=pText(LineNo);
    fmt=OpenCfmt(w,LineNo);
    ptr[0]=BreakChar;
    fmt[0]=CurFont;

    CloseCfmt(w,LineNo);

    if (CurParaId>=0) pfmt(LineNo)=CurParaId;
    else {                                   // default formatting
       if ((PfmtId[pfmt(LineNo)].flags&PAGE_HDR_FTR)!=(PfmtId[pfmt(LineNo+1)].flags&PAGE_HDR_FTR)) pfmt(LineNo)=pfmt(LineNo+1);
    }

    if (CurCellId>=0) cid(LineNo)=CurCellId;
    if (BreakChar==ParaChar || BreakChar==CellChar) LineFlags(LineNo)|=LFLAG_PARA;

    // set any tabw flag
    if (TabwType) {
       AllocTabw(w,LineNo);
       tabw(LineNo)->type=TabwType;
    }

    return TRUE;
}

/******************************************************************************
    TerTab2Spaces:
    Convert a tab location to equivalent number of spaces
******************************************************************************/
int WINAPI _export TerTab2Spaces(HWND hWnd,long line, int col)
{
    PTERWND w;
    LPBYTE ptr;
    int SpaceCount,adj,TbWidth,SpaceWidth,CurFont;
    LPWORD pWidth;
    LPWORD fmt;
    static long PrevLine=-1;
    static int PrevCol=-1;
    static int PrevAdj=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data
 
    if (line<0 || line>=TotalLines) return 0;
    if (col<0 || col>=LineLen(line)) return 0;

    ptr=pText(line);
    if (ptr[col]!=TAB) return 0;
    fmt=OpenCfmt(w,line);

    // get previous widths
    adj=0;
    if (col>0 && PrevLine==line && PrevCol==(col-1) && ptr[col-1]==TAB && fmt[col-1]==fmt[col]) adj=PrevAdj;

    // get the tab width
    pWidth=GetLineCharWidth(w,line);
    TbWidth=pWidth[col]+adj;
    MemFree(pWidth);

    // get the space width
    fmt=OpenCfmt(w,line);
    CurFont=fmt[col];
    CloseCfmt(w,line);

    SpaceWidth=TerFont[CurFont].CharWidth[' '];
    if (SpaceWidth==0) return 0;                   // may be a hidden font

    SpaceCount=MulDiv(TbWidth,1,SpaceWidth);       // rounded

    // remember the adjustment for the next tab
    PrevLine=line;
    PrevCol=col;
    PrevAdj=TbWidth-SpaceCount*TerFont[CurFont].CharWidth[' '];

    return SpaceCount;
}


/******************************************************************************
    TerGetParam:
    Get the sizes of various arrays.
******************************************************************************/
long WINAPI _export TerGetParam(HWND hWnd,int type)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
 
    if (type==TP_TOTAL_LINES)  return TotalLines;
    if (type==TP_TOTAL_PFMTS)  return TotalPfmts;
    if (type==TP_TOTAL_FONTS)  return TotalFonts;
    if (type==TP_TOTAL_STYLES) return TotalSID;
    if (type==TP_TOTAL_SECTS)  return TotalSects;
    if (type==TP_TOTAL_PAGES)  return TotalPages;
    if (type==TP_TOTAL_PARA_FRAMES) return TotalParaFrames;
    if (type==TP_TOTAL_CHAR_TAGS)   return TotalCharTags;
    if (type==TP_TOTAL_BLTS)   return TotalBlts;
    if (type==TP_TOTAL_LISTS)  return TotalLists;
    if (type==TP_TOTAL_LIST_OR) return TotalListOr;
    if (type==TP_TOTAL_TABS)   return TotalTabs;
    if (type==TP_TOTAL_TABLE_ROWS) return TotalTableRows;
    if (type==TP_TOTAL_CELLS)  return TotalCells;
    if (type==TP_TOTAL_IMAGE_MAPS) return TotalImageMaps;
    if (type==TP_CUR_LINE) return CurLine;
    if (type==TP_CUR_COL) return CurCol;
    if (type==TP_CUR_SECT) return GetSection(w,CurLine);
    if (type==TP_SELECTION_TYPE) return HilightType;
    if (type==TP_SELECTION_START_LINE) return HilightBegRow;
    if (type==TP_SELECTION_START_COL) return HilightBegCol;
    if (type==TP_SELECTION_END_LINE) return HilightEndRow;
    if (type==TP_SELECTION_END_COL) return HilightEndCol;
    if (type==TP_WATERMARK_WASH) return WmWashed;
    if (type==TP_PAGE_BK_COLOR) return (long)(DWORD)PageBkColor;

    if (type==TP_WATERMARK_PICT) {
       if (WmParaFID>0) return ParaFrame[WmParaFID].pict;
       else             return -1;
    }

    return -1;
}
 
/******************************************************************************
    TerMenuEnable2:
    This function return TRUE if the menu item is enabled otherwise
    it returns FALSE
******************************************************************************/
BOOL WINAPI _export TerMenuEnable2(HWND hWnd,int MenuId)
{
    UINT result=TerMenuEnable(hWnd,MenuId);

    return (result==MF_ENABLED)?TRUE:FALSE;
}
 
/******************************************************************************
    TerMenuEnable:
    This function return MF_ENABLED if the menu item is enabled otherwise
    it returns MF_GRAYED;
******************************************************************************/
UINT WINAPI _export TerMenuEnable(HWND hWnd,int MenuId)
{
    PTERWND w;
    BOOL edit=TRUE,ReadOnly,WordWrap,protect;
    int sect,FirstList,FirstListOr,ParaId,ParaFID,CurFont,PictParaFID=0,indent;
    BOOL EditPara=FALSE,EditChar,ProtectFmt,TableEdit=FALSE,CanInsertBreak,CanInsertObj,
         CanInsertTextObj;
    UINT flags;
    int  BltId,ListOrId;


    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // disable all options during print preview
    if (InPrintPreview) return ((MenuId==ID_PRINT_PREVIEW || MenuId==ID_PGUP || MenuId==ID_PGDN)? MF_ENABLED : MF_GRAYED);

    ReadOnly=TerArg.ReadOnly;
    EditChar=!ReadOnly;           // edit of character attribytes
    if (!ReadOnly && (CurSID<0 || StyleId[CurSID].type==SSTYPE_PARA) && TerArg.WordWrap)
        EditPara=TRUE;  // allow editing for para attributes
    if (ReadOnly || CurSID>=0) edit=FALSE;  // other menu selections

    WordWrap=TerArg.WordWrap;
    sect=GetSection(w,CurLine);
    ParaId=pfmt(CurLine);
    ParaFID=fid(CurLine);
    CurFont=GetCurCfmt(w,CurLine,CurCol);
    if (TerFont[CurFont].style&PICT && TerFont[CurFont].FrameType!=PFRAME_NONE) PictParaFID=TerFont[CurFont].ParaFID;
    if (ParaFID==0) ParaFID=PictParaFID;

    protect=!CanInsert(w,CurLine,CurCol);
    ProtectFmt=(protect && TerFlags2&TFLAG2_PROTECT_FORMAT);  // protect formatting
    if (ProtectFmt) EditChar=FALSE;  // protect formatting

    CanInsertBreak=CanInsertBreakChar(w,CurLine,CurCol);
    CanInsertObj=CanInsertObject(w,CurLine,CurCol);
    CanInsertTextObj=CanInsertTextObject(w,CurLine,CurCol);

    for (FirstList=1;FirstList<TotalLists;FirstList++) if (list[FirstList].InUse) break;
    for (FirstListOr=1;FirstListOr<TotalListOr;FirstListOr++) if (ListOr[FirstListOr].InUse) break;

    //****** File Menu *****
    if (MenuId==ID_NEW)          return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_OPEN)         return (MF_ENABLED);
    if (MenuId==ID_SAVE)         return ((edit || ProtectForm) ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SAVEAS)       return (MF_ENABLED);
    if (MenuId==ID_PRINT)        return (PrinterAvailable ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PRINT_PREVIEW) return MF_ENABLED; // (PrinterAvailable ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PRINT_OPTIONS)return (PrinterAvailable ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PAGE_OPTIONS)return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_QUIT)         return (TerArg.UserCanClose ? MF_ENABLED : MF_GRAYED);

    //**** Line Edit Menu ***
    if (MenuId==ID_INS_AFT)      return (edit && !WordWrap && !InFootnote? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INS_BEF)      return (edit && !WordWrap && !InFootnote? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_JOIN_LINE)    return (edit && !WordWrap && !InFootnote? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SPLIT_LINE)   return (edit && !WordWrap && !InFootnote? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_DEL_LINE)     return (edit && !InFootnote ? MF_ENABLED : MF_GRAYED);

    //**** Block Edit Menu and other edit commands ***
    if (MenuId==ID_SELECT_ALL)   return (MF_ENABLED);
    if (MenuId==ID_UNDO)         return (edit && UndoCount>0 ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_REDO)         return (edit && UndoTblSize>UndoCount ? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_INLINE_IME)   return (edit && Win32 ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_DOC_RTL)      return (edit && Win32 ? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_CUT)          return (edit && (HilightType==HILIGHT_CHAR || HilightType==HILIGHT_LINE) ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_COPY)         return (HilightType==HILIGHT_CHAR || HilightType==HILIGHT_LINE ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PASTE)        return (edit && !InFootnote && !protect && (IsClipboardFormatAvailable(CF_TEXT) || (TerArg.WordWrap &&
                                        (  IsClipboardFormatAvailable(RtfClipFormat) || IsClipboardFormatAvailable(CF_BITMAP)
                                        || IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_METAFILEPICT) )))? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PASTE_TEXT)   return ((edit && !InFootnote && !protect && IsClipboardFormatAvailable(CF_TEXT))? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_PASTE_SPEC) {
       if (TerFlags&TFLAG_NO_OLE) return MF_GRAYED;
       if (edit && TerArg.WordWrap && !WaitForOle && !protect && !InFootnote) {
          if ( IsClipboardFormatAvailable(CF_TEXT)
            || IsClipboardFormatAvailable(CF_BITMAP)
            || IsClipboardFormatAvailable(CF_DIB)
            || IsClipboardFormatAvailable(CF_METAFILEPICT)
            || IsClipboardFormatAvailable(RtfClipFormat)
            || IsClipboardFormatAvailable(OwnerLinkClipFormat)
            || IsClipboardFormatAvailable(ObjectLinkClipFormat) )
                return MF_ENABLED;
          else  return MF_GRAYED;
       }
       else return MF_GRAYED;
    }

    if (MenuId==ID_EMBED_PICT) return (edit && !InFootnote ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_LINK_PICT) return (edit && !InFootnote ? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_EDIT_PICT || MenuId==ID_EDIT_OLE) {
       int CurCfmt=GetCurCfmt(w,CurLine,CurCol);
       if (MenuId==ID_EDIT_PICT) return (edit && !InFootnote && TerFont[CurCfmt].style&PICT ? MF_ENABLED : MF_GRAYED);
       else                      return (edit && !InFootnote && TerFont[CurCfmt].style&PICT && TerFont[CurCfmt].ObjectType!=OBJ_NONE && !(TerFlags&TFLAG_NO_EDIT_OLE)? MF_ENABLED : MF_GRAYED);
    }

    if (MenuId==ID_EDIT_INPUT_FIELD) return ((edit && (IsFormField(w,CurInputField,0) || TerFont[CurFont].FieldId==FIELD_TEXTBOX))? MF_ENABLED : MF_GRAYED);
    
    if (MenuId==ID_SECT_OPTIONS) return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_OVERRIDE_BIN) return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_EDIT_STYLE)   return (!ReadOnly ? MF_ENABLED : MF_GRAYED);

    
    if (MenuId==ID_CREATE_LIST)   return (!ReadOnly ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_EDIT_LIST)   return (!ReadOnly && FirstList<TotalLists ? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_CREATE_LIST_OR)   return (!ReadOnly && FirstList<TotalLists? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_EDIT_LIST_OR)   return (!ReadOnly && FirstListOr<TotalListOr ? MF_ENABLED : MF_GRAYED);
    
    if (MenuId==ID_EDIT_LIST_LEVEL) return (!ReadOnly && FirstList<TotalLists ? MF_ENABLED : MF_GRAYED);

    
    if (MenuId==ID_BLOCK_COPY) return (edit && HilightType==HILIGHT_LINE ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_BLOCK_MOVE) return (edit && HilightType==HILIGHT_LINE ? MF_ENABLED : MF_GRAYED);

    //**** Character Formatting Features ***
    if (MenuId==ID_CHAR_NORMAL) return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_BOLD_ON)     return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_ULINE_ON)    return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_ULINED_ON)   return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_ITALIC_ON)   return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_STRIKE_ON)   return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SUPSCR_ON)   return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SUBSCR_ON)   return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PROTECT_ON)  return (!ReadOnly && !ProtectionLock ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_COLOR)       return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_ULINE_COLOR) return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_BK_COLOR)    return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_FONTS)       return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_HIDDEN_ON)   return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_CHAR_SPACE)  return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_BOX_ON)      return (EditChar && CurSID<0 ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_HLINK_ON)    return (EditChar ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_CAPS_ON)     return ((EditChar && Win32)? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SCAPS_ON)    return ((EditChar && Win32)? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_CHAR_STYLE)  return (!ReadOnly && !ProtectFmt && CurSID<0 ? MF_ENABLED : MF_GRAYED);

    //**** Paragraph Formatting Features ***
    if (EditingParaStyle) {
       flags=StyleId[CurSID].ParaFlags;
       BltId=StyleId[CurSID].BltId;
       indent=StyleId[CurSID].LeftIndentTwips;
       if (StyleId[CurSID].FirstIndentTwips<0) indent+=StyleId[CurSID].FirstIndentTwips;
    }
    else {
       flags=PfmtId[pfmt(CurLine)].flags;
       BltId=PfmtId[pfmt(CurLine)].BltId;
       indent=PfmtId[pfmt(CurLine)].LeftIndentTwips;
       if (PfmtId[pfmt(CurLine)].FirstIndentTwips<0) indent+=PfmtId[pfmt(CurLine)].FirstIndentTwips;
    }

    if (True(flags&BULLET) && TerBlt[BltId].ls>0) {
       ListOrId=TerBlt[BltId].ls;
    } 
    
    if (MenuId==ID_PARA_NORMAL)    return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_DOUBLE_SPACE)   return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PARA_KEEP)      return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PARA_KEEP_NEXT) return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_WIDOW_ORPHAN)   return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PAGE_BREAK_BEFORE) return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_CENTER)         return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_LEFT_JUSTIFY)   return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_RIGHT_JUSTIFY)  return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_JUSTIFY)        return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_LEFT_INDENT)    return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_LEFT_INDENT_DEC) return ((EditPara && indent>0) ?MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_RIGHT_INDENT)   return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_HANGING_INDENT) return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PARA_BORDER)    return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PARA_SPACING)   return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_BULLET)         return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PARA_NBR)       return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_LIST_LEVEL_INC) return (EditPara && ListOrId>0? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_LIST_LEVEL_DEC) return (EditPara && ListOrId>0? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PARA_LIST)      return (EditPara && FirstListOr<TotalListOr ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PARA_BK_COLOR)  return (EditPara ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PARA_RTL)       return (EditPara ? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_PARA_STYLE)  return (!ReadOnly && CurSID<0 ? MF_ENABLED : MF_GRAYED);

    //**** Tab functions ***
    if (MenuId==ID_TAB_SET)        return (EditPara ? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_TAB_CLEAR) {
       int TabId;
       if (TerArg.WordWrap) TabId=PfmtId[pfmt(CurLine)].TabId;
       return (EditPara && TerTab[TabId].count>0 ? MF_ENABLED : MF_GRAYED);
    }
    if (MenuId==ID_TAB_CLEAR_ALL) {
       int TabId;
       if (TerArg.WordWrap) TabId=PfmtId[pfmt(CurLine)].TabId;
       return (EditPara && TerTab[TabId].count>0 ? MF_ENABLED : MF_GRAYED);
    }

    // table parameters
    if (HilightType==HILIGHT_CHAR && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0))
        TableEdit=InSameTable(w,cid(HilightBegRow),cid(HilightEndRow));
    else TableEdit=cid(CurLine);
    if (!edit || !TerArg.PrintView) TableEdit=FALSE;

    if (MenuId==ID_TABLE_INSERT)     return (edit && CanInsertTable(w,CurLine,CurCol) && TerArg.PrintView ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_INSERT_ROW) return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_MERGE_CELLS)return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_DEL_CELLS)  return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_SPLIT_CELL) return (TableEdit && HilightType==HILIGHT_OFF ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_ROW_POS)    return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_HDR_ROW)    return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_ROW_KEEP)   return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_ROW_HEIGHT) return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_ROW_RTL) return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_INSERT_COL) return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_CELL_BORDER)return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_CELL_BORDER_COLOR)return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_CELL_WIDTH)return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_CELL_SHADE) return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_CELL_COLOR) return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_CELL_VALIGN) return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_CELL_VTEXT) return (TableEdit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_SEL_COL)    return (edit && TerArg.PrintView && HilightType==HILIGHT_OFF && cid(CurLine)>0 ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TABLE_SHOW_GRID)  return (TerArg.PageMode ? MF_ENABLED : MF_GRAYED);


    //**** View functions ***
    if (MenuId==ID_PAGE_MODE)      return (TerArg.PrintView ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_FITTED_VIEW)    return (TerArg.PrintView ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_VIEW_HDR_FTR)   return (TerArg.PrintView? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SHOW_HIDDEN)    return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SHOW_FIELD_NAMES)  return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_EDIT_FNOTE)     return (edit && !TerArg.FittedView? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_EDIT_ENOTE)     return (edit && !TerArg.FittedView? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SHOW_PARA_MARK) return (edit && TerArg.WordWrap ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SHOW_PAGE_BORDER) return (TerArg.WordWrap && TerArg.PageMode && !TerArg.FittedView ? MF_ENABLED : MF_GRAYED);

    //**** Insert functions ***
    if (MenuId==ID_INSERT_OBJECT) return (edit && !InFootnote && !(TerFlags&TFLAG_NO_OLE)? MF_ENABLED : MF_GRAYED);

    //**** frame edit functions ***
    if (MenuId==ID_FRAME_YBASE)   return (edit && WordWrap && TerArg.PageMode && ParaFID>0 ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_EDIT_DOB)      return (edit && WordWrap && TerArg.PageMode
                                          && ParaFID>0 && ParaFrame[ParaFID].flags&(PARA_FRAME_OBJECT) ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_FRAME_ROTATE_TEXT)   return (edit && WordWrap && TerArg.PageMode && ParaFID>0 && 
                                           !(ParaFrame[ParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT)) &&
                                             ParaFrame[ParaFID].pict==0 ? MF_ENABLED : MF_GRAYED);

    //**** Miscellaenous functions ***
    if (MenuId==ID_REPAGINATE)      return (edit && WordWrap && TerArg.PrintView ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_PAGE_BREAK)      return (edit && CanInsertPageBreak(w,CurLine,CurCol) && WordWrap ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_PARA_FRAME) return (edit && !InFootnote && TerArg.PrintView ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_DRAW_OBJECT) return (edit && !InFootnote && TerArg.PrintView ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_INPUT_FIELD) return (edit && CanInsertTextObj ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_CHECKBOX) return (edit && CanInsertTextObj ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_HLINK)    return (edit && CanInsertTextObj ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_PAGE_NUMBER) return (edit && !InFootnote ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_PAGE_COUNT) return (edit && !InFootnote ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_DATE_TIME) return (edit && !InFootnote ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_TOC)      return (edit && !InFootnote && DocHasHeadings? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_NBSPACE)  return (edit && !protect ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_NBDASH)   return (edit && !protect ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_HYPH)     return (edit && !protect ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_FNOTE)    return (edit && !InFootnote && !(PfmtId[ParaId].flags&PAGE_HDR_FTR)? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_ENOTE)    return (edit && !InFootnote && !(PfmtId[ParaId].flags&PAGE_HDR_FTR)? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_INSERT_BOOKMARK) return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SECT_BREAK)      return (edit && CanInsertBreak && TerArg.PrintView && !EditPageHdrFtr ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_COL_BREAK)       return (edit && CanInsertBreak && TerArg.PrintView && TerSect[sect].columns>1? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_REPLACE)         return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_DEL_PREV_WORD)   return (edit ? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_PROTECTION_LOCK) return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_WATERMARK)       return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_TRACK_CHANGES)   return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_ACCEPT_CHANGE)   return ((edit && !TrackChanges && IsTrackChangeFont(w,CurFont)) ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_ACCEPT_ALL_CHANGES) return ((edit && !TrackChanges) ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_SNAP_TO_GRID)    return (edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_EDIT_HDR_FTR)    return ((edit && TerArg.PageMode)? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_CREATE_FIRST_HDR) return ((edit && EditPageHdrFtr && TerSect1[sect].fhdr.FirstLine==-1)? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_CREATE_FIRST_FTR) return ((edit && EditPageHdrFtr && TerSect1[sect].fftr.FirstLine==-1)? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_DELETE_FIRST_HDR) return ((edit && EditPageHdrFtr && TerSect1[sect].fhdr.FirstLine>=0)? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_DELETE_FIRST_FTR) return ((edit && EditPageHdrFtr && TerSect1[sect].fftr.FirstLine>=0)? MF_ENABLED : MF_GRAYED);

    if (MenuId==ID_SPELL)           return (hSpell!=NULL && edit ? MF_ENABLED : MF_GRAYED);
    if (MenuId==ID_AUTO_SPELL)      return (hSpell!=NULL && edit ? MF_ENABLED : MF_GRAYED);


    return MF_ENABLED;              // other options enabled by default
}

/******************************************************************************
    TerMenuSelect2:
    This function return TRUE if the menu item is to checked otherwise
    it returns FALSE;
******************************************************************************/
BOOL WINAPI _export TerMenuSelect2(HWND hWnd,int MenuId)
{
    UINT result=TerMenuSelect(hWnd,MenuId);

    return (result==MF_CHECKED)?TRUE:FALSE;
}     

/******************************************************************************
    TerMenuSelect:
    This function return MF_CHECKED if the menu item is to checked otherwise
    it returns MF_UNCHECKED;
******************************************************************************/
UINT WINAPI _export TerMenuSelect(HWND hWnd,int MenuId)
{
    PTERWND w;
    int font=DEFAULT_CFMT;
    int NewPfmt,LeftIndent,RightIndent,FirstIndent,SpaceBefore,
        SpaceAfter,SpaceBetween,LineSpacing,BltId,expand,row,cl,ParaFlow,ListOrId;
    struct StrListLevel far *pLevel=null;
    UINT flags,style,pflags;
    COLORREF ParaBkColor;

    w=GetWindowPointer(hWnd);       // get the pointer to window data

    if (MenuId==ID_PRINT_PREVIEW)   return (InPrintPreview ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_PAGE_MODE)       return (TerArg.PageMode ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_FITTED_VIEW)     return (TerArg.PageMode && TerArg.FittedView ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_VIEW_HDR_FTR)    return (ViewPageHdrFtr ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_SHOW_HIDDEN)     return (ShowHiddenText ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_SHOW_FIELD_NAMES)  return (ShowFieldNames ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_EDIT_FNOTE)      return (EditFootnoteText ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_EDIT_ENOTE)      return (EditEndnoteText ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_TOOL_BAR)        return (TerArg.ToolBar ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_RULER)           return (TerArg.ruler ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_STATUS_RIBBON)   return (TerArg.ShowStatus ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_SHOW_PARA_MARK)  return (ShowParaMark ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_SHOW_PAGE_BORDER)  return (ShowPageBorder ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PROTECTION_LOCK) return (ProtectionLock ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_TRACK_CHANGES)   return (TrackChanges ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_SNAP_TO_GRID)    return (SnapToGrid ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_BORDER_MARGIN)   return (TerArg.BorderMargins ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PROTECT_FORM)    return (ProtectForm ? MF_CHECKED : MF_UNCHECKED);

    // get the effective font for next character input
    if (  MenuId==ID_BOLD_ON ||   MenuId==ID_ULINE_ON ||  MenuId==ID_ULINED_ON
       || MenuId==ID_ITALIC_ON || MenuId==ID_STRIKE_ON || MenuId==ID_SUPSCR_ON
       || MenuId==ID_SUBSCR_ON || MenuId==ID_PROTECT_ON || MenuId==ID_HIDDEN_ON
       || MenuId==ID_BOX_ON || MenuId==ID_CHAR_STYLE || MenuId==ID_HLINK_ON
       || MenuId==ID_INSERT_PAGE_NUMBER || MenuId==ID_INSERT_PAGE_COUNT
       || MenuId==ID_INSERT_DATE_TIME   || MenuId==ID_INSERT_TOC
       || MenuId==ID_CHAR_SPACE || MenuId==ID_CAPS_ON || MenuId==ID_SCAPS_ON)
       font=GetEffectiveCfmt(w);

    if (CurSID>=0) style=StyleId[CurSID].style;
    else           style=TerFont[font].style;
    if (CurSID>=0) expand=StyleId[CurSID].expand;
    else           expand=TerFont[font].expand;

    if (MenuId==ID_BOLD_ON)     return (style&BOLD ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_ULINE_ON)    return (style&ULINE ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_ULINED_ON)   return (style&ULINED ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_ITALIC_ON)   return (style&ITALIC ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_STRIKE_ON)   return (style&STRIKE ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_SUPSCR_ON)   return (style&SUPSCR ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_SUBSCR_ON)   return (style&SUBSCR ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PROTECT_ON)  return (style&PROTECT ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_HIDDEN_ON)   return (style&HIDDEN ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_CHAR_SPACE)  return (expand ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_BOX_ON)      return (style&CHAR_BOX ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_HLINK_ON)    return (style&HLINK ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_CAPS_ON)     return (style&CAPS ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_SCAPS_ON)    return (style&SCAPS ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_CHAR_STYLE)  return (TerFont[font].CharStyId>1 ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_INSERT_PAGE_NUMBER)  return (TerFont[font].FieldId==FIELD_PAGE_NUMBER ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_INSERT_PAGE_COUNT)   return (TerFont[font].FieldId==FIELD_PAGE_COUNT ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_INSERT_DATE_TIME)    return ((TerFont[font].FieldId==FIELD_DATE || TerFont[font].FieldId==FIELD_PRINTDATE)? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_INSERT_TOC)          return (TerFont[font].FieldId==FIELD_TOC ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_INSERT_NBSPACE)  {
       LPBYTE ptr=pText(CurLine);
       return ((CurCol<LineLen(CurLine) && ptr[CurCol]==NBSPACE_CHAR) ? MF_CHECKED : MF_UNCHECKED);
    }
    if (MenuId==ID_INSERT_NBDASH)  {
       LPBYTE ptr=pText(CurLine);
       return ((CurCol<LineLen(CurLine) && ptr[CurCol]==NBDASH_CHAR) ? MF_CHECKED : MF_UNCHECKED);
    }
    if (MenuId==ID_INSERT_HYPH)  {
       LPBYTE ptr=pText(CurLine);
       return ((CurCol<LineLen(CurLine) && ptr[CurCol]==HYPH_CHAR) ? MF_CHECKED : MF_UNCHECKED);
    }

    // paragraph styles
    NewPfmt=pfmt(CurLine);
    if (CurLine>0 && CurCol==0 && HilightType!=HILIGHT_OFF) {
       if (HilightEndRow==(CurLine-1) && HilightEndCol==LineLen(HilightEndRow)) NewPfmt=pfmt(CurLine-1);
       if (HilightEndRow==CurLine && HilightEndCol==0 && HilightBegRow<HilightEndRow) NewPfmt=pfmt(CurLine-1);
    }

    if (EditingParaStyle) {
       flags=StyleId[CurSID].ParaFlags;
       LeftIndent=StyleId[CurSID].LeftIndentTwips;
       RightIndent=StyleId[CurSID].RightIndentTwips;
       FirstIndent=StyleId[CurSID].FirstIndentTwips;
       SpaceBefore=StyleId[CurSID].SpaceBefore;
       SpaceAfter=StyleId[CurSID].SpaceAfter;
       SpaceBetween=StyleId[CurSID].SpaceBetween;
       LineSpacing=StyleId[CurSID].LineSpacing;
       BltId=StyleId[CurSID].BltId;
       pflags=StyleId[CurSID].pflags;
       ParaBkColor=StyleId[CurSID].ParaBkColor;
    }
    else {
       flags=PfmtId[NewPfmt].flags;
       LeftIndent=PfmtId[NewPfmt].LeftIndentTwips;
       RightIndent=PfmtId[NewPfmt].RightIndentTwips;
       if (LineFlags2(CurLine)&LFLAG2_RTL) SwapInts(&LeftIndent,&RightIndent);
       FirstIndent=PfmtId[NewPfmt].FirstIndentTwips;
       SpaceBefore=PfmtId[NewPfmt].SpaceBefore;
       SpaceAfter=PfmtId[NewPfmt].SpaceAfter;
       SpaceBetween=PfmtId[NewPfmt].SpaceBetween;
       LineSpacing=PfmtId[NewPfmt].LineSpacing;
       BltId=PfmtId[NewPfmt].BltId;
       pflags=PfmtId[NewPfmt].pflags;
       ParaBkColor=PfmtId[NewPfmt].BkColor;
       ParaFlow=PfmtId[NewPfmt].flow;
    }

    if (True(flags&BULLET) && TerBlt[BltId].ls>0) {
       pLevel=GetListLevelPtr(w,TerBlt[BltId].ls,TerBlt[BltId].lvl);
       ListOrId=TerBlt[BltId].ls;
    } 

    if (MenuId==ID_DOUBLE_SPACE)   return (flags&DOUBLE_SPACE ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PARA_KEEP)      return (flags&PARA_KEEP ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PARA_KEEP_NEXT) return (flags&PARA_KEEP_NEXT ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_WIDOW_ORPHAN)   return (pflags&PFLAG_WIDOW ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PAGE_BREAK_BEFORE) return (pflags&PFLAG_PAGE_BREAK ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_CENTER)         return (flags&CENTER ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_LEFT_JUSTIFY)   return (!(flags&(RIGHT_JUSTIFY|CENTER|JUSTIFY)) ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_RIGHT_JUSTIFY)  return (flags&RIGHT_JUSTIFY ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_JUSTIFY)        return (flags&JUSTIFY ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_LEFT_INDENT)    return (LeftIndent!=0 ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_LEFT_INDENT_DEC)  return (MF_UNCHECKED);
    if (MenuId==ID_RIGHT_INDENT)   return (RightIndent!=0 ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_HANGING_INDENT) return (FirstIndent!=0 ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PARA_BORDER)    return (flags&PARA_BOX ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PARA_SPACING)   return (SpaceBefore || SpaceAfter || SpaceBetween || LineSpacing? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_BULLET)         return (True(flags&BULLET) && ((ListOrId==0 && TerBlt[BltId].IsBullet) || (ListOrId!=0 && pLevel && pLevel->NumType==LIST_BLT))? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PARA_NBR)       return (True(flags&BULLET) && ((ListOrId==0 && !TerBlt[BltId].IsBullet) || (ListOrId!=0 && pLevel && pLevel->NumType!=LIST_BLT))? MF_CHECKED : MF_UNCHECKED);
    
    if (MenuId==ID_PARA_LIST)      return (flags&BULLET && TerBlt[BltId].ls!=0? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PARA_BK_COLOR)  return (ParaBkColor!=CLR_WHITE ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_PARA_RTL)       return (ParaFlow==FLOW_RTL ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_PARA_STYLE)  return (PfmtId[NewPfmt].StyId>0 ? MF_CHECKED : MF_UNCHECKED);

    // Table options
    cl=cid(CurLine);
    row=(cl>0 && cl<=TotalCells)?cell[cl].row:0;

    if (MenuId==ID_TABLE_SHOW_GRID)  return (ShowTableGridLines && TerArg.PageMode ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_TABLE_HDR_ROW)    return ((row>0 && TableRow[row].flags&ROWFLAG_HDR) ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_TABLE_ROW_KEEP)   return ((row>0 && TableRow[row].flags&ROWFLAG_KEEP) ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_TABLE_ROW_RTL)    return ((row>0 && TableRow[row].flow==FLOW_RTL) ? MF_CHECKED : MF_UNCHECKED);

    // Hyperlink Cursor
    if (MenuId==ID_SHOW_HYPERLINK_CURSOR)  return (ShowHyperlinkCursor  ? MF_CHECKED : MF_UNCHECKED);

    // other options
    if (MenuId==ID_EDIT_STYLE)      return (CurSID>=0 ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_EDIT_HDR_FTR)    return (EditPageHdrFtr ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_UNDO)            return (UndoCount>0 ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_REDO)            return (UndoTblSize>UndoCount ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_ZOOM)            return (ZoomPercent!=100 ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_BKND_PICT)       return (BkPictId>0 ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_WATERMARK)       return (WmParaFID>0 ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_INSERT)          return (InsertMode ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_INLINE_IME)      return (InlineIme ? MF_CHECKED : MF_UNCHECKED);
    if (MenuId==ID_DOC_RTL)         return (DocTextFlow==FLOW_RTL ? MF_CHECKED : MF_UNCHECKED);

    if (MenuId==ID_AUTO_SPELL)      return ((hSpell && TerFlags4&TFLAG4_AUTO_SPELL) ? MF_CHECKED : MF_UNCHECKED);

    return MF_UNCHECKED;
}

/******************************************************************************
    DispMetaRecName:
    Display the metafile record name
******************************************************************************/
DispMetaRecName(UINT msg)
{
    BYTE string[30];
    LPBYTE pMsg=string;

    wsprintf(string,"Other: %x",msg);

    if      (msg==0x0817)  pMsg="Arc";
    else if (msg==0x0830)  pMsg="Chord";
    else if (msg==0x0418)  pMsg="Ellipse";
    else if (msg==0x0415)  pMsg="ExcludeClipRect";
    else if (msg==0x0419)  pMsg="FloodFill";
    else if (msg==0x0416)  pMsg="IntersectClipRect";
    else if (msg==0x0213)  pMsg="LineTo";
    else if (msg==0x0214)  pMsg="MoveTo";
    else if (msg==0x0220)  pMsg="OffsetClipRgn";
    else if (msg==0x0211)  pMsg="OffsetViewportOrg";
    else if (msg==0x020F)  pMsg="OffsetWindowOrg";
    else if (msg==0x061D)  pMsg="PatBlt";
    else if (msg==0x081A)  pMsg="Pie";
    else if (msg==0x0035)  pMsg="RealizePalette";
    else if (msg==0x041B)  pMsg="Rectangle";
    else if (msg==0x0139)  pMsg="ResizePalette";
    else if (msg==0x0127)  pMsg="RestoreDC";
    else if (msg==0x061C)  pMsg="RoundRect";
    else if (msg==0x001E)  pMsg="SaveDC";
    else if (msg==0x0412)  pMsg="ScaleViewportExt";
    else if (msg==0x0400)  pMsg="ScaleWindowExt";
    else if (msg==0x0201)  pMsg="SetBkColor";
    else if (msg==0x0102)  pMsg="SetBkMode";
    else if (msg==0x0103)  pMsg="SetMapMode";
    else if (msg==0x0231)  pMsg="SetMapperFlag";
    else if (msg==0x041F)  pMsg="SetPixel";
    else if (msg==0x0106)  pMsg="SetPolyFillMode";
    else if (msg==0x0104)  pMsg="SetROP2";
    else if (msg==0x0107)  pMsg="SetStretchBltMode";
    else if (msg==0x012E)  pMsg="SetTextAlign";
    else if (msg==0x0108)  pMsg="SetTextCharExtra";
    else if (msg==0x0209)  pMsg="SetTextColor";
    else if (msg==0x020A)  pMsg="SetTextJustifications";
    else if (msg==0x020E)  pMsg="SetViewportExt";
    else if (msg==0x020D)  pMsg="SetViewportOrg";
    else if (msg==0x020C)  pMsg="SetWindowExt";
    else if (msg==0x020B)  pMsg="SetWindowOrg";
    else if (msg==0x0436)  pMsg="AnimatePaletteRecord";
    else if (msg==0x0922)  pMsg="BitBlt(old)";
    else if (msg==0x0940)  pMsg="BitBlt";
    else if (msg==0x02FC)  pMsg="CreateBrushIndirect";
    else if (msg==0x02FB)  pMsg="CreateFontIndirect";
    else if (msg==0x00F7)  pMsg="CreatePaletteRecord";
    else if (msg==0x01F9)  pMsg="CreatePatternBrush";
    else if (msg==0x0142)  pMsg="CreatePatternBrush";
    else if (msg==0x02FA)  pMsg="CreatePenIndirect";
    else if (msg==0x06FF)  pMsg="CreateRegion";
    else if (msg==0x01F0)  pMsg="DeleteObject";
    else if (msg==0x062F)  pMsg="DrawText";
    else if (msg==0x0626)  pMsg="Escape";
    else if (msg==0x0A32)  pMsg="ExtTextOut";
    else if (msg==0x0324)  pMsg="Polygon";
    else if (msg==0x0538)  pMsg="PolyPolygon";
    else if (msg==0x0325)  pMsg="PolyLine";
    else if (msg==0x012C)  pMsg="SelectClipRegion";
    else if (msg==0x012D)  pMsg="SelectObject";
    else if (msg==0x0234)  pMsg="SelectPalette";
    else if (msg==0x0D33)  pMsg="SetDIBitsToDevice";
    else if (msg==0x0037)  pMsg="SetPaletteEntry";
    else if (msg==0x0B23)  pMsg="StretchBlt";
    else if (msg==0x0B41)  pMsg="StretchBlt";
    else if (msg==0x0F43)  pMsg="StretchDIBits";
    else if (msg==0x0521)  pMsg="TextOut";

    OurPrintf("%s",(LPBYTE)pMsg);

    return TRUE;
}
    
