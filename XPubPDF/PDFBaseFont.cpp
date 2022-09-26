
#include "PDFBaseFont.h"
#include "pdffnc.h"
#include "PDFDocument.h"

#include <string>
#include <sstream>

using namespace std;


#if !defined(WIN32)
//TODO: move it to CPDFDocument and merge it with MultiByteToWideChar
bool_a ConvertToUnicde(string& i_strIn, const char *i_czCodePage, wstring& o_wstrOut)
{
  size_t sizeIn(i_strIn.size());
  wchar_t *pwChar = (wchar_t *)malloc(sizeIn * sizeof(wchar_t));

  iconv_t cd = iconv_open( "UTF-16LE" , i_czCodePage  );
  if (cd == (iconv_t)(-1))
  {
    return false_a;
  }

  char *pIn = (char*)i_strIn.c_str();
  char *pOut = (char*)pwChar;
  int i;

  size_t LenOfString(sizeIn * sizeof(char));
  size_t LenOfWString(sizeIn * sizeof(wchar_t));

  //wide string from string
  size_t status = iconv(cd, &pIn, &LenOfString, &pOut, &LenOfWString);

  if (status == (size_t)(-1))
  {
    char *pIn = (char*)i_strIn.c_str();
    char *pOut = (char*)pwChar;
    for (i = 0; i < sizeIn; i++ )
    {
      pOut[2 * i] = pIn[i];
      pOut[2 * i + 1] = 0;
    }
    
    o_wstrOut.assign(pwChar, i_strIn.size());
    free(pwChar);
    iconv_close(cd);
    return false_a;
  }

  short* pSString = (short*)pwChar;
  //stretch the string to double size
  for (i = sizeIn - 1; i >= 0; i-- )
  {
    pSString[ 2 * i ] = pSString[i];
    pSString[ 2 * i + 1] = 0;
  }

  o_wstrOut.assign((wchar_t *)pSString, sizeIn);

  free(pwChar);

  return true_a;
}
#endif


CPDFBaseFont::CPDFBaseFont(const char *i_pFontName, long i_lID, 
                            long i_lStyle, long i_lCodePage, CPDFDocument *i_pDoc)
{
  m_lObjNum = NOT_USED;
  m_lToUnicodeObjNum = NOT_USED;
  m_eFontType = eUnknown;
  m_strBaseName = i_pFontName;
  m_strFontName = i_pFontName;
  m_strResName = "";
  m_iFirstChar = NOT_USED;
  m_iLastChar = NOT_USED;
  m_pFontDescriptor = NULL;
  m_pFontEncoding = NULL;
  m_lFontDocID = i_lID;
  m_lFontStyle = i_lStyle;
  if (i_lCodePage > 0)
    m_lCodePage = i_lCodePage;
  else
    m_lCodePage = lDefCodePage;
  m_iDefComWidth = NOT_USED;
  m_pDoc = i_pDoc;

  m_bUsedInForm = false_a;
}

CPDFBaseFont::~CPDFBaseFont()
{
  if (m_pFontDescriptor)
  {
    delete m_pFontDescriptor;
    m_pFontDescriptor = NULL;
  }
  if (m_pFontEncoding)
  {
    delete m_pFontEncoding;
    m_pFontEncoding = NULL;
  }
}

CPDFBaseFont *CPDFBaseFont::MakeFont(const char *i_pFontName, long i_lID, long i_lStyle,
                                      long i_lCodePage, CPDFDocument *i_pDoc)
{
  CPDFBaseFont *pRetFont = NULL;

  if (!i_pFontName || i_lID < 0)
    return NULL;

  pRetFont = new CPDFBaseFont(i_pFontName, i_lID, i_lStyle, i_lCodePage, i_pDoc);
  if (!pRetFont)
    return NULL;

  if (!pRetFont->Init())
  {
    delete pRetFont;
    pRetFont = NULL;
  }

  return pRetFont;
}

