
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "pdferr.h"
#include "pdfimg.h"
#include "pdffnc.h"
#include "png.h"

#include <XPictureInfo.h>

#if !defined(MAC_OS_MWE)
using namespace Magick;
#endif

bool_a PDFAddImage(PDFDocPTR i_pPDFDoc, const char *i_pczImageName,
                   long i_lX, long i_lY, long i_lAngle,
                   float i_fXScale, float i_fYScale)
{
  float a, b = 0, c = 0, d;
  CPDFImage *pImage = NULL;
  char czTemp[201] = {0};

  pImage = PDFGetDocClass(i_pPDFDoc)->CreateImage(i_pczImageName);
  if (!pImage)
    return false_a;

  if (i_fXScale > 0)
  {
    a = i_fXScale * (pImage->width / (pImage->Xdimension / DEFPOINTSPERINCH));
  }
  else
    a = (float)pImage->width;
  if (i_fYScale > 0)
  {
    d = i_fYScale * (pImage->height / (pImage->Ydimension / DEFPOINTSPERINCH));
  }
  else
    d = (float)pImage->height;

  /* Write image use line to current Contents stream */
  PDFSaveGraphicsState(i_pPDFDoc);
  PDFTranslateL(i_pPDFDoc, i_lX, i_lY);
  PDFrotate(i_pPDFDoc, i_lAngle);

  PDFConcat(i_pPDFDoc, PDFConvertDimensionToDefault(a), PDFConvertDimensionToDefault(b),
            PDFConvertDimensionToDefault(c), PDFConvertDimensionToDefault(d), 0.0, 0.0);

  sprintf(czTemp, "/%s %s\n", pImage->name, czOPERATORDO);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
  PDFRestoreGraphicsState(i_pPDFDoc);
  return(0);
}

bool_a PDFAddImageWH(PDFDocPTR i_pPDFDoc, const char *i_pczImageName,
                    long i_lX, long i_lY, long i_lAngle,
                    long i_lWidth, long i_lHeight)
{
  long a, b = 0, c = 0, d;
  CPDFImage *pImage = NULL;
  char czTemp[201] = {0};

  pImage = PDFGetDocClass(i_pPDFDoc)->CreateImage(i_pczImageName);
  if (!pImage)
    return false_a;

  if (i_lWidth > 0)
  {
    a = i_lWidth;
  }
  else
    a = pImage->width;
  if (i_lHeight)
  {
    d = i_lHeight;
  }
  else
    d = pImage->height;

  /* Write image use line to current Contents stream */
  PDFSaveGraphicsState(i_pPDFDoc);
  PDFTranslateL(i_pPDFDoc, i_lX, i_lY);
  PDFrotate(i_pPDFDoc, i_lAngle);

  PDFConcat(i_pPDFDoc, PDFConvertDimensionToDefault((float)a), PDFConvertDimensionToDefault((float)b),
            PDFConvertDimensionToDefault((float)c), PDFConvertDimensionToDefault((float)d), 0.0, 0.0);

  sprintf(czTemp, "/%s %s\n", pImage->name, czOPERATORDO);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
  PDFRestoreGraphicsState(i_pPDFDoc);
  return(0);
}

bool_a PDFAddImageWHIndex(PDFDocPTR i_pPDFDoc, long i_lIndex,
                          long i_lX, long i_lY, long i_lAngle,
                          long i_lWidth, long i_lHeight)
{
  long a, b = 0, c = 0, d;
  CPDFImage *pImage = NULL;
  char czTemp[201] = {0};

  pImage = PDFGetDocClass(i_pPDFDoc)->GetImage(i_lIndex);
  if (!pImage)
    return false_a;

  PDFAddImageToPage(i_pPDFDoc, i_lIndex, i_pPDFDoc->m_lActualPage);

  if (i_lWidth > 0)
  {
    a = i_lWidth;
  }
  else
    a = pImage->width;
  if (i_lHeight)
  {
    d = i_lHeight;
  }
  else
    d = pImage->height;

  /* Write image use line to current Contents stream */
  PDFSaveGraphicsState(i_pPDFDoc);
  PDFTranslateL(i_pPDFDoc, i_lX, i_lY);
  PDFrotate(i_pPDFDoc, i_lAngle);

  PDFConcat(i_pPDFDoc, PDFConvertDimensionToDefault((float)a), PDFConvertDimensionToDefault((float)b),
            PDFConvertDimensionToDefault((float)c), PDFConvertDimensionToDefault((float)d), 0.0, 0.0);

  sprintf(czTemp, "/%s %s\n", pImage->name, czOPERATORDO);
  PDFWriteToPageStream(i_pPDFDoc, czTemp, strlen(czTemp), i_pPDFDoc->m_lActualPage);
  PDFRestoreGraphicsState(i_pPDFDoc);
  return(0);
}

