
#include <stdlib.h>
#include <malloc.h>
#include <string>
#include "md5.h"

using namespace std;

#define ulPDFPasswordLen      32
//#define iEncRevision          3

static unsigned char ucState[256];
static unsigned char ucX, ucY;


static void rc4InitKey(unsigned char *key, int keyLen, unsigned char *state);
static unsigned char rc4EcryptByte(unsigned char *state, unsigned char *x, unsigned char *y, unsigned char c);


#include "pdfdef.h"
#include "pdffnc.h"

void PDFEncryptInit(unsigned char *i_pEncrKey, int i_iObjNum, int i_iObjGen);

unsigned char PDFEncryptByte(unsigned char i_ucByte);

bool_a PDFCreateEncrKey(const unsigned char *i_pPassword, const unsigned char *i_pucEnDicOwPassw, const unsigned char *i_pucFileID, int i_iPermission,
                             unsigned char *o_pucEncrKey);


bool_a PDFCreateOwnerEncrKey(const unsigned char *i_pOwnerPassword, int i_iLen,
                              int i_iEncRevision, unsigned char *o_pucEncrKey);
bool_a PDFMakeUserPassw(const unsigned char *i_pucOwnerPassw, const unsigned char *i_pucO,
                        int i_iEncRevision, int i_iLen, unsigned char *o_pucUserPassw);

bool_a PDFCreateEncrDictOwnerPassw(unsigned char *i_pOwnerPassword, unsigned char *i_pUserPassword,
                                        unsigned char *o_pucEnDicOwPassw);

bool_a PDFCreateEncrDictUserPassw(const unsigned char *i_pUserPassword, const unsigned char *i_pucEnDicOwPassw, const unsigned char *i_pucFileID, int i_iPermission,
                                       unsigned char *o_pucEnDicUserPassw);

void PDFCreateFileID(PDFDocPTR i_pPDFDoc);





bool_a PDFEncrypt(PDFDocPTR i_pPDFDoc, unsigned char *i_pBuffer, long i_lSize, int i_iObjNum, int i_iObjGen)
{
  long i, length;
  unsigned char *tempbuf = NULL;

  if (!PDFStrLenUC(i_pPDFDoc->ucFileID))
    strncpy((char*)i_pPDFDoc->ucFileID, (const char*)ucFILE_ID_1, 32);

  tempbuf = (unsigned char*)PDFMalloc(PDFStrLenUC(i_pPDFDoc->ucFileID) / 2 + 3);
  if (!tempbuf)
    return false_a;
  PDFconvertHexToBinary((const char*)i_pPDFDoc->ucFileID, (unsigned char*)tempbuf, &length);

//  PDFCreateFileID(i_pPDFDoc);

  if (PDFStrLenUC(i_pPDFDoc->ucO) <= 0)
  {
    if (!PDFCreateEncrDictOwnerPassw(i_pPDFDoc->ucOwnerPassw, i_pPDFDoc->ucUserPassw, i_pPDFDoc->ucO))
      return false_a;
  }
  if (PDFStrLenUC(i_pPDFDoc->ucEncrKey) <= 0)
  {
    if (!PDFCreateEncrKey(i_pPDFDoc->ucUserPassw, i_pPDFDoc->ucO, tempbuf, i_pPDFDoc->iPermission, i_pPDFDoc->ucEncrKey))
      return false_a;
  }
  if (PDFStrLenUC(i_pPDFDoc->ucU) <= 0)
  {
    if (!PDFCreateEncrDictUserPassw(i_pPDFDoc->ucUserPassw, i_pPDFDoc->ucO, tempbuf, i_pPDFDoc->iPermission, i_pPDFDoc->ucU))
      return false_a;
  }

  PDFEncryptInit(i_pPDFDoc->ucEncrKey, i_iObjNum, i_iObjGen);

  for (i = 0; i < i_lSize; i++)
  {
    i_pBuffer[i] = PDFEncryptByte(i_pBuffer[i]);
  }

  free(tempbuf);

  return true_a;
}

