
#include "PDFType0Font.h"
#include "PDFTTFFontDesc.h"
#include "PDFDocument.h"
#include "pdffnc.h"

#include <string>

using namespace std;



CPDFType0Font::CPDFType0Font(const char *i_pFontName, long i_lID, 
                            long i_lStyle, long i_lCodePage, CPDFDocument *i_pDoc)
  :CPDFBaseFont(i_pFontName, i_lID, i_lStyle, i_lCodePage, i_pDoc)
{
  m_lDescendantFontsObjNum = NOT_USED;
  m_eFontType = eType0;
}

CPDFType0Font::~CPDFType0Font()
{
}

CPDFBaseFont *CPDFType0Font::MakeFont(const char *i_pFontName, long i_lID, long i_lStyle,
                                      long i_lCodePage, CPDFDocument *i_pDoc)
{
  CPDFType0Font *pRetFont = NULL;

  if (!i_pFontName || i_lID < 0)
    return NULL;

  pRetFont = new CPDFType0Font(i_pFontName, i_lID, i_lStyle, i_lCodePage, i_pDoc);
  if (!pRetFont)
    return NULL;

  if (!pRetFont->Init())
  {
    delete pRetFont;
    pRetFont = NULL;
  }

  return pRetFont;
}

bool_a CPDFType0Font::Init()
{
  char czTemp[100];


  sprintf(czTemp, "%s%ld", czFONT_NAME, m_lFontDocID);
  m_strResName = czTemp;

  m_pFontEncoding = new CPDFFontEncoding();
  m_pFontEncoding->SetEncoding(CPDFFontEncoding::eWinAnsiEncoding);

//  m_pFontEncoding->SetCodePage(m_lCodePage);

  m_pFontDescriptor = MakeDescriptor();
  if (!m_pFontDescriptor)
    return false_a;

  if (m_pDoc)
  {
    //CPDFFontDescriptor *pDesc = m_pDoc->GetFontDescriptor(m_strBaseName.c_str());
    //if (pDesc)
    //{
    //  delete m_pFontDescriptor;
    //  m_pFontDescriptor = pDesc;
    //} else
    {
      m_pDoc->AddFontDescriptor(m_pFontDescriptor, m_strBaseName.c_str());
    }
  }

  //m_mapToUnicode;

  return true_a;
}

bool_a CPDFType0Font::PdfGetTTF(pdf_font *font, const char *fontname, long enc, long lFontFlags)
{
  if (pdf_parse_tt(fontname, font, enc, lFontFlags) == 0)
  {
    return false_a;
  }
  pdf_make_fontflag(font);
  return true_a;
}

