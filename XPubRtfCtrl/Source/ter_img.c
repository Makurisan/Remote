/*==============================================================================
   TER_IMG.C
   TER Image support functions

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

// Local constant defintions

#define TIF_BYTE             1
#define TIF_ASCII            2
#define TIF_SHORT            3
#define TIF_LONG             4
#define TIF_RATIONAL         5
#define TIF_SBYTE            6
#define TIF_UNDEFINED        7
#define TIF_SSHORT           8
#define TIF_SLONG            9
#define TIF_SRATIONAL       10
#define TIF_FLOAT           11
#define TIF_DOUBLE          12

#define TIF_BITS_PER_SAMPLE   258
#define TIF_COLOR_MAP         320
#define TIF_COMPRESSION       259
#define TIF_HEIGHT            257
#define TIF_WIDTH             256
#define TIF_PHOTO_INTERPRET   262
#define TIF_RESOLUTION        296
#define TIF_ROWS_PER_STRIP    278
#define TIF_SAMPLES_PER_PIXEL 277
#define TIF_STRIP_BYTE_COUNTS 279
#define TIF_STRIP_OFFSETS     273

#define TIF_CMP_NONE          1
#define TIF_CMP_HUFFMAN       2
#define TIF_CMP_LZW           5
#define TIF_CMP_PACKBITS      32773

#define TIF_MAX_IN_BUF_SIZE   320000           // upgrade from 32000

#define RUN_NONE              0
#define RUN_WHITE             1
#define RUN_BLACK             2

/*****************************************************************************
    Bmp2Png:
    This routine translates input DIB buffer to the PNG format output buffer or 
    output file.
******************************************************************************/
BOOL WINAPI _export TerBmp2Png(HWND hWnd,int pict, int output, LPBYTE OutFile,HGLOBAL far *phBuf,LPLONG pBufLen)
{
    PTERWND w;
    HANDLE hFile=INVALID_HANDLE_VALUE;
    struct StrPng far *png=NULL;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
 
    if (pict<0 || pict>=TotalFonts || !(TerFont[pict].style&PICT)) return FALSE;

    // open the output file
    if (output==TER_FILE) {               // data in the file
       //lstrupr(w,OutFile);
       lstrcpy(TempString,OutFile);      // make a local copy
       rTrim(TempString);
       if (lstrlen(TempString)==0) return FALSE;
       _unlink(TempString);
       if (INVALID_HANDLE_VALUE==(hFile=CreateFile(TempString,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"Png2Bmp");
    }

    // allocate space for the png info structure
    if (NULL==(png=(struct StrPng far *)OurAlloc(sizeof(struct StrPng)))) {
       if (output==TER_FILE) CloseHandle(hFile);
       return PrintError(w,MSG_OUT_OF_MEM,"Png2Bmp");
    }
    FarMemSet(png,0,sizeof(struct StrPng));  // initialize with zeros
    png->w=w;                                // store our pointer
    png->pict=pict;                          // picture id

    if (output==TER_FILE) png->hFile=hFile;
    else {
       png->BufLen=100;                  // initial size of the buffer

       if ( NULL==(png->hBuf=GlobalAlloc(GMEM_MOVEABLE,png->BufLen))
         || NULL==(png->buf=(BYTE huge *)GlobalLock(png->hBuf)) ){
           return PrintError(w,MSG_OUT_OF_MEM,"Bmp2Png");
       }
       png->BufIndex=0;              // next character position in the buffer
       png->hFile=INVALID_HANDLE_VALUE;
    }

    PngInitVars(w,png);            // initialize variables and tables
    
    // allocate the circular buffer
    if ( NULL==(png->CircData=OurAlloc((long)(PNG_CIRC_BUF_HI+1000)*sizeof(BYTE))) 
      || NULL==(png->CircNext=OurAlloc((long)(PNG_CIRC_BUF_HI+1000)*sizeof(WORD))) ) { 
       if (output==TER_FILE) CloseHandle(hFile);
       return PrintError(w,MSG_OUT_OF_MEM,"Png2Bmp(a)");
    }

    // check if we are writing palette for this picture
    png->PalUsed=(TerFont[pict].PictType==PICT_DIBITMAP && TerFont[pict].ColorsUsed==256);
    png->PalUsed=False(TerFlags5&TFLAG5_PNG_RGB);

    if (TerFont[pict].FrameType!=PFRAME_NONE) SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),FALSE);

    // Get the picture dimension
    png->width=TerFont[pict].CharWidth[PICT_CHAR];   // width and height of the picture
    png->height=TerFont[pict].height;
    if (png->PalUsed) {
       // Determine the size of the DIB and allocate memory
       int BitsPerPixel=8;  // current implementation
       png->ScanLineSize=((long)png->width*BitsPerPixel+31)/32; // scan line width in number of DWORDS
       png->ScanLineSize*=4;  // scan line size in number of bytes
    }
     
    //render the picture in a memory device context or set palette based access
    if (NULL==(png->hDC=CreateCompatibleDC(hTerDC))) return PrintError(w,MSG_OUT_OF_COMP_DC,"Bmp2Png");
    if (NULL==(png->hBM=CreateCompatibleBitmap(hTerDC,png->width,png->height))) {
        return PrintError(w,MSG_OUT_OF_MEM,"Bmp2Png");
    }
    png->hOldBM=SelectObject(png->hDC,png->hBM);
    
    // set the background
    if (TerFont[pict].PictType==PICT_METAFILE || TerFont[pict].PictType==PICT_ENHMETAFILE) {
       COLORREF OldBkColor=SetBkColor(png->hDC,0xFFFFFF /*TextDefBkColor*/);
       RECT rect;
       SetRect(&rect,0,0,png->width,png->height);
       ExtTextOut(png->hDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);
       SetBkColor(png->hDC,OldBkColor); 
    } 
    
    PictOut(w,png->hDC,0,0,pict,1);
    
    if (TerFont[pict].FrameType!=PFRAME_NONE) SetPictSize(w,pict,0,0,TRUE);           // set it to the icon size

    // extract the picture information
    if (png->PalUsed && !PngExtractBitmap(w,png)) return FALSE;
    
    // start png output
    PngWriteHdr(w,png);            // write the header
    if (png->PalUsed) PngWritePal(w,png); // write the palette
    PngWriteImage(w,png);          // write the image data group

    // write the ending chunk
    if (!PngBeginChunk(w,png,"IEND",0)) return FALSE;
    if (!PngEndChunk(w,png)) return FALSE;

    // close file and release buffers
    if (output==TER_FILE) {
       PngFlushBytes(w,png);     // flush the remaining byte to the disk file
       CloseHandle(png->hFile);
    }
    else {
       png->buf[png->BufIndex]=0; 
       GlobalUnlock(png->hBuf);

       if (phBuf) (*phBuf)=png->hBuf;
       if (pBufLen) (*pBufLen)=png->BufIndex;
    }    


    // release resources
    if (png->hDC) {
      if (png->hOldBM) SelectObject(png->hDC,png->hOldBM);
      DeleteDC(png->hDC);
    }
    if (png->hBM)  DeleteObject(png->hBM);

    if (png->pInfo) OurFree(png->pInfo);
    if (png->pImage) OurFree(png->pImage);

    OurFree(png->CircData);
    OurFree(png->CircNext);
    
    OurFree(png);

    return TRUE;
}

/*****************************************************************************
    PngWriteHdr:
    This function writes the header.
******************************************************************************/
BOOL PngWriteHdr(PTERWND w,struct StrPng far *png)
{
    BYTE sign[]="\211PNG\r\n\032\n";
    struct StrPngHdr {
       DWORD width;
       DWORD height;
       BYTE BitDepth;
       BYTE ColorType;
       BYTE compression;
       BYTE filter;
       BYTE interlace;
    } hdr;

    // write the file signature
    if (!PngPutBytes(w,png,sign,8)) return FALSE;

    // write the header chunk size
    if (!PngBeginChunk(w,png,"IHDR",sizeof(struct StrPngHdr))) return FALSE;

    // write the header
    hdr.width=png->width;
    PngXlateDWord(&hdr.width);            // xlate to intel byte order
    
    hdr.height=png->height;
    PngXlateDWord(&hdr.height);
    
    hdr.BitDepth=8;   // 8 bits per color
    if (png->PalUsed) hdr.ColorType=PNGCLR_PAL;
    else              hdr.ColorType=PNGCLR_TRUE;
    hdr.compression=0;        //deflte/inflate compression
    hdr.filter=0;
    hdr.interlace=0;          // no interlace
    
    if (!PngPutBytes(w,png,(LPBYTE)&hdr,sizeof(hdr))) return FALSE;

    if (!PngEndChunk(w,png)) return FALSE;

    return TRUE;
}

/*****************************************************************************
    PngWritePal:
    This the palette chunk
******************************************************************************/
BOOL PngWritePal(PTERWND w,struct StrPng far *png)
{
    int pict=png->pict,i,ColorsUsed;
    long ChunkSize;
    RGBQUAD far *pColor;

    ColorsUsed=256;

    // write the chunk name size
    ChunkSize=3*ColorsUsed;
    
    if (!PngBeginChunk(w,png,"PLTE",ChunkSize)) return FALSE;

    // get the pointer to the color table
    pColor=(RGBQUAD far *)&(png->pInfo[sizeof(BITMAPINFOHEADER)]);

    // write the pallete
    for (i=0;i<(int)ColorsUsed;i++) {
       if (!PngPutByte(w,png,pColor[i].rgbRed)) return FALSE;
       if (!PngPutByte(w,png,pColor[i].rgbGreen)) return FALSE;
       if (!PngPutByte(w,png,pColor[i].rgbBlue)) return FALSE;
    } 

    if (!PngEndChunk(w,png)) return FALSE;

    return TRUE;
}

/*****************************************************************************
    PngWriteImage:
    This function writes image data
******************************************************************************/
BOOL PngWriteImage(PTERWND w,struct StrPng far *png)
{
    BYTE CMF;
    int y;
    struct StrPngHuffTbl HuffLit,HuffDist;
    long ChunkSize,SaveFilePos,ChunkFilePos;
   
    // initialize the variables
    png->Checksum=1;                 // initialize adler checksum

    FarMemSet(&HuffLit,0,sizeof(struct StrPngHuffTbl));
    FarMemSet(&HuffDist,0,sizeof(struct StrPngHuffTbl));
    PngCreateFixedHuffTable(w,png,&HuffLit,&HuffDist);
    
    // write the header chunk size
    ChunkFilePos=png->FilePos;                            // this position will be fixed later
    if (!PngBeginChunk(w,png,"IDAT",0)) return FALSE;     // The size will be fixed after the image is written

    // write the zlib header (2 bytes)
    CMF=0x78;   // cm=8: deflate compression, cinof=7: 32 k window
    if (!PngPutByte(w,png,CMF)) return FALSE;
    if (!PngPutByte(w,png,0x9c)) return FALSE;   // FLG byte
   
    png->CurChar=0;           // byte being prepared to be written out
    png->DataBitPos=0;        // number of bits prepared in png->CurChar which can be written out

    // Begin zlib block - we will write the entire image into one zlib block
    if (!PngPutBits(w,png,1,1)) return FALSE;   // the last block
    if (!PngPutBits(w,png,2,1)) return FALSE;   // use fixed Huffman codes

    for (y=0;y<png->height;y++) {
       if (png->PalUsed) {
         if (!PngWriteScan256(w,png,y,&HuffLit,&HuffDist)) return FALSE;  // write one scan line
       }
       else {
         if (!PngWriteScanRGB(w,png,y,&HuffLit,&HuffDist)) return FALSE;  // write one scan line
       } 
    }

    if (png->RunLen>0 && !PngWriteRun(w,png,&HuffLit,&HuffDist)) return FALSE;   // write the last run

    if (!PngWriteHuffCode(w,png,256,&HuffLit)) return FALSE;  // end of block character

    if (png->DataBitPos>0 && !PngPutByte(w,png,png->CurChar)) return FALSE;  // flush the last incomplete byte
   
    //End zlib block - write checksum of the databytes
    if (!PngPutDWord(w,png,png->Checksum)) return FALSE;

    // end IDAT chunk
    ChunkSize=png->ChunkSize;    // IDAT chunk size
    if (!PngEndChunk(w,png)) return FALSE;
    
    // fix the chunksize for the IDAT chunk
    SaveFilePos=png->FilePos;
    if (!PngFlushBytes(w,png)) return FALSE;
    if (!PngSetFilePos(w,png,ChunkFilePos)) return FALSE;   // position at the IDAT chunksize field
    if (!PngPutDWord(w,png,ChunkSize)) return FALSE;
    if (!PngFlushBytes(w,png)) return FALSE;
    
    if (!PngSetFilePos(w,png,SaveFilePos)) return TRUE;    // reset the file position
    
    // free the data resources
    PngFreeHuffTbl(w,png,&HuffLit);
    PngFreeHuffTbl(w,png,&HuffDist);
    
    return TRUE;
}

/*****************************************************************************
    PngWriteScanRGB:
    Write one scan line for RGB outut
******************************************************************************/
BOOL PngWriteScanRGB(PTERWND w,struct StrPng far *png, int y, struct StrPngHuffTbl far *pHuffLit, struct StrPngHuffTbl far *pHuffDist)
{
    BYTE red,green,blue,filter;
    BYTE CurRed,CurGreen,CurBlue;
    BYTE PrevRed,PrevGreen,PrevBlue;
    int  x;
    COLORREF color;
    
    // write the filter byte
    filter=0;                                                    // write the difference from the previous byte
    if (!PngWriteDataByte(w,png,filter,pHuffLit,pHuffDist)) return FALSE;  // write the huffman code the given value (16 bit)
    if (!PngUpdateChecksum(w,png,filter)) return FALSE;          // update adler checksum
   
    for (x=0;x<png->width;x++) {                   // write each pixel
       color=GetPixel(png->hDC,x,y);
       CurRed=GetRValue(color);
       CurGreen=GetGValue(color);
       CurBlue=GetBValue(color);

       // set the pixel colors to write
       if (filter==0 || x==0) {
          red=CurRed;
          green=CurGreen;
          blue=CurBlue;
       }
       else { 
          red=CurRed-PrevRed;
          green=CurGreen-PrevGreen;
          blue=CurBlue-PrevBlue;
       }
       PrevRed=CurRed;
       PrevGreen=CurGreen;
       PrevBlue=CurBlue;

       // write uncompressed
       //if (!PngWriteHuffCode(w,png,red,pHuffLit)) return FALSE;  
       //if (!PngWriteHuffCode(w,png,green,pHuffLit)) return FALSE;
       //if (!PngWriteHuffCode(w,png,blue,pHuffLit)) return FALSE;
       
       // write compressed
       if (!PngWriteDataByte(w,png,red,pHuffLit,pHuffDist)) return FALSE;  
       if (!PngWriteDataByte(w,png,green,pHuffLit,pHuffDist)) return FALSE;  
       if (!PngWriteDataByte(w,png,blue,pHuffLit,pHuffDist)) return FALSE;  

       // write the check sum
       if (!PngUpdateChecksum(w,png,red)) return FALSE;         // update adler checksum
       if (!PngUpdateChecksum(w,png,green)) return FALSE;       // update adler checksum
       if (!PngUpdateChecksum(w,png,blue)) return FALSE;        // update adler checksum
       
    } 
    
    if (png->RunLen>0) PngWriteRun(w,png,pHuffLit,pHuffDist);     // write the last run
    
    png->CircBufLen=0;                                          // flush the buffer to increase performance

    return TRUE;    
}
    
/*****************************************************************************
    PngWriteScan256:
    Write one scan line for the 256 color palette based output
******************************************************************************/
BOOL PngWriteScan256(PTERWND w,struct StrPng far *png, int y, struct StrPngHuffTbl far *pHuffLit, struct StrPngHuffTbl far *pHuffDist)
{
    int pict=png->pict;
    BYTE filter;
    int  x;
    BYTE PrevColor,CurColor,color;
    BYTE huge *pImage=png->pImage;
    BYTE huge *pScan;

    // write the filter byte
    filter=0;                                                    // write the difference from the previous byte
    if (!PngWriteHuffCode(w,png,filter,pHuffLit)) return FALSE;  // write the huffman code the given value (16 bit)
    if (!PngUpdateChecksum(w,png,filter)) return FALSE;          // update adler checksum
   
    // get the pointer to the scan line
    pScan=&(pImage[(png->height-y-1)*png->ScanLineSize]);
    

    for (x=0;x<png->width;x++) {                   // write each pixel
       CurColor=pScan[x];

       // set the pixel colors to write
       if (filter==0 || x==0) color=CurColor;
       else                   color=CurColor-PrevColor;
       PrevColor=CurColor;

       // write compressed
       if (!PngWriteDataByte(w,png,color,pHuffLit,pHuffDist)) return FALSE;  

       // write the check sum
       if (!PngUpdateChecksum(w,png,color)) return FALSE;         // update adler checksum
       
    } 
    
    if (png->RunLen>0) PngWriteRun(w,png,pHuffLit,pHuffDist);     // write the last run
    
    png->CircBufLen=0;                                          // flush the buffer to increase performance
    
    return TRUE;    
}
    
/*****************************************************************************
    PngWriteDataByte:
    Write one color value. it can any one of the three color components.
******************************************************************************/
BOOL PngWriteDataByte(PTERWND w,struct StrPng far *png, BYTE data, struct StrPngHuffTbl far *pHuffLit, struct StrPngHuffTbl far *pHuffDist)
{
    WORD i;
    WORD PrevRun;
    LPBYTE CircData=png->CircData;
    LPWORD CircNext=png->CircNext;

    if (png->CircBufLen==0) {    // first entry of the circular buffer, happens only for the first data byte
       PngWriteHuffCode(w,png,data,pHuffLit);    // write the first data byte and begin the circular buffer
       
       CircData[0]=data;
       CircNext[0]=0xFFFF;
       png->FirstRun=0xFFFF;    // first run starts at index 0
       png->RunLen=0;           // no pending runs to write
       png->CircBufLen=1;       // lenght of cicular buffer in use
    
    }
    else if (png->RunLen==0) {  // check if a new run can begin
       WORD CircBufLen=png->CircBufLen;
       png->FirstRun=0xFFFF;
       PrevRun=0xFFFF;
       i=0;                     // initial index

       memset(CircNext,0xFF,CircBufLen*sizeof(WORD));    // initialize the run chain
       for (i=0;i<CircBufLen;i++) {
          if (CircData[i]==data) {
             if (PrevRun!=0xFFFF) CircNext[PrevRun]=i;   // connect the run chain
             if (png->FirstRun==0xFFFF) png->FirstRun=i;   // first run
             PrevRun=i;
          }
       }
       if (png->FirstRun!=0xFFFF) png->RunLen=1;  // atleast one run started

       // if no run begins, write this byte and append it to the end of the circular buffer
       if (png->FirstRun==0xFFFF) {
          PngWriteHuffCode(w,png,data,pHuffLit);    // write the first data byte and begin the circular buffer
       
          CircData[png->CircBufLen]=data;
          CircNext[png->CircBufLen]=0xFFFF;
          png->CircBufLen++;       // lenght of cicular buffer in use
          png->FirstRun=0xFFFF;    // first run starts at index 0
          png->RunLen=0;           // no pending runs to write

          if (png->CircBufLen>PNG_CIRC_BUF_HI) PngScrollCircBuf(w,png);
       }   
    
    }
    else if (png->RunLen==PNG_MAX_RUN_LEN) {
       if (!PngWriteRun(w,png,pHuffLit,pHuffDist)) return FALSE;   //  write this run
       if (!PngWriteDataByte(w,png,data,pHuffLit,pHuffDist)) return FALSE;  // call this function again to start the new run
    } 
    else {         // one of more runs exist
       WORD RunLen=png->RunLen,PrevRun=0xFFFF;
       BOOL extend=FALSE;
       WORD FirstRun=0xFFFF,SaveRun;
       WORD run=png->FirstRun;

       while (run!=0xFFFF) {    // scan through the runs
          if ((run+RunLen)<png->CircBufLen && CircData[run+RunLen]==data) {  // this run is extendable
             if (PrevRun==0xFFFF) FirstRun=run;
             else                 CircNext[PrevRun]=run;
             PrevRun=run;
          }

          SaveRun=run;
          run=CircNext[run];         // advance to the next run 
          CircNext[SaveRun]=0xFFFF;  // will be set when the next extendable run is found
       } 

       if (FirstRun!=0xFFFF) {
           png->RunLen++;                //atleast one run can be extended
           png->FirstRun=FirstRun;
       }
       else {                          // write the previous run and update the circular buffer
          if (!PngWriteRun(w,png,pHuffLit,pHuffDist)) return FALSE;   //  write this run
          if (!PngWriteDataByte(w,png,data,pHuffLit,pHuffDist)) return FALSE;  // call this function again to start the new run
       } 
    }         
    
    return TRUE;   
}
        
/*****************************************************************************
    PngWriteRun:
    Write the previous run
******************************************************************************/
BOOL PngWriteRun(PTERWND w,struct StrPng far *png, struct StrPngHuffTbl far *pHuffLit, struct StrPngHuffTbl far *pHuffDist)
{
    WORD sym;
    WORD i,AppendBits,RunLen,dist;
    WORD AppendVal,FirstRun;
    BYTE data;
    LPBYTE CircData=png->CircData;
    LPWORD CircNext=png->CircNext;
    LPINT RunLenVal=png->RunLenVal;
    LPINT DistVal=png->DistVal;
    WORD CircBufLen;

    FirstRun=png->FirstRun;
    RunLen=png->RunLen;

    // write short run as individual items
    if (RunLen<3) {    // run length less than 3 can not stored as a run
       for (i=0;i<png->RunLen;i++) {
          data=CircData[FirstRun+i];
          PngWriteHuffCode(w,png,data,pHuffLit);    // store them as individual entires
       }
       goto END;
    }    

    // write the run length
    for (i=1;i<29;i++) if ((WORD)RunLenVal[i]>RunLen) break;  // max RunLenVal table length is 28
    i--;

    sym=i+257;   // symbol tbe bewritten out
    PngWriteHuffCode(w,png,sym,pHuffLit);

    AppendBits=(WORD)png->RunLenBits[i];   // number of bits to append
    AppendVal=RunLen-(WORD)png->RunLenVal[i];
    if (AppendBits>0 && !PngPutBits(w,png,AppendBits,AppendVal)) return FALSE;   // the last block


    // Write offset
    dist=png->CircBufLen-FirstRun;
    for (i=1;i<30;i++) if ((WORD)DistVal[i]>dist) break;  // max DistVal table length is 29
    i--;

    sym=i;        // symbol tbe bewritten out
    PngWriteHuffCode(w,png,sym,pHuffDist);

    AppendBits=(WORD)png->DistBits[i];   // number of bits to append
    AppendVal=dist-(WORD)png->DistVal[i];
    if (AppendBits>0 && !PngPutBits(w,png,AppendBits,AppendVal)) return FALSE;   // the last block


    END:

    // append written out data stream to the circular buffer
    CircBufLen=png->CircBufLen;
    for (i=0;i<RunLen;i++) {
       CircData[CircBufLen]=CircData[FirstRun+i];  //data;  // apppend to the circular buffer
       CircBufLen++;
    }
    png->CircBufLen=CircBufLen; 

    png->RunLen=0;
    if (png->CircBufLen>PNG_CIRC_BUF_HI) PngScrollCircBuf(w,png);

    return TRUE;
}

/*****************************************************************************
    PngScrollCircBuf:
    Scroll the circular buffer. In our implementation, this circualar buffer is really linear.
******************************************************************************/
BOOL PngScrollCircBuf(PTERWND w,struct StrPng far *png)
{
    int count=png->CircBufLen-PNG_CIRC_BUF_LO;  // amount to scroll

    if (png->CircBufLen<=PNG_CIRC_BUF_HI) return TRUE;  //scrolling not necessary yet

    memmove(png->CircData,&(png->CircData[count]),PNG_CIRC_BUF_LO);
    memmove(png->CircNext,&(png->CircNext[count]),PNG_CIRC_BUF_LO*sizeof(WORD));
    png->CircBufLen=PNG_CIRC_BUF_LO;

    return TRUE;
}
 
/*****************************************************************************
    PngWriteHuffCode:
    Write one huff code
******************************************************************************/
BOOL PngWriteHuffCode(PTERWND w,struct StrPng far *png, WORD sym, struct StrPngHuffTbl far *pHuff)
{
    if (!PngPutBits(w,png,pHuff->pCodeLen[sym],pHuff->pCode[sym])) return FALSE;   // the last block

    return TRUE;   
}
        
/*****************************************************************************
    PngBeginChunk:
    Write the chunk size.
******************************************************************************/
BOOL PngBeginChunk(PTERWND w,struct StrPng far *png,LPBYTE name, DWORD size)
{
    if (!PngPutDWord(w,png,size)) return FALSE;
   
    png->crc=0xFFFFFFFFL;    // initiaize running crc

    if (!PngPutBytes(w,png,name,4)) return FALSE;     // chunk name is always 4 bytes
    png->ChunkSize=0;        // iniitialize the chunksize

    return TRUE; 
}
    
/*****************************************************************************
    PngEndChunk:
    End the currnt chunk.
******************************************************************************/
BOOL PngEndChunk(PTERWND w,struct StrPng far *png)
{
    DWORD crc=(png->crc)^(0xFFFFFFFFL);
    
    if (!PngPutDWord(w,png,crc)) return FALSE;
   
    return TRUE; 
}
    
/*****************************************************************************
    PngUpdateChecksum:
    Update Adler checksum.
******************************************************************************/
BOOL PngUpdateChecksum(PTERWND w,struct StrPng far *png,BYTE CurChar)
{
    #define ADLER_BASE 65521L  /*largest prime smaller than 65536 */

    DWORD lo=(png->Checksum&0xFFFF);       // first 16 bits
    DWORD hi=(png->Checksum>>16)&0xFFFF;    // second 16 bits

    lo=(lo+CurChar)%ADLER_BASE;
    hi=(hi+lo)%ADLER_BASE;

    png->Checksum=((hi<<16)+lo);

    return TRUE;
}
    
/*****************************************************************************
    PngExtratBitmap:
    Extact bitmap data in uncompressed format (RGB)
******************************************************************************/
BOOL PngExtractBitmap(PTERWND w,struct StrPng far *png)
{
    LPBYTE  pImage;
    LPBITMAPINFOHEADER pInfo;
    int    NumColors=256;
    DWORD  InfoSize,ImageSize;

    // free the bitmap from DC
    SelectObject(png->hDC,png->hOldBM);

    //********** create the device independent bitmap ***********************
    InfoSize=sizeof(BITMAPINFOHEADER)+NumColors*sizeof(RGBQUAD);  // create a 256 color bitmap structure

    if (NULL==(pInfo=(LPVOID)OurAlloc(InfoSize))) {
        PrintError(w,MSG_OUT_OF_MEM,"PngExtractBitmap");
        return -1;
    }

    //***** fill in the info structure ******
    pInfo->biSize=sizeof(BITMAPINFOHEADER);
    pInfo->biWidth=png->width;
    pInfo->biHeight=png->height;
    pInfo->biPlanes=1;                       // DIB have plane = 1
    pInfo->biBitCount=8;                     // 8 bits per pixel or color
    pInfo->biCompression=BI_RGB;             // no compression
    pInfo->biSizeImage=0;                    // initialize to zero
    pInfo->biXPelsPerMeter=0;
    pInfo->biYPelsPerMeter=0;
    pInfo->biClrUsed=0;                      // depends on biBitCount
    pInfo->biClrImportant=0;                 // all colors important

    if (!GetDIBits(png->hDC,png->hBM,0,png->height,NULL,(LPBITMAPINFO)pInfo,DIB_RGB_COLORS)) {
        PrintError(w,MSG_ERR_DIB_CREATE,"PngExtractBitmap");
        return -1;
    }

    ImageSize=pInfo->biSizeImage;

    if (NULL==(pImage=OurAlloc(ImageSize))) {
        PrintError(w,MSG_OUT_OF_MEM,"PngExtractBitmap(a)");
        return -1;
    }

    if (!GetDIBits(png->hDC,png->hBM,0,png->height,pImage,(LPBITMAPINFO)pInfo,DIB_RGB_COLORS)) {
        PrintError(w,MSG_ERR_DIB_CREATE,"PngExtractBitmap");
        return -1;
    }

    png->pInfo=(LPBYTE)pInfo;
    png->pImage=pImage;
    
    DeleteDC(png->hDC);
    png->hDC=NULL;
 
    return TRUE;
}
 
