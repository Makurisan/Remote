
#include "pdferr.h"
#include "pdfimg.h"
#include "pdffnc.h"
#include "PDFBaseFont.h"
#include "PDFUtil.h"


using namespace std;


long CalcParagraphsHeights(PDFParagraph *i_pPara)
{
  if (!i_pPara)
    return 0;
  long lRet = 0;
  PDFParagraph *pPara = i_pPara;
  while (pPara)
  {
    //pLine = pPara->pPDFLastLine;
    //while (pLine)
    //{
    //  pElem = pLine->pElemList;
    //  while (bF && pElem)
    //  {
    //    if (pElem->bCanPageBreak)
    //      pRetElem = pElem;
    //    pElem = pElem->pLCNext;
    //  }
    //  if (pRetElem)
    //     return pRetElem;
    //  if (!bF && (pLine == i_pLine || !i_pLine))
    //    bF = true_a;
    //  pLine = pLine->pLinePrev;
    //}
    lRet += pPara->lParaHeight;
    pPara = pPara->pParaNext;
  }

  return lRet;
}


PDFLineElemList *PDFGetFixTableElem(CPDFTextBox *i_pTextBox, PDFLine *i_pLine)
{
  PDFLineElemList *pElem    = NULL;
  if (!i_pLine || !i_pTextBox)
    return false_a;
  pElem = i_pLine->pElemList;
  while (pElem)
  {
    if (pElem->lSpace > 0)
    {
      PDFTBSpace *pTBSpace = PDFGetTBSpace(i_pTextBox, pElem->lSpace);
      if (pTBSpace)
      {
        if (eFixTableWidth == pTBSpace->eSpaceType)
          return pElem;
      }
    }
    pElem = pElem->pLCNext;
  }
  return NULL;
}

bool_a PDFHasParagraphFixTable(CPDFTextBox *i_pTextBox, PDFParagraph *i_pParagraph)
{
  PDFLine         *pLine    = NULL;
  PDFLineElemList *pElem    = NULL;
  if (!i_pParagraph)
    return false_a;
  pLine = i_pParagraph->pPDFLine;
  while (pLine)
  {
    pElem = pLine->pElemList;
    while (pElem)
    {
      if (pElem->lSpace > 0)
      {
        PDFTBSpace *pTBSpace = PDFGetTBSpace(i_pTextBox, pElem->lSpace);
        if (pTBSpace)
        {
          if (eFixTableWidth == pTBSpace->eSpaceType)
            return true_a;
        }
      }
      pElem = pElem->pLCNext;
    }
    pLine = pLine->pLineNext;
  }
  return false_a;
}

bool_a PDFClearTextBox(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox)
{
/*
  i_pTextBox->m_Color.SetColor(PDF_DEFAULT_RED, PDF_DEFAULT_GREEN, PDF_DEFAULT_BLUE);
  i_pTextBox->m_BgColor.SetColor(PDF_DEFAULT_BG_RED, PDF_DEFAULT_BG_GREEN, PDF_DEFAULT_BG_BLUE);
  i_pTextBox->bTransparent = true_a;
  i_pTextBox->m_TBBgColor.SetColor(PDF_DEFAULT_BG_RED, PDF_DEFAULT_BG_GREEN, PDF_DEFAULT_BG_BLUE);
  i_pTextBox->lTBBgImage = NOT_USED;
  i_pTextBox->recIndents.lBottom = 0;
  i_pTextBox->recIndents.lTop = 0;
  i_pTextBox->recIndents.lLeft = 0;
  i_pTextBox->recIndents.lRight = 0;
  i_pTextBox->lIndent = PDF_DEFAULT_FIRST_INDENT;
  i_pTextBox->sAlignment = TF_LEFT;
  i_pTextBox->sVAlignment = TF_UP;
  i_pTextBox->lPage = i_lPage - 1;
  i_pTextBox->recBorder.fBottom = 0;
  i_pTextBox->recBorder.fLeft = 0;
  i_pTextBox->recBorder.fRight = 0;
  i_pTextBox->recBorder.fTop = 0;
  i_pTextBox->recMargins.lBottom = 0;
  i_pTextBox->recMargins.lLeft = 0;
  i_pTextBox->recMargins.lRight = 0;
  i_pTextBox->recMargins.lTop = 0;
  i_pTextBox->lLineSpace = PDF_DEFAULT_LINE_SPACE;
  i_pTextBox->lLetterSpace = i_pPDFDoc->lLetterSpace;

  i_pTextBox->lTabSize = i_pPDFDoc->lDefTabSpace;
  i_pTextBox->cTabDecimalType = i_pPDFDoc->cTabDecimalType;
*/
  i_pTextBox->lTBHeight = 0;
  i_pTextBox->lTBWidth = 0;
  i_pTextBox->bTBCanPageBreak = false_a;
  i_pTextBox->bTBBreak = false_a;
  i_pTextBox->bColumnBreak = false_a;
  i_pTextBox->bPageMark = false_a;
  i_pTextBox->pTBTabStops = NULL;
  i_pTextBox->sTabsCount = 0;
  i_pTextBox->pTBImages = NULL;
  i_pTextBox->pTBSpaces = NULL;
  i_pTextBox->pParagraph = NULL;
  i_pTextBox->pActiveParagraph = NULL;
  i_pTextBox->pTBNext = NULL;
  i_pTextBox->pTBPrev = NULL;
  i_pTextBox->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBox);
  i_pTextBox->pActiveParagraph = i_pTextBox->pParagraph;
  return true_a;
}

bool_a PDFCopyElementAttributtesToTextBox(CPDFTextBox *i_pTextBox, PDFLineElemList *i_pElement)
{
  if (!i_pTextBox || !i_pElement)
    return false_a;
  COPY_ELEM_TO_TEXTBOX(i_pTextBox, i_pElement);
  return true_a;
}

bool_a PDFCopyParagraphAttributtesToTextBox(CPDFTextBox *i_pTextBox, PDFParagraph *i_pParagraph)
{
  if (!i_pTextBox || !i_pParagraph)
    return false_a;
  COPY_PARA_TO_TEXTBOX(i_pTextBox, i_pParagraph);
  if (i_pTextBox->pActiveParagraph)
  {
    free(i_pTextBox->pActiveParagraph->pParaTabStops);
    i_pTextBox->pActiveParagraph->pParaTabStops = NULL;
    i_pTextBox->pActiveParagraph->sParaTabsCount = 0;
    if (i_pTextBox->sTabsCount > 0 && i_pTextBox->pTBTabStops)
    {
      i_pTextBox->pActiveParagraph->pParaTabStops = (PDFTabStop*)PDFMalloc(i_pTextBox->sTabsCount * sizeof(PDFTabStop));
      memcpy(i_pTextBox->pActiveParagraph->pParaTabStops, i_pTextBox->pTBTabStops, i_pTextBox->sTabsCount * sizeof(PDFTabStop));
      i_pTextBox->pActiveParagraph->sParaTabsCount = i_pTextBox->sTabsCount;
    }

    i_pTextBox->pActiveParagraph->lParaHeight += i_pParagraph->recIndents.lTop
                                                      - i_pTextBox->pActiveParagraph->recIndents.lTop;
    i_pTextBox->pActiveParagraph->lParaHeight += i_pParagraph->recIndents.lBottom
                                                      - i_pTextBox->pActiveParagraph->recIndents.lBottom;
    i_pTextBox->pActiveParagraph->lParaHeight += i_pParagraph->lLineSpace
                                                      - i_pTextBox->pActiveParagraph->lLineSpace;

    PDFLine *pLine = i_pTextBox->pActiveParagraph->pPDFLine;
    while (pLine)
    {
      pLine->lCurrLineHeight += i_pParagraph->lLineSpace - i_pTextBox->pActiveParagraph->lLineSpace;
      pLine = pLine->pLineNext;
    }

    COPY_PARA_TO_PARA(i_pTextBox->pActiveParagraph, i_pParagraph);

    if (PDFIsParagraphEmpty(i_pTextBox->pActiveParagraph) &&
          PDFIsParagraphEmpty(i_pParagraph))
    {
      i_pTextBox->pActiveParagraph->lParaWidth = i_pParagraph->lParaWidth;
      i_pTextBox->pActiveParagraph->lParaHeight = i_pParagraph->lParaHeight;
      if (i_pTextBox->pActiveParagraph->pPDFLine &&
            (i_pTextBox->pActiveParagraph->pPDFLine == i_pTextBox->pActiveParagraph->pPDFLastLine) &&
            i_pParagraph->pPDFLine)
      {
        i_pTextBox->pActiveParagraph->pPDFLine->lCurrLineWidth = i_pParagraph->pPDFLine->lCurrLineWidth;
        i_pTextBox->pActiveParagraph->pPDFLine->lCurrLineHeight = i_pParagraph->pPDFLine->lCurrLineHeight;
        i_pTextBox->pActiveParagraph->pPDFLine->lCurrLineAsc = i_pParagraph->pPDFLine->lCurrLineAsc;
        i_pTextBox->pActiveParagraph->pPDFLine->lCurrLineDesc = i_pParagraph->pPDFLine->lCurrLineDesc;
        i_pTextBox->pActiveParagraph->pPDFLine->sLastBlock = i_pParagraph->pPDFLine->sLastBlock;
        i_pTextBox->pActiveParagraph->pPDFLine->sLastTab = i_pParagraph->pPDFLine->sLastTab;
        i_pTextBox->pActiveParagraph->pPDFLine->bNewLine = i_pParagraph->pPDFLine->bNewLine;
      }
    }
  }
  return true_a;
}

bool_a PDFCopyTextBoxAttributtesToTextBox(CPDFTextBox *i_pTextBoxTo, CPDFTextBox *i_pTextBoxFrom)
{
  if (!i_pTextBoxTo || !i_pTextBoxFrom)
    return false_a;
  COPY_TB(i_pTextBoxTo, i_pTextBoxFrom);
  return true_a;
}

void PDFCopyTextBox(CPDFTextBox *i_pTextBoxTo, CPDFTextBox *i_pTextBoxFrom)
{
  if (!i_pTextBoxTo || !i_pTextBoxFrom)
    return;
  i_pTextBoxTo->m_Color = i_pTextBoxFrom->m_Color;
  i_pTextBoxTo->m_BgColor = i_pTextBoxFrom->m_BgColor;
  i_pTextBoxTo->bTransparent = i_pTextBoxFrom->bTransparent;
  i_pTextBoxTo->m_TBBgColor = i_pTextBoxFrom->m_TBBgColor;
  i_pTextBoxTo->lTBBgImage = i_pTextBoxFrom->lTBBgImage;
  COPY_REC(i_pTextBoxTo->recMargins, i_pTextBoxFrom->recMargins);
  COPY_REC(i_pTextBoxTo->recIndents, i_pTextBoxFrom->recIndents);
  i_pTextBoxTo->lIndent = i_pTextBoxFrom->lIndent;
  COPY_FREC(i_pTextBoxTo->recBorder, i_pTextBoxFrom->recBorder);
  i_pTextBoxTo->sAlignment = i_pTextBoxFrom->sAlignment;
  i_pTextBoxTo->sVAlignment = i_pTextBoxFrom->sVAlignment;
  i_pTextBoxTo->lPage = i_pTextBoxFrom->lPage;
  i_pTextBoxTo->m_eTextDirection = i_pTextBoxFrom->m_eTextDirection;
  COPY_REC(i_pTextBoxTo->recTB, i_pTextBoxFrom->recTB);
  i_pTextBoxTo->lLineSpace = i_pTextBoxFrom->lLineSpace;
  i_pTextBoxTo->lLetterSpace = i_pTextBoxFrom->lLetterSpace;
  i_pTextBoxTo->bTBCanPageBreak = i_pTextBoxFrom->bTBCanPageBreak;
  i_pTextBoxTo->bTBBreak = i_pTextBoxFrom->bTBBreak;
  i_pTextBoxTo->bColumnBreak = i_pTextBoxFrom->bColumnBreak;
  i_pTextBoxTo->bPageMark = i_pTextBoxFrom->bPageMark;
  i_pTextBoxTo->bTBCopy = i_pTextBoxFrom->bTBCopy;
  i_pTextBoxTo->lTBWidth = i_pTextBoxFrom->lTBWidth;
  i_pTextBoxTo->lTBHeight = i_pTextBoxFrom->lTBHeight;
  i_pTextBoxTo->lTabSize = i_pTextBoxFrom->lTabSize;
  i_pTextBoxTo->cTabDecimalType = i_pTextBoxFrom->cTabDecimalType;
  i_pTextBoxTo->pTBTabStops = i_pTextBoxFrom->pTBTabStops;
  i_pTextBoxTo->sTabsCount = i_pTextBoxFrom->sTabsCount;
  i_pTextBoxTo->pTBImages = i_pTextBoxFrom->pTBImages;
  i_pTextBoxTo->pTBSpaces = i_pTextBoxFrom->pTBSpaces;
  i_pTextBoxTo->pParagraph = i_pTextBoxFrom->pParagraph;
  i_pTextBoxTo->pActiveParagraph = i_pTextBoxFrom->pActiveParagraph;
  i_pTextBoxTo->pTBNext = i_pTextBoxFrom->pTBNext;
  i_pTextBoxTo->pTBPrev = i_pTextBoxFrom->pTBPrev;
}

bool_a PDFCopyTextBoxAttributes(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBoxTo, CPDFTextBox *i_pTextBoxFrom)
{
  long lTop = 0;
  if (!i_pTextBoxTo || !i_pTextBoxFrom || !i_pPDFDoc)
    return false_a;
  i_pTextBoxTo->m_Color = i_pTextBoxFrom->m_Color;
  i_pTextBoxTo->m_BgColor = i_pTextBoxFrom->m_BgColor;
  i_pTextBoxTo->bTransparent = i_pTextBoxFrom->bTransparent;
  i_pTextBoxTo->m_TBBgColor = i_pTextBoxFrom->m_TBBgColor;
  i_pTextBoxTo->lTBBgImage = i_pTextBoxFrom->lTBBgImage;

  i_pTextBoxTo->lTBHeight += i_pTextBoxFrom->recIndents.lTop - i_pTextBoxTo->recIndents.lTop;
  i_pTextBoxTo->lTBHeight += i_pTextBoxFrom->recIndents.lBottom - i_pTextBoxTo->recIndents.lBottom;

  COPY_REC(i_pTextBoxTo->recIndents, i_pTextBoxFrom->recIndents);

  i_pTextBoxTo->lIndent = i_pTextBoxFrom->lIndent;
  i_pTextBoxTo->sAlignment = i_pTextBoxFrom->sAlignment;
  i_pTextBoxTo->sVAlignment = i_pTextBoxFrom->sVAlignment;
//  i_pTextBoxTo->lPage = i_pTextBoxFrom->lPage;
  i_pTextBoxTo->recBorder.fBottom = i_pTextBoxFrom->recBorder.fBottom;
  i_pTextBoxTo->recBorder.fLeft = i_pTextBoxFrom->recBorder.fLeft;
  i_pTextBoxTo->recBorder.fRight = i_pTextBoxFrom->recBorder.fRight;
  i_pTextBoxTo->recBorder.fTop = i_pTextBoxFrom->recBorder.fTop;
  COPY_REC(i_pTextBoxTo->recMargins, i_pTextBoxFrom->recMargins);
  i_pTextBoxTo->lLineSpace = i_pTextBoxFrom->lLineSpace;
  i_pTextBoxTo->lLetterSpace = i_pTextBoxFrom->lLetterSpace;
  i_pTextBoxTo->lTabSize = i_pTextBoxFrom->lTabSize;
  i_pTextBoxTo->cTabDecimalType = i_pTextBoxFrom->cTabDecimalType;
//  i_pTextBox->lTBHeight = ;
//  i_pTextBox->lTBWidth = ;
//  i_pTextBox->bTBCanPageBreak = ;
//  i_pTextBox->bTBBreak = ;
//  i_pTextBox->bColumnBreak = ;
//  i_pTextBox->bPageMark = ;
  if (i_pTextBoxTo->pTBTabStops)
  {
    free(i_pTextBoxTo->pTBTabStops);
    i_pTextBoxTo->pTBTabStops = NULL;
  }
  if (i_pTextBoxFrom->sTabsCount && i_pTextBoxFrom->pTBTabStops)
  {
    i_pTextBoxTo->pTBTabStops = (PDFTabStop*)PDFMalloc(i_pTextBoxFrom->sTabsCount * sizeof(PDFTabStop));
    memcpy(i_pTextBoxTo->pTBTabStops, i_pTextBoxFrom->pTBTabStops, i_pTextBoxFrom->sTabsCount * sizeof(PDFTabStop));
    i_pTextBoxTo->sTabsCount = i_pTextBoxFrom->sTabsCount;
  }

  i_pTextBoxTo->pParagraph->lParaHeight += i_pTextBoxTo->recIndents.lTop
                                                          - i_pTextBoxTo->pParagraph->recIndents.lTop;
  i_pTextBoxTo->pParagraph->lParaHeight += i_pTextBoxTo->recIndents.lBottom
                                                          - i_pTextBoxTo->pParagraph->recIndents.lBottom;

  COPY_REC(i_pTextBoxTo->pParagraph->recIndents, i_pTextBoxTo->recIndents);
  i_pTextBoxTo->pParagraph->lIndent = i_pTextBoxTo->lIndent;
  i_pTextBoxTo->pParagraph->sAlignment = i_pTextBoxTo->sAlignment;


  PDFLine *pLine = i_pTextBoxTo->pParagraph->pPDFLine;
  while (pLine)
  {
    pLine->lCurrLineHeight += i_pTextBoxTo->lLineSpace - i_pTextBoxTo->pParagraph->lLineSpace;
    pLine = pLine->pLineNext;
  }
  i_pTextBoxTo->pParagraph->lLineSpace = i_pTextBoxTo->lLineSpace;


  i_pTextBoxTo->pParagraph->lParaTabSize = i_pTextBoxTo->lTabSize;
  i_pTextBoxTo->pParagraph->cParaTabDecimalType = i_pTextBoxTo->cTabDecimalType;

  if (i_pTextBoxTo->sTabsCount && i_pTextBoxTo->pTBTabStops)
  {
    if (i_pTextBoxTo->pParagraph->pParaTabStops)
      free(i_pTextBoxTo->pParagraph->pParaTabStops);
    i_pTextBoxTo->pParagraph->pParaTabStops = (PDFTabStop*)PDFMalloc(i_pTextBoxTo->sTabsCount * sizeof(PDFTabStop));
    memcpy(i_pTextBoxTo->pParagraph->pParaTabStops, i_pTextBoxTo->pTBTabStops, i_pTextBoxTo->sTabsCount * sizeof(PDFTabStop));
    i_pTextBoxTo->pParagraph->sParaTabsCount = i_pTextBoxTo->sTabsCount;
  }

//  i_pTextBox->pTBImages = ;
//  i_pTextBox->pTBSpaces = ;
//  i_pTextBox->pParagraph = ;
//  i_pTextBox->pActiveParagraph = ;
//  i_pTextBox->pTBNext = ;
//  i_pTextBox->pTBPrev = ;
  lTop = i_pTextBoxTo->GetTopDrawPos();
  if (!PDFAdjustParagraph(i_pPDFDoc, i_pTextBoxTo, lTop, false_a))
    return false_a;
  return true_a;
}

bool_a PDFMoveTextBoxTo(PDFDocPTR i_pPDFDoc, long i_lX, long i_lY, CPDFTextBox *i_pTextBox)
{
  long lX, lY;
  PDFTBImage  *pImages = NULL;
  PDFTBSpace  *pSpace = NULL;
  if (!i_pTextBox)
    return false_a;
  lX = i_pTextBox->recTB.lLeft - i_lX;
  lY = i_pTextBox->recTB.lTop - i_lY;
  i_pTextBox->recTB.lLeft -= lX;
  i_pTextBox->recTB.lRight -= lX;
  i_pTextBox->recTB.lTop -= lY;
  i_pTextBox->recTB.lBottom -= lY;
  pImages = i_pTextBox->pTBImages;
  while (pImages)
  {
    pImages->recPos.lLeft -= lX;
    pImages->recPos.lRight -= lX;
    pImages->recPos.lTop -= lY;
    pImages->recPos.lBottom -= lY;
    pImages = pImages->pTBImageNext;
  }
  pSpace = i_pTextBox->pTBSpaces;
  while (pSpace)
  {
    pSpace->recSpacePos.lLeft -= lX;
    pSpace->recSpacePos.lRight -= lX;
    pSpace->recSpacePos.lTop -= lY;
    pSpace->recSpacePos.lBottom -= lY;
    if (i_pPDFDoc->pDocFunc)
      (*i_pPDFDoc->pDocFunc)((void *)pSpace->lSpaceID, pSpace->eSpaceType, i_pTextBox->lPage + 1,
                             pSpace->recSpacePos.lTop, pSpace->recSpacePos.lLeft, false_a, (void *)i_pTextBox);
    else
      return false_a;
    pSpace = pSpace->pTBSpaceNext;
  }
  return true_a;
}

bool_a PDFJoinTextBox(CPDFTextBox *i_pParentTB, CPDFTextBox *i_pChildTB)
{
  if (!i_pParentTB || !i_pChildTB)
    return false_a;
  if ((i_pParentTB->pTBNext) || (i_pChildTB->pTBPrev))
    return false_a;
  i_pParentTB->pTBNext = i_pChildTB;
  i_pChildTB->pTBPrev = i_pParentTB;
  return true_a;
}

CPDFTextBox *PDFUnJoinTextBox(CPDFTextBox *i_pTextBox)
{
  CPDFTextBox *pRetTB = NULL;
  if (!i_pTextBox)
    return pRetTB;
  pRetTB = i_pTextBox->pTBNext;
  i_pTextBox->pTBNext = NULL;
  return pRetTB;
}


