/*==============================================================================
   TER_MISC.C
   TER miscellaneous functions.

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

/*************** Compiler specific include files and library translations ****/
#if defined(__TURBOC__)
   #include "alloc.h"
   #include "mem.h"
   #define   _fmemmove memmove
#else
   #include "malloc.h"
   #include "memory.h"
   #include "time.h"
   #if !defined (WIN32)
      #define farmalloc _fmalloc
      #define farrealloc _frealloc
      #define farfree   _ffree
   #endif
#endif

/******************************************************************************
    GetWindowPointer:
    This function returns the pointer to the window data for the specified window
*******************************************************************************/
PTERWND GetWindowPointer(HWND hWnd)
{
    PTERWND w=NULL;
    BYTE class[20]="";
    int i;

    if (hWnd) GetClassName(hWnd,class,sizeof(class)-1);
    if (lstrcmpi(class,TER_CLASS)==0) w=(PTERWND)(DWORD)GetWindowLong(hWnd,0);  // get it from the window
    else {
       for (i=0;i<TotalWinPtrs;i++) if (WinPtr[i].InUse && WinPtr[i].hWnd==hWnd) break;
       if (i>=TotalWinPtrs) w=NULL;
       else                 w=WinPtr[i].data;
    }

    if (!w) TerLastMsg=MSG_BAD_WIN_HANDLE;

    return w;
}

/******************************************************************************
    TerGetLastError:
    Wrapper for GetLastError
******************************************************************************/
DWORD TerGetLastError()
{
    #if defined(WIN32)
       return GetLastError();
    #else
       return 0L;
    #endif
}

/******************************************************************************
    TerGetVersion:
    Returnst the dll version string
******************************************************************************/
BOOL WINAPI _export TerGetVersion(LPBYTE version)
{
    HRSRC hRsrc;
    HGLOBAL hMem;
    LPWORD pVersionInfo;
    DWORD *p;
    int i;
    DWORD dwMS,dwLS;

    version[0]=0;

    hRsrc = FindResource(hTerInst, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
    if (hRsrc==NULL) return false;

    hMem = LoadResource( hTerInst, hRsrc );
    if (hMem==NULL) return false;
   
    
    pVersionInfo = (LPWORD)LockResource(hMem );

    p = (DWORD*)pVersionInfo;

    // Looks for the Value signature: 0xFEEF04BD
    for (i = 0; i < (*pVersionInfo) / 2; i++, p++ ) {
       if ( *p  == 0xFEEF04BD ) {  
          // Jumps the signature and the structure version fields.
          p += 2;
          dwMS = *p++;
          dwLS = *p;
          wsprintf(version,"%i.%i.%i.%i", HIWORD( dwMS ), LOWORD(dwMS), HIWORD( dwLS ), LOWORD( dwLS ) );
          break;
       }
    }
    
    UnlockResource( hMem );
    FreeResource( hMem );      
 
    return (lstrlen(version)>0);
}

/******************************************************************************
    TerIsInIE:
    Add auto-complete word/phrase pair
******************************************************************************/
BOOL WINAPI _export TerIsInIE(HWND hWnd,BOOL IsInIE)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    InIE=IsInIE;
    return true;
}
 
/******************************************************************************
    TerAddAutoCompWord:
    Add auto-complete word/phrase pair
******************************************************************************/
BOOL WINAPI _export TerAddAutoCompWord(HWND hWnd,LPBYTE ACWord, LPBYTE ACPhrase)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    if ((TotalAutoComps+1)>MAX_AUTO_COMPS) return false;
    if (ACWord==null) ACWord="";
    if (ACPhrase==null) ACPhrase="";
    if (lstrlen(ACWord)==0) return false;

    AutoCompWord[TotalAutoComps]=MemAlloc(lstrlen(ACWord)+1);
    lstrcpy(AutoCompWord[TotalAutoComps],ACWord);

    AutoCompPhrase[TotalAutoComps]=MemAlloc(lstrlen(ACPhrase)+1);
    lstrcpy(AutoCompPhrase[TotalAutoComps],ACPhrase);

    TotalAutoComps++;

    return true;
}

/******************************************************************************
    TerClearAutoCompList:
    Clear the auto-completion list
******************************************************************************/
BOOL WINAPI _export TerClearAutoCompList(HWND hWnd)
{
    PTERWND w;
    int i;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // free auto-complete strings
    for (i=0;i<TotalAutoComps;i++) {
       MemFree(AutoCompWord[i]);
       MemFree(AutoCompPhrase[i]);
    } 
    TotalAutoComps=0;

    return true;
}

/******************************************************************************
    TerSetParamRect:
    Set the parameter rectangle
******************************************************************************/
BOOL WINAPI _export TerSetParamRect(HWND hWnd, int left, int right, int top, int bot)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    ParamRect.left=left;
    ParamRect.right=right;
    ParamRect.top=top;
    ParamRect.bottom=bot;

    return TRUE;
}

/******************************************************************************
    TerSetLinkDblClick:
    Returns TRUE if double-click is required to invoke a link.
******************************************************************************/
BOOL WINAPI _export TerSetLinkDblClick(HWND hWnd,BOOL DblClick)
{
    PTERWND w;
    BOOL OldValue;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    OldValue=LinkDblClick;
    LinkDblClick=DblClick;
    return OldValue;
}

/******************************************************************************
    TerProcessTab:
    Returns TRUE if the tab key is used by the editor to edit text.
******************************************************************************/
BOOL WINAPI _export TerProcessTab(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return False(TerFlags6&TFLAG6_DONT_PROCESS_TAB);
}
 
/******************************************************************************
    TerQueryExit:
    Returns TRUE if a TE window can be closed
******************************************************************************/
BOOL WINAPI _export TerQueryExit(HWND hWnd)
{
   PTERWND w;
   int MessageResult;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
   
   if (InDialogBox) return FALSE;

   if (TerArg.modified && !TerArg.ReadOnly) { // let user save the modifications
       if (IDYES==(MessageResult=MessageBox(hTerWnd,MsgString[MSG_SAVE_MODS],"",MB_YESNOCANCEL))) {
          if (lstrlen(DocName)>0) return TerSave(w,DocName,TRUE);
          else                    return TerSaveAs(w,DocName);
       }
       else if (MessageResult==IDCANCEL) return FALSE;
   }

   return TRUE;
}

/******************************************************************************
    TerLoadDll:
    Load a dll file.
******************************************************************************/
HINSTANCE TerLoadDll(LPBYTE DllName)
{
    UINT OldErrorMode;
    HINSTANCE hInst=NULL;
    BYTE path[300];
    int i;

    // set the error mode to catch the 'file not found' error
    OldErrorMode=SetErrorMode(SEM_NOOPENFILEERRORBOX);

    // load the SpellTime product
    if (!hInst) {       // try to load from ter32 directory
      if (GetModuleFileName(hTerInst,path,sizeof(path))>0) {
          i=lstrlen(path)-1;
          while(i>=0 && path[i]!=':' && path[i]!='\\') i--;
          i++;
          path[i]=0;
          strcat(path,DllName);
          hInst=LoadLibrary(path);
      } 
    } 

    if (!hInst) {       // try to load from the exe directory
      if (GetModuleFileName(NULL,path,sizeof(path))>0) {
          i=lstrlen(path)-1;
          while(i>=0 && path[i]!=':' && path[i]!='\\') i--;
          i++;
          path[i]=0;
          strcat(path,DllName);
          hInst=LoadLibrary(path);
      } 
    } 
    
    if (!hInst) hInst=LoadLibrary(DllName);  // load using general Windows load scheme

    SetErrorMode(OldErrorMode);    // reset the error mode

    return hInst;
}

/******************************************************************************
    GetBitVal:
    Extract specified bits from the specified offset.
******************************************************************************/
DWORD GetBitVal(DWORD val, int off, int bits)
{
    val=val<<(31-off);
    val=val>>(32-bits);

    return val;
} 
 
/******************************************************************************
    GetTopParent:
    Given the top parent window.
******************************************************************************/
HWND GetTopParent(PTERWND w,HWND hWnd)
{
    HWND hParent;
 
    if (!hWnd) return hWnd;

    hParent=GetParent(hWnd);

    if (hParent && hParent!=hWnd) return GetTopParent(w,hParent);
    else return hWnd;            // no more parents
}
 
/******************************************************************************
    DateToDays:
    Given the date in the yyyymmdd format, find number of days since 12/31/1899
******************************************************************************/
long DateToDays(long date)
{
    long mm,dd,yy,days;
    int i;

    if (date<=999999L) date=19000000L+date;
 
    yy=(date)/10000L;
    mm=(date-yy*10000L)/100L;
    dd=date-yy*10000L-mm*100L;
             
    yy=yy-1900;
    days=yy*365;                   // calculate days since 12/31/1899 
    if (yy>0) days=days+((yy-1)/4)+1; // add leap days 
    for (i=1;i<mm;i++) {           // add days for the previous months 
       if (i==2) {                 // feb 
          if (((yy/4)*4)==yy) days=days+29;
          else                days=days+28;
       }
       else if (i==4 || i==6 || i==9 || i==11) days=days+30;
       else                                    days=days+31;
    }
    days=days+dd;                  // add the day of the month 

    return days;
}


/******************************************************************************
  StrPrepend:
  Prepend a string
*******************************************************************************/
void StrPrepend(LPBYTE string,LPBYTE pre)
{
   BYTE TempStr[MAX_WIDTH];
   lstrcpy(TempStr,pre);      //insert pre
   lstrcat(TempStr,string);   // add string to tempstr
   lstrcpy(string,TempStr);   // put back.
}

/******************************************************************************
    AlphaFormat:
    Turn a number into alpha format numberals.
*******************************************************************************/
void AlphaFormat(LPBYTE string, int nbr, BOOL upper)
{
    int count=(int)((nbr-1)/26),j;   // how many times over 'z'
    BYTE chr=(upper?'A':'a')+(BYTE)((nbr-1)%26);

    for (j=0;j<(count+1);j++) string[j]=chr;
    string[j]=0;
}
 
/******************************************************************************
    romanize:
    Turn a number into roman numberals.
*******************************************************************************/
void romanize(LPBYTE string, int val, BOOL upper)
{

    int base = 1; // units

    string[0]=0;

    while(val > 0) {
        int RemDigits = val / 10;
        int LeftDigit = val - (10 * RemDigits);
        val = RemDigits;
        if (base == 1) {       // first digit from the left
            switch (LeftDigit) {
                case 1: lstrcpy(string, "I"); break;
                case 2: lstrcpy(string, "II"); break;
                case 3: lstrcpy(string, "III"); break;
                case 4: lstrcpy(string, "IV"); break;
                case 5: lstrcpy(string, "V"); break;
                case 6: lstrcpy(string, "VI"); break;
                case 7: lstrcpy(string, "VII"); break;
                case 8: lstrcpy(string, "VIII"); break;
                case 9: lstrcpy(string, "IX"); break;
                default:break; // 0
            }
        }
        else if (base == 2) {      // second digit from the left (tens)
            switch (LeftDigit)
            {
                case 1: StrPrepend(string, "X"); break;
                case 2: StrPrepend(string, "XX"); break;
                case 3: StrPrepend(string, "XXX"); break;
                case 4: StrPrepend(string, "XL"); break;
                case 5: StrPrepend(string, "L"); break;
                case 6: StrPrepend(string, "LX"); break;
                case 7: StrPrepend(string, "LXX"); break;
                case 8: StrPrepend(string, "LXXX"); break;
                case 9: StrPrepend(string, "XC"); break;
                default:break; // 0
            }
        }
        else if (base == 3) { // third digit from the left (hundreds)
            switch (LeftDigit)
            {
                case 1: StrPrepend(string, "C"); break;
                case 2: StrPrepend(string, "CC"); break;
                case 3: StrPrepend(string, "CCC"); break;
                case 4: StrPrepend(string, "CD"); break;
                case 5: StrPrepend(string, "D"); break;
                case 6: StrPrepend(string, "DC"); break;
                case 7: StrPrepend(string, "DCC"); break;
                case 8: StrPrepend(string, "DCCC"); break;
                case 9: StrPrepend(string, "CM"); break;
                default:break; // 0
            }
        }
        else if (base == 4) { // fourth digit from the left (thousands)
           while (LeftDigit-- > 0) StrPrepend(string, "M" );
        }
        else StrPrepend(string, "!"); // too big. Silly index for a list anyway.

        base++;
    }

    if (!upper) CharLowerBuff(string,lstrlen(string));
     
}

/******************************************************************************
    OridnalString:
    Turn a number into oridinal string: First, Second, Third
*******************************************************************************/
LPBYTE OrdinalString(LPBYTE str, int val, bool AddSuffix, bool upper)
{
    BYTE str1[MAX_WIDTH];

    strcpy(str,"");
      
    if (val>999999)  wsprintf(str,"%d",val);
    else if (val==0) strcpy(str,"Zero");
    else strcpy(str,OrdinalStringThousands(str1,val,AddSuffix));

    StrTrim(str);
    if (upper) CharUpperBuff(str,strlen(str));
     
    return str;
}

/******************************************************************************
    OridnalString20:
    Oridinal string from 0 to 20.
*******************************************************************************/
LPBYTE OrdinalString20(LPBYTE str, int val, bool AddSuffix)
{
    strcpy(str,"");

    if (AddSuffix) {
      switch (val)
      {
         case 1: strcpy(str,"First");break;
         case 2: strcpy(str,"Second");break;
         case 3: strcpy(str,"Third");break;
         case 4: strcpy(str,"Fourth");break;
         case 5: strcpy(str,"Fifth");break;
         case 6: strcpy(str,"Sixth");break;
         case 7: strcpy(str,"Seventh");break;
         case 8: strcpy(str,"Eighth");break;
         case 9: strcpy(str,"Ninth");break;
         case 10: strcpy(str,"Tenth");break;
         case 11: strcpy(str,"Eleventh");break;
         case 12: strcpy(str,"Twelfth");break;
         case 13: strcpy(str,"Thirteenth");break;
         case 14: strcpy(str,"Fourteenth");break;
         case 15: strcpy(str,"Fifteenth");break;
         case 16: strcpy(str,"Sixteenth");break;
         case 17: strcpy(str,"Seventeenth");break;
         case 18: strcpy(str,"Eighteenth");break;
         case 19: strcpy(str,"Nineteenth");break;
         case 20: strcpy(str,"Twentieth");break;
         default: break;
      }
    }
    else {
      switch (val)
      {
         case 1: strcpy(str,"One ");break;
         case 2: strcpy(str,"Two ");break;
         case 3: strcpy(str,"Three ");break;
         case 4: strcpy(str,"Four ");break;
         case 5: strcpy(str,"Five ");break;
         case 6: strcpy(str,"Six ");break;
         case 7: strcpy(str,"Seven ");break;
         case 8: strcpy(str,"Eight ");break;
         case 9: strcpy(str,"Nine ");break;
         case 10: strcpy(str,"Ten ");break;
         case 11: strcpy(str,"Eleven ");break;
         case 12: strcpy(str,"Twelve ");break;
         case 13: strcpy(str,"Thirteen ");break;
         case 14: strcpy(str,"Fourteen ");break;
         case 15: strcpy(str,"Fifteen ");break;
         case 16: strcpy(str,"Sixteen ");break;
         case 17: strcpy(str,"Seventeen ");break;
         case 18: strcpy(str,"Eighteen ");break;
         case 19: strcpy(str,"Nineteen ");break;
         case 20: strcpy(str,"Twenty ");break;
         default: break;
      }
    } 

    return str;
}     
   
/******************************************************************************
    OrdinalString99:
    Oridinal string from 0 to 99.
*******************************************************************************/
LPBYTE OrdinalString99(LPBYTE str, int val, bool AddSuffix)
{
    int LeftDigit,RightDigit;
    BYTE str1[MAX_WIDTH];

    if (val<=20) return OrdinalString20(str,val,AddSuffix);
    if (val>99)  return str;
    
    strcpy(str,"");

    LeftDigit=val/10;
    RightDigit=val % 10;   // remainder

    switch (LeftDigit)
    {
       case 2: strcpy(str,"Twenty ");break;
       case 3: strcpy(str,"Thirty ");break;
       case 4: strcpy(str,"Forty ");break;
       case 5: strcpy(str,"Fifty ");break;
       case 6: strcpy(str,"Sixty ");break;
       case 7: strcpy(str,"Seventy ");break;
       case 8: strcpy(str,"Eighty ");break;
       case 9: strcpy(str,"Ninety ");break;
       default: break;
    }

    strcat(str,OrdinalString20(str1,RightDigit,AddSuffix));
    return str;
}

/******************************************************************************
    OrdinalString999:
    Oridinal string from 0 to 999.
*******************************************************************************/
LPBYTE OrdinalString999(LPBYTE str, int val,bool AddSuffix)
{
    int LeftDigit,RightDigits;
    BYTE str1[MAX_WIDTH];
    LPBYTE hundreds;

    if (val<=99) return OrdinalString99(str1,val,AddSuffix);
    if (val>999)  return str;

    strcpy(str,"");

    LeftDigit=val/100;
    RightDigits=val % 100;   // remainder


    OrdinalString20(str,LeftDigit,false);    // left string

    hundreds="Hundred ";
    if (RightDigits==0 && AddSuffix) hundreds="Hundredth";
    strcat(str,hundreds);
      
    if (RightDigits>0) {
        strcat(str,"And ");
        strcat(str,OrdinalString99(str1,RightDigits,AddSuffix));
    }

    return str;
}

/******************************************************************************
    OrdinalStringThousands:
    Oridinal string from 0 to 999999.
*******************************************************************************/
LPBYTE OrdinalStringThousands(LPBYTE str, int val,bool AddSuffix)
{
    int LeftDigit,RightDigits;
    BYTE str1[MAX_WIDTH];
    LPBYTE thousands;

    if (val<=999) return OrdinalString999(str,val,AddSuffix);
    if (val>999999)  return str;

    strcpy(str,"");

    LeftDigit=val/1000;
    RightDigits=val % 1000;   // remainder


    OrdinalString999(str,LeftDigit,false);    // left string

    thousands="Thousand ";
    if (RightDigits==0 && AddSuffix) thousands="Thousandth";
    strcat(str,thousands);
      
    if (RightDigits>0) {
       strcat(str,"And ");
       strcat(str,OrdinalString999(str1,RightDigits,AddSuffix));
    }

    return str;
}


/******************************************************************************
    LeadPtr:
    Get the pointer to the specified lead character.
******************************************************************************/
LPBYTE LeadPtr(LPBYTE lead, int col)
{
   return (lead?&(lead[col]):NULL);
}

/******************************************************************************
    LeadByte:
    Get the lead character at the specified column position
******************************************************************************/
BYTE LeadByte(LPBYTE lead, int col)
{
   return (lead?lead[col]:0);
}

/******************************************************************************
    TerSetDefDir:
    Set default directory for loading and saving the files.  You can also set the
    default file type to show in the File->Open dialog.  The type should be one of
    the SAVE_ constants.
******************************************************************************/
BOOL WINAPI _export TerSetDefDir(HWND hWnd, LPBYTE DefDir, int type)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    lstrcpy(UserDir,DefDir);
    UserFileType=type;

    return TRUE;
}

/******************************************************************************
    TerSetLinkPictDir:
    Set default directory for reading the linked pictures from the rtf file.
******************************************************************************/
BOOL WINAPI _export TerSetLinkPictDir(HWND hWnd, LPBYTE PictDir)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    lstrcpy(LinkPictDir,PictDir);

    return TRUE;
}

/******************************************************************************
    TerSetFocus:
    Set focus to the editor using mouse down/up message.
    Set default directory for loading and saving the files.  You can also set the
    default file type to show in the File->Open dialog.  The type should be one of
    the SAVE_ constants.
******************************************************************************/
BOOL WINAPI _export TerSetFocus(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    TerOpFlags|=TOFLAG_SET_FOCUS;
    PostMessage(hWnd,WM_LBUTTONDOWN,0,0L);
    PostMessage(hWnd,WM_LBUTTONUP,0,0L);

    return TRUE;
}

/******************************************************************************
    TerGetRtfDocInfo:
    Get the rtf document information.  The InfoType is one of the INFO_
    constants.
    This function returns the length of the string.
    Set the string to NULL to just get the length.
******************************************************************************/
int WINAPI _export TerGetRtfDocInfo(HWND hWnd, int InfoType, LPBYTE string)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (InfoType<0 || InfoType>=INFO_MAX) return 0;

    if (string) string[0]=0;
    if (!pRtfInfo[InfoType]) return 0;

    if (string) lstrcpy(string,pRtfInfo[InfoType]);

    return lstrlen(pRtfInfo[InfoType]);
}

/******************************************************************************
    TerSetRtfDocInfo:
    Get the rtf document information.
    This function returns the length of the string.
    Set the string to NULL to just get the length.
******************************************************************************/
BOOL WINAPI _export TerSetRtfDocInfo(HWND hWnd, int InfoType, LPBYTE string)
{
    PTERWND w;
    int len;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (InfoType<0 || InfoType>=INFO_MAX) return FALSE;

    if (pRtfInfo[InfoType]) {   // release the old string
       MemFree(pRtfInfo[InfoType]);
       pRtfInfo[InfoType]=NULL;
    }
    if (!string) return TRUE;

    len=lstrlen(string)+1;

    pRtfInfo[InfoType]=MemAlloc(len+1);
    lstrcpy(pRtfInfo[InfoType],string);

    TerArg.modified++;

    return TRUE;
}


/*****************************************************************************
    FreeRtfInfo:
    Free up the rtf information array.
******************************************************************************/
FreeRtfInfo(PTERWND w)
{
    int i;

    for (i=0;i<INFO_MAX;i++) {
       if (pRtfInfo[i]) MemFree(pRtfInfo[i]);
       pRtfInfo[i]=NULL;
    }

    return TRUE;
}

/*****************************************************************************
    FrameToMargX:
    Convert a X value relative to the beginning of the frame set to the X value
    retaive to the left margin of the page.
******************************************************************************/
int FrameToMargX(PTERWND w, int x)
{
   int sect;

   if (BorderShowing) {
      x-=PrtToTwipsY(LeftBorderWidth);  // subtract the border width
      // subtract margin
      sect=TerGetPageSect(hTerWnd,CurPage);
      x-=(int)(TerSect[sect].LeftMargin*1440);
   }

   return x;
}

/*****************************************************************************
    FrameToPageY:
    Convert a Y value relative to the top of the frame set to the Y value
    retaive to the top of the page.
******************************************************************************/
int FrameToPageY(PTERWND w, int y)
{
   if (y>ScrToTwipsY(FirstPageHeight)) y-=ScrToTwipsY(FirstPageHeight);  // remove the first page height if any
   if (BorderShowing) y-=PrtToTwipsY(TopBorderHeight);
   else if (!ViewPageHdrFtr) {          // add the top margin
      int sect=TerGetPageSect(hTerWnd,CurPage);
      y+=(int)(TerSect[sect].TopMargin*1440);
   }

   return y;
}

/*****************************************************************************
    PageToFrameY:
    Convert a Y value (twips) relative to the top of the page to the Y value
    retaive to the top of the FrameSet. The PageY is assumed to be relative to 
    the curent page.
******************************************************************************/
int PageToFrameY(PTERWND w, int y)
{
   if (BorderShowing) y+=PrtToTwipsY(TopBorderHeight);
   else if (!ViewPageHdrFtr) {          // add the top margin
      int sect=TerGetPageSect(hTerWnd,CurPage);
      y-=(int)(TerSect[sect].TopMargin*1440);
   }

   if (CurPage>FirstFramePage) y+=ScrToTwipsY(FirstPageHeight);  // remove the first page height if any

   return y;
}

/*****************************************************************************
    FileExists:
    Returns TRUE if a file exists.
******************************************************************************/
BOOL FileExists(LPBYTE name)
{
    HANDLE hFile;

    hFile=CreateFile(name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); 

    if (hFile!=INVALID_HANDLE_VALUE) CloseHandle(hFile);

    return (hFile!=INVALID_HANDLE_VALUE);
}

/******************************************************************************
    TerLineInfoFlags:
    This function returns TRUE if any of the given INFO_ flags are set for the line
******************************************************************************/
BOOL WINAPI _export TerLineInfoFlags(HWND hWnd, long LineNo, UINT flags)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return LineInfo(w,LineNo,flags);
}

/*****************************************************************************
    LineInfo:
    Check if any of the INFO_ flag is set for a line
******************************************************************************/
BOOL LineInfo(PTERWND w, long LineNo, UINT flag)
{
   return (LineNo>=0 && LineNo<TotalLines && tabw(LineNo) && tabw(LineNo)->type&flag);
}

/*****************************************************************************
    LineEndsInBreak:
    This function returns TRUE if the line ends in any of the break characters.
******************************************************************************/
BOOL LineEndsInBreak(PTERWND w, long LineNo)
{
    LPBYTE ptr;
    BYTE BreakChar;

    if (LineNo<0 || LineNo>=TotalLines) return FALSE;
    if (LineLen(LineNo)==0) return FALSE;

    ptr=pText(LineNo);
    BreakChar=ptr[LineLen(LineNo)-1];

    return IsBreakChar(w,BreakChar);
}

/*****************************************************************************
    IsBreakChar:
    This function returns TRUE if the given character is a break character
******************************************************************************/
BOOL IsBreakChar(PTERWND w, BYTE chr)
{
    return (chr==ParaChar || chr==CellChar || lstrchr(BreakChars,chr)!=NULL);
}

/*****************************************************************************
    RoundInt:
    Round off an integer to a given precision step
******************************************************************************/
RoundInt(int val, int step)
{
    BOOL neg=FALSE;
    int result;

    if (val<0) {      // is it a negative number
      val=-val;       // convert to positive
      neg=TRUE;
    }

    result = step*((val+step/2)/step);   // do rounding

    if (neg) return -result;
    else     return result;
}

/*****************************************************************************
    SwapInts:
    swap two integers
******************************************************************************/
SwapInts(LPINT pVal1, LPINT pVal2)
{
    int temp=(*pVal1);
    (*pVal1)=(*pVal2);
    (*pVal2)=temp;
    return TRUE;
}

/*****************************************************************************
    SwapLongs:
    swap two long values
******************************************************************************/
SwapLongs(LPLONG pVal1, LPLONG pVal2)
{
    long temp=(*pVal1);
    (*pVal1)=(*pVal2);
    (*pVal2)=temp;
    return TRUE;
}

/*****************************************************************************
    SwapDbls:
    swap two double values
******************************************************************************/
SwapDbls(double *pVal1, double *pVal2)
{
    double temp=(*pVal1);
    (*pVal1)=(*pVal2);
    (*pVal2)=temp;
    return TRUE;
}

/*****************************************************************************
    ParaCharIncluded:
    This function returns TRUE if atleast one para char is included in the
    highlighted text.  This function assumes that the highlighted block
    is normalized.
******************************************************************************/
BOOL ParaCharIncluded(PTERWND w,long line1,int col1,long line2,int col2)
{
    long l;
    int  EndCol;

    if (HilightType==HILIGHT_OFF) return TRUE;   // assume included

    for (l=line1; l<=line2;l++) {
       if (l==line2) EndCol=col2;
       else          EndCol=LineLen(l);

       if (EndCol>0) {
          LPBYTE ptr=pText(l);
          if (ptr[EndCol-1]==ParaChar) return TRUE;
       }
    }
    return FALSE;
}

/*******************************************************************************
    ToggleFittedView:
    Toggle Fitted Page Mode.
*******************************************************************************/
ToggleFittedView(PTERWND w)
{
    if (!TerArg.PrintView) return FALSE;  // fitted mode not available in non-word wrap mode

    RepageBeginLine=0;                    // start repagination

    if (TerArg.FittedView) TerArg.FittedView=FALSE;     // turn-off fitted page mode
    else {
       TerArg.FittedView=TRUE;
       if      (!TerArg.PageMode) return TogglePageMode(w);
       else if (ViewPageHdrFtr) return ToggleViewHdrFtr(w);    // page header/footer not available in fitted mode
    }

    if (TerArg.PageMode && !(TerArg.FittedView)) PagesShowing=TRUE;
    else                                         PagesShowing=FALSE;

    DisplayStatus(w);                    // redisplay the status line
    if (TerArg.PageMode) TerRepaginate(hTerWnd,TRUE);
    else                 PaintTer(w);

    return TRUE;
}