short CPDFBaseFont::MapStandartFontStyle(short i_sIndex, long i_lStyle)
{
  short sIndex = i_sIndex;
  sIndex = sIndex % 4;
  switch (sIndex)
  {
  case 0:
    if (i_lStyle & ePDFBold && i_lStyle & ePDFItalic)
      i_sIndex += 3;
    else
      if (i_lStyle & ePDFBold)
        i_sIndex += 1;
      else
        if (i_lStyle & ePDFItalic)
          i_sIndex += 2;
    break;
  case 1:
    if (i_lStyle & ePDFBold && i_lStyle & ePDFItalic)
      i_sIndex += 2;
    else if (i_lStyle & ePDFItalic)
      i_sIndex += 1;
    else if (!(i_lStyle & ePDFBold))
      i_sIndex -= 1;
    break;
  case 2:
    if (i_lStyle & ePDFBold && i_lStyle & ePDFItalic)
      i_sIndex += 1;
    else if (i_lStyle & ePDFBold)
      i_sIndex -= 1;
    else if (!(i_lStyle & ePDFItalic))
      i_sIndex -= 2;
    break;
  case 3:
    if ((i_lStyle & ePDFBold) && !(i_lStyle & ePDFItalic))
      i_sIndex -= 2;
    else if (!(i_lStyle & ePDFBold) && (i_lStyle & ePDFItalic))
      i_sIndex -= 1;
    else if (!(i_lStyle & ePDFBold) && !(i_lStyle & ePDFItalic))
      i_sIndex -= 3;
    break;
  default:
    break;
  }

  return i_sIndex;
}

bool_a CPDFBaseFont::Init()
{
  short i, sDefFontIndex = NOT_USED;
  //char czTemp[100];

  for(i = 0; i < sSTANDART_FONT_COUNT; i++)
  {
    if (m_strFontName.compare(pczSTANDART_FONT_NAME[i][0]) == 0)
    {
      if (m_lFontStyle >= 0)
        sDefFontIndex = MapStandartFontStyle(i, m_lFontStyle);
      else
        sDefFontIndex = i;
      break;
    }
  }
  if (NOT_USED == sDefFontIndex)
    return false_a;

  ReadStdFont(sDefFontIndex);

  //sprintf(czTemp, "%s%ld", czFONT_NAME, m_lFontDocID);
  m_strResName = pczDEF_AFM_FONTS_SHORTCUT[sDefFontIndex];//czTemp;

  m_pFontDescriptor = MakeDescriptor(sDefFontIndex);
  if (!m_pFontDescriptor)
    return false_a;

  m_pFontEncoding = new CPDFFontEncoding();
  m_pFontEncoding->SetEncoding(CPDFFontEncoding::eWinAnsiEncoding);
  m_pFontEncoding->SetCodePage(m_lCodePage);

  m_eFontType = eType1Base14;
  //m_mapToUnicode;

  return true_a;
}

void CPDFBaseFont::ReadStdFont(short i_sDefFontIndex)
{
  m_strBaseName = pczSTANDART_FONT_NAME[i_sDefFontIndex][0];

  m_iFirstChar = 0;
  m_iLastChar = 268;
  glyph_info glyph;
  for (short i = m_iFirstChar; i <= m_iLastChar; i++)
  {
    glyph.m_iWidth = sSTANDART_FONT_WIDTH[i_sDefFontIndex][i];
    glyph.m_lGidx = i;
    m_mapGlyphs[i] = glyph;
  }
}

CPDFFontDescriptor *CPDFBaseFont::MakeDescriptor(short i_sFontIndex)
{
  rec_a rec;
  CPDFFontDescriptor *pRetDesc = NULL;
  pRetDesc = new CPDFFontDescriptor(m_pDoc);
  if (!pRetDesc)
    return NULL;

  rec.lLeft = STANDART_FONT_FONTBBOX[i_sFontIndex][0];
  rec.lBottom = STANDART_FONT_FONTBBOX[i_sFontIndex][1];
  rec.lRight = STANDART_FONT_FONTBBOX[i_sFontIndex][2];
  rec.lTop = STANDART_FONT_FONTBBOX[i_sFontIndex][3];

  pRetDesc->SetFontName(m_strBaseName.c_str());
  //pRetDesc->GetFlags(long i_lFlags);
  pRetDesc->SetFontBBox(rec);
  //pRetDesc->SetItalicAngle(short i_sAngle);
  pRetDesc->SetAscent(sSTANDART_FONT_ASCENT[i_sFontIndex]);
  pRetDesc->SetDescent(sSTANDART_FONT_DESCENT[i_sFontIndex]);
  //pRetDesc->SetCapHeight(short i_sCapHeight);
  //pRetDesc->SetXHeight(short i_sXHeight);
  //pRetDesc->SetStemV(short i_sStemV);
  //pRetDesc->SetStemH(short i_sStemH);

  pRetDesc->SetUnderlinePosition(sSTANDART_FONT_UNDERLINEPOS[i_sFontIndex]);
  pRetDesc->SetUnderlineThickness(sSTANDART_FONT_UNDERLINETHICKNESS[i_sFontIndex]);

  return pRetDesc;
}




