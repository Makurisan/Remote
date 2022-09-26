/*==============================================================================
   TER_CTL.C
   TER Custom control handling functions

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


#if defined (WIN32)
/***************************************************************************
    WIN32 Functions
****************************************************************************/
/***************************************************************************
    Function Prototypes:
****************************************************************************/
UINT    CALLBACK _export CustomControlInfoA(LPCCINFO);
BOOL    CALLBACK _export CustomControlStyle(HWND,LPCCSTYLE);
INT     CALLBACK _export CustomControlSizeToText(DWORD,DWORD,HFONT,LPBYTE);
int     XlateCtlFlag(DWORD,LPBYTE);

BOOL    CALLBACK _export CtlStyle(void);
BOOL    CALLBACK _export CtlInfo(void);
BOOL    CALLBACK _export CtlFlags(void);

/***************************************************************************
    CustomControlInfoA:
    This routine is called by the dialog editor to get the basic information
    about the TER control.
****************************************************************************/
UINT CALLBACK _export CustomControlInfoA(LPCCINFO acci)
{
    int i;

    if (!acci) return 1;                     // return number of control supported

    // fill the values
    lstrcpy(acci[0].szClass,TER_CLASS);        // control class
    acci[0].flOptions=0;                       // CCF_* options
    lstrcpy(acci[0].szDesc,"TER Edit Control");// description
    acci[0].cxDefault=160;                     // suggested width in dialog units
    acci[0].cyDefault=64;                      // suggested height in dialog units
    acci[0].flStyleDefault=WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|TER_WORD_WRAP|TER_VSCROLL|TER_BORDER_MARGIN;  // control style
    acci[0].flExtStyleDefault=0;               // extended styles
    acci[0].flCtrlTypeMask=0;                  // Mask for the control type styles
    lstrcpy(acci[0].szTextDefault," ");        // default text, can not be ""

    // set the style table
    acci[0].cStyleFlags=TER_NUM_STYLES;        
    acci[0].aStyleFlags=CtlStyleFlag;          // style structure
    for (i=0;i<TER_NUM_STYLES;i++) CtlStyleFlag[i].flStyleMask=0;

    CtlStyleFlag[0].flStyle=TER_WORD_WRAP;
    CtlStyleFlag[0].pszStyle="TER_WORD_WRAP";
    CtlStyleFlag[1].flStyle=TER_PRINT_VIEW;
    CtlStyleFlag[1].pszStyle="TER_PRINT_VIEW";
    CtlStyleFlag[2].flStyle=TER_HSCROLL;
    CtlStyleFlag[2].pszStyle="TER_HSCROLL";
    CtlStyleFlag[3].flStyle=TER_VSCROLL;
    CtlStyleFlag[3].pszStyle="TER_VSCROLL";
    CtlStyleFlag[4].flStyle=TER_SHOW_STATUS;
    CtlStyleFlag[4].pszStyle="TER_SHOW_STATUS";
    CtlStyleFlag[5].flStyle=TER_SHOW_RULER;
    CtlStyleFlag[5].pszStyle="TER_SHOW_RULER";
    CtlStyleFlag[6].flStyle=TER_BORDER_MARGIN;
    CtlStyleFlag[6].pszStyle="TER_BORDER_MARGIN";
    CtlStyleFlag[7].flStyle=TER_OUTPUT_RTF;
    CtlStyleFlag[7].pszStyle="TER_OUTPUT_RTF";
    CtlStyleFlag[8].flStyle=TER_USE_PARAMS;
    CtlStyleFlag[8].pszStyle="TER_USE_PARAMS";
    CtlStyleFlag[9].flStyle=TER_READ_ONLY;
    CtlStyleFlag[9].pszStyle="TER_READ_ONLY";
    CtlStyleFlag[10].flStyle=TER_PAGE_MODE;
    CtlStyleFlag[10].pszStyle="TER_PAGE_MODE";
    CtlStyleFlag[11].flStyle=TER_SHOW_TOOLBAR;
    CtlStyleFlag[11].pszStyle="TER_SHOW_TOOLBAR";
    CtlStyleFlag[12].flStyle=TER_INVISIBLE;
    CtlStyleFlag[12].pszStyle="TER_INVISIBLE";
    CtlStyleFlag[13].flStyle=TER_FITTED_VIEW;
    CtlStyleFlag[13].pszStyle="TER_FITTED_VIEW";

    acci[0].lpfnStyle=CustomControlStyle;
    acci[0].lpfnSizeToText=(LPVOID) CustomControlSizeToText;

    acci[0].dwReserved1=0;
    acci[0].dwReserved2=0;

    return 1;                                // number of supported controls
}

