/*==============================================================================
   TER_FMT.C
   TER Format Handling functions.

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
   ReadCfmt:
   1.  read the character formatting information from the input file (argument #1).
       The character formatting information starts at the position specified by
       the second argument.
   2.  The routine also read the font table, which is located after the
       character formatting information.
   3.  Create the fonts as specified in the font table.
*******************************************************************************/
BOOL ReadCfmt(PTERWND w,LPBYTE InputFile,BYTE huge *buf,long CfmtPos,BYTE InputType)
{
    int  i,j,font,RecLen,size;
    HANDLE InFile=INVALID_HANDLE_VALUE;
    int  FmtLen,len;
    LPBYTE pImage,pInfo,pObj;
    LPWORD fmt,ct;
    BYTE  signature,OnePfmt=0;
    WORD  fonts;
    DWORD ImageSize,InfoSize;
    BYTE temp[MAX_WIDTH];
    struct StrFontIO FontRec;
    struct StrParaIO ParaRec;
    long   l,line,ZeroCount;
    int   NewCount;
    BOOL AnimUsed;

    if (CfmtPos==0) return TRUE;

    // read the file information block
    if (!ReadFileInfo(w,InputFile,buf,InputType,TRUE)) return TRUE;
   
    if (FileInfo.RevId>63) return true;  // this is an older ter dll reading an sse file created by more recent ter dll


    SelectObject(hTerDC,GetStockObject(SYSTEM_FONT));
    FontInUseSize=MAX_FONTS;
    pFontInUse=MemAlloc(FontInUseSize);
    for (i=0;i<FontInUseSize;i++) pFontInUse[i]=FALSE;
    for (i=0;i<MaxFonts;i++) if (TerFont[i].InUse) DeleteTerObject(w,i);   // delete old font/picture
    TerFont[DEFAULT_CFMT].InUse=pFontInUse[0]=TRUE;   // default should always be created

    //******************* read the character formatting data *********
    if (InputType=='F') {
        lstrcpy(TempString,InputFile);  // copy to data segment
        if ((InFile=CreateFile(TempString,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
            return PrintError(w,MSG_ERR_FILE_OPEN,"ReadCfmt");
        SetFilePointer(InFile,(long) CfmtPos,NULL,FILE_BEGIN);
    }

    for (line=0;line<TotalLines;line++) {
       if (FileInfo.RevId>=3) {         // length stored as integer
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FmtLen),(BYTE huge *)&FmtLen)) goto NO_CHAR_FMT;
       }
       else {                           // length stored as byte
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),(BYTE huge *)&(temp[0]))) goto NO_CHAR_FMT;
          FmtLen=(int)(BYTE)temp[0];
       }

       if (FmtLen==1) {
          if (FileInfo.RevId>=13) {    // fmt ids are WORD now
            if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(WORD),(BYTE huge *)&(cfmt(line).info.fmt))) goto NO_CHAR_FMT;
          }
          else {
            BYTE byte;
            if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&byte)) goto NO_CHAR_FMT;
            cfmt(line).info.fmt=byte;
          }
          //if (cfmt(line).info.fmt>=(WORD)MAX_FONTS) cfmt(line).info.fmt=DEFAULT_CFMT;
          font=cfmt(line).info.fmt;
          RecordFontInUse(w,font);
       }
       else {
          if (LineLen(line)!=FmtLen) goto NO_CHAR_FMT;

          if (FmtLen>0) {
             fmt=OpenCfmt(w,line);
             if (FileInfo.RevId>=13) {    // fmt ids are WORD now
                if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,FmtLen*sizeof(WORD),(BYTE huge *)fmt)) goto NO_CHAR_FMT;
             }
             else {
                if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,FmtLen,temp)) goto NO_CHAR_FMT;
                for (i=0;i<FmtLen;i++) fmt[i]=temp[i];
             }

             for (i=0;i<FmtLen;i++) {        // find the last font used
                //if (fmt[i]>=MAX_FONTS) fmt[i]=DEFAULT_CFMT;
                font=fmt[i];
                RecordFontInUse(w,font);         // mark the font in use
             }
             CloseCfmt(w,line);
          }
       }
    }

    goto READ_TAG_IDS;

    NO_CHAR_FMT:
    PrintError(w,MSG_CHAR_FMT_MISSING,NULL);


    READ_TAG_IDS:
    if (FileInfo.RevId<18) goto READ_LEAD_BYTES;    // tag ids not used

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto NO_TAG_IDS;

    for (line=0;line<TotalLines;line++) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FmtLen),(BYTE huge *)&FmtLen)) goto NO_TAG_IDS;

       if (FmtLen==0) {                // lines with no tag ids
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(ZeroCount),(BYTE huge *)&ZeroCount)) goto NO_TAG_IDS;
          line+=(ZeroCount-1);         // skip lines that do not use tag ids
       }
       else {
          if (LineLen(line)!=FmtLen) goto NO_TAG_IDS;

          ct=OpenCtid(w,line);
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,FmtLen*sizeof(WORD),(BYTE huge *)ct)) goto NO_TAG_IDS;
          CloseCfmt(w,line);
       }
    }

    goto READ_LEAD_BYTES;

    NO_TAG_IDS:
    PrintError(w,MSG_CHAR_FMT_MISSING,"Char Tag Ids");


    READ_LEAD_BYTES:
    if (FileInfo.RevId<23) goto READ_FONT_TABLE;    // tag ids not used

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto NO_LEAD_BYTES;

    for (line=0;line<TotalLines;line++) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FmtLen),(BYTE huge *)&FmtLen)) goto NO_LEAD_BYTES;

       if (FmtLen==0) {                // lines with no lead bytes
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(ZeroCount),(BYTE huge *)&ZeroCount)) goto NO_LEAD_BYTES;
          line+=(ZeroCount-1);         // skip lines that do not use tag ids
       }
       else {
          LPBYTE lead;

          if (LineLen(line)!=FmtLen) goto NO_LEAD_BYTES;

          lead=OpenLead(w,line);
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,FmtLen*sizeof(BYTE),(BYTE huge *)lead)) goto NO_LEAD_BYTES;
          CloseCfmt(w,line);
       }
    }

    goto READ_FONT_TABLE;

    NO_LEAD_BYTES:
    PrintError(w,MSG_CHAR_FMT_MISSING,"Char Lead Bytes");


    READ_FONT_TABLE:                 // read the fonts in use by the document

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto NO_FONT;
    if (FileInfo.RevId>=13) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(fonts),(BYTE huge *)&fonts)) goto NO_FONT;
    }
    else {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&(temp[0]))) goto NO_FONT;
       fonts=temp[0];
    }

    if (signature!=FmtSign && signature!=FmtSignOld) goto NO_FONT;
    TotalFonts=fonts;                // number of fonts in the font table

    // expand the font table if necessary
    if (TotalFonts>MaxFonts) ExpandFontTable(w,TotalFonts);
    if (TotalFonts>MAX_FONTS && !Win32) TotalFonts=MAX_FONTS;

    for (i=0;i<TotalFonts;i++) {     // read each font table element
       if (i>0) InitTerObject(w,i);  // The first slot already initialized

       if (signature==FmtSignOld) {
          RecLen=34;                       // bytes per font record
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,RecLen,temp)) goto NO_FONT;
          TerFont[i].style=(UINT)temp[0]; // first field is the font style
          FarMove(&temp[1],&(TerFont[i].TwipsSize),sizeof(int));
          FarMove(&temp[3],TerFont[i].TypeFace,31);
          TerFont[i].FontFamily=FF_DONTCARE;
          TerFont[i].PictType=PICT_DIBITMAP;
          TerFont[i].TextColor=0x000000;    // black color
          TerFont[i].TextBkColor=0xFFFFFF;  // white color
          TerFont[i].UlineColor=CLR_AUTO;   // underline color
          TerFont[i].bmWidth=TerFont[i].bmHeight=0;
          TerFont[i].ObjectType=OBJ_NONE;
          TerFont[i].ObjectAspect=OASPECT_NONE;
          TerFont[i].ObjectUpdate=FALSE;
          TerFont[i].FieldId=TerFont[i].AuxId=TerFont[i].Aux1Id=0; // default field id
          TerFont[i].CharId=0; // default field id
          TerFont[i].PictAlign=ALIGN_BOT;   // picture alignment
          TerFont[i].FrameType=PFRAME_NONE; // picture not in a frame
          TerFont[i].ParaFID=0;             // para frame id
          TerFont[i].CharStyId=1;           // default character style
          TerFont[i].ParaStyId=0;           // default paragraph style
          TerFont[i].expand=0;              // default character space expansion
          TerFont[i].lang=0;
          TerFont[i].offset=0;              // offset from the base
          TerFont[i].UcBase=0;              // UcBase
          TerFont[i].CharSet=DEFAULT_CHARSET;
          TerFont[i].TextAngle=0;
          TerFont[i].CropLeft=0;
          TerFont[i].CropRight=0;
          TerFont[i].CropTop=0;
          TerFont[i].CropBot=0;
          TerFont[i].InsRev=0;
          TerFont[i].InsTime=0;
          TerFont[i].DelRev=0;
          TerFont[i].DelTime=0;
          FontRec.LinkNameLen=0;            // length of the linked picture file name
       }
       else {
          RecLen=sizeof(FontRec);
          if (Win32 && FileInfo.RevId<25) RecLen-=20;  // older win32 records were 20 bytes short
          if (FileInfo.RevId<29) RecLen-=32;           // expanded typeface[32] was added on rev 29
          if (FileInfo.RevId<45) RecLen-=37;           // original picture information was added in rev 45
          if (FileInfo.RevId<51) RecLen-=30;           // 30 bytes add to support picture cropping in rev 51
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,RecLen,(LPBYTE)&FontRec)) goto NO_FONT;
          TerFont[i].style=FontRec.style;
          TerFont[i].TwipsSize=FontRec.TwipsSize;
          if (FileInfo.RevId<29) lstrcpy(TerFont[i].TypeFace,FontRec.OldTypeFace);
          else                   lstrcpy(TerFont[i].TypeFace,FontRec.TypeFace);
          TerFont[i].FontFamily=FontRec.FontFamily;
          TerFont[i].TextColor=FontRec.TextColor;// text color
          TerFont[i].PictType=FontRec.PictType;  // picture type if font represents a picture
          TerFont[i].bmWidth=FontRec.bmWidth;    // actual width of the picture
          TerFont[i].bmHeight=FontRec.bmHeight;  // actual height of the picture
          TerFont[i].ObjectType=FontRec.ObjectType;
          TerFont[i].ObjectSize=FontRec.ObjectSize;
          TerFont[i].TextBkColor=FontRec.TextBkColor;// text background color
          TerFont[i].UlineColor=FontRec.UlineColor;  // underline color
          TerFont[i].FieldId=FontRec.FieldId;        // text background color
          TerFont[i].AuxId=FontRec.AuxId;            // text background color
          TerFont[i].CharId=FontRec.CharId;          // text background color
          TerFont[i].PictAlign=FontRec.PictAlign;    // text background color
          TerFont[i].FrameType=FontRec.FrameType;  // horizontal picture alignement
          TerFont[i].ParaFID=FontRec.ParaFID;        // paragraph frame id for the framed picture
          TerFont[i].OrigPictType=FontRec.OrigPictType;
          TerFont[i].OrigImageSize=FontRec.OrigImageSize;
          
          if (FileInfo.RevId>=8) TerFont[i].CharStyId=FontRec.CharStyId;
          else                   TerFont[i].CharStyId=1; // default character style
          if (FileInfo.RevId>=8) TerFont[i].ParaStyId=FontRec.ParaStyId;
          else                   TerFont[i].ParaStyId=0; // default paragraph style
          if (FileInfo.RevId>=25) TerFont[i].expand=FontRec.expand;
          else                    TerFont[i].expand=0; // default paragraph style
          if (FileInfo.RevId<27) TerFont[i].TwipsSize*=20;     // used to be specified in points
          if (FileInfo.RevId<28 && TerFont[i].style&PICT && TerFont[i].PictType==PICT_METAFILE) {
             TerFont[i].bmWidth=ScrToTwipsX(TerFont[i].bmWidth);
             TerFont[i].bmHeight=ScrToTwipsX(TerFont[i].bmHeight);
          }
          if (FileInfo.RevId>=30) TerFont[i].lang=FontRec.lang;
          else                    TerFont[i].lang=0; // default language
          if (FileInfo.RevId>=32) TerFont[i].offset=FontRec.offset;
          else                    TerFont[i].offset=0; // default language
          if (FileInfo.RevId>=33) TerFont[i].ObjectAspect=FontRec.ObjectAspect;
          else                    TerFont[i].ObjectAspect=OASPECT_CONTENT; // object aspect type
          if (FileInfo.RevId>=41) TerFont[i].UcBase=FontRec.UcBase;
          else                    TerFont[i].UcBase=0;     // Unicode base
          if (FileInfo.RevId<42)  ResetUintFlag(TerFont[i].style,ENOTETEXT);  // this bit was previously used for ULINEF
          
          if (FileInfo.RevId>=44) TerFont[i].CharSet=FontRec.CharSet;
          else                    TerFont[i].CharSet=DEFAULT_CHARSET;     // Unicode base
          if (FileInfo.RevId>=49) TerFont[i].ObjectUpdate=FontRec.ObjectUpdate;
          else                    TerFont[i].ObjectUpdate=FALSE;

          if (FileInfo.RevId<45) {
             TerFont[i].OrigPictType=0;
             TerFont[i].OrigImageSize=0;
             TerFont[i].pOrigImage=0;
          }
           
          if (FileInfo.RevId>=48) TerFont[i].TextAngle=FontRec.TextAngle;
          else                    TerFont[i].TextAngle=0;     // Unicode base

          if (FileInfo.RevId>=51) {
             TerFont[i].CropLeft=FontRec.CropLeft;
             TerFont[i].CropRight=FontRec.CropRight;
             TerFont[i].CropTop=FontRec.CropTop;
             TerFont[i].CropBot=FontRec.CropBot;
          } 
          else {
             TerFont[i].CropLeft=0;
             TerFont[i].CropRight=0;
             TerFont[i].CropTop=0;
             TerFont[i].CropBot=0;
             TerFont[i].OrigPictWidth=0;   // will be assigned to PictWidth on picture realization
             TerFont[i].OrigPictHeight=0;
          } 
          if (FileInfo.RevId<55)  TerFont[i].UlineColor=CLR_AUTO;
          if (FileInfo.RevId<61)  TerFont[i].CharId=0;
          
          if (FileInfo.RevId>=62)  {
             TerFont[i].InsRev=FontRec.InsRev;
             TerFont[i].InsTime=FontRec.InsTime;
             TerFont[i].DelRev=FontRec.DelRev;
             TerFont[i].DelTime=FontRec.DelTime;
          }
          else {
             TerFont[i].InsRev=0;
             TerFont[i].InsTime=0;
             TerFont[i].DelRev=0;
             TerFont[i].DelTime=0;
          }      
       }


       // read the field code
       if (TerFont[i].FieldId>0 && FileInfo.RevId>=31) {
          int len;
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(len),(BYTE huge *)&len)) goto NO_FONT;
          if (len>0) {
             TerFont[i].FieldCode=OurAlloc(len+1);
             if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,len,(BYTE huge *)TerFont[i].FieldCode)) goto NO_FONT;
             TerFont[i].FieldCode[len]=0;
          }  
       }
       else TerFont[i].FieldCode=NULL; 

       // read the picture information
       if (TerFont[i].style&PICT) {      // read in the picture bitmap
           TerFont[i].hObject=NULL;
           TerFont[i].pImage=TerFont[i].pInfo=NULL;
           TerFont[i].LinkFile=NULL;

           //**** read the link file name
           if (FontRec.LinkNameLen>1) {   // includes the NULL in the length
              if (NULL==(TerFont[i].LinkFile=OurAlloc(FontRec.LinkNameLen))) {  // length of the link file name
                  PrintError(w,MSG_OUT_OF_MEM,"Link file name");
                  TerFont[i].style=0;
                  goto NO_FONT;
              }
              if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,FontRec.LinkNameLen,(BYTE huge *)(TerFont[i].LinkFile))) goto NO_FONT;
           }

           //**** read the display dimensions of the picture ******
           if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(UINT),(BYTE huge *)&(TerFont[i].PictHeight))) goto NO_FONT;
           if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(UINT),(BYTE huge *)&(TerFont[i].PictWidth))) goto NO_FONT;

           if (FileInfo.RevId<28) {
              TerFont[i].PictWidth*=20;     // convert to twips
              TerFont[i].PictHeight*=20;     // convert to twips
           }

           //*** read the picture storage size in bytes ***
           if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(DWORD),(BYTE huge *)&ImageSize)) goto NO_FONT;
           if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(DWORD),(BYTE huge *)&InfoSize)) goto NO_FONT;
           TerFont[i].ImageSize=ImageSize;
           TerFont[i].InfoSize=InfoSize;

           if (ImageSize>0) {     // read image data
               if  (NULL==(TerFont[i].pImage=OurAlloc(ImageSize))) {
                     PrintError(w,MSG_OUT_OF_MEM,"Image Data");
                     TerFont[i].style=0;
                     goto NO_FONT;
               }
               else {
                  pImage=(LPVOID)TerFont[i].pImage;
                  if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,ImageSize,pImage)) goto NO_FONT;
               }
           }
           if (InfoSize>0) {      // read image information
               int SizeAdj=0;
               BOOL FixStrForm=false;
               if (FileInfo.RevId<58 && InfoSize==sizeof(struct StrFormOld) && (TerFont[i].PictType==PICT_FORM || TerFont[i].PictType==PICT_CONTROL)) {
                  TerFont[i].InfoSize=sizeof(struct StrForm);   // new size
                  SizeAdj=sizeof(struct StrForm)-InfoSize;
                  FixStrForm=true;
               } 
               if  (NULL==(TerFont[i].pInfo=OurAlloc(InfoSize+SizeAdj))) {
                     PrintError(w,MSG_OUT_OF_MEM,"Image info");
                     TerFont[i].style=0;
                     goto NO_FONT;
               }
               else {
                  pInfo=(LPVOID)TerFont[i].pInfo;
                  if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,InfoSize,pInfo)) goto NO_FONT;
                  if (FixStrForm) DoFixStrForm(w,pInfo);
               }
           }
           // read the object storage data
           if (TerFont[i].ObjectType!=OBJ_NONE && TerFont[i].ObjectSize>0) {
               if  (NULL==(TerFont[i].hObject=GlobalAlloc(GMEM_MOVEABLE,TerFont[i].ObjectSize))
                 || NULL==(pObj=GlobalLock(TerFont[i].hObject)) ) {
                     PrintError(w,MSG_OUT_OF_MEM,"Object data");
                     TerFont[i].style=0;
                     goto NO_FONT;
               }
               else {
                  if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,TerFont[i].ObjectSize,pObj)) goto NO_FONT;
                  GlobalUnlock(TerFont[i].hObject);
               }
           }

           // read the animation information
           if (FileInfo.RevId>=19) {
              if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(AnimUsed),(LPBYTE)&AnimUsed)) goto NO_FONT;
              if (AnimUsed) {
                 CreateAnimInfo(w,i);
                 if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrAnim),(LPBYTE)TerFont[i].anim)) goto NO_FONT;
                 TerFont[i].anim->LoopCount=TerFont[i].anim->OrigLoopCount;
                 TerFont[i].anim->CurAnim=i;
                 TerFont[i].anim->NextAnim=0;
              }
           }

           // read the original picture information
           if (FileInfo.RevId>=45 && TerFont[i].OrigImageSize>0) {
               if  (NULL==(TerFont[i].pOrigImage=OurAlloc(TerFont[i].OrigImageSize))) {
                     PrintError(w,MSG_OUT_OF_MEM,"Original Image Data");
                     TerFont[i].OrigImageSize=0;
                     TerFont[i].OrigPictType=0;
               }
               else {
                  if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,TerFont[i].OrigImageSize,TerFont[i].pOrigImage)) goto NO_FONT;
               }
           } 
           
           if (ImageSize==0 && InfoSize==0 && TerFont[i].ObjectSize==0 && !TerFont[i].LinkFile) ResetUintFlag(TerFont[i].style,PICT); // picture missing
       }
    }


    goto READ_PARA_FMT;

    NO_FONT:
    PrintError(w,MSG_FONT_TBL_INCOMPL,NULL);
    goto END_READ;


    READ_PARA_FMT:                         // read paragraph and cell ids

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,1,&signature)) goto NO_PARA_FMT;
    if (signature!=FmtSign && signature!=FmtSignOld) goto NO_PARA_FMT;

    if (FileInfo.RevId>=2) {
       for (l=0;l<TotalLines;l++) {        // read each pfmt id
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(pfmt(l)))) goto NO_PARA_FMT;
          if (FileInfo.RevId>=11) {
             if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(cid(l)))) goto NO_PARA_FMT;
             if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(fid(l)))) goto NO_PARA_FMT;
             if (FileInfo.RevId<33 && cid(l)>0) fid(l)=0;  // table-in-frame not supported in earlier versions
          }
       }
    }
    else {
       for (l=0;l<TotalLines;l++) {        // read each pfmt id
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),(BYTE huge *)&OnePfmt)) goto NO_PARA_FMT;
          pfmt(l)=(int)OnePfmt;
       }
    }

    goto READ_PARA_TABLE;

    NO_PARA_FMT:
    PrintError(w,MSG_PARA_FMT_MISSING,NULL);
    goto END_READ;


    READ_PARA_TABLE:                      // read the paragraph table

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,1,&signature)) goto NO_PARA_TABLE;
    if (signature!=FmtSign && signature!=FmtSignOld) goto NO_PARA_TABLE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalPfmts)) goto NO_PARA_TABLE;

    if (TotalPfmts<1) TotalPfmts=1;
    
    if (TotalPfmts>MaxPfmts) {            // enlarge the para table
       MaxPfmts=TotalPfmts;
       GlobalUnlock(hPfmtId);
       GlobalFree(hPfmtId);
       if ( NULL==(hPfmtId=GlobalAlloc(GMEM_MOVEABLE,((long)MaxPfmts+1)*sizeof(struct StrPfmt)))
         || NULL==(PfmtId=(struct StrPfmt far *)GlobalLock(hPfmtId)) ) goto NO_PARA_TABLE;
    }

    for (i=0;i<TotalPfmts;i++) {   // read each paragraph entry
       if (signature==FmtSign) {
          int AdjustSize=0;
          if (FileInfo.RevId<5) { // older version used smaller structure sizes
             if (Win32) AdjustSize=-32;
             else       AdjustSize=-16;
          }
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(ParaRec) + AdjustSize,(LPBYTE)&ParaRec)) goto NO_PARA_TABLE;
          PfmtId[i].LeftIndentTwips=ParaRec.LeftIndent;
          PfmtId[i].RightIndentTwips=ParaRec.RightIndent;
          PfmtId[i].FirstIndentTwips=ParaRec.FirstIndent;
          PfmtId[i].flags=ParaRec.flags;
          PfmtId[i].TabId=ParaRec.TabId;
          PfmtId[i].BltId=ParaRec.BltId;
          PfmtId[i].AuxId=ParaRec.AuxId;
          PfmtId[i].shading=ParaRec.shading;
          PfmtId[i].pflags=ParaRec.pflags;
          PfmtId[i].BorderSpace=ParaRec.BorderSpace;
          PfmtId[i].BorderColor=ParaRec.BorderColor;
          if (AdjustSize==0) {   // uses new structure size
             PfmtId[i].SpaceBefore=ParaRec.SpaceBefore;
             PfmtId[i].SpaceAfter=ParaRec.SpaceAfter;
             PfmtId[i].SpaceBetween=ParaRec.SpaceBetween;
             PfmtId[i].LineSpacing=ParaRec.LineSpacing;
             PfmtId[i].StyId=ParaRec.StyId;  // paragraph style id
             PfmtId[i].BkColor=ParaRec.BkColor;
          }
          else {
             PfmtId[i].SpaceBefore=PfmtId[i].SpaceAfter=PfmtId[i].SpaceBetween=0;
             PfmtId[i].LineSpacing=0;
             PfmtId[i].AuxId=0; 
             PfmtId[i].StyId=0;              // default paragraph style id
             PfmtId[i].BkColor=CLR_WHITE;
          }
          if (FileInfo.RevId<8)  PfmtId[i].StyId=0;
          if (FileInfo.RevId<17) PfmtId[i].BkColor=CLR_WHITE;
          if (FileInfo.RevId<38) PfmtId[i].BorderSpace=PARA_BOX_SPACE;
          if (FileInfo.RevId<59) PfmtId[i].BorderColor=CLR_AUTO;
       }
       else {                      // read the old format table
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(PfmtId[i].LeftIndentTwips))) goto NO_PARA_TABLE;
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(PfmtId[i].RightIndentTwips))) goto NO_PARA_TABLE;
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(PfmtId[i].flags))) goto NO_PARA_TABLE;
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,20,temp)) goto NO_PARA_TABLE;     // reserved space
          PfmtId[i].FirstIndentTwips=0;    // use default for this valuef
          PfmtId[i].TabId=0;               // use default for this valuef
          PfmtId[i].BltId=0;               // use default for this valuef
          PfmtId[i].shading=0;
          PfmtId[i].pflags=0;
          PfmtId[i].StyId=0;               // default paragraph style
          PfmtId[i].BkColor=CLR_WHITE;
          PfmtId[i].BorderSpace=PARA_BOX_SPACE;
          PfmtId[i].flow=FLOW_DEF;
          PfmtId[i].BorderColor=CLR_AUTO;
       }
       if (!(PfmtId[i].flags&(CENTER|RIGHT_JUSTIFY|JUSTIFY))) PfmtId[i].flags|=LEFT;
    }

    // assign cell ids when reading an old version of the file
    if (FileInfo.RevId<11) {
       for (l=0;l<TotalLines;l++) {
          cid(l)=PfmtId[pfmt(l)].BltId;     // this field was previously used to store the cell id
          fid(l)=PfmtId[pfmt(l)].pflags;    // para id field is reused to store additional flags
       }
       for (i=1;i<TotalPfmts;i++)   {
         if (PfmtId[i].pflags>0) PfmtId[i].pflags=PFLAG_FRAME;
         PfmtId[i].BltId=0;           // not used in the earlier versions
       }
    }


    goto READ_TAB_TABLE;

    NO_PARA_TABLE:
    PrintError(w,MSG_PARA_TBL_INCOMPL,NULL);
    for (l=0;l<TotalLines;l++) if (pfmt(l)>=TotalPfmts) pfmt(l)=DEFAULT_PFMT;

    READ_TAB_TABLE:                      // read the tab table

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,1,&signature)) goto NO_TAB_TABLE;
    if (signature!=FmtSign) goto NO_TAB_TABLE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalTabs)) goto NO_TAB_TABLE;

    if (TotalTabs>MAX_TABS) TotalTabs=MAX_TABS;
    if (TotalTabs<1)        TotalTabs=1;

    if (FileInfo.RevId>=8) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrTab)*TotalTabs,(LPBYTE)TerTab)) goto NO_TAB_TABLE;
    }
    else {                  // read shorter records
       for (i=0;i<TotalTabs;i++) {
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrTab)-sizeof(BYTE)*TE_MAX_TAB_STOPS,(LPBYTE)&(TerTab[i]))) goto NO_TAB_TABLE;
          for (j=0;j<TE_MAX_TAB_STOPS;j++) TerTab[i].flags[j]=0;   // initialize to zero
       }
    }

    goto READ_SECT_TABLE;

    NO_TAB_TABLE:
    PrintError(w,MSG_TAB_TBL_MISSING,NULL);
    for (i=0;i<TotalPfmts;i++) PfmtId[i].TabId=0;
    goto END_READ;

    READ_SECT_TABLE:                      // read the section table
    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto NO_SECT_TABLE;
    if (signature!=FmtSign) goto NO_SECT_TABLE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalSects)) goto NO_SECT_TABLE;

    if (TotalSects>MaxSects)  ExpandSectArray(w,TotalSects+1);
    if (TotalSects<1)         TotalSects=1;

    if (FileInfo.RevId<50) { // smaller records
       int size=sizeof(struct StrSect)-24;  // 24 bytes added at RevId 50
       if (FileInfo.RevId<46) size-=58;     // 46 bytes added at revid 46
       for (i=0;i<TotalSects;i++) {
          FarMemSet(&(TerSect[i]),0,sizeof(struct StrSect));
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,size,(LPBYTE)&(TerSect[i]))) goto NO_SECT_TABLE;
       } 
    } 
    else if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrSect)*TotalSects,(LPBYTE)TerSect)) goto NO_SECT_TABLE;

    // fix fields for older versions
    for (i=0;i<TotalSects;i++) {
       if (FileInfo.RevId<8) TerSect[i].orient=DMORIENT_PORTRAIT;
       if (FileInfo.RevId<15) {
          if (TerSect[i].flags==0) TerSect[i].flags=SECT_NEW_PAGE;
          else                     TerSect[i].flags=0;
       }
       if (FileInfo.RevId<20) {
          TerSect[i].HdrMargin=TerSect[i].TopMargin;
          TerSect[i].FtrMargin=TerSect[i].BotMargin;
       }
       if (FileInfo.RevId<40) TerSect[i].PageNumFmt=NBR_DEC;
       if (FileInfo.RevId<46) TerSect[i].border=0;
       if (FileInfo.RevId<47) {
          TerSect[i].PprSize=PaperSize;
          TerSect[i].PprWidth=PageWidth;
          TerSect[i].PprHeight=PageHeight;
       }
       if (FileInfo.RevId<50) TerSect[i].flow=FLOW_DEF;
       if (FileInfo.RevId<52) TerSect[i].AuxId=0;
    }


    goto READ_TABLE_ARRAY;

    NO_SECT_TABLE:
    PrintError(w,MSG_SECT_TBL_MISSING,NULL);
    goto END_READ;


    READ_TABLE_ARRAY:                      // read the table array
    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_TABLE_ARRAY;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalTableRows)) goto NO_TABLE_ARRAY;

    if (TotalTableRows<1) TotalTableRows=1;

    if (TotalTableRows>MaxTableRows) {            // enlarge the TableRow table
       MaxTableRows=TotalTableRows;
       GlobalUnlock(hTableRow);
       GlobalFree(hTableRow);
       OurFree(TableAux);
       if ( NULL==(hTableRow=GlobalAlloc(GMEM_MOVEABLE,((long)MaxTableRows+1)*sizeof(struct StrTableRow)))
         || NULL==(TableRow=(struct StrTableRow far *)GlobalLock(hTableRow))
         || NULL==(TableAux=OurAlloc(((long)(MaxTableRows+1))*sizeof(struct StrTableAux))) ) goto NO_CELL_TABLE;
    }

    if (FileInfo.RevId>=10 || Win32) {
       if (Win32 && FileInfo.RevId<21) {
          for (i=0;i<TotalTableRows;i++) {
             FarMemSet(&(TableRow[i]),0,sizeof(struct StrTableRow));
             if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrTableRow)-24,(LPBYTE)&(TableRow[i]))) goto NO_TABLE_ARRAY;  // older records were 22 bytes shorter
          }
       }
       else if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrTableRow)*TotalTableRows,(LPBYTE)TableRow)) goto NO_TABLE_ARRAY;
    }
    else {         // read individual elements
       for (i=0;i<TotalTableRows;i++) {
          FarMemSet(&(TableRow[i]),0,sizeof(struct StrTableRow));
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrTableRow)-22,(LPBYTE)&(TableRow[i]))) goto NO_TABLE_ARRAY;  // older records were 22 bytes shorter
       }
    }

    // initialize fields for the previous versions
    for (i=0;i<TotalTableRows;i++) {
        if (FileInfo.RevId<8) TableRow[i].MinHeight=0;
        if (FileInfo.RevId<50) TableRow[i].flow=FLOW_DEF;
    }


    goto READ_CELL_TABLE;

    NO_TABLE_ARRAY:
    PrintError(w,MSG_TBL_DATA_MISSING,NULL);
    goto END_READ;


    READ_CELL_TABLE:                      // read the section table
    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_CELL_TABLE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalCells)) goto NO_CELL_TABLE;

    if (TotalCells<1)         TotalCells=1;
    
    if (TotalCells>MaxCells) {            // enlarge the cell table
       MaxCells=TotalCells;
       GlobalUnlock(hCell);
       GlobalFree(hCell);
       OurFree(CellAux);
       if ( NULL==(hCell=GlobalAlloc(GMEM_MOVEABLE,((long)MaxCells+1)*sizeof(struct StrCell)))
         || NULL==(cell=(struct StrCell far *)GlobalLock(hCell))
         || NULL==(CellAux=OurAlloc(((long)(MaxCells+1))*sizeof(struct StrCellAux))) ) goto NO_CELL_TABLE;
    }
    for (i=0;i<MaxCells;i++) FarMemSet(&(CellAux[i]),0,sizeof(struct StrCellAux));

    if (FileInfo.RevId>=50) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrCell)*TotalCells,(LPBYTE)cell)) goto NO_CELL_TABLE;
    }
    else {         // read individual cell elements
       int adj=0;
       
       if (FileInfo.RevId<12) adj+=16;       // older win32 records were 16 bytes shorter
       if (FileInfo.RevId<43) adj+=16;      // 16 bytes added for BorderColor[4]
       if (FileInfo.RevId<50) adj+=24;      // 24 bytes added at rev id 50

       for (i=0;i<TotalCells;i++) {
          int j;
          FarMemSet(&(cell[i]),0,sizeof(struct StrCell));
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrCell)-adj,(BYTE huge *)&(cell[i]))) goto NO_CELL_TABLE;  // older records were 20 bytes shorter
          if (FileInfo.RevId<12) cell[i].BackColor=CLR_WHITE;  // initialize new fields
          if (FileInfo.RevId<43) for (j=0;j<4;j++) cell[i].BorderColor[j]=CLR_AUTO;
       }
    }

    goto READ_PARA_FRAMES;

    NO_CELL_TABLE:
    PrintError(w,MSG_CELL_DATA_MISSING,NULL);
    goto END_READ;


    READ_PARA_FRAMES:                      // read the section table
    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_PARA_FRAMES;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalParaFrames)) goto NO_PARA_FRAMES;


    if (TotalParaFrames<1)               TotalParaFrames=1;

    if (TotalParaFrames>MaxParaFrames) {
       MaxParaFrames=TotalParaFrames;
       GlobalUnlock(hParaFrame);
       GlobalFree(hParaFrame);
       if ( NULL==(hParaFrame=GlobalAlloc(GMEM_MOVEABLE,((long)MaxParaFrames+1)*sizeof(struct StrParaFrame)))
         || NULL==(ParaFrame=(struct StrParaFrame far *)GlobalLock(hParaFrame)) ) goto NO_CELL_TABLE;
    }

    if (FileInfo.RevId>=60) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrParaFrame)*TotalParaFrames,(LPBYTE)ParaFrame)) goto NO_PARA_FRAMES;
    }
    else {                        // older records were 38 bytes shorter
       for (i=0;i<TotalParaFrames;i++) {
          int adj;
          FarMemSet(&(ParaFrame[i]),0,sizeof(struct StrParaFrame));
          if (FileInfo.RevId>=48)     adj=40;           // 40 bytes added at revid: 60
          else if (FileInfo.RevId>=7) adj=22+40;         // 22 bytes added at revid: 48
          else if (FileInfo.RevId>=6) adj=18+22+40;
          else                        adj=38+22+40;

          FarMemSet(&(ParaFrame[i]),0,sizeof(struct StrParaFrame));
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrParaFrame)-adj,(LPBYTE)&(ParaFrame[i]))) goto NO_PARA_FRAMES; // older records ver 32 bytes shorter

          ParaFrame[i].BackColor=0xFFFFFF;  // initialize new fields
          ParaFrame[i].FillPattern=1;
          ParaFrame[i].TextAngle=0;
          ParaFrame[i].OrgX=0;
          ParaFrame[i].OrgY=0;
       }
    }
    for (i=0;i<TotalParaFrames;i++) {  //initialize older rev variables
       if (FileInfo.RevId<22 && ParaFrame[i].flags&PARA_FRAME_OBJECT) ParaFrame[i].flags|=PARA_FRAME_WRAP_THRU;
    }

    goto READ_STYLE_SHEET;

    NO_PARA_FRAMES:
    PrintError(w,MSG_PFRAME_DATA_MISSING,NULL);
    goto END_READ;

    READ_STYLE_SHEET:                      // read the stylesheet table
    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_STYLE_SHEET;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalSID)) goto NO_STYLE_SHEET;

    if (TotalSID<2)      TotalSID=2;

    if (TotalSID>MaxSID) {            // enlarge the StyleId table
       MaxSID=TotalSID;
       GlobalUnlock(hStyleId);
       GlobalFree(hStyleId);
       if ( NULL==(hStyleId=GlobalAlloc(GMEM_MOVEABLE,((long)MaxSID+1)*sizeof(struct StrStyleId)))
         || NULL==(StyleId=(struct StrStyleId far *)GlobalLock(hStyleId)) ) goto NO_STYLE_SHEET;
    }

    size=sizeof(struct StrStyleId);
    if (FileInfo.RevId<56) {
       if (FileInfo.RevId<56) size-=44;  // 44 bytes added in rev 56
       if (FileInfo.RevId<29) size-=32;  // new typeface[32] was added on rev 29
       for (i=0;i<TotalSID;i++) {    // read one element at a time
          FarMemSet(&(StyleId[i]),0,sizeof(struct StrStyleId));
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)size,(LPBYTE)&(StyleId[i]))) goto NO_STYLE_SHEET;
          lstrcpy(StyleId[i].TypeFace,StyleId[i].OldTypeFace);
       }
    }
    else if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)size*TotalSID,(LPBYTE)StyleId)) goto NO_STYLE_SHEET;

    if (FileInfo.RevId<14) for (i=0;i<TotalSID;i++) StyleId[i].pflags=0;
    if (FileInfo.RevId<17) for (i=0;i<TotalSID;i++) StyleId[i].ParaBkColor=CLR_WHITE;
    if (FileInfo.RevId<27) for (i=0;i<TotalSID;i++) StyleId[i].TwipsSize*=20;            // used to be stored in point sizes
    if (FileInfo.RevId<39) for (i=0;i<TotalSID;i++) StyleId[i].LineSpacing=0;  
    if (FileInfo.RevId<56) for (i=0;i<TotalSID;i++) StyleId[i].OutlineLevel=-1;          // default 
    if (FileInfo.RevId<59) for (i=0;i<TotalSID;i++) StyleId[i].ParaBorderColor=CLR_AUTO;          // default 
    for (i=0;i<2 && i<TotalSID;i++) if (StyleId[i].ParaBkColor==0) StyleId[i].ParaBkColor=CLR_WHITE;

    goto READ_BULLET;

    NO_STYLE_SHEET:
    PrintError(w,MSG_STYLESHEET_MISSING,NULL);
    goto END_READ;

    READ_BULLET:                      // read the bullet table
    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_BULLET;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalBlts)) goto NO_BULLET;

    if (TotalBlts>MaxBlts) {   // expand the table
       OurFree(TerBlt);
       MaxBlts=TotalBlts;
       if (NULL==(TerBlt=OurAlloc(((long)(MaxBlts+1))*sizeof(struct StrBlt))) ) goto NO_LIST;
    }

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrBlt)*TotalBlts,(LPBYTE)TerBlt)) goto NO_BULLET;

    for (i=1;i<TotalBlts;i++) {
       if (FileInfo.RevId<26 && !TerBlt[i].IsBullet) {
          TerBlt[i].BefText[0]=TerBlt[i].BefChar;
          TerBlt[i].BefText[1]=0;
       }
       if (FileInfo.RevId<35) TerBlt[i].ls=TerBlt[i].lvl=0;
    }

    goto READ_TAG_TABLE;

    NO_BULLET:
    PrintError(w,MSG_BULLET_MISSING,NULL);
    goto END_READ;

    READ_TAG_TABLE:                    // read the tag table
    if (FileInfo.RevId<18) goto READ_EOF_SIGNATURE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_TAG_TABLE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalCharTags)) goto NO_TAG_TABLE;
    
    
    if (TotalCharTags>MaxCharTags) {   // expand the table
       MaxCharTags=TotalCharTags;
       OurFree(CharTag);
       if (NULL==(CharTag=OurAlloc(((long)(MaxCharTags+1))*sizeof(struct StrCharTag))) ) goto NO_TAG_TABLE;
    }

    for (i=0;i<TotalCharTags;i++) {
       if (FileInfo.RevId>=57) {
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrCharTag),(LPBYTE)&(CharTag[i]))) goto NO_TAG_TABLE;
       }
       else {
          FarMemSet((LPBYTE)&(CharTag[i]),0,sizeof(struct StrCharTag));
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrCharTag)-20,(LPBYTE)&(CharTag[i]))) goto NO_TAG_TABLE;  // old records 20 bytes short
       } 

       CharTag[i].name=CharTag[i].AuxText=NULL;
       
       // read the tag name
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&len)) goto NO_TAG_TABLE;
       if (len>0) {
          CharTag[i].name=MemAlloc(len+1);   // allocate space for the tag name
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,len,(LPBYTE)CharTag[i].name)) goto NO_TAG_TABLE;
          CharTag[i].name[len]=0;            // null terminate the string
       }
       
       // read the misc string, not in use currently
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&len)) goto NO_TAG_TABLE;

       // read the tag AuxText
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&len)) goto NO_TAG_TABLE;
       if (len>0) {
          CharTag[i].AuxText=MemAlloc(len+1);   // allocate space for the tag AuxText
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,len,(LPBYTE)CharTag[i].AuxText)) goto NO_TAG_TABLE;
          CharTag[i].AuxText[len]=0;
       }
    }

    goto READ_RTF_INFO;

    NO_TAG_TABLE:
    PrintError(w,MSG_TAG_TABLE_MISSING,NULL);
    goto END_READ;


    READ_RTF_INFO:
    if (FileInfo.RevId<34) goto READ_EOF_SIGNATURE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_RTF_INFO;

    for (i=0;i<INFO_MAX;i++) if (!TerReadRtfInfo(w,InputType,InFile,buf,&CfmtPos)) goto END_READ;
     
    goto READ_LIST;

    NO_RTF_INFO:
    PrintError(w,MSG_NO_RTF_INFO_BLOCK,NULL);
    goto END_READ;


    READ_LIST:                 // list table
    if (FileInfo.RevId<35) goto READ_EOF_SIGNATURE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_LIST;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&NewCount)) goto NO_LIST;
    if (NewCount>MaxLists) {   // expand the table
       for (i=1;i<TotalLists;i++) FreeList(w,i);  // free list table entires
       OurFree(list);
       MaxLists=TotalLists=NewCount;
       if (NULL==(list=OurAlloc(((long)(MaxLists+1))*sizeof(struct StrList))) ) goto NO_LIST;
    }
    else TotalLists=NewCount;


    for (i=0;i<TotalLists;i++) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrList),(LPBYTE)&(list[i]))) goto NO_TAG_TABLE;
       if (!list[i].InUse) continue;

       // read the list name
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&len)) goto NO_LIST;
       if (len>0) {
          list[i].name=MemAlloc(len+1);   // allocate space for the tag name
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,len,(LPBYTE)list[i].name)) goto NO_LIST;
          list[i].name[len]=0;            // null terminate the string
       }
       else list[i].name=NULL;


       // read list levels
       list[i].level=NULL;
       if (list[i].LevelCount>0) {
          list[i].level=MemAlloc(sizeof(struct StrListLevel)*list[i].LevelCount);
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrListLevel)*list[i].LevelCount,(LPBYTE)list[i].level)) goto NO_LIST;
       
          // mark the font as in use
          for (j=0;j<list[i].LevelCount;j++) {
             RecordFontInUse(w,list[i].level[j].FontId);
          }
       }
    }
     
    goto READ_LIST_OR;

    NO_LIST:
    PrintError(w,MSG_NO_LIST_BLOCK,NULL);
    
    goto END_READ;


    READ_LIST_OR:              // list override table
    if (FileInfo.RevId<35) goto READ_EOF_SIGNATURE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_LIST_OR;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&NewCount)) goto NO_LIST_OR;
    

    if (NewCount>MaxListOr) {   // expand the table
       for (i=1;i<TotalListOr;i++) FreeListOr(w,i);
       OurFree(ListOr);
       MaxListOr=TotalListOr=NewCount;
       if (NULL==(ListOr=OurAlloc(((long)(MaxListOr+1))*sizeof(struct StrListOr))) ) goto NO_LIST_OR;
    }
    else TotalListOr=NewCount;


    for (i=0;i<TotalListOr;i++) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrListOr),(LPBYTE)&(ListOr[i]))) goto NO_LIST_OR;
       if (!ListOr[i].InUse) continue;


       // read list levels
       ListOr[i].level=NULL;
       if (ListOr[i].LevelCount>0) {
          ListOr[i].level=MemAlloc(sizeof(struct StrListLevel)*ListOr[i].LevelCount);
          if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrListLevel)*ListOr[i].LevelCount,(LPBYTE)ListOr[i].level)) goto NO_LIST;
       
          // mark the font as in use
          for (j=0;j<ListOr[i].LevelCount;j++) {
             RecordFontInUse(w,ListOr[i].level[j].FontId);
          }
       }
    }
     
    goto READ_REVIEWER;

    NO_LIST_OR:
    PrintError(w,MSG_NO_LIST_OR_BLOCK,NULL);
    
    goto END_READ;


    READ_REVIEWER:              // reviewer table
    if (FileInfo.RevId<62) goto READ_EOF_SIGNATURE;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;
    if (signature==SIGN_EOF) goto READ_FILE_INFO;
    if (signature!=FmtSign) goto NO_LIST_OR;

    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&NewCount)) goto NO_REVIEWER;
    

    if (NewCount>MaxReviewers) {   // expand the table
       for (i=1;i<TotalReviewers;i++) FreeReviewer(w,i);
       OurFree(reviewer);
       MaxReviewers=TotalReviewers=NewCount;
       if (NULL==(reviewer=OurAlloc(((long)(MaxReviewers+1))*sizeof(struct StrReviewer))) ) goto NO_REVIEWER;
    }
    else TotalReviewers=NewCount;

    size=sizeof(struct StrReviewer);
    if (FileInfo.RevId<63) size-=24;     // 24 bytes add in rev 63
    for (i=0;i<TotalReviewers;i++) {
       if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)size,(LPBYTE)&(reviewer[i]))) goto NO_REVIEWER;
       if (FileInfo.RevId<63) {
          reviewer[i].RtfId=0;
          FarMemSet(&(reviewer[i].reserved),0,sizeof(reviewer[i].reserved));
       } 
    }
     
    goto READ_EOF_SIGNATURE;

    NO_REVIEWER:
    PrintError(w,MSG_NO_REVIEWER_BLOCK,NULL);
    
    goto END_READ;



    READ_EOF_SIGNATURE:
    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&signature)) goto END_READ;

    READ_FILE_INFO:
    if (signature!=SIGN_EOF) {
       PrintError(w,MSG_FILE_INFO_MISSING,NULL);
       goto END_READ;
    }
    if (!TerFarRead(w,InputType,InFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrFileInfo),(LPBYTE)&FileInfo)) goto END_READ;
    if (FileInfo.sign!=SIGN_EOF) {
       PrintError(w,MSG_FILE_SIGN_MISSING,NULL);
       goto END_READ;
    }


    END_READ:
    if (InputType=='F') CloseHandle(InFile);
    FileFormat=SAVE_TER;               // input in TER file format

    //***************** Set the InUse flag for the objects *******************
    if (TotalFonts>=FontInUseSize) {
       RecordFontInUse(w,TotalFonts);  // exapnd the pFontInUse array
       pFontInUse[TotalFonts]=FALSE;
    }
    
    for (i=0;i<TotalFonts;i++) {
       TerFont[i].InUse=pFontInUse[i];  // set the inuse flags
       if (IsFirstAnimPict(w,i)) {
         int NextPict=TerFont[i].anim->NextPict;
         while (NextPict>0 && NextPict<TotalFonts && (TerFont[NextPict].style&PICT) && TerFont[NextPict].anim) {
            TerFont[NextPict].InUse=pFontInUse[NextPict]=TRUE;
            NextPict=TerFont[NextPict].anim->NextPict;
         }
       }
       else if (IsFormField(w,i,0)) {
          struct StrForm far *pForm=(LPVOID)TerFont[i].pInfo;
          if (pForm->FontId>=0 && pForm->FontId<TotalFonts) {
             RecordFontInUse(w,pForm->FontId);
             TerFont[pForm->FontId].InUse=TRUE;
          }
       }
    }

    //******************* realize fonts ***********************************
    for (i=0;i<TotalFonts;i++) {
       if (TerFont[i].InUse) {
          // fixup font table for old version
          if (FileInfo.RevId<1) TerFont[i].TextBkColor=0xFFFFFF;

          if (TerFont[i].style&PICT) {                    // create device bitmap
             if (TerFont[i].LinkFile) {
                int NewId,SaveHeight,SaveWidth;
                LPINT SaveCharWidth;
                BYTE LinkFile[300];

                // resolve link file name
                strcpy(LinkFile,TerFont[i].LinkFile);
                ResolveLinkFileName(w,LinkFile);
                if (strlen(LinkFile)!=strlen(TerFont[i].LinkFile)) TerFont[i].LinkFile=OurRealloc(TerFont[i].LinkFile,strlen(LinkFile)+1);
                strcpy(TerFont[i].LinkFile,LinkFile);

                if ((NewId=TerInsertPictureFile(hTerWnd,TerFont[i].LinkFile,FALSE,TerFont[i].PictAlign,FALSE))<=0) {
                   long line;
                   int  col;
                   ResetUintFlag(TerFont[i].style,PICT);
                   lstrcpy(TerFont[i].TypeFace,TerArg.FontTypeFace);
                   TerFont[i].TwipsSize=TerArg.PointSize*20;
                   if (TerLocateFontId(hTerWnd,i,&line,&col)) pText(line)[col]=' ';  // convert picture character to space
                   goto CREATE_FONT;      // link picture not found, treat it as a regular font
                }
                TerFont[NewId].style=TerFont[i].style; // use the stored height/width
                SaveHeight=TerFont[NewId].PictHeight=TerFont[i].PictHeight; // use the stored height/width
                SaveWidth=TerFont[NewId].PictWidth=TerFont[i].PictWidth;
                SaveCharWidth=TerFont[i].CharWidth;    // swap the char width table

                DeleteTerObject(w,i);     // delete the existing object
                FarMove(&(TerFont[NewId]),&(TerFont[i]),sizeof(struct StrFont));
                SetPictSize(w,i,TwipsToScrY(SaveHeight),TwipsToScrX(SaveWidth),TRUE);

                TerFont[NewId].CharWidth=SaveCharWidth;   // transfer the pointer
                InitTerObject(w,NewId);   // re-initialize the new slot
             }
             else if (TerFont[i].PictType==PICT_DIBITMAP) DIB2Bitmap(w,hTerDC,i);
             else if (TerFont[i].PictType==PICT_CONTROL)  continue;  // do it in the second pass
             else if (TerFont[i].PictType==PICT_FORM)     continue;  // do it in the second pass - pForm structure has font id
             else if (TerFont[i].PictType==PICT_METAFILE || TerFont[i].PictType==PICT_ENHMETAFILE) {
                 if (TerFont[i].PictType==PICT_METAFILE)
                      TerFont[i].hMeta=TerSetMetaFileBits(TerFont[i].pImage); // convert to metafile
                 else TerFont[i].hEnhMeta=TerSetEnhMetaFileBits(TerFont[i].pImage); // convert to metafile
                 TerFont[i].pImage=0;
                 SetPictSize(w,i,TwipsToScrY(TerFont[i].PictHeight),TwipsToScrX(TerFont[i].PictWidth),TRUE);
                 if (TerFont[i].bmHeight==0) TerFont[i].bmHeight=TerFont[i].height;       // actual height of the picture
                 if (TerFont[i].bmWidth==0) TerFont[i].bmWidth=TerFont[i].CharWidth[PICT_CHAR];
             }
             if (FileInfo.RevId<51) {
                 TerFont[i].OrigPictWidth=TerFont[i].PictWidth;
                 TerFont[i].OrigPictHeight=TerFont[i].PictHeight;
             } 

             XlateSizeForPrt(w,i);                       // translate picture dimension for the subfont table

             //  load the OLE object
             if (TerFlags&TFLAG_NO_OLE) TerFont[i].ObjectType=OBJ_NONE;
             if (TerFont[i].ObjectType!=OBJ_NONE) TerOleLoad(w,i);
          }
          // create fonts for the non-pict type entries
          CREATE_FONT:
          if (!(TerFont[i].style&PICT)) {
             if (!CreateOneFont(w,hTerDC,i,TRUE)) {                 // create a new font
                wsprintf(temp,"Font: %s, TwipsSize: %d",TerFont[i].TypeFace,TerFont[i].TwipsSize);
                return PrintError(w,MSG_ERR_FONT_CREATE,temp);
             }
          }
       }
       else {
          if (TerFont[i].pInfo)   OurFree(TerFont[i].pInfo);
          if (TerFont[i].pImage)  OurFree(TerFont[i].pImage);
          if (TerFont[i].hObject) GlobalFree(TerFont[i].hObject);
          if (TerFont[i].LinkFile) OurFree(TerFont[i].LinkFile);
          InitTerObject(w,i);
       }
    }

    // second pass for font realization
    for (i=0;i<TotalFonts;i++) {
       if (!TerFont[i].InUse || !(TerFont[i].style&PICT)) continue;

       if (TerFont[i].PictType==PICT_CONTROL)       RealizeControl(w,i);
       else if (TerFont[i].PictType==PICT_FORM && TerFont[i].FieldId!=FIELD_TEXTBOX) RealizeControl(w,i);
    }

    //*********** re-establish the current font **************
    hTerRegFont=hTerCurFont=TerFont[0].hFont;
    SelectObject(hTerDC,hTerRegFont);
    GetTextMetrics(hTerDC,&TerTextMet);       // get new text metrics
    GetWinDimension(w);                       // recalulate our window dimension
    if (TerArg.ShowStatus) DisplayStatus(w);  // paint the status line

    //**************** realize para table **********************************
    for (i=0;i<TotalPfmts;i++) {
       PfmtId[i].LeftIndent=TwipsToScrX(PfmtId[i].LeftIndentTwips);
       PfmtId[i].RightIndent=TwipsToScrX(PfmtId[i].RightIndentTwips);
       PfmtId[i].FirstIndent=TwipsToScrX(PfmtId[i].FirstIndentTwips);
    }

    MemFree(pFontInUse);

    return TRUE;
}

