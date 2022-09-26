
#include "pdferr.h"
#include "pdffnc.h"

bool_a PDFDestSpec(char **o_pBuff, eDESTSPEC i_eDest, long i_lLeft, long i_lTop,
                   long i_lRight, long i_lBottom, float i_fZoom)
{
  char czBuff[200] = {0};

  switch (i_eDest)
  {
    case eDestNull:
      sprintf(czBuff, pczDestSpecification[i_eDest]);
      break;
    case eDestLeftTopZoom:
      sprintf(czBuff, pczDestSpecification[i_eDest], (float)i_lLeft, (float)i_lTop, i_fZoom);
      break;
    case eDestFit:
      sprintf(czBuff, pczDestSpecification[i_eDest]);
      break;
    case eDestFitH:
      sprintf(czBuff, pczDestSpecification[i_eDest], (float)i_lTop);
      break;
    case eDestFitV:
      sprintf(czBuff, pczDestSpecification[i_eDest], (float)i_lLeft);
      break;
    case eDestFitR:
      sprintf(czBuff, pczDestSpecification[i_eDest], (float)i_lLeft, (float)i_lBottom, (float)i_lRight, (float)i_lTop);
      break;
    case eDestFitB:
      sprintf(czBuff, pczDestSpecification[i_eDest]);
      break;
    case eDestFitBH:
      sprintf(czBuff, pczDestSpecification[i_eDest], (float)i_lTop);
      break;
    case eDestFitBV:
      sprintf(czBuff, pczDestSpecification[i_eDest], (float)i_lLeft);
      break;
  }
  *o_pBuff = PDFStrDup(czBuff);
  if (!(*o_pBuff))
    return false_a;
  return true_a;
}

PDFOutlineEntry *PDFAddOutlineEntry(PDFDocPTR i_pPDFDoc, PDFOutlineEntry *i_pFollowThis, bool_a i_bSubEntry,
                                    bool_a i_bOpen, long i_lPage, char *i_pTitle, eDESTSPEC i_eDest, long i_lLeft,
                                    long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                                    float i_fR, float i_fG, float i_fB, short i_sFkey, long i_lFont,
                                    PDFOutlineEntry *i_pOutlineEntry)
{
  PDFOutlineEntry *newOL, *parentptr;
  bool_a bOpenParent = 1;
  if(i_lPage > PDFGetDocClass(i_pPDFDoc)->GetPageCount())
  {
    return NULL;
  }
  if (!i_pOutlineEntry)
  {
    newOL = (PDFOutlineEntry *)PDFMalloc(sizeof(PDFOutlineEntry));
    if (!newOL)
      return NULL;
  }
  else
    newOL = i_pOutlineEntry;
  newOL->lPage = i_lPage - 1;
  newOL->bOpen = i_bOpen;
  newOL->fCkeyR = i_fR;
  newOL->fCkeyG = i_fG;
  newOL->fCkeyB = i_fB;
  newOL->sFkey = i_sFkey;
  newOL->lFont = i_lFont;

  if (!PDFDestSpec(&(newOL->pDestSpec), i_eDest, i_lLeft, i_lTop, i_lRight, i_lBottom, i_fZoom))
    return NULL;

  if(i_pPDFDoc->hex_string)
  {
    /* Outline title is in HEX encoding */
    long length;
    char *tempbuf = (char *)PDFMalloc((long)((strlen(i_pTitle) / 2 + 3)));
    if (!tempbuf)
      return NULL;
    PDFconvertHexToBinary(i_pTitle, (unsigned char *)tempbuf, &length);
    newOL->pTitle = PDFescapeSpecialCharsBinary(tempbuf, length, &(newOL->lTitleLength));
    free(tempbuf);
  }
  else
  {
    /* Outline title is in standard ASCII */
    newOL->pTitle = PDFescapeSpecialChars(i_pTitle);  /* this function allocates storage */
    newOL->lTitleLength = (long) strlen(newOL->pTitle);;
  }

  if(i_bSubEntry && i_pFollowThis != NULL)
  {
    /* Add the new entry at one level below as "i_pFollowThis", i.e., as a sub-section. */
    newOL->pParent = i_pFollowThis;    /* that is our parent */
    newOL->pPrev = NULL;
    newOL->pFirst = NULL;      /* first, last are only needed if kids are added */
    newOL->pLast = NULL;
    newOL->pNext = NULL;
    newOL->lCount = 0;      /* will become non-zero only if kids are added later */
    /* now adding a sub-section requires modification to parent entry */
    if(i_pFollowThis->lCount == 0)
      i_pFollowThis->pFirst = newOL;    /* this is the first kid of i_pFollowThis */
    i_pFollowThis->pLast = newOL;
    /* Increment "lCount" of all open ancestors */
    parentptr = newOL->pParent;      /* initialize to my parent */
    if(parentptr == NULL)
      i_pPDFDoc->lOutlineEntriesCount++;    /* incremeent global outline entry count */
    else
      if(parentptr->bOpen)
        i_pPDFDoc->lOutlineEntriesCount++;    /* incremeent global outline entry count */
    while((parentptr != NULL) && bOpenParent)
    {
      parentptr->lCount++;      /* increment kids count of ancestors */
      bOpenParent = parentptr->bOpen;    /* if parent is open, then continue on */
      parentptr = parentptr->pParent;
    }
  }
  else
  {
    /* Add the new entry at the same level as "i_pFollowThis" */
    if(i_pFollowThis == NULL)
    {
      /* This is the first ever outline entry for this document. */
      newOL->pParent = NULL;    /* eventuall will point to global Outlines object */
      newOL->pPrev = NULL;
      newOL->pFirst = NULL;    /* first, last are only needed if kids are added */
      newOL->pLast = NULL;
      newOL->pNext = NULL;
      newOL->lCount = 0;      /* will become non-zero only if kids are added later */
      /* These 3 are for the global Outlines object */
      i_pPDFDoc->lOutlineEntriesCount++;    /* incremeent global outline entry count */
      i_pPDFDoc->pFirstOutlineEntry = newOL;
      i_pPDFDoc->pLastOutlineEntry = newOL;
    }
    else
    {
      /* We are adding an entry at the same level as "i_pFollowThis" */
      newOL->pParent = i_pFollowThis->pParent;  /* common parent */
      newOL->pPrev = i_pFollowThis;
      newOL->pFirst = NULL;    /* first, last are only needed if kids are added */
      newOL->pLast = NULL;
      newOL->pNext = NULL;
      newOL->lCount = 0;      /* will become non-zero only if kids are added later */
      /* new, set fields of the previous entry */
      i_pFollowThis->pNext = newOL;
      if(i_pFollowThis->pParent)
        i_pFollowThis->pParent->pLast = newOL;

      /* Increment "lCount" of all open ancestors */
      parentptr = newOL->pParent;      /* initialize to my parent */
      if(parentptr == NULL)
        i_pPDFDoc->lOutlineEntriesCount++;    /* incremeent global outline entry count */
      else
        if(parentptr->bOpen)
          i_pPDFDoc->lOutlineEntriesCount++;    /* incremeent global outline entry count */
      while((parentptr != NULL) && bOpenParent)
      {
        parentptr->lCount++;      /* increment kids count of ancestors */
        bOpenParent = parentptr->bOpen;    /* if parent is open, then continue on */
        parentptr = parentptr->pParent;
      }
    }
  }

  if(newOL->pParent == NULL)
    i_pPDFDoc->pLastOutlineEntry = newOL;    /* if our direct parent is the global Outline object */
  return(newOL);
}

