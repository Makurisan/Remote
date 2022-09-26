#ifndef __PDFANNOTATION_H__
#define __PDFANNOTATION_H__

#include "XPubPDFExpImp.h"
#include "pdfdef.h"

class CPDFDocument;


class XPUBPDF_EXPORTIMPORT CPDFAnnotation :protected PDFAnnotation
{
public:
  CPDFAnnotation(CPDFDocument *i_pDoc, eDICTFIELDTYPE i_eType, const char *i_pczName,
                  rec_a i_recPosSize, long i_lPage = -1);
  CPDFAnnotation(CPDFDocument *i_pDoc, const char *i_pczName, long i_lPage,
                             char *i_pczDest, char *i_pczAction, float i_fZoom);
  ~CPDFAnnotation();


  void SetPosSize(rec_a i_recPosSize);      //set position and size of formfield
  rec_a GetPosSize();

  const char *GetName();
  void SetName(const char *i_pczName);      //set name(ID) of formfield

  //Set Get flag /F

  void SetPage(long i_lPage);               //set page, where formfield will be placed
  long GetPage();

  //Set Get check type

  CPDFColor &GetBGColor(); //get bacground color of formfield
  void SetBorderWidth(short i_sWidth);      //set border width of formfield
  short GetBorderWidth();
  CPDFColor &GetBorderColor(); //get border color of formfield

  CPDFColor &GetColor(); //get text color of formfield

  //Set Get flag /Ff

  short GetAlignment();
  void SetAlignment(short i_sAlignment) { this->lQ = i_sAlignment; };  //set text alignment of formfield

  void SetShortDesc(const char *i_pczDesc);  //set short description of formfield (like tooltip)
  const char *GetShortDesc();

  void SetValue(const char *i_pczText);     //set value of formfield
  const char *GetValue();

  void SetMaxLen(long i_lLen);              //set max. length of text in formfield
  long GetMaxLen();

  eANNOTATIONTYPE GetAnnotationType(){ return this->eAnnotType;};

  void SetDocument(CPDFDocument *i_pDoc);
  PDFDocPTR GetBaseDoc();

  void AssignObjNum(long &io_lObjNum);
  bool_a GetEntireFormStream(std::vector<std::string> &o_vctFormStreams);
  long GetFormObjNum() {return this->lObjIndex[0];};

protected:
  bool_a GetAPStream(std::vector<std::string> &o_vctFormStreams);
  bool_a GetAPOnNormal(std::vector<std::string> &o_vctFormStreams);
  bool_a GetAPOffNormal(std::vector<std::string> &o_vctFormStreams);
  bool_a GetAPOnDown(std::vector<std::string> &o_vctFormStreams);
  bool_a GetAPOffDown(std::vector<std::string> &o_vctFormStreams);
  bool_a GetAPNormal(std::vector<std::string> &o_vctFormStreams);


  CPDFDocument  *m_pDoc;
};


#endif  /*  __PDFANNOTATION_H__  */