/*****************************************************************************
    PngPutDWord:
    Write a long variable.
******************************************************************************/
BOOL PngPutDWord(PTERWND w,struct StrPng far *png,DWORD val)
{
    PngXlateDWord(&val);
    return PngPutBytes(w,png,(LPBYTE)&val,sizeof(DWORD));
}
    
/*****************************************************************************
    PngPutBytes:
    Write the specified number of bytes to the png output destination.  The text
    can include the binary charactes including the 0 character.
******************************************************************************/
BOOL PngPutBytes(PTERWND w,struct StrPng far *png,LPBYTE text, int len)
{
    int i;

    for (i=0;i<len;i++) if (!PngPutByte(w,png,text[i])) return FALSE;

    return TRUE;
}

/*****************************************************************************
    PngPutBits:
    Save the specified number of bits to the output stream
******************************************************************************/
BOOL PngPutBits(PTERWND w,struct StrPng far *png, int NumBits, DWORD val)
{
    BYTE CurChar=png->CurChar;
    int  CurBitPos=png->DataBitPos,CurBits;
    DWORD mask;
    DWORD AddVal;

    if (NumBits<=0) return TRUE;

    while(NumBits>0) {
       CurBits=8-CurBitPos;   // max bits that can be store in the current byte
       if (CurBits>NumBits) CurBits=NumBits;
       mask=(((DWORD)1)<<CurBits)-1;          // mask for current number of bits
       AddVal=(val&mask);                     // extract the bits
       CurChar=(BYTE)((AddVal<<CurBitPos)+CurChar);
       CurBitPos+=CurBits;
       if (CurBitPos==8) {                    // write this byte out
          if (!PngPutByte(w,png,CurChar)) return FALSE;
          CurChar=0;
          CurBitPos=0;
       } 
       val>>=CurBits;                         // discard these many bits 
       NumBits-=CurBits;
    }    

    png->CurChar=CurChar;
    png->DataBitPos=CurBitPos;

    return TRUE;
}

/*****************************************************************************
    PngPutByte:
    Write a character to png output
******************************************************************************/
BOOL PngPutByte(PTERWND w,struct StrPng far *png,BYTE CurChar)
{
    if (png->hFile!=INVALID_HANDLE_VALUE) {
       if (png->TextIndex>=MAX_WIDTH) PngFlushBytes(w,png);
       png->text[png->TextIndex]=CurChar;
       png->TextIndex++;
    }
    else {
        if (png->BufIndex>=png->BufLen) {    // make the buffer bigger
           png->BufLen+=100;                  // allocate additiona 100 bytes
           GlobalUnlock(png->hBuf);
           if ( NULL==(png->hBuf=GlobalReAlloc(png->hBuf,png->BufLen+1, GMEM_MOVEABLE))
             || NULL==(png->buf=(BYTE huge *)GlobalLock(png->hBuf)) ){
              return PrintError(w,MSG_OUT_OF_MEM,"PngPutByte");
           }
        }
        png->buf[png->BufIndex]=CurChar;
        png->BufIndex++;    
    }

    png->FilePos++;                          // number of bytes written to output
    png->ChunkSize++;                        // current chunk size
    
    // update running crc
    png->crc=png->CrcTable[(png->crc^CurChar)&0xFF] ^ (png->crc>>8);
        
    return TRUE;
}

/*****************************************************************************
    PngSetFilePos:
    Set the file pointer to the given position
******************************************************************************/
BOOL PngSetFilePos(PTERWND w,struct StrPng far *png,long NewFilePos)
{
    png->FilePos=NewFilePos;
    if (png->hFile!=INVALID_HANDLE_VALUE) {
       SetFilePointer(png->hFile,png->FilePos,0,FILE_BEGIN);
    }
    else png->BufIndex=png->FilePos;

    return TRUE;
}
    
/*****************************************************************************
    PngFlushByte:
    Flush the png buffer to the file
******************************************************************************/
BOOL PngFlushBytes(PTERWND w,struct StrPng far *png)
{
    DWORD written;

    if (png->hFile==INVALID_HANDLE_VALUE) return TRUE;  // output not going to a file
    if (png->TextIndex==0) return TRUE;        // buffer empty

    if (!WriteFile(png->hFile,png->text,png->TextIndex,&written,NULL)) 
         return PrintError(w,MSG_ERR_FILE_WRITE,"PngFlushBytes");
    png->TextIndex=0;

    return TRUE;
}
    