bool_a PDFReadImage(PDFDocPTR i_pPDFDoc, const char *i_pczImageName, long *o_pIndex)
{
  int type = 0;
  int retcode=0;
  int imageIndexFound = 0;
  int currentImageIndex = 0;
  CPDFImage *newImage;
  char imagename[32], *image;
  FILE *fpimg = NULL;
  size_t size;
  char *pSaveSave = (char *)i_pczImageName;

  size = strlen(i_pczImageName);
  if (i_pczImageName[size-1] == 'g' &&
      i_pczImageName[size-2] == 'n' &&
      i_pczImageName[size-3] == 'p')
  {
    type = PNG_IMG;
  }

  if( _isNewImage(i_pPDFDoc, i_pczImageName, &imageIndexFound) )
  {
    newImage = new CPDFImage(PDFGetDocClass(i_pPDFDoc));
    if (!newImage)
      return false_a;
    newImage->imagemask = 0;
    newImage->invert = 0;
    newImage->photometric = 1;  /* min is black - tiff */
    newImage->orientation = 1;  /* normal orientation - tiff */
    newImage->bUsePNG = false_a;
    newImage->filesize = getFileSize(i_pczImageName);  /* used only for JPEG, overwritten for others */
    newImage->data = NULL;
    currentImageIndex = PDFGetDocClass(i_pPDFDoc)->GetImagesCount();
    /* Process different image file formats here */
    switch(type)
    {
      case JPEG_IMG:

        newImage->data = (unsigned char *)PDFMalloc((newImage->filesize+16));
        if (!newImage->data)
          return false_a;
        if((fpimg = fopen(i_pczImageName, BINARY_READ)) != NULL)
        {
          fread(newImage->data, 1, (size_t)(newImage->filesize), fpimg);
          fclose(fpimg);
        }
        else
          return false_a;

        //retcode = read_JPEG_header(i_pczImageName, newImage);
        //if(retcode)
        //{
        //  if(retcode == -1)
        //  {
        //    return false_a;
        //  }
        //  if(retcode == -2)
        //  {
        //    return false_a;
        //  }
        //  if( newImage->process != M_SOF0 )
        //  {
        //    PDFSetVersion(i_pPDFDoc, 1, 3); /* PDF-1.3 */
        //  }
        //}

        try
        {
          CXPubPictureInfo cPicInfo(i_pczImageName);

          newImage->width = cPicInfo.baseColumns();
          newImage->height = cPicInfo.baseRows();
          
          //UNITS ???
          newImage->Xdimension = (int)cPicInfo.xResolution();
          if (newImage->Xdimension == 0)
            newImage->Xdimension = 96;//TODO: it is not correct - get corect value
          newImage->Ydimension = (int)cPicInfo.yResolution();
          if (newImage->Ydimension == 0)
            newImage->Ydimension = 96;//TODO: it is not correct - get corect value

          newImage->bitspersample = cPicInfo.depth();

          switch (cPicInfo.colorSpace())
          {
          case CMYKColorspace:
            newImage->ncomponents = 4;
            break;
          case RGBColorspace:
            newImage->ncomponents = 3;
            break;
          default:
            newImage->ncomponents = 1;
          }
        }
        catch (Exception &error_)
        {
          cout << error_.what() << endl;
          return false_a;
        }
        catch (...)
        {
          return false_a;
          //const char *pWhat = e.what();
          //pWhat = pWhat;
        }


        break;
      case PNG_IMG:
        newImage->data = (unsigned char *)PDFMalloc((newImage->filesize+16));
        if (!newImage->data)
          return false_a;
        if((fpimg = fopen(i_pczImageName, BINARY_READ)) != NULL)
        {
          fread(newImage->data, 1, (size_t)(newImage->filesize), fpimg);
          fclose(fpimg);
        }
        else
          return false_a;
        if (!PDFReadPNG(i_pczImageName, newImage))
          return false_a;


        break;
      default:
        return(false_a);
      break;
    }

    if(newImage->ncomponents == 1)
      i_pPDFDoc->imageFlagBCI |= 1;   /* /ImageB procset */
    else
      if(newImage->ncomponents > 2 )
        i_pPDFDoc->imageFlagBCI |= 2;   /* /ImageC procset */
    sprintf(imagename, "Imgpdf%ld", PDFGetDocClass(i_pPDFDoc)->GetImagesCount());
    image = imagename;
    /* memcpy(newImage, &tmpImage, sizeof(PDFImage)); */
    newImage->name = PDFStrDup(imagename);
    if (!(newImage->name))
      return false_a;
    newImage->filepath = PDFStrDup(i_pczImageName);
    if (!(newImage->filepath))
      return false_a;
    newImage->type = type;
    newImage->lIndex = PDFGetDocClass(i_pPDFDoc)->GetImagesCount();
    i_pPDFDoc->m_vctImages.push_back(newImage);
  }
  else {
    newImage = PDFGetDocClass(i_pPDFDoc)->GetImage(imageIndexFound);
    if (!newImage)
      return false_a;
    image = newImage->name;
    currentImageIndex = imageIndexFound;
  }

  PDFAddImageToPage(i_pPDFDoc, currentImageIndex, i_pPDFDoc->m_lActualPage);

  *o_pIndex = currentImageIndex;
  i_pczImageName = pSaveSave;

  return true_a;
}

