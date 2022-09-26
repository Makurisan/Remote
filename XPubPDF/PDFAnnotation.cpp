

#include "pdffnc.h"
#include "PDFAnnotation.h"
#include "PDFDocument.h"
#include "PDFBaseFont.h"
#include <sstream>

using namespace std;

CPDFAnnotation::CPDFAnnotation(CPDFDocument *i_pDoc, eDICTFIELDTYPE i_eType,
                               const char *i_pczName, rec_a i_recPosSize, long i_lPage)
{
  m_pDoc = i_pDoc;
  m_ColorDA.SetColor(0, 0, 0);
  eAnnotType = eAnnotWidget;
  eDictFieldType = i_eType;
  recAnnot = i_recPosSize;
  if (i_pczName)
    pczAnnotName = PDFStrDup(i_pczName);
  else
    pczAnnotName = NULL;
  if (i_lPage <= 0)
    lDestPage = m_pDoc ? m_pDoc->GetCurrentPage() - 1 : NOT_USED;
  else
    lDestPage = i_lPage - 1;
  lFont = NOT_USED;
  fFontSize = NOT_USED;
  if (m_pDoc)
  {
    CPDFBaseFont *pFont = m_pDoc->GetActualFont();
    if (pFont)
    {
      lFont = pFont->GetFontID();
      fFontSize = m_pDoc->GetConvertFontSize();
      pFont->SetUsedInForm();
      pFont->SetEmbedded();
    }
  }

  lObjIndex[0] = NOT_USED;
  iAnnotFlags = 4;//Print flag
  pczAnnotBorder = PDFStrDup("[0 0 0]");
  pczAction = NULL;
  pDestSpec = NULL;
  pczCA = PDFStrDup("8");
  sBorderWidth = 0;
  lFieldFlags = 0;
  lQ = 0;
  pczShortDesc = NULL;
  pczValue = NULL;
  lMaxLen = NOT_USED;

  PDFAddAnnotToPage(GetBaseDoc(), m_pDoc->GetFormCount(), lDestPage);
}

CPDFAnnotation::CPDFAnnotation(CPDFDocument *i_pDoc, const char *i_pczName, long i_lPage,
               char *i_pczDest, char *i_pczAction, float i_fZoom)
{
  m_pDoc = i_pDoc;
  memset(this->lObjIndex, -1, sizeof(this->lObjIndex));
  this->lFont = m_pDoc->GetActualFontID();
  this->recAnnot.lLeft = 0;
  this->recAnnot.lTop = 0;
  this->recAnnot.lBottom = 0;
  this->recAnnot.lRight = 0;
  this->pczAnnotName = PDFStrDup(i_pczName);
  this->iAnnotFlags = 4;//Print flag
  this->pczAnnotBorder = PDFStrDup("[0 0 0]");
  this->pczAction = i_pczAction;
  this->pDestSpec = i_pczDest;
  this->lDestPage = i_lPage - 1;
  this->m_fZoom = i_fZoom;
  this->eAnnotType = eAnnotLink;
}

CPDFAnnotation::~CPDFAnnotation()
{
}

PDFDocPTR CPDFAnnotation::GetBaseDoc()
{
  return (PDFDocPTR)m_pDoc;
}

void CPDFAnnotation::SetPosSize(rec_a i_recPosSize)
{
  this->recAnnot = i_recPosSize;
}

rec_a CPDFAnnotation::GetPosSize()
{
  return this->recAnnot;
}

const char *CPDFAnnotation::GetName()
{
  return this->pczAnnotName;
}

void CPDFAnnotation::SetName(const char *i_pczName)
{
  if (this->pczAnnotName)
    free(this->pczAnnotName);
  this->pczAnnotName = PDFStrDup(i_pczName);
}

void CPDFAnnotation::SetPage(long i_lPage)
{
  if (i_lPage > 0)
    this->lDestPage = i_lPage - 1;
}

long CPDFAnnotation::GetPage()
{
  return this->lDestPage + 1;
}

CPDFColor &CPDFAnnotation::GetBGColor()
{
  return m_ColorBG;
}

void CPDFAnnotation::SetBorderWidth(short i_sWidth)
{
  this->sBorderWidth = i_sWidth;
}

short CPDFAnnotation::GetBorderWidth()
{
  return this->sBorderWidth;
}

CPDFColor &CPDFAnnotation::GetBorderColor()
{
  return m_ColorBC;
}

CPDFColor &CPDFAnnotation::GetColor()
{
  return m_ColorDA;
}

void CPDFAnnotation::SetShortDesc(const char *i_pczDesc)
{
  if (this->pczShortDesc)
    free(this->pczShortDesc);
  this->pczShortDesc = PDFStrDup(i_pczDesc);
}

const char *CPDFAnnotation::GetShortDesc()
{
  return this->pczShortDesc;
}

void CPDFAnnotation::SetValue(const char *i_pczText)
{
  if (this->pczValue)
    free(this->pczValue);
  this->pczValue = PDFStrDup(i_pczText);
}

const char *CPDFAnnotation::GetValue()
{
  return this->pczValue;
}

void CPDFAnnotation::SetMaxLen(long i_lLen)
{
  this->lMaxLen = i_lLen;
}

long CPDFAnnotation::GetMaxLen()
{
  return this->lMaxLen;
}

void CPDFAnnotation::SetDocument(CPDFDocument *i_pDoc)
{
  m_pDoc = i_pDoc;
}

void CPDFAnnotation::AssignObjNum(long &io_lObjNum)
{
  this->lObjIndex[0] = io_lObjNum++;
  if (eAnnotWidget == this->eAnnotType)
  {
    switch (this->eDictFieldType)
    {
    case eText:
      this->lObjIndex[1] = io_lObjNum++;
      break;
    case eButton:
      this->lObjIndex[1] = io_lObjNum++;
      this->lObjIndex[2] = io_lObjNum++;
      this->lObjIndex[3] = io_lObjNum++;
      this->lObjIndex[4] = io_lObjNum++;
      break;
    case eChoice:
    case eSignature:
    default:
      break;
    }
  }
}

