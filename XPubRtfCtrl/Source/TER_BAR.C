/*==============================================================================
   TER_BAR.C
   TER Tool Bar handling functions

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

/***************************************************************************
    ToggleToolBar:
    Toggle tool bar on/off
****************************************************************************/
BOOL ToggleToolBar(PTERWND w)
{
    TerArg.ToolBar=!TerArg.ToolBar;         // toggle

    if (TerArg.ToolBar) CreateToolBar(w);   // create tool bar
    else                DestroyToolBar(w);  // destroy tool bar

    InvalidateRect(hTerWnd,NULL,TRUE);     // force WM_PAINT with erase background

    return TRUE;
}

/***************************************************************************
    InitToolBar:
    Initialize the default toolbar
****************************************************************************/
BOOL InitToolbar(PTERWND w)
{
    int i,j,FirstRow,LastRow=MAX_TLB_LINES-1;
    BOOL GroupItemCount;
    int  SpaceCount,NonSpaceCount;

    FirstRow=(TerFlags4&TFLAG4_ONE_ROW_TOOLBAR)?1:0;
    if (True(TerFlags5&TFLAG5_TOP_ROW_TOOLBAR)) FirstRow=LastRow=0;

    // add standard icons
    for (i=FirstRow;i<=LastRow;i++) {
       GroupItemCount=0;
       SpaceCount=0;                    // number of spacer items
       NonSpaceCount=0;                 // number of non-spacer items
       for (j=0;j<MAX_TLB_PER_LINE;j++) {
          if (TlbItem[i][j]==TLB_LINE) {
             if (j>0 && TlbItem[i][j-1]!=0) {  // not a consequtive line
                if (GroupItemCount==0) {  // discard the ending vertical line if no items in the group
                   if ((j+1)<MAX_TLB_PER_LINE && TlbItem[i][j]==TLB_SPACER) j++;
                   continue;
                } 
             } 
             GroupItemCount=0;                 // new group begins
          }

          if (TlbItem[i][j]==TLB_STYLE && !(TerFlags3&TFLAG3_STYLES_ON_TOOLBAR)) continue;
          if (TlbItemHide[i][j]) continue;  // this icon is to be hidden
          if (TlbItem[i][j]==TLB_SPACER && j>0 && TlbItemHide[i][j-1]) continue;  // discard space after the hidden item
          if (TlbItem[i][j]==TLB_SPACER && j>1 && TlbItem[i][j-1]==TLB_SPACER && TlbItemHide[i][j-2]) continue;  // discard space after space after the hidden item

          if (TlbItem[i][j]==TLB_SPACER && NonSpaceCount==0 && SpaceCount>=2) continue;  // don't let space accumulate at the beginning

          if (TlbItem[i][j]>0 && TlbItem[i][j]<MAX_TLB) TerAddToolbarItem(hTerWnd,i,TlbItem[i][j],NULL,NULL,NULL,0,-1);

          if (TlbItem[i][j]!=TLB_LINE && TlbItem[i][j]!=TLB_SPACER) GroupItemCount++;
          
          if (TlbItem[i][j]==TLB_SPACER) SpaceCount++;
          else                           NonSpaceCount++;
       } 
    } 

    // Add custom icons
    for (i=0;i<TotalTlbCustIds;i++) {
       if (!TlbCustId[i].InUse) continue;

       if (TlbCustId[i].id>0) 
            TerAddToolbarItem(hTerWnd,TlbCustId[i].line,TlbCustId[i].id,TlbCustId[i].pBalloon,NULL,NULL,0,-1);
       else TerAddToolbarItem(hTerWnd,TlbCustId[i].line,0,TlbCustId[i].pBalloon,
                              TlbCustId[i].pInfo,TlbCustId[i].pImage,TlbCustId[i].height,TlbCustId[i].CmdId);
    } 
    
    return TRUE;
} 

/***************************************************************************
    FreeToolbar:
    Hide the toolbar and free its resouces
****************************************************************************/
BOOL FreeToolbar(PTERWND w)
{
    int i,j;
    struct StrTlb far *pTlb;

    if (TerArg.ToolBar) ToggleToolBar(w);   // hide the toolbar

    for (i=0;i<MAX_TLB_LINES;i++) {
      for (j=0;j<TlbItemCount[i];j++) {
         pTlb=&(TlbId[i][j]);
      
         if (pTlb->pBalloon) MemFree(pTlb->pBalloon);
         FarMemSet(pTlb,0,sizeof(struct StrTlb));
      }
      TlbItemCount[i]=0; 
    } 

    return TRUE;
} 

/***************************************************************************
    TerRecreateToolbar:
    Recreate the toolbar
****************************************************************************/
BOOL WINAPI _export TerRecreateToolbar(HWND hWnd, BOOL show)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    FreeToolbar(w);  // hide the toolbar and free the toolbar defintion

    InitToolbar(w);  // recreate toolbar definition

    if (show) ToggleToolBar(w);

    return TRUE;
}

/***************************************************************************
    TerHideToolbarIcon:
    Hide a toolbar icon
****************************************************************************/
BOOL WINAPI _export TerHideToolbarIcon(int id, BOOL hide)
{
    int i,j;

    if (id<=0 || id>=MAX_TLB) return FALSE;

    for (i=0;i<MAX_TLB_LINES;i++) {
      for (j=0;j<MAX_TLB_PER_LINE;j++) {
         if (TlbItem[i][j]==id) {
            TlbItemHide[i][j]=hide;
            if ((j+1)==MAX_TLB_PER_LINE && TlbItem[i][j+1]==TLB_SPACER) {
               TlbItemHide[i][j+1]=hide;
            }
            return TRUE; 
         } 
      }
    }   

    return FALSE;
}
 
/***************************************************************************
    TerEditTooltip:
    Specify new tooltip text
****************************************************************************/
BOOL WINAPI _export TerEditTooltip(HWND hWnd, int id, LPBYTE pBalloon)
{
    PTERWND w;
    int i,j;
    struct StrTlb far *pTlb;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (id<=0 || id>=MAX_TLB) return FALSE;

    for (i=0;i<MAX_TLB_LINES;i++) {
      for (j=0;j<TlbItemCount[i];j++) {
         pTlb=&(TlbId[i][j]);
      
         if (pTlb->id!=id) continue;

         if (pTlb->pBalloon) MemFree(pTlb->pBalloon);
         pTlb->pBalloon=MemAlloc(lstrlen(pBalloon)+1); 
         lstrcpy(pTlb->pBalloon,pBalloon);
      } 
    } 

    return TRUE;
}

/***************************************************************************
    TerAddToolbarIcon:
    Add a custom toolbar icon.  To specify a standard toolbar item, 
    set CmdId to negative Tlb id.
****************************************************************************/
BOOL WINAPI _export TerAddToolbarIcon(HWND hWnd, int line, int id, int CmdId, LPBYTE BmpFile, LPBYTE pBalloon)
{
    PTERWND w;
    int pict,height;
    LPBYTE pInfo;
    LPBYTE pImage;
    struct StrTlbCustId far *pTlbCust;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (TotalTlbCustIds>=MAX_CUST_ICONS) return FALSE;
 
    if (line<0)line=0;
    if (line>1) line=1;
    
    pTlbCust=&(TlbCustId[TotalTlbCustIds]);
    FarMemSet(pTlbCust,0,sizeof(struct StrTlbCustId));
    pTlbCust->InUse=true;
    pTlbCust->line=line;
    
    if (id>0) {    // standard toolbar item specified
       if (id>=MAX_TLB) return FALSE;
       pTlbCust->id=id;
    } 
    else {

       // read the bimap file.
       if (InIE) {         // get the picture file from internet
          BYTE TempName[MAX_WIDTH+1];
          wsprintf(TempName,"TerTlb%d.tmp",hTerWnd);
          if (!TerInternetGet(hTerWnd,BmpFile,TempName,true)) return FALSE;  // file not found
          BmpFile=TempName;  // redirect the pointer the temp file
       } 
       if ((pict=TerInsertPictureFile(hWnd,BmpFile,TRUE,0,FALSE))<=0) return FALSE;
       if (InIE) _unlink(BmpFile);    // delete the temporary file

       pInfo=TerFont[pict].pInfo;
       TerFont[pict].pInfo=NULL;       // transferred

       pImage=TerFont[pict].pImage;
       TerFont[pict].pImage=NULL;      // transferred
       height=TerFont[pict].bmHeight;
       DeleteTerObject(w,pict);

       if (pInfo==NULL || pImage==NULL) return FALSE;

       // save the information in the StrTlbCustId structure
       pTlbCust->CmdId=CmdId;
       pTlbCust->pInfo=pInfo;
       pTlbCust->pImage=pImage;
       pTlbCust->height=height;
       
       if (pBalloon==NULL) pBalloon="";
    }
 
    if (pBalloon!=NULL) {
       pTlbCust->pBalloon=MemAlloc(strlen(pBalloon)+1);
       strcpy(pTlbCust->pBalloon,pBalloon);
    }

    TotalTlbCustIds++;

    return TRUE;
}
  