bool_a PDFEncryptWithPassword(unsigned char *i_pBuffer, long i_lSize, int i_iObjNum, int i_iObjGen,
                              string i_strID, string i_strO, int i_iPerm,
                              string i_strOwnerPassw, int i_iEncRevision, int i_iLen)
{
  long i, length;
  unsigned char *tempbuf = NULL;
  unsigned char ucUserPassw[32];
  unsigned char ucEncrKey[32];

  tempbuf = (unsigned char*)PDFMalloc(i_strID.size() / 2 + 3);
  if (!tempbuf)
    return false_a;
  PDFconvertHexToBinary(i_strID.c_str(), (unsigned char*)tempbuf, &length);


  if (!PDFMakeUserPassw((const unsigned char *)i_strOwnerPassw.c_str(), (const unsigned char *)i_strO.c_str(),
                          i_iEncRevision, i_iLen, ucUserPassw))
    return false_a;

  if (!PDFCreateEncrKey(ucUserPassw, (const unsigned char *)i_strO.c_str(), tempbuf, i_iPerm, ucEncrKey))
    return false_a;


  PDFEncryptInit(ucEncrKey, i_iObjNum, i_iObjGen);
  for (i = 0; i < i_lSize; i++)
  {
    i_pBuffer[i] = PDFEncryptByte(i_pBuffer[i]);
  }

  free(tempbuf);

  return true_a;
}

void PDFEncryptInit(unsigned char *i_pEncrKey, int i_iObjNum, int i_iObjGen)
{
  int i, iKeyLen = 16;
  unsigned char ucObjKey[21];
  MD5_CTX mdContext;

  for (i = 0; i < iKeyLen; ++i)
  {
    ucObjKey[i] = i_pEncrKey[i];
  }
  if (i_iObjNum != 0 || i_iObjGen != 0)
  {
    ucObjKey[iKeyLen] = (unsigned char)(i_iObjNum & 0xff);
    ucObjKey[iKeyLen + 1] = (unsigned char)(i_iObjNum >> 8) & 0xff;
    ucObjKey[iKeyLen + 2] = (unsigned char)(i_iObjNum >> 16) & 0xff;
    ucObjKey[iKeyLen + 3] = (unsigned char)i_iObjGen & 0xff;
    ucObjKey[iKeyLen + 4] = (unsigned char)(i_iObjGen >> 8) & 0xff;
  } else
  {
    ucObjKey[iKeyLen] = 0;
    ucObjKey[iKeyLen + 1] = 0;
    ucObjKey[iKeyLen + 2] = 0;
    ucObjKey[iKeyLen + 3] = 0;
    ucObjKey[iKeyLen + 4] = 0;
  }

  MD5Init (&mdContext);
  MD5Update(&mdContext, ucObjKey, iKeyLen + 5);
  MD5Final(&mdContext);
  memset(ucObjKey, 0, 21);
  memcpy(ucObjKey, mdContext.digest, 16);

  ucX = ucY = 0;
  rc4InitKey(ucObjKey, iKeyLen, ucState);
}

unsigned char PDFEncryptByte(unsigned char i_ucByte)
{
  return rc4EcryptByte(ucState, &ucX, &ucY, i_ucByte);
}

void PDFCreateFileID(PDFDocPTR i_pPDFDoc)
{
  char *pczDate;
  long lLen;
  time_t current_time;
  struct tm *bd_time;
  MD5_CTX mdContext;


  MD5Init (&mdContext);

  pczDate = (char *)malloc(16);
  memset(pczDate, 0, 16);
  time(&current_time);
  bd_time = localtime(&current_time);
  sprintf (pczDate, "%04d%02d%02d%02d%02d%02d", bd_time->tm_year + 1900, bd_time->tm_mon + 1, bd_time->tm_mday,
                                                    bd_time->tm_hour, bd_time->tm_min, bd_time->tm_sec);
  MD5Update(&mdContext, (unsigned char *)pczDate, PDFStrLenC(pczDate));
  free(pczDate);
  pczDate = NULL;

  lLen = PDFStrLenC(i_pPDFDoc->strInfo.czAuthor) + PDFStrLenC(i_pPDFDoc->strInfo.czCreator)
          + PDFStrLenC(i_pPDFDoc->strInfo.czKeywords) + PDFStrLenC(i_pPDFDoc->strInfo.czProducer)
          + PDFStrLenC(i_pPDFDoc->strInfo.czTitle);
  pczDate = (char *)malloc(lLen + 1);
  sprintf(pczDate, "%s%s%s%s%s", i_pPDFDoc->strInfo.czAuthor, i_pPDFDoc->strInfo.czCreator, i_pPDFDoc->strInfo.czKeywords,
                                 i_pPDFDoc->strInfo.czProducer, i_pPDFDoc->strInfo.czTitle);
  MD5Update(&mdContext, (unsigned char *)pczDate, lLen);
  free(pczDate);
  pczDate = NULL;

  MD5Final(&mdContext);
  memset(i_pPDFDoc->ucFileID, 0, 17);
  memcpy(i_pPDFDoc->ucFileID, mdContext.digest, 16);
}

