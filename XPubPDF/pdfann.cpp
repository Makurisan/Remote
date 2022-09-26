

#include "pdferr.h"
#include "pdfdef.h"
#include "pdffnc.h"
#include "PDFBaseFont.h"
#include "PDFUtil.h"
#include "PDFAnnotation.h"

#include <vector>
#include <string>

using namespace std;

long PDFCreateLinkAnnotation(PDFDocPTR i_pPDFDoc, const char *i_pczName, long i_lPage,
                             char *i_pczDest, char *i_pczAction, float i_fZoom)
{
  long lRet = NOT_USED;
  CPDFAnnotation *pAnnot = NULL;

  pAnnot = new CPDFAnnotation(PDFGetDocClass(i_pPDFDoc), i_pczName, i_lPage, i_pczDest, i_pczAction, i_fZoom);
  if (!pAnnot)
    return lRet;
  i_pPDFDoc->m_vctAnnots.push_back((PDFAnnotation *)pAnnot);

  return i_pPDFDoc->m_vctAnnots.size() - 1;
}

long PDFCreateLinkAnnotationFile(PDFDocPTR i_pPDFDoc, const char *i_pczName,
                                  long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                                  long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                                  const char *i_pLocation)
{
  long lLen/*, lRet*/;
  char *pczTemp = NULL, *pczDest = NULL, *pczAction = NULL, czPage[10] = {0};
  if (!i_pLocation)
    return NOT_USED;
/*
  lRet = PDFGetAnnotationIndex(i_pPDFDoc, i_pczName);
  if (lRet >= 0)
    return lRet;
*/
  pczTemp = PDFescapeSpecialCharsBinary(i_pLocation, PDFStrLenC(i_pLocation), &lLen);
  if (!pczTemp)
    return NOT_USED;
  if (!PDFDestSpec(&pczDest, i_eDest, PDFConvertDimensionToDefaultL(i_lLeft),
                   PDFConvertDimensionToDefaultL(i_lTop),
                   PDFConvertDimensionToDefaultL(i_lRight),
                   PDFConvertDimensionToDefaultL(i_lBottom), i_fZoom))
  {
    free(pczTemp);
    return NOT_USED;
  }
  sprintf(czPage, "%ld", i_lPage - 1);
  pczAction = (char *)PDFMalloc(37 + PDFStrLenC(pczTemp) + PDFStrLenC(pczDest) + PDFStrLenC(czPage));
  sprintf(pczAction, "/Type /Action /S /GoToR /D [%s %s] /F (%s)", czPage, pczDest, pczTemp);
  free(pczTemp);
  free(pczDest);
  return PDFCreateLinkAnnotation(i_pPDFDoc, i_pczName, NOT_USED, NULL, pczAction, i_fZoom);
}

long PDFCreateLinkAnnotationURI(PDFDocPTR i_pPDFDoc, const char *i_pczName,
                                const char *i_pLocation)
{
  long lLen/*, lRet*/;
  char *pTemp = NULL, *pczDest = NULL;
  if (!i_pLocation)
    return NOT_USED;
/*
  lRet = PDFGetAnnotationIndex(i_pPDFDoc, i_pczName);
  if (lRet >= 0)
    return lRet;
*/
  pTemp = PDFescapeSpecialCharsBinary(i_pLocation, PDFStrLenC(i_pLocation), &lLen);
  if (!pTemp)
    return NOT_USED;
  pczDest = (char *)PDFMalloc(30 + PDFStrLenC(pTemp));
  sprintf(pczDest, "/Type /Action /S /URI /URI (%s)", pTemp);
  free(pTemp);
  return PDFCreateLinkAnnotation(i_pPDFDoc, i_pczName, NOT_USED, NULL, pczDest, NOT_USED);
}

