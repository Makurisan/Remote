
#include "pdffnc.h"
#include "PDFDocument.h"
#include "PDFImage.h"
#include "pdfimg.h"

using namespace std;

CPDFImage::CPDFImage(CPDFDocument *i_pDoc)
  :m_pDoc(i_pDoc)
{
  this->name = NULL;
  this->filepath = NULL;
  this->data = NULL;
  this->raster = NULL;
  this->colormap = NULL;
  this->lColorSpaceObjIndex = NOT_USED;
  this->lObjIndex = NOT_USED;
}

CPDFImage::~CPDFImage()
{
  if(this->name)
  {
    free(this->name);
    this->name = NULL;
  }
  if(this->filepath)
  {
    free(this->filepath);
    this->filepath = NULL;
  }
  if(this->data)
  {
    free(this->data);
    this->data = NULL;
  }
  if (this->raster)
  {
    free(this->raster);
    this->raster = NULL;
  }
  if (this->colormap)
  {
    free(this->colormap);
    this->colormap = NULL;
  }
}

long CPDFImage::GetImageID()
{
  return this->lIndex;
}

void CPDFImage::AssignObjNum(long &io_lObjNum)
{
  this->lObjIndex = io_lObjNum++;
  if (this->colorspace == Indexed)
    this->lColorSpaceObjIndex = io_lObjNum++;
}

