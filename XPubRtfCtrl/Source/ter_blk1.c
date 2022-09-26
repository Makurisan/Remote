/*==============================================================================
   TER_BLK1.C
   TER Block Copy/Move/Delete additional functions.

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
    TerRotateFrameText:
    Set the text direction for a frame.
******************************************************************************/
BOOL WINAPI _export TerRotateFrameText(HWND hWnd,BOOL dialog, long LineNo, int direction, BOOL repaint)
{
    PTERWND w;
    int frm,angle;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (LineNo>=TotalLines) return FALSE;

    if (LineNo>=0) {
       if (cid(LineNo)) return FALSE;  // cann't rotate if the frame contains a table
       frm=fid(LineNo);
    }
    else frm=(int)-LineNo;

    if (frm<0 || frm>=TotalParaFrames) return FALSE;

    if (ParaFrame[frm].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT)) return FALSE;
    if (ParaFrame[frm].pict>0) return FALSE;  // can't rotate a picture frame

    if (dialog) {            // show dialog box
       DlgInt1=ParaFrame[frm].TextAngle;
       if (!CallDialogBox(w,"TextRotationParam",TextRotationParam,0L)) return FALSE;
       angle=DlgInt1;
    }
    else {
       angle=0;
       if      (direction==TEXT_TOP_TO_BOT) angle=270;
       else if (direction==TEXT_BOT_TO_TOP) angle=90;
    }
    
    ParaFrame[frm].TextAngle=angle;
    ParaFrame[frm].MinHeight=ParaFrame[frm].height;

    TerArg.modified++;

    if (CurLine<RepageBeginLine) RepageBeginLine=CurLine;

    if (repaint) PaintTer(w);

    return TRUE; 
}

/******************************************************************************
    CheckImageMapHit:
    Check if the current logical point is positioned over a image map area
*******************************************************************************/
BOOL CheckImageMapHit(PTERWND w,int pict)
{
    int i,x,y,MapId;
    struct StrImageMap far *pMap;
    struct StrImageMapRect far *pMapRect;

    CurMapPict=CurMapId=CurMapRect=0;  // reset the hits

    if (TotalImageMaps==1) return FALSE;
    if (!(TerFont[pict].InUse) || !(TerFont[pict].style&PICT)) return FALSE;
    if (TerFont[pict].MapId==0) return FALSE;

    // get the mouse positions
    x=MouseX;
    y=MouseY;
    x=x-TerWinRect.left;    // relative to the top of the text box
    y=y-TerWinRect.top;

    x-=TerFont[pict].PictX;  // relative to the top/left of the pict
    y-=TerFont[pict].PictY;

    MapId=TerFont[pict].MapId;

    if (MapId<=0 || MapId>=TotalImageMaps) return FALSE;

    pMap=&(ImageMap[MapId]);

    for (i=0;i<pMap->TotalRects;i++) {
       pMapRect=&(pMap->pMapRect[i]);

       if (    x>=pMapRect->rect.left && x<=pMapRect->rect.right
           &&  y>=pMapRect->rect.top && y<=pMapRect->rect.bottom) {
           
           CurMapPict=pict;
           CurMapId=MapId;
           CurMapRect=i;
           
           return TRUE;
       }
    } 

    return FALSE;
}

/******************************************************************************
    TerCreateImageMap:
    Create a new image map. This function returns the id for the new map.  It returns
    zero if unsuccessful.
*******************************************************************************/
int WINAPI _export TerCreateImageMap(HWND hWnd, LPBYTE name)
{
    PTERWND w;
    struct StrImageMap far *pMap;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (TotalImageMaps>=MAX_IMAGE_MAPS) return PrintError(w,MSG_OUT_OF_IMAGE_MAP_TABLE,"TerCreateImageMap");
 
    pMap=&(ImageMap[TotalImageMaps]);
    TotalImageMaps++;

    FarMemSet(pMap,0,sizeof(struct StrImageMap));
    if (name) {
       pMap->name=MemAlloc(lstrlen(name)+1); 
       lstrcpy(pMap->name,name);
    }

    return (TotalImageMaps-1);         // return the id of the current image map
}

/******************************************************************************
    TerAddImageMapRect:
    Add a rectangle to the given image map
*******************************************************************************/
BOOL WINAPI _export TerAddImageMapRect(HWND hWnd, int MapId, LPBYTE name, LPBYTE link, LPBYTE target, int left, int top, int right, int bottom)
{
    PTERWND w;
    struct StrImageMap far *pMap;
    struct StrImageMapRect far *pMapRect;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (MapId<=0 || MapId>=TotalImageMaps) return FALSE;

    pMap=&(ImageMap[MapId]);

    // allocate a new image map rect
    if (pMap->TotalRects==0) pMap->pMapRect=MemAlloc(sizeof(struct StrImageMapRect));
    else                     pMap->pMapRect=MemReAlloc(pMap->pMapRect,(pMap->TotalRects+1)*sizeof(struct StrImageMapRect)); 

    if (!pMap->pMapRect) return PrintError(w,MSG_OUT_OF_MEM,"TerAddImageMapRect");
    
    pMapRect=&(pMap->pMapRect[pMap->TotalRects]);  // pointer to the current image map
    pMap->TotalRects++;

    // populate the image map structure
    if (name) {
       pMapRect->name=MemAlloc(lstrlen(name)+1);
       lstrcpy(pMapRect->name,name);
    } 
    if (link) {
       pMapRect->link=MemAlloc(lstrlen(link)+1);
       lstrcpy(pMapRect->link,link);
    } 
    if (target) {
       pMapRect->target=MemAlloc(lstrlen(target)+1);
       lstrcpy(pMapRect->target,target);
    } 
    
    pMapRect->rect.left=left; 
    pMapRect->rect.top=top; 
    pMapRect->rect.right=right; 
    pMapRect->rect.bottom=bottom; 

    return TRUE;
}

/******************************************************************************
    TerSetMapRectInfo:
    Set info for the specified map rect or currently clicked maprect
*******************************************************************************/
BOOL WINAPI _export TerSetMapRectInfo(HWND hWnd, int MapId, int RectId, LPBYTE name, LPBYTE link, LPBYTE target)
{
    PTERWND w;
    struct StrImageMap far *pMap;
    struct StrImageMapRect far *pMapRect;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurMapId>0) {             // image clicked
       if (MapId<0) {
          MapId=CurMapId;
          RectId=CurMapRect;
       }
       else if (RectId<0) RectId=CurMapRect;
    }
     
    if (MapId<=0 || MapId>=TotalImageMaps) return FALSE;

    pMap=&(ImageMap[MapId]);

    if (RectId<0 || RectId>=pMap->TotalRects) return FALSE;

    pMapRect=&(pMap->pMapRect[RectId]);  // pointer to the current image map

    // populate the image map structure
    if (name) {
       if (pMapRect->name) MemFree(pMapRect->name);
       pMapRect->name=MemAlloc(lstrlen(name)+1);
       lstrcpy(pMapRect->name,name);
    } 
    if (link) {
       if (pMapRect->link) MemFree(pMapRect->link);
       pMapRect->link=MemAlloc(lstrlen(link)+1);
       lstrcpy(pMapRect->link,link);
    } 
    if (target) {
       if (pMapRect->target) MemFree(pMapRect->target);
       pMapRect->target=MemAlloc(lstrlen(target)+1);
       lstrcpy(pMapRect->target,target);
    } 
    
    return TRUE;
}

/******************************************************************************
    TerImageMapNameToId:
    Return the map id for a map name.
*******************************************************************************/
int WINAPI _export TerImageMapNameToId(HWND hWnd, LPBYTE name)
{
    PTERWND w;
    int i;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data
 
    for (i=1;i<TotalImageMaps;i++) {
       if (ImageMap[i].name && lstrcmpi(ImageMap[i].name,name)==0) return i;
    } 

    return 0;
}

/******************************************************************************
    TerGetPictMapId:
    Get the map id for a picture.
******************************************************************************/
int WINAPI _export TerGetPictMapId(HWND hWnd, int PictId)
{
    PTERWND w;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (PictId<0 || PictId>=TotalFonts) return 0;
    if (!(TerFont[PictId].InUse) || !(TerFont[PictId].style&PICT)) return 0;

    return TerFont[PictId].MapId;
}  


/******************************************************************************
    TerSetPictMapId:
    Set a map id for a picture.  This function can also reset the map id to 0.
*******************************************************************************/
BOOL WINAPI _export TerSetPictMapId(HWND hWnd, int PictId, int MapId)
{
    PTERWND w;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (MapId<0 || MapId>=TotalImageMaps) return FALSE;

    if (PictId<0 || PictId>=TotalFonts) return FALSE;
    if (!(TerFont[PictId].InUse) || !(TerFont[PictId].style&PICT)) return FALSE;

    TerFont[PictId].MapId=MapId;
 
    return TRUE;
}  

/******************************************************************************
    TerGetImageMapInfo:
    Get the image map information.
******************************************************************************/
BOOL WINAPI _export TerGetImageMapInfo(HWND hWnd, int MapId, LPBYTE name, LPINT pCount)
{
    PTERWND w;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    // initialize the return parameters
    if (name) lstrcpy(name,"");
    if (pCount) (*pCount)=0;

    if (MapId<0 || MapId>=TotalImageMaps) return FALSE;

    if (name) lstrcpy(name,ImageMap[MapId].name);
    if (pCount) (*pCount)=ImageMap[MapId].TotalRects;

    return TRUE;
}

/******************************************************************************
    TerGetImageMapRectInfo:
    Get the image map rectangle information.
******************************************************************************/
BOOL WINAPI _export TerGetImageMapRectInfo(HWND hWnd, int MapId, int MapRectId, LPBYTE name, LPBYTE link, 
                                        LPBYTE target, LPINT pLeft, LPINT pTop, LPINT pRight, LPINT pBottom)
{
    PTERWND w;
    struct StrImageMap far *pMap;
    struct StrImageMapRect far *pMapRect;
 
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    // initialize the return parameters
    if (name)   lstrcpy(name,"");
    if (link)   lstrcpy(link,"");
    if (target) lstrcpy(target,"");
    if (pLeft)    (*pLeft)=0;
    if (pRight)   (*pRight)=0;
    if (pTop)     (*pTop)=0;
    if (pBottom)  (*pBottom)=0;

    if (CurMapId>0) {    // image hit
       if (MapId<0) {
          MapId=CurMapId;
          MapRectId=CurMapRect;
       }
       else if (MapRectId<0) MapRectId=CurMapRect;
    }

    if (MapId<0 || MapId>=TotalImageMaps) return FALSE;
    pMap=&(ImageMap[MapId]);

    if (MapRectId<0 || MapRectId>=pMap->TotalRects) return FALSE;
    pMapRect=&(pMap->pMapRect[MapRectId]);

    if (name && pMapRect->name)     lstrcpy(name,pMapRect->name);
    if (link && pMapRect->link)     lstrcpy(link,pMapRect->link);
    if (target && pMapRect->target) lstrcpy(target,pMapRect->target);

    if (pLeft)  (*pLeft)=pMapRect->rect.left;
    if (pRight) (*pRight)=pMapRect->rect.right;
    if (pTop)   (*pTop)=pMapRect->rect.top;
    if (pBottom)(*pBottom)=pMapRect->rect.bottom;

    return TRUE;
}