long PDFCreateLinkAnnotationDest(PDFDocPTR i_pPDFDoc, const char *i_pczName,
                  long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                  long i_lTop, long i_lRight, long i_lBottom, float i_fZoom)
{
  char *pczDest = NULL;
/*
  long lRet;
  lRet = PDFGetAnnotationIndex(i_pPDFDoc, i_pczName);
  if (lRet >= 0)
    return lRet;
*/
  if (!PDFDestSpec(&pczDest, i_eDest, PDFConvertDimensionToDefaultL(i_lLeft),
                   PDFConvertDimensionToDefaultL(i_lTop),
                   PDFConvertDimensionToDefaultL(i_lRight),
                   PDFConvertDimensionToDefaultL(i_lBottom), i_fZoom))
    return NOT_USED;
  return PDFCreateLinkAnnotation(i_pPDFDoc, i_pczName, i_lPage, pczDest, NULL, i_fZoom);
}

long PDFCreateLinkAnnotationEmptyDest(PDFDocPTR i_pPDFDoc, const char *i_pczName)
{
  long lRet;
  if (!i_pczName)
    return NOT_USED;
  lRet = PDFGetAnnotationIndex(i_pPDFDoc, i_pczName);
  if (lRet >= 0)
    return lRet;
  return PDFCreateLinkAnnotation(i_pPDFDoc, i_pczName, NOT_USED, NULL, NULL, NOT_USED);
}

bool_a PDFGetAnnotationIndex(PDFDocPTR i_pPDFDoc, const char *i_pczName)
{
  long i;
  if (!i_pczName)
    return NOT_USED;
  for(i = 0; i < i_pPDFDoc->m_vctAnnots.size(); i++)
  {
    if (i_pPDFDoc->m_vctAnnots[i]->pczAnnotName)
    {
      if (0 == strcmp(i_pPDFDoc->m_vctAnnots[i]->pczAnnotName, i_pczName))
      {
        return i;
      }
    }
  }
  return NOT_USED;
}

bool_a PDFChangeAnnotationDest(PDFDocPTR i_pPDFDoc, const char *i_pczName,
                  long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                  long i_lTop, long i_lRight, long i_lBottom, float i_fZoom)
{
  long i;
  if (!i_pczName)
    return false_a;
  for(i = 0; i< i_pPDFDoc->m_vctAnnots.size(); i++)
  {
    if (i_pPDFDoc->m_vctAnnots[i]->pczAnnotName)
    {
      if (0 == strcmp(i_pPDFDoc->m_vctAnnots[i]->pczAnnotName, i_pczName))
      {
        if (i_fZoom < 0)
          i_fZoom = i_pPDFDoc->m_vctAnnots[i]->m_fZoom;
        char *pczDest = NULL;
        if (!PDFDestSpec(&pczDest, i_eDest, PDFConvertDimensionToDefaultL(i_lLeft),
                        PDFConvertDimensionToDefaultL(i_lTop),
                        PDFConvertDimensionToDefaultL(i_lRight),
                        PDFConvertDimensionToDefaultL(i_lBottom), i_fZoom))
          return false_a;
        if (i_pPDFDoc->m_vctAnnots[i]->pczAction)
        {
          free(i_pPDFDoc->m_vctAnnots[i]->pczAction);
          i_pPDFDoc->m_vctAnnots[i]->pczAction = NULL;
        }
        if (i_pPDFDoc->m_vctAnnots[i]->pDestSpec)
        {
          free(i_pPDFDoc->m_vctAnnots[i]->pDestSpec);
          i_pPDFDoc->m_vctAnnots[i]->pDestSpec = NULL;
        }
        i_pPDFDoc->m_vctAnnots[i]->pDestSpec = pczDest;
        i_pPDFDoc->m_vctAnnots[i]->lDestPage = i_lPage - 1;
      }
    }
  }

  return true_a;
}

