#ifndef __PDFLAYER_H__
#define __PDFLAYER_H__
#pragma warning( disable: 4251 )

#include "XPubPDFExpImp.h"
#include <map>

#include "PDFDocument.h"

class XPUBPDF_EXPORTIMPORT CPDFLayer :public CPDFDocument
{
public:
  CPDFLayer(CPDFDocument *i_pDoc);
  ~CPDFLayer();

  typedef enum
  {
    eViewIntent = 0,
    eDesignIntent = 1,
  } eLayerIntent;


  long GetObjNum() {return m_lObjNum;};
  void SetObjNum(long i_lObjNum) {m_lObjNum = i_lObjNum;};
  const char *GetLayerName() {return m_strName.c_str();};
  void SetLayerName(const char *i_pName);
  const char *GetLayerResName() {return m_strResName.c_str();};
  void SetLayerResName(const char *i_pResName);

  bool_a GetLayerStream(std::string &o_stream);

  eLayerIntent GetLayerIntent();
  void SetLayerIntent(eLayerIntent i_eIntent);
  bool_a IsLocked() {return m_bLocked;};
  void SetLocked(bool_a i_bLocked = true_a) {m_bLocked = i_bLocked;};


  virtual float GetFontSize();
  virtual float GetConvertFontSize();
  virtual long GetActualFontHeight();
  virtual long GetFontHeight(long i_lID);
  virtual long GetActualFontAscentHeight();
  virtual long GetActualFontDescentHeight();
  virtual long GetFontsCount();
  virtual CPDFBaseFont *GetActualFont();
  virtual long GetActualFontID();
  virtual void SetActualFontID(long i_lID);
  virtual CPDFBaseFont *GetFont(long i_lID);
  virtual CPDFBaseFont *SetActualFont(const char *i_pFontName, float i_fSize, long i_lCodePage = NOT_USED);
  virtual CPDFBaseFont *SetActualFont(const char *i_pFontName);
  virtual CPDFBaseFont *SetDefaultFont(const char *i_pFontName, float i_fSize);
  virtual CPDFBaseFont *SetDefaultFont(const char *i_pFontName);
  virtual bool_a SetDefaultFontSize(long i_fSize);
  virtual bool_a SetFontSize(float i_fSize);
  virtual bool_a SetFontStyle(short i_sFlag);
  virtual float GetActualFontSize();
  virtual void SetActualFontSize(float i_fSize);
  virtual long GetActualFontStyle();
  virtual void SetActualFontStyle(long i_lStyle);
  virtual long GetActualCodePage();
  virtual bool_a SetActualCodePage(long i_lCodePage);



  virtual CPDFImage *CreateImage(const char *i_pImage);
  virtual long GetImagesCount();
  virtual bool_a AddImageToPage(long i_lPage, CPDFImage *i_pImage, long i_lX, long i_lY,
                        long i_lWidth, long i_lHeight, long i_lRotate = 0);
  virtual bool_a AddImageToPage(long i_lPage, long i_lIndex, long i_lX, long i_lY,
                        long i_lWidth, long i_lHeight, long i_lRotate = 0);
  virtual CPDFImage *GetImage(long i_lID);

  bool_a GetPageContent(long i_lPage, std::string &o_strContent);

  const std::string &GetGroup();
  void SetGroup(const char *i_pGroup);
  const std::string &GetParent();
  void SetParent(const char *i_pParent);
  bool_a GetDefaultState();
  void SetDefaultState(bool_a i_bState);

  void InsertChildLayer(CPDFLayer *i_pChild);
  virtual CPDFLayer *GetLayer(const char *i_pName);
  virtual bool_a GetLayerOrderStream(std::string &o_stream);

protected:


  std::string m_strParent;
  std::string m_strGroup;
  bool_a m_bDefState;

  std::vector<CPDFLayer *> m_vctChilds;

  long m_lObjNum;
  std::string m_strName;
  std::string m_strResName;
  eLayerIntent m_eIntent;
  bool_a m_bLocked;
  CPDFDocument *m_pMainDoc;
};

#endif  /*  __PDFLAYER_H__  */

