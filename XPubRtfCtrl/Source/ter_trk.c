/*==============================================================================
   TER_TRK.C
   Track-changes functions.

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (2005)
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

#include "lmcons.h"    // defines the UNLEN variable

/******************************************************************************
    TerAcceptChanges:
    Accept one or all changes.
 ******************************************************************************/
int WINAPI _export TerAcceptChanges(HWND hWnd, BOOL all, BOOL msg, BOOL repaint)
{
    PTERWND w;
    int GroupUndoRef;
    long SaveCurLine;
    int SaveCurCol,font;
    int count=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (TrackChanges) return -1;   // can not accept in track-change in progress


    // ** Accept current change
    if (!all) {
       if (AcceptChange(w,CurLine,CurCol)) {
          if (repaint) PaintTer(w);
          return 1;
       }
       else return 0; 
    } 

    // ** Accept all

    if (msg && MB_OK==MessageBox(hTerWnd,MsgString[MSG_ACCEPT_CHANGES],"",MB_YESNO)) return -1;

    GroupUndoRef=UndoRef;         // connect undos for all accepts

    SaveCurLine=CurLine;
    SaveCurCol=CurCol;

    // Locate and accept each change
    CurLine=0;                   // start from the beginning
    CurCol=0; 

    while (TerFindNextChange(hTerWnd,true,false)) {
       UndoRef=GroupUndoRef;     // connect to previous undo

       if (!AcceptChange(w,CurLine,CurCol)) break; 
    
       // check for the condition when two strings are back-to-back
       font=GetCurCfmt(w,CurLine,CurCol);  // check if positioned on changed text
       if (IsTrackChangeFont(w,font))  PrevTextPos(w,&CurLine,&CurCol);   // position on a non-changed text just before the current changed text

       count++;
    } 

    if (count==0) {              // restore the cursor position
       CurLine=SaveCurLine;
       CurCol=SaveCurCol;
    } 
    else if (repaint) {
       PaintTer(w);
    } 

    if (msg) {
      wsprintf(TempString,"%s %d",MsgString[MSG_CHANGES_COUNT],count);
      MessageBox(hTerWnd,TempString,"",MB_OK);
    } 
    
    return count;
} 

/******************************************************************************
    AcceptChange:
    Accept the current changed text.
 ******************************************************************************/
BOOL AcceptChange(PTERWND w, long line, int col)
{
    int i,font,CurCfmt,RevId;
    long l,BegLine,EndLine;
    int  BegCol,EndCol,StartPos,EndPos;
    LPWORD fmt;

    CurCfmt=GetCurCfmt(w,line,col);
    if (!IsTrackChangeFont(w,CurCfmt)) return false;

    BegLine=EndLine=line;
    BegCol=EndCol=col;
    if (!GetChangeBeginPos(w,&BegLine, &BegCol)) return false;
    if (!TerLocateChangedChar(hTerWnd,TerFont[CurCfmt].InsRev,TerFont[CurCfmt].DelRev,false,&EndLine,&EndCol,true)) return false;

    // ** Accept deletion **
    if (TerFont[CurCfmt].DelRev>0) {
       BOOL SaveUnprotDel=True(TerFlags&TFLAG_UNPROTECTED_DEL);
       
       HilightBegRow=BegLine;
       HilightBegCol=BegCol;
       HilightEndRow=EndLine;
       HilightEndCol=EndCol;
       StretchHilight=false;
       HilightType=HILIGHT_CHAR;

       TerFlags|=TFLAG_UNPROTECTED_DEL;
       TerDeleteBlock(hTerWnd,false);
       if (!SaveUnprotDel) ResetUintFlag(TerFlags,TFLAG_UNPROTECTED_DEL);

       return true;
    } 
 
    // ** Accept insertion **

    // Save undo for insertion
    SaveUndo(w,BegLine,BegCol,EndLine,EndCol,'F');  // save font ids

    // Reset reviewer ids
    for (l=BegLine;l<=EndLine;l++) {
       StartPos=(l==BegLine)?BegCol:0;
       EndPos=(l==EndLine)?EndCol:LineLen(l);     // exclusive
   
       // scan each character in the line to perform indicated delete operation (regular delete or track-delete)
       fmt=OpenCfmt(w,l);
       for (i=StartPos;i<EndPos;i++) {
          font=fmt[i];
          
          RevId=TerFont[font].InsRev;

          font=SetTrackingFont(w,font,TRACK_NONE);
          if (reviewer[RevId].InsColor==TerFont[font].TextColor) {      // reset color
             font=(int)GetNewColor(w,(WORD)font,(DWORD)CLR_AUTO,0L,l,i);          
          }
          if (True(reviewer[RevId].InsStyle&TerFont[font].style)) {      // reset style
             font=SetFontStyle(w,font,reviewer[RevId].InsStyle,false);   // reset the reviewer style
          }

          fmt[i]=font; 
       } 
       CloseCfmt(w,l);
    } 
 
    TerArg.modified++;

    return true;
}
 