/***************************************************************************
    TerAddToolbarItem:
    Add a toolbar item
****************************************************************************/
BOOL WINAPI _export TerAddToolbarItem(HWND hWnd, int line, int id, LPBYTE pBalloon, LPBYTE pInfo, LPBYTE pImage, int ImageHeight, int CmdId)
{
    PTERWND w;
    struct StrTlb far *pTlb;
    int i,j,ItemNo;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (line<0 || line>=MAX_TLB_LINES) return FALSE;
    if (TlbItemCount[line]>=MAX_TLB_PER_LINE) return FALSE;

    if (id<0 || id>=MAX_TLB) return FALSE;


    ItemNo=TlbItemCount[line];
    TlbItemCount[line]++;

    pTlb=&(TlbId[line][ItemNo]);
    
    // initialize common variables
    FarMemSet(pTlb,0,sizeof(struct StrTlb));

    pTlb->id=id;

    if (!pBalloon) pBalloon=BalloonText[id];
    if (pBalloon && lstrlen(pBalloon)>0) {                         // save the balloon text
       pTlb->pBalloon=MemAlloc(lstrlen(pBalloon)+1);
       lstrcpy(pTlb->pBalloon,pBalloon);
    } 
    pTlb->height=TLB_ICON_HEIGHT;
    pTlb->width=TLB_ICON_HEIGHT;
 
    // set other variable
    if (id==TLB_SPACER || id==TLB_LINE) {
       pTlb->width=TLB_SPACER_WIDTH;
    } 
    else if (id==TLB_TYPEFACE) {
       pTlb->width=6*TLB_ICON_HEIGHT;
       pTlb->height=8*TLB_ICON_HEIGHT;
       pTlb->flags|=TLBFLAG_COMBO;      // uses combo box
       pTlb->flags|=TLBFLAG_EDITABLE;   // combo box includes edit control
    } 
    else if (id==TLB_POINTSIZE) {
       pTlb->width=3*TLB_ICON_HEIGHT;
       pTlb->height=8*TLB_ICON_HEIGHT;
       pTlb->flags|=TLBFLAG_COMBO;      // uses combo box
       pTlb->flags|=TLBFLAG_EDITABLE;   // combo box includes edit control
    } 
    else if (id==TLB_STYLE) {
       pTlb->width=5*TLB_ICON_HEIGHT;
       pTlb->height=8*TLB_ICON_HEIGHT;
       pTlb->flags|=TLBFLAG_COMBO;      // uses combo box
    } 
    else if (id==TLB_ZOOM) {
       pTlb->width=11*TLB_ICON_HEIGHT/4;
       pTlb->height=8*TLB_ICON_HEIGHT;
       pTlb->flags|=TLBFLAG_COMBO;      // uses combo box
       pTlb->flags|=TLBFLAG_EDITABLE;   // combo box includes edit control
    } 
    else if (id==TLB_BOLD) {
       pTlb->CmdId=ID_BOLD_ON;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=2;                 // position of this icon in the ter_tlb.bmp file
       pTlb->IconY=23;
    } 
    else if (id==TLB_ULINE) {
       pTlb->CmdId=ID_ULINE_ON;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=57;
       pTlb->IconY=23;
    } 
    else if (id==TLB_ITALIC) {
       pTlb->CmdId=ID_ITALIC_ON;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=31;
       pTlb->IconY=23;
    } 
    else if (id==TLB_ALIGN_LEFT) {
       pTlb->CmdId=ID_LEFT_JUSTIFY;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=89;
       pTlb->IconY=23;
    } 
    else if (id==TLB_ALIGN_CENTER) {
       pTlb->CmdId=ID_CENTER;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=117;
       pTlb->IconY=23;
    } 
    else if (id==TLB_ALIGN_RIGHT) {
       pTlb->CmdId=ID_RIGHT_JUSTIFY;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=140;
       pTlb->IconY=23;
    } 
    else if (id==TLB_ALIGN_JUSTIFY) {
       pTlb->CmdId=ID_JUSTIFY;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=160;
       pTlb->IconY=23;
    } 
    else if (id==TLB_INC_INDENT) {
       pTlb->CmdId=ID_LEFT_INDENT;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=205;
       pTlb->IconY=23;
    } 
    else if (id==TLB_DEC_INDENT) {
       pTlb->CmdId=ID_LEFT_INDENT_DEC;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=181;
       pTlb->IconY=23;
    } 
    else if (id==TLB_CUT) {
       pTlb->CmdId=ID_CUT;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=0;
       pTlb->IconY=47;
    } 
    else if (id==TLB_COPY) {
       pTlb->CmdId=ID_COPY;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=19;
       pTlb->IconY=47;
    } 
    else if (id==TLB_PASTE) {
       pTlb->CmdId=ID_PASTE;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=40;
       pTlb->IconY=46;
    } 
    else if (id==TLB_NEW) {
       pTlb->CmdId=ID_NEW;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=74;
       pTlb->IconY=0;
    } 
    else if (id==TLB_OPEN) {
       pTlb->CmdId=ID_OPEN;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=94;
       pTlb->IconY=0;
    } 
    else if (id==TLB_SAVE) {
       pTlb->CmdId=ID_SAVE;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=118;
       pTlb->IconY=0;
    } 
    else if (id==TLB_PRINT) {
       pTlb->CmdId=ID_PRINT;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=143;
       pTlb->IconY=0;
    } 
    else if (id==TLB_PREVIEW) {
       pTlb->CmdId=ID_PRINT_PREVIEW;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=166;
       pTlb->IconY=0;
    } 
    else if (id==TLB_HELP) {
       pTlb->CmdId=ID_TER_HELP;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=80;
       pTlb->IconY=70;
    } 
    else if (id==TLB_PAR) {
       pTlb->CmdId=ID_SHOW_PARA_MARK;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=61;
       pTlb->IconY=70;
    } 
    else if (id==TLB_NUMBER) {
       pTlb->CmdId=ID_PARA_NBR;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=51;
       pTlb->IconY=0;
    } 
    else if (id==TLB_BULLET) {
       pTlb->CmdId=ID_BULLET;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=30;
       pTlb->IconY=0;
    } 
    else if (id==TLB_UNDO) {
       pTlb->CmdId=ID_UNDO;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=65;
       pTlb->IconY=46;
    } 
    else if (id==TLB_REDO) {
       pTlb->CmdId=ID_REDO;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=88;
       pTlb->IconY=47;
    } 
    else if (id==TLB_FIND) {
       pTlb->CmdId=ID_SEARCH;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=110;
       pTlb->IconY=48;
    } 
    else if (id==TLB_DATE) {
       pTlb->CmdId=ID_INSERT_DATE_TIME;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=1;
       pTlb->IconY=70;
    } 
    else if (id==TLB_PAGE_NUM) {
       pTlb->CmdId=ID_INSERT_PAGE_NUMBER;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=22;
       pTlb->IconY=71;
    } 
    else if (id==TLB_PAGE_COUNT) {
       pTlb->CmdId=ID_INSERT_PAGE_COUNT;
       pTlb->flags|=TLBFLAG_MOUSE;    // check mouse click
       pTlb->IconX=41;
       pTlb->IconY=71;
    } 
    else if (id==0) {              // custom icon
       pTlb->CmdId=CmdId;
       pTlb->flags|=TLBFLAG_MOUSE;
       pTlb->pInfo=pInfo;         // custom icon image info
       pTlb->pImage=pImage;
       pTlb->ImageHeight=ImageHeight;
    
       // assign a unique id
       pTlb->id=MAX_TLB;
       for (i=0;i<MAX_TLB_LINES;i++) {
         for (j=0;j<TlbItemCount[i];j++) {
            struct StrTlb far *pCurTlb=&(TlbId[i][j]);
      
            if (pCurTlb->id>=pTlb->id) pTlb->id=pCurTlb->id+1;
         }
       }

    }                                   

    return TRUE;
}