CPDFFontDescriptor *CPDFType0Font::MakeDescriptor()
{
  CPDFFontDescriptor *pRetDesc = NULL;

  pRetDesc = new CPDFTTFFontDescriptor(m_pDoc);
  if (!pRetDesc)
    return NULL;
  rec_a rec;

  pdf_font font;
  memset(&font, 0, sizeof(pdf_font));
  font.m_pmapUnicodeGidx = new std::map<long, long>;
  font.m_pmapUnicodeWidth = new std::map<long, long>;


  short i;
  char czTemp[10], czTemp1[10];
  //char czBuf[256];
  //wchar_t wczBuf[256];
  char c;
  wchar_t wc;
  string strIn;
  wstring wstrOut;
  wstring::iterator iter;


  for (i = 0, c = 0; i < 256; c++, i++)
  {
    strIn.append(&c, 1);
    //czBuf[i] = c;
  }
  if (!GetUnicodeValue(strIn, wstrOut))
  {
    delete font.m_pmapUnicodeGidx;
    delete font.m_pmapUnicodeWidth;
    delete pRetDesc;
    return NULL;
  }
  //if ( 0 == MultiByteToWideChar(m_lCodePage, 0, czBuf, 256, wczBuf, 256))
  //{
  //  delete pRetDesc;
  //  return NULL;
  //}

  //for (i = 0; i < 256; i++)
  for (i = 0; i < 256; i++)
  {
    //wc = wczBuf[i];
    wc = wstrOut.at(i);//*iter;
    if (wc != i)
    {
      m_pFontEncoding->SetCharCodeToGlyphName(i, PDFGetGlyphName(wc));
      font.uchar[i] = wc;
    }
    else
      font.uchar[i] = wc;
  }


  if (!PdfGetTTF(&font, m_strFontName.c_str(), 0, m_lFontStyle))
  {
    delete font.m_pmapUnicodeGidx;
    delete font.m_pmapUnicodeWidth;
    delete pRetDesc;
    return NULL;
  }

  //for (font.t
  //CTrueTypeTables

  //TODO; get right name
  m_strBaseName = font.ttname;

  m_iFirstChar = 0;
  m_iLastChar = 255;

  std::map<long, long>::iterator iterUnicode, iterWidth;
  for (iterUnicode = font.m_pmapUnicodeGidx->begin(); iterUnicode != font.m_pmapUnicodeGidx->end(); iterUnicode++)
  {
    if (0 == (long)iterUnicode->second)
      continue;
    iterWidth = font.m_pmapUnicodeWidth->find(iterUnicode->first);
    if (iterWidth == font.m_pmapUnicodeWidth->end())
      continue;
    m_mapGlyphs[(long)iterUnicode->first].m_iWidth = (int)iterWidth->second;
    m_mapGlyphs[(long)iterUnicode->first].m_lGidx = iterUnicode->second;
    m_mapGlyphs[(long)iterUnicode->first].m_bUsed = false_a;

    sprintf(czTemp, "%04X", (long)iterUnicode->second);
    sprintf(czTemp1, "%04X", (long)iterUnicode->first);
    m_mapToUnicode[czTemp] = czTemp1;

    //font.m_pmapUnicodeGidx;
    //font.m_pmapUnicodeWidth
  }

  for (i = m_iFirstChar; i <= m_iLastChar; i++)
  {
    m_mapGlyphs[i].m_iWidth = font.widths[i];
    m_mapGlyphs[i].m_lGidx = font.lGdixs[i];
    m_mapGlyphs[i].m_bUsed = false_a;

    sprintf(czTemp, "%04X", font.lGdixs[i]);
    sprintf(czTemp1, "%04X", font.uchar[i]);
    m_mapToUnicode[czTemp] = czTemp1;
  }



  rec.lLeft = font.llx;
  rec.lBottom = font.lly;
  rec.lRight = font.urx;
  rec.lTop = font.ury;

  string str;
  str.assign((const char *)font.img, font.filelen);
  pRetDesc->SetEmbedStream(str);

  pRetDesc->SetFontName(m_strBaseName.c_str());
  pRetDesc->SetFlags(font.flags);
  pRetDesc->SetFontBBox(rec);
  pRetDesc->SetItalicAngle(font.italicAngle);
  pRetDesc->SetAscent(font.ascender/*rec.lTop*/);
  pRetDesc->SetDescent(font.descender/*rec.lBottom*/);
  pRetDesc->SetCapHeight(font.capHeight);
  pRetDesc->SetXHeight(font.xHeight);
  pRetDesc->SetStemV(font.StdVW);
  pRetDesc->SetStemH(font.StdHW);

  pRetDesc->SetUnderlinePosition(font.underlinePosition);
  pRetDesc->SetUnderlineThickness(font.underlineThickness);

  free(font.ttname);
  free(font.pFontName);
  free(font.encodingScheme);
  free(font.img);
  if (font.m_pmapUnicodeGidx)
    delete font.m_pmapUnicodeGidx;
  if (font.m_pmapUnicodeWidth)
    delete font.m_pmapUnicodeWidth;

  return pRetDesc;
}

bool_a CPDFType0Font::HasFontToUnicodeStream()
{
  if (m_lToUnicodeObjNum > 0)
    return true_a;
  return false_a;
}

bool_a CPDFType0Font::HasFontDescendantFonts()
{
  return true_a;
}

bool_a CPDFType0Font::HasFontEncodingStream()
{
  if (m_pFontEncoding)
    return m_pFontEncoding->HasEncodingStream();
  return false_a;
}