/******************************************************************************
    IsTrackChangeFont:
    returns true when a font id corresponds to track-change text.
 ******************************************************************************/
BOOL IsTrackChangeFont(PTERWND w, int CurCfmt)
{
    return (TerFont[CurCfmt].InsRev>0 || TerFont[CurCfmt].DelRev>0);
} 

/******************************************************************************
    GetChangeBeginPos:
    Get the change beginining position
 ******************************************************************************/
BOOL GetChangeBeginPos(PTERWND w, LPLONG pBeginLine, LPINT pBeginCol)
{
    long line=(*pBeginLine);
    int  col=(*pBeginCol);
    int CurCfmt;

    CurCfmt=GetCurCfmt(w,line,col);
    if (TerLocateChangedChar(hTerWnd,TerFont[CurCfmt].InsRev,TerFont[CurCfmt].DelRev,false,&line,&col,false)) {   // scan backward for the character beyond the end of this string
       NextTextPos(w,&line,&col);
    }
    else {                        // possibly at position 0
       int FirstCfmt=GetCurCfmt(w,0,0);   //
       if (TerFont[FirstCfmt].InsRev==TerFont[CurCfmt].InsRev && TerFont[FirstCfmt].DelRev==TerFont[CurCfmt].DelRev) {
          line=0;
          col=0;
       }
       else return false;  
    }  

    (*pBeginLine)=line;
    (*pBeginCol)=col;

    return true;
}
 
/******************************************************************************
    TerFindNextChange:
    Find next change.
 ******************************************************************************/
BOOL WINAPI _export TerFindNextChange(HWND hWnd, BOOL forward, BOOL repaint)
{
    PTERWND w;
    int  CurCfmt;
    BOOL result=false;
    long line;
    int  col;

    if (NULL==(w=GetWindowPointer(hWnd))) return false;  // get the pointer to window data

    line=CurLine;
    col=CurCol;

    CurCfmt=GetCurCfmt(w,line,col);
    if (TerFont[CurCfmt].DelRev>0) {
       if (!TerLocateChangedChar(hTerWnd,-1,TerFont[CurCfmt].DelRev,false,&line,&col,forward)) goto END;
    } 
    else if (TerFont[CurCfmt].InsRev>0) {
       if (!TerLocateChangedChar(hTerWnd,TerFont[CurCfmt].InsRev,0,false,&line,&col,forward)) goto END;
    } 
       
    result=TerLocateChangedChar(hTerWnd,-1,-1,true,&line,&col,forward);  // find for the presense of any reviewer id type (ins or del type)

    if (result && !forward) GetChangeBeginPos(w,&line,&col);         // position at the first character of the changed text

    END:

    if (result) {
      CurCol=col;
      
      if (repaint) TerPosLine(w,line+1);
      else         CurLine=line;
    }
    else {
      if (repaint) MessageBox(hTerWnd,MsgString[MSG_NO_CHANGED_TEXT],"",MB_OK);
    }  

    return result;
}
 
