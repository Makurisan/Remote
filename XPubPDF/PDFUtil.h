#ifndef __PDFUTIL_H__
#define __PDFUTIL_H__

#include "XPubPDFExpImp.h"
#include <string>

#define bool_a        int
#define false_a       0
#define true_a        1

#pragma  warning(disable:4251)
#pragma  warning(disable:4172)
#pragma  warning(disable:4996)

class XPUBPDF_EXPORTIMPORT CPDFColor
{

public:

  CPDFColor();
  CPDFColor(float i_fR, float i_fG, float i_fB);
  CPDFColor(float i_fC, float i_fM, float i_fY, float i_fK);
  CPDFColor(float i_fGray);
  ~CPDFColor();

  typedef enum
  {
    eNoColor = -1,
    eRgbColor = 1,
    eCmykColor,
    eGrayColor
  } eColorType;

  void operator=(const CPDFColor& r);

  void SetColor(float i_fR, float i_fG, float i_fB);
  void SetColor(float i_fC, float i_fM, float i_fY, float i_fK);
  void SetColor(float i_fGray);

  const char *GetColorString();
  const char *GetColorStringWithNonStrokingOperator();
  const char *GetColorStringWithStrokingOperator();

  float GetR(){return m_RGB.fR;};
  float GetG(){return m_RGB.fG;};
  float GetB(){return m_RGB.fB;};
  float GetC(){return m_CMYK.fC;};
  float GetM(){return m_CMYK.fM;};
  float GetY(){return m_CMYK.fY;};
  float GetK(){return m_CMYK.fK;};
  float GetGray(){return m_fGray;};

  eColorType GetColorType();
  void SetColorTypeToEmpty(){m_eColorType = eNoColor;};
  //bool_a Valid();
  void Invalidate();


private:

  union
  {
    struct
    {
      float fR;
      float fG;
      float fB;
    } m_RGB;
    struct
    {
      float fC;
      float fM;
      float fY;
      float fK;
    } m_CMYK;
    float m_fGray;
  };

  eColorType m_eColorType;
  std::string m_strColor;
  std::string m_strColorWithNonStrokingOperator;
  std::string m_strColorWithStrokingOperator;
};

class XPUBPDF_EXPORTIMPORT CPDFChar
{
public:
  CPDFChar();
  CPDFChar(char i_cChar);
  CPDFChar(unsigned char i_ucChar);
  CPDFChar(wchar_t i_wChar);
  ~CPDFChar();

  typedef enum
  {
    eUndefinedString = -1,
    eMultibyteString = 1,
    eWideString,
  } eStringType;

  void operator=(const CPDFChar& c);
  void operator=(const char& c);
  void operator=(const wchar_t& w);

  void SetChar(char i_cChar);
  void SetChar(unsigned char i_ucChar);
  void SetChar(wchar_t i_wChar);

  bool_a IsSpace();
  bool_a IsWhite();
  bool_a IsTab();
  bool_a IsNewLine();
  bool_a IsBrek();
  bool_a Equal(CPDFChar pdfChar);
  const char GetChar();
  const wchar_t GetWChar();

  eStringType GetCharType();
  bool_a Valid();
  void Invalidate();

  union
  {
    char m_cChar;
    wchar_t m_wChar;
  };

  eStringType m_eCharType;
};

class XPUBPDF_EXPORTIMPORT CPDFString
{
public:

  //CPDFString();
  CPDFString(std::wstring &i_wstr);
  CPDFString(const wchar_t *i_pWstr);
  CPDFString(const wchar_t *i_pWstr, size_t size);
  CPDFString(std::string &i_str);
  CPDFString(const char *i_pStr);
  CPDFString(const char *i_pStr, size_t size);
  CPDFString(CPDFString& rPDFString);
  ~CPDFString();

  void operator=(const CPDFString& r);

  void SetString(std::wstring &i_wstr);
  void SetString(std::string &i_str);
  void SetString(const char *i_pStr);
  void SetString(CPDFString& rPDFString);
  
  bool_a SubString(CPDFString& o_Str, size_t Off, size_t Count);
  bool_a Erase(size_t Off, size_t Count);
  bool_a Append(CPDFChar pdfChar);

  bool_a HasOnlyWhiteSigns();
  bool_a IsLastSpace();
  size_t SpaceCount();

  //const void *GetString(long &o_lSize);
  const std::string *GetString();
  const std::wstring *GetWString();
  size_t Length();
  bool_a Empty();
  CPDFChar GetPDFChar(size_t i_index);

  //long GetWidth(CPDFBaseFont *i_pFont);

  CPDFChar::eStringType GetStringType();
  bool_a Valid();
  void Invalidate();

private:

  void Clean();

  union
  {
    std::string *m_pStr;
    std::wstring *m_pWstr;
  };

  CPDFChar::eStringType m_eStringType;
};


#endif  /*  __PDFUTIL_H__  */