/******************************************************************************
    FreeImageMapTable:
    Free the image map table.
*******************************************************************************/
FreeImageMapTable(PTERWND w)
{
    int i,j;
    struct StrImageMap far *pMap;
    struct StrImageMapRect far *pMapRect;

    for (i=1;i<TotalImageMaps;i++) {
      pMap=&(ImageMap[i]);
      
      if (pMap->name) MemFree(pMap->name);
      
      if (pMap->pMapRect && pMap->TotalRects>0) {
         for (j=0;j<pMap->TotalRects;j++) {
            pMapRect=&(pMap->pMapRect[j]);

            if (pMapRect->name)   MemFree(pMapRect->name);
            if (pMapRect->link)   MemFree(pMapRect->link);
            if (pMapRect->target) MemFree(pMapRect->target);
         } 
         MemFree(pMap->pMapRect);
      }  
    } 
    
    TotalImageMaps=1;          // reset to the initial value

    return TRUE;
}
 
/******************************************************************************
    GetDocHeight:
    Get the document line height
*******************************************************************************/
long GetDocHeight(PTERWND w)
{
    long l,height=0;

    for (l=0;l<TotalLines;l++) height+=ScrLineHeight(w,l,TRUE,FALSE);
 
    return height;
} 

/******************************************************************************
    CheckWindowOverflow:
    Check for text overflow in the window
*******************************************************************************/
CheckWindowOverflow(PTERWND w)
{
    long NewDocHeight=GetDocHeight(w);
 
    if (  NewDocHeight>=DocHeight && NewDocHeight>TerWinHeight 
       && UndoCount>0 && undo[UndoCount-1].type!='D') {
       MessageBeep(0);
       TerPageUp(w,FALSE);
       PostMessage(hTerWnd,WM_COMMAND,ID_UNDO,0L);
    }
 
    return TRUE;
}
 
/******************************************************************************
    TerUndo:
    Undo/redo last deleted text (if exists)
*******************************************************************************/
TerUndo(PTERWND w, BOOL DoUndo)
{
    BYTE huge *pUndo;
    BYTE huge *pUndoLead;
    WORD huge *pUndoCfmt;
    WORD huge *pUndoPfmt;
    LPBYTE ptr,lead;
    LPWORD fmt;
    WORD huge *pPrevFmt;
    BYTE PrevChar,PrevLead;
    WORD PrevFmt;
    int CurId=-1,idx,NewType;
    long BegLine,EndLine;
    int  i,pict,BegCol,EndCol,BegCl,EndCl,UndoParaId,TotalIds;
    long l,ChrIdx,ParaIdx,frm;
    HANDLE hPrevFmt;
    bool  SaveTrackChanges;


    if (UndoCount<=0 && DoUndo) {
       //PrintError(w,MSG_UNDO_EMPTY,"");
       return TRUE;
    }
    if (UndoTblSize==UndoCount && !DoUndo) {
       //PrintError(w,MSG_REDO_EMPTY,"");
       return TRUE;
    }

    // get operation id to undo
    InUndo=TRUE;                  //set flag to disable SaveUndo temporarily
    if (UndoTblSize<UndoCount) UndoTblSize=UndoCount;

    // perform undos for the given operation id
    while (TRUE) {
       if (DoUndo) idx=UndoCount-1;                    // current index to undo
       else        idx=UndoCount;
       if (CurId<0) CurId=undo[idx].id;

       if (idx<0 || idx>=UndoTblSize || undo[idx].id!=CurId) break;

       if (undo[idx].type!=UNDO_FRAME && undo[idx].type!=UNDO_FRAME_LIMITED 
          && undo[idx].type!=UNDO_PICT_SIZE && undo[idx].type!=UNDO_TABLE_ATTRIB) {
          AbsToRowCol(w,undo[idx].beg,(long far *)&BegLine,(int far *)&BegCol);
          AbsToRowCol(w,undo[idx].end,(long far *)&EndLine,(int far *)&EndCol);
          CurLine=BegLine;
          CurCol=BegCol;
       }
       
       NewType=CurUndoType=undo[idx].type;

       switch (undo[idx].type) {
          case 'I':                          // delete the inserted block
             // save for redo for the first time, since SaveUndo does not save for 'insert'
             if (DoUndo) SaveUndoDel(w,idx,BegLine,BegCol,EndLine,EndCol);

             HilightType=HILIGHT_CHAR;       // prepare for BYTE block delete
             HilightBegRow=BegLine;
             HilightEndRow=EndLine;
             HilightBegCol=BegCol;
             HilightEndCol=EndCol+1;
             if (HilightEndCol>=LineLen(HilightEndRow) && (HilightEndRow+1)<TotalLines && cid(HilightEndRow)==cid(HilightEndRow+1)) {
                HilightEndRow++;
                HilightEndCol=0;
             }
             
             if (HilightBegRow==HilightEndRow && BegCol==EndCol) TerOpFlags2|=TOFLAG2_NO_NORMALIZE_FNOTE;

             SaveTrackChanges=TrackChanges;
             TrackChanges=false;                   // do regular delete
             
             DeleteCharBlock(w,FALSE,FALSE);       // delete the block, do not paint the screen
             
             TrackChanges=SaveTrackChanges;        // restore

             ResetUintFlag(TerOpFlags2,TOFLAG2_NO_NORMALIZE_FNOTE);

             HilightType=HILIGHT_OFF;

             NewType='D';                    // toggle for redo/undo
             break;

          case 'R':                          // reinstate the current character
             if ((NULL==(pUndo=GlobalLock(undo[idx].hText)))
               || (NULL==(pUndoCfmt=GlobalLock(undo[idx].hCfmt))) ) {
                 PrintError(w,MSG_OUT_OF_MEM,"Undo");
                 break;
             }
             pUndoLead=(mbcs?GlobalLock(undo[idx].hLead):NULL);

             PrevChar=pUndo[0];              // save
             PrevFmt=pUndoCfmt[0];
             PrevLead=mbcs?pUndoLead[0]:0;

             GlobalUnlock(undo[idx].hText);         // unlock undo memory
             GlobalUnlock(undo[idx].hCfmt);     // unlock undo memory
             if (mbcs) GlobalUnlock(undo[idx].hLead);

             SaveUndoRep(w,idx,BegLine,BegCol,EndLine,EndCol);  // toggle for next redo/undo

             // apply current redo/undo
             ptr=pText(CurLine);            // replace the character
             ptr[CurCol]=PrevChar;

             fmt=OpenCfmt(w,CurLine);          // replace the format
             fmt[CurCol]=PrevFmt;
             CloseCfmt(w,CurLine);

             if (mbcs) {
                lead=OpenLead(w,CurLine);
                lead[CurCol]=PrevLead;
                CloseLead(w,CurLine);
             }

             break;

          case 'D':
             if ((NULL==(pUndo=GlobalLock(undo[idx].hText)))
               || (undo[idx].hCfmt && (NULL==(pUndoCfmt=GlobalLock(undo[idx].hCfmt)))) ) {
                 PrintError(w,MSG_OUT_OF_MEM,"Undo");
                 break;
             }
             pUndoLead=(mbcs?GlobalLock(undo[idx].hLead):NULL);

             SaveTrackChanges=TrackChanges;
             TrackChanges=false;                 // turn-off track-changes so that inserted text is not altered

             if (undo[idx].hCfmt) {
                InsertBuffer(w,pUndo,NULL,pUndoLead,pUndoCfmt,NULL,TRUE);  // insert the buffer data at current position
                GlobalUnlock(undo[idx].hText);            // unlock undo memory
                GlobalUnlock(undo[idx].hCfmt);
                if (mbcs) GlobalUnlock(undo[idx].hLead);
             }
             else {                                  // restore from rtf buffer

                ClipTblLevel=undo[idx].TblLevel;
                ClipEmbTable=undo[idx].EmbTable;
                
                InsertRtfBuf(hTerWnd,(BYTE huge *)pUndo,GlobalSize(undo[idx].hText),CurLine,CurCol,FALSE);
                GlobalUnlock(undo[idx].hText);
                
                ClipTblLevel=1;
                ClipEmbTable=TRUE;
             }
             
             TrackChanges=SaveTrackChanges;     // restore

             NewType='I';                  // toggles to insert
             break;

          case 'F':                        // font changes
             hPrevFmt=undo[idx].hCfmt;      // save
             undo[idx].hCfmt=NULL;          // handle transferred
             SaveUndoFont(w,idx,BegLine,BegCol,EndLine,EndCol);  // toggle for next redo/undo

             // apply the previous font
             if (NULL==(pPrevFmt=GlobalLock(hPrevFmt))) {
                 PrintError(w,MSG_OUT_OF_MEM,"Undo");
                 break;
             }
             ChrIdx=0;

             for (l=BegLine;l<=EndLine;l++) {
               if (LineLen(l)==0) continue;
               if (l==BegLine) BegCl=BegCol;
               else            BegCl=0;
               if (l==EndLine) EndCl=EndCol;
               else            EndCl=LineLen(l)-1;

               fmt=OpenCfmt(w,l);
               for (i=BegCl;i<=EndCl;i++,ChrIdx++) fmt[i]=pPrevFmt[ChrIdx];
               CloseCfmt(w,l);
             }
             GlobalUnlock(hPrevFmt);
             GlobalFree(hPrevFmt);

             break;

          case 'P':                        // paragraph changes
             if (NULL==(pUndoPfmt=GlobalLock(undo[idx].hPfmt))) {
                 PrintError(w,MSG_OUT_OF_MEM,"Undo");
                 break;
             }

             TotalIds=pUndoPfmt[0];
             ParaIdx=1;

             for (l=BegLine;l<=EndLine;l++) {
                if (LineFlags(l)&LFLAG_PARA_FIRST || l==BegLine) {
                   UndoParaId=pUndoPfmt[ParaIdx];
                   pUndoPfmt[ParaIdx]=pfmt(l);  // set for redo

                   if (ParaIdx<TotalIds) ParaIdx++;
                }
                pfmt(l)=UndoParaId;
             }

             GlobalUnlock(undo[idx].hPfmt);

             break;

          case UNDO_ROW_DEL:          // undo of table row deletion
             if ((NULL==(pUndo=GlobalLock(undo[idx].hText)))
               || (undo[idx].hCfmt && (NULL==(pUndoCfmt=GlobalLock(undo[idx].hCfmt)))) ) {
                 PrintError(w,MSG_OUT_OF_MEM,"Undo");
                 break;
             }
             
             ClipTblLevel=undo[idx].TblLevel;
             ClipEmbTable=undo[idx].EmbTable;
                
             InsertRtfBuf(hTerWnd,(BYTE huge *)pUndo,GlobalSize(undo[idx].hText),CurLine,CurCol,FALSE);
             GlobalUnlock(undo[idx].hText);
                
             ClipTblLevel=1;
             ClipEmbTable=TRUE;

             NewType=UNDO_ROW_INS;                  // toggles to insert
             
             break;
          
          case UNDO_ROW_INS:                        // undo of table row insertion
             // save for redo for the first time, since SaveUndo does not save for 'insert'
             if (DoUndo) SaveUndoRowDel(w,idx,&BegLine,&EndLine);

             HilightType=HILIGHT_CHAR;       // prepare for BYTE block delete
             HilightBegRow=BegLine;
             HilightEndRow=EndLine;
             HilightBegCol=BegCol;
             HilightEndCol=EndCol+1;
             if (HilightEndCol>=LineLen(HilightEndRow) && (HilightEndRow+1)<TotalLines && cid(HilightEndRow)==cid(HilightEndRow+1)) {
                HilightEndRow++;
                HilightEndCol=0;
             }
             
             if (HilightBegRow==HilightEndRow && BegCol==EndCol) TerOpFlags2|=TOFLAG2_NO_NORMALIZE_FNOTE;
             DeleteCharBlock(w,FALSE,FALSE);       // delete the block, do not paint the screen
             ResetUintFlag(TerOpFlags2,TOFLAG2_NO_NORMALIZE_FNOTE);

             HilightType=HILIGHT_OFF;

             NewType=UNDO_ROW_DEL;                    // toggle for redo/undo
             break;

          case UNDO_FRAME:                          // undo of para frame object
          case UNDO_FRAME_LIMITED:                  // undo of para frame object - line array do not change
             frm=undo[idx].ObjId;                   //frame id
             if (frm>=0 && frm<TotalParaFrames) {
                struct StrParaFrame TempFrame;
                struct StrLinePtr far **LinePtrU;
                long   TotalLinesU,MaxLinesU,CursPos;

                // restore the line pointers
                if (undo[idx].type==UNDO_FRAME) {
                   LinePtrU=LinePtr;                        // save current
                   TotalLinesU=TotalLines;
                   MaxLinesU=MaxLines;
                   CursPos=RowColToAbs(w,CurLine,CurCol);

                   LinePtr=undo[idx].LinePtrU;             // resotre from undo
                   TotalLines=undo[idx].TotalLinesU;
                   MaxLines=undo[idx].MaxLinesU;
                   AbsToRowCol(w,undo[idx].CursPos,&CurLine,&CurCol);

                   undo[idx].LinePtrU=LinePtrU;            // save current to undo
                   undo[idx].TotalLinesU=TotalLinesU;
                   undo[idx].MaxLinesU=MaxLinesU;
                   undo[idx].CursPos=CursPos;
                }

                // restore the frame
                FarMove(&(ParaFrame[frm]),&TempFrame,sizeof(TempFrame));  // save the current object
                FarMove(undo[idx].pFrame,&(ParaFrame[frm]),sizeof(TempFrame));  // restore
                FarMove(&TempFrame,undo[idx].pFrame,sizeof(TempFrame));  // store for undo current

                if (ParaFrame[frm].pict>0) {    // set the associated picture type
                   int pict=ParaFrame[frm].pict;
                   if (pict>=0 && pict<TotalFonts) {
                      TerFont[pict].PictWidth=ParaFrame[frm].width;
                      TerFont[pict].PictHeight=ParaFrame[frm].height;
                      SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),(TerFont[pict].FrameType==PFRAME_NONE?FALSE:TRUE));
                      XlateSizeForPrt(w,pict);              // convert size to printer resolution

                   } 
                } 
                TerRepaginate(hTerWnd,true);
                //RequestPagination(w,true);
             } 
             break;
          case UNDO_PICT_SIZE:                          // undo picture size
             pict=undo[idx].ObjId;

             SwapInts(&TerFont[pict].PictWidth,&undo[idx].width);
             SwapInts(&TerFont[pict].PictHeight,&undo[idx].height);
             SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),(TerFont[pict].FrameType==PFRAME_NONE?FALSE:TRUE));
             XlateSizeForPrt(w,pict);              // convert size to printer resolution
             RepaintTer(w);
             break;
          
          case UNDO_TABLE_ATTRIB:                      // undo table attributes
             UndoTableAttrib(w,idx);
             break;
       }

       undo[idx].type=NewType;        // toggled type for undo/redo

       if (DoUndo) UndoCount--;
       else      UndoCount++;
    }

    InUndo=FALSE;

    if (CurLine>=TotalLines) CurLine--;
    if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) BeginLine=CurLine-WinHeight/2;            // position the current line at center

    if (BeginLine<0) BeginLine=0;
    CurRow=CurLine-BeginLine;

    PaintFlag=PAINT_WIN;                         // refresh from the text
    PaintTer(w);
    return TRUE;
}