/***************************************************************************
    TerLocateChangedChar:
    This routine scans the text starting from the given line and col position
    to search for the changed text.  The called specifies the direction of
    the search.  The caller also specifies to look for the presence of absence
    of such a change.
    Set the InsRev and DelRev to the reviewer id for the change.  
    Set thse parameters to -1 to match any reviewer id.
    If the changed text is located, the routine returns a TRUE value.
    It also updates the line and col parameters with the location of the changed text.
****************************************************************************/
BOOL WINAPI _export TerLocateChangedChar(HWND hWnd, int InsRev, int DelRev, BOOL present, long far *StartLine, int far *StartCol, BOOL forward)
{
    PTERWND w;
    long line;
    int  i,col,CurCfmt,CurInsRev,CurDelRev;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return false;  // get the pointer to window data

    // reset the checked flag
    for (i=0;i<TotalFonts;i++) ResetUintFlag(TerFont[i].flags,FFLAG_CHECKED);

    // scan each line in the forward direction
    if (forward) {
       for (line=(*StartLine);line<TotalLines;line++) {
          if (line==(*StartLine)) col=*StartCol;
          else                    col=0;

          if (col>=LineLen(line)) continue;                   // go to next line
          if (LineLen(line)==0) continue;

          if (cfmt(line).info.type==UNIFORM) {
             CurCfmt=cfmt(line).info.fmt;
             if (True(TerFont[CurCfmt].flags&FFLAG_CHECKED)) continue;  // this font id already checked
             TerFont[CurCfmt].flags|=FFLAG_CHECKED;
             
             CurInsRev=TerFont[CurCfmt].InsRev;
             CurDelRev=TerFont[CurCfmt].DelRev;

             if (LocateRevMatched(w,present,InsRev,DelRev,CurInsRev,CurDelRev)) {
                *StartLine=line;
                *StartCol=col;
                return true;
             }
             else continue;                       // style not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i<LineLen(line);i++) {
            CurCfmt=fmt[i];
            if (True(TerFont[CurCfmt].flags&FFLAG_CHECKED)) continue;  // this font id already checked
            TerFont[CurCfmt].flags|=FFLAG_CHECKED;

            CurInsRev=TerFont[CurCfmt].InsRev;
            CurDelRev=TerFont[CurCfmt].DelRev;

            if (LocateRevMatched(w,present,InsRev,DelRev,CurInsRev,CurDelRev)) {
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return true;
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
             if (True(TerFont[CurCfmt].flags&FFLAG_CHECKED)) continue;  // this font id already checked
             TerFont[CurCfmt].flags|=FFLAG_CHECKED;
             
             CurInsRev=TerFont[CurCfmt].InsRev;
             CurDelRev=TerFont[CurCfmt].DelRev;

             if (LocateRevMatched(w,present,InsRev,DelRev,CurInsRev,CurDelRev)) {
                *StartLine=line;
                *StartCol=col;
                return true;
             }
             else continue;                       // style not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i>=0;i--) {
            CurCfmt=fmt[i];
            if (True(TerFont[CurCfmt].flags&FFLAG_CHECKED)) continue;  // this font id already checked
            TerFont[CurCfmt].flags|=FFLAG_CHECKED;
            
            CurInsRev=TerFont[CurCfmt].InsRev;
            CurDelRev=TerFont[CurCfmt].DelRev;

            if (LocateRevMatched(w,present,InsRev,DelRev,CurInsRev,CurDelRev)) {
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return true;
            }
          }
          CloseCfmt(w,line);
       }
    }

    return false;
}
 
 /******************************************************************************
    LocateRevMatched:
    Check if presence or abasense of reviewer id is matched.
 ******************************************************************************/
BOOL LocateRevMatched(PTERWND w, BOOL present, int InsRev, int DelRev, int CurInsRev, int CurDelRev)
{
    if (present) {             // search for presense of a reviewer id
       if (InsRev==CurInsRev && DelRev==CurDelRev) return true;   // matched
       if (InsRev==CurInsRev && DelRev==-1)  return true;         // match all del rev
       if (InsRev==-1 && DelRev==CurDelRev) return true;           // match all ins rev
       if ((InsRev<0 && DelRev<0) && (CurInsRev>0 || CurDelRev>0)) return true;  // match any non-zero reviewer
    }
    else {                    // check for not-present condition
       if (InsRev>=0 && DelRev>=0 && (InsRev!=CurInsRev || DelRev!=CurDelRev)) return true;  // configuration not matched
       if (InsRev<0 && DelRev>=0 && DelRev!=CurDelRev) return true;
       if (DelRev<0 && InsRev>=0 && InsRev!=CurInsRev) return true;
       
       if (InsRev<0 && DelRev<0 && (CurInsRev==0 && CurDelRev==0)) return true;                // match for absense of any reviewer
    }  
 
    return false;
} 

 /******************************************************************************
    TrackDelBlock:
    Delete the specified block of text in the tracking mode. The function returns TRUE
    if track-deletion is successful.  
 ******************************************************************************/
BOOL TrackDelBlock(PTERWND w, long BegLine, int BegCol, long EndLine, int EndCol, BOOL ResetHilight, BOOL repaint)
{
   long l;
   int i,StartPos,EndPos,font;
   LPWORD fmt;
   BOOL   TrackDelTextFound;   // indicates the text that needs to be deleted using the tracking method (not by simply removing the character)
   BOOL   IsTrackDelLine;
   BOOL   RevInsertedLine;
   long   DelCount=0,GroupUndoRef,EndAbsPos;

   if (!TrackChanges) return false;

   // check if text the text to be deleted using tracking method exists in the block
   TrackDelTextFound=false;     
   for (l=BegLine;l<=EndLine;l++) {
      if (False(LineFlags2(l)&LFLAG2_INS_REV)) {    // no text in this line inserted by a reviewer
          TrackDelTextFound=true;   // this line needs to be deleted using the tracking method
          break;
      } 
      
      StartPos=(l==BegLine)?BegCol:0;
      EndPos=(l==EndLine)?EndCol:LineLen(l);     // exclusive

      fmt=OpenCfmt(w,l);
      for (i=StartPos;i<EndPos;i++) {
         font=fmt[i];
         if (TerFont[font].InsRev!=TrackRev) {
            TrackDelTextFound=true;   // this text needs to be deleted using the tracking method
            break;
         }
      } 
      CloseCfmt(w,l);
      if (TrackDelTextFound) break;
   } 
   if (!TrackDelTextFound) return false;   // normal deletion will do

   // undo consists of saving the original text, and marking the resultant text as inserted
   EndAbsPos=RowColToAbs(w,EndLine,EndCol);
   GroupUndoRef=UndoRef;
   SaveUndo(w,BegLine,BegCol,EndLine,EndCol-1,'D');  // save undo

   // scan the range of text and do deletion of both types (regular delete and track-delete)
   for (l=BegLine;l<=EndLine;l++) {
      StartPos=(l==BegLine)?BegCol:0;
      EndPos=(l==EndLine)?EndCol:LineLen(l);     // exclusive
   
      IsTrackDelLine=(False(LineFlags2(l)&LFLAG2_INS_REV));    // no text in this line inserted by a reviewer
      if (IsTrackDelLine) {       // do track delete of the text in this line
         fmt=OpenCfmt(w,l);
         for (i=StartPos;i<EndPos;i++) {
            font=fmt[i];
            if (TerFont[font].DelRev!=TrackRev) fmt[i]=SetTrackingFont(w,fmt[i],TRACK_DEL);
         } 
         CloseCfmt(w,l);
         continue;
      } 

      // check if the entire line contains the text entered by the current reviewer (and not deleted by another reviewer)
      RevInsertedLine=true;
      fmt=OpenCfmt(w,l);
      for (i=StartPos;i<EndPos;i++) {
         font=fmt[i];
         if (TerFont[font].InsRev!=TrackRev || True(TerFont[font].DelRev)) {
            RevInsertedLine=false;
            break;
         }
      } 
      CloseCfmt(w,l);

      if (RevInsertedLine && StartPos==0 && EndPos==LineLen(l)) {        // delete the whole line
         DelCount+=LineLen(l);            // number of characters actually removed
         if (l<CurLine) CurLine--;        // adjust current line
         if (l==CurLine) {
            CurLine--;
            CurCol=LineLen(CurLine)-1;
         }

         MoveLineArrays(w,l,1,'D');
         EndLine--;
         continue;
      }
      
      // scan each character in the line to perform indicated delete operation (regular delete or track-delete)
      fmt=OpenCfmt(w,l);
      for (i=StartPos;i<EndPos;i++) {
         font=fmt[i];
         if (True(TerFont[font].DelRev)) continue;   // already track-delete

         if (TerFont[font].InsRev==TrackRev) {    // text inserted by the current reviewer - do regular delete
            MoveLineData(w,l,i,1,'D');
            fmt=OpenCfmt(w,l);    // MoveLineData invalidates the fmt pointer
            i--;                  // don't let i advance because the character deleted at this position
            EndPos--;
            DelCount++;
            if (l==CurLine && i<CurCol) CurCol--;
         } 
         else  {     // regular text or text inserted by another reviewer - track delete
            if (TerFont[font].DelRev!=TrackRev) fmt[i]=SetTrackingFont(w,fmt[i],TRACK_DEL);
         }
      } 
      CloseCfmt(w,l);
   } 
 
   // mark the resultant text as inserted
   UndoRef=GroupUndoRef;
   EndAbsPos-=DelCount;      // new ending pos
   AbsToRowCol(w,EndAbsPos,&EndLine,&EndCol);
   SaveUndo(w,BegLine,BegCol,EndLine,EndCol-1,'I');  // save undo

   if (CurLine>=TotalLines) CurLine=TotalLines-1;
   if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
   if (CurCol<0) CurCol=0;

   if (ResetHilight) HilightType=HILIGHT_OFF;

   if (repaint) PaintTer(w);
 
   return true;
} 
 /******************************************************************************
    TrackDel:
    Delete current character in the tracking mode. The function returns TRUE
    if track-deletion is successful.  
 ******************************************************************************/
BOOL TrackDel(PTERWND w, long line, int col, BOOL forward)
{
    int font;
    LPWORD fmt;
    long SaveLine=CurLine;

    if (!TrackChanges) return false;

    font=GetCurCfmt(w,line,col);

    if (TerFont[font].InsRev==TrackRev) return false;  // this character was inserted by the current reviewer, so it let be deleted normally

    if (True(TerFont[font].DelRev)) goto END;  // already track deleted

    SaveUndo(w,line,col,line,col,'F');  // save undo of current formatting

    fmt=OpenCfmt(w,line);

    fmt[col]=SetTrackingFont(w,fmt[col],TRACK_DEL);
     
    CloseCfmt(w,line);

    END:

    if (forward) NextTextPos(w,&CurLine,&CurCol);

    return true;
} 

 /******************************************************************************
     TerEnableTracking:
     Enable or disable tracking of text changes.  This function returns the
     previous status.
 ******************************************************************************/
 BOOL WINAPI _export TerEnableTracking(HWND hWnd, bool enable, LPBYTE UName, BOOL UseDefaultClrStyle,
     DWORD InsStyle, COLORREF InsColor, DWORD DelStyle, COLORREF DelColor)
 {
     bool PrevStatus;
     BYTE name[UNLEN+1];
     LPBYTE pName=null;
     int  i,len;
     SYSTEMTIME SysTime;
     FILETIME  FileTime;
     WORD DosDate,DosTime;
     bool NewUser=false;

     PTERWND w;

     if (NULL==(w=GetWindowPointer(hWnd))) return false;  // get the pointer to window data

     if (!TerArg.WordWrap) return false;

     if (TrackChanges == enable) return true;   // no status change requested

     PrevStatus=TrackChanges;
     TrackChanges=enable;

     if (enable) {
        if (UName && lstrlen(UName)>0) pName=UName;
        else {  // get system user name
          len=UNLEN;
          name[0]=0;
          GetUserName(name,&len);
          if (len>49) len=49;   // StrReviewer limit
          name[len]=0;
          pName=name;
        }

        // check if this reviewer exists
        for (i=1;i<TotalReviewers;i++) if (lstrcmpi(reviewer[i].name,pName)==0) break;

        if (i==TotalReviewers) {
           i=GetReviewerSlot(w);
           lstrncpy(reviewer[i].name,pName,49);  // copy name
           reviewer[i].name[49]=0;
           NewUser=true;

           if (!UseDefaultClrStyle) {
              reviewer[i].InsStyle=InsStyle;
              reviewer[i].DelStyle=DelStyle;

              reviewer[i].InsColor=InsColor;
              reviewer[i].DelColor=DelColor;
           } 
        }
        
        TrackRev=i;   // current reviewer id

        // get the current date/time as DWORD
        GetLocalTime(&SysTime);
        SystemTimeToFileTime(&SysTime,&FileTime);
        FileTimeToDosDateTime(&FileTime,&DosDate,&DosTime);
        TrackTime=(((DWORD)DosDate)<<16)+DosTime;

        InsertMode=true;              // insert mode always true when tracking

        if (!NewUser) {               // font/style might have changed, so recreate the fonts
           RecreateFonts(w,hTerDC);
           RequestPagination(w,true);
           PaintTer(w);
        }     
     } 
     else {
        TrackRev=0;
        TrackTime=0;
     } 
     return PrevStatus;
 }

 /******************************************************************************
     TrackingComment:
     Get tracking comment string. This function returns TRUE if tracking comment
     exist at the given loction.
 ******************************************************************************/
 BOOL TrackingComment(PTERWND w, long line, int col, LPBYTE pMsg)
 {
     int font=GetCurCfmt(w,line,col);
     int len=0,DateLen;
     int rev;
     WORD DosDate,DosTime;
     FILETIME FileTime;
     SYSTEMTIME SysTime;
     BYTE pDateTime[MAX_WIDTH+1];

     if (True(TerFlags6&TFLAG6_NO_TRACK_MSG)) return false;

     if (False(TerFont[font].InsRev) && False(TerFont[font].DelRev)) return false;

     if (True(TerFont[font].InsRev)) {
         rev=TerFont[font].InsRev;

         // format date/time
         DosDate=HIWORD(TerFont[font].InsTime);
         DosTime=LOWORD(TerFont[font].InsTime);
         DosDateTimeToFileTime(DosDate,DosTime,&FileTime);
         FileTimeToSystemTime(&FileTime,&SysTime);
         
         GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&SysTime,NULL,pDateTime,MAX_WIDTH);
         strcat(pDateTime," ");
         DateLen=lstrlen(pDateTime);
         GetTimeFormat(LOCALE_USER_DEFAULT,TIME_NOSECONDS,&SysTime,NULL,&(pDateTime[DateLen]),MAX_WIDTH);

         wsprintf(&(pMsg[len]),"%s%s\r\n    @ %s",MsgString[MSG_INSERTED_BY],reviewer[rev].name,pDateTime);

         len=lstrlen(pMsg);
     } 

     if (True(TerFont[font].DelRev)) {
         rev=TerFont[font].DelRev;

         // format date/time
         DosDate=HIWORD(TerFont[font].DelTime);
         DosTime=LOWORD(TerFont[font].DelTime);
         DosDateTimeToFileTime(DosDate,DosTime,&FileTime);
         FileTimeToSystemTime(&FileTime,&SysTime);
         
         GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&SysTime,NULL,pDateTime,MAX_WIDTH);
         strcat(pDateTime," ");
         DateLen=lstrlen(pDateTime);
         GetTimeFormat(LOCALE_USER_DEFAULT,TIME_NOSECONDS,&SysTime,NULL,&(pDateTime[DateLen]),MAX_WIDTH);

         if (len>0) {                 // start on the new line
           lstrcat(pMsg,"\n");
           len=lstrlen(pMsg);
         }
          
         wsprintf(&(pMsg[len]),"%s%s\r\n    @ %s",MsgString[MSG_DELETED_BY],reviewer[rev].name,pDateTime);

         len=lstrlen(pMsg);
     } 

     return (lstrlen(pMsg)>0);
 }
  
 /******************************************************************************
     SetTrackingFont:
     Set font for tracking changes.
 ******************************************************************************/
 int SetTrackingFont(PTERWND w, int CurFont, int type)
 {
     int NewFont;
     struct StrFont font;

     if (TerFont[CurFont].style&PICT) {
        TerFont[CurFont].InsRev=0;
        TerFont[CurFont].DelRev=0;
        TerFont[CurFont].InsTime=0;
        TerFont[CurFont].DelTime=0;

        if (type==TRACK_INS) {
          TerFont[CurFont].InsRev=TrackRev;
          TerFont[CurFont].InsTime=TrackTime;
          
          TerFont[CurFont].DelRev=0;         // insertion text is always new
          TerFont[CurFont].DelTime=0;
        } 
        else if (type==TRACK_DEL) {
          TerFont[CurFont].DelRev=TrackRev;
          TerFont[CurFont].DelTime=TrackTime;
        }
        else {
          TerFont[CurFont].InsRev=0;
          TerFont[CurFont].DelRev=0;
          TerFont[CurFont].InsTime=0;
          TerFont[CurFont].DelTime=0;
        }  
        SetPictSize(w,CurFont,TwipsToScrY(TerFont[CurFont].PictHeight),TwipsToScrX(TerFont[CurFont].PictWidth),TRUE);
        XlateSizeForPrt(w,CurFont);              // convert size to printer resolution
        return CurFont;   // do not modify a picture font
     }

     FarMove(&(TerFont[CurFont]),&font,sizeof(font));
     if (type==TRACK_INS) {
       font.InsRev=TrackRev;
       font.InsTime=TrackTime;

       font.DelRev=0;       // insertion text is always new
       font.DelTime=0;
     } 
     else if (type==TRACK_DEL) {  // retain any previous 'insertion' reviewer
       font.DelRev=TrackRev;
       font.DelTime=TrackTime;
     }
     else {
       font.InsRev=0;
       font.DelRev=0;
       font.InsTime=0;
       font.DelTime=0;
     } 
     
     if (True(NewFont=GetNewFont2(w,hTerDC,CurFont,&font))) 
          return NewFont;             // return the new font
     else return CurFont;
 }

