
#include "pdferr.h"
#include "pdfdef.h"
#include "pdffnc.h"
#include "pdfttf.h"

#if defined(WIN32)
#include <windows.h>
#endif

#include "PDFBaseFont.h"

using namespace std;

#define TT_ASSERT(ttf, cond) ((cond) ? (void) 0 : (void) 1)
#define TT_IOCHECK(ttf, cond) ((cond) ? (void) 0 : (void) 1)

void pdf_init_font_struct(pdf_font *font);
static void pdf_cleanup_tt(tt_file *ttf);

static void TTFSeek(tt_file *i_TTF, long i_lOff)
{
  TT_IOCHECK(i_TTF, i_TTF->img + (tt_ulong) i_lOff <= i_TTF->end);
  i_TTF->pos = i_TTF->img + (tt_ulong) i_lOff;
}

static void tt_read(tt_file *ttf, void *buf, long nbytes)
{
  TT_IOCHECK(ttf, ttf->pos + (tt_ulong) nbytes <= ttf->end);
  memcpy(buf, ttf->pos, (size_t) nbytes);
  ttf->pos += (tt_ulong) nbytes;
}

static long tt_tell(tt_file *ttf)
{
  return (long) (ttf->pos - ttf->img);
}

static tt_ushort tt_get_ushort(tt_file *ttf)
{
  tt_byte *pos = ttf->pos;
  TT_IOCHECK(ttf, (ttf->pos += 2) <= ttf->end);
  return (tt_ushort) (((tt_ushort) pos[0] << 8) | pos[1]);
}

static tt_short tt_get_short(tt_file *ttf)
{
  tt_byte *pos = ttf->pos;
  TT_IOCHECK(ttf, (ttf->pos += 2) <= ttf->end);
  return (tt_short) (((tt_short) (tt_char) pos[0] << 8) | pos[1]);
}

static tt_ulong tt_get_ulong(tt_file *ttf)
{
  tt_byte *pos = ttf->pos;
  TT_IOCHECK(ttf, (ttf->pos += 4) <= ttf->end);
  return  ((tt_ulong) pos[0] << 24) |
          ((tt_ulong) pos[1] << 16) |
          ((tt_ulong) pos[2] <<  8) |
          pos[3];
}

static tt_long tt_get_long(tt_file *ttf)
{
  tt_byte *pos = ttf->pos;
  TT_IOCHECK(ttf, (ttf->pos += 4) <= ttf->end);
  return  ((tt_long) (tt_char)   pos[0] << 24) |
          ((tt_long)       pos[1] << 16) |
          ((tt_long)       pos[2] <<  8) |
          pos[3];
}


static void tt_get_dirent(tt_dirent *dirent, tt_file *ttf)
{
  tt_read(ttf, dirent->tag, 4);
  dirent->tag[4] = 0;
  dirent->checksum = tt_get_ulong(ttf);
  dirent->offset = tt_get_ulong(ttf);
  dirent->length = tt_get_ulong(ttf);
} /* tt_get_dirent */


static long tt_tag2idx(tt_file *ttf, char *tag)
{
  long i;

  for (i = 0; i < ttf->n_tables; ++i)
    if (strcmp(ttf->dir[i].tag, tag) == 0)
      return i;

  return -1;
} /* tt_tag2idx */

static void tt_get_cmap0(tt_file *ttf, tt_cmap0_6 *cm0_6)
{
  cm0_6->length = tt_get_ushort(ttf);
  cm0_6->language = tt_get_ushort(ttf);

  /* These are not used in format 0 */
  cm0_6->firstCode  = 0;
  cm0_6->entryCount = 256;

  tt_read(ttf, cm0_6->glyphIdArray, 256);
} /* tt_get_cmap0 */

static void tt_get_cmap6(tt_file *ttf, tt_cmap0_6 *cm0_6)
{
  tt_ushort c, last;

  cm0_6->length = tt_get_ushort(ttf);
  cm0_6->language = tt_get_ushort(ttf);
  cm0_6->firstCode  = tt_get_ushort(ttf);
  cm0_6->entryCount = tt_get_ushort(ttf);

  /* default for codes outside the range specified in this table */
  for (c = 0; c < 256; c++)
    cm0_6->glyphIdArray[c] = 0;

  last = (tt_ushort) (cm0_6->firstCode + cm0_6->entryCount);
  TT_IOCHECK(ttf, last <= 256);

  for (c = cm0_6->firstCode; c < last; c++)
    cm0_6->glyphIdArray[c] = (unsigned char) tt_get_ushort(ttf);

} /* tt_get_cmap6 */

static long tt_code2gidx0_6(tt_file *ttf, long code)
{
  TT_ASSERT(ttf, ttf->tab_cmap != (tt_tab_cmap *) 0);
  TT_ASSERT(ttf, ttf->tab_cmap->mac != (tt_cmap0_6 *) 0);
  TT_ASSERT(ttf, 0 <= code && code <= 255);

  return ttf->tab_cmap->mac->glyphIdArray[code];
} /* tt_code2gidx0_6 */