bool_a PDFAddScaledImageToTextBox(PDFDocPTR i_pPDFDoc, char *i_pImageName, CPDFTextBox *i_pTextBox,
                              long i_lLeftPos, long i_lTopPos, long i_lScaleX, long i_lScaleY,
                              long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                              bool_a i_bAddToLine)
{
  long           lHeight       = 0;
  long           lWidth        = 0;
  long           lTBDrawWidth  = 0;
  long           lTBHeightPos  = 0;
  CPDFImage      *pImage       = NULL;
  CPDFTextBox     *pTextBox     = i_pTextBox;
  PDFLine        *pLastLine    = NULL;
  rec_a          recImage;
  rec_a          recDist;

  pImage = PDFGetDocClass(i_pPDFDoc)->CreateImage(i_pImageName);
  if (!pImage)
    return false_a;

  recDist.lBottom = i_lBottomDist;
  recDist.lLeft = i_lLeftDist;
  recDist.lRight = i_lRightDist;
  recDist.lTop = i_lTopDist;

  lHeight = PDFRound((i_lScaleY / 100.f) * ((float)pImage->height / (float)(pImage->Ydimension / DEFPOINTSPERINCH)));
  lHeight = PDFConvertDimensionFromDefaultL(lHeight);
  lHeight += recDist.lBottom + recDist.lTop;

  lWidth = PDFRound((i_lScaleX / 100.f) * ((float)pImage->width / (float)(pImage->Xdimension / DEFPOINTSPERINCH)));
  lWidth = PDFConvertDimensionFromDefaultL(lWidth);
  lWidth += recDist.lLeft + recDist.lRight;

  lTBDrawWidth = pTextBox->GetDrawWidthInd(pTextBox->pActiveParagraph);
  lTBHeightPos = pTextBox->GetTopDrawPos() - pTextBox->lTBHeight;

  if (i_bAddToLine)
  {
    if (!(pTextBox->pActiveParagraph))
    {
      pTextBox->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBox);
      pTextBox->pActiveParagraph = pTextBox->pParagraph;
    }
    pLastLine = pTextBox->pActiveParagraph->pPDFLastLine;
    if ((pLastLine->lCurrLineWidth > 0) &&
        ((lWidth + pLastLine->lCurrLineWidth) > lTBDrawWidth))
    {
      pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = PDFCreateLine(i_pPDFDoc, pTextBox->pActiveParagraph, false_a);
      pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = pTextBox->pActiveParagraph->pPDFLastLine;
      pLastLine = pTextBox->pActiveParagraph->pPDFLastLine = pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;

      pTextBox->pActiveParagraph->lParaHeight += pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
      pTextBox->lTBHeight += pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
    }

    if (!PDFAddToLineElemList(i_pPDFDoc, pLastLine, lWidth, lHeight, NULL, pImage->lIndex, &recDist, NOT_USED, pTextBox))
      return false_a;

    if (pTextBox->lTBWidth < pLastLine->lCurrLineWidth)
      pTextBox->lTBWidth = pLastLine->lCurrLineWidth;
    if (pTextBox->pActiveParagraph->lParaWidth < pLastLine->lCurrLineWidth)
      pTextBox->pActiveParagraph->lParaWidth = pLastLine->lCurrLineWidth;

    return true_a;
  }

  recImage.lLeft = pTextBox->GetLeftDrawPos() + i_lLeftPos;
  recImage.lTop = pTextBox->GetTopDrawPos() - i_lTopPos;
  recImage.lRight = recImage.lLeft + lWidth;
  recImage.lBottom = recImage.lTop - lHeight;

  PDFAddToTBImageList(i_pPDFDoc, pTextBox, pImage->lIndex, recImage, recDist);

  return true_a;
}

bool_a PDFAddImageToTextBox(PDFDocPTR i_pPDFDoc, char *i_pImageName, CPDFTextBox *i_pTextBox,
                         long i_lLeftPos, long i_lTopPos, long i_lWidth, long i_lHeight,
                         long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                         bool_a i_bAddToLine)
{
  long lHeight, lWidth, lOrigH, lOrigW;
  long lTBDrawWidth = 0;
  long lTBHeightPos = 0;
  float fR;
  CPDFImage *pImage = NULL;
  CPDFTextBox *pTextBox = i_pTextBox;
  PDFLine *pLastLine = NULL;
  rec_a recImage;
  rec_a recDist;

  pImage = PDFGetDocClass(i_pPDFDoc)->CreateImage(i_pImageName);
  if (!pImage)
    return false_a;

  recDist.lBottom = i_lBottomDist;
  recDist.lLeft = i_lLeftDist;
  recDist.lRight = i_lRightDist;
  recDist.lTop = i_lTopDist;

  lOrigH = PDFRound((pImage->height / (float)(pImage->Ydimension / DEFPOINTSPERINCH)));
  lOrigW = PDFRound((pImage->width / (float)(pImage->Xdimension / DEFPOINTSPERINCH)));
  lOrigH = PDFConvertDimensionFromDefaultL(lOrigH);
  lOrigW = PDFConvertDimensionFromDefaultL(lOrigW);
  fR = (float)lOrigW / (float)lOrigH;
  if (i_lHeight <= 0 && i_lWidth <= 0)
  {
    i_lHeight = lOrigH;
    i_lWidth = lOrigW;
  }
  if (i_lHeight > 0)
    lHeight = i_lHeight;
  else
    lHeight = PDFRound(i_lWidth / fR);
  if (i_lWidth > 0)
    lWidth = i_lWidth;
  else
    lWidth = PDFRound(i_lHeight * fR);
  lHeight += recDist.lBottom + recDist.lTop;
  lWidth += recDist.lLeft + recDist.lRight;

  lTBDrawWidth = i_pTextBox->GetDrawWidthInd(pTextBox->pActiveParagraph);
  lTBHeightPos = pTextBox->GetTopDrawPos() - pTextBox->lTBHeight;

  if (i_bAddToLine)
  {
    if (!(pTextBox->pActiveParagraph))
    {
      pTextBox->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBox);
      pTextBox->pActiveParagraph = pTextBox->pParagraph;
    }
    pLastLine = pTextBox->pActiveParagraph->pPDFLastLine;
    if ((pLastLine->lCurrLineWidth > 0) &&
        ((lWidth + pLastLine->lCurrLineWidth) > lTBDrawWidth))
    {
      pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = PDFCreateLine(i_pPDFDoc, pTextBox->pActiveParagraph, false_a);
      pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = pTextBox->pActiveParagraph->pPDFLastLine;
      pLastLine = pTextBox->pActiveParagraph->pPDFLastLine = pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;

      pTextBox->pActiveParagraph->lParaHeight += pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
      pTextBox->lTBHeight += pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
    }

    if (!PDFAddToLineElemList(i_pPDFDoc, pLastLine, lWidth, lHeight, NULL, pImage->lIndex, &recDist, NOT_USED, pTextBox))
      return false_a;

    if (pTextBox->lTBWidth < pLastLine->lCurrLineWidth)
      pTextBox->lTBWidth = pLastLine->lCurrLineWidth;
    if (pTextBox->pActiveParagraph->lParaWidth < pLastLine->lCurrLineWidth)
      pTextBox->pActiveParagraph->lParaWidth = pLastLine->lCurrLineWidth;

    return true_a;
  }

  recImage.lLeft = pTextBox->GetLeftDrawPos() + i_lLeftPos;
  recImage.lTop = i_pTextBox->GetTopDrawPos() - i_lTopPos;
  recImage.lRight = recImage.lLeft + lWidth;
  recImage.lBottom = recImage.lTop - lHeight;

  PDFAddToTBImageList(i_pPDFDoc, pTextBox, pImage->lIndex, recImage, recDist);

  return true_a;
}

bool_a PDFSetTextBoxTabs(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFTabStop *i_pTabs, short i_sTabsCount)
{
  PDFTabStop *pNewTabs = NULL;
  PDFTabStop strTabSave;
  short i = 0;
  if (!i_pTextBox || !i_pPDFDoc)
    return false_a;
  if (!i_pTabs || i_sTabsCount == 0)
  {
    free(i_pTextBox->pTBTabStops);
    i_pTextBox->pTBTabStops = NULL;
    i_pTextBox->sTabsCount = 0;
    if (i_pTextBox->pActiveParagraph)
    {
      free(i_pTextBox->pActiveParagraph->pParaTabStops);
      i_pTextBox->pActiveParagraph->pParaTabStops = NULL;
      i_pTextBox->pActiveParagraph->sParaTabsCount = 0;

      if (!PDFAdjustParagraph(i_pPDFDoc, i_pTextBox,
                                i_pTextBox->GetTopDrawPos() - i_pTextBox->lTBHeight
                                            + i_pTextBox->pActiveParagraph->lParaHeight - 1, false_a))
        return false_a;
    }
    return true_a;
  }
  pNewTabs = (PDFTabStop *)PDFMalloc(i_sTabsCount * sizeof(PDFTabStop));
  if (!pNewTabs)
    return false_a;
  for (i = 0; i < i_sTabsCount; i++)
  {
    COPY_TABS(pNewTabs[i], i_pTabs[i]);
  }
  if (i_pTextBox->pTBTabStops)
    free(i_pTextBox->pTBTabStops);
  i_pTextBox->pTBTabStops = pNewTabs;
  i_pTextBox->sTabsCount = i_sTabsCount;
  for (i = 0; i < (i_pTextBox->sTabsCount - 1); i++)
  {
    if ((i_pTextBox->pTBTabStops)[i].lTabPos > (i_pTextBox->pTBTabStops)[i + 1].lTabPos)
    {
      COPY_TABS(strTabSave, (i_pTextBox->pTBTabStops)[i]);
      COPY_TABS((i_pTextBox->pTBTabStops)[i], (i_pTextBox->pTBTabStops)[i + 1]);
      COPY_TABS((i_pTextBox->pTBTabStops)[i + 1], strTabSave);
      i = -1;
    }
  }
  if (i_pTextBox->pActiveParagraph)
  {
    free(i_pTextBox->pActiveParagraph->pParaTabStops);
    i_pTextBox->pActiveParagraph->pParaTabStops = (PDFTabStop *)PDFMalloc(i_pTextBox->sTabsCount * sizeof(PDFTabStop));
    memcpy(i_pTextBox->pActiveParagraph->pParaTabStops, i_pTextBox->pTBTabStops, i_pTextBox->sTabsCount * sizeof(PDFTabStop));
    i_pTextBox->pActiveParagraph->sParaTabsCount = i_pTextBox->sTabsCount;
    if (!PDFAdjustParagraph(i_pPDFDoc, i_pTextBox,
                                i_pTextBox->GetTopDrawPos() - i_pTextBox->lTBHeight
                                          + i_pTextBox->pActiveParagraph->lParaHeight - 1, false_a))
      return false_a;
  }
  return true_a;
}

bool_a PDFSetTabDefault(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lTabSize, char i_cTabDec)
{
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph)
    return false_a;
  i_pTextBox->lTabSize = i_lTabSize;
  i_pTextBox->cTabDecimalType = i_cTabDec;
  if (i_pTextBox->pActiveParagraph->lParaTabSize == i_lTabSize &&
      i_pTextBox->pActiveParagraph->cParaTabDecimalType == i_cTabDec)
    return true_a;
  i_pTextBox->pActiveParagraph->lParaTabSize = i_lTabSize;
  i_pTextBox->pActiveParagraph->cParaTabDecimalType = i_cTabDec;
  return PDFAdjustParagraph(i_pPDFDoc, i_pTextBox, i_pTextBox->GetTopDrawPos()
                                  - i_pTextBox->lTBHeight + i_pTextBox->pActiveParagraph->lParaHeight - 1, false_a);
}

bool_a PDFInsertTextBoxTab(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFTabStop i_Tab)
{
  short i = 0;
  if (!i_pTextBox || !i_pPDFDoc || i_Tab.lTabPos < 0)
    return false_a;
  PDFRealloc((void**)&(i_pTextBox->pTBTabStops), (i_pTextBox->sTabsCount + 1) * sizeof(PDFTabStop), sizeof(PDFTabStop));
  if (!(i_pTextBox->pTBTabStops))
    return false_a;
  for (i = 0; i < i_pTextBox->sTabsCount; i++)
  {
    if ((i_pTextBox->pTBTabStops)[i].lTabPos > i_Tab.lTabPos)
      break;
  }
  if (i < (i_pTextBox->sTabsCount))
  {
    memmove((i_pTextBox->pTBTabStops) + (i + 1), (i_pTextBox->pTBTabStops) + i,
            ((i_pTextBox->sTabsCount - i) * sizeof(PDFTabStop)));
  }
  (i_pTextBox->pTBTabStops)[i].lTabPos = i_Tab.lTabPos;
  (i_pTextBox->pTBTabStops)[i].sTabFlag = i_Tab.sTabFlag;
  i_pTextBox->sTabsCount += 1;
  if (i_pTextBox->pActiveParagraph)
  {
    free(i_pTextBox->pActiveParagraph->pParaTabStops);
    i_pTextBox->pActiveParagraph->pParaTabStops = (PDFTabStop *)PDFMalloc(i_pTextBox->sTabsCount * sizeof(PDFTabStop));
    memcpy(i_pTextBox->pActiveParagraph->pParaTabStops, i_pTextBox->pTBTabStops, i_pTextBox->sTabsCount * sizeof(PDFTabStop));
    i_pTextBox->pActiveParagraph->sParaTabsCount = i_pTextBox->sTabsCount;
    if (!PDFAdjustParagraph(i_pPDFDoc, i_pTextBox, i_pTextBox->GetTopDrawPos()
                                        - i_pTextBox->lTBHeight + i_pTextBox->pActiveParagraph->lParaHeight - 1, false_a))
      return false_a;
  }
  return true_a;
}

bool_a PDFRemoveLastTab(PDFLine *i_pLine)
{
  PDFLineElemList *pElem = NULL;
  if (!i_pLine)
    return false_a;
  pElem = i_pLine->pElemList;
  while (pElem)
  {
    if (!pElem->pLCNext)
    {
      if (pElem->lSpace < 0 && pElem->lImage < 0 && !pElem->bBlockEnd && pElem->lAnnot < 0
          && !pElem->bCanPageBreak &&  !pElem->bTBBreak && !pElem->m_strAnchor.size() && !pElem->bColumnBreak
          && (pElem->eElemPageMark == eNoPageMark) && !pElem->m_pStr && pElem->lFillBreak < 0)
      {
         pElem->pLCPrev->pLCNext = NULL;
         PDFDeleteLineElemList(&pElem);
      }
      else
         pElem->sTabStop--;
      i_pLine->sLastTab--;
      return true_a;
    }
    pElem = pElem->pLCNext;
  }
  return false_a;
}

short PDFGetNearestTabStop(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, short i_sTab)
{
  long  lLineWidth  = 0, lSpaceWidth = 0, lRightPos;
  short sRet        = NOT_USED;
  PDFTabStop strTabStop;

  if (!i_pTextBox || !i_pTextBox->pActiveParagraph)
    return sRet;

  lRightPos = i_pTextBox->GetRightDrawPosInd(i_pTextBox->pActiveParagraph, true_a);
//  if (i_pTextBox->pActiveParagraph->pParaTabStops && (i_pTextBox->pActiveParagraph->sParaTabsCount > 0))
  {
    if (!(i_pTextBox->pActiveParagraph) || !(i_pTextBox->pActiveParagraph->pPDFLastLine))
      return sRet;

    if (i_sTab >= 0)
    {
      strTabStop = PDFGetTabStop(i_pTextBox->pActiveParagraph->pParaTabStops,
                                  i_pTextBox->pActiveParagraph->sParaTabsCount, i_sTab + 1,
                                  i_pTextBox->pActiveParagraph->lParaTabSize);
      if (strTabStop.lTabPos >= 0 &&
            (lRightPos < (i_pTextBox->GetLeftDrawPos(true_a) + strTabStop.lTabPos)))
      {
        i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = PDFCreateLine(i_pPDFDoc, i_pTextBox->pActiveParagraph, false_a);
        i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = i_pTextBox->pActiveParagraph->pPDFLastLine;
        i_pTextBox->pActiveParagraph->pPDFLastLine = i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;

        i_pTextBox->pActiveParagraph->lParaHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
        i_pTextBox->lTBHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;

        //PDFAppendEmptyLineElement(i_pPDFDoc, false_a, NOT_USED, true_a, 0, i_pTextBox);
        sRet = 0;
      }
      else
      {
        lLineWidth = i_pTextBox->recIndents.lLeft
                      + PDFGetLineWidth(i_pPDFDoc, i_pTextBox, i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList, &lSpaceWidth,
                                          i_pTextBox->pActiveParagraph);
        if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
          lLineWidth += i_pTextBox->pActiveParagraph->lIndent;

        sRet = PDFGetTabStopIndex(i_pTextBox->pActiveParagraph->pParaTabStops,
                                  i_pTextBox->pActiveParagraph->sParaTabsCount, lLineWidth,
                                  i_pTextBox->pActiveParagraph->lParaTabSize);
        strTabStop = PDFGetTabStop(i_pTextBox->pActiveParagraph->pParaTabStops,
                                    i_pTextBox->pActiveParagraph->sParaTabsCount, sRet,
                                    i_pTextBox->pActiveParagraph->lParaTabSize);

        if (lRightPos < (i_pTextBox->GetLeftDrawPos(true_a) + strTabStop.lTabPos))
        {
          i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = PDFCreateLine(i_pPDFDoc, i_pTextBox->pActiveParagraph, false_a);
          i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = i_pTextBox->pActiveParagraph->pPDFLastLine;
          i_pTextBox->pActiveParagraph->pPDFLastLine = i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;

          i_pTextBox->pActiveParagraph->lParaHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
          i_pTextBox->lTBHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;

          //PDFAppendEmptyLineElement(i_pPDFDoc, false_a, NOT_USED, true_a, 0, i_pTextBox);
          sRet = 0;
        }
      }
      return sRet;
    }

    lLineWidth = i_pTextBox->recIndents.lLeft
                  + PDFGetLineWidth(i_pPDFDoc, i_pTextBox, i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList, &lSpaceWidth,
                                    i_pTextBox->pActiveParagraph);
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lLineWidth += i_pTextBox->pActiveParagraph->lIndent;

    sRet = PDFGetTabStopIndex(i_pTextBox->pActiveParagraph->pParaTabStops,
                              i_pTextBox->pActiveParagraph->sParaTabsCount, lLineWidth,
                              i_pTextBox->pActiveParagraph->lParaTabSize);
    strTabStop = PDFGetTabStop(i_pTextBox->pActiveParagraph->pParaTabStops,
                                i_pTextBox->pActiveParagraph->sParaTabsCount, sRet,
                                i_pTextBox->pActiveParagraph->lParaTabSize);

    //if (lRightPos < (i_pTextBox->GetLeftDrawPos(true_a) + strTabStop.lTabPos))
    //{
    //  i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = PDFCreateLine(i_pPDFDoc, pTextBox->pActiveParagraph, false_a);
    //  i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = i_pTextBox->pActiveParagraph->pPDFLastLine;
    //  i_pTextBox->pActiveParagraph->pPDFLastLine = i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;

    //  i_pTextBox->pActiveParagraph->lParaHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
    //  i_pTextBox->lTBHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;

    //  PDFAppendEmptyLineElement(i_pPDFDoc, false_a, NOT_USED, true_a, 0, i_pTextBox);
    //  sRet = 0;
    //}
  }
  return sRet;
}

short PDFGetTabStopIndex(PDFTabStop *i_pTabStops, short i_sTabCount, long i_lWidth, long i_lTabSize)
{
  short lIndex = 0, i = 0;
  PDFTabStop strTabStop;
  if ((i_pTabStops && i_sTabCount <= 0) || (!i_pTabStops && i_sTabCount > 0) || i_lWidth <= 0)
    return lIndex;
  for (;;)
  {
    strTabStop = PDFGetTabStop(i_pTabStops, i_sTabCount, i, i_lTabSize);
    if (i_lWidth < strTabStop.lTabPos)
    {
      lIndex = i;
      break;
    }
    i++;
  }
  return lIndex;
}

PDFTabStop PDFGetTabStop(PDFTabStop *i_pTabStops, short i_sTabCount, short i_sTab, long i_lTabSize)
{
  PDFTabStop strTabStop;
  strTabStop.lTabPos = NOT_USED;
  strTabStop.sTabFlag  = TAB_LEFT;
  if ((i_pTabStops && i_sTabCount <= 0) || (!i_pTabStops && i_sTabCount > 0) || i_sTab < 0)
    return strTabStop;
  if (i_sTab < i_sTabCount)
  {
    COPY_TABS(strTabStop, i_pTabStops[i_sTab]);
  }
  else
  {
    long lPos, i;
    if (i_sTabCount > 0)
    {
      COPY_TABS(strTabStop, i_pTabStops[i_sTabCount - 1]);
      lPos = strTabStop.lTabPos;
    }
    else
      lPos = 0;
    i = lPos / i_lTabSize;

    strTabStop.lTabPos = (i + 1 + (i_sTab - i_sTabCount)) * i_lTabSize;
    strTabStop.sTabFlag = TAB_LEFT;
  }
  return strTabStop;
}

bool_a PDFAddTabStop(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, short sTab)
{
  PDFLineElemList *pLastElem = NULL;
  long lHeight = 0;
  if (!i_pTextBox || !(i_pTextBox->pActiveParagraph) || !(i_pTextBox->pActiveParagraph->pPDFLastLine) || sTab < 0)
    return false_a;
//  pLastElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
//  if (!pLastElem)
  {
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
    if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      return false_a;
    if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
      return false_a;
    pLastElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  }
  while (pLastElem)
  {
    if (!pLastElem->pLCNext)
    {
      if (pLastElem->sTabStop >= 0)
      {
        if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
          lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
        if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
          return false_a;
      }
      else
      {
        pLastElem->sTabStop = sTab;
      }
    }
    pLastElem = pLastElem->pLCNext;
  }
  i_pTextBox->pActiveParagraph->pPDFLastLine->sLastTab = sTab;
  return true_a;
}

bool_a PDFAddPageMark(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, ePAGEMARKTYPE i_eType)
{
  PDFLineElemList *pLastElem = NULL;
  long lHeight = 0;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  //pLastElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  //if (!pLastElem)
  {
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
    if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      return false_a;
    if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
      return false_a;
    pLastElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  }
  while (pLastElem)
  {
    if (pLastElem->pLCNext)
      pLastElem = pLastElem->pLCNext;
    else
    {
      pLastElem->eElemPageMark = i_eType;
      break;
    }
  }
  i_pTextBox->bPageMark = true_a;
  return true_a;
}