short CPDFBaseFont::GetFontUnderlineThickness()
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->GetUnderlineThickness();
  return 0;
}

short CPDFBaseFont::GetFontUnderlinePosition()
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->GetUnderlinePosition();
  return 0;
}

rec_a CPDFBaseFont::GetFontBBox()
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->GetFontBBox();
  rec_a rec = {0};
  return rec;
}

long CPDFBaseFont::GetFontHeight(float i_fSize)
{
  float fHeight = 0;
  if (!m_pFontDescriptor)
    return (long)fHeight;
  fHeight = (float)(GetFontAscent() - GetFontDescent());
  return PDFRound(fHeight * i_fSize);
}

float CPDFBaseFont::GetFontAscent()
{
  float fAscent = 0;
  if (!m_pFontDescriptor)
    return fAscent;
  fAscent = (float)(m_pFontDescriptor->GetAscent() / 1000.0);
  return fAscent;
}

long CPDFBaseFont::GetFontAscentHeight(float i_fSize)
{
  float fAscent = 0;
  if (!m_pFontDescriptor)
    return (long)fAscent;
  fAscent = GetFontAscent();
  return PDFRound(fAscent * i_fSize);
}

float CPDFBaseFont::GetFontDescent()
{
  float fDescent = 0;
  if (!m_pFontDescriptor)
    return fDescent;
  fDescent = (float)(m_pFontDescriptor->GetDescent() / 1000.0);
  return fDescent;
}

long CPDFBaseFont::GetFontDescentHeight(float i_fSize)
{
  float fDescent = 0;
  if (!m_pFontDescriptor)
    return (long)fDescent;
  fDescent = GetFontDescent();
  return PDFRound(fDescent * i_fSize);
}

void CPDFBaseFont::SetToUnicode(std::string i_strFrom, std::string i_strTo)
{
  m_mapToUnicode[i_strFrom] = i_strTo;
}

const char *CPDFBaseFont::GetToUnicode(std::string i_strChar)
{
  std::map<std::string, std::string>::iterator iter;
  iter = m_mapToUnicode.find(i_strChar);
  if (iter != m_mapToUnicode.end())
    return iter->second.c_str();
  if (m_pFontEncoding)
    return PDFGetGlyphNameUnicode(m_pFontEncoding->GetCharCodeToGlyphName(PDFConvertHEXToLong(i_strChar)));
  return "";
}

size_t CPDFBaseFont::GetToUnicodeSize()
{
  return m_mapToUnicode.size();
}

size_t CPDFBaseFont::GetToUnicodeLen()
{
  return m_mapToUnicode.begin()->first.size();
}

int CPDFBaseFont::GetAverageWidth()
{
  int average = 0;

  if (!m_mapGlyphs.empty())
  {
    std::map<long, glyph_info>::iterator iter;
    for (iter = m_mapGlyphs.begin(); iter != m_mapGlyphs.end(); iter++)
    {
      average += iter->second.m_iWidth;
    }
    average = average / m_mapGlyphs.size();
  } else if (!m_mapComWidth.empty())
  {
    std::map<int, int>::iterator iter1;
    for (iter1 = m_mapComWidth.begin(); iter1 != m_mapComWidth.end(); iter1)
    {
      average += iter1->second;
    }
    average = average / m_mapComWidth.size();
  }
  return average;
}

int CPDFBaseFont::GetFontCharWidth(CPDFChar i_Char)
{
  std::map<long, glyph_info>::iterator iter;
  std::map<int, int>::iterator iter1;
  if (!m_mapGlyphs.empty())
  {
    if (i_Char.GetCharType() == CPDFChar::eMultibyteString)
      iter = m_mapGlyphs.find((unsigned char)i_Char.GetChar());
    else if (i_Char.GetCharType() == CPDFChar::eWideString)
      iter = m_mapGlyphs.find((wchar_t)i_Char.GetWChar());
    else
      iter = m_mapGlyphs.end();
    if (iter != m_mapGlyphs.end())
      return iter->second.m_iWidth;
  } else if (!m_mapComWidth.empty())
  {
    if (i_Char.GetCharType() == CPDFChar::eMultibyteString)
      iter1 = m_mapComWidth.find((unsigned char)i_Char.GetChar());
    else if (i_Char.GetCharType() == CPDFChar::eWideString)
      iter1 = m_mapComWidth.find((wchar_t)i_Char.GetWChar());
    else
      iter1 = m_mapComWidth.end();
    if (iter1 != m_mapComWidth.end())
      return iter1->second;
    return m_iDefComWidth;
  }
  return 0;
}