/******************************************************************************
    UndoTableAttrib:
    Undo table attributes.
******************************************************************************/
UndoTableAttrib(PTERWND w, int idx)
{
    int i,row,cl;
    int          RowCount;            // number of rows in the undo row table
    int          CellCount;           // number of cells in the undo row table
    LPINT        RowId;               // row ids saved
    LPINT        CellId;              // cell ids saved
    struct StrTableRow *pRow;         // undo row table - stores the modified row (MemAlloc)
    struct StrCell     *pCell;        // undo cell table                          (MemAlloc)

    if (undo[idx].RowCount==0 || undo[idx].CellCount==0) return false;

    // ensure that table row chain is intact
    for (i=0;i<undo[idx].RowCount;i++) {
       row=undo[idx].RowId[i];
       if (TableRow[row].PrevRow!=undo[idx].pRow[i].PrevRow) return false;
       if (TableRow[row].NextRow!=undo[idx].pRow[i].NextRow) return false;
       if (TableRow[row].FirstCell!=undo[idx].pRow[i].FirstCell) return false;
    } 

    // ensure that table cell chain is intact
    for (i=0;i<undo[idx].CellCount;i++) {
       cl=undo[idx].CellId[i];
       if (cell[cl].PrevCell!=undo[idx].pCell[i].PrevCell) return false;
       if (cell[cl].NextCell!=undo[idx].pCell[i].NextCell) return false;
    } 

    // save the current information for redo
    RowCount=undo[idx].RowCount;
    CellCount=undo[idx].CellCount;
    
    RowId=MemAlloc(sizeof(int)*(RowCount+1));                        // allocate memory
    pRow=MemAlloc(sizeof(struct StrTableRow)*(RowCount+1));
    CellId=MemAlloc(sizeof(int)*(CellCount+1));
    pCell=MemAlloc(sizeof(struct StrCell)*(CellCount+1));

    for (i=0;i<RowCount;i++) {                                       // save current row
       row=RowId[i]=undo[idx].RowId[i];
       FarMove(&(TableRow[row]),&(pRow[i]),sizeof(struct StrTableRow));
    } 
    for (i=0;i<CellCount;i++) {                                       // save current row
       cl=CellId[i]=undo[idx].CellId[i];
       FarMove(&(cell[cl]),&(pCell[i]),sizeof(struct StrCell));
    } 

    // do undo
    for (i=0;i<undo[idx].RowCount;i++) {
       row=undo[idx].RowId[i];        // row id that was saved
       FarMove(&(undo[idx].pRow[i]),&(TableRow[row]),sizeof(struct StrTableRow));
    } 
    for (i=0;i<undo[idx].CellCount;i++) {
       cl=undo[idx].CellId[i];        // row id that was saved
       FarMove(&(undo[idx].pCell[i]),&(cell[cl]),sizeof(struct StrCell));
    } 
 
    // free this undo information and save the redo information
    MemFree(undo[idx].RowId);
    MemFree(undo[idx].pRow);
    MemFree(undo[idx].CellId);
    MemFree(undo[idx].pCell);

    undo[idx].RowId=RowId;
    undo[idx].CellId=CellId;
    undo[idx].pRow=pRow;
    undo[idx].pCell=pCell;

    RequestPagination(w,true);
    RefreshFrames(w,true);       // to rebuild the 'aux' structure for the row and cells
    
    return true;
} 

/******************************************************************************
    GetTextRange:
    Get the range of the selected text.
******************************************************************************/
GetTextRange(PTERWND w, LPLONG pBegRow, LPINT pBegCol, LPLONG pEndRow, LPINT pEndCol)
{
    long BegRow,EndRow;
    int  BegCol,EndCol;

    if (HilightType==HILIGHT_LINE) {         // line block
        NormalizeBlock(w);                   // normalize beginning and ending of the block
        BegRow=HilightBegRow;
        BegCol=0;
        EndRow=HilightEndRow;
        EndCol=LineLen(EndRow);
        if (EndCol<0) EndCol=0;
    }
    else if (HilightType==HILIGHT_CHAR) {  // character block
        NormalizeBlock(w);                   // normalize beginning and ending of the block
        BegRow=HilightBegRow;
        BegCol=HilightBegCol;
        if (BegCol>=LineLen(BegRow)) BegCol=LineLen(BegRow)-1;
        if (BegCol<0) BegCol=0;

        EndRow=HilightEndRow;
        EndCol=HilightEndCol;
        if (EndCol>LineLen(EndRow)) EndCol=LineLen(EndRow);
        if (EndCol<0) EndCol=0;
        if (BegRow==EndRow && BegCol>EndCol) BegCol=EndCol;
    }
    else {                                  // if text not highlighted, check the entire file
        BegRow=0;
        BegCol=0;
        EndRow=TotalLines-1;
        if (EndRow<0) EndRow=0;
        EndCol=LineLen(EndRow);
        if (EndCol<0) EndCol=0;
    }

    if (pBegRow) (*pBegRow)=BegRow;
    if (pEndRow) (*pEndRow)=EndRow;
    if (pBegCol) (*pBegCol)=BegCol;
    if (pEndCol) (*pEndCol)=EndCol;

    return TRUE;
}

