
#include "pdffnc.h"
#include "PDFOutlineEntry.h"

using namespace std;

CPDFOutlineEntry::CPDFOutlineEntry(CPDFDocument *i_pDoc)
  :m_pDoc(i_pDoc)
{
  m_bIgnorePos = false_a;
  m_fZoom = 0;
  this->pTitle = NULL;
  this->pDestSpec = NULL;
}

CPDFOutlineEntry::~CPDFOutlineEntry()
{
  if(this->pTitle)
    free(this->pTitle);
  if(this->pDestSpec)
    free(this->pDestSpec);
}

CPDFOutlineEntry *CPDFOutlineEntry::AddOutlineEntry(CPDFOutlineEntry *i_pFollowThis, bool_a i_bSubEntry,
                                                    bool_a i_bOpen, long i_lPage, char *i_pTitle,
                                                    eDESTSPEC i_eDest, long i_lLeft, long i_lTop,
                                                    long i_lRight, long i_lBottom, float i_fZoom,
                                                    float i_fR, float i_fG, float i_fB, short i_sFkey, long i_lFont)
{
  PDFAddOutlineEntry((PDFDocPTR)m_pDoc, i_pFollowThis, i_bSubEntry, i_bOpen, i_lPage, i_pTitle,
      i_eDest, i_lLeft, i_lTop, i_lRight, i_lBottom, i_fZoom,
      i_fR, i_fG, i_fB, i_sFkey, i_lFont,
      (PDFOutlineEntry *)this);
  m_eDest = i_eDest;
  m_fZoom = i_fZoom;

  return this;
}

bool_a CPDFOutlineEntry::SetDestination(long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                                        long i_lTop, long i_lRight, long i_lBottom, float i_fZoom)
{
  this->lPage = i_lPage - 1;

  free(this->pDestSpec);
  this->pDestSpec = NULL;
  m_fZoom = i_fZoom;

  if (!PDFDestSpec(&(this->pDestSpec), i_eDest, i_lLeft, i_lTop, i_lRight, i_lBottom, m_fZoom))
    return false_a;

  return true_a;
}

void CPDFOutlineEntry::SetIgnorePos(bool_a i_bIgnore)
{
  m_bIgnorePos = i_bIgnore;
}

bool_a CPDFOutlineEntry::GetIgnorePos()
{
  return m_bIgnorePos;
}

const string& CPDFOutlineEntry::GetAnchor()
{
  return m_strAnchor;
}

void CPDFOutlineEntry::SetAnchor(string i_strAnchor)
{
  m_strAnchor.assign(i_strAnchor);
}