float CPDFBaseFont::GetFontSpaceWidth(float i_fSize, float i_fHScaling)
{
  float fRetWidth = 0.;
  CPDFChar pdfChar(' ');

  i_fSize *= 0.001f;
  int iWidth = GetFontCharWidth(pdfChar);
  if (iWidth == 0)
    iWidth = GetAverageWidth();
  fRetWidth = i_fSize * iWidth;
  fRetWidth *= (i_fHScaling / 100.0f);

  return fRetWidth;
}

float CPDFBaseFont::GetFontStringWidth(CPDFString i_Str, float i_fSize, float i_fCharSpacing,
                                float i_fWordSpacing, float i_fHScaling)
{
  if (i_Str.Empty())
    return 0.;

  float fRetWidth = 0.;
  CPDFChar pdfChar;
  size_t i;

  i_fSize *= 0.001f;

  for (i = 0; i < i_Str.Length(); i++)
  {
    pdfChar = i_Str.GetPDFChar(i);
    if (!pdfChar.Valid())
      continue;
    fRetWidth += i_fSize * GetFontCharWidth(pdfChar) + i_fCharSpacing;
    if(pdfChar.IsSpace())
      fRetWidth += i_fWordSpacing;
  }
  fRetWidth -= i_fCharSpacing;
  fRetWidth *= (i_fHScaling / 100.0f);

  return fRetWidth;
}

float CPDFBaseFont::GetFontLastSpaceWidth(CPDFString& i_Str, float i_fSize, float i_fCharSpacing,
                                float i_fWordSpacing, float i_fHScaling)
{
  if (i_Str.Empty())
    return 0.;

  float fRetWidth = 0.;
  CPDFChar pdfChar;
  size_t i;

  i_fSize *= 0.001f;

  for (i = 0; i < i_Str.Length(); i++)
  {
    pdfChar = i_Str.GetPDFChar(i);
    if (!pdfChar.Valid())
      continue;
    fRetWidth += i_fSize * GetFontCharWidth(pdfChar) + i_fCharSpacing;
    if(pdfChar.IsSpace())
      fRetWidth += i_fWordSpacing;
    else
      fRetWidth = 0;
  }
  fRetWidth -= i_fCharSpacing;
  fRetWidth *= (i_fHScaling / 100.0f);
  if (fRetWidth < 0)
    fRetWidth = 0;

  return fRetWidth;
}

float CPDFBaseFont::GetFontHexStringWidth(std::string i_str, float i_fSize, float i_fCharSpacing,
                                float i_fWordSpacing, float i_fHScaling)
{
  if (i_str.empty())
    return 0.;

  float fRetWidth = 0.;
  size_t HexSize = 1;//GetToUnicodeSize();
  string strTmp;
  long lTemp;
  CPDFChar pdfChar;

  i_fSize *= 0.001f;
  //TODO: encoding

  for (size_t i = 0; i < i_str.size(); i += HexSize)
  {
    strTmp.assign(i_str.c_str() + i, i_str.c_str() + i + HexSize);
    lTemp = PDFConvertHEXToLong(strTmp);
    pdfChar.SetChar((char)lTemp);
    //strTmp = GetToUnicode(strTmp);

    fRetWidth += i_fSize * GetFontCharWidth(pdfChar) + i_fCharSpacing;

    //lTemp = PDFConvertHEXToLong(strTmp);
    if(pdfChar.IsSpace())
      fRetWidth += i_fWordSpacing;

  }
  fRetWidth -= i_fCharSpacing;
  fRetWidth *= (i_fHScaling / 100.0f);


  return fRetWidth;
}

float CPDFBaseFont::CalcFontHeight(string i_str, float i_fHeight, float i_fWidth)
{
  if (i_str.empty())
    return 0.;

  float fRetWidth = 0.;
  CPDFChar pdfChar;

  //TODO: encoding

  for (string::iterator iter = i_str.begin(); iter != i_str.end(); iter++)
  {
    pdfChar.SetChar((unsigned char)*iter);
    fRetWidth += GetFontCharWidth(pdfChar);
  }

  fRetWidth *= 0.001f;
  fRetWidth = i_fWidth / fRetWidth;
  if (i_fHeight < fRetWidth)
    return i_fHeight;

  return fRetWidth;
}

void CPDFBaseFont::SetFontType(ePDFFontType i_eType)
{
  m_eFontType = i_eType;
}

void CPDFBaseFont::SetFontBaseName(const char *i_pczBaseName)
{
  m_strBaseName = i_pczBaseName;
}

void CPDFBaseFont::SetFontResName(const char *i_pczResName)
{
  m_strResName = i_pczResName;
}