bool_a CPDFAnnotation::GetEntireFormStream(std::vector<std::string> &o_vctFormStreams)
{
  if (this->lFont < 0 || this->lFont >= this->m_pDoc->GetFontsCount())
    return false_a;
  ostringstream strBuffer("");
  size_t Size;
  char *pObj;
  char czTemp[200];
  CPDFBaseFont *pAnnotFont = m_pDoc->GetFont(this->lFont);
  if (!pAnnotFont)
    return false_a;

  strBuffer << this->lObjIndex[0] << " 0 obj << ";
  strBuffer << "/Type/Annot /Subtype/" << pczANNOTATIONTYPE[this->eAnnotType] << " ";
  sprintf(czTemp, "/Rect [%.5f %.5f %.5f %.5f] ", PDFConvertDimensionToDefault(this->recAnnot.lLeft),
            PDFConvertDimensionToDefault(this->recAnnot.lBottom), PDFConvertDimensionToDefault(this->recAnnot.lRight),
            PDFConvertDimensionToDefault(this->recAnnot.lTop));
  strBuffer << czTemp;
  strBuffer << "/F " << this->iAnnotFlags << " ";

  if (eAnnotLink == this->eAnnotType)
  {
    if (this->lDestPage < 0)
    {
      if (this->pczAction)
        strBuffer << "/A << " << this->pczAction << " >> ";
    }
    else
    {
      char czPage[10] = {0};
      if (this->lDestPage >= 0 && this->lDestPage < m_pDoc->GetPageCount())
        sprintf(czPage, "%ld", m_pDoc->m_vctPages[this->lDestPage]->lObjIndex);
      else
        sprintf(czPage, "%ld", 0);
      strBuffer << "/Dest [" << czPage << " 0 R" << this->pDestSpec << "] ";
    }
  }
  else
  {
    strBuffer << "/P " << m_pDoc->m_vctPages[this->lDestPage]->lObjIndex << " 0 R ";
  }

  if (eAnnotLink == this->eAnnotType)
  {
    strBuffer << "/Border " << this->pczAnnotBorder << " ";
  }

  if (eAnnotWidget == this->eAnnotType)
  {
    strBuffer << "/FT /" << pczDICTFIELDTYPE[this->eDictFieldType] << " ";

    if (m_pDoc->Encrypt())
    {
      Size = PDFStrLenC(this->pczAnnotName);
      pObj = (char *)PDFMalloc(Size + 1);
      if (!pObj)
        return false_a;
      strcpy(pObj, this->pczAnnotName);
      pObj[Size] = 0;
      if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[0], 0))
      {
        free(pObj);
        return false_a;
      }

  //    pTemp = PDFescapeSpecialCharsBinary(pObj, lSize, &lSize);

      free(this->pczAnnotName);
      this->pczAnnotName = pObj;
    }
    strBuffer << "/T (" << this->pczAnnotName << ") ";

    if (eButton == this->eDictFieldType || eText == this->eDictFieldType)
    {
      if (this->sBorderWidth > 0)
        strBuffer << "/BS << /W " << this->sBorderWidth << " /S /S >> ";
    }

    if (this->pczShortDesc)
    {
      if (m_pDoc->Encrypt())
      {
        Size = PDFStrLenC(this->pczShortDesc);
        pObj = (char *)PDFMalloc(Size + 1);
        if (!pObj)
          return false_a;
        strcpy(pObj, this->pczShortDesc);
        pObj[Size] = 0;
        if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[0], 0))
        {
          free(pObj);
          return false_a;
        }

        free(this->pczShortDesc);
        this->pczShortDesc = pObj;
      }
      strBuffer << "/TU (" << this->pczShortDesc << ") ";
    }

    if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType() ||
        CPDFColor::eNoColor != this->m_ColorBG.GetColorType() ||
        (this->pczCA && eButton == this->eDictFieldType))
      strBuffer << "/MK << ";

    if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType())
      strBuffer << "/BC [" << this->m_ColorBC.GetColorString() << "] ";
    if (CPDFColor::eNoColor != this->m_ColorBG.GetColorType())
      strBuffer << "/BG [" << this->m_ColorBG.GetColorString() << "] ";

    if (this->pczCA && eButton == this->eDictFieldType)
    {
      if (m_pDoc->Encrypt())
      {
        Size = PDFStrLenC(this->pczCA);
        pObj = (char *)PDFMalloc(Size + 1);
        if (!pObj)
          return false_a;
        strcpy(pObj, this->pczCA);
        pObj[Size] = 0;
        if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[0], 0))
        {
          free(pObj);
          return false_a;
        }

        free(this->pczCA);
        this->pczCA = pObj;
      }
      strBuffer << "/CA (" << this->pczCA << ") ";
    }

    if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType() ||
        CPDFColor::eNoColor != this->m_ColorBG.GetColorType() ||
        (this->pczCA && eButton == this->eDictFieldType))
      strBuffer << ">> ";

    strBuffer << "/DA (/" << pAnnotFont->GetFontResName() << " ";

    if (eButton == this->eDictFieldType)
    {
      strBuffer << "0 Tf " << this->m_ColorDA.GetColorStringWithNonStrokingOperator() << ") ";
    }
    else
    {
      sprintf(czTemp, "%.5f Tf ", PDFConvertDimensionToDefault(this->fFontSize));
      strBuffer << czTemp << this->m_ColorDA.GetColorStringWithNonStrokingOperator() << ") ";
    }

    if (eButton == this->eDictFieldType)
    {
      strBuffer << "/AP << /N << /On " << this->lObjIndex[1] << " 0 R /Off " << this->lObjIndex[2] << " 0 R >> ";
      strBuffer << "/D << /On " << this->lObjIndex[3] << " 0 R /Off " << this->lObjIndex[4] << " 0 R >> >> ";
      strBuffer << "/AS /Off ";
    }

    if (eText == this->eDictFieldType)
    {
      strBuffer << "/Q " << this->lQ << " ";
      strBuffer << "/Ff " << this->lFieldFlags << " ";
      if (this->lMaxLen > 0)
        strBuffer << "/MaxLen " << this->lMaxLen << " ";

      if (this->pczValue)
      {
        string strIn = this->pczValue;
        string strOut;
        long lTemp;
        if (!pAnnotFont->GetUnicodeValue(strIn, strOut))
          return false_a;
        pObj = PDFescapeSpecialCharsBinary(strOut.c_str(), strOut.size(), &lTemp);
        strIn.assign(pObj, lTemp);
        free(pObj);

        if (m_pDoc->Encrypt())
        {
          Size = strIn.size();
          pObj = (char *)PDFMalloc(Size + 1);
          if (!pObj)
            return false_a;
          strcpy(pObj, strIn.c_str());
          pObj[Size] = 0;
          if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[0], 0))
          {
            free(pObj);
            return false_a;
          }

          strIn.assign(pObj, Size);
          free(pObj);
        }


        strBuffer << "/DV (" << strIn << ") ";
        strBuffer << "/V (" << strIn << ") ";
      }

      strBuffer << "/AP << /N " << this->lObjIndex[1] << " 0 R >> ";
    }
  }
  strBuffer << czOBJECTEND;
  o_vctFormStreams.push_back(strBuffer.str());
  strBuffer.clear();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!GetAPStream(o_vctFormStreams))
    return false_a;

  return true_a;
}

