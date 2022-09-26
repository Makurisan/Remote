
#include "pdferr.h"
#include "pdfdef.h"
#include "pdffnc.h"
#include "pdfimg.h"
#include "zlib.h"
#include "PDFBaseFont.h"
#include "PDFFontDesc.h"
#include "PDFTTFFont.h"
#include "PDFDocument.h" // MIK
#include <stdio.h>
#include <stdarg.h>
#include <string>

using namespace std;

// already defined in pdferr.h
//static short g_sInit = false_a;

#define           FirstPageToShow      0
#define           LinearizeObjSize     173

void PDFRemoveObsoleteObj(PDFDocPTR i_pPDFDoc, long i_lPage);
long PDFGetFontPos(PDFStream *i_pStream, CPDFBaseFont *i_pFont);
long PDFGetImagePos(PDFStream *i_pStream, CPDFImage *i_pImg);


CPDFDocument *PDFGetDocClass(PDFDocPTR i_pPDFDoc)
{
  CPDFDocument *pDoc;
  pDoc = (CPDFDocument *)i_pPDFDoc;
  return pDoc;
}

bool_a PDFInit(PDFDocPTR i_pPDFDoc)
{
  i_pPDFDoc->bFlushToFile = true_a;
  //PDF level
  i_pPDFDoc->lMajorLevel = 1;
  i_pPDFDoc->lMinorLever = 4;
  //init info
  i_pPDFDoc->strInfo.czAuthor[0] = '\0';
  i_pPDFDoc->strInfo.czCreationDate[0] = '\0';
  i_pPDFDoc->strInfo.czModDate[0] = '\0';
  i_pPDFDoc->strInfo.czCreator[0] = '\0';
  i_pPDFDoc->strInfo.czProducer[0] = '\0';
  i_pPDFDoc->strInfo.czTitle[0] = '\0';
  i_pPDFDoc->strInfo.czSubject[0] = '\0';
  i_pPDFDoc->strInfo.czKeywords[0] = '\0';
  //ViewerPreferences
  i_pPDFDoc->strCatalog.strViewerPreferences.bHideToolbar = 0;
  i_pPDFDoc->strCatalog.strViewerPreferences.bHideMenubar = 0;
  i_pPDFDoc->strCatalog.strViewerPreferences.bHideWindowUI = 0;
  i_pPDFDoc->strCatalog.strViewerPreferences.bFitWindow = 0;
  i_pPDFDoc->strCatalog.strViewerPreferences.bCenterWindow = 0;
  i_pPDFDoc->strCatalog.strViewerPreferences.eNonFullScreenPageMode = eUseNone;
  i_pPDFDoc->strCatalog.strViewerPreferences.bDirection = false_a;
  //linearized
  i_pPDFDoc->strLinearized.lLength = NOT_USED;
  i_pPDFDoc->strLinearized.lHintOff = NOT_USED;
  i_pPDFDoc->strLinearized.lHintLen = NOT_USED;
  i_pPDFDoc->strLinearized.lPageObjNum = NOT_USED;
  i_pPDFDoc->strLinearized.lPageEndOff = NOT_USED;
  i_pPDFDoc->strLinearized.lPagesNum = NOT_USED;
  i_pPDFDoc->strLinearized.lWSOff = NOT_USED;
  //Catalog
  i_pPDFDoc->strCatalog.ePageMode = eUseNone;
  i_pPDFDoc->strCatalog.ePageLayout = eOneColumn;

  i_pPDFDoc->compressionON = 0;
  i_pPDFDoc->streamFilterList = NULL;
  i_pPDFDoc->bTextObj = false_a;

  i_pPDFDoc->pDocFunc = NULL;

  PDFGetDocClass(i_pPDFDoc)->SetActualFontID(0);

  i_pPDFDoc->imageFlagBCI = 0;    // bit-0 (/ImageB), bit-1 (/ImageC), bit-2 (/ImageI) 
  i_pPDFDoc->lOutlineEntriesCount = 0;    // total # of outline (book mark) entries 
  i_pPDFDoc->pFirstOutlineEntry = NULL;    // pointer to first outline entry 
  i_pPDFDoc->pLastOutlineEntry = NULL;    // pointer to last outline entry 

  i_pPDFDoc->hex_string = 0;// Current string mode: HEX or ASCII
  PDFGetDocClass(i_pPDFDoc)->SetActualFontSize(PDFConvertDimensionFromDefault(DEFFONTHEIGHT));
  PDFGetDocClass(i_pPDFDoc)->SetActualFontStyle(CPDFBaseFont::ePDFStandard);
  i_pPDFDoc->fWordSpacing = 0;
  i_pPDFDoc->lDefTabSpace = PDF_DEFAULT_TAB_SPACE;
  i_pPDFDoc->cTabDecimalType = PDF_DEFAULT_TAB_DECIMAL_COMMA;
  i_pPDFDoc->lLetterSpace = 0;
  i_pPDFDoc->horiz_scaling = 100;
  PDFresetTextCTM(i_pPDFDoc);


  i_pPDFDoc->m_lActualPage = 0;
  i_pPDFDoc->pOutputFile = NULL;       // Output file 
  i_pPDFDoc->lActualSize = 0;    // # of bytes written, or offset of next object 
  i_pPDFDoc->startXref = 0;      // offset of xref 


  memset(i_pPDFDoc->ucOwnerPassw, 0, 33);
  memset(i_pPDFDoc->ucUserPassw, 0, 33);
  memset(i_pPDFDoc->ucEncrKey, 0, 33);
  memset(i_pPDFDoc->ucO, 0, 33);
  memset(i_pPDFDoc->ucU, 0, 33);
  i_pPDFDoc->iPermission = 0;
  memset(i_pPDFDoc->ucFileID, 0, 33);

  memset(i_pPDFDoc->strInfo.czAuthor, 0, NAME_SIZE);
  memset(i_pPDFDoc->strInfo.czCreator, 0, NAME_SIZE);
  memset(i_pPDFDoc->strInfo.czKeywords, 0, NAME_SIZE);
  memset(i_pPDFDoc->strInfo.czTitle, 0, NAME_SIZE);
  memset(i_pPDFDoc->strInfo.czProducer, 0, NAME_SIZE);
  strcpy(i_pPDFDoc->strInfo.czProducer, "X-Publisher, XPubPDFLib Alpha-Version 1.0");

  return true_a;
}

bool_a PDFEndDoc(PDFDocPTR *i_pPDFDoc)
{
  bool_a bRet = true_a;
  PDFDocPTR pPDFDoc = *i_pPDFDoc;

  if (pPDFDoc->bFlushToFile && pPDFDoc->pDocStream && pPDFDoc->pDocStream->pStream)
  {
    if (0 == pPDFDoc->m_strFileName.size())
      bRet = false_a;
    pPDFDoc->pOutputFile = fopen(pPDFDoc->m_strFileName.c_str(), BINARY_WRITE);
    if(pPDFDoc->pOutputFile == NULL)
      bRet = false_a;

    fwrite(pPDFDoc->pDocStream->pStream, 1, pPDFDoc->pDocStream->curSize, pPDFDoc->pOutputFile);

    if(pPDFDoc->pOutputFile)
    {
      fclose(pPDFDoc->pOutputFile);
      pPDFDoc->pOutputFile = NULL;
    }
  }

  return bRet;
}

