
#include "PDFTTFFont.h"
#include "PDFTTFFontDesc.h"
#include "PDFDocument.h"
#include "pdffnc.h"

#include <string>

using namespace std;



CPDFTrueTypeFont::CPDFTrueTypeFont(const char *i_pFontName, long i_lID, 
                            long i_lStyle, long i_lCodePage, CPDFDocument *i_pDoc)
  :CPDFBaseFont(i_pFontName, i_lID, i_lStyle, i_lCodePage, i_pDoc)
{
  m_lDescendantFontsObjNum = NOT_USED;
  m_eFontType = eTrueType;
}

CPDFTrueTypeFont::~CPDFTrueTypeFont()
{
}

CPDFBaseFont *CPDFTrueTypeFont::MakeFont(const char *i_pFontName, long i_lID, long i_lStyle,
                                      long i_lCodePage, CPDFDocument *i_pDoc)
{
  CPDFTrueTypeFont *pRetFont = NULL;

  if (!i_pFontName || i_lID < 0)
    return NULL;

  pRetFont = new CPDFTrueTypeFont(i_pFontName, i_lID, i_lStyle, i_lCodePage, i_pDoc);
  if (!pRetFont)
    return NULL;

  if (!pRetFont->Init())
  {
    delete pRetFont;
    pRetFont = NULL;
  }

  return pRetFont;
}

bool_a CPDFTrueTypeFont::Init()
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

bool_a CPDFTrueTypeFont::PdfGetTTF(pdf_font *font, const char *fontname, long enc, long lFontFlags)
{
  if (pdf_parse_tt(fontname, font, enc, lFontFlags) == 0)
  {
    return false_a;
  }
  pdf_make_fontflag(font);
  return true_a;
}

