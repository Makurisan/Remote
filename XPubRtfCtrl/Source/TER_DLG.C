/*==============================================================================
   TER_DLG.C
   TER Dialog call back functions

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
    PasteSpecialParam:
    This routine handles to dialog box for the OLE paste objects.
******************************************************************************/
BOOL CALLBACK _export PasteSpecialParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    LPBYTE PasteFmts;
    int i;
    BOOL flag;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            PasteFmts=dlg->data;                     // available formats

            // set the description
            SetDlgItemText(hDlg,IDC_PASTE_SOURCE,TempString);
            SetDlgItemText(hDlg,IDC_PASTE_ITEM,TempString1);


            // Fill the list box
            for (i=0;i<MAX_PASTE_FORMATS;i++) {    // add format names to the list box
               if (PasteFmts[i]) {
                  if (i==PASTE_OBJECT) {                        // add object string
                     lstrcat(TempString, " ");
                     lstrcat(TempString,PASTE_DESC_OBJECT);
                     SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_ADDSTRING, 0, (DWORD)(LPBYTE)(TempString));
                  }
                  else if (i==PASTE_RTF)  SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_ADDSTRING, 0, (DWORD)(LPBYTE)(PASTE_DESC_RTF));
                  else if (i==PASTE_TEXT) SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_ADDSTRING, 0, (DWORD)(LPBYTE)(PASTE_DESC_TEXT));
                  else if (i==PASTE_PICT) SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_ADDSTRING, 0, (DWORD)(LPBYTE)(PASTE_DESC_PICT));
                  else if (i==PASTE_BM)   SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_ADDSTRING, 0, (DWORD)(LPBYTE)(PASTE_DESC_BM));
                  else if (i==PASTE_DIB)  SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_ADDSTRING, 0, (DWORD)(LPBYTE)(PASTE_DESC_DIB));
               }
            }
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_SETCURSEL, 0,0L);

            // Enable PasteLink button if Object format specified
            flag=FALSE;
            if (PasteFmts[0] && IsClipboardFormatAvailable(ObjectLinkClipFormat)) flag=TRUE;
            EnableWindow(GetDlgItem(hDlg,IDC_PASTE_LINK),flag);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)==LBN_SELCHANGE) {     // enable/disable paste link
                       i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                       if (i==0 && IsClipboardFormatAvailable(ObjectLinkClipFormat)) {
                          SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETTEXT, (WPARAM)  i,(DWORD)(LPBYTE)TempString1);
                          if (lstrcmpi(TempString,TempString1)==0)
                               EnableWindow(GetDlgItem(hDlg,IDC_PASTE_LINK),TRUE);
                          else EnableWindow(GetDlgItem(hDlg,IDC_PASTE_LINK),FALSE);
                       }
                       else EnableWindow(GetDlgItem(hDlg,IDC_PASTE_LINK),FALSE);
                       return TRUE;
                   }
                   else if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDC_PASTE:
                   i=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),LB_GETCURSEL, 0,0L);
                   EndDialog(hDlg, i);
                   return (TRUE);

                case IDC_PASTE_LINK:
                   i=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),LB_GETCURSEL, 0,0L);
                   EndDialog(hDlg, i+MAX_PASTE_FORMATS);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg,-1);
                   return (TRUE);

                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    InsertActiveXParam:
    This routine handles to dialog box for the OLE paste objects.
******************************************************************************/
int CALLBACK _export InsertActiveXParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    DWORD SubKey=0;
    long BufSize;
    BYTE string[100],clsid[100],key[100];
    int i;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // Fill the list box
            for (SubKey=0;TRUE;SubKey++) {
               if (ERROR_SUCCESS!=RegEnumKey(HKEY_CLASSES_ROOT,SubKey,TempString,sizeof(TempString)-1)) break;

               // check if it is an OCX control
               lstrcpy(key,TempString);
               lstrcat(key,"\\CLSID"); // look for this clsid subkey now
               BufSize=sizeof(clsid)-1;
               if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,key,clsid,&BufSize)) continue;  // CLSID subkey not found for this key
               lstrcpy(key,"CLSID\\");
               lstrcat(key,clsid);
               lstrcat(key,"\\Control");
               BufSize=sizeof(TempString1)-1;
               if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,key,TempString1,&BufSize)) continue;  // 'Control' subkey not found for this CLSID

               // get the class description and add to the list box
               BufSize=sizeof(string)-1;
               if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString,string,&BufSize)) continue;  // class description missing
               StrTrim(string);
               if (lstrlen(string)>0) StuffListBox(hDlg,IDC_LIST_BOX,string,SubKey);
            }
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_SETCURSEL, 0,0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                   i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                   i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETITEMDATA, i,0L);
                   EndDialog(hDlg, i);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg,-1);
                   return (TRUE);

                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    InsertObjectParam:
    This routine handles to dialog box for the OLE paste objects.
******************************************************************************/
int CALLBACK _export InsertObjectParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    DWORD SubKey=0;
    long BufSize;
    BYTE string[100];
    LPBYTE SaveName[101];
    int i,MaxNames=100,TotalNames=0;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // Fill the list box
            for (SubKey=0;TRUE;SubKey++) {
               if (ERROR_SUCCESS!=RegEnumKey(HKEY_CLASSES_ROOT,SubKey,TempString,sizeof(TempString)-1)) break;
               // check if it is an OLE server
               lstrcpy(TempString1,TempString);
               lstrcat(TempString1,"\\protocol\\StdFileEditing\\server"); // look for this key now
               BufSize=sizeof(string)-1;
               if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString1,string,&BufSize)) {
                  lstrcpy(TempString1,TempString);
                  lstrcat(TempString1,"\\Insertable"); // look for this key now
                  BufSize=sizeof(string)-1;
                  if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString1,string,&BufSize)) continue;
               } 
               
               // get the class description and add to the list box
               BufSize=sizeof(string)-1;
               if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString,string,&BufSize)) continue;  // class description missing
               StrTrim(string);
               if (lstrlen(string)>0) {
                  // check if this name already picked
                  for (i=0;i<TotalNames;i++) if (lstrcmp(string,SaveName[i])==0) break;
                  if (i==TotalNames) {
                     StuffListBox(hDlg,IDC_LIST_BOX,string,SubKey);

                     // save this name in the array
                     if (TotalNames<MaxNames) {
                        SaveName[TotalNames]=MemAlloc(lstrlen(string)+1);
                        lstrcpy(SaveName[TotalNames],string);
                        TotalNames++;
                     } 
                  }
               }
            }
            
            // release name buffer
            for (i=0;i<TotalNames;i++) MemFree(SaveName[i]);
            
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_SETCURSEL, 0,0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                   i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                   i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETITEMDATA, i,0L);
                   EndDialog(hDlg, i);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg,-1);
                   return (TRUE);

                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CharSpaceParam:
    Process messages for 'SearchParam' dialog box
    This dialog box accepts the parameters to locate a string.
******************************************************************************/
BOOL CALLBACK _export CharSpaceParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int expand;
    BOOL reset;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            expand=DlgInt1;                  // value of the adjustment

            SendDlgItemMessage(hDlg,IDC_EXPAND,BM_SETCHECK, expand>0, 0L);
            SendDlgItemMessage(hDlg,IDC_REDUCE,BM_SETCHECK, expand<0, 0L);
            SendDlgItemMessage(hDlg,IDC_RESET,BM_SETCHECK,  expand==0, 0L);

            if (expand==0) {
               SetDlgItemInt(hDlg,IDC_ADJ,20,0);     // default value
               EnableWindow(GetDlgItem(hDlg,IDC_ADJ),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_ADJ_TITLE),FALSE);
            }
            else SetDlgItemInt(hDlg,IDC_ADJ,abs(expand),0);     // current value

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_EXPAND:
                case IDC_REDUCE:
                case IDC_RESET:
                   reset=(BOOL)SendDlgItemMessage(hDlg,IDC_RESET,BM_GETCHECK, 0, 0L);
                   EnableWindow(GetDlgItem(hDlg,IDC_ADJ),!reset);
                   EnableWindow(GetDlgItem(hDlg,IDC_ADJ_TITLE),!reset);
                   break;

                case IDOK:
                   DlgInt1=0;        // value to pass
                   reset=(BOOL)SendDlgItemMessage(hDlg,IDC_RESET,BM_GETCHECK, 0, 0L);
                   if (!reset) {
                      if (!AssignNumVal(hDlg,IDC_ADJ,&expand)) return (TRUE);
                      if (expand<1 || expand>5000) {
                         SetFocus(GetDlgItem(hDlg,IDC_ADJ));
                         MessageBeep(0);
                         return TRUE;
                      }
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_EXPAND,BM_GETCHECK, 0, 0L))
                           DlgInt1=expand;
                      else DlgInt1=-expand;
                   }

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    DrawObjectParam:
    Process messages for 'SearchParam' dialog box
    This dialog box accepts the parameters to locate a string.
******************************************************************************/
int CALLBACK _export DrawObjectParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int ObjType;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            SendDlgItemMessage(hDlg,IDC_TEXT_BOX,BM_SETCHECK, TRUE, 0L);
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   if      ((BOOL)SendDlgItemMessage(hDlg,IDC_TEXT_BOX,BM_GETCHECK, 0, 0L)) ObjType=DOB_TEXT_BOX;
                   else if ((BOOL)SendDlgItemMessage(hDlg,IDC_RECT,BM_GETCHECK, 0, 0L))     ObjType=DOB_RECT;
                   else                                                                     ObjType=DOB_LINE;

                   EndDialog(hDlg, ObjType);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    SearchParam:
    Process messages for 'SearchParam' dialog box
    This dialog box accepts the parameters to locate a string.
******************************************************************************/
BOOL CALLBACK _export SearchParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;


    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            SetDlgItemText(hDlg,IDC_SEARCH_STRING,SearchString);
            SetGuiFont(w,hDlg,IDC_SEARCH_STRING);

            SendMessage(GetDlgItem(hDlg,IDC_SEARCH_CASE),BM_SETCHECK, SearchFlags&SRCH_CASE, 0L);
            SearchDirection='E';             // search entire file by default
            SendMessage(GetDlgItem(hDlg,IDC_SEARCH_BEGIN),BM_SETCHECK, TRUE, 0L);
            SendMessage(GetDlgItem(hDlg,IDOK),BM_SETCHECK, TRUE, 0L);
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   GetDlgItemText(hDlg,IDC_SEARCH_STRING,SearchString,39);

                   SearchFlags=ResetUintFlag(SearchFlags,SRCH_CASE);
                   if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_SEARCH_CASE),BM_GETCHECK, 0, 0L)) SearchFlags|=SRCH_CASE;

                   if      (SendMessage(GetDlgItem(hDlg,IDC_SEARCH_BEGIN),BM_GETCHECK, 0, 0L)) SearchDirection='E';  // entire file
                   else if (SendMessage(GetDlgItem(hDlg,IDC_SEARCH_FOR),BM_GETCHECK, 0, 0L))   SearchDirection='F';  // forward direction
                   else                                                                        SearchDirection='B';  // backward direction
                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ReplaceParam:
    Process messages for 'ReplaceParam' dialog box
    This dialog box accepts the parameters to replace a string.
******************************************************************************/
BOOL CALLBACK _export ReplaceParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    BOOL flag;
    PTERWND w;
    struct StrDlgParam far *dlg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            SetDlgItemText(hDlg,IDC_REPLACE_STRING,ReplaceString);
            SetDlgItemText(hDlg,IDC_REPLACE_WITH,ReplaceWith);
            SetGuiFont(w,hDlg,IDC_REPLACE_STRING);
            SetGuiFont(w,hDlg,IDC_REPLACE_WITH);

            ReplaceVerify=FALSE;
            SendMessage(GetDlgItem(hDlg,IDC_REPLACE_VERIFY),BM_SETCHECK, ReplaceVerify, 0L);
            if (HilightType==HILIGHT_LINE || HilightType==HILIGHT_CHAR) ReplaceBlock=TRUE;else ReplaceBlock=FALSE;
            SendMessage(GetDlgItem(hDlg,IDC_REPLACE_BLOCK),BM_SETCHECK, ReplaceBlock, 0L);
            if (ReplaceBlock) flag=FALSE;else flag=TRUE;
            SendMessage(GetDlgItem(hDlg,IDC_REPLACE_FILE),BM_SETCHECK, flag, 0L);
            SendMessage(GetDlgItem(hDlg,IDOK),BM_SETCHECK, FALSE, 0L);
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   GetDlgItemText(hDlg,IDC_REPLACE_STRING,ReplaceString,39);
                   GetDlgItemText(hDlg,IDC_REPLACE_WITH,ReplaceWith,39);
                   ReplaceVerify=(BOOL)SendMessage(GetDlgItem(hDlg,IDC_REPLACE_VERIFY),BM_GETCHECK, 0, 0L);
                   ReplaceBlock=(BOOL)SendMessage(GetDlgItem(hDlg,IDC_REPLACE_BLOCK),BM_GETCHECK, 0, 0L);
                   if (ReplaceBlock && HilightType!=HILIGHT_LINE && HilightType!=HILIGHT_CHAR) {
                       MessageBox(hDlg,MsgString[MSG_INVALID_SELECT],NULL,MB_OK);
                       SetFocus(GetDlgItem(hDlg,IDC_REPLACE_BLOCK));
                       return (TRUE);
                   }
                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    JumpParam:
    Process messages for 'JumpParam' dialog box
    This dialog box accepts the parameters to jump to a line number.
******************************************************************************/
int CALLBACK _export JumpParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    int LineNumber;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            SetFocus(GetDlgItem(hDlg,IDC_JUMP_TO));
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   if (!AssignNumVal(hDlg,IDC_JUMP_TO,&LineNumber)) return (TRUE);
                   EndDialog(hDlg, LineNumber);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, -1);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ZoomParam:
    Accept the zoom percentage.
******************************************************************************/
int CALLBACK _export ZoomParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int percent;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            SetDlgItemInt(hDlg,IDC_PERCENT,ZoomPercent,0);

            SetFocus(GetDlgItem(hDlg,IDC_PERCENT));
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   if (!AssignNumVal(hDlg,IDC_PERCENT,&percent)) return (TRUE);
                   if (percent<10 || percent>500) {
                      SetFocus(GetDlgItem(hDlg,IDC_PERCENT));
                      MessageBeep(0);
                   }
                   else EndDialog(hDlg, percent);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, -1);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    FootnoteParam:
    Process messages for 'FootnoteParam' dialog box
    This dialog box accepts the parameters to create a footnote.
******************************************************************************/
BOOL CALLBACK _export FootnoteParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int i;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            SendDlgItemMessage(hDlg,IDC_FNOTE_SUPSCR,BM_SETCHECK, TRUE, 0L);

            // set the max data length for the text fields
            SendDlgItemMessage(hDlg,IDC_FNOTE_CHAR,EM_LIMITTEXT,10,0L);
            SendDlgItemMessage(hDlg,IDC_FNOTE_TEXT,EM_LIMITTEXT,MAX_WIDTH-100,0L);  // 10 bytes for footnote marker and 2 byte tolerance

            SetGuiFont(w,hDlg,IDC_FNOTE_CHAR);
            SetGuiFont(w,hDlg,IDC_FNOTE_TEXT);

            SetFocus(GetDlgItem(hDlg,IDC_FNOTE_CHAR));
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   // get the fields
                   GetDlgItemText(hDlg,IDC_FNOTE_CHAR,TempString,10);
                   TempString[10]=0;
                   if (lstrlen(TempString)==0) return TRUE;

                   // no space or break characters allowed in the footnote marker
                   for (i=0;i<lstrlen(TempString);i++) {
                      if (TempString[i]==' ' || lstrchr(BreakChars,TempString[i])!=NULL) {
                         MessageBox(hDlg,MsgString[MSG_BAD_FNOTE_MARKER],NULL,MB_OK);
                         SetFocus(GetDlgItem(hDlg,IDC_FNOTE_CHAR));
                         return TRUE;
                      }
                   }

                   GetDlgItemText(hDlg,IDC_FNOTE_TEXT,TempString1,MAX_WIDTH-100+1);
                   TempString1[MAX_WIDTH-100]=0;
                   if (lstrlen(TempString1)==0) return TRUE;  // also includes the footnote character

                   // no space or break characters allowed in the footnote marker
                   for (i=0;i<lstrlen(TempString1);i++) {
                      if (lstrchr(BreakChars,TempString1[i])!=NULL) {
                         MessageBox(hDlg,MsgString[MSG_BAD_FNOTE_MARKER],NULL,MB_OK);
                         SetFocus(GetDlgItem(hDlg,IDC_FNOTE_TEXT));
                      }
                   }

                   // get superscript flag
                   DlgInt1=0;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_FNOTE_SUPSCR,BM_GETCHECK, 0, 0L)) DlgInt1=SUPSCR;

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    DataFieldParam:
    This dialog box accepts the datafield parameters
******************************************************************************/
BOOL CALLBACK _export DataFieldParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    HFONT hFont;
    LOGFONT lf;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            SetFocus(GetDlgItem(hDlg,IDC_FIELD_NAME));
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   // get the fields
                   GetDlgItemText(hDlg,IDC_FIELD_NAME,DlgText1,MAX_WIDTH);
                   DlgText1[MAX_WIDTH]=0;
                   if (lstrlen(DlgText1)==0) return TRUE;

                   GetDlgItemText(hDlg,IDC_FIELD_DATA,DlgText2,MAX_WIDTH);
                   DlgText2[MAX_WIDTH]=0;

                   // get the data field font
                   DlgInt1=0;
                   hFont=(HFONT)SendMessage(GetDlgItem(hDlg,IDC_FIELD_NAME),WM_GETFONT,0,0L);
                   if (hFont && GetObject(hFont,sizeof(lf),&lf)>0) DlgInt1=lf.lfCharSet;
                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    HyperlinkParam:
    This dialog box accepts the datafield parameters
******************************************************************************/
BOOL CALLBACK _export HyperlinkParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            SetFocus(GetDlgItem(hDlg,IDC_LINK_TEXT));
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   // get the fields
                   GetDlgItemText(hDlg,IDC_LINK_TEXT,DlgText1,MAX_WIDTH);
                   DlgText1[MAX_WIDTH]=0;
                   if (lstrlen(DlgText1)==0) return TRUE;

                   GetDlgItemText(hDlg,IDC_LINK_CODE,DlgText2,MAX_WIDTH);
                   DlgText2[MAX_WIDTH]=0;

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    InputFieldParam:
    This dialog box accepts the text input field parameters
******************************************************************************/
BOOL CALLBACK _export InputFieldParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int font;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set the font variables
            font=GetEffectiveCfmt(w);
            lstrcpy(TempString,TerFont[font].TypeFace);
            DlgInt3=TerFont[font].TwipsSize;
            DlgInt4=TerFont[font].style;
            DlgLong=(long)TerFont[font].TextColor;

            SetFocus(GetDlgItem(hDlg,IDC_FIELD_NAME));
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_FONT:
                   DlgEditFont(w,hDlg);
                   break;

                case IDOK:
                   // get the fields
                   GetDlgItemText(hDlg,IDC_FIELD_NAME,DlgText1,MAX_WIDTH);
                   DlgText1[MAX_WIDTH]=0;
                   if (lstrlen(DlgText1)==0) return TRUE;

                   GetDlgItemText(hDlg,IDC_FIELD_DATA,DlgText2,MAX_WIDTH);
                   DlgText2[MAX_WIDTH]=0;

                   DlgInt1=GetDlgItemInt(hDlg,IDC_MAX_LEN,NULL,TRUE);
                   DlgInt2=(int)SendDlgItemMessage(hDlg,IDC_BORDER,BM_GETCHECK, 0, 0L);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    EditInputFieldParam:
    This dialog box accepts the text input field parameters
******************************************************************************/
BOOL CALLBACK _export EditInputFieldParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int FieldId;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set the current data
            FieldId=DlgInt6;
            SetDlgItemText(hDlg,IDC_FIELD_NAME,DlgText1);
            SetDlgItemInt(hDlg,IDC_MAX_LEN,DlgInt1,0);
            SetDlgItemInt(hDlg,IDC_PICT_WIDTH,DlgInt5,0);
            SendDlgItemMessage(hDlg,IDC_BORDER,BM_SETCHECK, DlgInt2, 0L);

            // disable certain fields
            if (FieldId==FIELD_CHECKBOX) {
               EnableWindow(GetDlgItem(hDlg,IDC_MAX_LEN),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_PICT_WIDTH),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_BORDER),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_FONT),FALSE);
            }    

            SetFocus(GetDlgItem(hDlg,IDC_FIELD_NAME));
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_FONT:
                   DlgEditFont(w,hDlg);
                   break;

                case IDOK:
                   // get the fields
                   GetDlgItemText(hDlg,IDC_FIELD_NAME,DlgText1,MAX_WIDTH);
                   DlgText1[MAX_WIDTH]=0;
                   if (lstrlen(DlgText1)==0) return TRUE;

                   DlgInt1=GetDlgItemInt(hDlg,IDC_MAX_LEN,NULL,TRUE);
                   DlgInt5=GetDlgItemInt(hDlg,IDC_PICT_WIDTH,NULL,TRUE);
                   DlgInt2=(int)SendDlgItemMessage(hDlg,IDC_BORDER,BM_GETCHECK, 0, 0L);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CheckBoxFieldParam:
    This dialog box accepts the checkbox input field parameters
******************************************************************************/
BOOL CALLBACK _export CheckBoxFieldParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set the default checkbox size
            SetDlgItemInt(hDlg,IDC_BOXSIZE,200,0);

            SetFocus(GetDlgItem(hDlg,IDC_FIELD_NAME));
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   // get the fields
                   GetDlgItemText(hDlg,IDC_FIELD_NAME,DlgText1,MAX_WIDTH);
                   DlgText1[MAX_WIDTH]=0;
                   if (lstrlen(DlgText1)==0) return TRUE;

                   DlgInt1=GetDlgItemInt(hDlg,IDC_BOXSIZE,NULL,TRUE);
                   DlgInt2=(int)SendDlgItemMessage(hDlg,IDC_CHECKED,BM_GETCHECK, 0, 0L);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    TableParam:
    Dialog box proceeded to edit the table parameters