/***************************************************************************
    CreateToolBar:
    This routine creates the tool bar for the editor
    about the TER control. This routine returns the handle to the CTLINFO
    structure.
****************************************************************************/
BOOL CreateToolBar(PTERWND w)
{
    int  i,id,idx,j,k,x,y,BarHeight;
    HWND hWnd,hEditWnd;
    struct StrEnumFonts far * font;
    OLDFONTENUMPROC lpProc;
    struct StrTlb far *pTlb;

    if (!TerArg.ToolBar) return TRUE;

    ToolBarHeight=0;
    BarHeight=TLB_ICON_HEIGHT+2*TLB_MARGIN;
    y=TLB_MARGIN;      // initial y
    for (i=0;i<MAX_TLB_LINES;i++) {
       if (TlbItemCount[i]) {
          y+=ToolBarHeight;
          ToolBarHeight+=BarHeight;
       }
       x=TLB_MARGIN;

       for (j=0;j<TlbItemCount[i];j++) {
          pTlb=&(TlbId[i][j]);
          id=pTlb->id;

          pTlb->x=x;
          pTlb->y=y;
          if (!(pTlb->flags&TLBFLAG_COMBO)) pTlb->y++;  // centering

          x+=pTlb->width;
       } 
    }       


    hToolBarWnd=CreateWindow(TlbClass,"Ter Toolbar",WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,0,0,TerRect.right-TerRect.left,ToolBarHeight,
                             hTerWnd,NULL,hTerInst,NULL);
    if (hToolBarWnd==NULL) return FALSE;



    // create the toolbar items
    for (i=0;i<MAX_TLB_LINES;i++) {
       for (j=0;j<TlbItemCount[i];j++) {
          pTlb=&(TlbId[i][j]);
          id=pTlb->id;

          // process combo-box type of items
          if (pTlb->flags&TLBFLAG_COMBO) {
             DWORD ListStyle=CBS_DROPDOWNLIST;
             if (pTlb->flags&TLBFLAG_EDITABLE) ListStyle=CBS_DROPDOWN;  // allow edit box
             if (id==TLB_TYPEFACE || id==TLB_STYLE) ListStyle|=CBS_SORT;

             pTlb->hWnd=hWnd=CreateWindow("ComboBox",pTlb->pBalloon,WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_AUTOHSCROLL|ListStyle
                                     ,pTlb->x,pTlb->y,pTlb->width,pTlb->height,hToolBarWnd,(HMENU)id,hTerInst,NULL);
          
             SendMessage(hWnd,WM_SETFONT,(WPARAM)hRulerFont,(LPARAM)0);  // set combobox font
             SendMessage(hWnd,CB_SETITEMHEIGHT,(WPARAM)-1,14);  // set the edit control height

             hEditWnd=GetComboEditWindow(hToolBarWnd,id);       // edit control window in the combo box

             if (!ToolBarComboProc) ToolBarComboProc=(WNDPROC)MakeProcInstance((FARPROC)BarComboSubclass, hTerInst);

             // do needed subclassing
             if (id==TLB_TYPEFACE) {
                OrigBarFontProc=(WNDPROC)(DWORD)GetWindowLong(hEditWnd,GWL_WNDPROC); // original control proceedure
                SetWindowLong(hEditWnd,GWL_WNDPROC,(DWORD)ToolBarComboProc);
             
                // fill the combo-box
                if (NULL!=(font=OurAlloc(sizeof(struct StrEnumFonts)))) {
                   LOGFONT lFont;

                   font->GetMultiple=font->EnumTypeFace=TRUE; // get all type faces
                   font->TypeCount=font->PointCount=TotalTypes=0;
                   
                   //lpProc = (OLDFONTENUMPROC) MakeProcInstance((FARPROC)TerEnumFonts, hTerInst);
                   //EnumFonts(hTerDC,NULL,lpProc,(LPARAM)(LPBYTE)font);
                   FarMemSet(&lFont,0,sizeof(lFont));
                   SetCurLang(w,GetKeyboardLayout(0)); // set current language and character set
                   lFont.lfCharSet=(ReqCharSet==ANSI_CHARSET)?DEFAULT_CHARSET:ReqCharSet;
                   lpProc = (OLDFONTENUMPROC) MakeProcInstance((FARPROC)TerEnumFontFamiliesEx, hTerInst);
                   EnumFontFamiliesEx(hTerDC,&lFont,lpProc,(LPARAM)(LPBYTE)font,0);
                   
                   FreeProcInstance((FARPROC)lpProc);      // set back the old window

                   TotalTypes=font->TypeCount;          // number of fonts available
                   for (k=0;k<TotalTypes;k++) {        // add names to the list box
                      SendMessage(hWnd,CB_ADDSTRING, 0, (DWORD)(LPBYTE)(font->TypeNames[k]));
                   }
                   OurFree(font);                      // freeup the font structure
                }
             } 
             else if (id==TLB_POINTSIZE) {
                OrigBarPointProc=(WNDPROC)(DWORD)GetWindowLong(hEditWnd,GWL_WNDPROC); // original control proceedure
                SetWindowLong(hEditWnd,GWL_WNDPROC,(DWORD)ToolBarComboProc);
             } 
             else if (id==TLB_ZOOM) {
                int  MaxZooms=7;
                int  zoom[7]={200,150,100,75,50,25,10};
                
                OrigBarZoomProc=(WNDPROC)(DWORD)GetWindowLong(hEditWnd,GWL_WNDPROC); // original control proceedure
                SetWindowLong(hEditWnd,GWL_WNDPROC,(DWORD)ToolBarComboProc);

                for (k=0;k<MaxZooms;k++) {
                   wsprintf(TempString,"%d%%",zoom[k]);
                   idx=(int)SendMessage(hWnd,CB_ADDSTRING, 0, (DWORD)(LPSTR)TempString);
                   SendMessage(hWnd,CB_SETITEMDATA, idx, zoom[k]);
                }
             } 
             else if (id==TLB_STYLE) {
                OrigBarStyleProc=(WNDPROC)(DWORD)GetWindowLong(hWnd,GWL_WNDPROC); // original control proceedure
                SetWindowLong(hWnd,GWL_WNDPROC,(DWORD)ToolBarComboProc);
                FillStyleBox(w,hToolBarWnd,TLB_STYLE,0,TRUE,FALSE);   // fill initially
             } 
          } 

       } 
    }
    

    // disable controls in read only mode
    if (TerArg.ReadOnly) EnableToolbarIcons(w,FALSE);

    // initialize Tool bar font and para id
    ToolBarCfmt=-1;       // reset
    ToolBarPfmt=-1;

    // update the toolbar
    UpdateToolBar(w,TRUE);

    return TRUE;
}