/***************************************************************************
    CustomControlStyle:
    This routine is called by the dialog editor to allow the user to edit
    the style flags for the TER custom control. The routine returns 
    TRUE if successful.
****************************************************************************/
BOOL  CALLBACK _export CustomControlStyle(HWND hWnd,LPCCSTYLE pccs)
{
    struct StrDlgParam dlg;
    DLGPROC lpProc;

    lpProc = (DLGPROC) MakeProcInstance((FARPROC)CtlDlgFn, hTerInst);
    dlg.data=(LPBYTE)(pccs->flStyle);     // pass style data

    if (DialogBoxParam(hTerInst,"CtlDlgParam",hWnd,lpProc,(DWORD)(LPBYTE)&dlg)) pccs->flStyle=(DWORD)(dlg.data);

    FreeProcInstance((FARPROC)lpProc);   // set back the old window 

    return TRUE;
}

/***************************************************************************
    CustomControlSizeToText:
    This routine is called by the dialog editor to translate a flag into
    corresponding string.
****************************************************************************/
INT CALLBACK _export CustomControlSizeToText(DWORD flags,DWORD ExtFlags, HFONT hFont, LPBYTE FlagString)
{
    return -1;
}

/****************************************************************************
    The following 16 bit function are in the WIN32 section so that the
    linker finds them.
*****************************************************************************/
BOOL CALLBACK _export CtlStyle() {return TRUE;}
BOOL CALLBACK _export CtlInfo()  {return TRUE;}
BOOL CALLBACK _export CtlFlags() {return TRUE;}

#else

/***************************************************************************
    Windows 3.1 Functions
****************************************************************************/
/***************************************************************************
    Function Prototypes:
****************************************************************************/
BOOL    CALLBACK _export CtlStyle(HWND,HANDLE,LPFNSTRTOID,LPFNIDTOSTR);
HGLOBAL CALLBACK _export CtlInfo(void);
int     XlateCtlFlag(WORD,LPBYTE);
WORD    CALLBACK _export CtlFlags(DWORD,LPBYTE,WORD);

UINT    CALLBACK _export CustomControlInfoA(void);
BOOL    CALLBACK _export CustomControlStyle(void);
int     CALLBACK _export CustomControlSizeToText(void);

/***************************************************************************
    CtlInfo:
    This routine is called by the dialog editor to get the basic information
    about the TER control. This routine returns the handle to the CTLINFO
    structure.
****************************************************************************/
HGLOBAL CALLBACK _export CtlInfo()
{
    HGLOBAL hCtl;
    LPCTLINFO pCtl;

    if ( NULL==(hCtl=GlobalAlloc(GMEM_MOVEABLE,sizeof(CTLINFO)))
      || NULL==(pCtl=(LPCTLINFO)GlobalLock(hCtl)) ){  // allocate memory for the control information structure
        return NULL;
    }

    // fill the values
    pCtl->wVersion=0x332;                 // version 3.50
    pCtl->wCtlTypes=1;
    lstrcpy(pCtl->szClass,TER_CLASS);    // control class name
    lstrcpy(pCtl->szTitle,"TER Edit Control");

    pCtl->Type[0].wType=0;               // reserved, must be 0
    pCtl->Type[0].wWidth=160;            // suggested width in dialog units
    pCtl->Type[0].wHeight=64;            // suggested height in dialog units
    pCtl->Type[0].dwStyle=WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|TER_WORD_WRAP|TER_VSCROLL|TER_BORDER_MARGIN;  // control style
    lstrcpy(pCtl->Type[0].szDescr,"TER Edit Control");

    GlobalUnlock(hCtl);                   // unlock the structure
    return hCtl;
}