/******************************************************************************
    EditingInputField:
    This function returns TRUE when we are in the input field edit mode.
******************************************************************************/
BOOL EditingInputField(PTERWND w, BOOL del, BOOL bksp)
{
    int CurFont,col;
    long line;

    if (!TerArg.ReadOnly) return FALSE;  // field edit mode is valid in read-only mode

    if (HilightType==HILIGHT_OFF) {
       if (del || bksp) {         // don't let the last field character be deleted
         BOOL PrevCharIsField,NextCharIsField;
         long InitLine=CurLine;
         int  InitCol=CurCol;

         if (bksp) PrevTextPos(w,&InitLine,&InitCol);      // locate the actual character being deleted
         
         line=InitLine;                  // check the previous character
         col=InitCol;
         PrevTextPos(w,&line,&col);
         CurFont=GetCurCfmt(w,line,col);
         PrevCharIsField=(TerFont[CurFont].FieldId==FIELD_TEXTBOX);
       
         line=InitLine;                // check the next character
         col=InitCol;
         NextTextPos(w,&line,&col);
         CurFont=GetCurCfmt(w,line,col);
         NextCharIsField=(TerFont[CurFont].FieldId==FIELD_TEXTBOX);

         if (!PrevCharIsField && !NextCharIsField) return FALSE;
       } 

       
       CurFont=GetCurCfmt(w,CurLine,CurCol);
       if (del && TerFont[CurFont].FieldId!=FIELD_TEXTBOX) return FALSE;   // text box can be filled in the read only mode
       if (!bksp && TerFont[CurFont].FieldId==FIELD_TEXTBOX) return TRUE;   // text box can be filled in the read only mode

       // check the previous char
       line=CurLine;
       col=CurCol;
       PrevTextPos(w,&line,&col);
       CurFont=GetCurCfmt(w,line,col);
       if (bksp && TerFont[CurFont].FieldId!=FIELD_TEXTBOX) return false;
       return (TerArg.ReadOnly && TerFont[CurFont].FieldId==FIELD_TEXTBOX);   // the cursor is past the field, but let the field extend in fill-mode
    }

    NormalizeBlock(w);
    
    // check the first column
    CurFont=GetCurCfmt(w,HilightBegRow,HilightBegCol);
    if (TerFont[CurFont].FieldId!=FIELD_TEXTBOX) return FALSE;

    // check the last column
    line=HilightEndRow;
    col=HilightEndCol;
    PrevTextPos(w,&line,&col);    // hilight end pos is exclusive
    CurFont=GetCurCfmt(w,line,col);
    if (TerFont[CurFont].FieldId!=FIELD_TEXTBOX) return FALSE;

    if (del || bksp) {   // don't allow the field to be deleted
       CurFont=GetCurCfmt(w,HilightEndRow,HilightEndCol);
       if (TerFont[CurFont].FieldId!=FIELD_TEXTBOX) return FALSE;
    }
     
    return TRUE;
}

/******************************************************************************
    EditInputField:
    Edit input field name and Retrieve the input field information such as name and border. 
******************************************************************************/
EditInputField(PTERWND w)
{
    int pict=CurInputField;
    struct StrForm far *pForm;
    BYTE typeface[33],MaxLenStr[10];

    if (IsFormField(w,pict,0)) {
       pForm=(LPVOID)TerFont[pict].pInfo;
    
       lstrcpy(DlgText1,pForm->name);
       lstrcpy(TempString,pForm->typeface);

       DlgInt1=pForm->MaxLen;
       DlgInt2=(pForm->style&WS_BORDER)?TRUE:FALSE;
       DlgInt3=pForm->TwipsSize;
       DlgInt4=pForm->FontStyle;
       DlgInt6=TerFont[pict].FieldId;
       DlgLong=(DWORD)pForm->TextColor;
    }
    else {
       int font=GetCurCfmt(w,CurLine,CurCol);
       pict=TerFont[font].AuxId;
         
       if (FindTextInputField(w,pict)) {
          GetStringField(TerFont[font].FieldCode,1,'|',DlgText1);  // second field is name
          strcpy(TempString,TerFont[font].TypeFace);

          DlgInt1=atoi(GetStringField(TerFont[font].FieldCode,0,'|',MaxLenStr));
          DlgInt2=True(TerFont[font].style&CHAR_BOX);
          DlgInt3=TerFont[font].TwipsSize;
          
          DlgInt4=(int)TerFont[font].style;
          DlgInt4=ResetFlag(DlgInt4,CHAR_BOX);  // this style is set by the border parameter

          DlgInt6=TerFont[font].FieldId;
          DlgLong=TerFont[font].TextColor;
       }
       else return false; 
    }
    
    
    if (!CallDialogBox(w,"EditInputFieldParam",EditInputFieldParam,0L)) return FALSE;

    lstrcpy(typeface,TempString);  // store to the local variable

    TerSetInputFieldInfo(hTerWnd,pict,DlgText1,DlgInt2);
    TerSetTextFieldInfo(hTerWnd,pict,NULL,DlgInt1,0,typeface,DlgInt3,DlgInt4);

    return TRUE;
}

/******************************************************************************
    TerGetInputFieldInfo:
    Retrieve the input field information such as name and border. 
******************************************************************************/
BOOL WINAPI _export TerGetInputFieldInfo(HWND hWnd, int pict, LPBYTE name, LPINT type, LPINT border)
{
    PTERWND w;
    struct StrForm far *pForm;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (IsFormField(w,pict,0)) {
       pForm=(LPVOID)TerFont[pict].pInfo;
    
       if (name) lstrcpy(name,pForm->name);
       if (type) (*type)=TerFont[pict].FieldId;
       if (border) (*border)=((pForm->style&WS_BORDER)?1:0);

       return true;
    }
    else {
       if (FindTextInputField(w,pict)) {
          int CurFont=GetCurCfmt(w,CurLine,CurCol);
          if (type)   (*type)=FIELD_TEXTBOX;
          if (name)   GetStringField(TerFont[CurFont].FieldCode,1,'|',name);
          if (border) (*border)=True(TerFont[CurFont].style&CHAR_BOX);
          return true;
       } 
    } 


    return FALSE;
}

/******************************************************************************
    TerGetCheckboxInfo:
    Retrieve the checkbox information
******************************************************************************/
BOOL WINAPI _export TerGetCheckboxInfo(HWND hWnd, int pict, LPINT checked)
{
    PTERWND w;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!IsFormField(w,pict,FIELD_CHECKBOX)) return FALSE;

    if (checked) (*checked)=(BOOL)SendMessage(TerFont[pict].hWnd,BM_GETCHECK,0,0L);
    
    return TRUE;
}

/******************************************************************************
    TerGetTextFieldInfo:
    Retrieve the text input field information such as data, length, font etc.
******************************************************************************/
BOOL WINAPI _export TerGetTextFieldInfo(HWND hWnd, int pict, LPBYTE data, LPINT MaxChars, LPINT width, LPBYTE typeface, LPINT TwipsSize, LPINT style)
{
    PTERWND w;
    int font;
    BYTE MaxLenStr[10];
    HGLOBAL hMem;
    DWORD size;
    LPBYTE pMem;
           
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!FindTextInputField(w,pict)) return false;
      
    font=GetCurCfmt(w,CurLine,CurCol);
      
    GetStringField(TerFont[font].FieldCode,0,'|',MaxLenStr);  // first field is max-len  
    if (MaxChars) (*MaxChars)=atoi(MaxLenStr);
    
    if (typeface)  strcpy(typeface,TerFont[font].TypeFace);
    if (TwipsSize) (*TwipsSize)=TerFont[font].TwipsSize;
    if (style)     (*style)=TerFont[font].style;

    if (data) {
       SelectTextInputField(w,false);          // select the current field
       hMem=TerGetTextSel(hTerWnd,&size);
       pMem=GlobalLock(hMem);
       FarMove(pMem,data,size+1);
       GlobalUnlock(hMem);
       GlobalFree(hMem);

       DeselectTerText(hTerWnd,false);
    }

    return TRUE;
}

/******************************************************************************
    TerSetInputFieldInfo:
    Set new name and other common information for the field.
******************************************************************************/
BOOL WINAPI _export TerSetInputFieldInfo(HWND hWnd, int pict, LPBYTE name, int border)
{
    PTERWND w;
    struct StrForm far *pForm;
    BYTE  MaxLenStr[10],OldName[MAX_WIDTH+1],NewCode[MAX_WIDTH+1],CurName[MAX_WIDTH+1];
    int font,NextFont,FieldLine,FieldCol;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (IsFormField(w,pict,0)) goto SET_CONTROL_FIELD;

    // set the text input info
    if (!FindTextInputField(w,pict))  return false;  // find the text input field by its id
    FieldLine=CurLine;
    FieldCol=CurCol;
      
    font=GetCurCfmt(w,CurLine,CurCol);
    if (TerFont[font].FieldId!=FIELD_TEXTBOX || TerFont[font].FieldCode==null || TerFont[font].AuxId!=pict) return false;
      
    GetStringField(TerFont[font].FieldCode,0,'|',MaxLenStr);  // first field is max len field
    GetStringField(TerFont[font].FieldCode,1,'|',OldName);
    wsprintf(NewCode,"%s|%s",MaxLenStr,name);

    while (true ) {             // modify field code
       TerFont[font].FieldCode=OurAlloc(strlen(NewCode)+1);
       strcpy(TerFont[font].FieldCode,NewCode);

       if (CurLine==TotalLines && CurCol==LineLen(CurLine)-1) break;
         
       // advance the next character 
       NextTextPos(w,&CurLine,&CurCol);
       NextFont=GetCurCfmt(w,CurLine,CurCol);
       if (NextFont==font) continue;   // this font already done
         
       GetStringField(TerFont[font].FieldCode,1,'|',CurName);

       font=NextFont;
       if (TerFont[font].FieldId!=FIELD_TEXTBOX || TerFont[font].AuxId!=pict || strcmp(CurName,OldName)!=0) break;
    } 

    // set/reset the border
    CurLine=FieldLine;
    CurCol=FieldCol;
    SelectTextInputField(w,false);
    SetTerCharStyle(hTerWnd,CHAR_BOX,border,true);

    TerArg.modified++;

    return true;


    SET_CONTROL_FIELD:
    pForm=(LPVOID)TerFont[pict].pInfo;

    if (name) lstrcpy(pForm->name,name);

    if (TerFont[pict].FieldId==FIELD_TEXTBOX) {
       if (((pForm->style&WS_BORDER) && !border) || (!(pForm->style&WS_BORDER) && border)) {
          ResetLongFlag(pForm->style,WS_BORDER);
          if (border) pForm->style|=WS_BORDER;
       
          TerSetWinBorder(TerFont[pict].hWnd,border?1:0);
       }  
    }
    
    TerArg.modified++;
    
    return TRUE;       
}
        