long PDFAddTextToTextBox(PDFDocPTR i_pPDFDoc, CPDFString i_Text, CPDFTextBox *i_pTextBox)
{
  long swid = 0, swidlast = 0;
  long lWrite = 0;
  short sIn = 0;
  size_t Len = 0;
  long lLineWidth = 0;
  long lWidth = 0, lSpaceWidth = 0;
  long lTBHeightPos  = 0;
  CPDFTextBox *pTextBox = i_pTextBox;
  short sBlockCount = 0;
  short sCurrBlock = 0;
  short sSpaceCount = 0;
  rec_a **ppRec = NULL;
  bool_a bLong = false_a, bEmpty = false_a;
  short sTab = NOT_USED;
  PDFTabStop strTab;

  CPDFString pdfTempStr(""), pdfTempStrCut("");
  size_t IndexLast, IndexCurrent;
  CPDFChar pdfTempChar;


  if (!i_pPDFDoc || !i_pTextBox)
    return 0;

  lWidth = pTextBox->GetDrawWidthInd(pTextBox->pActiveParagraph, true_a);

  if (lWidth < 0)
    return 0;

  //pSave = currline = PDFStrDup(i_Text.GetString());
  //if (!pSave)
  //  return 0;

  if (PDFGetDocClass(i_pPDFDoc)->GetActualFontID() < 0 ||
        PDFGetDocClass(i_pPDFDoc)->GetActualFontID() >= PDFGetDocClass(i_pPDFDoc)->GetFontsCount())
    return 0;

  CPDFBaseFont *pFont = PDFGetDocClass(i_pPDFDoc)->GetFont(PDFGetDocClass(i_pPDFDoc)->GetActualFontID());
  if (!pFont)
      return 0;


lTBHeightPos = i_pTextBox->GetTopDrawPos() - i_pTextBox->lTBHeight;
if (pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth > 0)
{
  lTBHeightPos += pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
  ppRec = PDFGetTBRanges(lTBHeightPos, lTBHeightPos - pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight,
                          pTextBox, &sBlockCount);
}
else
  ppRec = PDFGetTBRanges(lTBHeightPos, lTBHeightPos - PDFGetDocClass(i_pPDFDoc)->GetActualFontHeight(),
                            pTextBox, &sBlockCount);

sCurrBlock = pTextBox->pActiveParagraph->pPDFLastLine->sLastBlock;
sTab = pTextBox->pActiveParagraph->pPDFLastLine->sLastTab;

  pdfTempStr.SetString(i_Text);

  while (!pdfTempStr.Empty())
  {
    IndexCurrent = IndexLast = 0;
    //p = plast = currline;

    while (IndexCurrent != pdfTempStr.Length())
    {
      pdfTempChar = pdfTempStr.GetPDFChar(IndexCurrent);
      if (!pdfTempChar.IsWhite() && IndexCurrent != pdfTempStr.Length())
      {
        IndexCurrent++;
        if (pdfTempStr.Length() == IndexCurrent && (sIn || (pdfTempStr.Length() == 1)))
          ;
        else
        {
          sIn = 1;
          continue;
        }
      }

      pdfTempStr.SubString(pdfTempStrCut, 0, IndexCurrent);//Offset, Count
      //czSave = *p;
      //*p = '\0';

      swid = pFont->GetFontStringWidth(pdfTempStrCut, PDFGetDocClass(i_pPDFDoc)->GetActualFontSize());

      if ((pTextBox->pActiveParagraph))
      {
        lLineWidth = PDFGetLineWidth(i_pPDFDoc, pTextBox, pTextBox->pActiveParagraph->pPDFLastLine->pElemList, &lSpaceWidth,
                                      i_pTextBox->pActiveParagraph);
        if ((pTextBox->pActiveParagraph->pPDFLastLine == pTextBox->pActiveParagraph->pPDFLine)
            &&
            ((sBlockCount == 0) || (sBlockCount > 0 && sCurrBlock == 0)))
          lLineWidth += pTextBox->pActiveParagraph->lIndent;
      }
      else
        lLineWidth = 0;

      if (ppRec)
      {
        if (sCurrBlock >= sBlockCount)
        {
          if ((ppRec[sBlockCount - 1]->lRight
                  - pTextBox->GetLeftDrawPosInd(pTextBox->pActiveParagraph, true_a)) <= 0)
          {
          }
          else
          {
            lLineWidth += ppRec[sBlockCount - 1]->lRight
                  - pTextBox->GetLeftDrawPosInd(pTextBox->pActiveParagraph, true_a);
            lLineWidth -= pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth;
            lLineWidth += PDFGetBlockWidth(pTextBox->pActiveParagraph->pPDFLastLine->pElemList, sBlockCount);
          }
        }
        else
        {
          if (sCurrBlock == 0)
          {
            if ((pTextBox->GetRightDrawPosInd(pTextBox->pActiveParagraph, true_a)
                    - ppRec[sCurrBlock]->lLeft) <= 0)
            {
            }
            else
            {
              lLineWidth += pTextBox->GetRightDrawPosInd(pTextBox->pActiveParagraph, true_a) - ppRec[sCurrBlock]->lLeft;
              lLineWidth -= pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth;
              lLineWidth += PDFGetBlockWidth(pTextBox->pActiveParagraph->pPDFLastLine->pElemList, sCurrBlock);
            }
          }
          else
          {
            lLineWidth += (pTextBox->GetRightDrawPosInd(pTextBox->pActiveParagraph, true_a) - ppRec[sCurrBlock]->lLeft);
            lLineWidth += ppRec[sCurrBlock - 1]->lRight
                - pTextBox->GetLeftDrawPosInd(pTextBox->pActiveParagraph, true_a);

            lLineWidth -= pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth;
            lLineWidth += PDFGetBlockWidth(pTextBox->pActiveParagraph->pPDFLastLine->pElemList, sCurrBlock);
          }
        }
      }

      if (sTab >= 0)
      {
        strTab = PDFGetTabStop(i_pTextBox->pActiveParagraph->pParaTabStops,
                                i_pTextBox->pActiveParagraph->sParaTabsCount, sTab,
                                i_pTextBox->pActiveParagraph->lParaTabSize);
        lLineWidth = strTab.lTabPos - pTextBox->pActiveParagraph->recIndents.lLeft;

        switch (strTab.sTabFlag)
        {
          case TAB_LEFT:
            lLineWidth += PDFGetWidthAfterTab(pTextBox->pActiveParagraph->pPDFLastLine->pElemList, sTab);
            break;
          case TAB_RIGHT:
            lLineWidth -= lLineWidth - PDFGetLineWidth(i_pPDFDoc, pTextBox, pTextBox->pActiveParagraph->pPDFLastLine->pElemList, &lSpaceWidth,
                                                        i_pTextBox->pActiveParagraph);
            if (lSpaceWidth < swid)
              lLineWidth -= lSpaceWidth;
            else
              lLineWidth -= swid;
            break;
          case TAB_CENTER:
            lLineWidth -= lLineWidth - PDFGetLineWidth(i_pPDFDoc, pTextBox, pTextBox->pActiveParagraph->pPDFLastLine->pElemList, &lSpaceWidth,
                                                        i_pTextBox->pActiveParagraph);
            if (lSpaceWidth < (swid / 2))
              lLineWidth -= lSpaceWidth;
            else
              lLineWidth -= (swid / 2);
            break;
          case TAB_DECIMAL:
            lLineWidth -= lLineWidth - PDFGetLineWidth(i_pPDFDoc, pTextBox, pTextBox->pActiveParagraph->pPDFLastLine->pElemList, &lSpaceWidth,
                                                        i_pTextBox->pActiveParagraph);
            break;
        }
      }

      if (((lLineWidth + swid) > lWidth) || (bLong && sCurrBlock > sBlockCount) )
      {
        if ((pTextBox->pActiveParagraph)
            &&  ((ppRec && (sCurrBlock <= sBlockCount)
                  && (PDFGetBlockWidth(pTextBox->pActiveParagraph->pPDFLastLine->pElemList,
                                              sCurrBlock) == 0))
                  ||
                  ((pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth == 0)
                    || ((pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth != 0)
                        && PDFGetBlockSpaces(pTextBox->pActiveParagraph->pPDFLastLine->pElemList,
                                            sCurrBlock) == 0)))
            && sSpaceCount == 0)
        {
          if (sTab > 0)
          {
            PDFRemoveLastTab(i_pTextBox->pActiveParagraph->pPDFLastLine);
            i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = PDFCreateLine(i_pPDFDoc, pTextBox->pActiveParagraph, false_a);
            i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = i_pTextBox->pActiveParagraph->pPDFLastLine;
            i_pTextBox->pActiveParagraph->pPDFLastLine = i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;

            i_pTextBox->pActiveParagraph->lParaHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
            i_pTextBox->lTBHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;

            PDFAppendEmptyLineElement(i_pPDFDoc, false_a, NOT_USED, true_a, 0, i_pTextBox);
            sTab = 0;
            continue;
          }
          else
          {
            CPDFString pdfLongStr("");
            Len = pdfTempStrCut.Length();
//            *p = czSave;
            for (;;Len--)
            {
              //czSave = currline[sLen];
              //currline[sLen] = '\0';
              pdfTempStrCut.SubString(pdfLongStr, 0, Len);
              swid = pFont->GetFontStringWidth(pdfLongStr, PDFGetDocClass(i_pPDFDoc)->GetActualFontSize());
              if (((lLineWidth + swid) <= lWidth) || swid == 0 || (1 == Len && !ppRec && 0 >= lLineWidth))
              {
                IndexLast = pdfLongStr.Length();
                //plast = currline + sLen;
                //currline[sLen] = czSave;
                swidlast = swid;
                if (ppRec &&
                    //(lWidth < (ppRec[sCurrBlock == sBlockCount ? sCurrBlock - 1 : sCurrBlock]->lRight
                    //                - ppRec[sCurrBlock == sBlockCount ? sCurrBlock - 1 : sCurrBlock]->lLeft))
                    (lWidth <= (ppRec[sCurrBlock >= sBlockCount ? sBlockCount - 1 : sCurrBlock]->lRight
                                    - ppRec[sCurrBlock >= sBlockCount ? sBlockCount - 1 : sCurrBlock]->lLeft))
                    )
                {
                  PDFAppendEmptyLine(i_pPDFDoc, pTextBox, lWidth,
                                      pTextBox->GetTopDrawPos() - pTextBox->lTBHeight
                                      - ppRec[sCurrBlock >= sBlockCount ? sBlockCount - 1 : sCurrBlock]->lBottom);
                  bEmpty = true_a;
                }
                break;
              }
              //currline[sLen] = czSave;
            }
          }
          break;
        }
        //*p = czSave;
        bLong = true_a;
        break;
      }

      sSpaceCount++;
      IndexLast = IndexCurrent;
      //plast = p;
      swidlast = swid;
      //*p = czSave;//' ';

      if (pdfTempStr.Length() == IndexCurrent || pdfTempStr.GetPDFChar(IndexCurrent).IsBrek())
//      if (czSave == '\n' || czSave == '\r' || czSave == '\0' || czSave == '\t')
      {
        break;
      }

      IndexCurrent++;
      //p++;
      sIn = 1;
    }

    if (bEmpty)
    {
      bEmpty = false_a;
      if (ppRec)
        PDFFreeTBRanges(&ppRec);
      lTBHeightPos = i_pTextBox->GetTopDrawPos() - i_pTextBox->lTBHeight;
      ppRec = PDFGetTBRanges(lTBHeightPos, lTBHeightPos - PDFGetDocClass(i_pPDFDoc)->GetActualFontHeight(), pTextBox, &sBlockCount);
      sCurrBlock = 0;
      sSpaceCount = 0;
      sIn = 0;
      continue;
    }

    //czSave = *plast;
    //*plast = '\0';
    pdfTempStr.SubString(pdfTempStrCut, 0, IndexLast);//Offset, Count

    //sLen = strlen(currline);
    Len = pdfTempStrCut.Length();

    if (pdfTempStr.Length() != IndexLast && pdfTempStr.GetPDFChar(IndexLast).IsSpace())
    //if (czSave == ' ')//(sLen && czSave == ' ') || (' ' == *i_pText && strlen(i_pText) == 1))
    {
      //*plast = czSave;
      ////*plast = '\0';
      ////swidlast = pFont->GetFontStringWidth(currline, PDFGetDocClass(i_pPDFDoc)->GetActualFontSize()); // ignore last space
      ////*plast = czSave;
      //plast++;
      IndexLast++;
      pdfTempStrCut.Append(pdfTempStr.GetPDFChar(IndexLast - 1));
      Len++;
      //czSave = *plast;
      //*plast = '\0';
      swidlast = pFont->GetFontStringWidth(pdfTempStrCut, PDFGetDocClass(i_pPDFDoc)->GetActualFontSize());
    }

    if (Len > 0 || (Len == 0 && !pdfTempStr.GetPDFChar(IndexLast).IsTab()))
    {
      if (ppRec && /*sCurrBlock > 0 && */sCurrBlock <= sBlockCount)
      {
        if (!PDFAppendLineElement(i_pPDFDoc, swidlast, pdfTempStrCut, true_a, sCurrBlock, pTextBox))
        {
          if (!lWrite)
            lWrite = (long)(pdfTempStrCut.Length() - pdfTempStr.Length());
        }
      }
      else
      {
        if (!PDFAppendLineElement(i_pPDFDoc, swidlast, pdfTempStrCut, false_a, sCurrBlock, pTextBox))
        {
          if (!lWrite)
            lWrite = (long)(pdfTempStrCut.Length() - pdfTempStr.Length());
        }
      }
    }

    if (pdfTempStr.GetPDFChar(IndexLast).IsNewLine())
      PDFAddParagraph(i_pPDFDoc, pTextBox);

    if (pdfTempStr.GetPDFChar(IndexLast).IsNewLine() || pdfTempStr.GetPDFChar(IndexLast).IsTab() || Len == 0)
    {
      if (pdfTempStr.GetPDFChar(IndexLast).IsTab())
      {
        sTab = PDFGetNearestTabStop(i_pPDFDoc, pTextBox, sTab);
        PDFAddTabStop(i_pPDFDoc, pTextBox, sTab);
      }
      else
        sTab = NOT_USED;
    }
/*
    if (sLen = strlen(currline))
    {
      currline = plast;
    }
*/

    //*plast = czSave;

    if ((Len || ((sCurrBlock <= sBlockCount) && (sBlockCount > 0))) && IndexLast != pdfTempStr.Length())
      sCurrBlock++;
    else
    {
      if ((sCurrBlock > sBlockCount && sBlockCount > 0) || sBlockCount == 0)
      {
        if (ppRec)
          PDFFreeTBRanges(&ppRec);
        lTBHeightPos = i_pTextBox->GetTopDrawPos() - i_pTextBox->lTBHeight;
        if (pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth > 0)
        {
          lTBHeightPos += pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
          ppRec = PDFGetTBRanges(lTBHeightPos,
                        lTBHeightPos - pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight, pTextBox, &sBlockCount);
        }
        else
          ppRec = PDFGetTBRanges(lTBHeightPos, lTBHeightPos - PDFGetDocClass(i_pPDFDoc)->GetActualFontHeight(), pTextBox, &sBlockCount);

        sCurrBlock = 0;
      }
      bLong = false_a;
    }

    {
      if (pdfTempStr.GetPDFChar(IndexLast).IsNewLine() || pdfTempStr.GetPDFChar(IndexLast).IsTab())
        IndexLast++;
    }

    //currline = plast;
    pdfTempStr.Erase(0, IndexLast);
    sSpaceCount = 0;
    sIn = 0;

  }

  if (ppRec)
    PDFFreeTBRanges(&ppRec);


  long lTmpTestHeight = CalcParagraphsHeights(i_pTextBox->pParagraph);
  if (lTmpTestHeight != i_pTextBox->lTBHeight)
    lTmpTestHeight = lTmpTestHeight;

  if (!lWrite)
    return (long)i_Text.Length();
  return lWrite;
}

bool_a PDFAppendEmptyLine(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lWidth, long i_lHeight)
{
  PDFLine *pLine = NULL;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  if (i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth > 0)
  {
    pLine = PDFCreateLine(i_pPDFDoc, i_pTextBox->pActiveParagraph, false_a);
    if (!pLine)
      return false_a;

    i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = pLine;
    i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = i_pTextBox->pActiveParagraph->pPDFLastLine;
  }
  else
  {
    pLine = i_pTextBox->pActiveParagraph->pPDFLastLine;
    i_pTextBox->pActiveParagraph->lParaHeight -= pLine->lCurrLineHeight;
    i_pTextBox->lTBHeight -= pLine->lCurrLineHeight;
  }
  pLine->lCurrLineWidth = i_lWidth;
  pLine->lCurrLineHeight = i_lHeight;
  pLine->lCurrLineHeight += i_pTextBox->pActiveParagraph->lLineSpace;
  i_pTextBox->pActiveParagraph->lParaHeight += pLine->lCurrLineHeight;
  i_pTextBox->lTBHeight += pLine->lCurrLineHeight;

  if (i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext)
    i_pTextBox->pActiveParagraph->pPDFLastLine = i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;

  pLine->pLineNext = PDFCreateLine(i_pPDFDoc, i_pTextBox->pActiveParagraph, false_a);
  pLine->pLineNext->pLinePrev = pLine;
  i_pTextBox->pActiveParagraph->pPDFLastLine = i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;
  return true_a;
}

bool_a PDFAppendLineElement(PDFDocPTR i_pPDFDoc, long i_lWidth, CPDFString i_String,
                            bool_a i_bBlockEnd, short sBlock, CPDFTextBox *i_pTextBox)
{
  long            lHeight = PDFGetDocClass(i_pPDFDoc)->GetActualFontHeight();
  CPDFTextBox      *pTextBox = i_pTextBox;
  PDFLine         *pLastLine = NULL;
  bool_a          bRet = true_a;
  PDFLineElemList *pLastElem;
  if (!i_pPDFDoc || !i_pTextBox)
    return false_a;
  if (!(pTextBox->pActiveParagraph))
  {
    pTextBox->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBox);
    pTextBox->pActiveParagraph = pTextBox->pParagraph;
  }
  if (i_String.Empty() && !i_bBlockEnd)
  {
    if (pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth > 0
        || pTextBox->pActiveParagraph->pPDFLastLine->sLastTab >= 0)
    {
      pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = PDFCreateLine(i_pPDFDoc, pTextBox->pActiveParagraph, false_a);
      pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = pTextBox->pActiveParagraph->pPDFLastLine;
      pTextBox->pActiveParagraph->pPDFLastLine = pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;

      pTextBox->pActiveParagraph->lParaHeight += pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
      pTextBox->lTBHeight += pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
    }
    return true_a;
  }
  pLastLine = pTextBox->pActiveParagraph->pPDFLastLine;

  if (i_bBlockEnd)
  {
    pLastElem = pLastLine->pElemList;
    if (pLastElem)
    {
      for(;;)
      {
        if (pLastElem->pLCNext)
          pLastElem = pLastElem->pLCNext;
        else
        {
          if (sBlock > pLastLine->sLastBlock)
            pLastElem->bBlockEnd = i_bBlockEnd;
          break;
        }
      }
    }
    pLastLine->sLastBlock = sBlock;
  }

  if (pLastLine->lCurrLineHeight < lHeight || 0 == pLastLine->lCurrLineWidth)
    if ((lHeight + (pTextBox->lTBHeight - pLastLine->lCurrLineHeight))
                        > pTextBox->GetDrawHeight(true_a))
      bRet = false_a; //textbox overflow
  if (!PDFAddToLineElemList(i_pPDFDoc, pLastLine, i_lWidth, lHeight, &i_String, NOT_USED, NULL, NOT_USED, pTextBox))
    return false_a;
  if (pTextBox->lTBWidth < pLastLine->lCurrLineWidth)
    pTextBox->lTBWidth = pLastLine->lCurrLineWidth;
  if (pTextBox->pActiveParagraph->lParaWidth < pLastLine->lCurrLineWidth)
    pTextBox->pActiveParagraph->lParaWidth = pLastLine->lCurrLineWidth;
  return bRet;
}

bool_a PDFAppendEmptyLineElement(PDFDocPTR i_pPDFDoc, bool_a i_bBlockEnd, short i_sBlock,
                                 bool_a i_bTabStop, short i_sTab, CPDFTextBox *i_pTextBox)
{
  long            lHeight = PDFGetDocClass(i_pPDFDoc)->GetActualFontHeight();
  CPDFTextBox      *pTextBox = i_pTextBox;
  PDFLine         *pLastLine = NULL;
  bool_a          bRet = true_a;
  PDFLineElemList *pLastElem;
  if (!i_pPDFDoc || !pTextBox || i_sTab < 0)
    return false_a;
  if (i_bBlockEnd && i_bTabStop)
    return false_a;
  if (!(pTextBox->pActiveParagraph))
  {
    pTextBox->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBox);
    pTextBox->pActiveParagraph = pTextBox->pParagraph;
  }
  pLastLine = pTextBox->pActiveParagraph->pPDFLastLine;
  if (pLastLine->lCurrLineHeight < lHeight || 0 == pLastLine->lCurrLineWidth)
    if ((lHeight + (pTextBox->lTBHeight - pLastLine->lCurrLineHeight))
                        > pTextBox->GetDrawHeight(true_a))
      bRet = false_a;//textbox overflow
  if (!PDFAddToLineElemList(i_pPDFDoc, pLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, pTextBox))
    return false_a;
  if (pTextBox->lTBWidth < pLastLine->lCurrLineWidth)
    pTextBox->lTBWidth = pLastLine->lCurrLineWidth;
  if (pTextBox->pActiveParagraph->lParaWidth < pLastLine->lCurrLineWidth)
    pTextBox->pActiveParagraph->lParaWidth = pLastLine->lCurrLineWidth;
  pLastElem = pLastLine->pElemList;
  if (pLastElem)
  {
    for(;;)
    {
      if (pLastElem->pLCNext)
        pLastElem = pLastElem->pLCNext;
      else
      {
        if (i_bBlockEnd)
        {
          if (i_sBlock > pLastLine->sLastBlock)
            pLastElem->bBlockEnd = i_bBlockEnd;
        }
        if (i_bTabStop)
        {
          pLastElem->sTabStop = i_sTab;
        }
        break;
      }
    }
  }
  if (i_bBlockEnd)
    pLastLine->sLastBlock = i_sBlock;
  if (i_bTabStop)
    pLastLine->sLastTab = i_sTab;

  return bRet;
}

bool_a PDFAddLink(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lIndex)
{
  long lHeight = 0;
  PDFLineElemList *pElem = NULL;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  if (i_lIndex < 0)
    return false_a;
  if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
  {
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
    if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      return false_a;
  }
  pElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  while (pElem)
  {
    if (!pElem->pLCNext)
    {
      if (pElem->lAnnot >= 0)
      {
        if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
          lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
        if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
          return false_a;
      }
      else
      {
        pElem->lAnnot = i_lIndex;
      }
    }
    pElem = pElem->pLCNext;
  }
  return true_a;
}

bool_a PDFAddFillBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lBreakCount)
{
  long lHeight = 0;
  PDFLineElemList *pElem = NULL;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  if (i_lBreakCount < 0)
    return false_a;
  i_pTextBox->bTBBreak = true_a;
  if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
  {
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
    if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      return false_a;
  }
  pElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  while (pElem)
  {
    if (!pElem->pLCNext)
    {
      //if (pElem->lFillBreak >= 0)
      //{
      //  if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      //    lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
      //  if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      //    return false_a;
      //}
      //else
      //{
        pElem->lFillBreak = i_lBreakCount;
      //}
    }
    pElem = pElem->pLCNext;
  }
  return true_a;
}

bool_a PDFAddAnchor(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, string i_strAnchor)
{
  long lHeight = 0;
  PDFLineElemList *pElem = NULL;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
  {
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
    if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      return false_a;
  }
  pElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  while (pElem)
  {
    if (!pElem->pLCNext)
    {
      if (pElem->m_strAnchor.size() > 0 || pElem->bCanPageBreak || pElem->bTBBreak || pElem->bColumnBreak)
      {
        if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
          lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
        if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
          return false_a;
      }
      else
      {
        pElem->m_strAnchor.assign(i_strAnchor);
      }
    }
    pElem = pElem->pLCNext;
  }
  return true_a;
}

bool_a PDFHasLineBreak(PDFLine *i_pLine)
{
  PDFLineElemList *pElem = NULL;
  if (!i_pLine)
    return false_a;
  pElem = i_pLine->pElemList;
  while (pElem)
  {
    if (pElem->bTBBreak)
      return true_a;
    pElem = pElem->pLCNext;
  }
  return false_a;
}

bool_a PDFHasLineColumnBreak(PDFLine *i_pLine)
{
  PDFLineElemList *pElem = NULL;
  if (!i_pLine)
    return false_a;
  pElem = i_pLine->pElemList;
  while (pElem)
  {
    if (pElem->bColumnBreak)
      return true_a;
    pElem = pElem->pLCNext;
  }
  return false_a;
}

long PDFGetFirstFillBreak(PDFLine *i_pLine)
{
  PDFLineElemList *pElem = NULL;
  if (!i_pLine)
    return NOT_USED;
  pElem = i_pLine->pElemList;
  while (pElem)
  {
    if (pElem->lFillBreak >= 0)
      return pElem->lFillBreak;
    pElem = pElem->pLCNext;
  }
  return NOT_USED;
}