/***************************************************************************
    PosToolbarIcon:
    Position the toolbar icon at the given x position. This functino returns
    the x position after the specified space after the icon.
****************************************************************************/
int PosToolbarIcon(PTERWND w,int IconId,int x,int space)
{
    HWND hWnd=GetDlgItem(hToolBarWnd,IconId);
    RECT rect,ParentRect;
    int width,y;

    GetWindowRect(hWnd,&rect);
    width=rect.right-rect.left;

    GetWindowRect(hToolBarWnd,&ParentRect);   // get the parent window position
    y=rect.top-ParentRect.top;

    SetWindowPos(hWnd,NULL,x,y,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOREDRAW|SWP_NOZORDER|SWP_SHOWWINDOW);
 
    return (x+width+space);
}
 
/***************************************************************************
    EnableToolbarIcons:
    Enable/Disable toolbar icons.
****************************************************************************/
BOOL EnableToolbarIcons(PTERWND w, BOOL enable)
{
    int i,j;
    HWND hWnd;
    struct StrTlb far *pTlb;

    for (i=0;i<MAX_TLB_LINES;i++) {
       for (j=0;j<TlbItemCount[i];j++) {
          pTlb=&(TlbId[i][j]);
          hWnd=pTlb->hWnd;

          if (hWnd && (enable || pTlb->id!=TLB_ZOOM)) EnableWindow(hWnd,enable);
       }
    }

    return TRUE;
}

/***************************************************************************
    DestroyToolBar:
    This routine destroys the toolbar.
****************************************************************************/
BOOL DestroyToolBar(PTERWND w)
{
   if (!hToolBarWnd) return TRUE;

   DestroyWindow(hToolBarWnd);  // destroy the tool bar

   // free the process instances
   #if !defined(WIN32)
      if (lpToolBar) FreeProcInstance(lpToolBar);
      if (ToolBarComboProc)  FreeProcInstance((FARPROC)ToolBarComboProc);
   #endif

   hToolBarWnd=NULL;            // reset the tool bar handle
   ToolBarHeight=0;
   ToolBarComboProc=NULL;

   return TRUE;
}

/***************************************************************************
    GetComboEditWindow:
    Get the edit window handle for the given combo control
****************************************************************************/
HWND GetComboEditWindow(HWND hDlg,int id)
{
    HWND hCombo;
    HWND hWnd;
    BYTE ClassName[32];

    hCombo=GetDlgItem(hDlg,id);   // handle of the combo window handle

    hWnd=GetWindow(hCombo,GW_CHILD);  // get the first child window

    while (TRUE) {               // select the child window of 'Edit' style
       GetClassName(hWnd,ClassName,sizeof(ClassName)-1);
       if (lstrcmpi(ClassName,"Edit")==0) break;
       if (NULL==(hWnd=GetWindow(hWnd,GW_HWNDNEXT))) return NULL;
    }

    return hWnd;
}

/****************************************************************************
    TerUpdateToolbar:
    Update toolbar
*****************************************************************************/
BOOL WINAPI _export TerUpdateToolbar(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return UpdateToolBar(w,TRUE);
}

/******************************************************************************
    UpdateToolBar:
    Update the tool bar control display
******************************************************************************/
BOOL UpdateToolBar(PTERWND w, BOOL always)
{
    HWND hFocusWnd;
    HDC hDC;

    if (!CaretEngaged && !always) return FALSE;

    if (MessageId==WM_NCHITTEST || MessageId==WM_SETCURSOR || MessageId==WM_NCMOUSEMOVE 
       || MessageId==WM_ENTERIDLE || MessageId==WM_INITMENU) return TRUE;  // these frequent message can cause the combo items to flicker

    hFocusWnd=GetFocus();
    TlbIdClicked=-1;

    hDC=GetDC(hToolBarWnd);
    PaintToolbar(hToolBarWnd,hDC);
    ReleaseDC(hToolBarWnd,hDC);

    SendMessageToParent(w,TER_UPDATE_TOOLBAR,(WPARAM)hTerWnd,0L,FALSE);  // update any external toolbar

    if (hFocusWnd==hTerWnd) SetFocus(hTerWnd);

    return TRUE;
}

/******************************************************************************
    TlbWndProc:
    Toolbar window process
******************************************************************************/
LRESULT CALLBACK _export TlbWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w=NULL;
    BOOL apply=FALSE;
    int CurType,zoom,TwipsSize,NewStyle;
    BYTE string[100];


    switch (message) {
       case WM_PAINT:
          w=GetWindowPointer(GetParent(hWnd));
          if (w) {
             PAINTSTRUCT PaintData;
             HDC hDestDC=BeginPaint(hWnd,&PaintData);
             TlbIdClicked=-1;
             PaintToolbar(hWnd,hDestDC);
             EndPaint(hWnd,&PaintData);
          }
          InvalidateToolbarComboItems(hWnd);
          break;

       case WM_ERASEBKGND:
          return (LRESULT)NULL;

       case WM_LBUTTONDOWN:
          TlbLButtonDown(hWnd,lParam);
          break;

       case WM_LBUTTONUP:
          w=GetWindowPointer(GetParent(hWnd));
          TlbIdClicked=-1;
          PostMessage(hTerWnd,TER_IDLE,0,0L); // do idle time activity
          break;

       case WM_MOUSEMOVE:
          TlbMouseMove(hWnd,lParam);
          break;

       case WM_SETCURSOR:                  
          SetCursor(LoadCursor(0,IDC_ARROW));
          break;

       case WM_TIMER:
          TlbTimer(hWnd,(int)wParam);
          break;

       case WM_COMMAND:
          w=GetWindowPointer(GetParent(hWnd));
          
          switch (GET_WM_COMMAND_ID(wParam,lParam)) {
             case TLB_TYPEFACE:
                if (w && hBlnBM) EraseBalloon(w);

                apply=FALSE;
                if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                    CurType=(int)SendMessage(GetDlgItem(hWnd,TLB_TYPEFACE),CB_GETCURSEL, 0,0L);
                    if (CurType==CB_ERR) return (TRUE);
                    SendMessage(GetDlgItem(hWnd,TLB_TYPEFACE),CB_GETLBTEXT, (WPARAM)  CurType,(DWORD)(LPBYTE)string);
                    apply=TRUE;  
                }
                else if (NOTIFY_MSG(wParam,lParam)==CBN_DBLCLK) { // subclass special message
                    GetDlgItemText(hWnd,TLB_TYPEFACE,string,sizeof(string)-1);
                    apply=TRUE;  
                }

                // apply new font if changed
                if (apply) {
                   SetFocus(hTerWnd);
                   SetTerFont(hTerWnd,string,TRUE);    // apply font without screen refresh
                   SendMessageToParent(w,TER_ACTION,ACTION_COMMAND,ID_FONTS,FALSE);
                   PostMessage(hTerWnd,TER_IDLE,0,0L); // do idle time activity
                   return TRUE;
                }

                break;
             
             case TLB_POINTSIZE:
                if (w && hBlnBM) EraseBalloon(w);
                
                if (NOTIFY_MSG(wParam,lParam)==CBN_DROPDOWN) {
                   if (CurSID>=0) SetPointSizes(w,hToolBarWnd,StyleId[CurSID].TypeFace,StyleId[CurSID].TwipsSize);
                   else {
                       int CurFont=GetEffectiveCfmt(w);
                       SetPointSizes(w,hToolBarWnd,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize);
                   }
                   return TRUE;
                }

                // respond to uesr selection
                apply=FALSE;
                if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                    CurType=(int)SendMessage(GetDlgItem(hToolBarWnd,TLB_POINTSIZE),CB_GETCURSEL, 0,0L);
                    if (CurType==CB_ERR) return (TRUE);
                    SendMessage(GetDlgItem(hToolBarWnd,TLB_POINTSIZE),CB_GETLBTEXT, (WPARAM)  CurType,(DWORD)(LPBYTE)TempString);
                    TwipsSize=(int)(atof(TempString)*20);
                    apply=TRUE;  
                }
                else if (NOTIFY_MSG(wParam,lParam)==CBN_DBLCLK) { // subclass special message
                    GetDlgItemText(hToolBarWnd,TLB_POINTSIZE,TempString,sizeof(TempString)-1);
                    TwipsSize=(int)(atof(TempString)*20);
                    apply=TRUE;  
                }

                // apply new font if changed
                if (apply) {
                   SetFocus(hTerWnd);
                   SetTerPointSize(hTerWnd,-TwipsSize,TRUE); // apply font without screen refresh
                   SendMessageToParent(w,TER_ACTION,ACTION_COMMAND,ID_FONTS,FALSE);
                   PostMessage(hTerWnd,TER_IDLE,0,0L);      // do idle time activity
                   return TRUE;
                }
                
                break;

             case TLB_ZOOM:
                if (w && hBlnBM) EraseBalloon(w);
                
                // respond to uesr selection
                apply=FALSE;
                if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                    CurType=(int)SendMessage(GetDlgItem(hToolBarWnd,TLB_ZOOM),CB_GETCURSEL, 0,0L);
                    if (CurType==CB_ERR) return (TRUE);
                    SendMessage(GetDlgItem(hToolBarWnd,TLB_ZOOM),CB_GETLBTEXT, (WPARAM)  CurType,(DWORD)(LPBYTE)TempString);
                    apply=TRUE;  
                }
                else if (NOTIFY_MSG(wParam,lParam)==CBN_DBLCLK) { // subclass special message
                    GetDlgItemText(hToolBarWnd,TLB_ZOOM,TempString,sizeof(TempString)-1);
                    apply=TRUE;  
                }

                // apply new font if changed
                if (apply) {
                   int len=lstrlen(TempString);
                   if (len>0 && TempString[len-1]=='%') TempString[len-1]=0;
                   zoom=atoi(TempString);
                   SetFocus(hTerWnd);
                   TerSetZoom(hTerWnd,zoom);
                   SendMessageToParent(w,TER_ACTION,ACTION_COMMAND,ID_ZOOM,FALSE);
                   PostMessage(hTerWnd,TER_IDLE,0,0L);      // do idle time activity
                   return TRUE;
                }
             
                   
                break;

             case TLB_STYLE:
                if (w && hBlnBM) EraseBalloon(w);
                   
                if (NOTIFY_MSG(wParam,lParam)==CBN_DROPDOWN) {
                   FillStyleBox(w,hWnd,TLB_STYLE,0,TRUE,FALSE);
                }
                
                // test for style change
                if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                    int idx=(int)SendMessage(GetDlgItem(hToolBarWnd,TLB_STYLE),CB_GETCURSEL, 0,0L);
                    if (idx==CB_ERR) return (TRUE);
                    NewStyle=(int)SendMessage(GetDlgItem(hToolBarWnd,TLB_STYLE),CB_GETITEMDATA, (WPARAM)idx,0L);
                   
                    // apply new style if changed
                    if (NewStyle>=0 && StyleId[NewStyle].InUse) {
                       if (StyleId[NewStyle].type==SSTYPE_CHAR) TerSelectCharStyle(hTerWnd,NewStyle,TRUE);
                       else                                     TerSelectParaStyle(hTerWnd,NewStyle,TRUE);
                       PostMessage(hTerWnd,TER_IDLE,0,0L);      // do idle time activity
                       return TRUE;
                    }
                }

                break;
             
             default:
                return (DefWindowProc(hWnd, message, wParam, lParam));
          }
          break;

       default:
          return (DefWindowProc(hWnd, message, wParam, lParam));
    }
 
    return (LRESULT)NULL;
}
 