/******************************************************************************
   SaveCfmt:
   1. save the character formatting information to output file (argument #1).
      The character formatting information will be written at the position
      specified by the second argument.
   2. Write the font table after the character formatting information.
*******************************************************************************/
BOOL SaveCfmt(PTERWND w,LPBYTE OutFile,BYTE huge *buf,long CfmtPos,BYTE InputType)
{
    int  i,len;
    HANDLE hFile= INVALID_HANDLE_VALUE;
    WORD fonts;
    int  FmtLen;
    LPWORD fmt;
    LPBYTE pImage,pInfo;
    struct StrFontIO FontRec;
    struct StrParaIO ParaRec;
    long l,line,ZeroCount;
    WORD CurFmt;
    BOOL AnimUsed,LinkedGif;

    if (CfmtPos==0) return TRUE;
    if (!CfmtNeeded(w)) return TRUE;

    if (InputType=='F') {
       if (INVALID_HANDLE_VALUE==(hFile=CreateFile(OutFile,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))) 
           return PrintError(w,MSG_ERR_FILE_OPEN,"SaveCfmt");
       SetFilePointer(hFile,CfmtPos,NULL,FILE_BEGIN);
    }

    for (line=0;line<TotalLines;line++) {
       CompressCfmt(w,line); // compress the formatting information

       if (cfmt(line).info.type==UNIFORM) FmtLen=1;
       else                               FmtLen=LineLen(line);

       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FmtLen),(LPBYTE)&FmtLen)) goto WRITE_ERROR;
       if (cfmt(line).info.type==UNIFORM) {
          CurFmt=cfmt(line).info.fmt;
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(CurFmt),(BYTE huge *)&CurFmt)) goto WRITE_ERROR;
       }
       else if (FmtLen>0) {
          fmt=cfmt(line).pFmt;
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,FmtLen*sizeof(WORD),(BYTE huge *)fmt)) goto WRITE_ERROR;
       }
    }

    // write the tag ids
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    ZeroCount=0;
    for (line=0;line<=TotalLines;line++) {
       if (line<TotalLines && (!pCtid(line) || LineLen(line)==0)) {
          ZeroCount++;
          continue;
       }

       if (ZeroCount>0) {             // write any zero count
          FmtLen=0;
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FmtLen),(LPBYTE)&FmtLen)) goto WRITE_ERROR;
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(ZeroCount),(LPBYTE)&ZeroCount)) goto WRITE_ERROR;
          ZeroCount=0;
       }
       if (line==TotalLines) break;

       FmtLen=LineLen(line);
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FmtLen),(LPBYTE)&FmtLen)) goto WRITE_ERROR;
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,FmtLen*sizeof(WORD),(BYTE huge *)pCtid(line))) goto WRITE_ERROR;
    }

    // write the lead bytes
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    ZeroCount=0;
    for (line=0;line<=TotalLines;line++) {
       if (line<TotalLines && (!pLead(line) || LineLen(line)==0)) {
          ZeroCount++;
          continue;
       }

       if (ZeroCount>0) {             // write any zero count
          FmtLen=0;
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FmtLen),(LPBYTE)&FmtLen)) goto WRITE_ERROR;
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(ZeroCount),(LPBYTE)&ZeroCount)) goto WRITE_ERROR;
          ZeroCount=0;
       }
       if (line==TotalLines) break;

       FmtLen=LineLen(line);
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FmtLen),(LPBYTE)&FmtLen)) goto WRITE_ERROR;
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,FmtLen*sizeof(BYTE),(BYTE huge *)pLead(line))) goto WRITE_ERROR;
    }


    // write the font table
    fonts=(WORD)TotalFonts;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;

    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(fonts),(BYTE huge *)&fonts)) goto WRITE_ERROR;

    for (i=0;i<TotalFonts;i++) {             // write each font record
       if (!(TerFont[i].InUse)) InitTerObject(w,i);

       // write the font record
       FarMemSet(&FontRec,0,sizeof(FontRec));   // initialize the FontRec io structure
       FontRec.style=TerFont[i].style;          // build a FontRec io record
       FontRec.TextColor=TerFont[i].TextColor;  // text color
       FontRec.TextBkColor=TerFont[i].TextBkColor;  // text background color
       FontRec.UlineColor=TerFont[i].UlineColor;  // text background color
       FontRec.TwipsSize=TerFont[i].TwipsSize;
       lstrcpy(FontRec.TypeFace,TerFont[i].TypeFace);
       FontRec.FontFamily=TerFont[i].FontFamily;
       FontRec.FieldId=TerFont[i].FieldId;
       FontRec.AuxId=TerFont[i].AuxId;
       FontRec.CharId=TerFont[i].CharId;
       FontRec.CharStyId=TerFont[i].CharStyId;
       FontRec.ParaStyId=TerFont[i].ParaStyId;
       FontRec.expand=TerFont[i].expand;
       FontRec.PictType=TerFont[i].PictType;    // picture type when font represents a picture
       FontRec.PictAlign=TerFont[i].PictAlign;  // picture type when font represents a picture
       FontRec.FrameType=TerFont[i].FrameType;// horizontal alignment
       FontRec.ParaFID=TerFont[i].ParaFID;      // picture type when font represents a picture
       FontRec.bmWidth=TerFont[i].bmWidth;      // actual width of the picture
       FontRec.bmHeight=TerFont[i].bmHeight;
       FontRec.lang=TerFont[i].lang;
       FontRec.offset=TerFont[i].offset;
       FontRec.ObjectAspect=TerFont[i].ObjectAspect;
       FontRec.ObjectUpdate=TerFont[i].ObjectUpdate;
       FontRec.UcBase=TerFont[i].UcBase;
       FontRec.CharSet=TerFont[i].CharSet;
       FontRec.OrigPictType=TerFont[i].OrigPictType;
       FontRec.OrigImageSize=TerFont[i].OrigImageSize;
       FontRec.TextAngle=TerFont[i].TextAngle;
       FontRec.InsRev=TerFont[i].InsRev;
       FontRec.InsTime=TerFont[i].InsTime;
       FontRec.DelRev=TerFont[i].DelRev;
       FontRec.DelTime=TerFont[i].DelTime;

       if (TerFont[i].LinkFile && TerFont[i].InUse
         && !(TerFlags&TFLAG_IGNORE_PICT_LINK)) FontRec.LinkNameLen=lstrlen(TerFont[i].LinkFile)+1;   // add the space for the NULL character
       LinkedGif=FontRec.LinkNameLen>1 && strstr(TerFont[i].LinkFile,".GIF");

       FontRec.ObjectType=TerFont[i].ObjectType;

       if (FontRec.style&PICT && FontRec.ObjectType==OBJ_OCX_EXT) {   // do not save external ocx object
          ResetUintFlag(FontRec.style,PICT);
          FontRec.ObjectType=OBJ_NONE;
       }

       if (FontRec.ObjectType==OBJ_EMBED_TEMP || TerFlags&TFLAG_NO_OLE)
          FontRec.ObjectType=OBJ_NONE;
       if (FontRec.ObjectType!=OBJ_NONE) {
          if (!TerOleQuerySize(w,i,&(FontRec.ObjectSize)) || FontRec.ObjectSize==0) {
              PrintError(w,MSG_ERR_OBJ_SAVE,"");
              goto WRITE_ERROR;
          }
       }

       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(FontRec),(LPBYTE)&FontRec)) goto WRITE_ERROR;

       // write any field code
       if (FontRec.FieldId>0) {   // RevId>=31
          int len=(TerFont[i].FieldCode?lstrlen(TerFont[i].FieldCode):0);
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(LPBYTE)&len)) goto WRITE_ERROR;  // write the length of the field code information
          if (len>0 && !TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,len,TerFont[i].FieldCode)) goto WRITE_ERROR;  // write the length of the field code information
       } 

       // write the picture information
       if (FontRec.style&PICT) {         // save pictures
          DWORD ImageSize=TerFont[i].ImageSize;
          DWORD InfoSize=TerFont[i].InfoSize;

           
          // adjust the picture size
          if (!TerFont[i].InUse) ImageSize=InfoSize=0;
          if (TerFont[i].anim && !IsFirstAnimPict(w,i)) {
             int FirstPict=TerFont[i].anim->FirstPict;
             if (TerFont[FirstPict].LinkFile && TerFont[FirstPict].InUse
               && strstr(TerFont[FirstPict].LinkFile,".GIF")
               && !(TerFlags&TFLAG_IGNORE_PICT_LINK)) {
                  LinkedGif=TRUE;
                  ImageSize=InfoSize=0;
               }
          }

          // write the link file name if any
          if (FontRec.LinkNameLen>1) {
             if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,FontRec.LinkNameLen,(BYTE huge *)(TerFont[i].LinkFile))) goto WRITE_ERROR;
             ImageSize=InfoSize=0;
          }

          //**** write the display dimensions of the picture ******
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(UINT),(BYTE huge *)&(TerFont[i].PictHeight))) goto WRITE_ERROR;
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(UINT),(BYTE huge *)&(TerFont[i].PictWidth))) goto WRITE_ERROR;

          //**** write the storage requirement ********************
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(DWORD),(BYTE huge *)&(ImageSize))) goto WRITE_ERROR;
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(DWORD),(BYTE huge *)&(InfoSize))) goto WRITE_ERROR;

          if (ImageSize>0) {        // write image data
             if (TerFont[i].PictType==PICT_METAFILE) TerFont[i].pImage=TerGetMetaFileBits(TerFont[i].hMeta,null);  // convert to global handle
             if (TerFont[i].PictType==PICT_ENHMETAFILE) TerFont[i].pImage=TerGetEnhMetaFileBits(TerFont[i].hEnhMeta);  // convert to global handle

             pImage=(LPVOID)TerFont[i].pImage;
             
             if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,TerFont[i].ImageSize,pImage)) goto WRITE_ERROR;
             if (TerFont[i].PictType==PICT_METAFILE) {
                TerFont[i].hMeta=TerSetMetaFileBits(TerFont[i].pImage);  // convert to metafile handle
                TerFont[i].pImage=0;
             }
             else if (TerFont[i].PictType==PICT_ENHMETAFILE) {
                TerFont[i].hEnhMeta=TerSetEnhMetaFileBits(TerFont[i].pImage);  // convert to metafile handle
                TerFont[i].pImage=0;
             }
          }
          if (InfoSize>0) {         // write image information
             struct StrForm form;
             pInfo=(LPVOID)TerFont[i].pInfo;
             
             if (TerFont[i].PictType==PICT_FORM) {      // get the curent data
                HWND hWnd;
                FarMove(pInfo,&form,sizeof(form));
                hWnd=GetDlgItem(hTerWnd,(int)form.id);

                if (hWnd) {
                  if (TerFont[i].FieldId==FIELD_TEXTBOX) {
                     GetWindowText(hWnd,form.InitText,MAX_WIDTH);
                     form.InitText[MAX_WIDTH]=0;
                  }
                  else if (TerFont[i].FieldId==FIELD_CHECKBOX) {
                    form.InitNum=((BOOL)SendMessage(hWnd,BM_GETCHECK,0,0L))?1:0;
                  }
                }     
                pInfo=(LPVOID)&form;
             } 
             
             if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,TerFont[i].InfoSize,pInfo)) goto WRITE_ERROR;
          }
          if (FontRec.ObjectType!=OBJ_NONE) {  // write the object
              if (!GetOleStorageData(w,i)) {
                   PrintError(w,MSG_ERR_OBJ_DATA,"");
                   goto WRITE_ERROR;
              }
              if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,FontRec.ObjectSize,pOleData)) goto WRITE_ERROR;
              GlobalUnlock(hOleData);
              GlobalFree(hOleData);            // buffer no longer needed
          }

          // write any animation information
          AnimUsed=(BOOL)(DWORD)(TerFont[i].anim) && !(LinkedGif);
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(AnimUsed),(LPBYTE)&AnimUsed)) goto WRITE_ERROR;
          if (AnimUsed && !TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrAnim),(LPBYTE)TerFont[i].anim)) goto WRITE_ERROR;

          // write any original picture data
          if (TerFont[i].OrigImageSize>0) {
             if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,TerFont[i].OrigImageSize,TerFont[i].pOrigImage)) goto WRITE_ERROR;
          } 
       }
    }

    // write paragraph and cell id information
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;

    for (l=0;l<TotalLines;l++) {
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(pfmt(l)))) goto WRITE_ERROR;
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(cid(l)))) goto WRITE_ERROR;
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&(fid(l)))) goto WRITE_ERROR;
    }

    // write paragraph table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalPfmts)) goto WRITE_ERROR;

    FarMemSet(&ParaRec,0,sizeof(ParaRec));

    for (i=0;i<TotalPfmts;i++) {
        ParaRec.LeftIndent=PfmtId[i].LeftIndentTwips;
        ParaRec.RightIndent=PfmtId[i].RightIndentTwips;
        ParaRec.FirstIndent=PfmtId[i].FirstIndentTwips;
        ParaRec.TabId=PfmtId[i].TabId;
        ParaRec.flags=PfmtId[i].flags;
        ParaRec.BltId=PfmtId[i].BltId;
        ParaRec.AuxId=PfmtId[i].AuxId;
        ParaRec.StyId=PfmtId[i].StyId;
        ParaRec.shading=PfmtId[i].shading;
        ParaRec.pflags=PfmtId[i].pflags;
        ParaRec.SpaceBefore=PfmtId[i].SpaceBefore;
        ParaRec.SpaceAfter=PfmtId[i].SpaceAfter;
        ParaRec.SpaceBetween=PfmtId[i].SpaceBetween;
        ParaRec.LineSpacing=PfmtId[i].LineSpacing;
        ParaRec.BkColor=PfmtId[i].BkColor;
        ParaRec.BorderSpace=PfmtId[i].BorderSpace;
        ParaRec.flow=PfmtId[i].flow;
        ParaRec.BorderColor=PfmtId[i].BorderColor;
        if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(ParaRec),(LPBYTE)&ParaRec)) goto WRITE_ERROR;
    }

    // write the tab table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalTabs)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrTab)*TotalTabs,(LPBYTE)TerTab)) goto WRITE_ERROR;

    // write the sect table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalSects)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrSect)*TotalSects,(LPBYTE)TerSect)) goto WRITE_ERROR;

    // write the table array
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalTableRows)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrTableRow)*TotalTableRows,(BYTE huge *)TableRow)) goto WRITE_ERROR;

    // write the cell table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalCells)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,(long)sizeof(struct StrCell)*TotalCells,(BYTE huge *)cell)) goto WRITE_ERROR;

    // write the paragraph frame table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalParaFrames)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrParaFrame)*TotalParaFrames,(LPBYTE)ParaFrame)) goto WRITE_ERROR;

    // write stylesheet table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalSID)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrStyleId)*TotalSID,(LPBYTE)StyleId)) goto WRITE_ERROR;

    // write bullet table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalBlts)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrBlt)*TotalBlts,(LPBYTE)TerBlt)) goto WRITE_ERROR;

    // write the tag table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalCharTags)) goto WRITE_ERROR;
    for (i=0;i<TotalCharTags;i++) {
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrCharTag),(LPBYTE)&(CharTag[i]))) goto WRITE_ERROR;

       len=CharTag[i].name?lstrlen(CharTag[i].name):0;  // write the name
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(len),(BYTE huge *)&len)) goto WRITE_ERROR;
       if (len && !TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,len,(BYTE huge *)CharTag[i].name)) goto WRITE_ERROR;

       len=0;  // write the length of the miscellaneous string not in use currently
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(len),(BYTE huge *)&len)) goto WRITE_ERROR;

       len=CharTag[i].AuxText?lstrlen(CharTag[i].AuxText):0;  // write the AuxText
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(len),(BYTE huge *)&len)) goto WRITE_ERROR;
       if (len && !TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,len,(BYTE huge *)CharTag[i].AuxText)) goto WRITE_ERROR;
    }

    // write rtf information block
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    for (i=0;i<INFO_MAX;i++) if (!TerWriteRtfInfo(w,i,TRUE,InputType,hFile,buf,&CfmtPos)) goto WRITE_ERROR;

    // write list
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalLists)) goto WRITE_ERROR;
    
    for (i=0;i<TotalLists;i++) {
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrList),(LPBYTE)&(list[i]))) goto WRITE_ERROR;
    
       if (list[i].InUse) {
          // write the list name
          len=list[i].name?lstrlen(list[i].name):0;  // write the name
          if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(len),(BYTE huge *)&len)) goto WRITE_ERROR;
          if (len && !TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,len,(BYTE huge *)list[i].name)) goto WRITE_ERROR;

          // write list levels
          if (list[i].LevelCount && !TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrListLevel)*list[i].LevelCount,(LPBYTE)list[i].level)) goto WRITE_ERROR;
       }
    }

    // write list override table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalListOr)) goto WRITE_ERROR;
    
    for (i=0;i<TotalListOr;i++) {
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrListOr),(LPBYTE)&(ListOr[i]))) goto WRITE_ERROR;
    
       // write ListOr levels
       if (ListOr[i].InUse) {
          if (ListOr[i].LevelCount && !TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrListLevel)*ListOr[i].LevelCount,(LPBYTE)ListOr[i].level)) goto WRITE_ERROR;
       }
    }
    
    // write list override table
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&FmtSign)) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&TotalReviewers)) goto WRITE_ERROR;
    
    for (i=0;i<TotalReviewers;i++) {
       if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrReviewer),(LPBYTE)&(reviewer[i]))) goto WRITE_ERROR;
    }
    
    // write the file information block
    FarMemSet(&FileInfo,0,sizeof(FileInfo));
    FileInfo.sign=SIGN_EOF;
    FileInfo.VerId=40;         // this value never changes since it is also used as a signature in GetFileInfo function
    //FileInfo.RevId=37;       // use TERTEAG_SECT to store section ids
    //FileInfo.RevId=38;       // added BorderSpace to ParaIO
    //FileInfo.RevId=39;       // BasedOn in StrStyle renamed to LineSpacing and added LineSpacing to ParaIO
    //FileInfo.RevId=40;       // TerSect: replaced LastPageHeight by PageNumFmt, TerSect1: added LastPageHeight
    //FileInfo.RevId=41;       // Add UcBase to FontRec
    //FileInfo.RevId=42;       // replace ULINEF by ENOTETEXT
    //FileInfo.RevId=43;       // Added BorderColor[4] to the StrCell structure
    //FileInfo.RevId=44;       // Added CharSet to StrFontIO
    //FileInfo.RevId=45;       // added original picture information, StrFileIO structure increased by 37 bytes
    //FileInfo.RevId=46;       // expanded StrSect by 62 bytes to add page border parameters
    //FileInfo.RevId=47;       // modified StrSect to support multisize paper in one document
    //FileInfo.RevId=48;       // added TextAngle support
    //FileInfo.RevId=49;       // added ObjectUpdate variable to the font structure
    //FileInfo.RevId=50;       // rtl support variables for paragraph, section and cell tables
    //FileInfo.RevId=51;       // picture cropping support variables
    //FileInfo.RevId=52;       // added AuxId in the StrSect structure
    //FileInfo.RevId=53;       // Word style Text Input fields
    //FileInfo.RevId=54;       // Added shape and shpgrp variables to StrParaFrame
    //FileInfo.RevId=55;       // Added underline color
    //FileInfo.RevId=56;       // Added OutlineLevel to StrStyleId
    //FileInfo.RevId=57;       // Expanded StrCharTag by 20 bytes
    //FileInfo.RevId=58;       // recognize the effect of expanded MAX_WIDTH of StrForm
    //FileInfo.RevId=59;       // BorderColor added to StrPfmt
    //FileInfo.RevId=60;       // Expanded StrParaFrame by 40 bytes
    //FileInfo.RevId=61;       // Added CharId to StrFont
    //FileInfo.RevId=62;       // Added Track-changes structure and FontIO variables
    FileInfo.RevId=63;         // Expand StrReviewer structure by 24 bytes
    FileInfo.PpHeight=(int)InchesToTwips(PageHeight);
    FileInfo.PpWidth=(int)InchesToTwips(PageWidth);
    FileInfo.PpSize=PaperSize;
    FileInfo.PpOrient=PaperOrient;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(BYTE),&(FileInfo.sign))) goto WRITE_ERROR;
    if (!TerFarWrite(w,InputType,hFile,buf,&CfmtPos,TerArg.BufferLen,sizeof(struct StrFileInfo),(LPBYTE)&FileInfo)) goto WRITE_ERROR;

    // Successful
    if (InputType=='F') CloseHandle(hFile);
    if (InputType=='B') TerArg.BufferLen=CfmtPos;  // set the actual length

    return TRUE;

    WRITE_ERROR:
    if (InputType=='F') CloseHandle(hFile);
    PrintError(w,MSG_SAVE_INCOMPL,NULL);
    return FALSE;
}

