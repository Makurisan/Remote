#ifndef __PDFTYPE0FONT_H__
#define __PDFTYPE0FONT_H__

#include "XPubPDFExpImp.h"
#include "PDFBaseFont.h"
#include "pdfdef.h"
#include <string>

#include "pdfttf.h"


class CPDFDocument;
class CPDFTTFFontDescriptor;


class XPUBPDF_EXPORTIMPORT CPDFType0Font :public CPDFBaseFont
{
public:
  ~CPDFType0Font();

  static CPDFBaseFont *MakeFont(const char *i_pFontName, long i_lID, 
                                long i_lStyle = CPDFBaseFont::ePDFStandard,
                                long i_lCodePage = NOT_USED, CPDFDocument *i_pDoc = NULL);

  virtual bool_a GetFontStream(std::string &o_strFontStream);
  virtual bool_a GetFontStreamWinEncoding(std::string &o_strFontStream);

  virtual bool_a GetToUnicodeStream(std::string& o_strToUnicode);
  virtual bool_a HasFontToUnicodeStream();

  virtual bool_a HasFontDescendantFonts();
  virtual long GetFontDescendantFontsStreamObjNum() {return m_lDescendantFontsObjNum;};
  virtual void SetFontDescendantFontsStreamObjNum(long i_lObjNum) {m_lDescendantFontsObjNum = i_lObjNum;};
  virtual bool_a GetFontDescendantFontsStream(std::string &o_strFontStream);

  virtual bool_a HasFontEncodingStream();

  virtual void AssignObjNum(long &io_lObjNum);
  virtual bool_a GetEntireFontStream(std::vector<std::string> &o_vctFontStreams);
  virtual void AssignObjNumType0(long &io_lObjNum);
  virtual bool_a GetEntireFontStreamType0(std::vector<std::string> &o_vctFontStreams);

  virtual bool_a GetGidxString(CPDFString& i_str, std::string& o_strOut);

protected:
  CPDFType0Font(const char *i_pFontName, long i_lID, 
                long i_lStyle = CPDFBaseFont::ePDFStandard,
                long i_lCodePage = NOT_USED, CPDFDocument *i_pDoc = NULL);

  virtual bool_a Init();
  CPDFFontDescriptor *MakeDescriptor();

  bool_a GetFontType0Stream(std::string &o_strFontStream);

  bool_a PdfGetTTF(pdf_font *font, const char *fontname, long enc, long lFontFlags);

protected://private:

  long m_lDescendantFontsObjNum;
};


#endif  /*  __PDFTYPE0FONT_H__  */