/******************************************************************************
    PaintToolbar:
    Paint the toolbar
******************************************************************************/
BOOL PaintToolbar(HWND hWnd, HDC hDestDC)
{
    HBITMAP hBM,hOldBM;
    RECT rect;
    int i,j,id,y,CurFont,CurTwipsSize,CurPointSize;
    struct StrTlb far *pTlb;
    HDC hDC,hIconDC;
    PTERWND w;
    HBRUSH hOldBrush;
    HPEN hOldPen;
    BYTE string[100];
    

    if (hWnd==NULL) return FALSE;
    if (NULL==(w=GetWindowPointer(GetParent(hWnd)))) return FALSE;  // get the pointer to window data
    
    if (NULL==(hBM=CreateCompatibleBitmap(hTerDC,TerRect.right-TerRect.left+1,ToolBarHeight+1))) return FALSE;
    hOldBM=SelectObject(hMemDC,hBM);

    if (NULL==(hIconDC=CreateCompatibleDC(hTerDC))) return FALSE;

    SetRect(&rect,0,0,TerRect.right-TerRect.left,ToolBarHeight);

    // clear the toolbar area
    hDC=hMemDC;                    // draw via memory DC
    
    hOldBrush=SelectObject(hDC,hToolbarBrush?hToolbarBrush:GetStockObject(LTGRAY_BRUSH));
    hOldPen=SelectObject(hDC,GetStockObject(NULL_PEN));
    Rectangle(hDC,rect.left,rect.top,rect.right+1,rect.bottom+1);
    SelectObject(hDC,hOldBrush);
    SelectObject(hDC,hOldPen);
    
    // get current font
    CurFont=GetEffectiveCfmt(w);
    if (CurFont<0) CurFont=0;

    // draw toolbar components
    y=0;
    for (i=0;i<MAX_TLB_LINES;i++) {
      DrawShadowLine(w,hDC,rect.left,y,rect.right,y,GetStockObject(BLACK_PEN),GetStockObject(WHITE_PEN));
      
      for (j=0;j<TlbItemCount[i];j++) {
         pTlb=&(TlbId[i][j]);
         id=pTlb->id;
      
         if (id==TLB_LINE) {                 // draw a vertical line
             DrawShadowLine(w,hDC,pTlb->x,pTlb->y,pTlb->x,pTlb->y+pTlb->height,GetStockObject(BLACK_PEN),GetStockObject(WHITE_PEN));
         } 
         
         // set combo-box
         else if (id==TLB_TYPEFACE) {       // update typeface
             if (CurSID>=0) SetDlgItemText(hToolBarWnd,id,StyleId[CurSID].TypeFace);
             else           SetDlgItemText(hToolBarWnd,id,TerFont[CurFont].TypeFace);
         }
         else if (id==TLB_POINTSIZE) { // update pointsize
              if (CurSID>=0) CurTwipsSize=StyleId[CurSID].TwipsSize;
              else           CurTwipsSize=TerFont[CurFont].TwipsSize;
              CurPointSize=CurTwipsSize/20;
              wsprintf(string,"%2d",CurPointSize);
              if ((CurPointSize*20)<CurTwipsSize) lstrcat(string,".5");   // show fractional pointsize
              if (CurPointSize==0) string[0]=0;
              SetDlgItemText(hToolBarWnd,TLB_POINTSIZE,string);
         } 
         else if (id==TLB_ZOOM) { // update zoom percent
              wsprintf(string,"%d",ZoomPercent);
              lstrcat(string,"%");
              SetDlgItemText(hToolBarWnd,TLB_ZOOM,string);
         } 
         else if (id==TLB_STYLE) {     // update style box
             int NewSID,idx;

             // set the stylesheet item
             if (PfmtId[pfmt(CurLine)].StyId) NewSID=PfmtId[pfmt(CurLine)].StyId;
             else {
                int sid=TerFont[CurFont].CharStyId;
                if(sid!=1 && StyleId[sid].InUse) NewSID=sid;
                else                             NewSID=0;     // normal paragraph
             }
             idx=(int)SendDlgItemMessage(hToolBarWnd,TLB_STYLE,CB_SELECTSTRING,(WPARAM)-1,(LPARAM)(LPBYTE)StyleId[NewSID].name);
             if (idx==CB_ERR) {        // refill the style box
                FillStyleBox(w,hToolBarWnd,TLB_STYLE,0,TRUE,FALSE);   // fill initially
                SendDlgItemMessage(hToolBarWnd,TLB_STYLE,CB_SELECTSTRING,(WPARAM)-1,(LPARAM)(LPBYTE)StyleId[NewSID].name);
             } 
         } 
         else if (id==TLB_DEC_INDENT) {     // decrease indent
            ResetUintFlag(pTlb->flags,TLBFLAG_ENABLED);
            ResetUintFlag(pTlb->flags,TLBFLAG_CHECKED);

            if (TerMenuEnable(hTerWnd,ID_LEFT_INDENT)==MF_ENABLED) pTlb->flags|=TLBFLAG_ENABLED;
            PaintToolbarIcon(w,hDC,hIconDC,pTlb);
         } 
          
         // set the enabled and checked status
         if (pTlb->CmdId>0) {
            int cmd=pTlb->CmdId;
            //if (LineFlags2(CurLine)&LFLAG2_RTL) {
            //   if (pTlb->CmdId==ID_LEFT_INDENT) cmd=ID_RIGHT_INDENT;
            //} 
            
            ResetUintFlag(pTlb->flags,TLBFLAG_ENABLED);
            ResetUintFlag(pTlb->flags,TLBFLAG_CHECKED);

            if (TerMenuEnable(hTerWnd,XlateCommandId(w,cmd))==MF_ENABLED) pTlb->flags|=TLBFLAG_ENABLED;
            if (id==TlbIdClicked || TerMenuSelect(hTerWnd,XlateCommandId(w,cmd))==MF_CHECKED) pTlb->flags|=TLBFLAG_CHECKED;
            
            PaintToolbarIcon(w,hDC,hIconDC,pTlb);
         }
      
      }

      y+=(TLB_ICON_HEIGHT+2*TLB_MARGIN);
    }


    // bit-blast to the toolbar window
    BitBlt(hDestDC,0,0,rect.right-rect.left,ToolBarHeight,hDC,0,0,SRCCOPY);

    //END:

    DeleteDC(hIconDC);   // delete the DC used to hold icon bitmaps

    SelectObject(hDC,hOldBM);
    DeleteObject(hBM);


    return TRUE;
} 