/*******************************************************************************
    TogglePageMode:
    Toggle Page Mode.
*******************************************************************************/
TogglePageMode(PTERWND w)
{
    int y,height;

    if (!TerArg.PrintView) return FALSE;

    // set current parameters
    if (TerArg.PageMode) {    // turn page mode off
       // turnoff header/footer editing and display
       if (EditPageHdrFtr) ToggleEditHdrFtr(w);
       ViewPageHdrFtr=FALSE;

       y=GetRowY(w,CurLine);  // y from the beginning of the page

       TerArg.PageMode=FALSE; // turn on page mode
       TerArg.FittedView=FALSE;// fitted view is not available in non-PageMode

       TotalFrames=1;         // Initialize the first frame
       InitFrame(w,0);
       frame[0].ScrFirstLine=frame[0].ScrLastLine=BeginLine;
       frame[0].RowOffset=0;
       frame[0].PageFirstLine=0;
       frame[0].PageLastLine=TotalLines-1;

       CurLineY=y-TerWinOrgY; // y from the beginning of window
       if (CurLineY<0) CurLineY=0;
       if (CurLineY>TerWinHeight) CurLineY=TerWinHeight;
       TerWinOrgY=0;

       BeginLine=CurLine;     // calculate begin line and cur row
       while (BeginLine>0) {
          height=ScrLineHeight(w,BeginLine,TRUE,FALSE);
          if (height>CurLineY) break;
          CurLineY-=height;
          BeginLine--;
       }

       frame[0].ScrFirstLine=frame[0].ScrLastLine=BeginLine;
       WinHeight=TerWinHeight/TerFont[0].height;    // appox WinHeight

       CurRow=CurLine-BeginLine;
       WinHeight=(int)(CurRow+1);
       WinYOffsetLine=-1;    // first line with offset
    }
    else {                   // turn on page mode
       CurLineY=GetRowY(w,CurLine); // y from the beginning of window

       TerArg.PageMode=TRUE; // turn on page mode
       CreateFrames(w,FALSE,CurPage,CurPage);// create frames for the current page

       y=GetRowY(w,CurLine); // y from the beginning of the page
       TerWinOrgY=y-CurLineY;
       if (TerWinOrgY<0) TerWinOrgY=0;
    }

    if (TerArg.PageMode && !(TerArg.FittedView)) PagesShowing=TRUE;
    else                                         PagesShowing=FALSE;

    SetTerWindowOrg(w);

    PaintTer(w);

    return TRUE;
}

/*****************************************************************************
    CellCharIncluded:
    This function returns TRUE if atleast one cell char of the specified level 
    is included in the highlighted text. Set level to -1 to check of any cell character
    This function assumes that the highlighted block
    is normalized.
******************************************************************************/
BOOL CellCharIncluded(PTERWND w,long line1,int col1,long line2,int col2, int level)
{
    long l;
    int  EndCol;

    if (HilightType==HILIGHT_OFF) return TRUE;   // assume included

    for (l=line1; l<=line2;l++) {
       if (l==line2) EndCol=col2;
       else          EndCol=LineLen(l);

       if (EndCol>0) {
          LPBYTE ptr=pText(l);
          if (ptr[EndCol-1]==CellChar) {
             if (level<0) return TRUE;
             else if (cell[cid(l)].level==level) return TRUE;
          }
       }
    }
    return FALSE;
}

/*****************************************************************************
    CallDialogBox:
    This function call a modal dialog box, and returns an interger result
******************************************************************************/
int CallDialogBox(PTERWND w,LPBYTE template,DLGPROC lpProc,DWORD lParam)
{
    int result;
    struct StrDlgParam dlg;
    BOOL SaveCaretEngaged=CaretEngaged;

    lpProc = (DLGPROC) MakeProcInstance((FARPROC)lpProc, hTerInst);
    dlg.w=w;                           // pass window data pointer
    dlg.data=(LPBYTE)lParam;            // other data

    InDialogBox=TRUE;                  // displaying a dialog box now

    result=DialogBoxParam(hTerInst,template,hTerWnd,lpProc,(DWORD)(LPBYTE)&dlg);

    InDialogBox=FALSE;

    TlbIdClicked=0;                              // reset any toolbar id clicked

    FreeProcInstance((FARPROC)lpProc);           // set back the old window
    
    SetFocus(hTerWnd);

    InitCaret(w);                                // redisplay the caret
    if (SaveCaretEngaged && !CaretEngaged) TerEngageCaret(hTerWnd,TRUE);

    return result;
}


/******************************************************************************
    SetWinStyle:
    Set a window style.
******************************************************************************/
SetWinStyle(HWND hWnd, BOOL set, DWORD style)
{
    DWORD styles=GetWindowLong(hWnd,GWL_STYLE);
    if (set) styles|=style;
    else     ResetLongFlag(styles,style);
    SetWindowLong(hWnd,GWL_STYLE,styles);
    return TRUE;
}

/******************************************************************************
    GetWinStyle:
    Set a window style.
******************************************************************************/
DWORD GetWinStyle(HWND hWnd, DWORD style)
{
    DWORD styles=GetWindowLong(hWnd,GWL_STYLE);
    return (styles&style);
}

/******************************************************************************
    TerSetWinBorder:
    Set the window border. border: 0=None, 1=Single line, 2=Double line
******************************************************************************/
BOOL WINAPI _export TerSetWinBorder(HWND hWnd, int border)
{
    DWORD styles=GetWindowLong(hWnd,GWL_STYLE);
    if (border==0)      styles = styles&(~(DWORD)(WS_CAPTION|WS_THICKFRAME));
    else if (border==1) styles = WS_BORDER|styles&(~(DWORD)(WS_CAPTION|WS_THICKFRAME));
    else if (border==2) styles = WS_CAPTION|WS_THICKFRAME|styles;
    SetWindowLong(hWnd,GWL_STYLE,styles);
    SetWindowPos(hWnd,NULL,0,0,0,0,SWP_DRAWFRAME|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

    return TRUE;
}

/******************************************************************************
    TerOverrideStyles:
    Style bits (TER_) to override at the creation time.
******************************************************************************/
DWORD WINAPI _export TerOverrideStyles(DWORD styles)
{
    DWORD OldStyles=TerStyles;
    TerStyles=styles;
    return OldStyles;
}

/******************************************************************************
    TerSetWrapWidth:
    Set the wrap width.  This function is valid in the simple word wrap mode
    only.
******************************************************************************/
BOOL WINAPI _export TerSetWrapWidth(HWND hWnd,int WidthChars, int WidthTwips, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (!TerArg.WordWrap) return FALSE;
    if (TerArg.PrintView) return FALSE;
    if (WidthChars>0 && WidthTwips>0) return FALSE;

    WrapWidthChars=WidthChars;
    WrapWidthTwips=WidthTwips;

    if (repaint) PaintTer(w);

    return TRUE;
}

/*****************************************************************************
    ReposPageHdrFtr:
    Repostion page header/footer
******************************************************************************/
BOOL ReposPageHdrFtr(PTERWND w,BOOL repaint)
{
    long l,k,FirstLine,LastLine,LineCount;
    BOOL HdrFtrFound,modified=FALSE,SectBreak;
    BOOL FHdrFound,FFtrFound;
    LPBYTE ptr;
    int i,sect;

    if (!TerArg.WordWrap) return TRUE;

    if (HilightType!=HILIGHT_OFF) HilightType=HILIGHT_OFF;

    RecreateSections(w);         // recreate the section boundaries

    HdrFtrFound=FHdrFound=FFtrFound=FALSE;
    FirstLine=LastLine=-1;
    sect=0;                      // initial section

    // scan the lines backword to detect hdr/ftr lines
    for (l=TotalLines-1;l>=-1;l--) {
       // check if section break character found
       if (l>=0) SectBreak=(BOOL)(LineFlags(l)&LFLAG_SECT) || LineInfo(w,l,INFO_SECT);
       else      SectBreak=FALSE;
       if (SectBreak && l>=0) {              // look for the actual section break character
          ptr=pText(l);
          for (i=0;i<LineLen(l);i++) if (ptr[i]==SECT_CHAR) break;
          if (i==LineLen(l)) SectBreak=FALSE;
       }

       // move the header/footer if needed
       if (l==-1 || SectBreak) {
          if (HdrFtrFound && FirstLine>(l+1)) {
              RepairHdrFtrDelims(w,FirstLine,LastLine);
              LineCount=LastLine-FirstLine+1;

              // make room for the new lines
              if (!CheckLineLimit(w,TotalLines+LineCount)) return TRUE;
              MoveLineArrays(w,l+1,LineCount,'B');

              FirstLine+=LineCount;
              LastLine+=LineCount;
              // copy the line pointers
              for (k=0;k<LineCount;k++) {
                 FreeLine(w,l+k+1);          // free line before overwriting
                 LinePtr[l+k+1]=LinePtr[FirstLine+k];
                 LinePtr[FirstLine+k]=NULL;  // this pointer transfered
              }
              MoveLineArrays(w,FirstLine,LineCount,'D');  // delete the old lines
              modified=TRUE;
          }

          // check if first page header/footer needs to be created
          if (TerSect[sect].flags&SECT_HAS_TITLE_PAGE && TerSect1[sect].PrevSect<0) {
             if (!FFtrFound && !CreatePageHdrFtr(w,FFTR_CHAR,sect)) return FALSE;
             if (!FHdrFound && !CreatePageHdrFtr(w,FHDR_CHAR,sect)) return FALSE;
             ResetUintFlag(TerSect[sect].flags,SECT_HAS_TITLE_PAGE);  // first page header/footer created
             modified=TRUE;
          }  

          if (l==-1) break;      // done with all the lines
          HdrFtrFound=FHdrFound=FFtrFound=FALSE;
          FirstLine=LastLine=-1;
          if (l>=0 && LineInfo(w,l,INFO_SECT)) sect=tabw(l)->section;
       }

       // detect header footer
       if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) {
          if (LineLen(l)==1 && GetCurChar(w,l,0)==FHDR_CHAR) FHdrFound=TRUE;
          if (LineLen(l)==1 && GetCurChar(w,l,0)==FFTR_CHAR) FFtrFound=TRUE;

          if (HdrFtrFound) {     // header/footer already found, delete this line
              MoveLineArrays(w,l,1,'D');
              FirstLine--;
              LastLine--;
              modified=TRUE;
              continue;
          }
          else {
              if (LastLine==-1) LastLine=l;
              FirstLine=l;
          }
       }
       else if (LastLine>=0) HdrFtrFound=TRUE;
    }

    // adjust the current line
    if (TotalLines<=0) {
       TotalLines=1;
       InitLine(w,0);
    }
    if (CurLine>=TotalLines) CurLine=TotalLines-1;

    // recreate the sections
    if (modified) {
       RecreateSections(w);
       RepairTable(w);
       if (TotalLines<SMALL_FILE) Repaginate(w,FALSE,FALSE,0,TRUE);
    }

    SectModified=FALSE;          // reset the modification flags
    PosPageHdrFtr=FALSE;

    if (repaint) PaintTer(w);

    return TRUE;
}

/*****************************************************************************
    RepairHdrFtrDelims
    Repair header/footer delimiters if necessary
******************************************************************************/
BOOL RepairHdrFtrDelims(PTERWND w,long FirstLine,long LastLine)
{
    BYTE  FirstChar,HdrFtrChar=0;
    BOOL  IsDelimLine;
    long  FirstDelimLine=-1;
    long  k;
    LPBYTE ptr,lead;

    // extract header and footer groups
    for (k=FirstLine;k<=LastLine;k++) {
       if (LineLen(k)==1) FirstChar=GetCurChar(w,k,0);
       else               FirstChar=0;

       IsDelimLine=IsHdrFtrChar(FirstChar);  // the line has a header/footer delimiter character

       if (IsDelimLine) {
          if (FirstDelimLine==-1) {
             FirstDelimLine=k;
             HdrFtrChar=FirstChar;
          }
          else {
             FirstDelimLine=-1;
             if (FirstChar!=HdrFtrChar) {   // fix the delimter line
                OpenTextPtr(w,k,&ptr,&lead);
                ptr[0]=HdrFtrChar;
                if (lead) lead[0]=0;
                CloseTextPtr(w,k);
             }
          }

          SetHdrFtrLineFlags(w,k,HdrFtrChar); // set the LineFlags for the delimiter line
          continue;
       }

       ResetLongFlag(LineFlags(k),LFLAG_HDRS_FTRS);
    }

    return TRUE;
}

/*****************************************************************************
    SetHdrFtrLineFlags:
    Set the LineFlags for header/footer delimiter lines.
******************************************************************************/
BOOL SetHdrFtrLineFlags(PTERWND w,long LineNo,BYTE HdrFtrChar)
{
    ResetLongFlag(LineFlags(LineNo),LFLAG_HDRS_FTRS);
    if      (HdrFtrChar==HDR_CHAR)  LineFlags(LineNo)|=LFLAG_HDR;
    else if (HdrFtrChar==FTR_CHAR)  LineFlags(LineNo)|=LFLAG_FTR;
    else if (HdrFtrChar==FHDR_CHAR) LineFlags(LineNo)|=LFLAG_FHDR;
    else if (HdrFtrChar==FFTR_CHAR) LineFlags(LineNo)|=LFLAG_FFTR;

    return TRUE;
}

/*****************************************************************************
    ToggleEditHdrFtr:
    Toggle the editing of page header and footer
******************************************************************************/
BOOL ToggleEditHdrFtr(PTERWND w)
{
   long l,m,FirstLine,LastLine,LineCount,adj;
   BOOL HdrFound,FtrFound,HdrFtrDeleted=FALSE,delete,HdrFtrCreated=FALSE;
   BOOL InFirstHdrFtr=FALSE;
   int sect;
   UINT CurFlag,PrevFlag;

   if (!TerArg.PageMode) return FALSE;

   // check if header/footer exist for the section
   RecreateSections(w);             // recreate the section boundaries


   if (EditPageHdrFtr) {           // turn off the page header/footer editing
      FirstLine=LastLine=-1;
      PrevFlag=0;
      for (l=0;l<=TotalLines;l++) {
          if (l<TotalLines && LineFlags(l)&(LFLAG_FHDR_FFTR)) InFirstHdrFtr=!InFirstHdrFtr;

          if (l<TotalLines && !InFirstHdrFtr)
                CurFlag=PfmtId[pfmt(l)].flags&PAGE_HDR_FTR;
          else  CurFlag=0;

          if ((l>=TotalLines || CurFlag!=PrevFlag) && FirstLine>=0) {
             sect=GetSection(w,FirstLine);
             LineCount=LastLine-FirstLine+1;
             delete=TRUE;
             if (TerSect[sect].FirstLine==0L) delete=FALSE;  // spare the first section
             if (LineCount!=3) delete=FALSE;
             if (LineLen(FirstLine+1)!=1) delete=FALSE;
             if (!(LineFlags(FirstLine)&(LFLAG_HDR|LFLAG_FTR))) delete=FALSE;
             if (!(LineFlags(LastLine)&(LFLAG_HDR|LFLAG_FTR))) delete=FALSE;

             if (delete) {
                MoveLineArrays(w,FirstLine,LineCount,'D');
                if (LastLine<HilightBegRow) HilightBegRow-=(long)LineCount;
                if (LastLine<HilightEndRow) HilightEndRow-=(long)LineCount;

                // adjust the current line
                adj=0;
                for (m=0;m<LineCount;m++) if ((FirstLine+m)<CurLine) adj++;
                CurLine-=adj;
                if (CurLine<0) CurLine=0;

                HdrFtrDeleted=TRUE;
                l-=LineCount;        // adjust for the deleted text
             }
             FirstLine=-1;
          }
          if (l>=TotalLines) break;

          if (CurFlag && FirstLine==-1) FirstLine=l;

          LastLine=l;
          PrevFlag=CurFlag;
      }

      if (HdrFtrDeleted) {
         RecreateSections(w);
         if (CurLine>=TotalLines) CurLine=TotalLines-1;
         if (TotalLines<SMALL_FILE) Repaginate(w,FALSE,FALSE,0,TRUE);
      }
      EditPageHdrFtr=FALSE;        // header/footer editing disabled
      PaintTer(w);

   }
   else {                          // turn on the page header/footer editing
      for (sect=0;sect<TotalSects;sect++) {
         if (!TerSect[sect].InUse) continue;

         HdrFound=FtrFound=FALSE;
         for (l=TerSect[sect].FirstLine;l<=TerSect[sect].LastLine;l++) {
            if (LineFlags(l)&LFLAG_HDR) HdrFound=TRUE;
            if (LineFlags(l)&LFLAG_FTR) FtrFound=TRUE;
            if (HdrFound && FtrFound) break;
         }

         if (!FtrFound) if (!CreatePageHdrFtr(w,FTR_CHAR,sect)) return FALSE;
         if (!HdrFound) if (!CreatePageHdrFtr(w,HDR_CHAR,sect)) return FALSE;
         if (!HdrFound || !FtrFound) HdrFtrCreated=TRUE;
      }

      ViewPageHdrFtr=EditPageHdrFtr=TRUE;

      if (HdrFtrCreated) {
         RecreateSections(w);
         if (TotalLines<SMALL_FILE) Repaginate(w,FALSE,FALSE,0,TRUE);
      }

      if (!TerArg.PageMode) TogglePageMode(w);
      else                  PaintTer(w);
   }


   return TRUE;
}

/*****************************************************************************
    ToggleViewHdrFtr:
    Toggle the display of page header and footer
******************************************************************************/
BOOL ToggleViewHdrFtr(PTERWND w)
{
   BOOL HdrFound=FALSE,FtrFound=FALSE;
   int sect;

   if (!TerArg.PrintView) return FALSE;

   // make sure that header and footer lines exist
   if (ViewPageHdrFtr) {
      if (EditPageHdrFtr) ToggleEditHdrFtr(w);
      ViewPageHdrFtr=FALSE;
   }
   else {
      // find the first section in the document
      RecreateSections(w);
      sect=GetSection(w,0L);

      // check if header and footer exist
      if (TerSect1[sect].hdr.FirstLine>=0) HdrFound=TRUE;
      if (TerSect1[sect].ftr.FirstLine>=0) FtrFound=TRUE;
      if (!FtrFound) if (!CreatePageHdrFtr(w,FTR_CHAR,sect)) return FALSE;
      if (!HdrFound) if (!CreatePageHdrFtr(w,HDR_CHAR,sect)) return FALSE;

      ViewPageHdrFtr=TRUE;
      if (!HdrFound || !FtrFound) {
         RecreateSections(w);
         if (TotalLines<SMALL_FILE) Repaginate(w,FALSE,FALSE,0,TRUE);
      }
   }

   if (!TerArg.PageMode) TogglePageMode(w);
   else                  PaintTer(w);

   return TRUE;
}

/*****************************************************************************
    CheckLineLimit:
    This routine checks if the requested number of lines exceeds the currently
    allocated line space. If so, it attempts to allocated more space for
    the text array.  No need to reallocate the page array.
******************************************************************************/
CheckLineLimit(PTERWND w,long count)
{
    long NewMaxLines,OldSize,NewSize;
    void huge *pMem;
    long l;

    // check user limit
    if (TerArg.LineLimit>0 && count>TerArg.LineLimit && !(TerOpFlags&TOFLAG_WRAPPING)) return FALSE;

    if (count<MaxLines) return TRUE;  // enough line space available

    NewMaxLines=MaxLines+MaxLines/4;  // allocate 25 percent more space
    if (NewMaxLines<=count) NewMaxLines=count+1;
    if (TerArg.LineLimit>0 && NewMaxLines>TerArg.LineLimit && !(TerOpFlags&TOFLAG_WRAPPING)) // apply caller's limit
                            NewMaxLines=TerArg.LineLimit;

    // reallocate space for text pointer
    OldSize=(MaxLines+1)*sizeof(struct StrLinePtr far *);
    NewSize=(NewMaxLines+1)*sizeof(struct StrLinePtr far *);
    if (NULL!=(pMem=OurAlloc(NewSize))) {

       // move data to new location
       HugeMove(LinePtr,pMem,OldSize);

       // free old memory and assign the new pointer
       OurFree(LinePtr);
       LinePtr=pMem;

       for (l=MaxLines;l<NewMaxLines;l++) LinePtr[l]=NULL;  // initialize the pointers
    }
    else return FALSE;

    // set new max lines
    MaxLines=NewMaxLines;


    return TRUE;
}

/*****************************************************************************
    ExpandArray:
    Expand table arrays. The array size is incremented by one third. To apply
    the total max only for 16 bit, pass the value as negative.
******************************************************************************/
BOOL ExpandArray(PTERWND w,int UnitSize,void far *(huge *ppMem),LPINT pCurMax,int NewMax, int TotalMax)
{
    long OldSize,NewSize;
    void huge *pMemOld=(*ppMem);
    void huge *pMemNew;
    int CurMax=(*pCurMax);
    BOOL FixedMax=TRUE;  // max applicable for win16 and win32

    if (TotalMax<0) {
       TotalMax=-TotalMax;
       FixedMax=FALSE;   // max applicable only for win16
    }

    if (NewMax<0) {
      NewMax=CurMax+(CurMax/3);
      if (FixedMax || !Win32) {
         if (NewMax>TotalMax) NewMax=TotalMax;
         if (CurMax==NewMax) return FALSE;      // no more space left
      }
    }

    // exapnd the TerFont structure
    OldSize=((long)CurMax+1)*UnitSize;
    NewSize=((long)NewMax+1)*UnitSize;
    if (NULL!=(pMemNew=OurAlloc(NewSize))) {

       // move data to new location
       HugeMove(pMemOld,pMemNew,OldSize);

       // free old memory and assign the new pointer
       OurFree(pMemOld);

       // pass the results
       (*ppMem)=pMemNew;
       (*pCurMax)=NewMax;

       return TRUE;
    }
    else return FALSE;
}

/***************************************************************************
    TerMbcsExt:
    Get the file extension, including the dot character
****************************************************************************/
LPBYTE WINAPI _export TerMbcsExt(HWND hWnd,LPBYTE file, LPBYTE ext)
{
   int len,i,j;
   WORD chr;
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return NULL;  // get the pointer to window data

   ext[0]=0;
   len=(int)TerMbcsLen(hTerWnd,file);

   if (len==0) return ext;

   // look for the period character
   for (i=len-1;i>=0;i--) {
     chr=TerMbcsChar(hTerWnd,file,i);
     if (chr==':' || chr=='\\') return ext;   // extension not found
     if (chr=='.') break;
   }
   if (i<0) return ext;                       // no extension found

   j=0;
   while (i<len) {
      chr=TerMbcsChar(hTerWnd,file,i);
      if (mbcs && chr&0xFF00) {
         ext[j]=HIBYTE(chr);
         j++;
      }
      ext[j]=LOBYTE(chr);
      j++;
      i++;
   }
   ext[j]=0;

   return ext;
}

/***************************************************************************
    TerMbcsDir:
    Get the file path name including the trailing ':' or '\'.  If the
    file does not have the path name then the result string is "".
****************************************************************************/
LPBYTE WINAPI _export TerMbcsDir(HWND hWnd,LPBYTE file, LPBYTE dir)
{
   int len,i,j;
   WORD chr;
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return NULL;  // get the pointer to window data

   dir[0]=0;
   len=(int)TerMbcsLen(hTerWnd,file);

   if (len==0) return dir;

   // look for the ':' or '\' character
   for (i=len-1;i>=0;i--) {
     chr=TerMbcsChar(hTerWnd,file,i);
     if (chr==':' || chr=='\\') break;   // dirension not found
   }
   if (i<0) return dir;                       // no dirension found

   j=0;
   len=i+1;                              // lenght of the path name
   i=0;
   while (i<len) {
      chr=TerMbcsChar(hTerWnd,file,i);
      if (mbcs && chr&0xFF00) {
         dir[j]=HIBYTE(chr);
         j++;
      }
      dir[j]=LOBYTE(chr);
      j++;
      i++;
   }
   dir[j]=0;

   return dir;
}

/***************************************************************************
    TerSetMergeDelim:
    Set the delimiter character used for mail-merge name and data arrays.
****************************************************************************/
BOOL WINAPI _export TerSetMergeDelim(LPBYTE delim)
{
    MergeDelim=delim[0];
    return TRUE;
}
 