bool_a CPDFAnnotation::GetAPStream(std::vector<std::string> &o_vctFormStreams)
{
  if (this->eAnnotType != eAnnotWidget)
    return true_a;

  if (eButton == this->eDictFieldType)
  {
    if (!GetAPOnNormal(o_vctFormStreams))
      return false_a;
    if (!GetAPOffNormal(o_vctFormStreams))
      return false_a;
    if (!GetAPOnDown(o_vctFormStreams))
      return false_a;
    if (!GetAPOffDown(o_vctFormStreams))
      return false_a;
  }
  if (eText == this->eDictFieldType)
  {
    if (!GetAPNormal(o_vctFormStreams))
      return false_a;
  }
  return true_a;
}

bool_a CPDFAnnotation::GetAPOnNormal(std::vector<std::string> &o_vctFormStreams)
{
  if (!m_pDoc)
    return false_a;
  if (this->lFont < 0 || this->lFont >= m_pDoc->GetFontsCount())
    return false_a;

  CPDFBaseFont *pFont = m_pDoc->GetFont(this->lFont);
  if (!pFont)
    return false_a;
  ostringstream strBuffer("");
  float fX, fY, fAnnotH, fAnnotW, fHeight, fDesc;
  char czTemp[200], *pObj;
  size_t Size;

  fX = this->sBorderWidth * 2.f;
  fY = this->sBorderWidth * 2.f;
  fAnnotH = PDFConvertDimensionToDefaultL(this->recAnnot.lTop - this->recAnnot.lBottom);
  fAnnotW = PDFConvertDimensionToDefault(this->recAnnot.lRight - this->recAnnot.lLeft);

  fHeight = pFont->GetFontHeight(this->fFontSize);
  fDesc = pFont->GetFontDescentHeight(this->fFontSize);

  if (CPDFColor::eNoColor != this->m_ColorBG.GetColorType())
  {
    strBuffer << this->m_ColorBG.GetColorStringWithNonStrokingOperator();
    sprintf(czTemp, "0 0 %.5f %.5f re f ", fAnnotW, fAnnotH);
    strBuffer << czTemp;
  }
  if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType())
  {
    strBuffer << this->m_ColorBC.GetColorStringWithStrokingOperator();
    sprintf(czTemp, "%d w ", this->sBorderWidth);
    strBuffer << czTemp;

    sprintf(czTemp, "%.5f %.5f %.5f %.5f re s ", (float)this->sBorderWidth / 2, (float)this->sBorderWidth / 2,
              fAnnotW - this->sBorderWidth, fAnnotH - this->sBorderWidth);
    strBuffer << czTemp;
  }

  strBuffer << "q ";

  sprintf(czTemp, "%d %d %.5f %.5f re W n ", this->sBorderWidth, this->sBorderWidth,
            fAnnotW - (2 * this->sBorderWidth), fAnnotH - (2 * this->sBorderWidth));
  strBuffer << czTemp;

  strBuffer << this->m_ColorDA.GetColorStringWithNonStrokingOperator() << "BT ";

  sprintf(czTemp, "/%s %.5f Tf ", pFont->GetFontResName(), PDFConvertDimensionToDefault(this->fFontSize));
  strBuffer << czTemp;

  sprintf(czTemp, "%.5f ", fX);
  strBuffer << czTemp;
  sprintf(czTemp, "%.5f ", fY);
  strBuffer << czTemp << "Td ";
  sprintf(czTemp, "%.5f ", fHeight);
  strBuffer << czTemp << "TL " << "(" << this->pczCA << ") Tj ET Q";

  if (m_pDoc->Encrypt())
  {
    Size = strBuffer.str().size();
    pObj = (char *)PDFMalloc(Size + 1);
    if (!pObj)
      return false_a;
    strcpy(pObj, strBuffer.str().c_str());
    pObj[Size] = 0;
    if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[1], 0))
    {
      free(pObj);
      return false_a;
    }

//    pTemp = PDFescapeSpecialCharsBinary(pObj, lSize, &lSize);

    strBuffer.clear();
    strBuffer << pObj;
    free(pObj);
  }

  ostringstream strTemp("");
  strTemp << this->lObjIndex[1] << " 0 obj << /Length " << strBuffer.str().length() << " /Subtype /Form ";
  sprintf(czTemp, "/BBox [ %.5f %.5f %.5f %.5f ] ", 0.f, 0.f, (float)fAnnotW, (float)fAnnotH);
  strTemp << czTemp << "/Resources << /ProcSet [ /PDF /Text ] /Font << /" << pFont->GetFontResName() << " " << pFont->GetFontObjNum() << " 0 R >> >> >> \nstream\n";
  strTemp << strBuffer.str();
  strTemp << "\nendstream\nendobj\n";

  o_vctFormStreams.push_back(strTemp.str());

  return true_a;
}

bool_a CPDFAnnotation::GetAPOffNormal(std::vector<std::string> &o_vctFormStreams)
{
  if (!m_pDoc)
    return false_a;
  if (this->lFont < 0 || this->lFont >= m_pDoc->GetFontsCount())
    return false_a;

  CPDFBaseFont *pFont = m_pDoc->GetFont(this->lFont);
  if (!pFont)
    return false_a;
  ostringstream strBuffer("");
  float fX, fY, fAnnotH, fAnnotW;
  char czTemp[200], *pObj;
  size_t Size;

  fX = this->sBorderWidth * 2.f;
  fY = this->sBorderWidth * 2.f;
  fAnnotH = PDFConvertDimensionToDefault(this->recAnnot.lTop - this->recAnnot.lBottom);
  fAnnotW = PDFConvertDimensionToDefault(this->recAnnot.lRight - this->recAnnot.lLeft);

  if (CPDFColor::eNoColor != this->m_ColorBG.GetColorType())
  {
    strBuffer << this->m_ColorBG.GetColorStringWithNonStrokingOperator();
    sprintf(czTemp, "0 0 %.5f %.5f re f ", fAnnotW, fAnnotH);
    strBuffer << czTemp;
  }

  if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType())
  {
    strBuffer << this->m_ColorBC.GetColorStringWithStrokingOperator();
    sprintf(czTemp, "%d w ", this->sBorderWidth);
    strBuffer << czTemp;
    sprintf(czTemp, "%.5f %.5f %.5f %.5f re s ", (float)this->sBorderWidth / 2, (float)this->sBorderWidth / 2,
              fAnnotW - this->sBorderWidth, fAnnotH - this->sBorderWidth);
    strBuffer << czTemp;
  }

  sprintf(czTemp, "q %d %d %.5f %.5f re W n BT ET Q", this->sBorderWidth, this->sBorderWidth,
            fAnnotW - (2 * this->sBorderWidth), fAnnotH - (2 * this->sBorderWidth));
  strBuffer << czTemp;

  if (m_pDoc->Encrypt())
  {
    Size = strBuffer.str().size();
    pObj = (char *)PDFMalloc(Size + 1);
    if (!pObj)
      return false_a;
    strcpy(pObj, strBuffer.str().c_str());
    pObj[Size] = 0;
    if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[2], 0))
    {
      free(pObj);
      return false_a;
    }