static void tt_get_cmap4(tt_file *ttf, tt_cmap4 *cm4, tt_ushort encID)
{
  long   i, segs;

  cm4->encodingID = encID;
  /* the instruction order is critical for cleanup after exceptions! */
  cm4->endCount = (tt_ushort *) 0;
  cm4->startCount = (tt_ushort *) 0;
  cm4->idDelta  = (tt_short *)  0;
  cm4->idRangeOffs  = (tt_ushort *) 0;
  cm4->glyphIdArray  = (tt_ushort *) 0;

  cm4->format    = tt_get_ushort(ttf);
  TT_IOCHECK(ttf, cm4->format == 4);
  cm4->length    = tt_get_ushort(ttf);
  cm4->version  = tt_get_ushort(ttf);
  cm4->segCountX2  = tt_get_ushort(ttf);
  cm4->searchRange  = tt_get_ushort(ttf);
  cm4->entrySelector  = tt_get_ushort(ttf);
  cm4->rangeShift  = tt_get_ushort(ttf);

  segs = cm4->segCountX2 / 2;
  cm4->numGlyphIds = (cm4->length - (16 + 8 * segs)) / 2;
  TT_IOCHECK(ttf, 0 <= cm4->numGlyphIds && cm4->numGlyphIds < cm4->length);

  cm4->endCount = (tt_ushort *) malloc((size_t) (sizeof (tt_ushort) * segs));
  if (!(cm4->endCount))
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }
  cm4->startCount = (tt_ushort *) malloc((size_t) (sizeof (tt_ushort) * segs));
  if (!(cm4->startCount))
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }
  cm4->idDelta = (tt_short *)  malloc((size_t) (sizeof (tt_ushort) * segs));
  if (!(cm4->idDelta))
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }
  cm4->idRangeOffs = (tt_ushort *) malloc((size_t) (sizeof (tt_ushort) * segs));
  if (!(cm4->idRangeOffs))
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  if (cm4->numGlyphIds)
  {
    cm4->glyphIdArray = (tt_ushort *) malloc((size_t) (sizeof (tt_ushort) * cm4->numGlyphIds));
    if (!(cm4->glyphIdArray))
    {
      PDFSetLastError(ERR_MALLOC_FAIL);
      return;
    }
  }

  for (i = 0; i < segs; ++i)
    cm4->endCount[i] = tt_get_ushort(ttf);

  TT_IOCHECK(ttf, cm4->endCount[segs - 1] == 0xFFFF);

  (void) tt_get_ushort(ttf);  /* padding */
  for (i = 0; i < segs; ++i)
    cm4->startCount[i] = tt_get_ushort(ttf);
  for (i = 0; i < segs; ++i)
    cm4->idDelta[i] = tt_get_short(ttf);
  for (i = 0; i < segs; ++i)
    cm4->idRangeOffs[i] = tt_get_ushort(ttf);

  for (i = 0; i < cm4->numGlyphIds; ++i)
    cm4->glyphIdArray[i] = tt_get_ushort(ttf);
} /* tt_get_cmap4 */

/* translate character code to glyph index using cmap4.  */

static long tt_code2gidx4(tt_file *ttf, long code)
{
  tt_cmap4 *  cm4;
  long    segs;
  long    i;

  TT_ASSERT(ttf, ttf->tab_cmap != (tt_tab_cmap *) 0);
  TT_ASSERT(ttf, ttf->tab_cmap->win != (tt_cmap4 *) 0);

  cm4 = ttf->tab_cmap->win;
  segs = cm4->segCountX2 / 2;

  for (i = 0; i < segs; ++i)
  {
    if (code <= cm4->endCount[i])
      break;
  }

  TT_IOCHECK(ttf, i != segs);
  if (code < cm4->startCount[i])
    return 0;

  if (cm4->idRangeOffs[i] == 0)
    return (code + cm4->idDelta[i]) & 0xFFFF;
  else
  {
    long idx = cm4->idRangeOffs[i] / 2 + (code - cm4->startCount[i]) - (segs - i);

    TT_IOCHECK(ttf, 0 <= idx && idx < cm4->numGlyphIds);

    if (cm4->glyphIdArray[idx] == 0)
      return 0;
    else
      return (cm4->glyphIdArray[idx] + cm4->idDelta[i]) & 0xFFFF;
  }
} /* tt_code2gidx4 */

static void tt_get_tab_cmap(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_cmap";
  long    idx = tt_tag2idx(ttf, pdf_str_cmap);
  long    i;
  tt_ulong  offset;
  tt_ushort  numEncTabs;
  tt_ushort  tableFormat;
  tt_tab_cmap *tp = (tt_tab_cmap *) malloc(sizeof (tt_tab_cmap));
  if (!tp)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  /* the instruction order is critical for cleanup after exceptions!
  */
  tp->win = (tt_cmap4 *) 0;
  tp->mac = (tt_cmap0_6 *) 0;
  ttf->tab_cmap = tp;

  TT_IOCHECK(ttf, idx != -1);
  offset = ttf->dir[idx].offset;
  TTFSeek(ttf, (long) offset);

  (void) tt_get_ushort(ttf);  /* version */
  numEncTabs = tt_get_ushort(ttf);

  for (i = 0; i < numEncTabs; ++i)
  {
    tt_ushort platformID = tt_get_ushort(ttf);
    tt_ushort encodingID = tt_get_ushort(ttf);
    tt_ulong offsetEncTab = tt_get_ulong(ttf);
    tt_long pos = tt_tell(ttf);

    TTFSeek(ttf, (long) (offset + offsetEncTab));

    if (platformID == tt_pfid_mac && encodingID == 0)
    {
      tableFormat = tt_get_ushort(ttf);

      /* we currently do not support cmaps
      ** other than format 0 and 6 for Macintosh cmaps.
      */

      if (tableFormat == 0 && tp->mac == (tt_cmap0_6 *) 0)
      {
        tp->mac = (tt_cmap0_6 *) malloc(sizeof (tt_cmap0_6));
        if (!(tp->mac))
        {
          PDFSetLastError(ERR_MALLOC_FAIL);
          return;
        }
        tp->mac->format = 0;
        tt_get_cmap0(ttf, tp->mac);
      }
      if (tableFormat == 6 && tp->mac == (tt_cmap0_6 *) 0)
      {
        tp->mac = (tt_cmap0_6 *) malloc(sizeof (tt_cmap0_6));
        if (!(tp->mac))
        {
          PDFSetLastError(ERR_MALLOC_FAIL);
          return;
        }
        tp->mac->format = 6;
        tt_get_cmap6(ttf, tp->mac);
      }
    }
    else
      if (platformID == tt_pfid_win && (encodingID == tt_wenc_symbol || encodingID == tt_wenc_text))
      {
        TT_IOCHECK(ttf, tp->win == (tt_cmap4 *) 0);
        tp->win = (tt_cmap4 *) malloc(sizeof (tt_cmap4));
        if (!(tp->win))
        {
          PDFSetLastError(ERR_MALLOC_FAIL);
          return;
        }
        tt_get_cmap4(ttf, tp->win, encodingID);
      }

    TTFSeek(ttf, pos);
  } /* for */
} /* tt_get_tab_cmap */
 