bool_a PDFSetCurrentPage(PDFDocPTR i_pPDFDoc, long i_lPage)
{
  PDFPage *pPage = NULL;

  if (i_lPage <= 0 || i_lPage > PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;
  if (i_lPage - 1 == i_pPDFDoc->m_lActualPage)
    return true_a;
  pPage = i_pPDFDoc->m_vctPages[i_lPage - 1];
  if (!pPage)
    return false_a;
  if (pPage->lPageNum == NOT_USED)
  {
    return false_a;
  }
  else
    if (pPage->bCompressed)
    {
      return false_a;
    }
  i_pPDFDoc->m_lActualPage = i_lPage - 1;

  return true_a;
}

bool_a PDFEndPage(PDFDocPTR in_docHandle, long page)
{
  PDFPage *pPage = NULL;
  if (page <= 0 || page > PDFGetDocClass(in_docHandle)->GetPageCount())
    return false_a;
  pPage = in_docHandle->m_vctPages[page - 1];
  if (!pPage)
    return false_a;
  if ((pPage->lPageNum != NOT_USED) && (!(pPage->bCompressed)))
  {

#ifdef __PAGE_BACKGROUND__
  float fYSize = (float)(pPage->recMediaBox.lTop - pPage->recMediaBox.lBottom);
  float fXSize = (float)(pPage->recMediaBox.lRight - pPage->recMediaBox.lLeft);
  float fRot, fLen, fPosX, fPosY, fFSize, fSin, fCos;
  float fTemp = fYSize / fXSize;
  char czTemp[200] = {0}, czT[200] = {0};
  long lLen = 0;
  fRot = (float)atan(fTemp);

  fLen = (float)cos(fRot);
  fLen = fXSize / fLen;


  fFSize = fLen / 160.f;

  PDFAddFontToPage(in_docHandle, 0, pPage->lPageNum);

  sprintf(czT, "/Fpdf0 %.4f Tf\n", fFSize);
  strcat(czTemp, czT);
  lLen += PDFStrLenC(czT);

  sprintf(czTemp + lLen, "BT\n");
  lLen += 3;
  sprintf(czT, "%.4f %.4f %.4f rg\n", 0.8f, 0.8f, 0.8f);
  strcat(czTemp, czT);
  lLen += PDFStrLenC(czT);


  fCos = (float)cos(fRot);
  fSin = (float)sin(fRot);


  if (fTemp > 1.f)
  {
    fPosY = fFSize;
    fPosX = fPosY / fTemp;
    fPosX += fFSize / 2;
  }
  else if (fTemp < 1.f)
  {
    fPosX = fFSize;
    fPosY = fPosX * fTemp;
    fPosY -= fFSize / 2;
  }
  else
  {
    fPosY = fFSize;
    fPosX = fFSize;
  }

  sprintf(czT, "%.4f %.4f %.4f %.4f %.4f %.4f Tm\n", fCos, fSin, -fSin, fCos, fPosX, fPosY);
  strcat(czTemp, czT);
  lLen += PDFStrLenC(czT);

  sprintf(czT, "(%s) %s\n", czBACKGROUNDTEXT, czSTRINGTJ);
  strcat(czTemp, czT);
  lLen += PDFStrLenC(czT);

  sprintf(czTemp + lLen, "ET\n");
  lLen += 3;

  memset(czT, ' ', lLen);

  PDFWriteToPageStream(in_docHandle, czT, lLen, pPage->lPageNum);

  memmove(pPage->pPageStream->pStream + lLen, pPage->pPageStream->pStream, pPage->pPageStream->curSize - lLen);
  memcpy(pPage->pPageStream->pStream, czTemp, lLen);

#endif

  PDFRemoveObsoleteObj(in_docHandle, page);

    PDFcloseContentMemStreamForPage(in_docHandle, page);
    return true_a;
  }
  return false_a;
}

bool_a PDFDeletePage(PDFDocPTR i_pPDFDoc, long i_iPage)
{
  PDFPage *pPage = NULL;
  PDFOutlineEntry *pOutlineEntry = NULL;

  if (i_iPage <= 0 || i_iPage > PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;

  pPage = i_pPDFDoc->m_vctPages[i_iPage - 1];
  if (!pPage)
    return false_a;

  pOutlineEntry = i_pPDFDoc->pFirstOutlineEntry;
  while (pOutlineEntry)
  {
    if (pOutlineEntry->lPage == i_iPage)
    {
      if (pOutlineEntry->pPrev)
        pOutlineEntry->pPrev->pNext = pOutlineEntry->pNext;
      else
        i_pPDFDoc->pFirstOutlineEntry = pOutlineEntry->pNext;
      if (pOutlineEntry->pNext)
        pOutlineEntry->pNext->pPrev = pOutlineEntry->pPrev;
      free(pOutlineEntry); // TO DO: this is not OK
      pOutlineEntry = NULL;
    }
    else
      pOutlineEntry = pOutlineEntry->pNext;
  }

  return true_a;
}

void PDFRemoveObsoleteObj(PDFDocPTR i_pPDFDoc, long i_lPage)
{
  PDFPage *pPage = NULL;
  PDFList *pList = NULL, *pListOld = NULL;
  CPDFBaseFont *pFont = NULL;
  CPDFImage *pImg = NULL;
  long lPos = 0, lOut;
  if (i_lPage <= 0 || i_lPage > PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return;
  pPage = i_pPDFDoc->m_vctPages[i_lPage - 1];
  if (!pPage)
    return;
  if ((pPage->lPageNum != NOT_USED) && (!(pPage->bCompressed)))
  {
    lOut = 0;
    if(pPage->lFontCount)
    {
      pList = pPage->pFontIdx;
      while(pList != NULL)
      {
        pFont = PDFGetDocClass(i_pPDFDoc)->GetFont(pList->lValue);

        lPos = PDFGetFontPos(pPage->pPageStream, pFont);
        if (lPos < 0 && pList->lValue >= 0)
        {
          pPage->lFontCount--;
          if (pPage->pFontIdx == pList)
          {
            pPage->pFontIdx = pList->pNext;
            free(pList);
            pList = pPage->pFontIdx;
            continue;
          }
          else
          {
            pListOld->pNext = pList->pNext;
            free(pList);
            pList = pListOld->pNext;
            continue;
          }
        }

        pListOld = pList;
        pList = pList->pNext;
      }
    }
    if(pPage->lImageCount)
    {
      pList = pPage->pImageIdx;
      while(pList != NULL)
      {
        pImg = PDFGetDocClass(i_pPDFDoc)->GetImage(pList->lValue);

        lPos = PDFGetImagePos(pPage->pPageStream, pImg);
        if (lPos < 0)
        {
          if (pPage->pImageIdx == pList)
          {
            pPage->pImageIdx = pList->pNext;
            free(pList);
            pList = pPage->pImageIdx;
            continue;
          }
          else
          {
            pListOld->pNext = pList->pNext;
            free(pList);
            pList = pListOld->pNext;
            continue;
          }
        }

        pListOld = pList;
        pList = pList->pNext;
      }
    }
/*
    if (pPage->lAnnotCount)
    {
      iL = pPage->pAnnotIdx;
      while(iL != NULL)
      {
        if(iL->lValue >= 0)
        {
          iL->lValue;
        }
        iL = iL->pNext;
      }
    }
*/
  }
  return;
}

long PDFGetFontPos(PDFStream *i_pStream, CPDFBaseFont *i_pFont)
{
  long lRet = NOT_USED;
  char *pTemp = NULL;
  if (!i_pStream || !i_pStream->curSize || !i_pStream->pStream || !i_pFont)
    return lRet;
  pTemp = strstr(i_pStream->pStream, i_pFont->GetFontResName());
  if (pTemp)
    lRet = pTemp - i_pStream->pStream + 1;
  return lRet;
}

long PDFGetImagePos(PDFStream *i_pStream, CPDFImage *i_pImg)
{
  long lRet = NOT_USED;
  char *pTemp = NULL;
  if (!i_pStream || !i_pStream->curSize || !i_pStream->pStream || !i_pImg)
    return lRet;
  pTemp = strstr(i_pStream->pStream, i_pImg->name);
  if (pTemp)
    lRet = pTemp - i_pStream->pStream + 1;
  return lRet;
}

long PDFcloseContentMemStreamForPage(PDFDocPTR pdf, long page)
{
Byte *inputBuf, *outputBuf;
PDFPage *pInf;
uLong comprLen = 0;
size_t filesize;
float headroom;
long iheadroom;
size_t bufSize;
long err;

  if (page <= 0 || page > PDFGetDocClass(pdf)->GetPageCount())
    return -1;

  pInf = pdf->m_vctPages[page - 1];
  inputBuf = (Byte *)pInf->pPageStream->pStream;
  filesize = pInf->pPageStream->curSize;
  bufSize = pInf->pPageStream->allocSize;

  if(pdf->compressionON)
  {
    headroom = (float)(filesize * 0.001 + 16.0);  /* 0.1% plus 12 bytes */
    iheadroom = (long)headroom;
    /* fprintf(stderr, "filesize=%d, bufSize=%d, headroom=%d\n", filesize, bufSize, iheadroom); */
    comprLen = (uLong) filesize + iheadroom;    /* available buf size - must set this for input */
    outputBuf = (Byte *)PDFMalloc(comprLen);  /* working buffer */
    if (!outputBuf)
      return ERR_MALLOC_FAIL;
    err = compress(outputBuf, &comprLen, inputBuf, (uLong) filesize);  /* #### COMPRESSION #### */
    if(err != Z_OK)
    {
    }
    pInf->pucCompStream = (unsigned char *)PDFMalloc((comprLen+1));
    if (!(pInf->pucCompStream))
      return ERR_MALLOC_FAIL;
    memcpy((void *)pInf->pucCompStream, (void *)outputBuf, (size_t)comprLen);
    free(outputBuf);      /* working buffer not needed now */
    pInf->lCompLength = comprLen;  /* indicate compressed data length */
    pInf->bCompressed = true_a;

    if(pInf->pPageStream)
    {
      PDFDeleteStream(&(pInf->pPageStream));  /* free uncompressed memory stream */
      pInf->pPageStream = NULL;
    }
  }
  else
  {
  }
  return(0);
}






//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
char *PDFGetDocStream(PDFDocPTR i_pPDFDoc)
{
  if (!i_pPDFDoc || !(i_pPDFDoc->pDocStream))
    return NULL;
  return i_pPDFDoc->pDocStream->pStream;
}

long PDFWriteFormatedBufToDocStream(PDFDocPTR i_pPDFDoc, const char *i_pczFormat, ...)
{
  va_list args;
  char* pczBuffer;
  long lCount, lRet = 0;
  long size = 4096;
  pczBuffer = (char*)PDFMalloc(size * sizeof(char));
  if (!pczBuffer)
    return lRet;
  va_start(args, i_pczFormat);

#if defined(LINUX_OS) || defined(MAC_OS_MWE) || defined(__LINUX__)
  lCount = vsnprintf(pczBuffer, size, i_pczFormat, args);
#elif defined(WIN32)
  lCount = _vsnprintf(pczBuffer, size, i_pczFormat, args);
#else
#error "not implemented"
#endif

  va_end(args);
  lRet = PDFWriteBufToDocStream((PDFDocPTR)i_pPDFDoc, pczBuffer, lCount);
  free(pczBuffer);
  return lRet;
}

long PDFWriteBufToDocStream(PDFDocPTR i_pPDFDoc, const char *i_pczBuf, size_t i_Size)
{
  PDFStream *pStream = NULL;
  long lRet = 0;
  if (NULL == i_pPDFDoc || NULL == i_pczBuf || NULL == i_pPDFDoc->pDocStream)
    return lRet;

  pStream = i_pPDFDoc->pDocStream;
  lRet = i_Size;
  if ((i_Size + pStream->curSize + 1) > pStream->allocSize)
  {
    if (pStream->pStream == NULL)
    {
      pStream->pStream = (char*)PDFMalloc(MIN_BUF_SIZE);
      if (!pStream)
        return false_a;
      pStream->allocSize = MIN_BUF_SIZE;
    }
    else
    {
      long lOldSize = (long)pStream->allocSize;
      while((i_Size + pStream->curSize + 1) > pStream->allocSize)
      {
        if(pStream->allocSize < MAX_BUF_BUMP)
          pStream->allocSize *= 2;
        else
          pStream->allocSize += MAX_BUF_BUMP;
      }
      PDFRealloc((void **)&pStream->pStream, (long)pStream->allocSize, (long)pStream->allocSize - lOldSize);
    }
  }
  memcpy(pStream->pStream + pStream->curSize, i_pczBuf, i_Size);
  pStream->curSize += i_Size;
  *(pStream->pStream + pStream->curSize) = 0;

  return lRet;
}

PDFStream *PDFCreateStream()
{
  PDFStream *pRetStream = NULL;
  pRetStream = (PDFStream *)PDFMalloc(sizeof(PDFStream));
  if (!pRetStream)
    return NULL;
  pRetStream->pStream = (char *)PDFMalloc(MIN_BUF_SIZE);
  if (!(pRetStream->pStream))
  {
    free(pRetStream);
    return NULL;
  }
  pRetStream->allocSize = MIN_BUF_SIZE;
  pRetStream->curSize = 0;
  return pRetStream;
}

bool_a PDFWriteToPageStream(PDFDocPTR i_pPDFDoc, char *i_pBuffer, size_t i_BuffSize, long i_lPage)
{
  PDFStream *pPageStream = NULL;
  if (i_lPage < 0 || i_lPage >= PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;
  if (!i_pPDFDoc || !i_pBuffer || i_BuffSize <= 0 || !(i_pPDFDoc->m_vctPages[i_lPage]))
    return false_a;

  pPageStream = i_pPDFDoc->m_vctPages[i_lPage]->pPageStream;

  if ((i_BuffSize + pPageStream->curSize + 1) > pPageStream->allocSize)
  {
    if (pPageStream->pStream == NULL)
    {
      pPageStream->pStream = (char *)PDFMalloc(MIN_BUF_SIZE);
      if (!pPageStream)
        return false_a;
      pPageStream->allocSize = MIN_BUF_SIZE;
    }
    else
    {
      long lOldSize = (long)pPageStream->allocSize;
      while((i_BuffSize + pPageStream->curSize + 1) > pPageStream->allocSize)
      {
        if(pPageStream->allocSize < MAX_BUF_BUMP)
          pPageStream->allocSize *= 2;
        else
          pPageStream->allocSize += MAX_BUF_BUMP;
      }
      PDFRealloc((void **)&pPageStream->pStream, (long)pPageStream->allocSize, (long)pPageStream->allocSize - lOldSize);
    }
  }
  memcpy(pPageStream->pStream + pPageStream->curSize, i_pBuffer, i_BuffSize);
  pPageStream->curSize += i_BuffSize;
  *(pPageStream->pStream + pPageStream->curSize) = 0;
  return true_a;
}

void PDFDeleteStream(PDFStream **i_pStream)
{
  if(!i_pStream || !(*i_pStream))
    return;
  if((*i_pStream)->pStream)
    free((*i_pStream)->pStream);
  free((*i_pStream));
  (*i_pStream) = NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

long PDFWritePagesObject(PDFDocPTR i_pPDFDoc, long objNumber)
{
  long i;

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%ld 0 obj\n", objNumber);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "<<\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/Type /Pages\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/Count %ld\n", PDFGetDocClass(i_pPDFDoc)->GetPageCount());
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/Kids [ ");
  for(i = 0; i < PDFGetDocClass(i_pPDFDoc)->GetPageCount(); i++)
  {
    i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%ld 0 R ",
                                                      i_pPDFDoc->m_vctPages[i]->lObjIndex);
  }
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "]\n>>\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "endobj\n");

  if ((objNumber + 1) >= i_pPDFDoc->iCurrAllocObjects)
  {
    i_pPDFDoc->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(i_pPDFDoc->pObjOffset), i_pPDFDoc->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(i_pPDFDoc->pObjIndex), i_pPDFDoc->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
  return(i_pPDFDoc->lActualSize);
}

long PDFWritePageObject(PDFDocPTR pdf, PDFPage *pInf)
{
  CPDFBaseFont *pFont;
  CPDFImage *pImg;
  PDFList *iL;
  char czTemp[100] = {0};
  long i;

  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 obj\n", pInf->lObjIndex);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<<\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Type /Page\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Parent %ld 0 R\n", pInf->lParent);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Resources <<\n");

  if (PDFGetDocClass(pdf)->GetLayerCount() > 0)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Properties << ");
    for (i = 0;i < PDFGetDocClass(pdf)->GetLayerCount(); i++)
    {
      CPDFLayer *pLayer = PDFGetDocClass(pdf)->GetLayer(i);
      if (pLayer)
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/%s %ld 0 R ",
                                                  pLayer->GetLayerResName(), pLayer->GetObjNum());
    }
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
  }

  //
//  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ExtGState << /GS1 << /Type /ExtGState /SA true /TR /Identity >> >>\n");
/*
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/SA true\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
*/

  if(pInf->lFontCount)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Font <<\n");
    iL = pInf->pFontIdx;
    while(iL != NULL)
    {
      if(iL->lValue >= 0)
      {
        pFont = PDFGetDocClass(pdf)->GetFont(iL->lValue);
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/%s %ld 0 R\n",
                                                            pFont->GetFontResName(), pFont->GetFontObjNum());
      }
      iL = iL->pNext;
    }
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
  }
  if(pInf->lImageCount)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/XObject <<\n");
    iL = pInf->pImageIdx;
    while(iL != NULL)
    {
      pImg = PDFGetDocClass(pdf)->GetImage(iL->lValue);
      if (pImg)
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/%s %ld 0 R\n", pImg->name, pImg->lObjIndex);

      iL = iL->pNext;
    }
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
  }

  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ProcSet %ld 0 R >>\n", pdf->pObjIndex[PDF_ProcSet]);
  
  sprintf(czTemp, "%s [%ld %ld %ld %ld]", czMEDIABOX, PDFConvertDimensionToDefaultL(pInf->recMediaBox.lLeft),
            PDFConvertDimensionToDefaultL(pInf->recMediaBox.lBottom), PDFConvertDimensionToDefaultL(pInf->recMediaBox.lRight),
            PDFConvertDimensionToDefaultL(pInf->recMediaBox.lTop));
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%s\n", czTemp);

  sprintf(czTemp, "%s [%ld %ld %ld %ld]", czCROPBOX, PDFConvertDimensionToDefaultL(pInf->recCropBox.lLeft),
            PDFConvertDimensionToDefaultL(pInf->recCropBox.lBottom), PDFConvertDimensionToDefaultL(pInf->recCropBox.lRight),
            PDFConvertDimensionToDefaultL(pInf->recCropBox.lTop));
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%s\n", czTemp);

  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Rotate %ld\n", pInf->lRotation);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Contents %ld 0 R\n", pInf->lContIndex);

  if (pInf->lAnnotCount)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Annots [ ");
    iL = pInf->pAnnotIdx;
    while(iL != NULL)
    {
      if(iL->lValue >= 0)
      {
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 R ",
                                  pdf->m_vctAnnots[iL->lValue]->lObjIndex[0]);
      }
      iL = iL->pNext;
    }
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "]\n");
  }

  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "endobj\n");

  if ((pInf->lObjIndex + 1) >= pdf->iCurrAllocObjects)
  {
    pdf->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(pdf->pObjOffset), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(pdf->pObjIndex), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
  return(pdf->lActualSize);
}