void CPDFBaseFont::SetFontFirstChar(int i_iFirstChar)
{
  m_iFirstChar = i_iFirstChar;
}

void CPDFBaseFont::SetFontLastChar(int i_iLastChar)
{
  m_iLastChar = i_iLastChar;
}

void CPDFBaseFont::SetCharWidth(int i_iChar, int i_iWidth)
{
  m_mapGlyphs[i_iChar].m_iWidth = i_iWidth;
}

void CPDFBaseFont::SetFontDescriptor(CPDFFontDescriptor *i_pDesc)
{
  if (m_pFontDescriptor)
    delete m_pFontDescriptor;
  m_pFontDescriptor = i_pDesc;
}

void CPDFBaseFont::SetFontEncoding(CPDFFontEncoding *i_pEnc)
{
  if (m_pFontEncoding)
    delete m_pFontEncoding;
  m_pFontEncoding = i_pEnc;
}

int CPDFBaseFont::GetComWidth(int i_iChar)
{
  std::map<int, int>::iterator iter;
  iter = m_mapComWidth.find(i_iChar);
  if (iter != m_mapComWidth.end())
    return m_mapComWidth[i_iChar];
  return GetDefComWidth();
}

int CPDFBaseFont::GetDefComWidth()
{
  return m_iDefComWidth;
}

void CPDFBaseFont::SetComWidth(int i_iChar, int i_iWidth)
{
  m_mapComWidth[i_iChar] = i_iWidth;
}

void CPDFBaseFont::SetDefComWidth(int i_iWidth)
{
  m_iDefComWidth = i_iWidth;
}

bool_a CPDFBaseFont::Compare(const char *i_pFontName, long i_lID, 
                              long i_lStyle, long i_lCodePage)
{
  short i, sDefFontIndex = NOT_USED;
  string strFontName = i_pFontName;

  for(i = 0; i < sSTANDART_FONT_COUNT; i++)
  {
    if (strFontName.compare(pczSTANDART_FONT_NAME[i][0]) == 0)
    {
      sDefFontIndex = MapStandartFontStyle(i, i_lStyle);
      break;
    }
  }
  if (NOT_USED != sDefFontIndex)
  {
    if (m_strBaseName.compare(pczSTANDART_FONT_NAME[sDefFontIndex][0]) != 0)
      return false_a;
  }

  if (strFontName.compare(m_strFontName) != 0)
    return false_a;
  if (m_lFontStyle != i_lStyle)
    return false_a;
  if (i_lCodePage != m_lCodePage)
    return false_a;
  //TODO: compare m_eFontType
  //TODO: compare m_pFontEncoding

  return true_a;
}