/******************************************************************************
    PaintToolbarIcon:
    Paint a toolbar icon
******************************************************************************/
BOOL PaintToolbarIcon(PTERWND w,HDC hDestDC,HDC hIconDC,struct StrTlb far *pTlb)
{
    HBITMAP hOldIconBM;
    BOOL TempDestDC=FALSE,TempIconDC=FALSE;
    HPEN hDarkPen,hWhitePen;
    BOOL dark,checked=FALSE;

    if (pTlb->flags&TLBFLAG_CHECKED && !(pTlb->flags&TLBFLAG_HILIGHTED)) checked=TRUE;
    
 
    // check if device context need to be created
    if (!hDestDC) {
       hDestDC=GetDC(hToolBarWnd);
       TempDestDC=TRUE;
    } 
    
    if (!hIconDC) {
       if (NULL==(hIconDC=CreateCompatibleDC(hTerDC))) return FALSE;
       TempIconDC=TRUE;
    }

    if (pTlb->pInfo!=NULL) {  // custom icon
       int y=pTlb->ImageHeight-pTlb->height;   // bmp scan go from bottom to top, so provide an adjustment
       if (y<0) y=0;
       StretchDIBits(hDestDC,pTlb->x,pTlb->y,pTlb->width,pTlb->height,0,y,pTlb->width,pTlb->height,pTlb->pImage,(LPBITMAPINFO)pTlb->pInfo,DIB_RGB_COLORS,SRCCOPY);
    }
    else { 
       hOldIconBM=SelectObject(hIconDC,hToolbarBM);
       BitBlt(hDestDC,pTlb->x,pTlb->y,pTlb->width,pTlb->height,hIconDC,pTlb->IconX,pTlb->IconY,SRCCOPY);
       SelectObject(hIconDC,hOldIconBM);
    }
    
    if (hToolbarBrush) ColorToolbarIcon(w,hDestDC,pTlb->x,pTlb->y,pTlb->width,pTlb->height);
    if (!(pTlb->flags&TLBFLAG_ENABLED)) GrayToolbarIcon(w,hDestDC,pTlb->x,pTlb->y,pTlb->width,pTlb->height);
    if (checked) CheckToolbarIcon(w,hDestDC,pTlb->x,pTlb->y,pTlb->width,pTlb->height);


    // draw hilight
    if(pTlb->flags&TLBFLAG_HILIGHTED) {
       hDarkPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_GRAYTEXT));
       hWhitePen=GetStockObject(WHITE_PEN);

       // draw top and left
       dark=(pTlb->flags&TLBFLAG_CHECKED);
       DrawShadowLine(w,hDestDC,pTlb->x,pTlb->y,pTlb->x+pTlb->width,pTlb->y,(dark?hDarkPen:hWhitePen),NULL);
       DrawShadowLine(w,hDestDC,pTlb->x,pTlb->y,pTlb->x,pTlb->y+pTlb->height,(dark?hDarkPen:hWhitePen),NULL);

       // draw bottom and right
       dark=!dark;
       DrawShadowLine(w,hDestDC,pTlb->x,pTlb->y+pTlb->height-1,pTlb->x+pTlb->width,pTlb->y+pTlb->height-1,(dark?hDarkPen:hWhitePen),NULL);
       DrawShadowLine(w,hDestDC,pTlb->x+pTlb->width-1,pTlb->y,pTlb->x+pTlb->width-1,pTlb->y+pTlb->height,(dark?hDarkPen:hWhitePen),NULL);

       DeleteObject(hDarkPen);
    } 


    //END:
    
    if (TempDestDC) ReleaseDC(hToolBarWnd,hDestDC);
    if (TempIconDC) DeleteDC(hIconDC);   // delete the DC used to hold icon bitmaps
 
    return TRUE;
}
 
/******************************************************************************
    ColorToolbarIcon:
    Color the toolbar icons to the StatusBkColor
******************************************************************************/
BOOL ColorToolbarIcon(PTERWND w,HDC hDC,int x, int y, int width, int height)
{
    int i,j;
    int LastX=x+width-1;     // last pixel not part of the rectangle
    int LastY=y+height-1;
    COLORREF color1,BkndColor;

    BkndColor=GetPixel(hDC,x,y);   // background color

    for (i=x;i<=LastX;i++) {
       for (j=y;j<=LastY;j++) {
          color1=GetPixel(hDC,i,j);
          if (color1==BkndColor) SetPixel(hDC,i,j,StatusBkColor);
       } 
    } 
 
    return TRUE;
}
 
/******************************************************************************
    GrayToolbarIcon:
    Gray a toolbar icon
******************************************************************************/
BOOL GrayToolbarIcon(PTERWND w,HDC hDC,int x, int y, int width, int height)
{
    int i,j;
    int LastX=x+width-1;     // last pixel not part of the rectangle
    int LastY=y+height-1;
    COLORREF color1,color2;
    COLORREF DkGray=0x808080;
    COLORREF white=0xFFFFFF;

    x--;  // go past the border pixel
    y--;
    LastX--;
    LastY--;

    for (i=x;i<=LastX;i++) {
       for (j=y;j<=LastY;j++) {
          color1=GetPixel(hDC,i,j);
          if (color1==0 || color1==0x800000) {
             SetPixel(hDC,i,j,DkGray);      // gray out the pixel

             color2=GetPixel(hDC,i+1,j+1);  // get the diagonal pixel
             if (color1!=color2) SetPixel(hDC,i+1,j+1,white);
          } 
       } 
    } 
 
    return TRUE;
}
 