bool_a PDFCreateEncrKey(const unsigned char *i_pPassword, const unsigned char *i_pucEnDicOwPassw, const unsigned char *i_pucFileID, int i_iPermission,
                        unsigned char *o_pucEncrKey)
{
  unsigned char ucBuffer[ulPDFPasswordLen];
  size_t len;
  int i;
  MD5_CTX mdContext;

////////////////////////////
// step 1 Alg. 3.2
////////////////////////////
  if (i_pPassword)
  {
    len = PDFStrLenUC(i_pPassword);
    if (len < ulPDFPasswordLen)
    {
      memcpy(ucBuffer, i_pPassword, len);
      memcpy(ucBuffer + len, PDFPasswordPad, ulPDFPasswordLen - len);
    }
    else
      memcpy(ucBuffer, i_pPassword, ulPDFPasswordLen);
  }
  else
  {
    memcpy(ucBuffer, PDFPasswordPad, ulPDFPasswordLen);
  }
////////////////////////////
// step 2 Alg. 3.2
////////////////////////////
  MD5Init (&mdContext);
  MD5Update(&mdContext, ucBuffer, 32);
////////////////////////////
// step 3 Alg. 3.2
////////////////////////////
  MD5Update(&mdContext, (unsigned char*)i_pucEnDicOwPassw, 32);
////////////////////////////
// step 4 Alg. 3.2
////////////////////////////
  memset(ucBuffer, 0, ulPDFPasswordLen);
  ucBuffer[0] = (unsigned char)(i_iPermission & 0xff);
  ucBuffer[1] = (unsigned char)(i_iPermission >> 8) & 0xff;
  ucBuffer[2] = (unsigned char)(i_iPermission >> 16) & 0xff;
  ucBuffer[3] = (unsigned char)(i_iPermission >> 24) & 0xff;
  MD5Update(&mdContext, ucBuffer, 4);
////////////////////////////
// step 5 Alg. 3.2
////////////////////////////
  MD5Update(&mdContext, (unsigned char*)i_pucFileID, 16);
  MD5Final(&mdContext);
  memset(ucBuffer, 0, 32);
  memcpy(ucBuffer, mdContext.digest, 16);
////////////////////////////
// step 6 Alg. 3.2
////////////////////////////
//  if (iEncRevision == 3)
  {
    for (i = 0; i < 50; i++)
    {
      MD5Init (&mdContext);
      MD5Update(&mdContext, ucBuffer, 16);
      MD5Final(&mdContext);
      memset(ucBuffer, 0, 32);
      memcpy(ucBuffer, mdContext.digest, 16);
    }
  }
////////////////////////////
// step 7 Alg. 3.2
////////////////////////////
  if (o_pucEncrKey)
    memcpy(o_pucEncrKey, ucBuffer, 16);
  return true_a;
////////////////////////////
//
////////////////////////////
}