bool_a CPDFBaseFont::GetFontStream(std::string &o_strFontStream)
{
  o_strFontStream.clear();
  char czTemp[200];

  if (m_lObjNum > 0)
  {
    sprintf(czTemp, "%ld 0 obj << /Type /Font ", m_lObjNum);
    o_strFontStream.append(czTemp);
  }
  if(m_eFontType == eType1Base14)
    o_strFontStream.append("/Subtype /Type1 ");
  if (!m_strBaseName.empty())
  {
    o_strFontStream.append("/BaseFont /");
    o_strFontStream.append(m_strBaseName.c_str());
    o_strFontStream.append(" ");
  }
  //m_iFirstChar - not written in eType1Base14
  //m_iLastChar - not written in eType1Base14
  //m_mapGlyphs - not written in eType1Base14
  //m_pFontDescriptor - not written in eType1Base14
  if (m_pFontEncoding)
  {
    if (m_pFontEncoding->HasEncodingStream())
    {
      sprintf(czTemp, "/Encoding %ld 0 R ", m_pFontEncoding->GetEncodingObjNum());
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

  //m_mapToUnicode - not written in eType1Base14

  o_strFontStream.append(">> endobj\n");

  return true_a;
}

bool_a CPDFBaseFont::GetFontStreamWinEncoding(std::string &o_strFontStream)
{
  o_strFontStream.clear();
  char czTemp[200];

  sprintf(czTemp, "%ld 0 obj << /Type /Font ", m_lObjNum);
  o_strFontStream.append(czTemp);
  o_strFontStream.append("/Subtype /Type1 ");
  o_strFontStream.append("/BaseFont /");
  o_strFontStream.append(m_strBaseName.c_str());
  o_strFontStream.append(" ");
  //m_iFirstChar - not written in eType1Base14
  //m_iLastChar - not written in eType1Base14
  //m_mapGlyphs - not written in eType1Base14
  //m_pFontDescriptor - not written in eType1Base14
  o_strFontStream.append("/Encoding/WinAnsiEncoding");

  //m_mapToUnicode - not written in eType1Base14

  o_strFontStream.append(">> endobj\n");

  return true_a;
}

long CPDFBaseFont::GetFontObjNum()
{
  return m_lObjNum;
}

void CPDFBaseFont::SetFontObjNum(long i_lObjNum)
{
  m_lObjNum = i_lObjNum;
}

void CPDFBaseFont::AssignObjNum(long &io_lObjNum)
{
  SetFontObjNum(io_lObjNum++);
  if (HasFontToUnicodeStream())
    SetFontToUnicodeStreamObjNum(io_lObjNum++);
  if (HasFontDescriptor())
    SetFontDescriptorObjNum(io_lObjNum++);
  if (HasFontEncodingStream())
    SetFontEncodingObjNum(io_lObjNum++);
  if (HasFontEmbedStream())
    SetFontEmbedStreamObjNum(io_lObjNum++);
}

bool_a CPDFBaseFont::GetEntireFontStream(std::vector<std::string> &o_vctFontStreams)
{
  o_vctFontStreams.clear();
  string strStream;

  GetFontStream(strStream);
  o_vctFontStreams.push_back(strStream);

  if (HasFontToUnicodeStream())
  {
    GetToUnicodeStream(strStream);
    o_vctFontStreams.push_back(strStream);
  }

  if (HasFontDescriptor())
  {
    m_pFontDescriptor->GetDescriptorStream(strStream);
    o_vctFontStreams.push_back(strStream);
  }

  if (HasFontEncodingStream())
  {
    m_pFontEncoding->GetEncodingStream(strStream);
    o_vctFontStreams.push_back(strStream);
  }

  if (HasFontEmbedStream())
  {
    m_pFontDescriptor->GetEmbedStream(strStream);
    o_vctFontStreams.push_back(strStream);
  }

  return true_a;
}


bool_a CPDFBaseFont::HasFontDescriptor()
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->HasDescriptorStream();
  return false_a;
}

bool_a CPDFBaseFont::GetDescriptorStream(std::string &o_strStream)
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->GetDescriptorStream(o_strStream);
  return false_a;
}

long CPDFBaseFont::GetFontDescriptorObjNum()
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->GetObjNum();
  return NOT_USED;
}

void CPDFBaseFont::SetFontDescriptorObjNum(long i_lObjNum)
{
  if (m_pFontDescriptor)
    m_pFontDescriptor->SetObjNum(i_lObjNum);
}



bool_a CPDFBaseFont::HasFontEmbedStream()
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->HasFontEmbedStream();
  return false_a;
}

bool_a CPDFBaseFont::GetEmbedStream(std::string& o_strEmbed)
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->GetEmbedStream(o_strEmbed);
  return false_a;
}

long CPDFBaseFont::GetFontEmbedStreamObjNum()
{
  if (m_pFontDescriptor)
    return m_pFontDescriptor->GetFontEmbedStreamObjNum();
  return NOT_USED;
}

void CPDFBaseFont::SetFontEmbedStreamObjNum(long i_lObjNum)
{
  if (m_pFontDescriptor)
    m_pFontDescriptor->SetFontEmbedStreamObjNum(i_lObjNum);
}

void CPDFBaseFont::SetEmbedded(bool_a i_bEmbed)
{
  if (m_pFontDescriptor)
    m_pFontDescriptor->SetEmbedded(i_bEmbed);
}




bool_a CPDFBaseFont::HasFontEncodingStream()
{
  if (m_pFontEncoding)
    return m_pFontEncoding->HasEncodingStream();
  return false_a;
}

bool_a CPDFBaseFont::GetFontEncodingStream(std::string& o_strEncoding)
{
  if (m_pFontEncoding)
    return m_pFontEncoding->GetEncodingStream(o_strEncoding);
  return false_a;
}

long CPDFBaseFont::GetFontEncodingObjNum()
{
  if (m_pFontEncoding)
    return m_pFontEncoding->GetEncodingObjNum();
  return NOT_USED;
}

void CPDFBaseFont::SetFontEncodingObjNum(long i_lObjNum)
{
  if (m_pFontEncoding)
    m_pFontEncoding->SetEncodingObjNum(i_lObjNum);
}

bool_a CPDFBaseFont::HasFontToUnicodeStream()
{
  return false_a;
}

bool_a CPDFBaseFont::GetToUnicodeStream(std::string& o_strToUnicode)
{
  return false_a;
}