bool_a PDFReplaceFirstFillBreak(PDFLine *i_pLine, long i_lCount, CPDFTextBox *i_pTextBox)
{
  PDFLineElemList *pElem = NULL, *pElemNew = NULL;
  long i;
  if (i_lCount <= 0)
    return true_a;
  if (!i_pLine || !i_pTextBox)
    return false_a;
  pElem = i_pLine->pElemList;
  while (pElem)
  {
    if (pElem->lFillBreak >= 0)
    {
      pElem->lFillBreak = NOT_USED;
      for (i = 0; i < i_lCount; i++)
      {
        pElemNew = PDFCreateCopyElemList(pElem);
        if (!pElemNew)
          return false_a;
        pElemNew->bTBBreak = true_a;
        pElemNew->pLCNext = pElem->pLCNext;
        pElemNew->pLCPrev = pElem;
        if (pElem->pLCNext)
        {
          pElem->pLCNext->pLCPrev = pElemNew;
          pElem->pLCNext = pElemNew;
        }
        pElem = pElemNew;
      }
      i_pTextBox->bTBBreak = true_a;
      return true_a;
    }
    pElem = pElem->pLCNext;
  }
  return false_a;
}

bool_a PDFAddBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox)
{
  long lHeight = 0;
  PDFLineElemList *pElem = NULL;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  i_pTextBox->bTBBreak = true_a;
//  if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
  {
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
    if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      return false_a;
  }
  pElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  while (pElem)
  {
    if (!pElem->pLCNext)
    {
//      if (pElem->bTBBreak)
//      {
//        if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
//          lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
//        if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
//          return false_a;
//      }
//      else
        pElem->bTBBreak = true_a;
    }
    pElem = pElem->pLCNext;
  }
  return true_a;
}

bool_a PDFAddColumnBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox)
{
  long lHeight = 0;
  PDFLineElemList *pElem = NULL;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  i_pTextBox->bColumnBreak = true_a;
//  if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
  {
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
    if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      return false_a;
  }
  pElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  while (pElem)
  {
    if (!pElem->pLCNext)
    {
        pElem->bColumnBreak = true_a;
    }
    pElem = pElem->pLCNext;
  }
  return true_a;
}

bool_a PDFAddCanPageBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox)
{
  long lHeight = 0;
  PDFLineElemList *pElem = NULL;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  i_pTextBox->bTBCanPageBreak = true_a;
//  if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
  {
    if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
      lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
    if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
      return false_a;
  }
  pElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
  while (pElem)
  {
    if (!pElem->pLCNext)
      pElem->bCanPageBreak = true_a;
    pElem = pElem->pLCNext;
  }
  return true_a;
}

PDFLineElemList *PDFGetCanPageBreakElem(PDFLine *i_pLine, CPDFTextBox *i_pTextBox)
{
  PDFLineElemList *pElem = NULL, *pRetElem = NULL;
  PDFLine *pLine = NULL;
  PDFParagraph *pParagraph = NULL;
  bool_a bF = false_a;
  if (!i_pTextBox || !(i_pTextBox->bTBCanPageBreak))
    return NULL;
  pParagraph = i_pTextBox->pActiveParagraph;
  while (pParagraph)
  {
    pLine = pParagraph->pPDFLastLine;
    while (pLine)
    {
      pElem = pLine->pElemList;
      while (bF && pElem)
      {
        if (pElem->bCanPageBreak)
          pRetElem = pElem;
        pElem = pElem->pLCNext;
      }
      if (pRetElem)
         return pRetElem;
      if (!bF && (pLine == i_pLine || !i_pLine))
        bF = true_a;
      pLine = pLine->pLinePrev;
    }
    pParagraph = pParagraph->pParaPrev;
  }
  return pRetElem;
}

void PDFCanPageBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, bool_a i_bDelete)
{
  PDFLineElemList *pElem = NULL;
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return;
  if (i_bDelete)
  {
    if (i_pTextBox->bTBCanPageBreak)
    {
      pElem = PDFGetCanPageBreakElem(NULL, i_pTextBox);
      if (pElem)
        pElem->bCanPageBreak = false_a;
    }
    i_pTextBox->bTBCanPageBreak = false_a;
  }
  else
  {
   PDFAddCanPageBreak(i_pPDFDoc, i_pTextBox);
/*
    if (i_pTextBox->bTBCanPageBreak)
    {
      pElem = PDFGetCanPageBreakElem(i_pTextBox);
      if (pElem)
        pElem->bCanPageBreak = false_a;
    }
    i_pTextBox->bTBCanPageBreak = true_a;
    if (!i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList)
    {
      if (i_pTextBox->pActiveParagraph->pPDFLastLine == i_pTextBox->pActiveParagraph->pPDFLine)
        lHeight = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight - i_pTextBox->pActiveParagraph->lLineSpace;
      if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, 0, lHeight, NULL, NOT_USED, NULL, NOT_USED, i_pTextBox))
        return;
    }
    pElem = i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList;
    while (pElem)
    {
      if (!pElem->pLCNext)
        pElem->bCanPageBreak = true_a;
      pElem = pElem->pLCNext;
    }
*/
  }
}

bool_a PDFInsertBreak(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBoxFrom, CPDFTextBox *i_pTextBoxTo, PDFLineElemList *i_pElem)
{
  PDFParagraph    *pParagraph = NULL, *pOldParagraph = NULL;
  PDFLine         *pLine = NULL, *pOldLine = NULL;
  PDFLineElemList *pElem = NULL, *pOldElem = NULL;
  bool_a          bFound = false_a, bSameElem = false_a, bSamePara = false_a;
  bool_a          bSameLine = false_a, bBreak;
  CPDFTextBox strTextBoxSave;

  if (!i_pPDFDoc || !i_pTextBoxFrom || !i_pTextBoxTo)
    return false_a;
  if (!i_pTextBoxFrom->bTBCanPageBreak && !i_pTextBoxFrom->bTBBreak && !i_pTextBoxFrom->bColumnBreak)
  {
    i_pTextBoxFrom->bTBBreak = true_a;
    return true_a;
  }
  if (!i_pTextBoxTo)
    return false_a;
  if (!PDFSetTextBoxTabs(i_pPDFDoc, i_pTextBoxTo, i_pTextBoxFrom->pTBTabStops, i_pTextBoxFrom->sTabsCount))
    return false_a;
  //memset(&strTextBoxSave, 0, sizeof(PDFTextBox));
  COPY_TB((&strTextBoxSave), i_pTextBoxTo);
  i_pTextBoxTo->bTBCopy = true_a;
  pParagraph = i_pTextBoxFrom->pParagraph;
  while (pParagraph)
  {
    pLine = pParagraph->pPDFLine;
    if (!bFound)
    {
      while (pLine)
      {
        pElem = pLine->pElemList;
        while (pElem)
        {
           if (i_pElem)
             bBreak = i_pElem == pElem ? pElem->bCanPageBreak : false_a;
           else
           {
             bBreak = pElem->bTBBreak ? pElem->bTBBreak : pElem->bColumnBreak;
           }
          if (bBreak/*bCanPageBreak*/)
          {
            if (!PDFCopyParagraphAttributtesToTextBox(i_pTextBoxTo, pParagraph))
              return false_a;
            if (!bFound)
              bSameElem = true_a;
            bFound = true_a;
            bSamePara = true_a;
            bSameLine = true_a;
            i_pTextBoxFrom->pActiveParagraph = pParagraph;
            i_pTextBoxFrom->pActiveParagraph->pPDFLastLine = pLine;
//            i_pTextBoxFrom->pActiveParagraph->pPDFLastLine->pLineNext = NULL;
          }
          pOldElem = pElem;
          pElem = pElem->pLCNext;
          if (bFound && !bSameElem/*!pOldElem->bTBBreak/*bCanPageBreak*/)
          {
            if (!PDFCopyLineElement(i_pPDFDoc, i_pTextBoxTo, pOldElem, i_pTextBoxFrom, false_a))
              return false_a;
            if (pOldElem->pLCPrev)
              pOldElem->pLCPrev->pLCNext = NULL;
            if (pOldElem->pLCNext)
              pOldElem->pLCNext->pLCPrev = NULL;
            delete pOldElem;
            pOldElem = NULL;
          }
          bSameElem = false_a;
        }
        pOldLine = pLine;
        pLine = pLine->pLineNext;
        if (pLine && i_pTextBoxFrom->pActiveParagraph->pPDFLastLine == pLine->pLinePrev)
          i_pTextBoxFrom->pActiveParagraph->pPDFLastLine->pLineNext = NULL;
        if (!bSameLine && bFound)
        {
          if (pOldLine->bNewLine)
          {
            if (!PDFInsertLine(i_pPDFDoc, i_pTextBoxTo))
               return false_a;
          }
          if (pOldLine->pLinePrev)
            pOldLine->pLinePrev->pLineNext = NULL;
          if (pOldLine->pLineNext)
            pOldLine->pLineNext->pLinePrev = NULL;
          pParagraph->lParaHeight -= pOldLine->lCurrLineHeight;
          i_pTextBoxFrom->lTBHeight -= pOldLine->lCurrLineHeight;
          free(pOldLine);
          pOldLine = NULL;
        }
        bSameLine = false_a;
      }
    }
    else
    {
      if (!PDFCopyParagraph(i_pPDFDoc, i_pTextBoxTo, pParagraph, i_pTextBoxFrom, false_a, true_a))
        return false_a;
      i_pTextBoxTo->pTBTabStops = strTextBoxSave.pTBTabStops;
      i_pTextBoxTo->sTabsCount = strTextBoxSave.sTabsCount;
    }
    pOldParagraph = pParagraph;
    pParagraph = pParagraph->pParaNext;
    if (!bSamePara && bFound)
    {
      if (pOldParagraph->pParaPrev)
        pOldParagraph->pParaPrev->pParaNext = NULL;
      if (pOldParagraph->pParaNext)
        pOldParagraph->pParaNext->pParaPrev = NULL;
      i_pTextBoxFrom->lTBHeight -= pOldParagraph->lParaHeight;
      PDFFreeParagraph(&pOldParagraph);
    }
    bSamePara = false_a;
  }
  COPY_TB(i_pTextBoxTo, (&strTextBoxSave));
  i_pTextBoxTo->bTBCopy = false_a;
  i_pTextBoxFrom->bTBBreak = true_a;

  if (!PDFAdjustParagraph(i_pPDFDoc, i_pTextBoxFrom, i_pTextBoxFrom->GetTopDrawPos(), false_a))
    return false_a;

  return true_a;
}

PDFParagraph *PDFGetFirstParaWithNext(PDFParagraph *i_pPara)
{
  if (!i_pPara)
    return NULL;
  PDFParagraph *pPara = i_pPara;
  while (pPara->pParaPrev && pPara->pParaPrev->m_bKeepWithNext)
    pPara = pPara->pParaPrev;
  if (pPara != i_pPara)
    return pPara;
  return NULL;
}

bool_a PDFIsLineEmpty(PDFLine *i_pLine)
{
  if (!i_pLine)
    return false_a;
  if (!(i_pLine->pElemList))
    return true_a;
  if (0 == i_pLine->lCurrLineWidth)
    return true_a;
  return false_a;
}

PDFLine *PDFGetLastUsedLine(PDFParagraph *i_pPara)
{
  if (!i_pPara)
    return NULL;
  PDFLine *pLine = i_pPara->pPDFLastLine;
  while (pLine)
  {
    if (!PDFIsLineEmpty(pLine))
      return pLine;

    pLine = pLine->pLinePrev;
  }
  return NULL;
}

bool_a PDFCopyOverflowLines(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBoxFrom, CPDFTextBox *i_pTextBoxTo,
                           bool_a i_bCheckBreak, long i_lHeight)
{
  PDFParagraph    *pParagraph = NULL, *pOldParagraph = NULL;
  PDFLine         *pLine = NULL, *pOldLine = NULL;
  PDFLineElemList *pElem = NULL, *pOldElem = NULL;
  bool_a          bFound = false_a, bSamePara = false_a, bSameLine = false_a;
  bool_a          bFPara = false_a, bNewPara = true_a, bNotEmpty = false_a;
  long            lHeight = 0, lMaxHeight, lFillBreakCount;
  CPDFTextBox strTextBoxSave;
  PDFTBSpace *pTBSpace = NULL;

  if (!i_pPDFDoc || !i_pTextBoxFrom)
    return false_a;
  lMaxHeight = i_lHeight == NOT_USED ? i_pTextBoxFrom->GetDrawHeight(true_a) : i_lHeight;
  if (lMaxHeight >= i_pTextBoxFrom->lTBHeight && !i_pTextBoxFrom->HasBreak()
        && !i_pTextBoxFrom->bColumnBreak)
    return true_a;
  if (!i_pTextBoxTo)
    return false_a;
  if ((i_pTextBoxTo->pActiveParagraph != i_pTextBoxTo->pParagraph) || !PDFIsParagraphEmpty(i_pTextBoxTo->pActiveParagraph))
    bNotEmpty = true_a;
  if (bNotEmpty)
  {
    strTextBoxSave.TBCpy(*i_pTextBoxTo);
//    COPY_TB((&strTextBoxSave), i_pTextBoxTo);
    if (!PDFClearTextBox(i_pPDFDoc, i_pTextBoxTo))
      return false_a;
  }
  else
  {
    if (!PDFSetTextBoxTabs(i_pPDFDoc, i_pTextBoxTo, i_pTextBoxFrom->pTBTabStops, i_pTextBoxFrom->sTabsCount))
      return false_a;
    strTextBoxSave.TBCpy(*i_pTextBoxTo);
//    COPY_TB((&strTextBoxSave), i_pTextBoxTo);
  }
  i_pTextBoxTo->bTBCopy = true_a;
  pParagraph = i_pTextBoxFrom->pParagraph;
  while (pParagraph)
  {
    lHeight += pParagraph->recIndents.lTop;
    //paragraph must be first on the page in case of m_bParagraphPageBreak set to true_a
    if (pParagraph->m_bParagraphPageBreak
        && i_pTextBoxFrom->pParagraph != pParagraph)
    {
      if (pParagraph->pParaPrev)
        i_pTextBoxFrom->pActiveParagraph = pParagraph->pParaPrev;
      else
      {
        i_pTextBoxFrom->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBoxFrom);
        i_pTextBoxFrom->pActiveParagraph = i_pTextBoxFrom->pParagraph;
      }
      i_pTextBoxFrom->pActiveParagraph->pParaNext = NULL;
      //i_pTextBoxFrom->lTBHeight -= pParagraph->lParaHeight;
      bFound = true_a;
      bNewPara = false_a;
    }

    pLine = pParagraph->pPDFLine;
    if (!bFound)
    {
      while (pLine)
      {
        pTBSpace = NULL;
        lHeight += pLine->lCurrLineHeight;
        if (lMaxHeight < lHeight && !bFound && !bSamePara && !bSameLine)
        {
          PDFLineElemList *pTempElem = NULL;

          //keep paragraph together and check prev. paragraph
          if (pParagraph->m_bKeepLinesTogether
            && i_pTextBoxFrom->pParagraph != pParagraph)//first paragraph - can run in circles!!!
          {
            PDFParagraph *pParaKeepFirst = PDFGetFirstParaWithNext(pParagraph);

            if (pParaKeepFirst && pParaKeepFirst != i_pTextBoxFrom->pParagraph)//first paragraph - can run in circles!!!
              pParagraph = pParaKeepFirst;

            if (pParagraph->pParaPrev)
              i_pTextBoxFrom->pActiveParagraph = pParagraph->pParaPrev;
            else
            {
              i_pTextBoxFrom->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBoxFrom);
              i_pTextBoxFrom->pActiveParagraph = i_pTextBoxFrom->pParagraph;
            }
            i_pTextBoxFrom->pActiveParagraph->pParaNext = NULL;
            //i_pTextBoxFrom->lTBHeight -= pParagraph->lParaHeight;
            bFound = true_a;
            bSamePara = true_a;
            bSameLine = true_a;
            bFPara = true_a;
            bNewPara = false_a;
            break;
          }
          //first line in paragraph - check prev. paragraph
          if (pParagraph->pPDFLine == pLine)
          {
            PDFParagraph *pParaKeepFirst = PDFGetFirstParaWithNext(pParagraph);

            if (pParaKeepFirst && pParaKeepFirst != i_pTextBoxFrom->pParagraph)//first paragraph - can run in circles!!!
            {
              pParagraph = pParaKeepFirst;

              if (pParagraph->pParaPrev)
                i_pTextBoxFrom->pActiveParagraph = pParagraph->pParaPrev;
              else
              {
                i_pTextBoxFrom->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBoxFrom);
                i_pTextBoxFrom->pActiveParagraph = i_pTextBoxFrom->pParagraph;
              }
              i_pTextBoxFrom->pActiveParagraph->pParaNext = NULL;
              //i_pTextBoxFrom->lTBHeight -= pParagraph->lParaHeight;
              bFound = true_a;
              bSamePara = true_a;
              bSameLine = true_a;
              bFPara = true_a;
              bNewPara = false_a;
              break;
            }
          }
          //Widow\Orphan
          if (pParagraph->m_bWidowOrphan)
          {
            if (pParagraph->pPDFLine == pLine->pLinePrev)
            {
              PDFParagraph *pParaKeepFirst = PDFGetFirstParaWithNext(pParagraph);
              if (pParaKeepFirst)
                pParagraph = pParaKeepFirst;

              if (pParagraph->pParaPrev)
                i_pTextBoxFrom->pActiveParagraph = pParagraph->pParaPrev;
              else
              {
                i_pTextBoxFrom->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBoxFrom);
                i_pTextBoxFrom->pActiveParagraph = i_pTextBoxFrom->pParagraph;
              }
              i_pTextBoxFrom->pActiveParagraph->pParaNext = NULL;
              //i_pTextBoxFrom->lTBHeight -= pParagraph->lParaHeight;
              bFound = true_a;
              bSamePara = true_a;
              bSameLine = true_a;
              bFPara = true_a;
              bNewPara = false_a;
              break;
            } else
            {
              PDFLine *pLastUsedLine = PDFGetLastUsedLine(pParagraph);
              if (pLastUsedLine == pLine && pLastUsedLine->pLinePrev)
                pLine = pLastUsedLine->pLinePrev;
            }
          }



          pTempElem = PDFGetCanPageBreakElem(pLine, i_pTextBoxFrom);
          if (pTempElem)
          {
            i_pTextBoxTo->TBCpy(strTextBoxSave);
            if (!i_bCheckBreak)
              return PDFInsertBreak(i_pPDFDoc, i_pTextBoxFrom, i_pTextBoxTo, pTempElem);
            else
              return false_a;
          }

          pTempElem = PDFGetFixTableElem(i_pTextBoxFrom, pLine);


          if (!PDFCopyParagraphAttributtesToTextBox(i_pTextBoxTo, pParagraph))
            return false_a;
          bFound = true_a;
          bSamePara = true_a;
          bSameLine = true_a;

          if (pTempElem/*pLine->pElemList && pLine->pElemList->lSpace > 0*/)
          {
            pTBSpace = PDFGetTBSpace(i_pTextBoxFrom, pTempElem->lSpace);
            if (!pTBSpace)
              return false_a;
          }
          if (pTBSpace && pTBSpace->eSpaceType == eFixTableWidth)
          {
            long lDivHeight = 0;
            rec_a recPosSize;
            if (i_pPDFDoc->pDocFunc)
              lDivHeight = (*i_pPDFDoc->pDocFunc)((void *)pTBSpace->lSpaceID, pTBSpace->eSpaceType, i_pTextBoxFrom->lPage + 1,
                                                  pTBSpace->recSpacePos.lTop, pTBSpace->recSpacePos.lLeft, true_a, (void *)i_pTextBoxFrom);
            else
              return false_a;

            if (lDivHeight > 0)
            {
              if (!strTextBoxSave.pTBSpaces)
                pTBSpace->recSpacePos.lBottom += lDivHeight;

              i_pTextBoxFrom->pActiveParagraph = pParagraph;
              pParagraph = pParagraph->pParaNext;
              i_pTextBoxFrom->pActiveParagraph->pParaNext = NULL;

              i_pTextBoxFrom->lTBHeight -= i_pTextBoxFrom->pActiveParagraph->lParaHeight
                                            - (pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom);
              i_pTextBoxFrom->pActiveParagraph->lParaHeight = (pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom);
              pLine->lCurrLineHeight = i_pTextBoxFrom->pActiveParagraph->lParaHeight;


              COPY_REC(recPosSize, pTBSpace->recSpacePos);
              recPosSize.lTop = i_pTextBoxTo->GetTopDrawPos();// - i_pTextBoxTo->lTBHeight;
              recPosSize.lBottom = recPosSize.lTop - lDivHeight;

              if (!PDFAddToTBSpaceList(i_pPDFDoc, i_pTextBoxTo, recPosSize, pTBSpace->lSpaceID, pTBSpace->eSpaceType))
                return false_a;

              bFPara = true_a;
              bNewPara = true_a;//false_a;
              break;
            }
            else if ((i_pTextBoxFrom->pParagraph == pParagraph) && (pParagraph->pPDFLine == pLine))
            {
              i_pTextBoxFrom->pActiveParagraph = pParagraph;
              pLine = pLine->pLineNext;
              continue;
            }
          }
          else if ((i_pTextBoxFrom->pParagraph == pParagraph) && (pParagraph->pPDFLine == pLine))
          {
            i_pTextBoxFrom->pActiveParagraph = pParagraph;
            pLine = pLine->pLineNext;
            continue;
          }
          if (pLine->pLinePrev)
          {
            i_pTextBoxFrom->pActiveParagraph = pParagraph;
            i_pTextBoxFrom->pActiveParagraph->pPDFLastLine = pLine->pLinePrev;
//            i_pTextBoxFrom->pActiveParagraph->pPDFLastLine->pLineNext = NULL;
          }
          else
          {
            if (pParagraph->pParaPrev)
              i_pTextBoxFrom->pActiveParagraph = pParagraph->pParaPrev;
            else
            {
              i_pTextBoxFrom->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBoxFrom);
              i_pTextBoxFrom->pActiveParagraph = i_pTextBoxFrom->pParagraph;
            }

            i_pTextBoxFrom->pActiveParagraph->pParaNext = NULL;
            //i_pTextBoxFrom->lTBHeight -= pParagraph->lParaHeight;
            bFPara = true_a;
            bNewPara = false_a;
            break;
          }
        }
        if (bFound)
        {
          pElem = pLine->pElemList;
          while (pElem)
          {
            pOldElem = pElem;
            pElem = pElem->pLCNext;
            if (!PDFCopyLineElement(i_pPDFDoc, i_pTextBoxTo, pOldElem, i_pTextBoxFrom, false_a))
              return false_a;
            if (pOldElem->pLCPrev)
              pOldElem->pLCPrev->pLCNext = NULL;
            if (pOldElem->pLCNext)
              pOldElem->pLCNext->pLCPrev = NULL;
            delete pOldElem;
            pOldElem = NULL;
          }
          if (pLine->bNewLine)
          {
            if (!PDFInsertLine(i_pPDFDoc, i_pTextBoxTo))
               return false_a;
          }
        }
        if (!bFound)
        {
          lFillBreakCount = PDFGetFirstFillBreak(pLine);
          if (lFillBreakCount > 0)
          {
            if (!PDFReplaceFirstFillBreak(pLine, lFillBreakCount - (PDFGetDocClass(i_pPDFDoc)->GetPageCount() - 1), i_pTextBoxFrom))
              return false_a;
            i_pTextBoxTo->TBCpy(strTextBoxSave);
            return PDFInsertBreak(i_pPDFDoc, i_pTextBoxFrom, i_pTextBoxTo, NULL);
          }
          if (PDFHasLineBreak(pLine))
          {
            i_pTextBoxTo->TBCpy(strTextBoxSave);
            if (!i_bCheckBreak)
              return PDFInsertBreak(i_pPDFDoc, i_pTextBoxFrom, i_pTextBoxTo, NULL);
            else
              return false_a;
          }
          if (PDFHasLineColumnBreak(pLine))
          {
            i_pTextBoxTo->TBCpy(strTextBoxSave);
            return PDFInsertBreak(i_pPDFDoc, i_pTextBoxFrom, i_pTextBoxTo, NULL);
          }
        }
        pOldLine = pLine;
        pLine = pLine->pLineNext;
        if (pLine && i_pTextBoxFrom->pActiveParagraph->pPDFLastLine == pLine->pLinePrev)
          i_pTextBoxFrom->pActiveParagraph->pPDFLastLine->pLineNext = NULL;
        if (bFound)
        {
          if (pOldLine->pLinePrev)
            pOldLine->pLinePrev->pLineNext = NULL;
          if (pOldLine->pLineNext)
            pOldLine->pLineNext->pLinePrev = NULL;
          pParagraph->lParaHeight -= pOldLine->lCurrLineHeight;
          i_pTextBoxFrom->lTBHeight -= pOldLine->lCurrLineHeight;
          free(pOldLine);
          pOldLine = NULL;
        }
        bSameLine = false_a;
      }
    }
    else
    {
      if (!PDFCopyParagraph(i_pPDFDoc, i_pTextBoxTo, pParagraph, i_pTextBoxFrom, false_a, bNewPara))
        return false_a;
      bNewPara = true_a;
      i_pTextBoxTo->pTBTabStops = strTextBoxSave.pTBTabStops;
      i_pTextBoxTo->sTabsCount = strTextBoxSave.sTabsCount;
    }
    if (bFPara)
    {
      bFPara = false_a;
      bSamePara = false_a;
      continue;
    }

    lHeight += pParagraph->recIndents.lBottom;

    pOldParagraph = pParagraph;
    pParagraph = pParagraph->pParaNext;
    if (!bSamePara && bFound)
    {
      if (pOldParagraph->pParaPrev)
        pOldParagraph->pParaPrev->pParaNext = NULL;
      if (pOldParagraph->pParaNext)
        pOldParagraph->pParaNext->pParaPrev = NULL;
      i_pTextBoxFrom->lTBHeight -= pOldParagraph->lParaHeight;
      PDFFreeParagraph(&pOldParagraph);
    }
    bSamePara = false_a;
  }

  if (bNotEmpty)
  {
    pParagraph = strTextBoxSave.pParagraph;
    while (pParagraph)
    {
      if (pParagraph != strTextBoxSave.pParagraph || (pParagraph == strTextBoxSave.pParagraph && !PDFHasParagraphFixTable(&strTextBoxSave, pParagraph)))
      {
        if (!PDFCopyParagraph(i_pPDFDoc, i_pTextBoxTo, pParagraph, &strTextBoxSave, false_a, !(pParagraph->bParaContinue)))
          return false_a;
        i_pTextBoxTo->pTBTabStops = strTextBoxSave.pTBTabStops;
        i_pTextBoxTo->sTabsCount = strTextBoxSave.sTabsCount;
      }
      pOldParagraph = pParagraph;
      pParagraph = pParagraph->pParaNext;
      if (pOldParagraph->pParaPrev)
        pOldParagraph->pParaPrev->pParaNext = NULL;
      if (pOldParagraph->pParaNext)
        pOldParagraph->pParaNext->pParaPrev = NULL;
      PDFFreeParagraph(&pOldParagraph);
    }
  }

  COPY_TB(i_pTextBoxTo, (&strTextBoxSave));
  i_pTextBoxFrom->pActiveParagraph->pParaNext = NULL;
  i_pTextBoxTo->bTBCopy = false_a;

  if (!PDFAdjustParagraph(i_pPDFDoc, i_pTextBoxFrom, i_pTextBoxFrom->GetTopDrawPos(), false_a))
    return false_a;

  return true_a;
}