/******************************************************************************
    DoFixStrForm:
    Fix the StrForm structure for expanded MAX_WIDTH.
******************************************************************************/
DoFixStrForm(PTERWND w,LPVOID ptr)
{
    struct StrFormOld OldForm;
    struct StrForm *pForm;

    FarMove(ptr,&OldForm,sizeof(struct StrFormOld));

    pForm=(struct StrForm*) ptr;

    strcpy(pForm->class,OldForm.class);
    pForm->style=OldForm.style;                        // control style bits
    pForm->id=OldForm.id;                              // id of the control (use long for 16/32 bit compatiblity)

    pForm->TextType=OldForm.TextType;                  // text field type
    pForm->CheckBoxType=OldForm.CheckBoxType;          // check box size, 0=auto, 1=exact
    pForm->protected=OldForm.protected;                // 1 = protected, 0=not protected
    pForm->MaxLen=OldForm.MaxLen;                      // max length of the text field
    pForm->CheckBoxSize=OldForm.CheckBoxSize;          // checkbox size in twips
    strcpy(pForm->name,OldForm.name);                  // field name
    strcpy(pForm->InitText,OldForm.InitText);          // initialize text data
    pForm->InitNum=OldForm.InitNum;                    // initial numeric data
    strcpy(pForm->typeface,OldForm.typeface);          // font typeface
    strcpy(pForm->family,OldForm.family);              // font family
    pForm->TwipsSize=OldForm.TwipsSize;                // font twips size
    pForm->FontStyle=OldForm.FontStyle;                // font style
    pForm->CharSet=OldForm.CharSet;                    // font character set
    pForm->TextColor=OldForm.TextColor;                // text color
    pForm->TextBkColor=OldForm.TextBkColor;            // text background color
    pForm->FontId=OldForm.FontId;                      // current font id
    pForm->OrigWndProc=OldForm.OrigWndProc;            // original window process
    pForm->flags=OldForm.flags;                        // FMFLAG_ constant

    return true;
}