******************************************************************************/
BOOL CALLBACK _export TableParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            SetDlgItemInt(hDlg,IDC_TABLE_ROWS,TableRows,0);
            SetDlgItemInt(hDlg,IDC_TABLE_COLS,TableCols,0);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   if (!CheckDlgValue(hDlg,'I',IDC_TABLE_ROWS,1,999)) return (TRUE);
                   if (!CheckDlgValue(hDlg,'I',IDC_TABLE_COLS,1,30)) return (TRUE);

                   // extract the values
                   TableRows=GetDlgItemInt(hDlg,IDC_TABLE_ROWS,NULL,TRUE);
                   TableCols=GetDlgItemInt(hDlg,IDC_TABLE_COLS,NULL,TRUE);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    DeleteCellsParam:
    Select the cells to be deleted.
******************************************************************************/
int CALLBACK _export DeleteCellsParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_ROWS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_ROWS);
                   else EndDialog(hDlg, 0);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CellShadingParam:
    Get the shading percentage for the cells
******************************************************************************/
int CALLBACK _export CellShadingParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurCell,cl,shading;
    BOOL SameShading=TRUE;
    long l,EndRow;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            CurCell=cid(CurLine);

            if (HilightType!=HILIGHT_OFF) {
               shading=-1;
               if (HilightEndCol==0) EndRow=HilightEndRow-1;
               else                  EndRow=HilightEndRow;
               for (l=HilightBegRow;l<=EndRow;l++) {
                 if (!LineSelected(w,l)) continue;

                 cl=cid(l);
                 if (cl==0) continue;
                 if (shading<0) shading=cell[cl].shading;  // set shading
                 if (cell[cl].shading!=shading) {
                    SameShading=FALSE;
                    break;
                 }
               }
               if (shading<0) shading=0;
            }
            else shading=cell[CurCell].shading;

            if (SameShading) SetDlgItemInt(hDlg,IDC_CELL_SHADE,shading,0);

            if (HilightType==HILIGHT_OFF) SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_SETCHECK, TRUE, 0L);
            else                          SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   // get the shading percentage
                   CurCell=cid(CurLine);

                   if (IsBlankDlg(hDlg,IDC_CELL_SHADE)) EndDialog(hDlg,0);
                   else {
                      if (!CheckDlgValue(hDlg,'I',IDC_CELL_SHADE,0,100)) return TRUE;
                      cell[CurCell].shading=GetDlgItemInt(hDlg,IDC_CELL_SHADE,NULL,TRUE);

                      if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_GETCHECK, 0, 0L))       EndDialog(hDlg,IDC_TABLE_ALL);
                      else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                      else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                      else                                                                                 EndDialog(hDlg,IDC_TABLE_SEL_ROWS);
                   }

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CellVertAlignParam:
    Get the shading percentage for the cells
******************************************************************************/
int CALLBACK _export CellVertAlignParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurCell,cl;
    UINT align;
    BOOL SameVertAlign=TRUE,AlignSet;
    long l,EndRow;
    UINT AlignFlags=CFLAG_VALIGN_CTR|CFLAG_VALIGN_BOT|CFLAG_VALIGN_BASE;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            CurCell=cid(CurLine);

            if (HilightType!=HILIGHT_OFF) {
               align=0;
               AlignSet=FALSE;
               if (HilightEndCol==0) EndRow=HilightEndRow-1;
               else                  EndRow=HilightEndRow;
               for (l=HilightBegRow;l<=EndRow;l++) {
                 if (!LineSelected(w,l)) continue;

                 cl=cid(l);
                 if (cl==0) continue;
                 if (!AlignSet) {
                    align=cell[cl].flags&AlignFlags;  // set align
                    AlignSet=TRUE;
                 }
                 if ((cell[cl].flags&AlignFlags)!=align) {
                    SameVertAlign=FALSE;
                    break;
                 }
               }
            }
            else align=cell[CurCell].flags&AlignFlags;

            if (SameVertAlign) {
               SendDlgItemMessage(hDlg,IDC_ALIGN_TOP,BM_SETCHECK, align==0, 0L);
               SendDlgItemMessage(hDlg,IDC_ALIGN_CTR,BM_SETCHECK, align==CFLAG_VALIGN_CTR, 0L);
               SendDlgItemMessage(hDlg,IDC_ALIGN_BOT,BM_SETCHECK, align==CFLAG_VALIGN_BOT, 0L);
               SendDlgItemMessage(hDlg,IDC_ALIGN_BASE,BM_SETCHECK, align==CFLAG_VALIGN_BASE, 0L);
            }

            if (HilightType==HILIGHT_OFF) SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_SETCHECK, TRUE, 0L);
            else                          SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   // get the align percentage
                   CurCell=cid(CurLine);

                   AlignSet=FALSE;
                   if (  SendDlgItemMessage(hDlg,IDC_ALIGN_TOP,BM_GETCHECK, 0, 0L)
                      || SendDlgItemMessage(hDlg,IDC_ALIGN_CTR,BM_GETCHECK, 0, 0L)
                      || SendDlgItemMessage(hDlg,IDC_ALIGN_BOT,BM_GETCHECK, 0, 0L)
                      || SendDlgItemMessage(hDlg,IDC_ALIGN_BASE,BM_GETCHECK, 0, 0L)) AlignSet=TRUE;

                   if (!AlignSet) EndDialog(hDlg,0);
                   else {
                      align=0;
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_ALIGN_CTR,BM_GETCHECK, 0, 0L)) align=CFLAG_VALIGN_CTR;
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_ALIGN_BOT,BM_GETCHECK, 0, 0L)) align=CFLAG_VALIGN_BOT;
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_ALIGN_BASE,BM_GETCHECK, 0, 0L)) align=CFLAG_VALIGN_BASE;

                      cell[CurCell].flags=ResetUintFlag(cell[CurCell].flags,AlignFlags);
                      cell[CurCell].flags|=align;

                      if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_GETCHECK, 0, 0L))       EndDialog(hDlg,IDC_TABLE_ALL);
                      else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                      else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                      else                                                                                 EndDialog(hDlg,IDC_TABLE_SEL_ROWS);
                   }

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CellRotationParam:
    Get the cell text rotation parameters.
******************************************************************************/
int CALLBACK _export CellRotationParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurCell,cl;
    int angle;
    BOOL SameAngle=TRUE,AngleSet;
    long l,EndRow;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            CurCell=cid(CurLine);

            if (HilightType!=HILIGHT_OFF) {
               angle=0;
               AngleSet=FALSE;
               if (HilightEndCol==0) EndRow=HilightEndRow-1;
               else                  EndRow=HilightEndRow;
               for (l=HilightBegRow;l<=EndRow;l++) {
                 if (!LineSelected(w,l)) continue;

                 cl=cid(l);
                 if (cl==0) continue;
                 if (!AngleSet) {
                    angle=cell[cl].TextAngle;  // set align
                    AngleSet=TRUE;
                 }
                 if ((cell[cl].TextAngle)!=angle) {
                    SameAngle=FALSE;
                    break;
                 }
               }
            }
            else angle=cell[CurCell].TextAngle;

            if (SameAngle) {
               SendDlgItemMessage(hDlg,IDC_HORZ,BM_SETCHECK, angle==0, 0L);
               SendDlgItemMessage(hDlg,IDC_TOP_TO_BOT,BM_SETCHECK, angle==270, 0L);
               SendDlgItemMessage(hDlg,IDC_BOT_TO_TOP,BM_SETCHECK, angle==90, 0L);
            }

            SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   // get the angle percentage
                   CurCell=cid(CurLine);

                   AngleSet=FALSE;
                   if (  SendDlgItemMessage(hDlg,IDC_HORZ,BM_GETCHECK, 0, 0L)
                      || SendDlgItemMessage(hDlg,IDC_TOP_TO_BOT,BM_GETCHECK, 0, 0L)
                      || SendDlgItemMessage(hDlg,IDC_BOT_TO_TOP,BM_GETCHECK, 0, 0L)) AngleSet=true;

                   if (!AngleSet) EndDialog(hDlg,0);
                   else {
                      angle=0;
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_TOP_TO_BOT,BM_GETCHECK, 0, 0L)) angle=270;
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_BOT_TO_TOP, BM_GETCHECK,0, 0L)) angle=90;

                      cell[CurCell].TextAngle=angle;

                      if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_GETCHECK, 0, 0L))       EndDialog(hDlg,IDC_TABLE_ALL);
                      else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                      else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                      else                                                                                 EndDialog(hDlg,IDC_TABLE_SEL_ROWS);
                   }

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CellColorParam:
    Get the color for the cells
******************************************************************************/
int CALLBACK _export CellColorParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurCell,cl;
    COLORREF color;
    BOOL SameColor=TRUE;
    long l,EndRow;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            CurCell=cid(CurLine);

            if (HilightType!=HILIGHT_OFF) {
               color=0xFFFFFFFF;
               if (HilightEndCol==0) EndRow=HilightEndRow-1;
               else                  EndRow=HilightEndRow;
               for (l=HilightBegRow;l<=EndRow;l++) {
                 if (!LineSelected(w,l)) continue;

                 cl=cid(l);
                 if (cl==0) continue;
                 if (color==0xFFFFFFFF) color=cell[cl].BackColor;  // set color
                 if (cell[cl].BackColor!=color) {
                    SameColor=FALSE;
                    break;
                 }
               }
               if (color<0xFFFFFFFF) color=CLR_WHITE;
            }
            else color=cell[CurCell].BackColor;

            if (SameColor) DlgLong=(DWORD)color;
            else           DlgLong=CLR_WHITE;

            if (HilightType==HILIGHT_OFF) SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_SETCHECK, TRUE, 0L);
            else                          SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_COLOR:
                   DlgLong=(DWORD)DlgEditColor(w,hDlg,(COLORREF)DlgLong,TRUE);
                   break;

                case IDOK:
                   // get the color percentage
                   CurCell=cid(CurLine);

                   cell[CurCell].BackColor=(COLORREF)DlgLong;

                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_GETCHECK, 0, 0L))       EndDialog(hDlg,IDC_TABLE_ALL);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                   else                                                                                 EndDialog(hDlg,IDC_TABLE_SEL_ROWS);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CellWidthFlagParam:
    Get the cell width flag
******************************************************************************/
int CALLBACK _export CellWidthFlagParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            if      (DlgInt1&CFLAG_FIX_WIDTH)      SendMessage(GetDlgItem(hDlg,IDC_FIX_WIDTH),BM_SETCHECK, TRUE, 0L);
            else if (DlgInt1&CFLAG_FIX_WIDTH_PCT)  SendMessage(GetDlgItem(hDlg,IDC_FIX_WIDTH_PCT),BM_SETCHECK, TRUE, 0L);
            else                                   SendMessage(GetDlgItem(hDlg,IDC_BEST_FIT),BM_SETCHECK, TRUE, 0L);

            if (HilightType==HILIGHT_OFF) SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_SETCHECK, TRUE, 0L);
            else                          SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:

                   DlgInt1=0;
                   if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_FIX_WIDTH),BM_GETCHECK, 0, 0L)) DlgInt1=CFLAG_FIX_WIDTH;
                   if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_FIX_WIDTH_PCT),BM_GETCHECK, 0, 0L)) DlgInt1=CFLAG_FIX_WIDTH_PCT;

                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_GETCHECK, 0, 0L))       EndDialog(hDlg,IDC_TABLE_ALL);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                   else                                                                                 EndDialog(hDlg,IDC_TABLE_SEL_ROWS);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CellBorderParam:
    Select the cells to be deleted.
******************************************************************************/
int CALLBACK _export CellBorderParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurCell,width,margin;
    BYTE string[20];

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set cell border width
            if (HilightType==HILIGHT_OFF) CurCell=cid(CurLine);
            else                          CurCell=cid(HilightBegRow);

            if (cell[CurCell].border&BORDER_TOP)     width=cell[CurCell].BorderWidth[BORDER_INDEX_TOP];
            else                                     width=0;
            if (width>0 || HilightType==HILIGHT_OFF) SetDlgItemInt(hDlg,IDC_BOX_TOP,width,0);
            if (cell[CurCell].border&BORDER_BOT)     width=cell[CurCell].BorderWidth[BORDER_INDEX_BOT];
            else                                     width=0;
            if (width>0 || HilightType==HILIGHT_OFF) SetDlgItemInt(hDlg,IDC_BOX_BOT,width,0);
            if (cell[CurCell].border&BORDER_LEFT)    width=cell[CurCell].BorderWidth[BORDER_INDEX_LEFT];
            else                                     width=0;
            if (width>0 || HilightType==HILIGHT_OFF) SetDlgItemInt(hDlg,IDC_BOX_LEFT,width,0);
            if (cell[CurCell].border&BORDER_RIGHT)   width=cell[CurCell].BorderWidth[BORDER_INDEX_RIGHT];
            else                                     width=0;
            if (width>0 || HilightType==HILIGHT_OFF) SetDlgItemInt(hDlg,IDC_BOX_RIGHT,width,0);

            if (HilightType==HILIGHT_OFF) SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_SETCHECK, TRUE, 0L);
            else                          SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            if (HilightType==HILIGHT_OFF) SetDlgItemInt(hDlg,IDC_MARGIN,cell[CurCell].margin,0);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_TABLE_SEL_COLS:                    // erase existing numbers
                case IDC_TABLE_SEL_ROWS:
                   SetDlgItemText(hDlg,IDC_BOX_TOP,"");
                   SetDlgItemText(hDlg,IDC_BOX_BOT,"");
                   SetDlgItemText(hDlg,IDC_BOX_LEFT,"");
                   SetDlgItemText(hDlg,IDC_BOX_RIGHT,"");
                   break;

                case IDOK:
                   // get border width
                   CurCell=cid(CurLine);
                   cell[CurCell].BorderWidth[BORDER_INDEX_TOP]  =-1;
                   cell[CurCell].BorderWidth[BORDER_INDEX_BOT]  =-1;
                   cell[CurCell].BorderWidth[BORDER_INDEX_LEFT] =-1;
                   cell[CurCell].BorderWidth[BORDER_INDEX_RIGHT]=-1;

                   GetDlgItemText(hDlg,IDC_MARGIN,string,sizeof(string)-1);
                   StrTrim(string);
                   if (lstrlen(string)>0) {
                      if (!CheckDlgValue(hDlg,'I',IDC_MARGIN,0,720)) return TRUE;
                      cell[CurCell].margin=GetDlgItemInt(hDlg,IDC_MARGIN,NULL,TRUE);
                   }
                   margin=cell[CurCell].margin;

                   // check if margin large enought
                   if (    (int)GetDlgItemInt(hDlg,IDC_BOX_TOP,NULL,TRUE)>margin
                        || (int)GetDlgItemInt(hDlg,IDC_BOX_BOT,NULL,TRUE)>margin
                        || (int)GetDlgItemInt(hDlg,IDC_BOX_LEFT,NULL,TRUE)>margin
                        || (int)GetDlgItemInt(hDlg,IDC_BOX_RIGHT,NULL,TRUE)>margin) {
                        PrintError(w,MSG_SMALL_CELL_MARGIN,"CellBorderParam");
                        SetFocus(GetDlgItem(hDlg,IDC_MARGIN));
                        return TRUE;
                   }

                   GetDlgItemText(hDlg,IDC_BOX_TOP,string,sizeof(string)-1);
                   StrTrim(string);
                   if (lstrlen(string)>0) {
                      if (!CheckDlgValue(hDlg,'I',IDC_BOX_TOP,0,margin)) return TRUE;
                      cell[CurCell].BorderWidth[BORDER_INDEX_TOP]=GetDlgItemInt(hDlg,IDC_BOX_TOP,NULL,TRUE);
                   }

                   GetDlgItemText(hDlg,IDC_BOX_BOT,string,sizeof(string)-1);
                   StrTrim(string);
                   if (lstrlen(string)>0) {
                      if (!CheckDlgValue(hDlg,'I',IDC_BOX_BOT,0,margin)) return TRUE;
                      cell[CurCell].BorderWidth[BORDER_INDEX_BOT]=GetDlgItemInt(hDlg,IDC_BOX_BOT,NULL,TRUE);
                   }

                   GetDlgItemText(hDlg,IDC_BOX_LEFT,string,sizeof(string)-1);
                   StrTrim(string);
                   if (lstrlen(string)>0) {
                      if (!CheckDlgValue(hDlg,'I',IDC_BOX_LEFT,0,margin)) return (TRUE);
                      cell[CurCell].BorderWidth[BORDER_INDEX_LEFT]=GetDlgItemInt(hDlg,IDC_BOX_LEFT,NULL,TRUE);
                   }

                   GetDlgItemText(hDlg,IDC_BOX_RIGHT,string,sizeof(string)-1);
                   StrTrim(string);
                   if (lstrlen(string)>0) {
                      if (!CheckDlgValue(hDlg,'I',IDC_BOX_RIGHT,0,margin)) return (TRUE);
                      cell[CurCell].BorderWidth[BORDER_INDEX_RIGHT]=GetDlgItemInt(hDlg,IDC_BOX_RIGHT,NULL,TRUE);
                   }

                   DlgInt1=(int)(DWORD)SendMessage(GetDlgItem(hDlg,IDC_OUTLINE),BM_GETCHECK,0,0L);  // set if outline border requested

                   // get the range selection
                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_GETCHECK, 0, 0L))       EndDialog(hDlg,IDC_TABLE_ALL);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                   else                                                                                 EndDialog(hDlg,IDC_TABLE_SEL_ROWS);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CellWidthParam:
    Select the cells to be deleted.
******************************************************************************/
int CALLBACK _export CellWidthParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurCell;
    BYTE string[20];

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set cell border width
            if (HilightType==HILIGHT_OFF) CurCell=cid(CurLine);
            else                          CurCell=cid(HilightBegRow);

            if (HilightType==HILIGHT_OFF) SetDlgItemInt(hDlg,IDC_WIDTH,cell[CurCell].width,0);

            if (HilightType==HILIGHT_OFF) SetDlgItemInt(hDlg,IDC_MARGIN,cell[CurCell].margin,0);

            SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {

                case IDOK:
                   // get border width
                   CurCell=cid(CurLine);
                   DlgInt1=DlgInt2=-1;

                   GetDlgItemText(hDlg,IDC_WIDTH,string,sizeof(string)-1);
                   StrTrim(string);
                   if (lstrlen(string)>0) {
                      DlgInt1=GetDlgItemInt(hDlg,IDC_WIDTH,NULL,TRUE);
                   }



                   GetDlgItemText(hDlg,IDC_MARGIN,string,sizeof(string)-1);
                   StrTrim(string);
                   if (lstrlen(string)>0) {
                      if (!CheckDlgValue(hDlg,'I',IDC_MARGIN,0,720)) return TRUE;
                      DlgInt2=GetDlgItemInt(hDlg,IDC_MARGIN,NULL,TRUE);
                   }

                   // get the range selection
                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_GETCHECK, 0, 0L))       EndDialog(hDlg,IDC_TABLE_ALL);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                   else                                                                                 EndDialog(hDlg,IDC_TABLE_SEL_ROWS);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    CellBorderColorParam:
    Select the cells to be deleted.
******************************************************************************/
int CALLBACK _export CellBorderColorParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurCell;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set cell border color
            if (HilightType==HILIGHT_OFF) CurCell=cid(CurLine);
            else                          CurCell=cid(HilightBegRow);

            DlgColor1=cell[CurCell].BorderColor[BORDER_INDEX_TOP];
            DlgColor2=cell[CurCell].BorderColor[BORDER_INDEX_BOT];
            DlgColor3=cell[CurCell].BorderColor[BORDER_INDEX_LEFT];
            DlgColor4=cell[CurCell].BorderColor[BORDER_INDEX_RIGHT];

            if (HtmlMode) {
               EnableWindow(GetDlgItem(hDlg,IDC_COLOR_TOP),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_COLOR_BOT),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_COLOR_LEFT),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_COLOR_RIGHT),FALSE);
            } 
            
            if (HilightType==HILIGHT_OFF) SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_SETCHECK, TRUE, 0L);
            else                          SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_COLOR_ALL:                         // set the top color
                   DlgColor1=DlgColor2=DlgColor3=DlgColor4=(DWORD)DlgEditColor(w,hDlg,DlgColor1,TRUE);
                   break;

                case IDC_COLOR_TOP:                         // set the top color
                   DlgColor1=(DWORD)DlgEditColor(w,hDlg,DlgColor1,TRUE);
                   break;

                case IDC_COLOR_BOT:                         // set the bottom color
                   DlgColor2=(DWORD)DlgEditColor(w,hDlg,DlgColor2,TRUE);
                   break;

                case IDC_COLOR_LEFT:                         // set the left color
                   DlgColor3=(DWORD)DlgEditColor(w,hDlg,DlgColor3,TRUE);
                   break;

                case IDC_COLOR_RIGHT:                         // set the right color
                   DlgColor4=(DWORD)DlgEditColor(w,hDlg,DlgColor4,TRUE);
                   break;

                case IDOK:
                   // get border color
                   CurCell=cid(CurLine);
                   cell[CurCell].BorderColor[BORDER_INDEX_TOP]  =DlgColor1;
                   cell[CurCell].BorderColor[BORDER_INDEX_BOT]  =DlgColor2;
                   cell[CurCell].BorderColor[BORDER_INDEX_LEFT] =DlgColor3;
                   cell[CurCell].BorderColor[BORDER_INDEX_RIGHT]=DlgColor4;

                   // get the range selection
                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL),BM_GETCHECK, 0, 0L))       EndDialog(hDlg,IDC_TABLE_ALL);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_CELLS),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_TABLE_SEL_CELLS);
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_SEL_COLS),BM_GETCHECK, 0, 0L))  EndDialog(hDlg,IDC_TABLE_SEL_COLS);
                   else                                                                                 EndDialog(hDlg,IDC_TABLE_SEL_ROWS);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    RowPositionParam:
    Select the table row position