int _isNewImage(PDFDocPTR pdf, const char *filepath, int *imageFound)
{
  int i, isNew = 1;
  CPDFImage *pImg;
  /* look in the list */
  for(i=0; i < PDFGetDocClass(pdf)->GetImagesCount(); i++)
  {
    pImg = PDFGetDocClass(pdf)->GetImage(i);
    if (pImg)
    {
      if( strcmp(filepath, pImg->filepath) == 0)
      {
        isNew = 0;    /* already defined */
        *imageFound = i;  /* return found image index */
        break;
      }
    }
  }
  if(isNew)
    *imageFound = PDFGetDocClass(pdf)->GetImagesCount();   /* index of the new image is this */
  return(isNew);
}

int read_JPEG_header(const char *filename, PDFImage *jInfo)
{
  int errcode = 0;
  FILE *infile;   /* input JPEG file */

  /* Open the input file. */
  if ((infile = fopen(filename, "rb")) == NULL)
  {
    return(NOT_USED);
  }
  /* Scan the JPEG headers. */
  errcode =  scan_JPEG_header(infile, jInfo);
  fclose(infile);
/*
  if (jInfo)
  {
    jInfo->width = jInfo->width / (300.f / DEFPOINTSPERINCH);//PDFConvertDimensionFromDefault(jInfo->width);
    jInfo->height = jInfo->height / (300.f / DEFPOINTSPERINCH);//PDFConvertDimensionFromDefault(jInfo->height);
  }
*/
  return(errcode);
}

int scan_JPEG_header(FILE *infile, PDFImage *jInfo)
{
  int marker;
  bool_a bRead = false_a;

  /* Expect SOI at start of file */
  if (first_marker(infile) != M_SOI)
    return(-2);

/*    ERREXIT("Expected SOI marker first"); */

  /* Scan miscellaneous markers until we reach SOS. */
  for (;;)
  {
    marker = next_marker(infile);
    switch (marker)
    {
      /* Note that marker codes 0xC4, 0xC8, 0xCC are not, and must not be,
       * treated as SOFn.  C4 in particular is actually DHT.
       */
    case M_SOF0:    /* Baseline */
    case M_SOF1:    /* Extended sequential, Huffman */
    case M_SOF2:    /* Progressive, Huffman */
    case M_SOF3:    /* Lossless, Huffman */
    case M_SOF5:    /* Differential sequential, Huffman */
    case M_SOF6:    /* Differential progressive, Huffman */
    case M_SOF7:    /* Differential lossless, Huffman */
    case M_SOF9:    /* Extended sequential, arithmetic */
    case M_SOF10:   /* Progressive, arithmetic */
    case M_SOF11:   /* Lossless, arithmetic */
    case M_SOF13:   /* Differential sequential, arithmetic */
    case M_SOF14:   /* Differential progressive, arithmetic */
    case M_SOF15:   /* Differential lossless, arithmetic */
      process_SOFn(infile, marker, jInfo);
      break;
    case M_SOS:      /* stop before hitting compressed data */
      return marker;
    case M_EOI:      /* in case it's a tables-only JPEG stream */
      return marker;
    case M_COM:
      skip_variable(infile);
      /* process_COM(infile); */  /* disabled */
      break;
    case M_APP12:
      skip_variable(infile);
      break;
    default:      /* Anything else just gets skipped */
      if (marker >= M_APP0 && marker <= M_APP12 && !bRead)
      {
        readDimension(infile, marker, jInfo);
        bRead = true_a;
      }
      else
        skip_variable(infile);    /* we assume it has a parameter count... */
      break;
    } /* end switch () */
  } /* end loop */

  return(0);
}