/******************************************************************************
    RecordFontInUse:
    Record that a font is used in the file during sse file input
******************************************************************************/
RecordFontInUse(PTERWND w,int FontId)
{
   int i,PrevFontInUseSize=FontInUseSize;

   if (FontId>=FontInUseSize) {
      FontInUseSize=FontId+50;
      pFontInUse=MemReAlloc(pFontInUse,FontInUseSize);
      for (i=PrevFontInUseSize;i<FontInUseSize;i++) pFontInUse[i]=FALSE;
   } 
   pFontInUse[FontId]=TRUE;

   return TRUE;
}

/******************************************************************************
    TerReadRtfInfo:
    read an rtf information block.
******************************************************************************/
TerReadRtfInfo(PTERWND w,BYTE InputType,HANDLE hFile,BYTE huge *buf,long far *CfmtPos)
{
    int idx,len;

    if (!TerFarRead(w,InputType,hFile,buf,CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&idx)) return FALSE;
    if (idx<0 || idx>=INFO_MAX) return FALSE;

    if (!TerFarRead(w,InputType,hFile,buf,CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&len)) return FALSE;

    // read the information string
    if (len>0) {
       pRtfInfo[idx]=MemAlloc(len+1);   // include the space for the NULL terminator
       if (!TerFarRead(w,InputType,hFile,buf,CfmtPos,TerArg.BufferLen,len+1,(BYTE huge *)pRtfInfo[idx])) return FALSE;
    } 

    return TRUE;
}
 