void CPDFType0Font::AssignObjNum(long &io_lObjNum)
{
  return AssignObjNumType0(io_lObjNum);

//  SetFontObjNum(io_lObjNum++);
//  SetFontDescendantFontsStreamObjNum(io_lObjNum++);
//  SetFontToUnicodeStreamObjNum(io_lObjNum++);
//  SetFontDescriptorObjNum(io_lObjNum++);
////  SetFontEncodingObjNum(io_lObjNum++);
//  if (HasFontEmbedStream())
//    SetFontEmbedStreamObjNum(io_lObjNum++);
}

bool_a CPDFType0Font::GetEntireFontStream(vector<string> &o_vctFontStreams)
{
  return GetEntireFontStreamType0(o_vctFontStreams);

  //o_vctFontStreams.clear();
  //string strStream;

  //GetFontStream(strStream);
  //o_vctFontStreams.push_back(strStream);

  ////if (HasFontToUnicodeStream())
  ////{
  ////  GetToUnicodeStream(strStream);
  ////  o_vctFontStreams.push_back(strStream);
  ////}

  ////if (HasFontDescendantFonts())
  ////{
  ////  GetFontDescendantFontsStream(strStream);
  ////  o_vctFontStreams.push_back(strStream);
  ////}

  //m_pFontDescriptor->GetDescriptorStream(strStream);
  //o_vctFontStreams.push_back(strStream);

  //m_pFontEncoding->GetEncodingStream(strStream);
  //o_vctFontStreams.push_back(strStream);

  //if (HasFontEmbedStream())
  //{
  //  m_pFontDescriptor->GetEmbedStream(strStream);
  //  o_vctFontStreams.push_back(strStream);
  //}

  //return true_a;
}

void CPDFType0Font::AssignObjNumType0(long &io_lObjNum)
{
  SetFontObjNum(io_lObjNum++);
  SetFontDescendantFontsStreamObjNum(io_lObjNum++);
  SetFontToUnicodeStreamObjNum(io_lObjNum++);
  SetFontDescriptorObjNum(io_lObjNum++);
//SetFontEncodingObjNum(io_lObjNum++);
  if (HasFontEmbedStream())
    SetFontEmbedStreamObjNum(io_lObjNum++);
}

bool_a CPDFType0Font::GetEntireFontStreamType0(vector<string> &o_vctFontStreams)
{
  o_vctFontStreams.clear();
  string strStream;

  GetFontType0Stream(strStream);
  o_vctFontStreams.push_back(strStream);

  GetFontDescendantFontsStream(strStream);
  o_vctFontStreams.push_back(strStream);

  GetToUnicodeStream(strStream);
  o_vctFontStreams.push_back(strStream);

  m_pFontDescriptor->GetDescriptorStream(strStream);
  o_vctFontStreams.push_back(strStream);

//m_pFontEncoding->GetEncodingStream(strStream);
//o_vctFontStreams.push_back(strStream);

  if (HasFontEmbedStream())
  {
    m_pFontDescriptor->GetEmbedStream(strStream);
    o_vctFontStreams.push_back(strStream);
  }

  return true_a;
}

bool_a CPDFType0Font::GetFontStream(std::string &o_strFontStream)
{
//  return GetFontType0Stream(o_strFontStream);

  o_strFontStream.clear();
  char czTemp[200];

  
  if (m_lObjNum > 0)
  {
    sprintf(czTemp, "%ld 0 obj << /Type /Font ", m_lObjNum);
    o_strFontStream.append(czTemp);
  }
  if(m_eFontType == eType0)
    o_strFontStream.append("/Subtype /Type0 ");

  o_strFontStream.append("/Encoding /Identity-H ");

//  "/DescendantFonts 41 0 R/BaseFont/UZAFCF+Arial/ToUnicode 42 0 R>>"

  if (!m_strBaseName.empty())
  {
    o_strFontStream.append("/BaseFont /");
    o_strFontStream.append(m_strBaseName.c_str());

    if (m_lFontStyle & ePDFItalic && m_lFontStyle & ePDFBold)
    {
      o_strFontStream.append(",BoldItalic ");
    }
    else
      if (m_lFontStyle & ePDFItalic)
      {
        o_strFontStream.append(",Italic ");
      }
      else
        if(m_lFontStyle & ePDFBold)
        {
          o_strFontStream.append(",Bold ");
        }

  }

  if (m_pFontDescriptor)
  {
    sprintf(czTemp, "/FontDescriptor %ld 0 R ", m_pFontDescriptor->GetObjNum());
    o_strFontStream.append(czTemp);
  }

  if (HasFontToUnicodeStream())
  {
    sprintf(czTemp, "/ToUnicode %ld 0 R ", m_lToUnicodeObjNum);
    o_strFontStream.append(czTemp);
  }

  o_strFontStream.append(">> endobj\n");

  return true_a;
}

