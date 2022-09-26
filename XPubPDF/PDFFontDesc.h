#ifndef __PDFFONTDESCRIPTOR_H__
#define __PDFFONTDESCRIPTOR_H__

#include "XPubPDFExpImp.h"
#include "pdfdef.h"

#pragma  warning(disable:4251)

class CPDFBaseFont;
class CPDFDocument;

class XPUBPDF_EXPORTIMPORT CPDFFontDescriptor
{
public:

  CPDFFontDescriptor(CPDFDocument *i_pDoc = NULL);
  ~CPDFFontDescriptor();

enum
{
  eFixedPitchDesc   = 0x01,
  eSerifDesc        = 0x02,
  eSymbolicDesc     = 0x04,
  eScriptDesc       = 0x08,
  eNonsymbolicDesc  = 0x20,
  eItalicDesc       = 0x40,
  eAllCapDesc       = 0x10000,
  eSmallCapDesc     = 0x20000,
  eForceBoldDesc    = 0x40000
} eFontFlagDesc;

  long GetObjNum() {return m_lObjNum;};

  const char *GetFontName() {return m_strFontName.c_str();};
  long GetFlags() {return m_lFlags;};
  rec_a GetFontBBox() {return m_recFontBBox;};
  short GetItalicAngle() {return m_sItalicAngle;};
  short GetAscent() {return m_sAscent;};
  short GetDescent() {return m_sDescent;};
  short GetCapHeight() {return m_sCapHeight;};
  short GetXHeight() {return m_sXHeight;};
  short GetStemV() {return m_sStemV;};
  short GetStemH() {return m_sStemH;};
  virtual const std::string& GetFontFile() {return m_strFontFile;};
  virtual const std::string& GetFontFile2() {return m_strFontFile2;};
  virtual const std::string& GetFontFile3() {return m_strFontFile3;};

  short GetUnderlinePosition() {return m_sUnderlinePosition;};
  short GetUnderlineThickness() {return m_sUnderlineThickness;};

protected:

  friend class CPDFTemplate;
  friend class CPDFBaseFont;
  friend class CPDFTrueTypeFont;
  friend class CPDFType0Font;

  void SetObjNum(long i_lObjNum);
  virtual bool_a HasDescriptorStream();
  virtual bool_a GetDescriptorStream(std::string &o_strStream);


  virtual bool_a HasFontEmbedStream() {return false_a;};
  virtual void SetFontEmbedStreamObjNum(long i_lObjNum) {m_lEmbedObjNum = i_lObjNum;};
  virtual long GetFontEmbedStreamObjNum() {return m_lEmbedObjNum;};
  virtual bool_a GetEmbedStream(std::string& o_strEmbed) {return false_a;};


  void SetFontName(const char *i_pczFontName);
  void SetFlags(long i_lFlags);
  void SetFontBBox(rec_a i_rec);
  void SetItalicAngle(short i_sAngle);
  void SetAscent(short i_sAscent);
  void SetDescent(short i_sDescent);
  void SetCapHeight(short i_sCapHeight);
  void SetXHeight(short i_sXHeight);
  void SetStemV(short i_sStemV);
  void SetStemH(short i_sStemH);

  void SetUnderlinePosition(short i_sPos);
  void SetUnderlineThickness(short i_sThick);

  void SetEmbedded(bool_a i_bEmbed = true_a);
  void SetEmbedStream(std::string& i_strEmbedStream);

protected:

  long m_lObjNum;

  std::string m_strEmbedStream;
  long m_lEmbedObjNum;
  bool_a m_bFontEmbed;

  CPDFDocument *m_pDoc;

  std::string m_strFontName;
  //std::string m_strFontFamily;
  //FontStretch
  //FontWeight
  long m_lFlags;
  rec_a m_recFontBBox;
  short m_sItalicAngle;
  short m_sAscent;
  short m_sDescent;
  //Leading
  short m_sCapHeight;
  short m_sXHeight;
  short m_sStemV;
  short m_sStemH;
  //AvgWidth
  //MaxWidth
  //MissingWidth
  std::string m_strFontFile;
  std::string m_strFontFile2;
  std::string m_strFontFile3;
  //CharSet

  short m_sUnderlinePosition;
  short m_sUnderlineThickness;

};


#endif  /*  __PDFFONTDESCRIPTOR_H__  */