static void tt_get_tab_head(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_head";
  long    idx = tt_tag2idx(ttf, pdf_str_head);
  tt_tab_head *tp = (tt_tab_head *) malloc(sizeof (tt_tab_head));
  if (!tp)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  ttf->tab_head = tp;
  TT_IOCHECK(ttf, idx != -1);
  TTFSeek(ttf, (long) ttf->dir[idx].offset);

  tp->version      = tt_get_fixed(ttf);
  tp->fontRevision    = tt_get_fixed(ttf);
  tp->checkSumAdjustment  = tt_get_ulong(ttf);
  tp->magicNumber    = tt_get_ulong(ttf);
  tp->flags      = tt_get_ushort(ttf);
  tp->unitsPerEm    = tt_get_ushort(ttf);
  tp->created[1]    = tt_get_ulong(ttf);
  tp->created[0]    = tt_get_ulong(ttf);
  tp->modified[1]    = tt_get_ulong(ttf);
  tp->modified[0]    = tt_get_ulong(ttf);
  tp->xMin      = tt_get_fword(ttf);
  tp->yMin      = tt_get_fword(ttf);
  tp->xMax      = tt_get_fword(ttf);
  tp->yMax      = tt_get_fword(ttf);
  tp->macStyle    = tt_get_ushort(ttf);
  tp->lowestRecPPEM    = tt_get_ushort(ttf);
  tp->fontDirectionHint  = tt_get_short(ttf);
  tp->indexToLocFormat  = tt_get_short(ttf);
  tp->glyphDataFormat    = tt_get_short(ttf);
} /* tt_get_tab_head */

static void tt_get_tab_hhea(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_hhea";
  long    idx = tt_tag2idx(ttf, pdf_str_hhea);
  tt_tab_hhea *tp = (tt_tab_hhea *) malloc(sizeof (tt_tab_hhea));
  if (!tp)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  ttf->tab_hhea = tp;
  TT_IOCHECK(ttf, idx != -1);
  TTFSeek(ttf, (long) ttf->dir[idx].offset);

  tp->version      = tt_get_fixed(ttf);
  tp->ascender    = tt_get_fword(ttf);
  tp->descender    = tt_get_fword(ttf);
  tp->lineGap      = tt_get_fword(ttf);
  tp->advanceWidthMax    = tt_get_ufword(ttf);
  tp->minLeftSideBearing  = tt_get_fword(ttf);
  tp->minRightSideBearing  = tt_get_fword(ttf);
  tp->xMaxExtent    = tt_get_fword(ttf);
  tp->caretSlopeRise    = tt_get_short(ttf);
  tp->caretSlopeRun    = tt_get_short(ttf);
  tp->res1      = tt_get_short(ttf);
  tp->res2      = tt_get_short(ttf);
  tp->res3      = tt_get_short(ttf);
  tp->res4      = tt_get_short(ttf);
  tp->res5      = tt_get_short(ttf);
  tp->metricDataFormat  = tt_get_short(ttf);
  tp->numberOfHMetrics  = tt_get_ushort(ttf);
} /* tt_get_tab_hhea */

static void tt_get_tab_hmtx(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_hmtx";
  long    idx = tt_tag2idx(ttf, pdf_str_hmtx);
  long    n_metrics;
  long    n_lsbs;
  long    i;
  tt_tab_hmtx *tp = (tt_tab_hmtx *) malloc(sizeof (tt_tab_hmtx));
  if (!tp)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  TT_ASSERT(ttf, ttf->tab_hhea != 0);
  TT_ASSERT(ttf, ttf->tab_maxp != 0);

  /* the instruction order is critical for cleanup after exceptions!
  */
  tp->metrics = 0;
  tp->lsbs = 0;
  ttf->tab_hmtx = tp;

  TT_IOCHECK(ttf, idx != -1);
  TTFSeek(ttf, (long) ttf->dir[idx].offset);

  n_metrics = ttf->tab_hhea->numberOfHMetrics;
  n_lsbs = ttf->tab_maxp->numGlyphs - n_metrics;
  TT_IOCHECK(ttf, n_metrics != 0);
  TT_IOCHECK(ttf, n_lsbs >= 0);
  tp->metrics = (tt_metric *) malloc(n_metrics * sizeof (tt_metric));
  if (!(tp->metrics))
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  for (i = 0; i < n_metrics; ++i)
  {
    tp->metrics[i].advanceWidth = tt_get_ufword(ttf);
    tp->metrics[i].lsb = tt_get_fword(ttf);
  }

  if (n_lsbs == 0)
    tp->lsbs = (tt_fword *) 0;
  else
  {
    tp->lsbs = (tt_fword *) malloc(n_lsbs * sizeof (tt_fword));
    if (!(tp->lsbs))
    {
      PDFSetLastError(ERR_MALLOC_FAIL);
      return;
    }
    for (i = 0; i < n_lsbs; ++i)
      tp->lsbs[i] = tt_get_fword(ttf);
  }
} /* tt_get_tab_hmtx */

static void tt_get_tab_CFF_(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_CFF_";
  long    idx = tt_tag2idx(ttf, pdf_str_CFF_);

  if (idx != -1)
  {
    /* CFF table found */
    ttf->tab_CFF_ = (tt_tab_CFF_ *) malloc(sizeof (tt_tab_CFF_));
    if (!(ttf->tab_CFF_))
    {
      PDFSetLastError(ERR_MALLOC_FAIL);
      return;
    }
    ttf->tab_CFF_->offset = ttf->dir[idx].offset;
    ttf->tab_CFF_->length = ttf->dir[idx].length;
  }
} /* tt_get_tab_CFF_ */