bool_a PDFMakeUserPassw(const unsigned char *i_pucOwnerPassw, const unsigned char *i_pucO,
                        int i_iEncRevision, int i_iLen, unsigned char *o_pucUserPassw)
{
  unsigned char ucEncrKey[32] = {0};
  unsigned char ucEncrKeyTmp[32] = {0};
  unsigned char ucO[32];
  unsigned char fState[256];
  unsigned char fx, fy;
  short i, j;

  memcpy(ucO, i_pucO, 32);

  if (!PDFCreateOwnerEncrKey(i_pucOwnerPassw, i_iLen, i_iEncRevision, ucEncrKey))
    return false_a;

  if (3 == i_iEncRevision)
  {
    for (i = 19; i >= 0; i--)
    {
      for (j = 0; j < 16; j++)
      {
        ucEncrKeyTmp[j] = ucEncrKey[j] ^ (unsigned char)i;
      }

      rc4InitKey(ucEncrKeyTmp, 16, fState);
      fx = fy = 0;
      for (j = 0; j < 32; j++)
      {
        ucO[j] = rc4EcryptByte(fState, &fx, &fy, ucO[j]);
      }

    }
  } else
  {
    rc4InitKey(ucEncrKey, 5, fState);
    fx = fy = 0;
    for (i = 0; i < 32; i++)
    {
      ucO[i] = rc4EcryptByte(fState, &fx, &fy, ucO[i]);
    }
  }

  for (i = 0; i < 32; i++)
  {
    for (j = 0; j < 32 - i; j++)
    {
      if (ucO[i + j] != PDFPasswordPad[j])
        break;
    }
    if (j == (32 - i))
    {
      ucO[i] = 0;
      break;
    }
  }


  if (o_pucUserPassw)
    memcpy(o_pucUserPassw, ucO, 32);


  return true_a;;
}

bool_a PDFCreateOwnerEncrKey(const unsigned char *i_pOwnerPassword, int i_iLen,
                              int i_iEncRevision, unsigned char *o_pucEncrKey)
{
  size_t len;
  unsigned char ucBuffer[ulPDFPasswordLen];
  unsigned char ucEncrKey[ulPDFPasswordLen];
  MD5_CTX mdContext;
  short i;
////////////////////////////
// step 1 Alg. 3.3
////////////////////////////
  if (i_pOwnerPassword)
  {
    len = PDFStrLenUC(i_pOwnerPassword);
    if (len < ulPDFPasswordLen)
    {
      memcpy(ucBuffer, i_pOwnerPassword, len);
      memcpy(ucBuffer + len, PDFPasswordPad, ulPDFPasswordLen - len);
    }
    else
      memcpy(ucBuffer, i_pOwnerPassword, ulPDFPasswordLen);
  } else
  {
    memcpy(ucBuffer, PDFPasswordPad, ulPDFPasswordLen);
  }
////////////////////////////
// step 2 Alg. 3.3
////////////////////////////
  MD5Init (&mdContext);
  MD5Update(&mdContext, ucBuffer, 32);
  MD5Final(&mdContext);
  memset(ucBuffer, 0, 32);
  memcpy(ucBuffer, mdContext.digest, 16);
////////////////////////////
// step 3 Alg. 3.3
////////////////////////////
  if (i_iEncRevision == 3)
  {
    for (i = 0; i < 50; i++)
    {
      MD5Init (&mdContext);
      MD5Update(&mdContext, ucBuffer, 16);
      MD5Final(&mdContext);
      memset(ucBuffer, 0, 32);
      memcpy(ucBuffer, mdContext.digest, 16);
    }
  }
////////////////////////////
// step 4 Alg. 3.3
////////////////////////////
  memset(ucEncrKey, 0, 32);

  if (i_iEncRevision == 3)
    memcpy(ucEncrKey, ucBuffer, i_iLen / 8);
  else if (i_iEncRevision == 2)
    memcpy(ucEncrKey, ucBuffer, 5);


  if (o_pucEncrKey)
  {
    if (3 == i_iEncRevision)
      memcpy(o_pucEncrKey, ucEncrKey, i_iLen / 8);
    else if (2 == i_iEncRevision)
      memcpy(o_pucEncrKey, ucEncrKey, 5);
  }

  return true_a;
}

