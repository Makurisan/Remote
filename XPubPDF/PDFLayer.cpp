

#include "pdffnc.h"
#include "PDFLayer.h"

using namespace std;


CPDFLayer::CPDFLayer(CPDFDocument *i_pDoc)
:m_strParent(""), m_strGroup(""), m_bDefState(true_a), m_lObjNum(NOT_USED),
  m_strName(""), m_pMainDoc(i_pDoc), m_strResName(""), m_eIntent(eViewIntent), m_bLocked(false_a)
{
}

CPDFLayer::~CPDFLayer()
{
  //main doc delete it 
  //std::vector<CPDFLayer *>::iterator iterLayer;
  //for (iterLayer = m_vctChilds.begin(); iterLayer != m_vctChilds.end(); iterLayer++)
  //{
  //  CPDFLayer *pLayer = (CPDFLayer *)(*iterLayer);
  //  delete pLayer;
  //}
  m_vctChilds.clear();
}

void CPDFLayer::SetLayerName(const char *i_pName)
{
  if (i_pName)
    m_strName = i_pName;
}

void CPDFLayer::SetLayerResName(const char *i_pResName)
{
  if (i_pResName)
    m_strResName = i_pResName;
}

const std::string &CPDFLayer::GetGroup()
{
  return m_strGroup;
}

void CPDFLayer::SetGroup(const char *i_pGroup)
{
  if (i_pGroup)
    m_strGroup = i_pGroup;
}

const std::string &CPDFLayer::GetParent()
{
  return m_strParent;
}

void CPDFLayer::SetParent(const char *i_pParent)
{
  if (i_pParent)
    m_strParent = i_pParent;
}

bool_a CPDFLayer::GetDefaultState()
{
  return m_bDefState;
}

void CPDFLayer::SetDefaultState(bool_a i_bState)
{
  m_bDefState = i_bState;
}

CPDFLayer::eLayerIntent CPDFLayer::GetLayerIntent()
{
  return m_eIntent;
}

void CPDFLayer::SetLayerIntent(CPDFLayer::eLayerIntent i_eIntent)
{
  m_eIntent = i_eIntent;
}

void CPDFLayer::InsertChildLayer(CPDFLayer *i_pChild)
{
  if (!i_pChild)
    return;
  m_vctChilds.push_back(i_pChild);
}

CPDFLayer *CPDFLayer::GetLayer(const char *i_pName)
{
  if (!i_pName)
    return NULL;
  if (0 == m_strName.compare(i_pName))
    return this;
  std::vector<CPDFLayer *>::iterator iterLayer;
  for (iterLayer = m_vctChilds.begin(); iterLayer != m_vctChilds.end(); iterLayer++)
  {
    CPDFLayer *pLayer = (CPDFLayer *)(*iterLayer);
    pLayer = pLayer->GetLayer(i_pName);
    if (pLayer)
      return pLayer;
  }
  return NULL;
}

bool_a CPDFLayer::GetLayerOrderStream(std::string &o_stream)
{
  string strTemp;
  char czTemp[100];
  o_stream.clear();

  sprintf(czTemp, "%ld 0 R ", GetObjNum());
  o_stream.append(czTemp);

  if (m_vctChilds.size() > 0)
    o_stream.append("[ ");
  for (long i = 0;i < m_vctChilds.size(); i++)
  {
    CPDFLayer *pLayer = m_vctChilds.at(i);
    if (pLayer)
    {
      pLayer->GetLayerOrderStream(strTemp);
      o_stream.append(strTemp);
      strTemp.clear();
    }
  }
  if (m_vctChilds.size() > 0)
    o_stream.append("] ");

  return true_a;
}

bool_a CPDFLayer::GetPageContent(long i_lPage, string &o_strContent)
{
  string strTemp;
  long lSize;
  o_strContent.clear();

  CPDFPage *pPage = GetPage(i_lPage);
  if (!pPage)
    return false_a;

  o_strContent.append("/OC /");
  o_strContent.append(GetLayerResName());
  o_strContent.append(" BDC\n");


  const char *pBuf = pPage->GetContent(lSize);
  if (pBuf && lSize > 0)
  {
    o_strContent.append(pBuf, lSize);
  }
  o_strContent.append("\n");



  for (long i = 0;i < m_vctChilds.size(); i++)
  {
    CPDFLayer *pLayer = m_vctChilds.at(i);
    if (pLayer)
    {
      pLayer->GetPageContent(i_lPage, strTemp);
      o_strContent.append(strTemp);
      strTemp.clear();
    }
  }


  o_strContent.append("\nEMC\n");

  return true_a;
}

bool_a CPDFLayer::GetLayerStream(std::string &o_stream)
{
  o_stream.clear();
  char czTemp[200];

  
  sprintf(czTemp, "%ld 0 obj << /Type /OCG ", m_lObjNum);
  o_stream.append(czTemp);

  if (m_eIntent == eDesignIntent)
  {
    o_stream.append("/Intent /Design ");
  }

  o_stream.append("/Name (");
  if (m_pMainDoc->Encrypt() && m_strName.size() > 0)
  {
    char *pBuf = PDFStrDup(m_strName.c_str());
    PDFEncrypt(m_pMainDoc, (unsigned char *)pBuf, m_strName.size(), m_lObjNum, 0);
    o_stream.append(pBuf, m_strName.size());
    free(pBuf);
  } else {
    o_stream.append(m_strName);
  }
  o_stream.append(")");

  o_stream.append(">> endobj\n");

  return true_a;
}

float CPDFLayer::GetFontSize()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetFontSize();
  return CPDFDocument::GetFontSize();
}

float CPDFLayer::GetConvertFontSize()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetConvertFontSize();
  return CPDFDocument::GetConvertFontSize();
}