//    pTemp = PDFescapeSpecialCharsBinary(pObj, lSize, &lSize);

    strBuffer.clear();
    strBuffer << pObj;
    free(pObj);
  }

  ostringstream strTemp("");
  strTemp << this->lObjIndex[2] << " 0 obj << /Length " << strBuffer.str().length() << " /Subtype /Form ";
  sprintf(czTemp, "/BBox [ %.5f %.5f %.5f %.5f ] ", 0.f, 0.f, (float)fAnnotW, (float)fAnnotH);
  strTemp << czTemp << "/Resources << /ProcSet [ /PDF /Text ] /Font << /" << pFont->GetFontResName() << " " << pFont->GetFontObjNum() << " 0 R >> >> >> \nstream\n";
  strTemp << strBuffer.str();
  strTemp << "\nendstream\nendobj\n";

  o_vctFormStreams.push_back(strTemp.str());

  return true_a;
}

bool_a CPDFAnnotation::GetAPOnDown(std::vector<std::string> &o_vctFormStreams)
{
  if (!m_pDoc)
    return false_a;
  if (this->lFont < 0 || this->lFont >= m_pDoc->GetFontsCount())
    return false_a;

  CPDFBaseFont *pFont = m_pDoc->GetFont(this->lFont);
  if (!pFont)
    return false_a;
  ostringstream strBuffer("");
  float fX, fY, fAnnotH, fAnnotW, fHeight, fDesc;
  char czTemp[200], *pObj;
  size_t Size;

  fX = this->sBorderWidth * 2.f;
  fY = this->sBorderWidth * 2.f;
  fAnnotH = PDFConvertDimensionToDefault(this->recAnnot.lTop - this->recAnnot.lBottom);
  fAnnotW = PDFConvertDimensionToDefault(this->recAnnot.lRight - this->recAnnot.lLeft);


  fHeight = pFont->GetFontHeight(this->fFontSize);
  fDesc = pFont->GetFontDescentHeight(this->fFontSize);

  if (CPDFColor::eNoColor != this->m_ColorBG.GetColorType())
  {
    strBuffer << this->m_ColorBG.GetColorStringWithNonStrokingOperator();
    sprintf(czTemp, "0 0 %.5f %.5f re f ", (fAnnotW), fAnnotH);
    strBuffer << czTemp;
  }
  if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType())
  {
    strBuffer << this->m_ColorBC.GetColorStringWithStrokingOperator() << this->sBorderWidth << " w ";
    sprintf(czTemp, "%.5f %.5f %.5f %.5f re s ", (float)this->sBorderWidth / 2, (float)this->sBorderWidth / 2,
              fAnnotW - this->sBorderWidth, fAnnotH - this->sBorderWidth);
    strBuffer << czTemp;
  }

  strBuffer << "q ";
  sprintf(czTemp, "%d %d %.5f %.5f re W n ", this->sBorderWidth, this->sBorderWidth,
            fAnnotW - (2 * this->sBorderWidth), fAnnotH - (2 * this->sBorderWidth));
  strBuffer << czTemp;

  strBuffer << this->m_ColorDA.GetColorStringWithNonStrokingOperator() << "BT ";
  sprintf(czTemp, "/%s %.5f Tf ", pFont->GetFontResName(), PDFConvertDimensionToDefault(this->fFontSize));
  strBuffer << czTemp;

  sprintf(czTemp, "%.5f ", fX);
  strBuffer << czTemp;
  sprintf(czTemp, "%.5f ", fY);
  strBuffer << czTemp;
  strBuffer << "Td ";
  sprintf(czTemp, "%.5f ", fHeight);
  strBuffer << czTemp;
  strBuffer << "TL (" << this->pczCA << ") Tj ET Q";

  if (m_pDoc->Encrypt())
  {
    Size = strBuffer.str().size();
    pObj = (char *)PDFMalloc(Size + 1);
    if (!pObj)
      return false_a;
    strcpy(pObj, strBuffer.str().c_str());
    pObj[Size] = 0;
    if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[3], 0))
    {
      free(pObj);
      return false_a;
    }

//    pTemp = PDFescapeSpecialCharsBinary(pObj, lSize, &lSize);

    strBuffer.clear();
    strBuffer << pObj;
    free(pObj);
  }


  ostringstream strTemp("");
  strTemp << this->lObjIndex[3] << " 0 obj << /Length " << strBuffer.str().length() << " /Subtype /Form ";
  sprintf(czTemp, "/BBox [ %.5f %.5f %.5f %.5f ] ", 0.f, 0.f, (float)fAnnotW, (float)fAnnotH);
  strTemp << czTemp << "/Resources << /ProcSet [ /PDF /Text ] /Font << /" << pFont->GetFontResName() << " " << pFont->GetFontObjNum() << " 0 R >> >> >> \nstream\n";
  strTemp << strBuffer.str();
  strTemp << "\nendstream\nendobj\n";

  o_vctFormStreams.push_back(strTemp.str());

  return true_a;
}