/******************************************************************************
    TerSetCheckboxInfo:
    Set the checkbox information.
******************************************************************************/
BOOL WINAPI _export TerSetCheckboxInfo(HWND hWnd, int pict, BOOL checked)
{
    PTERWND w;
    struct StrForm far *pForm;
    BOOL CurChecked;
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!IsFormField(w,pict,FIELD_CHECKBOX)) return FALSE;

    pForm=(LPVOID)TerFont[pict].pInfo;
        
    CurChecked=(BOOL)SendMessage(TerFont[pict].hWnd,BM_GETCHECK,0,0L);

    if ((CurChecked && !checked) || (!CurChecked && checked)) {  // status changing
        SendMessage(TerFont[pict].hWnd,BM_SETCHECK,checked,0L);
    }    
    TerArg.modified++;
    
    return TRUE;
}

/******************************************************************************
    TerSetTextFieldInfo:
    Set the text input field information such as data, length, font etc.
******************************************************************************/
BOOL WINAPI _export TerSetTextFieldInfo(HWND hWnd, int pict, LPBYTE data, int MaxChars, int reserved, LPBYTE typeface, int TwipsSize, UINT style)
{
    PTERWND w;
    int  NextFont,BegPos,EndPos,FieldLine,FieldCol,FieldMaxLen,font;
    BOOL HasBorder;
    BYTE MaxLenStr[10],name[MAX_WIDTH+1],NewCode[MAX_WIDTH+1],CurName[MAX_WIDTH+1];
           
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!FindTextInputField(w,pict)) return false;
    FieldLine=CurLine;           // field start position
    FieldCol=CurCol;
    font=GetCurCfmt(w,CurLine,CurCol);
    HasBorder=True(TerFont[font].style&CHAR_BOX);

    // apply MaxLen
    FieldMaxLen=atoi(GetStringField(TerFont[font].FieldCode,0,'|',MaxLenStr));  // first field in the string is MaxLen

    if (FieldMaxLen!=MaxChars) {
      GetStringField(TerFont[font].FieldCode,1,'|',name);
      wsprintf(NewCode,"%d|%s",MaxChars,name);

      while (true ) {             // modify field code
         TerFont[font].FieldCode=OurAlloc(strlen(NewCode)+1);
         lstrcpy(TerFont[font].FieldCode,NewCode);

         if (CurLine==(TotalLines-1) && CurCol==(LineLen(CurLine)-1)) break;
         
         // advance the next character 
         NextTextPos(w,&CurLine,&CurCol);
         NextFont=GetCurCfmt(w,CurLine,CurCol);
         if (NextFont==font) continue;   // this font already done
         
         GetStringField(TerFont[font].FieldCode,1,'|',CurName);

         font=NextFont;
         if (TerFont[font].FieldId!=FIELD_TEXTBOX || TerFont[font].AuxId!=pict || strcmp(CurName,name)!=0) break;
      } 
    }

    // text range to apply font modification to
    CurLine=FieldLine;
    CurCol=FieldCol;
    SelectTextInputField(w,false);
    BegPos=RowColToAbs(w,HilightBegRow,HilightBegCol);
    EndPos=RowColToAbs(w,HilightEndRow,HilightEndCol);
        
    // apply font modification
    SetTerFont(hTerWnd,typeface,false);
    SetTerPointSize(hTerWnd,-TwipsSize,false);

    // set styles
    SetTerCharStyle(hTerWnd,BOLD|ITALIC|ULINE|ULINED|STRIKE|SUBSCR|SUPSCR|CHAR_BOX,false,false);  // remove old style
    if (HasBorder) style|=CHAR_BOX;
    SetTerCharStyle(hTerWnd,style,true,false);  // set new styles

    if (data!=null) {    // Apply the new data
       ReplaceTextString(w,data,null,BegPos,EndPos-1);  // specify inclusive end position
    }    

    ReleaseUndo(w);
    TerArg.modified++;

    DeselectTerText(hTerWnd,true);  // deselect and repaint

    return TRUE;
}


/******************************************************************************
    TerLocateInputField:
    This function locates the input fields. The loc can be TER_FIRST, TER_LAST,
    TER_NEXT, or TER_PREV.
    When the field is located, the cursor is placed before the field control
    and the function returns the field id of the field.  This function does not
    update the screen.
******************************************************************************/
int WINAPI _export TerLocateInputField(HWND hWnd, int loc, BOOL repaint)
{
    PTERWND w;
    long LineNo;
    int  CurFont,col,id;
    BOOL found=false,SkipCurrent=false;
       
    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    CurFont=GetCurCfmt(w,CurLine,CurCol);

    if (loc==TER_CUR) {
       if (TerFont[CurFont].FieldId==FIELD_TEXTBOX) 
             return TerFont[CurFont].AuxId;
       else  return CurInputField;
    }

    LineNo=CurLine;
    col=CurCol;
    if (loc==TER_FIRST) {
      LineNo=0;
      col=-1;
      loc=TER_NEXT;
    }
    else if (loc==TER_LAST) {
      LineNo=TotalLines-1;
      col=LineLen(LineNo)-1;
      loc=TER_PREV;
    }    
    else if (loc==TER_NEXT && TerFont[CurFont].FieldId==FIELD_TEXTBOX) SkipCurrent=true;
    else if (loc==TER_PREV && TerFont[CurFont].FieldId==FIELD_TEXTBOX) SkipCurrent=true;

    // locate the input field
    while (TRUE) {
       if (loc==TER_NEXT && !NextTextPos(w,&LineNo,&col)) return 0;
       if (loc==TER_PREV && !PrevTextPos(w,&LineNo,&col)) return 0;
    
       id=GetCurCfmt(w,LineNo,col);

       if (SkipCurrent && TerFont[id].FieldId==FIELD_TEXTBOX) continue;  // skip over the current field
       SkipCurrent=false;
         
       if (IsFormField(w,id,0)) {
          found=true;
          break;
       }
       if (TerFont[id].FieldId==FIELD_TEXTBOX) {
          id=TerFont[id].AuxId;
          found=true;
          break;
       }

    } 

    SetTerCursorPos(hTerWnd,LineNo,col,repaint);
    
    return (found?id:0);
}

/******************************************************************************
    FindTextInputField:
    find the text input field by its field id. The cursor is placed on the
    the first character of the field when the field is found.
******************************************************************************/
BOOL FindTextInputField(PTERWND w,int id)
{
   int line=0;
   int col=0,font;

   while (TerLocateFieldChar(hTerWnd,FIELD_TEXTBOX,null,true,&line, &col, true)) {
      font=GetCurCfmt(w,line,col);
      if (TerFont[font].AuxId==id) {
         CurLine=line;
         CurCol=col;
         return true;
      }
      if (!TerLocateFieldChar(hTerWnd,FIELD_TEXTBOX,TerFont[font].FieldCode,false,&line, &col, true)) return false; // go to the end of the field
   } 

   return false;
}

/******************************************************************************
    SelectTextInputField:
    Select the current text input field
******************************************************************************/
BOOL SelectTextInputField(PTERWND w, BOOL repaint)
{
   return SelectTextInputField2(w,CurLine,CurCol,repaint);
}
 
BOOL SelectTextInputField2(PTERWND w, long InitLine, int InitCol, BOOL repaint)
{
   int font,line,col;

   font=GetCurCfmt(w,InitLine,InitCol);
   if (TerFont[font].FieldId!=FIELD_TEXTBOX) return false;  // not positioned on a text input field

   // find the first character of the field
   line=InitLine;
   col=InitCol;
   if (!TerLocateFieldChar(hTerWnd,FIELD_TEXTBOX,TerFont[font].FieldCode,false,&line, &col, false)) {  // scan backward for the absense of the field
      HilightBegRow=HilightBegCol=0;
   }
   else {
      NextTextPos(w,&line,&col);
      HilightBegRow=line;
      HilightBegCol=col;
   } 

   // find the character after the field
   line=InitLine;
   col=InitCol;
   if (!TerLocateFieldChar(hTerWnd,FIELD_TEXTBOX,TerFont[font].FieldCode,false,&line, &col, true)) {  // scan forward for the absense of the field
      HilightEndRow=TotalLines-1;
      HilightEndCol=LineLen(TotalLines-1);
   }
   else {
      HilightEndRow=line;
      HilightEndCol=col;
   } 

   HilightType=HILIGHT_CHAR;
   StretchHilight=false;

   if (repaint) PaintTer(w);

   return true;
}
      