bool_a PDFCreateEncrDictOwnerPassw(unsigned char *i_pOwnerPassword, unsigned char *i_pUserPassword,
                                        unsigned char *o_pucEnDicOwPassw)
{
  size_t len;
  unsigned char ucBuffer[ulPDFPasswordLen];
  unsigned char fState[256];
  unsigned char fx, fy;
  unsigned char ucEncrKey[ulPDFPasswordLen];
  unsigned char ucTmpKey[ulPDFPasswordLen];
  MD5_CTX mdContext;
  short i, j;
////////////////////////////
// step 1 Alg. 3.3
////////////////////////////
  if (i_pOwnerPassword)
  {
    len = PDFStrLenUC(i_pOwnerPassword);
    if (len < ulPDFPasswordLen)
    {
      memcpy(ucBuffer, i_pOwnerPassword, len);
      memcpy(ucBuffer + len, PDFPasswordPad, ulPDFPasswordLen - len);
    }
    else
      memcpy(ucBuffer, i_pOwnerPassword, ulPDFPasswordLen);
  }
  else if (i_pUserPassword)
  {
    len = PDFStrLenUC(i_pUserPassword);
    if (len < ulPDFPasswordLen)
    {
      memcpy(ucBuffer, i_pUserPassword, len);
      memcpy(ucBuffer + len, PDFPasswordPad, ulPDFPasswordLen - len);
    }
    else
      memcpy(ucBuffer, i_pUserPassword, ulPDFPasswordLen);
  }
  else
  {
    memcpy(ucBuffer, PDFPasswordPad, ulPDFPasswordLen);
  }
////////////////////////////
// step 2 Alg. 3.3
////////////////////////////
  MD5Init (&mdContext);
  MD5Update(&mdContext, ucBuffer, 32);
  MD5Final(&mdContext);
  memset(ucBuffer, 0, 32);
  memcpy(ucBuffer, mdContext.digest, 16);
////////////////////////////
// step 3 Alg. 3.3
////////////////////////////
//  if (iEncRevision == 3)
  {
    for (i = 0; i < 50; i++)
    {
      MD5Init (&mdContext);
      MD5Update(&mdContext, ucBuffer, 16);
      MD5Final(&mdContext);
      memset(ucBuffer, 0, 32);
      memcpy(ucBuffer, mdContext.digest, 16);
    }
  }
////////////////////////////
// step 4 Alg. 3.3
////////////////////////////
  memset(ucEncrKey, 0, 32);
  memcpy(ucEncrKey, ucBuffer, 16);
////////////////////////////
// step 5 Alg. 3.3
////////////////////////////
  if (i_pUserPassword)
  {
    len = PDFStrLenUC(i_pUserPassword);
    if (len < ulPDFPasswordLen)
    {
      memcpy(ucBuffer, i_pUserPassword, len);
      memcpy(ucBuffer + len, PDFPasswordPad, ulPDFPasswordLen - len);
    }
    else
      memcpy(ucBuffer, i_pUserPassword, ulPDFPasswordLen);
  }
  else
  {
    memcpy(ucBuffer, PDFPasswordPad, ulPDFPasswordLen);
  }
////////////////////////////
// step 6 Alg. 3.3
////////////////////////////
/*
  if (iEncRevision == 2)
  {
    rc4InitKey(ucEncrKey, 5, fState);
    fx = fy = 0;
    for (i = 0; i < 32; i++)
    {
      ucBuffer[i] = rc4EcryptByte(fState, &fx, &fy, ucBuffer[i]);
    }
  }
  else
*/
  {
    rc4InitKey(ucEncrKey, 16, fState);
    fx = fy = 0;
    for (i = 0; i < 32; i++)
    {
      ucBuffer[i] = rc4EcryptByte(fState, &fx, &fy, ucBuffer[i]);
    }
  }
////////////////////////////
// step 7 Alg. 3.3
////////////////////////////
//  if (iEncRevision == 3)
  {
    for (i = 1; i <= 19; i++)
    {
      memset(ucTmpKey, 0, 32);
      for (j = 0; j < 16; j++)
      {
        ucTmpKey[j] = ucEncrKey[j] ^ (unsigned char)i;
      }

      rc4InitKey(ucTmpKey, 16, fState);
      fx = fy = 0;
      for (j = 0; j < 32; j++)
      {
        ucBuffer[j] = rc4EcryptByte(fState, &fx, &fy, ucBuffer[j]);
      }
    }
  }
////////////////////////////
//
////////////////////////////
  if (o_pucEnDicOwPassw)
    memcpy(o_pucEnDicOwPassw, ucBuffer, 32);
  return true_a;
}