long PDFWriteContentsFromMemory(PDFDocPTR pdf, PDFPage *pInf)
{
  Byte *inputBuf;
  size_t filesize;
  size_t bufSize;

  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 obj\n", pInf->lContIndex);

  if (pInf->bCompressed)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<<\n/Length %ld\n/Filter [%s]\n>>\nstream\n",
                                          pInf->lCompLength, pdf->streamFilterList);

    if (PDFGetDocClass(pdf)->Encrypt())
      PDFEncrypt(pdf, pInf->pucCompStream, pInf->lCompLength, pInf->lContIndex, 0);

    pdf->lActualSize += PDFWriteBufToDocStream((PDFDocPTR)pdf, (const char*)pInf->pucCompStream, (long)pInf->lCompLength);
  }
  else
  {
    /* No compression */
    inputBuf = (Byte *)pInf->pPageStream->pStream;
    filesize = pInf->pPageStream->curSize;
    bufSize = pInf->pPageStream->allocSize;

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<</Length %ld>>\nstream\n", filesize);

    if (PDFGetDocClass(pdf)->Encrypt())
      PDFEncrypt(pdf, inputBuf, filesize, pInf->lContIndex, 0);

    pdf->lActualSize += PDFWriteBufToDocStream((PDFDocPTR)pdf, (const char*)inputBuf, (long)filesize);
  }
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "\nendstream\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "endobj\n");

  if ((pInf->lContIndex + 1) >= pdf->iCurrAllocObjects)
  {
    pdf->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(pdf->pObjOffset), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(pdf->pObjIndex), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
  return(pdf->lActualSize);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool_a PDFAddText(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY, long i_lOrient, char *i_pString)
{
  if (!i_pString)
    return false_a;
  float fA, fB, fC, fD, fAngle, fCos, fSin;

  fAngle = (float)(PI * i_lOrient / 180.0);
  fCos = (float)cos(fAngle);
  fSin = (float)sin(fAngle);
  fA =  fCos;
  fB =  fSin;
  fC = -fSin;
  fD =  fCos;
  PDFSetTextMatrix(i_pPDFDoc, fA, fB, fC, fD, (float)i_lX, (float)i_lY);
  PDFFlushString(i_pPDFDoc, i_pString, NULL);

  return true_a;
}

void PDFBgnText(PDFDocPTR pdf, long clipmode)
{
  char czBuff[101] = {0};

  czBuff[0] = '\n';
  strcat(czBuff, "BT\n");

  PDFWriteToPageStream(pdf, czBuff, strlen(czBuff), pdf->m_lActualPage);

  pdf->bTextObj = true_a;
  PDFresetTextCTM(pdf);

  clipmode;
}

void PDFEndText(PDFDocPTR pdf)
{
  char czBuff[101] = {0};

  strcpy(czBuff, "ET\n");

  PDFWriteToPageStream(pdf, czBuff, strlen(czBuff), pdf->m_lActualPage);

  pdf->bTextObj = false_a;
}

bool_a PDFNextLine(PDFDocPTR i_pPDFDoc)
{
  if (!i_pPDFDoc)
    return false_a;
  if (!PDFWriteToPageStream(i_pPDFDoc, "T*\n", strlen("T*\n"), i_pPDFDoc->m_lActualPage))
    return false_a;
  return true_a;
}

bool_a PDFContinueText(PDFDocPTR i_pPDFDoc, char *i_pczString)
{
  char *fixedstr = PDFescapeSpecialChars(i_pczString);
  long lSize = (PDFStrLenC(fixedstr) + PDFStrLenC(czSTRINGTJ) + 10) * sizeof(char);
  char *pczTemp = (char *)PDFMalloc(lSize);
  if (!pczTemp || !i_pPDFDoc)
    return false_a;

  sprintf(pczTemp, "(%s) %s\n", fixedstr, czSTRINGTJ);
  if (!PDFWriteToPageStream(i_pPDFDoc, pczTemp, strlen(pczTemp), i_pPDFDoc->m_lActualPage))
    return false_a;

  if(fixedstr)
    free(fixedstr);
  if (pczTemp)
    free(pczTemp);

  return true_a;
}

bool_a PDFSetLeading(PDFDocPTR i_pPDFDoc, float i_fLeading)
{
  char czTemp[101] = {0};
  if (!i_pPDFDoc)
    return false_a;
  sprintf(czTemp, "%.4f TL\n", PDFConvertDimensionToDefault(i_fLeading));
  if (!PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage))
    return false_a;
  return true_a;
}

