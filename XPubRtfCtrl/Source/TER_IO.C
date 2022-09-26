/*===============================================================================
   TER_IO.C
   TER file and buffer I/O routines.

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


/*****************************************************************************
    TerRead:
    Read the input file or parse the input buffer to separate each text line.
    The routine return a TRUE value when successful.
******************************************************************************/
BOOL TerRead(PTERWND w,LPBYTE InputFile)
{
    int  rMode=4,CurLen,i,SignLen,
         GetCfmt=FALSE,MaxLineExceeded=FALSE;
    HANDLE InFile;
    long l,BufferPtr=0,FilePos;
    BYTE huge *buffer=NULL;
    LPBYTE ptr;
    BYTE line[MAX_WIDTH+2],lead[MAX_WIDTH+2],SaveByte=0;
    int  result,LangFont;
    HCURSOR hSaveCursor=NULL;
    BOOL CheckCrLf=TRUE,AppendParaChar;

    SignLen=lstrlen(CfmtSign);      // length of character formatting signature

    if (TerArg.InputType=='B') {    // lock the buffer
         if (TerArg.hBuffer==NULL) return TRUE;    // empty buffer

         if (NULL==(buffer=GlobalLock(TerArg.hBuffer))) return PrintError(w,MSG_ERR_MEM_LOCK,"TerRead");
         if (RtfRead(w,RTF_BUF,NULL,buffer,TerArg.BufferLen)) {
            GlobalUnlock(TerArg.hBuffer);
            return TRUE;  // file in RTF format read
         }
         ReadFileInfo(w,InputFile,buffer,TerArg.InputType,FALSE);

         if (TerArg.delim==0xD) CrLfUsed=TRUE;   // will be reset if 0xA not found
         else                   CrLfUsed=FALSE;
    }
    else {                          // open the input file
         //lstrupr(w,InputFile);
         rTrim(InputFile);          // trim any spaces on right
         if (lstrlen(InputFile)==0) return TRUE;
         lstrcpy(TempString,InputFile);   // make a copy in the data segment
         if (_access(TempString,rMode)==-1) {
             if (IDNO==MessageBox(hTerWnd,MsgString[MSG_CREATE_NEW_FILE],InputFile,MB_YESNO)) {
                 CloseTer(hTerWnd,TRUE);
                 return FALSE;
             }
             else return TRUE;  // the file will be created at update time
         }

         if (RtfRead(w,RTF_FILE,InputFile,NULL,0)) return TRUE;  // file in RTF format read

         ReadFileInfo(w,InputFile,buffer,TerArg.InputType,FALSE);

         lstrcpy(TempString,InputFile);   // make a copy in the data segment
         
         if ((InFile=CreateFile(TempString,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
             return PrintError(w,MSG_ERR_FILE_OPEN,"TerRead");
    }

    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while reading

    // set the total lines to 0
    for (l=0;l<TotalLines;l++) FreeLine(w,l);
    TotalLines=0;

    // check for unicode read
    if (FileFormat==SAVE_UTEXT) {
       long MaxBufSize=TerArg.BufferLen;
       if (TerArg.InputType=='F') MaxBufSize=GetFileLength(InputFile);
       CurLine=0;
       UnicodeRead(w,TerArg.InputType,buffer,InFile,MaxBufSize,TRUE);
       goto END_FILE;
    } 
    
    if (DefLang!=ENGLISH) LangFont=SetCurLangFont2(w,0,(HKL)DefLang);

    READ_LINE:

    CurLen=0;
    if (SaveByte) {               // mbcs byte waiting from the previous line
       line[CurLen]=SaveByte;
       SaveByte=0;
       CurLen++;
    }

    if (TerArg.InputType=='F') {  // file input
       result=ReadLine(&(line[CurLen]),MAX_WIDTH-CurLen,InFile);
       if (result==-1) {
           PrintError(w,MSG_ERR_FILE_READ,"TerRead");
           goto END_FILE;
       }
       if (lstrlen(line)==CurLen) goto END_FILE;
    }
    else {   // read from input buffer
       if (BufferPtr>=TerArg.BufferLen) goto END_FILE;  // end of file
       i=CurLen;
       while (BufferPtr<TerArg.BufferLen && buffer[BufferPtr]!=TerArg.delim && i<(MAX_WIDTH-2)) {
          line[i]=buffer[BufferPtr];
          i++;
          BufferPtr++;
       }
       // check if line delimiter found
       line[i]=0;
       if (BufferPtr<TerArg.BufferLen && buffer[BufferPtr]==TerArg.delim) {
          line[i]=0xa;       // simulate a read from disk
          i++;line[i]=0;
       }

       // check if cr/lf pair used for line delimiter
       if (CheckCrLf) {
          if (TerArg.delim==0xD && buffer[BufferPtr]==TerArg.delim  && (BufferPtr+1)<TerArg.BufferLen && buffer[BufferPtr+1]!=0xA) CrLfUsed=FALSE; // cr/lf pair not found
          CheckCrLf=FALSE;
       }
       if (CrLfUsed && buffer[BufferPtr]==TerArg.delim  && (BufferPtr+1)<TerArg.BufferLen && buffer[BufferPtr+1]==0xA) BufferPtr++; // pass over cr/lf pair
       if (i>0 && line[i-1]==0xa) BufferPtr++;  // go past line delimiter
    }

    // break the text line into lead and tail bytes
    if (mbcs && FileFormat==SAVE_TEXT) {
       CurLen=(int)TerSplitMbcs(hTerWnd,line,line,lead);
       if (CurLen>0 && line[CurLen-1]==0) {    //partial character
          SaveByte=lead[CurLen-1];             // save it for the next read
          CurLen--;
          line[CurLen]=lead[CurLen]=0;
       }
    }

    CurLen=lstrlen(line);     // length of the line

    AppendParaChar=FALSE;
    if (CurLen>0 && line[CurLen-1]==0xa) {
       if (FileFormat==SAVE_TEXT && TerArg.WordWrap) AppendParaChar=TRUE;  // convert line feed into para character
       CurLen--;                 // discard line feed
    }

    line[CurLen]=0;
    if (CurLen==SignLen && lstrcmp(line,CfmtSign)==0) {
       GetCfmt=TRUE;
       goto END_FILE;              // character formatting information begins
    }

    if (AppendParaChar) {
       line[CurLen]=ParaChar;
       lead[CurLen]=0;
       CurLen++;
       line[CurLen]=0;
    }

    if (MaxLineExceeded) goto READ_LINE;   // this line can not be stored

    if (CurLen>MAX_WIDTH) {
         PrintError(w,MSG_LINE_TRUNCATED,"TerRead");
         CurLen=MAX_WIDTH-1;
    }

    //Convert old paragraph/cell markers to new markers
    if (FileFormat==SAVE_TER && FileInfo.RevId<9) {
       for (i=0;i<CurLen;i++) {
         if (line[i]==OldParaChar) line[i]=ParaChar;
         if (line[i]==OldCellChar) line[i]=CellChar;
       }
    }

    // allocate and store line
    if (!CheckLineLimit(w,TotalLines+1)) {
        HoldMessages=TRUE;               // data structure being built
        PrintError(w,MSG_LINE_TRUNCATED,"TerRead");
        HoldMessages=FALSE;
        MaxLineExceeded=TRUE;
        goto READ_LINE;           // no more lines will be stored
    }

    TotalLines++;
    CurLine=TotalLines-1;
    InitLine(w,CurLine);

    // set tabw flags
    if (CurLen==1) {
       if (line[0]==ROW_CHAR || line[0]==CellChar) {
          AllocTabw(w,CurLine);
          if (tabw(CurLine)) {
              if   (line[0]==ROW_CHAR) tabw(CurLine)->type|=INFO_ROW;
              if   (line[0]==CellChar) tabw(CurLine)->type|=INFO_CELL;
          }
       }
       SetHdrFtrLineFlags(w,CurLine,line[0]);
    }

    LineAlloc(w,CurLine,0,CurLen); // allocate space for the line
    ptr=pText(CurLine);
    if (CurLen>0) FarMove(line,ptr,CurLen);
    if (mbcs && FileFormat==SAVE_TEXT && CurLen>0) {
       OpenLead(w,CurLine);
       FarMove(lead,pLead(CurLine),CurLen);
       CloseLead(w,CurLine);
    }
    
    // set default language font, if font language not english
    if (DefLang!=ENGLISH && cfmt(CurLine).info.type==UNIFORM && FileFormat==SAVE_TEXT) {
       if (FileFormat!=SAVE_TER) {       // apply non-english fonts
          LPWORD fmt=OpenCfmt(w,CurLine);
          for (i=0;i<LineLen(CurLine);i++) {
             if (!IsEnglishChar(w,ptr,i,LineLen(CurLine))) fmt[i]=LangFont;
          }
          CloseCfmt(w,CurLine);

          if (TerFlags5&TFLAG5_INPUT_TO_UNICODE) {
            if (mbcs) MbcsToUnicode(w);
            else TextToUnicode(w);
          }

       } 
    }



    goto READ_LINE;                 // read next line

    END_FILE:
    if (TotalLines==0) {
       TotalLines=1;
       InitLine(w,0);
    }

    if (TerArg.InputType=='F') {
       FilePos=SetFilePointer(InFile,0,NULL,FILE_CURRENT);  // get the file pointer
       CloseHandle(InFile);
    }
    else GlobalUnlock(TerArg.hBuffer);

    if (GetCfmt && FileFormat==SAVE_TER) {
       BOOL result;
       if (TerArg.InputType=='F') result=ReadCfmt(w,InputFile,buffer,FilePos,'F');
       else                       result=ReadCfmt(w,InputFile,buffer,BufferPtr,'B');
       if (!result) return FALSE;
    }

    if (TotalLines==1 && LineLen(0)==0) FileFormat=SAVE_TER; // reset to the default format

    // adjust the section data
    if (True(TerFlags&TFLAG_APPLY_PRT_PROPS)) {
       if (FileFormat==SAVE_TER && FileInfo.RevId>=24 && FileInfo.PpSize!=PaperSize) TerSetPaperEx(hTerWnd,FileInfo.PpSize,FileInfo.PpWidth,FileInfo.PpHeight,FileInfo.PpOrient,FALSE);
       if (pDeviceMode) ApplyPrinterSetting(w,pDeviceMode->dmOrientation,pDeviceMode->dmDefaultSource,PaperSize,PageWidth,PageHeight);
    }

    if (!AttachSectionData(w)) return FALSE; // attach the section data to the section breaks
    if (!XlateTextInputField(w)) return FALSE;  // translate old type textinput field to new type

    CurLine=CurRow=CurCol=0;
    PageModifyCount=-1;       // force repagination

    if (TerArg.WordWrap) {
       BOOL ParaFramesFound=FALSE;
       InRtfRead=TRUE;        // to enable special consideration at load time
       TerOpFlags2|=TOFLAG2_SSE_READ;

       if (TerArg.PageMode) Repaginate(w,FALSE,FALSE,0,TRUE);     // wrap all lines
       else                 WordWrap(w,0,TotalLines);   // wrap the rtf text

       if (TerArg.PageMode) {
          for (i=1;i<TotalParaFrames;i++) {
             if (ParaFrame[i].InUse) {
                ParaFramesFound=TRUE;
                break;
             }
          }
       }

       // header/footer processing
       ReposPageHdrFtr(w,FALSE);
       if (ParaFramesFound && !ViewPageHdrFtr && TerArg.PageMode
          && !(TerFlags2&TFLAG2_NO_AUTO_HDR_FTR)) {
          BOOL SavePaintEnabled=PaintEnabled;
          PaintEnabled=FALSE;
          ToggleViewHdrFtr(w);
          PaintEnabled=SavePaintEnabled;
       }

       if (ParaFramesFound) {    // repaginate one more time
          TerRepaginate(hTerWnd,TRUE);
          //RepageBeginLine=0;
       }
       if (ViewPageHdrFtr && TerArg.PageMode) {   // position at the first header line
          BOOL LineSet=FALSE;
          for (l=0;l<TotalLines;l++) {
             if ((PfmtId[pfmt(l)].flags&PAGE_HDR) && !(LineFlags(l)&LFLAG_HDRS)) {
                if (!LineSet) CurLine=l;
                LineSet=TRUE;
                if (l>0 && LineFlags(l-1)&LFLAG_FHDR) {
                   CurLine=l;
                   break;
                }
             }
             if (!(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)) break;
          }
       }
       if (ParaFramesFound) {    // repaginate one more time
          TerRepaginate(hTerWnd,TRUE);
          RepageBeginLine=0;
       }
       InRtfRead=FALSE;
       ResetLongFlag(TerOpFlags2,TOFLAG2_SSE_READ);
    }

    UpdateToolBar(w,FALSE);

    TerArg.modified=0;
    RepageBeginLine=0;       // repage again on idle time
    PageModifyCount=TerArg.modified-1;       // force repagination

    if (hSaveCursor) SetCursor(hSaveCursor);

    return TRUE;
}

/*****************************************************************************
    TerNew:
    Clear the old document.
******************************************************************************/
TerNew(PTERWND w,LPBYTE file)
{
    int result;
    BYTE SaveInputType;

    // set the input type to 'buffer' temporariy
    SaveInputType=TerArg.InputType;
    if (TerArg.InputType!='F') {
       TerArg.InputType='F';
       if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;
    }

    // save the
    if (InIE && TerArg.modified==FILE_OPEN_IN_IE) TerArg.modified=0;   // special mod count that does not mean actual file modification
    if (TerArg.modified) {
       if (IDCANCEL==(result=MessageBox(hTerWnd,MsgString[MSG_SAVE_MODS],"",MB_YESNOCANCEL))) return FALSE;

       if (result==IDYES) {
          if (lstrlen(DocName)==0) result=TerSaveAs(w,DocName);
          else                         result=TerSave(w,DocName,TRUE);
          if (!result) return FALSE;
       }
    }

    ReadTerFile(hTerWnd,file);

    TerShrinkFontTable();        // shrink the font table

    SetWindowText(hTerWnd,file);

    TerArg.InputType=SaveInputType;
    lstrcpy(DocName,file);

    TerInvalidateRect(w,NULL,FALSE);     // to restore any windown 'validation' that an embedded control might have caused

    if (InIE) TerArg.modified=FILE_OPEN_IN_IE;   // so that the new text gets saved to the server

    return TRUE;
}

/*****************************************************************************
    TerOpen:
    Prompt the user for a new document and read the new document.
******************************************************************************/
TerOpen(PTERWND w)
{
    BYTE InFile[MAX_WIDTH]="",FileOpt[150];
    BOOL result;
    int  FileIndex=1;

    if      (UserFileType==SAVE_TER)   FileIndex=1;   // native files
    else if (UserFileType==SAVE_RTF)   FileIndex=3;   // rtf files
    else if (UserFileType==SAVE_UTEXT) FileIndex=4;   // rtf files
    else                               FileIndex=2;   // text files

    if (Win32) lstrcpy(FileOpt,"SSE Format(*.SSE)|*.SSE|Text Format(*.TXT)|*.TXT|Rich Text Format(*.RTF)|*.RTF|");
    else       lstrcpy(FileOpt,"Ter Format(*.TER)|*.TER|Text Format(*.TXT)|*.TXT|Rich Text Format(*.RTF)|*.RTF|");

    if (Win32) lstrcat(FileOpt,"Unicode Text File(*.TXT)|*.TXT|");

    result=GetFileName(w,TRUE,InFile,FileIndex,FileOpt);

    if (!result) return FALSE;

    if (lstrcmpi(InFile,DocName)==0) return TRUE;   // same file

    return TerNew(w,InFile);
}

/*****************************************************************************
    ReadLine:
    This function siumlates 'fgets' for a file handle.  
******************************************************************************/
ReadLine(LPBYTE out, int count, HANDLE InFile)
{
   int i,BytesRead,SeekBytes;

   out[0]=0;

   // read the file
   ReadFile(InFile,out,count,&((DWORD)BytesRead),NULL);
   if (BytesRead==0) {
      out[0]=0;
      return 0;
   }

   // look for a cr/lf pair
   for (i=0;i<(BytesRead-1);i++) {
      if (out[i]==0xd && out[i+1]==0xa) {
         // reposition the file pointer after cr/lf
         SeekBytes=i+2-BytesRead;
         if (SeekBytes!=0) SetFilePointer(InFile,SeekBytes,NULL,FILE_CURRENT);

         out[i]=0xa;      // convert cf/lf to lf
         out[i+1]=0;
         return 0;
      }
   }
   out[BytesRead]=0;

   return 0;
}

/*****************************************************************************
    XlateTextInputField:
    Convert old type text input field to new type.
******************************************************************************/
BOOL XlateTextInputField(PTERWND w)
{
    long line=0,pos;
    int  pict,col=0;
    struct StrForm far *pForm;
    
    while (TerLocateFieldChar(hTerWnd,FIELD_TEXTBOX,NULL,TRUE,&line,&col,TRUE)) {
       CurLine=line;
       CurCol=col;
       pos=RowColToAbs(w,line,col);
       col++;               // advance

       pict=GetCurCfmt(w,CurLine,CurCol);  // picture font
       if (!IsFormField(w,pict,0)) continue;
       if (TerFont[pict].FieldId!=FIELD_TEXTBOX) continue;

       pForm=(LPVOID)TerFont[pict].pInfo;

       MoveLineData(w,CurLine,CurCol,1,'D');  // delete this picture position

       //TerInsertTextInputField(hTerWnd,pForm->name,pForm->InitText,pForm->MaxLen,pForm->style&WS_BORDER,pForm->typeface,pForm->TwipsSize,pForm->FontStyle,pForm->TextColor,true,false);
// MAK
       TerInsertTextInputField(hTerWnd,pForm->name,pForm->InitText,pForm->MaxLen,pForm->style&WS_BORDER,pForm->typeface,
        pForm->TwipsSize,pForm->FontStyle,pForm->TextColor,true, 0, NULL, (LPBYTE)"", (LPBYTE)"EDIT", false);
       
			 pos+=strlen(pForm->InitText);
 
			 
			 AbsToRowCol(w,pos,&line,&col);
       
       DeleteTerObject(w,pict);             // picture converted
       
       col++;                               // advance to look for the next field
    } 

    return TRUE;
} 

/*****************************************************************************
    AttachSectionData:
    Attach section data to the section break lines.
******************************************************************************/
BOOL AttachSectionData(PTERWND w)
{
    long line;
    int i;

    if (!TerArg.WordWrap) return TRUE;

    // set the section id for the line
    for (i=1;i<TotalSects;i++) {
       if (!TerSect[i].InUse) continue;
       line=TerSect[i].LastLine;

       if (line<0 || line>=TotalLines) return PrintError(w,MSG_INVALID_SECT_DATA,NULL);
       if (!AllocTabw(w,line)) return PrintError(w,MSG_OUT_OF_MEM,"AttachSectionData");

       OpenCfmt(w,line);

       tabw(line)->type=INFO_SECT;
       tabw(line)->section=i;
       tabw(line)->count=0;

       CloseCfmt(w,line);

       // set section tag for older file formats
       if (FileInfo.RevId<37) {
          LPBYTE ptr=pText(line);
          int len=LineLen(line);
          if (len>0 && ptr[len-1]==SECT_CHAR) {
             BYTE string[20];
             wsprintf(string,"Section%d",i);
             SetTag(w,line,len-1,TERTAG_SECT,string,NULL,i);   // insert the section tag
          } 
       } 
    }

    SetSectPageSize(w);         // set the page sizes for the section

    if (TotalSects>1) RecreateSections(w);   // create section boundaries

    return TRUE;
}

/*******************************************************************************
    TerSaveAs:
    Save the text to the output file. As for the file name.
*******************************************************************************/
BOOL TerSaveAs(PTERWND w,LPBYTE OutFile)
{
    BOOL result;
    BYTE FileOpts[300];

    if (Win32) lstrcpy(FileOpts,"SSE Format(*.SSE)|*.SSE|Text Format(*.TXT)|*.TXT|Text with Line Breaks|*.TXT|Rich Text Format(*.RTF)|*.RTF|");
    else       lstrcpy(FileOpts,"Ter Format(*.TER)|*.TER|Text Format(*.TXT)|*.TXT|Text with Line Breaks|*.TXT|Rich Text Format(*.RTF)|*.RTF|");
    if (Win32) lstrcat(FileOpts,"Unicode Text Format(*.TXT)|*.TXT|");

    if (HtmlAddOnFound(w)) lstrcat(FileOpts,"HTML Format(*.HTM)|*.HTM|");

    if (!(result=GetFileName(w,FALSE,OutFile,1,FileOpts))) return FALSE;

    if (TerArg.SaveFormat==SAVE_HTML) {
       LoadHtmlAddOn(w); 
       result=HtsSaveFromTer(hTerWnd,TRUE,OutFile,NULL,NULL);
       FreeHtmlAddOn(w);
       return result;
    }
    else return(TerSave(w,OutFile,TRUE));
}

/*******************************************************************************
    TerSave:
    Save the text to the output file or output buffer.
*******************************************************************************/
BOOL TerSave(PTERWND w,LPBYTE OutFile,BOOL ToFile)
{

    int  ExistMode=0,rwMode=6,i,CurFont,len,result,SignLen,OutputFormat;
    HANDLE hFile=INVALID_HANDLE_VALUE;
    DWORD written;
    long lin,BufferPtr=0,SaveBufLen,FilePos,FormatLen,DbLen;
    BYTE line[MAX_WIDTH+2],BkpFile[MAX_WIDTH+1],LastChar;
    BYTE huge *buffer=NULL;
    LPBYTE ptr,lead;
    LPWORD fmt;
    HCURSOR hSaveCursor=NULL;
    BOOL TextOutput;
    LPBYTE pStr,pDB=NULL;
    BYTE SaveInputType=0;
    LPBYTE pXlateText;
    int    XlateLen;
    WORD   UcBase;
    BYTE   XlateLead,XlateChar;
    

    if (restrict) return TRUE;

    // set the input type to 'buffer' temporariy
    if (ToFile && TerArg.InputType!='F') {
       SaveInputType=TerArg.InputType;
       TerArg.InputType='F';
       if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;
    }

    // recalculate section boundaries before saving
    RecreateSections(w);

    // decide the output format
    if (TerArg.SaveFormat==SAVE_DEFAULT) OutputFormat=FileFormat;        // save as input
    else                                 OutputFormat=TerArg.SaveFormat; // save to specified format
    TextOutput=(OutputFormat==SAVE_TEXT || OutputFormat==SAVE_TEXT_LINES);

    if (TerArg.InputType=='B') {            // initialization of buffer input
       if (OutputFormat==SAVE_RTF)   return RtfWrite(w,RTF_BUF,NULL);

       RemoveDeletedObjects(w);        // reset InUse flag for the deleted objects

       SaveBufLen=TerArg.BufferLen;
       TerArg.BufferLen=0;
       if (TerArg.delim!=0xD) CrLfUsed=FALSE;

       // get the buffer size requirement
       if (OutputFormat==SAVE_UTEXT) {
          TerArg.BufferLen=2;   // to store the signature bytes
          for(lin=0;lin<TotalLines;lin++) TerArg.BufferLen+=(LineLen(lin)+1)*sizeof(WORD);
       }
       else { 
          for(lin=0;lin<TotalLines;lin++) TerArg.BufferLen+=(LineLen(lin)+1+((TextOutput && mbcs)?(LineLen(lin)+1):0));
          if (CrLfUsed) TerArg.BufferLen=TerArg.BufferLen+TotalLines;  // add one byte for each line for the 0xa character
          if (OutputFormat==SAVE_TER && (FormatLen=CfmtNeeded(w))>0) {
             TerArg.BufferLen=TerArg.BufferLen+lstrlen(CfmtSign)+1+FormatLen;
             if (CrLfUsed) TerArg.BufferLen++; // add one byte for 0xa after the signature line
          }
       }

       if (SaveBufLen!=TerArg.BufferLen) {
          if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
          if (NULL==(TerArg.hBuffer=GlobalAlloc(GMEM_MOVEABLE,TerArg.BufferLen))) {
             AbortTer(w,MsgString[MSG_OUT_OF_MEM],50);
          }
       }
       if (NULL==(buffer=GlobalLock(TerArg.hBuffer))) {
          AbortTer(w,MsgString[MSG_OUT_OF_MEM],51);
       }
    }
    else {                                  // initialization of file input
       // Get file name if not available
       if (lstrcmp(OutFile,"")==0) {        // get file name, if not available
          BYTE FileOpts[300];

          if (Win32) lstrcpy(FileOpts,"SSE Format(*.SSE)|*.SSE|Text Format(*.TXT)|*.TXT|Text with Line Breaks|*.TXT|Rich Text Format(*.RTF)|*.RTF|");
          else       lstrcpy(FileOpts,"Ter Format(*.TER)|*.TER|Text Format(*.TXT)|*.TXT|Text with Line Breaks|*.TXT|Rich Text Format(*.RTF)|*.RTF|");

          if (HtmlAddOnFound(w)) lstrcat(FileOpts,"HTML Format(*.HTM)|*.HTM|");

          if (!(result=GetFileName(w,FALSE,OutFile,1,FileOpts))) return FALSE;

          
          if (TerArg.SaveFormat==SAVE_DEFAULT) OutputFormat=FileFormat;        // save as input
          else                                 OutputFormat=TerArg.SaveFormat; // save to specified format
       }

       // save in the html format if requested
       if (TerArg.SaveFormat==SAVE_HTML) {
          BOOL result;
          LoadHtmlAddOn(w);
          result=HtsSaveFromTer(hTerWnd,TRUE,OutFile,NULL,NULL);
          FreeHtmlAddOn(w);
          return result;
       }

      // Backup existing file
       lstrcpy(TempString,OutFile);         // make a copy in the data segment
       if (_access(TempString,ExistMode)==0) {   // backup the previous file
          if (_access(TempString,rwMode)==-1) return PrintError(w,MSG_READ_ONLY,"TerRead");     // read-only file
          if (TerFlags2&TFLAG2_NO_BKP_FILE) unlink(TempString);
          else {
             lstrcpy(BkpFile,OutFile);         // build a backup file name with .TE extension
             i=lstrlen(BkpFile)-1;
             while(i>=0 && i>=lstrlen(BkpFile)-4 && BkpFile[i]!='.' && BkpFile[i]!='\\') i--;
             if (i>=0 && BkpFile[i]=='.') BkpFile[i]=0;
             lstrcat(BkpFile,".BU");
             lstrcpy(TempString,BkpFile);      // make a copy in the data segment
             if (_access(TempString,ExistMode)==0) unlink(TempString);
             lstrcpy(TempString1,OutFile);     // make a copy in the data segment
             rename(TempString1,TempString);
          }
       }

       if (OutputFormat==SAVE_RTF) {
           BOOL result=RtfWrite(w,RTF_FILE,OutFile);  // save in rtf format
           if (SaveInputType!=0) TerArg.InputType=SaveInputType;
           return result;
       }

       RemoveDeletedObjects(w);        // reset InUse flag for the deleted objects

       FilePos=0L;
       if (INVALID_HANDLE_VALUE==(hFile=CreateFile(OutFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) {
           return PrintError(w,MSG_ERR_FILE_OPEN,"TerSave");
       }
    }

    if (!(TerFlags&TFLAG_NO_HOUR_GLASS) && TerArg.InputType=='F') hSaveCursor=SetCursor(hWaitCursor);   // show hour glass while waiting

    if (OutputFormat==SAVE_TEXT_LINES) TerRewrap(hTerWnd);   // recreate text lines

    if (TextOutput && mbcs) pDB=OurAlloc(2*(MAX_WIDTH+2));

    // write unicode text
    if (OutputFormat==SAVE_UTEXT) {
       BufferPtr=UnicodeWrite(w,buffer,hFile,TerArg.BufferLen);
       goto END;
    }

    //WRITE_LINE:

    for (lin=0;lin<TotalLines;lin++) {
        ptr=pText(lin);
        
        //LogPrintf("line: %d, mbcs: %d, to: %d",lin,mbcs,TextOutput);
        pXlateText=NULL;
        XlateLen=DbLen=0;
        if (TextOutput) {
           if (mbcs) CloseLead(w,lin);   // close if it is already open
           if (mbcs && pLead(lin)) {     // lead found
              lead=OpenLead(w,lin);
              DbLen=(long)TerMakeMbcs(hTerWnd,pDB,ptr,lead,LineLen(lin));
              CloseLead(w,lin);
           }
           else {           // check for the unicode characters
              fmt=OpenCfmt(w,lin);  // check if unicode exists
              for (i=0;i<LineLen(lin);i++) if (TerFont[fmt[i]].UcBase>0) break;
              //LogPrintf("  i: %d, len: %d",i,LineLen(lin));
              if (i<LineLen(lin)) {   // unicode found - convert unicode
                 pXlateText=MemAlloc(2*LineLen(lin)+1);  // allocate enough space
                 XlateLen=0;
                 for (i=0;i<LineLen(lin);i++,XlateLen++) {
                     CurFont=fmt[i];
                     UcBase=TerFont[CurFont].UcBase;
                     if (UcBase!=0 && UnicodeToMultiByte(w,MakeUnicode(w,UcBase,ptr[i]),&XlateLead,&XlateChar,CurFont)) {
                         if (XlateLead!=0) {
                            pXlateText[XlateLen]=XlateLead;
                            XlateLen++;
                         }
                         pXlateText[XlateLen]=XlateChar;
                     }  
                     else pXlateText[XlateLen]=ptr[i];
                 }
                 pXlateText[XlateLen]=0; 
              } 
              CloseCfmt(w,lin);
           } 
        }

        if (TerArg.InputType=='B') {         // write the line to the buffer
            if (TextOutput && pXlateText!=NULL) {
               HugeMove(pXlateText,&(buffer[BufferPtr]),XlateLen);
               BufferPtr+=XlateLen;
            }
            else if (TextOutput && mbcs && DbLen>0) { 
               HugeMove(pDB,&(buffer[BufferPtr]),DbLen);
               BufferPtr+=DbLen;
            }
            else {
               HugeMove(ptr,&(buffer[BufferPtr]),(long)LineLen(lin));
               for (i=0;i<LineLen(lin);i++) if (buffer[BufferPtr+i]==0) buffer[BufferPtr+i]=1;  // convert any NULL character
               BufferPtr+=(long)LineLen(lin);
            }

            // treatment for para BYTE and line delimiter
            if (OutputFormat==SAVE_TEXT_LINES && TerArg.WordWrap && LineLen(lin)>0) {
                LastChar=buffer[BufferPtr-1];
                if (LastChar==ParaChar || LastChar==CellChar || LastChar==LINE_CHAR || (lstrchr(BreakChars,LastChar)!=NULL && LastChar!=PAGE_CHAR)) BufferPtr--;  // discard para BYTE for this format
            }

            if (OutputFormat==SAVE_TEXT && TerArg.WordWrap) {
               if (LineLen(lin)>0) {
                  LastChar=buffer[BufferPtr-1];
                  if (LastChar==ParaChar || LastChar==LINE_CHAR || LastChar==CellChar || (lstrchr(BreakChars,LastChar)!=NULL && LastChar!=PAGE_CHAR)) {
                     buffer[BufferPtr-1]=TerArg.delim; // convert para BYTE to line delimiter
                     if (CrLfUsed) {                   // append newline character
                        buffer[BufferPtr]=0xA;
                        BufferPtr++;
                     }
                  }
               }
            }
            else {
               buffer[BufferPtr]=TerArg.delim;
               BufferPtr++;
               if (CrLfUsed) {                 // append newline character
                  buffer[BufferPtr]=0xA;
                  BufferPtr++;
               }
            }
        }
        else {                              // write the line to the file
            if (TextOutput && pXlateText!=NULL) {
               len=(int)XlateLen;
               pStr=pXlateText;                    // pointer to the string
            }
            else if (mbcs && TextOutput && DbLen>0) {
               len=(int)DbLen;
               pStr=pDB;                    // pointer to the string
            }
            else {
               FarMove(ptr,line,LineLen(lin));
               pStr=line;                   // pointer to the line
               len=LineLen(lin);
               for (i=0;i<len;i++) if (pStr[i]==0) pStr[i]=1;  // convert any NULL character
            }

            // treatment for para BYTE and line delimiter
            if ( OutputFormat==SAVE_TEXT_LINES && TerArg.WordWrap && len>0 ) {
              LastChar=pStr[len-1];
              if (LastChar==ParaChar || LastChar==CellChar || LastChar==LINE_CHAR || (lstrchr(BreakChars,LastChar)!=NULL && LastChar!=PAGE_CHAR)) len--;  // discard para BYTE for this format
            }
            pStr[len]=0;                   // terminate the line

            if (TerArg.WordWrap && OutputFormat==SAVE_TEXT) {
               if (len>0) {
                  LastChar=pStr[len-1];
                  if (LastChar==ParaChar || LastChar==LINE_CHAR || LastChar==CellChar || (lstrchr(BreakChars,LastChar)!=NULL && LastChar!=PAGE_CHAR)) {
                     pStr[len-1]=0xd;             // replace para BYTE by cr/lf
                     pStr[len]=0xa;               // replace para BYTE by cr/lf
                     pStr[len+1]=0;               // replace para BYTE by cr/lf
                     FilePos=FilePos+len+1;       // current file pointer
                  }
               }
            }
            else {
               pStr[len]=0xd;
               pStr[len+1]=0xa;
               pStr[len+2]=0;
               FilePos=FilePos+len+2;       // current file pointer
            }

            if (!WriteFile(hFile,pStr,lstrlen(pStr),&written,NULL)) {
               PrintError(w,MSG_ERR_FILE_WRITE,"TerSave");
               CloseHandle(hFile);
               lstrcpy(TempString,OutFile);          // make a copy in the data segment
               lstrcpy(TempString1,BkpFile);         // make a copy in the data segment
               _unlink(TempString);
               rename(TempString,TempString1);
               if (hSaveCursor) SetCursor(hSaveCursor);
               return FALSE;
            }
        }

        if (pXlateText) {
           MemFree(pXlateText);
           pXlateText=NULL;
           XlateLen=0;
        }
    }                                 // write next line

    END:
    //** write the character formatting signature **
    if (OutputFormat==SAVE_TER) {
       SignLen=lstrlen(CfmtSign);    // length of character formatting signature
       if (TerArg.InputType=='B') {
           HugeMove(CfmtSign,&(buffer[BufferPtr]),(long)SignLen);
           BufferPtr+=(long)SignLen;
           buffer[BufferPtr]=TerArg.delim;
           BufferPtr++;
           if (CrLfUsed) {                 // append newline character
              buffer[BufferPtr]=0xA;
              BufferPtr++;
           }
       }
       else {
          FarMove(CfmtSign,TempString,SignLen);
          TempString[SignLen]=0xd;
          TempString[SignLen+1]=0xa;
          TempString[SignLen+2]=0;
          WriteFile(hFile,TempString,strlen(TempString),&written,NULL);
          FilePos=FilePos+SignLen+2;   // current file pointer
       }
    }

    if (TerArg.InputType=='F') CloseHandle(hFile);

    if (TotalLines==0) {
       InitLine(w,0);
       TotalLines=1;
    }

    if (OutputFormat==SAVE_TER) { // save formatting information
       BOOL result;
       if (TerArg.InputType=='F') result=SaveCfmt(w,OutFile,buffer,FilePos,'F');
       else                       result=SaveCfmt(w,OutFile,buffer,BufferPtr,'B');
       if (!result) return FALSE;
    }


    TerArg.modified=PageModifyCount=0;
    Notified=FALSE;               // notify again if modified

    if (TerArg.InputType=='B') {
       if (OutputFormat!=SAVE_TER) TerArg.BufferLen=BufferPtr;  // set the actual length
       if (TerArg.BufferLen==0) {
          buffer[0]=TerArg.delim;
          TerArg.BufferLen=1;
          if (CrLfUsed) {
             buffer[1]=0xa;
             TerArg.BufferLen++;
          }
       }
       GlobalUnlock(TerArg.hBuffer);
    }
    else {
       BOOL SetWinText=TRUE;
       if (TotalFonts>1 && OutputFormat!=SAVE_TER) SetWinText=FALSE;  // saving will loose formatting
       if (SetWinText) SetWindowText(hTerWnd,OutFile);
       if (hSaveCursor) SetCursor(hSaveCursor);
    }

    if (pDB) OurFree(pDB);

    return TRUE;
}

/*****************************************************************************
    CfmtNeeded:
    This routine the number of bytes of storage needed to store the character
    formatting information.
*****************************************************************************/
long CfmtNeeded(PTERWND w)
{
    int i;
    long l;
    long number=0,ZeroCount=0;
    DWORD size;

    for (l=0;l<TotalLines;l++) {
       CompressCfmt(w,l);
       number=number+sizeof(int);                    // add to store line length
       if (cfmt(l).info.type==UNIFORM)
             number=number+2;                        // add for font ids
       else  number=number+LineLen(l)*sizeof(WORD);
       number=number+sizeof(int);                    // add for paragrah id
       number=number+sizeof(int);                    // add for cell id
       number=number+sizeof(int);                    // add for para fid
    }

    // add attribute records
    number=number
           +1L+2L+((long)sizeof(struct StrFontIO)*TotalFonts)  // font record
           +1L                                        // signature byte for paragraph information
           +1L+sizeof(int)+((long)sizeof(struct StrParaIO)*TotalPfmts)  // paragraph table record
           +1L+sizeof(int)+((long)sizeof(struct StrTab)*TotalTabs)      // tab table record
           +1L+sizeof(int)+((long)sizeof(struct StrSect)*TotalSects)    // section table record
           +1L+sizeof(int)+((long)sizeof(struct StrTableRow)*TotalTableRows) // table rows
           +1L+sizeof(int)+((long)sizeof(struct StrCell)*TotalCells)    // table cells
           +1L+sizeof(int)+((long)sizeof(struct StrParaFrame)*TotalParaFrames)    // table para frames
           +1L+sizeof(int)+((long)sizeof(struct StrStyleId)*TotalSID)    // table style ids
           +1L+sizeof(int)+((long)sizeof(struct StrBlt)*TotalBlts)       // table bullet ids
           +1L+sizeof(int)+((long)sizeof(struct StrCharTag)*TotalCharTags) // char tag table
           +1L+sizeof(int)+((long)sizeof(struct StrList)*TotalLists)     // list table
           +1L+sizeof(int)+((long)sizeof(struct StrListOr)*TotalListOr)  // list override table
           +1L+sizeof(int)+((long)sizeof(struct StrReviewer)*TotalReviewers)  // reviewer table
           +1L+sizeof(struct StrFileInfo);            // file information block

    for (i=0;i<TotalFonts;i++) {     // add bytes to store pictures
       if (TerFont[i].InUse && TerFont[i].style&PICT) {
          DWORD ImageSize=TerFont[i].ImageSize;
          DWORD InfoSize=TerFont[i].InfoSize;
          DWORD OrigImageSize=TerFont[i].OrigImageSize;

          // add the link file name length if any
          if (TerFont[i].LinkFile && lstrlen(TerFont[i].LinkFile)>0) {
             number=number+lstrlen(TerFont[i].LinkFile)+1;  // add a space for the NULL character
             ImageSize=InfoSize=0;
          }

          number=number
                 +2L*sizeof(UINT)       // add picture dimension variables
                 +2L*sizeof(DWORD)      // add picture size variables
                 +ImageSize             // add image storage requirement
                 +InfoSize              // add image info structure storage requirement
                 +OrigImageSize;        // original image size
          if (TerFont[i].ObjectType!=OBJ_NONE && TerFont[i].ObjectType!=OBJ_OCX_EXT) {
             size=0;
             TerOleQuerySize(w,i,&size);
             number=number+size;        // add the object size
          }

          // add for animation information
          number+=sizeof(BOOL);         // animation flag
          if (TerFont[i].anim) number=number+sizeof(struct StrAnim);
       }
       if (TerFont[i].InUse && TerFont[i].FieldId>0) {   // add space to write field code
          number+=sizeof(int);
          if (TerFont[i].FieldCode && lstrlen(TerFont[i].FieldCode)>0) number+=lstrlen(TerFont[i].FieldCode);
       } 
    }

    // add space for lead bytes
    number+=1L;                       // signature before tag ids
    ZeroCount=0;
    for (l=0;l<=TotalLines;l++) {
       if (l<TotalLines && (!pLead(l) || LineLen(l)==0)) {
          ZeroCount++;
          continue;
       }

       if (ZeroCount>0) {             // write any zero count
          number+=(sizeof(int)+sizeof(ZeroCount));
          ZeroCount=0;
       }
       if (l==TotalLines) break;

       number+=sizeof(int);           // write the length
       number+=(sizeof(BYTE)*LineLen(l));
    }

    // add space for tag ids
    number+=1L;                       // signature before tag ids
    ZeroCount=0;
    for (l=0;l<=TotalLines;l++) {
       if (l<TotalLines && (!pCtid(l) || LineLen(l)==0)) {
          ZeroCount++;
          continue;
       }

       if (ZeroCount>0) {             // write any zero count
          number+=(sizeof(int)+sizeof(ZeroCount));
          ZeroCount=0;
       }
       if (l==TotalLines) break;

       number+=sizeof(int);           // write the length
       number+=(sizeof(WORD)*LineLen(l));
    }

    // add the space for the tag information
    for (i=0;i<TotalCharTags;i++) {
       number+=(3*sizeof(int));      // length information for the 3 strings
       number+=(CharTag[i].name?lstrlen(CharTag[i].name):0);
       number+=(CharTag[i].AuxText?lstrlen(CharTag[i].AuxText):0);
    }

    // add the space for the rtf information block
    number+=1L;                       // signature before tag ids
    for (i=0;i<INFO_MAX;i++) number+=TerWriteRtfInfo(w,i,FALSE,'F',(HFILE)NULL,NULL,NULL);
    
    // add space for list name and list levels
    for (i=0;i<TotalLists;i++) {
       if (!list[i].InUse) continue;

       number= number
              +sizeof(int) + (list[i].name?lstrlen(list[i].name):0)   //name length and name string
              +sizeof(struct StrListLevel)*list[i].LevelCount;
    } 
     
    // add space for list override levels
    for (i=0;i<TotalListOr;i++) {
       if (!ListOr[i].InUse) continue;

       number= number
              +sizeof(struct StrListLevel)*ListOr[i].LevelCount;  // size for the list level
    } 
     
    return number;
}

/******************************************************************************
   UnicodeRead:
   Read an unicode file
*******************************************************************************/
BOOL UnicodeRead(PTERWND w,BYTE InputType, LPBYTE pBuf,HANDLE InFile, long FileSize, BOOL append)
{
   WORD line[MAX_WIDTH+2],UChar,UcBase;
   long BufIdx=0,WordsRead,WordCount,count;
   int i,idx,len,TextLen,LastPos,LangFont;
   LPBYTE ptr;
   LPWORD fmt;
   BYTE chr;
   
   // get language font
   if (DefLang!=ENGLISH) LangFont=SetCurLangFont2(w,0,(HKL)DefLang);
   
   // discard the unicode sign word
   if (!TerFarRead(w,InputType,InFile,pBuf,&BufIdx,FileSize,sizeof(WORD),(BYTE huge *)line)) return FALSE;

   WordsRead=1;  // one word read so file
   WordCount=FileSize/2;

   while (WordsRead<WordCount) {
      count=WordCount-WordsRead;
      if (count>MAX_WIDTH) count=MAX_WIDTH;
      if (!TerFarRead(w,InputType,InFile,pBuf,&BufIdx,FileSize,count*sizeof(WORD),(BYTE huge *)line)) return FALSE;
   
      // check the last character read - read additional character if cr/lf is breaking
      if (count>0 && line[count-1]==0xd) {  // also read the following 0xa character
         if (!TerFarRead(w,InputType,InFile,pBuf,&BufIdx,FileSize,sizeof(WORD),(BYTE huge *)&(line[count]))) return FALSE;
         count++;
      } 

      // extract segments for this line to append to the document
      idx=0;  // index of the word in the line buffer to process
      while (idx<count) {
         len=0;  // length of the line segment extracted
         for (i=idx;i<count && len<(MAX_WIDTH/3);i++,len++) {
            if (i>idx && line[i-1]==0xd && line[i]==0xa) {
               len++;                 // go past the selected char
               break;
            }
         }
         if (len>0 && line[idx+len-1]==0xd) len--;       // do not let cr/lf pair break
         if (len==0) len++;
         
         // insert and allocate space for a new line
         if (!CheckLineLimit(w,TotalLines+1)) {
             PrintError(w,MSG_LINE_TRUNCATED,MsgString[MSG_MAX_LINES_EXCEEDED]);
             return -1;
         }
         if (append) {
            TotalLines++;
            CurLine=TotalLines-1;
            InitLine(w,CurLine);
         }
         else MoveLineArrays(w,CurLine,1,'B');// insert a line before the current line

         TextLen=len;
         LastPos=idx+len-1;
         if (len>=2 && line[LastPos]==0xa) TextLen--;  // subtract because cr/lf gets converted to ParaChar

         LineAlloc(w,CurLine,0,TextLen);

         ptr=pText(CurLine);
         fmt=OpenCfmt(w,CurLine);

         for (i=0;i<TextLen;i++) {
            UChar=line[idx+i];
            if (UChar==0xd) UChar=ParaChar;
            
            // split into UcBase and regular char
            UcBase=0;
            if (UChar<256) chr=(BYTE)UChar;
            else CrackUnicode(w,UChar,&UcBase,&chr);


            ptr[i]=chr;
            if   (UcBase==0) fmt[i]=0;          // default font
            else fmt[i]=SetUniFont(w,0,UcBase);

            if (DefLang!=ENGLISH) {
               if (!IsEnglishChar(w,ptr,i,i+1)) {
                  if (UcBase==0) fmt[i]=LangFont;
                  else           fmt[i]=SetCurLangFont2(w,fmt[i],(HKL)DefLang);
               }
            }
         } 

         CloseCfmt(w,CurLine);

         idx+=len;                      // index of the next segment 
      } 

      WordsRead+=count;
   } 
 
   return TRUE;
}
 
/******************************************************************************
   UnicodeWrite:
   Write the file in the unicode text format
*******************************************************************************/
long UnicodeWrite(PTERWND w,LPBYTE pBuf,HANDLE hFile, long MaxBufSize)
{
   long l;
   long BufIdx=0;
   int  len,i,CurFont,CodePage,CharSet;
   BYTE OutputType=(pBuf?'B':'F');
   WORD out[MAX_WIDTH+1],UcBase,UText[3];
   BOOL done;
   LPBYTE ptr;
   LPWORD fmt;
   BYTE mb[3];

   // write the signature byte
   out[0]=0xFEFF;
   if (!TerFarWrite(w,OutputType,hFile,pBuf,&BufIdx,MaxBufSize,1*sizeof(WORD),(BYTE huge *)out)) return 0;

   // write lines
   for (l=0;l<TotalLines;l++) {
      len=LineLen(l);
      if (len==0) continue;

      if (LineFlags(l)&(LFLAG_BREAK|LFLAG_PARA)) len--;   // don't write the break charancter

      ptr=pText(l);
      fmt=OpenCfmt(w,l);
      for (i=0;i<len;i++) {
         CurFont=fmt[i];
         UcBase=TerFont[CurFont].UcBase;
         CharSet=TerFont[CurFont].CharSet;
         done=false;
         if (UcBase==0 && CharSet>=128 && pLead(l)==NULL) {   // convert to unicode
            IsMbcsCharSet(w,CharSet,&CodePage);  // get the code page
            if (CharSet==77) CodePage=10000;     // mac character set corresponds to code page 10000
            mb[0]=ptr[i];
            if (MultiByteToWideChar(CodePage,0,mb,1,UText,1)==1) {
               out[i]=UText[0];
               done=true;
            }
         
         } 
         
         if (!done) out[i]=MakeUnicode(w,UcBase,ptr[i]);
      } 
      CloseCfmt(w,l); 

      // add any break character
      if (LineFlags(l)&(LFLAG_BREAK|LFLAG_PARA)) {
         len=LineLen(l);
         if (len>0 && ptr[len-1]==CELL_CHAR) {  // convert to tab
           out[i]=TAB;i++;
         }
         else { 
           out[i]=0xd;i++;
           out[i]=0xa;i++;
         }
      }
      out[i]=0;
      
      // write the line out 
      if (!TerFarWrite(w,OutputType,hFile,pBuf,&BufIdx,MaxBufSize,i*sizeof(WORD),(BYTE huge *)out)) return 0;
   }
 
   return BufIdx;
}
 
/******************************************************************************
   ReadFileInfo:
   This function reads the TER file information block.  This function also 
   updates the FileFormat variable.
*******************************************************************************/
BOOL ReadFileInfo(PTERWND w,LPBYTE InputFile,BYTE huge *buf,BYTE InputType, BOOL message)
{
    HANDLE InFile=NULL;
    long InfoPos,FileLen;
    WORD UnicodeSign;

    // set default value in the file information block
    InitFileInfo(w);

    FileFormat=SAVE_TEXT;   // default file format

    // find the position of the information block
    if (InputType=='F') {
        if ((InFile=CreateFile(InputFile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
             return PrintError(w,MSG_ERR_FILE_OPEN,"ReadFileInfo");
        
        // check if unicode text file
        if (!TerFarRead(w,InputType,InFile,buf,&InfoPos,TerArg.BufferLen,sizeof(UnicodeSign),(BYTE huge *)&UnicodeSign)) {
           CloseHandle(InFile);
           return TRUE;
        }
        if (UnicodeSign==0xFEFF) {
           CloseHandle(InFile);
           FileFormat=SAVE_UTEXT;
           return TRUE;
        } 

        FileLen=SetFilePointer(InFile,0,NULL,FILE_END);
        InfoPos=FileLen-sizeof(struct StrFileInfo);
        if (InfoPos>=0) SetFilePointer(InFile,(long) InfoPos,NULL,FILE_BEGIN);
    }
    else {
        // check if unicode text file
        InfoPos=0;
        if (!TerFarRead(w,InputType,InFile,buf,&InfoPos,TerArg.BufferLen,sizeof(UnicodeSign),(BYTE huge *)&UnicodeSign)) return TRUE;
        if (UnicodeSign==0xFEFF) {
           FileFormat=SAVE_UTEXT;
           return TRUE;
        }

        InfoPos=TerArg.BufferLen-sizeof(struct StrFileInfo);
    }

    // read the information block
    if (InfoPos>=0) if (!TerFarRead(w,InputType,InFile,buf,&InfoPos,TerArg.BufferLen,sizeof(struct StrFileInfo),(BYTE huge *)&FileInfo)) goto END_READ_INFO;
    if (InfoPos<0 || FileInfo.sign!=SIGN_EOF || FileInfo.VerId!=40) {
       if (message) PrintError(w,MSG_NOT_TE_FILE,"ReadFileInfo");
       if (InputType=='F') CloseHandle(InFile);

       InitFileInfo(w); // reinitialize the struture
       return FALSE;
    }

    END_READ_INFO:

    // check if the block is valid
    if (FileInfo.sign!=SIGN_EOF || FileInfo.VerId!=40) InitFileInfo(w);

    if (InputType=='F') CloseHandle(InFile);
    FileFormat=SAVE_TER;

    return TRUE;
}

/******************************************************************************
   InitFileInfo:
   This function initializes the FileInfo structure.
*******************************************************************************/
BOOL InitFileInfo(PTERWND w)
{

    FarMemSet(&FileInfo,0,sizeof(struct StrFileInfo));
    FileInfo.sign=SIGN_EOF;
    FileInfo.VerId=40;
    FileInfo.RevId=0;

    FileInfo.PpHeight=(int)InchesToTwips(PageHeight);
    FileInfo.PpWidth=(int)InchesToTwips(PageWidth);
    FileInfo.PpSize=PaperSize;
    FileInfo.PpOrient=PaperOrient;

    return TRUE;
}

/******************************************************************************
    OurReadLine:
    The ReadLine function for the new i/o functions.
    This functions returns -1 to indicate an error and 0 to indicate end of file.
*******************************************************************************/
OurReadLine(PTERWND w, LPBYTE OutBuf,int MaxLen,HANDLE InFile)
{
   DWORD BytesRead;
   int i,GoBack=0;

   OutBuf[0]=0;

   if (!ReadFile(InFile,OutBuf,MaxLen,&BytesRead,NULL)) return -1;

   if (BytesRead==0) return 0;
   if (OutBuf[BytesRead-1]==0xd) {
      GoBack++;                     // don't let cr/lf pair break
      BytesRead--;
   } 
   if (BytesRead==0) return 0;

   // locate the cr/lf character
   for (i=0;i<((int)BytesRead-1);i++) if (OutBuf[i]==0xd && OutBuf[i+1]==0xa) break;

   if (i<((int)BytesRead-1)) {
      GoBack+=(BytesRead-1-(i+1));
      OutBuf[i]=0xa;                  // to simulat the old ReadLine function
      OutBuf[i+1]=0;
   } 

   if (GoBack>0) SetFilePointer(InFile,-GoBack,NULL,FILE_CURRENT);   // position the file pointer back

   return i;
}
 
/******************************************************************************
    TerFarRead:
    This routine reads a specified number of bytes(long) from the input
    source.  The input can be either the buffer or the file file stream.
    A far pointer to the receiving buffer must be provided.

    If the input is a buffer,  the buffer pointer and the pointer to the
    current index must be provided.  The routine will update the index
    for the next read.

    If the input is file stream,  the file stream must be provided.

    The routine returns a TRUE or FALSE result.
*******************************************************************************/
TerFarRead(PTERWND w,BYTE InputType,HANDLE InFile,BYTE huge *InBuf,long far *BufPos,long MaxBufLen,long len,BYTE huge *OutBuf)
{
    if (InputType=='F') {            // file read
       DWORD read;
       if (!ReadFile(InFile,OutBuf,len,&read,NULL) || read!=(DWORD)len) return FALSE;
    }
    else {
       if (((*BufPos)+len)>MaxBufLen) return FALSE;
       HugeMove(&(InBuf[*BufPos]),OutBuf,len);
       (*BufPos)+=len;
    }
                                     // buffer read
    return TRUE;
}

/******************************************************************************
    TerFarWrite:
    This routine writes a specified number of bytes(long) to the output.
    The output can be either the buffer or the file file stream.
    A far pointer to the input buffer must be provided.

    If the output is a buffer,  the buffer pointer and the pointer to the
    current index must be provided.  The routine will update the index
    for the next write.

    If the output is a file stream,  the file stream must be provided.

    The routine returns a TRUE or FALSE result.
*******************************************************************************/
TerFarWrite(PTERWND w,BYTE OutputType,HANDLE OutFile,BYTE huge *OutBuf,long far *BufPos,long MaxBufLen,long len,BYTE huge *InBuf)
{

    if (OutputType=='F') {            // file read
       DWORD written;
       if (!WriteFile(OutFile,InBuf,len,&written,NULL)) return FALSE;
    }
    else {
       if (((*BufPos)+len)>MaxBufLen) return FALSE;
       HugeMove(InBuf,&(OutBuf[*BufPos]),len);
       (*BufPos)+=len;
    }                                // buffer read

    return TRUE;
}

/******************************************************************************
    GetFileName:
    This function gets the file name from the user.  The first argument is
    TRUE for 'open' dialog box, and FALSE for the 'Save As' dialog box.
    The second argument can be used to specify the initial file name and
    get the file selection from the user.  The third argument (filter) must
    be '|' for delimters for wild card pairs.
******************************************************************************/
BOOL GetFileName(PTERWND w,BOOL open,LPBYTE file,int FilterIndex, LPBYTE filter)
{
    int    i,len,result,OutFmt;
    OPENFILENAME ofn;
    MSG msg;
    LPBYTE pExt=NULL;

    // preprare to get the bitmap file name from the user
    FarMemSet(&ofn,0,sizeof(OPENFILENAME));        // initialize
    ofn.lStructSize=sizeof(OPENFILENAME);
    ofn.hwndOwner=hTerWnd;
    len=lstrlen(filter);
    for (i=0;i<len;i++) if (filter[i]=='|') filter[i]=0;
    ofn.lpstrFilter=filter;
    if (len) ofn.nFilterIndex=FilterIndex;
    else     ofn.nFilterIndex=0;
    ofn.lpstrFile=file;
    ofn.nMaxFile=256;
    if (lstrlen(UserDir)>0) ofn.lpstrInitialDir=UserDir;
    ofn.Flags=OFN_SHOWHELP|OFN_HIDEREADONLY|OFN_NOCHANGEDIR;

    // get the file name from the user
    if (open) result=GetOpenFileName(&ofn);
    else {                              // save file
       if (TerArg.SaveFormat==SAVE_DEFAULT) OutFmt=FileFormat;        // save as input
       else                                 OutFmt=TerArg.SaveFormat; // save to specified format
       
       ofn.nFilterIndex=1;                                                      
       if (OutFmt==SAVE_TER)        ofn.nFilterIndex=1;
       if (OutFmt==SAVE_TEXT)       ofn.nFilterIndex=2;
       if (OutFmt==SAVE_TEXT_LINES) ofn.nFilterIndex=3;
       if (OutFmt==SAVE_RTF)        ofn.nFilterIndex=4;
       if (Win32) {                                    
         if (OutFmt==SAVE_UTEXT)      ofn.nFilterIndex=5;    
         if (OutFmt==SAVE_HTML)       ofn.nFilterIndex=6;    
       }
       else if (OutFmt==SAVE_HTML)       ofn.nFilterIndex=5;    
       
       ofn.Flags|=OFN_OVERWRITEPROMPT;
       if (0!=(result=GetSaveFileName(&ofn)) ) {
          int index=(int)(ofn.nFilterIndex);   // base 1
          if (index==1) TerArg.SaveFormat=SAVE_TER;
          if (index==2) TerArg.SaveFormat=SAVE_TEXT;
          if (index==3) TerArg.SaveFormat=SAVE_TEXT_LINES;
          if (index==4) TerArg.SaveFormat=SAVE_RTF;
          
          if (Win32) {                                    
             if (index==5) TerArg.SaveFormat=SAVE_UTEXT; 
             if (index==6) TerArg.SaveFormat=SAVE_HTML;
          }     
          else if (index==5) TerArg.SaveFormat=SAVE_HTML;   

          // set the file extension
          pExt=NULL;
          if (TerArg.SaveFormat==SAVE_RTF)        pExt=".rtf";
          else if (TerArg.SaveFormat==SAVE_HTML)  pExt=".htm";
          else if (TerArg.SaveFormat==SAVE_UTEXT) pExt=".txt";
          else if (TerArg.SaveFormat==SAVE_TEXT)  pExt=".txt";
          else if (TerArg.SaveFormat==SAVE_TER) {
             if (Win32) pExt=".sse";
             else       pExt=".ter";
          }
          if (pExt) AddExt(file,pExt);
       }
    }

    SetFocus(hTerWnd);

    // discard any mouse messages left from the above call
    while (PeekMessage(&msg,hTerWnd,WM_MOUSEFIRST,WM_MOUSELAST,PM_REMOVE|PM_NOYIELD));;

    return result;
}

/******************************************************************************
    AddExt:
    This function adds an extension to the file name if the file name does
    not have an extension
*******************************************************************************/
AddExt(LPBYTE name, LPBYTE ext)
{
    int i,len;
    BOOL DotFound=FALSE;

    // look for an extension
    len=lstrlen(name);
    for (i=len-1;i>=0 && i>=(len-4);i--) {
       if (name[i]==':' || name[i]=='\\') break;
       if (name[i]=='.') {
          DotFound=TRUE;
          break;
       }
    }

    if (!DotFound) lstrcat(name,ext);

    return TRUE;
}

#if defined(USE_WININET)

BOOL  (WINAPI _export *pHttpQueryInfo)(HINTERNET,DWORD,LPVOID,LPDWORD,LPDWORD);
BOOL  (WINAPI _export *pFtpPutFile)(HINTERNET,LPBYTE,LPBYTE,DWORD,DWORD);
BOOL  (WINAPI _export *pFtpDeleteFile)(HINTERNET,LPBYTE);
BOOL  (WINAPI _export *pInternetGetLastResponseInfo)(LPDWORD,LPBYTE,LPDWORD);
HINTERNET  (WINAPI _export *pInternetConnect)(HINTERNET,LPBYTE,INTERNET_PORT,LPBYTE,LPBYTE,DWORD,DWORD,DWORD);

#endif

/*****************************************************************************
    TerInternetGet:
    Get a file from internet
******************************************************************************/
BOOL WINAPI _export TerInternetGet(HWND hWnd, LPBYTE url, LPBYTE OutFile, BOOL close)
{
   #if defined(USE_WININET)
     PTERWND w;
     HINTERNET hUrl;
     HANDLE hFile;
     DWORD BytesRead;
     int BufSize=4096;
     LPBYTE pBuf;
     HCURSOR hSaveCursor=NULL;
     BOOL DataExists;
     DWORD written;

     if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
     
     if (!hInternet && !TerInternetOpen(hWnd)) return FALSE;

     hUrl=pInternetOpenUrl(hInternet,url,NULL,0,INTERNET_FLAG_RELOAD,0);
     if (!hUrl) return FALSE;

     // open the local file to write data
     _unlink(OutFile);
     if (INVALID_HANDLE_VALUE==(hFile=CreateFile(OutFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) {
          return PrintError(w,MSG_ERR_FILE_OPEN,"TerInternetGet");
     }
     pBuf=MemAlloc(BufSize+1);

     if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while reading

     // read the file
     DataExists=TRUE;
     while (TRUE) {
        if (!pInternetReadFile(hUrl,pBuf,BufSize,&BytesRead)) {
           return PrintError(w,MSG_ERR_FILE_READ,"TerInternetGet");
        }
        if (BytesRead==0) break;

        if (BytesRead>190 && BytesRead<210) {      // check if data not found
           pBuf[BytesRead]=0;
           if (strstr(pBuf,"<TITLE>404 Not Found</TITLE>")) {
              DataExists=FALSE;
              break;
           }
        } 

        if (!WriteFile(hFile,pBuf,BytesRead,&written,NULL)) {
           return PrintError(w,MSG_ERR_FILE_WRITE,"TerInternetGet");
        }
     } 

     if (hSaveCursor) SetCursor(hSaveCursor);

     MemFree(pBuf);
     CloseHandle(hFile);
     pInternetCloseHandle(hUrl);        // close the url handle

     if (close) TerInternetClose(hWnd);   // close the session
     
     return DataExists;
   #else
      return FALSE;
   #endif
}


/*****************************************************************************
    TerInternetPut:
    Save a file to specified url on the internet
******************************************************************************/
BOOL WINAPI _export TerInternetPut(HWND hWnd, LPBYTE url, LPBYTE InFile, BOOL close)
{
   #if defined(USE_WININET)
     PTERWND w;
     HINTERNET hConnect;
     HCURSOR hSaveCursor=NULL;
     BYTE FilePath[300];

     if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
     
     if (!hInternet && !TerInternetOpen(hWnd)) return FALSE;

     
     if (NULL==(hConnect=pInternetConnect(hInternet,"ftp.subsystems.com",INTERNET_INVALID_PORT_NUMBER
                ,NULL/*"annonymous"*/,NULL/*"subsys@shore.net"*/,INTERNET_SERVICE_FTP,0,0))) {
        BYTE text[300]="";
        DWORD len=sizeof(text)-1,code;
        pInternetGetLastResponseInfo(&code,text,&len);
        OurPrintf("InternetConnect failed, error: %x, (%s)",code,text);
        return FALSE;
     }
     
     if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while reading
     
     lstrcpy(FilePath,"/temp/test.htm");
     pFtpDeleteFile(hConnect,FilePath);
     if (!pFtpPutFile(hConnect,FilePath,FilePath,FTP_TRANSFER_TYPE_BINARY,0)) {
         OurPrintf("FtpPutFile failed");
         if (hSaveCursor) SetCursor(hSaveCursor);
         return FALSE;
     } 

     if (hSaveCursor) SetCursor(hSaveCursor);

     pInternetCloseHandle(hConnect);

     if (close) TerInternetClose(hWnd);   // close the session
     
     return TRUE;
   #else
      return FALSE;
   #endif
}

/*****************************************************************************
    TerInternetPutHttp:
    Save a file to specified url on the internet
******************************************************************************/
BOOL WINAPI _export TerInternetPutHttp(HWND hWnd, LPBYTE url, LPBYTE InFile, BOOL close)
{
   #if defined(USE_WININET)
     PTERWND w;
     HINTERNET hRequest,hConnect;
     HANDLE hFile;
     DWORD BytesRead,BytesWritten;
     int BufSize=4096;
     LPBYTE pBuf;
     HCURSOR hSaveCursor=NULL;

     typedef struct _INTERNET_BUFFERSA {
         DWORD dwStructSize;                 // used for API versioning. Set to sizeof(INTERNET_BUFFERS)
         struct _INTERNET_BUFFERSA * Next;   // chain of buffers
         LPCSTR   lpcszHeader;               // pointer to headers (may be NULL)
         DWORD dwHeadersLength;              // length of headers if not NULL
         DWORD dwHeadersTotal;               // size of headers if not enough buffer
         LPVOID lpvBuffer;                   // pointer to data buffer (may be NULL)
         DWORD dwBufferLength;               // length of data buffer if not NULL
         DWORD dwBufferTotal;                // total size of chunk, or content-length if not chunked
         DWORD dwOffsetLow;                  // used for read-ranges (only used in HttpSendRequest2)
         DWORD dwOffsetHigh;
     } INTERNET_BUFFERS, * LPINTERNET_BUFFERS;

     INTERNET_BUFFERS BuffIn;

     if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
     
     if (!hInternet && !TerInternetOpen(hWnd)) return FALSE;

     
     if (NULL==(hConnect=pInternetConnect(hInternet,"www.subsystems.com",INTERNET_INVALID_PORT_NUMBER
                ,"","",INTERNET_SERVICE_HTTP,0,0))) {
        OurPrintf("InternetConnect failed");
        return FALSE;
     }
     
     hRequest=pHttpOpenRequest(hConnect,"PUT","/temp/test.htm"/*url*/,NULL,NULL,NULL,0,0);
     if (!hRequest) {
         OurPrintf("HttpOpenRequst failed");
         return FALSE;
     }

     FarMemSet(&BuffIn,0,sizeof(BuffIn));
     BuffIn.dwStructSize=sizeof(BuffIn);
     BuffIn.dwBufferTotal=GetFileLength(InFile);

     // send http request
     if (!pHttpSendRequestEx(hRequest,&BuffIn,NULL,0x8/*HSR_INITIATE*/,0)) {
         OurPrintf("pHttpSendRequestEx failed");
         return FALSE;
     }

     // open the local file to write data
     if ((hFile=CreateFile(InFile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
          return PrintError(w,MSG_ERR_FILE_OPEN,"TerInternetPut");
     }

     pBuf=MemAlloc(BufSize+1);

     if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while reading

     // read the file
     while (TRUE) {
        ReadFile(InFile,pBuf,BufSize,&BytesRead,NULL);
        if (BytesRead==0) break;

        BytesWritten=0;
        if (!pInternetWriteFile(hRequest,pBuf,BytesRead,&BytesWritten)) {
           return PrintError(w,MSG_ERR_FILE_WRITE,"TerInternetPut");
        }

        if (BytesRead!=BytesWritten) {
           OurPrintf("write error");
        }
         
        if ((int)BytesRead<BufSize) break;
     } 

     if (hSaveCursor) SetCursor(hSaveCursor);

     MemFree(pBuf);
     CloseHandle(hFile);
     if (!pHttpEndRequest(hRequest,NULL,0,0)) return FALSE;;        // close the url handle

     {
        DWORD code=0,size=sizeof(code);
        if (pHttpQueryInfo(hRequest,HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,&code,&size,NULL)) {
           OurPrintf("%d",code);
        } 
     }    
      
     pInternetCloseHandle(hConnect);

     if (close) TerInternetClose(hWnd);   // close the session
     
     return TRUE;
   #else
      return FALSE;
   #endif
}


/*****************************************************************************
    TerInternetOpen:
    Open the internet session for this window
******************************************************************************/
BOOL WINAPI _export TerInternetOpen(HWND hWnd)
{
   #if defined(USE_WININET)

     PTERWND w;
      
     if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
   
     if (WininetSearched && !hWininet) return FALSE;

     if (!hWininet && !SearchWininet(w)) return FALSE;
    
     if (hInternet) return (DWORD)hInternet;

     hInternet=pInternetOpen("TE Edit Control",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);

     return (hInternet!=NULL);
   
   #else
     return FALSE;
   #endif
} 

/*****************************************************************************
    TerInternetClose:
    close any opened internet session.
******************************************************************************/
BOOL WINAPI _export TerInternetClose(HWND hWnd)
{
   
   #if defined(USE_WININET) 
      PTERWND w;

      if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

      if (WininetSearched && !hWininet) return FALSE;
      if (!hWininet && !SearchWininet(w)) return FALSE;
    
      if (hInternet) {
         pInternetCloseHandle(hInternet);
         hInternet=FALSE;
      }
 
      return TRUE;
   #else
      return FALSE;
   #endif
} 

/******************************************************************************
    SearchWininet:
    Search for the Wininet product and find the function pointers.
******************************************************************************/
SearchWininet(PTERWND w)
{
    UINT OldErrorMode;
    BOOL result=TRUE;

    WininetSearched=TRUE;         // spelltime search complete
    hWininet=NULL;

    // set the error mode to catch the 'file not found' error
    OldErrorMode=SetErrorMode(SEM_NOOPENFILEERRORBOX);

    // load the Wininet product
    #if defined (USE_WININET)
       if ((hWininet=LoadLibrary("WININET.DLL"))==0) result=FALSE;

       SetErrorMode(OldErrorMode);    // reset the error mode

       if (!result) {
          hWininet=NULL;
          return FALSE;
       }

       // find the function pointers
       pInternetOpen=(LPVOID)GetProcAddress(hWininet,"InternetOpenA");
       pInternetCloseHandle=(LPVOID)GetProcAddress(hWininet,"InternetCloseHandle");
       pInternetReadFile=(LPVOID)GetProcAddress(hWininet,"InternetReadFile");
       pInternetWriteFile=(LPVOID)GetProcAddress(hWininet,"InternetWriteFile");
       pInternetOpenUrl=(LPVOID)GetProcAddress(hWininet,"InternetOpenUrlA");
       pInternetQueryDataAvailable=(LPVOID)GetProcAddress(hWininet,"InternetQueryDataAvailable");
       pInternetGetLastResponseInfo=(LPVOID)GetProcAddress(hWininet,"InternetGetLastResponseInfoA");
       pHttpOpenRequest=(LPVOID)GetProcAddress(hWininet,"HttpOpenRequestA");
       pHttpEndRequest=(LPVOID)GetProcAddress(hWininet,"HttpEndRequestA");
       pHttpSendRequestEx=(LPVOID)GetProcAddress(hWininet,"HttpSendRequestExA");
       pInternetConnect=(LPVOID)GetProcAddress(hWininet,"InternetConnectA");
       pHttpQueryInfo=(LPVOID)GetProcAddress(hWininet,"HttpQueryInfoA");
       pFtpPutFile=(LPVOID)GetProcAddress(hWininet,"FtpPutFileA");
       pFtpDeleteFile=(LPVOID)GetProcAddress(hWininet,"FtpDeleteFileA");
       pInternetGetLastResponseInfo=(LPVOID)GetProcAddress(hWininet,"InternetGetLastResponseInfoA");

       if (!pInternetOpen || !pInternetCloseHandle || !pInternetReadFile
           || !pInternetWriteFile || !pInternetOpenUrl
           || !pInternetQueryDataAvailable || !pInternetGetLastResponseInfo
           || !pHttpOpenRequest || !pHttpEndRequest || !pHttpSendRequestEx
           || !pInternetConnect || !pHttpQueryInfo || !pFtpPutFile || !pFtpDeleteFile
           || !pInternetGetLastResponseInfo) {
          FreeLibrary(hWininet);
          hWininet=NULL;
          return FALSE;
       }

       return TRUE;
    #else
      return FALSE;
    #endif
}

/******************************************************************************
    GetFileLength:
    Get the size of the given file in number of bytes.  This function returns
    -1 if an error occurs
*******************************************************************************/
long GetFileLength(LPSTR FileName)
{
    HANDLE iFile;
    long len=-1L;

    //lstrcpy(string1,FileName);                // copy to data segment
    if ((iFile=CreateFile(FileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
       MessageBox(NULL,MsgString[MSG_ERR_FILE_OPEN],FileName,MB_OK);
       return len;
    }

    len=SetFilePointer(iFile,0,NULL,FILE_END);

    CloseHandle(iFile);

    return len;
}

/******************************************************************************
    DispMsg:
    Display the WM message description.
******************************************************************************/
DispMsg(UINT msg)
{
    BYTE string[30];
    LPBYTE pMsg=string;

    wsprintf(string,"Other: %x",msg);

    if      (msg==0x0000)  pMsg="WM_NULL";
    else if (msg==0x0001)  pMsg="WM_CREATE";
    else if (msg==0x0002)  pMsg="WM_DESTROY";
    else if (msg==0x0003)  pMsg="WM_MOVE";
    else if (msg==0x0005)  pMsg="WM_SIZE";
    else if (msg==0x0006)  pMsg="WM_ACTIVATE";
    else if (msg==0x0007)  pMsg="WM_SETFOCUS";
    else if (msg==0x0008)  pMsg="WM_KILLFOCUS";
    else if (msg==0x000A)  pMsg="WM_ENABLE";
    else if (msg==0x000B)  pMsg="WM_SETREDRAW";
    else if (msg==0x000C)  pMsg="WM_SETTEXT";
    else if (msg==0x000D)  pMsg="WM_GETTEXT";
    else if (msg==0x000E)  pMsg="WM_GETTEXTLENGTH";
    else if (msg==0x000F)  pMsg="WM_PAINT"; 
    else if (msg==0x0010)  pMsg="WM_CLOSE"; 
    else if (msg==0x0011)  pMsg="WM_QUERYENDSESSION";
    else if (msg==0x0012)  pMsg="WM_QUIT";  
    else if (msg==0x0013)  pMsg="WM_QUERYOPEN";
    else if (msg==0x0014)  pMsg="WM_ERASEBKGND";  
    else if (msg==0x0015)  pMsg="WM_SYSCOLORCHANGE";
    else if (msg==0x0016)  pMsg="WM_ENDSESSION";  
    else if (msg==0x0017)  pMsg="WM_SYSTEMERROR"; 
    else if (msg==0x0018)  pMsg="WM_SHOWWINDOW";  
    else if (msg==0x0019)  pMsg="WM_CTLCOLOR";
    else if (msg==0x001A)  pMsg="WM_WININICHANGE";
    else if (msg==0x001B)  pMsg="WM_DEVMODECHANGE";
    else if (msg==0x001C)  pMsg="WM_ACTIVATEAPP"; 
    else if (msg==0x001D)  pMsg="WM_FONTCHANGE";  
    else if (msg==0x001E)  pMsg="WM_TIMECHANGE";  
    else if (msg==0x001F)  pMsg="WM_CANCELMODE";  
    else if (msg==0x0020)  pMsg="WM_SETCURSOR";
    else if (msg==0x0021)  pMsg="WM_MOUSEACTIVATE";
    else if (msg==0x0022)  pMsg="WM_CHILDACTIVATE";
    else if (msg==0x0023)  pMsg="WM_QUEUESYNC";
    else if (msg==0x0024)  pMsg="WM_GETMINMAXINFO";
    else if (msg==0x0027)  pMsg="WM_ICONERASEBKGND";
    else if (msg==0x0028)  pMsg="WM_NEXTDLGCTL";  
    else if (msg==0x002A)  pMsg="WM_SPOOLERSTATUS";
    else if (msg==0x002B)  pMsg="WM_DRAWITEM";
    else if (msg==0x002C)  pMsg="WM_MEASUREITEM"; 
    else if (msg==0x002D)  pMsg="WM_DELETEITEM";  
    else if (msg==0x002E)  pMsg="WM_VKEYTOITEM";  
    else if (msg==0x002F)  pMsg="WM_CHARTOITEM";  
    else if (msg==0x0030)  pMsg="WM_SETFONT";
    else if (msg==0x0031)  pMsg="WM_GETFONT";
    else if (msg==0x0037)  pMsg="WM_QUERYDRAGICON";
    else if (msg==0x0039)  pMsg="WM_COMPAREITEM"; 
    else if (msg==0x0041)  pMsg="WM_COMPACTING";  
    else if (msg==0x0044)  pMsg="WM_COMMNOTIFY";  
    else if (msg==0x0046)  pMsg="WM_WINDOWPOSCHANGING";
    else if (msg==0x0047)  pMsg="WM_WINDOWPOSCHANGED";
    else if (msg==0x0048)  pMsg="WM_POWER"; 
    else if (msg==0x0081)  pMsg="WM_NCCREATE";
    else if (msg==0x0082)  pMsg="WM_NCDESTROY";
    else if (msg==0x0083)  pMsg="WM_NCCALCSIZE";  
    else if (msg==0x0084)  pMsg="WM_NCHITTEST";
    else if (msg==0x0085)  pMsg="WM_NCPAINT";
    else if (msg==0x0086)  pMsg="WM_NCACTIVATE";  
    else if (msg==0x0087)  pMsg="WM_GETDLGCODE";  
    else if (msg==0x00A0)  pMsg="WM_NCMOUSEMOVE"; 
    else if (msg==0x00A1)  pMsg="WM_NCLBUTTONDOWN";
    else if (msg==0x00A2)  pMsg="WM_NCLBUTTONUP"; 
    else if (msg==0x00A3)  pMsg="WM_NCLBUTTONDBLCLK";
    else if (msg==0x00A4)  pMsg="WM_NCRBUTTONDOWN";
    else if (msg==0x00A5)  pMsg="WM_NCRBUTTONUP"; 
    else if (msg==0x00A6)  pMsg="WM_NCRBUTTONDBLCLK";
    else if (msg==0x00A7)  pMsg="WM_NCMBUTTONDOWN";
    else if (msg==0x00A8)  pMsg="WM_NCMBUTTONUP"; 
    else if (msg==0x00A9)  pMsg="WM_NCMBUTTONDBLCLK";
    else if (msg==0x0100)  pMsg="WM_KEYDOWN";
    else if (msg==0x0101)  pMsg="WM_KEYUP"; 
    else if (msg==0x0102)  pMsg="WM_CHAR";  
    else if (msg==0x0103)  pMsg="WM_DEADCHAR";
    else if (msg==0x0104)  pMsg="WM_SYSKEYDOWN";  
    else if (msg==0x0105)  pMsg="WM_SYSKEYUP";
    else if (msg==0x0106)  pMsg="WM_SYSCHAR";
    else if (msg==0x0107)  pMsg="WM_SYSDEADCHAR"; 
    else if (msg==0x010d)  pMsg="WM_IME_STARTCOMPOSITION"; 
    else if (msg==0x010e)  pMsg="WM_IME_ENDCOMPOSITION"; 
    else if (msg==0x010f)  pMsg="WM_IME_COMPOSITION"; 
    else if (msg==0x0110)  pMsg="WM_INITDIALOG";  
    else if (msg==0x0111)  pMsg="WM_COMMAND";
    else if (msg==0x0112)  pMsg="WM_SYSCOMMAND";  
    else if (msg==0x0113)  pMsg="WM_TIMER"; 
    else if (msg==0x0114)  pMsg="WM_HSCROLL";
    else if (msg==0x0115)  pMsg="WM_VSCROLL";
    else if (msg==0x0116)  pMsg="WM_INITMENU";
    else if (msg==0x0117)  pMsg="WM_INITMENUPOPUP";
    else if (msg==0x011F)  pMsg="WM_MENUSELECT";  
    else if (msg==0x0120)  pMsg="WM_MENUCHAR";
    else if (msg==0x0121)  pMsg="WM_ENTERIDLE";
    else if (msg==0x0200)  pMsg="WM_MOUSEMOVE";
    else if (msg==0x0201)  pMsg="WM_LBUTTONDOWN"; 
    else if (msg==0x0202)  pMsg="WM_LBUTTONUP";
    else if (msg==0x0203)  pMsg="WM_LBUTTONDBLCLK";
    else if (msg==0x0204)  pMsg="WM_RBUTTONDOWN"; 
    else if (msg==0x0205)  pMsg="WM_RBUTTONUP";
    else if (msg==0x0206)  pMsg="WM_RBUTTONDBLCLK";
    else if (msg==0x0207)  pMsg="WM_MBUTTONDOWN"; 
    else if (msg==0x0208)  pMsg="WM_MBUTTONUP";
    else if (msg==0x0209)  pMsg="WM_MBUTTONDBLCLK";
    else if (msg==0x0210)  pMsg="WM_PARENTNOTIFY";
    else if (msg==0x0220)  pMsg="WM_MDICREATE";
    else if (msg==0x0221)  pMsg="WM_MDIDESTROY";  
    else if (msg==0x0222)  pMsg="WM_MDIACTIVATE"; 
    else if (msg==0x0223)  pMsg="WM_MDIRESTORE";  
    else if (msg==0x0224)  pMsg="WM_MDINEXT";
    else if (msg==0x0225)  pMsg="WM_MDIMAXIMIZE"; 
    else if (msg==0x0226)  pMsg="WM_MDITILE";
    else if (msg==0x0227)  pMsg="WM_MDICASCADE";  
    else if (msg==0x0228)  pMsg="WM_MDIICONARRANGE";
    else if (msg==0x0229)  pMsg="WM_MDIGETACTIVE";
    else if (msg==0x0230)  pMsg="WM_MDISETMENU";  
    else if (msg==0x0233)  pMsg="WM_DROPFILES";
    else if (msg==0x0282)  pMsg="WM_IME_NOTIFY";
    else if (msg==0x0284)  pMsg="WM_IME_COMPOSITIONFULL";
    else if (msg==0x0286)  pMsg="WM_IME_CHAR";
    else if (msg==0x0300)  pMsg="WM_CUT";
    else if (msg==0x0301)  pMsg="WM_COPY";  
    else if (msg==0x0302)  pMsg="WM_PASTE"; 
    else if (msg==0x0303)  pMsg="WM_CLEAR"; 
    else if (msg==0x0304)  pMsg="WM_UNDO";  
    else if (msg==0x0305)  pMsg="WM_RENDERFORMAT";
    else if (msg==0x0306)  pMsg="WM_RENDERALLFORMATS";
    else if (msg==0x0307)  pMsg="WM_DESTROYCLIPBOARD";
    else if (msg==0x0308)  pMsg="WM_DRAWCLIPBOARD";
    else if (msg==0x0309)  pMsg="WM_PAINTCLIPBOARD";
    else if (msg==0x030A)  pMsg="WM_VSCROLLCLIPBOARD";
    else if (msg==0x030B)  pMsg="WM_SIZECLIPBOARD";
    else if (msg==0x030C)  pMsg="WM_ASKCBFORMATNAME";
    else if (msg==0x030D)  pMsg="WM_CHANGECBCHAIN";
    else if (msg==0x030E)  pMsg="WM_HSCROLLCLIPBOARD";
    else if (msg==0x030F)  pMsg="WM_QUERYNEWPALETTE";
    else if (msg==0x0310)  pMsg="WM_PALETTEISCHANGING";
    else if (msg==0x0311)  pMsg="WM_PALETTECHANGED";
    else if (msg==0x0380)  pMsg="WM_PENWINFIRST"; 
    else if (msg==0x038F)  pMsg="WM_PENWINLAST";  
    else if (msg==0x0390)  pMsg="WM_COALESCE_FIRST";
    else if (msg==0x039F)  pMsg="WM_COALESCE_LAST";
    else if (msg==0x0400)  pMsg="WM_USER";  
    else if (msg==WM_PRINT)          pMsg="WM_PRINT";
    else if (msg==WM_PRINTCLIENT)    pMsg="WM_PRINTCLIENT";
    else if (msg==WM_HANDHELDFIRST)  pMsg="WM_HANDHELDFIRST";
    else if (msg==WM_HANDHELDLAST)   pMsg="WM_HANDHELDLAST";
    else if (msg==WM_HOTKEY)         pMsg="WM_HOTKEY";

    OurPrintf("%s",(LPBYTE)pMsg);

    return TRUE;
}

/******************************************************************************
    DispEnhMetaRecType:
    Display metarecord type
*******************************************************************************/
DispEnhMetaRecType(PTERWND w,int id)
{
   LPBYTE str=NULL;

   if (id== EMR_HEADER) str="HEADER";
   if (id== EMR_POLYBEZIER) str="POLYBEZIER";
   if (id== EMR_POLYGON) str="POLYGON";
   if (id== EMR_POLYLINE) str="POLYLINE";
   if (id== EMR_POLYBEZIERTO) str="POLYBEZIERTO";
   if (id== EMR_POLYLINETO) str="POLYLINETO";
   if (id== EMR_POLYPOLYLINE) str="POLYPOLYLINE";
   if (id== EMR_POLYPOLYGON) str="POLYPOLYGON";
   if (id== EMR_SETWINDOWEXTEX) str="SETWINDOWEXTEX";
   if (id== EMR_SETWINDOWORGEX) str="SETWINDOWORGEX";
   if (id== EMR_SETVIEWPORTEXTEX) str="SETVIEWPORTEXTEX";
   if (id== EMR_SETVIEWPORTORGEX) str="SETVIEWPORTORGEX";
   if (id== EMR_SETBRUSHORGEX) str="SETBRUSHORGEX";
   if (id== EMR_EOF) str="EOF";
   if (id== EMR_SETPIXELV) str="SETPIXELV";
   if (id== EMR_SETMAPPERFLAGS) str="SETMAPPERFLAGS";
   if (id== EMR_SETMAPMODE) str="SETMAPMODE";
   if (id== EMR_SETBKMODE) str="SETBKMODE";
   if (id== EMR_SETPOLYFILLMODE) str="SETPOLYFILLMODE";
   if (id== EMR_SETROP2) str="SETROP2";
   if (id== EMR_SETSTRETCHBLTMODE) str="SETSTRETCHBLTMODE";
   if (id== EMR_SETTEXTALIGN) str="SETTEXTALIGN";
   if (id== EMR_SETCOLORADJUSTMENT) str="SETCOLORADJUSTMENT";
   if (id== EMR_SETTEXTCOLOR) str="SETTEXTCOLOR";
   if (id== EMR_SETBKCOLOR) str="SETBKCOLOR";
   if (id== EMR_OFFSETCLIPRGN) str="OFFSETCLIPRGN";
   if (id== EMR_MOVETOEX) str="MOVETOEX";
   if (id== EMR_SETMETARGN) str="SETMETARGN";
   if (id== EMR_EXCLUDECLIPRECT) str="EXCLUDECLIPRECT";
   if (id== EMR_INTERSECTCLIPRECT) str="INTERSECTCLIPRECT";
   if (id== EMR_SCALEVIEWPORTEXTEX) str="SCALEVIEWPORTEXTEX";
   if (id== EMR_SCALEWINDOWEXTEX) str="SCALEWINDOWEXTEX";
   if (id== EMR_SAVEDC) str="SAVEDC";
   if (id== EMR_RESTOREDC) str="RESTOREDC";
   if (id== EMR_SETWORLDTRANSFORM) str="SETWORLDTRANSFORM";
   if (id== EMR_MODIFYWORLDTRANSFORM) str="MODIFYWORLDTRANSFORM";
   if (id== EMR_SELECTOBJECT) str="SELECTOBJECT";
   if (id== EMR_CREATEPEN) str="CREATEPEN";
   if (id== EMR_CREATEBRUSHINDIRECT) str="CREATEBRUSHINDIRECT";
   if (id== EMR_DELETEOBJECT) str="DELETEOBJECT";
   if (id== EMR_ANGLEARC) str="ANGLEARC";
   if (id== EMR_ELLIPSE) str="ELLIPSE";
   if (id== EMR_RECTANGLE) str="RECTANGLE";
   if (id== EMR_ROUNDRECT) str="ROUNDRECT";
   if (id== EMR_ARC) str="ARC";
   if (id== EMR_CHORD) str="CHORD";
   if (id== EMR_PIE) str="PIE";
   if (id== EMR_SELECTPALETTE) str="SELECTPALETTE";
   if (id== EMR_CREATEPALETTE) str="CREATEPALETTE";
   if (id== EMR_SETPALETTEENTRIES) str="SETPALETTEENTRIES";
   if (id== EMR_RESIZEPALETTE) str="RESIZEPALETTE";
   if (id== EMR_REALIZEPALETTE) str="REALIZEPALETTE";
   if (id== EMR_EXTFLOODFILL) str="EXTFLOODFILL";
   if (id== EMR_LINETO) str="LINETO";
   if (id== EMR_ARCTO) str="ARCTO";
   if (id== EMR_POLYDRAW) str="POLYDRAW";
   if (id== EMR_SETARCDIRECTION) str="SETARCDIRECTION";
   if (id== EMR_SETMITERLIMIT) str="SETMITERLIMIT";
   if (id== EMR_BEGINPATH) str="BEGINPATH";
   if (id== EMR_ENDPATH) str="ENDPATH";
   if (id== EMR_CLOSEFIGURE) str="CLOSEFIGURE";
   if (id== EMR_FILLPATH) str="FILLPATH";
   if (id== EMR_STROKEANDFILLPATH) str="STROKEANDFILLPATH";
   if (id== EMR_STROKEPATH) str="STROKEPATH";
   if (id== EMR_FLATTENPATH) str="FLATTENPATH";
   if (id== EMR_WIDENPATH) str="WIDENPATH";
   if (id== EMR_SELECTCLIPPATH) str="SELECTCLIPPATH";
   if (id== EMR_ABORTPATH) str="ABORTPATH";
   if (id== EMR_GDICOMMENT) str="GDICOMMENT";
   if (id== EMR_FILLRGN) str="FILLRGN";
   if (id== EMR_FRAMERGN) str="FRAMERGN";
   if (id== EMR_INVERTRGN) str="INVERTRGN";
   if (id== EMR_PAINTRGN) str="PAINTRGN";
   if (id== EMR_EXTSELECTCLIPRGN) str="EXTSELECTCLIPRGN";
   if (id== EMR_BITBLT) str="BITBLT";
   if (id== EMR_MASKBLT) str="MASKBLT";
   if (id== EMR_PLGBLT) str="PLGBLT";
   if (id== EMR_SETDIBITSTODEVICE) str="SETDIBITSTODEVICE";
   if (id== EMR_STRETCHDIBITS) str="STRETCHDIBITS";
   if (id== EMR_EXTCREATEFONTINDIRECTW) str="EXTCREATEFONTINDIRECTW";
   if (id== EMR_EXTTEXTOUTA) str="EXTTEXTOUTA";
   if (id== EMR_EXTTEXTOUTW) str="EXTTEXTOUTW";
   if (id== EMR_POLYBEZIER16) str="POLYBEZIER16";
   if (id== EMR_POLYGON16) str="POLYGON16";
   if (id== EMR_POLYLINE16) str="POLYLINE16";
   if (id== EMR_POLYBEZIERTO16) str="POLYBEZIERTO16";
   if (id== EMR_POLYLINETO16) str="POLYLINETO16";
   if (id== EMR_POLYPOLYLINE16) str="POLYPOLYLINE16";
   if (id== EMR_POLYPOLYGON16) str="POLYPOLYGON16";
   if (id== EMR_POLYDRAW16) str="POLYDRAW16";
   if (id== EMR_CREATEMONOBRUSH) str="CREATEMONOBRUSH";
   if (id== EMR_CREATEDIBPATTERNBRUSHPT) str="CREATEDIBPATTERNBRUSHPT";
   if (id== EMR_EXTCREATEPEN) str="EXTCREATEPEN";
   if (id== EMR_POLYTEXTOUTA) str="POLYTEXTOUTA";
   if (id== EMR_POLYTEXTOUTW) str="POLYTEXTOUTW";
   if (id== EMR_SETICMMODE) str="SETICMMODE";
   if (id== EMR_CREATECOLORSPACE) str="CREATECOLORSPACE";
   if (id== EMR_SETCOLORSPACE) str="SETCOLORSPACE";
   if (id== EMR_DELETECOLORSPACE) str="DELETECOLORSPACE";
   if (id== EMR_GLSRECORD) str="GLSRECORD";
   if (id== EMR_GLSBOUNDEDRECORD) str="GLSBOUNDEDRECORD";
   if (id== EMR_PIXELFORMAT) str="PIXELFORMAT";
   //if (id== EMR_DRAWESCAPE) str="DRAWESCAPE";
   //if (id== EMR_EXTESCAPE) str="EXTESCAPE";
   //if (id== EMR_STARTDOC) str="STARTDOC";
   //if (id== EMR_SMALLTEXTOUT) str="SMALLTEXTOUT";
   //if (id== EMR_FORCEUFIMAPPING) str="FORCEUFIMAPPING";
   //if (id== EMR_NAMEDESCAPE) str="NAMEDESCAPE";
   //if (id== EMR_COLORCORRECTPALETTE) str="COLORCORRECTPALETTE";
   //if (id== EMR_SETICMPROFILEA) str="SETICMPROFILEA";
   //if (id== EMR_SETICMPROFILEW) str="SETICMPROFILEW";
   //if (id== EMR_ALPHABLEND) str="ALPHABLEND";
   //if (id== EMR_ALPHADIBBLEND) str="ALPHADIBBLEND";
   //if (id== EMR_TRANSPARENTBLT) str="TRANSPARENTBLT";
   //if (id== EMR_TRANSPARENTDIB) str="TRANSPARENTDIB";
   //if (id== EMR_GRADIENTFILL) str="GRADIENTFILL";
   //if (id== EMR_SETLINKEDUFIS) str="SETLINKEDUFIS";
   //if (id== EMR_SETTEXTJUSTIFICATION) str="SETTEXTJUSTIFICATION";

   if (str) OurPrintf("%s",str);
   else     OurPrintf("Rec type: %d",id);

   return TRUE;
} 

    