/******************************************************************************
    GetReviewerSlot:
    get an empty reviewer table slot.  This function returns -1 if
    a slot not found.
******************************************************************************/
GetReviewerSlot(PTERWND w)
{
    DWORD OldSize, NewSize;
    int   i,NewMaxReviewers;
    BYTE *pMem;
    COLORREF clr;


    // use the next slot
    if (TotalReviewers>=MaxReviewers) {        // allocate more memory for the cell table
       NewMaxReviewers=MaxReviewers+MaxReviewers/2;

       // allocate the reviewer array
       OldSize=((DWORD)MaxReviewers+1)*sizeof(struct StrReviewer);
       NewSize=((DWORD)NewMaxReviewers+1)*sizeof(struct StrReviewer);
       if (NULL==(pMem=OurAlloc(NewSize))) return -1;

       FarMove(reviewer,pMem,OldSize);
       OurFree(reviewer);
       reviewer=(struct StrReviewer *)pMem;

       MaxReviewers=NewMaxReviewers;    // new max cells
    }

    FarMemSet(&reviewer[TotalReviewers],0,sizeof(struct StrReviewer));
    TotalReviewers++;

    // initiaize the reviewer slot
    i=TotalReviewers-1;
    
    reviewer[i].InsStyle=0;
    reviewer[i].DelStyle=STRIKE;
        
    clr=CLR_AUTO;

    if      (i==1) clr=0xFF;      // red
    else if (i==2) clr=0xFF0000;  // blue
    else if (i==3) clr=0xFF00;    // green
    else if (i==4) clr=0xFFFF;   
    else if (i==5) clr=0xFF00FF; 
    else if (i==6) clr=0xFFFF00;
    else if (i==7) clr=0x80;      
    else if (i==8) clr=0x8000;    
    else if (i==9) clr=0x800000;  
    else if (i==10) clr=0x8080;   
    else if (i==11) clr=0x800080; 
    else if (i==12) clr=0x808000;

    reviewer[i].InsColor=reviewer[i].DelColor=clr;


    return (TotalReviewers-1);
}

/******************************************************************************
    FreeReviewer:
    Free a reviews slot.
******************************************************************************/
FreeReviewer(PTERWND w, int idx)
{
    if (idx<0 || idx>=TotalReviewers) return false;
    return true;   // for now this structure does not use any allocated resources to free
}