static long tt_gidx2width(tt_file *ttf, long gidx)
{
  TT_ASSERT(ttf, ttf->tab_hmtx != (tt_tab_hmtx *) 0);
  TT_ASSERT(ttf, ttf->tab_head != (tt_tab_head *) 0);
  TT_ASSERT(ttf, ttf->tab_hhea != (tt_tab_hhea *) 0);

  {
    long    n_metrics = ttf->tab_hhea->numberOfHMetrics;
    tt_ushort  unitsPerEm = ttf->tab_head->unitsPerEm;

    if (n_metrics <= gidx)
      gidx = n_metrics - 1;

    /*
    return (long) ROUND( (ttf->tab_hmtx->metrics[gidx].advanceWidth * 1000.0f) / unitsPerEm);
    */
    return (long) ((ttf->tab_hmtx->metrics[gidx].advanceWidth * 1000L) / unitsPerEm);
  }
} /* tt_gidx2width */


static void tt_get_tab_maxp(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_maxp";
  long    idx = tt_tag2idx(ttf, pdf_str_maxp);
  tt_tab_maxp *tp = (tt_tab_maxp *) malloc(sizeof (tt_tab_maxp));
  if (!tp)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  ttf->tab_maxp = tp;
  TT_IOCHECK(ttf, idx != -1);
  TTFSeek(ttf, (long) ttf->dir[idx].offset);

  tp->version      = tt_get_fixed(ttf);
  tp->numGlyphs    = tt_get_ushort(ttf);
  tp->maxPoints    = tt_get_ushort(ttf);
  tp->maxContours    = tt_get_ushort(ttf);
  tp->maxCompositePoints  = tt_get_ushort(ttf);
  tp->maxCompositeContours  = tt_get_ushort(ttf);
  tp->maxZones    = tt_get_ushort(ttf);
  tp->maxTwilightPoints  = tt_get_ushort(ttf);
  tp->maxStorage    = tt_get_ushort(ttf);
  tp->maxFunctionDefs    = tt_get_ushort(ttf);
  tp->maxInstructionDefs  = tt_get_ushort(ttf);
  tp->maxStackElements  = tt_get_ushort(ttf);
  tp->maxSizeOfInstructions  = tt_get_ushort(ttf);
  tp->maxComponentElements  = tt_get_ushort(ttf);
  tp->maxComponentDepth  = tt_get_ushort(ttf);
} /* tt_get_tab_maxp */


static void tt_get_tab_name(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_name";
  long    idx = tt_tag2idx(ttf, pdf_str_name);
  long    i;
  tt_ulong  offs;

  tt_tab_name *tp = (tt_tab_name *) malloc(sizeof (tt_tab_name));
  if (!tp)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  ttf->tab_name = tp;
  TT_IOCHECK(ttf, idx != -1);
  TTFSeek(ttf, (long) ttf->dir[idx].offset);

  tp->format = tt_get_ushort(ttf);
  TT_IOCHECK(ttf, tp->format == 0);
  tp->numNameRecords = tt_get_ushort(ttf);
  tp->offsetStrings = tt_get_ushort(ttf);
  offs = ttf->dir[idx].offset + tp->offsetStrings;


  tp->vecNames = (tt_nameref *)malloc(tp->numNameRecords * sizeof(tt_nameref));


  for (i = 0; i < tp->numNameRecords; ++i)
  {
    tp->vecNames[i].platformID = tt_get_ushort(ttf);
    tp->vecNames[i].platformSpecEncID = tt_get_ushort(ttf);
    tp->vecNames[i].languageID = tt_get_ushort(ttf);
    tp->vecNames[i].nameID = tt_get_ushort(ttf);
    tp->vecNames[i].strLength = tt_get_ushort(ttf);
    tp->vecNames[i].strOffset = tt_get_ushort(ttf);
    tp->vecNames[i].strName = 0;

  } /* for i */

  for (i = 0; i < tp->numNameRecords; ++i)
  {
    //fonts with empty name entries actually exist
    if (tp->vecNames[i].strLength == 0)
      continue;

    tp->vecNames[i].strName = (char *) malloc((size_t) tp->vecNames[i].strLength + 1);
    if (!(tp->vecNames[i].strName))
    {
      PDFSetLastError(ERR_MALLOC_FAIL);
      return;
    }

    memset(tp->vecNames[i].strName, 0, (tp->vecNames[i].strLength + 1) * sizeof(char));

    TTFSeek(ttf, (long) (offs + tp->vecNames[i].strOffset));
    tt_read(ttf, tp->vecNames[i].strName, tp->vecNames[i].strLength);

    if (tp->vecNames[i].platformID == tt_pfid_win || tp->vecNames[i].platformID == tt_pfid_apple)
    {
      long k;

      for (k = 0; k < tp->vecNames[i].strLength / 2; ++k)
      tp->vecNames[i].strName[k] = tp->vecNames[i].strName[2*k + 1];
      tp->vecNames[i].strName[k] = 0;
    }
  } /* for i */

  /* If these checks ever fail, we can consider constructing the
  ** missing name 4 from 1+2.
  */
  //TT_IOCHECK(ttf, tp->names[4] != (char *) 0);
} /* tt_get_tab_name */
 