/******************************************************************************
    TerWriteRtfInfo:
    Write rtf information block.  This function returns the size of information block.
******************************************************************************/
TerWriteRtfInfo(PTERWND w,int idx, BOOL write, BYTE OutType,HANDLE hFile,BYTE huge *buf,long far *CfmtPos)
{
    int len=0,BytesWritten=0;
    LPBYTE ptr=NULL;

    if (idx<INFO_MAX && pRtfInfo[idx]) {
      ptr=pRtfInfo[idx];
      len=lstrlen(ptr);
    }
 
    // write the index and the length
    if (write && !TerFarWrite(w,OutType,hFile,buf,CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&idx)) return 0;
    BytesWritten+=sizeof(int);

    if (write && !TerFarWrite(w,OutType,hFile,buf,CfmtPos,TerArg.BufferLen,sizeof(int),(BYTE huge *)&len)) return 0;
    BytesWritten+=sizeof(int);
    
    // write the string with the NULL terminator.
    if (len>0) {
       if (write && !TerFarWrite(w,OutType,hFile,buf,CfmtPos,TerArg.BufferLen,len+1,ptr)) return 0;  // write null terminator also
       BytesWritten+=(len+1);
    }

    return BytesWritten;
}
 
/******************************************************************************
    SetTerCharStyle:
    This is a driver routine to toggle on/off the character formatting attributes
******************************************************************************/
BOOL WINAPI _export SetTerCharStyle(HWND hWnd,UINT FmtType,BOOL OnOff,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (OnOff && FmtType&HLINK) LinkStyle|=HLINK; // set HLINK for linking

    if (CurSID>=0) {       // apply the style sheed
       FmtType=ResetUintFlag(FmtType,CHAR_BOX);  // char box style not allowed
       if (OnOff) StyleId[CurSID].style|=FmtType;
       else       StyleId[CurSID].style=ResetUintFlag(StyleId[CurSID].style,FmtType);
       return TRUE;
    }
    else return CharFmt(w,GetNewStyle,FmtType,OnOff,repaint);
}

/******************************************************************************
    GetNewStyle:
    This routine accepts the current format index for a character and
    the requested style change. It returns the format index for the
    character.
******************************************************************************/
WORD GetNewStyle(PTERWND w,WORD OldFmt,DWORD data1,DWORD data2, long line,int col)
{
    int  CurFont,NewFont;
    UINT style,FmtType;
    BOOL OnOff;

    FmtType=(UINT)data1;
    FmtType=(FmtType)&(~((UINT)PICT));            // don't change the picture style
    OnOff=(BOOL)data2;

    CurFont=OldFmt;                       // current font number
    style=TerFont[CurFont].style;         // current style

    if (OnOff)  style=(style)|(FmtType);  // toggle on
    else        style=(style)&(~FmtType); // toggle off

    if (OnOff && FmtType&SUPSCR) style=(style)&(~SUBSCR);  // super and subscript and mutually exclusive
    if (OnOff && FmtType&SUBSCR) style=(style)&(~SUPSCR);

    // check for protect style in a field name
    if (TerFont[OldFmt].FieldId==FIELD_NAME && col>=0) {
       BYTE CurChar=GetCurChar(w,line,col);
       if (CurChar=='{' || CurChar=='}') { 
          if (TerFont[CurFont].style&PROTECT) style|=PROTECT;  // don't disturbe the PROTECT attrbute
          if (!(TerFont[CurFont].style&PROTECT)) ResetUintFlag(style,PROTECT);  // don't disturhe the PROTECT attribute
       }
    }    

    if (style&PICT) {
       TerFont[CurFont].style=style;         // current style
       if (FmtType&HIDDEN) {
          SetPictSize(w,CurFont,TwipsToScrY(TerFont[CurFont].PictHeight),TwipsToScrX(TerFont[CurFont].PictWidth),TRUE);
          XlateSizeForPrt(w,CurFont);
       }
       return OldFmt;                        // do not change the picutre format
    }

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,style,
                            TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                            TerFont[CurFont].FieldId,TerFont[CurFont].AuxId,TerFont[CurFont].Aux1Id,
                            TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                            TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                            TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                            TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                            TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                            TerFont[CurFont].TextAngle))>=0) {
        return (WORD) NewFont;           // return the new font
    }
    else return OldFmt;
}

/******************************************************************************
    TerGetFontAux1Id:
    Get the value of the aux1 id for a given font id
******************************************************************************/
int WINAPI _export TerGetFontAux1Id(HWND hWnd,int CurFont)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurFont<0 || CurFont>=TotalFonts || !(TerFont[CurFont].InUse)) return FALSE;

    return TerFont[CurFont].Aux1Id;
}

/******************************************************************************
    TerSetNextFontAux1Id:
    Set the next aux1 id to be used by the TerCreateFont function
******************************************************************************/
BOOL WINAPI _export TerSetNextFontAux1Id(HWND hWnd,int Aux1Id)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    NextFontAux1Id=Aux1Id;
    return TRUE;
}

/******************************************************************************
    TerSetCharAuxId:
    This routine sets new character aux id.
******************************************************************************/
BOOL WINAPI _export TerSetCharAuxId(HWND hWnd,int AuxId,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurSID>=0) return FALSE;

    else return CharFmt(w,SetCharAuxId,AuxId,0,repaint);
}