long CPDFBaseFont::GetFontToUnicodeStreamObjNum()
{
  return m_lToUnicodeObjNum;
}

void CPDFBaseFont::SetFontToUnicodeStreamObjNum(long i_lObjNum)
{
  m_lToUnicodeObjNum = i_lObjNum;
}

bool_a CPDFBaseFont::GetUnicodeValue(string& i_strIn, wstring& o_wstrOut)
{
  o_wstrOut.clear();
  char cChar;
  wchar_t wChar;
  long lCodePage = m_lCodePage < 0 ? lDefCodePage : m_lCodePage;

//TODO: make common function for conversion to/from unicode (input will charset ...)
#if defined(WIN32)

  for (string::iterator iter = i_strIn.begin(); iter != i_strIn.end(); iter++)
  {
    cChar = (char)*iter;

    if ( 0 == MultiByteToWideChar(lCodePage, 0, &cChar, 1, &wChar, 1))
      return false_a;

    o_wstrOut.append(&wChar, 1);
  }
#else
  char czCodePage[100];
  sprintf(czCodePage, "CP%ld", lCodePage);

  string strTemp;
  wstring wstrTemp;

  
  for (string::iterator iter = i_strIn.begin(); iter != i_strIn.end(); iter++)
  {
    cChar = (char)*iter;
    strTemp.assign(&cChar, 1);

    ConvertToUnicde(strTemp, czCodePage, wstrTemp);
    
    o_wstrOut.append(wstrTemp);

  }

#endif

  return true_a;
}

bool_a CPDFBaseFont::GetUnicodeValue(string& i_strIn, string& o_strOut)
{
  o_strOut.clear();
  wstring wstrTemp;
  char cChar;
  wchar_t wChar;

  if (!GetUnicodeValue(i_strIn, wstrTemp))
    return false_a;

  cChar = (char)0xFE;
  o_strOut.append(&cChar, 1);
  cChar = (char)0xFF;
  o_strOut.append(&cChar, 1);

  for (wstring::iterator iter = wstrTemp.begin(); iter != wstrTemp.end(); iter++)
  {
    wChar = (wchar_t)*iter;

    cChar = wChar >> 8;
    o_strOut.append(&cChar, 1);
    cChar = (char)wChar;
    o_strOut.append(&cChar, 1);
  }

  return true_a;
}

bool_a CPDFBaseFont::GetUnicodeValue(std::wstring& i_wstrIn, std::string& o_strOut)
{
  o_strOut.clear();
  char cChar;
  wchar_t wChar;

  cChar = (char)0xFE;
  o_strOut.append(&cChar, 1);
  cChar = (char)0xFF;
  o_strOut.append(&cChar, 1);

  for (wstring::iterator iter = i_wstrIn.begin(); iter != i_wstrIn.end(); iter++)
  {
    wChar = (wchar_t)*iter;

    cChar = wChar >> 8;
    o_strOut.append(&cChar, 1);
    cChar = (char)wChar;
    o_strOut.append(&cChar, 1);
  }

  return true_a;
}

short CPDFBaseFont::GetBaseFontIndex(const char *i_pBaseFont)
{
  if (!i_pBaseFont)
    return NOT_USED;
  for (short i = 0; i < sSTANDART_FONT_COUNT; i++)
  {
    if (0 == strcmp(pczSTANDART_FONT_NAME[i][0], i_pBaseFont))
      return i;
  }
  return NOT_USED;
}

bool_a CPDFBaseFont::GetGidxString(CPDFString& i_str, string& o_strOut)
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
    o_strOut.append("(");
    for (string::iterator iter = strIn.begin(); iter != strIn.end(); iter++)
    {
      iter1 = m_mapGlyphs.find((unsigned char)*iter);
      if (iter1 != m_mapGlyphs.end())
      {
        m_mapGlyphs[(unsigned char)*iter].m_bUsed = true_a;
        sprintf(czTemp, "%c", (char)iter1->second.m_lGidx);
        o_strOut.append(czTemp);
      }
    }
    o_strOut.append(")");
    return true_a;
    break;
  }
  case CPDFChar::eWideString:
  {
    wstring strIn(*i_str.GetWString());
    char czTemp[30];
    o_strOut.append("(");
    for (wstring::iterator iter = strIn.begin(); iter != strIn.end(); iter++)
    {
      iter1 = m_mapGlyphs.find((wchar_t)*iter);
      if (iter1 != m_mapGlyphs.end())
      {
        m_mapGlyphs[(wchar_t)*iter].m_bUsed = true_a;
        sprintf(czTemp, "%c", (char)iter1->second.m_lGidx);
        o_strOut.append(czTemp);
      }
    }
    o_strOut.append(")");
    return true_a;
    break;
  }
  default:
    break;
  }

  return false_a;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CPDFFontEncoding::CPDFFontEncoding()
{
  m_eFontEncoding = CPDFFontEncoding::eStandardEncoding;
  m_lObjNum = NOT_USED;
}