bool_a CPDFAnnotation::GetAPOffDown(std::vector<std::string> &o_vctFormStreams)
{
  if (!m_pDoc)
    return false_a;
  if (this->lFont < 0 || this->lFont >= m_pDoc->GetFontsCount())
    return false_a;

  CPDFBaseFont *pFont = m_pDoc->GetFont(this->lFont);
  if (!pFont)
    return false_a;
  ostringstream strBuffer("");
  float fX, fY, fAnnotH, fAnnotW;
  char czTemp[200], *pObj;
  size_t Size;

  fX = this->sBorderWidth * 2.f;
  fY = this->sBorderWidth * 2.f;
  fAnnotH = PDFConvertDimensionToDefault(this->recAnnot.lTop - this->recAnnot.lBottom);
  fAnnotW = PDFConvertDimensionToDefault(this->recAnnot.lRight - this->recAnnot.lLeft);

  if (CPDFColor::eNoColor != this->m_ColorBG.GetColorType())
  {
    strBuffer << this->m_ColorBG.GetColorStringWithNonStrokingOperator();
    sprintf(czTemp, "0 0 %.5f %.5f re f ", fAnnotW, fAnnotH);
    strBuffer << czTemp;
  }
  if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType())
  {
    strBuffer << this->m_ColorBC.GetColorStringWithStrokingOperator() << this->sBorderWidth << " w ";
    sprintf(czTemp, "%.5f %.5f %.5f %.5f re s ", (float)this->sBorderWidth / 2, (float)this->sBorderWidth / 2,
              fAnnotW - this->sBorderWidth, fAnnotH - this->sBorderWidth);
    strBuffer << czTemp;
  }

  strBuffer << "q ";
  sprintf(czTemp, "%d %d %.5f %.5f re W n ", this->sBorderWidth, this->sBorderWidth,
            fAnnotW - (2 * this->sBorderWidth), fAnnotH - (2 * this->sBorderWidth));
  strBuffer << czTemp;

  strBuffer << this->m_ColorDA.GetColorStringWithNonStrokingOperator() << "BT ET Q";

  if (m_pDoc->Encrypt())
  {
    Size = strBuffer.str().size();
    pObj = (char *)PDFMalloc(Size + 1);
    if (!pObj)
      return false_a;
    strcpy(pObj, strBuffer.str().c_str());
    pObj[Size] = 0;
    if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[4], 0))
    {
      free(pObj);
      return false_a;
    }

//    pTemp = PDFescapeSpecialCharsBinary(pObj, lSize, &lSize);

    strBuffer.clear();
    strBuffer << pObj;
    free(pObj);
  }

  ostringstream strTemp("");
  strTemp << this->lObjIndex[4] << " 0 obj << /Length " << strBuffer.str().length() << " /Subtype /Form ";
  sprintf(czTemp, "/BBox [ %.5f %.5f %.5f %.5f ] ", 0.f, 0.f, (float)fAnnotW, (float)fAnnotH);
  strTemp << czTemp << "/Resources << /ProcSet [ /PDF /Text ] /Font << /" << pFont->GetFontResName() << " " << pFont->GetFontObjNum() << " 0 R >> >> >> \nstream\n";
  strTemp << strBuffer.str();
  strTemp << "\nendstream\nendobj\n";

  o_vctFormStreams.push_back(strTemp.str());

  return true_a;
}