******************************************************************************/
int CALLBACK _export RowPositionParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurRowId,CurCell;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set current position
            CurCell=cid(CurLine);
            CurRowId=cell[CurCell].row;
            if      (TableRow[CurRowId].flags&CENTER)        SendMessage(GetDlgItem(hDlg,IDC_JUST_CENTER),BM_SETCHECK, TRUE, 0L);
            else if (TableRow[CurRowId].flags&RIGHT_JUSTIFY) SendMessage(GetDlgItem(hDlg,IDC_JUST_RIGHT),BM_SETCHECK, TRUE, 0L);
            else                                             SendMessage(GetDlgItem(hDlg,IDC_JUST_LEFT),BM_SETCHECK, TRUE, 0L);

            SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL_ROWS),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_JUST_LEFT),BM_GETCHECK, 0, 0L))    TempString[0]=0;
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_JUST_CENTER),BM_GETCHECK, 0, 0L))  TempString[0]=CENTER;
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_JUST_RIGHT),BM_GETCHECK, 0, 0L))   TempString[0]=RIGHT_JUSTIFY;

                   if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL_ROWS),BM_GETCHECK, 0, 0L))
                        EndDialog(hDlg,IDC_TABLE_ALL_ROWS);
                   else EndDialog(hDlg,IDC_TABLE_SEL_ROWS);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    TextRotationParam:
    Select text rotation for a frame
******************************************************************************/
BOOL CALLBACK _export TextRotationParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int angle;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set current status
            angle=DlgInt1;
            if      (angle==0)  SendMessage(GetDlgItem(hDlg,IDC_HORZ),BM_SETCHECK, TRUE, 0L);
            else if (angle==90) SendMessage(GetDlgItem(hDlg,IDC_BOT_TO_TOP),BM_SETCHECK, TRUE, 0L);
            else                SendMessage(GetDlgItem(hDlg,IDC_TOP_TO_BOT),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   angle=0;
                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_HORZ),BM_GETCHECK, 0, 0L))       angle=0;
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_BOT_TO_TOP),BM_GETCHECK, 0, 0L)) angle=90;
                   else                                                                            angle=270;

                   DlgInt1=angle;

                   EndDialog(hDlg,TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ParaTextFlowParam:
    Select text flow for a paragraph
******************************************************************************/
BOOL CALLBACK _export ParaTextFlowParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int flow;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            SetWindowText(hDlg,DlgText1);

            // set current status
            flow=DlgInt1;
            if      (flow==FLOW_RTL) SendMessage(GetDlgItem(hDlg,IDC_RTL),BM_SETCHECK, TRUE, 0L);
            else if (flow==FLOW_LTR) SendMessage(GetDlgItem(hDlg,IDC_LTR),BM_SETCHECK, TRUE, 0L);
            else                     SendMessage(GetDlgItem(hDlg,IDC_DEF),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   flow=FLOW_DEF;
                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_RTL),BM_GETCHECK, 0, 0L))  flow=FLOW_RTL;
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_LTR),BM_GETCHECK, 0, 0L))  flow=FLOW_LTR;
                   
                   DlgInt1=flow;

                   EndDialog(hDlg,TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    RowHeightParam:
    Set the minimum table row height
******************************************************************************/
int CALLBACK _export RowHeightParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurRowId,CurCell,MinHeight;
    BOOL AutoHeight;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set current position
            CurCell=cid(CurLine);
            CurRowId=cell[CurCell].row;

            MinHeight=TableRow[CurRowId].MinHeight;
            AutoHeight=FALSE;
            if (MinHeight==0) {
               EnableWindow(GetDlgItem(hDlg,IDC_HEIGHT_LABEL),FALSE);
               SendDlgItemMessage(hDlg,IDC_AUTO,BM_SETCHECK, TRUE, 0L);
               AutoHeight=TRUE;
            }
            else if (MinHeight>0) {
               SetDlgItemInt(hDlg,IDC_HEIGHT,MinHeight,0);
               SendDlgItemMessage(hDlg,IDC_MIN,BM_SETCHECK, TRUE, 0L);
            }
            else {
               SetDlgItemInt(hDlg,IDC_HEIGHT,-MinHeight,0);
               SendDlgItemMessage(hDlg,IDC_EXACT,BM_SETCHECK, TRUE, 0L);
            }

            EnableWindow(GetDlgItem(hDlg,IDC_HEIGHT_LABEL),!AutoHeight);
            EnableWindow(GetDlgItem(hDlg,IDC_HEIGHT),!AutoHeight);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_AUTO:
                case IDC_MIN:
                case IDC_EXACT:
                   AutoHeight=(BOOL)SendDlgItemMessage(hDlg,IDC_AUTO,BM_GETCHECK, 0, 0L);
                   EnableWindow(GetDlgItem(hDlg,IDC_HEIGHT_LABEL),!AutoHeight);
                   EnableWindow(GetDlgItem(hDlg,IDC_HEIGHT),!AutoHeight);
                   break;

                case IDOK:

                   if  ((BOOL)SendDlgItemMessage(hDlg,IDC_AUTO,BM_GETCHECK, 0, 0L)) DlgInt1=0;
                   else {
                      if (!AssignNumVal(hDlg,IDC_HEIGHT,&DlgInt1)) return (TRUE);
                      if (DlgInt1<0 || DlgInt1>InchesToTwips(PageHeight)) {
                         MessageBeep(0);
                         SetFocus(GetDlgItem(hDlg,IDC_HEIGHT));
                         return TRUE;
                      }

                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_EXACT,BM_GETCHECK, 0, 0L)) DlgInt1=-DlgInt1;
                   }

                   if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL_ROWS),BM_GETCHECK, 0, 0L))
                        EndDialog(hDlg,IDC_TABLE_ALL_ROWS);
                   else EndDialog(hDlg,IDC_TABLE_SEL_ROWS);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    RowTextFlowParam:
    Set the minimum table row height
******************************************************************************/
int CALLBACK _export RowTextFlowParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurRowId,CurCell,flow;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set current position
            CurCell=cid(CurLine);
            CurRowId=cell[CurCell].row;

            flow=TableRow[CurRowId].flow;

            if      (flow==FLOW_RTL) SendMessage(GetDlgItem(hDlg,IDC_RTL),BM_SETCHECK, TRUE, 0L);
            else if (flow==FLOW_LTR) SendMessage(GetDlgItem(hDlg,IDC_LTR),BM_SETCHECK, TRUE, 0L);
            else                     SendMessage(GetDlgItem(hDlg,IDC_DEF),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   flow=FLOW_DEF;
                   if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_RTL),BM_GETCHECK, 0, 0L))  flow=FLOW_RTL;
                   else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_LTR),BM_GETCHECK, 0, 0L))  flow=FLOW_LTR;
                   
                   DlgInt1=flow;

                   if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_TABLE_ALL_ROWS),BM_GETCHECK, 0, 0L))
                        EndDialog(hDlg,IDC_TABLE_ALL_ROWS);
                   else EndDialog(hDlg,IDC_TABLE_SEL_ROWS);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    SectionParam:
    Dialog box proceeded to edit the section parameters
******************************************************************************/
BOOL CALLBACK _export SectionParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int  i,sect,flow,orient,BinNameSize=24,BinCount,FirstBinIdx,NextBinIdx;
    int  PaperNameSize=64,PaperCount,PaperIdx,PaperType;
    BOOL metric,RestartPageNo;
    LPBYTE pBinName=NULL,pPaperName=NULL;
    LPWORD pBin=NULL,pPaper=NULL;
    LPPOINT  pPaperSize=NULL;
    HANDLE hDriver=NULL;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            metric=(BOOL)(TerFlags&TFLAG_METRIC);

            // dialog field initialization
            sect=GetSection(w,CurLine);
            SendMessage(GetDlgItem(hDlg,IDC_SEC_NEW_PAGE),BM_SETCHECK, TerSect[sect].flags&SECT_NEW_PAGE, 0L);

            if (TerSect[sect].flags&SECT_RESTART_PAGE_NO) {
               SendMessage(GetDlgItem(hDlg,IDC_RESTART_PAGE_NO),BM_SETCHECK, TRUE, 0L);
               SetDlgItemInt(hDlg,IDC_PAGE_NO,TerSect[sect].FirstPageNo,0);
            }
            else EnableWindow(GetDlgItem(hDlg,IDC_PAGE_NO),FALSE);

            if (metric) SetDlgItemText(hDlg,IDC_UNITS,MsgString[MSG_COLUMNS]);

            // disable column selection when not in print view mode
            if (!TerArg.PrintView) {
               EnableWindow(GetDlgItem(hDlg,IDC_SEC_COLUMNS),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_SEC_COL_SPACE),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_PORTRAIT),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_LANDSCAPE),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_DEF),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_RTL),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_LTR),FALSE);
            }
            else {
               SetDlgItemInt(hDlg,IDC_SEC_COLUMNS,TerSect[sect].columns,0);
               if (metric) SetDlgItemText(hDlg,IDC_SEC_COL_SPACE,DoubleToStr(w,(double)InchesToCm(TerSect[sect].ColumnSpace),2,TempString));
               else        SetDlgItemText(hDlg,IDC_SEC_COL_SPACE,DoubleToStr(w,(double)TerSect[sect].ColumnSpace,2,TempString));
               if (TerSect[sect].orient==DMORIENT_PORTRAIT)
                    SendMessage(GetDlgItem(hDlg,IDC_PORTRAIT),BM_SETCHECK, TRUE, 0L);
               else SendMessage(GetDlgItem(hDlg,IDC_LANDSCAPE),BM_SETCHECK, TRUE, 0L);
            }

            // set the bin selection
            BinCount=0;
            FirstBinIdx=NextBinIdx=PaperIdx=0;
            if (PrinterAvailable && pDeviceMode) {
               if (0==(BinCount=DeviceCapabilities(PrinterName,PrinterPort,DC_BINNAMES,NULL,pDeviceMode))) goto BINS_DONE;
               pBinName=OurAlloc((BinCount+1)*BinNameSize);
               if (0==(BinCount=DeviceCapabilities(PrinterName,PrinterPort,DC_BINNAMES,pBinName,pDeviceMode))) goto BINS_DONE;
               pBin=OurAlloc((BinCount+1)*sizeof(WORD));
               if (0==(BinCount=DeviceCapabilities(PrinterName,PrinterPort,DC_BINS,(LPSTR)pBin,pDeviceMode))) goto BINS_DONE;

               if (BinCount==0) goto BINS_DONE;

               for (i=0;i<BinCount;i++) {
                  SendDlgItemMessage(hDlg,IDC_FIRST_BIN,LB_ADDSTRING, 0, (DWORD)(LPBYTE)&(pBinName[i*BinNameSize]));
                  SendDlgItemMessage(hDlg,IDC_NEXT_BIN,LB_ADDSTRING, 0, (DWORD)(LPBYTE)&(pBinName[i*BinNameSize]));
               }

               for (i=0;i<BinCount;i++) {
                  SendMessage(GetDlgItem(hDlg,IDC_FIRST_BIN),LB_SETITEMDATA, i, pBin[i]);
                  SendMessage(GetDlgItem(hDlg,IDC_NEXT_BIN),LB_SETITEMDATA, i, pBin[i]);

                  if (TerSect[sect].FirstPageBin==(int)pBin[i]) FirstBinIdx=i;
                  if (TerSect[sect].bin==(int)pBin[i]) NextBinIdx=i;
               }

               SendDlgItemMessage(hDlg,IDC_FIRST_BIN,LB_SETCURSEL, FirstBinIdx,0L);
               SendDlgItemMessage(hDlg,IDC_NEXT_BIN,LB_SETCURSEL, NextBinIdx,0L);
               
               BINS_DONE:

               //  Get the supported paper sizes
               if (0==(PaperCount=DeviceCapabilities(PrinterName,PrinterPort,DC_PAPERS,NULL,pDeviceMode))) goto PAPER_DONE;
               pPaper=OurAlloc((PaperCount+2)*sizeof(WORD));       // one extra for DMPAPER_USER
               if (0==(PaperCount=DeviceCapabilities(PrinterName,PrinterPort,DC_PAPERS,(LPSTR)pPaper,pDeviceMode))) goto PAPER_DONE;

               pPaperName=OurAlloc((PaperCount+2)*PaperNameSize);
               if (0==(PaperCount=DeviceCapabilities(PrinterName,PrinterPort,DC_PAPERNAMES,(LPSTR)pPaperName,pDeviceMode))) goto PAPER_DONE;

               pPaperSize=OurAlloc((PaperCount+2)*sizeof(POINT));
               if (0==(PaperCount=DeviceCapabilities(PrinterName,PrinterPort,DC_PAPERSIZE,(LPSTR)pPaperSize,pDeviceMode))) goto PAPER_DONE;

               PAPER_DONE:

               for (i=0;i<PaperCount;i++) if (pPaper[i]==DMPAPER_USER) break;  // check if DMPAPER_USER found
               if (i==PaperCount) {   // add DMPAPER_USER
                  pPaper[i]=DMPAPER_USER;
                  lstrcpy(&(pPaperName[i*PaperNameSize]),"Custom Size");
                  PaperIdx=i;         // select by default
                  PaperCount++;
               }
                
               for (i=0;i<PaperCount;i++) {
                  SendDlgItemMessage(hDlg,IDC_PAPER,LB_ADDSTRING, 0, (DWORD)(LPBYTE)&(pPaperName[i*PaperNameSize]));
               }
               for (i=0;i<PaperCount;i++) {
                  SendMessage(GetDlgItem(hDlg,IDC_PAPER),LB_SETITEMDATA, i, pPaper[i]);
                  if (TerSect[sect].PprSize==(int)pPaper[i]) PaperIdx=i;
               }
               SendDlgItemMessage(hDlg,IDC_PAPER,LB_SETCURSEL, PaperIdx,0L);

               if (metric) {
                  SetDlgItemText(hDlg,IDC_PAPER_WIDTH_LABEL,"Width (cm)");
                  SetDlgItemText(hDlg,IDC_PAPER_HEIGHT_LABEL,"Height (cm)");
               }
               
               if (pPaper[PaperIdx]==DMPAPER_USER) {
                 if (metric) SetDlgItemText(hDlg,IDC_PAPER_WIDTH,DoubleToStr(w,(double)InchesToCm(TerSect[sect].PprWidth),2,TempString));
                 else        SetDlgItemText(hDlg,IDC_PAPER_WIDTH,DoubleToStr(w,(double)TerSect[sect].PprWidth,2,TempString));
                 
                 if (metric) SetDlgItemText(hDlg,IDC_PAPER_HEIGHT,DoubleToStr(w,(double)InchesToCm(TerSect[sect].PprHeight),2,TempString));
                 else        SetDlgItemText(hDlg,IDC_PAPER_HEIGHT,DoubleToStr(w,(double)TerSect[sect].PprHeight,2,TempString));
               }
               else {
                 EnableWindow(GetDlgItem(hDlg,IDC_PAPER_WIDTH),FALSE);
                 EnableWindow(GetDlgItem(hDlg,IDC_PAPER_HEIGHT),FALSE);
               }   
            }

            if (hDriver)  FreeLibrary(hDriver);
            if (pBinName) OurFree(pBinName);
            if (pBin)     OurFree(pBin);
            if (pPaperName) OurFree(pPaperName);
            if (pPaper)   OurFree(pPaper);
            DlgPtr1=pPaperSize;   // save until the dialog is closed

            if (BinCount==0) {
               EnableWindow(GetDlgItem(hDlg,IDC_FIRST_BIN),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_NEXT_BIN),FALSE);
            }

            flow=TerSect[sect].flow;

            if      (flow==FLOW_RTL) SendMessage(GetDlgItem(hDlg,IDC_RTL),BM_SETCHECK, TRUE, 0L);
            else if (flow==FLOW_LTR) SendMessage(GetDlgItem(hDlg,IDC_LTR),BM_SETCHECK, TRUE, 0L);
            else                     SendMessage(GetDlgItem(hDlg,IDC_DEF),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_RESTART_PAGE_NO:
                   RestartPageNo=(BOOL)SendMessage(GetDlgItem(hDlg,IDC_RESTART_PAGE_NO),BM_GETCHECK, 0, 0L);
                   EnableWindow(GetDlgItem(hDlg,IDC_PAGE_NO),RestartPageNo);
                   break;

                case IDC_PAPER:
                   PaperIdx=(int)SendDlgItemMessage(hDlg,IDC_PAPER,LB_GETCURSEL, 0,0L);
                   PaperType=(int)SendDlgItemMessage(hDlg,IDC_PAPER,LB_GETITEMDATA, PaperIdx,0L);
                   EnableWindow(GetDlgItem(hDlg,IDC_PAPER_WIDTH),PaperType==DMPAPER_USER);
                   EnableWindow(GetDlgItem(hDlg,IDC_PAPER_HEIGHT),PaperType==DMPAPER_USER);
                   break;

                case IDOK:
                   // validate that the user entered the values with range
                   sect=GetSection(w,CurLine);
                   metric=(BOOL)(TerFlags&TFLAG_METRIC);
                   pPaperSize=DlgPtr1;          // paper size

                   if (TerArg.PrintView) {
                      if (!CheckDlgValue(hDlg,'I',IDC_SEC_COLUMNS,1,4)) return (TRUE);
                      if (!CheckDlgValue(hDlg,'F',IDC_SEC_COL_SPACE,.1,4)) return (TRUE);
                      // extract the margin and validate
                      TerSect[sect].columns=GetDlgItemInt(hDlg,IDC_SEC_COLUMNS,NULL,TRUE);
                      GetDlgItemText(hDlg,IDC_SEC_COL_SPACE,TempString,100);
                      if (metric) TerSect[sect].ColumnSpace=(float)CmToInches(TerAtof(w,TempString));
                      else        TerSect[sect].ColumnSpace=(float)TerAtof(w,TempString);

                      if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_PORTRAIT),BM_GETCHECK, 0, 0L))
                           TerSect[sect].orient=DMORIENT_PORTRAIT;
                      else TerSect[sect].orient=DMORIENT_LANDSCAPE;

                      orient=TerSect[sect].orient;
                      if (PrtDiffRes && pDeviceMode) {
                         if (pDeviceMode->dmOrientation!=orient) OpenCurPrinterOrient(w,orient,FALSE);
                         PaperOrient=pDeviceMode->dmOrientation;
                         if (ForceSectOrient(w)) for (i=0;i<TotalSects;i++) TerSect[i].orient=orient;  // only one orientation supported
                         RecreateFonts(w,hTerDC);
                         RequestPagination(w,TRUE);
                      }
                      
                      // Text flow 
                      flow=FLOW_DEF;
                      if      ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_RTL),BM_GETCHECK, 0, 0L))  flow=FLOW_RTL;
                      else if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_LTR),BM_GETCHECK, 0, 0L))  flow=FLOW_LTR;

                      TerSect[sect].flow=flow;
                   }

                   // get the page break/restart flags
                   if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_SEC_NEW_PAGE),BM_GETCHECK, 0, 0L))
                        TerSect[sect].flags|=SECT_NEW_PAGE;
                   else TerSect[sect].flags=ResetUintFlag(TerSect[sect].flags,SECT_NEW_PAGE);

                   if ((BOOL)SendMessage(GetDlgItem(hDlg,IDC_RESTART_PAGE_NO),BM_GETCHECK, 0, 0L)) {
                      TerSect[sect].flags|=SECT_RESTART_PAGE_NO;
                      TerSect[sect].FirstPageNo=GetDlgItemInt(hDlg,IDC_PAGE_NO,NULL,TRUE);
                      if (TerSect[sect].FirstPageNo<1) TerSect[sect].FirstPageNo=1;
                   }
                   else TerSect[sect].flags=ResetUintFlag(TerSect[sect].flags,SECT_RESTART_PAGE_NO);

                   // get the bin information
                   if (IsWindowEnabled(GetDlgItem(hDlg,IDC_FIRST_BIN))) {
                      FirstBinIdx=(int)SendDlgItemMessage(hDlg,IDC_FIRST_BIN,LB_GETCURSEL, 0,0L);
                      TerSect[sect].FirstPageBin=(int)SendDlgItemMessage(hDlg,IDC_FIRST_BIN,LB_GETITEMDATA, FirstBinIdx,0L);
                      NextBinIdx=(int)SendDlgItemMessage(hDlg,IDC_NEXT_BIN,LB_GETCURSEL, 0,0L);
                      TerSect[sect].bin=(int)SendDlgItemMessage(hDlg,IDC_NEXT_BIN,LB_GETITEMDATA, NextBinIdx,0L);
                   }

                   // get the paper size information
                   PaperIdx=(int)SendDlgItemMessage(hDlg,IDC_PAPER,LB_GETCURSEL, 0,0L);
                   PaperType=(int)SendDlgItemMessage(hDlg,IDC_PAPER,LB_GETITEMDATA, PaperIdx,0L);
                   TerSect[sect].PprSize=PaperType;
                   if (PaperType==DMPAPER_USER) {
                      GetDlgItemText(hDlg,IDC_PAPER_WIDTH,TempString,100);
                      if (metric) TerSect[sect].PprWidth=(float)CmToInches(TerAtof(w,TempString));
                      else        TerSect[sect].PprWidth=(float)TerAtof(w,TempString);

                      GetDlgItemText(hDlg,IDC_PAPER_HEIGHT,TempString,100);
                      if (metric) TerSect[sect].PprHeight=(float)CmToInches(TerAtof(w,TempString));
                      else        TerSect[sect].PprHeight=(float)TerAtof(w,TempString);
                   
                   }
                   else {
                      TerSect[sect].PprWidth=MmToInches(pPaperSize[PaperIdx].x)/10;
                      TerSect[sect].PprHeight=MmToInches(pPaperSize[PaperIdx].y)/10;
                   }

                   if (pPaperSize) OurFree(pPaperSize);
                   
                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   pPaperSize=DlgPtr1;          // paper size
                   if (pPaperSize) OurFree(pPaperSize);

                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    OverrideBinParam:
    Dialog box overide the paper bin during printing