void PDFWordSpacing(PDFDocPTR i_pPDFDoc, float i_fSpacing)
{
  char czTemp[101] = {0};
  float fWS;
  if (!i_pPDFDoc)
    return;
  i_pPDFDoc->fWordSpacing = i_fSpacing;
  fWS = PDFConvertDimensionToDefault(i_fSpacing);
  sprintf(czTemp, "%.2f Tw\n", fWS);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void *PDFMalloc(size_t i_Size)
{
  void *pRet;
  if (i_Size <= 0)
    return NULL;
  pRet = malloc(i_Size);
  if (!pRet)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return pRet;
  }
  memset(pRet, 0, i_Size);
  return pRet;
}

void PDFRealloc(void **io_ppBuffer, size_t i_Size, size_t i_Add)
{
  unsigned char *pP;
  *io_ppBuffer = (void *)realloc(*io_ppBuffer, i_Size);
  pP = (unsigned char *)*io_ppBuffer;
  memset(pP + (i_Size - i_Add), 0, i_Add);
}

long PDFRound(float i_fNum)
{
  long lRet;
  if (i_fNum < 0)
    lRet = (long)(i_fNum - 0.5f);
  else
    lRet = (long)(i_fNum + 0.5f);
  return lRet;
}

long PDFStrLenUC(const unsigned char *i_pBuf)
{
  long lLen = 0;
  if (!i_pBuf)
    return lLen;
  lLen = (long)strlen((char *)i_pBuf);
  return lLen;
}

long PDFStrLenC(const char *i_pBuf)
{
  long lLen = 0;
  if (!i_pBuf)
    return lLen;
  lLen = (long)strlen(i_pBuf);
  return lLen;
}

char *PDFStrDup(const char *i_pStr)
{
  char *pRetBuf;
  long lSize;
  if (!i_pStr)
    return NULL;
  lSize = PDFStrLenC(i_pStr) + 1;
  pRetBuf = (char *)PDFMalloc(lSize);
  if (!pRetBuf)
    return NULL;
  memcpy(pRetBuf, i_pStr, lSize);
  pRetBuf[lSize - 1] = 0;

  return pRetBuf;
}

char *PDFStrJoin(const char *i_pStr1, const char *i_pStr2)
{
  char *pRetBuf;
  long lSize1, lSize2;
  if (!i_pStr1 || !i_pStr2)
    return NULL;
  lSize1 = PDFStrLenC(i_pStr1);
  lSize2 = PDFStrLenC(i_pStr2);
  pRetBuf = (char *)PDFMalloc(lSize1 + lSize2 + 1);
  if (!pRetBuf)
    return NULL;
  memcpy(pRetBuf, i_pStr1, lSize1);
  memcpy(pRetBuf + lSize1, i_pStr2, lSize2);
  pRetBuf[lSize1 + lSize2] = 0;

  return pRetBuf;
}

float PDFConvertDimensionFromDefault(float i_fValue)
{
  float fTemp = ((float)(DEFPOINTSPERINCH / POINTSPERINCH));
  fTemp = i_fValue / fTemp;
  return fTemp;
}

long PDFConvertDimensionFromDefaultL(long i_lValue)
{
  float fRet = PDFConvertDimensionFromDefault((float)i_lValue);
  return PDFRound(fRet);
}

float PDFConvertDimensionToDefault(float i_fValue)
{
  float fTemp = ((float)(POINTSPERINCH / DEFPOINTSPERINCH));
  fTemp = i_fValue / fTemp;
  return fTemp;
}

long PDFConvertDimensionToDefaultL(long i_lValue)
{
  float fRet = PDFConvertDimensionToDefault((float)i_lValue);
  return PDFRound(fRet);
}

long PDFConvertMMToPoints(float i_fMM)
{
  return (long)(POINTSPERMM * i_fMM);
}

long PDFConvertPointsToTwips(long i_lPoints)
{
  float fTemp = POINTSPERINCH / 1440.f;
  return PDFRound(i_lPoints * fTemp);
}

long PDFConvertTwipsToPoints(long i_lTwips)
{
  float fTemp = POINTSPERINCH / 1440.f;
  return PDFRound(i_lTwips / fTemp);
}

long getFileSize(const char *file)
{
  long filesize = 0;

#if defined(LINUX_OS) || defined(MAC_OS_MWE) || defined(__LINUX__)
  struct stat filestat;

  if(stat(file, &filestat))
#elif defined(WIN32)
  struct _stat filestat;

  if(_stat(file, &filestat))
#else
#error "not implemented"
#endif

    filesize = 0;
  else
  {
    if( !(filestat.st_mode & S_IFREG) )
      filesize = 0;
    else
      filesize = filestat.st_size;
  }
  return(filesize);
}

void PDFSetTextMatrix(PDFDocPTR i_pPDFDoc, float i_fA, float i_fB, float i_fC, float i_fD, float i_fE, float i_fF)
{
  char czTemp[201] = {0};
  i_fE = PDFConvertDimensionToDefault(i_fE);
  i_fF = PDFConvertDimensionToDefault(i_fF);
  sprintf(czTemp, "%.4f %.4f %.4f %.4f %.4f %.4f Tm\n", i_fA, i_fB, i_fC, i_fD, i_fE, i_fF);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
  i_pPDFDoc->strTextCTM.m_fA = i_fA;
  i_pPDFDoc->strTextCTM.m_fB = i_fB;
  i_pPDFDoc->strTextCTM.m_fC = i_fC;
  i_pPDFDoc->strTextCTM.m_fD = i_fD;
  i_pPDFDoc->strTextCTM.m_fE = i_fE;
  i_pPDFDoc->strTextCTM.m_fF = i_fF;
}

void PDFSetTextMatrixL(PDFDocPTR i_pPDFDoc, long i_lA, long i_lB, long i_lC, long i_lD, long i_lE, long i_lF)
{
   PDFSetTextMatrix(i_pPDFDoc, (float)i_lA, (float)i_lB, (float)i_lC, (float)i_lD, (float)i_lE, (float)i_lF);
}

unsigned char PDFnibbleValue(unsigned char hexchar)
{
  unsigned char val;

  if(hexchar >= '0' && hexchar <= '9')
    val = hexchar - '0';
  else 
    if(hexchar >= 'A' && hexchar <= 'F')
      val = hexchar - 'A' + 10;
    else
      if(hexchar >= 'a' && hexchar <= 'f')
        val = hexchar - 'a' + 10;
      else
        val = 0xff;
  return(val);
}