bool_a CPDFAnnotation::GetAPNormal(std::vector<std::string> &o_vctFormStreams)
{
  float fX = 4, fY = 2, fHeight, fDesc;
  char *pTemp, *pObj, czX[20], czY[20], czHeight[20] = {"0"}, czTemp[200];
  size_t Size;
  float fWidth, f, fAnnotH, fAnnotW, fCountedFontSize, fDefFontSize;
  std::vector <string> values;
  std::vector <float> Xshift;
  std::string strValue;
  ostringstream strBuffer("");

  if (!m_pDoc)
    return false_a;
  if (this->lFont < 0 || this->lFont >= m_pDoc->GetFontsCount())
    return false_a;

  CPDFBaseFont *pFont = m_pDoc->GetFont(this->lFont);
  if (!pFont)
    return false_a;

  fX = this->sBorderWidth * 2.f;
  fY = this->sBorderWidth * 2.f;

  fDefFontSize = PDFConvertDimensionToDefault(this->fFontSize);
  fHeight = pFont->GetFontHeight(fDefFontSize);
  fDesc = pFont->GetFontDescentHeight(fDefFontSize);
  rec_a bbox = pFont->GetFontBBox();

  fAnnotH = PDFConvertDimensionToDefault(this->recAnnot.lTop - this->recAnnot.lBottom);
  fAnnotW = PDFConvertDimensionToDefault(this->recAnnot.lRight - this->recAnnot.lLeft);


  fHeight = (float)((bbox.lTop - bbox.lBottom) / 1000.0);//ToDo:Is it correct???
  if (0 != fDefFontSize)
    fHeight *= fDefFontSize;
  else
  {
    fHeight = (fAnnotH - (2 * this->sBorderWidth));
    fHeight /= (float)((bbox.lTop - bbox.lBottom) / 1000.0);
    fHeight /= (float)((bbox.lTop - bbox.lBottom) / 1000.0);
    
    //fHeight = pFont->CalcFontHeight(this->pczValue, fHeight, fAnnotH - (2 * this->sBorderWidth));
  }
  fDesc = pFont->GetFontDescent();
  if (0 != fDefFontSize)
  {
    fDesc *= fDefFontSize;
  }
  else
  {
    fDesc *= fHeight;
    fHeight *= (float)((bbox.lTop - bbox.lBottom) / 1000.0);

    fHeight = pFont->CalcFontHeight(this->pczValue, fHeight, fAnnotW - (2 * this->sBorderWidth));
  }
  fCountedFontSize = fHeight;



  sprintf(czHeight, "%.5f", fHeight);
  fHeight = fAnnotH - fHeight;
  if (this->lFieldFlags & 0x1000)//multiline
  {
    char *pSpace = NULL, *pB, *pE;
    fY = fHeight - fY;
    fWidth = (fAnnotW) - (2 * this->sBorderWidth * 2.f);
    pE = pB = this->pczValue;
    while (0 != *pE)
    {
      if (' ' == *pE || '\t' == *pE)
        pSpace = pE;
      strValue.append(pE, 1);
      f = pFont->GetFontStringWidth(strValue.c_str(), fDefFontSize);
      if (f > fWidth || *pE == '\r')
      {
        if (!pSpace || f < fWidth)
          pSpace = pE;
        else
          pSpace++;
        pE = pSpace;
        strValue.erase();
        strValue.append(pB, pE - pB);

        pObj = PDFescapeSpecialChars(strValue.c_str());
        strValue.assign(pObj);
        free(pObj);

        basic_string <char>::size_type index = strValue.find_last_of(" ");
        if (index != -1 && index == (strValue.length() - 1))
          strValue.erase(index, 1);

        switch (this->lQ)
        {
        case 2:
          f = pFont->GetFontStringWidth(strValue.c_str(), fDefFontSize);
          fX = (fAnnotW) - f - (this->sBorderWidth * 2.f);
          break;
        case 1:
          f = pFont->GetFontStringWidth(strValue.c_str(), fDefFontSize);
          fX = (fAnnotW) - f;
          fX /= 2;
          break;
        case 0:
        default:
          break;
        }
        strValue.append("\\012");

        values.push_back(strValue);
        Xshift.push_back(fX);
        strValue.erase();
        if (*pSpace == '\r')
        {
          pSpace++;
        }

        pB = pE = pSpace;
        pSpace = NULL;
      }
      else
        pE++;
      if (0 == *pE && !strValue.empty())
      {
        switch (this->lQ)
        {
        case 2:
          f = pFont->GetFontStringWidth(strValue.c_str(), fDefFontSize);
          fX = (fAnnotW) - f - (this->sBorderWidth * 2.f);
          break;
        case 1:
          f = pFont->GetFontStringWidth(strValue.c_str(), fDefFontSize);
          fX = (fAnnotW) - f;
          fX /= 2;
          break;
        case 0:
        default:
          break;
        }

        pObj = PDFescapeSpecialChars(strValue.c_str());
        strValue.assign(pObj);
        free(pObj);

        values.push_back(strValue);
        Xshift.push_back(fX);
      }
    }
  }
  else
  {
    fHeight /= 2;
    fY = fHeight - fDesc;
    switch (this->lQ)
    {
    case 2:
      fWidth = pFont->GetFontStringWidth(this->pczValue, fDefFontSize);
      fX = (fAnnotW) - fWidth - fX;
      break;
    case 1:
      fWidth = pFont->GetFontStringWidth(this->pczValue, fDefFontSize);
      fX = (fAnnotW) - fWidth;
      fX /= 2;
      break;
    case 0:
    default:
      break;
    }

    pObj = PDFescapeSpecialChars(this->pczValue);
    strValue.assign(pObj);
    free(pObj);

    values.push_back(strValue);
    Xshift.push_back(fX);
  }

  strValue.erase();

  if (CPDFColor::eNoColor != this->m_ColorBG.GetColorType())
  {
    strValue.append(this->m_ColorBG.GetColorStringWithNonStrokingOperator());
    sprintf(czTemp, "0 0 %.5f %.5f re f ", (fAnnotW), fAnnotH);
    strValue.append(czTemp);
  }
  if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType())
  {
    strValue.append(this->m_ColorBC.GetColorStringWithStrokingOperator());
    sprintf(czTemp, "%d w ", this->sBorderWidth);
    strValue.append(czTemp);
    sprintf(czTemp, "%.5f %.5f %.5f %.5f re s ", (float)this->sBorderWidth / 2, (float)this->sBorderWidth / 2,
              fAnnotW - this->sBorderWidth, fAnnotH - this->sBorderWidth);
    strValue.append(czTemp);
  }

  strValue.append("/Tx BMC q ");

  sprintf(czTemp, "%d %d %.5f %.5f re W n ", this->sBorderWidth, this->sBorderWidth,
            fAnnotW - (2 * this->sBorderWidth), fAnnotH - (2 * this->sBorderWidth));
  strValue.append(czTemp);

  strValue.append("BT\n");
  strValue.append(this->m_ColorDA.GetColorStringWithNonStrokingOperator());

  if (this->fFontSize == 0)
  {
    sprintf(czTemp, "/%s %.5f Tf\n", pFont->GetFontResName(), fCountedFontSize);
    strValue.append(czTemp);
  }
  else
  {
    sprintf(czTemp, "/%s %.5f Tf\n", pFont->GetFontResName(), fDefFontSize);
    strValue.append(czTemp);
  }

  for (size_t ii = 0; ii < values.size(); ii++)
  {
    string strOut;
    CPDFString pdfStrIn(values.at(ii));
    if (!ii)
    {
      if (pFont->GetGidxString(pdfStrIn, strOut))
      {
        strOut.append(" Tj\n");
      } else
      {
        strOut.assign("(");
        strOut.append(values.at(ii));
        strOut.append(") Tj\n");
      }
      fX = Xshift.at(ii);
      sprintf(czX, "%.5f", fX);
      sprintf(czY, "%.5f", fY);
      strValue.append(czX);
      strValue.append(" ");
      strValue.append(czY);
      strValue.append(" Td\n");
      strValue.append(czHeight);
      strValue.append(" TL\n");
      strValue.append(strOut);
    }
    else
    {
      if (pFont->GetGidxString(pdfStrIn, strOut))
      {
        strOut.append(" '\n");
      } else
      {
        strOut.assign("(");
        strOut.append(values.at(ii));
        strOut.append(") '\n");
      }
      sprintf(czX, "%.5f", Xshift.at(ii) - fX);
      fX = Xshift.at(ii);
      sprintf(czY, "%ld", 0);
      strValue.append(czX);
      strValue.append(" ");
      strValue.append(czY);
      strValue.append(" Td\n");
      strValue.append(strOut);
    }
  }
  strValue.append("ET Q EMC");

  if (m_pDoc->Encrypt())
  {
    Size = strValue.size();
    pObj = (char *)PDFMalloc(Size + 1);
    if (!pObj)
      return false_a;
    strcpy(pObj, strValue.c_str());
    pObj[Size] = 0;
    if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[1], 0))
    {
      free(pObj);
      return false_a;
    }