CPDFFontEncoding::~CPDFFontEncoding()
{
}

const char *CPDFFontEncoding::GetEncodingStr()
{
  static string strEncoding;

  switch (m_eFontEncoding)
  {
  case eMacRomanEncoding:
    strEncoding = "MacRomanEncoding";
    break;
  case eWinAnsiEncoding:
    strEncoding = "WinAnsiEncoding";
    break;
  case eMacExpertEncoding:
    strEncoding = "MacExpertEncoding";
    break;
  case eNoEncoding:
  case eStandardEncoding:
  case ePDFDocEncoding:
  case eSymbolEncoding:
  case eZapfDingbatsEncoding:
  default:
    strEncoding = "";
    break;
  }

  return strEncoding.c_str();
}

void CPDFFontEncoding::SetEncoding(eFontEncodingType i_eEncoding)
{
  m_eFontEncoding = i_eEncoding;
}

const char *CPDFFontEncoding::GetCharCodeToGlyphName(int i_iChar)
{
  std::map<int, std::string>::iterator iter;
  iter = m_vctCharCodeToGlyphName.find(i_iChar);
  if (iter != m_vctCharCodeToGlyphName.end())
    return m_vctCharCodeToGlyphName[i_iChar].c_str();
  return PDFGetGlyphName(i_iChar);
}

void CPDFFontEncoding::SetCharCodeToGlyphName(int i_iChar, std::string i_strGlyph)
{
  m_vctCharCodeToGlyphName[i_iChar] = i_strGlyph;
}

bool_a CPDFFontEncoding::HasEncodingStream()
{
  if (!m_vctCharCodeToGlyphName.empty())
    return true_a;
  return false_a;
}

void CPDFFontEncoding::SetEncodingObjNum(long i_lObjNum)
{
  m_lObjNum = i_lObjNum;
}

long CPDFFontEncoding::GetEncodingObjNum()
{
  return m_lObjNum;
}

bool_a CPDFFontEncoding::GetEncodingStream(std::string& o_strEncoding)
{
  o_strEncoding.clear();
  char czTemp[200];
  std::map<int, std::string>::iterator iter;

  if (!HasEncodingStream())
    return false_a;

  if (m_lObjNum > 0)
  {
    sprintf(czTemp, "%ld 0 obj << /Type /Encoding ", m_lObjNum);
    o_strEncoding.append(czTemp);
  }
  o_strEncoding.append("/BaseEncoding /");
  o_strEncoding.append(GetEncodingStr());
  o_strEncoding.append(" /Differences [ ");

  for (iter = m_vctCharCodeToGlyphName.begin(); iter != m_vctCharCodeToGlyphName.end(); iter++)
  {
    sprintf(czTemp, "%ld /%s ", (long)iter->first, (const char *)iter->second.c_str());
    o_strEncoding.append(czTemp);
  }

  o_strEncoding.append("] >> endobj\n");

  return true_a;
}

bool_a CPDFFontEncoding::SetCodePage(long i_lCodePage)
{
  short i;
  char c;
  wchar_t wc;
  wstring wstrOut;

#if defined(WIN32)
  char czBuf[256];
  wchar_t wczBuf[256];

  for (i = 0, c = 0; i < 256; c++, i++)
  {
    czBuf[i] = c;
  }
  if ( 0 == MultiByteToWideChar(i_lCodePage, 0, czBuf, 256, wczBuf, 256))
    return false_a;
  wstrOut.append(wczBuf, 256);
#else
  char cChar;
  char czCodePage[100];
  string strIn;
  wstring wstrTemp;
  sprintf(czCodePage, "CP%ld", i_lCodePage);

  for (i = 0, cChar = 0; i < 256; cChar++, i++)
  {
    strIn.append(&cChar, 1);
    
    ConvertToUnicde(strIn, czCodePage, wstrTemp);
    
    wstrOut.append(wstrTemp);
  }

#endif

  for (i = 0; i < 256; i++)
  {
    //wc = wczBuf[i];
    wc = wstrOut.at(i);
    if (wc != i)
      m_vctCharCodeToGlyphName[i] = PDFGetGlyphName(wc);
  }

  return true_a;
}