/******************************************************************************
    SetCharAuxId:
    This routine accepts the aux id in data1 and creates a new font
    with the given aux id.
******************************************************************************/
WORD SetCharAuxId(PTERWND w,WORD OldFmt,DWORD data1,DWORD data2, long line,int col)
{
    int  CurFont,NewFont;
    int  AuxId;

    AuxId=(int)data1;

    CurFont=OldFmt;                       // current font number

    if (True(TerFont[CurFont].style&PICT)) {
       TerFont[CurFont].AuxId=AuxId;
       return CurFont;
    } 

    if ((NewFont=GetNewFont(w,hTerDC,CurFont,TerFont[CurFont].TypeFace,TerFont[CurFont].TwipsSize,TerFont[CurFont].style,
                            TerFont[CurFont].TextColor,TerFont[CurFont].TextBkColor,TerFont[CurFont].UlineColor,
                            TerFont[CurFont].FieldId,AuxId,TerFont[CurFont].Aux1Id,
                            TerFont[CurFont].CharStyId,TerFont[CurFont].ParaStyId,
                            TerFont[CurFont].expand,TerFont[CurFont].TempStyle,
                            TerFont[CurFont].lang,TerFont[CurFont].FieldCode,
                            TerFont[CurFont].offset,TerFont[CurFont].UcBase,
                            TerFont[CurFont].CharSet,TerFont[CurFont].flags,
                            TerFont[CurFont].TextAngle))>=0) {
        return (WORD) NewFont;           // return the new font
    }
    else return OldFmt;
}

/******************************************************************************
    GetNewFont:
    This routine examines the font table to check if a font exists that
    matches the new specification.  If it does,  the index of that font
    is returned.  Otherwise,  the routine will create a new font in an
    open slot in the font table.

    If successful, the routine returns the index of the font in the table.
    Otherwise it returns (-1).
******************************************************************************/
GetNewFont(PTERWND w,HDC hDC, int OldFont,LPBYTE NewTypeFace,int NewTwipsSize,UINT NewStyle,
           COLORREF NewTextColor, COLORREF NewTextBkColor, COLORREF NewUlineColor, int NewFieldId, int NewAuxId, 
           int NewAux1Id, int NewCharStyId, int NewParaStyId, int NewExpand, UINT NewTempStyle, 
           int NewLang, LPBYTE NewFieldCode, int NewOffset, int NewUcBase, BYTE NewCharSet, UINT NewFlags, int NewTextAngle)
{
    struct StrFont font;

    FarMemSet(&font,0,sizeof(font));

    lstrcpy(font.TypeFace,NewTypeFace);
    font.TwipsSize=NewTwipsSize;
    font.TextColor=NewTextColor;
    font.TextBkColor=NewTextBkColor;
    font.UlineColor=NewUlineColor;
    font.style=NewStyle;
    font.FieldId=NewFieldId;
    font.AuxId=NewAuxId;
    font.Aux1Id=NewAux1Id;
    font.CharStyId=NewCharStyId;
    font.ParaStyId=NewParaStyId;
    font.expand=NewExpand;
    font.TempStyle=NewTempStyle;
    font.lang=NewLang;
    font.CharSet=NewCharSet;
    font.offset=NewOffset;
    font.TextAngle=NewTextAngle;
    font.UcBase=NewUcBase;

    ResetUintFlag(font.flags,FFLAG_MASK);
    font.flags|=NewFlags;
    
    if (NewFieldCode) {                 // copy the field code, if any
       font.FieldCode=OurAlloc(lstrlen(NewFieldCode)+1);
       lstrcpy(font.FieldCode,NewFieldCode);
    }

    return GetNewFont2(w,hDC,OldFont,&font);
}

GetNewFont2(PTERWND w,HDC hDC, int OldFont,struct StrFont *font)
{
    int i,NewFont;
    BYTE TypeFace[32];
    BOOL CharSetMatched;

    if (TerFont[OldFont].style&PICT) return OldFont;     // do not change the picture font

    font->flags&=FFLAG_MASK;                                // only these flags are used for comparison, other flags are not disturbed

    // check if the requested font is same as the old font
    if (MatchIds) {
       if (!(TerFont[OldFont].style&PICT)
         && lstrcmpi(TerFont[OldFont].TypeFace,font->TypeFace)==0
         && TerFont[OldFont].TwipsSize == font->TwipsSize
         && TerFont[OldFont].TextColor == font->TextColor
         && TerFont[OldFont].TextBkColor == font->TextBkColor
         && TerFont[OldFont].UlineColor == font->UlineColor
         && TerFont[OldFont].style == font->style
         && TerFont[OldFont].AuxId == font->AuxId
         && TerFont[OldFont].Aux1Id == font->Aux1Id
         && TerFont[OldFont].CharId == font->CharId
         && TerFont[OldFont].CharStyId == font->CharStyId
         && TerFont[OldFont].ParaStyId == font->ParaStyId
         && TerFont[OldFont].expand == font->expand
         && TerFont[OldFont].TempStyle == font->TempStyle
         && TerFont[OldFont].lang == font->lang
         
         && (TerFont[OldFont].CharSet == font->CharSet 
            || (font->CharSet==DEFAULT_CHARSET && (TerFont[OldFont].CharSet==0 || TerFont[OldFont].CharSet==SYMBOL_CHARSET)))
         
         && TerFont[OldFont].InsRev == font->InsRev
         && TerFont[OldFont].InsTime == font->InsTime
         && TerFont[OldFont].DelRev == font->DelRev
         && TerFont[OldFont].DelTime == font->DelTime

         && TerFont[OldFont].offset == font->offset
         && TerFont[OldFont].TextAngle == font->TextAngle
         && TerFont[OldFont].UcBase == (WORD)font->UcBase
         && (TerFont[OldFont].flags&FFLAG_MASK) == font->flags
         && IsSameFieldCode(w,TerFont[OldFont].FieldCode,font->FieldCode)
         && TerFont[OldFont].FieldId == font->FieldId) return OldFont;

       // check if a font already exists with the requested specifications
       for (i=0;i<TotalFonts;i++) {
         if (!TerFont[i].InUse || TerFont[i].style&PICT) continue;
           
           if (lstrcmp(TerFont[i].TypeFace,font->TypeFace)!=0) continue;
           if (TerFont[i].TwipsSize != font->TwipsSize) continue;
           if (TerFont[i].TextColor != font->TextColor) continue;
           if (TerFont[i].TextBkColor != font->TextBkColor) continue;
           if (TerFont[i].UlineColor != font->UlineColor) continue;
           if (TerFont[i].style != font->style) continue;
           if (TerFont[i].AuxId != font->AuxId) continue;
           if (TerFont[i].Aux1Id != font->Aux1Id) continue;
           if (TerFont[i].CharId != font->CharId) continue;
           if (TerFont[i].CharStyId != font->CharStyId) continue;
           if (TerFont[i].ParaStyId != font->ParaStyId) continue;
           if (TerFont[i].expand != font->expand) continue;
           if (TerFont[i].TempStyle != font->TempStyle) continue;
           if (TerFont[i].lang != font->lang) continue;
           
           CharSetMatched=(TerFont[i].CharSet == font->CharSet);
           if (!CharSetMatched && font->CharSet==DEFAULT_CHARSET && TerFont[i].CharSet==SYMBOL_CHARSET) CharSetMatched=TRUE;
           if (!CharSetMatched && font->CharSet==DEFAULT_CHARSET && TerFont[i].CharSet==ANSI_CHARSET) CharSetMatched=TRUE;     // ANSI_CHARSET=0, DEFAULT_CHARSET=1
           if (!CharSetMatched) continue;
           
           //if (TerFont[i].CharSet != font->CharSet && (font->CharSet!=DEFAULT_CHARSET || TerFont[i].CharSet!=ANSI_CHARSET)) continue;     // ANSI_CHARSET=0, DEFAULT_CHARSET=1
           
           if (TerFont[i].InsRev != font->InsRev) continue;
           if (TerFont[i].InsTime != font->InsTime) continue;
           if (TerFont[i].DelRev != font->DelRev) continue;
           if (TerFont[i].DelTime != font->DelTime) continue;

           if (TerFont[i].offset != font->offset) continue;
           if (TerFont[i].TextAngle != font->TextAngle) continue;
           if (TerFont[i].UcBase != (WORD)font->UcBase) continue;
           if ((TerFont[i].flags&FFLAG_MASK) != font->flags) continue;
           if (!IsSameFieldCode(w,TerFont[i].FieldCode,font->FieldCode)) continue;
           if (TerFont[i].FieldId != font->FieldId) continue;
           
           return i;      // matching font found
       }
    }
    else MatchIds=TRUE;         // reset this flag

    lstrcpy(TypeFace,font->TypeFace);  // move to the local variable since font->TypeFace may be pointing to TerFont structure which can be modified by FindOpenSlot

    if ((NewFont=FindOpenSlot(w))==-1) return OldFont;  // no more open slots available

    // create a font matching the new specification
    TerFont[NewFont].InUse=TRUE;

    //* store the requested specifications *
    lstrcpy(TerFont[NewFont].TypeFace,TypeFace);
    TerFont[NewFont].TwipsSize=font->TwipsSize;
    TerFont[NewFont].TextColor=font->TextColor;
    TerFont[NewFont].TextBkColor=font->TextBkColor;
    TerFont[NewFont].UlineColor=font->UlineColor;
    TerFont[NewFont].style=font->style;
    TerFont[NewFont].FieldId=font->FieldId;
    TerFont[NewFont].AuxId=font->AuxId;
    TerFont[NewFont].Aux1Id=font->Aux1Id;
    TerFont[NewFont].CharId=font->CharId;
    TerFont[NewFont].CharStyId=font->CharStyId;
    TerFont[NewFont].ParaStyId=font->ParaStyId;
    TerFont[NewFont].expand=font->expand;
    TerFont[NewFont].TempStyle=font->TempStyle;
    TerFont[NewFont].lang=font->lang;
    TerFont[NewFont].CharSet=font->CharSet;
    TerFont[NewFont].offset=font->offset;
    
    TerFont[NewFont].InsRev=font->InsRev;
    TerFont[NewFont].InsTime=font->InsTime;
    TerFont[NewFont].DelRev=font->DelRev;
    TerFont[NewFont].DelTime=font->DelTime;

    TerFont[NewFont].TextAngle=font->TextAngle;
    TerFont[NewFont].UcBase=font->UcBase;

    ResetUintFlag(TerFont[NewFont].flags,FFLAG_MASK);
    TerFont[NewFont].flags|=font->flags;
    
    if (font->FieldCode) {                 // copy the field code, if any
       TerFont[NewFont].FieldCode=OurAlloc(lstrlen(font->FieldCode)+1);
       lstrcpy(TerFont[NewFont].FieldCode,font->FieldCode);
    }

    if (!CreateOneFont(w,hDC,NewFont,TRUE)) PrintError(w,MSG_ERR_FONT_CREATE,NULL);     // create the new font

    SelectObject(hTerDC,hTerRegFont);

    return NewFont;
}

/******************************************************************************
    IsSameField:
    returns TRUE if two font ids refer to the same field.
******************************************************************************/
BOOL IsSameField(PTERWND w, int font1, int font2)
{
   if (TerFont[font1].FieldId==0 || TerFont[font2].FieldId==0) return FALSE;

   if (font1==font2) return TRUE;

   if (TerFont[font1].FieldId!=TerFont[font2].FieldId) return FALSE;

   return IsSameFieldCode(w,TerFont[font1].FieldCode,TerFont[font2].FieldCode);
}

/******************************************************************************
    IsSameFieldCode:
    returns TRUE if two field codes are the same.
******************************************************************************/
BOOL IsSameFieldCode(PTERWND w, LPBYTE code1, LPBYTE code2)
{
    if (code1==NULL && code2==NULL) return TRUE;
    return (code1 && code2 && lstrcmp(code1,code2)==0);
}    

/******************************************************************************
    FindOpenSlot:
    Find an empty slot in the TerFont arrays to create a new font/picture.
    If not slots are available,  the document is checked for duplicate
    and deleted fonts to free up a slot.
    If successful, the routine returns the index to the TerFont array,
    otherwise it returns -1
******************************************************************************/
FindOpenSlot(PTERWND w)
{
    int i,NewFont;
    BOOL OneSlotFreed;


    // use the font that is request to be reused
    if (NextFontId>=0) {
       NewFont=NextFontId;
       NextFontId=-1;

       DeleteTerObject(w,NewFont);  // delete the existing object
       InitTerObject(w,NewFont);
       return NewFont;
    }

    // do we need to release fonts
    if (ReclaimResources && !InPrinting && !InRtfRead) FreeFontResources(w,FALSE);

    // find an unused slot
    LOOK_FOR_UNUSED_SLOT:
    for (i=0;i<TotalFonts;i++) {
       if (!(TerFont[i].InUse)) {
         InitTerObject(w,i);
         return i;
       }
    }

    if (TotalFonts<MAX_FONTS || Win32) {     // create a new font in a new slot
       if (TotalFonts>=MaxFonts) {  // expand the font table
          int NewMaxFonts=MaxFonts+(MaxFonts/3)+1;
          if (NewMaxFonts>MAX_FONTS && !Win32) NewMaxFonts=MAX_FONTS;
          ExpandFontTable(w,NewMaxFonts);  // expand the font table
       }

       NewFont=TotalFonts;
       TotalFonts++;
       InitTerObject(w,NewFont);
       return NewFont;
    }

    // consolidate fonts in the current file to open up a slot
    if (!ReclaimResources) goto END;

    OneSlotFreed=FreeFontResources(w,TRUE);

    END:
    if (OneSlotFreed) goto LOOK_FOR_UNUSED_SLOT;
    else {
        PrintError(w,MSG_OUT_OF_FONT_SLOT,NULL);
        return -1;
    }
}


/******************************************************************************
    FreeFontResources:
    Free Font resources
******************************************************************************/
BOOL FreeFontResources(PTERWND w, BOOL always)
{
    BOOL FontUsed[MAX_FONTS];
    LPWORD fmt;
    int  count,i,j,font1,font2,SimilarFont[MAX_FONTS];
    BOOL OneSlotFreed=FALSE;
    long l;
    struct StrForm far *pForm;


    // check number of fonts in the font table
    if (!always) {
        count=0;
        for (i=0;i<TotalFonts;i++) if (TerFont[i].InUse && !(TerFont[i].style&PICT)) count++;
        if (count<=MAX_FONT_HANDLES) return FALSE;     // not enough fonts resources used yet to trigger font release
    }

    // examine the font table for similar fonts
    SimilarFont[0]=0;
    for (i=1;i<MaxFonts;i++) {
       SimilarFont[i]=i;
       if (TerFont[i].style&PICT) continue; // don't touch the pictures

       for(j=0;j<=i;j++) {       // compare this font with earlier fonts
        if(lstrcmp(TerFont[j].TypeFace,TerFont[i].TypeFace)==0
           && TerFont[j].TwipsSize == TerFont[i].TwipsSize
           && !(TerFont[j].style&PICT)
           && TerFont[j].style == TerFont[i].style
           && TerFont[j].TempStyle == TerFont[i].TempStyle
           && TerFont[j].TextColor == TerFont[i].TextColor
           && TerFont[j].TextBkColor == TerFont[i].TextBkColor
           && TerFont[j].AuxId == TerFont[i].AuxId
           && TerFont[j].Aux1Id == TerFont[i].Aux1Id
           && TerFont[j].flags == TerFont[i].flags
           && TerFont[j].CharSet == TerFont[i].CharSet
           && TerFont[j].CharStyId == TerFont[i].CharStyId
           && TerFont[j].ParaStyId == TerFont[i].ParaStyId
           && TerFont[j].expand == TerFont[i].expand
           && TerFont[j].FieldId == TerFont[i].FieldId) {
              SimilarFont[i]=j;
              break;
           }
       }
    }

    //* adjust the fonts in the document
    for (i=0;i<MaxFonts;i++) FontUsed[i]=FALSE;  // initialize 'font use' table


    // check if the font used by the form fields or user created fonts
    for (i=0;i<TotalFonts;i++) {
       if (!TerFont[i].InUse) continue;

       if (!(TerFont[i].style&PICT) && TerFont[i].flags&FFLAG_USER) {   // font created using TerCreateFont not subject to reclaiming
          FontUsed[i]=TRUE;
          SimilarFont[i]=i;
       }
       else if (TerFont[i].style&PICT && TerFont[i].PictType==PICT_FORM) {
          pForm=(LPVOID)TerFont[i].pInfo;
          font1=pForm->FontId;
          if (font1>=0 && font1<TotalFonts && TerFont[font1].InUse) {
             FontUsed[font1]=TRUE;
             SimilarFont[font1]=font1;               // don't translate this font
          }
       }
       else if (TerFont[i].style&PICT && TerFont[i].ObjectType==OBJ_OCX_EXT) {  // protect external ocx objects
          FontUsed[i]=TRUE;
          SimilarFont[i]=i;
       }
    }

    // check if the font used by list table
    for (i=1;i<TotalLists;i++) {
      if (!list[i].InUse) continue;
      for (j=0;j<list[i].LevelCount;j++) {
         int FontId=list[i].level[j].FontId;
         FontUsed[FontId]=TRUE;
         SimilarFont[FontId]=FontId;
      } 
    } 

    // check if the font used by listoverride table
    for (i=1;i<TotalListOr;i++) {
      if (!ListOr[i].InUse) continue;
      for (j=0;j<ListOr[i].LevelCount;j++) {
         int FontId=ListOr[i].level[j].FontId;
         FontUsed[FontId]=TRUE;
         SimilarFont[FontId]=FontId;
      } 
    } 



    // check if the font used by input font id
    if (InputFontId>=0 && InputFontId<TotalFonts && TerFont[InputFontId].InUse) {
       FontUsed[InputFontId]=TRUE;
       SimilarFont[InputFontId]=InputFontId;
    }

    // check if the fonts used in the file
    for (l=0;l<TotalLines;l++) {
        if (cfmt(l).info.type==UNIFORM) {
           font1=cfmt(l).info.fmt;
           font2=SimilarFont[font1];
           cfmt(l).info.fmt=(WORD)font2;
           FontUsed[font2]=TRUE;           // keep track of fonts actually used
        }
        else {
           fmt=cfmt(l).pFmt;
           for (j=0;j<LineLen(l);j++) {
              font1=fmt[j];
              font2=SimilarFont[font1];
              fmt[j]=(WORD)font2;
              FontUsed[font2]=TRUE;           // keep track of fonts actually used
           }
        }
    }

    //** mark the freed fonts as unused **
    OneSlotFreed=FALSE;
    for (i=1;i<MaxFonts;i++) {         // the default font (1) never freed
       if (TerFont[i].InUse && TerFont[i].style&PICT) continue;

       if (TerFont[i].InUse && SimilarFont[i]!=i) {DeleteTerObject(w,i);OneSlotFreed=TRUE;}            // delete font/picture
       if (TerFont[i].InUse && !FontUsed[i])      {DeleteTerObject(w,i);OneSlotFreed=TRUE;}
    }

    if (OneSlotFreed) TerShrinkFontTable();

    return OneSlotFreed;
}