static void tt_get_tab_OS_2(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_OS_2";
  long    idx = tt_tag2idx(ttf, pdf_str_OS_2);
  tt_tab_OS_2 *tp = (tt_tab_OS_2 *) malloc(sizeof (tt_tab_OS_2));
  if (!tp)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  ttf->tab_OS_2 = tp;
  TT_IOCHECK(ttf, idx != -1);
  TTFSeek(ttf, (long) ttf->dir[idx].offset);

  tp->version = tt_get_ushort(ttf);
  tp->xAvgCharWidth = tt_get_short(ttf);
  tp->usWeightClass = tt_get_ushort(ttf);
  tp->usWidthClass = tt_get_ushort(ttf);
  tp->fsType = tt_get_ushort(ttf);
  tp->ySubscriptXSize = tt_get_short(ttf);
  tp->ySubscriptYSize = tt_get_short(ttf);
  tp->ySubscriptXOffset = tt_get_short(ttf);
  tp->ySubscriptYOffset = tt_get_short(ttf);
  tp->ySuperscriptXSize = tt_get_short(ttf);
  tp->ySuperscriptYSize = tt_get_short(ttf);
  tp->ySuperscriptXOffset = tt_get_short(ttf);
  tp->ySuperscriptYOffset = tt_get_short(ttf);
  tp->yStrikeoutSize = tt_get_short(ttf);
  tp->yStrikeoutPosition = tt_get_short(ttf);
  tp->sFamilyClass = tt_get_ushort(ttf);

  tt_read(ttf, tp->panose, 10);

  tp->ulUnicodeRange1 = tt_get_ulong(ttf);
  tp->ulUnicodeRange2 = tt_get_ulong(ttf);
  tp->ulUnicodeRange3 = tt_get_ulong(ttf);
  tp->ulUnicodeRange4 = tt_get_ulong(ttf);

  tt_read(ttf, tp->achVendID, 4);

  tp->fsSelection = tt_get_ushort(ttf);
  tp->usFirstCharIndex = tt_get_ushort(ttf);
  tp->usLastCharIndex = tt_get_ushort(ttf);
  tp->sTypoAscender = tt_get_short(ttf);
  tp->sTypoDescender = tt_get_short(ttf);
  tp->sTypoLineGap = tt_get_short(ttf);
  tp->usWinAscent = tt_get_ushort(ttf);
  tp->usWinDescent = tt_get_ushort(ttf);

  if (tp->version >= 1)
  {
    tp->ulCodePageRange[0] = tt_get_ulong(ttf);
    tp->ulCodePageRange[1] = tt_get_ulong(ttf);
  }
  else
  {
    tp->ulCodePageRange[0] = 0;
    tp->ulCodePageRange[1] = 0;
  }

  if (tp->version >= 2)
  {
    tp->sxHeight = tt_get_short(ttf);
    tp->sCapHeight = tt_get_short(ttf);
    tp->usDefaultChar = tt_get_ushort(ttf);
    tp->usBreakChar = tt_get_ushort(ttf);
    tp->usMaxContext = tt_get_ushort(ttf);
  }
  else
  {
    tp->sxHeight = 0;
    tp->sCapHeight = 0;
    tp->usDefaultChar = 0;
    tp->usBreakChar = 0;
    tp->usMaxContext = 0;
  }
} /* tt_get_tab_OS_2 */

static void tt_get_tab_post(tt_file *ttf)
{
  static const char *fn = "tt_get_tab_post";
  long    idx = tt_tag2idx(ttf, pdf_str_post);
  tt_tab_post *tp = (tt_tab_post *) malloc(sizeof (tt_tab_post));
  if (!tp)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  ttf->tab_post = tp;
  TT_IOCHECK(ttf, idx != -1);
  TTFSeek(ttf, (long) ttf->dir[idx].offset);

  tp->formatType = tt_get_fixed(ttf);
  tp->italicAngle = tt_get_fixed(ttf) / (float) 65536.0;
  tp->underlinePosition = tt_get_fword(ttf);
  tp->underlineThickness = tt_get_fword(ttf);
  tp->isFixedPitch = tt_get_ulong(ttf);
  tp->minMemType42 = tt_get_ulong(ttf);
  tp->maxMemType42 = tt_get_ulong(ttf);
  tp->minMemType1 = tt_get_ulong(ttf);
  tp->maxMemType1 = tt_get_ulong(ttf);
} /* tt_get_tab_post */

void pdf_init_font_struct(pdf_font *font)
{
  font->used_on_current_page  = 1;
  font->type       = Type1;

  font->flags = 0L;
  font->ascender = 800;
  font->descender = -200;
  font->capHeight = 700;
  font->xHeight = 0;
  font->isFixedPitch = 0;
  font->italicAngle = 0;
  font->llx = -50;
  font->lly = -200;
  font->urx = 1000;
  font->ury = 900;
  font->StdHW = 0;
  font->StdVW = 0;
  font->underlinePosition = -100;
  font->underlineThickness = 50;

  font->pFontName = NULL;
  font->ttname = NULL;
  font->encodingScheme = NULL;
  font->encoding = builtin;

  font->numOfChars = 0;

  font->filelen = 0L;
  font->img = NULL;
}