bool_a PDFCreateEncrDictUserPassw(const unsigned char *i_pUserPassword, const unsigned char *i_pucEnDicOwPassw, const unsigned char *i_pucFileID, int i_iPermission,
                                       unsigned char *o_pucEnDicUserPassw)
{
  unsigned char fState[256];
  unsigned char fx, fy;
  unsigned char ucBuffer[ulPDFPasswordLen];
  unsigned char ucEncrKey[ulPDFPasswordLen];
  unsigned char ucTmpKey[ulPDFPasswordLen];
  short i, j;
  MD5_CTX mdContext;

////////////////////////////
// step 1 Alg. 3.5
////////////////////////////
  if (!PDFCreateEncrKey(i_pUserPassword, i_pucEnDicOwPassw, i_pucFileID, i_iPermission,
                        ucEncrKey))
    return false_a;
////////////////////////////
// step 2 Alg. 3.5
////////////////////////////
  memcpy(ucBuffer, PDFPasswordPad, 32);
  MD5Init (&mdContext);
  MD5Update (&mdContext, ucBuffer, 32);
////////////////////////////
// step 3 Alg. 3.5
////////////////////////////
  MD5Update (&mdContext, (unsigned char *)i_pucFileID, 16);
  MD5Final(&mdContext);
  memset(ucBuffer, 0, 32);
  memcpy(ucBuffer, mdContext.digest, 16);
////////////////////////////
// step 4 Alg. 3.5
////////////////////////////
  rc4InitKey(ucEncrKey, 16, fState);
  fx = fy = 0;
  for (i = 0; i < 16; i++)
  {
    ucBuffer[i] = rc4EcryptByte(fState, &fx, &fy, ucBuffer[i]);
  }
////////////////////////////
// step 5 Alg. 3.5
////////////////////////////
//  if (iEncRevision == 3)
  {
    for (i = 1; i <= 19; i++)
    {
      for (j = 0; j < 16; j++)
      {
        ucTmpKey[j] = ucEncrKey[j] ^ (unsigned char)i;
      }

      rc4InitKey(ucTmpKey, 16, fState);
      fx = fy = 0;
      for (j = 0; j < 16; ++j)
      {
        ucBuffer[j] = rc4EcryptByte(fState, &fx, &fy, ucBuffer[j]);
      }
    }
  }
////////////////////////////
// step 6 Alg. 3.5
////////////////////////////
  if (o_pucEnDicUserPassw)
    memcpy(o_pucEnDicUserPassw, ucBuffer, 32);
  return true_a;
}







//------------------------------------------------------------------------
// RC4-compatible decryption
//------------------------------------------------------------------------
static void rc4InitKey(unsigned char *key, int keyLen, unsigned char *state)
{
  unsigned char index1, index2;
  unsigned char t;
  short i;

  for (i = 0; i < 256; ++i)
    state[i] = (unsigned char)i;

  index1 = index2 = 0;
  for (i = 0; i < 256; ++i)
  {
    index2 = (key[index1] + state[i] + index2) % 256;
    t = state[i];
    state[i] = state[index2];
    state[index2] = t;
    index1 = (index1 + 1) % (unsigned char)keyLen;
  }
}

static unsigned char rc4EcryptByte(unsigned char *state, unsigned char *x, unsigned char *y, unsigned char c)
{
  unsigned char x1, y1, tx, ty;
  x1 = *x = (*x + 1) % 256;
  y1 = *y = (state[*x] + *y) % 256;
  tx = state[x1];
  ty = state[y1];
  state[x1] = ty;
  state[y1] = tx;
  return c ^ state[(tx + ty) % 256];
}

