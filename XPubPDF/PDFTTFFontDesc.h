#ifndef __PDFTTFFONTDESCRIPTOR_H__
#define __PDFTTFFONTDESCRIPTOR_H__

#include "XPubPDFExpImp.h"
#include "PDFFontDesc.h"
#include "pdfdef.h"


class XPUBPDF_EXPORTIMPORT CPDFTTFFontDescriptor :public CPDFFontDescriptor
{
public:
  CPDFTTFFontDescriptor(CPDFDocument *i_pDoc = NULL);
  ~CPDFTTFFontDescriptor();

protected:

  virtual bool_a HasFontEmbedStream();
  virtual void SetFontEmbedStreamObjNum(long i_lObjNum);
  virtual long GetFontEmbedStreamObjNum();
  virtual bool_a GetEmbedStream(std::string& o_strEmbed);

  virtual bool_a HasDescriptorStream();
  virtual bool_a GetDescriptorStream(std::string &o_strStream);
//private:
  friend class CPDFTrueTypeFont;
  friend class CPDFType0Font;

  virtual bool_a Init();

};


#endif  /*  __PDFTTFFONTDESCRIPTOR_H__  */