static void pdf_cleanup_tt(tt_file *ttf)
{
  if (ttf->fp != (FILE *) 0)
    fclose(ttf->fp);

  if (ttf->dir != (tt_dirent *) 0)
  {
    free(ttf->dir);
    ttf->dir = NULL;
  }

  if (ttf->tab_head != (tt_tab_head *) 0)
  {
    free(ttf->tab_head);
    ttf->tab_head = NULL;
  }
  if (ttf->tab_hhea != (tt_tab_hhea *) 0)
  {
    free(ttf->tab_hhea);
    ttf->tab_hhea = NULL;
  }
  if (ttf->tab_maxp != (tt_tab_maxp *) 0)
  {
    free(ttf->tab_maxp);
    ttf->tab_maxp = NULL;
  }
  if (ttf->tab_OS_2 != (tt_tab_OS_2 *) 0)
  {
    free(ttf->tab_OS_2);
    ttf->tab_OS_2 = NULL;
  }
  if (ttf->tab_CFF_ != (tt_tab_CFF_ *) 0)
  {
    free(ttf->tab_CFF_);
    ttf->tab_CFF_ = NULL;
  }
  if (ttf->tab_post != (tt_tab_post *) 0)
  {
    free(ttf->tab_post);
    ttf->tab_post = NULL;
  }

  if (ttf->tab_cmap != (tt_tab_cmap *) 0)
  {
    if (ttf->tab_cmap->mac != (tt_cmap0_6 *) 0)
    {
      free(ttf->tab_cmap->mac);
      ttf->tab_cmap->mac = NULL;
    }

    if (ttf->tab_cmap->win != (tt_cmap4 *) 0)
    {
      tt_cmap4 *cm4 = (tt_cmap4 *) ttf->tab_cmap->win;

      if (cm4->endCount != 0)
      {
        free(cm4->endCount);
        cm4->endCount = NULL;
      }
      if (cm4->startCount != 0)
      {
        free(cm4->startCount);
        cm4->startCount = NULL;
      }
      if (cm4->idDelta != 0)
      {
        free(cm4->idDelta);
        cm4->idDelta = NULL;
      }
      if (cm4->idRangeOffs != 0)
      {
        free(cm4->idRangeOffs);
        cm4->idRangeOffs = NULL;
      }
      if (cm4->glyphIdArray != 0)
      {
        free(cm4->glyphIdArray);
        cm4->glyphIdArray = NULL;
      }

      free(cm4);
      ttf->tab_cmap->win = NULL;
    }

    free(ttf->tab_cmap);
    ttf->tab_cmap = NULL;
  }

  if (ttf->tab_hmtx != (tt_tab_hmtx *) 0)
  {
    if (ttf->tab_hmtx->metrics != (tt_metric *) 0)
    {
      free(ttf->tab_hmtx->metrics);
      ttf->tab_hmtx->metrics = NULL;
    }
    if (ttf->tab_hmtx->lsbs != (tt_fword *) 0)
    {
      free(ttf->tab_hmtx->lsbs);
      ttf->tab_hmtx->lsbs = NULL;
    }
    free(ttf->tab_hmtx);
    ttf->tab_hmtx = NULL;
  }

  if (ttf->tab_name != (tt_tab_name *) 0)
  {
    long i;

    for (i = 0; i < ttf->tab_name->numNameRecords; ++i)
    {
      if (ttf->tab_name->vecNames[i].strName)
        free(ttf->tab_name->vecNames[i].strName);
    }

    free(ttf->tab_name->vecNames);
    //for (i = 0; i < TT_NUM_NAMES; ++i)
    //{
    //  if (ttf->tab_name->names[i] != (char *) 0)
    //  {
    //    free(ttf->tab_name->names[i]);
    //    ttf->tab_name->names[i] = NULL;
    //  }
    //}
    free(ttf->tab_name);
    ttf->tab_name = NULL;
  }

  /* Note: do not clean up ttf->img since the data belongs to font->img
  */

  free(ttf);

} /* pdf_cleanup_tt */

static void pdf_init_tt(tt_file *ttf)
{
  long i;

  ttf->dir = (tt_dirent *) 0;

  ttf->tab_cmap = (tt_tab_cmap *) 0;
  ttf->tab_head = (tt_tab_head *) 0;
  ttf->tab_hhea = (tt_tab_hhea *) 0;
  ttf->tab_hmtx = (tt_tab_hmtx *) 0;
  ttf->tab_maxp = (tt_tab_maxp *) 0;
  ttf->tab_name = (tt_tab_name *) 0;
  ttf->tab_post = (tt_tab_post *) 0;
  ttf->tab_OS_2 = (tt_tab_OS_2 *) 0;
  ttf->tab_CFF_ = (tt_tab_CFF_ *) 0;

  (void) tt_get_ulong(ttf);    /* version */

  /* set up table directory */
  ttf->n_tables = tt_get_ushort(ttf);
  ttf->dir = (tt_dirent *) malloc((size_t) (ttf->n_tables * sizeof (tt_dirent)));
  if (!(ttf->dir))
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return;
  }

  TTFSeek(ttf, 12);

  for (i = 0; i < ttf->n_tables; ++i)
    tt_get_dirent(ttf->dir + i, ttf);

  /* These are all required TrueType tables; optional tables are not read.
  **
  ** There's no error return; pdf_cleanup_tt() is called by all subroutines
  ** before exceptions are thrown. LATER: It is a known bug that p->malloc()
  ** exceptions can't be caught and will lead to resource leaks in this
  ** context!
  */
  tt_get_tab_cmap(ttf);
  tt_get_tab_head(ttf);
  tt_get_tab_hhea(ttf);
  tt_get_tab_maxp(ttf);
  tt_get_tab_hmtx(ttf);  /* MUST be read AFTER hhea & maxp! */
  tt_get_tab_name(ttf);
  tt_get_tab_post(ttf);
  tt_get_tab_OS_2(ttf);

  /* this is an optional table, present only in OpenType fonts */
  tt_get_tab_CFF_(ttf);
} /* pdf_init_tt */



#if defined(LINUX_OS) || defined(MAC_OS_MWE) || defined(__LINUX__)
static bool_a pdf_get_hostfont(const char *fontname, pdf_font *font, long lFontFlags)
{
  return 0;
}
#elif defined(WIN32)
static bool_a pdf_get_hostfont(const char *fontname, pdf_font *font, long lFontFlags)
{
  LOGFONT lf;
  HFONT   hf = NULL;
  HDC     hdc = NULL;

  lf.lfHeight = 0;
  lf.lfWidth = 0;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;
  if (lFontFlags & CPDFBaseFont::ePDFBold)
    lf.lfWeight = FW_BOLD;
  else
    lf.lfWeight = 0;
  if (lFontFlags & CPDFBaseFont::ePDFItalic)
    lf.lfItalic = 1;
  else
    lf.lfItalic = 0;
  if (lFontFlags & CPDFBaseFont::ePDFUnderline)
    lf.lfUnderline = 1;
  else
    lf.lfUnderline = 0;
  if (lFontFlags & CPDFBaseFont::ePDFStrikeout)
    lf.lfStrikeOut = 1;
  else
    lf.lfStrikeOut = 0;
  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
  lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lf.lfQuality = DEFAULT_QUALITY;
  lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

  if (strlen(fontname) >= 32)
  {
    return 0;
  }

  strcpy((char *)lf.lfFaceName, fontname);

  if ((hf = CreateFontIndirect(&lf)) == NULL)
    return 0;

  if ((hdc = GetDC(0)) == NULL)
  {
    DeleteObject(hf);
    return 0;
  }

  if (!SelectObject(hdc, hf))
  {
    DeleteObject(hf);
    return 0;
  }

  font->filelen = GetFontData(hdc, 0, 0, 0, 0);

  if (font->filelen == GDI_ERROR)
  {
    ReleaseDC(0, hdc);
    DeleteObject(hf);
    return 0;
  }

  font->img = (tt_byte *) malloc((size_t) font->filelen);
  if (!(font->img))
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return false_a;
  }



  if (GetFontData(hdc, 0, 0, font->img, (DWORD)font->filelen) == GDI_ERROR)
  {
    free(font->img);
    ReleaseDC(0, hdc);
    DeleteObject(hf);
    return 0;
  }