unsigned char *PDFconvertHexToBinary(const char *hexin, unsigned char *binout, long *length)
{
  long bcount=0;
  char *pin = (char *)hexin;
  unsigned char *pout = binout;
  unsigned char ch, chin;
  int HighNibble = 1;
  while( (chin = *pin++) != '\0')
  {
    ch = PDFnibbleValue(chin);
    if(ch > 15)
    continue;    /* Skip all non-HEX character */

    if(HighNibble)
    {
      *pout = ch << 4;
      HighNibble = 0;
    }
    else
    {
      *pout++ |= ch;
      HighNibble = 1;
      bcount++;
    }
  }
  *length = bcount;  /* byte count */
  return(binout);
}

char *PDFescapeSpecialCharsBinary(const char *instr, long lengthIn, long *lengthOut)
{
  char *ptr, *ptr2, *buf, ch;
  long i;
  int escapecount = 0;
  ptr = (char *)instr;
  for(i=0; i<lengthIn; i++)
  {
    ch = *ptr++;
    if( (ch == '(') || (ch == ')') || (ch == '\\') || (ch == '\r')
         || (ch == '\f') || (ch == '\b') || (ch == '\t') || (ch == '\n'))
      escapecount++;
  }
  *lengthOut = lengthIn + escapecount;

  ptr = (char *)instr;
  /* Enough string length for new escaped string. */
  buf = (char *)PDFMalloc((lengthIn + escapecount + 1));
  if (!buf)
    return NULL;
  ptr2 = buf;
  for(i=0; i<lengthIn; i++)
  {
    ch = *ptr++;
    switch (ch)
    {
    case '\\':
    case '(':
    case ')':
      *ptr2++ = '\\';
      *ptr2++ = ch;
      break;
    case '\r':
      *ptr2++ = '\\';
      *ptr2++ = 'r';
      break;
    case '\n':
      *ptr2++ = '\\';
      *ptr2++ = 'n';
      break;
    case '\f':
      *ptr2++ = '\\';
      *ptr2++ = 'f';
      break;
    case '\b':
      *ptr2++ = '\\';
      *ptr2++ = 'b';
      break;
    case '\t':
      *ptr2++ = '\\';
      *ptr2++ = 't';
      break;
    default:
      *ptr2++ = ch;
    }
  }
  *ptr2 = '\0';  /* not really needed */
  return(buf);
}

char *PDFescapeSpecialChars(const char *instr)
{
  char *ptr, *ptr2, *buf, ch;
  int escapecount = 0;
  if (!instr)
    return NULL;
  ptr = (char *)instr;
  while( (ch = *ptr++) != '\0')
  {
    if( (ch == '(') || (ch == ')') || (ch == '\\') || (ch == '\r')
         || (ch == '\f') || (ch == '\b') || (ch == '\t') || (ch == '\n'))
      escapecount++;
  }

  ptr = (char *)instr;
  /* Enough string length for new escaped string. */
  buf = (char *)PDFMalloc((PDFStrLenC(instr) + escapecount + 1));
  if (!buf)
    return false_a;
  ptr2 = buf;
  while( (ch = *ptr++) != '\0')
  {
    switch (ch)
    {
    case '\\':
    case '(':
    case ')':
      *ptr2++ = '\\';
      *ptr2++ = ch;
      break;
    case '\r':
      *ptr2++ = '\\';
      *ptr2++ = 'r';
      break;
    case '\n':
      *ptr2++ = '\\';
      *ptr2++ = 'n';
      break;
    case '\f':
      *ptr2++ = '\\';
      *ptr2++ = 'f';
      break;
    case '\b':
      *ptr2++ = '\\';
      *ptr2++ = 'b';
      break;
    case '\t':
      *ptr2++ = '\\';
      *ptr2++ = 't';
      break;
    default:
      *ptr2++ = ch;
    }
  }
  *ptr2 = '\0';  /* properly terminate the new string */
  return(buf);
}

void PDFEnableCompress(PDFDocPTR i_pPDFDoc, bool_a bON)
{
  if (bON == 0)
  {
    i_pPDFDoc->compressionON = 0;
    return;
  }
  // OK, we want compression, and the compression program exists 
  // Let's see what kind of decode filter we specify 
  if(i_pPDFDoc->streamFilterList != NULL)
  {
    i_pPDFDoc->compressionON = bON;   // Only if filter command is set 
    if(bON && (strstr(i_pPDFDoc->streamFilterList, "FlateDecode") != NULL))
      PDFSetVersion(i_pPDFDoc, 1, 2);  // FlateDecode requires PDF-1.2 
  }
  else
  {
    i_pPDFDoc->compressionON = 0;   // compression filter has not been set 
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
long PDFGetLastError()
{
  if (g_sInit)
    return (g_lErrorBuffer)[0];
  else
  {
    memset(g_lErrorBuffer, ERR_OK, sizeof(long) * LAST_ERROR_COUNT);
    g_sInit = true_a;
  }
  return ERR_OK;
}

void PDFSetLastError(long i_lError)
{
  if (g_sInit)
  {
    memcpy(g_lErrorBuffer + 1, g_lErrorBuffer, sizeof(long) * (LAST_ERROR_COUNT - 1));
    g_lErrorBuffer[0] = i_lError;
  }
  else
  {
    memset(g_lErrorBuffer, ERR_OK, sizeof(long) * LAST_ERROR_COUNT);
    g_sInit = true_a;
  }
}

const char *PDFGetErrorDescription(long i_lError)
{
  if (g_sInit)
  {
    if (i_lError == ERR_DOC_NOT_EXIST)
      return czDocNotExist;
    if (i_lError < 0 || i_lError >= ERROR_COUNT)
      return "";
    return pczErrorTable[i_lError];
  }
  else
  {
    memset(g_lErrorBuffer, ERR_OK, sizeof(long) * LAST_ERROR_COUNT);
    g_sInit = true_a;
  }
  return pczErrorTable[0];
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PDFFlushString(PDFDocPTR i_pPDFDoc, CPDFString i_String, CPDFBaseFont *i_pFont)
{
  if (!i_pPDFDoc)
    return;
  if (!i_pFont)
   i_pFont = PDFGetDocClass(i_pPDFDoc)->GetActualFont();
  string strOut;
  if (i_pFont->GetGidxString(i_String, strOut))
  {

    //PDFWriteToPageStream(i_pPDFDoc, "<", strlen("<"), i_pPDFDoc->m_lActualPage);
    //PDFWriteToPageStream(i_pPDFDoc, (char *)strOut.c_str(), strOut.size(), i_pPDFDoc->m_lActualPage);
    //PDFWriteToPageStream(i_pPDFDoc, "> Tj\n", strlen("> Tj\n"), i_pPDFDoc->m_lActualPage);

    PDFWriteToPageStream(i_pPDFDoc, (char *)strOut.c_str(), strOut.size(), i_pPDFDoc->m_lActualPage);
    PDFWriteToPageStream(i_pPDFDoc, " Tj\n", strlen(" Tj\n"), i_pPDFDoc->m_lActualPage);
  } else if(i_pPDFDoc->hex_string)
  {
    long length, lenEsc;
    char *escUnicode;
    char *tempbuf = (char *)PDFMalloc((i_String.GetString()->size()/2 + 3));
    if (!tempbuf)
      return;
    PDFconvertHexToBinary(i_String.GetString()->c_str(), (unsigned char *)tempbuf, &length);
    escUnicode = PDFescapeSpecialCharsBinary(tempbuf, length, &lenEsc);
    free(tempbuf);

    PDFWriteToPageStream(i_pPDFDoc, "(", strlen("("), i_pPDFDoc->m_lActualPage);
    PDFWriteToPageStream(i_pPDFDoc, escUnicode, lenEsc, i_pPDFDoc->m_lActualPage);
    PDFWriteToPageStream(i_pPDFDoc, ") Tj\n", strlen(") Tj\n"), i_pPDFDoc->m_lActualPage);
    free(escUnicode);
  }
  else
  {
    char *fixedstr = PDFescapeSpecialChars(i_String.GetString()->c_str());
    long lSize = (PDFStrLenC(fixedstr) + PDFStrLenC(czSTRINGTJ) + 10) * sizeof(char);
    char *pczTemp = (char *)PDFMalloc(lSize);
    if (!pczTemp)
      return;

    sprintf(pczTemp, "(%s) %s\n", fixedstr, czSTRINGTJ);
    PDFWriteToPageStream(i_pPDFDoc, pczTemp, strlen(pczTemp), i_pPDFDoc->m_lActualPage);
    if(fixedstr)
      free(fixedstr);
    if (pczTemp)
      free(pczTemp);
  }
}

long PDFWriteInfo(PDFDocPTR i_pPDFDoc, long objNumber)
{
  PPDFInfo pInfo   = &(i_pPDFDoc->strInfo);
  char *pTemp;
  long lLen;

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, czOBJECTBEGIN, objNumber);

  lLen = PDFStrLenC(pInfo->czAuthor);
  if (PDFGetDocClass(i_pPDFDoc)->Encrypt() && lLen > 0)
    PDFEncrypt(i_pPDFDoc, (unsigned char *)pInfo->czAuthor, PDFStrLenC(pInfo->czAuthor), objNumber, 0);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s (", czAUTHOR);
  pTemp = PDFescapeSpecialCharsBinary(pInfo->czAuthor, lLen, &lLen);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lLen);
  free(pTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, ")\n", PDFStrLenC(")\n"));

  lLen = PDFStrLenC(pInfo->czCreationDate);
  if (PDFGetDocClass(i_pPDFDoc)->Encrypt() && lLen > 0)
    PDFEncrypt(i_pPDFDoc, (unsigned char *)pInfo->czCreationDate, PDFStrLenC(pInfo->czCreationDate), objNumber, 0);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s (", czCREATIONDATE);
  pTemp = PDFescapeSpecialCharsBinary(pInfo->czCreationDate, lLen, &lLen);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lLen);
  free(pTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, ")\n", PDFStrLenC(")\n"));

  lLen = PDFStrLenC(pInfo->czModDate);
  if (PDFGetDocClass(i_pPDFDoc)->Encrypt() && lLen > 0)
    PDFEncrypt(i_pPDFDoc, (unsigned char *)pInfo->czModDate, PDFStrLenC(pInfo->czModDate), objNumber, 0);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s (", czMODDATE);
  pTemp = PDFescapeSpecialCharsBinary(pInfo->czModDate, lLen, &lLen);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lLen);
  free(pTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, ")\n", PDFStrLenC(")\n"));

  lLen = PDFStrLenC(pInfo->czCreator);
  if (PDFGetDocClass(i_pPDFDoc)->Encrypt() && lLen > 0)
    PDFEncrypt(i_pPDFDoc, (unsigned char *)pInfo->czCreator, PDFStrLenC(pInfo->czCreator), objNumber, 0);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s (", czCREATOR);
  pTemp = PDFescapeSpecialCharsBinary(pInfo->czCreator, lLen, &lLen);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lLen);
  free(pTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, ")\n", PDFStrLenC(")\n"));

  lLen = PDFStrLenC(pInfo->czProducer);
  if (PDFGetDocClass(i_pPDFDoc)->Encrypt() && lLen > 0)
    PDFEncrypt(i_pPDFDoc, (unsigned char *)pInfo->czProducer, PDFStrLenC(pInfo->czProducer), objNumber, 0);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s (", czPRODUCER);
  pTemp = PDFescapeSpecialCharsBinary(pInfo->czProducer, lLen, &lLen);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lLen);
  free(pTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, ")\n", PDFStrLenC(")\n"));

  lLen = PDFStrLenC(pInfo->czTitle);
  if (PDFGetDocClass(i_pPDFDoc)->Encrypt() && lLen > 0)
    PDFEncrypt(i_pPDFDoc, (unsigned char *)pInfo->czTitle, PDFStrLenC(pInfo->czTitle), objNumber, 0);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s (", czTITLE);
  pTemp = PDFescapeSpecialCharsBinary(pInfo->czTitle, lLen, &lLen);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lLen);
  free(pTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, ")\n", PDFStrLenC(")\n"));

  lLen = PDFStrLenC(pInfo->czSubject);
  if (PDFGetDocClass(i_pPDFDoc)->Encrypt() && lLen > 0)
    PDFEncrypt(i_pPDFDoc, (unsigned char *)pInfo->czSubject, PDFStrLenC(pInfo->czSubject), objNumber, 0);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s (", czSUBJECT);
  pTemp = PDFescapeSpecialCharsBinary(pInfo->czSubject, lLen, &lLen);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lLen);
  free(pTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, ")\n", PDFStrLenC(")\n"));

  lLen = PDFStrLenC(pInfo->czKeywords);
  if (PDFGetDocClass(i_pPDFDoc)->Encrypt() && lLen > 0)
    PDFEncrypt(i_pPDFDoc, (unsigned char *)pInfo->czKeywords, PDFStrLenC(pInfo->czKeywords), objNumber, 0);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s (", czKEYWORDS);
  pTemp = PDFescapeSpecialCharsBinary(pInfo->czKeywords, lLen, &lLen);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lLen);
  free(pTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, ")\n", PDFStrLenC(")\n"));


  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, czOBJECTEND);

  if ((objNumber + 1) >= i_pPDFDoc->iCurrAllocObjects)
  {
    i_pPDFDoc->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(i_pPDFDoc->pObjOffset), i_pPDFDoc->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(i_pPDFDoc->pObjIndex), i_pPDFDoc->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
  return(i_pPDFDoc->lActualSize);
}

long PDFWriteEncryption(PDFDocPTR i_pPDFDoc, long objNumber)
{
  //char *pTemp = NULL;
  //long lTemp;
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%ld 0 obj\n", objNumber);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "<<\n");

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/Filter /Standard\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/R 3\n");

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/O (");
  /*pTemp = PDFescapeSpecialCharsBinary((const char*)i_pPDFDoc->ucO, 32, &lTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lTemp);
  free(pTemp);*/
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, (const char*)i_pPDFDoc->ucO, 32);

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, ")\n");

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/U (");
  /*pTemp = PDFescapeSpecialCharsBinary((const char*)i_pPDFDoc->ucU, 32, &lTemp);
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, pTemp, lTemp);
  free(pTemp);*/
  i_pPDFDoc->lActualSize += PDFWriteBufToDocStream(i_pPDFDoc, (const char*)i_pPDFDoc->ucU, 32);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, ")\n");

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/P %d\n", i_pPDFDoc->iPermission);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/V 2\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/Length 128\n");

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, ">>\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "endobj\n");

  return (i_pPDFDoc->lActualSize);
}