long CPDFLayer::GetActualFontHeight()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetActualFontHeight();
  return CPDFDocument::GetActualFontHeight();
}

long CPDFLayer::GetFontHeight(long i_lID)
{
  if (m_pMainDoc)
    return m_pMainDoc->GetFontHeight(i_lID);
  return CPDFDocument::GetFontHeight(i_lID);
}

long CPDFLayer::GetActualFontAscentHeight()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetActualFontAscentHeight();
  return CPDFDocument::GetActualFontAscentHeight();
}

long CPDFLayer::GetActualFontDescentHeight()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetActualFontDescentHeight();
  return CPDFDocument::GetActualFontDescentHeight();
}

long CPDFLayer::GetFontsCount()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetFontsCount();
  return CPDFDocument::GetFontsCount();
}

CPDFBaseFont *CPDFLayer::GetActualFont()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetActualFont();
  return CPDFDocument::GetActualFont();
}

long CPDFLayer::GetActualFontID()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetActualFontID();
  return CPDFDocument::GetActualFontID();
}

void CPDFLayer::SetActualFontID(long i_lID)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetActualFontID(i_lID);
  return CPDFDocument::SetActualFontID(i_lID);
}

CPDFBaseFont *CPDFLayer::GetFont(long i_lID)
{
  if (m_pMainDoc)
    return m_pMainDoc->GetFont(i_lID);
  return CPDFDocument::GetFont(i_lID);
}

CPDFBaseFont *CPDFLayer::SetActualFont(const char *i_pFontName, float i_fSize, long i_lCodePage)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetActualFont(i_pFontName, i_fSize, i_lCodePage);
  return CPDFDocument::SetActualFont(i_pFontName, i_fSize, i_lCodePage);
}

CPDFBaseFont *CPDFLayer::SetActualFont(const char *i_pFontName)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetActualFont(i_pFontName);
  return CPDFDocument::SetActualFont(i_pFontName);
}

CPDFBaseFont *CPDFLayer::SetDefaultFont(const char *i_pFontName, float i_fSize)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetDefaultFont(i_pFontName, i_fSize);
  return CPDFDocument::SetDefaultFont(i_pFontName, i_fSize);
}

CPDFBaseFont *CPDFLayer::SetDefaultFont(const char *i_pFontName)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetDefaultFont(i_pFontName);
  return CPDFDocument::SetDefaultFont(i_pFontName);
}

bool_a CPDFLayer::SetDefaultFontSize(long i_fSize)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetDefaultFontSize(i_fSize);
  return CPDFDocument::SetDefaultFontSize(i_fSize);
}

bool_a CPDFLayer::SetFontSize(float i_fSize)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetFontSize(i_fSize);
  return CPDFDocument::SetFontSize(i_fSize);
}

bool_a CPDFLayer::SetFontStyle(short i_sFlag)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetFontStyle(i_sFlag);
  return CPDFDocument::SetFontStyle(i_sFlag);
}

float CPDFLayer::GetActualFontSize()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetActualFontSize();
  return CPDFDocument::GetActualFontSize();
}

void CPDFLayer::SetActualFontSize(float i_fSize)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetActualFontSize(i_fSize);
  return CPDFDocument::SetActualFontSize(i_fSize);
}

long CPDFLayer::GetActualFontStyle()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetActualFontStyle();
  return CPDFDocument::GetActualFontStyle();
}

void CPDFLayer::SetActualFontStyle(long i_lStyle)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetActualFontStyle(i_lStyle);
  return CPDFDocument::SetActualFontStyle(i_lStyle);
}

long CPDFLayer::GetActualCodePage()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetActualCodePage();
  return CPDFDocument::GetActualCodePage();
}

bool_a CPDFLayer::SetActualCodePage(long i_lCodePage)
{
  if (m_pMainDoc)
    return m_pMainDoc->SetActualCodePage(i_lCodePage);
  return CPDFDocument::SetActualCodePage(i_lCodePage);
}


CPDFImage *CPDFLayer::CreateImage(const char *i_pImage)
{
  if (m_pMainDoc)
    return m_pMainDoc->CreateImage(i_pImage);
  return CPDFDocument::CreateImage(i_pImage);
}

long CPDFLayer::GetImagesCount()
{
  if (m_pMainDoc)
    return m_pMainDoc->GetImagesCount();
  return CPDFDocument::GetImagesCount();
}

bool_a CPDFLayer::AddImageToPage(long i_lPage, CPDFImage *i_pImage, long i_lX, long i_lY,
                        long i_lWidth, long i_lHeight, long i_lRotate)
{
//  if (m_pMainDoc)
//    return m_pMainDoc->AddImageToPage(i_lPage, i_pImage, i_lX, i_lY, i_lWidth, i_lHeight, i_lRotate);
  return CPDFDocument::AddImageToPage(i_lPage, i_pImage, i_lX, i_lY, i_lWidth, i_lHeight, i_lRotate);
}

bool_a CPDFLayer::AddImageToPage(long i_lPage, long i_lIndex, long i_lX, long i_lY,
                        long i_lWidth, long i_lHeight, long i_lRotate)
{
//  if (m_pMainDoc)
//    return m_pMainDoc->AddImageToPage(i_lPage, i_lIndex, i_lX, i_lY, i_lWidth, i_lHeight, i_lRotate);
  return CPDFDocument::AddImageToPage(i_lPage, i_lIndex, i_lX, i_lY, i_lWidth, i_lHeight, i_lRotate);
}

CPDFImage *CPDFLayer::GetImage(long i_lID)
{
  if (m_pMainDoc)
    return m_pMainDoc->GetImage(i_lID);
  return CPDFDocument::GetImage(i_lID);
}