PDFLineElemList *PDFCreateCopyElemList(PDFLineElemList *i_pElemList)
{
  PDFLineElemList *pElemNew = NULL;
  if (!i_pElemList)
    return NULL;
  pElemNew = new PDFLineElemList;
  if (!pElemNew)
    return NULL;
  *pElemNew = *i_pElemList;
  //memcpy(pElemNew, i_pElemList, sizeof(PDFLineElemList));
  pElemNew->lElemWidth = 0;
  pElemNew->lImage = NOT_USED;
  pElemNew->lAnnot = NOT_USED;
  pElemNew->lFillBreak = NOT_USED;
  memset(&(pElemNew->recImageDist), 0, sizeof(rec_a));
  pElemNew->lSpace = NOT_USED;
  if (pElemNew->m_pStr)
    delete pElemNew->m_pStr;
  pElemNew->m_pStr = NULL;
  pElemNew->pLCNext = NULL;
  pElemNew->pLCPrev = NULL;
  pElemNew->bBlockEnd = false_a;
  pElemNew->bCanPageBreak = false_a;
  pElemNew->bTBBreak = false_a;
  pElemNew->bColumnBreak = false_a;
  pElemNew->m_strAnchor.assign("");
  pElemNew->eElemPageMark = eNoPageMark;
  pElemNew->sTabStop = NOT_USED;

  return pElemNew;
}

bool_a PDFAddToLineElemList(PDFDocPTR i_pPDFDoc, PDFLine *i_pLine, long i_lWidth, long i_lHeight,
                            CPDFString *i_pStr, long i_lImage, rec_a *i_precImageDist, long i_lSpace, CPDFTextBox *i_pTextBox)
{
  PDFLineElemList *pElemList, *pElemNext;
  long lAsc = PDFGetDocClass(i_pPDFDoc)->GetActualFontAscentHeight();
  long lDesc = PDFGetDocClass(i_pPDFDoc)->GetActualFontDescentHeight();
  bool_a bSpace = false_a;
  if (!i_pPDFDoc || !i_pTextBox || !i_pLine)
    return false_a;

  bSpace = PDFHasLineOnlySpace(i_pLine);
  pElemNext = i_pLine->pElemList;
  pElemList = new PDFLineElemList;
  if (!pElemList)
    return false_a;
  pElemList->m_Color = i_pTextBox->m_Color;
  pElemList->m_BgColor = i_pTextBox->m_BgColor;

  pElemList->lFont = PDFGetDocClass(i_pPDFDoc)->GetActualFontID();
  pElemList->fFontSize = PDFGetDocClass(i_pPDFDoc)->GetActualFontSize();
  pElemList->lElemWidth = i_lWidth;
  pElemList->lElemHeight = i_lHeight;
  pElemList->lLetterSpace = i_pTextBox->lLetterSpace;
  pElemList->lImage = i_lImage;
  pElemList->lAnnot = NOT_USED;
  pElemList->lFillBreak = NOT_USED;
  if (i_precImageDist)
  {
    pElemList->recImageDist.lTop = i_precImageDist->lTop;
    pElemList->recImageDist.lLeft = i_precImageDist->lLeft;
    pElemList->recImageDist.lBottom = i_precImageDist->lBottom;
    pElemList->recImageDist.lRight = i_precImageDist->lRight;
  }
  else
    memset(&(pElemList->recImageDist), 0, sizeof(rec_a));
  pElemList->lSpace = i_lSpace;
  pElemList->m_pStr = NULL;
  pElemList->pLCNext = NULL;
  pElemList->pLCPrev = NULL;
  pElemList->bBlockEnd = false_a;
  pElemList->bCanPageBreak = false_a;
  pElemList->bTBBreak = false_a;
  pElemList->bColumnBreak = false_a;
  pElemList->m_strAnchor.assign("");
  pElemList->eElemPageMark = eNoPageMark;
  pElemList->sTabStop = NOT_USED;
  if (i_pStr)
  {
    pElemList->m_pStr = new CPDFString(*i_pStr);
    if (!pElemList->m_pStr)
      return false_a;
  }
  if (i_pLine->pElemList)
  {
    for(;;)
    {
      if (pElemNext->pLCNext)
        pElemNext = pElemNext->pLCNext;
      else
      {
        pElemNext->pLCNext = pElemList;
        pElemList->pLCPrev = pElemNext;
        break;
      }
    }
  }
  else
    i_pLine->pElemList = pElemList;

  if (i_lImage < 0 && i_lSpace < 0)
  {
    size_t Len = 0, i;
    if (NULL != i_pStr && (Len = i_pStr->Length()) > 0)
    {
      bool_a bF = false_a;
      for (i = 0; i < Len; i++)
      {
        if (!i_pStr->GetPDFChar(i).IsSpace())
        {
          bF = true_a;
          break;
        }
      }
      if (!bF)
      {
        i_pLine->lCurrLineWidth += i_lWidth;
        return true_a;
      }
    }
    else
    {
      i_pLine->lCurrLineWidth += i_lWidth;
      return true_a;
    }
  }


  i_pTextBox->lTBHeight -= i_pLine->lCurrLineHeight;
  i_pTextBox->pActiveParagraph->lParaHeight -= i_pLine->lCurrLineHeight;

  if (NULL == i_pStr || i_pStr->Empty())
  {
    lAsc = i_lHeight;
    lDesc = 0;
  }
  else
    lAsc = i_lHeight - (-lDesc);

  if (0 == i_pLine->lCurrLineWidth)
  {
    i_pLine->lCurrLineHeight = i_lHeight;
    i_pLine->lCurrLineDesc = lDesc;
    i_pLine->lCurrLineAsc = lAsc;
    i_pLine->lCurrLineHeight += i_pTextBox->pActiveParagraph->lLineSpace;
  }
  else
  {
    if (bSpace || i_pLine->lCurrLineAsc < lAsc)
    {
      i_pLine->lCurrLineHeight += (lAsc - i_pLine->lCurrLineAsc);
      i_pLine->lCurrLineAsc = lAsc;
    }
    if (bSpace || i_pLine->lCurrLineDesc > lDesc)
    {
      i_pLine->lCurrLineHeight += (i_pLine->lCurrLineDesc - lDesc);
      i_pLine->lCurrLineDesc = lDesc;
    }
  }
  i_pLine->lCurrLineWidth += i_lWidth;
  i_pTextBox->lTBHeight += i_pLine->lCurrLineHeight;
  i_pTextBox->pActiveParagraph->lParaHeight += i_pLine->lCurrLineHeight;
  return true_a;
}

bool_a PDFFreeTBRanges(rec_a ***i_pppRec)
{
  rec_a **pRec = *i_pppRec;
  long i = 0;
  if (!pRec)
    return false_a;
  for (;;)
  {
    if (pRec[i])
    {
      free(pRec[i]);
      pRec[i] = NULL;
      i++;
    }
    else
      break;
  }
  free(pRec);
  *i_pppRec = NULL;
  return true_a;
}

rec_a **PDFGetTBRanges(long i_lYBegin, long i_lYEnd, CPDFTextBox *i_pTextBox, short *o_pCount)
{
  PDFTBSpace  *pSpaces    = NULL;
  rec_a       **pRec      = NULL;
  short       sCount      = 0;
  short       sAlloc      = 50;
  rec_a       recTemp;
  if (!i_pTextBox)
    return NULL;
  pSpaces = i_pTextBox->pTBSpaces;
  while (pSpaces)
  {
    if (pSpaces->eSpaceType == eFixTableWidth)
    {
      pSpaces = pSpaces->pTBSpaceNext;
      continue;
    }
    recTemp = pSpaces->recSpacePos;
    i_pTextBox->MakeDirection(recTemp);
    if (((i_lYEnd > recTemp.lBottom) && (i_lYEnd < recTemp.lTop))
        ||
        ((i_lYBegin > recTemp.lBottom) && (i_lYBegin < recTemp.lTop)))
    {
      if (!pRec)
      {
        pRec = (rec_a**)PDFMalloc(sAlloc * sizeof(rec_a *));
        if (!pRec)
          return NULL;
      }
      if (!sCount
          ||
          (recTemp.lLeft > pRec[sCount - 1]->lRight))
      {
        sCount++;
        if (sCount == sAlloc)
        {
          sAlloc += 50;
          pRec = (rec_a**)realloc(pRec, sAlloc * sizeof(rec_a *));
          memset(pRec + (sAlloc - 50), 0, 50 * sizeof(rec_a *));
        }
        pRec[sCount - 1] = (rec_a*)PDFMalloc(sizeof(rec_a));
        if (!(pRec[sCount - 1]))
          return NULL;
        rec_a recPosSize = i_pTextBox->GetPosSize();
        i_pTextBox->MakeDirection(recPosSize);
        pRec[sCount - 1]->lBottom = recTemp.lBottom/* > recPosSize.lBottom ? 
                                      recTemp.lBottom : recPosSize.lBottom*/;
        pRec[sCount - 1]->lLeft = recTemp.lLeft > recPosSize.lLeft ?
                                    recTemp.lLeft : recPosSize.lLeft;
        pRec[sCount - 1]->lRight = recTemp.lRight < recPosSize.lRight ?
                                    recTemp.lRight : recPosSize.lRight;
        pRec[sCount - 1]->lTop = recTemp.lTop/* < recPosSize.lTop ?
                                    recTemp.lTop : recPosSize.lTop*/;
        if (pRec[sCount - 1]->lLeft < i_pTextBox->GetLeftDrawPos(true_a)
                && pRec[sCount - 1]->lRight > i_pTextBox->GetRightDrawPos(true_a))
          break;
      }
      else
      {
        if (pRec[sCount - 1]->lRight < recTemp.lRight)
        {
          pRec[sCount - 1]->lRight = recTemp.lRight;
          if (pRec[sCount - 1]->lRight > i_pTextBox->GetRightDrawPos(true_a))
          {
            pRec[sCount - 1]->lRight = i_pTextBox->GetRightDrawPos(true_a);
            break;
          }
        }
      }
    }
    pSpaces = pSpaces->pTBSpaceNext;
  }
  *o_pCount = sCount;
  return pRec;
}

bool_a PDFFreeTBImageList(CPDFTextBox *i_pTextBox)
{
  PDFTBImage  *pTBImage = NULL;
  if (!i_pTextBox)
    return false_a;
  pTBImage = i_pTextBox->pTBImages;
  while (pTBImage)
  {
    if (pTBImage->pTBImageNext)
    {
      pTBImage = pTBImage->pTBImageNext;
      free(pTBImage->pTBImagePrev);
      pTBImage->pTBImagePrev = NULL;
    }
    else
    {
      free(pTBImage);
      pTBImage = NULL;
    }
  }
  i_pTextBox->pTBImages = NULL;
  return true_a;
}

bool_a PDFFreeTBSpaceList(CPDFTextBox *i_pTextBox)
{
  PDFTBSpace  *pSpaces = NULL;
  if (!i_pTextBox)
    return false_a;
  pSpaces = i_pTextBox->pTBSpaces;
  while (pSpaces)
  {
    if (pSpaces->pTBSpaceNext)
    {
      pSpaces = pSpaces->pTBSpaceNext;
      free(pSpaces->pTBSpacePrev);
      pSpaces->pTBSpacePrev = NULL;
    }
    else
    {
      free(pSpaces);
      pSpaces = NULL;
    }
  }
  i_pTextBox->pTBSpaces = NULL;
  return true_a;
}

bool_a PDFAddToTBImageList(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lIndex,
                           rec_a i_recImage, rec_a i_recDist)
{
  PDFTBImage  *pTBImage       = NULL;
  PDFTBImage  *pLastTBImage   = NULL;
  pTBImage = (PDFTBImage  *)PDFMalloc(sizeof(PDFTBImage));
  if (!pTBImage)
    return false_a;
  pTBImage->lImage = i_lIndex;
  pTBImage->pTBImageNext = NULL;
  pTBImage->pTBImagePrev = NULL;
  COPY_REC(pTBImage->recPos, i_recImage);
  COPY_REC(pTBImage->recDist, i_recDist);
  if (i_pTextBox->pTBImages)
  {
    pLastTBImage = i_pTextBox->pTBImages;
    for(;;)
    {
      if ((pLastTBImage->recPos.lLeft - pLastTBImage->recDist.lLeft)
              > (pTBImage->recPos.lLeft - pTBImage->recDist.lLeft))
      {
        if (pLastTBImage->pTBImagePrev)
        {
          pTBImage->pTBImageNext = pLastTBImage;
          pTBImage->pTBImagePrev = pLastTBImage->pTBImagePrev;
          pTBImage->pTBImageNext->pTBImagePrev = pTBImage;
          pTBImage->pTBImagePrev->pTBImageNext = pTBImage;
        }
        else
        {
          pTBImage->pTBImageNext = pLastTBImage;
          pLastTBImage->pTBImagePrev = pTBImage;
          i_pTextBox->pTBImages = pTBImage;
        }
        break;
      }
      else
      {
        if (pLastTBImage->pTBImageNext)
          pLastTBImage = pLastTBImage->pTBImageNext;
        else
        {
          pLastTBImage->pTBImageNext = pTBImage;
          pTBImage->pTBImagePrev = pLastTBImage;
          break;
        }
      }
    }
  }
  else
    i_pTextBox->pTBImages = pTBImage;

  i_recImage.lBottom -= i_recDist.lBottom;
  i_recImage.lLeft -= i_recDist.lLeft;
  i_recImage.lRight += i_recDist.lRight;
  i_recImage.lTop += i_recDist.lTop;
  return PDFAddToTBSpaceList(i_pPDFDoc, i_pTextBox, i_recImage, (long)pTBImage, eFixImage);
}

void PDFDeleteTBSpace(CPDFTextBox *i_pTextBox, long i_lID)
{
  PDFTBSpace  *pLastTBSpace = NULL;
  if (!i_pTextBox)
    return;
  pLastTBSpace = i_pTextBox->pTBSpaces;
  while (pLastTBSpace)
  {
    if (pLastTBSpace->lSpaceID == i_lID)
    {
      if (i_pTextBox->pTBSpaces == pLastTBSpace)
        i_pTextBox->pTBSpaces = pLastTBSpace->pTBSpaceNext;
      if (pLastTBSpace->pTBSpaceNext)
        pLastTBSpace->pTBSpaceNext->pTBSpacePrev = pLastTBSpace->pTBSpacePrev;
      if (pLastTBSpace->pTBSpacePrev)
        pLastTBSpace->pTBSpacePrev->pTBSpaceNext = pLastTBSpace->pTBSpaceNext;
      free(pLastTBSpace);
      return;
    }
    pLastTBSpace = pLastTBSpace->pTBSpaceNext;
  }
  return;
}

PDFTBSpace *PDFGetTBSpace(CPDFTextBox *i_pTextBox, long i_lID)
{
  PDFTBSpace  *pLastTBSpace = NULL;
  if (!i_pTextBox || i_lID < 0)
    return NULL;
  pLastTBSpace = i_pTextBox->pTBSpaces;
  while (pLastTBSpace)
  {
    if (pLastTBSpace->lSpaceID == i_lID)
      return pLastTBSpace;
    pLastTBSpace = pLastTBSpace->pTBSpaceNext;
  }
  return NULL;
}

PDFParagraph *PDFGetTBSpaceParagraph(CPDFTextBox *i_pTextBox, long i_lID)
{
  PDFLineElemList *pElem = NULL;
  PDFLine *pLine = NULL;
  PDFParagraph *pParagraph = NULL;
  if (!i_pTextBox)
    return NULL;
  pParagraph = i_pTextBox->pParagraph;
  while (pParagraph)
  {
    pLine = pParagraph->pPDFLine;
    while (pLine)
    {
      pElem = pLine->pElemList;
      while (pElem)
      {
        if (pElem->lSpace == i_lID)
          return pParagraph;
        pElem = pElem->pLCNext;
      }
      pLine = pLine->pLineNext;
    }
    pParagraph = pParagraph->pParaNext;
  }
  return NULL;
}

bool_a PDFNeededParagraph(CPDFTextBox *i_pTextBox)
{
  bool_a bRet = false_a;
  if (!i_pTextBox)
    return bRet;
/*
  if (i_pTextBox->pParagraph && i_pTextBox->pParagraph->pPDFLine && i_pTextBox->pParagraph->pPDFLine->pElemList
      && i_pTextBox->pParagraph->pPDFLine->pElemList->lSpace > 0)
    return false_a;
  if (i_pTextBox->pParagraph == i_pTextBox->pActiveParagraph && i_pTextBox->pParagraph->pPDFLine == i_pTextBox->pParagraph->pPDFLastLine
      && i_pTextBox->pParagraph->pPDFLine->lCurrLineWidth == 0)
    return false_a;
*/
//  if (i_pTextBox->bTBCopy)
//    return true_a;
  if (!PDFIsParagraphEmpty(i_pTextBox->pActiveParagraph))
    return true_a;
  if (!i_pTextBox->pActiveParagraph)
    return true_a;
  if (PDFIsParagraphEmpty(i_pTextBox->pActiveParagraph) &&
        PDFHasParagraphFixTable(i_pTextBox, i_pTextBox->pActiveParagraph->pParaPrev))
    return true_a;
  return false_a;
}

bool_a PDFIsParagraphEmpty(PDFParagraph *i_pPara)
{
  if (!i_pPara || !i_pPara->pPDFLastLine)
    return false_a;
  if (i_pPara->pPDFLastLine == i_pPara->pPDFLine)
  {
    if (!(i_pPara->pPDFLastLine->pElemList))
      return true_a;
    //TODO: this is not correct - for example, also content of elements must be checked 
    if (0 == i_pPara->pPDFLastLine->lCurrLineWidth)
      return true_a;
  }
  return false_a;
}