long PDFWriteXrefTrailer(PDFDocPTR pdf, long objNumber)
{
  int i;
  pdf->startXref = pdf->lActualSize;
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "xref\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "0 %ld\n", objNumber);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "0000000000 65535 f \n");
  for(i=1; i< objNumber; i++)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%010ld 00000 n \n", pdf->pObjOffset[i]);
  }
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "trailer\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<<\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Size %ld\n", objNumber);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Root %ld 0 R\n", pdf->pObjIndex[PDF_Catalog]);
  if (PDFGetDocClass(pdf)->Encrypt())
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Encrypt %ld 0 R\n", pdf->pObjIndex[PDF_Encrypt]);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Info %ld 0 R\n", pdf->pObjIndex[PDF_Info]);

  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ID[<%s><%s>]\n", ucFILE_ID_1, ucFILE_ID_2);


  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "startxref\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld\n", pdf->startXref);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%%%%EOF\n");

  return(pdf->lActualSize);
}

long PDFWriteCatalogObject(PDFDocPTR i_pPDFDoc, long objNumber)
{
  string strTemp;

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%ld 0 obj\n", objNumber);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "<<\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/Type /Catalog\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/Pages %ld 0 R\n", i_pPDFDoc->pObjIndex[PDF_Pages]);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/Outlines %ld 0 R\n", i_pPDFDoc->pObjIndex[PDF_Outlines]);

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/PageMode %s\n",
                              pczPAGEMODEATTR[i_pPDFDoc->strCatalog.ePageMode]);
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/PageLayout %s\n",
                              pczPAGELAYOUTATTR[i_pPDFDoc->strCatalog.ePageLayout]);


  if (PDFGetDocClass(i_pPDFDoc)->GetLayerCount() > 0)
  {
    if (!PDFGetDocClass(i_pPDFDoc)->GetOCPropertiesStream(strTemp))
      return i_pPDFDoc->lActualSize;

    i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/OCProperties << ");
    i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, strTemp.c_str());
    i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, " >> ");
  }

i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/ViewerPreferences <<\n");
if(i_pPDFDoc->strCatalog.strViewerPreferences.bHideToolbar)
{
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/HideToolbar true\n");
}
if(i_pPDFDoc->strCatalog.strViewerPreferences.bHideMenubar)
{
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/HideMenubar true\n");
}
if(i_pPDFDoc->strCatalog.strViewerPreferences.bHideWindowUI)
{
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/HideWindowUI true\n");
}
if(i_pPDFDoc->strCatalog.strViewerPreferences.bFitWindow)
{
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/FitWindow true\n");
}
if(i_pPDFDoc->strCatalog.strViewerPreferences.bCenterWindow)
{
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/CenterWindow true\n");
}
if(i_pPDFDoc->strCatalog.ePageMode == eFullScreen)
{
  if (i_pPDFDoc->strCatalog.strViewerPreferences.eNonFullScreenPageMode < eUseNone
      || i_pPDFDoc->strCatalog.strViewerPreferences.eNonFullScreenPageMode > eUseThumbs)
    i_pPDFDoc->strCatalog.strViewerPreferences.eNonFullScreenPageMode = eUseNone;
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/NonFullScreenPageMode %s\n",
                              pczPAGEMODEATTR[i_pPDFDoc->strCatalog.strViewerPreferences.eNonFullScreenPageMode]);
}
if (i_pPDFDoc->strCatalog.strViewerPreferences.bDirection)
{
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "%s /R2L\n", czDIRECTION);
}

i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, ">>\n");

  if (PDFGetDocClass(i_pPDFDoc)->HasWidget())
    i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "/AcroForm %ld 0 R\n", i_pPDFDoc->pObjIndex[PDF_Annots]);

  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, ">>\n");
  i_pPDFDoc->lActualSize += PDFWriteFormatedBufToDocStream(i_pPDFDoc, "endobj\n");

  if ((objNumber + 1) >= i_pPDFDoc->iCurrAllocObjects)
  {
    i_pPDFDoc->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(i_pPDFDoc->pObjOffset), i_pPDFDoc->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(i_pPDFDoc->pObjIndex), i_pPDFDoc->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
  return(i_pPDFDoc->lActualSize);
}

unsigned char *PDFCompressStream(unsigned char *i_pucBuffer, long *io_pSize)
{
  Byte *outputBuf;
  uLong comprLen = 0;
  float headroom;
  long iheadroom;

  headroom = (float)(*io_pSize * 0.001 + 16.0);  // 0.1% plus 12 bytes
  iheadroom = (long)headroom;
  comprLen = (uLong) *io_pSize + iheadroom;
  outputBuf = (Byte *)PDFMalloc(comprLen);
  if (!outputBuf)
    return NULL;
  if (compress2(outputBuf, &comprLen, i_pucBuffer, (uLong) *io_pSize, 6) != Z_OK)
    return NULL;
  *io_pSize = comprLen;
  return outputBuf;
}