bool_a CPDFType0Font::GetFontStreamWinEncoding(std::string &o_strFontStream)
{
  char czTemp[200];

  o_strFontStream.clear();

  sprintf(czTemp, "%ld 0 obj << /Type /Font ", m_lObjNum);
  o_strFontStream.append(czTemp);
  o_strFontStream.append("/Subtype /TrueType ");
  sprintf(czTemp, "/FirstChar %ld /LastChar %ld ", m_iFirstChar, m_iLastChar);
  o_strFontStream.append(czTemp);
  o_strFontStream.append("/Widths[ ");
  for (int i = m_iFirstChar; i <= m_iLastChar; i++)
  {
    sprintf(czTemp, "%ld ", (long)m_mapGlyphs[i].m_iWidth);
    o_strFontStream.append(czTemp);
  }
  o_strFontStream.append("] ");
  o_strFontStream.append("/Encoding/WinAnsiEncoding");

  if (!m_strBaseName.empty())
  {
    o_strFontStream.append("/BaseFont /");
    o_strFontStream.append(m_strBaseName.c_str());

    if (m_lFontStyle & ePDFItalic && m_lFontStyle & ePDFBold)
    {
      o_strFontStream.append(",BoldItalic ");
    }
    else
      if (m_lFontStyle & ePDFItalic)
      {
        o_strFontStream.append(",Italic ");
      }
      else
        if(m_lFontStyle & ePDFBold)
        {
          o_strFontStream.append(",Bold ");
        }
  }

  if (m_pFontDescriptor)
  {
    sprintf(czTemp, "/FontDescriptor %ld 0 R ", m_pFontDescriptor->GetObjNum());
    o_strFontStream.append(czTemp);
  }
  o_strFontStream.append(">> endobj\n");

  return true_a;
}

bool_a CPDFType0Font::GetFontType0Stream(std::string &o_strFontStream)
{
  o_strFontStream.clear();
  char czTemp[200];

  if (m_lObjNum > 0)
  {
    sprintf(czTemp, "%ld 0 obj << /Type /Font ", m_lObjNum);
    o_strFontStream.append(czTemp);
  }

  o_strFontStream.append("/Subtype /Type0 ");

  sprintf(czTemp, "/DescendantFonts [ %ld 0 R ]", GetFontDescendantFontsStreamObjNum());
  o_strFontStream.append(czTemp);

  if (!m_strBaseName.empty())
  {
    o_strFontStream.append("/BaseFont /");
    o_strFontStream.append(m_strBaseName.c_str());
  }

  if (HasFontToUnicodeStream())
  {
    sprintf(czTemp, "/ToUnicode %ld 0 R ", m_lToUnicodeObjNum);
    o_strFontStream.append(czTemp);
  }

  o_strFontStream.append("/Encoding/Identity-H ");
  o_strFontStream.append(">> endobj\n");

  return true_a;
}

