#ifndef __PDFOUTLINEENTRY_H__
#define __PDFOUTLINEENTRY_H__

#include "XPubPDFExpImp.h"
#include "pdfdef.h"

#pragma  warning(disable:4251)

class CPDFDocument;

class XPUBPDF_EXPORTIMPORT CPDFOutlineEntry :private PDFOutlineEntry
{
public:
  CPDFOutlineEntry(CPDFDocument *i_pDoc);
  ~CPDFOutlineEntry();

  CPDFOutlineEntry *AddOutlineEntry(CPDFOutlineEntry *i_pFollowThis, bool_a i_bSubEntry, bool_a i_bOpen,
                                    long i_lPage, char *i_pTitle, eDESTSPEC i_eDest, long i_lLeft,
                                    long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                                    float i_fR, float i_fG, float i_fB, short i_sFkey, long i_lFont);
  CPDFOutlineEntry *GetNext(){return (CPDFOutlineEntry *)pNext;};
  CPDFOutlineEntry *GetPrev(){return (CPDFOutlineEntry *)pPrev;};
  CPDFOutlineEntry *GetFirstSub(){return (CPDFOutlineEntry *)pFirst;};
  CPDFOutlineEntry *GetLastSub(){return (CPDFOutlineEntry *)pLast;};
  CPDFOutlineEntry *GetParent(){return (CPDFOutlineEntry *)pParent;};

  float GetZoom() {return m_fZoom;};
  eDESTSPEC GetDestSpec() {return m_eDest;};
  const std::string& GetAnchor();
  void SetAnchor(std::string i_strAnchor);
  void SetIgnorePos(bool_a i_bIgnore = true_a);
  bool_a GetIgnorePos();
  bool_a SetDestination(long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                          long i_lTop, long i_lRight, long i_lBottom, float i_fZoom);

private:

  float m_fZoom;
  eDESTSPEC m_eDest;
  std::string m_strAnchor;
  bool_a m_bIgnorePos;
  CPDFDocument *m_pDoc;
};


#endif  /*  __PDFOUTLINEENTRY_H__  */