/*
//------------------------------------------------------------------------
// MD5 message digest
//------------------------------------------------------------------------
// this works around a bug in older Sun compilers
static unsigned long rotateLeft(unsigned long x, int r)
{
  x &= 0xffffffff;
  return ((x << r) | (x >> (32 - r))) & 0xffffffff;
}

static unsigned long md5Round1(unsigned long a, unsigned long b, unsigned long c, unsigned long d, unsigned long Xk,  unsigned long s, unsigned long Ti)
{
  return b + rotateLeft((a + ((b & c) | (~b & d)) + Xk + Ti), s);
}

static unsigned long md5Round2(unsigned long a, unsigned long b, unsigned long c, unsigned long d, unsigned long Xk,  unsigned long s, unsigned long Ti)
{
  return b + rotateLeft((a + ((b & d) | (c & ~d)) + Xk + Ti), s);
}

static unsigned long md5Round3(unsigned long a, unsigned long b, unsigned long c, unsigned long d, unsigned long Xk,  unsigned long s, unsigned long Ti)
{
  return b + rotateLeft((a + (b ^ c ^ d) + Xk + Ti), s);
}

static unsigned long md5Round4(unsigned long a, unsigned long b, unsigned long c, unsigned long d, unsigned long Xk,  unsigned long s, unsigned long Ti)
{
  return b + rotateLeft((a + (c ^ (b | ~d)) + Xk + Ti), s);
}

static void md5(unsigned char *msg, int msgLen, unsigned char *digest)
{
  unsigned long x[16];
  unsigned long a, b, c, d, aa, bb, cc, dd;
  int n64;
  int i, j, k;

  // compute number of 64-byte blocks
  // (length + pad byte (0x80) + 8 bytes for length)
  n64 = (msgLen + 1 + 8 + 63) / 64;

  // initialize a, b, c, d
  a = 0x67452301;
  b = 0xefcdab89;
  c = 0x98badcfe;
  d = 0x10325476;

  // loop through blocks
  k = 0;
  for (i = 0; i < n64; ++i)
  {
    // grab a 64-byte block
    for (j = 0; j < 16 && k < msgLen - 3; ++j, k += 4)
      x[j] = (((((msg[k+3] << 8) + msg[k+2]) << 8) + msg[k+1]) << 8) + msg[k];

    if (i == n64 - 1)
    {
      if (k == msgLen - 3)
        x[j] = 0x80000000 + (((msg[k+2] << 8) + msg[k+1]) << 8) + msg[k];
      else if (k == msgLen - 2)
        x[j] = 0x800000 + (msg[k+1] << 8) + msg[k];
      else if (k == msgLen - 1)
        x[j] = 0x8000 + msg[k];
      else
        x[j] = 0x80;

      ++j;

      while (j < 16)
        x[j++] = 0;

      x[14] = msgLen << 3;
    }

    // save a, b, c, d
    aa = a;
    bb = b;
    cc = c;
    dd = d;

    // round 1
    a = md5Round1(a, b, c, d, x[0],   7, 0xd76aa478);
    d = md5Round1(d, a, b, c, x[1],  12, 0xe8c7b756);
    c = md5Round1(c, d, a, b, x[2],  17, 0x242070db);
    b = md5Round1(b, c, d, a, x[3],  22, 0xc1bdceee);
    a = md5Round1(a, b, c, d, x[4],   7, 0xf57c0faf);
    d = md5Round1(d, a, b, c, x[5],  12, 0x4787c62a);
    c = md5Round1(c, d, a, b, x[6],  17, 0xa8304613);
    b = md5Round1(b, c, d, a, x[7],  22, 0xfd469501);
    a = md5Round1(a, b, c, d, x[8],   7, 0x698098d8);
    d = md5Round1(d, a, b, c, x[9],  12, 0x8b44f7af);
    c = md5Round1(c, d, a, b, x[10], 17, 0xffff5bb1);
    b = md5Round1(b, c, d, a, x[11], 22, 0x895cd7be);
    a = md5Round1(a, b, c, d, x[12],  7, 0x6b901122);
    d = md5Round1(d, a, b, c, x[13], 12, 0xfd987193);
    c = md5Round1(c, d, a, b, x[14], 17, 0xa679438e);
    b = md5Round1(b, c, d, a, x[15], 22, 0x49b40821);

    // round 2
    a = md5Round2(a, b, c, d, x[1],   5, 0xf61e2562);
    d = md5Round2(d, a, b, c, x[6],   9, 0xc040b340);
    c = md5Round2(c, d, a, b, x[11], 14, 0x265e5a51);
    b = md5Round2(b, c, d, a, x[0],  20, 0xe9b6c7aa);
    a = md5Round2(a, b, c, d, x[5],   5, 0xd62f105d);
    d = md5Round2(d, a, b, c, x[10],  9, 0x02441453);
    c = md5Round2(c, d, a, b, x[15], 14, 0xd8a1e681);
    b = md5Round2(b, c, d, a, x[4],  20, 0xe7d3fbc8);
    a = md5Round2(a, b, c, d, x[9],   5, 0x21e1cde6);
    d = md5Round2(d, a, b, c, x[14],  9, 0xc33707d6);
    c = md5Round2(c, d, a, b, x[3],  14, 0xf4d50d87);
    b = md5Round2(b, c, d, a, x[8],  20, 0x455a14ed);
    a = md5Round2(a, b, c, d, x[13],  5, 0xa9e3e905);
    d = md5Round2(d, a, b, c, x[2],   9, 0xfcefa3f8);
    c = md5Round2(c, d, a, b, x[7],  14, 0x676f02d9);
    b = md5Round2(b, c, d, a, x[12], 20, 0x8d2a4c8a);

    // round 3
    a = md5Round3(a, b, c, d, x[5],   4, 0xfffa3942);
    d = md5Round3(d, a, b, c, x[8],  11, 0x8771f681);
    c = md5Round3(c, d, a, b, x[11], 16, 0x6d9d6122);
    b = md5Round3(b, c, d, a, x[14], 23, 0xfde5380c);
    a = md5Round3(a, b, c, d, x[1],   4, 0xa4beea44);
    d = md5Round3(d, a, b, c, x[4],  11, 0x4bdecfa9);
    c = md5Round3(c, d, a, b, x[7],  16, 0xf6bb4b60);
    b = md5Round3(b, c, d, a, x[10], 23, 0xbebfbc70);
    a = md5Round3(a, b, c, d, x[13],  4, 0x289b7ec6);
    d = md5Round3(d, a, b, c, x[0],  11, 0xeaa127fa);
    c = md5Round3(c, d, a, b, x[3],  16, 0xd4ef3085);
    b = md5Round3(b, c, d, a, x[6],  23, 0x04881d05);
    a = md5Round3(a, b, c, d, x[9],   4, 0xd9d4d039);
    d = md5Round3(d, a, b, c, x[12], 11, 0xe6db99e5);
    c = md5Round3(c, d, a, b, x[15], 16, 0x1fa27cf8);
    b = md5Round3(b, c, d, a, x[2],  23, 0xc4ac5665);

    // round 4
    a = md5Round4(a, b, c, d, x[0],   6, 0xf4292244);
    d = md5Round4(d, a, b, c, x[7],  10, 0x432aff97);
    c = md5Round4(c, d, a, b, x[14], 15, 0xab9423a7);
    b = md5Round4(b, c, d, a, x[5],  21, 0xfc93a039);
    a = md5Round4(a, b, c, d, x[12],  6, 0x655b59c3);
    d = md5Round4(d, a, b, c, x[3],  10, 0x8f0ccc92);
    c = md5Round4(c, d, a, b, x[10], 15, 0xffeff47d);
    b = md5Round4(b, c, d, a, x[1],  21, 0x85845dd1);
    a = md5Round4(a, b, c, d, x[8],   6, 0x6fa87e4f);
    d = md5Round4(d, a, b, c, x[15], 10, 0xfe2ce6e0);
    c = md5Round4(c, d, a, b, x[6],  15, 0xa3014314);
    b = md5Round4(b, c, d, a, x[13], 21, 0x4e0811a1);
    a = md5Round4(a, b, c, d, x[4],   6, 0xf7537e82);
    d = md5Round4(d, a, b, c, x[11], 10, 0xbd3af235);
    c = md5Round4(c, d, a, b, x[2],  15, 0x2ad7d2bb);
    b = md5Round4(b, c, d, a, x[9],  21, 0xeb86d391);

    // increment a, b, c, d
    a += aa;
    b += bb;
    c += cc;
    d += dd;
  }

  // break digest into bytes
  digest[0] = a & 0xff;
  digest[1] = (a >>= 8) & 0xff;
  digest[2] = (a >>= 8) & 0xff;
  digest[3] = (a >>= 8) & 0xff;
  digest[4] = b & 0xff;
  digest[5] = (b >>= 8) & 0xff;
  digest[6] = (b >>= 8) & 0xff;
  digest[7] = (b >>= 8) & 0xff;
  digest[8] = c & 0xff;
  digest[9] = (c >>= 8) & 0xff;
  digest[10] = (c >>= 8) & 0xff;
  digest[11] = (c >>= 8) & 0xff;
  digest[12] = d & 0xff;
  digest[13] = (d >>= 8) & 0xff;
  digest[14] = (d >>= 8) & 0xff;
  digest[15] = (d >>= 8) & 0xff;
}
*/