//    pTemp = PDFescapeSpecialCharsBinary(pObj, lSize, &lSize);

    strValue.assign(pObj, Size);
    free(pObj);
  }

  Size = PDFStrLenC(" 0 obj\n<< /Length  /Matrix[1.0 0.0 0.0 1.0 0.0 0.0] /Subtype /Form /BBox [     ]/FormType 1 /Resources << /ProcSet [ /PDF /Text ] /Font << /  0 R >> >>/Type/XObject >> \nstream\n\nendstream\nendobj\n")
                      + strValue.length() + 20 + (5 * 25);

  pObj = pTemp = (char *)PDFMalloc(Size);
  if (!pTemp)
    return false_a;

  sprintf(pTemp, "%ld 0 obj\n<< /Length %ld /Matrix[1.0 0.0 0.0 1.0 0.0 0.0] /Subtype /Form /BBox [ %.5f %.5f %.5f %.5f ] /Resources << /ProcSet [ /PDF /Text ] /Font << /%s %ld 0 R >> >>/Type/XObject >> \nstream\n",
    this->lObjIndex[1], strValue.length(), 0.f, 0.f, (float)(fAnnotW),
    fAnnotH, pFont->GetFontResName(), pFont->GetFontObjNum());
  Size = PDFStrLenC(pTemp);
  pObj += Size;

  memcpy(pObj, strValue.c_str(), strValue.length());
  pObj += strValue.size();
  Size += strValue.size();

  memcpy(pObj, "\nendstream\nendobj\n", PDFStrLenC("\nendstream\nendobj\n"));
  Size += PDFStrLenC("\nendstream\nendobj\n");

  strBuffer << pTemp;
  free(pTemp);
  o_vctFormStreams.push_back(strBuffer.str());

  return true_a;


