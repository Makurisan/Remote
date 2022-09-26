
#include "PDFTTFFontDesc.h"
#include "PDFDocument.h"

#include "pdffnc.h"

using namespace std;

CPDFTTFFontDescriptor::CPDFTTFFontDescriptor(CPDFDocument *i_pDoc)
:CPDFFontDescriptor(i_pDoc)
{
}

CPDFTTFFontDescriptor::~CPDFTTFFontDescriptor()
{
}


bool_a CPDFTTFFontDescriptor::Init()
{
  return false_a;
}

bool_a CPDFTTFFontDescriptor::HasDescriptorStream()
{
  return true_a;
}

bool_a CPDFTTFFontDescriptor::GetDescriptorStream(string &o_strStream)
{
  o_strStream.clear();
  char czTemp[200];

  if (m_lObjNum > 0)
  {
    sprintf(czTemp, "%ld 0 obj << /Type /FontDescriptor ", m_lObjNum);
    o_strStream.append(czTemp);
  }

  sprintf(czTemp, "/Ascent %ld ", m_sAscent);
  o_strStream.append(czTemp);
  sprintf(czTemp, "/CapHeight %ld ", m_sCapHeight);
  o_strStream.append(czTemp);
  sprintf(czTemp, "/Descent %ld ", m_sDescent);
  o_strStream.append(czTemp);
  sprintf(czTemp, "/Flags %ld ", m_lFlags);
  o_strStream.append(czTemp);
  sprintf(czTemp, "/FontBBox [%ld %ld %ld %ld] ", m_recFontBBox.lLeft, m_recFontBBox.lBottom,
                                                  m_recFontBBox.lRight, m_recFontBBox.lTop);
  o_strStream.append(czTemp);

  o_strStream.append("/FontName /");
  o_strStream.append(m_strFontName.c_str());
  o_strStream.append(" ");

  sprintf(czTemp, "/ItalicAngle %ld ", m_sItalicAngle);
  o_strStream.append(czTemp);
  sprintf(czTemp, "/StemV %ld ", m_sStemV);
  o_strStream.append(czTemp);

  if (m_sStemH > 0)
  {
    sprintf(czTemp, "/StemH %ld ", m_sStemH);
    o_strStream.append(czTemp);
  }
  if (m_sXHeight > 0)
  {
    sprintf(czTemp, "/XHeight %ld ", m_sXHeight);
    o_strStream.append(czTemp);
  }

  if (HasFontEmbedStream())
  {
    sprintf(czTemp, "/FontFile2 %ld 0 R ", m_lEmbedObjNum);
    o_strStream.append(czTemp);
  }

  o_strStream.append(">> endobj\n");

  return true_a;
}

bool_a CPDFTTFFontDescriptor::HasFontEmbedStream()
{
  return m_bFontEmbed;
}

void CPDFTTFFontDescriptor::SetFontEmbedStreamObjNum(long i_lObjNum)
{
  m_lEmbedObjNum = i_lObjNum;
}

long CPDFTTFFontDescriptor::GetFontEmbedStreamObjNum()
{
  return m_lEmbedObjNum;
}

bool_a CPDFTTFFontDescriptor::GetEmbedStream(string& o_strEmbed)
{
  o_strEmbed.clear();
  char czTemp[200];
  string strComp = m_strEmbedStream;

  if (!HasFontEmbedStream())
    return false_a;

  if (!CPDFDocument::CompressStream(strComp))
    return false_a;

  if (m_lEmbedObjNum > 0)
  {
    sprintf(czTemp, "%ld 0 obj\n", m_lEmbedObjNum);
    o_strEmbed.append(czTemp);
  }
  o_strEmbed.append("<< /Filter /FlateDecode /Length ");

  sprintf(czTemp, "%ld ", (long)strComp.size());
  o_strEmbed.append(czTemp);

  o_strEmbed.append("/Length1 ");

  sprintf(czTemp, "%ld ", (long)m_strEmbedStream.size());
  o_strEmbed.append(czTemp);

  o_strEmbed.append(">>\nstream\n");


  if (m_pDoc->Encrypt())
  {
    char *pObj = (char *)PDFMalloc(strComp.size());
    memcpy(pObj, strComp.c_str(), strComp.size());
    if (!PDFEncrypt((PDFDocPTR)m_pDoc, (unsigned char *)pObj, strComp.size(), m_lEmbedObjNum, 0))
    {
      o_strEmbed.clear();
      return false_a;
    }
    strComp.assign(pObj, strComp.size());
    free(pObj);
  }
  o_strEmbed.append(strComp);


  o_strEmbed.append("\nendstream\n");
  o_strEmbed.append("endobj\n");

  return true_a;
}