******************************************************************************/
BOOL CALLBACK _export OverrideBinParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    BOOL override;
    int  i,BinNameSize=24,BinCount,BinIdx;
    LPBYTE pBinName=NULL;
    LPWORD pBin=NULL;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // set the initial varibles
            override=(OverrideBinCopy>0);
            SendDlgItemMessage(hDlg,IDC_OVERRIDE,BM_SETCHECK, override, 0L);

            SetDlgItemInt(hDlg,IDC_COPY,OverrideBinCopy==0?2:OverrideBinCopy,0);     // default value


            // set the bin selection
            BinCount=0;
            BinIdx=0;
            if (PrinterAvailable && pDeviceMode) {
               if (0==(BinCount=DeviceCapabilities(PrinterName,PrinterPort,DC_BINNAMES,NULL,pDeviceMode))) goto BINS_DONE;
               pBinName=OurAlloc((BinCount+1)*BinNameSize);
               if (0==(BinCount=DeviceCapabilities(PrinterName,PrinterPort,DC_BINNAMES,pBinName,pDeviceMode))) goto BINS_DONE;
               pBin=OurAlloc((BinCount+1)*sizeof(WORD));
               if (0==(BinCount=DeviceCapabilities(PrinterName,PrinterPort,DC_BINS,(LPSTR)pBin,pDeviceMode))) goto BINS_DONE;

               if (BinCount==0) goto BINS_DONE;

               for (i=0;i<BinCount;i++) {
                  SendDlgItemMessage(hDlg,IDC_BIN,LB_ADDSTRING, 0, (DWORD)(LPBYTE)&(pBinName[i*BinNameSize]));
               }

               for (i=0;i<BinCount;i++) {
                  SendMessage(GetDlgItem(hDlg,IDC_BIN),LB_SETITEMDATA, i, pBin[i]);

                  if (OverrideBin==(int)pBin[i]) BinIdx=i;
               }

               SendDlgItemMessage(hDlg,IDC_BIN,LB_SETCURSEL, BinIdx,0L);
            }
               
            BINS_DONE:
            if (pBinName) OurFree(pBinName);
            if (pBin)     OurFree(pBin);

            if (BinCount==0) {
               EnableWindow(GetDlgItem(hDlg,IDC_BIN),FALSE);
            }
            
            if (!override) {          // no override in effect
               EnableWindow(GetDlgItem(hDlg,IDC_BIN),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_BIN_LBL),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_COPY),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_COPY_LBL),FALSE);
            } 

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_OVERRIDE:
                   override=(BOOL)SendMessage(GetDlgItem(hDlg,IDC_OVERRIDE),BM_GETCHECK, 0, 0L);
                   EnableWindow(GetDlgItem(hDlg,IDC_BIN),override);
                   EnableWindow(GetDlgItem(hDlg,IDC_BIN_LBL),override);
                   EnableWindow(GetDlgItem(hDlg,IDC_COPY),override);
                   EnableWindow(GetDlgItem(hDlg,IDC_COPY_LBL),override);
                   break;

                case IDOK:

                   override=(BOOL)SendMessage(GetDlgItem(hDlg,IDC_OVERRIDE),BM_GETCHECK, 0, 0L);
                   
                   DlgInt2=DMBIN_AUTO;      // pass bin using DlgInt2

                   if (override) {
                      if (!CheckDlgValue(hDlg,'I',IDC_COPY,1,9999)) return (TRUE);
                      DlgInt1=GetDlgItemInt(hDlg,IDC_COPY,NULL,TRUE);

                     // get the bin information
                     if (IsWindowEnabled(GetDlgItem(hDlg,IDC_BIN))) {
                        BinIdx=(int)SendDlgItemMessage(hDlg,IDC_BIN,LB_GETCURSEL, 0,0L);
                        DlgInt2=(int)SendDlgItemMessage(hDlg,IDC_BIN,LB_GETITEMDATA, BinIdx,0L);  // pass bin using DlgInt2
                     }
                   }
                   else DlgInt1=0;   // no override

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    EditPictureParam:
    Accepts the print options for a document mode printing.
******************************************************************************/
BOOL CALLBACK _export EditPictureParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int pict,align;
    double temp,MaxSize;
    BOOL metric;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            metric=(BOOL)(TerFlags&TFLAG_METRIC);

            // dialog field initialization
            pict=GetCurCfmt(w,CurLine,CurCol);
            if (metric) {
               SetDlgItemText(hDlg,IDC_PICT_SIZE_LBL,MsgString[MSG_PICTURE_SIZE]);
               SetDlgItemText(hDlg,IDC_PICT_WIDTH,DoubleToStr(w,TwipsToCm(TerFont[pict].PictWidth),2,TempString));
               SetDlgItemText(hDlg,IDC_PICT_HEIGHT,DoubleToStr(w,TwipsToCm(TerFont[pict].PictHeight),2,TempString));
            }
            else {
               SetDlgItemText(hDlg,IDC_PICT_WIDTH,DoubleToStr(w,TwipsToInches(TerFont[pict].PictWidth),2,TempString));
               SetDlgItemText(hDlg,IDC_PICT_HEIGHT,DoubleToStr(w,TwipsToInches(TerFont[pict].PictHeight),2,TempString));
            }

            if (TerFont[pict].offset>0) {
               SendDlgItemMessage(hDlg,IDC_OFFSET,BM_SETCHECK, TRUE, 0L);
               SetDlgItemInt(hDlg,IDC_OFFSET_VAL,TerFont[pict].offset,0);
            }
            else {
               if (TerFont[pict].PictAlign==ALIGN_TOP)    SendDlgItemMessage(hDlg,IDC_PICT_ALIGN_TOP,BM_SETCHECK, TRUE, 0L);
               if (TerFont[pict].PictAlign==ALIGN_MIDDLE) SendDlgItemMessage(hDlg,IDC_PICT_ALIGN_MIDDLE,BM_SETCHECK, TRUE, 0L);
               if (TerFont[pict].PictAlign==ALIGN_BOT)    SendDlgItemMessage(hDlg,IDC_PICT_ALIGN_BOT,BM_SETCHECK, TRUE, 0L);
               EnableWindow(GetDlgItem(hDlg,IDC_OFFSET_VAL),FALSE);
            }

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_OFFSET:
                   EnableWindow(GetDlgItem(hDlg,IDC_OFFSET_VAL),TRUE);
                   break;

                case IDC_PICT_ALIGN_TOP:
                case IDC_PICT_ALIGN_MIDDLE:
                case IDC_PICT_ALIGN_BOT:
                   EnableWindow(GetDlgItem(hDlg,IDC_OFFSET_VAL),TRUE);
                   break;

                case IDOK:
                   metric=(BOOL)(TerFlags&TFLAG_METRIC);
                   
                   // validate that the user entered the values with range
                   MaxSize=metric?40:15;
                   if (!CheckDlgValue(hDlg,'F',IDC_PICT_WIDTH,0.1,MaxSize)) return (TRUE);
                   if (!CheckDlgValue(hDlg,'F',IDC_PICT_HEIGHT,0.1,MaxSize)) return (TRUE);

                   // extract the margin and validate
                   pict=GetCurCfmt(w,CurLine,CurCol);

                   GetDlgItemText(hDlg,IDC_PICT_WIDTH,TempString,100);
                   temp=(double)TerAtof(w,TempString);
                   TerFont[pict].PictWidth=(int)(metric?CmToTwips(temp):InchesToTwips(temp));

                   GetDlgItemText(hDlg,IDC_PICT_HEIGHT,TempString,100);
                   temp=(double)TerAtof(w,TempString);
                   TerFont[pict].PictHeight=(int)(metric?CmToTwips(temp):InchesToTwips(temp));

                   // extract alignment
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_OFFSET,BM_GETCHECK, 0, 0L)) {
                      align=ALIGN_BOT;          // default alignment flag
                      TerFont[pict].offset=GetDlgItemInt(hDlg,IDC_OFFSET_VAL,NULL,TRUE);
                   }
                   else {
                      TerFont[pict].offset=0;
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_PICT_ALIGN_TOP,BM_GETCHECK, 0, 0L)) align=ALIGN_TOP;
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_PICT_ALIGN_BOT,BM_GETCHECK, 0, 0L)) align=ALIGN_BOT;
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_PICT_ALIGN_MIDDLE,BM_GETCHECK, 0, 0L)) align=ALIGN_MIDDLE;
                   }
                   TerFont[pict].PictAlign=align;

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/***************************************************************************
    CtlDlgFn:
    This routine is the dialog processing routine to allow user to edit
    control styles.
****************************************************************************/
BOOL CALLBACK _export CtlDlgFn(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    struct StrDlgParam far *dlg;
    DWORD style,mask;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            style=(DWORD)(dlg->data);
            SetWindowLong(hDlg,DWL_USER,(LONG)dlg);  // store the information pointer

            // dialog field initialization
            if (style&WS_VISIBLE)  SendDlgItemMessage(hDlg,IDC_CTL_VISIBLE,BM_SETCHECK, TRUE, 0L);
            if (style&WS_DISABLED) SendDlgItemMessage(hDlg,IDC_CTL_DISABLED,BM_SETCHECK, TRUE, 0L);
            if (style&WS_GROUP)    SendDlgItemMessage(hDlg,IDC_CTL_GROUP,BM_SETCHECK, TRUE, 0L);
            if (style&WS_TABSTOP)  SendDlgItemMessage(hDlg,IDC_CTL_TABSTOP,BM_SETCHECK, TRUE, 0L);
            if (style&WS_BORDER)   SendDlgItemMessage(hDlg,IDC_CTL_BORDER,BM_SETCHECK, TRUE, 0L);

            if (style&TER_WORD_WRAP)    SendDlgItemMessage(hDlg,IDC_CTL_WORD_WRAP,BM_SETCHECK, TRUE, 0L);
            if (style&TER_PRINT_VIEW)   SendDlgItemMessage(hDlg,IDC_CTL_PRINT_VIEW,BM_SETCHECK, TRUE, 0L);
            if (style&TER_PAGE_MODE)    SendDlgItemMessage(hDlg,IDC_CTL_PAGE_MODE,BM_SETCHECK, TRUE, 0L);
            if (style&TER_FITTED_VIEW)  SendDlgItemMessage(hDlg,IDC_CTL_FITTED_VIEW,BM_SETCHECK, TRUE, 0L);
            if (style&TER_HSCROLL)      SendDlgItemMessage(hDlg,IDC_CTL_HSCROLL,BM_SETCHECK, TRUE, 0L);
            if (style&TER_VSCROLL)      SendDlgItemMessage(hDlg,IDC_CTL_VSCROLL,BM_SETCHECK, TRUE, 0L);
            if (style&TER_SHOW_STATUS)  SendDlgItemMessage(hDlg,IDC_CTL_SHOW_STATUS,BM_SETCHECK, TRUE, 0L);
            if (style&TER_SHOW_RULER)   SendDlgItemMessage(hDlg,IDC_CTL_SHOW_RULER,BM_SETCHECK, TRUE, 0L);
            if (style&TER_SHOW_TOOLBAR) SendDlgItemMessage(hDlg,IDC_CTL_SHOW_TOOLBAR,BM_SETCHECK, TRUE, 0L);
            if (style&TER_BORDER_MARGIN)SendDlgItemMessage(hDlg,IDC_CTL_BORDER_MARGIN,BM_SETCHECK, TRUE, 0L);
            if (style&TER_OUTPUT_RTF)   SendDlgItemMessage(hDlg,IDC_CTL_OUTPUT_RTF,BM_SETCHECK, TRUE, 0L);
            if (style&TER_READ_ONLY)    SendDlgItemMessage(hDlg,IDC_CTL_READ_ONLY,BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            dlg=(struct StrDlgParam far *)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            style=(DWORD)(dlg->data);
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   mask=WS_VISIBLE|WS_DISABLED|WS_GROUP|WS_TABSTOP|WS_BORDER|
                        TER_WORD_WRAP|TER_PRINT_VIEW|TER_HSCROLL|TER_VSCROLL|
                        TER_SHOW_STATUS|TER_SHOW_RULER|TER_BORDER_MARGIN|
                        TER_OUTPUT_RTF|TER_READ_ONLY|TER_PAGE_MODE|TER_SHOW_TOOLBAR|TER_FITTED_VIEW;
                   style=style&(~mask);  // turnoff these bits

                   // turnon the bits as checked
                   if  ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_VISIBLE,BM_GETCHECK, 0, 0L))  style|=WS_VISIBLE;
                   if  ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_DISABLED,BM_GETCHECK, 0, 0L)) style|=WS_DISABLED;
                   if  ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_GROUP,BM_GETCHECK, 0, 0L))    style|=WS_GROUP;
                   if  ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_TABSTOP,BM_GETCHECK, 0, 0L))  style|=WS_TABSTOP;
                   if  ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_BORDER,BM_GETCHECK, 0, 0L))   style|=WS_BORDER;

                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_WORD_WRAP,BM_GETCHECK, 0, 0L))    style|=TER_WORD_WRAP;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_PRINT_VIEW,BM_GETCHECK, 0, 0L))   style|=TER_PRINT_VIEW;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_PAGE_MODE,BM_GETCHECK, 0, 0L))    style|=TER_PAGE_MODE;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_FITTED_VIEW,BM_GETCHECK, 0, 0L))  style|=TER_FITTED_VIEW;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_HSCROLL,BM_GETCHECK, 0, 0L))      style|=TER_HSCROLL;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_VSCROLL,BM_GETCHECK, 0, 0L))      style|=TER_VSCROLL;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_SHOW_STATUS,BM_GETCHECK, 0, 0L))  style|=TER_SHOW_STATUS;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_SHOW_RULER,BM_GETCHECK, 0, 0L))   style|=TER_SHOW_RULER;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_SHOW_TOOLBAR,BM_GETCHECK, 0, 0L)) style|=TER_SHOW_TOOLBAR;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_BORDER_MARGIN,BM_GETCHECK, 0, 0L))style|=TER_BORDER_MARGIN;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_OUTPUT_RTF,BM_GETCHECK, 0, 0L))   style|=TER_OUTPUT_RTF;
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_CTL_READ_ONLY,BM_GETCHECK, 0, 0L))    style|=TER_READ_ONLY;

                   dlg->data=(LPBYTE)style;    // assign to output
                   EndDialog(hDlg,TRUE);

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg,FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    FontParam:
    Accepts new font and point size.
******************************************************************************/
BOOL CALLBACK _export FontParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{

    int i,CurType,CurPoint;
    OLDFONTENUMPROC lpProc;
    struct StrEnumFonts far * font;
    BYTE string[32];
    PTERWND w;
    struct StrDlgParam far *dlg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            //******** find the available type faces ********
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

                FreeProcInstance((FARPROC)lpProc);       // set back the old window

                TotalTypes=font->TypeCount;          // number of fonts available
                for (i=0;i<TotalTypes;i++) {        // add names to the list box
                   SendMessage(GetDlgItem(hDlg,IDC_FONT_TYPES),CB_ADDSTRING, 0, (DWORD)(LPBYTE)(font->TypeNames[i]));
                }
                OurFree(font);                      // freeup the font structure
            }

            //*********** show the current font ************
            if (CurSID>=0 && lstrlen(StyleId[CurSID].TypeFace)==0) ReqTypeFace[0]=0;
            else {
               for (i=TotalTypes-1;i>0;i--) {      // select current font
                  SendMessage(GetDlgItem(hDlg,IDC_FONT_TYPES),CB_GETLBTEXT, (WPARAM)  i,(DWORD)(LPBYTE)string);
                  if (CurSID<0 && lstrcmpi(string,TerFont[(int)(DWORD)(dlg->data)].TypeFace)==0) break;
                  if (CurSID>=0 && lstrcmpi(string,StyleId[CurSID].TypeFace)==0) break;
               }
               CurType=i;
               SendMessage(GetDlgItem(hDlg,IDC_FONT_TYPES),CB_SETCURSEL, CurType,0L);
               if (CurSID<0) lstrcpy(ReqTypeFace,TerFont[(int)(DWORD)(dlg->data)].TypeFace);
               else          lstrcpy(ReqTypeFace,StyleId[CurSID].TypeFace);
            }

            //******** find the available point sizes ********
            if (CurSID>=0) ReqTwipsSize=StyleId[CurSID].TwipsSize;
            else           ReqTwipsSize=TerFont[(int)(DWORD)(dlg->data)].TwipsSize;
            SetPointSizes(w,hDlg,ReqTypeFace,ReqTwipsSize);

            // set the DEFAULT_GUI_FONT for dbcs enabled window
            if (mbcs) SetGuiFont(w,hDlg,IDC_FONT_TYPES);
            if (mbcs) SetGuiFont(w,hDlg,IDC_FONT_POINTS);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer

            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_FONT_TYPES:       // mark the default font
                   if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                       CurType=(int)SendMessage(GetDlgItem(hDlg,IDC_FONT_TYPES),CB_GETCURSEL, 0,0L);
                       if (CurType==CB_ERR) return (TRUE);
                       SendMessage(GetDlgItem(hDlg,IDC_FONT_TYPES),CB_GETLBTEXT, (WPARAM)  CurType,(DWORD)(LPBYTE)string);
                       SetPointSizes(w,hDlg,string,ReqTwipsSize);
                   }
                   return TRUE;

                case IDC_FONT_POINTS:       // mark the current point
                   if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                       CurPoint=(int)SendMessage(GetDlgItem(hDlg,IDC_FONT_POINTS),CB_GETCURSEL, 0,0L);
                       if (CurPoint==CB_ERR) return (TRUE);
                       SendMessage(GetDlgItem(hDlg,IDC_FONT_POINTS),CB_GETLBTEXT, (WPARAM)  CurPoint,(DWORD)(LPBYTE)string);
                       lstrcpy(TempString,string);
                       ReqTwipsSize=(int)(atof(TempString)*20);
                   }
                   return TRUE;

                case IDOK:
                   //***** get selections *******
                   GetDlgItemText(hDlg,IDC_FONT_TYPES,ReqTypeFace,sizeof(ReqTypeFace)-1);
                   GetDlgItemText(hDlg,IDC_FONT_POINTS,string,sizeof(string)-1);
                   lstrcpy(TempString,string);        // copy to data segment
                   ReqTwipsSize=(int)(atof(TempString)*20);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    BookmarkParam:
    Accepts new font and point size.
******************************************************************************/
BOOL CALLBACK _export BookmarkParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    int i,count,cmd;
    PTERWND w;
    struct StrDlgParam far *dlg;
    BYTE name[MAX_WIDTH+1],CurBookmark[MAX_WIDTH+1];
    int  CurIdx=-1;
    BOOL ValidName,exists;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            //******** find the available bookmarks ********
            if (!GetTag(w,CurLine,CurCol,TERTAG_BKM,CurBookmark,NULL,NULL)) CurBookmark[0]=0;

            count=TerGetBookmark(hTerWnd,-1,NULL);
            for (i=0;i<count;i++) {        // add names to the list box
               TerGetBookmark(hTerWnd,i,name);
               SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_ADDSTRING, 0, (DWORD)(LPBYTE)name);
               if (lstrcmpi(name,CurBookmark)==0 && lstrlen(CurBookmark)>0) CurIdx=i;
            }
            if (CurIdx>=0) SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_SETCURSEL, CurIdx,0L);

            if (count==0) {
               EnableWindow(GetDlgItem(hDlg,IDC_INSERT),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_DELETE),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_GOTO),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_PAGE_REF),FALSE);
            }

            SetFocus(GetDlgItem(hDlg,IDC_LIST_BOX));

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            cmd=CONTROL_ID(wParam,lParam);

            switch(cmd) {
                case IDC_LIST_BOX:       // mark the default font
                   name[0]=0;
                   if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {  // selection about to change
                     int idx=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_GETCURSEL,0,0L);
                     if (idx!=LB_ERR) SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_GETLBTEXT,idx,(LPARAM)(LPBYTE)name);
                   } 
                   else GetDlgItemText(hDlg,IDC_LIST_BOX,name,sizeof(name)-1);
                   ValidName=IsValidBookmark(w,name,FALSE);
                   exists=IsValidBookmark(w,name,TRUE);
                   EnableWindow(GetDlgItem(hDlg,IDC_INSERT),ValidName);
                   EnableWindow(GetDlgItem(hDlg,IDC_PAGE_REF),exists);
                   EnableWindow(GetDlgItem(hDlg,IDC_DELETE),exists);
                   EnableWindow(GetDlgItem(hDlg,IDC_GOTO),exists);

                   if (NOTIFY_MSG(wParam,lParam)!=CBN_DBLCLK) break;
                   if (!ValidName) break;

                   if (exists) cmd=IDC_GOTO;         // now treat it as goto
                   else        cmd=IDC_INSERT;

                case IDC_INSERT:
                case IDC_DELETE:
                case IDC_GOTO:
                case IDC_PAGE_REF:
                   //***** get selections *******
                   GetDlgItemText(hDlg,IDC_LIST_BOX,name,sizeof(name)-1);
                   StrTrim(name);
                   if (!IsValidBookmark(w,name,FALSE)) return TRUE;

                   lstrcpy(TempString,name);
                   DlgInt1=cmd;          // pass the command

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ObjectPosParam:
    This function accepts the y position base for a frame or drawing object.