/***************************************************************************
    CtlStyle:
    This routine is called by the dialog editor to allow the user to edit
    the style flags for the TER custom control. The routine returns 
    TRUE if successful.
****************************************************************************/
BOOL  CALLBACK _export CtlStyle(HWND hWnd,HANDLE hStyle,LPFNSTRTOID fn1,LPFNIDTOSTR fn2)
{
    LPCTLSTYLE pStyle;
    struct StrDlgParam dlg;
    DLGPROC lpProc;

    if (NULL==(pStyle=(LPCTLSTYLE)GlobalLock(hStyle))) return FALSE;

    lpProc = (DLGPROC) MakeProcInstance((FARPROC)CtlDlgFn, hTerInst);
    dlg.data=(LPBYTE)(pStyle->dwStyle);   // pass style data

    if (DialogBoxParam(hTerInst,"CtlDlgParam",hWnd,lpProc,(DWORD)(LPBYTE)&dlg)) pStyle->dwStyle=(DWORD)(dlg.data);

    FreeProcInstance((FARPROC)lpProc);   // set back the old window 

    GlobalUnlock(hStyle);

    return TRUE;
}

/***************************************************************************
    CtlFlags:
    This routine is called by the dialog editor to translate a flag into
    corresponding string.
****************************************************************************/
WORD CALLBACK _export CtlFlags(DWORD flags,LPBYTE FlagString,WORD MaxLen)
{
    WORD len,flag,i,CurLen;
    BYTE string[30];
    
    FlagString[0]=0;
    CurLen=0;

    for (i=0;i<16;i++) {  // process each of lower 16 bits
       flag=1<<i;
       flag=((WORD)flags)&flag;
       len=XlateCtlFlag(flag,string);
       if (len>0 && len+CurLen+3<MaxLen-1) {     // add length of ' | '
          if (FlagString[0]!=0) lstrcat(FlagString," | ");
          lstrcat(FlagString,string);
          CurLen=lstrlen(FlagString);
       }
    }

    return CurLen;
}

/***************************************************************************
    XlateCtlFlag:
    This routine converts a numeric style id to string format.  This routine
    returns the length of the formated string.
****************************************************************************/
XlateCtlFlag(WORD flag,LPBYTE string)
{
    string[0]=0;
    if      (flag==TER_WORD_WRAP)    lstrcpy(string,"TER_WORD_WRAP");
    else if (flag==TER_PRINT_VIEW)   lstrcpy(string,"TER_PRINT_VIEW");
    else if (flag==TER_PAGE_MODE)    lstrcpy(string,"TER_PAGE_MODE");
    else if (flag==TER_FITTED_VIEW)  lstrcpy(string,"TER_FITTED_VIEW");
    else if (flag==TER_HSCROLL)      lstrcpy(string,"TER_HSCROLL");
    else if (flag==TER_VSCROLL)      lstrcpy(string,"TER_VSCROLL");
    else if (flag==TER_SHOW_STATUS)  lstrcpy(string,"TER_SHOW_STATUS");
    else if (flag==TER_SHOW_RULER)   lstrcpy(string,"TER_SHOW_RULER");
    else if (flag==TER_SHOW_TOOLBAR) lstrcpy(string,"TER_SHOW_TOOLBAR");
    else if (flag==TER_BORDER_MARGIN)lstrcpy(string,"TER_BORDER_MARGIN");
    else if (flag==TER_OUTPUT_RTF)   lstrcpy(string,"TER_OUTPUT_RTF");
    else if (flag==TER_READ_ONLY)    lstrcpy(string,"TER_READ_ONLY");


    return lstrlen(string);
}

/***************************************************************************
    CustomControlInfoA:
    This is actually a 32 bit version code.  It is provided in the 16 bit
    section for the benefit of the linker only.
****************************************************************************/
UINT CALLBACK _export CustomControlInfoA()      {return TRUE;}
BOOL CALLBACK _export CustomControlStyle()      {return TRUE;}
int  CALLBACK _export CustomControlSizeToText() {return TRUE;}

#endif