long PDFWriteImage(PDFDocPTR pdf, CPDFImage *i_pImg, long *i_pObjCount)
{
  if (!i_pImg)
    return pdf->lActualSize;

  long lSize = i_pImg->height * i_pImg->rowbytes;
  unsigned char *pOutput = NULL;
  if (!i_pImg->bUsePNG)
  {
    int Invert = i_pImg->invert;

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 obj\n", i_pImg->lObjIndex);
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<<\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Type /XObject\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Subtype /Image\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Name /%s\n", i_pImg->name);

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Width %ld\n", i_pImg->width);
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Height %ld\n", i_pImg->height);

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Filter /DCTDecode\n");

    if(Invert)
    {
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Decode %s\n", pczInvArray[i_pImg->ncomponents]);
    }
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/BitsPerComponent %ld\n", i_pImg->bitspersample);
    if (i_pImg->imagemask == 1 && i_pImg->ncomponents == 1 && i_pImg->bitspersample == 1)
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ImageMask true\n");
    else
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ColorSpace /%s\n", pczColorSpaces[i_pImg->ncomponents]);

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Interpolate true\n");

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Length %ld\n", i_pImg->filesize);

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "stream\n");

    if (PDFGetDocClass(pdf)->Encrypt())
      PDFEncrypt(pdf, i_pImg->data, i_pImg->filesize, i_pImg->lObjIndex, 0);

    pdf->lActualSize += PDFWriteBufToDocStream((PDFDocPTR)pdf, (const char *)i_pImg->data, i_pImg->filesize);

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "\nendstream\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "endobj\n");
  }
  else
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 obj\n", i_pImg->lObjIndex);
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<<\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Type /XObject\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Subtype /Image\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Name /%s\n", i_pImg->name);

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Width %d\n", i_pImg->width);
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Height %d\n", i_pImg->height);
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/BitsPerComponent %d\n", i_pImg->bitspersample);
    // Transparency handling
    // Masking by color: single transparent color value
    if (i_pImg->transparent)
    {
      if (i_pImg->colorspace == Indexed || i_pImg->colorspace == DeviceGray)
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Mask[%d %d]\n", (int)i_pImg->transval[0], (int)i_pImg->transval[0]);
      else if (i_pImg->colorspace == DeviceRGB)
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Mask[%d %d %d %d %d %d]\n", (int)i_pImg->transval[0], (int) i_pImg->transval[0],
                                                            (int) i_pImg->transval[1], (int) i_pImg->transval[1], (int) i_pImg->transval[2],
                                                            (int) i_pImg->transval[2]);
      else if (i_pImg->colorspace == DeviceCMYK)
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Mask[%d %d %d %d %d %d %d %d]\n", (int) i_pImg->transval[0], (int) i_pImg->transval[0],
                                                            (int) i_pImg->transval[1], (int) i_pImg->transval[1], (int) i_pImg->transval[2],
                                                            (int) i_pImg->transval[2], (int) i_pImg->transval[3], (int) i_pImg->transval[3]);
      else
        ;//???
      // Masking by position: separate bitmap mask
    }
    else if (i_pImg->mask != -1)
    {
//      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "", );

//      pdf_printf(p, "/Mask %ld 0 R\n", p->xobjects[p->images[image->mask].no].obj_id);
    }
    switch (i_pImg->colorspace)
    {
      case ImageMask:
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ImageMask true\n");
        break;
      case Indexed:
        {
          if (i_pImg->components != 1)
            break;

          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ColorSpace[/Indexed/DeviceRGB ");
          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%d %ld 0 R]\n", i_pImg->palette_size - 1, (*i_pObjCount));
        }
        break;
      default:
        // colorize the image with a spot color
/*
        if (image->colorspace >= LastCS)
        {
          // TODO: reconsider spot color numbering scheme
          int spot = image->colorspace - LastCS;
          p->colorspaces[spot].used_on_current_page = pdf_true;
          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "", );

          pdf_printf(p, "/ColorSpace %ld 0 R\n", p->colorspaces[spot].obj_id);
          // the following is tricky: /Separation color space always works as a subtractive color space. The image, however, is Grayscale, and therefore additive.
          if (firststrip)
            image->invert = !image->invert;
        }
        else
*/
        {
          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ColorSpace/%s\n", pdf_colorspace_names[i_pImg->colorspace]);
        }
        break;
    }
/*
    //* special case: referenced image data instead of direct data
    if (image->reference != pdf_ref_direct)
    {
      if (image->compression != none)
      {
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/FFilter[/%s]\n", pdf_filter_names[i_pImg->compression]);
      }
      if (image->compression == ccitt)
      {
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/FDecodeParms[<<", );
        if ((int) image->width != 1728)  // CCITT default width
          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Columns %d", (int)i_pImg->width);
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Rows %d", (int)i_pImg->height);
        //* write CCITT parameters if required
        if (image->params)
          pdf_puts(p, image->params);
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>]\n", );
      }
      if (image->reference == pdf_ref_file)
      {
        // LATER: make image file name platform-neutral: Change : to / on the Mac Change \ to / on Windows
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/F(%s)/Length 0", i_pImg->filename);
      }
      else if (image->reference == pdf_ref_url)
      {
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/F<</FS/URL/F(%s)>>/Length 0", i_pImg->filename);
      }
      pdf_end_dict(p);    // XObject
      pdf_begin_stream(p);    // dummy image data
      pdf_end_stream(p);    // dummy image data
      if (PDF_GET_STATE(p) == pdf_state_page)
        pdf_begin_contents_section(p);
      pdf_end_obj(p);      // XObject
      return;
    }
*/
    // Now the (more common) handling of actual image data to be included in the PDF output.
    // do we need a filter (either ASCII or decompression)?
/*
    if (p->debug['a'])
    {
      pdf_puts(p, "/Filter[/ASCIIHexDecode");
      if (image->compression != none)
        pdf_printf(p, "/%s", pdf_filter_names[image->compression]);
      pdf_puts(p, "]\n");
    }
    else
*/
    {
      // force compression if not a recognized precompressed image format
      if (!i_pImg->use_raw)
        i_pImg->compression = flate;
      if (i_pImg->compression != none)
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Filter[/%s]\n", pdf_filter_names[i_pImg->compression]);
    }
    // prepare precompressed (raw) image data
    if (i_pImg->use_raw)
    {
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/DecodeParms[<<");
      // write EarlyChange or CCITT parameters if required
      if (i_pImg->compression == flate || i_pImg->compression == lzw)
      {
        if (i_pImg->predictor != pred_default)
        {
          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Predictor %d", (int) i_pImg->predictor);
          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Columns %d", (int) i_pImg->width);
          if (i_pImg->bitspersample != 8)
            pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/BitsPerComponent %d", i_pImg->bitspersample);
          if (i_pImg->colorspace < LastCS)
          {
            switch (i_pImg->colorspace)
            {
              case Indexed:
              case ImageMask:
              case DeviceGray:
                // 1 is default
                break;
              case DeviceRGB:
                pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Colors 3");
                break;
              case DeviceCMYK:
                pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Colors 4");
                break;
              default:
              break;
            }
          }
        }
      }
      if (i_pImg->compression == ccitt)
      {
        if ((int) i_pImg->width != 1728)  // CCITT default width
          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Columns %d", (int)i_pImg->width);
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Rows %d", (int)i_pImg->height);
      }
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>]\n");
    }
    if (i_pImg->invert)
    {
      int i;
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Decode[1 0");
      for (i = 1; i < i_pImg->components; i++)
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, " 1 0");
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "]\n");
    }
    // Write the actual image data

    if (i_pImg->use_raw)
    {
      if (i_pImg->nbytes > 0)
      {
        unsigned char *pSave = NULL;
        unsigned char *pFile = i_pImg->data;
        size_t Alloc = 10000 > i_pImg->nbytes ? 10000 : 10000 + i_pImg->nbytes;
        size_t Used = 0;
        lSize = i_pImg->nbytes;
        pOutput = (unsigned char *)PDFMalloc(Alloc);
        memset(pOutput, 0, Alloc);
        pSave = pOutput;
        do
        {
          pFile += i_pImg->startpos;
          memcpy(pOutput, pFile, i_pImg->nbytes);
          pFile += i_pImg->nbytes;
          pOutput += i_pImg->nbytes;
          Used += i_pImg->nbytes;
          // proceed to next IDAT chunk
          pFile += 4;//CRC
          i_pImg->nbytes = ((int) pFile[0] << 24) + (pFile[1] << 16) + (pFile[2] << 8) + pFile[3]; //length
          pFile += 4;
          if ((((int) pFile[0] << 24) + (pFile[1] << 16) + (pFile[2] << 8) + pFile[3]) != SPNG_CHUNK_IDAT)
          {
            i_pImg->nbytes = 0;
            break;
          }
          lSize += i_pImg->nbytes;
          pFile += 4;
          if (Alloc - Used < i_pImg->nbytes)
          {
            Alloc += i_pImg->nbytes - (Alloc - Used);
            Alloc += 10000;

            PDFRealloc((void **)&pSave, Alloc, 10000);
            pOutput = pSave + Used;
          }
        } while (i_pImg->nbytes > 0);
        pOutput = pSave;
      }
    }
    else
    {
      lSize = i_pImg->height * i_pImg->rowbytes;
      pOutput = NULL;

      pOutput = PDFCompressStream(i_pImg->raster, &lSize);
    }

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Length %ld\n", lSize);

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "stream\n");

    if (PDFGetDocClass(pdf)->Encrypt())
      PDFEncrypt(pdf, pOutput, lSize, i_pImg->lObjIndex, 0);

    pdf->lActualSize += PDFWriteBufToDocStream((PDFDocPTR)pdf, (const char *)pOutput, lSize);

    free(pOutput);

    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "\nendstream\n");
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "endobj\n");
  }

  if ((i_pImg->lObjIndex + 1) >= pdf->iCurrAllocObjects)
  {
    pdf->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(pdf->pObjOffset), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(pdf->pObjIndex), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
//  pdf->pObjOffset[i_pImg->lObjIndex +1] = pdf->lActualSize;

  if (i_pImg->colorspace == Indexed)
  {
    long lSize = i_pImg->palette_size * 3;
    unsigned char *pOutput = NULL;

    pdf->pObjOffset[(*i_pObjCount)] = pdf->lActualSize;

    pOutput = PDFCompressStream((unsigned char *)i_pImg->colormap, &lSize);
    if (pOutput)
    {
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 obj\n", (*i_pObjCount));
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<<\n");

      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Filter[/FlateDecode]\n");
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Length %ld\n", lSize);

      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");

      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "stream\n");

      if (PDFGetDocClass(pdf)->Encrypt())
        PDFEncrypt(pdf, pOutput, lSize, (*i_pObjCount), 0);

      pdf->lActualSize += PDFWriteBufToDocStream((PDFDocPTR)pdf, (const char *)pOutput, lSize);

      free(pOutput);

      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "\nendstream\n");
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "endobj\n");

      if ((*i_pObjCount) >= pdf->iCurrAllocObjects)
      {
        pdf->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
        PDFRealloc((void **)&(pdf->pObjOffset), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
        PDFRealloc((void **)&(pdf->pObjIndex), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
      }
      (*i_pObjCount)++;
      pdf->pObjOffset[i_pImg->lObjIndex +1] = pdf->lActualSize;
    }
  }
  else
    pdf->pObjOffset[i_pImg->lObjIndex +1] = pdf->lActualSize;

  return(pdf->lActualSize);
}

