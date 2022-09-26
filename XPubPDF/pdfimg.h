

#ifndef __PDF_IMG_H__
#define __PDF_IMG_H__

#include "pdfdef.h"

#define SPNG_SIGNATURE  "\211\120\116\107\015\012\032\012"
#define SPNG_CHUNK_IHDR 0x49484452
#define SPNG_CHUNK_PLTE 0x504C5445
#define SPNG_CHUNK_tRNS 0x74524E53
#define SPNG_CHUNK_IDAT 0x49444154
#define SPNG_CHUNK_IEND 0x49454E44


bool_a PDFReadImage(PDFDocPTR i_pPDFDoc, const char *i_pczImageName, long *o_pIndex);

int read_JPEG_header(const char *filename, PDFImage *jInfo);
int _isNewImage(PDFDocPTR pdf, const char *filepath, int *imageFound);
int scan_JPEG_header(FILE *infile, PDFImage *jInfo);
void skip_variable (FILE *infile);
unsigned int read_2_bytes (FILE *infile);
int read_1_byte (FILE *infile);
void readDimension(FILE *infile, int marker, PDFImage *jInfo);
void process_SOFn (FILE *infile, int marker, PDFImage *jInfo);
int next_marker (FILE *infile);
int first_marker (FILE *infile);



bool_a PDFReadPNG(const char *i_pczImageName, PDFImage *i_pImage);

#endif  /*  __PDF_IMG_H__  */