/******************************************************************************
    ExpandFontTable:
    Expand the font table.
******************************************************************************/
ExpandFontTable(PTERWND w, int NewMaxFonts)
{
    int i,SaveMaxFonts=MaxFonts;

    // exapnd the TerFont structure
    if (!ExpandArray(w,sizeof(struct StrFont),(void far * (huge *))&TerFont,&MaxFonts,NewMaxFonts,MAX_FONTS)) return FALSE;
    MaxFonts=SaveMaxFonts;      // reset for proper allocation in ExpandArray
    if (!ExpandArray(w,sizeof(struct StrPrtFont),(void far *(huge *))&PrtFont,&MaxFonts,NewMaxFonts,MAX_FONTS)) return FALSE;

    // initialize the new fonts
    for (i=SaveMaxFonts;i<NewMaxFonts;i++) {
       TerFont[i].CharWidth=PrtFont[i].CharWidth=NULL;
       TerFont[i].InUse=FALSE;
    }

    return TRUE;
}

/******************************************************************************
    RecreateFonts:
    Recreate all fonts for a specified device context.
******************************************************************************/
RecreateFonts(PTERWND w,HDC hDC)
{
    int i;
    BYTE string[80];

    dm("RecreateFonts");

    if (hDC!=hTerDC) {         // deselect the current display font
       hTerCurFont=GetStockObject(SYSTEM_FONT);
       SelectObject(hTerDC,hTerCurFont);
    }

    if (mbcs) InitCharWidthCache(w);   // initialize the character width cache

    // realize fonts
    for (i=0;i<TotalFonts;i++) {
      if (TerFont[i].InUse && !(TerFont[i].style&PICT)) {
         if (!CreateOneFont(w,hDC,i,TRUE)) {
            wsprintf(string,"#: %d, Font: %s, Twips Size: %d",i,TerFont[i].TypeFace,TerFont[i].TwipsSize);
            PrintError(w,MSG_ERR_FONT_CREATE,string);
         }
      }
    }

    // adjust the picture sizes - do it after the all fonts are done because formfield picture use font ids
    for (i=0;i<TotalFonts;i++) {
      if (TerFont[i].InUse && TerFont[i].style&PICT) {
         if (!InPrinting && TerFont[i].PictType==PICT_FORM && TerFont[i].FieldId==FIELD_TEXTBOX) {
            SetTextInputFieldWnd(w,i,TerFont[i].PictWidth,FALSE);
         }
         if (hDC==hTerDC) {
            if (InPrinting && UseScrMetrics) 
                 SetPictSize(w,i,TwipsToUnitY(TerFont[i].PictHeight),TwipsToUnitX(TerFont[i].PictWidth),TRUE);
            else SetPictSize(w,i,TwipsToScrY(TerFont[i].PictHeight),TwipsToScrX(TerFont[i].PictWidth),TRUE);
            XlateSizeForPrt(w,i);
         }
         else {
            if (InPrinting && UseScrMetrics) 
                 SetPictSize(w,i,TwipsToUnitY(TerFont[i].PictHeight),TwipsToUnitX(TerFont[i].PictWidth),TRUE);
            else SetPictSize(w,i,TwipsToPrtY(TerFont[i].PictHeight),TwipsToPrtX(TerFont[i].PictWidth),TRUE);
         }
      }
    }

    FontsReleased=FALSE;                 // fonts recreated
    if (hDC==hTerDC) hTerRegFont=TerFont[0].hFont;

    return TRUE;
}

/******************************************************************************
    CreateOneFont:
    This routine creates a font whose table index is given by the argument.
    This  table must already have specified point size, style and typeface
    of the font.
    This routine selects the new font into the display context.
    The new font is created for the specified display context.
******************************************************************************/
#pragma optimize("g",off)  // turn-off global optimization
CreateOneFont(PTERWND w,HDC hDC,int NewFont, BOOL ScreenFont)
{
    int i,ResY,OrigHeight,CharHeight,BaseHeight;
    UINT  NewStyle;
    BYTE  PitchFamily,CharSet,TypefaceCharSet;
    TEXTMETRIC met;
    HFONT hOldFont;
    LOGFONT lFont;
    int     GlbFontId,offset;
    BOOL   IsTrueType,IsScrDC;
    DWORD  LangInfo;
    BYTE   TypeFace[33];
    
    if (TerFont[NewFont].style&PICT) return TRUE;   // not a true font

    IsScrDC=(GetDeviceCaps(hDC,TECHNOLOGY)==DT_RASDISPLAY);

    // Allocate the character width table if not already allocated
    if (TerFont[NewFont].CharWidth==NULL) {
       TerFont[NewFont].CharWidth=OurAlloc(sizeof(int)*NUM_WIDTH_CHARS);
       PrtFont[NewFont].CharWidth=OurAlloc(sizeof(int)*NUM_WIDTH_CHARS);
    }

    // delete any old font
    if (TerFont[NewFont].hFont) {               // delete old font
       hTerCurFont=GetStockObject(SYSTEM_FONT); // deselect first
       SelectObject(hTerDC,hTerCurFont);
       SelectObject(hDC,hTerCurFont);

       GlbFontId=TerFont[NewFont].GlbFontId;
       if (GlbFontId>=0) GlbFont[GlbFontId].UseCount--;        // used global font
       else              DeleteObject(TerFont[NewFont].hFont); // used private font
       TerFont[NewFont].hFont=NULL;
       TerFont[NewFont].GlbFontId=-1;
    }

    // create subfont table information for wysiwyg display
    if (TerArg.PrintView && ScreenFont) {      // create printer character matrixs
       FarMove(PrtFont[NewFont].CharWidth,TerFont[NewFont].CharWidth,256*sizeof(int)); // to help reduce time in TerGetCharWidth

       CreateOneFont(w,UseScrMetrics?hTerDC:hPr,NewFont,FALSE);     // create font for printer display

       PrtFont[NewFont].height=TerFont[NewFont].height;
       PrtFont[NewFont].BaseHeight=TerFont[NewFont].BaseHeight;
       FarMove(TerFont[NewFont].CharWidth,PrtFont[NewFont].CharWidth,256*sizeof(int));
       PrtFont[NewFont].ExtLead=TerFont[NewFont].ExtLead;
       PrtFont[NewFont].OffsetVal=TerFont[NewFont].OffsetVal;

       if (TerFont[NewFont].hidden) {
         if (!PrtFont[NewFont].hidden) PrtFont[NewFont].hidden=OurAlloc(sizeof(struct StrHdnFont));
         FarMove(TerFont[NewFont].hidden,PrtFont[NewFont].hidden,sizeof(struct StrHdnFont));
       }

       // delete the existing printer font
       if (PrtFont[NewFont].GlbFontId>=0 || PrtFont[NewFont].hFont) {
          GlbFontId=PrtFont[NewFont].GlbFontId;
          if (GlbFontId>=0) GlbFont[GlbFontId].UseCount--;        // used global font
          else              DeleteObject(PrtFont[NewFont].hFont); // used private font
          PrtFont[NewFont].hFont=NULL;
          PrtFont[NewFont].GlbFontId=-1;
       }
       
       PrtFont[NewFont].hFont=TerFont[NewFont].hFont;
       PrtFont[NewFont].GlbFontId=TerFont[NewFont].GlbFontId;
    }

    // adjust typeface
    lstrcpy(TypeFace,TerFont[NewFont].TypeFace);
    if (TerFont[NewFont].CharSet!=SYMBOL_CHARSET) {  
       if (lstrcmpi(TypeFace,"Times")==0) lstrcpy(TypeFace,"Times New Roman");
       if (lstrcmpi(TypeFace,"Tms Rmn")==0) lstrcpy(TypeFace,"Times New Roman");
       if (UseScrMetrics) {
          if (lstrcmpi(TypeFace,"System")==0) lstrcpy(TypeFace,"Courier New");
       } 
    }

    // calculate new font height, font family and character set
    if (InPrinting || !ScreenFont || ZoomPercent==100 || ZoomPercent==0) {
       ResY=GetDeviceCaps(hDC,LOGPIXELSY);
       if (UseScrMetrics) {
         if (( InPrintPreview || !ScreenFont
            || True(TerOpFlags2&(TOFLAG2_PRINT_TO_META_DC|TOFLAG2_SCALE_PRT_DC)))) ResY=UNITS_PER_INCH;  // twips units
       }
       OrigHeight=MulDiv(TerFont[NewFont].TwipsSize,ResY,1440);     // convert points to pixels
       
       if (false && True(TerOpFlags2&(TOFLAG2_PRINT_TO_META_DC))) {
          double PhyResY=(((long)GetDeviceCaps(hDC,VERTRES)*25.4)/GetDeviceCaps(hDC,VERTSIZE));  // physical resolution
          double AdjResY=UNITS_PER_INCH*GetDeviceCaps(hDC,LOGPIXELSY)/PhyResY;
          OrigHeight=(int)(TerFont[NewFont].TwipsSize*AdjResY/1440);
          ResY=(int)AdjResY;
       }
    }
    else {
       ResY=OrigScrResY;
       OrigHeight=(int)(((long)TerFont[NewFont].TwipsSize*ResY*ZoomPercent)/((long)1440*100));
    }


    // get the font height
    NewStyle=TerFont[NewFont].style;
    if (True(TerFont[NewFont].InsRev)) NewStyle|=reviewer[TerFont[NewFont].InsRev].InsStyle;
    if (True(TerFont[NewFont].DelRev)) NewStyle|=reviewer[TerFont[NewFont].DelRev].DelStyle;

    if (!(TerFlags3&TFLAG3_EXACT_SCREEN_FONT) && ScreenFont && hDC==hTerDC && TerArg.PrintView 
       && !UsingZoomFonts && PrinterAvailable && hPr && SavePrtFontHeight>0) {
       CharHeight=PrtToScrY(SavePrtFontHeight)+1;
    }
    else {   // get the height from the point size
       CharHeight=OrigHeight;
       if (NewStyle&SUBSCR || NewStyle&SUPSCR) {
          CharHeight=(CharHeight+1)/REDUCTION;  // round up
          if ((TerFont[NewFont].TwipsSize/20)<=14) CharHeight=MulDiv(CharHeight,5,4);  // don't let the fonts be too small
       }
       else if (NewStyle&SCAPS && TerFont[NewFont].flags&FFLAG_SCAPS) CharHeight=MulDiv(CharHeight,2,3);  // reduce size for scaps

       if (CharHeight==0) CharHeight=1;  // otherwise the font manager will pickup a default size
       CharHeight=-CharHeight;   // height of the character in the font
    }

    // Get font character set, pitch/family, truetype etc
    CharSet=TerFont[NewFont].CharSet;
    TypefaceCharSet=GetCharSet(w,hTerDC,TypeFace,NULL,NULL,NULL);  // get character set for this type face
    if (/*CharSet==DEFAULT_CHARSET || */TypefaceCharSet==SYMBOL_CHARSET) CharSet=TypefaceCharSet;

    if (!mbcs && CharSet!=DEFAULT_CHARSET && CharSet!=ANSI_CHARSET 
        && CharSet!=SYMBOL_CHARSET && IsMbcsCharSet(w,CharSet,NULL)) EnableMbcs(w,CharSet);

    // get pitch family
    if (ScreenFont && hDC==hTerDC && TerArg.PrintView && !UsingZoomFonts && PrinterAvailable && hPr) {
       PitchFamily=SavePrtPitchFamily;
       IsTrueType=SavePrtIsTrueType;
    }
    else GetCharSet(w,hDC,TypeFace,&PitchFamily,&IsTrueType,NULL);

    if ((PitchFamily&0xF0)==FF_DONTCARE) PitchFamily=(BYTE)(DEFAULT_PITCH|TerFont[NewFont].FontFamily);
    if (TerFont[NewFont].FontFamily==FF_DONTCARE && hDC==hTerDC) TerFont[NewFont].FontFamily=(BYTE)(PitchFamily&0xF0);
    if (!ScreenFont) {
        SavePrtPitchFamily=PitchFamily;
        SavePrtIsTrueType=IsTrueType;
    }

    // fill the logical font structure
    FarMemSet(&lFont,0,sizeof(lFont));
    lFont.lfHeight=CharHeight;
    lFont.lfWidth=lFont.lfOrientation=0;
    lFont.lfEscapement=(OSCanRotate?0:(TerFont[NewFont].TextAngle*10));  // in tenth of degrees
    lFont.lfItalic=(BYTE)(FALSE);
    lFont.lfUnderline=(BYTE)((TerFont[NewFont].TempStyle&TSTYLE_ULINEF 
                      /*|| (TerFont[NewFont].style&ULINE && TerFont[NewFont].style&(SUBSCR|SUPSCR))*/) ? TRUE : FALSE);
    lFont.lfStrikeOut=(BYTE)(NewStyle&STRIKE ? TRUE : FALSE);
    lFont.lfCharSet=(CharSet==77?0:CharSet);   // all mac extended characters are conveted to unicode during rtf read
    lFont.lfOutPrecision=(BYTE)(UsingZoomFonts? OUT_TT_ONLY_PRECIS : OUT_DEFAULT_PRECIS);
    lFont.lfClipPrecision= CLIP_DEFAULT_PRECIS;
    lFont.lfQuality=DRAFT_QUALITY;
    lFont.lfPitchAndFamily=PitchFamily;
    lstrcpy(lFont.lfFaceName,TypeFace);


    // translate certain bitmap fonts to true type font
    if (IsScrDC || UsingZoomFonts) ConvertibleToTrueType(w,TypeFace,lFont.lfFaceName);

    // get the base height of the regulsr font for a bold/italic font
    BaseHeight=0;
    if ((NewStyle&(BOLD|ITALIC)) && (!ScreenFont/* || InPrinting*/) && !(TerOpFlags2&TOFLAG2_PDF) && AdjustFontHeight(w,NewFont,TypeFace,ResY,FALSE)) {
       HFONT hFont=NULL;
       if (!UsingZoomFonts && (GlbFontId=CreateGlbFont(w,&lFont,hDC))>=0) hFont=GlbFont[GlbFontId].hFont;
       else {
          hFont=CreateFontIndirect(&lFont);
          GlbFontId=-1;
       }
       if (hFont) {
         if (NULL!=(hOldFont=SelectObject(hDC,hFont))) {
            if (GetTextMetrics(hDC,&met)) BaseHeight=met.tmAscent;
            SelectObject(hDC,hOldFont);
         }
         if (GlbFontId>=0) GlbFont[GlbFontId].UseCount--;     // used global font
         else              DeleteObject(hFont);               // used private font
       }          
    } 

    // create the actual font
    lFont.lfWeight=NewStyle&BOLD   ? FW_BOLD  :  FW_REGULAR;
    lFont.lfItalic=(BYTE)(NewStyle&ITALIC ? TRUE : FALSE);

    // create the new font
    if (/*!UsingZoomFonts && */(GlbFontId=CreateGlbFont(w,&lFont,hDC))>=0) {    // a shared font created
       TerFont[NewFont].hFont=GlbFont[GlbFontId].hFont;
       TerFont[NewFont].GlbFontId=GlbFontId;         // shared font id
    }
    else {                        // try creating a private font
       TerFont[NewFont].hFont=CreateFontIndirect(&lFont);
       TerFont[NewFont].GlbFontId=-1;
    }

    if (TerFont[NewFont].hFont==NULL) return PrintError(w,MSG_ERR_FONT_CREATE,"CreateOneFont(a)");

    //** setup the character width table for the new font
    TerFont[NewFont].CharSet=CharSet;
    if (NULL==(hOldFont=SelectObject(hDC,TerFont[NewFont].hFont))) return PrintError(w,MSG_ERR_FONT_CREATE,"CreateOneFont(b)");
    if (!GetTextMetrics(hDC,&met)) return PrintError(w,MSG_ERR_FONT_CREATE,"CreateOneFont(c)");
    if (met.tmAscent==0) return PrintError(w,MSG_ERR_FONT_CREATE,"CreateOneFont(d)");
    TerFont[NewFont].ExtLead=met.tmExternalLeading;  // used for printing

    // get font shape and rtl properties
    LangInfo=GetFontLanguageInfo(hDC);
    TerFont[NewFont].VarWidth=((LangInfo&(GCP_GLYPHSHAPE|GCP_LIGATE))?TRUE:FALSE);
    //if (lstrcmpi(TerFont[NewFont].TypeFace,"latha")==0) TerFont[NewFont].VarWidth=TRUE;
    //if (lstrcmpi(TerFont[NewFont].TypeFace,"mangal")==0) TerFont[NewFont].VarWidth=TRUE;

    TerFont[NewFont].rtl=((LangInfo&(GCP_REORDER))?TRUE:FALSE);
    if (TerFont[NewFont].VarWidth || TerFont[NewFont].rtl) HasVarWidthFont=TRUE;

    // save the printer font height to create matching screen font
    if (!ScreenFont) {
       if (IsTrueType /*|| ConvertibleToTrueType(w,TerFont[NewFont].TypeFace,NULL)*/)
            SavePrtFontHeight=0;  // screen font size will be calculated from the pointsize
       else SavePrtFontHeight=met.tmHeight; // save for creating the screen font
    }

    if (UsingZoomFonts && ScreenFont) TerFont[NewFont].ExtLead=0;  // external leading not used with screen fonts

    if (UsingZoomFonts) goto END;  // use the old parameters

    // build the character width table
    if (!TerGetCharWidth(w,hDC,NewFont,ScreenFont,met.tmOverhang,met.tmPitchAndFamily)) return PrintError(w,MSG_ERR_FONT_CREATE,"CreateOneFont(d)");

    // determine if the italic style of this font overhangs
    if   (met.tmOverhang>1) TerFont[NewFont].flags|=FFLAG_ITALIC_OH;  // allow 1 pixel to filter the bold overhangs
    else TerFont[NewFont].flags=ResetUintFlag(TerFont[NewFont].flags,FFLAG_ITALIC_OH);


    //** calculate the distance from the base of the font to the top of the font
    TerFont[NewFont].height=met.tmHeight +met.tmExternalLeading;    // adjust the to real height of the font
    
    TerFont[NewFont].BaseHeightAdj=0;            // reset
    if (BaseHeight==0) BaseHeight=met.tmAscent;
    else               TerFont[NewFont].BaseHeightAdj=BaseHeight-met.tmAscent;  // BaseHeightAdj is not used for line height calculation, it is used only for text drawing
    TerFont[NewFont].BaseHeight=BaseHeight;

    //TerFont[NewFont].BaseHeight=(BaseHeight>0?BaseHeight:met.tmAscent);
    
    if ((!ScreenFont || InPrinting) && !(NewStyle&(SUPSCR|SUBSCR|SCAPS))) AdjustFontHeight(w,NewFont,TypeFace,ResY,TRUE);
    

    // apply font offset and super/subscript
    offset=(int)((long)TerFont[NewFont].offset*ResY/1440);
    TerFont[NewFont].OffsetVal=offset;
    if (NewStyle&SUPSCR || NewStyle&SUBSCR) {
       int FontHeight=TerFont[NewFont].height;
       TerFont[NewFont].height=OrigHeight;
       TerFont[NewFont].BaseHeight=met.tmAscent*OrigHeight/FontHeight;

       if (NewStyle&SUPSCR) TerFont[NewFont].BaseHeightAdj=-(int)((double)met.tmAscent/4.0); // -(int)((double)met.tmAscent/1.25);
       else                 TerFont[NewFont].BaseHeightAdj=OrigHeight-FontHeight;
    }
    if (TerFont[NewFont].offset!=0) {
       if (offset>0) TerFont[NewFont].BaseHeight+=offset;   // BaseHeight affects the line base height, whereas BaseHeightAdj just affects the display or printing
       else          TerFont[NewFont].BaseHeightAdj=-offset;
       TerFont[NewFont].height+=abs(offset);
    } 

    // adjust height for boxed characters
    if (TerFont[NewFont].style&CHAR_BOX) {
       if (hDC==hTerDC) TerFont[NewFont].BaseHeightAdj=TwipsToScrY(BOX_BORDER_WIDTH);
       else             TerFont[NewFont].BaseHeightAdj=TwipsToPrtY(BOX_BORDER_WIDTH);

       TerFont[NewFont].height+=(7*TerFont[NewFont].BaseHeightAdj/4);
       TerFont[NewFont].BaseHeight+=TerFont[NewFont].BaseHeightAdj;
    }

    //** set dimension to zero for hidden/footnote text
    if (HiddenText(w,NewFont)) {
         // save the original information
         if (!TerFont[NewFont].hidden) TerFont[NewFont].hidden=OurAlloc(sizeof(struct StrHdnFont));
         TerFont[NewFont].hidden->height=TerFont[NewFont].height;
         TerFont[NewFont].hidden->BaseHeight=TerFont[NewFont].BaseHeight;
         TerFont[NewFont].hidden->ExtLead=TerFont[NewFont].ExtLead;
         TerFont[NewFont].hidden->BaseHeightAdj=TerFont[NewFont].BaseHeightAdj;
         FarMove(TerFont[NewFont].CharWidth,TerFont[NewFont].hidden->CharWidth,sizeof(TerFont[NewFont].hidden->CharWidth));

         // set the height and width parameters to 0
         TerFont[NewFont].height=TerFont[NewFont].BaseHeight=0;
         TerFont[NewFont].ExtLead=TerFont[NewFont].BaseHeightAdj=0;
         for (i=0;i<256;i++) TerFont[NewFont].CharWidth[i]=0;
    }
    else if (TerFont[NewFont].hidden) {
         OurFree(TerFont[NewFont].hidden);
         TerFont[NewFont].hidden=NULL;
    }


    END:

    SelectObject(hDC,hOldFont);                  // restore the previous font

    return TRUE;
}
#pragma optimize("",off)  // restore optimization

