
#include "PDFBaseFont.h"
#include "PDFFontDesc.h"


CPDFFontDescriptor::CPDFFontDescriptor(CPDFDocument *i_pDoc)
{
  m_pDoc = i_pDoc;
  m_strFontName = "";
  m_lFlags = NOT_USED;
  m_recFontBBox.lTop = NOT_USED;
  m_recFontBBox.lBottom = NOT_USED;
  m_recFontBBox.lRight = NOT_USED;
  m_recFontBBox.lLeft = NOT_USED;
  m_sItalicAngle = NOT_USED;
  m_sAscent = NOT_USED;
  m_sDescent = NOT_USED;
  m_sCapHeight = NOT_USED;
  m_sXHeight = NOT_USED;
  m_sStemV = NOT_USED;
  m_sStemH = NOT_USED;
  m_strFontFile = "";
  m_strFontFile2 = "";
  m_strFontFile3 = "";
  m_strEmbedStream = "";
  m_bFontEmbed = false_a;
  m_lEmbedObjNum = NOT_USED;

  m_sUnderlinePosition = NOT_USED;
  m_sUnderlineThickness = NOT_USED;
}

CPDFFontDescriptor::~CPDFFontDescriptor()
{
}

void CPDFFontDescriptor::SetObjNum(long i_lObjNum)
{
  m_lObjNum = i_lObjNum;
}

void CPDFFontDescriptor::SetFontName(const char *i_pczFontName)
{
  if (i_pczFontName)
    m_strFontName = i_pczFontName;
}

void CPDFFontDescriptor::SetFlags(long i_lFlags)
{
  m_lFlags = i_lFlags;
}

void CPDFFontDescriptor::SetFontBBox(rec_a i_rec)
{
  m_recFontBBox = i_rec;
}

void CPDFFontDescriptor::SetItalicAngle(short i_sAngle)
{
  m_sItalicAngle = i_sAngle;
}

void CPDFFontDescriptor::SetAscent(short i_sAscent)
{
  m_sAscent = i_sAscent;
}

void CPDFFontDescriptor::SetDescent(short i_sDescent)
{
  m_sDescent = i_sDescent;
}

void CPDFFontDescriptor::SetCapHeight(short i_sCapHeight)
{
  m_sCapHeight = i_sCapHeight;
}

void CPDFFontDescriptor::SetXHeight(short i_sXHeight)
{
  m_sXHeight = i_sXHeight;
}

void CPDFFontDescriptor::SetStemV(short i_sStemV)
{
  m_sStemV = i_sStemV;
}

void CPDFFontDescriptor::SetStemH(short i_sStemH)
{
  m_sStemH = i_sStemH;
}



void CPDFFontDescriptor::SetUnderlinePosition(short i_sPos)
{
  m_sUnderlinePosition = i_sPos;
}

void CPDFFontDescriptor::SetUnderlineThickness(short i_sThick)
{
  m_sUnderlineThickness = i_sThick;
}

bool_a CPDFFontDescriptor::HasDescriptorStream()
{
  return false_a;
}

bool_a CPDFFontDescriptor::GetDescriptorStream(std::string &o_strStream)
{
  return false_a;//No Description for standart PDF fonts
}

void CPDFFontDescriptor::SetEmbedded(bool_a i_bEmbed)
{
  m_bFontEmbed = i_bEmbed;
}

void CPDFFontDescriptor::SetEmbedStream(std::string& i_strEmbedStream)
{
  m_strEmbedStream = i_strEmbedStream;
}