bool_a PDFIsLastLineEmpty(CPDFTextBox *i_pTextBox)
{
  if (!i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  if (!(i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList))
    return true_a;
    //TODO: this is not correct - for example, also content of elements must be checked 
  if (0 == i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth)
    return true_a;
  return false_a;
}

bool_a PDFIsTextBoxEmpty(CPDFTextBox *i_pTextBox)
{
  if (!i_pTextBox || !i_pTextBox->pActiveParagraph || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  if (i_pTextBox->pParagraph != i_pTextBox->pActiveParagraph)
    return false_a;
  if (i_pTextBox->pActiveParagraph->pPDFLastLine != i_pTextBox->pActiveParagraph->pPDFLine)
    return false_a;
   if (!(i_pTextBox->pActiveParagraph->pPDFLastLine->pElemList))
     return true_a;
   if (0 == i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth)
     return true_a;
  return false_a;
}

bool_a PDFAddToTBSpaceList(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, rec_a i_recSpace,
                            long i_lID, eSPACETYPE i_eSpaceType)
{
  PDFTBSpace  *pTBSpace       = NULL;
  PDFTBSpace  *pLastTBSpace   = NULL;
  PDFParagraph *pParagraph = NULL;
  bool_a bAdj = true_a;
  long lW;

  pTBSpace = PDFGetTBSpace(i_pTextBox, i_lID);
  if (!pTBSpace)
  {
    pTBSpace = (PDFTBSpace *)PDFMalloc(sizeof(PDFTBSpace));
    if (!pTBSpace)
      return false_a;
    pTBSpace->pTBSpaceNext = NULL;
    pTBSpace->pTBSpacePrev = NULL;
    COPY_REC(pTBSpace->recSpacePos, i_recSpace);
    pTBSpace->lSpaceID = i_lID;
    pTBSpace->eSpaceType = i_eSpaceType;
    if (i_eSpaceType == eFixTableWidth || i_eSpaceType == eFrame || i_eSpaceType == eFixEmptySpace)
    {
      if (PDFNeededParagraph(i_pTextBox) && i_eSpaceType == eFixTableWidth)
        PDFAddParagraph(i_pPDFDoc, i_pTextBox);
      else
      {
        long lHeight = pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom;
        if (i_eSpaceType != eFrame)
          pTBSpace->recSpacePos.lTop = i_pTextBox->GetTopDrawPos()
                        - (i_pTextBox->lTBHeight - i_pTextBox->pActiveParagraph->lParaHeight);//paragraph must be empty
        else
          pTBSpace->recSpacePos.lTop = i_pTextBox->GetTopDrawPos()
                        - (i_pTextBox->lTBHeight - i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight);
        pTBSpace->recSpacePos.lBottom = pTBSpace->recSpacePos.lTop - lHeight;
//        pTBSpace->recSpacePos.lLeft = i_pTextBox->GetLeftDrawPos() - 1;
//        pTBSpace->recSpacePos.lRight = i_pTextBox->GetRightDrawPos() + 1;


long lTmpTestHeight = CalcParagraphsHeights(i_pTextBox->pParagraph);
if (lTmpTestHeight != i_pTextBox->lTBHeight)
  lTmpTestHeight = lTmpTestHeight;


        if (i_pPDFDoc->pDocFunc)
        {
          if ((*i_pPDFDoc->pDocFunc)((void *)pTBSpace->lSpaceID, pTBSpace->eSpaceType, i_pTextBox->lPage + 1, pTBSpace->recSpacePos.lTop,
                                     pTBSpace->recSpacePos.lLeft, false_a, (void *)i_pTextBox) < 0)
            return false_a;
        }
      }
      if (i_eSpaceType == eFixTableWidth)
        lW = i_pTextBox->GetDrawWidth(true_a);
      else
        lW = pTBSpace->recSpacePos.lRight - pTBSpace->recSpacePos.lLeft;
      if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine, lW,
                                i_recSpace.lTop - i_recSpace.lBottom, NULL, NOT_USED, NULL, i_lID, i_pTextBox))
        return false_a;
      i_pTextBox->pActiveParagraph->lParaWidth = i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineWidth;
      if (i_pTextBox->lTBWidth < i_pTextBox->pActiveParagraph->lParaWidth)
        i_pTextBox->lTBWidth = i_pTextBox->pActiveParagraph->lParaWidth;

//      if (!i_pTextBox->bTBCopy)
//        PDFAddParagraph(i_pPDFDoc, i_pTextBox);
    }
  }
  else
  {
    if (pTBSpace->pTBSpaceNext)
      pTBSpace->pTBSpaceNext->pTBSpacePrev = pTBSpace->pTBSpacePrev;
    if (pTBSpace->pTBSpacePrev)
      pTBSpace->pTBSpacePrev->pTBSpaceNext = pTBSpace->pTBSpaceNext;
    if (!pTBSpace->pTBSpaceNext && !pTBSpace->pTBSpacePrev)
      i_pTextBox->pTBSpaces = NULL;
    else
      if (!pTBSpace->pTBSpacePrev)
        i_pTextBox->pTBSpaces = pTBSpace->pTBSpaceNext;
    pParagraph = PDFGetTBSpaceParagraph(i_pTextBox, pTBSpace->lSpaceID);
    if (!pParagraph)
      return false_a;
    pTBSpace->pTBSpaceNext = NULL;
    pTBSpace->pTBSpacePrev = NULL;
    if (pTBSpace->eSpaceType == eFixTableWidth)
    {
      i_pTextBox->lTBHeight -= (pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom);
      pParagraph->lParaHeight -= (pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom);
    }
    COPY_REC(pTBSpace->recSpacePos, i_recSpace);
    if (i_eSpaceType == eFixTableWidth)
    {
      i_pTextBox->lTBHeight += (i_recSpace.lTop - i_recSpace.lBottom);
      pParagraph->lParaHeight += (pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom);
      pParagraph->pPDFLine->lCurrLineHeight = (pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom);
      pParagraph->pPDFLine->lCurrLineHeight += pParagraph->lLineSpace;
      pParagraph->pPDFLastLine->lCurrLineWidth = i_pTextBox->GetDrawWidth(true_a);

      if (pParagraph->lParaWidth < pParagraph->pPDFLastLine->lCurrLineWidth)
        pParagraph->lParaWidth = pParagraph->pPDFLastLine->lCurrLineWidth;
      if (i_pTextBox->lTBWidth < pParagraph->lParaWidth)
        i_pTextBox->lTBWidth = pParagraph->lParaWidth;
      bAdj = false_a;
    }
    pTBSpace->eSpaceType = i_eSpaceType;
  }
  if (i_pTextBox->pTBSpaces)
  {
    pLastTBSpace = i_pTextBox->pTBSpaces;
    for(;;)
    {
      if (pLastTBSpace->recSpacePos.lLeft > pTBSpace->recSpacePos.lLeft)
      {
        if (pLastTBSpace->pTBSpacePrev)
        {
          pTBSpace->pTBSpaceNext = pLastTBSpace;
          pTBSpace->pTBSpacePrev = pLastTBSpace->pTBSpacePrev;
          pTBSpace->pTBSpaceNext->pTBSpacePrev = pTBSpace;
          pTBSpace->pTBSpacePrev->pTBSpaceNext = pTBSpace;
        }
        else
        {
          pTBSpace->pTBSpaceNext = pLastTBSpace;
          pTBSpace->pTBSpacePrev = NULL;
          pLastTBSpace->pTBSpacePrev = pTBSpace;
          i_pTextBox->pTBSpaces = pTBSpace;
        }
        break;
      }
      else
      {
        if (pLastTBSpace->pTBSpaceNext)
          pLastTBSpace = pLastTBSpace->pTBSpaceNext;
        else
        {
          pLastTBSpace->pTBSpaceNext = pTBSpace;
          pTBSpace->pTBSpacePrev = pLastTBSpace;
          break;
        }
      }
    }
  }
  else
    i_pTextBox->pTBSpaces = pTBSpace;

  if (bAdj)
   if (!PDFAdjustParagraph(i_pPDFDoc, i_pTextBox, pTBSpace->recSpacePos.lTop, false_a))
      return false_a;

  return true_a;
}

bool_a PDFInsertLine(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox)
{
  if (!i_pPDFDoc || !i_pTextBox || !i_pTextBox->pActiveParagraph
        || !i_pTextBox->pActiveParagraph->pPDFLastLine)
    return false_a;
  i_pTextBox->pActiveParagraph->pPDFLastLine->bNewLine = true_a;
  i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext = PDFCreateLine(i_pPDFDoc, i_pTextBox->pActiveParagraph, true_a);
  i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext->pLinePrev = i_pTextBox->pActiveParagraph->pPDFLastLine;
  i_pTextBox->pActiveParagraph->pPDFLastLine = i_pTextBox->pActiveParagraph->pPDFLastLine->pLineNext;
  i_pTextBox->pActiveParagraph->lParaHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
  i_pTextBox->lTBHeight += i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight;
  return true_a;
}

PDFLine *PDFCreateLine(PDFDocPTR i_pPDFDoc, PDFParagraph *i_pParagraph, bool_a i_bFirstLine)
{
  PDFLine *pRetLine = NULL;
  pRetLine = (PDFLine *)PDFMalloc(sizeof(PDFLine));
  if (!pRetLine)
    return NULL;
  pRetLine->lCurrLineWidth = 0;
  pRetLine->lCurrLineHeight = i_bFirstLine ? PDFGetDocClass(i_pPDFDoc)->GetActualFontHeight() : 0;
  pRetLine->lCurrLineHeight += i_pParagraph->lLineSpace;
  pRetLine->lCurrLineAsc = i_bFirstLine ? PDFGetDocClass(i_pPDFDoc)->GetActualFontAscentHeight() : 0;
  pRetLine->lCurrLineDesc = i_bFirstLine ? PDFGetDocClass(i_pPDFDoc)->GetActualFontDescentHeight() : 0;
  pRetLine->sLastBlock = 0;
  pRetLine->sLastTab = NOT_USED;
  pRetLine->bNewLine = false_a;
  pRetLine->pElemList = NULL;
  pRetLine->pLineNext = NULL;
  pRetLine->pLinePrev = NULL;
  return pRetLine;
}

//paragraph
PDFParagraph *PDFCreateParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox)
{
  PDFParagraph *pParagraph = NULL;
  pParagraph = (PDFParagraph *)PDFMalloc(sizeof(PDFParagraph));
  if (!pParagraph)
    return NULL;

  PDFGetDocClass(i_pPDFDoc)->SetActualFontID(PDFGetDocClass(i_pPDFDoc)->GetActualFontID());
  PDFGetDocClass(i_pPDFDoc)->SetActualFontSize(PDFGetDocClass(i_pPDFDoc)->GetActualFontSize());

  pParagraph->recIndents.lBottom = i_pTextBox->recIndents.lBottom;
  pParagraph->recIndents.lLeft = i_pTextBox->recIndents.lLeft;
  pParagraph->recIndents.lRight = i_pTextBox->recIndents.lRight;
  pParagraph->recIndents.lTop = i_pTextBox->recIndents.lTop;
  pParagraph->lIndent = i_pTextBox->lIndent;
  pParagraph->sAlignment = i_pTextBox->sAlignment;
  pParagraph->m_bKeepLinesTogether = false_a;
  pParagraph->m_bKeepWithNext = false_a;
  pParagraph->m_bWidowOrphan = false_a;
  pParagraph->m_bParagraphPageBreak = false_a;
  pParagraph->lParaHeight = pParagraph->recIndents.lTop + pParagraph->recIndents.lBottom;
  pParagraph->lParaWidth = 0;
  pParagraph->lLineSpace = i_pTextBox->lLineSpace;
  pParagraph->bParaContinue = false_a;

  pParagraph->lParaTabSize = i_pTextBox->lTabSize;
  pParagraph->cParaTabDecimalType = i_pTextBox->cTabDecimalType;

  pParagraph->pParaTabStops = NULL;
  pParagraph->sParaTabsCount = 0;
  if (i_pTextBox->pTBTabStops && i_pTextBox->sTabsCount)
  {
    pParagraph->pParaTabStops = (PDFTabStop *)PDFMalloc(i_pTextBox->sTabsCount * sizeof(PDFTabStop));
    memcpy(pParagraph->pParaTabStops, i_pTextBox->pTBTabStops, i_pTextBox->sTabsCount * sizeof(PDFTabStop));
    pParagraph->sParaTabsCount = i_pTextBox->sTabsCount;
  }

  pParagraph->pPDFLine = NULL;
  pParagraph->pPDFLastLine = NULL;
  pParagraph->pParaNext = NULL;
  pParagraph->pParaPrev = NULL;

  pParagraph->pPDFLine = PDFCreateLine(i_pPDFDoc, pParagraph, true_a);
  pParagraph->pPDFLastLine = pParagraph->pPDFLine;

  pParagraph->lParaHeight += pParagraph->pPDFLastLine->lCurrLineHeight;
  i_pTextBox->lTBHeight += pParagraph->lParaHeight;

  return pParagraph;
}

bool_a PDFAddParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox)
{
  PDFParagraph *pActPara = NULL;
  pActPara = PDFCreateParagraph(i_pPDFDoc, i_pTextBox);
  if (!(i_pTextBox->pActiveParagraph))
    i_pTextBox->pActiveParagraph = i_pTextBox->pParagraph = pActPara;
  else
  {
    i_pTextBox->pActiveParagraph->pParaNext = pActPara;
    pActPara->pParaPrev = i_pTextBox->pActiveParagraph;
    i_pTextBox->pActiveParagraph = pActPara;
  }
  return true_a;
}

bool_a PDFMoveTextBoxContents(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBoxFrom, CPDFTextBox *i_pTextBoxTo)
{
  float         fFontSize;
  long          lFont;
  long          lLSpace;
  float         fWSpace;
  PDFParagraph  *pOldParagraph  = NULL;
  PDFParagraph  *pParagraph = NULL;
  if (!i_pPDFDoc || !i_pTextBoxFrom || !i_pTextBoxTo)
      return false_a;
  if (PDFIsTextBoxEmpty(i_pTextBoxFrom))
     return true_a;
  fFontSize = PDFGetDocClass(i_pPDFDoc)->GetActualFontSize();
  lFont = PDFGetDocClass(i_pPDFDoc)->GetActualFontID();
  lLSpace = i_pPDFDoc->lLetterSpace;
  fWSpace = i_pPDFDoc->fWordSpacing;

  pParagraph = i_pTextBoxFrom->pParagraph;
  while (pParagraph)
  {
    if (!PDFCopyParagraph(i_pPDFDoc, i_pTextBoxTo, pParagraph, i_pTextBoxFrom,
                          false_a, pParagraph != i_pTextBoxFrom->pParagraph))
      return false_a;
    pOldParagraph = pParagraph;
    pParagraph = pParagraph->pParaNext;
    PDFFreeParagraph(&pOldParagraph);
    pOldParagraph = NULL;
  }
  i_pTextBoxFrom->pParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBoxFrom);
  i_pTextBoxFrom->pActiveParagraph = i_pTextBoxFrom->pParagraph;
  i_pTextBoxFrom->lTBHeight = 0;
  i_pTextBoxFrom->lTBWidth = 0;

  PDFGetDocClass(i_pPDFDoc)->SetActualFontSize(fFontSize);
  PDFGetDocClass(i_pPDFDoc)->SetActualFontID(lFont);
  i_pPDFDoc->lLetterSpace = lLSpace;
  i_pPDFDoc->fWordSpacing = fWSpace;

  return true_a;
}

bool_a PDFAdjustParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lTop, bool_a i_bReplacePageMark)
{
  float         fFontSize;
  long          lFont;
  long          lLSpace;
  float         fWSpace;
  CPDFTextBox    strTextBoxSave;
  PDFParagraph  *pNewParagraph  = NULL;
  PDFParagraph  *pOldParagraph  = NULL;
  PDFParagraph  *pParagraph = NULL;
  long          lY, lll = 0;
  if (!i_pPDFDoc || !i_pTextBox)
      return false_a;
  if (PDFIsTextBoxEmpty(i_pTextBox))
      return true_a;


  long lTmpTestHeight = CalcParagraphsHeights(i_pTextBox->pParagraph);
  if (lTmpTestHeight != i_pTextBox->lTBHeight)
    lTmpTestHeight = lTmpTestHeight;

  fFontSize = PDFGetDocClass(i_pPDFDoc)->GetActualFontSize();
  lFont = PDFGetDocClass(i_pPDFDoc)->GetActualFontID();
  lLSpace = i_pPDFDoc->lLetterSpace;
  fWSpace = i_pPDFDoc->fWordSpacing;
  //memset(&strTextBoxSave, 0, sizeof(PDFTextBox));
  lY = i_pTextBox->GetTopDrawPos();
  pParagraph = i_pTextBox->pParagraph;
  while (pParagraph)
  {
    if (/*(i_lTop <= lY) && */(i_lTop >= (lY - (pParagraph->recIndents.lTop + pParagraph->lParaHeight + pParagraph->recIndents.lBottom))))
      break;
    lY -= pParagraph->lParaHeight;
    pParagraph = pParagraph->pParaNext;
  }
  if (!pParagraph)
    return true_a;
  COPY_TB((&strTextBoxSave), i_pTextBox);
  i_pTextBox->bTBCopy = true_a;
  pOldParagraph = pParagraph;

  pParagraph = i_pTextBox->pParagraph;
  while (pParagraph)
  {
    lll += pParagraph->lParaHeight;
    pParagraph = pParagraph->pParaNext;
  }

  pParagraph = pOldParagraph;
  while (pParagraph)
  {
    i_pTextBox->lTBHeight -= pParagraph->lParaHeight;
    pParagraph = pParagraph->pParaNext;
  }


  pNewParagraph = PDFCreateParagraph(i_pPDFDoc, i_pTextBox);
  pNewParagraph->pParaPrev = pOldParagraph->pParaPrev;
  if (pOldParagraph->pParaPrev)
    pOldParagraph->pParaPrev->pParaNext = pNewParagraph;
  else
    i_pTextBox->pParagraph = pNewParagraph;
  i_pTextBox->pActiveParagraph = pNewParagraph;

  pParagraph = pOldParagraph;
  while (pParagraph)
  {
    if (!PDFCopyParagraph(i_pPDFDoc, i_pTextBox, pParagraph, i_pTextBox, i_bReplacePageMark, pParagraph != pOldParagraph))
      return false_a;
    pOldParagraph = pParagraph;
    pParagraph = pParagraph->pParaNext;
    PDFFreeParagraph(&pOldParagraph);
    pOldParagraph = NULL;
    i_pTextBox->pTBTabStops = strTextBoxSave.pTBTabStops;
    i_pTextBox->sTabsCount = strTextBoxSave.sTabsCount;
  }

  COPY_TB(i_pTextBox, (&strTextBoxSave));
  i_pTextBox->bTBCopy = false_a;
  PDFGetDocClass(i_pPDFDoc)->SetActualFontSize(fFontSize);
  PDFGetDocClass(i_pPDFDoc)->SetActualFontID(lFont);
  i_pPDFDoc->lLetterSpace = lLSpace;
  i_pPDFDoc->fWordSpacing = fWSpace;

  lTmpTestHeight = CalcParagraphsHeights(i_pTextBox->pParagraph);
  if (lTmpTestHeight != i_pTextBox->lTBHeight)
    lTmpTestHeight = lTmpTestHeight;


  return true_a;
}

bool_a PDFCopyLineElement(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFLineElemList *i_pElement,
                          CPDFTextBox *i_pTextBoxFrom, bool_a i_bReplacePageMark)
{
  float fFontSize;
  long lFontIndex;
  long lLSpace, lW;
  float fWSpace;
  if (!i_pPDFDoc || !i_pTextBox || !i_pElement || !i_pTextBoxFrom)
    return false_a;
  fFontSize = PDFGetDocClass(i_pPDFDoc)->GetActualFontSize();
  lFontIndex = PDFGetDocClass(i_pPDFDoc)->GetActualFontID();
  lLSpace = i_pPDFDoc->lLetterSpace;
  fWSpace = i_pPDFDoc->fWordSpacing;

  COPY_ELEM_TO_TEXTBOX(i_pTextBox, i_pElement);
  PDFGetDocClass(i_pPDFDoc)->SetActualFontSize(i_pElement->fFontSize);
  PDFGetDocClass(i_pPDFDoc)->SetActualFontID(i_pElement->lFont);

  if (i_pElement->lImage != NOT_USED)
  {
    CPDFImage *pImg = PDFGetDocClass(i_pPDFDoc)->GetImage(i_pElement->lImage);
    if (!pImg)
      return false_a;
    if (!PDFAddImageToTextBox(i_pPDFDoc, pImg->filepath, i_pTextBox, NOT_USED, NOT_USED,
                              i_pElement->lElemWidth, i_pElement->lElemHeight, i_pElement->recImageDist.lLeft,
                              i_pElement->recImageDist.lRight, i_pElement->recImageDist.lTop, i_pElement->recImageDist.lBottom, true_a))
      return false_a;
    if (i_pElement->sTabStop >= 0)
      PDFAddTextToTextBox(i_pPDFDoc, czTABSTOP, i_pTextBox);
  }
  else
    if (i_pElement->lSpace != NOT_USED)
    {
      PDFTBSpace  *pTBSpace = PDFGetTBSpace(i_pTextBox, i_pElement->lSpace);
      if (pTBSpace)
      {
        long lHeight = pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom;

        long lTmpTestHeight = CalcParagraphsHeights(i_pTextBox->pParagraph);
        if (lTmpTestHeight != i_pTextBox->lTBHeight)
          lTmpTestHeight = lTmpTestHeight;

        if (pTBSpace->eSpaceType != eFrame)
          pTBSpace->recSpacePos.lTop = i_pTextBox->GetTopDrawPos()
                      - (i_pTextBox->lTBHeight - i_pTextBox->pActiveParagraph->lParaHeight);//paragraph must be empty
        else
          pTBSpace->recSpacePos.lTop = i_pTextBox->GetTopDrawPos()
                      - (i_pTextBox->lTBHeight - i_pTextBox->pActiveParagraph->pPDFLastLine->lCurrLineHeight);
        pTBSpace->recSpacePos.lBottom = pTBSpace->recSpacePos.lTop - lHeight;
        //pTBSpace->recSpacePos.lLeft = i_pTextBox->GetLeftDrawPos();
        //pTBSpace->recSpacePos.lRight = i_pTextBox->GetRightDrawPos();
        if (i_pPDFDoc->pDocFunc)
        {
          if ((*i_pPDFDoc->pDocFunc)((void *)pTBSpace->lSpaceID, pTBSpace->eSpaceType, i_pTextBox->lPage + 1, pTBSpace->recSpacePos.lTop,
                                      pTBSpace->recSpacePos.lLeft, false_a, (void *)i_pTextBox) < 0)
            return false_a;
        }
        if (pTBSpace->eSpaceType == eFixTableWidth)
          lW = i_pTextBox->GetDrawWidth(true_a);
        else
          lW = pTBSpace->recSpacePos.lRight - pTBSpace->recSpacePos.lLeft;
        if (!PDFAddToLineElemList(i_pPDFDoc, i_pTextBox->pActiveParagraph->pPDFLastLine,
                                  lW,
                                  pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom,
                                  NULL, NOT_USED, NULL, i_pElement->lSpace, i_pTextBox))
          return false_a;
      }
      else
      {
        rec_a recPosSize;
        pTBSpace = PDFGetTBSpace(i_pTextBoxFrom, i_pElement->lSpace);
        if (!pTBSpace)
          return false_a;
        recPosSize.lTop = i_pTextBox->GetTopDrawPos() - i_pTextBox->lTBHeight;
        recPosSize.lBottom = recPosSize.lTop - (pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom);
        recPosSize.lLeft = /*pTBSpace->recSpacePos.lLeft;//*/i_pTextBox->GetLeftDrawPos();
        recPosSize.lRight = /*pTBSpace->recSpacePos.lRight;//*/i_pTextBox->GetRightDrawPos();
        if (!PDFAddToTBSpaceList(i_pPDFDoc, i_pTextBox, recPosSize, pTBSpace->lSpaceID, pTBSpace->eSpaceType))
          return false_a;
        PDFDeleteTBSpace(i_pTextBoxFrom, i_pElement->lSpace);

        //if (!PDFAdjustParagraph(i_pPDFDoc, i_pTextBoxFrom, i_pTextBoxFrom->GetTopDrawPos(), false_a))
        //    return false_a;
      }
    }
    else
      if (i_pElement->m_pStr && i_pElement->m_pStr->Length() > 0)
      {
        PDFAddTextToTextBox(i_pPDFDoc, *(i_pElement->m_pStr), i_pTextBox);
        if (i_pElement->sTabStop >= 0)
          PDFAddTextToTextBox(i_pPDFDoc, czTABSTOP, i_pTextBox);
      }
      else
        if (i_pElement->sTabStop >= 0)
          PDFAddTextToTextBox(i_pPDFDoc, czTABSTOP, i_pTextBox);

  if (i_pElement->bCanPageBreak)
    PDFAddCanPageBreak(i_pPDFDoc, i_pTextBox);
//    PDFCanPageBreak(i_pPDFDoc, i_pTextBox, false_a);
  if (i_pElement->bTBBreak)
    PDFAddBreak(i_pPDFDoc, i_pTextBox);
  if (i_pElement->bColumnBreak)
    PDFAddColumnBreak(i_pPDFDoc, i_pTextBox);
  if (i_pElement->lAnnot >= 0)
    PDFAddLink(i_pPDFDoc, i_pTextBox, i_pElement->lAnnot);
  if (i_pElement->lFillBreak >= 0)
    PDFAddFillBreak(i_pPDFDoc, i_pTextBox, i_pElement->lFillBreak);
  if (i_pElement->m_strAnchor.size() > 0)
    PDFAddAnchor(i_pPDFDoc, i_pTextBox, i_pElement->m_strAnchor);
  if (i_pElement->eElemPageMark != eNoPageMark)
    if (i_bReplacePageMark)
    {
      char czTemp[50];
      if (i_pElement->eElemPageMark == ePageNum)
        sprintf(czTemp, "%ld", i_pTextBox->lPage + 1);
      else
        sprintf(czTemp, "%ld", PDFGetDocClass(i_pPDFDoc)->GetPageCount());
      PDFAddTextToTextBox(i_pPDFDoc, czTemp, i_pTextBox);
    }
    else
    {
      if (!PDFAddPageMark(i_pPDFDoc, i_pTextBox, i_pElement->eElemPageMark))
        return false_a;
    }

  PDFGetDocClass(i_pPDFDoc)->SetActualFontSize(fFontSize);
  PDFGetDocClass(i_pPDFDoc)->SetActualFontID(lFontIndex);
  i_pPDFDoc->lLetterSpace = lLSpace;
  i_pPDFDoc->fWordSpacing = fWSpace;

  return true_a;
}