//free(font->img);
//
//FILE *pPdfFile = fopen("D:\\PDF\\xpdf-3.00_test\\xpdfdll\\_tempTTFont", "rb");
//
//fseek(pPdfFile, 0, SEEK_END);
//font->filelen = ftell(pPdfFile);
//fseek(pPdfFile, 0, SEEK_SET);
//font->img = (unsigned char *)malloc(sizeof(unsigned char) * font->filelen);
//if (!font->img)
//  return 0;
//fread( font->img, sizeof( unsigned char ), font->filelen, pPdfFile );
//
//fclose(pPdfFile);

  ReleaseDC(0, hdc);
  DeleteObject(hf);

  if (font->filelen >= 10)  /* we need 10 bytes for plausibility checks */
  {
    /* The TrueType (including OpenType/TT) "version" is always 0x00010000
    */
    if (font->img[0] == 0x00 && font->img[1] == 0x01 &&
          font->img[2] == 0x00 && font->img[3] == 0x00)
      return 1;

    /* The OpenType/CFF version is always 'OTTO' */
    if (font->img[0] == 'O' && font->img[1] == 'T' && font->img[2] == 'T' && font->img[3] == 'O')
      return 1;
  }

  free(font->img);
  return 0;
}
#else
#error "not implemented"
#endif


void pdf_make_fontflag(pdf_font *font)
{
  if (font->type != Type1 && font->type != MMType1 &&
        font->type != TrueType && font->type != Type1C)
    return;

  if (font->isFixedPitch)
    font->flags |= FIXEDWIDTH;

  if (!strcmp(font->encodingScheme, "AdobeStandardEncoding"))
    font->flags |= ADOBESTANDARD;
  else
    font->flags |= SYMBOL;

  if (font->italicAngle < 0)
    font->flags |= ITALIC;

  /* heuristic to identify (small) caps fonts */
  if (font->pFontName &&  (strstr(font->pFontName, "Caps") ||
      !strcmp(font->pFontName + strlen(font->pFontName) - 2, "SC")))
    font->flags |= SMALLCAPS;

  if (strstr(font->pFontName, "Bold") || font->StdVW > PDF_STEMV_SEMIBOLD)
    font->flags |= FORCEBOLD;
}