******************************************************************************/
BOOL CALLBACK _export ObjectPosParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int FrameId;
    UINT flags;
    BOOL BasePage,BaseMarg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            FrameId=DlgInt1;                         // frame id passed in DlgInt1
            flags=ParaFrame[FrameId].flags;

            SendDlgItemMessage(hDlg,IDC_VERT_PAGE,BM_SETCHECK,flags&PARA_FRAME_VPAGE, 0L);
            SendDlgItemMessage(hDlg,IDC_VERT_MARG,BM_SETCHECK,flags&PARA_FRAME_VMARG, 0L);
            SendDlgItemMessage(hDlg,IDC_VERT_PARA,BM_SETCHECK,!(flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG)), 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   //***** get selections *******
                   BasePage=(BOOL)SendDlgItemMessage(hDlg,IDC_VERT_PAGE,BM_GETCHECK, 0, 0L);
                   BaseMarg=(BOOL)SendDlgItemMessage(hDlg,IDC_VERT_MARG,BM_GETCHECK, 0, 0L);

                   // pass the selection using the DlgInt2 variable
                   DlgInt2=BASE_PARA;
                   if (BasePage) DlgInt2=BASE_PAGE;
                   if (BaseMarg) DlgInt2=BASE_MARG;

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ObjectAttribParam:
    This function accepts the attributes for a drawing object.
******************************************************************************/
BOOL CALLBACK _export ObjectAttribParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int FrameId;
    UINT flags;
    BOOL DrawLine,xparent;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            FrameId=DlgInt1;                         // frame id passed in DlgInt1
            flags=ParaFrame[FrameId].flags;

            // set line thickness and line type
            SetDlgItemInt(hDlg,IDC_LINE_THICK,ParaFrame[FrameId].LineWdth,0);
            SetDlgItemInt(hDlg,IDC_ZORDER,ParaFrame[FrameId].ZOrder,TRUE);

            if (flags&PARA_FRAME_BOXED) {
               SendDlgItemMessage(hDlg,IDC_LINE_DRAW,BM_SETCHECK,TRUE, 0L);
               SendDlgItemMessage(hDlg,IDC_LINE_SOLID,BM_SETCHECK,!(flags&PARA_FRAME_DOTTED), 0L);
               SendDlgItemMessage(hDlg,IDC_LINE_DOTTED,BM_SETCHECK,(flags&PARA_FRAME_DOTTED), 0L);
            }
            else {     // diable solid/dotted line choice
               SendDlgItemMessage(hDlg,IDC_LINE_SOLID,BM_SETCHECK,TRUE, 0L);
               EnableWindow(GetDlgItem(hDlg,IDC_LINE_SOLID),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_LINE_DOTTED),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_LINE_COLOR),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_LINE_THICK),FALSE);
            }

            // Set fill type
            SendDlgItemMessage(hDlg,IDC_FILL_XPARENT,BM_SETCHECK,ParaFrame[FrameId].FillPattern==0, 0L);
            if (ParaFrame[FrameId].FillPattern==0) EnableWindow(GetDlgItem(hDlg,IDC_FILL_COLOR),FALSE);

            // wrapping
            if (flags&(PARA_FRAME_RECT|PARA_FRAME_TEXT_BOX)) {
               SendDlgItemMessage(hDlg,IDC_WRAP_THRU,BM_SETCHECK,(flags&PARA_FRAME_WRAP_THRU), 0L);
               SendDlgItemMessage(hDlg,IDC_NO_WRAP,BM_SETCHECK,(flags&PARA_FRAME_NO_WRAP), 0L);
               SendDlgItemMessage(hDlg,IDC_WRAP_AROUND,BM_SETCHECK,!(flags&(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU)), 0L);
            } 

            // disable fill options for the line type object
            if (flags&PARA_FRAME_LINE) {
               EnableWindow(GetDlgItem(hDlg,IDC_FILL_XPARENT),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_FILL_COLOR),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_WRAP_THRU),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_WRAP_AROUND),FALSE);
               EnableWindow(GetDlgItem(hDlg,IDC_NO_WRAP),FALSE);
            }

            // save the line color and the fill color in temporary variables
            DlgLong=(DWORD)ParaFrame[FrameId].LineColor;
            DlgLong1=(DWORD)ParaFrame[FrameId].BackColor;

            SetFocus(GetDlgItem(hDlg,IDC_LINE_THICK));

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LINE_DRAW:
                   DrawLine=(BOOL)SendDlgItemMessage(hDlg,IDC_LINE_DRAW,BM_GETCHECK, 0, 0L);
                   EnableWindow(GetDlgItem(hDlg,IDC_LINE_SOLID),DrawLine);
                   EnableWindow(GetDlgItem(hDlg,IDC_LINE_DOTTED),DrawLine);
                   EnableWindow(GetDlgItem(hDlg,IDC_LINE_COLOR),DrawLine);
                   EnableWindow(GetDlgItem(hDlg,IDC_LINE_THICK),DrawLine);
                   break;

                case IDC_FILL_XPARENT:
                   xparent=(BOOL)SendDlgItemMessage(hDlg,IDC_FILL_XPARENT,BM_GETCHECK, 0, 0L);
                   EnableWindow(GetDlgItem(hDlg,IDC_FILL_COLOR),!xparent);
                   break;

                case IDC_LINE_COLOR:
                   DlgLong=(DWORD)DlgEditColor(w,hDlg,(COLORREF)DlgLong,TRUE);
                   break;

                case IDC_FILL_COLOR:
                   DlgLong1=(DWORD)DlgEditColor(w,hDlg,(COLORREF)DlgLong1,TRUE);
                   break;

                case IDOK:
                   //***** get selections *******
                   if (!AssignNumVal(hDlg,IDC_LINE_THICK,&DlgInt1)) return (TRUE); // pass line thick using DlgInt1 temporary variable
                   DlgInt4=GetDlgItemInt(hDlg,IDC_ZORDER,NULL,TRUE);

                   DlgInt2=DOB_LINE_NONE;   // pass line type using DlgInt2
                   DrawLine=(BOOL)SendDlgItemMessage(hDlg,IDC_LINE_DRAW,BM_GETCHECK, 0, 0L);
                   if (DrawLine) {
                      if ((BOOL)SendDlgItemMessage(hDlg,IDC_LINE_SOLID,BM_GETCHECK, 0, 0L))
                           DlgInt2=DOB_LINE_SOLID;
                      else DlgInt2=DOB_LINE_DOTTED;
                   }

                   // pass fill solid/transparent using DlgInt3
                   DlgInt3=!((BOOL)SendDlgItemMessage(hDlg,IDC_FILL_XPARENT,BM_GETCHECK, 0, 0L));

                   // pass the wrapping flags
                   DlgInt5=0;
                   if (((BOOL)SendDlgItemMessage(hDlg,IDC_WRAP_THRU,BM_GETCHECK, 0, 0L))) DlgInt5=PARA_FRAME_WRAP_THRU;
                   if (((BOOL)SendDlgItemMessage(hDlg,IDC_NO_WRAP,BM_GETCHECK, 0, 0L))) DlgInt5=PARA_FRAME_NO_WRAP;


                   EndDialog(hDlg,TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ParaBoxParam:
    Accepts new font and point size.
******************************************************************************/
BOOL CALLBACK _export ParaBoxParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    BOOL SamePfmt=TRUE;
    long l;
    int ParaId,value,CtlId;
    UINT shading;
    UINT flags,IdLeft=IDC_BOX_LEFT,IdRight=IDC_BOX_RIGHT;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            if (EditingParaStyle) {
               flags=StyleId[CurSID].ParaFlags;
               shading=StyleId[CurSID].shading;
               DlgColor1=StyleId[CurSID].ParaBorderColor;
               SamePfmt=TRUE;
            }
            else {
               ParaId=pfmt(CurLine);
               flags=PfmtId[ParaId].flags;
               shading=PfmtId[ParaId].shading;
               DlgColor1=PfmtId[ParaId].BorderColor;

               if ((DlgInt2=IsLineRtl(w,CurLine))) {   // reverse
                  IdLeft=IDC_BOX_RIGHT;
                  IdRight=IDC_BOX_LEFT;
               } 

               if (HilightType!=HILIGHT_OFF) {
                  for (l=HilightBegRow+1;l<=HilightEndRow;l++) {
                     if (!LineSelected(w,l)) continue;
                     if ((flags&PARA_BOX_ATTRIB)!=(PfmtId[pfmt(l)].flags&PARA_BOX_ATTRIB)
                       || shading!=PfmtId[pfmt(l)].shading) break;
                  }
                  if (l<=HilightEndRow) SamePfmt=FALSE;
               }
            }

            SendMessage(GetDlgItem(hDlg,IDC_BOX_TOP)   ,BM_SETCHECK, SamePfmt ? (flags&PARA_BOX_TOP   ?BM_CHECKED:BM_UNCHECKED) :BM_NONE, 0L);
            SendMessage(GetDlgItem(hDlg,IDC_BOX_BOT)   ,BM_SETCHECK, SamePfmt ? (flags&PARA_BOX_BOT   ?BM_CHECKED:BM_UNCHECKED) :BM_NONE, 0L);
            SendMessage(GetDlgItem(hDlg,IDC_BOX_BETWEEN)   ,BM_SETCHECK, SamePfmt ? (flags&PARA_BOX_BETWEEN   ?BM_CHECKED:BM_UNCHECKED) :BM_NONE, 0L);
            SendMessage(GetDlgItem(hDlg,IdLeft)  ,BM_SETCHECK, SamePfmt ? (flags&PARA_BOX_LEFT  ?BM_CHECKED:BM_UNCHECKED) :BM_NONE, 0L);
            SendMessage(GetDlgItem(hDlg,IdRight) ,BM_SETCHECK, SamePfmt ? (flags&PARA_BOX_RIGHT ?BM_CHECKED:BM_UNCHECKED) :BM_NONE, 0L);
            SendMessage(GetDlgItem(hDlg,IDC_BOX_DOUBLE),BM_SETCHECK, SamePfmt ? (flags&PARA_BOX_DOUBLE?BM_CHECKED:BM_UNCHECKED) :BM_NONE, 0L);
            SendMessage(GetDlgItem(hDlg,IDC_BOX_THICK) ,BM_SETCHECK, SamePfmt ? (flags&PARA_BOX_THICK ?BM_CHECKED:BM_UNCHECKED) :BM_NONE, 0L);

            if (SamePfmt) SetDlgItemInt(hDlg,IDC_SHADING,shading/100,0);

            DlgInt1=SamePfmt;

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_COLOR:
                   DlgColor1=DlgEditColor(w,hDlg,DlgColor1,TRUE);
                   break;
                
                case IDC_BOX_TOP:
                case IDC_BOX_BOT:
                case IDC_BOX_BETWEEN:
                case IDC_BOX_LEFT:
                case IDC_BOX_RIGHT:
                case IDC_BOX_DOUBLE:
                case IDC_BOX_THICK:
                   CtlId=CONTROL_ID(wParam,lParam);
                   SamePfmt=DlgInt1;
                   if (SamePfmt) {
                      value=(int)SendMessage(GetDlgItem(hDlg,CtlId),BM_GETCHECK, 0, 0L);
                      if (value==BM_NONE) SendMessage(GetDlgItem(hDlg,CtlId) ,BM_SETCHECK, FALSE, 0L);
                   }
                   break;

                case IDOK:
                   //***** get selections *******
                   DlgOnFlags=DlgOffFlags=0;               // enumerate on and off flags
                   SamePfmt=DlgInt1;

                   if ((DlgInt2=IsLineRtl(w,CurLine))) {   // reverse
                      IdLeft=IDC_BOX_RIGHT;
                      IdRight=IDC_BOX_LEFT;
                   } 

                   // top edge
                   value=(int)SendMessage(GetDlgItem(hDlg,IDC_BOX_TOP),BM_GETCHECK, 0, 0L);
                   if (value!=BM_NONE) {
                      if (value) DlgOnFlags|=PARA_BOX_TOP;   // this flag turned on
                      else       DlgOffFlags|=PARA_BOX_TOP;  // this flag turned off
                   }
                   // bottom edge
                   value=(int)SendMessage(GetDlgItem(hDlg,IDC_BOX_BOT),BM_GETCHECK, 0, 0L);
                   if (value!=BM_NONE) {
                      if (value) DlgOnFlags|=PARA_BOX_BOT;   // this flag turned on
                      else       DlgOffFlags|=PARA_BOX_BOT;  // this flag turned off
                   }

                   // between lines
                   value=(int)SendMessage(GetDlgItem(hDlg,IDC_BOX_BETWEEN),BM_GETCHECK, 0, 0L);
                   if (value!=BM_NONE) {
                      if (value) DlgOnFlags|=PARA_BOX_BETWEEN;   // this flag turned on
                      else       DlgOffFlags|=PARA_BOX_BETWEEN;  // this flag turned off
                   }

                   // left edge
                   value=(int)SendMessage(GetDlgItem(hDlg,IdLeft),BM_GETCHECK, 0, 0L);
                   if (value!=BM_NONE) {
                      if (value) DlgOnFlags|=PARA_BOX_LEFT;   // this flag turned on
                      else       DlgOffFlags|=PARA_BOX_LEFT;  // this flag turned off
                   }
                   // right edge
                   value=(int)SendMessage(GetDlgItem(hDlg,IdRight),BM_GETCHECK, 0, 0L);
                   if (value!=BM_NONE) {
                      if (value) DlgOnFlags|=PARA_BOX_RIGHT;   // this flag turned on
                      else       DlgOffFlags|=PARA_BOX_RIGHT;  // this flag turned off
                   }
                   // double line frame
                   value=(int)SendMessage(GetDlgItem(hDlg,IDC_BOX_DOUBLE),BM_GETCHECK, 0, 0L);
                   if (value!=BM_NONE) {
                      if (value) DlgOnFlags|=PARA_BOX_DOUBLE;   // this flag turned on
                      else       DlgOffFlags|=PARA_BOX_DOUBLE;  // this flag turned off
                   }
                   // thick frame
                   value=(int)SendMessage(GetDlgItem(hDlg,IDC_BOX_THICK),BM_GETCHECK, 0, 0L);
                   if (value!=BM_NONE) {
                      if (value) DlgOnFlags|=PARA_BOX_THICK;   // this flag turned on
                      else       DlgOffFlags|=PARA_BOX_THICK;  // this flag turned off
                   }

                   // shading percentage
                   GetDlgItemText(hDlg,IDC_SHADING,TempString,sizeof(TempString));
                   StrTrim(TempString);   // pass using the temp string variable

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ParaSpaceParam:
    Accepts new paragraph spacing parameters.
******************************************************************************/
BOOL CALLBACK _export ParaSpaceParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    BOOL SamePfmt=TRUE;
    long l;
    int ParaId,SpaceBetween,LineSpacing,CurIdx;
    float multiple;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // load the list box
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_ADDSTRING, PSPACE_SINGLE, (DWORD)(LPBYTE)MsgString[MSG_SINGLE]);
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_ADDSTRING, PSPACE_15, (DWORD)(LPBYTE)MsgString[MSG_15LINES]);
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_ADDSTRING, PSPACE_20, (DWORD)(LPBYTE)MsgString[MSG_DBL_LINE]);
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_ADDSTRING, PSPACE_ATLEAST, (DWORD)(LPBYTE)MsgString[MSG_ATLEAST]);
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_ADDSTRING, PSPACE_EXACTLY, (DWORD)(LPBYTE)MsgString[MSG_EXACTLY]);
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_ADDSTRING, PSPACE_MULTIPLE, (DWORD)(LPBYTE)MsgString[MSG_MULTIPLE]);
            
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_SETCURSEL, 0,0L);
            EnableWindow(GetDlgItem(hDlg,IDC_PARA_SPACE),FALSE);
            SetDlgItemText(hDlg,IDC_PARA_SPACE_LBL,"");

            // dialog field initialization
            if (EditingParaStyle) SamePfmt=TRUE;
            else {
               if (HilightType!=HILIGHT_OFF) {
                  for (l=HilightBegRow+1;l<=HilightEndRow;l++) {
                     if (!LineSelected(w,l)) continue;
                     if (pfmt(l)!=pfmt(HilightBegRow)) break;
                  }
                  if (l<=HilightEndRow) SamePfmt=FALSE;
               }
               ParaId=pfmt(CurLine);
            }

            SpaceBetween=12;              // default
            LineSpacing=0;
            
            if (SamePfmt) {
               if (EditingParaStyle) {
                  SetDlgItemInt(hDlg,IDC_PARA_SPACE_BEF,TwipsToPoints(StyleId[CurSID].SpaceBefore),0);
                  SetDlgItemInt(hDlg,IDC_PARA_SPACE_AFT,TwipsToPoints(StyleId[CurSID].SpaceAfter),0);
                  SpaceBetween=TwipsToPoints(StyleId[CurSID].SpaceBetween);
                  LineSpacing=StyleId[CurSID].LineSpacing;
                  if (StyleId[CurSID].ParaFlags&DOUBLE_SPACE) LineSpacing=100;
               }
               else {
                  SetDlgItemInt(hDlg,IDC_PARA_SPACE_BEF,TwipsToPoints(PfmtId[ParaId].SpaceBefore),0);
                  SetDlgItemInt(hDlg,IDC_PARA_SPACE_AFT,TwipsToPoints(PfmtId[ParaId].SpaceAfter),0);
                  SpaceBetween=TwipsToPoints(PfmtId[ParaId].SpaceBetween);
                  LineSpacing=PfmtId[ParaId].LineSpacing;
                  if (PfmtId[ParaId].flags&DOUBLE_SPACE) LineSpacing=100;
               }

               if (LineSpacing==0) {
                  if (SpaceBetween==0)        CurIdx=PSPACE_SINGLE;
                  else if (SpaceBetween>0)    CurIdx=PSPACE_ATLEAST;
                  else                        CurIdx=PSPACE_EXACTLY;
               }
               else {
                 if (LineSpacing==50)         CurIdx=PSPACE_15;
                 else if (LineSpacing==100)   CurIdx=PSPACE_20;
                 else                         CurIdx=PSPACE_MULTIPLE;
               }

               SetParaSpaceDlg(w,hDlg,CurIdx,SpaceBetween,LineSpacing);
            }
            
            DlgInt3=SpaceBetween;         // globals
            DlgInt4=LineSpacing;

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                       CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_GETCURSEL, 0,0L);
                       if (CurIdx==CB_ERR) return (TRUE);
                       SetParaSpaceDlg(w,hDlg,CurIdx,DlgInt3,DlgInt4);
                   }
                   break;
                
                
                case IDOK:
                   // pass space parameters using DlgInt... variables
                   if (!AssignNumVal(hDlg,IDC_PARA_SPACE_BEF,&DlgInt1)) return (TRUE);
                   if (!AssignNumVal(hDlg,IDC_PARA_SPACE_AFT,&DlgInt2)) return (TRUE);
                   
                   CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_GETCURSEL, 0,0L);
                   if (CurIdx==CB_ERR) CurIdx=0;
                   
                   SpaceBetween=LineSpacing=0;
                   if (CurIdx==PSPACE_15) LineSpacing=50;      // percent increase
                   if (CurIdx==PSPACE_20) LineSpacing=100;     // percent increase
                   if (CurIdx==PSPACE_MULTIPLE) {
                      GetDlgItemText(hDlg,IDC_PARA_SPACE,TempString,100);
                      multiple=(float)TerAtof(w,TempString);
                      if (multiple<(float).5) multiple=(float).5;
                      if (multiple>(float)9) multiple=(float)9;
                      LineSpacing=(int)((multiple*100)-100);         // pecentage incease
                   } 
                   if (CurIdx==PSPACE_ATLEAST) {
                      if (!AssignNumVal(hDlg,IDC_PARA_SPACE,&SpaceBetween)) return (TRUE);
                   } 
                   if (CurIdx==PSPACE_EXACTLY) {
                      if (!AssignNumVal(hDlg,IDC_PARA_SPACE,&SpaceBetween)) return (TRUE);
                      SpaceBetween=-SpaceBetween;
                   } 
                   DlgInt3=SpaceBetween;
                   DlgInt4=LineSpacing;

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    SetParaSpaceDlg:
    Set dialog controls for para spacing
******************************************************************************/
SetParaSpaceDlg(PTERWND w, HWND hDlg,int idx, int SpaceBetween, int LineSpacing)
{
    double multiple;

    SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_SETCURSEL, idx,0L);

    if (idx==PSPACE_SINGLE || idx==PSPACE_15 || idx==PSPACE_20) {
       EnableWindow(GetDlgItem(hDlg,IDC_PARA_SPACE),FALSE);
       SetDlgItemText(hDlg,IDC_PARA_SPACE,"");
       SetDlgItemText(hDlg,IDC_PARA_SPACE_LBL,"");
    }
    else if (idx==PSPACE_MULTIPLE) {
        multiple=((double)LineSpacing+100)/100;
        SetDlgItemText(hDlg,IDC_PARA_SPACE,DoubleToStr(w,multiple,2,TempString));
        EnableWindow(GetDlgItem(hDlg,IDC_PARA_SPACE),TRUE);
        SetDlgItemText(hDlg,IDC_PARA_SPACE_LBL,MsgString[MSG_LINES]);
    }
    else if (idx==PSPACE_EXACTLY || idx==PSPACE_ATLEAST) {  // exact or 'atleast' line spacing
       if (SpaceBetween<0) SpaceBetween=-SpaceBetween;
       if (SpaceBetween==0) SpaceBetween=12;                // supply default
       SetDlgItemInt(hDlg,IDC_PARA_SPACE,SpaceBetween,0);
       EnableWindow(GetDlgItem(hDlg,IDC_PARA_SPACE),TRUE);
       SetDlgItemText(hDlg,IDC_PARA_SPACE_LBL,MsgString[MSG_POINTS]);
    }
 
    return TRUE;
}
 