bool_a PDFAddAnnotToPage(PDFDocPTR i_pPDFDoc, long i_lAnnot, long i_lPage)
{
  PDFPage *pPage = NULL;
  if (i_lAnnot < 0/* || i_lAnnot >= i_pPDFDoc->m_vctAnnots.size()*/
    || i_lPage < 0 || i_lPage >= PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;
  pPage = i_pPDFDoc->m_vctPages[i_lPage];
  if (!pPage)
    return false_a;
  {
    PDFList *pAnnotP, *pAnnotL = pPage->pAnnotIdx;
    bool_a bF = false_a;
    pAnnotP = pAnnotL;
    while(pAnnotL)
    {
      if(pAnnotL->lValue == i_lAnnot)
      {
        bF = true_a;
        break;
      }
      pAnnotP = pAnnotL;
      pAnnotL = pAnnotL->pNext;
    }
    if(!bF)
    {
      pAnnotL = (PDFList *)PDFMalloc((size_t)sizeof(PDFList));
      if (!pAnnotL)
        return false_a;
      pAnnotP->pNext = pAnnotL;
      pAnnotP->lValue = i_lAnnot;
      pAnnotL->pNext = NULL;
      pAnnotL->lValue = NOT_USED;
      i_pPDFDoc->m_vctPages[i_lPage]->lAnnotCount++;
    }
  }
  return true_a;
}

bool_a PDFHasPageAnnot(PDFDocPTR i_pPDFDoc, long i_lAnnot, long i_lPage)
{
  PDFPage *pPage = NULL;
  if (i_lAnnot < 0 || i_lAnnot >= i_pPDFDoc->m_vctAnnots.size()
      || i_lPage < 0 || i_lPage >= PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;
  pPage = i_pPDFDoc->m_vctPages[i_lPage];
  if (!pPage)
    return false_a;
  {
    PDFList *pAnnotL = pPage->pAnnotIdx;
    while(pAnnotL)
    {
      if(pAnnotL->lValue == i_lAnnot)
        return true_a;
      pAnnotL = pAnnotL->pNext;
    }
  }
  return false_a;
}

bool_a PDFHasAnnotOnlyOnePage(PDFDocPTR i_pPDFDoc, long i_lAnnot)
{
  PDFPage *pPage = NULL;
  long lPage;
  bool_a bF = false_a;
  if (i_lAnnot < 0 || i_lAnnot >= i_pPDFDoc->m_vctAnnots.size())
    return false_a;
  for (lPage = 0; lPage < PDFGetDocClass(i_pPDFDoc)->GetPageCount(); lPage++)
  {
    pPage = i_pPDFDoc->m_vctPages[lPage];
    if (pPage)
    {
      PDFList *pAnnotL = pPage->pAnnotIdx;
      while(pAnnotL)
      {
        if(pAnnotL->lValue == i_lAnnot)
        {
          if (bF)
            return false_a;
          else
          {
            bF = true_a;
            break;
          }
        }
        pAnnotL = pAnnotL->pNext;
      }
    }
  }
  return true_a;
}

PDFAnnotation::PDFAnnotation()
{
  memset(lObjIndex, NOT_USED, sizeof(lObjIndex));
  pczAnnotName = NULL;
  iAnnotFlags = NOT_USED;
  pczAnnotBorder = NULL;
  lFont = NOT_USED;
  fFontSize = NOT_USED;
  pczAction = NULL;
  pDestSpec = NULL;
  m_fZoom = NOT_USED;
  lDestPage = NOT_USED;
  pczCA = NULL;
  sBorderWidth = NOT_USED;
  lFieldFlags = NOT_USED;
  lQ = NOT_USED;
  pczShortDesc = NULL;
  pczValue = NULL;
  lMaxLen = NOT_USED;
}

PDFAnnotation::~PDFAnnotation()
{
  if (this->pczAction)
  {
    free(this->pczAction);
    this->pczAction = NULL;
  }
  if (this->pczAnnotBorder)
  {
    free(this->pczAnnotBorder);
    this->pczAnnotBorder = NULL;
  }
  if (this->pczAnnotName)
  {
    free(this->pczAnnotName);
    this->pczAnnotName = NULL;
  }
  if (this->pDestSpec)
  {
    free(this->pDestSpec);
    this->pDestSpec = NULL;
  }
  if (this->pczCA)
  {
    free(this->pczCA);
    this->pczCA = NULL;
  }
  if (this->pczShortDesc)
  {
    free(this->pczShortDesc);
    this->pczShortDesc = NULL;
  }
  if (this->pczValue)
  {
    free(this->pczValue);
    this->pczValue = NULL;
  }
}