bool_a PDFCopyParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pDestTextBox, PDFParagraph *i_pSourceParagraph,
                        CPDFTextBox *i_pTextBoxFrom, bool_a i_bReplacePageMark, bool_a i_bNewParagraph)
{
  PDFLine         *pLine    = NULL;
  CPDFTextBox      *pTextBox = i_pDestTextBox;
  PDFLineElemList *pElem    = NULL;
  if (!i_pPDFDoc || !pTextBox || !i_pSourceParagraph || !i_pTextBoxFrom || !(pTextBox->pActiveParagraph))
    return false_a;
  if (i_bNewParagraph)
    PDFAddParagraph(i_pPDFDoc, pTextBox);
  if (!PDFCopyParagraphAttributtesToTextBox(pTextBox, i_pSourceParagraph))
    return false_a;
  pLine = i_pSourceParagraph->pPDFLine;
  while (pLine)
  {
    pElem = pLine->pElemList;
    while (pElem)
    {
      if (!PDFCopyLineElement(i_pPDFDoc, pTextBox, pElem, i_pTextBoxFrom, i_bReplacePageMark))
        return false_a;
      pElem = pElem->pLCNext;
    }
    if (pLine->bNewLine)
    {
      if (!PDFInsertLine(i_pPDFDoc, pTextBox))
        return false_a;
    }
    pLine = pLine->pLineNext;
  }
  return true_a;
}

//free 
bool_a PDFFreeParagraph(PDFParagraph **i_ppParagraph)
{
  PDFParagraph  *pParagraph = *i_ppParagraph;
  PDFLine       *pLine      = NULL;
  PDFLine       *pOldLine   = NULL;

  while (pParagraph)
  {
    pLine = pParagraph->pPDFLine;
    while (pLine)
    {
      PDFDeleteLineElemList(&(pLine->pElemList));
      pOldLine = pLine;
      pLine = pLine->pLineNext;
      free(pOldLine);
      pOldLine = NULL;
    }
    free(pParagraph->pParaTabStops);
    pParagraph->sParaTabsCount = 0;
    free(pParagraph);
    pParagraph = NULL;
  }
  *i_ppParagraph = pParagraph;
  return true_a;
}

bool_a PDFDeleteLineElemList(PDFLineElemList **i_ppElemList)
{
  PDFLineElemList *pElemList = *i_ppElemList;
  PDFLineElemList *pOldTextList = NULL;

  while(pElemList)
  {
    //free(pElemList->pStr);
    //pElemList->pStr = NULL;
    pOldTextList = pElemList;
    pElemList = pElemList->pLCNext;
    delete pOldTextList;
    pOldTextList = NULL;
  }
  *i_ppElemList = NULL;
  return true_a;
}

//set textbox attributes

bool_a PDFHasTextBoxTransparentBg(CPDFTextBox *i_pTextBox)
{
  if (!i_pTextBox)
    return false_a;
  return i_pTextBox->bTransparent;
}

bool_a PDFSetTextBoxTransparentBg(CPDFTextBox *i_pTextBox, bool_a i_bTransparent)
{
  if (!i_pTextBox)
    return false_a;
  i_pTextBox->bTransparent = i_bTransparent;
  return true_a;
}

bool_a PDFGetTextBoxBgImage(CPDFTextBox *i_pTextBox, long *o_plTB)
{
  if (!i_pTextBox)
    return false_a;
  *o_plTB = i_pTextBox->lTBBgImage;
  return true_a;
}

bool_a PDFSetTextBoxBgImage(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, const char *i_pczImageName)
{
  if (!i_pPDFDoc || !i_pTextBox || !i_pczImageName || strlen(i_pczImageName) == 0)
    return false_a;

  CPDFImage *pImage = PDFGetDocClass(i_pPDFDoc)->CreateImage(i_pczImageName);
  if (!pImage)
    return false_a;

  i_pTextBox->lTBBgImage = pImage->lIndex;
  return true_a;
}

bool_a PDFClearTextBoxBgImage(CPDFTextBox *i_pTextBox)
{
  if (!i_pTextBox)
    return false_a;
  i_pTextBox->lTBBgImage = NOT_USED;
  return true_a;
}

bool_a PDFSetParaAlignment(CPDFTextBox *i_pTextBox, short i_sAlignment)
{
  if (!i_pTextBox || !(i_pTextBox->pActiveParagraph))
    return false_a;

  i_pTextBox->pActiveParagraph->sAlignment = i_pTextBox->sAlignment = i_sAlignment;

  return true_a;
}

bool_a PDFSetVerticalAlignment(CPDFTextBox *i_pTextBox, short i_sVAlignment)
{
  if (!i_pTextBox)
    return false_a;
  i_pTextBox->sVAlignment = i_sVAlignment;
  return true_a;
}

bool_a PDFGetTextBoxActualSize(CPDFTextBox *i_pTextBox, long *o_plWidth, long *o_plHeight)
{
  if (!i_pTextBox || !(i_pTextBox->pActiveParagraph))
    return false_a;
  
  *o_plWidth = i_pTextBox->lTBWidth;
  *o_plHeight = i_pTextBox->lTBHeight;

  return true_a;
}

bool_a PDFSetParaLineSpace(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lLineSpace)
{
  if (!i_pTextBox || !(i_pTextBox->pActiveParagraph))
    return false_a;
  if (i_lLineSpace == i_pTextBox->pActiveParagraph->lLineSpace)
    return true_a;
  i_pTextBox->pActiveParagraph->lLineSpace = i_pTextBox->lLineSpace = i_lLineSpace;

  return PDFAdjustParagraph(i_pPDFDoc, i_pTextBox, i_pTextBox->GetTopDrawPos() - i_pTextBox->lTBHeight
                                + i_pTextBox->pActiveParagraph->lParaHeight - 1, false_a);
}

bool_a PDFSetTextDirection(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, eTEXTDIRECTION i_eDirection)
{
  if (!i_pTextBox || !(i_pTextBox->pActiveParagraph))
    return false_a;
  if (i_eDirection == i_pTextBox->m_eTextDirection)
    return true_a;
  i_pTextBox->m_eTextDirection = i_eDirection;

  return PDFAdjustParagraph(i_pPDFDoc, i_pTextBox, i_pTextBox->GetTopDrawPos(), false_a);
}

bool_a PDFSetParaIndents(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lLeft, long i_lTop,
                         long i_lRight, long i_lBottom, long i_lFirstIndent)
{
  if (!i_pPDFDoc || !i_pTextBox || !(i_pTextBox->pActiveParagraph))
    return false_a;

  if (i_pTextBox->recIndents.lBottom == i_lBottom
      && i_pTextBox->recIndents.lLeft == i_lLeft
      && i_pTextBox->recIndents.lRight == i_lRight
      && i_pTextBox->recIndents.lTop == i_lTop
      && i_pTextBox->lIndent == i_lFirstIndent)
    return true_a;

  i_pTextBox->pActiveParagraph->lParaHeight -= (i_pTextBox->recIndents.lTop - i_lTop);
  i_pTextBox->pActiveParagraph->lParaHeight -= (i_pTextBox->recIndents.lBottom - i_lBottom);
  i_pTextBox->lTBHeight -= (i_pTextBox->recIndents.lTop - i_lTop);
  i_pTextBox->lTBHeight -= (i_pTextBox->recIndents.lBottom - i_lBottom);

  if (!(i_pTextBox->pActiveParagraph->pPDFLine) || !(i_pTextBox->pActiveParagraph->pPDFLine->pElemList))
  {
    i_pTextBox->pActiveParagraph->recIndents.lBottom = i_pTextBox->recIndents.lBottom = i_lBottom;
    i_pTextBox->pActiveParagraph->recIndents.lLeft = i_pTextBox->recIndents.lLeft = i_lLeft;
    i_pTextBox->pActiveParagraph->recIndents.lRight = i_pTextBox->recIndents.lRight = i_lRight;
    i_pTextBox->pActiveParagraph->recIndents.lTop = i_pTextBox->recIndents.lTop = i_lTop;
    i_pTextBox->pActiveParagraph->lIndent = i_pTextBox->lIndent = i_lFirstIndent;
    return true_a;
  }

  i_pTextBox->pActiveParagraph->recIndents.lBottom = i_pTextBox->recIndents.lBottom = i_lBottom;
  i_pTextBox->pActiveParagraph->recIndents.lLeft = i_pTextBox->recIndents.lLeft = i_lLeft;
  i_pTextBox->pActiveParagraph->recIndents.lRight = i_pTextBox->recIndents.lRight = i_lRight;
  i_pTextBox->pActiveParagraph->recIndents.lTop = i_pTextBox->recIndents.lTop = i_lTop;
  i_pTextBox->pActiveParagraph->lIndent = i_pTextBox->lIndent = i_lFirstIndent;

  return PDFAdjustParagraph(i_pPDFDoc, i_pTextBox, i_pTextBox->GetTopDrawPos()
                                  - i_pTextBox->lTBHeight + i_pTextBox->pActiveParagraph->lParaHeight - 1, false_a);
}

bool_a PDFGetParaIndents(CPDFTextBox *i_pTextBox, long *o_lLeft, long *o_lTop,
                         long *o_lRight, long *o_lBottom, long *o_lFirstIndent)
{
  *o_lBottom = i_pTextBox->recIndents.lBottom;
  *o_lLeft = i_pTextBox->recIndents.lLeft;
  *o_lRight = i_pTextBox->recIndents.lRight;
  *o_lTop = i_pTextBox->recIndents.lTop;
  *o_lFirstIndent = i_pTextBox->lIndent;
  return true_a;
}

bool_a PDFSetTextBoxBorder(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, float i_fLeft, float i_fTop,
                         float i_fRight, float i_fBottom)
{
  if (!i_pTextBox)
    return false_a;

  i_fBottom = i_fBottom < 0 ? i_pTextBox->recBorder.fBottom : i_fBottom;
  i_fLeft = i_fLeft < 0 ? i_pTextBox->recBorder.fLeft : i_fLeft;
  i_fRight = i_fRight < 0 ? i_pTextBox->recBorder.fRight : i_fRight;
  i_fTop = i_fTop < 0 ? i_pTextBox->recBorder.fTop : i_fTop;

  if (i_pTextBox->recBorder.fBottom == i_fBottom
      && i_pTextBox->recBorder.fLeft == i_fLeft
      && i_pTextBox->recBorder.fRight == i_fRight
      && i_pTextBox->recBorder.fTop == i_fTop)
    return true_a;
  if (i_pTextBox->recBorder.fLeft == i_fLeft
      && i_pTextBox->recBorder.fRight == i_fRight)
  {
    i_pTextBox->recBorder.fBottom = i_fBottom;
    i_pTextBox->recBorder.fTop = i_fTop;
    return true_a;
  }
  i_pTextBox->recBorder.fBottom = i_fBottom;
  i_pTextBox->recBorder.fLeft = i_fLeft;
  i_pTextBox->recBorder.fRight = i_fRight;
  i_pTextBox->recBorder.fTop = i_fTop;

  return PDFAdjustParagraph(i_pPDFDoc, i_pTextBox, i_pTextBox->GetTopDrawPos(), false_a);
}

bool_a PDFGetTextBoxBorder(CPDFTextBox *i_pTextBox, float *o_fLeft, float *o_fTop,
                            float *o_fRight, float *o_fBottom)
{
  *o_fBottom = i_pTextBox->recBorder.fBottom;
  *o_fLeft = i_pTextBox->recBorder.fLeft;
  *o_fRight = i_pTextBox->recBorder.fRight;
  *o_fTop = i_pTextBox->recBorder.fTop;
  return true_a;
}

bool_a PDFSetTextBoxMargins(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, long i_lLeft, long i_lTop,
                         long i_lRight, long i_lBottom)
{
  if (!i_pTextBox)
    return false_a;

  if (i_pTextBox->recMargins.lBottom == i_lBottom
      && i_pTextBox->recMargins.lLeft == i_lLeft
      && i_pTextBox->recMargins.lRight == i_lRight
      && i_pTextBox->recMargins.lTop == i_lTop)
    return true_a;
  if (i_pTextBox->recMargins.lLeft == i_lLeft
      && i_pTextBox->recMargins.lRight == i_lRight)
  {
    i_pTextBox->recMargins.lBottom = i_lBottom;
    i_pTextBox->recMargins.lTop = i_lTop;
    return true_a;
  }
  i_pTextBox->recMargins.lBottom = i_lBottom;
  i_pTextBox->recMargins.lLeft = i_lLeft;
  i_pTextBox->recMargins.lRight = i_lRight;
  i_pTextBox->recMargins.lTop = i_lTop;

  return PDFAdjustParagraph(i_pPDFDoc, i_pTextBox, i_pTextBox->GetTopDrawPos(), false_a);
}

bool_a PDFGetTextBoxMargins(CPDFTextBox *i_pTextBox, long *o_lLeft, long *o_lTop,
                            long *o_lRight, long *o_lBottom)
{
  if (!i_pTextBox)
    return false_a;
  *o_lBottom = i_pTextBox->recMargins.lBottom;
  *o_lLeft = i_pTextBox->recMargins.lLeft;
  *o_lRight = i_pTextBox->recMargins.lRight;
  *o_lTop = i_pTextBox->recMargins.lTop;
  return true_a;
}

bool_a PDFDeleteLastParagraph(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox)
{
  CPDFTextBox    *pTextBox       = i_pTextBox;
  PDFParagraph  *pParagraph     = NULL;
  PDFParagraph  *pParagraphDel  = NULL;

  if (!pTextBox || !(pTextBox->pActiveParagraph))
    return false_a;

  pParagraph = pTextBox->pActiveParagraph;

  if (!(pParagraph->pPDFLastLine) || !(pParagraph->pPDFLastLine->pElemList))
  {
    pParagraphDel = pParagraph->pParaPrev;
    if (!pParagraphDel)
      return false_a;
  }
  else
  {
    pParagraphDel = pParagraph;
    pParagraph = PDFCreateParagraph(i_pPDFDoc, pTextBox);
  }

  pTextBox->lTBHeight -= pParagraphDel->lParaHeight;

  if (pParagraphDel->pParaPrev)
    pParagraphDel->pParaPrev->pParaNext = pParagraph;
  pParagraph->pParaPrev = pParagraphDel->pParaPrev;
  PDFFreeParagraph(&pParagraphDel);

  return true_a;
}


// flush functions

long PDFCreateDuplicityAnnot(PDFDocPTR i_pPDFDoc, long i_lAnnot)
{
  char *pDest = NULL;
  char *pAction = NULL;
  long lRet = NOT_USED;
  if (!i_pPDFDoc || i_lAnnot < 0 || i_lAnnot >= i_pPDFDoc->m_vctAnnots.size())
    return NOT_USED;
  pDest = PDFStrDup(i_pPDFDoc->m_vctAnnots[i_lAnnot]->pDestSpec);
  pAction = PDFStrDup(i_pPDFDoc->m_vctAnnots[i_lAnnot]->pczAction);
  lRet = PDFCreateLinkAnnotation(i_pPDFDoc, i_pPDFDoc->m_vctAnnots[i_lAnnot]->pczAnnotName,
            i_pPDFDoc->m_vctAnnots[i_lAnnot]->lDestPage + 1, pDest, pAction, i_pPDFDoc->m_vctAnnots[i_lAnnot]->m_fZoom);
  if (lRet < 0)
    return lRet;
  i_pPDFDoc->m_vctAnnots[lRet]->recAnnot.lLeft = i_pPDFDoc->m_vctAnnots[i_lAnnot]->recAnnot.lLeft;
  i_pPDFDoc->m_vctAnnots[lRet]->recAnnot.lTop = i_pPDFDoc->m_vctAnnots[i_lAnnot]->recAnnot.lTop;
  i_pPDFDoc->m_vctAnnots[lRet]->recAnnot.lBottom = i_pPDFDoc->m_vctAnnots[i_lAnnot]->recAnnot.lBottom;
  i_pPDFDoc->m_vctAnnots[lRet]->recAnnot.lRight = i_pPDFDoc->m_vctAnnots[i_lAnnot]->recAnnot.lRight;
  return lRet;
}