/******************************************************************************
    CheckToolbarIcon:
    check (brighten) a toolbar icon
******************************************************************************/
BOOL CheckToolbarIcon(PTERWND w,HDC hDC,int x, int y, int width, int height)
{
    int i,j;
    int LastX=x+width-1;
    int LastY=y+height-1;
    COLORREF color1;
    COLORREF DkGray=0x808080,BkndColor;
    COLORREF white=0xFFFFFF;
    BOOL  StartWhite,start;

    BkndColor=GetPixel(hDC,x,y);   // background color

    // set the borders
    for (i=x;i<=LastX;i++) {
       SetPixel(hDC,i,y,DkGray);      // top border
       if (i>x) SetPixel(hDC,i,LastY,white);  // bottom border
    }
    for (i=y;i<=LastY;i++) {
       SetPixel(hDC,x,i,DkGray);      // left border
       if (i>y) SetPixel(hDC,LastX,i,white);   // right border
    }
    
    x++;  // go past the border pixel
    y++;
    LastX--;
    LastY--;

    StartWhite=TRUE;
    for (i=x;i<=LastX;i++) {
       start=(StartWhite?y:(y+1));
       for (j=start;j<=LastY;j=j+2) {     // process every other pixels
          color1=GetPixel(hDC,i,j);
          if (color1==BkndColor) SetPixel(hDC,i,j,white);      // apply white the pixel
       }
       StartWhite=!StartWhite; 
    } 
 
    return TRUE;
}
 
/******************************************************************************
    PaintBalloon:
    Paint a toolbar balloon
******************************************************************************/
BOOL PaintBalloon(PTERWND w)
{
    HDC hDC,hTempDC;
    HFONT hOldFont=NULL;
    HPEN hOldPen=NULL,hShadowPen;
    HPEN hBlackPen=GetStockObject(BLACK_PEN);
    LPBYTE pText;
    SIZE size;
    int OldBkMode;
    HBRUSH hBr;
    POINT pt;
    HWND hCombo;

    if (!pBlnTlb) return FALSE;

    // delete any existing balloon background
    if (hBlnBM) {
      DeleteObject(hBlnBM);
      hBlnBM=NULL;
    }

    // don't display ballon if any combo-edit box has focus
    hCombo=GetParent(GetFocus());   // handle of the combo control if edit control has focus
    if (hCombo && hToolBarWnd==GetParent(hCombo)) return TRUE;   // handle of the toolbar window

    // get balloon item
    pText=pBlnTlb->pBalloon;
    if (!pText || lstrlen(pText)==0) return TRUE;

    hDC=GetDC(NULL);        // use the desktop DC
    hOldFont=SelectObject(hDC,hRulerFont);
    hOldPen=SelectObject(hDC,hBlackPen);

    // build the balloon rectangle
    GetCursorPos(&pt);
    BlnRect.left=pt.x;
    BlnRect.top=pt.y+20;
    
    GetTextExtentPoint(hDC,pText,lstrlen(pText),&size);
    BlnRect.right=BlnRect.left+size.cx+5;
    BlnRect.bottom=BlnRect.top+size.cy+5;

    // save the ballon background
    hTempDC=CreateCompatibleDC(hDC);
    if (hTempDC) {
       int width=BlnRect.right-BlnRect.left;
       int height=BlnRect.bottom-BlnRect.top;
       hBlnBM=CreateCompatibleBitmap(hDC,width+1,height+1);
       if (hBlnBM) {
          HBITMAP hOldBM=SelectObject(hTempDC,hBlnBM);
          BitBlt(hTempDC,0,0,width,height,hDC,BlnRect.left,BlnRect.top,SRCCOPY);
          SelectObject(hTempDC,hOldBM);
       }
       DeleteDC(hTempDC); 
    } 
    
    // draw the rectangle
    hShadowPen=CreatePen(PS_SOLID,0,0xAFAFAF);
    hBr=CreateSolidBrush(RGB(255,255,225));  // light yellow
    FillRect(hDC,&BlnRect,hBr);
    DeleteObject(hBr);

    DrawShadowBox(w,hDC,BlnRect.left,BlnRect.top,BlnRect.right-1,BlnRect.bottom-1,hShadowPen,hBlackPen);

    // write the page number
    OldBkMode=SetBkMode(hDC,TRANSPARENT);
    TextOut(hDC,BlnRect.left+3,BlnRect.top+3,pText,lstrlen(pText));
    SetBkMode(hDC,OldBkMode);

    // END:
    if (hOldFont) SelectObject(hDC,hOldFont);
    if (hOldPen) SelectObject(hDC,hOldPen);
    
    DeleteObject(hShadowPen);
    
    ReleaseDC(NULL,hDC);

    // set timer to erase balloon
    SetTimer(hToolBarWnd,TIMER_TLB_BALLOON_OFF,10000,NULL);  // timer to erase the balloon

    return TRUE;
}
 
/******************************************************************************
    EraseBalloon:
    Restore the background behind the balloon
******************************************************************************/
BOOL EraseBalloon(PTERWND w)
{
    HDC hDC,hTempDC;

    if (!hBlnBM) return TRUE;  // nothing to erase
    
    hDC=GetDC(NULL);        // use the desktop DC

    hTempDC=CreateCompatibleDC(hDC);
    if (hTempDC) {
       int width=BlnRect.right-BlnRect.left;
       int height=BlnRect.bottom-BlnRect.top;
       HBITMAP hOldBM=SelectObject(hTempDC,hBlnBM);
       BitBlt(hDC,BlnRect.left,BlnRect.top,width,height,hTempDC,0,0,SRCCOPY);
       SelectObject(hTempDC,hOldBM);
       DeleteDC(hTempDC); 
    } 

    // END:
    ReleaseDC(NULL,hDC);
    
    DeleteObject(hBlnBM);
    hBlnBM=NULL;

    return TRUE;
}
 
/******************************************************************************
    InvalidateToolbarComboItems:
    Invalidate combo items in the toolbar
******************************************************************************/
BOOL InvalidateToolbarComboItems(HWND hWnd)
{
    PTERWND w;
    struct StrTlb far *pTlb;
    int i,j,id;

    if (NULL==(w=GetWindowPointer(GetParent(hWnd)))) return FALSE;  // get the pointer to window data

    for (i=0;i<MAX_TLB_LINES;i++) {
      for (j=0;j<TlbItemCount[i];j++) {
         pTlb=&(TlbId[i][j]);
         id=pTlb->id;

         if (pTlb->hWnd) InvalidateRect(pTlb->hWnd,NULL,TRUE);
      }
    }

    return TRUE;
}

/******************************************************************************
    TlbLButtonDown:
    Left button click on the toolbar
******************************************************************************/
BOOL TlbLButtonDown(HWND hWnd, LPARAM lParam)
{
    PTERWND w;
    struct StrTlb far *pTlb;
    HDC hDC;

    if (NULL==(w=GetWindowPointer(GetParent(hWnd)))) return FALSE;  // get the pointer to window data

    // get the element under the mouse
    if (NULL==(pTlb=TlbMousePos(w,lParam))) return TRUE;


    if ((pTlb->flags&TLBFLAG_MOUSE) && (pTlb->flags&TLBFLAG_ENABLED)) {
       int CmdId=pTlb->CmdId;


       // kill any pending timers
       KillTimer(hWnd,TIMER_TLB_BALLOON);
       KillTimer(hWnd,TIMER_TLB_BALLOON_OFF);
       
       // show the current button as clicked
       hDC=GetDC(hToolBarWnd);
       TlbIdClicked=pTlb->id;
       PaintToolbar(hToolBarWnd,hDC);
       ReleaseDC(hToolBarWnd,hDC);
       //TlbIdClicked=-1;

       if (CmdId) {
          if (CmdId==ID_LEFT_INDENT && LineFlags2(CurLine)&LFLAG2_RTL) CmdId=ID_RIGHT_INDENT;  // WM_COMMAND would reverse it back to left indent
          PostMessage(hTerWnd,WM_COMMAND,CmdId,0L);
       }
    }

    return TRUE;
}