bool_a CPDFType0Font::GetFontDescendantFontsStream(std::string &o_strDescendantFontsStream)
{
  o_strDescendantFontsStream.clear();
  char czTemp[200];

  sprintf(czTemp, "%ld 0 obj << /Subtype /CIDFontType2 /Type /Font /DW 1000 ", GetFontDescendantFontsStreamObjNum());
  o_strDescendantFontsStream.append(czTemp);

  if (m_pFontDescriptor)
  {
    sprintf(czTemp, "/FontDescriptor %ld 0 R ", m_pFontDescriptor->GetObjNum());
    o_strDescendantFontsStream.append(czTemp);
  }
  if (!m_strBaseName.empty())
  {
    o_strDescendantFontsStream.append("/BaseFont /");
    o_strDescendantFontsStream.append(m_strBaseName.c_str());
  }

  o_strDescendantFontsStream.append("/W [");
  //for (int i = m_iFirstChar; i <= m_iLastChar; i++)
  //{
  //  sprintf(czTemp, " %ld [ %ld ]", m_mapGlyphs[i].m_lGidx, (long)m_mapGlyphs[i].m_iWidth);
  //  o_strDescendantFontsStream.append(czTemp);
  //}

  std::map<long, glyph_info>::iterator iterGlyph;

  for (iterGlyph = m_mapGlyphs.begin(); iterGlyph != m_mapGlyphs.end(); iterGlyph++)
  {
    if (!iterGlyph->second.m_bUsed)
      continue;
    sprintf(czTemp, " %ld [ %ld ]", iterGlyph->second.m_lGidx, (long)iterGlyph->second.m_iWidth);
    o_strDescendantFontsStream.append(czTemp);
  }


  o_strDescendantFontsStream.append(" ] ");


  o_strDescendantFontsStream.append("/CIDSystemInfo << /Supplement 0 /Ordering (");
  sprintf(czTemp, "Identity");
  long lLen = PDFStrLenC(czTemp);
  if (m_pDoc->Encrypt())
    PDFEncrypt(m_pDoc, (unsigned char *)czTemp, lLen, GetFontDescendantFontsStreamObjNum(), 0);
  char *pTemp = PDFescapeSpecialCharsBinary(czTemp, lLen, &lLen);
  o_strDescendantFontsStream.append(pTemp, lLen);
  free(pTemp);

  o_strDescendantFontsStream.append(") /Registry(");
  sprintf(czTemp, "Adobe");
  lLen = PDFStrLenC(czTemp);
  if (m_pDoc->Encrypt())
    PDFEncrypt(m_pDoc, (unsigned char *)czTemp, lLen, GetFontDescendantFontsStreamObjNum(), 0);
  pTemp = PDFescapeSpecialCharsBinary(czTemp, lLen, &lLen);
  o_strDescendantFontsStream.append(pTemp, lLen);
  free(pTemp);

  o_strDescendantFontsStream.append(") >> ");
  o_strDescendantFontsStream.append(">> endobj\n");

  return true_a;
}