/***************************************************************************
    TerMergeFields:
    This function is used to merge the data for the mail merge fields.
****************************************************************************/
BOOL WINAPI _export TerMergeFields(HWND hWnd,LPBYTE names,LPBYTE data, BOOL repaint)
{
    PTERWND w;
    int  i,NameLen,DataLen,NewLen,MaxFields=MAX_FIELDS;
    int  NameCount,DataCount,len,FieldStart,col,SpacesBefore,SpacesAfter;
    long line;
    LPBYTE name=NULL;
    LPBYTE ptr,lead,pData,text,LeadText,value,LeadValue,pCopy;
    LPWORD fmt,ct;
    //BOOL PreviouslyDisabled;
    BOOL rewrap=FALSE,result=FALSE,matched;
    struct StrMergeData far *pMerge=NULL;             // pointer to the initial buffer

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    //OurPrintf("tmf Begin");

    if (names==NULL || data==NULL) return TRUE;

    // allocate the merge array
    if (NULL==(pMerge=OurAlloc(sizeof(struct StrMergeData)*(MaxFields+1)))) return FALSE;
    name=OurAlloc(2*(MAX_WIDTH+2));

    // Extract the field names
    NameCount=FieldStart=0;
    len=lstrlen(names);
    pMerge[NameCount].pName=names;      // first name
    for (i=0;i<=len;i++) {
       if (i==len || names[i]==MergeDelim) {  // if a delimeter encountered
          if (i==FieldStart) {
             PrintError(w,MSG_NULL_FIELD,"");
             goto END;
          }
          pMerge[NameCount].NameLen=i-FieldStart;
          NameCount++;

          // increase the size of the array if needed
          if (NameCount>=MaxFields) {
             MaxFields=MaxFields+MAX_FIELDS;  // increment the size
             pMerge=OurRealloc(pMerge,sizeof(struct StrMergeData)*(MaxFields+1));
          }

          i++;                         // go past the delimeter
          if (i>=len) break;           // end of list
          FieldStart=i;                // next field starts here
          pMerge[NameCount].pName=&names[FieldStart];
       }
    }

    // Extract the field data
    DataCount=FieldStart=0;
    len=lstrlen(data);
    pMerge[DataCount].pData=data;      // first name
    for (i=0;i<=len;i++) {
       if (i==len || data[i]==MergeDelim) {  // if a delimeter encountered
          pMerge[DataCount].DataLen=i-FieldStart;
          DataCount++;
          if (DataCount>=MaxFields) break;   // past the array size
          if (i>=len) break;          // end of list
          FieldStart=i+1;             // go past the delimiter
          pMerge[DataCount].pData=&data[FieldStart];
       }
    }

    if (NameCount!=DataCount || DataCount==MaxFields) {
       PrintError(w,MSG_COUNT_MISMATCH,"");
       goto END;
    }


    // Locate the fields and supply data
    line=0;                            // start from 0,0
    col=0;
    while (TerLocateStyle(hTerWnd,FIELD_NAME_STYLE,&line,&col,&len) ){
       //OurPrintf("tmf -2, l: %d, c: %d, ln: %d",line,col,len);
       
       SpacesAfter=SpacesBefore=0;

       // remove break character from the beginning of the string
       OpenTextPtr(w,line,&ptr,&lead);
       for (i=0;i<len;i++) if (ptr[i+col]!=ParaChar && ptr[i+col]!=CellChar && lstrchr(BreakChars,ptr[i+col])==NULL) break;
       col+=i;                         // advance the beginning index
       len-=i;
       //OurPrintf("tmf -1, len: %d",len);
       if (len==0) continue;           // get the next string
       

       // Copy the string
       len=TerMakeMbcs(hTerWnd,name,&ptr[col],(mbcs?&lead[col]:NULL),len);

       if ((col+len)>=LineLen(line) && !(LineFlags(line)&(LFLAG_PARA|LFLAG_BREAK))) { // check if the field name overflows to the next line
          long  NextLine=line+1;
          BOOL NoMore=FALSE;
          while (NextLine<TotalLines && !NoMore) {
             if (LineLen(NextLine)>0) {
                LPBYTE ptr1=pText(NextLine),lead1;
                LPWORD fmt1;
                int ExtraLen=0,PrevLen;

                fmt1=OpenCfmt(w,NextLine);
                for (i=0;i<LineLen(NextLine);i++) {
                   if (!(TerFont[fmt1[i]].style&FIELD_NAME_STYLE)) break;
                   if (ptr1[i]==ParaChar || ptr1[i]==CellChar || lstrchr(BreakChars,ptr1[i])!=NULL) NoMore=TRUE;
                   if (NoMore) break;
                }
                CloseCfmt(w,NextLine);

                ExtraLen=i;
                PrevLen=LineLen(line);
                if (ExtraLen==0 || (ExtraLen+PrevLen)>=(MAX_WIDTH)) break; // move these bytes to the original line

                // add the text from the next line to the current line
                LineAlloc(w,line,PrevLen,PrevLen+ExtraLen);
                MoveCharInfo(w,NextLine,0,line,PrevLen,ExtraLen);

                // add to the field name
                OpenTextPtr(w,NextLine,&ptr1,&lead1);
                len+=TerMakeMbcs(hTerWnd,&name[len],ptr1,(mbcs?lead1:NULL),ExtraLen);
                CloseTextPtr(w,NextLine);

                MoveLineData(w,NextLine,0,ExtraLen,'D');   // delete it from the beginning of the next line
                if (LineLen(NextLine)>0) break;            // non field character in the first character
             }
             NextLine++;
          }
       }
       CloseTextPtr(w,line);
       name[len]=0;

       //OurPrintf("tmf 0");

       // remove break character from the end of the string
       for (i=len-1;i>=0;i--) if (name[i]!=ParaChar && name[i]!=CellChar && lstrchr(BreakChars,name[i])==NULL) break;
       len=i+1;
       name[len]=0;

       // truncate spaces before the field name
       for (i=0;i<len;i++) if (name[i]!=' ') break;
       SpacesBefore=i;
       col+=SpacesBefore;              // position beyond the current string
       if (SpacesBefore==len) continue;// blank field

       for (i=SpacesBefore;i<=len;i++) name[i-SpacesBefore]=name[i]; // truncate spaces before, includes null
       len-=SpacesBefore;              // new length

       // truncate spaces after the field name
       for (i=len-1;i>=0;i--) if (name[i]!=' ') break;
       SpacesAfter=len-i-1;
       len-=SpacesAfter;               // new length
       name[len]=0;
       NameLen=len;

       //OurPrintf("tmf 0a, NameCount: %d",NameCount);

       // find the field name in the table
       for (i=0;i<NameCount;i++) {
          LPBYTE pName;

          if (pMerge[i].NameLen!=NameLen) continue;
          
          // compare
          pName=MakeCopy(pMerge[i].pName,NameLen);   // make copy to an allocated memory
          matched=(lstrcmpi(pName,name)==0);  // lstrcmpi is mbcs enabled
          MemFree(pName);            // free the temporary pointer
          if (matched) break;
       }

       //OurPrintf("tmf 0a-1, i: %d",i);
       if (i==NameCount) {             // field not found, get from the message
           if (TerArg.hParentWnd) {
              //PreviouslyDisabled=EnableWindow(hTerWnd,FALSE);   // disable our window
              pData=(LPBYTE)SendMessageToParent(w,TER_MERGE,(WPARAM)hTerWnd,(DWORD)(LPBYTE)name,TRUE);
              //pData=(LPBYTE)SendMessage(TerArg.hParentWnd,TER_MERGE,(WPARAM)hTerWnd,(DWORD)(LPBYTE)name);
              //if (!PreviouslyDisabled) EnableWindow(hTerWnd,TRUE); // enable our window
           }
           if (!TerArg.hParentWnd || !pData) { // data not available
              col+=(len+SpacesAfter);
              continue;
           }
           DataLen=lstrlen(pData);
       }
       else {
           pData=pMerge[i].pData;
           DataLen=pMerge[i].DataLen;
       }

       //OurPrintf("tmf 0b");

       // check the data len
       if (DataLen>MAX_DATA_LEN) {
          if (TerArg.WordWrap) rewrap=TRUE;   // very long lines will need complete rewapping
          else {
             PrintError(w,MSG_MERGE_DATA_TOO_LONG,"");
             goto END;
          }
       }

       //OurPrintf("tmf 1");
       // split the field name and field value
       NameLen=AllocDB(w,name,&text,&LeadText);
       
       pCopy=MakeCopy(pData,DataLen);     // make copy of the string
       NewLen=AllocDB(w,pCopy,&value,&LeadValue);
       MemFree(pCopy);                    // delete the temporary copy

       DataLen=NewLen;

       //OurPrintf("tmf 2");

       // replace the field name by field value string
       if (DataLen>NameLen) MoveLineData(w,line,col+NameLen-1,DataLen-NameLen,'A'); // insert spaces
       if (DataLen<NameLen) MoveLineData(w,line,col+DataLen,NameLen-DataLen,'D'); // delete

       //OurPrintf("tmf 3, DataLen: %d",DataLen);

       OpenTextPtr(w,line,&ptr,&lead);
       for (i=0;i<DataLen;i++) {
          ptr[col+i]=value[i];
          if (mbcs) lead[col+i]=LeadValue[i];
       }
       CloseTextPtr(w,line);

       //OurPrintf("tmf 4");

       OpenCharInfo(w,line,&fmt,&ct);
       for (i=NameLen;i<DataLen;i++) {
          fmt[col+i]=fmt[col+NameLen-1];
          ct[col+i]=0;
       }
       CloseCharInfo(w,line);

       //OurPrintf("tmf 5");

       // free the temporary buffers
       OurFree(text);              // free the temporary buffers
       OurFree(LeadText);
       OurFree(value);
       OurFree(LeadValue);

       if (LineLen(line)>LineWidth && !rewrap) LineAlloc(w,line,LineLen(line),LineWidth);

       //OurPrintf("tmf 6");

       // remove the FIELD formatting from the characters
       HilightType=HILIGHT_CHAR;   // select the text
       HilightBegRow=HilightEndRow=line;
       HilightBegCol=col-SpacesBefore;
       HilightEndCol=col+DataLen+SpacesAfter;
       SetTerCharStyle(hWnd,FIELD_NAME_STYLE,FALSE,FALSE);

       TerArg.modified++;

       col+=(DataLen+SpacesAfter);     // position beyond the current string
       
       //OurPrintf("tmf 7, line: %d, c: %d, tl: %d, ln: %d",line,col,TotalLines,(line>=0 && line<TotalLines)?LineLen(line):-1);
    }
    //OurPrintf("tmf 8, fields done");

    if (rewrap) TerRewrap(hTerWnd);
    //OurPrintf("tmf 9, wrapped");

    if (repaint) PaintTer(w);                       // refresh the screen
    //OurPrintf("tmf 10, painted");

    result=TRUE;

    END:
    if (pMerge) OurFree(pMerge);  // allocated data structure
    if (name)   OurFree(name);

    //OurPrintf("tmf End");

    return result;
}

/***************************************************************************
    TerLocateStyle:
    This routine scans the text starting from the given line and col position
    to search for the given style.  If the style is located, the routine
    returns a TRUE value.  It also updates the line and col parameters with
    the location of the style. It also returns the length of the text with
    the given style.
****************************************************************************/
BOOL WINAPI _export TerLocateStyle(HWND hWnd, UINT style, long far *StartLine, int far *StartCol, int far *StringLen)
{
    PTERWND w;
    long line;
    int  i,j,col,CurCfmt,NextCfmt;
    LPWORD fmt;
    UINT CurStyle;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // scan each line
    for (line=(*StartLine);line<TotalLines;line++) {
       if (line==(*StartLine)) col=*StartCol;
       else                    col=0;

       if (col>=LineLen(line)) continue;                   // go to next line
       if (LineLen(line)==0) continue;

       if (cfmt(line).info.type==UNIFORM) {
          CurCfmt=cfmt(line).info.fmt;
          CurStyle=TerFont[CurCfmt].style;
          //if (CurStyle&PICT && !(style&PICT)) continue;   // picture does'nt count
          if (CurStyle&style) {                // style found
             *StartLine=line;
             *StartCol=col;
             *StringLen=LineLen(line)-col;
             return TRUE;
          }
          else continue;                       // style not found
       }

       // open the line
       fmt=OpenCfmt(w,line);
       for (i=col;i<LineLen(line);i++) {
         CurCfmt=fmt[i];
         CurStyle=TerFont[CurCfmt].style;
         //if (CurStyle&PICT && !(style&PICT)) continue;          // picture does'nt count
         if (CurStyle&style) {                 // style begins
            for (j=i+1;j<LineLen(line);j++) {  // find the end of the style
               NextCfmt=fmt[j];
               CurStyle=TerFont[NextCfmt].style;
               //if (CurStyle&PICT && !(style&PICT)) break;       // end of style
               if (!(CurStyle&style)) break;   // end of style
            }
            // return location parameters
            *StartLine=line;
            *StartCol=i;
            *StringLen=j-i;
            CloseCfmt(w,line);
            return TRUE;
         }
       }
       CloseCfmt(w,line);
    }

    return FALSE;
}