void skip_variable (FILE *infile)
/* Skip over an unknown or uninteresting variable-length marker */
{
  unsigned int length;

  /* Get the marker parameter length count */
  length = read_2_bytes(infile);
  /* Length includes itself, so must be at least 2 */
  if (length < 2)
  {
    //error
  }
  length -= 2;
  /* Skip over the remaining bytes */
  while (length > 0) {
    (void) read_1_byte(infile);
    length--;
  }
}

unsigned int read_2_bytes (FILE *infile)
{
  int c1, c2;

  c1 = getc(infile);
  if (c1 == EOF)
  {
    //error
  }
  c2 = getc(infile);
  if (c2 == EOF)
  {
    //error
  }
  return (((unsigned int) c1) << 8) + ((unsigned int) c2);
}

int read_1_byte (FILE *infile)
{
  int c;

  c = getc(infile);
  if (c == EOF)
  {
    //error
  }
  return c;
}

void readDimension(FILE *infile, int marker, PDFImage *jInfo)
{
  unsigned int length;
  unsigned int version, units, Xdensity, Ydensity, Xthumbnail, Ythumbnail, n, i;
  if (!jInfo)
    return;
  jInfo->Xdimension = 72;
  jInfo->Ydimension = 72;
  if (marker != M_APP0)
    return;
  length = read_2_bytes(infile);
  read_1_byte(infile);//read 'J'
  read_1_byte(infile);//read 'F'
  read_1_byte(infile);//read 'I'
  read_1_byte(infile);//read 'F'
  read_1_byte(infile);//read '\0'
  version = read_2_bytes(infile);
  units = read_1_byte(infile);
  Xdensity = read_2_bytes(infile);
  Ydensity = read_2_bytes(infile);
  Xthumbnail = read_1_byte(infile);
  Ythumbnail = read_1_byte(infile);
  n = Xthumbnail * Ythumbnail;

  if (units == 2)
  {
    jInfo->Xdimension = (int)(Xdensity * INCHINCM);
    jInfo->Ydimension = (int)(Ydensity * INCHINCM);
  }
  else
  {
    jInfo->Xdimension = Xdensity;
    jInfo->Ydimension = Ydensity;
  }

  for (i = 0; i < n; i++)
  {
    (void) read_1_byte(infile);
    (void) read_1_byte(infile);
    (void) read_1_byte(infile);
  }
}

void process_SOFn (FILE *infile, int marker, PDFImage *jInfo)
{
  unsigned int length;
  unsigned int image_height, image_width;
  int data_precision, num_components;
  int ci;

  length = read_2_bytes(infile);  /* usual parameter length count */

  data_precision = read_1_byte(infile);
  image_height = read_2_bytes(infile);
  image_width = read_2_bytes(infile);
  num_components = read_1_byte(infile);

  jInfo->process = marker;
  jInfo->width = image_width;
  jInfo->height = image_height;
  jInfo->ncomponents = num_components;
  jInfo->bitspersample = data_precision;

  if (length != (unsigned int) (8 + num_components * 3))
  {
    //error
  }

  for (ci = 0; ci < num_components; ci++) {
    (void) read_1_byte(infile);  /* Component ID code */
    (void) read_1_byte(infile);  /* H, V sampling factors */
    (void) read_1_byte(infile);  /* Quantization table number */
  }
}