/******************************************************************************
    TlbMouseMove:
    Moues move in the toolbar
******************************************************************************/
BOOL TlbMouseMove(HWND hWnd, LPARAM lParam)
{
    PTERWND w;
    struct StrTlb far *pMouseTlb;
    struct StrTlb far *pTlb;
    int i,j;
    BOOL ItemHilighted=FALSE,MouseOverSpacer=FALSE;

    if (NULL==(w=GetWindowPointer(GetParent(hWnd)))) return FALSE;  // get the pointer to window data

    if (lParam==0) goto END;         // mouse out of the toolbar

    // get the element under the mouse
    pMouseTlb=TlbMousePos(w,lParam);

    // find the icon being clicked
    for (i=0;i<MAX_TLB_LINES;i++) {
      for (j=0;j<TlbItemCount[i];j++) {
         pTlb=&(TlbId[i][j]);
         if (pTlb->id==TLB_SPACER || pTlb->id==TLB_LINE) {
            MouseOverSpacer=TRUE;
            continue;
         }

         if (pTlb==pMouseTlb) {     // hilight, if not already hilighted
            if (!(pTlb->flags&TLBFLAG_HILIGHTED)) {
               if (!(pTlb->flags&TLBFLAG_COMBO)) {
                  pTlb->flags|=TLBFLAG_HILIGHTED;
                  PaintToolbarIcon(w,NULL,NULL,pTlb);
               }
               
               if (hBlnBM) {     // delete old ballon and display new one
                  EraseBalloon(w);
                  pBlnTlb=pTlb;
                  PaintBalloon(w);
               } 
               else {            // set timer to display ballon on delay
                  pBlnTlb=pTlb;
                  SetTimer(hWnd,TIMER_TLB_BALLOON,1200,NULL);  // wait to show the balloon
               }
            }
            
            SetTimer(hWnd,TIMER_TLB_HILIGHT,50,NULL);  // this time is used to check when the mouse leaves the toolbar
            if (!(pTlb->flags&TLBFLAG_COMBO)) ItemHilighted=TRUE;
         }
         else {         // unhighlighted
            if ((pTlb->flags&TLBFLAG_HILIGHTED)) {
               ResetUintFlag(pTlb->flags,TLBFLAG_HILIGHTED);
               PaintToolbarIcon(w,NULL,NULL,pTlb);
            } 
         }  
      }
    }

    END:
    // when mouse going out of toolbar
    if (((!ItemHilighted) && (!MouseOverSpacer)) || lParam==0) {
       if (hBlnBM) EraseBalloon(w);
       KillTimer(hWnd,TIMER_TLB_BALLOON);
       KillTimer(hWnd,TIMER_TLB_BALLOON_OFF);
       if (lParam==0) KillTimer(hWnd,TIMER_TLB_HILIGHT);
    } 

    return TRUE;
}

/******************************************************************************
    TlbTimer:
    toolbar timer handler
******************************************************************************/
BOOL TlbTimer(HWND hWnd, int TimerId)
{
    PTERWND w;
    POINT pt;
    RECT rect;
    BOOL MouseInToolbar;

    if (NULL==(w=GetWindowPointer(GetParent(hWnd)))) return FALSE;  // get the pointer to window data

    // check if mouse is in the window
    GetCursorPos(&pt);
    GetWindowRect(hWnd,&rect);
    MouseInToolbar=(pt.x>=rect.left && pt.y>=rect.top && pt.x<rect.right && pt.y<rect.bottom);

    if (TimerId==TIMER_TLB_BALLOON) {        // show balloon
       KillTimer(hWnd,TIMER_TLB_BALLOON);
       if (MouseInToolbar) PaintBalloon(w);
    } 
    else if (TimerId==TIMER_TLB_BALLOON_OFF) {   // erase balloon
       KillTimer(hWnd,TIMER_TLB_BALLOON_OFF);
       if (hBlnBM) EraseBalloon(w);
    } 
    else if (TimerId==TIMER_TLB_HILIGHT) {   // deselect when the mouse leaves the window
       if (!MouseInToolbar) TlbMouseMove(hWnd,0);
    }

    return TRUE;
}

/******************************************************************************
    TlbMousePos:
    Return the pointer to the toolbar icon pointed by the mouse.  This function
    returns FALSE if mouse is not over a toolbar element
******************************************************************************/
struct StrTlb far *TlbMousePos(PTERWND w, LPARAM lParam)
{
    struct StrTlb far *pTlb;
    int i,j,x,y,height;

    // get the mouse x/y
    x=(int)(short)LOWORD(lParam); // casting preserves sign
    y=(int)(short)HIWORD(lParam);

    // find the icon being clicked
    for (i=0;i<MAX_TLB_LINES;i++) {
      for (j=0;j<TlbItemCount[i];j++) {
         pTlb=&(TlbId[i][j]);
      
         height=(pTlb->flags&TLBFLAG_COMBO)?TLB_ICON_HEIGHT:pTlb->height;

         if (x<pTlb->x || x>(pTlb->x+pTlb->width)) continue;
         if (y<pTlb->y || y>(pTlb->y+height)) continue;

         return pTlb;
      }
    }

    return (struct StrTlb far *)NULL;
}
 
/****************************************************************************
    BarComboSubclass:
    This routine subclass the font input for the tool bar.  It supplies the
    carriage return key.
****************************************************************************/
BOOL CALLBACK _export BarComboSubclass(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    HWND hTblWnd,hEditWnd,hCombo;
    PTERWND w;
    int id;
    LPVOID OrigProc=NULL;

    // get the parent window handle and our context
    id=GetDlgCtrlID(hWnd);        // control id
    if (id==TLB_STYLE) {          // no edit box in this combo box
       hCombo=hWnd;
    } 
    else {
       hCombo=GetParent(hWnd);       // handle of the combo control
       id=GetDlgCtrlID(hCombo);      // control id
    }
    hTblWnd=GetParent(hCombo);    // handle of the toolbar window
    hEditWnd=GetParent(hTblWnd);   // handle of TE window
    
    if (NULL==(w=GetWindowPointer(hEditWnd))) return FALSE;  // get the pointer to window data

    if      (id==TLB_TYPEFACE)  OrigProc=OrigBarFontProc;
    else if (id==TLB_POINTSIZE) OrigProc=OrigBarPointProc;
    else if (id==TLB_ZOOM)      OrigProc=OrigBarZoomProc;
    else if (id==TLB_STYLE)     OrigProc=OrigBarStyleProc;

    // Notify the parent (dialog) window
    if (message==WM_CHAR && wParam==VK_RETURN) {
        #if defined(WIN32)
            TlbWndProc(hTblWnd,WM_COMMAND,(WPARAM)MAKELONG(id,CBN_DBLCLK),(LPARAM)hCombo);
        #else
            TlbWndProc(hTblWnd,WM_COMMAND,id,((DWORD)CBN_DBLCLK)<<16);
        #endif
        return TRUE;
    }
    else {
        BOOL result=(BOOL) CallWindowProc((WNDPROC)OrigProc,hWnd,message,wParam,lParam);

        if (message==WM_MOUSEMOVE) {      // let toolbar window examine it
           int x=(int)(short)LOWORD(lParam); // casting preserves sign
           int y=(int)(short)HIWORD(lParam);
           RECT rect1,rect2;
           BOOL dropped=(BOOL)SendMessage(hTblWnd,CB_GETDROPPEDSTATE,0,0L);

           if (!dropped) {
              GetWindowRect(hWnd,&rect1);      // edit box rectangle
              GetWindowRect(hTblWnd,&rect2);   // toolbar window rectangle
           
              x+=(rect1.left-rect2.left);
              y+=(rect1.top-rect2.top);

              lParam=(((DWORD)(WORD)y)<<16)|(WORD)x;

              PostMessage(hTblWnd,message,wParam,lParam);
           }
        } 

        return result;
    }
}