long PDFWriteOutlinesObject(PDFDocPTR pdf, long objNumber)
{
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 obj\n", objNumber);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<<\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Type /Outlines\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Count %ld\n", pdf->lOutlineEntriesCount);
  if(pdf->lOutlineEntriesCount)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/First %ld 0 R\n", pdf->pFirstOutlineEntry->lObjectIndex);
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Last %ld 0 R\n", pdf->pLastOutlineEntry->lObjectIndex);
  }
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "endobj\n");

  if ((objNumber + 1) >= pdf->iCurrAllocObjects)
  {
    pdf->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(pdf->pObjOffset), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(pdf->pObjIndex), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
  return(pdf->lActualSize);
}

long PDFWriteProcSetArray(PDFDocPTR pdf, long objNumber)
{
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 obj\n", objNumber);
  if(PDFGetDocClass(pdf)->GetImagesCount())
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "[/PDF /Text ");
    if(pdf->imageFlagBCI & 1)
    {
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ImageB ");
    }
    if(pdf->imageFlagBCI & 2)
    {
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ImageC ");
    }
    if(pdf->imageFlagBCI & 4)
    {
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/ImageI ");
    }
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "]\n");
  }
  else
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "[/PDF /Text]\n");
  }
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "endobj\n");

  if ((objNumber + 1) >= pdf->iCurrAllocObjects)
  {
    pdf->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(pdf->pObjOffset), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(pdf->pObjIndex), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
  return(pdf->lActualSize);
}

long PDFWriteOneOutlineEntry(PDFDocPTR pdf, PDFOutlineEntry *olent)
{
  char *pTemp;
  long lTemp;

  lTemp = pdf->lActualSize;

  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "%ld 0 obj\n", olent->lObjectIndex);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "<<\n");
  if(olent->pParent)
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Parent %ld 0 R\n", olent->pParent->lObjectIndex);
  else
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Parent %ld 0 R\n", pdf->pObjIndex[PDF_Outlines]);
  if(olent->lPage >= 0)
  {
    /* goto a specified page in the current document */
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Dest [%ld 0 R %s]\n", pdf->m_vctPages[olent->lPage]->lObjIndex, olent->pDestSpec);
  }
  else
  {
    /* Action Outline, we just copy content of dictionary as this is quite complex.
    No checking is performed on the correctness of syntax.
    */
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/A <<\n%s\n>>\n", olent->pDestSpec);
  }

  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/C [%.4f %.4f %.4f]", olent->fCkeyR, olent->fCkeyG, olent->fCkeyB);
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/F %d", olent->sFkey);


  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Title (");

  CPDFBaseFont *pFont = PDFGetDocClass(pdf)->GetFont(olent->lFont);
  if (pFont && pFont->GetFontCodePage() != lDefCodePage)
  {
    string strIn = olent->pTitle;
    string strOut;
    long lLen;
    char *pEncr;
    if (!pFont->GetUnicodeValue(strIn, strOut))
      return pdf->lActualSize;

    pEncr = (char *)PDFMalloc(strOut.size());
    if (!pEncr)
      return pdf->lActualSize;
    memcpy(pEncr, strOut.c_str(), strOut.size());

    if (PDFGetDocClass(pdf)->Encrypt())
      PDFEncrypt(pdf, (unsigned char *)pEncr, strOut.size(), olent->lObjectIndex, 0);

    pTemp = PDFescapeSpecialCharsBinary(pEncr, strOut.size(), &lLen);
    pdf->lActualSize += PDFWriteBufToDocStream((PDFDocPTR)pdf, pTemp, lLen);
    free(pTemp);
    free(pEncr);

  } else
  {
    if (PDFGetDocClass(pdf)->Encrypt())
      PDFEncrypt(pdf, (unsigned char *)olent->pTitle, olent->lTitleLength, olent->lObjectIndex, 0);
    pTemp = PDFescapeSpecialCharsBinary(olent->pTitle, olent->lTitleLength, &olent->lTitleLength);
    pdf->lActualSize += PDFWriteBufToDocStream((PDFDocPTR)pdf, pTemp, olent->lTitleLength);
    free(pTemp);
  }


  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ")\n");

  /* Now, write out, first, last, next, prev if they are not NULL */
  if(olent->pNext)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Next %ld 0 R\n", olent->pNext->lObjectIndex);
  }
  if(olent->pPrev)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Prev %ld 0 R\n", olent->pPrev->lObjectIndex);
  }
  if(olent->pFirst)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/First %ld 0 R\n", olent->pFirst->lObjectIndex);
  }
  if(olent->pLast)
  {
    pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Last %ld 0 R\n", olent->pLast->lObjectIndex);
  }
  if(olent->lCount != 0)
  {
    if(olent->bOpen)
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Count %ld\n", olent->lCount);
    else
      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Count -%ld\n", olent->lCount);
  }
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>\n");
  pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "endobj\n");

  if ((olent->lObjectIndex + 1) >= pdf->iCurrAllocObjects)
  {
    pdf->iCurrAllocObjects += OBJECTS_ALLOC_STEP;
    PDFRealloc((void **)&(pdf->pObjOffset), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
    PDFRealloc((void **)&(pdf->pObjIndex), pdf->iCurrAllocObjects * sizeof(long), OBJECTS_ALLOC_STEP * sizeof(long));
  }
  pdf->pObjOffset[olent->lObjectIndex + 1] = pdf->lActualSize;

  lTemp = pdf->lActualSize - lTemp;

  return(pdf->lActualSize);
}

long PDFOutlineEntriesCount(PDFOutlineEntry *pFirst)
{
  PDFOutlineEntry *pEnt;
  long lCount = 0;
  pEnt = pFirst;
  do
  {
    if(pEnt->pFirst != NULL)
      lCount += PDFOutlineEntriesCount(pEnt->pFirst);
    lCount++;
    pEnt = pEnt->pNext;
  } while(pEnt != NULL);
  return lCount;
}

void PDFserializeOutlineEntries(long *objCount, PDFOutlineEntry *first, PDFOutlineEntry *last)
{
  PDFOutlineEntry *olent;
  olent = first;
  do {
  olent->lObjectIndex = (*objCount)++;
  if(olent->pFirst != NULL)
  PDFserializeOutlineEntries(objCount, olent->pFirst, olent->pLast);  /* recurse */
  olent = olent->pNext;    /* do the next one on the same level */
  } while(olent != NULL);

  last;
}

void PDFWriteOutlineEntries(PDFDocPTR pdf, PDFOutlineEntry *first, PDFOutlineEntry *last)
{
  PDFOutlineEntry *olent;
  olent = first;
  do
  {
    PDFWriteOneOutlineEntry(pdf, olent);
    if(olent->pFirst != NULL)
      PDFWriteOutlineEntries(pdf, olent->pFirst, olent->pLast);
    olent = olent->pNext;
  } while(olent != NULL);

  last;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PDFfreeAllOutlineEntries(PDFOutlineEntry *firstOL, PDFOutlineEntry *lastOL)
{
  PDFOutlineEntry *olent, *olent2;
  olent = firstOL;
  do
  {
    if(olent->pFirst != NULL)
      PDFfreeAllOutlineEntries(olent->pFirst, olent->pLast);
    if(olent->pTitle)
      free(olent->pTitle);
    if(olent->pDestSpec)
      free(olent->pDestSpec);
    olent2 = olent->pNext;
    free(olent);
    olent = olent2;
  }
  while(olent != NULL);
  lastOL;
}

// shared object