int next_marker (FILE *infile)
{
  int c;
  int discarded_bytes = 0;

  /* Find 0xFF byte; count and skip any non-FFs. */
  c = read_1_byte(infile);
  while (c != 0xFF) {
    discarded_bytes++;
    c = read_1_byte(infile);
  }
  /* Get marker code byte, swallowing any duplicate FF bytes.  Extra FFs
   * are legal as pad bytes, so don't count them in discarded_bytes.
   */
  do {
    c = read_1_byte(infile);
  } while (c == 0xFF);

  if (discarded_bytes != 0)
  {
    //error
  }

  return c;
}

int first_marker (FILE *infile)
{
  int c1, c2;

  c1 = getc(infile);
  c2 = getc(infile);
  if (c1 != 0xFF || c2 != M_SOI)
  {
    //error
  }
  return c2;
}
























/////////////////////////////

int spng_getint(FILE *i_pFile)
{
  unsigned char ucBuf[4];

  if (fread(ucBuf, 1, 4, i_pFile) != 4)
    return -1;

  return ((int) ucBuf[0] << 24) + (ucBuf[1] << 16) + (ucBuf[2] << 8) + ucBuf[3];
}

bool_a spng_init(const char *i_pczImageName, PDFImage *i_pImage)
{
  FILE *pFile = NULL;
  char cBuf[8];

  pFile = fopen(i_pczImageName, "rb");
  if (!pFile)
    return false_a;

  i_pImage->startpos += 8;
  /* check signature*/
  if (fread(cBuf, 1, 8, pFile) != 8 || strncmp(cBuf, SPNG_SIGNATURE, 8) != 0)
  {
    fclose(pFile);
    return false_a;
  }
  /* read IHDR*/
  i_pImage->startpos += 8;
  if (spng_getint(pFile) != 13 || spng_getint(pFile) != SPNG_CHUNK_IHDR)
  {
    fclose(pFile);
    return false_a;
  }

  i_pImage->startpos += 8;
  spng_getint(pFile);//width
  spng_getint(pFile);//height

  i_pImage->startpos += 5;
  if (fread(cBuf, 1, 5, pFile) != 5)
  {
    fclose(pFile);
    return false_a;
  }

  cBuf[0];//bit depth
  cBuf[1];//color type
  cBuf[2]; //compr. type
  cBuf[3];//filter type
  cBuf[4];//interlace type

  i_pImage->startpos += 4;
  spng_getint(pFile); //CRC

  //decide whether this image is "simple"
  if (cBuf[0] > 8 || cBuf[1] > 3 || cBuf[4] != 0)
  {
    i_pImage->use_raw = false_a;
    {
      fclose(pFile);
      return true_a;
    }
  }
  else
    i_pImage->use_raw = true_a;
  //read (or skip) all chunks up to the first IDAT
  for (;;)
  {
    int len = spng_getint(pFile);
    int type = spng_getint(pFile);
    i_pImage->startpos += 8;
    switch (type)
    {
      case SPNG_CHUNK_IDAT:   // prepare data xfer
        i_pImage->nbytes = (size_t) len;
        fclose(pFile);
        return true_a;
      case -1:
        fclose(pFile);
        return false_a;
      //if we decide to live without LIBPNG, ** we should handle these cases, too.
      case SPNG_CHUNK_tRNS:   //transparency chunk
      case SPNG_CHUNK_PLTE:   //read in palette
      default:
        fseek(pFile, len + 4, SEEK_CUR);//skip data & CRC
        i_pImage->startpos = ftell(pFile);
        break;
    }
  }
  fclose(pFile);
  return true_a;
}