bool_a CPDFType0Font::GetToUnicodeStream(std::string& o_strToUnicode)
{
  o_strToUnicode.clear();
  string str;
  char czTemp[100], czTemp1[100];

str.append("/CIDInit /ProcSet findresource begin\n");
str.append("12 dict begin\n");
str.append("begincmap\n");
str.append("CIDSystemInfo\n");
str.append("<< /Registry (Adobe)\n");
str.append("/Ordering (UCS) /Supplement 0 >> def\n");
str.append("/CMapName /Adobe-Identity-UCS def\n");
str.append("/CMapType 2 def\n");
str.append("1 begincodespacerange\n");
str.append("<0000> <FFFF>\n");
str.append("endcodespacerange\n");

  size_t index;
  long j = 0, i = 0;
  std::map<long, glyph_info>::iterator iter;

  index = str.size();
  for (iter = m_mapGlyphs.begin(); iter != m_mapGlyphs.end(); iter++, i++)
  {
    j++;

    if (j == 100)
    {
      str.insert(index, "100 beginbfchar\n");
    }

    if (m_mapGlyphs[i].m_lGidx > 0 && m_mapGlyphs[i].m_bUsed)
    {
      sprintf(czTemp1, "%04X", m_mapGlyphs[i].m_lGidx);
      sprintf(czTemp, "<%04X> <%s>\n", m_mapGlyphs[i].m_lGidx, m_mapToUnicode[czTemp1].c_str());
      str.append(czTemp);
    } else
      j--;


    if (j == 100)
    {
      str.append("endbfchar\n");
      index = str.size();
      j = 0;
    }
  }
  if (j > 0)
  {
    sprintf(czTemp, "%ld beginbfchar\n", j);
    str.insert(index, czTemp);
    str.append("endbfchar\n");
  }


str.append("endcmap CMapName currentdict /CMap defineresource pop end end");


  //if (!CPDFDocument::CompressStream(str))
  //  return false_a;

  if (m_lToUnicodeObjNum > 0)
  {
    sprintf(czTemp, "%ld 0 obj\n", m_lToUnicodeObjNum);
    o_strToUnicode.append(czTemp);
  }
  //o_strToUnicode.append("<< /Filter /FlateDecode /Length ");
  o_strToUnicode.append("<< /Length ");

  sprintf(czTemp, "%ld ", (long)str.size());
  o_strToUnicode.append(czTemp);

  o_strToUnicode.append(">>\nstream\n");


  if (m_pDoc->Encrypt())
  {
    char *pObj = (char *)PDFMalloc(str.size());
    memcpy(pObj, str.c_str(), str.size());
    if (!PDFEncrypt((PDFDocPTR)m_pDoc, (unsigned char *)pObj, str.size(), m_lToUnicodeObjNum, 0))
    {
      o_strToUnicode.clear();
      return false_a;
    }
    str.assign(pObj, str.size());
    free(pObj);
  }
  o_strToUnicode.append(str);

  o_strToUnicode.append("\nendstream\n");
  o_strToUnicode.append("endobj\n");


  return true_a;
}

bool_a CPDFType0Font::GetGidxString(CPDFString& i_str, string& o_strOut)
{
  if (i_str.Empty())
    return false_a;
  if (m_mapGlyphs.empty())
    return false_a;
  std::map<long, glyph_info>::iterator iter1;
  o_strOut.clear();
  switch (i_str.GetStringType())
  {
  case CPDFChar::eMultibyteString:
  {
    string strIn(*i_str.GetString());
    char czTemp[30];
    o_strOut.append("<");
    for (string::iterator iter = strIn.begin(); iter != strIn.end(); iter++)
    {
      iter1 = m_mapGlyphs.find((unsigned char)*iter);
      if (iter1 != m_mapGlyphs.end())
      {
        m_mapGlyphs[(unsigned char)*iter].m_bUsed = true_a;
        sprintf(czTemp, "%04X", (long)iter1->second.m_lGidx);
        o_strOut.append(czTemp);
      }
    }
    o_strOut.append(">");
    return true_a;
    break;
  }
  case CPDFChar::eWideString:
  {
    wstring strIn(*i_str.GetWString());
    char czTemp[30];
    o_strOut.append("<");
    for (wstring::iterator iter = strIn.begin(); iter != strIn.end(); iter++)
    {
      iter1 = m_mapGlyphs.find((wchar_t)*iter);
      if (iter1 != m_mapGlyphs.end())
      {
        m_mapGlyphs[(wchar_t)*iter].m_bUsed = true_a;
        sprintf(czTemp, "%04X", (long)iter1->second.m_lGidx);
        o_strOut.append(czTemp);
      }
    }
    o_strOut.append(">");
    return true_a;
    break;
  }
  default:
    break;
  }

  return false_a;
}




//typedef unsigned char  tt_byte;
//typedef signed char  tt_char;
//typedef unsigned short  tt_ushort;
//typedef signed short  tt_short;
//typedef unsigned int  tt_ulong;
//typedef signed int  tt_long;
//
//typedef signed long  tt_fixed;
//typedef signed short  tt_fword;
//typedef unsigned short  tt_ufword;
//
//
//
//
//
//tt_ushort tt_get_ushort(tt_file *ttf)
//{
//  tt_byte *pos = ttf->pos;
//  TT_IOCHECK(ttf, (ttf->pos += 2) <= ttf->end);
//  return (tt_ushort) (((tt_ushort) pos[0] << 8) | pos[1]);
//}