/******************************************************************************
    EditStyleParam:
    Accepts new paragraph spacing parameters.
******************************************************************************/
BOOL CALLBACK _export EditStyleParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    BOOL NewStyle,type;
    int i;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // select the paragraph styles by default
            SendDlgItemMessage(hDlg,IDC_SS_PARA, BM_SETCHECK,TRUE, 0L);

            FillStyleBox(w,hDlg,IDC_LIST_BOX,SSTYPE_PARA,FALSE,TRUE);  // fill the list box with paragraph styles

            EnableWindow(GetDlgItem(hDlg,IDC_SS_NAME),FALSE);  // disable the
            EnableWindow(GetDlgItem(hDlg,IDC_SS_NAME_LABEL),FALSE);  // disable the

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_SS_CHAR:
                case IDC_SS_PARA:
                   if  ((BOOL)SendDlgItemMessage(hDlg,IDC_SS_PARA,BM_GETCHECK, 0, 0L)) {
                      FillStyleBox(w,hDlg,IDC_LIST_BOX,SSTYPE_PARA,FALSE,TRUE);   // fill the list box with paragraph styles
                      EnableWindow(GetDlgItem(hDlg,IDC_LIST_BOX),TRUE);
                      EnableWindow(GetDlgItem(hDlg,IDC_SS_BOX_LABEL),TRUE);
                      //EnableWindow(GetDlgItem(hDlg,IDC_SS_NAME),FALSE);
                      //EnableWindow(GetDlgItem(hDlg,IDC_SS_NAME_LABEL),FALSE);
                   }
                   else {
                      int count=0;
                      FillStyleBox(w,hDlg,IDC_LIST_BOX,SSTYPE_CHAR,FALSE,TRUE);   // fill the list box with character styles
                      for (i=0;i<TotalSID;i++) if (StyleId[i].InUse && StyleId[i].type==SSTYPE_CHAR) count++;  // count the character styles
                      if (count<=1) {
                         SendDlgItemMessage(hDlg,IDC_SS_NEW, BM_SETCHECK,TRUE, 0L);
                         EnableWindow(GetDlgItem(hDlg,IDC_LIST_BOX),FALSE);
                         EnableWindow(GetDlgItem(hDlg,IDC_SS_BOX_LABEL),FALSE);
                         EnableWindow(GetDlgItem(hDlg,IDC_SS_NAME),TRUE);
                         EnableWindow(GetDlgItem(hDlg,IDC_SS_NAME_LABEL),TRUE);
                      }
                   }
                   break;

                case IDC_SS_NEW:
                   NewStyle=(BOOL)SendDlgItemMessage(hDlg,IDC_SS_NEW,BM_GETCHECK, 0, 0L);
                   EnableWindow(GetDlgItem(hDlg,IDC_LIST_BOX),!NewStyle);
                   EnableWindow(GetDlgItem(hDlg,IDC_SS_BOX_LABEL),!NewStyle);
                   EnableWindow(GetDlgItem(hDlg,IDC_SS_NAME),NewStyle);
                   EnableWindow(GetDlgItem(hDlg,IDC_SS_NAME_LABEL),NewStyle);
                   break;

                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                   NewStyle=DlgInt1=(BOOL)SendDlgItemMessage(hDlg,IDC_SS_NEW,BM_GETCHECK, 0, 0L);
                   type=(BOOL)SendDlgItemMessage(hDlg,IDC_SS_PARA,BM_GETCHECK, 0, 0L);
                   if (type) type=SSTYPE_PARA;
                   else      type=SSTYPE_CHAR;
                   DlgInt2=type;              // to pass to the calling routine

                   if (NewStyle) {
                      GetDlgItemText(hDlg,IDC_SS_NAME,TempString,MAX_SS_NAME_WIDTH);
                      StrTrim(TempString);
                      if (lstrlen(TempString)==0) {
                         MessageBox(hDlg,MsgString[MSG_TYPE_STYLE_NAME],NULL,MB_OK);
                         SetFocus(GetDlgItem(hDlg,IDC_SS_NAME));
                         return TRUE;
                      }

                      // check for duplicate names
                      for (i=0;i<TotalSID;i++) {  // check for duplicate names
                         if (StyleId[i].InUse && lstrcmpi(StyleId[i].name,TempString)==0) {
                            if (StyleId[i].type==SSTYPE_PARA) MessageBox(hDlg,MsgString[MSG_PARASTYLE_IN_USE],NULL,MB_OK);
                            else                              MessageBox(hDlg,MsgString[MSG_CHARSTYLE_IN_USE],NULL,MB_OK);
                            SetFocus(GetDlgItem(hDlg,IDC_SS_NAME));
                            return TRUE;
                         }
                      }
                   }
                   else {
                      i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                      i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETITEMDATA, i,0L);
                      lstrcpy(TempString,StyleId[i].name);   // pass to the calling routine
                   }

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, 0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    SelectStyleParam:
    Accepts new paragraph spacing parameters.
******************************************************************************/
int CALLBACK _export SelectStyleParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int type;
    int i;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            type=(int)(DWORD)(dlg->data);    // SSTYPE_CHAR or SSTYPE_PARA

            FillStyleBox(w,hDlg,IDC_LIST_BOX,type,TRUE,TRUE);  // fill the list box with paragraph styles

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                  i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                  i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETITEMDATA, i,0L);

                   EndDialog(hDlg, i);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, -1);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ListParaParam:
    Select a list override/level for paragraph
******************************************************************************/
BOOL CALLBACK _export ListParaParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int i;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            FillListOrBox(w,hDlg,IDC_LIST_BOX,TRUE,TRUE,DlgInt1,FALSE);  // fill the list box with paragraph styles
            SetDlgListParaLevel(w,hDlg,DlgInt2);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)==LBN_SELCHANGE) {     // enable/disable paste link
                       SetDlgListParaLevel(w,hDlg,DlgInt2);
                       break;
                   }
                   else if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                   i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                   DlgInt1=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETITEMDATA, i,0L);

                   i=(int)SendDlgItemMessage(hDlg,IDC_LEVEL,CB_GETCURSEL, 0,0L);
                   DlgInt2=(int)SendDlgItemMessage(hDlg,IDC_LEVEL,CB_GETITEMDATA, i,0L);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}


/******************************************************************************
    ListOrSelectParam:
    Select a list override 
******************************************************************************/
int CALLBACK _export ListOrSelectParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int i;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            FillListOrBox(w,hDlg,IDC_LIST_BOX,TRUE,TRUE,-1,FALSE);  // fill the list box with paragraph styles

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                  i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                  i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETITEMDATA, i,0L);

                   EndDialog(hDlg, i);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, -1);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ListOrPropParam:
    List override property parameters
******************************************************************************/
BOOL CALLBACK _export ListOrPropParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurIdx;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            FillListBox(w,hDlg,IDC_LIST_BOX,TRUE,FALSE,DlgInt1);  // fill the list box with paragraph styles
            SendDlgItemMessage(hDlg,IDC_OVERRIDE,BM_SETCHECK, DlgInt2, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   // get the list id
                   CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_GETCURSEL, 0,0L);
                   if (CurIdx==CB_ERR) return (TRUE);
                   DlgInt1=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_GETITEMDATA, CurIdx,0L);
             
                   DlgInt2=(BOOL)SendDlgItemMessage(hDlg,IDC_OVERRIDE,BM_GETCHECK, 0, 0L);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}



/******************************************************************************
    ListSelectParam:
    Select a list 
******************************************************************************/
int CALLBACK _export ListSelectParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int i;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            FillListBox(w,hDlg,IDC_LIST_BOX,TRUE,TRUE,-1);  // fill the list box with paragraph styles

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                  i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                  i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETITEMDATA, i,0L);

                   EndDialog(hDlg, i);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, -1);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ListPropParam:
    List property parameters
******************************************************************************/
BOOL CALLBACK _export ListPropParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            SetDlgItemText(hDlg,IDC_NAME,DlgText1);
            SendDlgItemMessage(hDlg,IDC_NESTED,BM_SETCHECK, DlgInt1, 0L);
            SendDlgItemMessage(hDlg,IDC_RESTART_AT_SEC,BM_SETCHECK, (DlgLong&LISTFLAG_RESTART_SEC)?TRUE:FALSE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   GetDlgItemText(hDlg,IDC_NAME,DlgText1,99);
             
                   DlgInt1=(BOOL)SendDlgItemMessage(hDlg,IDC_NESTED,BM_GETCHECK, 0, 0L);

                   ResetUintFlag(DlgLong,LISTFLAG_RESTART_SEC);
                   if ((BOOL)SendDlgItemMessage(hDlg,IDC_RESTART_AT_SEC,BM_GETCHECK, 0, 0L)) DlgLong|=LISTFLAG_RESTART_SEC; 

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    ListLevelParam:
    List level parameters
******************************************************************************/
BOOL CALLBACK _export ListLevelParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int CurListOr;
    struct StrListLevel far *pLevel;
    struct StrListLevel level;
    BYTE string[200];
    BOOL CheckListItem;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            FillListBox(w,hDlg,IDC_LIST_BOX,TRUE,FALSE,-1);  // fill the list box with paragraph styles
            CurListOr=FillListOrBox(w,hDlg,IDC_LIST_OR_BOX,TRUE,FALSE,-1,TRUE);  // fill the list box with paragraph styles
            
            CheckListItem=(CurListOr<=0 || ListOr[CurListOr].LevelCount==0);
            SendDlgItemMessage(hDlg,IDC_LIST_ITEM,BM_SETCHECK, CheckListItem, 0L);
            SendDlgItemMessage(hDlg,IDC_LIST_OR,BM_SETCHECK, !CheckListItem, 0L);

            SetDlgListLevel(w,hDlg);
            SetDlgListLevelProp(w,hDlg);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_ITEM:
                case IDC_LIST_OR:
                   SetDlgListLevel(w,hDlg);
                   SetDlgListLevelProp(w,hDlg);
                   break;

                case IDC_LIST_BOX:
                case IDC_LIST_OR_BOX:
                   if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                      SetDlgListLevel(w,hDlg);
                      SetDlgListLevelProp(w,hDlg);
                   } 
                   break;
                
                case IDC_LEVEL:
                   if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                      SetDlgListLevelProp(w,hDlg);
                   }
                   break;
             
                case IDC_FONTS:
                   DlgInt1=DlgEditListFont(w,hDlg,DlgInt1);
                   break;

                case IDOK:
                   // get the list level info
                   if (!(pLevel=GetDlgListLevelPtr(w,hDlg))) return TRUE;
                   FarMove(pLevel,&level,sizeof(level));  // make a temporary copy

                   // get the flags
                   ResetLongFlag(level.flags,(LISTLEVEL_RESTART|LISTLEVEL_LEGAL|LISTLEVEL_REFORMAT|LISTLEVEL_NO_RESET));
                   if (SendDlgItemMessage(hDlg,IDC_RESTART,BM_GETCHECK, 0, 0L)) level.flags|=LISTLEVEL_RESTART; 
                   if (SendDlgItemMessage(hDlg,IDC_LEGAL,BM_GETCHECK, 0, 0L)) level.flags|=LISTLEVEL_LEGAL; 
                   if (SendDlgItemMessage(hDlg,IDC_REFORMAT,BM_GETCHECK, 0, 0L)) level.flags|=LISTLEVEL_REFORMAT; 
                   if (SendDlgItemMessage(hDlg,IDC_NO_RESET,BM_GETCHECK, 0, 0L)) level.flags|=LISTLEVEL_NO_RESET; 
                   
                   // get the text
                   GetDlgItemText(hDlg,IDC_TEXT,string,100);
                   CodeListText(w,string,level.text);

                   // Get start at
                   level.start=GetDlgItemInt(hDlg,IDC_START_AT,NULL,TRUE);

                   // get combobox fields
                   level.NumType=DlgListNumType(w,hDlg,IDC_NUM_TYPE,level.NumType,FALSE);
                   level.CharAft=DlgListCharAft(w,hDlg,IDC_CHAR_AFT,level.CharAft,FALSE);

                   level.FontId=DlgInt1;   // font id

                   FarMove(&level,pLevel,sizeof(level));  // make a temporary copy

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}



/******************************************************************************
    DateParam:
    Accepts new paragraph spacing parameters.
******************************************************************************/
BOOL CALLBACK _export DateParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int i;
    BYTE DateString[50];

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            for (i=0;i<TotalDateFmts;i++) {
               GetDateString(w,DateFieldText[i],DateString,-2);
               SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_ADDSTRING, 0, (DWORD)(LPBYTE)DateString);
            }
            SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_SETCURSEL, 0,0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                   i=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETCURSEL, 0,0L);
                   lstrcpy(DlgText1,DateFieldText[i]);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg,FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    FillStyleBox:
    Fill the style list box with either the paragraph or character styles. Set type to 0
    to fill both paragraph and character styles
******************************************************************************/
FillStyleBox(PTERWND w,HWND hDlg,int CtlId, int type, BOOL SelectCurrent, BOOL ListBox)
{
    int i,idx,CurStyle=0,font,ItemCount=0;
    UINT msg;

    // get the current style
    if (SelectCurrent) {
       if (type==SSTYPE_CHAR) {
          font=GetEffectiveCfmt(w);
          CurStyle=TerFont[font].CharStyId;
       }
       else if (type==SSTYPE_PARA) CurStyle=PfmtId[pfmt(CurLine)].StyId;
       else {          // both character and paragraph
          CurStyle=PfmtId[pfmt(CurLine)].StyId;
          if (CurStyle==0) {  // default paragraph style
            font=GetEffectiveCfmt(w);
            if (TerFont[font].CharStyId!=1) CurStyle=TerFont[font].CharStyId;
          }
       } 
    }

    // delete the previous contents of thelist box
    msg=ListBox?LB_RESETCONTENT:CB_RESETCONTENT;
    SendMessage(GetDlgItem(hDlg,CtlId),msg, 0, 0L);

    // fill the list box
    for (i=0;i<TotalSID;i++) {    // process each styles
       if (StyleId[i].InUse && (StyleId[i].type==type || type==0)) {
          msg=ListBox?LB_ADDSTRING:CB_ADDSTRING;
          idx=(int)SendDlgItemMessage(hDlg,CtlId,msg, 0, (DWORD)(LPBYTE)(StyleId[i].name));
          msg=ListBox?LB_SETITEMDATA:CB_SETITEMDATA;
          SendMessage(GetDlgItem(hDlg,CtlId),msg, idx, i);
          ItemCount++;
       }
    }

    // select the first item
    if (SelectCurrent) {
       for (i=0;i<ItemCount;i++) {
          msg=ListBox?LB_GETITEMDATA:CB_GETITEMDATA;
          if (CurStyle==(int)SendDlgItemMessage(hDlg,CtlId,msg,i,0L)) {
             msg=ListBox?LB_SETCURSEL:CB_SETCURSEL;
             SendDlgItemMessage(hDlg,CtlId,msg, i,0L);
             break;
          }
       }
    }
    else {
       msg=ListBox?LB_SETCURSEL:CB_SETCURSEL;
       SendDlgItemMessage(hDlg,CtlId,msg, 0,0L);
    }

    return TRUE;
}

/******************************************************************************
    FillListOrBox:
    Fill the list override box with list override ids.
******************************************************************************/
FillListOrBox(PTERWND w,HWND hDlg,int CtlId, BOOL SelectCurrent, BOOL ListBox, int CurListOr, BOOL MustHaveLevels)
{
    int i,idx,CurList,ItemCount=0;
    UINT msg;
    BYTE name[100];

    // get the current style
    if (SelectCurrent && CurListOr<0) {
       int ParaId=pfmt(CurLine);
       int BltId=PfmtId[ParaId].BltId;
       CurListOr=1;
       if (BltId>0 && TerBlt[BltId].ls>0) CurListOr=TerBlt[BltId].ls;
    }

    // delete the previous contents of thelist box
    msg=ListBox?LB_RESETCONTENT:CB_RESETCONTENT;
    SendMessage(GetDlgItem(hDlg,CtlId),msg, 0, 0L);

    // fill the list box
    for (i=1;i<TotalListOr;i++) {    // process each list
       if (ListOr[i].InUse) {
          if (MustHaveLevels && ListOr[i].LevelCount==0) continue;

          msg=ListBox?LB_ADDSTRING:CB_ADDSTRING;
          CurList=ListOr[i].ListIdx;
          if (list[CurList].name && lstrlen(list[CurList].name)>0) 
                wsprintf(name,"List: %s, Override# %03d",list[CurList].name,i);
          else  wsprintf(name,"List Override #%03d",i);
          idx=(int)SendDlgItemMessage(hDlg,CtlId,msg, 0, (DWORD)(LPBYTE)(name));
          msg=ListBox?LB_SETITEMDATA:CB_SETITEMDATA;
          SendMessage(GetDlgItem(hDlg,CtlId),msg, idx, i);
          ItemCount++;
       }
    }

    // select the first item
    if (SelectCurrent && CurListOr!=0) {
       for (i=0;i<ItemCount;i++) {
          msg=ListBox?LB_GETITEMDATA:CB_GETITEMDATA;
          if (CurListOr==(int)SendDlgItemMessage(hDlg,CtlId,msg,i,0L)) {
             msg=ListBox?LB_SETCURSEL:CB_SETCURSEL;
             SendDlgItemMessage(hDlg,CtlId,msg, i,0L);
             break;
          }
       }
    }
    else {
       msg=ListBox?LB_SETCURSEL:CB_SETCURSEL;
       SendDlgItemMessage(hDlg,CtlId,msg, 0,0L);
    }

    return CurListOr;
}


/******************************************************************************
    FillListBox:
    Fill the list box with list ids.
******************************************************************************/
FillListBox(PTERWND w,HWND hDlg,int CtlId, BOOL SelectCurrent, BOOL ListBox, int CurList)
{
    int i,idx,ItemCount=0;
    UINT msg;
    BYTE name[100];

    // get the current style
    if (SelectCurrent && CurList<0) {
       int ParaId=pfmt(CurLine);
       int BltId=PfmtId[ParaId].BltId;
       CurList=1;
       if (BltId>0 && TerBlt[BltId].ls>0) {
          int CurListOr=TerBlt[BltId].ls;
          CurList=ListOr[CurListOr].ListIdx;
       } 
    }

    // delete the previous contents of thelist box
    msg=ListBox?LB_RESETCONTENT:CB_RESETCONTENT;
    SendMessage(GetDlgItem(hDlg,CtlId),msg, 0, 0L);

    // fill the list box
    for (i=1;i<TotalLists;i++) {    // process each list
       if (list[i].InUse) {
          msg=ListBox?LB_ADDSTRING:CB_ADDSTRING;
          if (list[i].name && lstrlen(list[i].name)>0) 
                lstrcpy(name,list[i].name);
          else  wsprintf(name,"Unnamed List #%02d",i);
          idx=(int)SendDlgItemMessage(hDlg,CtlId,msg, 0, (DWORD)(LPBYTE)(name));
          msg=ListBox?LB_SETITEMDATA:CB_SETITEMDATA;
          SendMessage(GetDlgItem(hDlg,CtlId),msg, idx, i);
          ItemCount++;
       }
    }

    // select the first item
    if (SelectCurrent) {
       for (i=0;i<ItemCount;i++) {
          msg=ListBox?LB_GETITEMDATA:CB_GETITEMDATA;
          if (CurList==(int)SendDlgItemMessage(hDlg,CtlId,msg,i,0L)) {
             msg=ListBox?LB_SETCURSEL:CB_SETCURSEL;
             SendDlgItemMessage(hDlg,CtlId,msg, i,0L);
             break;
          }
       }
    }
    else {
       msg=ListBox?LB_SETCURSEL:CB_SETCURSEL;
       SendDlgItemMessage(hDlg,CtlId,msg, 0,0L);
    }

    return TRUE;
}

/******************************************************************************
    SetDlgListParaLevel:
    Fill the list level box in the ListParaParam dialog box
******************************************************************************/
SetDlgListParaLevel(PTERWND w,HWND hDlg, int CurLevel)
{
    int i,idx,CurIdx,CurList,CurListOr,CurCount,LevelCount;
    BYTE string[10];
    BOOL ListActive=FALSE;

    CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),LB_GETCURSEL, 0,0L);
    if (CurIdx==LB_ERR) return (TRUE);
    CurListOr=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,LB_GETITEMDATA, CurIdx,0L);
    LevelCount=ListOr[CurListOr].LevelCount;
    if (LevelCount==0) {     // get the level info from the list
        CurList=ListOr[CurListOr].ListIdx;
        LevelCount=list[CurList].LevelCount;
    } 
 
    CurCount=(int)SendMessage(GetDlgItem(hDlg,IDC_LEVEL),CB_GETCOUNT, 0,0L);

    if (CurCount!=LevelCount) {              // set the new count
       SendMessage(GetDlgItem(hDlg,IDC_LEVEL),CB_RESETCONTENT, 0, 0L);
       for (i=0;i<LevelCount;i++) {
         wsprintf(string,"%d",i+1);
         idx=(int)SendDlgItemMessage(hDlg,IDC_LEVEL,CB_ADDSTRING, 0, (DWORD)(LPBYTE)string);
         SendMessage(GetDlgItem(hDlg,IDC_LEVEL),CB_SETITEMDATA, idx, i);
       }
       
       for (i=0;i<LevelCount;i++) {
          if (CurLevel==(int)SendDlgItemMessage(hDlg,IDC_LEVEL,CB_GETITEMDATA,i,0L)) {
             SendDlgItemMessage(hDlg,IDC_LEVEL,CB_SETCURSEL, i,0L);
             break;
          }
       }
        
    }

    return TRUE;
} 