/******************************************************************************
    TerLocateFontId:
    This function searches for the font id in the file and returns TRUE if the font
    id is in use.  The pLinNo, and pCol provide the initial search position.  These 
    variables are update if the font is found.  Specify -1 for (*pLineNo) to assume 
    the current cursor location.
******************************************************************************/
BOOL WINAPI _export TerLocateFontId(HWND hWnd,int FontId, LPLONG pLineNo, LPINT pCol)
{
    long line;
    int  col;
    PTERWND w;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (!pLineNo || !pCol) return FALSE;

    line=(*pLineNo);
    col=(*pCol);

    if (line<0 || line>=TotalLines) {
       line=CurLine;
       col=CurCol;
    }
    if (col>=LineLen(line)) col=LineLen(line)-1;

    for (;line<TotalLines;line++,col=0) {
       if (LineLen(line)==0) continue;
       if (cfmt(line).info.type==UNIFORM) {
         if (cfmt(line).info.fmt==(WORD)FontId) goto FOUND;
       }
       else {
         fmt=cfmt(line).pFmt;
         for (;col<LineLen(line);col++) if (fmt[col]==(WORD)FontId) goto FOUND;
       }
    }    
    return FALSE;

    FOUND:
    
    (*pLineNo)=line;
    (*pCol)=col;
    return TRUE;
}    
/******************************************************************************
TerInsertTextInputField:
Insert a text input field (formfield)        
******************************************************************************/
int WINAPI _export TerInsertTextInputFieldEx(HWND hWnd,LPBYTE name, LPBYTE data, int MaxLen, BOOL border, 
                                           LPBYTE pTypeface, int TwipsSize, UINT TextStyle, COLORREF TextColor, BOOL insert, UINT typetxt, 
                                           HWND* phWnd, LPBYTE mask, LPBYTE class, BOOL repaint)
{
  PTERWND w;
  BYTE typeface[33];
  LPBYTE  ptr;
  LPWORD fmt;
  int    pict,CurCfmt,width,height,len;
  DWORD InfoSize;
  struct StrForm far *pForm;
  BOOL   ReplacePicture;
  DWORD style;
  HFONT hOldFont;
  TEXTMETRIC tm;
  int SysCharWidth,SysCharHeight,id=0;

  if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

  if (!CanInsertTextObject(w,CurLine,CurCol)) return FALSE;

  if (!name || lstrlen(name)==0) {
    if (!CallDialogBox(w,"InputFieldParam",InputFieldParam,0L)) return FALSE;
    name=DlgText1;
    data=DlgText2;
    lstrcpy(typeface,TempString);
    pTypeface=typeface;
    MaxLen=DlgInt1;
    border=DlgInt2;
    TwipsSize=DlgInt3;
    TextStyle=DlgInt4;
    TextColor=(COLORREF)DlgLong;

    if (lstrlen(name)==0) return FALSE;
  }

  // check if the cursor is on the protected text
  ReplacePicture=(NextFontId>=0?TRUE:FALSE);
  CurCfmt=GetCurCfmt(w,CurLine,CurCol);

  // find an open slot in the font table
  if ((pict=FindOpenSlot(w))==-1) return 0;    // ran out of picture table

  // Set control parameters
  style=WS_CHILD|WS_CLIPSIBLINGS|ES_AUTOHSCROLL;    // force child window
  if (border) style=style|WS_BORDER;

  // store the control information
  InfoSize=sizeof(struct StrForm);
  if  (NULL==(TerFont[pict].pInfo=OurAlloc(InfoSize)) ){
    PrintError(w,MSG_OUT_OF_MEM,"TerInsertTextInputField");
    InitTerObject(w,pict);
    goto EOR;
  }

  pForm=(LPVOID)TerFont[pict].pInfo;
  FarMemSet(pForm,0,sizeof(struct StrForm));   // initialize the structure

  // MAK
  // set the class and style
  if(class && strlen(class))
    lstrcpy(pForm->class, class);
  else
    lstrcpy(pForm->class,"Edit");
  // MAK		
  pForm->typetxt=typetxt; // normal Text
  if(mask && strlen(mask))
    lstrcpy(pForm->maskdata, mask);

  lstrcpy(pForm->name,name);
  lstrcpy(pForm->InitText,data);
  pForm->style=style;
  pForm->MaxLen=MaxLen+1;
  pForm->id=id;

  // set the height/width
  hOldFont=SelectObject(hTerDC,GetStockObject(SYSTEM_FIXED_FONT));

  GetTextMetrics(hTerDC,&tm);
  SelectObject(hTerDC,hOldFont);
  SysCharHeight=tm.tmHeight;
  SysCharWidth=tm.tmAveCharWidth;

  if (MaxLen) len=MaxLen;
  else        len=10;         // default character width

  width=ScrToTwipsX(SysCharWidth*len);
  height=ScrToTwipsY(SysCharHeight);
  if (border) height=height*5/4;


  // save the font information
  pForm->FontId=-1;             // font id not yet created
  lstrcpy(pForm->typeface,pTypeface);
  lstrcpy(pForm->family,"");
  pForm->TwipsSize=TwipsSize;
  pForm->TextColor=TextColor;
  if (border) pForm->TextBkColor=0xFFFFFF;
  else        pForm->TextBkColor=PageBorderColor;

  pForm->FontStyle=TextStyle;
  pForm->CharSet=DEFAULT_CHARSET;


  // Erase any previously highlighted text
  if (HilightType==HILIGHT_CHAR) TerDeleteBlock(hTerWnd,TRUE);
  UndoRef--;                    // restore the undo reference

  //******** initialize other TerFont variables **********
  TerFont[pict].InUse=TRUE;
  TerFont[pict].PictType=PICT_FORM;
  TerFont[pict].FieldId=FIELD_TEXTBOX;
  TerFont[pict].ObjectType=OBJ_NONE;
  TerFont[pict].PictHeight=height;  // dimension in twips
  TerFont[pict].PictWidth=width;
  TerFont[pict].ImageSize=0;
  TerFont[pict].InfoSize=InfoSize;
  TerFont[pict].style=PICT;
  TerFont[pict].PictAlign=ALIGN_BOT;
  TerFont[pict].AuxId=id;                          // user assigned id


  // Create the control
  if (!RealizeControl(w,pict)) {
    InitTerObject(w,pict);
    goto EOR;
  }

  lstrcpy(pForm->class,"Edit");
  if(phWnd) 
    *phWnd = TerFont[pict].hWnd;

  XlateSizeForPrt(w,pict);              // convert size to printer resolution

  //*************** insert the picture at the current cursor location ***
  if (insert && !ReplacePicture) {
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine);
    if (CurCol<0) CurCol=0;

    MoveLineData(w,CurLine,CurCol,1,'B');   // make extra space

    ptr=pText(CurLine);
    ptr[CurCol]=PICT_CHAR;                      // any character


    fmt=OpenCfmt(w,CurLine);
    fmt[CurCol]=(WORD)pict;               // picture index serves as the formatting attribute
    CloseCfmt(w,CurLine);
  }

EOR:                                  // end of routine
  PaintFlag=PAINT_WIN;                 // refresh from the text
  if (insert || ReplacePicture) PaintTer(w);   // suppress word-wrap when not inserting into text here

  return pict;
}


/******************************************************************************
    TerInsertTextInputField:
    Insert a text input field (formfield)   
MAK: changes     
******************************************************************************/
//int WINAPI _export TerInsertTextInputField(HWND hWnd,LPBYTE name, LPBYTE data, int MaxLen,
//   BOOL border, LPBYTE pTypeface, int TwipsSize, UINT TextStyle, COLORREF TextColor, BOOL reserved, BOOL repaint)
//
int WINAPI _export TerInsertTextInputField(HWND hWnd,LPBYTE name, LPBYTE data, int MaxLen, BOOL border, 
                                           LPBYTE pTypeface, int TwipsSize, UINT TextStyle, COLORREF TextColor, BOOL insert, UINT typetxt, 
                                           HWND* phWnd, LPBYTE mask, LPBYTE class, BOOL repaint)
{
    PTERWND w;
    BYTE typeface[33],str[MAX_WIDTH+1];
    int i,id,SaveUndo,font;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!CanInsertTextObject(w,CurLine,CurCol)) return FALSE;

    if (!name || lstrlen(name)==0) {
       if (!CallDialogBox(w,"InputFieldParam",InputFieldParam,0L)) return FALSE;
       name=DlgText1;
       data=DlgText2;
       lstrcpy(typeface,TempString);
       pTypeface=typeface;
       MaxLen=DlgInt1;
       border=DlgInt2;
       TwipsSize=DlgInt3;
       TextStyle=DlgInt4;
       TextColor=(COLORREF)DlgLong;

       if (lstrlen(name)==0) return FALSE;
    }

    // create the new font id
    if (border) TextStyle|=CHAR_BOX;
    font=TerCreateFont(hWnd,-1,false,pTypeface,TwipsSize/20,TextStyle,TextColor,CLR_WHITE,FIELD_TEXTBOX,0);
    if (font<0) return 0;
      
    // apply the field code to the font
    wsprintf(str,"%d|%s",MaxLen,name);     
    TerFont[font].FieldCode=OurAlloc(strlen(str)+1);
    strcpy(TerFont[font].FieldCode,str); // we can assign FieldCode because this font is created as non-shared
    
    // assigne the field id
    id=1;   // Aux id works like field id
    for (i=0;i<TotalFonts;i++) {
      if (i!=font && TerFont[i].FieldId==FIELD_TEXTBOX && TerFont[i].AuxId>=id) id=TerFont[i].AuxId+1;
    } 
    TerFont[font].AuxId=id;    // use as the id for the field

    // Erase any previously highlighted text
    SaveUndo=UndoRef;
    if (HilightType==HILIGHT_CHAR) TerDeleteBlock(hWnd,true);
    UndoRef=SaveUndo;                    // restore the undo reference

    TerInsertText(hWnd,data,font,-1,repaint);

    return font;

}

/******************************************************************************
    TerInsertCheckBoxField:
    Insert a text input field (formfield)
******************************************************************************/
int WINAPI _export TerInsertCheckBoxField(HWND hWnd,LPBYTE name, int TwipsSize, BOOL checked, BOOL insert, BOOL repaint)
{
    PTERWND w;
    LPBYTE  ptr;
    LPWORD fmt;
    int    pict,CurCfmt,width,height;
    DWORD InfoSize;
    struct StrForm far *pForm;
    BOOL   ReplacePicture;
    DWORD style;
    int  id=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!CanInsertTextObject(w,CurLine,CurCol)) return FALSE;

    if (!name || lstrlen(name)==0) {
       if (!CallDialogBox(w,"CheckBoxFieldParam",CheckBoxFieldParam,0L)) return FALSE;
       name=DlgText1;
       TwipsSize=DlgInt1;
       checked=DlgInt2;
       if (lstrlen(name)==0) return FALSE;
    }

    // check if the cursor is on the protected text
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;    // ran out of picture table

    // Set control parameters
    style=WS_CHILD|WS_CLIPSIBLINGS|WS_BORDER|BS_AUTOCHECKBOX;    // force child window

    // store the control information
    InfoSize=sizeof(struct StrForm);
    if  (NULL==(TerFont[pict].pInfo=OurAlloc(InfoSize)) ){
          PrintError(w,MSG_OUT_OF_MEM,"TerInsertTextInputField");
          InitTerObject(w,pict);
          goto EOR;
    }

    pForm=(LPVOID)TerFont[pict].pInfo;
    FarMemSet(pForm,0,sizeof(struct StrForm));   // initialize the structure

    // set the class and style
    lstrcpy(pForm->class,"Button");
    lstrcpy(pForm->name,name);
    pForm->style=style;
    pForm->CheckBoxSize=TwipsSize;
    pForm->InitNum=checked;
    pForm->id=id;

    // set the height/width
    width=height=TwipsSize;

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    //******** initialize other TerFont variables **********
    TerFont[pict].InUse=TRUE;
    TerFont[pict].PictType=PICT_FORM;
    TerFont[pict].FieldId=FIELD_CHECKBOX;
    TerFont[pict].ObjectType=OBJ_NONE;
    TerFont[pict].PictHeight=height;  // dimension in twips
    TerFont[pict].PictWidth=width;
    TerFont[pict].ImageSize=0;
    TerFont[pict].InfoSize=InfoSize;
    TerFont[pict].style=PICT;
    TerFont[pict].PictAlign=ALIGN_MIDDLE;
    TerFont[pict].AuxId=id;                          // user assigned id

    // Create the control
    if (!RealizeControl(w,pict)) {
       InitTerObject(w,pict);
       goto EOR;
    }

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine);
       if (CurCol<0) CurCol=0;

       MoveLineData(w,CurLine,CurCol,1,'B');   // make extra space

       ptr=pText(CurLine);
       ptr[CurCol]=PICT_CHAR;                      // any character


       fmt=OpenCfmt(w,CurLine);
       fmt[CurCol]=(WORD)pict;               // picture index serves as the formatting attribute
       CloseCfmt(w,CurLine);
    }

    EOR:                                  // end of routine
    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);   // suppress word-wrap when not inserting into text here

    return pict;
}

