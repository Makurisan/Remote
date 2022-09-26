#ifndef __PDFIMAGE_H__
#define __PDFIMAGE_H__

#include "XPubPDFExpImp.h"
#include "pdfdef.h"

class CPDFDocument;

class XPUBPDF_EXPORTIMPORT CPDFImage :public PDFImage
{
public:
  CPDFImage(CPDFDocument *i_pDoc);
  ~CPDFImage();

  //static CPDFImage *MakeImage(const char *i_pImageName, long i_lID);

  //const std::string &GetImageFileName();
  long GetImageID();
  void AssignObjNum(long &io_lObjNum);
  bool_a GetEntireImageStream(std::vector<std::string> &o_vctFontStreams);
  const char *GetImageResName() {return this->name;};
  long GetImageObjNum() {return this->lObjIndex;};
  //const std::string &GetImageResName();

//protected:
//  CPDFImage(const char *i_pImageName, long i_lID, CPDFDocument *i_pDoc = NULL);

  CPDFDocument *m_pDoc;
};


#endif  /*  __PDFIMAGE_H__  */