bool_a PDFAddImageToPage(PDFDocPTR i_pPDFDoc, long i_lImage, long i_lPage)
{
  PDFPage *pPage = NULL;
  if (i_lImage < 0 || i_lImage >= PDFGetDocClass(i_pPDFDoc)->GetImagesCount())
    return false_a;
  if (i_lPage < 0 || i_lPage >= PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;
  pPage = i_pPDFDoc->m_vctPages[i_lPage];
  if (!pPage)
    return false_a;
  {
    PDFList *pImageP, *pImageL = pPage->pImageIdx;
    bool_a bF = false_a;
    pImageP = pImageL;
    while(pImageL)
    {
      if(pImageL->lValue == i_lImage)
      {
        bF = true_a;
        break;
      }
      pImageP = pImageL;
      pImageL = pImageL->pNext;
    }
    if(!bF)
    {
      pImageL = (PDFList *)PDFMalloc((size_t)sizeof(PDFList));
      if (!pImageL)
        return false_a;
      pImageP->pNext = pImageL;
      pImageP->lValue = i_lImage;
      pImageL->pNext = NULL;
      pImageL->lValue = NOT_USED;
      i_pPDFDoc->m_vctPages[i_lPage]->lImageCount++;
    }
  }
  return true_a;
}

bool_a PDFHasPageImage(PDFDocPTR i_pPDFDoc, long i_lImage, long i_lPage)
{
  PDFPage *pPage = NULL;
  if (i_lImage < 0 || i_lImage >= PDFGetDocClass(i_pPDFDoc)->GetImagesCount()
      || i_lPage < 0 || i_lPage >= PDFGetDocClass(i_pPDFDoc)->GetPageCount())
    return false_a;
  pPage = i_pPDFDoc->m_vctPages[i_lPage];
  if (!pPage)
    return false_a;
  {
    PDFList *pImageL = pPage->pImageIdx;
    while(pImageL)
    {
      if(pImageL->lValue == i_lImage)
        return true_a;
      pImageL = pImageL->pNext;
    }
  }
  return false_a;
}

bool_a PDFHasImageOnlyOnePage(PDFDocPTR i_pPDFDoc, long i_lImage)
{
  PDFPage *pPage = NULL;
  long lPage;
  bool_a bF = false_a;
  if (i_lImage < 0 || i_lImage >= PDFGetDocClass(i_pPDFDoc)->GetImagesCount())
    return false_a;
  for (lPage = 0; lPage < PDFGetDocClass(i_pPDFDoc)->GetPageCount(); lPage++)
  {
    pPage = i_pPDFDoc->m_vctPages[lPage];
    if (pPage)
    {
      PDFList *pImageL = pPage->pImageIdx;
      while(pImageL)
      {
        if(pImageL->lValue == i_lImage)
        {
          if (bF)
            return false_a;
          else
          {
            bF = true_a;
            break;
          }
        }
        pImageL = pImageL->pNext;
      }
    }
  }
  return true_a;
}

bool_a PDFReadPNG(const char *i_pczImageName, PDFImage *i_pImage)
{
  png_structp     pPng = NULL;
  png_infop       pInfo = NULL;
  FILE            *pFile = NULL;
  png_uint_32     width, height, ui;
  png_bytep       *row_pointers, trans;
  png_color_16p   trans_values;
  int             bit_depth, color_type, num_trans, i;
  unsigned char   sig[8];
  png_color_8p    sig_bit;
  int mask = -1;
  float dpi_x, dpi_y;

  char *pSave = (char *)i_pczImageName;

  i_pImage->bUsePNG = true_a;

  pPng = png_create_read_struct("1.2.5", NULL, NULL, NULL);

  pInfo = png_create_info_struct(pPng);

  if ((pFile = fopen(i_pczImageName, "rb")) == NULL)
  {
    png_destroy_read_struct(&pPng, &pInfo, NULL);
    return false_a;
  }

  if (fread(sig, 1, 8, pFile) == 0 || !png_check_sig(sig, 8))
  {
    fclose(pFile);
    png_destroy_read_struct(&pPng, &pInfo, NULL);
    return false_a;
  }
  png_init_io(pPng, pFile);
  png_set_sig_bytes(pPng, 8);
  png_read_info(pPng, pInfo);
  png_get_IHDR(pPng, pInfo, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

  i_pImage->width = width;
  i_pImage->height = height;

  /* reduce 16-bit images to 8 bit since PDF stops at 8 bit */
  if (bit_depth == 16)
  {
    png_set_strip_16(pPng);
    bit_depth = 8;
  }
  i_pImage->bitspersample = bit_depth;

  /* Since PDF doesn't support a real alpha channel but only binary tranparency ("poor man's alpha"), we do our best and treat
  * alpha values of up to 50% as transparent, and values above 50% as opaque.
  * Since this behaviour is not exactly what the image author had in mind, it should probably be made user-configurable.*/
#define ALPHA_THRESHOLD 128

  switch (color_type)
  {
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      /* LATER: construct mask from alpha channel */
      /*png_set_IHDR(image->info.png.png_ptr, image->info.png.info_ptr, width, height, bit_depth, PNG_COLOR_MASK_ALPHA,
                      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);*/
      png_set_strip_alpha(pPng);
    /* fall through */
    case PNG_COLOR_TYPE_GRAY:
      if (png_get_sBIT(pPng, pInfo, &sig_bit))
        png_set_shift(pPng, sig_bit);
      i_pImage->colorspace  = DeviceGray;
      i_pImage->components = 1;
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      /* LATER: construct mask from alpha channel */
      png_set_strip_alpha(pPng);
      /* fall through */
    case PNG_COLOR_TYPE_RGB:
      i_pImage->colorspace  = DeviceRGB;
      i_pImage->components = 3;
      break;
    case PNG_COLOR_TYPE_PALETTE:
      {
        pdf_colormap *pColorMap = NULL;
        png_get_PLTE(pPng, pInfo, (png_colorp*) &pColorMap, &i_pImage->palette_size);
        if (pColorMap)
        {
          i_pImage->colormap = (pdf_colormap*)PDFMalloc(sizeof(pdf_colormap));
          memcpy(i_pImage->colormap, pColorMap, sizeof(pdf_colormap));
        }
        i_pImage->colorspace  = Indexed;
        i_pImage->components = 1;
        break;
      }
  }

  i_pImage->mask = mask;
  /* let libpng expand interlaced images */
  (void) png_set_interlace_handling(pPng);
  /* read the physical dimensions chunk to find the resolution values */
  dpi_x = (float) png_get_x_pixels_per_meter(pPng, pInfo);
  dpi_y = (float) png_get_y_pixels_per_meter(pPng, pInfo);

  if (dpi_x != 0 && dpi_y != 0)
  {  /* absolute values */
    i_pImage->Xdimension = PDFRound(dpi_x * 0.0254f);
    i_pImage->Ydimension = PDFRound(dpi_y * 0.0254f);
  }
  else
  { /* aspect ratio */
    i_pImage->Ydimension = (int)-png_get_pixel_aspect_ratio(pPng, pInfo);
    if (i_pImage->Ydimension == 0)  // unknown
      i_pImage->Xdimension = 0;
    else
      i_pImage->Xdimension = -1;

    i_pImage->Xdimension = 72;
    i_pImage->Ydimension = 72;
  }

  // read the transparency chunk
  if (png_get_valid(pPng, pInfo, PNG_INFO_tRNS))
  {
    png_get_tRNS(pPng, pInfo, &trans, &num_trans, &trans_values);
    if (num_trans > 0)
    {
      if (color_type == PNG_COLOR_TYPE_GRAY)
      {
        i_pImage->transparent = !i_pImage->transparent;
        // LATER: scale down 16-bit transparency values ?
        i_pImage->transval[0] = (unsigned char) trans_values[0].gray;
      }
      else if (color_type == PNG_COLOR_TYPE_RGB)
      {
        i_pImage->transparent = !i_pImage->transparent;
        // LATER: scale down 16-bit transparency values ?
        i_pImage->transval[0] = (unsigned char) trans_values[0].red;
        i_pImage->transval[1] = (unsigned char) trans_values[0].green;
        i_pImage->transval[2] = (unsigned char) trans_values[0].blue;
      }
      else if (color_type == PNG_COLOR_TYPE_PALETTE)
      {
        // we use the first transparent entry in the tRNS palette
        for (i = 0; i < num_trans; i++)
        {
          if ((unsigned char) trans[i] < ALPHA_THRESHOLD)
          {
            i_pImage->transparent = !i_pImage->transparent;
            i_pImage->transval[0] = (unsigned char) i;
            break;
          }
        }
      }
    }
  }

  png_read_update_info(pPng, pInfo);

  i_pImage->rowbytes = png_get_rowbytes(pPng, pInfo);

  i_pImage->raster = (unsigned char *)malloc(i_pImage->rowbytes * height);

  row_pointers = (png_bytep *)malloc(height * sizeof(png_bytep));

  for (ui = 0; ui < height; ui++)
  {
    row_pointers[ui] = i_pImage->raster + ui * i_pImage->rowbytes;
  }

  i_pImage->startpos = 0;
  if (spng_init(i_pczImageName, i_pImage) && i_pImage->use_raw)
  {
    i_pImage->predictor  = pred_png;
    i_pImage->compression  = flate;
  }
  else
  {
    png_read_image(pPng, row_pointers);
  }

  fclose(pFile);

  free(row_pointers);

  png_destroy_read_struct(&pPng, &pInfo, NULL);

  i_pczImageName = pSave;

  return true_a;
}