/******************************************************************************
    SetDlgListLevel:
    Fill the list level box in the ListLevelParam dialog box
******************************************************************************/
SetDlgListLevel(PTERWND w,HWND hDlg)
{
    int i,idx,CurIdx,CurList,CurListOr,CurCount,LevelCount;
    BYTE string[10];
    BOOL ListActive=FALSE;

    if (SendDlgItemMessage(hDlg,IDC_LIST_ITEM,BM_GETCHECK, 0, 0L)) {
       CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_GETCURSEL, 0,0L);
       if (CurIdx==CB_ERR) return (TRUE);
       CurList=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_GETITEMDATA, CurIdx,0L);
       LevelCount=list[CurList].LevelCount;
       ListActive=TRUE;
    }
    else {
       CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_OR_BOX),CB_GETCURSEL, 0,0L);
       if (CurIdx==CB_ERR) return (TRUE);
       CurListOr=(int)SendDlgItemMessage(hDlg,IDC_LIST_OR_BOX,CB_GETITEMDATA, CurIdx,0L);
       LevelCount=ListOr[CurListOr].LevelCount;
    } 
 
    // enable/diable the list/listor comboboxes
    EnableWindow(GetDlgItem(hDlg,IDC_LIST_BOX),ListActive);
    EnableWindow(GetDlgItem(hDlg,IDC_LIST_OR_BOX),!ListActive);


    CurCount=(int)SendMessage(GetDlgItem(hDlg,IDC_LEVEL),CB_GETCOUNT, 0,0L);

    if (CurCount!=LevelCount) {              // set the new count
       SendMessage(GetDlgItem(hDlg,IDC_LEVEL),CB_RESETCONTENT, 0, 0L);
       for (i=0;i<LevelCount;i++) {
         wsprintf(string,"%d",i+1);
         idx=(int)SendDlgItemMessage(hDlg,IDC_LEVEL,CB_ADDSTRING, 0, (DWORD)(LPBYTE)string);
         SendMessage(GetDlgItem(hDlg,IDC_LEVEL),CB_SETITEMDATA, idx, i);
       } 
       SendDlgItemMessage(hDlg,IDC_LEVEL,CB_SETCURSEL, 0,0L);
    }

    return TRUE;
} 

/******************************************************************************
    SetDlgListLevelProp:
    Set the ListLevelParam dialog field for the current list level
******************************************************************************/
SetDlgListLevelProp(PTERWND w,HWND hDlg)
{
    struct StrListLevel far *pLevel;
    BYTE text[200];

    if (!(pLevel=GetDlgListLevelPtr(w,hDlg))) return FALSE;    // get the current list level pointer
 
    // Set flags
    SendDlgItemMessage(hDlg,IDC_RESTART,BM_SETCHECK, (pLevel->flags&LISTLEVEL_RESTART)?TRUE:FALSE, 0L);
    SendDlgItemMessage(hDlg,IDC_LEGAL,BM_SETCHECK, (pLevel->flags&LISTLEVEL_LEGAL)?TRUE:FALSE, 0L);
    SendDlgItemMessage(hDlg,IDC_REFORMAT,BM_SETCHECK, (pLevel->flags&LISTLEVEL_REFORMAT)?TRUE:FALSE, 0L);
    SendDlgItemMessage(hDlg,IDC_NO_RESET,BM_SETCHECK, (pLevel->flags&LISTLEVEL_NO_RESET)?TRUE:FALSE, 0L);

    // set start at
    SetDlgItemInt(hDlg,IDC_START_AT,pLevel->start,0);     // default value

    // set combo fields
    DlgListNumType(w,hDlg,IDC_NUM_TYPE,pLevel->NumType,TRUE);
    DlgListCharAft(w,hDlg,IDC_CHAR_AFT,pLevel->CharAft,TRUE);
     
    // set text
    DecodeListText(w,pLevel->text,text);
    SetDlgItemText(hDlg,IDC_TEXT,text);

    DlgInt1=pLevel->FontId;          // save it to a global variable
 
    return TRUE;
}
 
/******************************************************************************
    GetDlgListLevelPtr:
    Get the pointer to the currently selected list level
******************************************************************************/
struct StrListLevel far *GetDlgListLevelPtr(PTERWND w,HWND hDlg)
{
    struct StrListLevel far *pLevel=NULL;
    int CurIdx,CurList,CurListOr,CurLevel,LevelCount;

    if (SendDlgItemMessage(hDlg,IDC_LIST_ITEM,BM_GETCHECK, 0, 0L)) {
       CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_GETCURSEL, 0,0L);
       if (CurIdx==CB_ERR) return (NULL);
       CurList=(int)SendDlgItemMessage(hDlg,IDC_LIST_BOX,CB_GETITEMDATA, CurIdx,0L);
       LevelCount=list[CurList].LevelCount;
       pLevel=list[CurList].level;
    }
    else {
       CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_OR_BOX),CB_GETCURSEL, 0,0L);
       if (CurIdx==CB_ERR) return (NULL);
       CurListOr=(int)SendDlgItemMessage(hDlg,IDC_LIST_OR_BOX,CB_GETITEMDATA, CurIdx,0L);
       LevelCount=ListOr[CurListOr].LevelCount;
       pLevel=ListOr[CurListOr].level;
    } 
    if (!pLevel) return NULL;

    // get the current level
    CurIdx=(int)SendMessage(GetDlgItem(hDlg,IDC_LEVEL),CB_GETCURSEL, 0,0L);
    if (CurIdx==CB_ERR) return (NULL);
    CurLevel=(int)SendDlgItemMessage(hDlg,IDC_LEVEL,CB_GETITEMDATA, CurIdx,0L);

    if (CurLevel>=LevelCount) return NULL;

    return &(pLevel[CurLevel]);
}
 
/******************************************************************************
    DlgListNumType:
    Get or Set list number type for the ListLevelParam dialog box
******************************************************************************/
DlgListNumType(PTERWND w,HWND hDlg, int CtlId, int val, BOOL set)
{
    int    i,idx,CurIdx,LabelVal[20],count;
    LPBYTE pLabel[20];

    i=0;
    LabelVal[i]=LIST_DEC;pLabel[i]="Decimal";i++;
    LabelVal[i]=LIST_UPR_ROMAN;pLabel[i]="Uppercase Roman letters";i++;
    LabelVal[i]=LIST_LWR_ROMAN;pLabel[i]="Lowercase Roman letters";i++;
    LabelVal[i]=LIST_UPR_ALPHA;pLabel[i]="Uppercase alphabets";i++;
    LabelVal[i]=LIST_LWR_ALPHA;pLabel[i]="Lowercase alphabets";i++;
    LabelVal[i]=LIST_CARDINAL; pLabel[i]="Cardinal numbering";i++;
    LabelVal[i]=LIST_ORDINAL_TEXT; pLabel[i]="Ordinal text numbering";i++;
    LabelVal[i]=LIST_LWR_ALPHA;pLabel[i]="Lowercase alphabets";i++;
    LabelVal[i]=LIST_DEC_PAD;  pLabel[i]="Decimal padded";i++;
    LabelVal[i]=LIST_BLT;      pLabel[i]="Bullet";i++;
    LabelVal[i]=LIST_NO_NUM;   pLabel[i]="Hidden";i++;
    count=i;

    if (set) {
       SendMessage(GetDlgItem(hDlg,CtlId),CB_RESETCONTENT, 0, 0L);
       CurIdx=-1;
       for (i=0;i<count;i++) {
          idx=(int)SendDlgItemMessage(hDlg,CtlId,CB_ADDSTRING, 0, (DWORD)(LPBYTE)pLabel[i]);
          SendMessage(GetDlgItem(hDlg,CtlId),CB_SETITEMDATA, idx, LabelVal[i]);
          if (LabelVal[i]==val) SendDlgItemMessage(hDlg,CtlId,CB_SETCURSEL, idx,0L);
       }
    }
    else {
       CurIdx=(int)SendMessage(GetDlgItem(hDlg,CtlId),CB_GETCURSEL, 0,0L);
       if (CurIdx==CB_ERR) return 0;
       val=(int)SendDlgItemMessage(hDlg,CtlId,CB_GETITEMDATA, CurIdx,0L);
    }     
 
    return val;
}
 
/******************************************************************************
    DlgListCharAft:
    Get or Set 'character after' for the ListLevelParam dialog box
******************************************************************************/
DlgListCharAft(PTERWND w,HWND hDlg, int CtlId, int val, BOOL set)
{
    int    i,idx,CurIdx,LabelVal[20],count;
    LPBYTE pLabel[20];

    i=0;
    LabelVal[i]=LISTAFT_TAB;pLabel[i]="Tab";i++;
    LabelVal[i]=LISTAFT_SPACE;pLabel[i]="Space";i++;
    LabelVal[i]=LISTAFT_NONE;pLabel[i]="None";i++;
    count=i;

    if (set) {
       SendMessage(GetDlgItem(hDlg,CtlId),CB_RESETCONTENT, 0, 0L);
       CurIdx=-1;
       for (i=0;i<count;i++) {
          idx=(int)SendDlgItemMessage(hDlg,CtlId,CB_ADDSTRING, 0, (DWORD)(LPBYTE)pLabel[i]);
          SendMessage(GetDlgItem(hDlg,CtlId),CB_SETITEMDATA, idx, LabelVal[i]);
          if (LabelVal[i]==val) SendDlgItemMessage(hDlg,CtlId,CB_SETCURSEL, idx,0L);
       }
    }
    else {
       CurIdx=(int)SendMessage(GetDlgItem(hDlg,CtlId),CB_GETCURSEL, 0,0L);
       if (CurIdx==CB_ERR) return 0;
       val=(int)SendDlgItemMessage(hDlg,CtlId,CB_GETITEMDATA, CurIdx,0L);
    }     
 
    return val;
}
 
/******************************************************************************
    SetTabParam:
    Accepts a tab position to create
******************************************************************************/
BOOL CALLBACK _export SetTabParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int TabId,ParaId,i,TabPosInt;
    BYTE string[20];
    double TabPos;
    BOOL metric;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            metric=(BOOL)(TerFlags&TFLAG_METRIC);
            if (metric) SetDlgItemText(hDlg,IDC_UNITS,MsgString[MSG_TAB_POS]);

            // dialog field initialization
            if (EditingParaStyle) TabId=StyleId[CurSID].TabId;
            else {
               ParaId=pfmt(CurLine);
               TabId=PfmtId[ParaId].TabId;
            }

            // fill the tab position list box
            for (i=0;i<TerTab[TabId].count;i++) {
               TabPos=((double)(TerTab[TabId].pos[i]))/1440; // tab position in inches
               if (metric) TabPos=InchesToCm(TabPos);
               DoubleToStr(w,TabPos,2,string);  // convert to string
               SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_ADDSTRING, 0, (DWORD)(LPBYTE)string);
            }

            // set rtl label
            if (IsLineRtl(w,CurLine)) {   // reverse
               SetDlgItemText(hDlg,IDC_TAB_TYPE_LEFT,"Right");
               SetDlgItemText(hDlg,IDC_TAB_TYPE_RIGHT,"Left");
            }
            
            SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_SETCURSEL, 0,0L);

            // Set the tab type and leaders
            DlgSetTabAttrib(w,hDlg,TabId,0);        // set the tab type and flags

            DlgInt1=TabId;                           // save the tab id

            SetFocus(GetDlgItem(hDlg,IDC_TAB_POS));

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                   if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                       int TabNo=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),CB_GETCURSEL, 0,0L);
                       if (TabNo==CB_ERR) return (TRUE);
                       DlgSetTabAttrib(w,hDlg,DlgInt1,TabNo);         // set the tab type and flags
                   }
                   break;

                case IDOK:
                   metric=(BOOL)(TerFlags&TFLAG_METRIC);

                   GetDlgItemText(hDlg,IDC_LIST_BOX,TempString,sizeof(TempString)-1);
                   TabPos=TerAtof(w,TempString);
                   if (metric) TabPos=CmToInches(TabPos);
                   TabPosInt=(int)InchesToTwips(TabPos);   // convert to twips

                   if (TabPosInt<=0) {
                      MessageBeep(0);
                      SetFocus(GetDlgItem(hDlg,IDC_TAB_POS));
                      return TRUE;
                   }

                   // get tab type and leader
                   DlgInt2=TAB_LEFT;
                   if      ((BOOL)SendDlgItemMessage(hDlg,IDC_TAB_TYPE_RIGHT,BM_GETCHECK, 0, 0L)) DlgInt2=TAB_RIGHT;
                   else if ((BOOL)SendDlgItemMessage(hDlg,IDC_TAB_TYPE_CENTER,BM_GETCHECK, 0, 0L)) DlgInt2=TAB_CENTER;
                   else if ((BOOL)SendDlgItemMessage(hDlg,IDC_TAB_TYPE_DECIMAL,BM_GETCHECK, 0, 0L)) DlgInt2=TAB_DECIMAL;

                   DlgInt3=0;
                   if      ((BOOL)SendDlgItemMessage(hDlg,IDC_TAB_DOT,BM_GETCHECK, 0, 0L)) DlgInt3=TAB_DOT;
                   else if ((BOOL)SendDlgItemMessage(hDlg,IDC_TAB_HYPH,BM_GETCHECK, 0, 0L)) DlgInt3=TAB_HYPH;
                   else if ((BOOL)SendDlgItemMessage(hDlg,IDC_TAB_ULINE,BM_GETCHECK, 0, 0L)) DlgInt3=TAB_ULINE;

                   // pass the tab position using the DlgInt1 variable
                   DlgInt1=TabPosInt;

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}


/******************************************************************************
    DlgSetTabAttrib:
    Set the tab type and tab flag
******************************************************************************/
DlgSetTabAttrib(PTERWND w, HWND hDlg, int TabId, int TabNo)
{
    int type;
    BYTE flags;

    if (TabNo<0 || TabNo>=TerTab[TabId].count) {  // new tab position
       SendDlgItemMessage(hDlg,IDC_TAB_TYPE_LEFT,BM_SETCHECK, TRUE, 0L);
       SendDlgItemMessage(hDlg,IDC_TAB_NONE,BM_SETCHECK, TRUE, 0L);
       return TRUE;
    }

    // get the type and flag
    type=TerTab[TabId].type[TabNo];
    flags=TerTab[TabId].flags[TabNo];

    // set the type
    SendDlgItemMessage(hDlg,IDC_TAB_TYPE_LEFT,BM_SETCHECK, type==TAB_LEFT, 0L);
    SendDlgItemMessage(hDlg,IDC_TAB_TYPE_RIGHT,BM_SETCHECK, type==TAB_RIGHT, 0L);
    SendDlgItemMessage(hDlg,IDC_TAB_TYPE_CENTER,BM_SETCHECK, type==TAB_CENTER, 0L);
    SendDlgItemMessage(hDlg,IDC_TAB_TYPE_DECIMAL,BM_SETCHECK, type==TAB_DECIMAL, 0L);

    // set the dot leader
    SendDlgItemMessage(hDlg,IDC_TAB_NONE,BM_SETCHECK, flags==0, 0L);
    SendDlgItemMessage(hDlg,IDC_TAB_DOT,BM_SETCHECK, flags==TAB_DOT, 0L);
    SendDlgItemMessage(hDlg,IDC_TAB_HYPH,BM_SETCHECK, flags==TAB_HYPH, 0L);
    SendDlgItemMessage(hDlg,IDC_TAB_ULINE,BM_SETCHECK, flags==TAB_ULINE, 0L);

    return TRUE;
}

/******************************************************************************
    ClearTabParam:
    Accepts a tab position to clear
******************************************************************************/
BOOL CALLBACK _export ClearTabParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    int i,TabId;
    BYTE string[20];
    PTERWND w;
    struct StrDlgParam far *dlg;
    BOOL metric;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);
            TabId=(int)(DWORD)(dlg->data);          // initialization data
            metric=(BOOL)(TerFlags&TFLAG_METRIC);

            // add tab positions to the list box
            for (i=0;i<TerTab[TabId].count;i++) {    // add tab positions to the list box
               if (metric) DoubleToStr(w,InchesToCm(((double)(TerTab[TabId].pos[i]))/1440),2,string);  // convert twips to inches
               else        DoubleToStr(w,((double)(TerTab[TabId].pos[i]))/1440,2,string);  // convert twips to inches
               SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),LB_ADDSTRING, 0, (DWORD)(LPBYTE)string);
            }
            SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),LB_SETCURSEL, 0,0L);
            SetFocus(GetDlgItem(hDlg,IDC_LIST_BOX));

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_LIST_BOX:
                    if (NOTIFY_MSG(wParam,lParam)!=LBN_DBLCLK) break;

                case IDOK:
                   i=(int)SendMessage(GetDlgItem(hDlg,IDC_LIST_BOX),LB_GETCURSEL, 0,0L);
                   EndDialog(hDlg, i);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, -1);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    PageParam:
    Accepts the print options for a document mode printing.
******************************************************************************/
BOOL CALLBACK _export PageParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    int sect,i;
    float LeftMargin,RightMargin,TopMargin,BotMargin,HdrMargin,FtrMargin;
    BOOL metric,all;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            metric=(BOOL)(TerFlags&TFLAG_METRIC);

            // dialog field initialization
            sect=GetSection(w,CurLine);
            if (metric) {
               SetDlgItemText(hDlg,IDC_UNITS,MsgString[MSG_MARGIN]);
               SetDlgItemText(hDlg,IDC_LEFT_MARGIN,DoubleToStr(w,(double)InchesToCm(TerSect[sect].LeftMargin),2,TempString));
               SetDlgItemText(hDlg,IDC_RIGHT_MARGIN,DoubleToStr(w,(double)InchesToCm(TerSect[sect].RightMargin),2,TempString));
               SetDlgItemText(hDlg,IDC_TOP_MARGIN,DoubleToStr(w,(double)InchesToCm(TerSect[sect].TopMargin),2,TempString));
               SetDlgItemText(hDlg,IDC_BOT_MARGIN,DoubleToStr(w,(double)InchesToCm(TerSect[sect].BotMargin),2,TempString));
               SetDlgItemText(hDlg,IDC_HDR_MARGIN,DoubleToStr(w,(double)InchesToCm(TerSect[sect].HdrMargin),2,TempString));
               SetDlgItemText(hDlg,IDC_FTR_MARGIN,DoubleToStr(w,(double)InchesToCm(TerSect[sect].FtrMargin),2,TempString));
            }
            else {
               SetDlgItemText(hDlg,IDC_LEFT_MARGIN,DoubleToStr(w,(double)TerSect[sect].LeftMargin,2,TempString));
               SetDlgItemText(hDlg,IDC_RIGHT_MARGIN,DoubleToStr(w,(double)TerSect[sect].RightMargin,2,TempString));
               SetDlgItemText(hDlg,IDC_TOP_MARGIN,DoubleToStr(w,(double)TerSect[sect].TopMargin,2,TempString));
               SetDlgItemText(hDlg,IDC_BOT_MARGIN,DoubleToStr(w,(double)TerSect[sect].BotMargin,2,TempString));
               SetDlgItemText(hDlg,IDC_HDR_MARGIN,DoubleToStr(w,(double)TerSect[sect].HdrMargin,2,TempString));
               SetDlgItemText(hDlg,IDC_FTR_MARGIN,DoubleToStr(w,(double)TerSect[sect].FtrMargin,2,TempString));
            }
            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDOK:
                   metric=(BOOL)(TerFlags&TFLAG_METRIC);
                   all=(BOOL)SendDlgItemMessage(hDlg,IDC_ALL_SECTS,BM_GETCHECK, 0, 0L);

                   // validate that the user entered the values with range
                   if (!CheckDlgValue(hDlg,'F',IDC_LEFT_MARGIN,0,9)) return (TRUE);
                   if (!CheckDlgValue(hDlg,'F',IDC_RIGHT_MARGIN,0,9)) return (TRUE);
                   if (!CheckDlgValue(hDlg,'F',IDC_TOP_MARGIN,0,9)) return (TRUE);
                   if (!CheckDlgValue(hDlg,'F',IDC_BOT_MARGIN,0,9)) return (TRUE);
                   if (!CheckDlgValue(hDlg,'F',IDC_HDR_MARGIN,0,9)) return (TRUE);
                   if (!CheckDlgValue(hDlg,'F',IDC_FTR_MARGIN,0,9)) return (TRUE);

                   // extract the margin and validate
                   GetDlgItemText(hDlg,IDC_LEFT_MARGIN,TempString,100);
                   LeftMargin=(float)TerAtof(w,TempString);
                   if (metric) LeftMargin=CmToInches(LeftMargin);

                   GetDlgItemText(hDlg,IDC_RIGHT_MARGIN,TempString,100);
                   RightMargin=(float)TerAtof(w,TempString);
                   if (metric) RightMargin=CmToInches(RightMargin);

                   GetDlgItemText(hDlg,IDC_TOP_MARGIN,TempString,100);
                   TopMargin=(float)TerAtof(w,TempString);
                   if (metric) TopMargin=CmToInches(TopMargin);

                   GetDlgItemText(hDlg,IDC_BOT_MARGIN,TempString,100);
                   BotMargin=(float)TerAtof(w,TempString);
                   if (metric) BotMargin=CmToInches(BotMargin);

                   GetDlgItemText(hDlg,IDC_HDR_MARGIN,TempString,100);
                   HdrMargin=(float)TerAtof(w,TempString);
                   if (metric) HdrMargin=CmToInches(HdrMargin);

                   GetDlgItemText(hDlg,IDC_FTR_MARGIN,TempString,100);
                   FtrMargin=(float)TerAtof(w,TempString);
                   if (metric) FtrMargin=CmToInches(FtrMargin);

                   OpenCurPrinter(w,FALSE);       // open printer if not yet opened
                   if ( (PageWidth<=(LeftMargin+RightMargin+(float)MIN_TEXT_WIDTH))
                      ||(PageHeight<=(TopMargin+BotMargin+(float)MIN_TEXT_HEIGHT))
                      /*||HdrMargin>TopMargin || FtrMargin>BotMargin */){
                      MessageBox(hDlg,MsgString[MSG_MARGIN_TOO_LARGE],NULL,MB_OK);
                      break;
                   }

                   // store values
                   sect=GetSection(w,CurLine);

                   for (i=0;i<TotalSects;i++) {
                      if (!all && i!=sect) continue;
                      TerSect[i].LeftMargin=LeftMargin;
                      TerSect[i].RightMargin=RightMargin;
                      TerSect[i].TopMargin=TopMargin;
                      TerSect[i].BotMargin=BotMargin;
                      TerSect[i].HdrMargin=HdrMargin;
                      TerSect[i].FtrMargin=FtrMargin;

                      if (TerSect[i].FirstLine<RepageBeginLine) RepageBeginLine=TerSect[sect].FirstLine;
                   }


                   TerArg.modified++;

                   EndDialog(hDlg, TRUE);
                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg, FALSE);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    RangeParam:
    Accepts print range