PDFLineElemList *PDFFlushStringToFile(PDFDocPTR i_pPDFDoc, PDFLineElemList *i_pElemList, long i_lX,
                                      long i_lY, long i_lFirstIndent, CPDFTextBox *i_pTextBox,
                                      PDFParagraph *i_pParagraph, PDFLine *i_pLine, long *i_pAnnot)
{
  CPDFBaseFont *pFont          = NULL;
  CPDFImage       *pImage         = NULL;
  long            lWidth          = 0;
  char            czTemp[201]     = {0};
  long            lFont           = NOT_USED;
  long            lRightPos, lLeftPos, lTemp;
  long lStrWidth = 0; // MIK
  long lAnnotStart = 0, lAnnotWidth = 0;
  PDFLineElemList *pElemList = i_pElemList;
  PDFTabStop      strTabStop;
  long lXSave = i_lX;

  lRightPos = i_pTextBox->GetRightDrawPosInd(i_pParagraph, true_a);
  lLeftPos = i_pTextBox->GetLeftDrawPosInd(i_pParagraph, true_a);

  if (*i_pAnnot >= 0)
  {
    long lRet = PDFCreateDuplicityAnnot(i_pPDFDoc, *i_pAnnot);
    lAnnotStart = i_lX;
    lAnnotWidth = 0;
    PDFAddAnnotToPage(i_pPDFDoc, lRet, i_pPDFDoc->m_lActualPage);
  }


  while(pElemList)
  {
    if (pElemList->lSpace > 0)
    {
      pElemList = pElemList->pLCNext;
      continue;
    }
    if((lFont < 0) || (pElemList->lFont >= 0))
      lFont = pElemList->lFont;
    pFont = PDFGetDocClass(i_pPDFDoc)->GetFont(lFont);
    if (pElemList->lAnnot >= 0)
      PDFAddAnnotToPage(i_pPDFDoc, pElemList->lAnnot, i_pPDFDoc->m_lActualPage);

    PDFAddFontToPage(i_pPDFDoc, lFont, i_pPDFDoc->m_lActualPage);

    if (pElemList->lImage >= 0)
      PDFAddImageToPage(i_pPDFDoc, pElemList->lImage, i_pPDFDoc->m_lActualPage);

    if (pElemList->lImage >= 0)
    {
      pImage = PDFGetDocClass(i_pPDFDoc)->GetImage(pElemList->lImage);
      if (!pImage)
        return NULL;

      /* Write image use line to current Contents stream */
PDFEndText(i_pPDFDoc);

      PDFSaveGraphicsState(i_pPDFDoc);

      PDFTranslateL(i_pPDFDoc, i_lX + lWidth + pElemList->recImageDist.lLeft,
                    i_lY + pElemList->recImageDist.lBottom);
      PDFrotate(i_pPDFDoc, 0);

      PDFConcatL(i_pPDFDoc, PDFConvertDimensionToDefaultL(pElemList->lElemWidth - pElemList->recImageDist.lLeft - pElemList->recImageDist.lRight), 0, 0,
                PDFConvertDimensionToDefaultL(pElemList->lElemHeight - pElemList->recImageDist.lTop - pElemList->recImageDist.lBottom), 0, 0);

      sprintf(czTemp, "/%s %s\n", pImage->name, czOPERATORDO);
      PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
      PDFRestoreGraphicsState(i_pPDFDoc);
PDFBgnText(i_pPDFDoc, 0);
    }
    else
    {
      sprintf(czTemp, "/%s %.5f %s\n", pFont->GetFontResName(), PDFConvertDimensionToDefault(pElemList->fFontSize), czFONTNAMESIZE);
      PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);

      PDFGetDocClass(i_pPDFDoc)->SetTextColor(pElemList->m_Color);
      PDFSetTextMatrixL(i_pPDFDoc, 1, 0, 0, 1, (i_lX + lWidth), i_lY);

      if (pElemList->m_pStr)
        PDFFlushString(i_pPDFDoc, *(pElemList->m_pStr), pFont);
      if (pFont->GetFontStyle() & CPDFBaseFont::ePDFUnderline)
      {
        PDFDrawUnderline(i_pPDFDoc, pFont, pElemList->fFontSize, pElemList->lElemWidth, i_lX + lWidth, i_lY,
                          pElemList->m_Color);
      }
      if (pFont->GetFontStyle() & CPDFBaseFont::ePDFDoubleUnderline)
      {
        PDFDrawDoubleUnderline(i_pPDFDoc, pFont, pElemList->fFontSize, pElemList->lElemWidth, i_lX + lWidth, i_lY,
                                pElemList->m_Color);
      }
      if (pFont->GetFontStyle() & CPDFBaseFont::ePDFStrikeout)
      {
        PDFDrawStrikeout(i_pPDFDoc, pFont, pElemList->fFontSize, pElemList->lElemWidth, i_lX + lWidth, i_lY,
                          pElemList->m_Color);
      }
    }
    if (0 < i_pPDFDoc->fWordSpacing)
    {
      if (pElemList->lFont >= 0 && pElemList->lFont < PDFGetDocClass(i_pPDFDoc)->GetFontsCount())
      {
        CPDFBaseFont *pFont = PDFGetDocClass(i_pPDFDoc)->GetFont(pElemList->lFont);
        if (pFont)
        {
          lStrWidth = pFont->GetFontStringWidth(*(pElemList->m_pStr), pElemList->fFontSize, i_pPDFDoc->lLetterSpace,
                                                i_pPDFDoc->fWordSpacing, i_pPDFDoc->horiz_scaling);
        }
      }
    }
    else
      lStrWidth = pElemList->lElemWidth;

    lWidth += lStrWidth;

    if (pElemList->sTabStop >= 0)
    {
      strTabStop = PDFGetTabStop(i_pParagraph->pParaTabStops, i_pParagraph->sParaTabsCount,
                                  pElemList->sTabStop, i_pParagraph->lParaTabSize);
      lTemp = i_lX + lWidth;
      switch (strTabStop.sTabFlag)
      {
        case TAB_LEFT:
            i_lX = lXSave/*i_pTextBox->GetLeftDrawPos(true_a)*/ + strTabStop.lTabPos;
            lWidth = 0;
          break;
        case TAB_RIGHT:
            i_lX = lXSave/*i_pTextBox->GetLeftDrawPos(true_a)*/ + strTabStop.lTabPos;
            i_lX -= PDFGetWidthAfterTab(pElemList, pElemList->sTabStop);

            lWidth = PDFGetLineWidthToTab(i_pPDFDoc, i_pTextBox, i_pElemList, i_pParagraph,
                                          pElemList->sTabStop);

            if (i_lX < (lLeftPos + i_lFirstIndent + lWidth))
            {
              i_lX = lLeftPos + i_lFirstIndent + lWidth;
            }

            lWidth = 0;
          break;
        case TAB_CENTER:
            i_lX = lXSave/*i_pTextBox->GetLeftDrawPos(true_a)*/ + strTabStop.lTabPos;
            i_lX -= PDFGetWidthAfterTab(pElemList, pElemList->sTabStop) / 2;

            lWidth = PDFGetLineWidthToTab(i_pPDFDoc, i_pTextBox, i_pElemList, i_pParagraph,
                                          pElemList->sTabStop);

            if ((i_lX + PDFGetWidthAfterTab(pElemList, pElemList->sTabStop)) > lRightPos)
            {
              i_lX -= (i_lX + PDFGetWidthAfterTab(pElemList, pElemList->sTabStop)) - lRightPos;
            }
            else
              if ((lLeftPos + i_lFirstIndent + lWidth) > i_lX)
              {
                i_lX = lLeftPos + i_lFirstIndent + lWidth;
              }

            lWidth = 0;
          break;
        case TAB_DECIMAL:
            i_lX = lXSave/*i_pTextBox->GetLeftDrawPos(true_a)*/ + strTabStop.lTabPos;
            i_lX -= PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType);

            lWidth = PDFGetLineWidthToTab(i_pPDFDoc, i_pTextBox, i_pElemList, i_pParagraph,
                                          pElemList->sTabStop);

            if ((i_lX + PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType))
                > lRightPos)
            {
              i_lX -= (i_lX + PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType))
                      - lRightPos;
            }
            else
              if ((lLeftPos + i_lFirstIndent + lWidth) > i_lX)
              {
                i_lX = lLeftPos + i_lFirstIndent + lWidth;
              }

            lWidth = 0;
          break;
      }
      if (pFont->GetFontStyle() & CPDFBaseFont::ePDFUnderline)
      {
        PDFDrawUnderline(i_pPDFDoc, pFont, pElemList->fFontSize, i_lX - lTemp, lTemp, i_lY,
                          pElemList->m_Color);
      }
      if (pFont->GetFontStyle() & CPDFBaseFont::ePDFDoubleUnderline)
      {
        PDFDrawDoubleUnderline(i_pPDFDoc, pFont, pElemList->fFontSize, i_lX - lTemp, lTemp, i_lY,
                                pElemList->m_Color);
      }
      if (pFont->GetFontStyle() & CPDFBaseFont::ePDFStrikeout)
      {
        PDFDrawStrikeout(i_pPDFDoc, pFont, pElemList->fFontSize, i_lX - lTemp, lTemp, i_lY,
                          pElemList->m_Color);
      }
      if (*i_pAnnot >= 0)
      {
        lAnnotWidth += i_lX - lTemp;
      }
    }

    if (pElemList->lAnnot >= 0 && pElemList->lAnnot < i_pPDFDoc->m_vctAnnots.size())
    {
      if (*i_pAnnot >= 0 && *i_pAnnot == pElemList->lAnnot && i_pLine)
      {
        lAnnotWidth += lStrWidth;

        i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lRight =
          i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lLeft =
            lAnnotStart;
        i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lTop =
          i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lBottom =
            i_lY + i_pLine->lCurrLineDesc;
        i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lRight += lAnnotWidth;
        i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lTop += i_pLine->lCurrLineHeight;

        *i_pAnnot = NOT_USED;
      }
      else
      {
        *i_pAnnot = pElemList->lAnnot;
        lAnnotStart = i_lX + lWidth;
        lAnnotWidth = 0;
      }
    }
    else
      if (*i_pAnnot >= 0)
      {
         lAnnotWidth += lStrWidth;
      }

    if (pElemList->bBlockEnd)
    {
      if (*i_pAnnot >= 0 && *i_pAnnot < i_pPDFDoc->m_vctAnnots.size() && i_pLine)
      {
        i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lRight =
          i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lLeft =
            lAnnotStart;
        i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lTop =
          i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lBottom =
            i_lY + i_pLine->lCurrLineDesc;
        i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lRight += lAnnotWidth;
        i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lTop += i_pLine->lCurrLineHeight;
      }
      return pElemList->pLCNext;
    }

    pElemList = pElemList->pLCNext;
  }

  if (*i_pAnnot >= 0 && *i_pAnnot < i_pPDFDoc->m_vctAnnots.size())
  {
    i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lRight =
       i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lLeft =
        lAnnotStart;
      i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lTop =
         i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lBottom =
         i_lY + i_pLine->lCurrLineDesc;
      i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lRight += lAnnotWidth;
      i_pPDFDoc->m_vctAnnots[*i_pAnnot]->recAnnot.lTop += i_pLine->lCurrLineHeight;
  }
  return NULL;
}

void PDFDrawUnderline(PDFDocPTR i_pPDFDoc, CPDFBaseFont *i_pFont, float i_fFontSize, long i_lWidth,
                      long i_lX, long i_lY, CPDFColor& i_rColor)
{
  float fLineWidth;
  float fY;
  if (!i_pFont)
    return;

  fLineWidth = (i_pFont->GetFontUnderlineThickness() / 1000.f) * i_fFontSize;

  if (fLineWidth < 0)
    fLineWidth *= -1;

  if (i_lWidth <= 0)
    return;

  fY = (i_pFont->GetFontUnderlinePosition() / 1000.f) * i_fFontSize;

  PDFEndText(i_pPDFDoc);
  PDFSaveGraphicsState(i_pPDFDoc);
    PDFGetDocClass(i_pPDFDoc)->SetLineColor(i_rColor);
    PDFSetLineWidth(i_pPDFDoc, fLineWidth);
    PDFMoveTo(i_pPDFDoc, (float)i_lX, (float)i_lY + fY - fLineWidth);
    PDFLineTo(i_pPDFDoc, (float)(i_lX + i_lWidth), (float)i_lY + fY - fLineWidth);
  PDFRestoreGraphicsState(i_pPDFDoc);
  PDFBgnText(i_pPDFDoc, 0);
}

void PDFDrawDoubleUnderline(PDFDocPTR i_pPDFDoc, CPDFBaseFont *i_pFont, float i_fFontSize, long i_lWidth,
                            long i_lX, long i_lY, CPDFColor& i_rColor)
{
  float fLineWidth;
  float fY;
  if (!i_pFont)
    return;

  fLineWidth = (i_pFont->GetFontUnderlineThickness() / 1000.f) * i_fFontSize;

  fLineWidth /= 2;
  if (fLineWidth < 0)
    fLineWidth *= -1;

  if (i_lWidth <= 0)
    return;

  fY = (i_pFont->GetFontUnderlinePosition() / 1000.f) * i_fFontSize;

  PDFEndText(i_pPDFDoc);
  PDFSaveGraphicsState(i_pPDFDoc);
    PDFGetDocClass(i_pPDFDoc)->SetLineColor(i_rColor);
    PDFSetLineWidth(i_pPDFDoc, fLineWidth);
    PDFMoveTo(i_pPDFDoc, (float)i_lX, (float)(i_lY + fY) - 2.0f * fLineWidth);
    PDFLineTo(i_pPDFDoc, (float)(i_lX + i_lWidth), (float)(i_lY + fY) - 2.0f * fLineWidth);
//    fY = (i_pFont->ttfont.descender / 1000.f) * i_fFontSize;
    PDFMoveTo(i_pPDFDoc, (float)i_lX, (float)(i_lY + fY) + fLineWidth);
    PDFLineTo(i_pPDFDoc, (float)(i_lX + i_lWidth), i_lY + fY + fLineWidth);
  PDFRestoreGraphicsState(i_pPDFDoc);
  PDFBgnText(i_pPDFDoc, 0);
}

void PDFDrawStrikeout(PDFDocPTR i_pPDFDoc, CPDFBaseFont *i_pFont, float i_fFontSize, long i_lWidth,
                      long i_lX, long i_lY, CPDFColor& i_rColor)
{
  float fLineWidth;
  float fY, fLineHeight;
  if (!i_pFont)
    return;

  fLineWidth = (i_pFont->GetFontUnderlineThickness() / 1000.f) * i_fFontSize;

  if (fLineWidth < 0)
    fLineWidth *= -1;

  if (i_lWidth <= 0)
    return;

  fLineHeight = (float)(((i_pFont->GetFontBBox().lTop - i_pFont->GetFontBBox().lBottom) / 1000.0) / 4) * i_fFontSize;

  fY = (i_pFont->GetFontUnderlinePosition() / 1000.f) * i_fFontSize;

  PDFEndText(i_pPDFDoc);
  PDFSaveGraphicsState(i_pPDFDoc);
    PDFGetDocClass(i_pPDFDoc)->SetLineColor(i_rColor);
    PDFSetLineWidth(i_pPDFDoc, fLineWidth);
    PDFMoveTo(i_pPDFDoc, (float)i_lX, (float)i_lY + fLineHeight + fY);
    PDFLineTo(i_pPDFDoc, (float)(i_lX + i_lWidth), (float)i_lY + fLineHeight + fY);
  PDFRestoreGraphicsState(i_pPDFDoc);
  PDFBgnText(i_pPDFDoc, 0);
}

long PDFGetStartPos(CPDFTextBox *i_pTextBox, PDFParagraph *i_pParagraph,
                      PDFLine *i_pLine, PDFLineElemList *i_pLineElem)
{
  long lRet = 0;

  if (!i_pTextBox || !i_pParagraph || !i_pLine)
    return lRet;
  lRet = i_pTextBox->GetLeftDrawPos(true_a) + i_pParagraph->recIndents.lLeft;
  if (i_pLine == i_pParagraph->pPDFLine && !i_pLineElem)
    lRet += i_pParagraph->lIndent;
  return lRet;
}

long PDFGetBlockWidth(PDFLineElemList *i_pElemList, short sCurrBlock)
{
  long            lWidth      = 0;
  PDFLineElemList *pElemList  = i_pElemList;

  if (sCurrBlock < 0)
    return lWidth;

  while (pElemList)
  {
    lWidth += pElemList->lElemWidth;

    if (pElemList->bBlockEnd)
    {
      sCurrBlock--;
      if (sCurrBlock < 0)
        return lWidth;
      lWidth = 0;
    }

    pElemList = pElemList->pLCNext;
  }

  if (sCurrBlock != 0)
    lWidth = 0;

  return lWidth;
}

long PDFGetLineWidth(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFLineElemList *i_pElemList,
                      long *o_plSpaceWidth, PDFParagraph *i_pParagraph)
{
  long            lWidth      = 0;
  long            lLastWidth  = 0;
  PDFLineElemList *pElemList  = i_pElemList;
  long            lX          = 0;
  long            lRightPos;
  PDFTabStop      strTabStop;

  lRightPos = i_pTextBox->GetRightDrawPosInd(i_pParagraph, true_a);
  *o_plSpaceWidth = 0;
  while (pElemList)
  {
    lWidth += pElemList->lElemWidth;
    if (pElemList->sTabStop >= 0)
    {
      lLastWidth = lX + lWidth;
      strTabStop = PDFGetTabStop(i_pParagraph->pParaTabStops, i_pParagraph->sParaTabsCount,
                                 pElemList->sTabStop, i_pParagraph->lParaTabSize);
      lX = strTabStop.lTabPos - i_pParagraph->recIndents.lLeft;
      if (i_pParagraph->pPDFLine == i_pParagraph->pPDFLastLine)
        lX -= i_pParagraph->lIndent;
      switch (strTabStop.sTabFlag)
      {
        case TAB_LEFT:
          break;
        case TAB_RIGHT:
            lWidth = PDFGetWidthAfterTab(pElemList, pElemList->sTabStop);
            lX -= lWidth;//PDFGetWidthAfterTab(pElemList, pElemList->sTabStop);
            if (lX < lLastWidth)
              lX = lLastWidth;
            *o_plSpaceWidth = lX - lLastWidth;
          break;
        case TAB_CENTER:
            lWidth = PDFGetWidthAfterTab(pElemList, pElemList->sTabStop) / 2;
            lX -= lWidth;//PDFGetWidthAfterTab(pElemList, pElemList->sTabStop) / 2;
            if ((lX + PDFGetWidthAfterTab(pElemList, pElemList->sTabStop)) > lRightPos)
            {
              lX -= (lX + PDFGetWidthAfterTab(pElemList, pElemList->sTabStop)) - lRightPos;
            }
            else
              if (lLastWidth > lX)
                lX = lLastWidth;
            *o_plSpaceWidth = lX - lLastWidth;
          break;
        case TAB_DECIMAL:
            lWidth = PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType);
            lX -= lWidth;//PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop);
            if ((lX + PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType))
                > lRightPos)
            {
              lX -= (lX + PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType))
                      - lRightPos;
            }
            else
              if (lLastWidth > lX)
                lX = lLastWidth;
            *o_plSpaceWidth = lX - lLastWidth;
          break;
      }
      lWidth = 0;
    }
    pElemList = pElemList->pLCNext;
  }
  lLastWidth = lX + lWidth;
  return lLastWidth;
}

long PDFGetLineWidthToTab(PDFDocPTR i_pPDFDoc, CPDFTextBox *i_pTextBox, PDFLineElemList *i_pElemList,
                          PDFParagraph *i_pParagraph, short i_sTab)
{
  long            lWidth      = 0;
  long            lLastWidth  = 0;
  PDFLineElemList *pElemList  = i_pElemList;
  long            lX          = 0;
  long            lRightPos;
  PDFTabStop      strTabStop;

  lRightPos = i_pTextBox->GetRightDrawPosInd(i_pParagraph, true_a);
  while (pElemList)
  {
    lWidth += pElemList->lElemWidth;
    if (pElemList->sTabStop >= 0)
    {
      if (pElemList->sTabStop == i_sTab)
        break;
      lLastWidth = lX + lWidth;
      strTabStop = PDFGetTabStop(i_pParagraph->pParaTabStops, i_pParagraph->sParaTabsCount,
                                  pElemList->sTabStop, i_pParagraph->lParaTabSize);
      lX = strTabStop.lTabPos - i_pTextBox->pActiveParagraph->recIndents.lLeft;
      switch (strTabStop.sTabFlag)
      {
        case TAB_LEFT:
          break;
        case TAB_RIGHT:
            lWidth = PDFGetWidthAfterTab(pElemList, pElemList->sTabStop);
            lX -= lWidth;//PDFGetWidthAfterTab(pElemList, pElemList->sTabStop);
            if (lX < lLastWidth)
              lX = lLastWidth;
          break;
        case TAB_CENTER:
            lWidth = PDFGetWidthAfterTab(pElemList, pElemList->sTabStop) / 2;
            lX -= lWidth;//PDFGetWidthAfterTab(pElemList, pElemList->sTabStop) / 2;
            if ((lX + PDFGetWidthAfterTab(pElemList, pElemList->sTabStop)) > lRightPos)
            {
              lX -= (lX + PDFGetWidthAfterTab(pElemList, pElemList->sTabStop)) - lRightPos;
            }
            else
              if (lLastWidth > lX)
                lX = lLastWidth;
          break;
        case TAB_DECIMAL:
            lWidth = PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType);
            lX -= lWidth;//PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop);
            if ((lX + PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType))
                > lRightPos)
            {
              lX -= (lX + PDFGetDecimalStopWidthAfterTab(i_pPDFDoc, pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType))
                      - lRightPos;
            }
            else
              if (lLastWidth > lX)
                lX = lLastWidth;
          break;
      }
      lWidth = 0;
    }
    pElemList = pElemList->pLCNext;
  }
  lLastWidth = lX + lWidth;
  return lLastWidth;
}

long PDFGetWidthAfterTab(PDFLineElemList *i_pElemList, short i_sTab)
{
  long            lWidth      = 0;
  PDFLineElemList *pElemList  = i_pElemList;
  bool_a          bBreak      = false_a;

  if (i_sTab < 0)
    return lWidth;

  while (pElemList)
  {
    lWidth += pElemList->lElemWidth;

    if (pElemList->sTabStop >= 0 || bBreak)
    {
      if (bBreak && pElemList->sTabStop >= 0)
        return lWidth;
      if (pElemList->sTabStop == i_sTab)
      {
        bBreak = true_a;
        lWidth = 0;
      }
    }

    pElemList = pElemList->pLCNext;
  }

  if (!bBreak)
    lWidth = 0;

  return lWidth;
}

long PDFGetDecimalStopWidthAfterTab(PDFDocPTR i_pPDFDoc, PDFLineElemList *i_pElemList, short i_sTab, char i_cDecimalType)
{
  long            lWidth          = 0;
  size_t          lCount          = 0;
  PDFLineElemList *pElemList      = i_pElemList;
  bool_a          bBreak          = false_a;
  char            *pBuf           = NULL;

  if (i_sTab < 0 || !i_pPDFDoc || !i_pElemList)
    return lWidth;

  while (pElemList)
  {
    if (pElemList->m_pStr)
      lCount += pElemList->m_pStr->Length();

    if (pElemList->sTabStop >= 0 || bBreak)
    {
      if (bBreak)
        break;
      if (pElemList->sTabStop == i_sTab)
      {
        bBreak = true_a;
      }
      lCount = 0;
    }

    pElemList = pElemList->pLCNext;
  }
  if (!lCount)
    return lWidth;

  pBuf = (char*)PDFMalloc((long)(sizeof(char) * lCount) + 1);
  if (!pBuf)
    return 0;

  lCount = 0;
  bBreak = false_a;
  pElemList = i_pElemList;
  while (pElemList)
  {
    if (bBreak && pElemList->m_pStr)
    {
      memcpy(pBuf + lCount, pElemList->m_pStr->GetString(), pElemList->m_pStr->Length());
      lCount += pElemList->m_pStr->Length();
    }

    if (pElemList->sTabStop >= 0 || bBreak)
    {
      if (bBreak)
        break;
      if (pElemList->sTabStop == i_sTab)
        bBreak = true_a;
      memset(pBuf, 0, sizeof(char) * lCount);
      lCount = 0;
    }

    pElemList = pElemList->pLCNext;
  }
  if (!bBreak)
  {
    free(pBuf);
    return lWidth;
  }

  bBreak = false_a;
  for (lCount = 0; lCount < strlen(pBuf); lCount++)
  {
    if (pBuf[lCount] == i_cDecimalType)
      break;
    if (pBuf[lCount] == ' ')
    {
      bBreak = true_a;
    }
    else
    {
      if (bBreak)
        break;
    }
  }

  pElemList = i_pElemList;
  pBuf[lCount] = 0;

  if (pElemList->lFont >= 0 && pElemList->lFont < PDFGetDocClass(i_pPDFDoc)->GetFontsCount())
  {
    CPDFBaseFont *pFont = PDFGetDocClass(i_pPDFDoc)->GetFont(pElemList->lFont);
    if (pFont)
    {
      lWidth = pFont->GetFontStringWidth(pBuf, pElemList->fFontSize);
    }
  }

  free(pBuf);

  return lWidth;
}

long PDFGetBlockSpaces(PDFLineElemList *i_pElemList, short sCurrBlock)
{
  long lCount = 0;
  PDFLineElemList   *pElemList  = i_pElemList;
  CPDFString *pStr = NULL;

  if (sCurrBlock < 0)
    return lCount;

  while (pElemList)
  {
    pStr = pElemList->m_pStr;

    if (pStr)
      lCount += pStr->SpaceCount();

    if (pElemList->bBlockEnd)
    {
      sCurrBlock--;
      if (sCurrBlock < 0)
        return lCount;
      lCount = 0;
    }

    pElemList = pElemList->pLCNext;
  }

  if (sCurrBlock != 0)
    lCount = 1;

  return lCount;
}

long PDFGetCountSpaces(PDFLineElemList *i_pElemList)
{
  long              lCount      = 0;
  PDFLineElemList   *pElemList  = i_pElemList;
  CPDFString *pStrLast = NULL;
  bool_a bDec = false_a;

  while (pElemList)
  {
    pStrLast = pElemList->m_pStr;
    if (!pStrLast)
      continue;
    if (pStrLast->Length() > 0)
      bDec = false_a;
    if (pStrLast->IsLastSpace())
      bDec = true_a;

    lCount += pStrLast->SpaceCount();

    if (pElemList->bBlockEnd)
      break;
    pElemList = pElemList->pLCNext;
  }
  if (bDec)
    lCount--;
  return lCount;
}

bool_a PDFHasLineOnlySpace(PDFLine *i_pLine)
{
  PDFLine *pLine;
  PDFLineElemList *pElem;
  if (!i_pLine)
    return true_a;
  pLine = i_pLine;
  while (pLine)
  {
    pElem = pLine->pElemList;
    while (pElem)
    {
      if (NULL != pElem->m_pStr)
      {
        if (!pElem->m_pStr->HasOnlyWhiteSigns())
          return false_a;
      }else if (pElem->lImage >= 0)
        return false_a;
      pElem = pElem->pLCNext;
    }
    pLine = pLine->pLineNext;
  }
  return true_a;
}










void PDFLineElemList::operator=(const PDFLineElemList& r)
{
  m_Color = r.m_Color;
  m_BgColor = r.m_BgColor;
  lFont = r.lFont;
  fFontSize = r.fFontSize;
  lElemWidth = r.lElemWidth;
  lElemHeight = r.lElemHeight;
  lLetterSpace = r.lLetterSpace;
  lSpace = r.lSpace;
  lImage = r.lImage;
  lAnnot = r.lAnnot;
  lFillBreak = r.lFillBreak;
  recImageDist = r.recImageDist;
  bBlockEnd = r.bBlockEnd;
  bCanPageBreak = r.bCanPageBreak;
  bTBBreak = r.bTBBreak;
  bColumnBreak = r.bColumnBreak;
  m_strAnchor.assign(r.m_strAnchor);
  eElemPageMark = r.eElemPageMark;
  sTabStop = r.sTabStop;
  if (r.m_pStr)
    m_pStr = new CPDFString(*r.m_pStr);
  pLCNext = NULL;
  pLCPrev = NULL;
}

PDFLineElemList::PDFLineElemList()
{
  m_pStr = NULL;
}
PDFLineElemList::~PDFLineElemList()
{
  if (m_pStr)
    delete m_pStr;
}