CPDFFontDescriptor *CPDFTrueTypeFont::MakeDescriptor()
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

    sprintf(czTemp, "%04x", i);
    sprintf(czTemp1, "%04x", wc);
    m_mapToUnicode[czTemp] = czTemp1;
  }


  if (!PdfGetTTF(&font, m_strFontName.c_str(), 0, m_lFontStyle))
  {
    delete pRetDesc;
    return NULL;
  }

  //for (font.t
  //CTrueTypeTables

  //TODO; get right name
  m_strBaseName = font.ttname;

  m_iFirstChar = 0;
  m_iLastChar = 255;

  for (i = m_iFirstChar; i <= m_iLastChar; i++)
  {
    m_mapGlyphs[i].m_iWidth = font.widths[i];
    m_mapGlyphs[i].m_lGidx = font.lGdixs[i];
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

bool_a CPDFTrueTypeFont::HasFontToUnicodeStream()
{
  if (m_lToUnicodeObjNum > 0)
    return true_a;
  return false_a;
}

bool_a CPDFTrueTypeFont::HasFontDescendantFonts()
{
  return true_a;
}

bool_a CPDFTrueTypeFont::HasFontEncodingStream()
{
  if (m_pFontEncoding)
    return m_pFontEncoding->HasEncodingStream();
  return false_a;
}

void CPDFTrueTypeFont::AssignObjNum(long &io_lObjNum)
{
  SetFontObjNum(io_lObjNum++);
  //SetFontToUnicodeStreamObjNum(io_lObjNum++);
  //SetFontDescendantFontsStreamObjNum(io_lObjNum++);
  SetFontDescriptorObjNum(io_lObjNum++);
  SetFontEncodingObjNum(io_lObjNum++);
  if (HasFontEmbedStream())
    SetFontEmbedStreamObjNum(io_lObjNum++);
}

bool_a CPDFTrueTypeFont::GetEntireFontStream(vector<string> &o_vctFontStreams)
{
  o_vctFontStreams.clear();
  string strStream;

  GetFontStream(strStream);
  o_vctFontStreams.push_back(strStream);

  //if (HasFontToUnicodeStream())
  //{
  //  GetToUnicodeStream(strStream);
  //  o_vctFontStreams.push_back(strStream);
  //}

  //if (HasFontDescendantFonts())
  //{
  //  GetFontDescendantFontsStream(strStream);
  //  o_vctFontStreams.push_back(strStream);
  //}

  m_pFontDescriptor->GetDescriptorStream(strStream);
  o_vctFontStreams.push_back(strStream);

  m_pFontEncoding->GetEncodingStream(strStream);
  o_vctFontStreams.push_back(strStream);

  if (HasFontEmbedStream())
  {
    m_pFontDescriptor->GetEmbedStream(strStream);
    o_vctFontStreams.push_back(strStream);
  }

  return true_a;
}

void CPDFTrueTypeFont::AssignObjNumType0(long &io_lObjNum)
{
  SetFontObjNum(io_lObjNum++);
  SetFontToUnicodeStreamObjNum(io_lObjNum++);
  SetFontDescendantFontsStreamObjNum(io_lObjNum++);
  //  SetFontDescriptorObjNum(io_lObjNum++);
//SetFontEncodingObjNum(io_lObjNum++);
  //  SetFontEmbedStreamObjNum(io_lObjNum++);
}

bool_a CPDFTrueTypeFont::GetEntireFontStreamType0(vector<string> &o_vctFontStreams)
{
  o_vctFontStreams.clear();
  string strStream;

  GetFontType0Stream(strStream);
  o_vctFontStreams.push_back(strStream);

  GetToUnicodeStream(strStream);
  o_vctFontStreams.push_back(strStream);

  GetFontDescendantFontsStream(strStream);
  o_vctFontStreams.push_back(strStream);

  //m_pFontDescriptor->GetDescriptorStream(strStream);
  //o_vctFontStreams.push_back(strStream);

//m_pFontEncoding->GetEncodingStream(strStream);
//o_vctFontStreams.push_back(strStream);

  //m_pFontDescriptor->GetEmbedStream(strStream);
  //o_vctFontStreams.push_back(strStream);

  return true_a;
}

bool_a CPDFTrueTypeFont::GetFontStream(std::string &o_strFontStream)
{
//  return GetFontType0Stream(o_strFontStream);

  o_strFontStream.clear();
  char czTemp[200];

  
  if (m_lObjNum > 0)
  {
    sprintf(czTemp, "%ld 0 obj << /Type /Font ", m_lObjNum);
    o_strFontStream.append(czTemp);
  }
  if(m_eFontType == eTrueType)
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

  if (m_pFontEncoding)
  {
    if (m_pFontEncoding->HasEncodingStream())
    {
      sprintf(czTemp, "/Encoding %ld 0 R", m_pFontEncoding->GetEncodingObjNum());
      o_strFontStream.append(czTemp);
    }
    else
    if((m_pFontEncoding->GetEncoding() == CPDFFontEncoding::eWinAnsiEncoding ||
        m_pFontEncoding->GetEncoding() == CPDFFontEncoding::eMacRomanEncoding ||
        m_pFontEncoding->GetEncoding() == CPDFFontEncoding::eMacExpertEncoding))
    {
      o_strFontStream.append("/Encoding /");
      o_strFontStream.append(m_pFontEncoding->GetEncodingStr());
      o_strFontStream.append(" ");
    }
  }


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

bool_a CPDFTrueTypeFont::GetFontStreamWinEncoding(std::string &o_strFontStream)
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

bool_a CPDFTrueTypeFont::GetFontType0Stream(std::string &o_strFontStream)
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

bool_a CPDFTrueTypeFont::GetFontDescendantFontsStream(std::string &o_strDescendantFontsStream)
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
  for (int i = m_iFirstChar; i <= m_iLastChar; i++)
  {
    sprintf(czTemp, " %ld [ %ld ]", m_mapGlyphs[i].m_lGidx, (long)m_mapGlyphs[i].m_iWidth);
    o_strDescendantFontsStream.append(czTemp);
  }
  o_strDescendantFontsStream.append(" ] ");

  o_strDescendantFontsStream.append("/CIDSystemInfo << /Supplement 0 /Ordering (Identity) /Registry(Adobe) >> ");
  o_strDescendantFontsStream.append(">> endobj\n");

  return true_a;
}

bool_a CPDFTrueTypeFont::GetToUnicodeStream(std::string& o_strToUnicode)
{
  o_strToUnicode.clear();
  string str;
  char czTemp[100], czTemp1[100];

str.append("/CIDInit /ProcSet findresource begin\n");
str.append("3142 dict begin\n");
str.append("begincmap\n");
str.append("/CIDSystemInfo\n");
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
      str.insert(index, "400 beginbfchar\n");
    }

    if (m_mapGlyphs[i].m_lGidx > 0)
    {
      sprintf(czTemp1, "%04x", i);
      sprintf(czTemp, "<%04x> <%s>\n", m_mapGlyphs[i].m_lGidx, m_mapToUnicode[czTemp1].c_str());
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
    sprintf(czTemp, "%ld beginbfchar\n", j * 4);
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


  //if (!PDFEncrypt((PDFDocPTR)m_pDoc, (unsigned char *)pObj, lSize,
  //                  lObjNum, lGenNum))
  //  return false_a;
  o_strToUnicode.append(str);

  o_strToUnicode.append("\nendstream\n");
  o_strToUnicode.append("endobj\n");


  return true_a;
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