/******************************************************************************
    AdjustFontHeight:
    Adjust font height for certain common fonts.
******************************************************************************/
BOOL AdjustFontHeight(PTERWND w,int NewFont, LPBYTE TypeFace, int ResY,BOOL apply)
{
    BOOL   AdjustHeight=TRUE;
    float factor;
    UINT style=TerFont[NewFont].style;

    if (TerFont[NewFont].CharSet!=ANSI_CHARSET && TerFont[NewFont].CharSet!=DEFAULT_CHARSET) return FALSE;  // no adjustment

    if (   lstrcmpi(TypeFace,"Arial")==0
        || lstrcmpi(TypeFace,"Times New Roman")==0) factor=(float)(.015975);  //.015975
    else if (lstrcmpi(TypeFace,"Courier New")==0)   factor=(float)(.01575);
    else if (lstrcmpi(TypeFace,"Courier")==0)       factor=(float)(.0166);
    else AdjustHeight=FALSE;

    if (AdjustHeight && apply) {
        float PointHeight=(float)(ResY*factor);  // apply a factor get the height of one point
        float height=(PointHeight*TerFont[NewFont].TwipsSize/20);
        int NewHeight=(int)(height+.5); // round it to nearest  integer
        TerFont[NewFont].height=NewHeight; // round it to nearest  integer
    }

    return AdjustHeight;
}

/******************************************************************************
    ConvertibleToTrueType:
    If a font is has a true type avaiable, return the true type font name.
******************************************************************************/
BOOL ConvertibleToTrueType(PTERWND w,LPBYTE RastFont, LPBYTE TtFont)
{
    BYTE name[32]="";

    if      (lstrcmpi(RastFont,"Courier")==0) lstrcpy(name,"Courier New");
    else if (lstrcmpi(RastFont,"Helv")==0) lstrcpy(name,"Arial");
    else if (lstrcmpi(RastFont,"Helvetica")==0) lstrcpy(name,"Arial");
    else if (lstrcmpi(RastFont,"MS Sans Serif")==0) lstrcpy(name,"Arial");
    else if (lstrcmpi(RastFont,"MS Serif")==0) lstrcpy(name,"Times New Roman");

    if (lstrlen(name)>0) {
       if (TtFont) lstrcpy(TtFont,name);
       return TRUE;
    }
    else return FALSE;
}

/******************************************************************************
    CreateGlbFont:
    Create a shared font id.  This function return the shared font id.  -1
    indicates an error.
******************************************************************************/
int CreateGlbFont(PTERWND w,LPLOGFONT pFont,HDC hDC)
{
    int i;
    LPLOGFONT pTblFont;
    BOOL IsScrDC;

    if (True(TerFlags5&TFLAG5_NO_SHARE)) return -1;     // do not shared fonts

    IsScrDC=(GetDeviceCaps(hDC,TECHNOLOGY)==DT_RASDISPLAY);
     
    // check if the font already exists in the table
    for (i=0;i<TotalGlbFonts;i++) {
       if (!GlbFont[i].hFont) continue;  // this slot not allocated

       pTblFont=&(GlbFont[i].lFont);

       // compare LOGFONT member
       if (GlbFont[i].hDC!=hDC) {
          if (!IsScrDC || !GlbFont[i].IsScrDC) continue;
       }

       if (pTblFont->lfHeight!=pFont->lfHeight) continue;
       if (pTblFont->lfWidth!=pFont->lfWidth) continue;
       if (pTblFont->lfEscapement!=pFont->lfEscapement) continue;
       if (pTblFont->lfOrientation!=pFont->lfOrientation) continue;
       if (pTblFont->lfWeight!=pFont->lfWeight) continue;
       if (pTblFont->lfItalic!=pFont->lfItalic) continue;
       if (pTblFont->lfUnderline!=pFont->lfUnderline) continue;
       if (pTblFont->lfStrikeOut!=pFont->lfStrikeOut) continue;
       if (pTblFont->lfCharSet!=pFont->lfCharSet) continue;
       if (pTblFont->lfOutPrecision!=pFont->lfOutPrecision) continue;
       if (pTblFont->lfClipPrecision!=pFont->lfClipPrecision) continue;
       if (pTblFont->lfQuality!=pFont->lfQuality) continue;
       if (pTblFont->lfPitchAndFamily!=pFont->lfPitchAndFamily) continue;
       if (lstrcmpi(pTblFont->lfFaceName,pFont->lfFaceName)!=0) continue;

       // match found
       GlbFont[i].UseCount++;
       return i;
    }

    // find an unused slot
    for (i=0;i<TotalGlbFonts;i++) if (!GlbFont[i].hFont) break;
    if (i==TotalGlbFonts) { // create a new font id
       if (TotalGlbFonts==MAX_GLB_FONTS) return -1;   // ran out of table space
       i=TotalGlbFonts;
       TotalGlbFonts++;
    }

    FarMemSet(&(GlbFont[i]),0,sizeof(struct StrGlbFont));
    if (NULL==(GlbFont[i].hFont=CreateFontIndirect(pFont))) return -1;  // font creation error

    GlbFont[i].lFont=(*pFont);
    GlbFont[i].UseCount=1;       // in use

    GlbFont[i].hDC=hDC;
    GlbFont[i].IsScrDC=IsScrDC;

    return i;
}

/******************************************************************************
    GetCurChar:
    Get the character at the given location.
******************************************************************************/
BYTE GetCurChar(PTERWND w,long line, int col)
{
    LPBYTE ptr;
    BYTE  CurChar;

    if (line>=TotalLines) return 0;

    if (col<0 || col>=LineLen(line)) return 0;

    // open the line and extract the format byte
    ptr=pText(line);
    CurChar=ptr[col];


    return CurChar;
}

/******************************************************************************
    TerGetCurFont:
    Get the format id of the given location.
******************************************************************************/
int WINAPI _export TerGetCurFont(HWND hWnd,long line, int col)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (line<0) {
       line=CurLine;
       col=CurCol;
    }

    return GetCurCfmt(w,line,col);
}

/******************************************************************************
    GetCurLead:
    Get the lead byte at the given location.
******************************************************************************/
BYTE GetCurLead(PTERWND w,long line, int col)
{
    LPBYTE lead;
    BYTE   CurLead;

    if (line>=TotalLines) return 0;

    if (col<0 || col>=LineLen(line)) return 0;

    if (pLead(line)==0) return 0;

    // open the line and extract the format byte
    lead=OpenLead(w,line);
    CurLead=lead[col];
    CloseLead(w,line);

    return CurLead;
}

/******************************************************************************
    GetTextFont:
    Turn-off attributes which are not used for regular text.
******************************************************************************/
GetTextFont(PTERWND w,int CurFont)
{
    if (TerFont[CurFont].style&PICT) return 0;

    if (TerFont[CurFont].FieldId>0 || TerFont[CurFont].FieldCode) 
        CurFont=(int)GetNewFieldId(w,(WORD)CurFont,0L,0L,0L,0);

    return CurFont;
}

/******************************************************************************
    GetCurCfmt:
    Get the format id of the given location.
******************************************************************************/
GetCurCfmt(PTERWND w,long line, int col)
{
    LPWORD fmt;
    int   CurCfmt;

    if (line<0 || line>=TotalLines) return DEFAULT_CFMT;

    if (col<0 || col>=LineLen(line)) return DEFAULT_CFMT;

    if (cfmt(line).info.type==UNIFORM) return cfmt(line).info.fmt;

    // open the line and extract the format byte
    fmt=OpenCfmt(w,line);
    CurCfmt=fmt[col];
    CloseCfmt(w,line);

    return CurCfmt;
}

/******************************************************************************
    GetPrevCfmt:
    Get the format id of the character before the given location.
******************************************************************************/
GetPrevCfmt(PTERWND w,long line, int col)
{
    int   len;

    if (line>=TotalLines) return DEFAULT_CFMT;

    len=LineLen(line);
    if (col<0 || col>len) return DEFAULT_CFMT;

    col--;
    if (col>=0) return GetCurCfmt(w,line,col);

    if (line==0) return 0;

    // get it from the previous line
    while (line>0) {
      line--;
      col=LineLen(line)-1;
      if (col>=0) break;
    }
    if (col<0) return 0;
    else       return GetCurCfmt(w,line,col);
}

/******************************************************************************
    GetNextCfmt:
    Get the format id of the character following the given location.
******************************************************************************/
GetNextCfmt(PTERWND w,long line, int col)
{
    int   len;

    if (line>=TotalLines) return DEFAULT_CFMT;

    len=LineLen(line);
    if (col<0 || col>=len) return DEFAULT_CFMT;

    col++;
    if (col<len) return GetCurCfmt(w,line,col);

    if ((line+1)==TotalLines) return 0;

    // get it from the next line
    while ((line+1)<TotalLines) {
      line++;
      if (LineLen(line)>0) break;
    }
    return GetCurCfmt(w,line,0);
}

/******************************************************************************
    TerSetEffectiveFont:
    Get the font id in effect for new character input
******************************************************************************/
BOOL WINAPI _export TerSetEffectiveFont(HWND hWnd, int NewFont)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (NewFont!=-1) {
       if (NewFont<0 || NewFont>=TotalFonts || !TerFont[NewFont].InUse || TerFont[NewFont].style&PICT) NewFont=0;
    }

    InputFontId=NewFont;

    return TRUE;
}

/******************************************************************************
MAK:
TerGetEffectiveFontEx:
Get the font id in effect for new character input
******************************************************************************/
HFONT WINAPI _export TerGetEffectiveFontEx(HWND hWnd)
{
	PTERWND w;int id;

	if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

	id = GetEffectiveCfmt(w);
	return TerFont[id].hFont;

}

/******************************************************************************
    TerGetEffectiveFont:
    Get the font id in effect for new character input
******************************************************************************/
int WINAPI _export TerGetEffectiveFont(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return GetEffectiveCfmt(w);
}