******************************************************************************/
int CALLBACK _export RangeParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    struct StrDlgParam far *dlg;
    BOOL SelectPages;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            // initialize data pointer
            dlg=(struct StrDlgParam far *)lParam;    // initialization data
            w=dlg->w;
            SetWindowLong(hDlg,DWL_USER,(LONG)w);    // store parent window control data
            PosDialog(w,hDlg);

            // dialog field initialization
            SendMessage(GetDlgItem(hDlg,IDC_RANGE_ALL),BM_SETCHECK, TRUE, 0L);

            if (HilightType==HILIGHT_OFF) EnableWindow(GetDlgItem(hDlg,IDC_RANGE_SELECTED),FALSE);

            // set the pages
            SetDlgItemInt(hDlg,IDC_FIRST_PAGE,1,0);
            if (TerArg.PageMode && !TerArg.FittedView) 
                 SetDlgItemInt(hDlg,IDC_LAST_PAGE,TotalPages,0);
            //else SetDlgItemInt(hDlg,IDC_LAST_PAGE,1,0);

            EnableWindow(GetDlgItem(hDlg,IDC_FIRST_PAGE),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_LAST_PAGE),FALSE);

            // set the number of copies
            SetDlgItemInt(hDlg,IDC_COPIES,1,0);
            SendMessage(GetDlgItem(hDlg,IDC_COLATE),BM_SETCHECK, TRUE, 0L);

            return (TRUE);

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_RANGE_SELECTED:
                case IDC_RANGE_ALL:
                case IDC_CUR_PAGE:
                case IDC_RANGE_PAGES:
                   SelectPages=(BOOL)(SendMessage(GetDlgItem(hDlg,IDC_RANGE_PAGES),BM_GETCHECK, 0, 0L));
                   EnableWindow(GetDlgItem(hDlg,IDC_FIRST_PAGE),SelectPages);
                   EnableWindow(GetDlgItem(hDlg,IDC_LAST_PAGE),SelectPages);
                   break;

                case IDOK:
                   // extract the copy count and coloate
                   if (!CheckDlgValue(hDlg,'I',IDC_COPIES,1,9999)) return (TRUE);
                   DlgInt3=GetDlgItemInt(hDlg,IDC_COPIES,NULL,TRUE);
                   DlgInt4=(BOOL)SendMessage(GetDlgItem(hDlg,IDC_COLATE),BM_GETCHECK, 0, 0L);

                   if (SendMessage(GetDlgItem(hDlg,IDC_RANGE_SELECTED),BM_GETCHECK, 0, 0L)) {
                      if (HilightType!=HILIGHT_CHAR && HilightType!=HILIGHT_LINE) {
                         MessageBox(hDlg,MsgString[MSG_NO_HILIGHT],MsgString[MSG_INVALID_SELECT],MB_OK);
                         SetFocus(GetDlgItem(hDlg,IDC_RANGE_SELECTED));
                      }
                      else EndDialog(hDlg,IDC_RANGE_SELECTED);
                   }
                   else if (SendMessage(GetDlgItem(hDlg,IDC_RANGE_PAGES),BM_GETCHECK, 0, 0L)) {
                      if (!CheckDlgValue(hDlg,'I',IDC_FIRST_PAGE,1,9999)) return (TRUE);
                      if (!CheckDlgValue(hDlg,'I',IDC_LAST_PAGE,1,9999)) return (TRUE);

                      // extract the values
                      DlgInt1=GetDlgItemInt(hDlg,IDC_FIRST_PAGE,NULL,TRUE);
                      DlgInt2=GetDlgItemInt(hDlg,IDC_LAST_PAGE,NULL,TRUE);
                      if (DlgInt1<0 || DlgInt2<DlgInt1) return (TRUE);

                      EndDialog(hDlg,IDC_RANGE_PAGES);
                   }
                   else if (SendMessage(GetDlgItem(hDlg,IDC_CUR_PAGE),BM_GETCHECK, 0, 0L)) EndDialog(hDlg,IDC_CUR_PAGE);    // entire file
                   else EndDialog(hDlg,IDC_RANGE_ALL);    // entire file

                   return (TRUE);

                case IDCANCEL:
                   EndDialog(hDlg,0);
                   return (TRUE);
                default:
                   ;
            }
            break;
    }
    return (FALSE);
}

/******************************************************************************
    PrintAbortParam:
    Handles the user request to abort printing.
******************************************************************************/
BOOL CALLBACK _export PrintAbortParam(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;

    switch (message) {
        case WM_INITDIALOG:                  // initialize parameters
            SetFocus(GetDlgItem(hDlg,IDCANCEL));
            return (TRUE);

        case WM_ACTIVATE:                    // set the current abort window
            if (wParam) hCurAbortWnd=hDlg;   // set common global variable
            else        hCurAbortWnd=0;
            break;

       #if !defined (WIN32)
       case WM_CTLCOLOR:
            return SetDlgColor(wParam,lParam);
       #endif

        case WM_COMMAND:
            w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER);
            return (AbortPrint=TRUE);
    }
    return (FALSE);
}

/******************************************************************************
    PrintPreviewProc:
    Process messages from the Print Preview Tool Bar dialog box
******************************************************************************/
BOOL CALLBACK _export PrintPreviewProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    int  NewPreviewZoom;
    int  i,len,idx;
    int  zoom[MAX_ZOOMS]={0,200,150,100,75,50,25,10};
    RECT rect;

    switch (message) {
       case WM_INITDIALOG:                  // initialize parameters
            w=(PTERWND) lParam;
            for (i=0;i<MAX_ZOOMS;i++) {
               if (zoom[i]<=0) lstrcpy(TempString,MsgString[MSG_PP_FITTED]);
               else            wsprintf(TempString,"%d%%",zoom[i]);
               idx=(int)SendMessage(GetDlgItem(hDlg,IDC_PV_ZOOM),CB_ADDSTRING, 0, (DWORD)(LPSTR)TempString);
               SendMessage(GetDlgItem(hDlg,IDC_PV_ZOOM),CB_SETITEMDATA, idx, zoom[i]);
            }

            // Set the zoom edit box
            if (PreviewZoom<=0) SetDlgItemText(hDlg,IDC_PV_ZOOM,MsgString[MSG_PP_FITTED]);
            else                SetDlgItemInt(hDlg,IDC_PV_ZOOM,PreviewZoom,0);

            // Set the number of pages
            if (TotalPreviewPages==1) SetDlgItemText(hDlg,IDC_PV_PAGES,MsgString[MSG_PP_TWO_PAGES]);
            else                      SetDlgItemText(hDlg,IDC_PV_PAGES,MsgString[MSG_PP_ONE_PAGE]);

            if (!TerArg.ReadOnly) SetDlgItemText(hDlg,IDC_PV_CLOSE,MsgString[MSG_PP_EDIT]);

            SetDlgItemText(hDlg,IDC_PV_PRINT,MsgString[MSG_PP_PRINT]);

            GetWindowRect(hDlg,&rect);
            SetWindowPos(hDlg,NULL,TerRect.left,0,TerRect.right,rect.bottom-rect.top,SWP_NOZORDER);

            SetFocus(hTerWnd);

            return (FALSE);

       #if defined (WIN32)
       case WM_CTLCOLORDLG:
            if (NULL==(w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER))) return FALSE;
            return (BOOL) (hToolbarBrush?hToolbarBrush:GetStockObject(LTGRAY_BRUSH));
       case WM_CTLCOLORSTATIC:
            if (NULL==(w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER))) return FALSE;
            return (BOOL) (hToolbarBrush?hToolbarBrush:GetStockObject(LTGRAY_BRUSH));
       #else
       case WM_CTLCOLOR:
            if (NULL==(w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER))) return FALSE;
            if (HIWORD(lParam)==CTLCOLOR_DLG)    return (BOOL) (hToolbarBrush?hToolbarBrush:GetStockObject(LTGRAY_BRUSH));
            if (HIWORD(lParam)==CTLCOLOR_STATIC) return (BOOL) (hToolbarBrush?hToolbarBrush:GetStockObject(LTGRAY_BRUSH));
            break;
       #endif

       case WM_COMMAND:
            if (NULL==(w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER))) return FALSE;

            switch(CONTROL_ID(wParam,lParam)) {
                case IDC_PV_ZOOM:
                   TempString[0]=0;
                   if (NOTIFY_MSG(wParam,lParam)==CBN_SELCHANGE) {
                       idx=(int)SendMessage(GetDlgItem(hDlg,IDC_PV_ZOOM),CB_GETCURSEL, 0,0L);
                       if (idx==CB_ERR) return (TRUE);
                       SendMessage(GetDlgItem(hDlg,IDC_PV_ZOOM),CB_GETLBTEXT, (WPARAM)  idx,(DWORD)(LPBYTE)TempString);
                   }
                   else if (NOTIFY_MSG(wParam,lParam)==CBN_DBLCLK || HIWORD(lParam)==CBN_DBLCLK) { // subclass special message
                       GetDlgItemText(hDlg,IDC_PV_ZOOM,TempString,sizeof(TempString)-1);
                   }
                   if (lstrlen(TempString)>0) {
                       if (_strcmpi(TempString,MsgString[MSG_PP_FITTED])==0) NewPreviewZoom=0;
                       else {
                          len=lstrlen(TempString);
                          if (len>0 && TempString[len-1]=='%') len--;
                          TempString[len]=0;
                          NewPreviewZoom=atoi(TempString);
                       }
                       SetFocus(hTerWnd);
                   }
                   else NewPreviewZoom=PreviewZoom;

                   // apply new font if changed
                   if (NewPreviewZoom!=PreviewZoom) {
                      PreviewZoom=NewPreviewZoom;
                      rect=TerRect;
                      rect.top=TerWinRect.top;
                      TerWinOrgX=0;
                      TerWinOrgY=0;
                      SetFocus(hTerWnd);
                      TerInvalidateRect(w,&rect,TRUE);   // force WM_PAINT with erase background
                      return TRUE;
                   }
                   break;

                case IDC_PV_PAGES:
                   if (TotalPreviewPages==1) {
                      TotalPreviewPages=2;
                      SetDlgItemText(hDlg,IDC_PV_PAGES,MsgString[MSG_PP_ONE_PAGE]);
                   }
                   else {
                      TotalPreviewPages=1;
                      SetDlgItemText(hDlg,IDC_PV_PAGES,MsgString[MSG_PP_TWO_PAGES]);
                   }
                   rect=TerRect;
                   rect.top=TerWinRect.top;
                   SetFocus(hTerWnd);
                   TerInvalidateRect(w,&rect,TRUE);   // force WM_PAINT with erase background
                   SetFocus(hTerWnd);

                   break;

                case IDC_PV_PRINT:         // invoke printing
                   GoPrintPreview=TRUE;    // go back to print preview after printing
                   PostMessage(hTerWnd,WM_COMMAND,ID_PRINT_PREVIEW,0L);  // terminate print preview
                   PostMessage(hTerWnd,WM_COMMAND,ID_PRINT,0L);  // terminate print preview
                   break;

                case IDC_PV_CLOSE:
                   PostMessage(hTerWnd,WM_COMMAND,ID_PRINT_PREVIEW,0L);  // terminate print preview
                   break;

                default:
                   break;
            }
            break;

    }
    return (FALSE);
}

/****************************************************************************
    SetGuiFont:
    Set the gui font for a text input box.
****************************************************************************/
SetGuiFont(PTERWND w, HWND hDlg,int id)
{
    HFONT hFont;

    if (!mbcs) return TRUE;

    #if defined(WIN32)
      hFont=GetStockObject(DEFAULT_GUI_FONT);
    #else
      hFont=GetStockObject(SYSTEM_FONT);
    #endif

    if (hFont) {
       SendMessage(hDlg,WM_SETFONT,(WPARAM)hFont,0L);
       SendDlgItemMessage(hDlg,id,WM_SETFONT,(WPARAM)hFont,0L);
    }
    return TRUE;
}

/****************************************************************************
    DlgEditFont:
    This dialog box allows the user to select the font for a dislog element.
    The typeface is passed by TempString, TwipsSize by DlgInt3, style by DlgInt4,
    and text color by DlgLong.
****************************************************************************/
DlgEditFont(PTERWND w, HWND hDlg)
{
    LOGFONT lFont;
    CHOOSEFONT cFont;

    FarMemSet(&lFont,0,sizeof(lFont));
    lFont.lfHeight=-TwipsToScrY(DlgInt3);
    lFont.lfWidth=lFont.lfEscapement=lFont.lfOrientation=0;
    lFont.lfWeight=DlgInt4&BOLD   ? FW_BOLD  :  FW_REGULAR;
    lFont.lfItalic=(BYTE)(DlgInt4&ITALIC ? TRUE : FALSE);
    lFont.lfUnderline=(BYTE)(DlgInt4&ULINE  ? TRUE : FALSE);
    lFont.lfStrikeOut=(BYTE)(DlgInt4&STRIKE ? TRUE : FALSE);
    lFont.lfOutPrecision=(BYTE)(OUT_DEFAULT_PRECIS);
    lFont.lfClipPrecision= CLIP_DEFAULT_PRECIS;
    lFont.lfQuality=DRAFT_QUALITY;
    lstrcpy(lFont.lfFaceName,TempString);


    // fill the CHOOSEFONT structure
    FarMemSet(&cFont,0,sizeof(CHOOSEFONT));
    cFont.lStructSize=sizeof(CHOOSEFONT);
    cFont.hwndOwner=hDlg;
    cFont.hDC=hTerDC;
    cFont.lpLogFont=&lFont;
    cFont.Flags=CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT;
    cFont.rgbColors=(COLORREF)DlgLong;


    if (!ChooseFont(&cFont)) return FALSE;

    // Extract the font dialog values
    lstrcpy(TempString,lFont.lfFaceName);
    DlgInt3=cFont.iPointSize*2;   // convert PointSize/10 to TwipsSize
    DlgInt4=0;
    if (lFont.lfWeight==FW_BOLD) DlgInt4|=BOLD;
    if (lFont.lfItalic)    DlgInt4|=ITALIC;
    if (lFont.lfUnderline) DlgInt4|=ULINE;
    if (lFont.lfStrikeOut) DlgInt4|=STRIKE;

    DlgLong=cFont.rgbColors;

    return TRUE;
}

/****************************************************************************
    DlgEditListFont:
    This function lets user select another font for the list level.  This function
    returns the final font id.
****************************************************************************/
DlgEditListFont(PTERWND w, HWND hDlg, int FontId)
{
    LOGFONT lFont;
    CHOOSEFONT cFont;
    UINT style=TerFont[FontId].style;
    int  TwipsSize;

    FarMemSet(&lFont,0,sizeof(lFont));
    lFont.lfHeight=-TwipsToScrY(TerFont[FontId].TwipsSize);
    lFont.lfWidth=lFont.lfEscapement=lFont.lfOrientation=0;
    lFont.lfWeight=style&BOLD   ? FW_BOLD  :  FW_REGULAR;
    lFont.lfItalic=(BYTE)(style&ITALIC ? TRUE : FALSE);
    lFont.lfUnderline=(BYTE)(style&ULINE  ? TRUE : FALSE);
    lFont.lfStrikeOut=(BYTE)(style&STRIKE ? TRUE : FALSE);
    lFont.lfOutPrecision=(BYTE)(OUT_DEFAULT_PRECIS);
    lFont.lfClipPrecision= CLIP_DEFAULT_PRECIS;
    lFont.lfQuality=DRAFT_QUALITY;
    lstrcpy(lFont.lfFaceName,TerFont[FontId].TypeFace);


    // fill the CHOOSEFONT structure
    FarMemSet(&cFont,0,sizeof(CHOOSEFONT));
    cFont.lStructSize=sizeof(CHOOSEFONT);
    cFont.hwndOwner=hDlg;
    cFont.hDC=hTerDC;
    cFont.lpLogFont=&lFont;
    cFont.Flags=CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT;
    cFont.rgbColors=(COLORREF)TerFont[FontId].TextColor;


    if (!ChooseFont(&cFont)) return FontId;   // return the old font id

    // Extract the font dialog values
    TwipsSize=cFont.iPointSize*2;   // convert PointSize/10 to TwipsSize
    style=0;
    if (lFont.lfWeight==FW_BOLD) style|=BOLD;
    if (lFont.lfItalic)    style|=ITALIC;
    if (lFont.lfUnderline) style|=ULINE;
    if (lFont.lfStrikeOut) style|=STRIKE;

    FontId=GetNewFont(w,hTerDC,FontId,lFont.lfFaceName,TwipsSize,style,
           cFont.rgbColors,TerFont[FontId].TextBkColor,TerFont[FontId].UlineColor,
           TerFont[FontId].FieldId, 
           TerFont[FontId].AuxId,TerFont[FontId].Aux1Id, 
           TerFont[FontId].CharStyId,TerFont[FontId].ParaStyId, 
           TerFont[FontId].expand,TerFont[FontId].TempStyle, 
           TerFont[FontId].lang,TerFont[FontId].FieldCode, 
           TerFont[FontId].offset,TerFont[FontId].UcBase, 
           TerFont[FontId].CharSet,TerFont[FontId].flags,
           TerFont[FontId].TextAngle);


    return FontId;
}

/****************************************************************************
    DlgEditColor:
    This dialog box allows the user to select the new color.  The third
    option when TRUE allows for customized colors.
****************************************************************************/
COLORREF DlgEditColor(PTERWND w, HWND hDlg,COLORREF InColor,BOOL FullOpen)
{
    CHOOSECOLOR cColor;
    DWORD CustColors[16];
    BYTE  clr;
    int i;
    float increment;

    DlgCancel=TRUE;

    // fill the CustColors array with 16 shades of gray
    increment=(float)255/(float)16;
    for (i=0;i<16;i++) {
       clr=(BYTE)((i+1)*increment);
       CustColors[i]=RGB(clr,clr,clr);
    }

    // fill the CHOOSECOLOR structure
    FarMemSet(&cColor,0,sizeof(CHOOSECOLOR));
    cColor.lStructSize=sizeof(CHOOSECOLOR);
    cColor.hwndOwner=hDlg;
    cColor.rgbResult=InColor;
    cColor.lpCustColors=CustColors;
    if (FullOpen) cColor.Flags=CC_FULLOPEN|CC_RGBINIT;
    else          cColor.Flags=CC_PREVENTFULLOPEN|CC_RGBINIT;

    if (ChooseColor(&cColor)) {
       DlgCancel=FALSE;         // not cancelled
       return cColor.rgbResult;
    }
    else return InColor;

}


/****************************************************************************
    ZoomSubclass:
    This routine subclass the preview zoom input. It supplies the
    carriage return key.
****************************************************************************/
BOOL CALLBACK _export ZoomSubclass(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    HWND hDlg,hCombo;
    PTERWND w;

    // get the parent window handle and our context
    if ((hCombo=GetParent(hWnd))==NULL) return FALSE;   // handle of the combo control
    if ((hDlg=GetParent(hCombo))==NULL) return FALSE;   // handle of the dialog box
    if (NULL==(w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER))) return FALSE;

    // Notify the parent (dialog) window
    if (message==WM_CHAR && wParam==VK_RETURN) {
        PrintPreviewProc(hDlg,WM_COMMAND,IDC_PV_ZOOM,((DWORD)CBN_DBLCLK)<<16);
        return TRUE;
    }
    else return (BOOL) CallWindowProc(OrigZoomSubclassProc,hWnd,message,wParam,lParam);
}

/****************************************************************************
    PosDialog:
    Position the dialog box.
****************************************************************************/
PosDialog(PTERWND w, HWND hDlg)
{
    RECT WinRect,DlgRect;
    int  CenterX,CenterY,DlgWidth,DlgHeight,x,y;

    GetWindowRect(hDlg,&DlgRect);
    DlgWidth=DlgRect.right-DlgRect.left;
    DlgHeight=DlgRect.bottom-DlgRect.top;

    if (True(TerFlags6&TFLAG6_CENTER_DLGS_ON_SCR)) { // find the center of the screen
      CenterX=GetSystemMetrics(SM_CXSCREEN)/2;
      CenterY=GetSystemMetrics(SM_CYSCREEN)/2;
    }
    else {  // find the center of the ter window
      GetWindowRect(hTerWnd,&WinRect);
      CenterX=(WinRect.left+WinRect.right)/2;
      CenterY=(WinRect.top+WinRect.bottom)/2;
    }
        
    x=CenterX-DlgWidth/2;
    if (x<0) x=0;
    y=CenterY-DlgHeight/2;
    if (y<0) y=0;

    MoveWindow(hDlg,x,y,DlgWidth,DlgHeight,FALSE);

    return TRUE;
}


/****************************************************************************
    SetDlgColor:
    Set the dialog background color for 16 bit.
****************************************************************************/
BOOL SetDlgColor(WPARAM wParam, LPARAM lParam)
{
     int CtlType=HIWORD(lParam);

     if (CtlType==CTLCOLOR_EDIT) return TRUE;     // skip this control

     SetBkColor((HDC)wParam,GetSysColor(COLOR_BTNFACE));
     return (BOOL) GetStockObject(LTGRAY_BRUSH);
}
