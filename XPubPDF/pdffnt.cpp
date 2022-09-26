

#include "pdferr.h"
#include "pdfdef.h"
#include "pdffnc.h"
#include "PDFBaseFont.h"

bool_a PDFAddFontToPage(PDFDocPTR i_pPDFDoc, long i_lFont, long i_lPage)
{
  PDFPage *pPage = NULL;
  if (i_lFont < 0 || i_lFont >= PDFGetDocClass(i_pPDFDoc)->GetFontsCount()
      || i_lPage < 0 || i_lPage >= PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;
  pPage = i_pPDFDoc->m_vctPages[i_lPage];
  if (!pPage)
    return false_a;
  {
    PDFList *pFontP, *pFontL = pPage->pFontIdx;
    bool_a bF = false_a;
    pFontP = pFontL;
    while(pFontL)
    {
      if(pFontL->lValue == i_lFont)
      {
        bF = true_a;
        break;
      }
      pFontP = pFontL;
      pFontL = pFontL->pNext;
    }
    if(!bF)
    {
      pFontL = (PDFList *)PDFMalloc((size_t)sizeof(PDFList));
      if (!pFontL)
        return false_a;
      pFontP->pNext = pFontL;
      pFontP->lValue = i_lFont;
      pFontL->pNext = NULL;
      pFontL->lValue = NOT_USED;
      i_pPDFDoc->m_vctPages[i_lPage]->lFontCount++;
    }
  }
  return true_a;
}

bool_a PDFHasPageFont(PDFDocPTR i_pPDFDoc, long i_lFont, long i_lPage)
{
  PDFPage *pPage = NULL;
  if (i_lFont < 0 || i_lFont >= PDFGetDocClass(i_pPDFDoc)->GetFontsCount()
      || i_lPage < 0 || i_lPage >= PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;
  pPage = i_pPDFDoc->m_vctPages[i_lPage];
  if (!pPage)
    return false_a;

  {
    PDFList *pFontL = pPage->pFontIdx;
    while(pFontL)
    {
      if(pFontL->lValue == i_lFont)
        return true_a;
      pFontL = pFontL->pNext;
    }
  }
  return false_a;
}

bool_a PDFHasFontOnlyOnePage(PDFDocPTR i_pPDFDoc, long i_lFont)
{
  PDFPage *pPage = NULL;
  long lPage;
  bool_a bF = false_a;
  if (i_lFont < 0 || i_lFont >= PDFGetDocClass(i_pPDFDoc)->GetFontsCount())
    return false_a;
  for (lPage = 0; lPage < PDFGetDocClass(i_pPDFDoc)->GetPageCount(); lPage++)
  {
    pPage = i_pPDFDoc->m_vctPages[lPage];
    if (pPage)
    {
      PDFList *pFontL = pPage->pFontIdx;
      while(pFontL)
      {
        if(pFontL->lValue == i_lFont)
        {
          if (bF)
            return false_a;
          else
          {
            bF = true_a;
            break;
          }
        }
        pFontL = pFontL->pNext;
      }
    }
  }
  return true_a;
}