bool_a pdf_parse_tt(const char *fontname, pdf_font *font, long enc, long lFontFlags)
{
  long    i;
  float  w;
  tt_cmap4  *win;
  tt_cmap0_6  *mac;
  tt_file  *ttf;

  ttf = (tt_file *) malloc((size_t) sizeof (tt_file)); /* LEAK */
  if (!ttf)
  {
    PDFSetLastError(ERR_MALLOC_FAIL);
    return false_a;
  }
  ttf->img = 0;
  ttf->fp = 0;
  ttf->filename = NULL;

  pdf_init_font_struct(font);  /* initialize font struct */

  if (!pdf_get_hostfont(fontname, font, lFontFlags))
  {
    free(ttf);
    return 0;
  }
  ttf->img = font->img;
  ttf->pos = ttf->img;
  ttf->end = ttf->img + font->filelen;

  pdf_init_tt(ttf);    /* read font file */

  if (ttf->tab_CFF_)
  {
    font->type = Type1C;
    font->cff_offset = (long) ttf->tab_CFF_->offset;
    font->cff_length = ttf->tab_CFF_->length;
  }
  else
  {
    font->type = TrueType;
  }

  /* check if all the tables required by Acrobat are contained in the font.
  */
  /*  TT_IOCHECK(ttf, tt_tag2idx(ttf, pdf_str_cvt_) != -1);  c0d0gb12 */
  /*  TT_IOCHECK(ttf, tt_tag2idx(ttf, pdf_str_fpgm) != -1);  c0d0gb12 */
  /*  TT_IOCHECK(ttf, tt_tag2idx(ttf, pdf_str_prep) != -1);  8940hg-symbol */

  /* These tables are not present in OT fonts */
  if (font->type != Type1C)
  {
    TT_IOCHECK(ttf, tt_tag2idx(ttf, pdf_str_glyf) != -1);
    TT_IOCHECK(ttf, tt_tag2idx(ttf, pdf_str_loca) != -1);
  }

  font->numOfChars  = 256;

  /* check whether our font is compatible with the requested encoding */
  win = ttf->tab_cmap->win;
  mac = ttf->tab_cmap->mac;

  if (win == 0 && mac == 0)
  {
    pdf_cleanup_tt(ttf);
    return 0;
  }

  switch (enc)
  {
    case builtin:
      if (win != 0)
      {
        if (win->encodingID == tt_wenc_symbol)
        {
          font->flags |= SYMBOL;
          break;
        }
        else
          if (win->encodingID == tt_wenc_text)
          {
            enc = winansi;
            break;
          }
      }
      else
        if (mac != 0)
        {
          enc = macroman;
          break;
        }
        else
        {
          pdf_cleanup_tt(ttf);
          return 0;
        }
      break;
    case winansi:
      if (win == 0)
      {
        pdf_cleanup_tt(ttf);
        return 0;
      }

      if (win->encodingID == tt_wenc_symbol)
      {
        font->flags |= SYMBOL;
        enc = builtin;
      }
      break;
    case macroman:
      if (mac == 0)
      {
        pdf_cleanup_tt(ttf);
        return 0;
      }
      break;
    default:
      /* construct a Unicode-compatible encoding later */
      if (win == 0)
      {
        pdf_cleanup_tt(ttf);
        return 0;
      }
      break;
  } /* switch (enc) */

  font->encoding  = enc;

  /* The name which is exposed in Windows/Mac OS and at the PDFlib API... */

  font->pFontName  = PDFStrDup(ttf->tab_name->vecNames[4].strName);

  if (ttf->tab_name->vecNames[6].strName == (char *) 0)
    font->ttname  = PDFStrDup(ttf->tab_name->vecNames[4].strName);
  else
    font->ttname  = PDFStrDup(ttf->tab_name->vecNames[6].strName);

  w = ttf->tab_OS_2->usWeightClass / (float) 65;
  font->StdVW    = (long) (PDF_STEMV_MIN + w * w);

  font->italicAngle  = (float) ttf->tab_post->italicAngle;
  font->isFixedPitch  = (bool_a) ttf->tab_post->isFixedPitch;

  #define PDF_TT2PDF(v)  (long) ROUND(v * 1000.0f / ttf->tab_head->unitsPerEm)

  font->llx = PDF_TT2PDF(ttf->tab_head->xMin);
  font->lly = PDF_TT2PDF(ttf->tab_head->yMin);
  font->urx = PDF_TT2PDF(ttf->tab_head->xMax);
  font->ury = PDF_TT2PDF(ttf->tab_head->yMax);

  font->underlinePosition  = PDF_TT2PDF(ttf->tab_post->underlinePosition);
  font->underlineThickness  = PDF_TT2PDF(ttf->tab_post->underlineThickness);

  if (ttf->tab_OS_2->sCapHeight)
  {
    font->capHeight  = PDF_TT2PDF(ttf->tab_OS_2->sCapHeight);
  }
  else
  {
    font->capHeight  = 666;  /* an educated guess */
  }

//  font->xHeight  = PDF_TT2PDF(ttf->tab_OS_2->sxHeight);
  font->ascender  = PDF_TT2PDF(ttf->tab_OS_2->sTypoAscender);
  font->descender  = PDF_TT2PDF(ttf->tab_OS_2->sTypoDescender);

  font->ascender  = PDF_TT2PDF(ttf->tab_OS_2->usWinAscent);
  font->descender  = PDF_TT2PDF(ttf->tab_OS_2->usWinDescent);

  font->ascender  = PDF_TT2PDF(ttf->tab_hhea->ascender);
  font->descender  = PDF_TT2PDF(ttf->tab_hhea->descender);

  if (ttf->tab_cmap->win && ttf->tab_cmap->win->encodingID == tt_wenc_text)
    font->encodingScheme = PDFStrDup("AdobeStandardEncoding");
  else
    font->encodingScheme = PDFStrDup("FontSpecific");

  /*
  * If we have a PostScript encoding with no Unicode table attached,
  * convert it now. The Unicode table will survive for use with
  * another font later. Do this only for table-driven encodings,
  * i.e., not builtin.
  */
  //if (enc >= 0 && !p->encodings[enc]->codes && !(font->flags & SYMBOL))
  //{
  //  p->encodings[enc]->codes = (unsigned short *) malloc(256 * sizeof (unsigned short));

  //  for (i = 0; i < 256; i++)
  //  {
  //      char *name = p->encodings[enc]->chars[i];
  //      p->encodings[enc]->codes[i] = (unsigned short) pdf_adobe2unicode(name);
  //  }
  //}

  for (i = 0; i < 256; i++)
  {
    long gidx, uv;

    uv = font->uchar[i];

    if (enc == macroman)
      gidx = tt_code2gidx0_6(ttf, uv);
    else
      if (font->flags & SYMBOL)
      {
        /* e.g. WingDings has usFirstChar = 0xF020, but we must start
        ** at 0xF000. I haven't seen non-symbol fonts with a usFirstChar
        ** like that; perhaps we have to apply similar tricks then...
        */
        //uv = i + (ttf->tab_OS_2->usFirstCharIndex & 0xFF00);
        gidx = tt_code2gidx4(ttf, uv);
      }
      else
      {
        /* fetch the Unicode value from the encoding table */
        //uv = i + (ttf->tab_OS_2->usFirstCharIndex & 0xFF00);
        gidx = tt_code2gidx4(ttf, uv);
        //          uv = (long) p->encodings[enc]->codes[i];
        //          gidx = tt_code2gidx4(ttf, uv);
      }
    /* if there's no glyph for this code
    ** this is the width of the "missingGlyph".
    */

    font->lGdixs[i] = gidx;
    font->widths[i] = (short) tt_gidx2width(ttf, gidx);
  }

  for (i = 0; i < lADOBE_GLYPH_LIST_COUNT; i++)
  {
    long gidx, uv;

    uv = PDFConvertHEXToLong(pczADOBE_GLYPH_LIST[i][1]);

    if (enc == macroman)
      gidx = tt_code2gidx0_6(ttf, uv);
    else
      if (font->flags & SYMBOL)
      {
        gidx = tt_code2gidx4(ttf, uv);
      }
      else
      {
        gidx = tt_code2gidx4(ttf, uv);
      }


    font->m_pmapUnicodeGidx->insert(pair <long, long>(uv, gidx));
    font->m_pmapUnicodeWidth->insert(pair<long, long>(uv, tt_gidx2width(ttf, gidx)));
  }


  /* LATER: raise an exception if no character was found
  if (enc != macroman && enc != winansi && no_character_found) {
  pdf_cleanup_tt(ttf);
  if (p->debug['F']) {
  pdf_error(PDF_RuntimeError,
  "Font '%s' can't be used with encoding '%s'",
  font->ttname, p->encodings[enc].apiname);
  } else
  return 0;
  }
  */

  pdf_cleanup_tt(ttf);

  return 1;
}