/******************************************************************************
    TerInsertControl:
    This routine inserts a control of the specified class into text.
    if 'insert' argument is TRUE, It inserts the control at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the control alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    The 'width' and 'height' for the control are specified in twips unit.
    This function returns the picture id if successful. Otherwise it returns 0.
*******************************************************************************/
int WINAPI _export TerInsertControl(HWND hWnd,LPBYTE class, DWORD style, int width, int height, int align, int id, BOOL insert)
{
    LPBYTE  ptr;
    LPWORD fmt;
    int    pict,CurCfmt;
    DWORD InfoSize;
    PTERWND w;
    DWORD border;
    struct StrControl far *pInfo;
    BOOL   ReplacePicture;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (TerFont[CurCfmt].style&PROTECT && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;    // ran out of picture table

    // Set control parameters
    border=style&WS_BORDER;                     // save border style
    style=style&(~(DWORD)(WS_OVERLAPPEDWINDOW|WS_VISIBLE)); // turnoff overlapped and visible styles
    style=style|WS_CHILD|WS_CLIPSIBLINGS;                   // force child window
    if (border) style=style|WS_BORDER;

    // store the control information
    InfoSize=sizeof(struct StrControl);
    if  (NULL==(TerFont[pict].pInfo=OurAlloc(InfoSize)) ) {
          PrintError(w,MSG_OUT_OF_MEM,"hInfo");
          InitTerObject(w,pict);
          goto EOR;
    }
    pInfo=(LPVOID)TerFont[pict].pInfo;

    lstrcpy(pInfo->class,class);
    pInfo->style=style;
    pInfo->id=id;


    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    //******** initialize other TerFont variables **********
    TerFont[pict].InUse=TRUE;
    TerFont[pict].PictType=PICT_CONTROL;
    TerFont[pict].ObjectType=OBJ_NONE;
    TerFont[pict].PictHeight=height;  // dimension in twips
    TerFont[pict].PictWidth=width;
    TerFont[pict].ImageSize=0;
    TerFont[pict].InfoSize=InfoSize;
    TerFont[pict].style=PICT;
    TerFont[pict].PictAlign=align;
    TerFont[pict].AuxId=id;                          // user assigned id

    // Create the control
    if (!RealizeControl(w,pict)) {
       InitTerObject(w,pict);
       goto EOR;
    }

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine);
       if (CurCol<0) CurCol=0;

       MoveLineData(w,CurLine,CurCol,1,'B');   // make extra space

       ptr=pText(CurLine);
       ptr[CurCol]=PICT_CHAR;                      // any character


       fmt=OpenCfmt(w,CurLine);
       fmt[CurCol]=(WORD)pict;               // picture index serves as the formatting attribute
       CloseCfmt(w,CurLine);
    }

    EOR:                                  // end of routine
    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);   // suppress word-wrap when not inserting into text here

    return pict;
}

/****************************************************************************
    FormFieldSubclass:
    This function intercepts the formfield control messages
*****************************************************************************/
LRESULT CALLBACK _export FormFieldSubclass(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    int CtlId=GetDlgCtrlID(hWnd);
    HWND hParent=GetParent(hWnd);
    PTERWND w;
    int i,pict;
    struct StrForm far *pForm=NULL;
    LRESULT result;
    BOOL  ExitingTextField=FALSE;

    if (NULL==(w=GetWindowPointer(hParent))) {
       hParent=GetParent(hParent);
       if (NULL==(w=GetWindowPointer(hParent))) return (LRESULT)FALSE;
    }
    
    for (i=0;i<TotalFonts;i++) {
        if (!IsFormField(w,i,0)) continue;
        pForm=(LPVOID)TerFont[i].pInfo;
        ExitingTextField=(TerFont[i].FieldId==FIELD_TEXTBOX);
        if (TerFont[i].FieldId!=FIELD_TEXTBOX && pForm->id==CtlId) break;
    }
    if (i==TotalFonts) return (LRESULT)TRUE;
    pict=i;


    if (/*message==WM_KEYDOWN && */ wParam==VK_TAB) {   // tab key hit
        BOOL backward=GetKeyState(VK_SHIFT)&0x8000;

        if (message!=WM_KEYDOWN) return (LRESULT)FALSE;
        
        TabOnControl(w,CtlId,backward);

        return (LRESULT) TRUE;
    }
    else if (message==WM_SETFOCUS) {
        CurInputField=pict;
        if (IsFormField(w,pict,FIELD_CHECKBOX)) SelectFormField(w,pict);   // position the cursor on the check-box control
    }

    result=(LRESULT)CallWindowProc(pForm->OrigWndProc,hWnd,message,wParam,lParam);

    if (message==WM_PAINT) {     // clear any border spill
       RECT rect;
       POINT pt[2];
       GetWindowRect(hWnd,&rect);        // get the control position relative to the top of the screen
       pt[0].x=rect.left;
       pt[0].y=rect.top;
       pt[1].x=rect.right;
       pt[1].y=rect.bottom;
       ScreenToClient(hTerWnd,&(pt[0]));   // relative to edit client area
       ScreenToClient(hTerWnd,&(pt[1]));

       // record the directions in which the control spilled over the border
       CalcBorderSpill(w,pt[0].x,pt[0].y,pt[1].x,pt[1].y);
       RedrawNonText(w);
       BorderSpill=0;
    }
    else if (message==WM_KILLFOCUS) {
       // set to the beginning of the field being exited
       if (ExitingTextField) {
          if (Win32) SendMessage(hWnd,EM_SETSEL,0,0);
          else       SendMessage(hWnd,EM_SETSEL,0,MAKELONG(0,0));
       }
    }
    else if (message==WM_LBUTTONDOWN) {
       if (IsFormField(w,CurInputField,FIELD_CHECKBOX)) TerArg.modified++;
    }
     
    return result;
}

/******************************************************************************
    TabOnControl:
    Process Tab click on the input-field
*******************************************************************************/
BOOL TabOnControl(PTERWND w, int ctl, int backward)
{
    long CtlPos,CurCtlPos,line;
    int  pict,col;
    
    // check if tab is pressed on a text-input field
    if (ctl<0) {
      int CurFont=GetCurCfmt(w,CurLine,CurCol);
      if (TerFont[CurFont].FieldId==FIELD_TEXTBOX) {
         pict=TerFont[CurFont].AuxId;  // field id
         
         SelectTextInputField(w,false);      // to get the field begin position
         if (backward) CurCtlPos=CtlPos=RowColToAbs(w,HilightBegRow,HilightBegCol);   // position of the current control
         else          CurCtlPos=CtlPos=RowColToAbs(w,HilightEndRow,HilightEndCol)-1;   // position of the current control
         
         DeselectTerText(hTerWnd,true);
      }
      else return false;
    }
    else {
      CurCtlPos=CtlPos=TerGetControlPos(hTerWnd,ctl);   // position of the current control
      if (CtlPos<0) return false;
    }

    if (backward) {
       // scan to the beginning of file
       while ((CtlPos=TerGetPrevControlPos(hTerWnd,CtlPos-1))>=0) {  // scan backward to the beginning of the file
            AbsToRowCol(w,CtlPos,&line,&col);
            pict=TerGetCurFont(hTerWnd,line,col);
            if (TerFont[pict].FieldId==FIELD_TEXTBOX || IsFormField(w,pict,0)) goto FIELD_FOUND;
       }

       // scan from the end of file to the current location
       CtlPos=RowColToAbs(w,TotalLines-1,LineLen(TotalLines-1));
       while ((CtlPos=TerGetPrevControlPos(hTerWnd,CtlPos-1))>=0) {  // scan backward to the beginning of the file
            if (CtlPos<=CurCtlPos) return TRUE;   // no more controls found
            AbsToRowCol(w,CtlPos,&line,&col);
            pict=TerGetCurFont(hTerWnd,line,col);
            if (TerFont[pict].FieldId==FIELD_TEXTBOX || IsFormField(w,pict,0)) goto FIELD_FOUND;
       }
    }
    else {
       // scan to the end of file
       while ((CtlPos=TerGetNextControlPos(hTerWnd,CtlPos+1))>=0) {  // scan backward to the beginning of the file
            AbsToRowCol(w,CtlPos,&line,&col);
            pict=TerGetCurFont(hTerWnd,line,col);
            if (TerFont[pict].FieldId==FIELD_TEXTBOX || IsFormField(w,pict,0)) goto FIELD_FOUND;
       }

       // scan from the beginning of file to the current location
       CtlPos=0;
       while ((CtlPos=TerGetNextControlPos(hTerWnd,CtlPos-1))>=0) {  // scan backward to the beginning of the file
            if (CtlPos>=CurCtlPos) return TRUE;   // no more controls found
            AbsToRowCol(w,CtlPos,&line,&col);
            pict=TerGetCurFont(hTerWnd,line,col);
            if (TerFont[pict].FieldId==FIELD_TEXTBOX || IsFormField(w,pict,0)) goto FIELD_FOUND;
       }

    }
    return (LRESULT) TRUE;       // no other control found

    FIELD_FOUND:

    SelectFormField(w,pict);

    return TRUE;
}
    