/***************************************************************************
    TerLocateStyleChar:
    This routine scans the text starting from the given line and col position
    to search for the given style.  The called specifies the direction of
    the search.  The caller also specifies to look for the presence of absence
    of such a style
    If the style is located, the routine returns a TRUE value.
    It also updates the line and col parameters with the location of the style.
****************************************************************************/
BOOL WINAPI _export TerLocateStyleChar(HWND hWnd, UINT style, BOOL present, long far *StartLine, int far *StartCol, BOOL forward)
{
    PTERWND w;
    long line;
    int  i,col,CurCfmt;
    LPWORD fmt;
    UINT CurStyle;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
                                                                                 
    // scan each line in the forward direction
    if (forward) {
       for (line=(*StartLine);line<TotalLines;line++) {
          if (line==(*StartLine)) col=*StartCol;
          else                    col=0;

          if (col>=LineLen(line)) continue;                   // go to next line
          if (LineLen(line)==0) continue;

          if (cfmt(line).info.type==UNIFORM) {
             CurCfmt=cfmt(line).info.fmt;
             CurStyle=TerFont[CurCfmt].style;
             //if (present && !(style&PICT) && CurStyle&PICT) continue;         // picture does'nt count
             if ((present && CurStyle&style) || (!present && !(CurStyle&style))) {                // style found
                *StartLine=line;
                *StartCol=col;
                return TRUE;
             }
             else continue;                       // style not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i<LineLen(line);i++) {
            CurCfmt=fmt[i];
            CurStyle=TerFont[CurCfmt].style;
            //if (present && !(style&PICT) && CurStyle&PICT) continue;         // picture does'nt count
            if ((present && CurStyle&style) || (!present && !(CurStyle&style))) {                // style found
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return TRUE;
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
             CurStyle=TerFont[CurCfmt].style;
             //if (present && !(style&PICT) && CurStyle&PICT) continue;         // picture does'nt count
             if ((present && CurStyle&style) || (!present && !(CurStyle&style))) {                // style found
                *StartLine=line;
                *StartCol=col;
                return TRUE;
             }
             else continue;                       // style not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i>=0;i--) {
            CurCfmt=fmt[i];
            CurStyle=TerFont[CurCfmt].style;
            //if (present && !(style&PICT) && CurStyle&PICT) continue;         // picture does'nt count
            if ((present && CurStyle&style) || (!present && !(CurStyle&style))) {                // style found
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return TRUE;
            }
          }
          CloseCfmt(w,line);
       }
    }

    return FALSE;
}

/***************************************************************************
    TerLocateFontFlags:
    This routine scans the text starting from the given line and col position
    to search for the given font flag.  The called specifies the direction of
    the search.  The caller also specifies to look for the presence of absence
    of such a style
    If the style is located, the routine returns a TRUE value.
    It also updates the line and col parameters with the location of the style.
****************************************************************************/
BOOL WINAPI _export TerLocateFontFlags(HWND hWnd, UINT flags, BOOL present, long far *StartLine, int far *StartCol, BOOL forward)
{
    PTERWND w;
    long line;
    int  i,col,CurCfmt;
    LPWORD fmt;
    UINT CurFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // scan each line in the forward direction
    if (forward) {
       for (line=(*StartLine);line<TotalLines;line++) {
          if (line==(*StartLine)) col=*StartCol;
          else                    col=0;

          if (col>=LineLen(line)) continue;                   // go to next line
          if (LineLen(line)==0) continue;

          if (cfmt(line).info.type==UNIFORM) {
             CurCfmt=cfmt(line).info.fmt;
             CurFlags=TerFont[CurCfmt].flags;
             //if (present && !(flags&PICT) && CurFlags&PICT) continue;         // picture does'nt count
             if ((present && CurFlags&flags) || (!present && !(CurFlags&flags))) {                // flags found
                *StartLine=line;
                *StartCol=col;
                return TRUE;
             }
             else continue;                       // flags not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i<LineLen(line);i++) {
            CurCfmt=fmt[i];
            CurFlags=TerFont[CurCfmt].flags;
            //if (present && !(flags&PICT) && CurFlags&PICT) continue;         // picture does'nt count
            if ((present && CurFlags&flags) || (!present && !(CurFlags&flags))) {                // flags found
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return TRUE;
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
             CurFlags=TerFont[CurCfmt].flags;
             //if (present && !(flags&PICT) && CurFlags&PICT) continue;         // picture does'nt count
             if ((present && CurFlags&flags) || (!present && !(CurFlags&flags))) {                // flags found
                *StartLine=line;
                *StartCol=col;
                return TRUE;
             }
             else continue;                       // flags not found
          }

          // open the line
          fmt=OpenCfmt(w,line);
          for (i=col;i>=0;i--) {
            CurCfmt=fmt[i];
            CurFlags=TerFont[CurCfmt].flags;
            //if (present && !(flags&PICT) && CurFlags&PICT) continue;         // picture does'nt count
            if ((present && CurFlags&flags) || (!present && !(CurFlags&flags))) {                // flags found
               *StartLine=line;
               *StartCol=i;
               CloseCfmt(w,line);
               return TRUE;
            }
          }
          CloseCfmt(w,line);
       }
    }

    return FALSE;
}

/***************************************************************************
    ToggleHiddenText:
    Toggle the display of hidden text on/off
****************************************************************************/
BOOL ToggleHiddenText(PTERWND w)
{
    ShowHiddenText=!ShowHiddenText;     // toggle
    
    if (!ShowHiddenText && TerFlags4&TFLAG4_HIDE_HIDDEN_PARA_MARK) HideHiddenParaMarkers(w);
      
    RecreateFonts(w,hTerDC);            // recreate fonts

    PageModifyCount=-1;                 // force repagination
    RepageBeginLine=0;                  // complete repagination

    PaintTer(w);

    return TRUE;
}

/***************************************************************************
    ToggleFieldNames:
    Toggle the display of field name/data
****************************************************************************/
BOOL ToggleFieldNames(PTERWND w)
{
    ShowFieldNames=!ShowFieldNames;     // toggle
    RecreateFonts(w,hTerDC);            // recreate fonts

    PageModifyCount=-1;                 // force repagination
    RepageBeginLine=0;                  // complete repagination

    PaintTer(w);

    return TRUE;
}

/***************************************************************************
    ToggleFootnoteEdit:
    Toggle the editing of inline footnote text text on/off
****************************************************************************/
BOOL ToggleFootnoteEdit(PTERWND w, BOOL footnote)
{
    if (footnote) EditFootnoteText=!EditFootnoteText;  // toggle
    else          EditEndnoteText=!EditEndnoteText;    // toggle
    
    RecreateFonts(w,hTerDC);             // recreate fonts

    PageModifyCount=-1;                  // force repagination
    RepageBeginLine=0;                   // complete repagination

    PaintTer(w);

    return TRUE;
}

/***************************************************************************
    ToggleStatusRibbon:
    Toggle status ribbon on/off
****************************************************************************/
BOOL ToggleStatusRibbon(PTERWND w)
{
    TerArg.ShowStatus=!TerArg.ShowStatus;         // toggle

    InvalidateRect(hTerWnd,NULL,TRUE);     // force WM_PAINT with erase background

    return TRUE;
}

/***************************************************************************
    ToggleRuler:
    Toggle ruler on/off
****************************************************************************/
BOOL ToggleRuler(PTERWND w)
{
    TerArg.ruler=!TerArg.ruler;            // toggle

    TerInvalidateRect(w,NULL,TRUE);     // force WM_PAINT with erase background

    return TRUE;
}

/***************************************************************************
    ToggleParaMark:
    Toggle para mark on/off
****************************************************************************/
BOOL ToggleParaMark(PTERWND w)
{
    int i;
    BOOL FramePictFound=FALSE;

    ShowParaMark=!ShowParaMark;            // toggle

    // set the size of the framed pictures
    for (i=0;i<TotalFonts;i++) {
       if (!(TerFont[i].InUse) || !(TerFont[i].style&PICT) || TerFont[i].FrameType==PFRAME_NONE) continue;
       SetPictSize(w,i,0,0,TRUE);    // recalulate the icon sizes
       FramePictFound=TRUE;
    }

    DeleteTextMap(w,TRUE);

    if (FramePictFound && TerArg.PageMode) TerRepaginate(hTerWnd,TRUE);
    else                                   PaintTer(w);

    return TRUE;
}

/***************************************************************************
    SetPictSize:
    Set height and width (specified in device units) of the picture.  When the
    'icon' argument is true and the picture is horizontally aligned, the
    'height' and 'width' are ignored and the function sets the 'icon' size
    for the picture (instead of the full size).
****************************************************************************/
SetPictSize(PTERWND w, int pict, int height, int width, BOOL icon)
{
    int j;

    // hidden text
    if (HiddenText(w,pict)) {
       TerFont[pict].height=TerFont[pict].BaseHeight=0;
       for (j=0;j<256;j++) TerFont[pict].CharWidth[j]=0;
       return TRUE;
    } 

    if (icon && TerFont[pict].FrameType!=PFRAME_NONE && TerArg.PageMode) {
       int ParaFID=TerFont[pict].ParaFID;
       if (False(ParaFrame[ParaFID].flags&PARA_FRAME_WATERMARK)) {
          height=(TerFont[0].height*3)/4;   // use the height of the default font
          width=(ShowParaMark?DblCharWidth(w,0,TRUE,'W',0):1);
       }
    }

    if (height==0) height=1;
    if (width==0) width=1;

    if (TerFont[pict].TextAngle>0) SwapInts(&height,&width);   // swap height/width

    TerFont[pict].height=TerFont[pict].BaseHeight=height;

    // set the base height
    if (TerFont[pict].FieldId==FIELD_TEXTBOX) {  // find the base height of the field font
        struct StrForm far *pForm=(LPVOID)TerFont[pict].pInfo;
        if (pForm && pForm->FontId>=0) {
            TerFont[pict].BaseHeight=TerFont[pForm->FontId].BaseHeight;
            TerFont[pict].BaseHeight+=((TerFont[pict].height-TerFont[pForm->FontId].height)/2);  // add the top half of the border height
        }
    }
    else if (TerFont[pict].FieldId==FIELD_CHECKBOX) {  // set base height at 3/4 of height
        TerFont[pict].BaseHeight=(TerFont[pict].height*7/8);
    }
    else {
        if (TerFont[pict].offset>0) {
           int offset=InPrinting?TwipsToPrtY(TerFont[pict].offset):TwipsToScrY(TerFont[pict].offset);
           TerFont[pict].BaseHeight=height-offset;
           if (TerFont[pict].BaseHeight<0) TerFont[pict].BaseHeight=0;
        }
        else { 
           if (TerFont[pict].PictAlign==ALIGN_MIDDLE) TerFont[pict].BaseHeight=height/2 + TerFont[0].BaseHeight/2;
           if (TerFont[pict].PictAlign==ALIGN_TOP)    TerFont[pict].BaseHeight=TerFont[0].height;  // use the default font for top alignment
           if (TerFont[pict].PictAlign!=ALIGN_BOT && TerFont[pict].BaseHeight>height) {
              TerFont[pict].BaseHeight=TerFont[0].BaseHeight;
              if (TerFont[pict].BaseHeight>height) TerFont[pict].BaseHeight=height;
           }
        }
    }
    TerFont[pict].CharWidth[0]=0;         // to work with mbcs
    for (j=1;j<256;j++) TerFont[pict].CharWidth[j]=width;

    return TRUE;
}

/******************************************************************************
    Bitmap2DIB:
    This routine creates a device indenpendent bitmap from a source
    bitmap.  The source bitmap is assumed to be compatible with the
    current display context.
    The newly created device independent bitmap is stored in the TerFont
    structure in an open slot.
    If successful, the routine returns the index of the newly created
    bitmap, otherwise it returns -1.
******************************************************************************/
Bitmap2DIB(PTERWND w,HBITMAP hBM)
{
    LPBYTE  pImage=(LPVOID)NULL;
    LPBITMAPINFOHEADER pInfo=(LPVOID)NULL;
    int   pict,NumColors=256;
    DWORD  InfoSize,ImageSize;
    BITMAP bm;

    //**** find an empty slot in the font/picture table ****
    if ((pict=FindOpenSlot(w))==-1) return -1;

    GetObject(hBM,sizeof(BITMAP),(LPBYTE)&bm);     // get the dimension of bit map

    //********** create the device independent bitmap ***********************
    if (((int)bm.bmPlanes*bm.bmBitsPixel)>8 && !Clr256Display) NumColors=0;
    InfoSize=sizeof(BITMAPINFOHEADER)+NumColors*sizeof(RGBQUAD);  // create a 256 color bitmap structure

    if (NULL==(pInfo=(LPVOID)OurAlloc(InfoSize))) {
        PrintError(w,MSG_OUT_OF_MEM,"Bitmap2DIB");
        return -1;
    }

    //***** fill in the info structure ******
    pInfo->biSize=sizeof(BITMAPINFOHEADER);
    pInfo->biWidth=bm.bmWidth;
    pInfo->biHeight=bm.bmHeight;
    pInfo->biPlanes=1;                       // DIB have plane = 1
    if (NumColors==256) {
       pInfo->biBitCount=8;                  // 8 bits per pixel or color
       pInfo->biCompression=BI_RLE8;         // compression
    }
    else {
       pInfo->biBitCount=24;                 // 24 bits per pixel or color
       pInfo->biCompression=BI_RGB;          // no compression
    }
    pInfo->biSizeImage=0;                    // initialize to zero
    pInfo->biXPelsPerMeter=0;
    pInfo->biYPelsPerMeter=0;
    pInfo->biClrUsed=0;                      // depends on biBitCount
    pInfo->biClrImportant=0;                 // all colors important

    if (!GetDIBits(hTerDC,hBM,0,bm.bmHeight,NULL,(LPBITMAPINFO)pInfo,DIB_RGB_COLORS)) {
        PrintError(w,MSG_ERR_DIB_CREATE,NULL);
        if (pInfo) OurFree(pInfo);
        return -1;
    }

    ImageSize=pInfo->biSizeImage;

    if (NULL==(pImage=OurAlloc(ImageSize))) {
        PrintError(w,MSG_OUT_OF_MEM,"Bitmap2DIB(a)");
        if (pInfo) OurFree(pInfo);
        return -1;
    }

    if (!GetDIBits(hTerDC,hBM,0,bm.bmHeight,pImage,(LPBITMAPINFO)pInfo,DIB_RGB_COLORS)) {
        PrintError(w,MSG_ERR_DIB_CREATE,"Bitmap Image");
        if (pInfo) OurFree(pInfo);
        if (pImage) OurFree(pImage);
        return -1;
    }

    //*********************** fill in the TerFont structure ****************
    TerFont[pict].InUse=TRUE;
    TerFont[pict].bmWidth=bm.bmWidth;              // actual width  of the picture
    TerFont[pict].bmHeight=bm.bmHeight;            // actual height of the picture
    TerFont[pict].PictHeight=(UINT)((DWORD)(bm.bmHeight)*1440L/(DWORD)ScrResY); // display height in twips
    TerFont[pict].PictWidth=(UINT)((DWORD)(bm.bmWidth)*1440L/(DWORD)ScrResX);   // display width in twips
    TerFont[pict].ImageSize=ImageSize;
    TerFont[pict].pImage=pImage;
    TerFont[pict].InfoSize=InfoSize;
    TerFont[pict].pInfo=(LPVOID)pInfo;
    TerFont[pict].style=PICT;
    TerFont[pict].PictType=PICT_DIBITMAP;
    TerFont[pict].ObjectType=OBJ_NONE;

    return pict;
}

/******************************************************************************
    ExtractDIB:
    This routine extracts a device indenpendent bitmap from a source
    global memory handle.
    The newly created device independent bitmap is stored in the TerFont
    structure in an open slot.
    If successful, the routine returns the index of the newly created
    bitmap, otherwise it returns -1.
******************************************************************************/
ExtractDIB(PTERWND w,HGLOBAL hClip)
{
    BYTE huge *  pImage, huge *ptr;
    LPBITMAPINFOHEADER pInfo,pClip;
    int   pict;
    DWORD  InfoSize,ImageSize;

    //**** find an empty slot in the font/picture table ****
    if ((pict=FindOpenSlot(w))==-1) return -1;

    // lock the clipboard memory handle
    if (NULL==(pClip=(LPBITMAPINFOHEADER)GlobalLock(hClip))) {
        PrintError(w,MSG_OUT_OF_MEM,"ExtractDIB");
        return -1;
    }

    // extract the BITMAPINFO structure
    InfoSize=sizeof(BITMAPINFOHEADER);
    if (pClip->biCompression==BI_BITFIELDS) InfoSize+=(3*sizeof(DWORD));

    if (pClip->biClrUsed>0 && pClip->biBitCount<24) {
       InfoSize+=(pClip->biClrUsed)*sizeof(RGBQUAD);
    }
    else {
       if      (pClip->biBitCount==1) InfoSize+=2*sizeof(RGBQUAD);
       else if (pClip->biBitCount==4) InfoSize+=16*sizeof(RGBQUAD);
       else if (pClip->biBitCount==8) InfoSize+=256*sizeof(RGBQUAD);
    }

    if (NULL==(pInfo=(LPVOID)OurAlloc(InfoSize))) {
        PrintError(w,MSG_OUT_OF_MEM,"ExtractDIB(a)");
        return -1;
    }

    FarMove(pClip,pInfo,(UINT)InfoSize);

    // Extract the pixel data
    ImageSize=pInfo->biSizeImage;
    if (ImageSize==0 && pInfo->biCompression==BI_RGB) {
         long WidthBytes=(pInfo->biWidth*pInfo->biBitCount)/8;
         long RoundedWidth;
         RoundedWidth=(WidthBytes/sizeof(LONG))*sizeof(LONG);  // round to long boundaries
         if (RoundedWidth<WidthBytes) RoundedWidth+=(long)sizeof(LONG);
         ImageSize=RoundedWidth*pInfo->biHeight;
    }

    if (NULL==(pImage=OurAlloc(ImageSize))) {
        PrintError(w,MSG_OUT_OF_MEM,"ExtractDIB(b)");
        return -1;
    }

    // move image
    ptr=(BYTE huge *)pClip;             // do it 2 statement to avoid error because of compiler optimization
    HugeMove(&(ptr[InfoSize]),pImage,ImageSize);

    //*********************** fill in the TerFont structure ****************
    TerFont[pict].InUse=TRUE;
    
    if (ImgDenX==-1) {          // calculate iamge density here
       ImgDenY=ScrResX;         // resolution (pixel/inch) of the current device
       ImgDenX=ScrResY;

       if (!HtmlMode && False(TerFlags6&TFLAG6_SHOW_PICT_IN_PIXEL_SIZE) && pInfo->biXPelsPerMeter!=0 && pInfo->biYPelsPerMeter!=0) {                              // use the resolution embedded in the file to be compatible with MSWord
          ImgDenY=MulDiv(pInfo->biYPelsPerMeter,254,10000);
          ImgDenX=MulDiv(pInfo->biXPelsPerMeter,254,10000);
       }
    } 

    TerFont[pict].bmWidth=(int)(ImgDenX==OrigScrResX?pInfo->biWidth:MulDiv(pInfo->biWidth,OrigScrResX,ImgDenX));           // actual width  of the picture //>>> RZW 3/15/02 - Adjust dimensions using density
    TerFont[pict].bmHeight=(int)(ImgDenY==OrigScrResY?pInfo->biHeight:MulDiv(pInfo->biHeight,OrigScrResY,ImgDenY));         // actual height of the picture //>>> RZW 3/15/02 - Adjust dimensions using density
    TerFont[pict].PictHeight=(UINT)((DWORD)(TerFont[pict].bmHeight)*1440L/(DWORD)OrigScrResY); // display height in twips //>>> RZW 3/15/02 - Use adjusted dimensions
    TerFont[pict].PictWidth=(UINT)((DWORD)(TerFont[pict].bmWidth)*1440L/(DWORD)OrigScrResX);   // display width in twips //>>> RZW 3/15/02 - Use adjusted dimension

    TerFont[pict].OrigPictHeight=TerFont[pict].PictHeight;
    TerFont[pict].OrigPictWidth=TerFont[pict].PictWidth;

    TerFont[pict].ImageSize=ImageSize;
    TerFont[pict].pImage=pImage;
    TerFont[pict].InfoSize=InfoSize;
    TerFont[pict].pInfo=(LPVOID)pInfo;
    TerFont[pict].style=PICT;
    TerFont[pict].PictType=PICT_DIBITMAP;
    TerFont[pict].ObjectType=OBJ_NONE;

    GlobalUnlock(hClip);

    return pict;
}

/******************************************************************************
    DIB2Bitmap:
    This routine creates a device specific bitmap from a device independent
    bit map.  The first argument specifies the device context to build
    the bitmap for. The second argument specifies an index into the TerFont
    structure that contains the handle to the device independent image and
    information.                
******************************************************************************/
DIB2Bitmap(PTERWND w,HDC hDC,int pict)
{
     int i,ColorsUsed,planes,BitsPerPixel;
     LPBITMAPINFOHEADER pInfo;
     BYTE huge *pImage;
     BITMAP bm;


     if (!(TerFont[pict].style&PICT)) return TRUE;   // not a picutre element

     //*********** lock the DIB handles ***********
     pImage=(LPVOID)TerFont[pict].pImage;
     pInfo=(LPVOID)TerFont[pict].pInfo;

     //******** delete old bitmap if present *******
     if (TerFont[pict].hBM) {                      // delete the old bit map
        DeleteObject(TerFont[pict].hBM);
        TerFont[pict].hBM=0;
     }


    //********* create our bitmap *******************************************
    planes=GetDeviceCaps(hDC,PLANES);             // get the device charistrics
    BitsPerPixel=GetDeviceCaps(hDC,BITSPIXEL);

    if (NULL==(TerFont[pict].hBM=CreateBitmap((int)(pInfo->biWidth),(int)(pInfo->biHeight),planes,BitsPerPixel,NULL))) {
        PrintError(w,MSG_ERR_BMP_CREATE,"DIB2Bitmap");
        return FALSE;
    }

    if (!SetDIBits(hDC,TerFont[pict].hBM,0,(int) pInfo->biHeight,(LPBYTE)pImage,(LPBITMAPINFO)pInfo,DIB_RGB_COLORS)) {
        PrintError(w,MSG_ERR_INIT_BMP,NULL);
        return FALSE;
    }

    GetObject(TerFont[pict].hBM,sizeof(BITMAP),(LPBYTE)&bm);   // get the dimension of bit map
    TerFont[pict].bmHeight=bm.bmHeight;                        // actual dimensions
    TerFont[pict].bmWidth=bm.bmWidth;
    //TerFont[pict].OrigPictHeight=ScrToTwipsY(bm.bmHeight);    // record the original picture width/height
    //TerFont[pict].OrigPictWidth=ScrToTwipsX(bm.bmWidth);

    // calculate the number of significant colors used
    TerFont[pict].ColorsUsed=pInfo->biClrUsed;

    if (TerFont[pict].ColorsUsed==0) {
       if      (pInfo->biBitCount==1) TerFont[pict].ColorsUsed=2;
       else if (pInfo->biBitCount==4) TerFont[pict].ColorsUsed=16;
       else if (pInfo->biBitCount==8) TerFont[pict].ColorsUsed=256;
    }
    if (pInfo->biClrImportant>0) TerFont[pict].ColorsUsed=pInfo->biClrImportant;
    if (TerFont[pict].ColorsUsed>256) TerFont[pict].ColorsUsed=0; //color table not used

    // Digitise rgb into 256 colors in 256 color mode only
    if (  Clr256Display && TerFont[pict].ColorsUsed==0
       && pInfo->biBitCount==24
       && NULL!=(TerFont[pict].rgb=OurAlloc(sizeof(struct StrRgb)*MAX_RGB)) ){
       int  i,j,LeastUsed,TotalRgbs=0;
       struct StrRgb far *rgb=TerFont[pict].rgb,temp;
       long BytesPerLine=(pInfo->biBitCount*pInfo->biWidth+7)/8;   // round to byte boundary
       long ScanLineSize=sizeof(DWORD)*((BytesPerLine+sizeof(DWORD)-1)/sizeof(DWORD)); // number of dwords, round to DWORD boundary
       long l,pix;
       BYTE huge *pLine;
       BYTE red,green,blue,mask;
       int  MaxCache=10;
       int  idx[20],CacheIdx;

       CacheIdx=MaxCache;
       for (i=0;i<MaxCache;i++) idx[i]=0;
       if (TerFlags&TFLAG_BETTER_256) mask=0xF0;
       else                           mask=0xE0;  // less colors

       // modify color and build color table
       for (l=0;l<pInfo->biHeight;l++) {    // scan each line
          pLine=&(pImage[ScanLineSize*l]);  // pointer to the pixel line data
          for (pix=0;pix<pInfo->biWidth;pix++) {
             // Place the colors at fixed steps
             // Each color is left shifted by 2 bits because out of 8 only
             // high 6 bits are used by the VGA hardware.  Each color is
             // left shifted another 2 bits and then right shifted (2 bits) to
             // place the color at the of 4, ex: 0, 4, 8, etc
             // The same logic is implemented by simply 'anding' by 0xF0

             red=pLine[2]&mask;                  // extract each color component
             green=pLine[1]&mask;
             blue=pLine[0]&mask;


             // update the color table
             i=TotalRgbs;
             if (TotalRgbs>=MaxCache) {
                if (CacheIdx<MaxCache) {
                   i=idx[CacheIdx];
                   if (red!=rgb[i].red || green!=rgb[i].green || blue!=rgb[i].blue) i=TotalRgbs;
                }
                if (i==TotalRgbs) {
                   for (CacheIdx=0;CacheIdx<MaxCache;CacheIdx++) {
                      i=idx[CacheIdx];
                      if (red==rgb[i].red && green==rgb[i].green && blue==rgb[i].blue) break;
                   }
                   if (CacheIdx==MaxCache) i=TotalRgbs;     // not found in the cache
                }
             }

             LeastUsed=0;                   // find this color in the table
             if (i==TotalRgbs) {
                for (i=0;i<TotalRgbs;i++) {
                   if (red!=rgb[i].red || green!=rgb[i].green || blue!=rgb[i].blue){
                     if (i>0 && rgb[i].count<rgb[LeastUsed].count) LeastUsed=i;
                   }
                   else break;
                }
             }

             if (i==TotalRgbs) {            // color not in table
                if (TotalRgbs<MAX_RGB) TotalRgbs++; // use the next entry
                else  i=LeastUsed;  // use the least used entry
                rgb[i].red=red;
                rgb[i].green=green;
                rgb[i].blue=blue;
                rgb[i].count=0;
             }
             rgb[i].count++;     // increase the use count for this color

             // put this index in the cache
             if (CacheIdx==MaxCache) {    // not found in the cache
               for (j=MaxCache-1;j>0;j--) idx[j]=idx[j-1];
               idx[0]=i;
               CacheIdx=0;
             }

             pLine=&(pLine[3]);  // position at the next pixel (3 byte each pixel)
          }
       }

       // sort in the 'count' descending order
       TerFont[pict].TotalRgbs=TotalRgbs;
       for (i=0;i<TotalRgbs;i++) {
          for (j=i+1;j<TotalRgbs;j++) {
             if (rgb[j].count>rgb[i].count) {
                temp=rgb[i];
                rgb[i]=rgb[j];
                rgb[j]=temp;
             }
          }
       }
    }

    // *********** create a palette if necessary
    if (TerFont[pict].hPal) {         // delete any existing palette
       SelectPalette(hTerDC,GetStockObject(DEFAULT_PALETTE),TRUE);
       DeleteObject(TerFont[pict].hPal);
       TerFont[pict].hPal=NULL;
    }

    if (Clr256Display && (TerFont[pict].ColorsUsed>16 || TerFont[pict].TotalRgbs>0 || HtmlMode)) {
       LOGPALETTE far * pPal;
       LPBITMAPINFO pBmInfo=(LPBITMAPINFO)pInfo;
       //int BaseColors=TotalBaseColors;
       int BaseColors=0;

       if (TerFont[pict].TotalRgbs>0) ColorsUsed=TerFont[pict].TotalRgbs;
       else                           ColorsUsed=(WORD)TerFont[pict].ColorsUsed;

       if (NULL==(pPal=OurAlloc(sizeof(LOGPALETTE)+(ColorsUsed+BaseColors)*sizeof(PALETTEENTRY)))) return TRUE;
       pPal->palVersion=0x300;
       pPal->palNumEntries=(WORD)ColorsUsed+BaseColors;

       for (i=0;i<(int)(pPal->palNumEntries);i++) {
          if (i<BaseColors) {
             pPal->palPalEntry[i].peRed=GetRValue(PalColor[i]);
             pPal->palPalEntry[i].peGreen=GetGValue(PalColor[i]);
             pPal->palPalEntry[i].peBlue=GetBValue(PalColor[i]);
          }
          else {
             int idx=i-BaseColors;
             if (TerFont[pict].TotalRgbs>0) {
                pPal->palPalEntry[i].peRed=TerFont[pict].rgb[idx].red;
                pPal->palPalEntry[i].peGreen=TerFont[pict].rgb[idx].green;
                pPal->palPalEntry[i].peBlue=TerFont[pict].rgb[idx].blue;
             }
             else {
                pPal->palPalEntry[i].peRed=pBmInfo->bmiColors[idx].rgbRed;
                pPal->palPalEntry[i].peGreen=pBmInfo->bmiColors[idx].rgbGreen;
                pPal->palPalEntry[i].peBlue=pBmInfo->bmiColors[idx].rgbBlue;
             }
          }
          pPal->palPalEntry[i].peFlags=0;
       }

       TerFont[pict].hPal=CreatePalette(pPal);
       OurFree(pPal);
    }

    //******** delete this bitmap - it is no longer needed *******
    if (TerFont[pict].hBM) {                      // delete the old bit map
       DeleteObject(TerFont[pict].hBM);
       TerFont[pict].hBM=0;
    }

    //************ calculate the display height ******************
    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
    XlateSizeForPrt(w,pict);     // convert size to printer resolution

    return TRUE;
}


/*******************************************************************************
    DIB2Metafile:
    This function embeds the DIB intoa metafile and return the handle of the
    metafile.
*******************************************************************************/
HMETAFILE DIB2Metafile(PTERWND w, int pict)
{
    HDC hMetaDC;
    BYTE huge *pImage;
    LPBITMAPINFO pInfo;
    long width,height;
    HMETAFILE hMeta;
    METAHEADER far *hdr;         // metafile header
    int i,ColorsUsed;
    LOGPALETTE far * pPal=NULL;
    HPALETTE hPal,hOldPal;

    if (NULL==(hMetaDC=CreateMetaFile(NULL))) {
       PrintError(w,MSG_ERR_META_CREATE,"Rtf error");
       return NULL;
    }

    // blast the bitmap
    pImage=(LPVOID)TerFont[pict].pImage;
    pInfo=(LPVOID)TerFont[pict].pInfo;

    width=pInfo->bmiHeader.biWidth;
    height=pInfo->bmiHeader.biHeight;

    // initialize the metafile
    SetWindowOrgEx(hMetaDC,0,0,NULL);
    SetWindowExtEx(hMetaDC,(int)(width),(int)height,NULL);
    SetTextColor(hMetaDC,(COLORREF)0);
    SetBkColor(hMetaDC,(COLORREF)0xFFFFFF);
    SetStretchBltMode(hMetaDC,COLORONCOLOR);  // same as STRETCH_DELETESCANS

    // create a palette for 256 color bitmaps
    if ((TerFont[pict].ColorsUsed>16 || TerFont[pict].TotalRgbs>0) && !(TerFlags&TFLAG_NO_PALETTE)) {       // use default palette
        // build logical palette
        if (TerFont[pict].TotalRgbs>0) ColorsUsed=TerFont[pict].TotalRgbs;
        else                           ColorsUsed=(WORD)TerFont[pict].ColorsUsed;
        if (NULL==(pPal=OurAlloc(sizeof(LOGPALETTE)+ColorsUsed*sizeof(PALETTEENTRY)))) return (HMETAFILE)NULL;
        pPal->palVersion=0x300;
        pPal->palNumEntries=(WORD)ColorsUsed;
        for (i=0;i<(int)(pPal->palNumEntries);i++) {
            if (TerFont[pict].TotalRgbs>0) {
                pPal->palPalEntry[i].peRed=TerFont[pict].rgb[i].red;
                pPal->palPalEntry[i].peGreen=TerFont[pict].rgb[i].green;
                pPal->palPalEntry[i].peBlue=TerFont[pict].rgb[i].blue;
            }
            else {
                pPal->palPalEntry[i].peRed=pInfo->bmiColors[i].rgbRed;
                pPal->palPalEntry[i].peGreen=pInfo->bmiColors[i].rgbGreen;
                pPal->palPalEntry[i].peBlue=pInfo->bmiColors[i].rgbBlue;
            }
            pPal->palPalEntry[i].peFlags=0;
        }
        hPal=CreatePalette(pPal);
        OurFree(pPal);

        hOldPal=SelectPalette(hMetaDC,hPal,0);   // select new palette
        RealizePalette(hMetaDC);
    }

    // draw the picture
    StretchDIBits(hMetaDC,0,0,(int)width,(int)height,
        0,0,(int)width,(int)height,pImage,pInfo,DIB_RGB_COLORS,SRCCOPY);

    if (TerFont[pict].ColorsUsed>16 || TerFont[pict].TotalRgbs>0) {       // use default palette
        SelectPalette(hMetaDC,hOldPal,0);    // reset to old palette
        UnrealizeObject(hPal);
        DeleteObject(hPal);                  // delete the palette
    }


    hMeta=CloseMetaFile(hMetaDC);  // get the metafile handle

    // Get the metafile size
    if ( NULL==(hdr=(LPVOID)TerGetMetaFileBits(hMeta,null))) {    // convert to handler
        PrintError(w,MSG_ERR_META_ACCESS,NULL);
        return NULL;
    }
    TerFont[pict].ImageSize=hdr->mtSize*2;         // size specified in words
    if (NULL==(hMeta=TerSetMetaFileBits((LPVOID)hdr))) {  // recreate metafile handle
       PrintError(w,MSG_ERR_META_RECREATE,NULL);
       return NULL;
    }

    // set the font array
    TerFont[pict].PictType=PICT_METAFILE;
    TerFont[pict].ObjectType=OBJ_NONE;
    TerFont[pict].hMeta=hMeta;            // meta file handle
    TerFont[pict].InfoSize=0;             // info size not used by metafiles
    TerFont[pict].pImage=NULL;
    TerFont[pict].pInfo=NULL;
    TerFont[pict].bmWidth=OrigScrToTwipsX(TerFont[pict].bmWidth);   // convert to twisp for metafile
    TerFont[pict].bmHeight=OrigScrToTwipsY(TerFont[pict].bmHeight);   // convert to twisp for metafile
    TerFont[pict].hBM=0;

    return hMeta;
}


/******************************************************************************
    Set a string for find next or find previous commands.
******************************************************************************/
BOOL WINAPI _export TerSetSearchString(HWND hWnd, LPBYTE SearchFor, BOOL CaseSensitive)
{
  PTERWND w;

  if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

  lstrncpy(SearchString, SearchFor,sizeof(SearchString));

  // Dont use case sensitive search
  if (CaseSensitive) SearchFlags |= SRCH_CASE;
  else               SearchFlags = ResetUintFlag(SearchFlags,SRCH_CASE);
  SearchFlags |= SRCH_SCROLL;

  return TRUE;
}

/******************************************************************************
    TerSearchReplace2:
    Search Replace API function
******************************************************************************/
int WINAPI _export TerSearchReplace2(HWND hWnd,LPBYTE search,LPBYTE replace,UINT flags,long StartPos,long EndPos)
{
    long BufSize;
    BOOL result;

    result=TerSearchReplace(hWnd,search,replace,flags,StartPos,&EndPos,&BufSize);

    if (flags&SRCH_REPLACE) return result;

    if (result) return EndPos;
    else return -1;
}

/******************************************************************************
    TerSearchReplace:
    Search Replace API function
******************************************************************************/
BOOL WINAPI _export TerSearchReplace(HWND hWnd,LPBYTE search,LPBYTE replace,UINT flags,long StartPos,long far *EndPos,long far *BufSize)
{
    PTERWND w;
    BOOL result;
    long StartLine,SaveCurLine;
    int  StartCol,SaveCurCol;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // copy to global variables
    SearchFlags=flags;

    // Do search
    if (SearchFlags&SRCH_SEARCH) {
       if (!ParseUserString(w,search,SearchString)) return FALSE;

       AbsToRowCol(w,StartPos,&StartLine,&StartCol);
       SaveCurLine=CurLine;              // CurLine is modified by SearchDisplay
       SaveCurCol=CurCol;                // CurCol is modified by SearchDisplay
       if (SearchFlags&SRCH_BACK) result=SearchDisplay(w,SearchString,NULL,'S',StartLine,StartCol,0,0);
       else                       result=SearchDisplay(w,SearchString,NULL,'R',StartLine,StartCol,TotalLines-1,LineLen(TotalLines-1));

       SearchString[0]=0;                // reset the search string
       if (!result) return FALSE;

       (*EndPos)=RowColToAbs(w,CurLine,CurCol);
       if (!(SearchFlags&SRCH_SCROLL)) { // reset position variables
          CurLine=SaveCurLine;
          CurCol=SaveCurCol;
       }
       return TRUE;
    }

    // Do replace
    if (SearchFlags&SRCH_REPLACE) {
       LPBYTE pStr=NULL;
       BOOL AllocStr=(lstrlen(replace)>=MAX_WIDTH);
       
       // search for protected text
       if (flags&SRCH_NO_REPLACE_PROT_TEXT) {
          AbsToRowCol(w,StartPos,&HilightBegRow,&HilightEndCol);
          AbsToRowCol(w,(*EndPos)+1,&HilightEndRow,&HilightEndCol);
          HilightType=HILIGHT_CHAR;
          StretchHilight=FALSE;
          if (IsProtected(w,FALSE,TRUE)) return FALSE;
       } 
       pStr=AllocStr?MemAlloc(lstrlen(replace)+10):ReplaceString;
       if (!ParseUserString(w,replace,pStr)) return FALSE;
       if (!ReplaceTextString(w,pStr,NULL,(long) StartPos,(long)(*EndPos))) return FALSE;
       
       if (AllocStr) MemFree(pStr);

       PaintTer(w);
       return TRUE;
    }

    // Do retrieve
    if (SearchFlags&SRCH_RETRIEVE) {
       long StartLine,EndLine,l,j;
       int  StartCol,EndCol,BegIdx,EndIdx;
       LPBYTE ptr,lead;

       // convert to line/col
       AbsToRowCol(w,StartPos,&StartLine,&StartCol);
       AbsToRowCol(w,(*EndPos)+1,&EndLine,&EndCol);

       // copy text
       j=0;
       for (l=StartLine;l<=EndLine;l++) {
          if (LineLen(l)==0) continue;
          // make range to replace
          BegIdx=0;
          EndIdx=LineLen(l);
          if (l==StartLine) BegIdx=StartCol;
          if (l==EndLine) EndIdx=EndCol;

          // replace the text in the range
          ptr=pText(l);
          if (mbcs) lead=OpenLead(w,l);
          j+=TerMakeMbcs(hTerWnd,(LPBYTE)&(search[j]),&ptr[BegIdx],(mbcs?(&lead[BegIdx]):NULL),EndIdx-BegIdx);
          if (mbcs) CloseLead(w,l);
       }

       ((BYTE huge *)search)[j]=0;
       if (BufSize) (*BufSize)=j;
    }

    return TRUE;
}

/******************************************************************************
    TerSearchReplaceU:
    Search Replace API function with unicode text strings
******************************************************************************/
BOOL WINAPI _export TerSearchReplaceU(HWND hWnd,PWCHAR search,PWCHAR replace,UINT flags,long StartPos,long far *EndPos,long far *BufSize)
{
    PTERWND w;
    BOOL result;
    long StartLine,SaveCurLine;
    int  StartCol,SaveCurCol;
    WORD UcBase[MAX_WIDTH+1];
    LPWORD pUcBase;
    LPBYTE pTxt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // copy to global variables
    SearchFlags=flags;

    // Do search
    if (SearchFlags&SRCH_SEARCH) {
       int i,len;
       CrackUnicodeString(w,search,UcBase,SearchString);
       
       // check if the input string is actually a unicode string
       len=strlen(SearchString);
       for (i=0;i<len;i++) if (UcBase[i]!=0) break;
       if (i==len) pUcBase=null;  // not a unicode string
       else        pUcBase=UcBase;

       AbsToRowCol(w,StartPos,&StartLine,&StartCol);
       SaveCurLine=CurLine;              // CurLine is modified by SearchDisplay
       SaveCurCol=CurCol;                // CurCol is modified by SearchDisplay
       if (SearchFlags&SRCH_BACK) result=SearchDisplay(w,SearchString,pUcBase,'S',StartLine,StartCol,0,0);
       else                       result=SearchDisplay(w,SearchString,pUcBase,'R',StartLine,StartCol,TotalLines-1,LineLen(TotalLines-1));

       SearchString[0]=0;                // reset the search string
       if (!result) return FALSE;

       (*EndPos)=RowColToAbs(w,CurLine,CurCol);
       if (!(SearchFlags&SRCH_SCROLL)) { // reset position variables
          CurLine=SaveCurLine;
          CurCol=SaveCurCol;
       }
       return TRUE;
    }

    // Do replace
    if (SearchFlags&SRCH_REPLACE) {
       LPBYTE pStr=NULL;
       
       pUcBase=MemAlloc(sizeof(WORD)*(lstrlenW(replace)+1));
       pTxt=MemAlloc(lstrlenW(replace)+1);
       
       CrackUnicodeString(w,replace,pUcBase,pTxt);

       if (!ReplaceTextString(w,pTxt,pUcBase,(long) StartPos,(long)(*EndPos))) return FALSE;
       
       MemFree(pUcBase);
       MemFree(pTxt);

       PaintTer(w);
       return TRUE;
    }

    // Do retrieve
    if (SearchFlags&SRCH_RETRIEVE) {
       long StartLine,EndLine,l,j;
       int  i,StartCol,EndCol,BegIdx,EndIdx;
       LPBYTE ptr,lead;
       LPWORD fmt;

       // convert to line/col
       AbsToRowCol(w,StartPos,&StartLine,&StartCol);
       AbsToRowCol(w,(*EndPos)+1,&EndLine,&EndCol);

       // copy text
       j=0;
       for (l=StartLine;l<=EndLine;l++) {
          if (LineLen(l)==0) continue;
          // make range to replace
          BegIdx=0;
          EndIdx=LineLen(l);
          if (l==StartLine) BegIdx=StartCol;
          if (l==EndLine) EndIdx=EndCol;

          // replace the text in the range
          ptr=pText(l);
          fmt=OpenCfmt(w,l);
          if (mbcs) lead=OpenLead(w,l);
          
          for (i=BegIdx;i<=EndIdx;i++,j++) {
             if (TerFont[fmt[i]].UcBase) search[j]=MakeUnicode(w,TerFont[fmt[i]].UcBase,ptr[i]);  // unicode character
             else if (mbcs && lead[i]) {
                search[j]=lead[i];
                j++;
                search[j]=ptr[i];
             } 
             else search[j]=ptr[i];
          }
           
          CloseCfmt(w,l);
          if (mbcs) CloseLead(w,l);
       }

       ((BYTE huge *)search)[j]=0;
       if (BufSize) (*BufSize)=j;
    }

    return TRUE;
}

/*******************************************************************************
    TerSearchString:
    Accept a string from the user along with other parameters and search for
    the first instances of the string.
*******************************************************************************/
TerSearchString(PTERWND w)
{
    BOOL result;

    SearchFlags=SRCH_SCROLL|SRCH_SKIP_HIDDEN_TEXT;
    if (SearchFlags&SRCH_CASE) SearchFlags|=SRCH_CASE;

    result=CallDialogBox(w,"SearchParam",SearchParam,0L);
    if (!result || lstrlen(SearchString)==0) return TRUE;

    if (SearchDirection=='F') NextTextPos(w,&CurLine,&CurCol);
    
    result=SearchDisplay(w,SearchString,NULL,SearchDirection,0,0,0,0);    // search for the string and highlight it if found

    if (!result) {
        PrintError(w,MSG_NO_STRING,"Find");
        if (CurLine>=TotalLines) CurLine=TotalLines-1;
    }
    return TRUE;
}

/******************************************************************************
    TerSearchForward:
    Search for another occurrence of a previously located string in the
    forward direction.
*******************************************************************************/
TerSearchForward(PTERWND w)
{
    if (lstrlen(SearchString)==0) {      // accept a new string and search
       TerSearchString(w);
       return TRUE;
    }
    CurCol++;
    if (!SearchDisplay(w,SearchString,NULL,'F',0,0,0,0)) {
       PrintError(w,MSG_NO_STRING,MsgString[MSG_END_FORWARD_SEARCH]);
       CurCol--;
    }
    return TRUE;
}

/******************************************************************************
    TerSearchBackward:
    Search for another occurrence of a previously located string in the
    backward direction.
*******************************************************************************/
TerSearchBackward(PTERWND w)
{

    if (lstrlen(SearchString)==0) {      // accept a new string and search
       TerSearchString(w);
       return TRUE;
    }

    //CurCol--;
    if (!SearchDisplay(w,SearchString,NULL,'B',0,0,0,0)) {
       PrintError(w,MSG_NO_STRING,MsgString[MSG_END_BACKWARD_SEARCH]);
       CurCol++;
    }
    return TRUE;
}

/******************************************************************************
    TerReplaceString:
    Replace a character string with another string
*******************************************************************************/
TerReplaceString(PTERWND w)
{
    int FromLen,FromLen1,ToLen,i,SaveCol=0,MessageResult;
    int BegCol,EndCol;
    long BegRow,EndRow;
    BYTE ReplaceOpt;
    BOOL result,OneChanged=FALSE,protected;
    HCURSOR hSaveCursor=NULL;
    LPWORD fmt,ct;
    int  InitUndoRef=UndoRef;

    if (!NormalizeBlock(w)) return TRUE;

    result=CallDialogBox(w,"ReplaceParam",ReplaceParam,0L);
    if (!result || lstrlen(ReplaceString)==0) return TRUE;

    //ReleaseUndo(w);     // release any existing undos

    // get the length of the strings
    FromLen=StrLenSplit(w,ReplaceString);    // These strings can be mbcs
    ToLen=StrLenSplit(w,ReplaceWith);

    if (ReplaceBlock) ReplaceOpt='R';        // set a range for the operation
    else              ReplaceOpt='E';        // search the entire file

    if (ReplaceOpt=='E') {
       BegRow=0;
       BegCol=0;
       EndRow=TotalLines-1;
       EndCol=LineLen(EndRow)-1;
    }
    else { 
       BegRow=HilightBegRow;                    // set begin and end range
       if (HilightType==HILIGHT_CHAR) BegCol=HilightBegCol;
       else                           BegCol=0;
       EndRow=HilightEndRow;
       if (HilightType==HILIGHT_CHAR) EndCol=HilightEndCol-1;
       else                           EndCol=LineLen(HilightEndRow)-1;
    }
    if (EndCol<0) EndCol=0;
                                             // show hour glass while reading
    SearchFlags=SRCH_CASE|SRCH_SKIP_HIDDEN_TEXT;

    if (ReplaceVerify)                         SearchFlags|=SRCH_SCROLL;
    else  if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);

    FIND_NEXT:
    result=SearchDisplay(w,ReplaceString,NULL,ReplaceOpt,BegRow,BegCol,EndRow,EndCol);

    if (!result) {                           // string not found
        if (!ReplaceVerify && hSaveCursor) SetCursor(hSaveCursor);

        if (CurLine>=TotalLines) CurLine=TotalLines-1;
        if (!OneChanged) {
           PrintError(w,MSG_NO_STRING,"");
           PaintTer(w);
        }
        if (OneChanged) {                    // highlight the last replaced string
           CurCol=SaveCol;
           TerPosLine(w,CurLine+1);

           if (ToLen>0) {
              HilightType=HILIGHT_CHAR;         // hilight the replaced character
              HilightBegRow=HilightEndRow=CurLine;
              HilightBegCol=CurCol;
              HilightEndCol=CurCol+ToLen;
              if (HilightEndCol>LineLen(CurLine)) {
                 long BegPos=RowColToAbs(w,CurLine,CurCol);
                 AbsToRowCol(w,BegPos+ToLen,&HilightEndRow,&HilightEndCol);
              }
           }
           PaintTer(w);
        }
        return TRUE;
    }

    // check for protected text
    HilightType=HILIGHT_CHAR;            // hilight the character to be replaced
    HilightBegRow=HilightEndRow=CurLine;
    HilightBegCol=CurCol;
    HilightEndCol=CurCol+FromLen;
    if (HilightEndCol>LineLen(CurLine)) {
       long BegPos=RowColToAbs(w,CurLine,CurCol);
       AbsToRowCol(w,BegPos+FromLen,&HilightEndRow,&HilightEndCol);
    }

    protected=IsProtected(w,FALSE,TRUE);
    HilightType=HILIGHT_OFF;
    if (protected) {
       SaveCol=CurCol;
       CurCol=CurCol+FromLen;
       BegRow=HilightEndRow;
       BegCol=HilightEndCol;
       if (ReplaceOpt=='E') ReplaceOpt='F';     // forward search/replace
       goto FIND_NEXT;
    }


    if (ReplaceVerify) {
        HilightType=HILIGHT_CHAR;            // hilight the character to be replaced
        HilightBegRow=HilightEndRow=CurLine;
        HilightBegCol=CurCol;
        HilightEndCol=CurCol+FromLen;
        if (HilightEndCol>LineLen(CurLine)) {
           long BegPos=RowColToAbs(w,CurLine,CurCol);
           AbsToRowCol(w,BegPos+FromLen,&HilightEndRow,&HilightEndCol);
        }

        TerPosLine(w,CurLine+1);
        PaintTer(w);

        if (IDNO==(MessageResult=MessageBox(hTerWnd,MsgString[MSG_REPLACE_STRING],"",MB_YESNOCANCEL))) {
           SaveCol=CurCol;
           CurCol=CurCol+FromLen;
           if (CurCol>LineWidth) CurCol=LineWidth;
           BegRow=CurLine;
           BegCol=CurCol;
           if (ReplaceOpt=='E') ReplaceOpt='F';     // forward search/replace
           goto FIND_NEXT;
        }
        else if (MessageResult==IDCANCEL) return TRUE;
    }
    OneChanged=TRUE;                         // atleast one string found to replace

    // find the length of the original string on the current line
    if ((CurCol+FromLen)>LineLen(CurLine) && CurLine<(TotalLines-1)) {
         int DeleteLen;

         FromLen1=LineLen(CurLine)-CurCol;  // length to be deleted from the current line

         // delete the portion of the string on the following lines
         DeleteLen=FromLen-FromLen1;  // characters to delete from the following lines
         while (DeleteLen>0 && (CurLine+1)<TotalLines) {
            if (DeleteLen>=LineLen(CurLine+1)) {
               DeleteLen-=LineLen(CurLine+1);
               
               // record for undo
               UndoRef=InitUndoRef;  // tie the undo operations
               if (LineLen(CurLine+1)>0) SaveUndo(w,CurLine+1,0,CurLine+1,LineLen(CurLine+1)-1,'D');  // save deleted text

               MoveLineArrays(w,CurLine+1,1,'D');  // delete this line
            }
            else {                                 // last line segment to delete
               // record for undo
               UndoRef=InitUndoRef;  // tie the undo operations
               SaveUndo(w,CurLine+1,0,CurLine+1,DeleteLen-1,'D');  // save deleted text

               MoveLineData(w,CurLine+1,0,DeleteLen,'D'); // delete
               break;
            }
         }
    }
    else FromLen1=FromLen;

    // record for undo
    UndoRef=InitUndoRef;  // tie the undo operations
    SaveUndo(w,CurLine,CurCol,CurLine,CurCol+FromLen1-1,'D');  // save deleted text


    // insert the replacement string
    if (ToLen>FromLen1) MoveLineData(w,CurLine,CurCol+FromLen1-1,ToLen-FromLen1,'A'); // insert elements
    if (ToLen<FromLen1) MoveLineData(w,CurLine,CurCol+ToLen,FromLen1-ToLen,'D'); // delete

    // apply the replacement text
    SetLineText(w,ReplaceWith,CurLine,CurCol);  // overwrite the existing text with new text

    OpenCharInfo(w,CurLine,&fmt,&ct);
    for (i=FromLen1;i<ToLen;i++) {
       fmt[CurCol+i]=fmt[CurCol+FromLen1-1];
       ct[CurCol+i]=0;
    }
    CloseCharInfo(w,CurLine);

    if (LineLen(CurLine)>LineWidth) LineAlloc(w,CurLine,LineLen(CurLine),LineWidth);

    // record for undo
    UndoRef=InitUndoRef;  // tie the undo operations
    SaveUndo(w,CurLine,CurCol,CurLine,CurCol+ToLen-1,'I');  // record the inserted text

    TerArg.modified++;

    if (ReplaceOpt=='E') ReplaceOpt='F';     // forward search/replace
    SaveCol=CurCol;
    CurCol=CurCol+ToLen;
    if (CurCol>=LineWidth) CurCol=LineWidth;
    BegRow=CurLine;
    BegCol=CurCol;
    if (ReplaceOpt=='R' && BegRow==EndRow && BegCol>EndCol) {     // range exhausted
       PaintTer(w);
       return TRUE;
    }

    goto FIND_NEXT;
}

/******************************************************************************
    SearchDisplay:
    Locate the string (argument #1) and display if highlight it if requested.
    The 'opt' field is interpreted as following:
       opt = 'E'    : Search the entire file.
       opt = 'F'    : Search forward from file current file position to the end of file
       opt = 'B'    : Search backward from current file position to the end of file
       opt = 'R'    : Forward search in the specified range: StartLine through End col.
       opt = 'S'    : Backward search in the specified range: StartLine through End col.


       The result is TRUE is the string is located, otherwise it is FALSE.
*******************************************************************************/
BOOL SearchDisplay(PTERWND w,LPBYTE SrchString,LPWORD UcBaseString, BYTE opt,long StartLine,int StartCol,long EndLine,int EndCol)
{
    int  i,TempLen,LimitCol;
    long l,ln;
    BYTE line[MAX_WIDTH*2],WordDelims[60];
    LPBYTE text,LeadText;
    int  StringPos,SearchTextLen,SearchLineLen,PrevLen,font1,font2;
    BOOL   SkipHiddenText=(/*!ShowHiddenText &&*/ SearchFlags&SRCH_SKIP_HIDDEN_TEXT);
    BOOL   SkipHdrFtr=(!ViewPageHdrFtr && SearchFlags&SRCH_SKIP_HIDDEN_TEXT);
    BOOL   found=FALSE,IsWord;
    WORD   pUcBase[MAX_WIDTH*2];

    if ((SearchTextLen=lstrlen(SrchString))==0) return FALSE;

    // split the mbcs text if requested
    SearchTextLen=AllocDB(w,SrchString,&text,&LeadText);

    // copy the search text
    if (!(SearchFlags&SRCH_CASE)) {
       if (UcBaseString) StrLwrDBU(w,text,LeadText,UcBaseString);
       else              StrLwrDB(w,text,LeadText);
    }

    // build a list of word delimiters
    lstrcpy(WordDelims,BreakChars);
    lstrcat(WordDelims," .,:<>?;'!@#$%^&*()-+=|\\/~01234567890");  // add other delimiters
    TempLen=lstrlen(WordDelims);
    WordDelims[TempLen]=TAB;                 // add the tab character to the list
    WordDelims[TempLen+1]=ParaChar;          // add the para character to the list
    WordDelims[TempLen+2]=CellChar;          // add the cell character to the list
    WordDelims[TempLen+3]=LINE_CHAR;         // add the line character to the list
    WordDelims[TempLen+4]=0;

    //****************** Set Search Limits ********************************
    if (opt=='E' || opt=='F' || opt=='B') {
        StartLine=CurLine;                      // begin search location
        StartCol=CurCol;
    }
    if (opt=='E') {                       // search the entire file
        StartLine=0;
        StartCol=0;
        opt='F';
    }
    if (opt=='F') {                      // forward search
       EndLine=TotalLines-1;
       EndCol=LineLen(EndLine)-1;
    }
    if (opt=='B') {                      // backward search
       EndLine=0;
       EndCol=0;
    }
    if (opt=='R') opt = 'F';             // forward search in the specified range
    if (opt=='S') opt = 'B';             // backward search in the specified range

    if (StartCol>=LineWidth) StartCol=LineWidth-1;
    if (StartCol<0) StartCol=0;
    if (EndCol>=LineWidth) EndCol=LineWidth-1;
    if (EndCol<0) EndCol=0;

    //******************* Do Forward Search *********************************
    if (opt=='F') {
        for (l=StartLine;l<=EndLine;l++) {
            if (StartCol>=LineLen(l)) {
               StartCol=0;
               continue;
            }
            if (SkipHdrFtr && PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;

            // make a text line by combining current and the next line
            GetLineData(w,l,0,LineLen(l),line,LineLead,LineCfmt,NULL);

            SearchLineLen=LineLen(l);

            // append a part of the next line
            ln=l+1;
            while (ln<=EndLine && SearchLineLen<(LineLen(l)+SearchTextLen)) {
               if (LineLen(ln)>0) {
                  PrevLen=SearchLineLen;
                  GetLineData(w,ln,0,LineLen(ln),&line[PrevLen],&LineLead[PrevLen],&LineCfmt[PrevLen],NULL);

                  line[LineLen(l)+SearchTextLen]=0; // set max len
                  SearchLineLen=lstrlen(line);      // current len
               }
               ln++;
            }
            if (SearchLineLen==(LineLen(l)+SearchTextLen)) SearchLineLen--;  // last character used only for space (full word search)

            // In unicode search mode, convert extended characters to unicode
            if (UcBaseString!=null) GetSearchUcBase(w,line,SearchLineLen,pUcBase,LineCfmt,LineLead, SearchTextLen, text, UcBaseString);

            if (!(SearchFlags&SRCH_CASE)) StrLwrDB(w,line,LineLead);

            TempLen=SearchLineLen-StartCol;
            if (l==EndLine) LimitCol=EndCol-StartCol-SearchTextLen+1; // set limit on the line length to scan
            else            LimitCol=SearchLineLen-SearchTextLen-StartCol;

            // search the line
            StringPos=FarStringIndex(&line[StartCol],text); // locate the text

            if (StringPos<TempLen && StringPos<=LimitCol) {  // text located
                // check for the lead bytes
                if (mbcs) {
                   for (i=0;i<SearchTextLen;i++) if (LineLead[StartCol+StringPos+i]!=LeadText[i]) break;
                   if (i<SearchTextLen) {        // no match
                     StartCol=0;
                     continue;
                   }
                }

                // Check for unicode base string
                if (UcBaseString) {
                   for (i=0;i<SearchTextLen;i++) {
                      if (UcBaseString[i]!=pUcBase[StartCol+StringPos+i]) break;
                   }
                   if (i<SearchTextLen) {        // no match
                     StartCol=0;
                     continue;
                   }
                } 

                // check for whole word condition
                if (SearchFlags&SRCH_WORD) {                  // accept only whole words
                   IsWord=TRUE;
                   if ((StartCol+StringPos)>0 && lstrchr(WordDelims,line[StartCol+StringPos-1])==NULL) {
                      font1=LineCfmt[StartCol+StringPos-1];
                      font2=LineCfmt[StartCol+StringPos];
                      if (TerFont[font1].FieldId==TerFont[font2].FieldId) IsWord=FALSE;    // check for delimiter before the text
                   }
                   if (StringPos<LimitCol && lstrchr(WordDelims,line[StartCol+StringPos+SearchTextLen])==NULL) {
                      font1=LineCfmt[StartCol+StringPos+SearchTextLen-1];
                      font2=LineCfmt[StartCol+StringPos+SearchTextLen];
                      if (TerFont[font1].FieldId==TerFont[font2].FieldId) IsWord=FALSE;    // check for delimiter before the text
                   }
                   //if (  (StringPos>0 && lstrchr(WordDelims,line[StartCol+StringPos-1])==NULL)    // check for delimiter before the text
                   //   || (StringPos<LimitCol && lstrchr(WordDelims,line[StartCol+StringPos+SearchTextLen])==NULL) ){ // check for the delimiter after the text
                   if (!IsWord) {   
                      if ((StringPos+SearchTextLen)<LineLen(l)) {
                         StartCol=StartCol+StringPos+SearchTextLen;
                         l--;      // scan the current
                      }
                      continue;
                   }
                }

                // check for hidden text
                if (SkipHiddenText) {            // skip over any hidden text
                   //for (i=0;i<SearchTextLen;i++) if (TerFont[LineCfmt[StartCol+StringPos+i]].style&HIDDEN) break;          // look for the hidden text
                   for (i=0;i<SearchTextLen;i++) if (HiddenText(w,LineCfmt[StartCol+StringPos+i])) break;          // look for the hidden text
                   if (i<SearchTextLen) {                   // hidden text found, find the end of the hidden text
                      //for (;i<SearchTextLen;i++) if (!(TerFont[LineCfmt[StartCol+StringPos+i]].style&HIDDEN)) break;      // look for the end of the hidden text
                      for (;i<SearchTextLen;i++) if (!HiddenText(w,LineCfmt[StartCol+StringPos+i])) break;      // look for the end of the hidden text
                      StartCol=StartCol+StringPos+i;
                      while (l<TotalLines && StartCol>=LineLen(l)) {
                         StartCol-=LineLen(l);
                         l++;
                      }
                      l--;
                      continue;
                   }
                }

                // word found
                CurCol=StartCol+StringPos;                   // set column and line of the located text
                CurLine=l;
                if (SearchFlags&SRCH_SCROLL) {
                    HilightType=HILIGHT_CHAR;                // hilight the located text
                    StretchHilight=FALSE;
                    HilightBegRow=HilightEndRow=CurLine;
                    HilightBegCol=CurCol;
                    HilightEndCol=CurCol+SearchTextLen;
                    if (HilightEndCol>LineLen(l)) {
                       long BegPos=RowColToAbs(w,CurLine,CurCol);
                       AbsToRowCol(w,BegPos+SearchTextLen,&HilightEndRow,&HilightEndCol);
                    }
                    // disengage caret if search text located in hdr/ftr area
                    if (PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR && ViewPageHdrFtr && !EditPageHdrFtr && CaretEngaged) 
                       DisengageCaret(w);

                    TerPosLine(w,CurLine+1);
                    PaintTer(w);
                }
                found=TRUE;
                goto END;
            }
            StartCol=0;
        }
    }

    //******************* Do Backward Search *********************************
    if (opt=='B') {
        for (l=StartLine;l>=EndLine;l--) {
            if (LineLen(l)==0) continue;
            if (SkipHdrFtr && PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;

            GetLineData(w,l,0,LineLen(l),line,LineLead,LineCfmt,NULL);
            SearchLineLen=LineLen(l);

            // append a part of the next line
            ln=l+1;
            while (ln<=StartLine && SearchLineLen<(LineLen(l)+SearchTextLen)) {
               if (LineLen(ln)>0) {
                  PrevLen=SearchLineLen;
                  GetLineData(w,ln,0,LineLen(ln),&line[PrevLen],&LineLead[PrevLen],&LineCfmt[PrevLen],NULL);
                  line[LineLen(l)+SearchTextLen]=0; // set max len
                  SearchLineLen=lstrlen(line);      // current len
               }
               ln++;
            }
            if (SearchLineLen==(LineLen(l)+SearchTextLen)) SearchLineLen--;  // last character used only for space (full word search)

            // In unicode search mode, convert extended characters to unicode
            if (UcBaseString!=null) GetSearchUcBase(w,line,SearchLineLen,pUcBase,LineCfmt,LineLead, SearchTextLen, text, UcBaseString);

            if (!(SearchFlags&SRCH_CASE)) lstrlwr(w,line);

            if (l!=StartLine) StartCol=SearchLineLen/*-1*/;
            if (StartCol<0) continue;
            TempLen=StartCol/*+1*/;
            if (l==EndLine) LimitCol=EndCol;                 // set limit on the line length to scan
            else            LimitCol=0;

            StringPos=FarStringRevIndex(line,text,TempLen);

            if (StringPos!=TempLen && StringPos>=LimitCol) { // text located
                // check for the lead bytes
                if (mbcs) {
                   for (i=0;i<SearchTextLen;i++) if (LineLead[StringPos+i]!=LeadText[i]) break;
                   if (i<SearchTextLen) continue;        // no match
                }

                // Check for unicode base string
                if (UcBaseString) {
                   for (i=0;i<SearchTextLen;i++) {
                      if (UcBaseString[i]!=pUcBase[StringPos+i]) break;
                   }
                   if (i<SearchTextLen) continue;        // no match
                } 

                
                // check for whole word condition
                if (SearchFlags&SRCH_WORD) {                  // accept only whole words
                   IsWord=TRUE;
                   if (StringPos>LimitCol && lstrchr(WordDelims,line[StringPos-1])==NULL) {
                      font1=LineCfmt[StringPos-1];
                      font2=LineCfmt[StringPos];
                      if (TerFont[font1].FieldId==TerFont[font2].FieldId) IsWord=FALSE;    // check for delimiter before the text
                   }
                   if ((StringPos+SearchTextLen)<=StartCol && lstrchr(WordDelims,line[StringPos+SearchTextLen])==NULL) {
                      font1=LineCfmt[StringPos+SearchTextLen-1];
                      font2=LineCfmt[StringPos+SearchTextLen];
                      if (TerFont[font1].FieldId==TerFont[font2].FieldId) IsWord=FALSE;    // check for delimiter before the text
                   }
                   //if (StringPos>LimitCol && lstrchr(WordDelims,line[StringPos-1])==NULL) continue;
                   //if ((StringPos+SearchTextLen)<=StartCol && lstrchr(WordDelims,line[StringPos+SearchTextLen])==NULL) continue;
                   if (!IsWord) continue;
                }

                // text found
                CurCol=StringPos;                            // set column and line of the located text
                CurLine=l;
                if (SearchFlags&SRCH_SCROLL) {
                    HilightType=HILIGHT_CHAR;                // hilight the located text
                    StretchHilight=FALSE;
                    HilightBegRow=HilightEndRow=CurLine;
                    HilightBegCol=CurCol;
                    HilightEndCol=CurCol+SearchTextLen;
                    if (HilightEndCol>LineLen(l)) {
                       long BegPos=RowColToAbs(w,CurLine,CurCol);
                       AbsToRowCol(w,BegPos+SearchTextLen,&HilightEndRow,&HilightEndCol);
                    }
                    TerPosLine(w,CurLine+1);
                    PaintTer(w);
                }
                found=TRUE;
                goto END;
            }

        }
    }

    END:
    if (text) OurFree(text);
    if (LeadText) OurFree(LeadText);

    return found;
}

/*****************************************************************************
    GetSearchUcBase:
    Get the UcBase array for the text lines int search function.
******************************************************************************/
BOOL GetSearchUcBase(PTERWND w,LPBYTE line,int SearchLineLen, LPWORD pUcBase,LPWORD pFmt,LPBYTE pLead, int SrchTextLen, LPBYTE SrchText, LPWORD UcBaseString)
{
    int i,j,CharSet,CodePage,font;
    WORD UText[3],UcBase;
    BYTE txt;

    for (i=0;i<SearchLineLen;i++) {
       font=pFmt[i];
       UcBase=TerFont[font].UcBase;
       pUcBase[i]=UcBase;             // save uc base
       if (UcBase>0) continue;        // already a unicode character

       if (pLead[i]!=0 || line[i]<128) continue;
       for (j=0;j<SrchTextLen;j++) if (SrchText[j]==line[i]) break;
       if (j<SrchTextLen) {   // no need to convert this character because the search strings also has it in extended ASCII form
          if (UcBaseString[j]==1) pUcBase[i]=1;  // use the same flag
          continue;
       } 

       // convert this extended character to unicode
       CharSet=TerFont[font].CharSet;
       if (CharSet>2) {
         IsMbcsCharSet(w,CharSet,&CodePage);  // get the code page
         if (MultiByteToWideChar(CodePage,0,&(line[i]),1,UText,1)==1) {
            CrackUnicode(w,UText[0],&UcBase,&txt);
            pUcBase[i]=UcBase;
            line[i]=txt;
         }
       }
    }
    
    return true; 
} 

/*****************************************************************************
    ReplaceTextString
    Replace the text within the specified absolute positions (inclusive) by
    the specified text
******************************************************************************/
BOOL ReplaceTextString(PTERWND w,LPBYTE replace,LPWORD pUcBase, long StartPos,long EndPos)
{
    long FromLen,RepLen,MidLen,StartLine,EndLine,MidLine,l;
    int  StartCol,EndCol,MidCol,i,BegIdx,EndIdx,RepIdx,SaveCurCol;
    long SaveCurLine,SaveBeginLine,SaveCurRow;
    LPBYTE ptr,lead,text=NULL,LeadText=NULL;
    LPWORD fmt;
    int  SaveUndoRef=UndoRef,SaveFontId;
    DWORD SaveFlags;

    // check out of bound
    AbsToRowCol(w,StartPos,&StartLine,&StartCol);
    StartPos=RowColToAbs(w,StartLine,StartCol);
    EndPos++;                       // go past the last included column
    AbsToRowCol(w,EndPos,&EndLine,&EndCol);
    EndPos=RowColToAbs(w,EndLine,EndCol);

    FromLen=EndPos-StartPos;       // length to replace
    if (FromLen<0) return FALSE;

    // split mbcs if necessary
    if (pUcBase) {                 // do not consider lead text
       RepLen=lstrlen(replace);
       text=OurAlloc(RepLen+1);
       lstrcpy(text,replace);
    }
    else RepLen=AllocDB(w,replace,&text,&LeadText);


    if (FromLen<RepLen) MidLen=FromLen;   // length to copy directly
    else                MidLen=RepLen;

    AbsToRowCol(w,StartPos+MidLen,&MidLine,&MidCol);
    RepIdx=0;                     // next index into replacement string
    TerArg.modified++;

    // replace the smaller text directly
    if (MidLen>0) {
       SaveUndo(w,StartLine,StartCol,MidLine,MidCol,'D');  // save deleted text
       
       for (l=StartLine;l<=MidLine;l++) {
          if (LineLen(l)==0) continue;
          // make range to replace
          BegIdx=0;
          EndIdx=LineLen(l);
          if (l==StartLine) BegIdx=StartCol;
          if (l==MidLine) EndIdx=MidCol;

          // replace the text in the range
          ptr=pText(l);
          fmt=OpenCfmt(w,l);
          if (LeadText) lead=OpenLead(w,l);
          
          for (i=BegIdx;i<EndIdx;i++,RepIdx++) {
             if (RepIdx<RepLen) {
                ptr[i]=text[RepIdx];
                if (LeadText) lead[i]=LeadText[RepIdx];

                fmt[i]=SetUniFont(w,fmt[i],(WORD)(pUcBase?pUcBase[RepIdx]:0));
             }
          }
          
          if (LeadText) CloseLead(w,l);
          CloseCfmt(w,l);
       }
       
       UndoRef=SaveUndoRef;   // to connect the next undo with the previous undo
       SaveUndo(w,StartLine,StartCol,MidLine,MidCol,'I');  // save undo for the inserted text
    }

    if (FromLen==RepLen) goto END;// exact length replacement

    // delete any additional text or insert remaining text
    if (FromLen>RepLen) {            // delete additional text
        HilightType=HILIGHT_CHAR;
        HilightBegRow=MidLine;
        HilightBegCol=MidCol;
        HilightEndRow=EndLine;
        HilightEndCol=EndCol;
        
        UndoRef=SaveUndoRef;   // to connect the next undo with the previous undo
        
        SaveFlags=TerOpFlags2;
        TerOpFlags2|=TOFLAG2_NO_NORMALIZE;  // do not adjust the block
        DeleteCharBlock(w,FALSE,FALSE);    // suppress painting
        TerOpFlags2=SaveFlags;             // restore the flags
        
        HilightType=HILIGHT_OFF;
    }
    else {                           // insert remaining text
        SaveCurLine=CurLine;         // save the text position
        SaveCurRow=CurRow;
        SaveCurCol=CurCol;
        SaveBeginLine=BeginLine;
        SaveFontId=InputFontId;

        CurLine=MidLine;
        CurCol=MidCol;
        if (!ImeEnabled) InputFontId=GetPrevCfmt(w,MidLine,MidCol);

        UndoRef=SaveUndoRef;   // to connect the next undo with the previous undo
        
        InsertBuffer(w,&text[RepIdx],
                       (pUcBase?(&pUcBase[RepIdx]):NULL),
                       (LeadText?(&LeadText[RepIdx]):NULL),
                       NULL,NULL,FALSE);
        
        CurLine=SaveCurLine;         // restore the text position
        CurRow=SaveCurRow;
        CurCol=SaveCurCol;
        InputFontId=SaveFontId;
        BeginLine=SaveBeginLine;
    }

    END:

    if (text)     OurFree(text);
    if (LeadText) OurFree(LeadText);

    return TRUE;
}

/*****************************************************************************
    ParseUserString:
    Parse the user string
******************************************************************************/
int ParseUserString(PTERWND w, LPBYTE src, LPBYTE dest)
{
    int len,i,j;

    len=lstrlen(src);

    dest[0]=0;                    // initialize the destination string
    if (len==0) return TRUE;

    j=0;
    for (i=0;i<len;i++,j++) {
       if ((!mbcs) && src[i]=='^' && i<(len-1)) {
          if      (src[i+1]=='p') dest[j]=ParaChar;
          else if (src[i+1]=='t') dest[j]=TAB;
          else if (src[i+1]=='^') dest[j]='^';
          else if (src[i+1]=='+') dest[j]=(BYTE)151;
          else if (src[i+1]=='-') dest[j]=(BYTE)150;
          else if (src[i+1]=='m') dest[j]=PAGE_CHAR;
          else if (src[i+1]=='b') dest[j]=SECT_CHAR;
          else {                      // treat this '^' as regular character
             dest[j]=src[i];
             continue;
          }
          i++;                        // advance i over special '^'
       }
       else if (src[i]==0xd && (i+1)<len && src[i+1]==0xa) {
          dest[j]=ParaChar;
          i++;                // advance i over 0xa
       } 
       else dest[j]=src[i];// copy regular current character
    }
    dest[j]=0;

    return TRUE;
}

/*****************************************************************************
    TerTextExtentX:
    Get the width of a text string for the selected font
******************************************************************************/
int TerTextExtentX(HDC hDC,LPBYTE string,int len)
{
    SIZE size;

    GetTextExtentPoint(hDC,string,len,&size);
    return size.cx;
}

/*****************************************************************************
    TerSetEnhMetaFileBits:
    A common routine for 16 and 32 bit windows to set the enh metafile bits
******************************************************************************/
HENHMETAFILE TerSetEnhMetaFileBits(LPBYTE pMem)
{
    HENHMETAFILE hMeta=NULL;

    #if defined(WIN32)
       ENHMETAHEADER *emh;
       UINT size;

       // Lock the buffer and calculate the size
       emh=(ENHMETAHEADER*)pMem;
       size=emh->nBytes;
       hMeta=SetEnhMetaFileBits(size,(LPBYTE)emh);
       if (NULL==(hMeta)) return NULL;
       OurFree(pMem);
    #endif
    return hMeta;
}

/*****************************************************************************
    TerGetEnhMetaFileBits:
    A common routine for 16 and 32 bit windows to retrieve the enh metafile data
******************************************************************************/
LPBYTE TerGetEnhMetaFileBits(HENHMETAFILE hMeta)
{
    LPBYTE pMem=NULL;

    #if defined(WIN32)
       // allocate the memory to receive the data
       UINT size=GetEnhMetaFileBits(hMeta,0,NULL);   // get the size of the buffer to allocate
       if (size==0) return NULL;

       if (NULL==(pMem=OurAlloc(size+1))) return NULL;

       // retrieve the metafile bits
       if (size!=GetEnhMetaFileBits(hMeta,size,pMem)) return NULL;

       // delete the input metafile handle
       DeleteEnhMetaFile(hMeta);
    #endif

    return pMem;
}


/*****************************************************************************
    TerSetMetaFileBits:
    A common routine for 16 and 32 bit windows to set the metafile bits
******************************************************************************/
HMETAFILE TerSetMetaFileBits(LPBYTE pMem)
{
    HMETAFILE hMeta;

    #if defined(WIN32)
      METAHEADER far *hdr;         // metafile header
      UINT size;

      // Lock the buffer and calculate the size
      hdr=(METAHEADER far *)pMem;
      size=hdr->mtSize*2;       // size specified in words
      if (NULL==(hMeta=SetMetaFileBitsEx(size,(LPBYTE)hdr))) return NULL;

      OurFree(pMem);
    #else
      DWORD temp=GlobalHandle((UINT)(((DWORD)pMem)>>16));
      HGLOBAL hMem=(HGLOBAL)LOWORD(temp);

      if (NULL==(hMeta=SetMetaFileBits(hMem))) return NULL;   // Windows owns this handle after the call
    #endif

    return hMeta;
}

/*****************************************************************************
    TerGetMetaFileBits:
    A common routine for 16 and 32 bit windows to retrieve the metafile data
******************************************************************************/
LPBYTE TerGetMetaFileBits(HMETAFILE hMeta, LPLONG pSize)
{
    LPBYTE pMem;

    // allocate the memory to receive the data
    UINT size=GetMetaFileBitsEx(hMeta,0,NULL);   // get the size of the buffer to allocate
    
    if (size==0) return NULL;

    if (NULL==(pMem=OurAlloc(size+1))) return NULL;

    // retrieve the metafile bits
    if (size!=GetMetaFileBitsEx(hMeta,size,pMem)) return NULL;

    if (pSize) (*pSize)=(long)size;

    // delete the input metafile handle
    DeleteMetaFile(hMeta);

    return pMem;
}

/*****************************************************************************
    lMulDiv:
    Multiply, divide and round the long numbers.
******************************************************************************/
long lMulDiv(long num1, long num2, long num3)
{
    double result;

    result=(double)num1*num2/num3;

    return (long)(result+.5);       // round it
}

/*****************************************************************************
    Make a copy of a string of the specified length:
    copy one string to another to the specified length. Set len to -1 to copy
    the entire string.
    The pointer returned by this function should be freed using the MemFree function.
******************************************************************************/
LPBYTE MakeCopy(LPBYTE string, int len)
{
    LPBYTE pMem;

    if (len<0) len=lstrlen(string);
    
    pMem=MemAlloc(len+1);

    lstrncpy(pMem, string,len);
    
    return pMem; 
}
 
/*****************************************************************************
    lstrncpy:
    copy one string to another to the specified length.
******************************************************************************/
void lstrncpy(LPBYTE string1, LPBYTE string2, int len)
{
    int i;

    for (i=0;i<len;i++) {
       string1[i]=string2[i];
       if (string1[i]==0) break;       // string2 ended
    }
    string1[i]=0;

    return;
}

/*****************************************************************************
    strncmpi:
    copy one string to another to the specified length.
******************************************************************************/
int strncmpi(LPBYTE string1, LPBYTE string2, int len)
{
    BYTE str1[MAX_WIDTH+1],str2[MAX_WIDTH+1];

    lstrncpy(str1,string1,len);
    lstrncpy(str2,string2,len);

    return strcmpi(str1,str2);
}


/*****************************************************************************
    StripSlashes:
    Strip slash so that only one is left for 4 slashes in the string.
    Also ignore any text after the closing double quotes
******************************************************************************/
void StripSlashes(PTERWND w, LPBYTE in, LPBYTE out)
{
    int len=lstrlen(in),slashes=0;
    int i,j;
    BOOL QuoteFound=FALSE;

    for (i=j=0;i<len;i++) {
       if (in[i]=='\\') {
          slashes++;
          if ((slashes%2)!=1) continue;
       }
       else slashes=0;    // reset the consequtive slash counter

       if (in[i]=='"') {
          if (QuoteFound) break;  // disacrd text after the closing quotes
          QuoteFound=TRUE;
          continue;
       }
       out[j]=in[i];
       j++;
    }
    out[j]=0;

    StrTrim(out);
}

/*****************************************************************************
    ExtractQuotedText:
    If quoted text found, extract it, otherwise return the original string
    unchanged.
******************************************************************************/
LPBYTE ExtractQuotedText(LPBYTE str)
{
   LPBYTE ptr;
   BYTE temp[MAX_WIDTH+1];

   if (NULL==(ptr=strchr(str,QUOTE))) return str;  // no quotes found

   strcpy(temp,&(ptr[1]));  // copy after the quote

   if (NULL!=(ptr=strchr(temp,QUOTE))) (*ptr)=0;  // truncate

   strcpy(str,temp);

   return str;
}

/*****************************************************************************
    StrQuote:
    Surround a string in quotes
******************************************************************************/
StrQuote(LPBYTE string)
{
    int len=lstrlen(string);
    int i;

    for (i=len;i>=0;i--) string[i+1]=string[i]; // make room for the beginning quote
    len=lstrlen(string);
    string[0]=string[len]=QUOTE;
    string[len+1]=0;

    return TRUE;
}

/*****************************************************************************
    AddChar:
    Append a character to the string
******************************************************************************/
LPBYTE AddChar(LPBYTE str, BYTE chr)
{
    int len;

    len=lstrlen(str);
    str[len]=chr;
    str[len+1]=0;

    return str;
}
        
/*****************************************************************************
    AddSlashes:
    Replace one slash by the specified number of slashes
******************************************************************************/
void AddSlashes(PTERWND w, LPBYTE in, LPBYTE out, int count)
{
    int len=lstrlen(in);
    int i,j,k;

    for (i=j=0;i<=len;i++) {        // copy the NULL terminator also
       out[j]=in[i];
       j++;

       if (in[i]=='\\') {     // copy a slash three more times
         for (k=0;k<(count-1);k++) {
            out[j]=in[i];
            j++;
         }
       }
    }
}

/*****************************************************************************
    hstrlen:
    Calculate the length of a huge string.
******************************************************************************/
long hstrlen(BYTE huge *string)
{
    long l;

    if (!string) return 0;

    if (Win32) return strlen(string);

    for (l=0;string[l]!=0;l++);

    return l;
}


/*****************************************************************************
    StrTrim:
    Remove spaces from the left and right of a NULL terminated string.
******************************************************************************/
void StrTrim(LPBYTE string)
{
    rTrim(string);lTrim(string);
}

/******************************************************************************
    rTrim:
    Remove spaces on the right of a string.
*****************************************************************************/
void rTrim(LPBYTE string)
{
    int i,TempLen;

    TempLen=lstrlen(string);
    for (i=TempLen-1;i>=0 && string[i] == ' ';i--);
    string[i+1] = '\0';
}

/******************************************************************************
    lTrim:
    Trim initial spaces from the string.
*******************************************************************************/
void lTrim(LPBYTE string)
{
    int i,TempLen,BeginPoint;BYTE TempStr[MAX_WIDTH+1];

    TempLen=lstrlen(string);
    for (i=0;i < TempLen && string[i] == ' ';i++);
    BeginPoint=i;
    for (i=BeginPoint;i<TempLen;i++) TempStr[i-BeginPoint]=string[i];
    TempStr[TempLen-BeginPoint] = '\0';
    lstrcpy(string,TempStr);
}

/******************************************************************************
     StuffListBox:
     This function adds a string and a numeric value to the list box.  The
     new entry is appended to the list box.
*******************************************************************************/
BOOL StuffListBox(HWND hDlg,int ControlId,LPBYTE string, DWORD val)
{
    int index;

    if (LB_ERR==(index=(int)SendDlgItemMessage(hDlg,ControlId,LB_ADDSTRING,0,(LPARAM)string))) {
       MessageBox(NULL,MsgString[MSG_ERR_ADDING_STRING],NULL,MB_OK);
       return FALSE;
    }
    if (LB_ERR==(index=(int)SendDlgItemMessage(hDlg,ControlId,LB_SETITEMDATA,index,(LPARAM)val))) {
       MessageBox(NULL,MsgString[MSG_ERR_ADDING_VALUE],NULL,MB_OK);
       return FALSE;
    }

    return TRUE;
}

/******************************************************************************
     AssignNumVal:
     Extract a numeric value from the dialog item.  Display error if a non-
     numeric value is encountered and return a false result.
*******************************************************************************/
BOOL AssignNumVal(HWND hWin,int ControlId,LPINT pInt)
{
    BOOL ErrorFlag;

    *pInt=GetDlgItemInt(hWin,ControlId,&ErrorFlag,(BOOL) 0);
    if (!ErrorFlag) {
       MessageBox(hWin,MsgString[MSG_INVALID_NUMBER],NULL,MB_OK);
       SetFocus(GetDlgItem(hWin,ControlId));
       return (FALSE);
    }
    return TRUE;
}

/******************************************************************************
     AssignLongVal:
     Extract a long value from the dialog item.  Display error if a non-
     numeric value is encountered and return a false result.
*******************************************************************************/
BOOL AssignLongVal(HWND hWin,int ControlId,LPLONG pLong)
{
    int i;
    BYTE string[MAX_WIDTH+1];

    GetDlgItemText(hWin,ControlId,string,sizeof(string)-1);  // get the number

    for (i=0;i<(int)strlen(string);i++) {  // check for non numeric characters
        if (string[i]=='-') continue;
        if (string[i]<'0' || string[i]>'9') {
           MessageBox(hWin,MsgString[MSG_INVALID_NUMBER],NULL,MB_OK);
           SetFocus(GetDlgItem(hWin,ControlId));
           return (FALSE);
        }
    }

    (*pLong)=atol(string);       // convert to long

    return TRUE;
}

/******************************************************************************
    CheckDlgValue:
    This routine checks if the user entered a numeric value within the given
    range.  If a value is found outside the range,  the routine displays
    an error message, sets the focus on the item, and returns a false value.
*******************************************************************************/
BOOL CheckDlgValue(HWND hDlg,BYTE type,int id,double BeginRange,double EndRange)
{
    double value;
    BOOL flag=TRUE;
    BYTE string[100];
    PTERWND w;

    if (type=='I') {
       value=(double)GetDlgItemInt(hDlg,id,&flag,0);
       if (!flag) goto ERROR_FOUND;
    }
    else {
       GetDlgItemText(hDlg,id,string,100);
       lstrcpy(TempString,string);        // copy to the data segment

       w=(PTERWND)(DWORD)GetWindowLong(hDlg,DWL_USER); // get data pointer
       if (w) value=TerAtof(w,TempString);
       else   value=atof(TempString);
    }

    if (value<BeginRange || value>EndRange) goto ERROR_FOUND;

    return TRUE;

    ERROR_FOUND:
    MessageBox(hDlg,MsgString[MSG_INVALID_NUMBER],MsgString[MSG_OUT_OF_RANGE],MB_OK);
    SetFocus(GetDlgItem(hDlg,id));
    return FALSE;
}



/******************************************************************************
    IsBlankDlg:
    This routine returns TRUE if the specified edit field contains blanks.
*******************************************************************************/
BOOL IsBlankDlg(HWND hDlg,int id)
{
    BYTE string[100];

    GetDlgItemText(hDlg,id,string,100);
    StrTrim(string);

    if (lstrlen(string)==0) return TRUE;
    else                    return FALSE;
}

/******************************************************************************
    ApplyPrinterSetting:
    Apply the current orientation of the printer paper to all the sections
    in the document.
*******************************************************************************/
ApplyPrinterSetting(PTERWND w, int orient, int bin, int PprSize, float PprWidth, float PprHeight)
{
    int i;

    if (orient>=0 && orient!=DMORIENT_PORTRAIT && orient!=DMORIENT_LANDSCAPE) orient=DMORIENT_PORTRAIT;

    PaperOrient=orient;

    for (i=0;i<TotalSects;i++) {
       if (orient!=-1) TerSect[i].orient=orient;
       if (bin!=-1) {
          TerSect[i].FirstPageBin=bin;
          TerSect[i].bin=bin;
       }
       if (PprSize!=-1) TerSect[i].PprSize=PprSize;
       if (PprWidth>0) TerSect[i].PprWidth=PprWidth;
       if (PprHeight>0) TerSect[i].PprHeight=PprHeight;
    }

    return TRUE;
}

/******************************************************************************
    TerSetPaper:
    Set new paper size and orientation.  The size is specified by the DMPAPER_
    WIN SDK constants.
*******************************************************************************/
BOOL WINAPI _export TerSetPaper(HWND hWnd,int size, int orient, BOOL repaint)
{
   return TerSetPaperEx(hWnd,size,0,0,orient,repaint);
}

/******************************************************************************
    TerSetPaperEx:
    Set new paper size and orientation.  The size is specified by the DMPAPER_
    WIN SDK constants. When the 'size' is set to 0, the size is then taken
    from the 'width' and 'height' parameters (in twips). Set the orient to to -1
    to leave it unchanged.
*******************************************************************************/
BOOL WINAPI _export TerSetPaperEx(HWND hWnd,int size, int width, int height, int orient, BOOL repaint)
{
    PTERWND w;
    HANDLE hDriver;


    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!PrinterAvailable && size==0) {  // set for screen only
       PageWidth=TwipsToInches(width);
       PageHeight=TwipsToInches(height);
       goto END;
    }

    if (!PrinterAvailable || !hDeviceMode) return TRUE;    // printer not available

    // update the pDeviceMode structure
    pDeviceMode->dmFields=0;
    if (orient>=0) {
       if (orient!=DMORIENT_PORTRAIT && orient!=DMORIENT_LANDSCAPE) orient=DMORIENT_PORTRAIT;
       PaperOrient=pDeviceMode->dmOrientation=(short)orient;
       pDeviceMode->dmFields|=DM_ORIENTATION;
    }
    PaperSize=pDeviceMode->dmPaperSize=(short)size;
    pDeviceMode->dmFields|=DM_PAPERSIZE;
    if (PaperSize==0) {       // custom size
       pDeviceMode->dmPaperSize=DMPAPER_USER;
       pDeviceMode->dmPaperWidth=MulDiv(width,254,1440);  // specified in tenth of mm
       pDeviceMode->dmPaperLength=MulDiv(height,254,1440);  // specified in tenth of mm
       pDeviceMode->dmFields|=(DM_PAPERWIDTH|DM_PAPERLENGTH);
    }

    // update the program variables and section properties
    if (!InRtfRead) {
       int sect=GetSection(w,CurLine);

       if (orient>=0) TerSect[sect].orient=PaperOrient=orient;
       PaperSize=size;
    }

    if (OpenPrinter(PrinterName,&hDriver,NULL)) {
       if (!DocumentProperties(hTerWnd,hDriver,PrinterName,pDeviceMode,pDeviceMode,DM_MODIFY|DM_COPY)) {
          PrintError(w,MSG_ERR_INIT_PRINTER,"TerSetPaper");
       }
       ClosePrinter(hDriver);
    }
    else return FALSE;


    // reopen printer and new page size
    OpenCurPrinter(w,FALSE);

    if (orient>=0 && ForceSectOrient(w)) ApplyPrinterSetting(w,PaperOrient,-1,-1,-1,-1);

    // apply page size
    if (size==0) ApplyPrinterSetting(w,-1,-1,DMPAPER_USER,TwipsToInches(width),TwipsToInches(height));   // set custom size even if the current printer does not support it
    else         ApplyPrinterSetting(w,-1,-1,PaperSize,PageWidth,PageHeight);

    END:

    SetSectPageSize(w);      // set the page sizes for the sections

    if (repaint) {           // no repagination when !repaint as input routine uses this function
       if (TerArg.PrintView) {
          Repaginate(w,FALSE,FALSE,0,TRUE);
          PageModifyCount=TerArg.modified++;
          RepageBeginLine=0;
       }
       PaintTer(w);
    }

    return TRUE;
}

/******************************************************************************
    TerOverridePageSize:
    Override the printer page size. Width/height given in twips.  The dimensons
    must be given for the portrait orientation.
*******************************************************************************/
BOOL WINAPI _export TerOverridePageSize(int width, int height)
{
    InitPageWidth=width;
    InitPageHeight=height;

    return TRUE;
}

/******************************************************************************
    TerSetPrinter:
    Set new printer
*******************************************************************************/
BOOL WINAPI _export TerSetPrinter(HWND hWnd,LPBYTE device,LPBYTE driver, LPBYTE port, HGLOBAL hInitDevMode)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // release the existing printer structure memory
    if (hDeviceMode) {
       GlobalUnlock(hDeviceMode);
       GlobalFree(hDeviceMode);
    }
    hDeviceMode=0;
    pDeviceMode=NULL;

    PrinterAvailable=FALSE;

    // set the printer name global variables
    if (device && lstrlen(device)==0) device=NULL;
    if (device==NULL) {
       if (!CurrentPrinter(w,PrinterName,PrinterDriver,PrinterPort) || !PrinterAvailable) return FALSE; // printer not available
    }
    else {
       lstrcpy(PrinterName,device);
       if (driver) lstrcpy(PrinterDriver,driver);
       else        PrinterDriver[0]=0;
       if (port)   lstrcpy(PrinterPort,port);
       else        PrinterPort[0]=0;

       lstrcpy(TempString,PrinterDriver);        // add .drv extension to the PrinterDriver name
       lstrupr(w,TempString);
       if (strstr(TempString,".DRV")==NULL && lstrlen(TempString)>0) lstrcat(PrinterDriver,".drv");
    }

    // initialize the DEVMODE structure
    if (hInitDevMode) {        // copy the given buffer
       DWORD bytes=GlobalSize(hInitDevMode);
       LPBYTE pInitDeviceMode;

       // allocate memory for the structure
       if ( (NULL==(hDeviceMode=GlobalAlloc(GMEM_MOVEABLE,bytes+1)))
           || (NULL==(pDeviceMode=(DEVMODE far *)GlobalLock(hDeviceMode)))  ) {
           return PrintError(w,MSG_OUT_OF_MEM,"TerSetPrinter");
       }

       // copy the structure
       if (NULL!=(pInitDeviceMode=GlobalLock(hInitDevMode))) {
          FarMove(pInitDeviceMode,pDeviceMode,(UINT)bytes);
          GlobalUnlock(hInitDevMode);
          PrinterAvailable=TRUE;
       }
    }
    else {
       if (True(TerFlags&TFLAG_NO_PRINTER)) 
             PrinterAvailable=FALSE;
       else  PrinterAvailable=InitDevMode(w,FALSE/*TRUE*/);
    }

    // reopen existing printer device context
    if (PrinterAvailable) {   // reopen if already open
       // update our paper size/orientation
       PaperSize=pDeviceMode->dmPaperSize;
       PaperOrient=pDeviceMode->dmOrientation;
       if (TerFlags&TFLAG_APPLY_PRT_PROPS) ApplyPrinterSetting(w,PaperOrient,-1,-1,-1,-1);
    }

    if (!OpenCurPrinter(w,FALSE)) return FALSE;

    if (TerFlags&TFLAG_APPLY_PRT_PROPS) ApplyPrinterSetting(w,-1,-1,PaperSize,PageWidth,PageHeight);

    SetSectPageSize(w);

    if (TerArg.PrintView) {
        long l;

        RecreateFonts(w,hTerDC);  // recreate fonts
        TotalPages=1;

        for (l=0;l<TotalLines;l++) {  // initialize line x and heights
           LineX(l)=LineY(l)=0;
           LineHt(l)=0;
        }
        Repaginate(w,FALSE,FALSE,0,TRUE);
        PageModifyCount=TerArg.modified++;
        RepageBeginLine=0;
    }
    PaintTer(w);

    return PrinterAvailable;
}

/******************************************************************************
    TerSetDefPrinter:
    Set the default printer for the internal use of TER.
******************************************************************************/
BOOL WINAPI _export TerSetDefPrinter(LPBYTE name,LPBYTE driver,LPBYTE port)
{
    lstrcpy(DefPrinterName,name);
    lstrcpy(DefPrinterDriver,driver);
    lstrcpy(DefPrinterPort,port);
    return TRUE;
}

/******************************************************************************
    CurrentPrinter:
    Get information (name,driver,port) for the currently selected printer.
    The function returns FALSE if the current printer not set.
******************************************************************************/
BOOL CurrentPrinter(PTERWND w,LPBYTE name,LPBYTE driver,LPBYTE port)
{
    BYTE PrintData[300],ext[5];
    LPBYTE temp,name1,driver1=0,port1=0;
    int len;

    // use the TER default printer if specified
    if (lstrlen(DefPrinterName)>0) {
       lstrcpy(name,DefPrinterName);
       lstrcpy(driver,DefPrinterDriver);
       lstrcpy(port,DefPrinterPort);
       PrinterAvailable=TRUE;
       return TRUE;
    }

    name[0]=driver[0]=port[0]=0;       // initialized the output variables
    PrinterAvailable=FALSE;

    if (!GetProfileString("windows","device",(LPBYTE) "",PrintData,300)) return FALSE;

    temp=name1=PrintData;              // beginning pointer for the scan

    while(*temp) {                     // scan until a NULL is found
       if (*temp==',') {               // a delimited found
          *temp=0;                     // NULL terminate the previous string, and advance the pointer
          temp++;
          while (*temp==' ') temp=AnsiNext(temp);   // skip leading spaces
          if (!driver1) driver1=temp;  // Initiate the driver string if not found yet
          else {                       // else it is the last string, i.e. the port string
             port1=temp;
             break;                    // all strings parsed
          }
       }
       else temp=AnsiNext(temp);       // advance the pointer
    }

    lstrcpy(name,name1);               // copy the strings to the user specified area
    lstrcpy(port,port1);
    lstrcpy(driver,driver1);


    // add .drv extension to the driver name, if necessary
    len=lstrlen(driver);
    if (len<=4) lstrcat(driver,".drv");
    else {
       lstrcpy(ext,&driver[len-4]);
       lstrupr(w,ext);
       if (lstrcmp(ext,".DRV")!=0) lstrcat(driver,".DRV");
    }

    PrinterAvailable=TRUE;

    return TRUE;
}

/******************************************************************************
    OpenCurPrinterOrient
    Open the current printer device context in the given orientation
******************************************************************************/
BOOL OpenCurPrinterOrient(PTERWND w, int orient, BOOL private)
{
    if (PrinterAvailable && pDeviceMode) pDeviceMode->dmOrientation=orient;

    return OpenCurPrinter(w,private);
}

/******************************************************************************
    OpenCurPrinter:
    Open the current printer device context.
******************************************************************************/
BOOL OpenCurPrinter(PTERWND w, BOOL private)
{
    BYTE       driver[MAX_WIDTH+1];
    POINT      pt;
    int        i,SaveOrient;

    if (True(TerFlags5&TFLAG5_NO_SHARE)) private=true;   // resources not being shared

    if (hPr && hPr!=hTerDC) {
       SelectObject(hPr,GetStockObject(SYSTEM_FONT));
       
       if (PrivatePrtDC) {
          DeleteDC(hPr);      // delete any old device context
          
          // delete all fonts for this DC - if not Windows leaks some font related memory
          for (i=0;i<TotalGlbFonts;i++) {
             if (GlbFont[i].hDC==hPr) {
                if (GlbFont[i].hFont) DeleteObject(GlbFont[i].hFont);
                GlbFont[i].hFont=NULL;
                //GlbFont[i].UseCount=0;       UseCount will be deleted automatically when a PrtFont is deleted
             } 
          } 
       }
       else GlbPrtDC[GlbPrtDCId].UseCount--;

    }
    hPr=0;

    //************ create printer device context ******************
    if (PrinterAvailable && False(TerFlags6&TFLAG6_HI_RES)) {
       driver[0]=0;
       if (PrinterDriver) {
         lstrcpy(driver,PrinterDriver);
         if (lstrlen(driver)>4) driver[lstrlen(driver)-4]=0;             // strip the .drv extension
       }
       
       if (private) hPr=CreateDC(driver,PrinterName,PrinterPort,pDeviceMode);
       else         hPr=CreateGlbPrtDC(w,driver,PrinterName,PrinterPort,pDeviceMode);
       if (!hPr) {
           BYTE string[100];
           wsprintf(string,"OpenCurPrinter: printer: %s, port: %s, driver: %s",(LPBYTE)PrinterName,(LPBYTE)PrinterPort,(LPBYTE)driver);
           PrintError(w,MSG_ERR_INIT_PRINTER,string);
           PrinterAvailable=FALSE;
           return FALSE;
       }

       PrivatePrtDC=private;

       PrtResX=GetDeviceCaps(hPr,LOGPIXELSX);    // number of pixels per inch of X direction
       PrtResY=GetDeviceCaps(hPr,LOGPIXELSY);    // number of pixels per inch of Y direction
       ExtraSpacePrtX=TwipsToPrtX(BOX_BORDER_WIDTH);  // a unit of extra space

       PrtDiffRes=(PrtResX!=PrtResY);

       Escape(hPr,GETPHYSPAGESIZE,0,NULL,&pt);
       PageWidth=(float)pt.x/PrtResX;            // actual page size
       PageHeight=(float)pt.y/PrtResY;

       if (pDeviceMode->dmOrientation!=DMORIENT_PORTRAIT && pDeviceMode->dmOrientation!=DMORIENT_LANDSCAPE)
          pDeviceMode->dmOrientation=DMORIENT_PORTRAIT;        // fix for some printers

       if (pDeviceMode->dmOrientation==DMORIENT_LANDSCAPE) {
          float temp=PageWidth;                  // get portrait page width
          PageWidth=PageHeight;
          PageHeight=temp;
       }
       if (InitPageWidth)  PageWidth=TwipsToInches(InitPageWidth);
       if (InitPageHeight) PageHeight=TwipsToInches(InitPageHeight);

       Escape(hPr,GETPRINTINGOFFSET,0,NULL,&pt); // offset where printing begins
       if (pDeviceMode->dmOrientation==DMORIENT_PORTRAIT) { // get the portrait margins
          PortraitHX=pt.x;
          PortraitHY=pt.y;
       }
       else {
          LandscapeHX=pt.x;
          LandscapeHY=pt.y;
       }
       //PortraitHX=PortraitHY=LandscapeHX=LandscapeHY=0; // let the printing go into the hidden area

       // get the hidden x/y for the other orientation
       SaveOrient=pDeviceMode->dmOrientation;
       if (pDeviceMode->dmOrientation==DMORIENT_PORTRAIT)
            pDeviceMode->dmOrientation=DMORIENT_LANDSCAPE;
       else pDeviceMode->dmOrientation=DMORIENT_PORTRAIT;

       ResetDC(hPr,pDeviceMode);
       Escape(hPr,GETPRINTINGOFFSET,0,NULL,&pt); // offset where printing begins
       if (pDeviceMode->dmOrientation==DMORIENT_PORTRAIT) { // get the portrait margins
          PortraitHX=pt.x;
          PortraitHY=pt.y;
       }
       else {
          LandscapeHX=pt.x;
          LandscapeHY=pt.y;
       }
       //PortraitHX=PortraitHY=LandscapeHX=LandscapeHY=0; // let the printing go into the hidden area

       // restore the orientation
       pDeviceMode->dmOrientation=SaveOrient;
       ResetDC(hPr,pDeviceMode);

    }
    else {                                       // use the screen device context
       hPr=hTerDC;
       if (UseScrMetrics) {
          PrtResX=UNITS_PER_INCH;  // 1440
          PrtResY=UNITS_PER_INCH;  // 1440
       }
       else {
          PrtResX=ScrResX;
          PrtResY=ScrResY;
       }
       ExtraSpacePrtX=ExtraSpaceScrX;            // a unit of extra space
       PortraitHX=PortraitHY=LandscapeHX=LandscapeHY=0;
       PrtDiffRes=FALSE;
    }

    return TRUE;
}

/*******************************************************************************
    CreateGlbPrtDC:
    Create a global printer device context.
*******************************************************************************/
HDC CreateGlbPrtDC(PTERWND w, LPBYTE drv,LPBYTE name,LPBYTE port,LPDEVMODE pDevMode)
{
    int i;
    HDC hDC;

    // check if a device context is available
    for (i=0;i<GlbPrtDCCount;i++) {
       if (  GlbPrtDC[i].hDC
          && lstrcmpi(drv,GlbPrtDC[i].drv)==0
          && lstrcmpi(name,GlbPrtDC[i].name)==0
          && lstrcmpi(port,GlbPrtDC[i].port)==0
          && memcmp(pDevMode,&(GlbPrtDC[i].DevMode),sizeof(DEVMODE))==0 ) {
          GlbPrtDC[i].UseCount++;
          GlbPrtDCId=i;
          return GlbPrtDC[i].hDC;        // match found;
       }
    }

    // check for an old unused slot
    for (i=0;i<GlbPrtDCCount;i++) if (GlbPrtDC[i].UseCount<=0) break;

    if (i==GlbPrtDCCount) {
       if (GlbPrtDCCount==MAX_GLB_DC) return NULL;   // ran out of the table
    }
    else DeleteDC(GlbPrtDC[i].hDC);                  // release the old DC

    FarMemSet(&(GlbPrtDC[i]),0,sizeof(struct StrGlbPrtDC));

    lstrcpy(GlbPrtDC[i].drv,drv);
    lstrcpy(GlbPrtDC[i].name,name);
    lstrcpy(GlbPrtDC[i].port,port);
    FarMove(pDevMode,&(GlbPrtDC[i].DevMode),sizeof(DEVMODE));

    hDC=GlbPrtDC[i].hDC=CreateDC(drv,name,port,pDevMode);

    if (hDC) {
      GlbPrtDC[i].UseCount++;
      GlbPrtDCId=i;
      if (i==GlbPrtDCCount) GlbPrtDCCount++;
    }

    return hDC;
}

/*******************************************************************************
    TerJump:
    Jump to a uesr requested line number.
*******************************************************************************/
TerJump(PTERWND w)
{
    BOOL result;


    result=CallDialogBox(w,"JumpParam",JumpParam,0L);
    if (result<0) return FALSE;

    TerPosLine(w,result);               // position at the requested line number

    return TRUE;
}

/******************************************************************************
     SetDlgLong:
     Set a long value to a dialog field.
*******************************************************************************/
BOOL SetDlgLong(HWND hDlg,int ControlId,long value)
{
    BYTE string[20];

    wsprintf(string,"%ld",value);
    SetDlgItemText(hDlg,ControlId,string);

    return TRUE;
}


/******************************************************************************
    lstrchr:
    locates a character within a string.  The routine returns the pointer
    the first occurence of the character within the string.
******************************************************************************/
LPBYTE lstrchr(LPBYTE string,BYTE chr)
{
    int   i,len;

    len=lstrlen(string);

    for (i=0;i<len;i++) {
       if (string[i]==chr) return &string[i];
    }
    return (LPBYTE)NULL;
}

/******************************************************************************
    StrLenSplit:
    Get the length of a multibyte string after splitting it.
*******************************************************************************/
int StrLenSplit(PTERWND w,LPBYTE string)
{
    if (mbcs) return (int)TerSplitMbcs(hTerWnd,string,NULL,NULL);
    else      return lstrlen(string);
}

/******************************************************************************
    StrLwrDB:
    Converts a string to lower case when the lead bytes are also involved
*******************************************************************************/
LPBYTE StrLwrDB(PTERWND w,LPBYTE string, LPBYTE lead)
{
    int i,len;
    BYTE chr;

    if (!mbcs) return lstrlwr(w,string);

    len=lstrlen(string);

    for (i=0;i<len;i++) {
       chr = string[i];
       if (lead[i]==0 && chr>='A' && chr<='Z') string[i]=(char)tolower(chr);
    }
    return string;
}

/******************************************************************************
    StrLwrDBU:
    Converts a string to lower case when the lead bytes are also involved
*******************************************************************************/
LPBYTE StrLwrDBU(PTERWND w,LPBYTE string, LPBYTE lead, LPWORD UcBase)
{
    int i,len;
    BYTE chr;

    if (!mbcs) lead=null;

    len=lstrlen(string);

    for (i=0;i<len;i++) {
       chr = string[i];
       if ((lead==null || lead[i]==0) && UcBase[i]==0 && chr>='A' && chr<='Z') string[i]=(char)tolower(chr);
    }
    return string;
}


/******************************************************************************
    lstrlwr:
    Converts a far string to lower case.  This function is mbcs enabled.
*******************************************************************************/
LPBYTE lstrlwr(PTERWND w,LPBYTE string)
{
    int i,len;
    BYTE chr;
    LPBYTE text,LeadText;

    if (mbcs) {
       len=AllocDB(w,string,&text,&LeadText);

       for (i=0;i<len;i++) {
          chr = text[i];
          if (LeadText[i]==0 && chr>='A' && chr<='Z') text[i]=(char)tolower(chr);
       }

       TerMakeMbcs(hTerWnd,string,text,LeadText,len);

       OurFree(text);
       OurFree(LeadText);
    }
    else {
       CharLowerBuff(string,lstrlen(string));
    }


    return string;
}

/******************************************************************************
    lstrupr:
    Converts a far string to upper case.  This function is mbcs enabled
*******************************************************************************/
LPBYTE lstrupr(PTERWND w,LPBYTE string)
{
    int i,len;
    BYTE chr;
    LPBYTE text,LeadText;

    if (mbcs) {
       len=AllocDB(w,string,&text,&LeadText);

       for (i=0;i<len;i++) {
          chr = text[i];
          if (LeadText[i]==0 && chr>='a' && chr<='z') text[i]=(char)toupper(chr);
       }

       TerMakeMbcs(hTerWnd,string,text,LeadText,len);

       OurFree(text);
       OurFree(LeadText);
    }
    else {
       #if defined(WIN32)
          CharUpperBuff(string,lstrlen(string));
       #else
          len=lstrlen(string);
          for (i=0;i<len;i++) {
             chr = string[i];
             if (chr>='a' && chr<='z') string[i]=(char)toupper(chr);
          }
       #endif
    }


    return string;
}

/******************************************************************************
    TerAtof:
    Do 'atof' taking into account the TFLAG_SWAP_DECIMAL flag;
*******************************************************************************/
double TerAtof(PTERWND w,LPBYTE string)
{
    int i,len;
    BYTE string1[40];

    if (!(TerFlags&TFLAG_SWAP_DECIMAL)) return atof(string);

    // restore decimal and comma characters
    lstrcpy(string1,string);
    len=lstrlen(string1);

    for (i=0;i<len;i++) {
       if      (string1[i]=='.') string1[i]=',';
       else if (string1[i]==',') string1[i]='.';
    }
    return atof(string1);
}

 /******************************************************************************
    TerLparam2String:
    This function is used by a VB application to copy text from a lParam
    pointer to the given string.
*******************************************************************************/
BOOL WINAPI _export TerLparam2String(long lParam, LPBYTE text)
{
    lstrcpy(text,(LPBYTE)lParam);
    return TRUE;
}

/******************************************************************************
    TerString2Lparam:
    This function is used by a VB application to copy text from a 'string'
    variable to a pointer specified by lParam
*******************************************************************************/
BOOL WINAPI _export TerString2Lparam(LPBYTE text,long lParam)
{
    lstrcpy((LPBYTE)lParam,text);
    return TRUE;
}

/******************************************************************************
    DoubleToStr:
    This routine converts a double value to a string.  The second argument
    provides the digits after the decimal point.
    The function returns the pointer to the third argument after storing
    the result in the third argument.
*******************************************************************************/
LPBYTE DoubleToStr(PTERWND w,double nbr,int dec,LPBYTE out)
{
    long i,len,BeforeDec,AfterDec,multiply;
    BYTE string[20],decimal='.';
    double dAfterDec;

    out[0]=0;

    if (TerFlags&TFLAG_SWAP_DECIMAL) decimal=',';

    //************* apply sign *******
    if (nbr<0) {
       lstrcat(out,"-");
       nbr=-nbr;
    }

    BeforeDec = (long) nbr;           // number before the decimal point

    //****** append the digits after the decimal point ****
    if (dec>0) {
       nbr=nbr-(double)BeforeDec;
       multiply=1;
       for (i=0;i<dec;i++) multiply=multiply*10;
       dAfterDec=nbr*(double)multiply;
       AfterDec=(long)dAfterDec;
       nbr=dAfterDec-(double)AfterDec;
       if (nbr>=.5) AfterDec++;
       if (AfterDec>=multiply) {    // overflow
          BeforeDec++;
          AfterDec=0;
       }

       //************* make the out *****************
       if (BeforeDec>0) {
          wsprintf(string,"%ld",BeforeDec);
          lstrcat(out,string);
       }

       len=lstrlen(out);     // appen the decimal character
       out[len]=decimal;
       out[len+1]=0;

       wsprintf(string,"%ld",AfterDec);
       len=lstrlen(string);
       for (i=len;i<dec;i++) lstrcat(out,"0");  // first pad with necessary space
       lstrcat(out,string);                     // then append the digits
    }
    else if (BeforeDec>0) {
       wsprintf(string,"%ld",BeforeDec);
       lstrcat(out,string);
    }

    return out;
}

/******************************************************************************
    TerTest:
    Test code entry point.
*******************************************************************************/
BOOL WINAPI _export TerTest(HWND hWnd, int val1, int val2, int val3, LPBYTE text1, LPBYTE text2, LPBYTE text3)
{
    PTERWND w;
    int i,cl;
    LPBYTE indent="";

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    for (i=1;i<TotalTableRows;i++) {
       for (cl=TableRow[i].FirstCell;cl>0;cl=cell[cl].NextCell) {
           indent=cell[cl].level==0?"":"    ";
           OurPrintf("%s row: %d, next row: %d, lvl: %d, p: %d, cl: %d, next: %d, prev: %d, row span: %d, row-spanned: %d, col span: %d, w: %d",
              indent,cell[cl].row,TableRow[i].NextRow,cell[cl].level,cell[cl].ParentCell,cl,cell[cl].NextCell,cell[cl].PrevCell,
              cell[cl].RowSpan,cell[cl].flags&CFLAG_ROW_SPANNED,
              cell[cl].ColSpan,cell[cl].width);
       }
       OurPrintf(" "); 
    } 

    return TRUE;
}


/******************************************************************************
    TerSetDebug:
    Set debug mode on/off
*******************************************************************************/
BOOL WINAPI _export TerSetDebug(BOOL on)
{
    BOOL prev=DebugMode;
    DebugMode=on;
    return prev;
}

/******************************************************************************
    dm:
    show debug message.
*******************************************************************************/
dm(LPBYTE msg)
{
   if (DebugMode) OurPrintf("%s",msg);
   return TRUE;
} 

/******************************************************************************
    OurPrintf:
    This routine formats and display a given set of arguments.  The format
    is given by the first argument.  The argument 2 to n contain the
    arguments for the specified format.  The function uses MessageBox to
    display the formatted string.
******************************************************************************/
int OurPrintf(LPBYTE fmt,...)
{
    LPSTR ArgStart;
    char string[MAX_WIDTH+2];
    HWND hWnd;

    ArgStart=(LPSTR) &fmt;  // pointer to first argument
    ArgStart=&ArgStart[4];  // go past the first argument
    wvsprintf(string,fmt,ArgStart);

    if (NULL!=(hWnd=FindWindow("DBWin",NULL))) { // debug window open
        if (NULL!=(hWnd=GetWindow(hWnd,GW_CHILD))) {    // get the edit window
           int len=lstrlen(string);
           string[len]=0xd;
           string[len+1]=0xa;
           string[len+2]=0;
           SendMessage(hWnd,EM_SETSEL,32767,32767);  // show the last line
           SendMessage(hWnd,EM_REPLACESEL,0,(DWORD)(LPSTR)string);
           SendMessage(hWnd,EM_SCROLLCARET,0,0);     // scroll caret into view
        }
    }
    else MessageBox(NULL,string,NULL,MB_OK);
    
    return TRUE;
}

/******************************************************************************
    LogPrintf:
    This routine formats and logsa given set of arguments.  The format
    is given by the first argument.  The argument 2 to n contain the
    arguments for the specified format
******************************************************************************/
int LogPrintf(LPBYTE fmt,...)
{
    LPSTR ArgStart;
    char string[MAX_WIDTH+2];
    LPBYTE pLogFile="c:\\temp\\ter.log";
    HANDLE hFile;

    ArgStart=(LPSTR) &fmt;  // pointer to first argument
    ArgStart=&ArgStart[4];  // go past the first argument
    wvsprintf(string,fmt,ArgStart);


    if (INVALID_HANDLE_VALUE!=(hFile=CreateFile(pLogFile,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) {
       struct tm *pTM;
       time_t clock;
       LPBYTE pTimeString;
       DWORD written;
       BYTE CrLf[3]={13,10,0};

       time(&clock );                 /* Get time in seconds */
       pTM = localtime( &clock );  /* Convert time to struct tm form */
       pTimeString=asctime(pTM);
       
       SetFilePointer(hFile,0,NULL,FILE_END);  // position at the end of the file
       WriteFile(hFile,pTimeString,lstrlen(pTimeString)-1,&written,NULL);   // don't include new line character
       WriteFile(hFile," --> ",5,&written,NULL);
       WriteFile(hFile,string,lstrlen(string),&written,NULL);
       WriteFile(hFile,CrLf,lstrlen(CrLf),&written,NULL);

       CloseHandle(hFile);

       return TRUE;
    }
    
    return TRUE;
}

/******************************************************************************
    TerEngageCaret:
    Returns TRUE if a TE window can be closed
******************************************************************************/
BOOL WINAPI _export TerEngageCaret(HWND hWnd, BOOL AtCursorLoc)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CaretEngaged) {
       if (!CaretEnabled && UseCaret(w)) InitCaret(w);   //20050920     
       return TRUE;   // caret already engaged
    }

    return EngageCaret(w,(AtCursorLoc?-1:0));
}

/******************************************************************************
    EngageCaret:
    Engage the caret to the current position.  Set the cmd to 0 to engage the
    caret at the CaretPos.  Set the cmd to -1 to enage the caret at the
    current cursor location.
*******************************************************************************/
EngageCaret(PTERWND w,int cmd)
{
    long NewCurLine;

    if (CaretEngaged) return TRUE;

    // caret not engage for the following commands
    if (cmd==ID_PAGE_MODE) return TRUE;
    if (cmd==ID_VIEW_HDR_FTR) return TRUE;
    if (cmd==ID_EDIT_FNOTE) return TRUE;
    if (cmd==ID_TOOL_BAR) return TRUE;
    if (cmd==ID_RULER) return TRUE;
    if (cmd==ID_STATUS_RIBBON) return TRUE;
    if (cmd==ID_SHOW_HIDDEN) return TRUE;
    if (cmd==ID_SHOW_PARA_MARK) return TRUE;
    if (cmd==ID_SHOW_HYPERLINK_CURSOR) return TRUE;
    if (cmd==ID_PRINT) return TRUE;
    if (cmd==ID_PRINT_PREVIEW) return TRUE;


    CaretEngaged=TRUE;

    // caret engaged at the current line for these command
    if (cmd==ID_PGUP) return TRUE;
    if (cmd==ID_PGDN) return TRUE;
    if (cmd==ID_INSERT_PARA_FRAME) return TRUE;
    if (cmd==-1) return TRUE;

    // current line move to caret line for the rest of the commands
    AbsToRowCol(w,CaretPos,&NewCurLine,&CurCol);   // set the current position

    if (TerArg.PageMode) {
       CurPage=CaretPage;
       CurLine=NewCurLine;
       PaintTer(w);
    }
    else TerPosLine(w,NewCurLine+1);

    return TRUE;
}

/******************************************************************************
    DisengageCaret:
    Disengage the caret from the current position
*******************************************************************************/
DisengageCaret(PTERWND w)
{
    if (!CaretEngaged) return TRUE;

    CaretPos=RowColToAbs(w,CurLine,CurCol);   // save the caret position
    CaretPage=CurPage;
    CaretEngaged=FALSE;

    return TRUE;
}

/******************************************************************************
    UseCaret:
    This function returns TRUE if the caret is to be used by the editor
*******************************************************************************/
BOOL UseCaret(PTERWND w)
{
    return ( (!TerArg.ReadOnly || TerFlags&TFLAG_SHOW_CARET || ProtectForm) && !(TerFlags2&TFLAG2_NO_CARET));
}

/******************************************************************************
    InitCaret:
    Create the caret if destoryed, or recreated the caret if the height
    changes.
    In ReadOnly mode, it destroys the caret if it exists
*******************************************************************************/
InitCaret(PTERWND w)
{
    int height,y,CaretCol,SpcBef,NewCaretVert;
    long CaretLine;

    // hide caret in readonly mode
    if (!UseCaret(w) || InPrintPreview || (hTerWnd!=GetFocus() && !InDragDrop) /* || !IsWindowVisible(hTerWnd)*/) {  // IsWindowVisible sometimes does not return TRUE when a control is within a dialog 
       if (CaretEnabled) TerDestroyCaret(w);
       CaretEnabled=FALSE;
       CaretHidden=TRUE;
       CaretHeight=0;
       return TRUE;
    }

    // is caret engaged to the current position
    if (CaretEngaged) {
       CaretLine=CurLine;
       CaretCol=CurCol;
    }
    else {
       AbsToRowCol(w,CaretPos,&CaretLine,&CaretCol);
    }

    y=LineToUnits(w,CaretLine);          // current line position

    // calculate the caret height
    height=GetRowHeight(w,CaretLine);
    GetLineSpacing(w,CaretLine,height,&SpcBef,NULL,TRUE);
    y+=SpcBef;            // adjust for space before the paragraph

    if (y<TerWinOrgY) {   // handle the partial begin lines
       height=height-(TerWinOrgY-y);
       y=TerWinOrgY;
    }
    if ((height+y-TerWinOrgY)>TerWinHeight) height=TerWinHeight-y+TerWinOrgY;
    if (height<=0) height=TerFont[DEFAULT_CFMT].height;
    NewCaretVert=(LineTextAngle(w,CaretLine)==0);

    // Destroy the caret if the height changes
    if ((height!=CaretHeight || CaretVert!=NewCaretVert) && !CaretHidden) TerDestroyCaret(w);
    CaretHeight=height;
    CaretVert=NewCaretVert;

    CaretEnabled=TRUE;                 // this window owns the caret now

    if (IsCaretVisible(w,CaretLine,CaretCol)) {
       int CaretX,CaretY;
       GetCaretXY(w,CaretLine,CaretCol,y,&CaretX,&CaretY);
       if (CaretX<=TerRect.right) SetCaretPos(CaretX,CaretY); // CaretX be 9999 if frames are not yet created
    }

    return TRUE;
}

/******************************************************************************
    TerDestroyCaret:
    Destroy the caret and set the 'CaretEnabled' to FALSE
*******************************************************************************/
TerDestroyCaret(PTERWND w)
{
    if (!CaretHidden) DestroyCaret();
    CaretEnabled=FALSE; // suspend caret
    CaretHidden=TRUE;
    return TRUE;
}

/******************************************************************************
    IsCaretVisible:
    Returns TRUE is caret is visible
*******************************************************************************/
BOOL IsCaretVisible(PTERWND w,long CaretLine,int CaretCol)
{
   BOOL show=TRUE;
   int FrameNo,x,y,ParaFID;

   if (CaretEngaged) goto END;     // show the caret

   if (CaretLine<0) AbsToRowCol(w,CaretPos,&CaretLine,&CaretCol);

   // check if the caret is in the visible area
   show=FALSE;
   FrameNo=GetFrame(w,CaretLine);
   if (  FrameNo>=0
      && CaretLine>=frame[FrameNo].ScrFirstLine
      && CaretLine<=frame[FrameNo].ScrLastLine) {

      x=ColToUnits(w,CaretCol,CaretLine,LEFT);
      if (x>=TerWinOrgX && x<(TerWinOrgX+TerWinWidth)) show=TRUE;

      // when displaying from bitmap check the y position
      if (hScrollBM) {
         y=GetRowY(w,CaretLine);
         if (y<TerWinOrgY || y>=(TerWinOrgY+TerWinHeight)) show=FALSE;  // out of visible area
      }

   }

   END:
   // is caret on a protected text
   if (!ShowProtectCaret && show) {    // don't show it over protected text
      int CurFont=GetEffectiveCfmt(w); //GetCurCfmt(w,CaretLine,CaretCol);
      if (TerFont[CurFont].style&PROTECT) show=FALSE;
   }

   // is caret on non-text drawing object
   if (show) {
      ParaFID=fid(CaretLine);
      if (ParaFrame[ParaFID].flags&(PARA_FRAME_RECT|PARA_FRAME_LINE)) show=FALSE;
      if (CurSID>=0) show=FALSE;        // stylesheet active
   }

   if (   HilightType!=HILIGHT_OFF && !DraggingText
      && (HilightBegRow!=HilightEndRow || HilightBegCol!=HilightEndCol)) show=FALSE;  // caret not visible during highlighting

   if (show && CaretHidden) { // create and show caret
      if (CaretVert) CreateCaret(hTerWnd,NULL,TerTextMet.tmAveCharWidth/4,CaretHeight);
      else           CreateCaret(hTerWnd,NULL,CaretHeight,TerTextMet.tmAveCharWidth/4);
      ShowCaret(hTerWnd);
      CaretHidden=FALSE;
   }
   if (!show && !CaretHidden) {
      DestroyCaret();
      CaretHidden=TRUE;
   }

   return show;
}

/******************************************************************************
    TerGetCurParaNum:
    Get current paragraph number.
*******************************************************************************/
long WINAPI _export TerGetCurParaNum(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    
    return LineToParaNum(w,CurLine);
}
 
/******************************************************************************
    TerGetParaCount:
    Get current paragraph number.
*******************************************************************************/
long WINAPI _export TerGetParaCount(HWND hWnd)
{
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
   
   return (LineToParaNum(w,TotalLines-1)+1);
}
 
/******************************************************************************
    TerPosAtParaNum:
    Position at the given paragraph number.
*******************************************************************************/
BOOL WINAPI _export TerPosAtParaNum(HWND hWnd, int ParaNum, BOOL repaint)
{
    int line;
    
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    line=ParaToLineNum(w,ParaNum);

    return SetTerCursorPos(hWnd,line,0,repaint);
}
 
/******************************************************************************
    LineToParaNum:
    Convert line number to paragraph number.
*******************************************************************************/
long LineToParaNum(PTERWND w,long line)
{
   long ParaNum=0;
   long l;

   for (l=0;l<=line;l++) {
      if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;
      if (LineFlags(l)&LFLAG_PARA_FIRST) ParaNum++;
   }

   if (ParaNum>0) ParaNum--;  // zero based;

   return ParaNum;
}
 
/******************************************************************************
    LineToParaNum:
    Convert line number to paragraph number.
*******************************************************************************/
long ParaToLineNum(PTERWND w,long para)
{
   int LineNum=0;
   long l;

   for (l=0;l<TotalLines;l++) {
      if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;
      if (LineFlags(l)&LFLAG_PARA_FIRST) {
         if (para==0) break;
         para--;
      } 
   } 
   if (l==TotalLines) l--;

   return l;
}

/******************************************************************************
    TerTocToText:
    Convert table of contents to regular text.
*******************************************************************************/
BOOL WINAPI _export TerTocToText(HWND hWnd, BOOL repaint)
{
    int i,len,l,ParaId;
    long BegLine,StartLine,EndLine,count;
    LPWORD fmt,pCt;
    WORD OldFmt,NewFmt;

    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
 
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

    StartLine=0;

    NEXT_TOC:

    // locate next toc 
    for (l=StartLine;l<TotalLines;l++) if (LineFlags(l)&LFLAG_TOC) break;
    if (l==TotalLines) goto END;
    BegLine=l;
    
    for (l=BegLine;l<TotalLines;l++) {
       if (!(LineFlags(l)&LFLAG_TOC)) {
          if (LineLen(l)==0) continue;
          break;
       }
    }
    EndLine=l-1;
    count=EndLine-BegLine+1;     // number of lines to delete

    // reset the field id
    for (l=BegLine;l<=EndLine;l++) {
       len=LineLen(l); 
       fmt=OpenCfmt(w,l);
       OldFmt=0xFFFF;
       for (i=0;i<len;i++) {
         if (fmt[i]!=OldFmt) {
           OldFmt=fmt[i];
           if (TerFont[OldFmt].style&PICT) {
              TerFont[OldFmt].FieldId=0;
              NewFmt=OldFmt;
           }
           else {
              NewFmt=(int)GetNewFieldId(w,OldFmt,(DWORD)0,(DWORD)0,l,i);  // get the toc font id
           }
           
           if (True(TerFlags4&TFLAG4_NO_TOC_UPDATE)) {
              NewFmt=(int)GetNewColor(w,NewFmt,(DWORD)0,(DWORD)0,l,i);  // get the toc font id
              NewFmt=(int)GetNewCharStyle(w,NewFmt,(DWORD)0,(DWORD)0,l,i);  // get the toc font id
           }
              
         }
         fmt[i]=NewFmt;
       }
       CloseCfmt(w,l);
    }

    // remove para-style id
    for (l=BegLine;l<=EndLine;l++) {
       ParaId=pfmt(l);
       pfmt(l)=NewParaId(w,ParaId,
                         PfmtId[ParaId].LeftIndentTwips,
                         PfmtId[ParaId].RightIndentTwips,
                         PfmtId[ParaId].FirstIndentTwips,
                         PfmtId[ParaId].TabId,
                         PfmtId[ParaId].BltId,
                         PfmtId[ParaId].AuxId,
                         PfmtId[ParaId].Aux1Id,
                         0,
                         PfmtId[ParaId].shading,
                         PfmtId[ParaId].pflags,
                         PfmtId[ParaId].SpaceBefore,
                         PfmtId[ParaId].SpaceAfter,
                         PfmtId[ParaId].SpaceBetween,
                         PfmtId[ParaId].LineSpacing,
                         PfmtId[ParaId].BkColor,
                         PfmtId[ParaId].BorderSpace,
                         PfmtId[ParaId].flow,
                         PfmtId[ParaId].flags);
    } 


    StartLine=EndLine+1;
    goto NEXT_TOC;                     // process next toc

    END:

    if (repaint) PaintTer(w);
    return TRUE;
}
 
/******************************************************************************
    TerFieldToText:
    Convert the fields to regular text.
*******************************************************************************/
BOOL WINAPI _export TerFieldToText(HWND hWnd, BOOL all, BOOL repaint)
{
    long BegLine,EndLine,l;
    int  FieldId,len,FromCol,ToCol,i,BegCol,EndCol;
    WORD CurFmt,OldFmt,NewFmt;
    LPWORD fmt;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
 
    // set the range of operation
    BegLine=0;
    BegCol=0;
    EndLine=TotalLines-1;
    EndCol=LineLen(EndLine)-1;

    if (!all) {
      if (HilightType==HILIGHT_OFF) return FALSE;
      NormalizeBlock(w);

      BegLine=HilightBegRow;
      BegCol=HilightBegCol;
      EndLine=HilightEndRow;
      EndCol=HilightEndCol-1;   // inclusive
    } 

    // reset the field id
    for (l=BegLine;l<=EndLine;l++) {
       len=LineLen(l); 
       fmt=OpenCfmt(w,l);
       OldFmt=0xFFFF;
       
       FromCol=(l==BegLine)?BegCol:0;
       ToCol=(l==EndLine)?EndCol:(LineLen(l)-1);
       
       for (i=FromCol;i<=ToCol;i++) {
         CurFmt=fmt[i];
         FieldId=TerFont[CurFmt].FieldId;
         if (FieldId==0) continue;
         if (FieldId!=FIELD_NAME && FieldId!=FIELD_DATA && FieldId!=FIELD_PAGE_COUNT && FieldId!=FIELD_DATE) continue;
         
         if (FieldId==FIELD_NAME) {         // delete the field name text
            MoveLineData(w,l,i,1,'D');      // delete this character
            fmt=OpenCfmt(w,l);              // get the new pointer
            i--;                            // character deleted
            ToCol--;
            continue;
         } 

         if (CurFmt!=OldFmt) {
           OldFmt=CurFmt;
           if (TerFont[OldFmt].style&PICT) TerFont[OldFmt].FieldId=0;
           else {
              NewFmt=(int)GetNewFieldId(w,OldFmt,(DWORD)0,(DWORD)0,l,i);  // get the toc font id
           }
         }
         fmt[i]=NewFmt;
       }
       CloseCfmt(w,l);
    }

    if (repaint) PaintTer(w);

    return TRUE;
}
 
/******************************************************************************
    OldCheckEval:
    evaluation.
*******************************************************************************/
OldCheckEval()
{

    eval=expired=FALSE;

    #if !defined(__TURBOC__)
    {
       BYTE file[128];
       struct _stat st;
       struct tm *tm1;
       time_t tm2;

       if (GetModuleFileName(hTerInst,file,128)==0) return TRUE;

       if (_stat(file,&st)!=0) return TRUE;
       //tm1=gmtime(&(st.st_mtime));
       tm1=localtime(&(st.st_mtime));
       
       if (tm1->tm_hour!=1 || tm1->tm_min!=22 || tm1->tm_sec!=38) return TRUE;

       eval=TRUE;
       time(&tm2);
       expired=((difftime(tm2,st.st_mtime)/(60L*60*24))>30);
    }
    #endif

    return TRUE;
}

/****************************************************************************
   CheckEval:
   Check the evaluation status of the program.
*****************************************************************************/
BOOL CheckEval(BOOL KeyProvided, LPBYTE key)
{
   BYTE OurKey[100];
   BYTE EvalKey[100];
   BYTE   temp[100];
   HKEY   hKey;
   SYSTEMTIME SysDate;
   long  StartNumDays=0,yy,mm,dd,CurNumDays,EvalDaysUsed;
   BYTE  date[100];
   DWORD type;
   long result;
   int i,days=15;
   

   if (!eval) return true;

   if (extended && key==null || lstrlen(key)==0) return false;
   extended=false;

   if (!KeyProvided /*&& !InIE*/) {
     LPBYTE file="c:\\tecf\\key.txt";
     HANDLE InFile;
     int len;
     DWORD BytesRead;

     if ((InFile=CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
            goto CHECK_REGISTRY;
        
     len=17;  
     ReadFile(InFile,OurKey,len,&((DWORD)BytesRead),NULL);
     CloseHandle(InFile);
       
     OurKey[len]=0;

     key=OurKey;
   }
  
   if (strcmpi(key,"ttttt-ttttt-ttttt")==0) {
     days=21;  // extend
     extended=true;
     goto CHECK_REGISTRY;
   }
   else if (strcmpi(key,"uuuuu-uuuuu-uuuuu")==0) {
     days=28;  // extend
     extended=true;
     goto CHECK_REGISTRY;
   }
  
   if (lstrlen(key)!=lstrlen(LicenseKey)) goto CHECK_REGISTRY;

   if (strncmpi(key,LicenseKey,4)!=0) goto CHECK_REGISTRY;
   if (strncmpi(&(key[5]),&(LicenseKey[5]),5)!=0) goto CHECK_REGISTRY;
   if (strncmpi(&(key[11]),&(LicenseKey[11]),5)!=0) goto CHECK_REGISTRY;
      
   eval=false;
   expired=false;
   disable=false;
   EvalDaysRemaining=99999;
   return true;

   CHECK_REGISTRY:
   //if (InIE) return false;

   // check the registry key
   strcpy(EvalKey,"Software\\SubSystems\\Tecf13\\Eval");
   
   // get current date/time
   GetLocalTime(&SysDate);
   CurNumDays=SysDate.wYear*365+(int)((SysDate.wMonth-1)*(30.5))+SysDate.wDay;   // approx calculation for number of days

   // check the existing key to get the start time
   result=RegOpenKeyEx(HKEY_CURRENT_USER,EvalKey,0,KEY_ALL_ACCESS,&hKey);
   if (result==ERROR_SUCCESS) {
      DWORD DateSize=sizeof(date);
      result=RegQueryValueEx(hKey,"Date",NULL,&type,date,&DateSize);
      if (result==ERROR_SUCCESS) date[8]=0;
      if (result==ERROR_SUCCESS && strlen(date)==8) {
         for (i=0;i<8;i++) date[i]-=(i+1);  // decrypt

         strncpy(temp,date,4);
         temp[4]=0;
         yy=atol(temp);  // year
         strncpy(temp,&(date[4]),2);
         temp[2]=0;
         mm=atol(temp);  // year
         strncpy(temp,&(date[6]),2);
         temp[2]=0;
         dd=atol(temp);  // year
         StartNumDays=yy*365+(int)((mm-1)*(30.5))+dd;   // approx calculation for number of days

         if (CurNumDays>(StartNumDays+30)) {            // reset evaluation
             RegDeleteKey(hKey,EvalKey);
             StartNumDays=0;                            // reset
         } 
      }
      RegCloseKey(hKey); 
   } 

   if (result!=ERROR_SUCCESS || StartNumDays==0) {              // create the initialial SubKey
      StartNumDays=CurNumDays;                                  // set to current date
      
      // make a yyyymmdd string for today
      wsprintf(date,"%04d%02d%02d",(int)(SysDate.wYear),(int)(SysDate.wMonth),(int)(SysDate.wDay));
      for (i=0;i<8;i++) date[i]+=(i+1);  // encrypt

      result=RegCreateKeyEx(HKEY_CURRENT_USER,EvalKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);

      if (result==ERROR_SUCCESS) {
         result=RegSetValueEx(hKey,"Date",0,REG_SZ,date,9);
         RegCloseKey(hKey);
      }
   }

   // get the current date time
   EvalDaysUsed=(int)(CurNumDays-StartNumDays);   // TimeSpan.Days
   EvalDaysRemaining=days-EvalDaysUsed;
   expired=(EvalDaysRemaining<=0);
   disable=(eval && expired);                     // disable the control if in eval and expired

   return false;
}

/****************************************************************************
   TerSetLicenseKey:
   Set the licenes key.
*****************************************************************************/
BOOL WINAPI _export TerSetLicenseKey(LPBYTE key)
{
   CheckEval(true,key);
   if (eval && lstrcmpi(key,"ttttt-ttttt-ttttt")!=0 && lstrcmpi(key,"uuuuu-uuuuu-uuuuu")!=0) 
       MessageBox(NULL,"Invalid Product License Key!","Error",MB_OK);

   return (!eval);
}


/******************************************************************************
    PrintError:
    Print the specified error and return a FALSE value;
*******************************************************************************/
BOOL PrintError(PTERWND w, int MsgId,LPBYTE DebugMsg)
{
    HWND hWnd=NULL;
    BOOL SaveMessage=FALSE;

    if (w) {
       if (IsWindowVisible(hTerWnd)) hWnd=hTerWnd;
       if (TerFlags&TFLAG_RETURN_MSG_ID) SaveMessage=TRUE;
    }
    else if (TerInitFlags&TFLAG_RETURN_MSG_ID) SaveMessage=TRUE;

    
    TerLastMsg=MsgId;

    if (SaveMessage) {
       if (DebugMsg) lstrcpy(TerLastDebugMsg,DebugMsg);
       else          lstrcpy(TerLastDebugMsg,"");
    }
    else {
       if (w) InDialogBox=TRUE;
       if   (DebugMsg) MessageBox(hWnd,MsgString[MsgId],DebugMsg,MB_OK);
       else            MessageBox(hWnd,MsgString[MsgId],MsgString[MSG_ERROR],MB_OK);
       if (w) InDialogBox=FALSE;
    }

    return FALSE;
}

/******************************************************************************
    OurAlloc:
    Allocate a far memory object of given size. Lock the object, and return
    the far pointer to the object.
*******************************************************************************/
void far *OurAlloc(DWORD size)
{
    void far *pFar;
    pFar = MemAlloc((UINT)size);
 
    if(pFar != NULL) { memset(pFar, 0, size); };
    return(pFar);
}

/******************************************************************************
    OurRealloc:
    Realloc a far memory object to a given size. Lock the object, and return
    the far pointer to the object.
*******************************************************************************/
void far *OurRealloc(void far *pMem,DWORD size)
{

    #if defined(WIN32) 
       return MemReAlloc(pMem,(UINT)size);
    #else
       return GlbRealloc(pMem,size);
    #endif
}

/******************************************************************************
    OurFree:
    Free up the given memory object.  This object must have been allocated
    using the OurAlloc routine.
*******************************************************************************/
void OurFree(void far *pMem)
{
    if (pMem!=NULL) {
       #if defined(WIN32)
          if(!IsBadReadPtr(pMem, sizeof(BYTE)))  MemFree(pMem);
       #else
          GlbFree(pMem);
       #endif
    }
}

/******************************************************************************
    OurAllocCopy:
    Allocate and copy.
*******************************************************************************/
void far *OurAllocCopy(LPBYTE pSrc, DWORD size)
{
    LPBYTE dest=OurAlloc(size+1);

    if (dest!=null) FarMove(pSrc,dest,size);
    return dest;
}


/******************************************************************************
    GlbAlloc:
    Allocate a far memory object of given size. Lock the object, and return
    the far pointer to the object.
*******************************************************************************/
void far *GlbAlloc(DWORD size)
{
    HGLOBAL hMem;

    if (NULL==(hMem=GlobalAlloc(GMEM_MOVEABLE, size+1))) {
       PrintError(NULL,MSG_OUT_OF_MEM,"GlbAlloc");
       return NULL;
    }

    return (void far *) GlobalLock(hMem);
}

/******************************************************************************
    GlbRealloc:
    Realloc a far memory object to a given size. Lock the object, and return
    the far pointer to the object.
*******************************************************************************/
void far *GlbRealloc(void far *pMem,DWORD size)
{
    HGLOBAL hMem;

    #if defined(WIN32)
       hMem=GlobalHandle(pMem);      // convert pointer to handle
    #else
       {
       DWORD temp=GlobalHandle((UINT)(((DWORD)pMem)>>16));
       hMem=(HGLOBAL)LOWORD(temp);
       }
    #endif

    GlobalUnlock(hMem);

    if (NULL==(hMem=GlobalReAlloc(hMem,size+1, GMEM_MOVEABLE))) {
       PrintError(NULL,MSG_OUT_OF_MEM,"GlbRealloc");
       return NULL;
    }

    return (void far *) GlobalLock(hMem);
}

/******************************************************************************
    GlbFree:
    Free up the given memory object.  This object must have been allocated
    using the GlbAlloc routine.
*******************************************************************************/
void GlbFree(void far *pMem)
{
    HGLOBAL hMem;

    if (pMem!=NULL) {
       #if defined(WIN32)
          hMem=GlobalHandle(pMem);      // convert pointer to handle
       #else
          {
          DWORD temp=GlobalHandle((UINT)(((DWORD)pMem)>>16));
          hMem=(HGLOBAL)LOWORD(temp);
          }
       #endif

       if (hMem) {
          GlobalUnlock(hMem);
          GlobalFree(hMem);
       }
    }
}


/******************************************************************************
    FarMemSet:
    This function every byte of the given far buffer to the specified character.
******************************************************************************/
void FarMemSet(void huge *ptr,BYTE chr, DWORD count)
{
    #if defined(WIN32)
       memset(ptr,chr,count);
    #else
       DWORD  l;
       BYTE huge *lptr;

       lptr=(BYTE huge *)ptr;

       for (l=0;l<(DWORD)count;l++) lptr[l]=chr;
    #endif
}

/******************************************************************************
    AllocTabwCharFlags:
    Allocate memory for the tabw character flags.
*******************************************************************************/
BOOL AllocTabwCharFlags(PTERWND w, long line)
{
    if (!tabw(line) && !AllocTabw(w,line)) return FALSE;

    if (NULL!=(tabw(line)->CharFlags=MemAlloc(LineLen(line)))) {
        FarMemSet(tabw(line)->CharFlags,0,LineLen(line));
        tabw(line)->CharFlagsLen=LineLen(line);
        return TRUE;
    }
    else return FALSE;
}

/******************************************************************************
    AllocTabw:
    Allocate memory for the tabw structure and initialize the structure.
*******************************************************************************/
BOOL AllocTabw(PTERWND w, long line)
{
    if (NULL!=(tabw(line)=MemAlloc(sizeof(struct StrTabw)))) {
        FarMemSet(tabw(line),0,sizeof(struct StrTabw));
        return TRUE;
    }
    else return FALSE;
}

/******************************************************************************
    FreeTabw:
    Free the memory for a tabw structure.
*******************************************************************************/
BOOL FreeTabw(PTERWND w, long line)
{
    if (tabw(line)) {
       FreeTabwMembers(w,tabw(line));
       MemFree(tabw(line));
       tabw(line)=NULL;
    }
    return TRUE;
}

/******************************************************************************
    FreeTabwMembers:
    Free tabw members
*******************************************************************************/
BOOL FreeTabwMembers(PTERWND w, struct StrTabw far *ptr)
{
    if (ptr) {
       if (ptr->CharFlags) MemFree(ptr->CharFlags);
       ptr->CharFlags=NULL;

       if (ptr->ListText)  MemFree(ptr->ListText);
       ptr->ListText=NULL;

       if (ptr->pListnum)  MemFree(ptr->pListnum);
       ptr->pListnum=NULL;
    }
    return TRUE;
}
 
/******************************************************************************
    CopyTabw:
    Copy tabw from one line to another.
*******************************************************************************/
BOOL CopyTabw(PTERWND w, long FromLine, long ToLine)
{
    if (tabw(ToLine)) FreeTabwMembers(w,tabw(ToLine));
    else if (!AllocTabw(w,ToLine)) AbortTer(w,"RAN OUT OF MEMORY (CopyTabw)",40);
    
    FarMove(tabw(FromLine),tabw(ToLine),sizeof(struct StrTabw));
    
    // copy member pointer data
    if (tabw(FromLine)->CharFlags) {
       tabw(ToLine)->CharFlags=NULL;
       if (AllocTabwCharFlags(w,ToLine)) FarMove(tabw(FromLine)->CharFlags,tabw(ToLine)->CharFlags,tabw(ToLine)->CharFlagsLen);
    }
    
    if (tabw(FromLine)->ListText) {
       tabw(ToLine)->ListText=MemAlloc(lstrlen(tabw(FromLine)->ListText)+1);
       lstrcpy(tabw(ToLine)->ListText,tabw(FromLine)->ListText);
    }  
    
    if (tabw(FromLine)->pListnum) {
       tabw(ToLine)->pListnum=MemAlloc((tabw(FromLine)->ListnumCount+1)*sizeof(struct StrListnum));
       FarMove(tabw(FromLine)->pListnum,tabw(ToLine)->pListnum,(tabw(FromLine)->ListnumCount)*sizeof(struct StrListnum));
    } 

    return TRUE;
}
 
/******************************************************************************
    sAlloc:
    Replacement of the Winmem package routine. Simply allocates the memory using
    malloc and returns it as a handle of 32 bit quantity.
*******************************************************************************/
LPVOID sAlloc(UINT size)
{
    void far *ptr;

    if (NULL==(ptr=farmalloc(size+1))) PrintError(NULL,MSG_OUT_OF_MEM,"sAlloc");

    return ptr;
}

/******************************************************************************
    sReAlloc:
    Replacement of the Winmem package routine. Simply reallocates the memory,
    and returns it as a handle of 32 bit quantity.
*******************************************************************************/
LPVOID sReAlloc(LPVOID pMem,UINT size)
{
    void far *ptr;

    if (NULL==(ptr=farrealloc(pMem,size+1))) PrintError(NULL,MSG_OUT_OF_MEM,"sReAlloc");

    return ptr;
}

/******************************************************************************
    sFree:
    Replacement of the Winmem package routine. Simply frees memory pointer.
*******************************************************************************/
int sFree(LPVOID pMem)
{

   if (pMem) farfree(pMem);

   return TRUE;
}

/******************************************************************************
    AllocLinePtr:
    This function simply allocates the memory for the line structure
*******************************************************************************/
BOOL AllocLinePtr(PTERWND w, long line)
{
    if (LinePtrCount>0) {     // allocate it from the cache
       LinePtrCount--;
       LinePtr[line]=LinePtrCache[LinePtrCount];
    }
    else {
       LinePtr[line]=MemAlloc(sizeof(struct StrLinePtr));
       if (NULL==LinePtr[line]) return PrintError(w,MSG_OUT_OF_MEM,"AllocLinePtr");
    }

    return TRUE;
}

/******************************************************************************
    FreeLinePtr:
    This function frees the memory for the line structure
*******************************************************************************/
BOOL FreeLinePtr(PTERWND w, long line)
{
    if (LinePtrCount<MAX_LINE_PTRS) {   // cache it
       LinePtrCache[LinePtrCount]=LinePtr[line];
       LinePtrCount++;
    }
    else MemFree(LinePtr[line]);

    LinePtr[line]=NULL;

    return TRUE;
}

/******************************************************************************
    MemAlloc:
    This function simply allocates using the runtime library
*******************************************************************************/
void far *MemAlloc(UINT size)
{
    void far *ptr;

    if (NULL==(ptr=farmalloc(size+1))) PrintError(NULL,MSG_OUT_OF_MEM,"MemAlloc");

    return ptr;
}

/******************************************************************************
    MemReAlloc:
    This function simply reallocates using the runtime library
*******************************************************************************/
void far *MemReAlloc(void far *InPtr,UINT size)
{
    void far *ptr;

    if (NULL==(ptr=farrealloc(InPtr,size+1))) PrintError(NULL,MSG_OUT_OF_MEM,"MemReAlloc");

    return ptr;
}

/******************************************************************************
    MemFree:
    This function simply frees a runtime library allocated memory block
*******************************************************************************/
int MemFree(void far *ptr)
{
   if (ptr) farfree(ptr);

   return TRUE;
}

/******************************************************************************
    MemAllocCopy:
    Allocate and copy.
*******************************************************************************/
void far *MemAllocCopy(LPBYTE pSrc, DWORD size)
{
    LPBYTE dest=MemAlloc(size+1);

    if (dest!=null) FarMove(pSrc,dest,size);
    return dest;
}

/******************************************************************************
    Copy a specified number of bytes from the source to the destination location.
    Both the source and destination locations are specified by using far pointers.

    When source and destination memory overlaps,  use FarMoveOl function for
    a proper trasfer.

    Small/Medium Model Note:  This function will not work if compiled
    as an EXECUTABLE under small/medium model using the Borland compiler.
    This is because Borland 'C' does not have a fmemmove library function.
*******************************************************************************/
void FarMove(void far * src,void far * dest, UINT count)
{
    #if defined(WIN32)
       memmove(dest,src,count);
    #else
       WORD i,SrcOff,DestOff,SrcSeg,DestSeg;
       BYTE huge *SrcPtr,huge *DestPtr;

       if (count==0) return;             // nothing to move

       SrcSeg=HIWORD((DWORD)(src));     // source segment
       DestSeg=HIWORD((DWORD)(dest));   // destination segment
       SrcOff=LOWORD((DWORD)(src));     // source offset
       DestOff=LOWORD((DWORD)(dest));   // destination offset

       // use _fmemmove function if source and destination do not wrap
       if ((SrcOff+count)>SrcOff && (DestOff+count)>DestOff) _fmemmove(dest,src,count);
       else {                            // move one by one if source of destination wrap
           SrcPtr=(BYTE huge *)src;
           DestPtr=(BYTE huge *)dest;

           if ( (GetSelectorBase(SrcSeg)+SrcOff)
              > (GetSelectorBase(DestSeg)+DestOff) ){  // source at higher address
              for (i=0;i<count;i++) DestPtr[i]=SrcPtr[i];
           }
           else {  // src at lower address, move in reverse order
              for (i=count-1;i!=0xFFFF;i--) DestPtr[i]=SrcPtr[i];
           }
       }

    #endif

    return;

}

/******************************************************************************
    Copy a specified number of bytes from the source to the destination location.
    Both the source and destination locations are specified by using far pointers.
*******************************************************************************/
void FarMoveOl(void far * src,void far * dest, UINT count)
{
    FarMove(src,dest,count);  // current version of FarMove takes care of overlapping
    return;
}

/******************************************************************************
    HugeMove:
    Use this function to move data from one buffer to another when the
    buffer may be larger than 64K.  This function handles overlapping
    buffers.
    Small/Medium Model Note:  This function will not work if compiled
    as an EXECUTABLE under small/medium model using the Borland compiler.
    This is because Borland 'C' does not have a fmemmove library function.

    Note that this restriction is only for an executable and not for a DLL.
*******************************************************************************/
void HugeMove(void huge *src,void huge * dest, long count)
{
    #if defined(WIN32)
       memmove(dest,src,count);
    #else
       long BuffSize=0x1000; // 4 K
       long BytesMoved=0;
       long MoveSize;
       void far *CurSrc,far *CurDest;
       BYTE huge *CharSrc;      // source cast as character pointer
       BYTE huge *CharDest;     // destination cast as character pointer



       if (count<=0 || src==dest) return;

       if (count<=0xFFFFL) {   // less than 64 K
          FarMoveOl(src,dest,(WORD)count);
          return;
       }

       // move in 64K blocks now
       CharSrc=(BYTE huge *)src;
       CharDest=(BYTE huge *)dest;

       while (count>0) {
          // calculate amount to move
          if (count>=BuffSize) MoveSize=BuffSize;
          else                 MoveSize=count;

          // calculate source and destination addresses
          if (src>dest) {       // move from the top
             CurSrc=(void far *)&CharSrc[BytesMoved];
             CurDest=(void far *)&CharDest[BytesMoved];
          }
          else {               // move from the bottom to handle overlapping block
             CurSrc=(void far *)&CharSrc[count-MoveSize];
             CurDest=(void far *)&CharDest[count-MoveSize];
          }

          // move this block
          FarMoveOl(CurSrc,CurDest,(WORD)MoveSize);
          count-=MoveSize;      // bytes left to move
          BytesMoved+=MoveSize;
       }
    #endif

    return;
}

/******************************************************************************
    GetStringField:
    Get a field from the string delimited by the given delimiter.
******************************************************************************/
LPBYTE GetStringField(LPBYTE in, int nbr, BYTE delim, LPBYTE out)
{
   int i,StartPos,len;

   out[0]=0;
   
   if (in==null) return out;

   len=strlen(in);
   for (i=0;i<len;i++) {
      if (i>0 && in[i-1]==delim) nbr--;
      if (nbr==0) break;                // no positioned on the field
   } 
   if (i==len) return out;

   StartPos=i;
   for (i=StartPos;i<len;i++) {
      if (in[i]==delim) break;         // this field ends here
      out[i-StartPos]=in[i];
   } 
   out[i-StartPos]=0;

   return out;
}

/******************************************************************************
    Substring:
    Extract a substring. When count is -1, substring is extracted to the end
    of the string.  This function returns the pointer to the second string
******************************************************************************/
LPBYTE Substring(LPBYTE in, int StartPos, int count, LPBYTE out)
{
    int len,i;

    out[0]=0;

    len=strlen(in);
    if (count<0) count=len-StartPos;
    for (i=0;i<count;i++) out[i]=in[StartPos+i];
    out[i]=0;

    return out;
} 

/******************************************************************************
    FarStringIndex:
    This function returns the position of the second string within the first
    string.  The position is an index from the beginning of the first string.
    If the string is located, the return value will range from 0 to length of
    first string minus the length of the second string.
    If the second string could not be located,  the return value is equal to
    the length of the first string.
******************************************************************************/
UINT FarStringIndex(LPBYTE string1,LPBYTE string2)
{
    unsigned i,j,k,len1,len2,SearchLen;
    BYTE FirstChar;

    len1=lstrlen(string1);
    len2=lstrlen(string2);
    if (len2>len1) return len1;    // the second string too big to fit within the first string

    FirstChar=string2[0];                // first character of the second string

    i=0;
    do {
        SearchLen=len1-len2-i+1;         // length of the string to search
        j=FarCharIndex(&string1[i],FirstChar,SearchLen);
        if (j==SearchLen) return len1;   // first character of the second string in the first string not found

        for (k=1;k<len2;k++) if (string1[i+j+k]!=string2[k]) goto CONTINUE_LOOP;
        return (i+j);

        CONTINUE_LOOP:
        i=i+j+1;                         // repeat after the first character match location
    } while (i<=(len1-len2));

    return len1;                         // the string was not located
}

/******************************************************************************
    FarStringRevIndex:
    This function returns the position of the second string within the first
    string.  This function scans the first string in the reverse order.  The
    third argument specifies the length of the string1 to be scanned. Ex. if
    length=10, then the first string will be scanned from position 9 in the
    reverse order until position 0 is scanned.  The function returns the
    index of the located string from the beginning of the first string.

    Reverse string search is slower than forward string search (FarStringIndex);
*******************************************************************************/
UINT FarStringRevIndex(LPBYTE string1,LPBYTE string2,UINT len)
{
    UINT i,k,len2;

    len2=lstrlen(string2);
    if (len2>len) return len;    /* string to big to fit */

    i=len-len2;

    do {
       for (k=0;k<len2;k++) if (string1[i+k]!=string2[k]) break;
       if (k==len2) return i;             /* string located */
       if (i==0) break;else i--;
    } while (TRUE);

    return len;                             /* string not located */
}


/******************************************************************************
    FarCharIndex:
    This function returns the position of the specified character within the
    given string.  The position is an index from the beginning of the string.
    If the character is found, the return value will range from 0 to SearchLen
    minus 1 (index of the character).
    If the character could not be located,  the return value is equal to
    SearchLen.
******************************************************************************/
UINT FarCharIndex(LPBYTE string,BYTE chr, UINT SearchLen)
{
    UINT i;

    for (i=0;i<SearchLen;i++) if (string[i]==chr) return i;
    return i;
}

/******************************************************************************
    DebugFct:
    Debugging function.
******************************************************************************/
DebugFct(PTERWND w)
{
    return TRUE;
}

/******************************************************************************
    RecordMem:
    Debugging function.
******************************************************************************/
RecordMem(PTERWND w,LPVOID pMem,BOOL alloc,LPBYTE msg)
{
    static LPVOID *pArray=null;
    static int count=0;
    static int max=0;
    int i;

    if (pArray==null) {
       max=400;
       pArray=MemAlloc(sizeof(LPVOID)*(max+1));
    }
    
    if (count>=max) {
       max=count+400;
       pArray=MemReAlloc(pArray, sizeof(LPVOID)*(max+1));
    }     
 
    if (alloc) {
      for (i=0;i<count;i++) if (pArray[i]==null) break;
      if (i<count) pArray[i]=pMem; // record
      else {
         i=count;
         count++;
         pArray[i]=pMem;
      }
      //OurPrintf("Pointer recorded, %s %x",msg,pMem); 
    }
    else {
      for (i=0;i<count;i++) if (pArray[i]==pMem) break;
      if (i==count) OurPrintf("pointer to be freed NOT found, %s %x",msg,pMem);
      else {
         pArray[i]=null;
         OurPrintf("pointer to be freed found, %s %x",msg,pMem);
      }
    }  

    return true;
} 
 