//  char *pTemp, *pObj, czX[20], czY[20], czH[20] = {"0"}, czTemp[200];
//  size_t Size;
//  float fX = 4, fY = 2, fWidth, f, fFontSize;
//  vector <string> values;
//  vector <float> Xshift;
//  string strValue;
//  ostringstream strBuffer("");
//
//  if (!m_pDoc)
//    return false_a;
//  if (this->lFont < 0 || this->lFont >= m_pDoc->GetFontsCount())
//    return false_a;
//
//  CPDFBaseFont *pFont = m_pDoc->GetFont(this->lFont);
//  if (!pFont)
//    return false_a;
//
//  fX = this->sBorderWidth * 2.f;
//  fY = this->sBorderWidth * 2.f;
//
//  strValue = "";
//
//  float fHeight, fDesc;
//  rec_a bbox = pFont->GetFontBBox();
//
//  fHeight = pFont->GetFontHeight(this->fFontSize);
//
//  fHeight = (float)((bbox.lTop - bbox.lBottom) / 1000.0);//ToDo:Is it correct???
//  if (0 != this->fFontSize)
//    fHeight *= this->fFontSize;
//  else
//  {
//    fHeight = ((this->recAnnot.lTop - this->recAnnot.lBottom) - (2 * this->sBorderWidth));
//    fHeight /= (float)((bbox.lTop - bbox.lBottom) / 1000.0);
//    fHeight /= (float)((bbox.lTop - bbox.lBottom) / 1000.0);
//    
//    //fHeight = pFont->CalcFontHeight(this->pczValue, fHeight,
//    //            (this->recAnnot.lRight - this->recAnnot.lLeft) - (2 * this->sBorderWidth));
//  }
//
//  fDesc = pFont->GetFontDescent();
//  if (0 != this->fFontSize)
//  {
//    fDesc *= this->fFontSize;
//  }
//  else
//  {
//    fDesc *= fHeight;
//    fHeight *= (float)((bbox.lTop - bbox.lBottom) / 1000.0);
//
//    fHeight = pFont->CalcFontHeight(this->pczValue, fHeight,
//                (this->recAnnot.lRight - this->recAnnot.lLeft) - (2 * this->sBorderWidth));
//  }
//  fFontSize = fHeight;
//
//
//  sprintf(czH, "%.4f", fHeight);
//  fHeight = (this->recAnnot.lTop - this->recAnnot.lBottom) - fHeight;
//  if (this->lFieldFlags & 0x1000)//multiline
//  {
//    fY = fHeight - fY;
//
//    char *pSpace = NULL, *pB, *pE;
//    fWidth = (this->recAnnot.lRight - this->recAnnot.lLeft) - (2 * this->sBorderWidth * 2.f);
//    pE = pB = this->pczValue;
//    while (0 != *pE)
//    {
//      if (' ' == *pE || '\t' == *pE)
//        pSpace = pE;
//      strValue.append(pE, 1);
//      f = pFont->GetFontStringWidth(strValue, this->fFontSize);
//      if (f > fWidth || *pE == '\r')
//      {
//        if (!pSpace || f < fWidth)
//          pSpace = pE;
//        else
//          pSpace++;
//        pE = pSpace;
//        strValue.erase();
//        strValue.append(pB, pE - pB);
//
//        pObj = PDFescapeSpecialChars(strValue.c_str());
//        strValue.assign(pObj);
//        free(pObj);
//
//        basic_string <char>::size_type index = strValue.find_last_of(" ");
//        if (index != -1 && index == (strValue.length() - 1))
//          strValue.erase(index, 1);
//
//        switch (this->lQ)
//        {
//        case 2:
//          f = pFont->GetFontStringWidth(strValue, this->fFontSize);
//          fX = (this->recAnnot.lRight - this->recAnnot.lLeft) - f - (this->sBorderWidth * 2.f);
//          break;
//        case 1:
//          f = pFont->GetFontStringWidth(strValue, this->fFontSize);
//          fX = (this->recAnnot.lRight - this->recAnnot.lLeft) - f;
//          fX /= 2;
//          break;
//        case 0:
//        default:
//          break;
//        }
//        strValue.append("\\012");
//
//        values.push_back(strValue);
//        Xshift.push_back(fX);
//        strValue.erase();
//        if (*pSpace == '\r')
//        {
//          pSpace++;
//        }
//
//        pB = pE = pSpace;
//        pSpace = NULL;
//      }
//      else
//        pE++;
//      if (0 == *pE && !strValue.empty())
//      {
//        switch (this->lQ)
//        {
//        case 2:
//          f = pFont->GetFontStringWidth(strValue, this->fFontSize);
//          fX = (this->recAnnot.lRight - this->recAnnot.lLeft) - f - (this->sBorderWidth * 2.f);
//          break;
//        case 1:
//          f = pFont->GetFontStringWidth(strValue, this->fFontSize);
//          fX = (this->recAnnot.lRight - this->recAnnot.lLeft) - f;
//          fX /= 2;
//          break;
//        case 0:
//        default:
//          break;
//        }
//
//        pObj = PDFescapeSpecialChars(strValue.c_str());
//        strValue.assign(pObj);
//        free(pObj);
//
//        values.push_back(strValue);
//        Xshift.push_back(fX);
//      }
//    }
//  }
//  else
//  {
//    fHeight /= 2;
//    fY = fHeight - fDesc;
//    switch (this->lQ)
//    {
//    case 2:
//      fWidth = pFont->GetFontStringWidth(this->pczValue, this->fFontSize);
//      fX = (this->recAnnot.lRight - this->recAnnot.lLeft) - fWidth - fX;
//      break;
//    case 1:
//      fWidth = pFont->GetFontStringWidth(this->pczValue, this->fFontSize);
//      fX = (this->recAnnot.lRight - this->recAnnot.lLeft) - fWidth;
//      fX /= 2;
//      break;
//    case 0:
//    default:
//      break;
//    }
//
//    pObj = PDFescapeSpecialChars(this->pczValue);
//    strValue.assign(pObj);
//    free(pObj);
//
////      strValue = i_pfield->m_pValue;
//    values.push_back(strValue);
//    Xshift.push_back(fX);
//  }
//
//  strValue.erase();
//
//  if (CPDFColor::eNoColor != this->m_ColorBG.GetColorType())
//  {
//    strValue.append(this->m_ColorBG.GetColorStringWithNonStrokingOperator());
//    sprintf(czTemp, "0 0 %.5f %.5f re f ", (this->recAnnot.lRight - this->recAnnot.lLeft),
//            (this->recAnnot.lTop - this->recAnnot.lBottom));
//    strValue.append(czTemp);
//  }
//  if (CPDFColor::eNoColor != this->m_ColorBC.GetColorType())
//  {
//    strValue.append(this->m_ColorBC.GetColorStringWithStrokingOperator());
//    sprintf(czTemp, "%d w ", this->sBorderWidth);
//    strValue.append(czTemp);
//    sprintf(czTemp, "%.5f %.5f %.5f %.5f re s ", this->sBorderWidth / 2, this->sBorderWidth / 2,
//              (this->recAnnot.lRight - this->recAnnot.lLeft) - this->sBorderWidth,
//              (this->recAnnot.lTop - this->recAnnot.lBottom) - this->sBorderWidth);
//    strValue.append(czTemp);
//  }
//
//  strValue.append("/Tx BMC q ");
//
//  sprintf(czTemp, "%d %d %.5f %.5f re W n ", this->sBorderWidth, this->sBorderWidth,
//            (this->recAnnot.lRight - this->recAnnot.lLeft) - (2 * this->sBorderWidth),
//            (this->recAnnot.lTop - this->recAnnot.lBottom) - (2 * this->sBorderWidth));
//  strValue.append(czTemp);
//
//  strValue.append("BT\n");
//  strValue.append(this->m_ColorDA.GetColorStringWithNonStrokingOperator());
//
//  sprintf(czTemp, "/%s %.5f Tf\n", pFont->GetFontResName(), PDFConvertDimensionToDefault(this->fFontSize));
//  strValue.append(czTemp);
//
//  for (size_t ii = 0; ii < values.size(); ii++)
//  {
//    string strOut;
//    CPDFString pdfStrIn(values.at(ii));
//    if (!ii)
//    {
//      if (pFont->GetGidxString(pdfStrIn, strOut))
//      {
//        strOut.append(" Tj\n");
//      } else
//      {
//        strOut.assign("(");
//        strOut.append(values.at(ii));
//        strOut.append(") Tj\n");
//      }
//      fX = Xshift.at(ii);
//      sprintf(czX, "%.5f", fX);
//      sprintf(czY, "%.5f", fY);
//      strValue.append(czX);
//      strValue.append(" ");
//      strValue.append(czY);
//      strValue.append(" Td\n");
//      strValue.append(czH);
//      strValue.append(" TL\n");
//      strValue.append(strOut);
//    }
//    else
//    {
//      if (pFont->GetGidxString(pdfStrIn, strOut))
//      {
//        strOut.append(" '\n");
//      } else
//      {
//        strOut.assign("(");
//        strOut.append(values.at(ii));
//        strOut.append(") '\n");
//      }
//      sprintf(czX, "%.5f", Xshift.at(ii) - fX);
//      fX = Xshift.at(ii);
//      sprintf(czY, "%ld", 0);
//      strValue.append(czX);
//      strValue.append(" ");
//      strValue.append(czY);
//      strValue.append(" Td\n");
//      strValue.append(strOut);
//    }
//  }
//  strValue.append("ET Q EMC");
//
//  if (m_pDoc->Encrypt())
//  {
//    Size = strValue.size();
//    pObj = (char *)PDFMalloc(Size + 1);
//    if (!pObj)
//      return false_a;
//    strcpy(pObj, strValue.c_str());
//    pObj[Size] = 0;
//    if (!PDFEncrypt(m_pDoc, (unsigned char *)pObj, Size, this->lObjIndex[1], 0))
//    {
//      free(pObj);
//      return false_a;
//    }
//
////    pTemp = PDFescapeSpecialCharsBinary(pObj, lSize, &lSize);
//
//    strValue.assign(pObj, Size);
//    free(pObj);
//  }
//
//  Size = PDFStrLenC(" 0 obj\n<< /Length  /Matrix[1.0 0.0 0.0 1.0 0.0 0.0] /Subtype /Form /BBox [     ]/FormType 1 /Resources << /ProcSet [ /PDF /Text ] /Font << /  0 R >> >>/Type/XObject >> \nstream\n\nendstream\nendobj\n")
//                      + strValue.length() + 20 + (5 * 25);
//
//  pObj = pTemp = (char *)PDFMalloc(Size);
//  if (!pTemp)
//    return false_a;
//
//  sprintf(pTemp, "%ld 0 obj\n<< /Length %ld /Matrix[1.0 0.0 0.0 1.0 0.0 0.0] /Subtype /Form /BBox [ %.5f %.5f %.5f %.5f ] /Resources << /ProcSet [ /PDF /Text ] /Font << /%s %ld 0 R >> >>/Type/XObject >> \nstream\n",
//    this->lObjIndex[1], strValue.length(), 0.f, 0.f, (float)(this->recAnnot.lRight - this->recAnnot.lLeft),
//    (float)(this->recAnnot.lTop - this->recAnnot.lBottom), pFont->GetFontResName(), pFont->GetFontObjNum());
//  Size = PDFStrLenC(pTemp);
//  pObj += Size;
//
//  memcpy(pObj, strValue.c_str(), strValue.length());
//  pObj += strValue.size();
//  Size += strValue.size();
//
//  memcpy(pObj, "\nendstream\nendobj\n", PDFStrLenC("\nendstream\nendobj\n"));
//  Size += PDFStrLenC("\nendstream\nendobj\n");
//
//  strBuffer << pTemp;
//  free(pTemp);
//  o_vctFormStreams.push_back(strBuffer.str());
//
//  return true_a;
}