/******************************************************************************
    SelectFirstFormField:
    Position at and select the first form field in the document.
*******************************************************************************/
SelectFirstFormField(PTERWND w)
{
    int i,pict,col;
    long CtlPos,line;


    for (i=0;i<TotalFonts;i++) if (IsFormField(w,i,0)) break;
    if (i==TotalFonts) return TRUE;        // no form fields found in the document

    // scan from the beginning of file to the current location
    CtlPos=0;
    while ((CtlPos=TerGetNextControlPos(hTerWnd,CtlPos-1))>=0) {  // scan backward to the beginning of the file
        AbsToRowCol(w,CtlPos,&line,&col);
        pict=TerGetCurFont(hTerWnd,line,col);
        if (IsFormField(w,pict,0)) return SelectFormField(w,pict);
    }

    return TRUE;
}

/******************************************************************************
    SelectFormField:
    Select the specified form field.
*******************************************************************************/
SelectFormField(PTERWND w, int pict)
{
    struct StrForm far *pForm;
    long CtlPos,line;
    int  col;
    HWND hCtl;

    if (TerFont[pict].FieldId==FIELD_TEXTBOX) {
      int id=TerFont[pict].AuxId;   // aux-id is used as the field id
      FindTextInputField(w,id);
      SetTerCursorPos(hTerWnd,CurLine,CurCol,!IsTextPosVisible(w,CurLine,CurCol));
      SelectTextInputField(w,true);
      if (CurInputField!=0) SetFocus(hTerWnd);  // Deselect the previous control
      CurInputField=0;
    }
    else {
      // scan the document to find this field
      pForm=(LPVOID)TerFont[pict].pInfo;
      CtlPos=TerGetControlPos(hTerWnd,(int)pForm->id);
      if (CtlPos<0) return TRUE;

      hCtl=GetDlgItem(hTerWnd,(int)pForm->id);
      if (GetFocus()!=hCtl) SetFocus(hCtl);
      hCurCtlWnd=hCtl;

      AbsToRowCol(w,CtlPos,&line,&col);
      SetTerCursorPos(hTerWnd,CtlPos,-1,IsTextPosVisible(w,line,col)?FALSE:TRUE);
    }


    return TRUE;
}

/******************************************************************************
    TerGetPrevControlPos:
    Find the position of the previous control.  This function returns -1 if the control
    is not found. This function returns the absolute postion
*******************************************************************************/
long WINAPI _export TerGetPrevControlPos(HWND hWnd,long pos)
{
    PTERWND w;
    long line,l;
    int col,i,CurFont,StartCol;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    AbsToRowCol(w,pos,&line,&col);

    for (l=line;l>=0;l--) {
       if (LineLen(l)==0) continue;

       if (l==line) StartCol=col;
       else         StartCol=LineLen(l)-1;

       fmt=OpenCfmt(w,l);
       for (i=StartCol;i>=0;i--) {
          if (IsControl(w,fmt[i])) goto CONTROL_FOUND;
       
          // check for the text-input field
          CurFont=fmt[i];
          if (TerFont[CurFont].FieldId==FIELD_TEXTBOX) {    // position at the beginning of the field
             SelectTextInputField2(w,l,i,false);
             l=HilightBegRow;          // beginning field position
             i=HilightBegCol;
             DeselectTerText(hTerWnd,false);
             goto CONTROL_FOUND;
          } 
       }
       CloseCfmt(w,l);
    }

    return -1;  // no control found

    CONTROL_FOUND:
    return RowColToAbs(w,l,i);
}

/******************************************************************************
    TerGetNextControlPos:
    Find the position of the next control.  This function returns -1 if the control
    is not found. This function returns the absolute postion
*******************************************************************************/
long WINAPI _export TerGetNextControlPos(HWND hWnd,long pos)
{
    PTERWND w;
    long line,l;
    int CurFont,col,i,StartCol;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    AbsToRowCol(w,pos,&line,&col);

    for (l=line;l<TotalLines;l++) {
       if (LineLen(l)==0) continue;

       if (l==line) StartCol=col;
       else         StartCol=0;

       fmt=OpenCfmt(w,l);
       for (i=StartCol;i<LineLen(l);i++) {
          if (IsControl(w,fmt[i])) goto CONTROL_FOUND;
       
          // check for the text-input field
          CurFont=fmt[i];
          if (TerFont[CurFont].FieldId==FIELD_TEXTBOX) goto CONTROL_FOUND;
       }
       CloseCfmt(w,l);
    }

    return -1;  // no control found

    CONTROL_FOUND:
    return RowColToAbs(w,l,i);
}

/******************************************************************************
    TerGetControlPos:
    Find the position of a control.  This function returns -1 if the control
    is not found. This function returns the absolute postion
*******************************************************************************/
long WINAPI _export TerGetControlPos(HWND hWnd,int CtlId)
{
    PTERWND w;
    int pict,i;
    long l;
    LPWORD fmt;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (-1==(pict=TerXlateControlId(hWnd,CtlId))) return -1;

    for (l=0;l<TotalLines;l++) {
       fmt=OpenCfmt(w,l);
       for (i=0;i<LineLen(l);i++) if ((int)fmt[i]==pict) break;
       CloseCfmt(w,l);

       if (i<LineLen(l)) break;
    }

    if (l<TotalLines) return RowColToAbs(w,l,i);
    else              return -1;
}

/******************************************************************************
    SetControlColor:
    Set the color of the embedded control.
******************************************************************************/
HBRUSH SetControlColor(PTERWND w, HDC hDC, HWND hCtl)
{
    HBRUSH hBr=NULL;
    int i,id;
    BOOL HasFocus=(GetFocus()==hCtl);

    id=GetDlgCtrlID(hCtl);

    for (i=0;i<TotalFonts;i++) {
       struct StrForm far *pForm=(LPVOID)TerFont[i].pInfo;
       struct StrControl far *pCtl=(LPVOID)TerFont[i].pInfo;

       if (false && IsFormField(w,i,FIELD_TEXTBOX)) {   // text boxes are no longer controls
          if (pForm->id==id && !(pForm->style&WS_BORDER)) {   // if the border is not set, shade the field
             if (FALSE && HasFocus) {
                SetTextColor(hDC,CLR_WHITE);
                SetBkColor(hDC,GetSysColor(COLOR_GRAYTEXT));
                return (HBRUSH) GetStockObject(GRAY_BRUSH);
             }
             else {
                SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
                return (HBRUSH) GetStockObject(LTGRAY_BRUSH);
             }
          }
       }
       else if (IsFormField(w,i,FIELD_CHECKBOX)) {
          if (HasFocus) {
             RECT rect;
             HBRUSH hOldBr;
             GetClientRect(hCtl,&rect);
             hOldBr=SelectObject(hDC,GetStockObject(NULL_BRUSH));
             Rectangle(hDC,rect.left,rect.top,rect.right+1,rect.bottom);
             SelectObject(hDC,hOldBr);
          }
       }
       else if (IsControl(w,i) && pCtl->id==id && !IsFormField(w,i,0)) {   // regular controls
          BYTE class[50];
          GetClassName(hCtl,class,sizeof(class)-1);
          if (lstrcmpi(class,"Button")==0) {
             if (pCtl->style&(BS_RADIOBUTTON|BS_CHECKBOX)) {         // clear the area outside the circle
                RECT rect;
                HBRUSH hOldBr,hBr;
                HPEN hOldPen;
                GetClientRect(hCtl,&rect);
                hBr=CreateSolidBrush(PageColor(w));
                hOldBr=SelectObject(hDC,hBr);
                hOldPen=SelectObject(hDC,GetStockObject(NULL_PEN));
                Rectangle(hDC,rect.left,rect.top,rect.right+1,rect.bottom+1);
                SelectObject(hDC,hOldBr);
                SelectObject(hDC,hOldPen);
                DeleteObject(hBr);
             }
             return (HBRUSH) GetStockObject(NULL_BRUSH);
          } 
       
       } 
    }


    return hBr;
}

/******************************************************************************
    IsFormField:
    Returns TRUE if the font id is a form field of the specified type. (type 0 will match with any
    form field).
******************************************************************************/
BOOL IsFormField(PTERWND w, int pict, int type)
{
    if (pict>=0 && pict<TotalFonts && TerFont[pict].InUse) {
        int fld=TerFont[pict].FieldId;
        if (fld!=FIELD_TEXTBOX && fld!=FIELD_CHECKBOX && fld!=FIELD_LISTBOX) return false;  // not an input field
        if (type!=0 && type!=fld) return false;           // type does not match
        if (fld!=FIELD_TEXTBOX) {
          if (False(TerFont[pict].style&PICT) || TerFont[pict].PictType!=PICT_FORM) return false;          // non-textbox must be a form picture type
        }
        return true;
    }

    return false;
}

/******************************************************************************
    TerGetWordCount:
    Get the word count. The flags uses WC_ constants.
******************************************************************************/
long WINAPI _export TerGetWordCount(HWND hWnd,UINT flags)
{
    PTERWND w;
    long BegLine,EndLine,l;
    int  BegCol,EndCol,FromCol,ToCol,i;
    int SaveHilightType;
    LPBYTE ptr;
    LPWORD fmt;
    BOOL   PrevDelim,CurDelim,first=TRUE;
    BYTE   chr;
    long   count=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (flags&WC_SELECTION && HilightType!=HILIGHT_OFF && !NormalizeBlock(w)) return -1;

    if (!(flags&WC_SELECTION)) {          // do not select the highlighted text
       SaveHilightType=HilightType;
       HilightType=HILIGHT_OFF;
    }
    GetTextRange(w,&BegLine,&BegCol,&EndLine,&EndCol);
    if (!(flags&WC_SELECTION)) HilightType=SaveHilightType;

    for (l=BegLine;l<=EndLine;l++) {
       if (!(flags&WC_INCLUDE_HDR_FTR) && PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;
       if (l==BegLine) FromCol=BegCol;
       else            FromCol=0;
       if (l==EndLine) ToCol=EndCol;
       else            ToCol=LineLen(l);   // exclusive of the last character

       ptr=pText(l);
       fmt=OpenCfmt(w,l);

       for (i=FromCol;i<ToCol;i++) {
          if (!(flags&WC_INCLUDE_HIDDEN) && HiddenText(w,fmt[i])) continue;

          chr=ptr[i];
          CurDelim=(chr==' ' || chr==TAB || IsBreakChar(w,chr) || TerFont[fmt[i]].style&PICT);

          if (CurDelim && !PrevDelim && !first) count++;
          PrevDelim=CurDelim;
          first=FALSE;
       }

       CloseCfmt(w,l);
    }

    if (!CurDelim) count++;   // add the last word

    return count;
}