/*****************************************************************************
    PNG file translation function
******************************************************************************/
/*****************************************************************************
    Png2Bmp:
    This routine translates input buffer or file in the PNG format to an
    output buffer in MS Windows DIB format.
******************************************************************************/
BOOL Png2Bmp(PTERWND w,int input, LPBYTE InFile,HGLOBAL hBuf,long BufLen, HGLOBAL far *hDIB)
{
    HANDLE hFile=INVALID_HANDLE_VALUE;
    struct StrPng far *png=NULL;
    BYTE huge *InBuf;
    BOOL import=TRUE,IsPng;
    DWORD ChunkSize;
    BYTE  ChunkType[5];

    if (hDIB) (*hDIB)=NULL;
    else      import=FALSE;

    // open the input file
    if (input==TER_FILE) {               // data in the file
       //lstrupr(w,InFile);
       rTrim(InFile);                   // trim any spaces on right
       if (lstrlen(InFile)==0) return FALSE;
       lstrcpy(TempString,InFile);      // make a copy in the data segment
       if ((hFile=CreateFile(TempString,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"Png2Bmp");
    }
    else {                                // lock the buffer
       if (NULL==(InBuf=GlobalLock(hBuf))) return PrintError(w,MSG_ERR_MEM_LOCK,"Png2Bmp");
    }

    // allocate space for the png info structure
    if (NULL==(png=(struct StrPng far *)OurAlloc(sizeof(struct StrPng)))) {
       if (input==TER_FILE) CloseHandle(hFile);
       return PrintError(w,MSG_OUT_OF_MEM,"Png2Bmp");
    }
    FarMemSet(png,0,sizeof(struct StrPng));  // initialize with zeros
    png->w=w;                                // store our pointer
    png->input=TRUE;                         // reading png data

    if (input==TER_FILE) png->hFile=hFile;
    else {
       png->buf=InBuf;
       png->BufLen=BufLen;
       png->hFile=INVALID_HANDLE_VALUE;
    }

    PngInitVars(w,png);            // initialize variables and tables

    // Read PNG header
    IsPng=PngReadHdr(w,png,import);
    if (!import) goto END_FILE;    // file type detection only
    if (!IsPng)  goto END_FILE;

    // read and process groups
    NEXT_CHUNK:
    if (!PngGetBytes(w,png,&ChunkSize,sizeof(ChunkSize))) goto END_FILE;  // end of file
    PngXlateDWord(&ChunkSize);
    if (!PngGetBytes(w,png,ChunkType,4)) goto END_FILE;  // end of file
    ChunkType[4]=0;

    // process the collected data
    if (lstrcmpi(ChunkType,"IDAT")!=0 && png->DataSize>0 && !PngProcessData(w,png)) goto SYNTAX_ERROR;

    if (lstrcmpi(ChunkType,"IEND")==0) goto END_FILE;
    else if (lstrcmpi(ChunkType,"IDAT")==0) {
        if (!PngCollectData(w,png,ChunkSize)) goto SYNTAX_ERROR;
    }
    else if (lstrcmpi(ChunkType,"PLTE")==0 && png->ColorType==PNGCLR_PAL) {
        if (!PngReadPalette(w,png,ChunkSize)) goto SYNTAX_ERROR;
    }
    else if (lstrcmpi(ChunkType,"bKGD")==0) {           // background color
        if (!PngReadBknd(w,png,ChunkSize)) goto SYNTAX_ERROR;
    }
    else if (lstrcmpi(ChunkType,"tRNS")==0) {           // transparent color
        if (!PngReadXparent(w,png,ChunkSize)) goto SYNTAX_ERROR;
    }
    else if (lstrcmpi(ChunkType,"pHYs")==0) {           // physical resolution
        if (!PngReadRes(w,png,ChunkSize)) goto SYNTAX_ERROR;
    }
    else PngGetBytes(w,png,NULL,ChunkSize);     // read over the unsupported chunks

    PngGetBytes(w,png,NULL,sizeof(DWORD));


    goto NEXT_CHUNK;

    SYNTAX_ERROR:
    MessageBox(hTerWnd,MsgString[MSG_PNG_ABORT],NULL,MB_OK);
    OurPrintf("pos: %ld",png->FilePos);

    END_FILE:

    // close file and release buffers
    if (input==TER_FILE) CloseHandle(png->hFile);

    if (png->hDIB) GlobalUnlock(png->hDIB);

    if (hDIB==NULL || !IsPng) {     // data not used
       GlobalFree(png->hDIB);
       png->hDIB=NULL;
    }
    if (hDIB && import) (*hDIB)=png->hDIB;

    if (png->pPrevScan) OurFree(png->pPrevScan);    // release the filter buffer
    if (png->pScan) OurFree(png->pScan);    // release the filter buffer
    OurFree(png);

    return (import?((BOOL)(*hDIB)):IsPng);
}

/*****************************************************************************
    PngReadHdr:
    This function reads the header and returns TRUE if it corresponds to
    a PNG file.
******************************************************************************/
BOOL PngReadHdr(PTERWND w,struct StrPng far *png, BOOL import)
{
    BYTE sign[]="\211PNG\r\n\032\n",string[5];
    BYTE InputSign[9];
    long l,BitmapSize;
    LPBITMAPINFOHEADER pInfo;
    struct StrPngHdr {
       DWORD width;
       DWORD height;
       BYTE BitDepth;
       BYTE ColorType;
       BYTE compression;
       BYTE filter;
       BYTE interlace;
    } hdr;
    DWORD ChunkSize;
    int BitsPerPixel;
    BOOL CreatePal=FALSE;
    COLORREF BkColor;

    // check the file signature
    if (!PngGetBytes(w,png,InputSign,8)) return FALSE;
    InputSign[8]=0;
    if (lstrcmpi(InputSign,sign)!=0) return FALSE;

    // read the header chunk
    if (!PngGetBytes(w,png,&ChunkSize,sizeof(ChunkSize))) return FALSE;
    PngXlateDWord(&ChunkSize);
    if (ChunkSize!=sizeof(hdr)) return FALSE;
    if (!PngGetBytes(w,png,string,4)) return FALSE;
    string[4]=0;
    if (lstrcmpi(string,"IHDR")!=0) return FALSE;
    if (!import) return TRUE;

    if (!PngGetBytes(w,png,&hdr,sizeof(hdr))) return FALSE;
    PngXlateDWord(&hdr.width);            // xlate to intel byte order
    PngXlateDWord(&hdr.height);

    PngGetBytes(w,png,NULL,sizeof(DWORD));    // discard the crc dword

    // Copy the hdr varaible to the png structure
    png->width=(int)hdr.width;
    png->height=(int)hdr.height;
    png->interlaced=hdr.interlace;
    png->ColorType=hdr.ColorType;

    png->ClrTblSize=0;
    png->BytesPerPixel=1;         // bytes per pixel excluding alpha channel
    png->BytesPerPixelAlpha=1;      // bytes per pixel including alpha channel
    png->BytesPerColor=1;
    png->BitDepth=hdr.BitDepth;
    png->ColorComponents=1;


    if (png->ColorType==PNGCLR_GRAY || png->ColorType==PNGCLR_GRAY_ALPHA) {
       BitsPerPixel=hdr.BitDepth;
       if (BitsPerPixel==2) BitsPerPixel=4;    // Windows does allow 2 bits per pixel
       if (BitsPerPixel>8)  BitsPerPixel=8;
       png->ClrTblSize=(1<<BitsPerPixel);
       png->PixelsPerByte=8/hdr.BitDepth;
       if (hdr.BitDepth==16) png->BytesPerPixel=png->BytesPerColor=2;
       CreatePal=TRUE;
       if (png->ColorType==PNGCLR_GRAY_ALPHA) {
          png->ReadAlpha=TRUE;  // read alpha channel bits
          png->BytesPerPixelAlpha=png->BytesPerPixel+hdr.BitDepth/8;
       }
       else png->BytesPerPixelAlpha=png->BytesPerPixel;
    }
    else if (png->ColorType==PNGCLR_PAL) {
       BitsPerPixel=hdr.BitDepth;
       if (BitsPerPixel==2) BitsPerPixel=4;    // Windows does allow 2 bits per pixel
       png->ClrTblSize=(1<<BitsPerPixel);
       png->PixelsPerByte=8/hdr.BitDepth;
    }
    else if (png->ColorType==PNGCLR_TRUE || png->ColorType==PNGCLR_TRUE_ALPHA) {
       png->BytesPerColor=hdr.BitDepth/8;
       png->BytesPerPixel=png->BytesPerColor*3;
       png->PixelsPerByte=0;                  // a pixel does not completely fit in one byte
       png->ColorComponents=3;                // RGB colors
       BitsPerPixel=24;
       if (png->ColorType==PNGCLR_TRUE_ALPHA) {
          png->ReadAlpha=TRUE;  // read alpha channel bits
          png->BytesPerPixelAlpha=png->BytesPerPixel+hdr.BitDepth/8;
       }
       else png->BytesPerPixelAlpha=png->BytesPerPixel;
    }

    // allocate the filter buffer
    png->FilterBufSize=(png->width+1)*png->BytesPerPixelAlpha;
    png->pPrevScan=OurAlloc(png->FilterBufSize);
    png->pScan=OurAlloc(png->FilterBufSize);
    FarMemSet(png->pPrevScan,0,png->FilterBufSize);
    FarMemSet(png->pScan,0,png->FilterBufSize);


    // Determine the size of the DIB and allocate memory
    png->ScanLineSize=((long)png->width*BitsPerPixel+31)/32; // scan line width in number of DWORDS
    png->ScanLineSize*=4;  // scan line size in number of bytes

    BitmapSize=sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*png->ClrTblSize+png->ScanLineSize*png->height;

    if (  NULL==(png->hDIB=GlobalAlloc(GMEM_MOVEABLE,BitmapSize+1))
       || NULL==(pInfo=GlobalLock(png->hDIB)) ){
        return PrintError(w,MSG_OUT_OF_MEM,"PngReadHdr");
    }

    // fill in the header variables
    FarMemSet(pInfo,0,sizeof(BITMAPINFOHEADER));
    pInfo->biSize=sizeof(BITMAPINFOHEADER);
    pInfo->biWidth=png->width;
    pInfo->biHeight=png->height;
    pInfo->biPlanes=1;
    pInfo->biBitCount=BitsPerPixel;
    pInfo->biCompression=BI_RGB;       // no compression
    pInfo->biSizeImage=png->ScanLineSize*png->height;
    pInfo->biClrUsed=png->ClrTblSize;

    // initialize the bitmap
    png->pColor=(RGBQUAD far *) & (((LPBYTE)pInfo)[(int)pInfo->biSize]); // pointer to the color table
    png->pImage=(LPBYTE) & (((LPBYTE)pInfo)[(int)pInfo->biSize+sizeof(RGBQUAD)*png->ClrTblSize]); // pointer to the color table
    if (pInfo->biSizeImage<=0xFFFFL) FarMemSet(png->pImage,(BYTE)0,(UINT)pInfo->biSizeImage);
    else {
        for (l=0;l<(long)pInfo->biSizeImage;l++) png->pImage[l]=(BYTE)0;
    }

    // set the background color values
    BkColor=PageColor(w);
    png->BkndColor[0]=GetRValue(BkColor);
    png->BkndColor[1]=GetGValue(BkColor);
    png->BkndColor[2]=GetBValue(BkColor);
    if (png->ColorType==PNGCLR_GRAY || png->ColorType==PNGCLR_GRAY_ALPHA) {
       png->BkndColor[0]=(BYTE)(((WORD)png->BkndColor[0]+(WORD)png->BkndColor[1]+(WORD)png->BkndColor[2])/3);
       png->BkndColor[1]=png->BkndColor[2]=png->BkndColor[0];
    }

    // create palette info if needed
    png->BkndIdx=128;                        // index of the background color
    if (CreatePal) PngReadPalette(w,png,0);  // create a palette for gray colors


    png->NewScanLine=TRUE;
    if (png->interlaced) {
       png->GridX=png->GridY=0;
       png->GridPass=1;
    }

    // set the default picture resolution
    ImgDenX=OrigScrResX;
    ImgDenY=OrigScrResY;

    return TRUE;
}

/*****************************************************************************
    PngReadRes:
    Read the pixel resolution information
******************************************************************************/
BOOL PngReadRes(PTERWND w,struct StrPng far *png, DWORD ChunkSize)
{
    DWORD ByteCount=0;

    struct StrRes {
       int ResX;
       int ResY;
       BYTE unit;
    }res;

    if (ChunkSize!=sizeof(res)) return FALSE;

    if (!PngGetBytes(w,png,&res,sizeof(res))) return FALSE;
    PngXlateDWord(&res.ResX);            // xlate to intel byte order
    PngXlateDWord(&res.ResY);

    if (res.unit==1) {            // per meter
       ImgDenX=MulDiv(res.ResX,254,10000);  // convert per meter to per inch
       ImgDenY=MulDiv(res.ResY,254,10000);  
    } 

    return TRUE;
}

/*****************************************************************************
    PngCollectData:
    Collect the data in a buffer
******************************************************************************/
BOOL PngCollectData(PTERWND w,struct StrPng far *png, DWORD ChunkSize)
{
    if (ChunkSize==0) return TRUE;

    if (png->pData) png->pData=OurRealloc(png->pData,png->DataSize+ChunkSize+2);
    else            png->pData=OurAlloc(ChunkSize+2);

    if (!PngGetBytes(w,png,&(png->pData[png->DataSize]),ChunkSize));    // read the data bayes

    png->DataSize+=ChunkSize;

    return TRUE;
}

/*****************************************************************************
    PngProcessData:
    Process the image data
******************************************************************************/
BOOL PngProcessData(PTERWND w,struct StrPng far *png)
{
    int  temp,CmpType,sym;
    WORD CopyFrom,CopyLen,CopyDist;
    WORD OutBufPos,OutBufSize;
    BYTE CurByte;
    BOOL LastBlock=FALSE;
    struct StrPngHuffTbl HuffLit,HuffDist;
    struct StrPngHuffTbl far *pHuffLit;
    struct StrPngHuffTbl far *pHuffDist;
    LPBYTE pOutBuf=NULL;
    int count=0;

    // apply the alpha factor on the palette
    if (png->ColorType==PNGCLR_PAL && png->AlphaCount>0) PngApplyPalAlpha(w,png);

    // initialize the variables
    FarMemSet(&HuffLit,0,sizeof(struct StrPngHuffTbl));
    FarMemSet(&HuffDist,0,sizeof(struct StrPngHuffTbl));

    // read the zlib header
    CurByte=png->pData[png->DataPos];
    png->DataPos++;

    if ((CurByte&0xF)!=8) return PrintError(w,MSG_BAD_PNG_DATA,"PngProcessData");
    

    temp=((CurByte&0xF0)>>4);
    OutBufSize=(WORD)(1L<<(temp+8))+1;    // word buffer size (circular buffer)
    pOutBuf=OurAlloc(OutBufSize+1);
    FarMemSet(pOutBuf,0,OutBufSize);
    OutBufPos=0;

    png->DataPos++;       // skip over the FLG byte
    png->DataBitPos;

    while (!LastBlock) {    // process each deflate block
        LastBlock=PngGetBits(w,png,1);

        CmpType=PngGetBits(w,png,2);
        if (CmpType==PNGCMP_ERROR) return PrintError(w,MSG_BAD_PNG_DATA,"PngProcessData(1)");  // bad compression code encountered

        if (CmpType==PNGCMP_FIXED)         PngCreateFixedHuffTable(w,png,&HuffLit,&HuffDist);
        else if (CmpType==PNGCMP_DYNAMIC)  PngReadHuffTables(w,png,&HuffLit,&HuffDist);  // build the huffman tables
        pHuffLit=&HuffLit;
        pHuffDist=&HuffDist;

        while(TRUE) {     // extract the pixel bytes
           sym=PngExtractHuffSym(w,png,pHuffLit);
           if (sym<0) return FALSE;

           if (sym==256) break;         // end of this block

           if (sym<256) {
              PngData(w,png,(BYTE)(WORD)sym);       // send the data bytes

              //  copy to the output buffer;
              pOutBuf[OutBufPos]=(BYTE)(WORD)sym;
              OutBufPos++;
              if (OutBufPos>=OutBufSize) OutBufPos=0;   // circular buffer
              count++;
           }
           else {                        // copy it from the output stream
              LPBYTE ptr1,ptr2;
              long temp;

              CopyLen=png->RunLenVal[sym-257]+PngGetBits(w,png,png->RunLenBits[sym-257]);  // number of bytes to copy
              sym=PngExtractHuffSym(w,png,pHuffDist);
              CopyDist=png->DistVal[sym]+PngGetBits(w,png,png->DistBits[sym]);

              temp=(long)(DWORD)OutBufPos-CopyDist;
              if (temp<0) temp+=OutBufSize;   // circular buffer
              CopyFrom=(WORD)temp;

              ptr1=&(pOutBuf[CopyFrom]);              // copy from
              ptr2=&(pOutBuf[OutBufPos]);        // store at

              while (CopyLen>0) {
                  sym=(*ptr1);
                  CopyFrom++;
                  ptr1++;
                  if (CopyFrom>=OutBufSize) {
                      CopyFrom=0;   // circular buffer
                      ptr1=pOutBuf;
                  }

                  PngData(w,png,(BYTE)(WORD)sym);    // send the data bytes

                  //  copy to the output buffer;
                  (*ptr2)=(BYTE)(WORD)sym;
                  OutBufPos++;
                  ptr2++;
                  if (OutBufPos>=OutBufSize) {
                     OutBufPos=0;   // circular buffer
                     ptr2=pOutBuf;
                  }

                  CopyLen--;
                  count++;
              }
           }
        }
    }

    // free the data resources
    PngFreeHuffTbl(w,png,&HuffLit);
    PngFreeHuffTbl(w,png,&HuffDist);

    OurFree(png->pData);           // free the databloack
    png->pData=NULL;
    png->DataSize=0;

    if (pOutBuf) OurFree(pOutBuf);

    return TRUE;
}

/*****************************************************************************
    PngCreateFixedHuffTable:
    Create the fixed huffman table
******************************************************************************/
PngCreateFixedHuffTable(PTERWND w,struct StrPng far *png, struct StrPngHuffTbl far *pHuffLit, struct StrPngHuffTbl far *pHuffDist)
{
    int i;

    PngAllocHuffTbl(w,png,pHuffLit,288);   // allocate the table
    PngAllocHuffTbl(w,png,pHuffDist,30);   // allocate the table

    // assign the symbol and codelength values for the literal/length table
    for (i=0;i<pHuffLit->count;i++) {
       pHuffLit->pSymbol[i]=i;
       if      (i<144) pHuffLit->pCodeLen[i]=8;
       else if (i<256) pHuffLit->pCodeLen[i]=9;
       else if (i<280) pHuffLit->pCodeLen[i]=7;
       else            pHuffLit->pCodeLen[i]=8;
    }

    for (i=0;i<pHuffDist->count;i++) {     // distance table
       pHuffDist->pSymbol[i]=i;
       pHuffDist->pCodeLen[i]=5;
    }

    // create the huffman codes
    PngCreateHuffCode(w,png,pHuffLit);
    PngCreateHuffCode(w,png,pHuffDist);

    return TRUE;
}
/*****************************************************************************
    PngReadHuffTables:
    Read the dynamic huff tables
******************************************************************************/
PngReadHuffTables(PTERWND w,struct StrPng far *png, struct StrPngHuffTbl far *pHuffLit, struct StrPngHuffTbl far *pHuffDist)
{
    struct StrPngHuffTbl HuffCodeLen;
    int i;
    LPINT pLen;
    int  CodeLen,repeat,LenCount,CodeLenCount;
    int  CodeLenIdx[19]={16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};  // order in which the CodeLen length are stored

    FarMemSet(&HuffCodeLen,0,sizeof(struct StrPngHuffTbl));

    pHuffLit->count=PngGetBits(w,png,5)+257;      // number of literal codes used
    pHuffDist->count=PngGetBits(w,png,5)+1;      // number of distance codes used
    CodeLenCount=PngGetBits(w,png,4)+4;    // number of CodeLen codes used

    // build the CodeLen huffman table - this table is used to decode the code lengths for literal and distance huffman codes
    HuffCodeLen.count=19;     // this table is always 19 long
    PngAllocHuffTbl(w,png,&HuffCodeLen,HuffCodeLen.count);   // allocate the table


    // get the code lengths
    for (i=0;i<HuffCodeLen.count;i++) {
       HuffCodeLen.pCodeLen[i]=0;     // initialize the entires
       HuffCodeLen.pSymbol[i]=i;
    }
    for (i=0;i<CodeLenCount;i++) HuffCodeLen.pCodeLen[CodeLenIdx[i]]=PngGetBits(w,png,3);  // may be less than 19

    PngCreateHuffCode(w,png,&HuffCodeLen);


    // allocate the literal and distance huffman tables
    PngAllocHuffTbl(w,png,pHuffLit,pHuffLit->count);   // allocate the table
    PngAllocHuffTbl(w,png,pHuffDist,pHuffDist->count);   // allocate the table

    for (i=0;i<pHuffLit->count;i++) pHuffLit->pSymbol[i]=i;
    for (i=0;i<pHuffDist->count;i++) pHuffDist->pSymbol[i]=i;

    // extract the code lenths
    LenCount=pHuffLit->count+pHuffDist->count;
    pLen=OurAlloc(sizeof(int)*(LenCount+1));

    for (i=0;i<LenCount;i++) {
       CodeLen=PngExtractHuffSym(w,png,&HuffCodeLen);
       if (CodeLen<0) return FALSE;

       if (CodeLen<16) pLen[i]=CodeLen;
       else if (CodeLen==16) {              // repeat previous codes
          repeat=PngGetBits(w,png,2)+3;     // next two bits gives the repeat count
          while(repeat>0 && i<LenCount) {
             pLen[i]=pLen[i-1];
             repeat--;
             i++;
          }
          i--;
       }
       else if (CodeLen==17 || CodeLen==18) { // repeat zeros
          if (CodeLen==17) repeat=PngGetBits(w,png,3)+3;     // next 3 bits gives the repeat count
          else             repeat=PngGetBits(w,png,7)+11;    // next 7 bits gives the repeat count
          while(repeat>0 && i<LenCount) {
             pLen[i]=0;
             repeat--;
             i++;
          }
          i--;
       }
    }

    // copy the lengths to the huffman tables
    FarMove(pLen,pHuffLit->pCodeLen,pHuffLit->count*sizeof(int));
    FarMove(&(pLen[pHuffLit->count]),pHuffDist->pCodeLen,pHuffDist->count*sizeof(int));
    OurFree(pLen);

    // create the huffman codes for the literal and distance codes
    PngCreateHuffCode(w,png,pHuffLit);
    PngCreateHuffCode(w,png,pHuffDist);

    PngFreeHuffTbl(w,png,&HuffCodeLen);

    return TRUE;
}

/*****************************************************************************
    PngExtractHuffSym:
    Extract a symbol using the given huffman table.
******************************************************************************/
PngExtractHuffSym(PTERWND w,struct StrPng far *png, struct StrPngHuffTbl far *pHuff)
{
    int  i,code=0,BitsRead=0;

    for (i=pHuff->FirstCode;i<pHuff->count;i++) {
       if (pHuff->pCodeLen[i]!=BitsRead) {
          if (BitsRead>0) PngPushBits(w,png,BitsRead);    // return the preivous bits back to the stream
          code=PngGetBits(w,png,pHuff->pCodeLen[i]);
          BitsRead=pHuff->pCodeLen[i];
       }
       if (code==pHuff->pCode[i]) return pHuff->pSymbol[i];
    }

    PrintError(w,MSG_BAD_PNG_DATA,"PngExtractHuffSym");

    return -1;
}

/*****************************************************************************
    PngCreateHuffCodes:
    Create the huff codes.
******************************************************************************/
PngCreateHuffCode(PTERWND w,struct StrPng far *png, struct StrPngHuffTbl far *pHuff)
{
   int MaxBits=20;         // use a large number
   int CountByBitLen[21],i,j,NextCode[21];   // cound stores the number of codes for each bit length
   int code,len;
   int TempCode,TempCodeLen,TempSymbol;

   for (i=0;i<MaxBits;i++) CountByBitLen[i]=0;

   // find the number of codes for each code len
   for (i=0;i<pHuff->count;i++) CountByBitLen[pHuff->pCodeLen[i]]++;
   CountByBitLen[0]=0;

   // find the smallest code for each bit len
   code=0;
   for (i=1;i<=MaxBits;i++) {
      code=(code+CountByBitLen[i-1])<<1;
      NextCode[i]=code;
   }

   // sort the huffman table by symbol value
   for (i=0;i<pHuff->count;i++) {
      for (j=i;j<pHuff->count;j++) {
         if (pHuff->pSymbol[j]<pHuff->pSymbol[i]) {
            TempCode=pHuff->pCode[i];
            TempCodeLen=pHuff->pCodeLen[i];
            TempSymbol=pHuff->pSymbol[i];

            pHuff->pCode[i]=pHuff->pCode[j];
            pHuff->pCodeLen[i]=pHuff->pCodeLen[j];
            pHuff->pSymbol[i]=pHuff->pSymbol[j];

            pHuff->pCode[j]=TempCode;
            pHuff->pCodeLen[j]=TempCodeLen;
            pHuff->pSymbol[j]=TempSymbol;
         }
      }
   }

   // assign codes for each symbol
   for (i=0;i<pHuff->count;i++) {
      len=pHuff->pCodeLen[i];         // code length
      if (len>0) {
         pHuff->pCode[i]=NextCode[len];
         NextCode[len]++;
      }
   }


   // reverse the huffman codes
   for (i=0;i<pHuff->count;i++) pHuff->pCode[i]=PngReverseBits(w,png,pHuff->pCode[i],pHuff->pCodeLen[i]);

   if (!png->input) return TRUE;   // in output mode don't sort by code length

   // sort the huffman table by code length
   for (i=0;i<pHuff->count;i++) {
      for (j=i;j<pHuff->count;j++) {
         if (pHuff->pCodeLen[j]<pHuff->pCodeLen[i]) {
            TempCode=pHuff->pCode[i];
            TempCodeLen=pHuff->pCodeLen[i];
            TempSymbol=pHuff->pSymbol[i];

            pHuff->pCode[i]=pHuff->pCode[j];
            pHuff->pCodeLen[i]=pHuff->pCodeLen[j];
            pHuff->pSymbol[i]=pHuff->pSymbol[j];

            pHuff->pCode[j]=TempCode;
            pHuff->pCodeLen[j]=TempCodeLen;
            pHuff->pSymbol[j]=TempSymbol;
         }
      }
   }

   // find the first non-zero length code
   pHuff->FirstCode=0;
   for (i=0;i<pHuff->count;i++) {
      if (pHuff->pCodeLen[i]!=0) {
         pHuff->FirstCode=i;
         break;
      }
   }

   return TRUE;
}

/*****************************************************************************
    PngAllocHuffTbl:
    Alloated the memory associated with the specified huffman tabl
******************************************************************************/
PngAllocHuffTbl(PTERWND w,struct StrPng far *png, struct StrPngHuffTbl far *pHuff, int count)
{
    PngFreeHuffTbl(w,png,pHuff);   // free the previous table

    pHuff->count=count;
    pHuff->pSymbol=OurAlloc(sizeof(int)*(count+1));
    pHuff->pCode=OurAlloc(sizeof(int)*(count+1));
    pHuff->pCodeLen=OurAlloc(sizeof(int)*(count+1));

    return TRUE;
}

/*****************************************************************************
    PngFreeHuffTbl:
    Free the memory associated with the specified huffman tabl
******************************************************************************/
PngFreeHuffTbl(PTERWND w,struct StrPng far *png, struct StrPngHuffTbl far *pHuff)
{

    if (pHuff->pSymbol) OurFree(pHuff->pSymbol);
    if (pHuff->pCode) OurFree(pHuff->pCode);
    if (pHuff->pCodeLen) OurFree(pHuff->pCodeLen);

    FarMemSet(pHuff,0,sizeof(struct StrPngHuffTbl));

    return TRUE;
}

/*****************************************************************************
    PngReverseBits:
    Reverse the order of the bits
******************************************************************************/
int PngReverseBits(PTERWND w,struct StrPng far *png, int in, int count)
{
    int i;
    DWORD mask=1,temp,val=in,result=0;

    for (i=0;i<count;i++) {
        temp=val&0x1;
        val>>=1;    // shift left
        result<<=1;
        result|=temp;
    }

    return (int) result;
}

/*****************************************************************************
    PngGetBits:
    Retrun specified number of bits as an integer form the data stream.
******************************************************************************/
int PngGetBits(PTERWND w,struct StrPng far *png, int count)
{
    long CurPos=png->DataPos;
    int CurBitPos=png->DataBitPos,ByteCount;
    int i;
    DWORD mask;
    DWORD result=0;
    BYTE huge *pByte;

    if (count<=0) return 0;

    // get data
    if (count==1)      ByteCount=1;    // number of bytes the bits might span
    else if (count<=8) ByteCount=2;
    else               ByteCount=3;
    pByte=&(png->pData[CurPos+ByteCount-1]);    // pointer to the highest byte
    for (i=0;i<ByteCount;i++) {
       result<<=8;
       result|=(*pByte);
       pByte--;                 // point to the next lower byte
    }

    if (CurBitPos>0) result=result>>CurBitPos;       // discard the spent bits
    mask=(((DWORD)1)<<count)-1;                      // mask for count number of bits
    result&=mask;

    // advance the byte/bit pointer
    CurBitPos+=count;
    while (CurBitPos>=8) {
       CurPos++;
       CurBitPos-=8;
    }

    png->DataPos=CurPos;
    png->DataBitPos=CurBitPos;

    return (int)result;
}

/*****************************************************************************
    PngPushBits:
    Push specified number of bits into the stream.
******************************************************************************/
int PngPushBits(PTERWND w,struct StrPng far *png, int count)
{
    long CurPos=png->DataPos;
    int CurBitPos=png->DataBitPos;

    CurBitPos-=count;

    while (CurBitPos<0) {
       CurPos--;
       CurBitPos+=8;
    }

    png->DataPos=CurPos;
    png->DataBitPos=CurBitPos;

    return TRUE;
}

/*****************************************************************************
    PngReadBknd:
    Read the background color information.
******************************************************************************/
BOOL PngReadBknd(PTERWND w,struct StrPng far *png, DWORD ChunkSize)
{
    DWORD ByteCount=0;
    BYTE red=255,green=255,blue=255;
    BYTE hi,lo;

    if (png->ColorType==PNGCLR_PAL) {
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;

       if (png->pColor) {
          red=png->pColor[png->CurChar].rgbRed;
          green=png->pColor[png->CurChar].rgbGreen;
          blue=png->pColor[png->CurChar].rgbBlue;
       }
    }
    else if (png->ColorType==PNGCLR_GRAY || png->ColorType==PNGCLR_GRAY_ALPHA) {
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       hi=png->CurChar;

       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       lo=png->CurChar;

       if (png->BitDepth<=8) red=green=blue=lo;   // assign the color from the low byte
       else                  red=green=blue=hi;   // assign the color from the low byte
    }
    else if (png->ColorType==PNGCLR_TRUE || png->ColorType==PNGCLR_TRUE_ALPHA) {
       // get the red component
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       hi=png->CurChar;

       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       lo=png->CurChar;

       if (png->BitDepth<=8) red=lo;   // assign the color from the low byte
       else                  red=hi;   // assign the color from the low byte

       // get the green component
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       hi=png->CurChar;

       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       lo=png->CurChar;

       if (png->BitDepth<=8) green=lo;   // assign the color from the low byte
       else                  green=hi;   // assign the color from the low byte

       // get the blue component
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       hi=png->CurChar;

       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       lo=png->CurChar;

       if (png->BitDepth<=8) blue=lo;   // assign the color from the low byte
       else                  blue=hi;   // assign the color from the low byte
    }

    if (png->AlphaCount==0) {               // 07/11/2002 - background with alpha adjustment not supported
      png->BkndColor[0]=red;
      png->BkndColor[1]=green;
      png->BkndColor[2]=blue;
    }

    if (ByteCount<ChunkSize) PngGetBytes(w,png,NULL,ChunkSize-ByteCount);    // discard the additional bytes

    return TRUE;
}

/*****************************************************************************
    PngReadXparent:
    Read the transparent color information.
******************************************************************************/
BOOL PngReadXparent(PTERWND w,struct StrPng far *png, DWORD ChunkSize)
{
    DWORD ByteCount=0;
    BYTE red=255,green=255,blue=255;
    BYTE hi,lo;

    if (png->ColorType==PNGCLR_PAL) {
       while (ByteCount<ChunkSize) {
          if (!PngGetChar(w,png)) return FALSE;
          ByteCount++;

          if (png->AlphaCount<256) {
             png->alpha[png->AlphaCount]=png->CurChar;
             png->AlphaCount++;
          }
       }
    }
    else if (png->ColorType==PNGCLR_GRAY) {
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       hi=png->CurChar;

       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       lo=png->CurChar;

       if (png->BitDepth<=8) red=green=blue=lo;   // assign the color from the low byte
       else                  red=green=blue=hi;   // assign the color from the low byte

       png->xparent=TRUE;           // transparency using tRNS chunk
    }
    else if (png->ColorType==PNGCLR_TRUE) {
       // get the red component
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       hi=png->CurChar;

       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       lo=png->CurChar;

       if (png->BitDepth<=8) red=lo;   // assign the color from the low byte
       else                  red=hi;   // assign the color from the low byte

       // get the green component
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       hi=png->CurChar;

       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       lo=png->CurChar;

       if (png->BitDepth<=8) green=lo;   // assign the color from the low byte
       else                  green=hi;   // assign the color from the low byte

       // get the blue component
       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       hi=png->CurChar;

       if (!PngGetChar(w,png)) return FALSE;
       ByteCount++;
       lo=png->CurChar;

       if (png->BitDepth<=8) blue=lo;   // assign the color from the low byte
       else                  blue=hi;   // assign the color from the low byte

       png->xparent=TRUE;           // transparency using tRNS chunk
    }

    png->XparentColor[0]=red;
    png->XparentColor[1]=green;
    png->XparentColor[2]=blue;

    if (ByteCount<ChunkSize) PngGetBytes(w,png,NULL,ChunkSize-ByteCount);    // discard the additional bytes

    return TRUE;
}

/*****************************************************************************
    PngApplyPalAlpha:
    Apply alpha factor on the palette
******************************************************************************/
BOOL PngApplyPalAlpha(PTERWND w,struct StrPng far *png)
{
    int i;
    WORD alpha;

    if (png->pColor==NULL) return TRUE;

    for (i=0;i<png->AlphaCount;i++) {
        alpha=png->alpha[i];
        if (alpha==0) {
            png->pColor[i].rgbRed=png->BkndColor[0];
            png->pColor[i].rgbGreen=png->BkndColor[1];
            png->pColor[i].rgbBlue=png->BkndColor[2];
        }
        else {
            png->pColor[i].rgbRed=(BYTE)((alpha*png->pColor[i].rgbRed+(WORD)(255-alpha)*png->BkndColor[0])>>8);
            png->pColor[i].rgbGreen=(BYTE)((alpha*png->pColor[i].rgbGreen+(WORD)(255-alpha)*png->BkndColor[1])>>8);
            png->pColor[i].rgbBlue=(BYTE)((alpha*png->pColor[i].rgbBlue+(WORD)(255-alpha)*png->BkndColor[2])>>8);
        }
    }

    return TRUE;
}

/*****************************************************************************
    PngReadPalette:
    Read the palette chunk.
******************************************************************************/
BOOL PngReadPalette(PTERWND w,struct StrPng far *png, DWORD ChunkSize)
{
    DWORD ByteCount=0;
    int   ColorCount=0,step,clr,LastColor;
    COLORREF BkColor;

    if (ChunkSize==0) {             // colors to be created
       step=256/(png->ClrTblSize-1);
       if (png->BitDepth==2) step=step*2;    // bitdepth 2 had been increased to 4 bits
       png->CurChar=clr=0;
       LastColor=(1<<png->BitDepth)-1;
    }

    while ((ByteCount<ChunkSize || ChunkSize==0) && ColorCount<png->ClrTblSize) {
        if (ChunkSize>0 && !PngGetChar(w,png)) return FALSE;
        png->pColor[ColorCount].rgbRed=png->CurChar;

        if (ChunkSize>0 && !PngGetChar(w,png)) return FALSE;
        png->pColor[ColorCount].rgbGreen=png->CurChar;

        if (ChunkSize>0 && !PngGetChar(w,png)) return FALSE;
        png->pColor[ColorCount].rgbBlue=png->CurChar;

        png->pColor[ColorCount].rgbReserved=0;

        ColorCount++;
        ByteCount=ByteCount+3;                 // three bytes read
        if (ChunkSize==0) {
           clr+=step;
           if (clr>255) clr=255;
           png->CurChar=(BYTE)(WORD)clr;
           if (ColorCount==LastColor) png->CurChar=255;
        }
    }

    if (ByteCount<ChunkSize) PngGetBytes(w,png,NULL,ChunkSize-ByteCount);    // discard the additional bytes

    if (ColorCount<png->ClrTblSize) {   // initialize remaining colors
       while (ColorCount<png->ClrTblSize) {
          png->pColor[ColorCount].rgbRed=0; 
          png->pColor[ColorCount].rgbGreen=0;
          png->pColor[ColorCount].rgbBlue=0;
          png->pColor[ColorCount].rgbReserved=0;
          ColorCount++;
       }
    }

    // set the background color when alpha channel is present
    if (ChunkSize==0 && png->ReadAlpha) {  // set the background color
       BkColor=PageColor(w);
       png->pColor[png->BkndIdx].rgbRed=GetRValue(BkColor);
       png->pColor[png->BkndIdx].rgbGreen=GetGValue(BkColor);
       png->pColor[png->BkndIdx].rgbBlue=GetBValue(BkColor);
    }

    png->PalUsed=TRUE;            // palette used for this image

    return TRUE;
}


/*****************************************************************************
    PngData:
    This item receives the output data from the LZW decompressor
******************************************************************************/
BOOL PngData(PTERWND w, struct StrPng far *png, BYTE chr)
{
    int y,PixCount=0;

    if (png->NewScanLine) {
       png->FilterType=chr;
       png->CurFilterByte=0;        // current filter byte
       png->NewScanLine=FALSE;
       FarMove(png->pScan,png->pPrevScan,png->FilterBufSize);  // move the newly created lines

       y=png->CurY;                   // y relative to top
       y=png->height-y-1;             // y relative to bottom
       png->pImageScan=&(png->pImage[y*png->ScanLineSize]);
       return TRUE;
    }

    // set the current pixel
    if (png->interlaced && png->GridPass>7) return TRUE;  // this should not happen
    if (!png->interlaced && png->CurY>=(int)(png->height)) return TRUE;


    // apply the filter
    if (png->FilterType==1) {     // difference from the previous pixel
        int PrevPix=png->CurFilterByte-png->BytesPerPixelAlpha;
        if (PrevPix>=0) chr=chr+png->pScan[PrevPix];
    }
    else if (png->FilterType==2) {
        int PrevYByte=png->pPrevScan[png->CurFilterByte];   // save the previous row byte
        chr=chr+PrevYByte;
    }
    else if (png->FilterType==3) {                       // averaging
        int PrevPix=png->CurFilterByte-png->BytesPerPixelAlpha;
        BYTE PrevXByte=(PrevPix>=0?png->pScan[PrevPix]:0);
        BYTE PrevYByte=png->pPrevScan[png->CurFilterByte];
        chr=chr+((PrevXByte+PrevYByte)>>1);              // >>1 used to divide byte 2 (average)
    }
    else if (png->FilterType==4) {                       // paeth filtering
        int PrevPix=png->CurFilterByte-png->BytesPerPixelAlpha;
        int PrevXByte=(int)(WORD)(PrevPix>=0?png->pScan[PrevPix]:0);
        int PrevYByte=(int)(WORD)png->pPrevScan[png->CurFilterByte];
        int PrevXYByte=(int)(WORD)(PrevPix>=0?png->pPrevScan[PrevPix]:0);
        int  est,EstX,EstY,EstXY;                       // initial estimate

        // apply the PaethPredictor filter
        est=PrevXByte+PrevYByte-PrevXYByte;
        EstX=est-PrevXByte;
        EstY=est-PrevYByte;
        EstXY=est-PrevXYByte;
        if (EstX<0) EstX=-EstX;
        if (EstY<0) EstY=-EstY;
        if (EstXY<0) EstXY=-EstXY;
        if (EstX<=EstY && EstX<=EstXY) est=PrevXByte;
        else if (EstY<=EstXY)          est=PrevYByte;
        else                           est=PrevXYByte;
        chr=chr+(BYTE)(WORD)(est);
    }


    // store this byte in the filter buffer
    png->pScan[png->CurFilterByte]=chr;
    png->CurFilterByte++;


    SET_PIXEL:

    // set DIB value
    if (png->PixelsPerByte==1 && !png->ReadAlpha && !png->xparent) {   // simple case
       png->pImageScan[png->CurX]=(BYTE)chr;
    }
    else if (png->PixelsPerByte>1) {
        int PixelsPerByte=png->PixelsPerByte,BitsPerPixel,BitOffset;
        BYTE NewBits,CurByte,mask;
        int ImageX;
        if (PixelsPerByte==2) {
            ImageX=(png->CurX>>1);  // divide by 2
            BitOffset=png->CurX-(ImageX<<1);
            BitOffset=1-BitOffset;  // revere the stuffing order
            BitOffset<<=2;    // bit offset in the image byte
            BitsPerPixel=4;
        }
        else if (PixelsPerByte==4) {
            ImageX=(png->CurX>>1);  // divide by 2 (Windows does not allows 2 bits/color, treat it as 4 bits/color
            BitOffset=png->CurX-(ImageX<<1);
            BitOffset=1-BitOffset;  // revere the stuffing order
            BitOffset<<=2;          // bit offset in the image byte
            BitsPerPixel=2;
        }
        else if (PixelsPerByte==8) {
            ImageX=(png->CurX>>3);  // divide by 8
            BitOffset=png->CurX-(ImageX<<3);
            BitOffset=7-BitOffset;  // revere the stuffing order
            BitsPerPixel=1;
        }
        // create the mask
        mask=(1<<BitsPerPixel)-1;  // set the required number of 1s
        mask<<=BitOffset;          // set 1s in the current position

        // extract the new bits from the begining of the input
        CurByte=png->pImageScan[ImageX];   // current byte in the image
        CurByte=CurByte&(~mask);           // initialize the current bit positions to zero

        NewBits=chr;
        NewBits>>=(8-png->BitDepth);  // move to the left side
        if (png->xparent && NewBits==png->XparentColor[0]) NewBits=png->BkndColor[0];

        NewBits<<=BitOffset;     // move the new bits to the current offset position
        NewBits=NewBits&mask;    // set positions other than current positions to 0
        CurByte|=NewBits;        // merge new bits

        png->pImageScan[ImageX]=CurByte;   // merge the current byte into the image stream

        chr<<=BitsPerPixel;      // discard the used up bits
    }
    else {                       // multibyte pixels
        int ColorComponents=png->ColorComponents,i;
        BYTE val;

        if (png->AlphaPending) {      // process the alpha channel
            if (png->CurAlphaByte==0) {   // process only the high order alpha byte
                int x=(png->CurX<<1)+png->CurX;   // multiply by 3, x when RGB used

                for (i=0;i<ColorComponents;i++) {
                    if (chr==0 && png->PalUsed) val=png->BkndIdx;  // background color index
                    else {                                  // use prorated value
                        val=(BYTE)(((WORD)chr*png->PixColor[i]+(WORD)(255-chr)*png->BkndColor[i])>>8);
                        if (val==png->BkndIdx && png->PalUsed) val--;  // slot 128 used for the background color
                    }

                    if (ColorComponents==1) png->pImageScan[png->CurX]=val;
                    else if (ColorComponents==3) {
                        png->pImageScan[x+2-png->CurColor]=val;
                        x--;                               // position at the next color component x
                    }
                }
            }

            png->CurAlphaByte++;
            if (png->CurAlphaByte<png->BytesPerColor) return TRUE;  // read next alpha byte

            png->AlphaPending=FALSE;   // alpha channel read complete, go to next pixel

        }
        else {                         // process the pixel values
            if (png->CurColorByte==0) {// discard the lower byte for a 16 bit color component
                if (png->ReadAlpha) {
                    png->PixColor[png->CurColor]=chr;  // store it for later processing
                }
                else {
                    if (ColorComponents==1) {
                        if (png->xparent && chr==png->XparentColor[0]) chr=png->BkndColor[0];
                        png->pImageScan[png->CurX]=(BYTE)chr;
                    }
                    else if (ColorComponents==3) {
                        int x=(png->CurX<<1)+png->CurX;   // multiply by 3
                        png->pImageScan[x+2-png->CurColor]=(BYTE)chr;

                        if (png->xparent) {
                           png->PixColor[png->CurColor]=chr;   // save the color until the last color is retrieved

                           if ((png->CurColor+1)==ColorComponents) {  // check if we have a transparent color
                              if (  png->PixColor[0]==png->XparentColor[0]
                                 && png->PixColor[1]==png->XparentColor[1]
                                 && png->PixColor[2]==png->XparentColor[2]) {  // set background color for this pixel
                                 png->pImageScan[x]=png->BkndColor[2];    // blue
                                 png->pImageScan[x+1]=png->BkndColor[1];  // green
                                 png->pImageScan[x+2]=png->BkndColor[0];  // red
                              }
                           }
                        }
                    }
                }
            }

            png->CurColorByte++;
            if (png->CurColorByte>=png->BytesPerColor) {
               png->CurColorByte=0;  // start the next color component
               png->CurColor++;
            }

            if (png->CurColor<ColorComponents) return TRUE;

            // check if alpha channel needs to be read
            if (png->ReadAlpha) {
                png->AlphaPending=TRUE;
                png->CurAlphaByte=0;
                return TRUE;
            }
        }

        png->CurColorByte=png->CurColor=0;

    }


    // advance to next pixel
    if (png->interlaced) {
        png->CurX++;
        png->GridX++;
        if (png->GridX>=8) png->GridX=0;

        while (TRUE) {          // row loop
           while (png->grid[png->GridY][png->GridX]!=png->GridPass) {  // column loop
              png->GridX++;
              png->CurX++;
              if (png->GridX>=8) png->GridX=0;
              if (png->CurX>=(int)(png->width)) break;   // past the end of the scan line
           }

           if (png->CurX>=(int)(png->width)) {    // past the end of the scan line
              if ((png->CurY+1)>=(int)png->height) {  // on the last scan line
                 png->GridPass++;                   // go to the next interlace pass
                 if (png->GridPass>7) return TRUE;  // this should not happen
                 png->GridX=png->GridY=0;
                 png->CurX=png->CurY=0;           // restart from the top of the image
                 FarMemSet(png->pPrevScan,0,png->FilterBufSize);   // reset the filter buffer
                 FarMemSet(png->pScan,0,png->FilterBufSize);       // reset the filter buffer
              }
              else {                              // advance to the next grid line
                 png->GridY++;
                 png->CurY++;
                 if (png->GridY>=8) png->GridY=0;
                 png->GridX=0;
                 png->CurX=0;
              }
              png->NewScanLine=TRUE;              // new scan line
           }
           else break;
        }

    }
    else {
        png->CurX++;
        if (png->CurX>=(int)(png->width)) {
          png->CurX=0;
          png->CurY++;
          png->NewScanLine=TRUE;
        }
    }

    PixCount++;
    if (!png->NewScanLine && PixCount<png->PixelsPerByte) goto SET_PIXEL;   // set next pixel


    return TRUE;
}

/*****************************************************************************
    PngInitVars:
    Init variables and tables.
******************************************************************************/
BOOL PngInitVars(PTERWND w,struct StrPng far *png)
{
    int i,j,k;
    int pass[8][8]={1,6,4,6,2,6,4,6,
                    7,7,7,7,7,7,7,7,
                    5,6,5,6,5,6,5,6,
                    7,7,7,7,7,7,7,7,
                    3,6,4,6,3,6,4,6,
                    7,7,7,7,7,7,7,7,
                    5,6,5,6,5,6,5,6,
                    7,7,7,7,7,7,7,7};


    // Initialize the Run length base values and extra bits
    png->RunLenVal[0]=3;png->RunLenBits[0]=0;
    png->RunLenVal[1]=4;png->RunLenBits[1]=0;
    png->RunLenVal[2]=5;png->RunLenBits[2]=0;
    png->RunLenVal[3]=6;png->RunLenBits[3]=0;
    png->RunLenVal[4]=7;png->RunLenBits[4]=0;
    png->RunLenVal[5]=8;png->RunLenBits[5]=0;
    png->RunLenVal[6]=9;png->RunLenBits[6]=0;
    png->RunLenVal[7]=10;png->RunLenBits[7]=0;
    png->RunLenVal[8]=11;png->RunLenBits[8]=1;
    png->RunLenVal[9]=13;png->RunLenBits[9]=1;
    png->RunLenVal[10]=15;png->RunLenBits[10]=1;
    png->RunLenVal[11]=17;png->RunLenBits[11]=1;
    png->RunLenVal[12]=19;png->RunLenBits[12]=2;
    png->RunLenVal[13]=23;png->RunLenBits[13]=2;
    png->RunLenVal[14]=27;png->RunLenBits[14]=2;
    png->RunLenVal[15]=31;png->RunLenBits[15]=2;
    png->RunLenVal[16]=35;png->RunLenBits[16]=3;
    png->RunLenVal[17]=43;png->RunLenBits[17]=3;
    png->RunLenVal[18]=51;png->RunLenBits[18]=3;
    png->RunLenVal[19]=59;png->RunLenBits[19]=3;
    png->RunLenVal[20]=67;png->RunLenBits[20]=4;
    png->RunLenVal[21]=83;png->RunLenBits[21]=4;
    png->RunLenVal[22]=99;png->RunLenBits[22]=4;
    png->RunLenVal[23]=115;png->RunLenBits[23]=4;
    png->RunLenVal[24]=131;png->RunLenBits[24]=5;
    png->RunLenVal[25]=163;png->RunLenBits[25]=5;
    png->RunLenVal[26]=195;png->RunLenBits[26]=5;
    png->RunLenVal[27]=227;png->RunLenBits[27]=5;
    png->RunLenVal[28]=258;png->RunLenBits[28]=0;


    // initialize the distance values and extra bits
    png->DistVal[0]=1;png->DistBits[0]=0;
    png->DistVal[1]=2;png->DistBits[1]=0;
    png->DistVal[2]=3;png->DistBits[2]=0;
    png->DistVal[3]=4;png->DistBits[3]=0;
    png->DistVal[4]=5;png->DistBits[4]=1;
    png->DistVal[5]=7;png->DistBits[5]=1;
    png->DistVal[6]=9;png->DistBits[6]=2;
    png->DistVal[7]=13;png->DistBits[7]=2;
    png->DistVal[8]=17;png->DistBits[8]=3;
    png->DistVal[9]=25;png->DistBits[9]=3;
    png->DistVal[10]=33;png->DistBits[10]=4;
    png->DistVal[11]=49;png->DistBits[11]=4;
    png->DistVal[12]=65;png->DistBits[12]=5;
    png->DistVal[13]=97;png->DistBits[13]=5;
    png->DistVal[14]=129;png->DistBits[14]=6;
    png->DistVal[15]=193;png->DistBits[15]=6;
    png->DistVal[16]=257;png->DistBits[16]=7;
    png->DistVal[17]=385;png->DistBits[17]=7;
    png->DistVal[18]=513;png->DistBits[18]=8;
    png->DistVal[19]=769;png->DistBits[19]=8;
    png->DistVal[20]=1025;png->DistBits[20]=9;
    png->DistVal[21]=1537;png->DistBits[21]=9;
    png->DistVal[22]=2049;png->DistBits[22]=10;
    png->DistVal[23]=3073;png->DistBits[23]=10;
    png->DistVal[24]=4097;png->DistBits[24]=11;
    png->DistVal[25]=6145;png->DistBits[25]=11;
    png->DistVal[26]=8193;png->DistBits[26]=12;
    png->DistVal[27]=12289;png->DistBits[27]=12;
    png->DistVal[28]=16385;png->DistBits[28]=13;
    png->DistVal[29]=24577;png->DistBits[29]=13;

    // populate the pass table
    for (i=0;i<8;i++) for (j=0;j<8;j++) png->grid[i][j]=pass[i][j];

    // initialize the crc table for output
    for (i=0;i<256;i++) {
       DWORD temp=i;
       for (k=0;k<8;k++) temp=(temp&1) ? (0xedb88320L ^ (temp>>1)) : (temp>>1);
       png->CrcTable[i]=temp;
    }    

    return TRUE;
}

/*****************************************************************************
    PngGetBytes:
    Reads the specified number of bytes from the source
******************************************************************************/
BOOL PngGetBytes(PTERWND w,struct StrPng far *png,void huge *dest,DWORD count)
{
    DWORD i;

    for (i=0;i<count;i++) {
       if (!PngGetChar(w,png)) return FALSE;  // get next byte
       if (dest) ((BYTE huge *)dest)[i]=png->CurChar;
    }

    return TRUE;
}

/*****************************************************************************
    PngXlateDword:
    Translate DWORD into intel format. Reverse the byte order.
******************************************************************************/
BOOL PngXlateDWord(LPLONG pIn)
{
    WORD word1,word2;
    BYTE byte1,byte2;
    DWORD in=(*pIn);

    word1=LOWORD(in);
    word2=HIWORD(in);

    byte1=LOBYTE(word1);
    byte2=HIBYTE(word1);
    word1=(((WORD)byte1)<<8)|byte2;       // swap bytes

    byte1=LOBYTE(word2);
    byte2=HIBYTE(word2);
    word2=(((WORD)byte1)<<8)|byte2;       // swap bytes

    (*pIn)=MAKELONG(word2,word1);

    return TRUE;
}

/*****************************************************************************
    PngGetWord:
    Reads the next 2 characters and combine them to return a word value.
******************************************************************************/
BOOL PngGetWord(PTERWND w,struct StrPng far *png)
{
    BYTE lo,hi;

    if (!PngGetChar(w,png)) return FALSE;    // get low byte
    lo=png->CurChar;

    if (!PngGetChar(w,png)) return FALSE;    // get high byte
    hi=png->CurChar;

    png->CurWord=(WORD)( ( ((WORD)hi) <<8) +lo);

    return TRUE;
}

/*****************************************************************************
    PngGetChar:
    Reads the next characters from the input file or the input buffer.
    When the InSteam is NULL, the characters are read from the buffer.
    Returns FALSE when no more characters available.
******************************************************************************/
BOOL PngGetChar(PTERWND w,struct StrPng far *png)
{
    // get the character from the character stack if available
    if (png->StackLen>0) {
       png->StackLen--;
       png->CurChar=png->stack[png->StackLen];
       png->FilePos++;       // advance file position
       return TRUE;
    }

    // read from the file
    png->eof=FALSE;
    if (png->hFile!=INVALID_HANDLE_VALUE) {
       if (png->TextIndex<png->TextLen) {  // read from the temporary buffer
          png->CurChar=png->text[png->TextIndex];
          (png->TextIndex)++;
          png->FilePos++;    // advance file position
          return TRUE;
       }

       // read more characters from the file
       png->TextLen=0;
       ReadFile(png->hFile,TempString,MAX_WIDTH,&((DWORD)(png->TextLen)),NULL);

       if (png->TextLen==0) {
          png->eof=TRUE;
          return FALSE;      // end of file
       }
       png->TextIndex=0;
       FarMove(TempString,png->text,png->TextLen);

       png->CurChar=png->text[png->TextIndex];
       (png->TextIndex)++;
       png->FilePos++;      // advance file position
       return TRUE;
    }
    else {              // read from the buffer
       if (png->BufLen>=0 && png->BufIndex>=png->BufLen) {
          png->eof=TRUE;
          return FALSE; // end of buffer
       }
       png->CurChar=png->buf[png->BufIndex];
       (png->BufIndex)++;
       png->FilePos++;  // advance file position
       return TRUE;
    }
}

/*****************************************************************************
    PngPushChar:
    Push the lastly read character onto stack.
******************************************************************************/
BOOL PngPushChar(PTERWND w,struct StrPng far *png)
{
    if (png->StackLen>=MAX_WIDTH) return PrintError(w,MSG_OUT_OF_CHAR_STACK,"PngPushChar");

    png->stack[png->StackLen]=png->CurChar;
    (png->StackLen)++;
    png->FilePos--;  // decrement file position
    return TRUE;
}

/*****************************************************************************
    TerIsSspAvail:
    Check if the ssp library is available
******************************************************************************/
BOOL WINAPI _export TerIsSspAvail()
{
    return IsSspAvail();
}
 
/*****************************************************************************
    IsSspAvail:
    Check if the ssp library is available
******************************************************************************/
BOOL IsSspAvail()
{
    UINT OldErrorMode;
    BOOL result=TRUE;
    
    if (SspSearched) return (BOOL)hSsp;
 
    // check if ssp library is available
    hSsp=NULL;
    SspSearched=TRUE;         // spelltime search complete

    // set the error mode to catch the 'file not found' error
    OldErrorMode=SetErrorMode(SEM_NOOPENFILEERRORBOX);

    // load the SpellTime product
    #if defined (WIN32)
       if ((hSsp=LoadLibrary("SSP32.DLL"))==0) result=FALSE;
    #else
       if (((WORD)(hSsp=LoadLibrary("SSP.DLL")))<32) result=FALSE;
    #endif

    SetErrorMode(OldErrorMode);    // reset the error mode

    if (!result) {
       hSsp=NULL;
       return FALSE;
    }

    // find the function pointers
    #if defined (WIN32)
       SspGif2Bmp=(LPVOID)GetProcAddress(hSsp,"SspGif2Bmp");
    #else
       SspGif2Bmp=(LPVOID)GetProcAddress(hSsp,"SSPGIF2BMP");
    #endif

    if (!SspGif2Bmp) {
       FreeLibrary(hSsp);
       hSsp=NULL;
    }

    return (BOOL)hSsp;
} 

/*****************************************************************************
    JPG file translation function
******************************************************************************/

/*****************************************************************************
    Jpg2Bmp:
    This routine translates input buffer or file in the TIF format to an
    output buffer in MS Windows DIB format.
******************************************************************************/
BOOL Jpg2Bmp(PTERWND w,int input, LPBYTE InFile,HGLOBAL hBuf,long BufLen, HGLOBAL far *hDIB, BOOL import)
{
    HANDLE hFile=INVALID_HANDLE_VALUE;
    struct StrJpg far *jpg=NULL;
    BYTE huge *InBuf;
    BYTE marker;
    int i;
    BOOL IsJpgFile=TRUE,ReadComplete=FALSE,ImageDataFound=FALSE;

    if (hDIB) (*hDIB)=NULL;

    // open the input file
    if (input==TER_FILE) {               // data in the file
       //lstrupr(w,InFile);
       rTrim(InFile);                   // trim any spaces on right
       if (lstrlen(InFile)==0) return FALSE;
       lstrcpy(TempString,InFile);      // make a copy in the data segment
       if ((hFile=CreateFile(TempString,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"Jpg2Bmp");
    }
    else {                                // lock the buffer
       if (NULL==(InBuf=GlobalLock(hBuf))) return PrintError(w,MSG_ERR_MEM_LOCK,"Jpg2Bmp");
    }

    // allocate space for the jpg info structure
    if (NULL==(jpg=(struct StrJpg far *)OurAlloc(sizeof(struct StrJpg)))) {
       if (input==TER_FILE) CloseHandle(hFile);
       return PrintError(w,MSG_OUT_OF_MEM,"Jpg2Bmp");
    }
    FarMemSet(jpg,0,sizeof(struct StrJpg));  // initialize with zeros
    jpg->w=w;                                // store our pointer

    if (input==TER_FILE) jpg->hFile=hFile;
    else {
       jpg->buf=InBuf;
       jpg->BufLen=BufLen;
       jpg->hFile=INVALID_HANDLE_VALUE;
    }

    // Read Start-of-Image
    if (!JpgGetMarker(w,jpg,&marker)) goto END_FILE;

    if (import && marker!=0xD8) return PrintError(w,MSG_BAD_JPG_FILE,"Jpg2Bmp");
    IsJpgFile=(marker==0xD8);
    if (!import) {
       ReadComplete=TRUE;
       goto END_FILE;
    }
    

    NEXT_MARKER:
    if (!JpgGetMarker(w,jpg,&marker)) goto END_FILE;   // get next marker

    if (marker==0xD9) {                    // end of file marker
        ReadComplete=TRUE;
        goto END_FILE;
    }
    else if (marker==0xE0) {               // read the application segment
       if (!JpgReadAppSeg(w,jpg)) goto END_FILE;
    }
    else if (marker==0xEE) {               // the marker used by adobe - read the application segment
       jpg->AdobeMarkerFound=TRUE;
       JpgSkipMarker(w,jpg);              // skip over the current marker
    }
    else if (marker==0xDB) {               // get the quantization table
       if (!JpgGetQTable(w,jpg)) goto END_FILE;
    }
    else if (marker==0xC0) {               // get the image information (SOF)
       if (!JpgGetImageInfo(w,jpg)) goto END_FILE;
       ImageDataFound=TRUE;
    }
    else if (marker==0xC1) goto END_FILE;  // Extended sequential DCT frame not supported
    else if (marker==0xC2) { // goto END_FILE;  // Progressive DCT frame not supported
       jpg->IsProgressive=TRUE;
       if (!JpgGetImageInfo(w,jpg)) goto END_FILE;
       ImageDataFound=TRUE;
    } 
    else if (marker==0xC3) goto END_FILE;  // Lossless DCT frame not supported
    else if (marker==0xC4) {               // Huffman table
       if (!JpgGetHuffTable(w,jpg)) goto END_FILE;
    }
    else if (marker==0xDD) {               // Restart interval
       if (!JpgGetWord(w,jpg)) goto END_FILE; // pass over the segment length
       if (!JpgGetWord(w,jpg)) goto END_FILE; // retrieve restart interval
       jpg->McuPerInterval=jpg->CurWord;
    }
    else if (marker==0xDA) {               // image data
       if (!JpgReadImage(w,jpg)) goto END_FILE;
       if (!jpg->IsProgressive) ReadComplete=TRUE;  // we got some data
    }
    else JpgSkipMarker(w,jpg);   // skip over the current marker

    goto NEXT_MARKER;


    END_FILE:

    if (ReadComplete && ImageDataFound) JpgSetPixels(w,jpg);    // apply reverse dct and calculate pixel values

    // pass the DIB handle
    GlobalUnlock(jpg->hDIB);
    if (hDIB==NULL || !IsJpgFile || !ReadComplete) {
       GlobalFree(jpg->hDIB);
       jpg->hDIB=NULL;
    }
    if (hDIB) (*hDIB)=jpg->hDIB;

    // close file and release buffers
    if (input==TER_FILE) CloseHandle(jpg->hFile);
    else                 GlobalUnlock(hBuf);

    // release memory
    OurFree(jpg->pHist);       // release the history buffer

    for (i=0;i<jpg->HuffTblCount;i++) {
       if (jpg->HuffTbl[i].pSymbol) OurFree(jpg->HuffTbl[i].pSymbol);
       if (jpg->HuffTbl[i].pSymLen) OurFree(jpg->HuffTbl[i].pSymLen);
       if (jpg->HuffTbl[i].pCode)   OurFree(jpg->HuffTbl[i].pCode);
    }
    OurFree(jpg);

    if (ReadComplete) {
       if (import) return (BOOL)(*hDIB);
       else        return IsJpgFile;
    }
    else return FALSE;
}

/*****************************************************************************
    JpgReadAppSeg:
    Read the application segment
******************************************************************************/
BOOL JpgReadAppSeg(PTERWND w,struct StrJpg far *jpg)
{
    int len;
    BYTE unit;
    BYTE jfif[6];

    if (!JpgGetWord(w,jpg)) return FALSE;
    len=jpg->CurWord-2;      // remaining length in the segment

    if (!JpgGetBytes(w,jpg,jfif,5)) return FALSE;
    len-=5;      
    jfif[4]=0;
    
    if (lstrcmpi(jfif,"JFIF")!=0) {
       JpgSkipBytes(w,jpg,(WORD)len);
       return TRUE;
    }   

    JpgGetBytes(w,jpg,NULL,2);       // skip the version uniformat
    len-=2; 

    JpgGetBytes(w,jpg,&unit,1);  // density unit 0=arbitrary, 1=pixel/inches, 2=pixles/cm
    len--;

    if (unit==0) {              // arbitraray unit
       JpgSkipBytes(w,jpg,(WORD)len);
       return TRUE;
    } 

    if (!JpgGetWord(w,jpg)) return FALSE; // retrieve x density
    len-=2;
    if (!HtmlMode && False(TerFlags6&TFLAG6_SHOW_PICT_IN_PIXEL_SIZE)) ImgDenX=(int)jpg->CurWord;

    if (!JpgGetWord(w,jpg)) return FALSE; // retrieve y density
    len-=2;
    if (!HtmlMode && False(TerFlags6&TFLAG6_SHOW_PICT_IN_PIXEL_SIZE)) ImgDenY=(int)jpg->CurWord;

    if (unit==2 && !HtmlMode && False(TerFlags6&TFLAG6_SHOW_PICT_IN_PIXEL_SIZE)) {              // convert cm to inchdes
       ImgDenX=MulDiv(ImgDenX,100,254);
       ImgDenY=MulDiv(ImgDenY,100,254);
    } 

    if (ImgDenX==0) ImgDenX=OrigScrResX;
    if (ImgDenY==0) ImgDenY=OrigScrResY;

    JpgSkipBytes(w,jpg,(WORD)len);       // skip to the end of the marker

    return TRUE;
}

/*****************************************************************************
    JpgGetQTable:
    Get the quantization table.
******************************************************************************/
BOOL JpgGetQTable(PTERWND w,struct StrJpg far *jpg)
{
    int len;
    BYTE PrecId;
    int  id;

    if (!JpgGetWord(w,jpg)) return FALSE;
    len=jpg->CurWord-2;      // remaining length in the segment

    // extract each table
    while (len>0) {
       if (!JpgGetChar(w,jpg)) return FALSE;
       len--;

       PrecId=jpg->CurChar;  // precision and id
       id=PrecId&0xF;        // low 4 bits are table id

       if (!JpgGetBytes(w,jpg,jpg->QTable[id],64)) return FALSE;
       len=len-64;
    }

    return TRUE;
}


/*****************************************************************************
    JpgGetImageInfo:
    Get the image information.
******************************************************************************/
BOOL JpgGetImageInfo(PTERWND w,struct StrJpg far *jpg)
{
    int i,x,y,ClrTblSize,MaxHorzSamp,MaxVertSamp,HorzBitsPerMcu,VertBitsPerMcu;
    int  HistSize;
    long BitmapSize;
    LPBITMAPINFOHEADER pInfo;
    LPBYTE pMem;

    if (!JpgGetWord(w,jpg)) return FALSE;  // skip over length

    if (!JpgGetChar(w,jpg)) return FALSE;  // skip over precision - always 8 bits for baseline jpeg - jpg->CurChar should be 8 after this read

    // get number of rows and columns in the image
    if (!JpgGetWord(w,jpg)) return FALSE;
    jpg->height=(int)jpg->CurWord;
    if (!JpgGetWord(w,jpg)) return FALSE;
    jpg->width=(int)jpg->CurWord;

    // get number of image components
    if (!JpgGetChar(w,jpg)) return FALSE;  // skip over precision - always 8 bits for baseline jpeg
    jpg->NumComps=(int)jpg->CurChar;

    // get information about each image component
    MaxHorzSamp=MaxVertSamp=0;
    jpg->BlocksPerMcu=0;
    for (i=0;i<jpg->NumComps;i++) {
       if (!JpgGetChar(w,jpg)) return FALSE;  // component id
       jpg->FrameCompId[i]=jpg->CurChar;           // id of the component in the frame header

       if (!JpgGetChar(w,jpg)) return FALSE;  // get samp factor
       jpg->HorzSamp[i]=(jpg->CurChar&0xF0)>>4;
       jpg->VertSamp[i]=jpg->CurChar&0xF;

       jpg->BlocksPerMcu+=(jpg->HorzSamp[i]*jpg->VertSamp[i]);

       if (jpg->HorzSamp[i]>MaxHorzSamp) MaxHorzSamp=jpg->HorzSamp[i];
       if (jpg->VertSamp[i]>MaxVertSamp) MaxVertSamp=jpg->VertSamp[i];

       if (!JpgGetChar(w,jpg)) return FALSE;  // get samp factor
       jpg->QTableId[i]=jpg->CurChar;
    }


    HorzBitsPerMcu=MaxHorzSamp*8;
    VertBitsPerMcu=MaxVertSamp*8;

    jpg->McuPerRow=(jpg->width+HorzBitsPerMcu-1)/HorzBitsPerMcu;
    jpg->McuPerCol=(jpg->height+VertBitsPerMcu/*-1*/)/VertBitsPerMcu;      // remove -1 to eliminate 'exceed max mcu' error
    jpg->RowsPerMcu=MaxVertSamp*8;
    jpg->ColsPerMcu=MaxHorzSamp*8;
    jpg->MaxMcu=jpg->McuPerRow*jpg->McuPerCol;
    

    // calculate pixels per sample
    for (i=0;i<jpg->NumComps;i++) {
       jpg->PixelsPerSampX[i]=MaxHorzSamp/jpg->HorzSamp[i];
       jpg->PixelsPerSampY[i]=MaxVertSamp/jpg->VertSamp[i];
       jpg->BlocksPerRow[i]=jpg->McuPerRow*jpg->HorzSamp[i];
    } 

    jpg->BytesPerMcu=jpg->BlocksPerMcu*64*sizeof(short int);
    HistSize=jpg->MaxMcu*jpg->BytesPerMcu+64*sizeof(short int); // add padding worth one block
    jpg->pHist=OurAlloc(HistSize);
    
    FarMemSet(jpg->pHist,0,HistSize);          // initialize the dct history buffer
    
    // set the pointer to each component data
    jpg->pComp[0]=jpg->pHist;                            // first component start
    for (i=0;i<(jpg->NumComps-1);i++) {
       jpg->pComp[i+1]=&(jpg->pComp[i][jpg->MaxMcu*jpg->HorzSamp[i]*jpg->VertSamp[i]*64*sizeof(short int)]);
    } 


    // create a blank image
    jpg->BitsPerPixel=24;      // 24 bit color image
    jpg->BytesPerPixel=3;      // store in rgb
    ClrTblSize=0;
    jpg->ScanLineSize=(jpg->width*jpg->BitsPerPixel+31)/32; // scan line width in number of DWORDS
    jpg->ScanLineSize*=4;  // scan line size in number of bytes

    // calculate the size of the bitmap
    BitmapSize=sizeof(BITMAPINFOHEADER)+(long)sizeof(RGBQUAD)*ClrTblSize+(long)jpg->ScanLineSize*jpg->height;

    if (  NULL==(jpg->hDIB=GlobalAlloc(GMEM_MOVEABLE,BitmapSize+1))
       || NULL==(pInfo=GlobalLock(jpg->hDIB)) ){
        return PrintError(w,MSG_OUT_OF_MEM,"JpgGetImageInfo");
    }

    // fill in the header variables
    FarMemSet(pInfo,0,sizeof(BITMAPINFOHEADER));
    pInfo->biSize=sizeof(BITMAPINFOHEADER);
    pInfo->biWidth=jpg->width;
    pInfo->biHeight=jpg->height;
    pInfo->biPlanes=1;
    pInfo->biBitCount=(WORD)jpg->BitsPerPixel;
    pInfo->biCompression=BI_RGB;       // no compression
    pInfo->biSizeImage=jpg->ScanLineSize*jpg->height;
    pInfo->biClrUsed=ClrTblSize;

    // set the pointer to the image area and initialize the image area
    jpg->pImage=(LPBYTE) & (((LPBYTE)pInfo)[(int)pInfo->biSize+sizeof(RGBQUAD)*ClrTblSize]); // pointer to the color table

    // initialize the image with white color
    for (y=0;y<jpg->height;y++) {
      pMem=&(jpg->pImage[(jpg->height-y-1)*jpg->ScanLineSize]);
      for (x=0;x<jpg->width;x++) {
         (*pMem)=0xFF;pMem++;
         (*pMem)=0xFF;pMem++;
         (*pMem)=0xFF;pMem++;
      } 
    } 
    return TRUE;
}

/*****************************************************************************
    JpgGetHuffTable:
    Get Huffman table
******************************************************************************/
BOOL JpgGetHuffTable(PTERWND w,struct StrJpg far *jpg)
{
    int i,j,id,CurSize,class,len,idx;
    int SymbolCount;
    struct StrJpgHuffTbl far *hm;
    WORD CurCode;
    BOOL IsDC;

    if (!JpgGetWord(w,jpg)) return FALSE;  // skip over length
    len=jpg->CurWord-2;

    while (len>0) {
       if (!JpgGetChar(w,jpg)) return FALSE;  // skip over precision - always 8 bits for baseline jpeg

       len--;

       class=jpg->CurChar>>4;  // table class
       if (class==0) IsDC=TRUE;
       else          IsDC=FALSE;

       id=jpg->CurChar&0xF;       // table id (0 or 1)

       // check if this id already exists
       for (i=0;i<jpg->HuffTblCount;i++) if (jpg->HuffTbl[i].IsDC==IsDC && jpg->HuffTbl[i].id==id) break;
       if (i<jpg->HuffTblCount) {
           if (jpg->HuffTbl[i].pSymbol) OurFree(jpg->HuffTbl[i].pSymbol);
           jpg->HuffTbl[i].pSymbol=NULL;
           if (jpg->HuffTbl[i].pSymLen) OurFree(jpg->HuffTbl[i].pSymLen);
           jpg->HuffTbl[i].pSymLen=NULL;
           if (jpg->HuffTbl[i].pCode)   OurFree(jpg->HuffTbl[i].pCode);
           jpg->HuffTbl[i].pCode=NULL;
       }
       else {
          if (jpg->HuffTblCount>=MAX_JPEG_HUFF_TBLS) return PrintError(w,MSG_ERR_JPG_HUFF,"JpgGetHuffTable");
          i=jpg->HuffTblCount;
          jpg->HuffTblCount++;
       }  
       hm=&(jpg->HuffTbl[i]);

       hm->id=id;
       hm->IsDC=IsDC;

       if (!JpgGetBytes(w,jpg,hm->CodeLen,16)) return FALSE;
       len-=16;

       SymbolCount=0;
       for (i=0;i<16;i++) SymbolCount=SymbolCount+hm->CodeLen[i];

       hm->pSymbol=OurAlloc(SymbolCount+1);  // huff symbols
       hm->pSymLen=OurAlloc(SymbolCount+1);  // huff symbol sizes
       hm->pCode=OurAlloc(sizeof(WORD)*(SymbolCount+1));  // huff codes

       if (!JpgGetBytes(w,jpg,hm->pSymbol,SymbolCount)) return FALSE;
       len=len-SymbolCount;

       // record the symbol sizes
       idx=0;
       for (i=0;i<16;i++) {
          for (j=0;j<hm->CodeLen[i];j++) {
             hm->pSymLen[idx]=i+1;
             idx++;
          }
       }
       hm->pSymLen[idx]=0;      // terminator length

       // build the code
       CurSize=hm->pSymLen[0];  // legth of the first symbol
       CurCode=0;
       idx=0;
       while (hm->pSymLen[idx]) {
          while (hm->pSymLen[idx]==CurSize) {   // increment code with the same size
             hm->pCode[idx]=CurCode;
             CurCode++;
             idx++;
          }
          CurCode=CurCode<<1;
          CurSize++;
       }

    }

    return TRUE;
}

/*****************************************************************************
    JpgReadImage:
    This function reads image data in the current scan segment
******************************************************************************/
BOOL JpgReadImage(PTERWND w,struct StrJpg far *jpg)
{
    int i,j,comp,CompId,CompCount,HmDCId,HmACId,McuCount=0;
    BYTE marker;

    if (!JpgGetWord(w,jpg)) return FALSE;  // skip over length

    if (!JpgGetChar(w,jpg)) return FALSE;  // read the number of component in this scan
    jpg->ScanCompCount=CompCount=jpg->CurChar;
    jpg->interleaved=(CompCount>1);

    for (comp=0;comp<CompCount;comp++) {
       if (!JpgGetChar(w,jpg)) return FALSE;   // component id, 1=Y, 2=Cb, 3=Cr
       for (j=0;j<jpg->NumComps;j++) if (jpg->FrameCompId[j]==jpg->CurChar) break;
       jpg->ScanCompId[comp]=CompId=j;         // index into component info storage area

       if (!JpgGetChar(w,jpg)) return FALSE;  // dc and ac huff table ids
       HmDCId=jpg->CurChar>>4;             // high 4 bits DC huffman table id
       HmACId=jpg->CurChar&0xF;            // low 4 bits AC huffman table id

       // get the Huffman table index for this component
       for (i=0;i<jpg->HuffTblCount;i++) {
          if (jpg->HuffTbl[i].IsDC && jpg->HuffTbl[i].id==HmDCId) jpg->HmDCId[CompId]=i;
          if (!(jpg->HuffTbl[i].IsDC) && jpg->HuffTbl[i].id==HmACId) jpg->HmACId[CompId]=i;
       }

       jpg->PrevDC[CompId]=0;                // initialize the previous DC value

       if (jpg->interleaved) {
           jpg->ScanBlocksPerMcu[CompId]=jpg->HorzSamp[CompId]*jpg->VertSamp[CompId];
       }
       else {
          int BlocksPerRow=(jpg->width+7)/8;
          int BlocksPerCol=(jpg->height+7)/8;
          if (jpg->PixelsPerSampX[CompId]==1) jpg->ScanMcuPerRow=BlocksPerRow;
          else {
             BlocksPerRow=(BlocksPerRow+jpg->PixelsPerSampX[CompId]-1)/jpg->PixelsPerSampX[CompId];
             jpg->ScanMcuPerRow=(BlocksPerRow*jpg->HorzSamp[CompId]);
          }
          if (jpg->PixelsPerSampY[CompId]==1) jpg->ScanMcuPerCol=BlocksPerCol;
          else {
             BlocksPerCol=(BlocksPerCol+jpg->PixelsPerSampY[CompId]-1)/jpg->PixelsPerSampY[CompId];
             jpg->ScanMcuPerCol=(BlocksPerCol*jpg->VertSamp[CompId]);
          }
          
          jpg->ScanMaxMcu=jpg->ScanMcuPerRow*jpg->ScanMcuPerCol;
          jpg->ScanBlocksPerMcu[CompId]=1;         // each mcu consists of one 8x8 block only
       }  
    }

    if (jpg->interleaved) {                    // calculate mcu parameter for an interleave case
       jpg->ScanMcuPerRow=jpg->McuPerRow;
       jpg->ScanMcuPerCol=jpg->McuPerCol;
       jpg->ScanMaxMcu=jpg->MaxMcu;
    } 

    // discard the next 3 bytes
    if (!JpgGetChar(w,jpg)) return FALSE;  // discard start of spectral sel
    jpg->FirstIdx=(jpg->IsProgressive?jpg->CurChar:0);  // starting index available in this scan
    if (!JpgGetChar(w,jpg)) return FALSE;  // discard end of spectral sel
    jpg->LastIdx=(jpg->IsProgressive?jpg->CurChar:63);  // starting index available in this scan
    if (!JpgGetChar(w,jpg)) return FALSE;  // discard hight/low bits of spectral sel
    if (jpg->IsProgressive) {
      jpg->ShiftFactor=(jpg->CurChar&0xF);   // number of bits to shift
      jpg->PrevShiftFactor=(jpg->CurChar>>4);// shift factor for the previous scan
    }
    else jpg->ShiftFactor=jpg->PrevShiftFactor=0;


    // image data begins now
    jpg->BitIdx=8*MAX_JPEG_IN_BUF;         // first available bit (none available as yet)
    jpg->EobCount=0;                       // number of End of block pending
    McuCount=0;                            // MCUs since the last restart
    jpg->CurMcu=0;

    while (TRUE) {
       if (jpg->CurMcu>(jpg->ScanMaxMcu)) return PrintError(w,MSG_ERR_JPG_MCU,"JpgReadImage");

       if (!JpgReadMcu(w,jpg)) return FALSE;

       McuCount++;                         // number of Mcus within current restart segment
       jpg->CurMcu++;

       // check if a marker found while scanning
       if (jpg->MarkerInScan) {
          marker=jpg->MarkerInScan;
          
          if (marker>=0xD0 && marker<=0xD7) {  // found restart interval
             if (jpg->McuPerInterval==0/* || McuCount!=jpg->McuPerInterval*/) {
                return PrintError(w,MSG_ERR_JPG_MARKER,"JpgReadImage a");
             }
             else {                            // start next interval
                if (!JpgGetMarker(w,jpg,&marker)) return FALSE;  // discard the marker

                // check if it was the last restart marker
                if (!JpgGetChar(w,jpg)) return FALSE;  // get marker prefix
                if (jpg->CurChar==0xFF) {
                   if (!JpgGetChar(w,jpg)) return FALSE;
                   if (jpg->CurChar==0 || jpg->CurChar==0xFF) { // not a real marker
                      JpgPushChar(w,jpg);
                      jpg->CurChar=0xFF;
                      JpgPushChar(w,jpg);
                   }
                   else return TRUE;  // end of the segment
                }
                else JpgPushChar(w,jpg);           // marker prefix not found

                if (McuCount<jpg->McuPerInterval) {
                   for (i=0;i<(jpg->McuPerInterval-McuCount);i++) JpgCopyMcu(w,jpg,jpg->CurMcu+i-1,jpg->CurMcu+i);   // copy mcu data
                   jpg->CurMcu+=(jpg->McuPerInterval-McuCount);  // skip over unavailable mcus
                }
                McuCount=0;                        // restart the mcu counter
                jpg->EobCount=0;                   // reset eob pending count
                for (comp=0;comp<CompCount;comp++) jpg->PrevDC[comp]=0;                // initialize the previous DC value
                jpg->BitIdx=8*MAX_JPEG_IN_BUF;         // first available bit (none available as yet)
             }
          }
          else return TRUE;              // end of this segment
       }
       else {
          // was a marker expected but not found?
          if (McuCount==jpg->McuPerInterval && jpg->McuPerInterval>0) {
              return PrintError(w,MSG_ERR_JPG_MARKER,"JpgReadImage b");  // restart marker missing
          }
       }
    }

    return TRUE;
}

/*****************************************************************************
    JpgCopydMcu:
    Copy one mcu.
******************************************************************************/
BOOL JpgCopyMcu(PTERWND w,struct StrJpg far *jpg, int from, int to)
{
    int CompIdx,comp,HSamp,VSamp,HSampCount,VSampCount,SaveCurMcu;
    short int far *pFrom;
    short int far *pTo;

    SaveCurMcu=jpg->CurMcu;

    for (CompIdx=0;CompIdx<jpg->ScanCompCount;CompIdx++) {
       comp=jpg->ScanCompId[CompIdx];

       HSampCount=VSampCount=1;
       if (jpg->interleaved) {
          HSampCount=jpg->HorzSamp[comp];
          VSampCount=jpg->VertSamp[comp];
       } 

       for (VSamp=0;VSamp<VSampCount;VSamp++) {  // get each horizontal sample
          for (HSamp=0;HSamp<HSampCount;HSamp++) {    // get each horizontal sample
             jpg->CurMcu=from;
             pFrom=JpgGetBlockPtr(w,jpg,comp,VSamp,HSamp);
             jpg->CurMcu=to;
             pTo=JpgGetBlockPtr(w,jpg,comp,VSamp,HSamp);
             FarMove(pFrom,pTo,64*sizeof(short int));   // copy the block
          }
       }
    }

    jpg->CurMcu=SaveCurMcu;

    return TRUE;
}


/*****************************************************************************
    JpgReadMcu:
    Read one mcu.  One mcu usaully consists of 4 Y components and one Cb and
    one Cr component.
******************************************************************************/
BOOL JpgReadMcu(PTERWND w,struct StrJpg far *jpg)
{
    int CompIdx,comp,HSamp,VSamp,HSampCount,VSampCount;

    // process each component: 0=Y, 1=Cb, 2=Cr
    jpg->MarkerInScan=0;    // will be set to any marker found while scanning

    for (CompIdx=0;CompIdx<jpg->ScanCompCount;CompIdx++) {
       comp=jpg->ScanCompId[CompIdx];

       HSampCount=VSampCount=1;
       if (jpg->interleaved) {
          HSampCount=jpg->HorzSamp[comp];
          VSampCount=jpg->VertSamp[comp];
       } 

       for (VSamp=0;VSamp<VSampCount;VSamp++) {  // get each horizontal sample
          for (HSamp=0;HSamp<HSampCount;HSamp++) {    // get each horizontal sample
             jpg->pBlock=JpgGetBlockPtr(w,jpg,comp,VSamp,HSamp);
             if (!JpgReadSamp(w,jpg,comp)) return FALSE;
          }
       }
    }

    return TRUE;
}

/*****************************************************************************
    JpgReadSamp:
    Read one 8x8 Sample for the given component.
******************************************************************************/
BOOL JpgReadSamp(PTERWND w,struct StrJpg far *jpg, int comp)
{
    int NumBits,HmDCId,HmACId,NumZeros,idx=0;
    BYTE byte;
    struct StrJpgHuffTbl far *hm;
    int  CurVal,CurSym;
    short int far *pBlock=jpg->pBlock;   // history data for this block of 64 values
    int   PointTransform=(1<<jpg->ShiftFactor); // point transform factor


    HmDCId=jpg->HmDCId[comp];
    HmACId=jpg->HmACId[comp];

    if (jpg->FirstIdx==0) {   // dc component available in this scan
      if (jpg->PrevShiftFactor==0) {    // first dc scan for this component
         // get the DC value
         hm=&(jpg->HuffTbl[HmDCId]);      // Huffman table to use
         if (!JpgGetHmSymbol(w,jpg,hm)) return FALSE;  // get the bits in the dc component

         NumBits=jpg->CurSym;             // number of bits to store the DC value

         if (!JpgGetImageSymbol(w,jpg,NumBits,TRUE)) return FALSE;  // get a symbol of NumBits (8 bits max)

         CurVal=jpg->PrevDC[comp]=jpg->PrevDC[comp]+jpg->CurSym;
         if (PointTransform>1) pBlock[0]=(CurVal<<jpg->ShiftFactor); // *PointTransform);
         else                  pBlock[0]=CurVal;

      }
      else {                            // successive dc scan for this component
         if (!JpgGetImageSymbol(w,jpg,1,FALSE)) return FALSE;  // get one bit
         if (jpg->CurSym) {
            pBlock[0]|=(jpg->CurSym<<jpg->ShiftFactor);
         }
      } 
      idx=1;
    }
    else idx=jpg->FirstIdx;

    // extract the ac values
    hm=&(jpg->HuffTbl[HmACId]);      // Huffman table to use
    
    if (idx<=jpg->LastIdx && jpg->IsProgressive && jpg->EobCount>0) { // check if end-of-block avaialble
       // read the adjustment bits for non-zero history dct's
       if (jpg->PrevShiftFactor>0) idx=JpgGetLsbForNonZeroItems(w,jpg,idx,pBlock,jpg->LastIdx,PointTransform);
       jpg->EobCount--;
       return TRUE;
    } 

    while (idx<=jpg->LastIdx) {
       if (!JpgGetHmSymbol(w,jpg,hm)) return FALSE;  // number of zeros and bits in the following non-zero value


       byte=(BYTE)jpg->CurSym;

       NumZeros=(byte>>4); // number of zeros
       NumBits=byte&0xF;


       // when not in successive aproximation (jpg->PrevShiftFactor==0) then get signed 
       // value for the next symbol, otherwise get a unsigned value
       // When in successive approxmation, the symbol correspond to first non-zero value for
       // an item.
       if (NumBits>0) {
          if (!JpgGetImageSymbol(w,jpg,NumBits,(jpg->PrevShiftFactor==0))) return FALSE;  // get a symbol of NumBits (8 bits max)
          CurSym=jpg->CurSym;                  // save the current symbol

          // advance over the zero indexes - also read the next bit for previously non-zero items
          if (NumZeros>0 || jpg->PrevShiftFactor>0) idx=JpgSkipZeroItems(w,jpg,idx,pBlock,NumZeros,PointTransform);

          // store the value
          
          if (idx<=jpg->LastIdx) {     // protect program against a corrupted image data
             if (jpg->PrevShiftFactor>0) {   // successive scan - previous value of pBlock[idx] is zero
                if (CurSym) pBlock[idx]=(1<<jpg->ShiftFactor);        // 1 means postive
                else        pBlock[idx]=-(1<<jpg->ShiftFactor);       // 0 mean negative
             }
             else {                          // first scan
                if (PointTransform>1) pBlock[idx]=(CurSym*PointTransform);
                else                  pBlock[idx]=CurSym;
             }
          }

          idx++;
       }
       else {
          if (NumZeros==0xF) {
             idx=JpgSkip16Zeros(w,jpg,idx,pBlock,PointTransform);
          }
          else {
             if (jpg->IsProgressive) {  // get number of eob blocks
                if (NumZeros==0) jpg->EobCount=1;
                else {
                   NumBits=NumZeros;
                   jpg->EobCount=(1<<NumBits);
                   if (!JpgGetImageSymbol(w,jpg,NumBits,FALSE)) return FALSE;  // get a symbol of NumBits (8 bits max)
                   jpg->EobCount+=jpg->CurSym;
                }
             
                // skip to the end of the current block
                idx=JpgGetLsbForNonZeroItems(w,jpg,idx,pBlock,jpg->LastIdx,PointTransform);
                jpg->EobCount--;
             }
             break;
          }
       }
    }

    return TRUE;
} 

/*****************************************************************************
    JpgGetLsbForNonZeroItems:
    Read least-significant-bits for previously non-zero items
******************************************************************************/
int JpgGetLsbForNonZeroItems(PTERWND w,struct StrJpg far *jpg,int idx,short int far * pBlock,int LastIdx,int PointTransform)
{
 
    while(idx<=LastIdx) {              // read the least significan bits for preivous non-zero history items
       if (pBlock[idx]!=0) {
          if (!JpgGetImageSymbol(w,jpg,1,FALSE)) return FALSE;  // get a symbol of NumBits (8 bits max)
          if (jpg->CurSym>0) {
             if (pBlock[idx]>0) pBlock[idx]+=PointTransform;
             else               pBlock[idx]-=PointTransform;
          }
       }
       idx++;
    } 
 
    return idx;
}

/*****************************************************************************
    JpgSkipZeroItems:
    Skip over specified count of zero-value items. This fucntion also skips over any intervenning
    non-zero items including all non-zero items after the last zero item. Finally the 'idx' will
    be pointing to a zero history item.  This zero history item has data coming in this scan
******************************************************************************/
int JpgSkipZeroItems(PTERWND w,struct StrJpg far *jpg,int idx,short int far * pBlock,int NumZeros,int PointTransform)
{
    int i;

    if (jpg->PrevShiftFactor==0) idx+=NumZeros;
    else {
       for (i=0;i<=NumZeros;idx++) {
          //if (idx==64 || (i==NumZeros && pBlock[idx]==0)) {
          //    idx--;    // respsition after the last non-zero item
          //    break;
          //} 
          if (pBlock[idx]!=0) {     // previous value not zero, get the least-significant bit
             if (!JpgGetImageSymbol(w,jpg,1,FALSE)) return FALSE;  // get a symbol of NumBits (8 bits max)
             if (jpg->CurSym>0) {
                if (pBlock[idx]>0) pBlock[idx]+=PointTransform;
                else               pBlock[idx]-=PointTransform;
             }
          }
          else {                  // another zero-history found
             if (i==NumZeros) break;
             i++;
          }
       } 
    } 

    return idx;
}

/*****************************************************************************
    JpgSkip16Zeros:
    Skip over 16 of zero-value items. This fucntion also skips over any intervenning
    non-zero items. This zero history item has data coming in this scan
******************************************************************************/
int JpgSkip16Zeros(PTERWND w,struct StrJpg far *jpg,int idx,short int far * pBlock,int PointTransform)
{
    int i,NumZeros=16;

    if (jpg->PrevShiftFactor==0) idx+=NumZeros;
    else {
       for (i=0;i<NumZeros;idx++) {
          //if (idx==64 || (i==NumZeros && pBlock[idx]==0)) {
          //    idx--;    // respsition after the last non-zero item
          //    break;
          //} 
          if (pBlock[idx]!=0) {     // previous value not zero, get the least-significant bit
             if (!JpgGetImageSymbol(w,jpg,1,FALSE)) return FALSE;  // get a symbol of NumBits (8 bits max)
             if (jpg->CurSym>0) {
                if (pBlock[idx]>0) pBlock[idx]+=PointTransform;
                else               pBlock[idx]-=PointTransform;
             }
          }
          else {                  // another zero-history found
             i++;
          }
       } 
    } 

    return idx;
}

/*****************************************************************************
    JpgSetPixels:
    Calcualte reverse dct and the pixel values
******************************************************************************/
BOOL JpgSetPixels(PTERWND w,struct StrJpg far *jpg)
{
    int i,comp,x,y,Y,U,V,PixY;
    int red,green,blue,SampX;
    int cyan,magenta,yellow,black;
    int BlockCount;
    long PixOffset;
    BOOL NoU,NoV;
    short int far *pComp1;
    short int far *pComp2;
    short int far *pComp3;
    short int far *pComp4;
    int   PixPerSamp1=jpg->PixelsPerSampX[0];
    int   PixPerSamp2=jpg->PixelsPerSampX[1];
    int   PixPerSamp3=jpg->PixelsPerSampX[2];
    int   PixPerSamp4=jpg->PixelsPerSampX[3];   // use in CMYK scheme

    jpg->CurY=jpg->CurX=0;
    
    BlockCount=jpg->MaxMcu*jpg->BlocksPerMcu;

    for (comp=0;comp<jpg->NumComps;comp++) {
       BlockCount=jpg->MaxMcu*jpg->HorzSamp[comp]*jpg->VertSamp[comp];
       for (i=0;i<BlockCount;i++) {
          jpg->pBlock=(short int far *)&(jpg->pComp[comp][i*64*sizeof(short int)]);
          
          if (comp==0 && i==0) DlgInt1=99;
          if (!JpgReverseDct(w,jpg,comp)) return FALSE;
          DlgInt1=0;
       } 
    }
    
    // allocate memory for one width of image data
    BlockCount=(jpg->width+7)/8;
    pComp1=MemAlloc(BlockCount*8*sizeof(short int));   // each block is 8 element wide
    pComp2=MemAlloc(BlockCount*8*sizeof(short int));
    pComp3=MemAlloc(BlockCount*8*sizeof(short int));
    pComp4=MemAlloc(BlockCount*8*sizeof(short int));

    NoU=(jpg->NumComps<2);   // no u component
    NoV=(jpg->NumComps<3);   // no v component

    // stuff pixels
    for (y=0;y<jpg->height;y++) {
       JpgStuffWidth(w,jpg,0,y,pComp1);
       JpgStuffWidth(w,jpg,1,y,pComp2);
       JpgStuffWidth(w,jpg,2,y,pComp3);
       JpgStuffWidth(w,jpg,3,y,pComp4);          // the fourth component is used in the CYMK scheme - cyan,yellow,magenta, key(black)
    
       PixY=jpg->height-y-1;  // y relative to bottom
       PixOffset=(PixY*(long)jpg->ScanLineSize);
    
       for (x=0;x<jpg->width;x++) {
           // get the Y component
           if      (PixPerSamp1==1) SampX=x;
           else if (PixPerSamp1==2) SampX=x>>1;
           else if (PixPerSamp1==4) SampX=x>>2;
           else                     SampX=x/PixPerSamp1;

           Y=pComp1[SampX]+128;

           // get the U component
           if (NoU) U=0;
           else {
              if      (PixPerSamp2==1) SampX=x;
              else if (PixPerSamp2==2) SampX=x>>1;
              else if (PixPerSamp2==4) SampX=x>>2;
              else                     SampX=x/PixPerSamp2;

              U=pComp2[SampX];
           }
           
           // get the V component
           if (NoV) V=0;
           else {
              if      (PixPerSamp3==1) SampX=x;
              else if (PixPerSamp3==2) SampX=x>>1;
              else if (PixPerSamp3==4) SampX=x>>2;
              else                     SampX=x/PixPerSamp3;

              V=pComp3[SampX];
           }

           if (jpg->NumComps<=3) { // YCrCb color scheme used
              red  = (int) (Y + 1.402 *V);
              green= (int) (Y - .34414 *U - .71414 * V);
              blue = (int) (Y + 1.772 *U);

              // But we do calculation in integer mode - shifted 6 bits
              // the constants get multipled by 64 (2^6)
              //Y=Y<<6;
              //red  = (Y + 90 *V) >>6;
              //green= (Y - 22*U - 46*V) >>6;
              //blue = (Y + 113*U) >>6;
           }
           else {          // CMYK color scheme used
              cyan=Y;              // 128 already added to Y
              magenta=U+128;
              yellow=V+128;
    
              // get the black level
              if      (PixPerSamp4==1) SampX=x;
              else if (PixPerSamp4==2) SampX=x>>1;
              else if (PixPerSamp4==4) SampX=x>>2;
              else                     SampX=x/PixPerSamp4;

              black=pComp4[SampX]+128;

              if (cyan<0)   cyan=0;
              if (cyan>255) cyan=255;
              if (magenta<0)   magenta=0;
              if (magenta>255) magenta=255;
              if (yellow<0)   yellow=0;
              if (yellow>255) yellow=255;
              if (black<0)   black=0;
              if (black>255) black=255;

              if (jpg->AdobeMarkerFound) {
                 red=black*cyan/255;          // use this formula when Adobe marker is seen
                 green=black*magenta/255;
                 blue=black*yellow/255;
              }
              else {
                 red=(255-black)*(255-cyan)/255;    // use this formula when the adobe marker is not seen
                 green=(255-black)*(255-magenta)/255;
                 blue=(255-black)*(255-yellow)/255;
              }
           } 

           if (red<0) red=0;
           if (red>255) red=255;
           if (green<0) green=0;
           if (green>255) green=255;
           if (blue<0) blue=0;
           if (blue>255) blue=255;

           jpg->pImage[PixOffset]=blue;
           jpg->pImage[PixOffset+1]=green;
           jpg->pImage[PixOffset+2]=red;


           PixOffset=PixOffset+3;   // 3 bytes per pixel
       } 
    } 
    
    MemFree(pComp1);
    MemFree(pComp2);
    MemFree(pComp3);
    MemFree(pComp4);

    return TRUE;
}
 
/*****************************************************************************
    JpgReverseDct:
    Reverse dct for a given block of a component
******************************************************************************/
BOOL JpgReverseDct(PTERWND w,struct StrJpg far *jpg, int comp)
{
    int idx,qtbl,row,col,i,j,l,k;
    int  data[8][8];
    short int out[8][8];
    long SumL,SumK;
    int  LastRow=0,LastCol=0;
    short int far *pBlock=jpg->pBlock;   // history data for this block of 64 values
    BOOL DcOnly=TRUE;

    // translate block data in the zig-zag form to 8x8 matrix form
    qtbl=jpg->QTableId[comp];     // quantization table to use
    FarMemSet(data,0,sizeof(data)); // initialize with zeros
    for (idx=0;idx<64;idx++) {
       if (idx==0) {   // dc component available in this scan
         data[0][0]=pBlock[0]*jpg->QTable[qtbl][0];  // apply quantization factor
       }
       else if (pBlock[idx]) {
         row=ZRow[idx];    // row number in the 8x8 block
         col=ZCol[idx];    // column number in the 8x8 block
         data[row][col]=pBlock[idx]*(int)jpg->QTable[qtbl][idx];  // store after applying quantization factor
         if (row>LastRow) LastRow=row;
         if (col>LastCol) LastCol=col;
         DcOnly=FALSE;
       }
    }
    
    // apply revsere DCT
    for (i=0;i<8;i++) {
       for (j=0;j<8;j++) {
          if (DcOnly) SumK=JpgFactor[i][0]*JpgFactor[j][0]*data[0][0];
          else {
             SumK=0;
             for (k=LastRow;k>=0;k--) {       // add the rows
                SumL=0;
                for (l=LastCol;l>=0;l--) {     // add the columns
                   if (data[k][l]!=0) SumL+=(JpgFactor[j][l]*data[k][l]);
                }

                // add to the out sum
                if (SumL!=0) SumK+=(JpgFactor[i][k]*SumL);
             }
          }
          // the >>16 made of precisions built into the JpgFactor value - See InitJpgTables
          out[i][j]=(short int)(SumK>>(16));
       }
    }

    FarMove(out,pBlock,sizeof(out));
    
    return TRUE;
} 

/*****************************************************************************
    JpgStuffWidth:
    Get one width worth of component data
******************************************************************************/
JpgStuffWidth(PTERWND w,struct StrJpg far *jpg, int comp, int y, short int far *pOut)
{
    int BlockRow,BlockY,BlockIdx;
    int BlocksPerRow,width;
    short int far *pBlock;

    if (comp>=jpg->NumComps) {     
       FarMemSet(pOut,0,jpg->width*sizeof(short int));
       return TRUE;
    } 

    y=y/jpg->PixelsPerSampY[comp];

    BlockRow=y/8;   // block row number
    BlockY=y-BlockRow*8;   // y with the block
    BlockIdx=BlockY*8;     // index of the data within a block to get at

    BlocksPerRow=jpg->BlocksPerRow[comp];
    pBlock=(short int far *)&(jpg->pComp[comp][BlockRow*BlocksPerRow*64*sizeof(short int)]);  // pointer to the first block
    pBlock=&(pBlock[BlockIdx]);                    // data begin pos

    width=jpg->width;
    if (jpg->PixelsPerSampX[comp]>1) width=(jpg->width+jpg->PixelsPerSampX[comp]-1)/jpg->PixelsPerSampX[comp];
    
    while(width>0) {
        FarMove(pBlock,pOut,8*sizeof(short int));  // move 8 bytes

        width-=8;
        if (width<=0) break;

        pBlock=&(pBlock[64]);
        pOut=&(pOut[8]);
    } 
 
    return TRUE;
}
 
/*****************************************************************************
    JpgGetBlockPtr:
    Get the pointer for the history data for the given block within current mcu
******************************************************************************/
short int far *JpgGetBlockPtr(PTERWND w,struct StrJpg far *jpg, int comp, int VSamp, int HSamp)
{
    int   offset=0;
    int   row,col;

    row=jpg->CurMcu/jpg->ScanMcuPerRow;                 // mcu row
    col=jpg->CurMcu-(row*jpg->ScanMcuPerRow);

    if (jpg->interleaved) {
       row=row*jpg->VertSamp[comp];   // convert to block row
       row+=VSamp;               // row for the current block

       col=col*jpg->HorzSamp[comp];   // convert to block column
       col+=HSamp;               // column for the current block
    }

    offset=row*jpg->BlocksPerRow[comp]+col;

    return (short int far*)&(jpg->pComp[comp][offset*64*sizeof(short int)]);
} 

/*****************************************************************************
    JpgGetImageSymbol:
    Get a symbol from the image of specified number of bits.  The number of
    bits may not exceed 8.
******************************************************************************/
BOOL JpgGetImageSymbol(PTERWND w,struct StrJpg far *jpg, int NumBits, BOOL ApplySign)
{
    WORD   sym,InWord;
    DWORD  InDword;

    if (!JpgFillInBuf(w,jpg)) return FALSE;  // fill the input buffer
    
    // Extract NumBits
    if (NumBits<=8) {
       InWord=(WORD)(jpg->InDword>>16);
       InWord=InWord<<jpg->BitIdx;     // discard used bits
       InWord=InWord>>(16-NumBits);
       sym=InWord;
    }
    else {
       InDword=jpg->InDword;
       InDword=InDword<<jpg->BitIdx;     // discard used bits
       InDword=InDword>>(32-NumBits);
       sym=(WORD)InDword;
    }
    jpg->BitIdx+=NumBits;

    // apply 1's complement when high bit is 0
    if (ApplySign && (sym>>(NumBits-1))==0) {
      int temp=sym;
      temp = ((-1)<<NumBits)+1;  // -1 will be 0xFFFF for win16 and 0xFFFFFFFF for win32
      jpg->CurSym = (temp+sym);
    }
    else jpg->CurSym=sym;


    return TRUE;
}

/*****************************************************************************
    JpgGetHmSymbol:
    Extract (decompress) next Huffman symbol
******************************************************************************/
BOOL JpgGetHmSymbol(PTERWND w,struct StrJpg far *jpg, struct StrJpgHuffTbl far *hm)
{
    int len,NumCodes,code,SymIdx=0;
    WORD   CurCode=0xFFFF,MatchWord,CurWord;
    DWORD  InDword;

    if (!JpgFillInBuf(w,jpg)) return FALSE;  // fill the input buffer

    // extract first 16 bits from the input buffer (code len is only upto 16)
    InDword=jpg->InDword<<jpg->BitIdx;   // discard used bits
    MatchWord=(WORD)(InDword>>16);  // extract the first word

    for (len=1;len<=16;len++) {   // access each code len
       NumCodes=hm->CodeLen[len-1];   // number of NumCodes for this length
       if (NumCodes==0) continue;  // wait for the first non zero code length
       
       CurWord=MatchWord>>(16-len);   // code to look for

       for (code=0;code<NumCodes;code++) {
          CurCode=hm->pCode[SymIdx];

          // check if CurCode matches with the bits in the MatchWord
          if (CurWord==CurCode) {    // match found
             jpg->CurSym=hm->pSymbol[SymIdx];
             jpg->BitIdx+=len;       // account for used up bits
             return TRUE;
          }
          
          SymIdx++;                   // index into the huffman symbol table
       }
    }

    return PrintError(w,MSG_ERR_JPG_NO_HUFF,"JpgGetHmSymbol");
}

/*****************************************************************************
    JpgFillInBuf:
    Fill the input buffer. Max buffer size is 3 bytes.
******************************************************************************/
BOOL JpgFillInBuf(PTERWND w,struct StrJpg far *jpg)
{
    int ShiftBytes,i,ShiftBits;

    ShiftBytes=jpg->BitIdx>>3;              // div by 8, gives number of bytes to shift
    if (ShiftBytes==0) return TRUE;
    
    ShiftBits=ShiftBytes<<3;                // multiply byte 8
    jpg->BitIdx-=ShiftBits;

    // read bytes
    for (i=0;i<ShiftBytes;i++) {
      if (!JpgGetChar(w,jpg)) return FALSE;
      if (jpg->CurChar==0xFF) {
         while (jpg->CurChar==0xFF) if (!JpgGetChar(w,jpg)) return FALSE;
         if (jpg->CurChar==0) {             // discard this 0 and use the 0xff
            jpg->InDword|=0xFF;             // store new byte
            jpg->InDword<<=8;               // make space for the next byte
         }
         else {                             // beginning of the marker, push the characters back
            jpg->MarkerInScan=jpg->CurChar; // marker found while scanning

            JpgPushChar(w,jpg);
            jpg->CurChar=0xFF;
            JpgPushChar(w,jpg);
            while (i<ShiftBytes) {  // stuff with dummy zeros
              jpg->InDword<<=8;
              i++;
            }
         }
      }
      else {
         jpg->InDword|=jpg->CurChar;           // store new byte
         jpg->InDword<<=8;                     // make space for the next byte
      }
    }

    return TRUE;
}

/*****************************************************************************
    JpgSkipMarker:
    Skip over the current marker.
******************************************************************************/
BOOL JpgSkipMarker(PTERWND w,struct StrJpg far *jpg)
{
    WORD len;

    if (!JpgGetWord(w,jpg)) return FALSE;
    len=jpg->CurWord-2;  // subtract the length of this word
    
    return JpgSkipBytes(w,jpg,len);
}

/*****************************************************************************
    JpgSkipBytes:
    Skip over the the number of specified bytes.
******************************************************************************/
BOOL JpgSkipBytes(PTERWND w,struct StrJpg far *jpg,WORD len)
{
    // empty the input buffers
    jpg->StackLen=0;
    jpg->TextLen=0;
    jpg->TextIndex=0;

    jpg->FilePos=jpg->FilePos+len;   // new position

    // set the file pointer at the offset
    if (jpg->hFile!=INVALID_HANDLE_VALUE) {
       SetFilePointer(jpg->hFile,jpg->FilePos,0,FILE_BEGIN);
    }
    else jpg->BufIndex=jpg->BufIndex+len;

    return TRUE;
}

/*****************************************************************************
    JpgGetOffsetBytes:
    Reads the specified number of bytes from the source at the given file offset.
******************************************************************************/
BOOL JpgGetOffsetBytes(PTERWND w,struct StrJpg far *jpg,DWORD offset, LPVOID dest,long count)
{

    // empty the input buffers
    jpg->StackLen=0;
    jpg->TextLen=0;
    jpg->TextIndex=0;
    jpg->FilePos=offset;      // advance file position
    jpg->BufIndex=offset;

    // set the file pointer at the offset
    if (jpg->hFile!=INVALID_HANDLE_VALUE) {
       SetFilePointer(jpg->hFile,offset,0,FILE_BEGIN);
    }

    // read the bytes
    if (!JpgGetBytes(w,jpg,dest,count)) return FALSE;

    return TRUE;
}

/*****************************************************************************
    JpgGetMarker:
    Reads the next marker from the file.  If a marker is not found at the 
    current location, this function return FALSE.
******************************************************************************/
BOOL JpgGetMarker(PTERWND w,struct StrJpg far *jpg,LPBYTE marker)
{
    if (!JpgGetChar(w,jpg)) return FALSE;
    if (jpg->CurChar!=0xFF) return FALSE;   // not a marker character

    while (TRUE) {
      if (!JpgGetChar(w,jpg)) return FALSE;
      if (jpg->CurChar==0xFF) continue;
      (*marker)=jpg->CurChar;
      break;
    }

    return TRUE;
}

/*****************************************************************************
    JpgGetBytes:
    Reads the specified number of bytes from the source
******************************************************************************/
BOOL JpgGetBytes(PTERWND w,struct StrJpg far *jpg,LPVOID dest,long count)
{
    long l;

    for (l=0;l<count;l++) {
       if (!JpgGetChar(w,jpg)) return FALSE;  // get next byte
       if (dest) ((BYTE huge *)dest)[l]=jpg->CurChar;
    }

    return TRUE;
}

/*****************************************************************************
    JpgGetWord:
    Reads the next 2 characters and combine them to return a word value.
******************************************************************************/
BOOL JpgGetWord(PTERWND w,struct StrJpg far *jpg)
{
    BYTE lo,hi;

    if (!JpgGetChar(w,jpg)) return FALSE;    // get low byte
    hi=jpg->CurChar;

    if (!JpgGetChar(w,jpg)) return FALSE;    // get high byte
    lo=jpg->CurChar;
    
    jpg->CurWord=(WORD)( ( ((WORD)hi) <<8) +lo);

    return TRUE;
}

/*****************************************************************************
    JpgGetChar:
    Reads the next characters from the input file or the input buffer.
    When the InSteam is NULL, the characters are read from the buffer.
    Returns FALSE when no more characters available.
******************************************************************************/
BOOL JpgGetChar(PTERWND w,struct StrJpg far *jpg)
{
    // get the character from the character stack if available
    if (jpg->StackLen>0) {
       jpg->StackLen--;
       jpg->CurChar=jpg->stack[jpg->StackLen];
       jpg->FilePos++;       // advance file position
       return TRUE;
    }
    
    // read from the file
    jpg->eof=FALSE;
    if (jpg->hFile!=INVALID_HANDLE_VALUE) {
       if (jpg->TextIndex<jpg->TextLen) {  // read from the temporary buffer
          jpg->CurChar=jpg->text[jpg->TextIndex];
          (jpg->TextIndex)++;
          jpg->FilePos++;    // advance file position
          return TRUE;
       }

       // read more characters from the file
       jpg->TextLen=0;
       ReadFile(jpg->hFile,TempString,MAX_WIDTH,&((DWORD)(jpg->TextLen)),NULL);
    
       if (jpg->TextLen==0) {
          jpg->eof=TRUE;
          return FALSE;      // end of file
       }
       jpg->TextIndex=0;
       FarMove(TempString,jpg->text,jpg->TextLen);

       jpg->CurChar=jpg->text[jpg->TextIndex];
       (jpg->TextIndex)++;
       jpg->FilePos++;      // advance file position
       return TRUE;
    }
    else {              // read from the buffer
       if (jpg->BufLen>=0 && jpg->BufIndex>=jpg->BufLen) {
          jpg->eof=TRUE;
          return FALSE; // end of buffer
       }
       jpg->CurChar=jpg->buf[jpg->BufIndex];
       (jpg->BufIndex)++;
       jpg->FilePos++;  // advance file position
       return TRUE;
    }
}

/*****************************************************************************
    JpgPushChar:
    Push the lastly read character onto stack.
******************************************************************************/
BOOL JpgPushChar(PTERWND w,struct StrJpg far *jpg)
{
    if (jpg->StackLen>=MAX_WIDTH) return PrintError(w,MSG_OUT_OF_CHAR_STACK,"JpgPushChar");

    jpg->stack[jpg->StackLen]=jpg->CurChar;
    (jpg->StackLen)++;
    jpg->FilePos--;  // decrement file position
    return TRUE;
}

/*****************************************************************************
    InitJpgZTable:
    Initialize the JPEG zig-zag table
******************************************************************************/
InitJpgZTable()
{
    int i,j;


    ZRow[0]=0;ZCol[0]=0;
    ZRow[1]=0;ZCol[1]=1;
    ZRow[2]=1;ZCol[2]=0;
    ZRow[3]=2;ZCol[3]=0;
    ZRow[4]=1;ZCol[4]=1;
    ZRow[5]=0;ZCol[5]=2;
    ZRow[6]=0;ZCol[6]=3;
    ZRow[7]=1;ZCol[7]=2;
    ZRow[8]=2;ZCol[8]=1;
    ZRow[9]=3;ZCol[9]=0;
    ZRow[10]=4;ZCol[10]=0;
    ZRow[11]=3;ZCol[11]=1;
    ZRow[12]=2;ZCol[12]=2;
    ZRow[13]=1;ZCol[13]=3;
    ZRow[14]=0;ZCol[14]=4;
    ZRow[15]=0;ZCol[15]=5;
    ZRow[16]=1;ZCol[16]=4;
    ZRow[17]=2;ZCol[17]=3;
    ZRow[18]=3;ZCol[18]=2;
    ZRow[19]=4;ZCol[19]=1;
    ZRow[20]=5;ZCol[20]=0;
    ZRow[21]=6;ZCol[21]=0;
    ZRow[22]=5;ZCol[22]=1;
    ZRow[23]=4;ZCol[23]=2;
    ZRow[24]=3;ZCol[24]=3;
    ZRow[25]=2;ZCol[25]=4;
    ZRow[26]=1;ZCol[26]=5;
    ZRow[27]=0;ZCol[27]=6;
    ZRow[28]=0;ZCol[28]=7;
    ZRow[29]=1;ZCol[29]=6;
    ZRow[30]=2;ZCol[30]=5;
    ZRow[31]=3;ZCol[31]=4;
    ZRow[32]=4;ZCol[32]=3;
    ZRow[33]=5;ZCol[33]=2;
    ZRow[34]=6;ZCol[34]=1;
    ZRow[35]=7;ZCol[35]=0;
    ZRow[36]=7;ZCol[36]=1;
    ZRow[37]=6;ZCol[37]=2;
    ZRow[38]=5;ZCol[38]=3;
    ZRow[39]=4;ZCol[39]=4;
    ZRow[40]=3;ZCol[40]=5;
    ZRow[41]=2;ZCol[41]=6;
    ZRow[42]=1;ZCol[42]=7;
    ZRow[43]=2;ZCol[43]=7;
    ZRow[44]=3;ZCol[44]=6;
    ZRow[45]=4;ZCol[45]=5;
    ZRow[46]=5;ZCol[46]=4;
    ZRow[47]=6;ZCol[47]=3;
    ZRow[48]=7;ZCol[48]=2;
    ZRow[49]=7;ZCol[49]=3;
    ZRow[50]=6;ZCol[50]=4;
    ZRow[51]=5;ZCol[51]=5;
    ZRow[52]=4;ZCol[52]=6;
    ZRow[53]=3;ZCol[53]=7;
    ZRow[54]=4;ZCol[54]=7;
    ZRow[55]=5;ZCol[55]=6;
    ZRow[56]=6;ZCol[56]=5;
    ZRow[57]=7;ZCol[57]=4;
    ZRow[58]=7;ZCol[58]=5;
    ZRow[59]=6;ZCol[59]=6;
    ZRow[60]=5;ZCol[60]=7;
    ZRow[61]=6;ZCol[61]=7;
    ZRow[62]=7;ZCol[62]=6;
    ZRow[63]=7;ZCol[63]=7;

    // initialize the cos table (use a factor of 16)
    CosVal[0]=CosVal[360]=16;       // some fpu's have problem with simply degrees

    for (i=1;i<360;i++) {
       if (i==90 || i==270) CosVal[i]=0;
       else if (i==180)     CosVal[180]=-16;
       else                 CosVal[i]=(int)(cos((i*PI)/180)*16);  // PI = 180 degrees
    }

    // calculate the reverse DCT factors
    //for (i=0;i<8;i++) {
    //  factor=(((2*i+1)*45)/4); // 45/4 is actually PI/16
    //  for (j=0;j<8;j++) {
    //      degree=factor*j;          
    //      degree=degree%360;        // remove multiple 360's
    //     factor1=CosVal[degree];
          
    //      if (j==0) factor1=(factor1*49)>>6;  // divide by sqrt(2)
    //      JpgFactor[i][j]=factor1;
    //  }
    //}

    for (i=0;i<8;i++) {
      float factor=(float)(((2*i+1)*45)/4); // 45/4 is actually PI/16
      float degree,factor1;
      for (j=0;j<8;j++) {
          degree=factor*j;          
          degree=degree-(float)(((int)degree/360)*360);
          factor1=(float)(cos(degree*PI/180)*16);

          if (j==0) factor1=factor1/(float)sqrt(2); // (factor1*49)>>6;  // divide by sqrt(2)
          factor1=factor1*8;                        // 3 more bits of precision
          JpgFactor[i][j]=(long)(factor1+.5F);
      }
    }


    return TRUE;
}               

/*****************************************************************************
    GIF file translation function
******************************************************************************/
/*****************************************************************************
    Gif2Bmp:
    This routine translates input buffer or file in the GIF format to an
    output buffer in MS Windows DIB format.
******************************************************************************/
BOOL Gif2Bmp(PTERWND w, int input, LPBYTE InFile,HGLOBAL hBuf,long BufLen, HGLOBAL far *hDIB, struct StrGifAnim far *anim)
{
    HANDLE  hFile = INVALID_HANDLE_VALUE;
    struct StrGif far *gif=NULL;
    BYTE huge *InBuf;
    BOOL import=TRUE,IsGif;

    if (hDIB) (*hDIB)=NULL;
    else      import=FALSE;

    if (anim) FarMemSet(anim,0,sizeof(struct StrGifAnim));

    // open the input file
    if (input==TER_FILE) {               // data in the file
       //lstrupr(w,InFile);
       rTrim(InFile);                   // trim any spaces on right
       if (lstrlen(InFile)==0) return FALSE;
       lstrcpy(TempString,InFile);      // make a copy in the data segment
       if ((hFile=CreateFile(TempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"Gif2Bmp");
    }
    else {                                // lock the buffer
       if (NULL==(InBuf=GlobalLock(hBuf))) return PrintError(w,MSG_ERR_MEM_LOCK,"Gif2Bmp");
    }


    // allocate space for the gif info structure
    if (NULL==(gif=(struct StrGif far *)OurAlloc(sizeof(struct StrGif)))) {
       if (input==TER_FILE) CloseHandle(hFile);
       return PrintError(w,MSG_OUT_OF_MEM,"Gif2Bmp");
    }
    FarMemSet(gif,0,sizeof(struct StrGif));  // initialize with zeros


    if (input==TER_FILE) gif->hFile=hFile;
    else {
       gif->buf=InBuf;
       gif->BufLen=BufLen;
       gif->hFile = INVALID_HANDLE_VALUE;
    }


    // Read GIF header
    IsGif=GifReadHdr(w,gif);
    if (!import) goto END_FILE;    // file type detection only
    if (!IsGif)  goto END_FILE;

    if (!GifReadScrDesc(w,gif)) goto SYNTAX_ERROR;

    // read and process groups
    NEXT_GROUP:
    if (!GifGetChar(w,gif)) goto END_FILE;  // end of file

    if (gif->CurChar==0x2c) {       // process image group
       BOOL result=GifReadImageGroup(w,gif);

       GlobalUnlock(gif->hDIB);
       if ((*hDIB)==NULL) {
          if (!result) goto SYNTAX_ERROR;

          (*hDIB)=gif->hDIB;
          if (!anim) goto END_FILE; // animation pictures not requested

          gif->animating=TRUE;      // subsequent pictures will be for animation
          anim->InitDelay=gif->DelayTime;
       }
       else if (result && anim && anim->AnimPicts<MAX_ANIM_GIFS) {
          anim->hDIB[anim->AnimPicts]=gif->hDIB;
          anim->delay[anim->AnimPicts]=gif->DelayTime;

          if (gif->DelayTime==0) {
             if (anim->AnimPicts>0) anim->delay[anim->AnimPicts]=anim->delay[anim->AnimPicts-1];
             else                   anim->delay[anim->AnimPicts]=anim->InitDelay;
          }

          anim->AnimPicts++;
       }
       gif->hDIB=NULL;

       if (anim && anim->AnimPicts==MAX_ANIM_GIFS) goto END_FILE; // all images extracted
    }
    else if (gif->CurChar==0x21) {  // process an extension block
       if (!GifReadExtension(w,gif)) goto SYNTAX_ERROR;
    }
    else if (gif->CurChar==0x3B) goto END_FILE;  // end of gif data
    else goto END_FILE;             // unrecognizable block


    goto NEXT_GROUP;

    SYNTAX_ERROR:
    MessageBox(NULL,"GIF File Read aborted",NULL,MB_OK);
    OurPrintf("pos: %ld",gif->FilePos);

    END_FILE:

    if (anim) anim->AnimLoops=gif->LoopCount;

    // close file and release buffers
    if (input==TER_FILE) CloseHandle(gif->hFile);

    if (gif->hPrevData) GlobalFree(gif->hPrevData);
    if (hDIB==NULL || !IsGif) {     // data not used
       GlobalFree(gif->hDIB);
       gif->hDIB=NULL;
    }
    OurFree(gif);

    return (import?((BOOL)(*hDIB)):IsGif);
}

/*****************************************************************************
    GifReadHdr:
    This function reads the header and returns TRUE if it corresponds to
    a GIF file.
******************************************************************************/
BOOL GifReadHdr(PTERWND w, struct StrGif far *gif)
{
    BYTE string[4];
    int i;

    // Read 3 byte 'GIF' string
    for (i=0;i<3;i++) {
       if (!GifGetChar(w,gif)) return FALSE;
       string[i]=gif->CurChar;
    }
    string[i]=0;

    if (lstrcmpi(string,"GIF")!=0) return FALSE;

    // read next 3 version bytes
    for (i=0;i<3;i++) {
       if (!GifGetChar(w,gif)) return FALSE;
    }

    return TRUE;
}

/*****************************************************************************
    GifReadScrDesc:
    Read screen descriptor and the global color table if exists.
******************************************************************************/
BOOL GifReadScrDesc(PTERWND w, struct StrGif far *gif)
{
    int i,bits;
    BYTE mask;

    struct GifScrDesc {
       short width;
       short height;
       BYTE packed;
       BYTE BkColor;
       BYTE AspectRatio;
    }blk;

    // Read the block
    if (!GifGetBytes(w,gif,&blk,sizeof(blk))) return FALSE;

    // get width/height
    gif->width=blk.width;
    gif->height=blk.height;

    // access the packed field
    if (blk.packed&0x80) gif->GlbClrTblExists=TRUE;
    bits=blk.packed&0x7;      // size of global color table in bits
    bits++;
    gif->GlbClrTblSize=1<<bits;

    gif->ColorBits=(blk.packed>>4)&0x7;
    gif->ColorBits++;

    // get other fields
    gif->BkColor=blk.BkColor;
    mask=(BYTE)((1<<(gif->ColorBits))-1);   // use only specified number of bits
    gif->BkColor&=mask;

    gif->AspectRatio=blk.AspectRatio;

    // read the color table
    if (gif->GlbClrTblExists) {
       BYTE red,green,blue;
       for (i=0;i<gif->GlbClrTblSize;i++) {
          if (!GifGetChar(w,gif)) return FALSE;  // get red color
          red=gif->CurChar;
          if (!GifGetChar(w,gif)) return FALSE;  // get green color
          green=gif->CurChar;
          if (!GifGetChar(w,gif)) return FALSE;  // get blue color
          blue=gif->CurChar;
          gif->GlobalColor[i]=RGB(red,green,blue);
       }
       if (gif->BkColor>=0 && gif->BkColor<=gif->GlbClrTblSize) {
          gif->PrevBkColor=gif->GlobalColor[gif->BkColor];
          //gif->GlobalColor[gif->BkColor]=TextDefBkColor;
       }
       else gif->BkColor=-1;

    }

    return TRUE;
}

/*****************************************************************************
    GifReadImageGroup:
    Read the current image descriptor, local color table and image data.
******************************************************************************/
BOOL GifReadImageGroup(PTERWND w, struct StrGif far *gif)
{
    int i,LclClrTblSize=0,bits,BlockSize,ClrTblSize,BitsPerPixel;
    struct StrLz LzBlock;               // lz decompression block
    struct StrLz far *lz=&LzBlock;
    BYTE input[256];
    long BitmapSize,l;
    LPBITMAPINFOHEADER pInfo;
    RGBQUAD far *rgb;
    LPBYTE pPrevData;
    BOOL result;

    // Read the block
    if (!GifGetBytes(w,gif,&(gif->image),sizeof(gif->image))) return FALSE;

    if (gif->width==0) gif->width=gif->image.width;
    if (gif->height==0) gif->height=gif->image.height;

    // extract the packed fields
    if ((gif->image).packed&0x80) {              // 8th bit
        gif->CurColor=gif->LocalColor;  // use local color table
        bits=(gif->image).packed&0x7;            // size of global color table in bits (first 3 bits)
        bits++;
        LclClrTblSize=1<<bits;
        ClrTblSize=LclClrTblSize;
    }
    else {
        gif->CurColor=gif->GlobalColor;
        ClrTblSize=gif->GlbClrTblSize;
    }

    gif->interlaced=(gif->image).packed&0x40;   // extract interlace bit (7th bit)
    gif->CurX=gif->CurY=0;                      // current pixel to be drawn
    gif->InterlacePass=0;                       // current interlace pass

    // extract the local color table if exists
    if (LclClrTblSize>0) {
       BYTE red,green,blue;
       for (i=0;i<LclClrTblSize;i++) {
          if (!GifGetChar(w,gif)) return FALSE;  // get red color
          red=gif->CurChar;
          if (!GifGetChar(w,gif)) return FALSE;  // get green color
          green=gif->CurChar;
          if (!GifGetChar(w,gif)) return FALSE;  // get blue color
          blue=gif->CurChar;
          gif->LocalColor[i]=RGB(red,green,blue);
       }
    }

    // Determine the size of the DIB and allocate memory
    BitsPerPixel=8;
    gif->ScanLineSize=(gif->width*BitsPerPixel+31)/32; // scan line width in number of DWORDS
    gif->ScanLineSize*=4;  // scan line size in number of bytes

    BitmapSize=sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*ClrTblSize+gif->ScanLineSize*gif->height;

    if (  NULL==(gif->hDIB=GlobalAlloc(GMEM_MOVEABLE,BitmapSize+1))
       || NULL==(pInfo=GlobalLock(gif->hDIB)) ){
        return PrintError(w,MSG_OUT_OF_MEM,"GifReadImageGroup");
    }

    // fill in the header variables
    FarMemSet(pInfo,0,sizeof(BITMAPINFOHEADER));
    pInfo->biSize=sizeof(BITMAPINFOHEADER);
    pInfo->biWidth=gif->width;
    pInfo->biHeight=gif->height;
    pInfo->biPlanes=1;
    pInfo->biBitCount=BitsPerPixel;
    pInfo->biCompression=BI_RGB;       // no compression
    pInfo->biSizeImage=gif->ScanLineSize*gif->height;
    pInfo->biClrUsed=ClrTblSize;

    // set the color table
    rgb=(RGBQUAD far *) & (((LPBYTE)pInfo)[(int)pInfo->biSize]); // pointer to the color table
    for (i=0;i<(int)(pInfo->biClrUsed);i++) {
       rgb[i].rgbRed=GetRValue(gif->CurColor[i]);
       rgb[i].rgbGreen=GetGValue(gif->CurColor[i]);
       rgb[i].rgbBlue=GetBValue(gif->CurColor[i]);
    }

    // set the pointer to the image area and initialize the image area
    gif->pImage=(LPBYTE) & (((LPBYTE)pInfo)[(int)pInfo->biSize+sizeof(RGBQUAD)*ClrTblSize]); // pointer to the color table
    if (gif->hPrevData && (gif->width!=gif->image.width || gif->height!=gif->image.height)) {  // copy the data from the previous image
       pPrevData=GlobalLock(gif->hPrevData);
       for (l=0;l<(long)pInfo->biSizeImage;l++) gif->pImage[l]=pPrevData[l];
       GlobalUnlock(gif->hPrevData);
    }
    else if (gif->BkColor<gif->GlbClrTblSize && gif->CurColor==gif->GlobalColor) {
        if (pInfo->biSizeImage<=0xFFFFL) FarMemSet(gif->pImage,(BYTE)(gif->BkColor),(UINT)pInfo->biSizeImage);
        else {
          for (l=0;l<(long)pInfo->biSizeImage;l++) gif->pImage[l]=(BYTE)gif->BkColor;
        }
    }


    // read the image data header
    if (!GifGetChar(w,gif)) return FALSE;       // get code size
    bits=gif->CurChar;                          // bits per output item

    LzLastChar=(1<<bits)-1;                    // last character
    LzClearCode=1<<bits;                       // clear code
    LzEofCode=LzClearCode+1;                   // eof code
    LzFirstCode=LzClearCode+2;                 // first code
    LzBeginInBits=bits+1;                      // starting number of bits per code
    LzMaxInBits=12;                            // maximum number of bits per code
    LzMaxInBufSize=256;                        // maximum bytes per input stream
    LzUserData=(void far *)gif;                // store our gif pointer here
    LzOutputCallback=GifData;                  // routine to receive output characters
    LzImageType=PICT_GIF;                      // bits stored in intel format

    if (!LzInit(w,lz)) return FALSE;             // initialize the lz decompression routine

    // Read each subblock and process
    result=TRUE;
    while (TRUE) {
       if (!GifGetChar(w,gif)) return FALSE;    // get code size
       BlockSize=gif->CurChar;

       if (BlockSize==0) {                      // terminator block
          LzExit(lz);                           // close lz routine
          break;
       }

       // read the data bits
       if (!GifGetBytes(w,gif,input,BlockSize)) return FALSE;

       if (LzEof) result=FALSE;

       if (result) result=LzDecompress(lz,input,BlockSize);
    }

    gif->extracted=TRUE;                        // atleast one picture extracted

    // make a copy of this image
    if (result) {
       if (gif->hPrevData==NULL) gif->hPrevData=GlobalAlloc(GMEM_MOVEABLE,pInfo->biSizeImage);
       if (gif->hPrevData) {
          pPrevData=GlobalLock(gif->hPrevData);
          for (l=0;l<(long)pInfo->biSizeImage;l++) pPrevData[l]=gif->pImage[l];
          GlobalUnlock(gif->hPrevData);
       }
    }

    // restore the global color table for any modification to the transparent color
    if (gif->IsTransparent) {
        gif->GlobalColor[gif->XparentColor]=gif->PrevXparentColor;
        if (gif->BkColor>=0) gif->GlobalColor[gif->BkColor]=gif->PrevBkColor;
    }
    gif->IsTransparent=FALSE;

    return result;
}

/*****************************************************************************
    GifReadExtension:
    Read an extension block
******************************************************************************/
BOOL GifReadExtension(PTERWND w, struct StrGif far *gif)
{
    int size;
    BYTE label;
    BOOL NetscapeSignFound;
    BYTE string[20];

    // read the block label
    if (!GifGetChar(w,gif)) return FALSE;  // read the block label
    label=gif->CurChar;

    if ( label==0xF9                         // graphic control block
      || label==0x01                         // text block
      || label==0xFF                         // Application extension
      || label==0xFE ) {                     // comment block

       if (label==0xF9) {                    // graphic control block
          if (!GifGetChar(w,gif)) return FALSE;  // skip over the size byte

          if (!GifGetChar(w,gif)) return FALSE;  // packed fields
          gif->IsTransparent=(gif->CurChar&0x1);

          if (!GifGetChar(w,gif)) return FALSE;  // delay time low order
          gif->DelayTime=gif->CurChar;
          if (!GifGetChar(w,gif)) return FALSE;  // delay high  order
          gif->DelayTime|=(((int)gif->CurChar)<<8);

          if (!GifGetChar(w,gif)) return FALSE;  // transparent color index
          gif->XparentColor=gif->CurChar;
          if (!gif->GlbClrTblExists || gif->XparentColor>=gif->GlbClrTblSize) gif->IsTransparent=FALSE;

          // transparent colors are used only for animation sequence.  It is
          // not used for the first picture.  The BkColor is used as the
          // background color for the window only for the first picture.  For
          // all other pictures, BkColor should retain its value.  For
          // animation sequence we swapt BkColor slot with the transparent
          // color slot because the transparent color slot is simply not
          // used.
          if (gif->IsTransparent) {
              gif->PrevXparentColor=gif->GlobalColor[gif->XparentColor];
              if (gif->animating && gif->BkColor>=0 && gif->XparentColor!=gif->BkColor) {
                   gif->GlobalColor[gif->XparentColor]=gif->PrevBkColor;  // swap the slot temporarily
                   if (gif->BkColor>=0) gif->GlobalColor[gif->BkColor]=PageColor(w);  // moved from after the else statement - test with d:\gif\test.gif
              }
              else gif->GlobalColor[gif->XparentColor]=PageColor(w);
          }
          else if (gif->BkColor>=0) gif->GlobalColor[gif->BkColor]=gif->PrevBkColor;  // transparent not used, restore

          if (!GifGetChar(w,gif)) return FALSE;  // skip over the terminator byte
       }
       else if (label==0xFF) {                        // look for netscape application extension
          NetscapeSignFound=FALSE;
          while (TRUE) {                         // read sub blocks
             if (!GifGetChar(w,gif)) return FALSE;  // read the block size
             size=gif->CurChar;
             if (size==0) break;
             if (size==11) {                     // check for the netscape signe
                if (!GifGetBytes(w,gif,string,size)) return FALSE;
                string[8]=0;
                if (lstrcmpi(string,"netscape")==0) NetscapeSignFound=TRUE;
             }
             else if (NetscapeSignFound && size==3) {
                if (!GifGetChar(w,gif)) return FALSE;  // read the fix byte

                if (!GifGetChar(w,gif)) return FALSE;  // delay time low order
                gif->LoopCount=gif->CurChar;
                if (!GifGetChar(w,gif)) return FALSE;  // delay high  order
                gif->LoopCount|=(((UINT)gif->CurChar)<<8);
                if (gif->LoopCount==0) gif->LoopCount=ANIM_CONTINUOUS;  // continuous loop
             }
             else if (!GifGetBytes(w,gif,NULL,size)) return FALSE;  // go past this block and the terminator block
          }
       }
       else {                         // skip the block
          while (TRUE) {                         // read sub blocks
             if (!GifGetChar(w,gif)) return FALSE;  // read the block size
             size=gif->CurChar;
             if (size==0) break;
             if (!GifGetBytes(w,gif,NULL,size)) return FALSE;  // go past this block and the terminator block
          }
       }
    }

    return TRUE;
}

/*****************************************************************************
    GifData:
    This item receives the output data from the LZW decompressor
******************************************************************************/
BOOL GifData(struct StrLz far *lz)
{
    struct StrGif far *gif;
    int y;
    BOOL UpdatePixel=TRUE;

    // extract the pointers
    gif=(struct StrGif far *)LzUserData;

    // set the current pixel
    if (gif->CurY>=(int)(gif->image.height)) return TRUE;

    // set DIB value
    y=gif->image.y+gif->CurY;      // y relative to top
    y=gif->height-y-1;             // y relative to bottom

    if (gif->IsTransparent && gif->animating && LzOutValue==gif->XparentColor) UpdatePixel=FALSE;
    if (gif->animating && LzOutValue==gif->BkColor && gif->IsTransparent) LzOutValue=gif->XparentColor;  // use this swapped slot temporarily

    if (UpdatePixel && (gif->image.x+gif->CurX)<gif->width) 
         gif->pImage[(y*gif->ScanLineSize)+gif->image.x+gif->CurX]=(BYTE)LzOutValue;

    // advance to next pixel
    gif->CurX++;
    if (gif->CurX>=(int)(gif->image.width)) {
      gif->CurX=0;
      if (gif->interlaced) {
         // increment amount specific to the current pass
         if      (gif->InterlacePass==0) gif->CurY+=8;
         else if (gif->InterlacePass==1) gif->CurY+=8;
         else if (gif->InterlacePass==2) gif->CurY+=4;
         else if (gif->InterlacePass==3) gif->CurY+=2;

         // end of current pass?
         if (gif->CurY>=(int)(gif->image.height)) {
            gif->InterlacePass++;       // switch to next pass
            if      (gif->InterlacePass==1) gif->CurY=4;  // starting row for the next pass
            else if (gif->InterlacePass==2) gif->CurY=2;
            else                            gif->CurY=1;
         }

      }
      else gif->CurY++;
    }

    return TRUE;
}

/*****************************************************************************
    GifGetBytes:
    Reads the specified number of bytes from the source
******************************************************************************/
BOOL GifGetBytes(PTERWND w, struct StrGif far *gif,LPVOID dest,int count)
{
    int i;

    for (i=0;i<count;i++) {
       if (!GifGetChar(w,gif)) return FALSE;  // get next byte
       if (dest) ((LPBYTE)dest)[i]=gif->CurChar;
    }

    return TRUE;
}

/*****************************************************************************
    GifGetWord:
    Reads the next 2 characters and combine them to return a word value.
******************************************************************************/
BOOL GifGetWord(PTERWND w, struct StrGif far *gif)
{
    BYTE lo,hi;

    if (!GifGetChar(w,gif)) return FALSE;    // get low byte
    lo=gif->CurChar;

    if (!GifGetChar(w,gif)) return FALSE;    // get high byte
    hi=gif->CurChar;

    gif->CurWord=(WORD)( ( ((WORD)hi) <<8) +lo);

    return TRUE;
}

/*****************************************************************************
    GifGetChar:
    Reads the next characters from the input file or the input buffer.
    When the InSteam is NULL, the characters are read from the buffer.
    Returns FALSE when no more characters available.
******************************************************************************/
BOOL GifGetChar(PTERWND w, struct StrGif far *gif)
{
    // get the character from the character stack if available
    if (gif->StackLen>0) {
       gif->StackLen--;
       gif->CurChar=gif->stack[gif->StackLen];
       gif->FilePos++;       // advance file position
       return TRUE;
    }

    // read from the file
    gif->eof=FALSE;
    if (gif->hFile!=INVALID_HANDLE_VALUE) {
       if (gif->TextIndex<gif->TextLen) {  // read from the temporary buffer
          gif->CurChar=gif->text[gif->TextIndex];
          (gif->TextIndex)++;
          gif->FilePos++;    // advance file position
          return TRUE;
       }

       // read more characters from the file
       gif->TextLen=0;
       ReadFile(gif->hFile,TempString,MAX_WIDTH,&((DWORD)(gif->TextLen)),NULL);

       if (gif->TextLen==0) {
          gif->eof=TRUE;
          return FALSE;      // end of file
       }
       gif->TextIndex=0;
       FarMove(TempString,gif->text,gif->TextLen);

       gif->CurChar=gif->text[gif->TextIndex];
       (gif->TextIndex)++;
       gif->FilePos++;      // advance file position
       return TRUE;
    }
    else {              // read from the buffer
       if (gif->BufLen>=0 && gif->BufIndex>=gif->BufLen) {
          gif->eof=TRUE;
          return FALSE; // end of buffer
       }
       gif->CurChar=gif->buf[gif->BufIndex];
       (gif->BufIndex)++;
       gif->FilePos++;  // advance file position
       return TRUE;
    }
}

/*****************************************************************************
    GifPushChar:
    Push the lastly read character onto stack.
******************************************************************************/
BOOL GifPushChar(PTERWND w, struct StrGif far *gif)
{
    if (gif->StackLen>=MAX_WIDTH) return PrintError(w,MSG_OUT_OF_CHAR_STACK,"GifPushChar");

    gif->stack[gif->StackLen]=gif->CurChar;
    (gif->StackLen)++;
    gif->FilePos--;  // decrement file position
    return TRUE;
}

/*****************************************************************************
    TIFF file translation function
******************************************************************************/

/*****************************************************************************
    Tif2Bmp:
    This routine translates input buffer or file in the TIF format to an
    output buffer in MS Windows DIB format.
******************************************************************************/
BOOL Tif2Bmp(PTERWND w,int input, LPBYTE InFile,HGLOBAL hBuf,long BufLen, HGLOBAL far *hDIB, BOOL import)
{
    HANDLE  hFile=INVALID_HANDLE_VALUE;
    struct StrTif far *tif=NULL;
    BYTE huge *InBuf;
    DWORD IfdTblOffset;
    struct StrTifIfd ifd;   // ifd entry
    short IfdCount,count;
    BOOL  IsTifFile=false;
    BOOL  ReadComplete=false;

    if (hDIB) (*hDIB)=NULL;

    // open the input file
    if (input==TER_FILE) {               // data in the file
       lstrupr(w,InFile);
       rTrim(InFile);                   // trim any spaces on right
       if (lstrlen(InFile)==0) return FALSE;
       lstrcpy(TempString,InFile);      // make a copy in the data segment
       if ((hFile=CreateFile(TempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
          return PrintError(w,MSG_ERR_FILE_OPEN,"TIF file not found or file open error!");
       }
    }
    else {                                // lock the buffer
       if (NULL==(InBuf=GlobalLock(hBuf))) return PrintError(w,MSG_OUT_OF_MEM,"Error locking the input buffer");
    }

    if (!HuffInitialized) InitTifHuf();   // initialize the tiff tables

    // allocate space for the tif info structure
    if (NULL==(tif=(struct StrTif far *)OurAlloc(sizeof(struct StrTif)))) {
       return PrintError(w,MSG_OUT_OF_MEM,"Ran out of memory for HTML data (TifRead).");
    }
    FarMemSet(tif,0,sizeof(struct StrTif));  // initialize with zeros
    tif->w=w;                                // store our pointer
    tif->BitsPerSample=1;                    // bits per sample
    tif->SamplesPerPixel=1;                  // samples per pixel
    tif->StripLenType=-1;                    // no type

    tif->RowsPerStrip=0;                     // rows per strip
    tif->compression=1;                      // huffman coded

    if (input==TER_FILE) tif->hFile=hFile;
    else {
       tif->buf=InBuf;
       tif->BufLen=BufLen;
       tif->hFile=INVALID_HANDLE_VALUE;
    }

    // Read TIF header
    if (0==(IfdTblOffset=TifReadHdr(w,tif)))  goto END_FILE;
    
    IsTifFile=true;

    if (!import) goto END_FILE;   // just needed to check if the file was tiff file

    // read and process groups
    NEXT_IFD_TABLE:
    if (!TifGetOffsetBytes(w,tif,IfdTblOffset,&IfdCount,sizeof(IfdCount))) goto FILE_INCOMPLETE;
    if (tif->motorala) IfdCount=ToIntelInt(IfdCount); // convert to intel standart
    IfdTblOffset+=sizeof(IfdCount);       // advance the offset

    // process each ifd
    for (count=0;count<IfdCount;count++) {
       // read this ifd entry
       if (!TifGetOffsetBytes(w,tif,IfdTblOffset,&ifd,sizeof(ifd))) goto FILE_INCOMPLETE;
       if (tif->motorala) {                    // convert to intel standard
          ifd.tag=ToIntelInt(ifd.tag);
          ifd.type=ToIntelInt(ifd.type);
          ifd.count=ToIntelLong(ifd.count);
          ifd.data=ToIntelLong(ifd.data);
       }
       IfdTblOffset+=sizeof(ifd);         // advance the offset

       // Process this baseline ifds
       if (ifd.tag==TIF_HEIGHT) {
          if (ifd.type==TIF_SHORT) tif->height=(int)(ifd.data&0xFFFF);
          else                     tif->height=(int)(ifd.data);
       }
       else if (ifd.tag==TIF_WIDTH) {
          if (ifd.type==TIF_SHORT) tif->width=(int)(ifd.data&0xFFFF);
          else                     tif->width=(int)ifd.data;
       }
       else if (ifd.tag==TIF_SAMPLES_PER_PIXEL) {
          tif->SamplesPerPixel=(int)(ifd.data&0xFFFF);
       }
       else if (ifd.tag==TIF_BITS_PER_SAMPLE) {
          int size;
          if (ifd.type==TIF_BYTE || ifd.type==TIF_SBYTE) size=1;  // size of the fields
          else                                           size=2;
          if ((size*tif->SamplesPerPixel)<=sizeof(DWORD)) {
             if (size==1) tif->BitsPerSample=(int)(ifd.data&0xFF);
             else         tif->BitsPerSample=(int)(ifd.data&0xFFFF);
          }
          else {          // read from the pointer ifd.data
             WORD data;
             if (!TifGetOffsetBytes(w,tif,ifd.data,&data,sizeof(data))) goto FILE_INCOMPLETE;
             if (tif->motorala) data=ToIntelInt(data);
             if (size==1) tif->BitsPerSample=data&0xFF;
             else         tif->BitsPerSample=data;
          }
       }
       else if (ifd.tag==TIF_PHOTO_INTERPRET) {
          tif->ImageType=(int)(ifd.data&0xFFFF);
       }
       else if (ifd.tag==TIF_ROWS_PER_STRIP) {
          if (ifd.type==TIF_SHORT) tif->RowsPerStrip=(int)(ifd.data&0xFFFF);
          else                     tif->RowsPerStrip=(int)(ifd.data);
       }
       else if (ifd.tag==TIF_COLOR_MAP) {
          tif->ColorTableOffset=ifd.data;
       }
       else if (ifd.tag==TIF_COMPRESSION) {
          tif->compression=(WORD)(ifd.data&0xFFFF);
       }
       else if (ifd.tag==TIF_STRIP_BYTE_COUNTS) {
          tif->StripLenOffset=ifd.data;
          tif->StripLenType=ifd.type;
       }
       else if (ifd.tag==TIF_STRIP_OFFSETS) {
          tif->StripOffset=ifd.data;
       }
    }
    
    // read the next ifd table position
    if (!TifGetOffsetBytes(w,tif,IfdTblOffset,&IfdTblOffset,sizeof(IfdTblOffset))) goto FILE_INCOMPLETE;
    if (tif->motorala) IfdTblOffset=ToIntelLong(IfdTblOffset);

    // end of the ifd table?
    if (IfdTblOffset) goto NEXT_IFD_TABLE;
    else {
       if (tif->StripOffset) TifReadStrips(w,tif);
       goto END_FILE;
    }        

    
    FILE_INCOMPLETE:
    PrintError(w,MSG_TIFF,"Unexpected End of TIF File");
    //OurPrintf("pos: %ld",tif->FilePos); 
    goto END_FILE;

    END_FILE:
      // pass the DIB handle
    if (tif->hDIB) {
       GlobalUnlock(tif->hDIB);
       if (import) (*hDIB)=tif->hDIB;
       else         GlobalFree(tif->hDIB);
    }

    // close file and release buffers
    if (input==TER_FILE) CloseHandle(tif->hFile);
    OurFree(tif);

    if (import) return (BOOL)(*hDIB);
    else        return IsTifFile;
}

/*****************************************************************************
    TifReadHdr:
    This function reads the header and returns the offset of the first
    IFD.
******************************************************************************/
DWORD TifReadHdr(PTERWND w,struct StrTif far *tif)
{
    struct StrTifHdr {
       BYTE byte1;           // signature byte 1
       BYTE byte2;           // signature byte 2
       WORD version;
       DWORD offset;         // offset to the first ifd
    }hdr;

    if (!TifGetBytes(w,tif,&hdr,sizeof(hdr))) return FALSE;
    if (hdr.byte1!='I' && hdr.byte2!='I') {
       if (hdr.byte1!='M' && hdr.byte2!='M') {
          return 0;  // (DWORD)PrintError(w,MSG_TIFF,"Invalid TIFF format");
       }
    }

    if (hdr.byte1=='M') tif->motorala=TRUE;

    if (tif->motorala) {             // switch to intel format
       hdr.version=ToIntelInt(hdr.version);
       hdr.offset=ToIntelLong(hdr.offset);
    }

    if (hdr.version!=0x2A) 0; // return (DWORD)PrintError(w,MSG_TIFF,"Invalid TIFF version");

    return hdr.offset;
}

/*****************************************************************************
    TifReadStrips:
    This function reads image data strips
******************************************************************************/
BOOL TifReadStrips(PTERWND w,struct StrTif far *tif)
{
    long BitmapSize;
    //short  StripLen;
    int  StripLen;
    int   strip,StripCount;
    DWORD StripOffset;
    long  StripLenLong;
    int i,ClrTblSize;
    LPBITMAPINFOHEADER pInfo;
    long l,BytesPerLine;
    BYTE huge *pStrip;
    struct StrLz LzBlock;               // lz decompression block
    struct StrLz far *lz=&LzBlock;
    RGBQUAD far *rgb;

    // intialize the lz struture
    FarMemSet(lz,0,sizeof(struct StrLz));

    // get the color table size
    if (tif->ImageType==0 || tif->ImageType==1) {  // black and white
       int color=0,step;
       tif->BitsPerPixel=tif->BitsPerSample;
       ClrTblSize=1<<tif->BitsPerSample;           // black and white

       // build the color table
       if (ClrTblSize>1) step=255/(ClrTblSize-1);
       else              step=255/ClrTblSize;
       for (i=0;i<ClrTblSize;i++) {                // build the color table
          if (tif->ImageType==1) tif->red[i]=tif->green[i]=tif->blue[i]=(WORD)(color<<8);
          else                   tif->red[i]=tif->green[i]=tif->blue[i]=(WORD)((255-color)<<8);
          color+=step;
          if (color>255) color=255;
          if (i==(ClrTblSize-1)) color=255;        // last color
       }
    }
    else if (tif->ImageType==2) {                  // RGB 
       tif->BitsPerPixel=24;                       // RGB stored directly
       ClrTblSize=0;                               // color table not used
       tif->BytesPerPixel=3;                       // 3 pixels per byte
       tif->CurPixByte=0;                          // current pixel byte
    }
    else if (tif->ImageType==3) {                  // read the color palette
       int size;

       tif->BitsPerPixel=tif->BitsPerSample;
       ClrTblSize=1<<tif->BitsPerSample;

       size=ClrTblSize*sizeof(WORD);

       // read the red table
       if (!TifGetOffsetBytes(w,tif,tif->ColorTableOffset,tif->red,size)) return FALSE;
       // read the green table
       tif->ColorTableOffset+=size;
       if (!TifGetOffsetBytes(w,tif,tif->ColorTableOffset,tif->green,size)) return FALSE;
       // read the blue table
       tif->ColorTableOffset+=size;
       if (!TifGetOffsetBytes(w,tif,tif->ColorTableOffset,tif->blue,size)) return FALSE;
    }
    else {
       return PrintError(w,MSG_TIFF,"This TIFF image not supported (ImageType>3)");
    }

    // check for bits per pixel
    if (tif->BitsPerPixel!=1 && tif->BitsPerPixel!=4 && tif->BitsPerPixel!=8 && tif->BitsPerPixel!=24) {
         return PrintError(w,MSG_TIFF,"This TIFF image not supported (unsupported Bits per Pixel)");
    }

    // get the scan lines size 
    BytesPerLine=(tif->BitsPerPixel*tif->width+7)/8;   // round to byte boundary
    tif->ScanLineSize=(BytesPerLine+sizeof(DWORD)-1)/sizeof(DWORD); // number of dwords, round to DWORD boundary
    tif->ScanLineSize=tif->ScanLineSize*sizeof(DWORD);

    // calculate the size of the bitmap
    BitmapSize=sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*ClrTblSize+tif->ScanLineSize*tif->height;

    if (  NULL==(tif->hDIB=GlobalAlloc(GMEM_MOVEABLE,BitmapSize+1))
       || NULL==(pInfo=GlobalLock(tif->hDIB)) ){
        return PrintError(w,MSG_TIFF,"Error allocating DIB memory(TifReadImageGroup)");
    }
    
    // fill in the header variables
    FarMemSet(pInfo,0,sizeof(BITMAPINFOHEADER));
    pInfo->biSize=sizeof(BITMAPINFOHEADER);
    pInfo->biWidth=tif->width;
    pInfo->biHeight=tif->height;
    pInfo->biPlanes=1;
    pInfo->biBitCount=(WORD)tif->BitsPerPixel;
    pInfo->biCompression=BI_RGB;       // no compression
    pInfo->biSizeImage=tif->ScanLineSize*tif->height;
    pInfo->biClrUsed=ClrTblSize;

    // set the color table
    rgb=(RGBQUAD far *) & (((LPBYTE)pInfo)[(int)pInfo->biSize]); // pointer to the color table
    if (ClrTblSize>0) {                               // copy color table
       for (i=0;i<(int)(pInfo->biClrUsed);i++) {
          rgb[i].rgbRed=HIBYTE(tif->red[i]);
          rgb[i].rgbGreen=HIBYTE(tif->green[i]);
          rgb[i].rgbBlue=HIBYTE(tif->blue[i]);
       }
    }

    // set the pointer to the image area and initialize the image area
    tif->pImage=(LPBYTE) & (((LPBYTE)pInfo)[(int)pInfo->biSize+sizeof(RGBQUAD)*ClrTblSize]); // pointer to the color table

    // Read each strip and process
    if (tif->RowsPerStrip==0) tif->RowsPerStrip=tif->height;   // default rows per strip
    StripCount=(tif->height+tif->RowsPerStrip-1)/tif->RowsPerStrip;

    for (strip=0;strip<StripCount;strip++) {
       // read the strip len
       if (tif->StripLenType==-1) {             // 'StripByteCount' not in the file
          long TotalBits=tif->BitsPerPixel*(long)tif->width*(long)tif->height;  // assume no compression
          StripLenLong=((TotalBits+7)/8);       // convert to bytes
          StripLen=(short)StripLenLong;
       }
       else if (tif->StripLenType==TIF_SHORT) {
          if (StripCount<=2) {
             if (strip==0) StripLen=(short)(tif->StripLenOffset&0xFFFF);
             else          StripLen=(short)((tif->StripLenOffset>>16)&0xFFFF);
          }
          else {
             if (!TifGetOffsetBytes(w,tif,tif->StripLenOffset,&(StripLen),sizeof(StripLen))) return FALSE;
             if (tif->motorala) StripLen=ToIntelInt((WORD)StripLen);
             tif->StripLenOffset+=sizeof(StripLen);
          }
          StripLenLong=StripLen;               // save it in long format also
       }
       else {
         if (StripCount==1) StripLenLong=tif->StripLenOffset;
         else {
            if (!TifGetOffsetBytes(w,tif,tif->StripLenOffset,&(StripLenLong),sizeof(StripLenLong))) return FALSE;
            if (tif->motorala) StripLenLong=ToIntelLong(StripLenLong);
            tif->StripLenOffset+=sizeof(StripLenLong);
         }
         //StripLen=(short)StripLenLong;           // convert to int
         StripLen=StripLenLong;           // convert to int
       }

       // check strip length limit
       if (  (StripLenLong>=TIF_MAX_IN_BUF_SIZE || StripLen>=TIF_MAX_IN_BUF_SIZE)
          && tif->compression!=TIF_CMP_NONE 
          && tif->compression!=TIF_CMP_PACKBITS ) return PrintError(w,MSG_TIFF,"TIFF Strips too long");

       // allocate space for the strip
       if (NULL==(pStrip=OurAlloc(StripLenLong+1))) return PrintError(w,MSG_TIFF,"Ran out of memory for TIFF strip allocation");

       // Read this strip offset
       if (StripCount==1) StripOffset=tif->StripOffset;
       else {
          if (!TifGetOffsetBytes(w,tif,tif->StripOffset,&(StripOffset),sizeof(StripOffset))) return FALSE;
          if (tif->motorala) StripOffset=ToIntelLong(StripOffset);
          tif->StripOffset+=sizeof(StripOffset);       // advance to the next pointer
       }

       // Read this strip data
       if (!TifGetOffsetBytes(w,tif,StripOffset,pStrip,StripLenLong)) return FALSE;

       // Decompress this strip
       if (tif->compression==TIF_CMP_NONE) {         // no compression
          LzUserData=(void far *)tif;                // use lz structure to pass bytes
          for (l=0;l<StripLenLong;l++) {
             LzOutValue=pStrip[l];
             if (tif->BitsPerPixel<8) {              // rows must end at byte boundaries
                LzBitsStuffed=(tif->width-tif->CurX)*tif->BitsPerPixel;
                if (LzBitsStuffed>8) LzBitsStuffed=8;
             }
             TifData(lz);                            // pass this value
          }
       }
       else if (tif->compression==TIF_CMP_HUFFMAN) { // Huffman coding
          LzUserData=(void far *)tif;                // use lz structure to pass bytes
          TifHufDecompress(w,tif,lz,(LPBYTE)pStrip,StripLen);
       }
       else if (tif->compression==TIF_CMP_PACKBITS) {// packbits

          LzUserData=(void far *)tif;                // use lz structure to pass bytes
          for (l=0;l<StripLenLong;l++) {
             char code=pStrip[l];
             int  run=abs(code)+1;                      // run length
             if (code<0) l++;                           // run length encoded

             while (run>0) {
                if (code>=0) l++;                       // copy mode
                LzOutValue=pStrip[l];

                //if (l>=StripLenLong) break;
                if (tif->BitsPerPixel<8) {              // rows must end at byte boundaries
                   LzBitsStuffed=(tif->width-tif->CurX)*tif->BitsPerPixel;
                   if (LzBitsStuffed>8) LzBitsStuffed=8;
                }
                TifData(lz);                       // pass this value
                run--;
             }
          }
       }
       else if (tif->compression==TIF_CMP_LZW) {     // LZW compression
          LzLastChar=255;                            // last character
          LzClearCode=256;                           // clear code
          LzEofCode=LzClearCode+1;                   // eof code
          LzFirstCode=LzClearCode+2;                 // first code
          LzBeginInBits=9;                           // starting number of bits per code
          LzMaxInBits=12;                            // maximum number of bits per code
          LzMaxInBufSize=StripLen+1;                 // maximum bytes per input stream
          LzUserData=(void far *)tif;                // store our tif pointer here
          LzOutputCallback=TifData;                  // routine to receive output characters
          LzImageType=PICT_TIFF;                     // bits stored in motorala order (high to low)

          if (!LzInit(w,lz)) return FALSE;             // initialize the lz decompression routine
          if (!LzDecompress(lz,(LPBYTE)pStrip,StripLen)) return FALSE;  // decompress these bytes
          LzExit(lz);                           // close lz routine
       }


       OurFree(pStrip);           // free the stip memory
    }

    return TRUE;
}

/*****************************************************************************
    TifData:
    This item receives the output data from a decompressor
******************************************************************************/
BOOL TifData(struct StrLz far *lz)
{
    struct StrTif far *tif;
    int y,i;
    

    // extract the pointers
    tif=(struct StrTif far *)LzUserData;

    // set the current pixels
    if (tif->CurY>=tif->height) return TRUE;

    if (tif->BitsPerPixel<=8) {  // set two pixels
       BYTE in,pixel,cur,mask;
       int  PixCount,PixPerByte,CurByteIdx,BitOffset;

       in=(BYTE)LzOutValue;
       PixPerByte=8/tif->BitsPerPixel;

       if (LzBitsStuffed>0) PixCount=LzBitsStuffed;
       else                 PixCount=PixPerByte;

       // extract pixel data and insert into the bitmap
       for (i=0;i<PixCount;i++) {

          if (tif->CurY>=tif->height) return TRUE;

          // Find the bit offset for the current pixel in the current image byte
          CurByteIdx=tif->CurX/PixPerByte; // byte index into the image
          BitOffset=(int)((tif->CurX*(long)tif->BitsPerPixel)-CurByteIdx*8L);

          // isolate current pixel in the high end of the byte with 0s in other location
          pixel=in;                            // our pixel is at the high end
          pixel=(BYTE)(pixel>>(8-tif->BitsPerPixel));  // move it to the low end
          pixel=(BYTE)(pixel<<(8-tif->BitsPerPixel));  // move it to the high end with low bits cleard
          pixel=(BYTE)(pixel>>BitOffset);          // move bits to their final offset

          // prepare mask
          mask=0xFF;
          mask=(BYTE)(mask>>(8-tif->BitsPerPixel));
          mask=(BYTE)(mask<<(8-tif->BitsPerPixel));
          mask=(BYTE)(mask>>BitOffset);    // has 1s in bit locations
          mask=(BYTE)(~mask);              // has 1s in locations other than bits

          // get current bit and apply mask and our bits
          y=tif->CurY;                     // y relative to top
          y=tif->height-y-1;               // y relative to bottom
          cur=tif->pImage[(y*tif->ScanLineSize)+CurByteIdx];  // current pixel
          cur=cur&mask;                    // clear the bit location
          cur=cur|pixel;                   // add bits
          tif->pImage[(y*tif->ScanLineSize)+CurByteIdx]=cur; // store

          // advance to next pixel
          tif->CurX++;
          if (tif->CurX>=tif->width) {
            tif->CurX=0;
            tif->CurY++;
          }

          in=(BYTE)(in<<(tif->BitsPerPixel));  // scroll off the processed bits
       }

    }
    else if (tif->BitsPerPixel==24) {
       // get the pointer to the current pixel
       y=tif->CurY;                     // y relative to top
       y=tif->height-y-1;               // y relative to bottom
       tif->pImage[(y*tif->ScanLineSize)+                 // row offset
                   ((long)tif->CurX*tif->BytesPerPixel)   // pixel offset
                   +(2-tif->CurPixByte)]                  // color offset, reversed to blue, green, red sequence
                   =(BYTE)LzOutValue;                     // assign new color

       // advance to next pixel
       tif->CurPixByte++;                          // advance to next color
       if (tif->CurPixByte>=tif->BytesPerPixel) {  // next pixel
          tif->CurPixByte=0;
          tif->CurX++;
          if (tif->CurX>=tif->width) {    // next row
            tif->CurX=0;
            tif->CurY++;
          }
       }

    }

    return TRUE;
}

/*****************************************************************************
    TifHufDecompress:
    Decompress data encoded in Huffman coding with Tiff codes
******************************************************************************/
BOOL TifHufDecompress(PTERWND w,struct StrTif far *tif,struct StrLz far *lz,LPBYTE pStrip,int StripLen)
{
    long BitsRead=0,BitLen;
    int  ExtractBits,CodeLen,row,pix,i,RunType,BitsAfter,
         RunLength,ByteOffset,BitOffset,BytesRead,SaveLen,PixRemaining;
    BYTE Byte1,Byte2,Byte3;
    DWORD code,mask;
    BOOL terminated;

    BitLen=StripLen*8L;            // length of the buffer in bits

    for (row=0;row<tif->RowsPerStrip;row++) {    // extract each row
       BitsRead=8*((BitsRead+7)/8);              // start next row on byte boundary
       BytesRead=(int)(BitsRead/8);
       if (BytesRead>=StripLen) return TRUE;     // end of data

       ExtractBits=13;                           // maximum bit size of a code
       RunType=RUN_WHITE;                        // start with white run
       pix=0;                                    // number of pixels extracted
       terminated=FALSE;                         // TRUE when a run is terminated

       while (pix<tif->width || !terminated) {   // extract pixels in a row
          if ((BitLen-BitsRead)<(long)ExtractBits) ExtractBits=(int)(BitLen-BitsRead);
          if (ExtractBits<=0) return PrintError(w,MSG_TIFF,"Error extracting image(TifHufDecompress)");

          ByteOffset=(int)(BitsRead/8L);         // current byte
          BitOffset=(int)(BitsRead-(8L*ByteOffset)); // offset of current item within current byte

          Byte1=pStrip[ByteOffset];               // lowest order byte
          Byte2=pStrip[ByteOffset+1];             // middle byte
          Byte3=pStrip[ByteOffset+2];             // highest byte

          // Case when logical and memory/disk bit representations are the same.
          code=(((DWORD)Byte1)<<16)+(((WORD)Byte2)<<8)+Byte3;      // make dword

          BitsAfter=24-ExtractBits-BitOffset;      // bits after our bits
          if (BitsAfter>0) code=code>>BitsAfter;   // scroll of the bits of the next item

          mask=(1L<<ExtractBits)-1;                // mask for the current number of bits
          code=code&mask;

          // match the codes
          CodeLen=ExtractBits;
          while (CodeLen>0) {
             if (((WORD)code)>255) {               // code value do not exceed 255
                code=code>>1;                      // discard one bit
                CodeLen--;
                continue;
             }

             // match code value and code size to get the run length
             PixRemaining=tif->width-pix;          // number of pixel remaining

             if (RunType==RUN_WHITE) {      // scan white codes
                for (i=0;i<MAX_HUFF_CODES;i++) {
                   if (  HuffWhiteLen[i]==(BYTE)CodeLen
                      && HuffLen[i]<=PixRemaining
                      && HuffWhite[i]==(BYTE)code ) break;
                }
             }
             else if (RunType==RUN_BLACK) { // scan black code
                for (i=0;i<MAX_HUFF_CODES;i++) {
                   if (  HuffBlackLen[i]==(BYTE)CodeLen 
                      && HuffLen[i]<=PixRemaining
                      && HuffBlack[i]==(BYTE)code) break;
                }
             }

             if (i==MAX_HUFF_CODES) {       // match not found
                code=code>>1;               // discard one bit
                CodeLen--;                  // look for smaller code
                continue;
             }

             // Match found
             RunLength=SaveLen=HuffLen[i];          // length of this run

             // transfer the bits
             pix+=RunLength;                // number of pixels processed
             if (RunType==RUN_WHITE) LzOutValue=0xFF;
             else                    LzOutValue=0;
             if (tif->ImageType==0)  LzOutValue=(~LzOutValue)&0xFF;  // reverse

             while (RunLength>0) {
                if (RunLength>=8) LzBitsStuffed=8;
                else              LzBitsStuffed=RunLength;
             
                TifData(lz);

                RunLength-=LzBitsStuffed;
             }

             if (SaveLen<=63) {      // switch run type on termination code
                if (RunType==RUN_WHITE) RunType=RUN_BLACK;
                else                    RunType=RUN_WHITE;
                terminated=TRUE;
             }
             else terminated=FALSE;

             break;
          }

          if (CodeLen==0) return PrintError(w,MSG_TIFF,"Error extracting image(TifHufDecompress a)");

          BitsRead+=CodeLen;                        // update number of bits read
       }
    }

    return TRUE;
}

/*****************************************************************************
    TifGetOffsetBytes:
    Reads the specified number of bytes from the source at the given file offset.
******************************************************************************/
BOOL TifGetOffsetBytes(PTERWND w,struct StrTif far *tif,DWORD offset, LPVOID dest,long count)
{

    // empty the input buffers
    tif->StackLen=0;
    tif->TextLen=0;
    tif->TextIndex=0;
    tif->FilePos=offset;      // advance file position
    tif->BufIndex=offset;

    // set the file pointer at the offset
    if (tif->hFile!=INVALID_HANDLE_VALUE) {
       SetFilePointer(tif->hFile,offset,NULL,FILE_BEGIN);
    }

    // read the bytes
    if (!TifGetBytes(w,tif,dest,count)) return FALSE;

    return TRUE;
}

/*****************************************************************************
    TifGetBytes:
    Reads the specified number of bytes from the source
******************************************************************************/
BOOL TifGetBytes(PTERWND w,struct StrTif far *tif,LPVOID dest,long count)
{
    long l;

    for (l=0;l<count;l++) {
       if (!TifGetChar(w,tif)) return FALSE;  // get next byte
       if (dest) ((BYTE huge *)dest)[l]=tif->CurChar;
    }

    return TRUE;
}

/*****************************************************************************
    TifGetWord:
    Reads the next 2 characters and combine them to return a word value.
******************************************************************************/
BOOL TifGetWord(PTERWND w,struct StrTif far *tif)
{
    BYTE lo,hi;

    if (!TifGetChar(w,tif)) return FALSE;    // get low byte
    lo=tif->CurChar;

    if (!TifGetChar(w,tif)) return FALSE;    // get high byte
    hi=tif->CurChar;
    
    tif->CurWord=(WORD)( ( ((WORD)hi) <<8) +lo);

    return TRUE;
}

/*****************************************************************************
    TifGetChar:
    Reads the next characters from the input file or the input buffer.
    When the InSteam is NULL, the characters are read from the buffer.
    Returns FALSE when no more characters available.
******************************************************************************/
BOOL TifGetChar(PTERWND w,struct StrTif far *tif)
{
    // get the character from the character stack if available
    if (tif->StackLen>0) {
       tif->StackLen--;
       tif->CurChar=tif->stack[tif->StackLen];
       tif->FilePos++;       // advance file position
       return TRUE;
    }
    
    // read from the file
    tif->eof=FALSE;
    if (tif->hFile!=INVALID_HANDLE_VALUE) {
       if (tif->TextIndex<tif->TextLen) {  // read from the temporary buffer
          tif->CurChar=tif->text[tif->TextIndex];
          (tif->TextIndex)++;
          tif->FilePos++;    // advance file position
          return TRUE;
       }

       // read more characters from the file
       tif->TextLen=0;
       ReadFile(tif->hFile,TempString,MAX_WIDTH,&((DWORD)(tif->TextLen)),NULL);
    
       if (tif->TextLen==0) {
          tif->eof=TRUE;
          return FALSE;      // end of file
       }
       tif->TextIndex=0;
       FarMove(TempString,tif->text,tif->TextLen);

       tif->CurChar=tif->text[tif->TextIndex];
       (tif->TextIndex)++;
       tif->FilePos++;      // advance file position
       return TRUE;
    }
    else {              // read from the buffer
       if (tif->BufLen>=0 && tif->BufIndex>=tif->BufLen) {
          tif->eof=TRUE;
          return FALSE; // end of buffer
       }
       tif->CurChar=tif->buf[tif->BufIndex];
       (tif->BufIndex)++;
       tif->FilePos++;  // advance file position
       return TRUE;
    }
}

/*****************************************************************************
    TifPushChar:
    Push the lastly read character onto stack.
******************************************************************************/
BOOL TifPushChar(PTERWND w,struct StrTif far *tif)
{
    if (tif->StackLen>=MAX_WIDTH) return PrintError(w,MSG_TIFF,"Out of HTML character stack.");

    tif->stack[tif->StackLen]=tif->CurChar;
    (tif->StackLen)++;
    tif->FilePos--;  // decrement file position
    return TRUE;
}

/*****************************************************************************
    InitTifHuf:
    Initialize the Tiff table for Huffman decompression.
******************************************************************************/
InitTifHuf()
{
    int i=-1;

    for (i=0;i<MAX_HUFF_CODES;i++) {
       HuffLen[i]=0;
       HuffWhite[i]=HuffBlack[i]=HuffWhiteLen[i]=HuffBlackLen[i]=0;
    }

    // initialize each code
    i=-1;
    i++;SetTifHuf(i,0,0x35,0x08,0x37,0x0A);
    i++;SetTifHuf(i,1,0x07,0x06,0x02,0x03);
    i++;SetTifHuf(i,2,0x07,0x04,0x03,0x02);
    i++;SetTifHuf(i,3,0x08,0x04,0x02,0x02);
    i++;SetTifHuf(i,4,0x0B,0x04,0x03,0x03);
    i++;SetTifHuf(i,5,0x0C,0x04,0x03,0x04);
    i++;SetTifHuf(i,6,0x0E,0x04,0x02,0x04);
    i++;SetTifHuf(i,7,0x0F,0x04,0x03,0x05);
    i++;SetTifHuf(i,8,0x13,0x05,0x05,0x06);
    i++;SetTifHuf(i,9,0x14,0x05,0x04,0x06);
    i++;SetTifHuf(i,10,0x07,0x05,0x04,0x07);
    i++;SetTifHuf(i,11,0x08,0x05,0x05,0x07);
    i++;SetTifHuf(i,12,0x08,0x06,0x07,0x07);
    i++;SetTifHuf(i,13,0x03,0x06,0x04,0x08);
    i++;SetTifHuf(i,14,0x34,0x06,0x07,0x08);
    i++;SetTifHuf(i,15,0x35,0x06,0x18,0x09);
    i++;SetTifHuf(i,16,0x2A,0x06,0x17,0x0A);
    i++;SetTifHuf(i,17,0x2B,0x06,0x18,0x0A);
    i++;SetTifHuf(i,18,0x27,0x07,0x08,0x0A);
    i++;SetTifHuf(i,19,0x0C,0x07,0x67,0x0B);
    i++;SetTifHuf(i,20,0x08,0x07,0x68,0x0B);
    i++;SetTifHuf(i,21,0x17,0x07,0x6C,0x0B);
    i++;SetTifHuf(i,22,0x03,0x07,0x37,0x0B);
    i++;SetTifHuf(i,23,0x04,0x07,0x28,0x0B);
    i++;SetTifHuf(i,24,0x28,0x07,0x17,0x0B);
    i++;SetTifHuf(i,25,0x2B,0x07,0x18,0x0B);
    i++;SetTifHuf(i,26,0x13,0x07,0xCA,0x0C);
    i++;SetTifHuf(i,27,0x24,0x07,0xCB,0x0C);
    i++;SetTifHuf(i,28,0x18,0x07,0xCC,0x0C);
    i++;SetTifHuf(i,29,0x02,0x08,0xCD,0x0C);
    i++;SetTifHuf(i,30,0x03,0x08,0x68,0x0C);
    i++;SetTifHuf(i,31,0x1A,0x08,0x69,0x0C);
    i++;SetTifHuf(i,32,0x1B,0x08,0x6A,0x0C);
    i++;SetTifHuf(i,33,0x12,0x08,0x6B,0x0C);
    i++;SetTifHuf(i,34,0x13,0x08,0xD2,0x0C);
    i++;SetTifHuf(i,35,0x14,0x08,0xD3,0x0C);
    i++;SetTifHuf(i,36,0x15,0x08,0xD4,0x0C);
    i++;SetTifHuf(i,37,0x16,0x08,0xD5,0x0C);
    i++;SetTifHuf(i,38,0x17,0x08,0xD6,0x0C);
    i++;SetTifHuf(i,39,0x28,0x08,0xD7,0x0C);
    i++;SetTifHuf(i,40,0x29,0x08,0x6C,0x0C);
    i++;SetTifHuf(i,41,0x2A,0x08,0x6D,0x0C);
    i++;SetTifHuf(i,42,0x2B,0x08,0xDA,0x0C);
    i++;SetTifHuf(i,43,0x2C,0x08,0xDB,0x0C);
    i++;SetTifHuf(i,44,0x2D,0x08,0x54,0x0C);
    i++;SetTifHuf(i,45,0x04,0x08,0x55,0x0C);
    i++;SetTifHuf(i,46,0x05,0x08,0x56,0x0C);
    i++;SetTifHuf(i,47,0x0A,0x08,0x57,0x0C);
    i++;SetTifHuf(i,48,0x0B,0x08,0x64,0x0C);
    i++;SetTifHuf(i,49,0x52,0x08,0x65,0x0C);
    i++;SetTifHuf(i,50,0x53,0x08,0x52,0x0C);
    i++;SetTifHuf(i,51,0x54,0x08,0x53,0x0C);
    i++;SetTifHuf(i,52,0x55,0x08,0x24,0x0C);
    i++;SetTifHuf(i,53,0x24,0x08,0x37,0x0C);
    i++;SetTifHuf(i,54,0x25,0x08,0x38,0x0C);
    i++;SetTifHuf(i,55,0x58,0x08,0x27,0x0C);
    i++;SetTifHuf(i,56,0x59,0x08,0x28,0x0C);
    i++;SetTifHuf(i,57,0x5A,0x08,0x58,0x0C);
    i++;SetTifHuf(i,58,0x5B,0x08,0x59,0x0C);
    i++;SetTifHuf(i,59,0x4A,0x08,0x2B,0x0C);
    i++;SetTifHuf(i,60,0x4B,0x08,0x2C,0x0C);
    i++;SetTifHuf(i,61,0x32,0x08,0x5A,0x0C);
    i++;SetTifHuf(i,62,0x33,0x08,0x66,0x0C);
    i++;SetTifHuf(i,63,0x34,0x08,0x67,0x0C);
    i++;SetTifHuf(i,64,0x1b,0x05,0x0F,0x0A);
    i++;SetTifHuf(i,128,0x12,0x05,0xC8,0x0C);
    i++;SetTifHuf(i,192,0x17,0x06,0xC9,0x0C);
    i++;SetTifHuf(i,256,0x37,0x07,0x5B,0x0C);
    i++;SetTifHuf(i,320,0x36,0x08,0x33,0x0C);
    i++;SetTifHuf(i,384,0x37,0x08,0x34,0x0C);
    i++;SetTifHuf(i,448,0x64,0x08,0x35,0x0C);
    i++;SetTifHuf(i,512,0x65,0x08,0x6C,0x0D);
    i++;SetTifHuf(i,576,0x68,0x08,0x6D,0x0D);
    i++;SetTifHuf(i,640,0x67,0x08,0x4A,0x0D);
    i++;SetTifHuf(i,704,0xCC,0x09,0x4B,0x0D);
    i++;SetTifHuf(i,768,0xCD,0x09,0x4C,0x0D);
    i++;SetTifHuf(i,832,0xD2,0x09,0x4D,0x0D);
    i++;SetTifHuf(i,896,0xD3,0x09,0x72,0x0D);
    i++;SetTifHuf(i,960,0xD4,0x09,0x73,0x0D);
    i++;SetTifHuf(i,1024,0xD5,0x09,0x74,0x0D);
    i++;SetTifHuf(i,1088,0xD6,0x09,0x75,0x0D);
    i++;SetTifHuf(i,1152,0xD7,0x09,0x76,0x0D);
    i++;SetTifHuf(i,1216,0xD8,0x09,0x77,0x0D);
    i++;SetTifHuf(i,1280,0xD9,0x09,0x52,0x0D);
    i++;SetTifHuf(i,1344,0xDA,0x09,0x53,0x0D);
    i++;SetTifHuf(i,1408,0xDB,0x09,0x54,0x0D);
    i++;SetTifHuf(i,1472,0x98,0x09,0x55,0x0D);
    i++;SetTifHuf(i,1536,0x99,0x09,0x5A,0x0D);
    i++;SetTifHuf(i,1600,0x9A,0x09,0x5B,0x0D);
    i++;SetTifHuf(i,1664,0x18,0x06,0x64,0x0D);
    i++;SetTifHuf(i,1728,0x9B,0x09,0x65,0x0D);
    i++;SetTifHuf(i,1792,0x08,0x0B,0x08,0x0B);
    i++;SetTifHuf(i,1856,0x0C,0x0B,0x0C,0x0B);
    i++;SetTifHuf(i,1920,0x0D,0x0B,0x0D,0x0B);
    i++;SetTifHuf(i,1984,0x12,0x0C,0x12,0x0C);
    i++;SetTifHuf(i,2048,0x13,0x0C,0x13,0x0C);
    i++;SetTifHuf(i,2112,0x14,0x0C,0x14,0x0C);
    i++;SetTifHuf(i,2176,0x15,0x0C,0x15,0x0C);
    i++;SetTifHuf(i,2240,0x16,0x0C,0x16,0x0C);
    i++;SetTifHuf(i,2304,0x17,0x0C,0x17,0x0C);
    i++;SetTifHuf(i,2368,0x1C,0x0C,0x1C,0x0C);
    i++;SetTifHuf(i,2432,0x1D,0x0C,0x1D,0x0C);
    i++;SetTifHuf(i,2496,0x1E,0x0C,0x1E,0x0C);
    i++;SetTifHuf(i,2560,0x1F,0x0C,0x1F,0x0C);

    if ((i+1)!=MAX_HUFF_CODES) {
       MessageBeep(0);
       MessageBox(NULL,"Error loading TIFF Hufman codes!",NULL,MB_OK);
    }

    HuffInitialized=true;

    return TRUE;
}

/*****************************************************************************
    SetTifHuf:
    Initialize one Huffman code for Tiff implementation
******************************************************************************/
SetTifHuf(int index,int len,BYTE white,BYTE WhiteLen,BYTE black,BYTE BlackLen)
{
    if (index<0 || index>=MAX_HUFF_CODES) return FALSE;

    HuffLen[index]=len;
    HuffWhite[index]=white;
    HuffWhiteLen[index]=WhiteLen;
    HuffBlack[index]=black;
    HuffBlackLen[index]=BlackLen;

    return TRUE;
}

/*****************************************************************************
    ToIntelInt:
    Convert a Motorala integer to the Intel integer.
******************************************************************************/
WORD ToIntelInt(WORD in)
{
    BYTE byte1,byte2;
    WORD out;

    // extract bytes
    byte1=(BYTE)(in&0xFF);        // extract first byte
    in>>=8;
    byte2=(BYTE)(in&0xFF);         // extract second byte

    // build output
    out=byte1;
    out<<=8;
    out|=byte2;

    return out;
}

/*****************************************************************************
    ToIntelLong:
    Convert a Motorala long integer to the Intel integer.
******************************************************************************/
DWORD ToIntelLong(DWORD in)
{
    BYTE byte1,byte2,byte3,byte4;
    DWORD out;

    // extract bytes
    byte1=(BYTE)(in&0xFF);        // extract first byte
    in>>=8;
    byte2=(BYTE)(in&0xFF);         // extract second byte
    in>>=8;
    byte3=(BYTE)(in&0xFF);         // extract third byte
    in>>=8;
    byte4=(BYTE)(in&0xFF);         // extract fourth byte

    // build output
    out=byte1;
    out<<=8;
    out|=byte2;
    out<<=8;
    out|=byte3;
    out<<=8;
    out|=byte4;

    return out;
}

/*****************************************************************************
    LZW decompression routines.
******************************************************************************/

/*****************************************************************************
    LzInit:
    Initialize the decompression routines.
******************************************************************************/
BOOL LzInit(PTERWND w, struct StrLz far *lz)
{
    int MaxCodes;

    // initilize the variables
    MaxCodes=(1<<(LzMaxInBits));
    LzMapSize=sizeof(struct StrLzMap)*MaxCodes;  // to hold lz map MaxCodes

    LzEof=FALSE;                 // true at eof of input file
    LzCurBufSize=0;              // number of bytes read in the input buffer
    LzBitsRead=0;                // bummber of bits read or writter from the io buffers

    // create the input map buffer
    if (NULL==(LzMap=OurAlloc(LzMapSize+1))) return PrintError(w,MSG_OUT_OF_MEM,"LzInit");
    if (NULL==(LzChar=OurAlloc(sizeof(int)*MaxCodes))) return PrintError(w,MSG_OUT_OF_MEM,"LzInit");

    LzInitMap(lz);                 // initialize the gif map

    // create the input buffer
    if (NULL==(LzInBuf=OurAlloc(LzMaxInBufSize+4))) return PrintError(w,MSG_OUT_OF_MEM,"LzInit");

    return TRUE;
}

/*****************************************************************************
    LzDecompress:
    decompress the input stream and write it to the output stream
******************************************************************************/
BOOL LzDecompress(struct StrLz far *lz,LPBYTE input, int size)
{
    int code,CurChar,CurCode;
    int i;

    if (LzEof) return TRUE;          // end code encountered

    // append the input to the existing buffer
    FarMove(input,&(LzInBuf[LzCurBufSize]),size);
    LzCurBufSize+=size;
    
    // read input codes and process
    while (LzReadCode(lz,&code)) {

       if (code==LzClearCode) LzInitMap(lz);

       // handle the first code
       else if (LzCleared) {
           LzWrite(lz,code);
           LzPrevChar=LzPrevCode=code;
           LzCleared=FALSE;
       }
       else {
           CurCode=code;

           if (code>=LzNextCode) {
              CurCode=LzPrevCode;
              LzChar[LzCharCount]=LzPrevChar;
              LzCharCount++;
           }
           
           // extract characters from the code
           LzExtractString(lz,CurCode);
           CurChar=LzChar[LzCharCount-1];     // first character of the string
       
           // output the characters
           for (i=LzCharCount-1;i>=0;i--) LzWrite(lz,LzChar[i]);
           LzCharCount=0;

           LzAddChar(lz,LzPrevCode,CurChar);

           LzPrevCode=code;
           LzPrevChar=CurChar;
       }
    }

    return TRUE;
}

/*****************************************************************************
    LzExtractString:
    Extract the characters from the prefix code.
******************************************************************************/
BOOL LzExtractString(struct StrLz far *lz, int code) 
{
    while (code>=0) {
       LzChar[LzCharCount]=LzMap[code].chr;
       LzCharCount++;
       code=LzMap[code].prev;
    }

    return TRUE;
}

/*****************************************************************************
    LzAddChar:
    This function pairs the given char 'chr' with the prefix code.  The next
    free entry is used for pairing.
******************************************************************************/
BOOL LzAddChar(struct StrLz far *lz,int prev, int chr)
{
    // note this latest prefix in the beginning of the string
    LzMap[LzNextCode].chr=chr;
    LzMap[LzNextCode].prev=prev;   // maintain backward path

    LzNextCode++;
    if (LzNextCode>((1L<<LzMaxInBits))) return FALSE;

    if (LzImageType==PICT_TIFF) {
       if (LzNextCode==((1<<LzCurInBits)-1)) {
          if (LzCurInBits<LzMaxInBits) LzCurInBits++;
       }
    }
    else {
       if (LzNextCode==(1<<LzCurInBits)) {
          if (LzCurInBits<LzMaxInBits) LzCurInBits++;
       }
    }

    
    return TRUE;
}

/*****************************************************************************
    LzReadCode:
    Read next code from the input stream.
******************************************************************************/
BOOL LzReadCode(struct StrLz far *lz,LPINT NewCode)
{
    int   BytesNeededInBuffer,ByteOffset,BitOffset,BitsAfter;
    DWORD code,mask;
    BYTE Byte1,Byte2,Byte3;


    // check if input buffer contains the next code
    BytesNeededInBuffer=(int)((LzBitsRead+LzCurInBits)/8);   //convert to bytes
    if ((BytesNeededInBuffer*8L)<(LzBitsRead+LzCurInBits)) BytesNeededInBuffer++;

    if (BytesNeededInBuffer>LzCurBufSize) {
       if (LzEof) return FALSE;                   // end of file

       // if the last byte not completely read
       // move the last 2 bytes to the beginning
       if ((LzCurBufSize*8L)>LzBitsRead) {
          int BytesSaved;
          if (LzCurBufSize>=2) {
             BytesSaved=2;
             LzInBuf[0]=LzInBuf[LzCurBufSize-2];
             LzInBuf[1]=LzInBuf[LzCurBufSize-1];
          }
          else {
             BytesSaved=1;
             LzInBuf[0]=LzInBuf[LzCurBufSize-1];
          }

          LzBitsRead=LzBitsRead-(LzCurBufSize-BytesSaved)*8L; // number of bits read in the new buffer
          LzCurBufSize=BytesSaved;                           // new buffer size
       }
       else LzBitsRead=LzCurBufSize=0;

       return FALSE;            // wait for the next stream
    }

    // extract the bits to return
    // Note that a logical H22H21 L22L21 H12H11 L12L11 sequence appears as
    // following in memory or disk image:
    // L11L12 H12H11 L22L21 H22H21
    // This behavior is caused by right to left bit packing within a byte in
    // Intel microprocessors.

    ByteOffset=(int)(LzBitsRead/8L);         // current byte
    BitOffset=(int)(LzBitsRead-(8L*ByteOffset)); // offset of current item within current byte

    Byte1=LzInBuf[ByteOffset];               // lowest order byte
    Byte2=LzInBuf[ByteOffset+1];             // middle byte
    Byte3=LzInBuf[ByteOffset+2];             // highest byte

    if (LzImageType==PICT_TIFF) {
       // Case when logical and memory/disk bit representations are the same.
       code=(((DWORD)Byte1)<<16)+(((WORD)Byte2)<<8)+Byte3;      // make dword

       BitsAfter=24-LzCurInBits-BitOffset;      // bits after our bits
       if (BitsAfter>0) code=code>>BitsAfter;   // scroll of the bits of the next item

       mask=(1L<<LzCurInBits)-1;                 // mask for the current number of bits
       code=code&mask;
    }
    else {
       // Case when logical H22H21 L22L21 H12H11 L12L11 is stored in the
       // memory or disk as L11L12 H12H11 L22L21 H22H21

       code=(((DWORD)Byte3)<<16)+(((WORD)Byte2)<<8)+Byte1;      // make dword

       if (BitOffset>0) code=code>>BitOffset;   // remove previously read bits

       code=code<<(8*sizeof(DWORD)-LzCurInBits);      // remove bits that belong to the next item
       code=code>>(8*sizeof(DWORD)-LzCurInBits);      
    }

    LzBitsRead+=LzCurInBits;                    // update number of bits read


    // handle eof and clear codes here
    if (code==(DWORD)LzEofCode) {
       LzEof=TRUE;
       return FALSE;                        // end of file
    }

    (*NewCode)=(WORD)code;                  // return code

    return TRUE;
}

/*****************************************************************************
    LzWrite:
    Write the character
******************************************************************************/
BOOL LzWrite(struct StrLz far *lz,int val)
{

    LzOutValue=val;
    LzOutputCallback(lz);

    return TRUE;
} 

/*****************************************************************************
    LzExit:
    Release resources after decompression
******************************************************************************/
BOOL LzExit(struct StrLz far *lz)
{
   OurFree(LzMap);
   OurFree(LzChar);
   OurFree(LzInBuf);

   return TRUE;
}

/*****************************************************************************
    LzInitMap:
    Initialize the lz map
******************************************************************************/
BOOL LzInitMap(struct StrLz far *lz)
{
    BYTE huge *ptr;
    long l;
    int  EntryCount=(1<<(LzMaxInBits));
    int  i;

    ptr=(BYTE huge *)LzMap;
    for (l=0;l<LzMapSize;l++) ptr[l]=0xFF;
    for (i=0;i<=LzLastChar;i++) LzMap[i].chr=i;
    
    LzNextCode=LzFirstCode;  // set the first code
    LzCurInBits=LzBeginInBits;
    LzCleared=TRUE;
    LzCharCount=0;

    return TRUE;
}