bool_a CPDFImage::GetEntireImageStream(std::vector<std::string> &o_vctFontStreams)
{
  long lSize = this->height * this->rowbytes;
  unsigned char *pOutput = NULL;
  string strBuffer("");
  char czTemp[200];
  if (!this->bUsePNG)
  {
    int Invert = this->invert;

    sprintf(czTemp, "%ld 0 obj ", this->lObjIndex);
    strBuffer.append(czTemp);
    strBuffer.append("<< /Type /XObject /Subtype /Image ");
    sprintf(czTemp, "/Name /%s ", this->name);
    strBuffer.append(czTemp);
    sprintf(czTemp, "/Width %ld ", this->width);
    strBuffer.append(czTemp);
    sprintf(czTemp, "/Height %ld ", this->height);
    strBuffer.append(czTemp);
    strBuffer.append("/Filter /DCTDecode ");

    if(Invert)
    {
      sprintf(czTemp, "/Decode %s ", pczInvArray[this->ncomponents]);
      strBuffer.append(czTemp);
    }
    sprintf(czTemp, "/BitsPerComponent %ld ", this->bitspersample);
    strBuffer.append(czTemp);
    if (this->imagemask == 1 && this->ncomponents == 1 && this->bitspersample == 1)
      strBuffer.append("/ImageMask true ");
    else
    {
      sprintf(czTemp, "/ColorSpace /%s ", pczColorSpaces[this->ncomponents]);
      strBuffer.append(czTemp);
    }

    strBuffer.append("/Interpolate true ");

    sprintf(czTemp, "/Length %ld ", this->filesize);
    strBuffer.append(czTemp);

    strBuffer.append(">> stream\n");

    if (m_pDoc->Encrypt())
      PDFEncrypt(m_pDoc, this->data, this->filesize, this->lObjIndex, 0);

    strBuffer.append((const char *)this->data, this->filesize);

    strBuffer.append("\nendstream endobj\n");
  }
  else
  {
    sprintf(czTemp, "%ld 0 obj ", this->lObjIndex);
    strBuffer.append(czTemp);
    strBuffer.append("<< /Type /XObject /Subtype /Image ");
    sprintf(czTemp, "/Name /%s ", this->name);
    strBuffer.append(czTemp);
    sprintf(czTemp, "/Width %ld ", this->width);
    strBuffer.append(czTemp);
    sprintf(czTemp, "/Height %ld ", this->height);
    strBuffer.append(czTemp);
    sprintf(czTemp, "/BitsPerComponent %ld ", this->bitspersample);
    strBuffer.append(czTemp);
    // Transparency handling
    // Masking by color: single transparent color value
    if (this->transparent)
    {
      if (this->colorspace == Indexed || this->colorspace == DeviceGray)
      {
        sprintf(czTemp, "/Mask[%d %d] ", (int)this->transval[0], (int)this->transval[0]);
      }
      else if (this->colorspace == DeviceRGB)
      {
        sprintf(czTemp,  "/Mask[%d %d %d %d %d %d] ", (int)this->transval[0], (int) this->transval[0],
                                      (int) this->transval[1], (int) this->transval[1],
                                      (int) this->transval[2], (int) this->transval[2]);
      }
      else if (this->colorspace == DeviceCMYK)
      {
        sprintf(czTemp, "/Mask[%d %d %d %d %d %d %d %d] ", (int) this->transval[0], (int) this->transval[0],
                              (int) this->transval[1], (int) this->transval[1], (int) this->transval[2],
                              (int) this->transval[2], (int) this->transval[3], (int) this->transval[3]);
      }
      else
      {
        czTemp[0] = 0;//???
      }
      strBuffer.append(czTemp);
      // Masking by position: separate bitmap mask
    }
    else if (this->mask != -1)
    {
//      pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "", );

//      pdf_printf(p, "/Mask %ld 0 R\n", p->xobjects[p->images[image->mask].no].obj_id);
    }

    switch (this->colorspace)
    {
      case ImageMask:
        sprintf(czTemp, "/ImageMask true ");
        break;
      case Indexed:
        {
          if (this->components != 1)
            break;

          strBuffer.append("/ColorSpace[/Indexed/DeviceRGB ");
          sprintf(czTemp, "%d %ld 0 R]\n", this->palette_size - 1, this->lColorSpaceObjIndex);
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
          sprintf(czTemp, "/ColorSpace/%s ", pdf_colorspace_names[this->colorspace]);
        }
        break;
    }
    strBuffer.append(czTemp);
/*
    //* special case: referenced image data instead of direct data
    if (image->reference != pdf_ref_direct)
    {
      if (image->compression != none)
      {
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/FFilter[/%s]\n", pdf_filter_names[this->compression]);
      }
      if (image->compression == ccitt)
      {
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/FDecodeParms[<<", );
        if ((int) image->width != 1728)  // CCITT default width
          pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Columns %d", (int)this->width);
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/Rows %d", (int)this->height);
        //* write CCITT parameters if required
        if (image->params)
          pdf_puts(p, image->params);
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, ">>]\n", );
      }
      if (image->reference == pdf_ref_file)
      {
        // LATER: make image file name platform-neutral: Change : to / on the Mac Change \ to / on Windows
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/F(%s)/Length 0", this->filename);
      }
      else if (image->reference == pdf_ref_url)
      {
        pdf->lActualSize += PDFWriteFormatedBufToDocStream(pdf, "/F<</FS/URL/F(%s)>>/Length 0", this->filename);
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
      if (!this->use_raw)
        this->compression = flate;
      if (this->compression != none)
      {
        sprintf(czTemp, "/Filter[/%s]\n", pdf_filter_names[this->compression]);
        strBuffer.append(czTemp);
      }
    }
    // prepare precompressed (raw) image data
    if (this->use_raw)
    {
      strBuffer.append("/DecodeParms[<< ");
      // write EarlyChange or CCITT parameters if required
      if (this->compression == flate || this->compression == lzw)
      {
        if (this->predictor != pred_default)
        {
          sprintf(czTemp, "/Predictor %d ", (int) this->predictor);
          strBuffer.append(czTemp);
          sprintf(czTemp, "/Columns %d ", (int) this->width);
          strBuffer.append(czTemp);

          if (this->bitspersample != 8)
          {
            sprintf(czTemp, "/BitsPerComponent %d ", this->bitspersample);
            strBuffer.append(czTemp);
          }
          if (this->colorspace < LastCS)
          {
            switch (this->colorspace)
            {
              case Indexed:
              case ImageMask:
              case DeviceGray:
                // 1 is default
                break;
              case DeviceRGB:
                strBuffer.append("/Colors 3 ");
                break;
              case DeviceCMYK:
                strBuffer.append("/Colors 4 ");
                break;
              default:
              break;
            }
          }
        }
      }
      if (this->compression == ccitt)
      {
        if ((int) this->width != 1728)  // CCITT default width
        {
          sprintf(czTemp, "/Columns %d ", (int)this->width);
          strBuffer.append(czTemp);
        }
        sprintf(czTemp, "/Rows %d ", (int)this->height);
        strBuffer.append(czTemp);
      }
        strBuffer.append(">>] ");
    }
    if (this->invert)
    {
      int i;
      strBuffer.append("/Decode[1 0");
      for (i = 1; i < this->components; i++)
      {
        strBuffer.append(" 1 0");
      }
      strBuffer.append("] ");
    }
    // Write the actual image data

    if (this->use_raw)
    {
      if (this->nbytes > 0)
      {
        unsigned char *pSave = NULL;
        unsigned char *pFile = this->data;
        size_t Alloc = 10000 > this->nbytes ? 10000 : 10000 + this->nbytes;
        size_t Used = 0;
        lSize = this->nbytes;
        pOutput = (unsigned char *)PDFMalloc(Alloc);
        memset(pOutput, 0, Alloc);
        pSave = pOutput;
        do
        {
          pFile += this->startpos;
          memcpy(pOutput, pFile, this->nbytes);
          pFile += this->nbytes;
          pOutput += this->nbytes;
          Used += this->nbytes;
          // proceed to next IDAT chunk
          pFile += 4;//CRC
          this->nbytes = ((int) pFile[0] << 24) + (pFile[1] << 16) + (pFile[2] << 8) + pFile[3]; //length
          pFile += 4;
          if ((((int) pFile[0] << 24) + (pFile[1] << 16) + (pFile[2] << 8) + pFile[3]) != SPNG_CHUNK_IDAT)
          {
            this->nbytes = 0;
            break;
          }
          lSize += this->nbytes;
          pFile += 4;
          if (Alloc - Used < this->nbytes)
          {
            Alloc += this->nbytes - (Alloc - Used);
            Alloc += 10000;

            PDFRealloc((void **)&pSave, Alloc, 10000);
            pOutput = pSave + Used;
          }
        } while (this->nbytes > 0);
        pOutput = pSave;
      }
    }
    else
    {
      lSize = this->height * this->rowbytes;
      pOutput = NULL;

      pOutput = PDFCompressStream(this->raster, &lSize);
    }

    sprintf(czTemp, "/Length %ld\n", lSize);
    strBuffer.append(czTemp);

    strBuffer.append(">>\n");
    strBuffer.append("stream\n");

    if (m_pDoc->Encrypt())
      PDFEncrypt(m_pDoc, pOutput, lSize, this->lObjIndex, 0);

    strBuffer.append((const char *)pOutput, lSize);

    free(pOutput);

    strBuffer.append("\nendstream endobj\n");
  }

  o_vctFontStreams.push_back(strBuffer);
  strBuffer.clear();

  if (this->colorspace == Indexed)
  {
    long lSize = this->palette_size * 3;
    unsigned char *pOutput = NULL;

    pOutput = PDFCompressStream((unsigned char *)this->colormap, &lSize);
    if (pOutput)
    {
      sprintf(czTemp, "%ld 0 obj ", this->lColorSpaceObjIndex);
      strBuffer.append(czTemp);

      strBuffer.append("<< ");

      strBuffer.append("/Filter[/FlateDecode] ");
      sprintf(czTemp, "/Length %ld ", lSize);
      strBuffer.append(czTemp);

      strBuffer.append(">>\n");

      strBuffer.append("stream\n");

      if (m_pDoc->Encrypt())
        PDFEncrypt(m_pDoc, pOutput, lSize, this->lColorSpaceObjIndex, 0);

      strBuffer.append((const char *)pOutput, lSize);

      free(pOutput);

      strBuffer.append("\nendstream endobj\n");
      o_vctFontStreams.push_back(strBuffer);
    }
  }

  return true_a;
}
