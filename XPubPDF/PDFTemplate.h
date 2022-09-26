#ifndef __PDFTEMPLATE_H__
#define __PDFTEMPLATE_H__


#include "pdfdef.h"
#include "PDFManager.h"
#include "PDFBaseFont.h"

#include <map>
#include <vector>
#include <string>


#if defined(WIN32)
#pragma warning(disable : 4251)
#endif // WIN32

#define NUM_SIZE                50
#define ALLOC_STEP              200
#define ACROFORM_POSTFIX        "xpub____"
#define ACROFORM_POSTFIX_DUP    "xpub_d__"


class CPDFBaseFont;
class CPDFImage;
class CPDFDocument;
class CPDFPage;
class CPDFTemplate;
class CPDFObjects;



typedef std::multimap<PDFCTM, CPDFObjects *, PDFCTM> PDFObjectsMap;

typedef void (CPDFTemplate::*OperatorPointer)(std::vector<std::string> *i_pValues);
typedef std::map<std::string, OperatorPointer> OperatorMap;
#define ADD_TEXTOPERATOR( operator_handler,operator_name ) \
  m_mapTextOperators.insert(OperatorMap::value_type(operator_name, &CPDFTemplate::operator_handler ));
#define ADD_NOTTEXTOPERATOR( operator_handler,operator_name ) \
  m_mapNotTextOperators.insert(OperatorMap::value_type(operator_name, &CPDFTemplate::operator_handler ));


// mik -->
class CStoreRestoreObject
{
public:
  CStoreRestoreObject();
  CStoreRestoreObject(const CStoreRestoreObject& cObject);
  ~CStoreRestoreObject();

  void Store(const CPDFTemplate* pTemplate);
  void Restore(CPDFTemplate* pTemplate);

protected:
  float m_fLeading;
  float m_fWordSpacing;
  float m_fCharSpacing;
  float m_fHScaling;
  PDFCTM m_ctm;
};
class CStoreRestoreObjectContainer
{
typedef std::vector<CStoreRestoreObject>  CObjects;
typedef CObjects::iterator                CObjectsIterator;
typedef CObjects::const_iterator          CObjectsConstIterator;
typedef CObjects::reverse_iterator        CObjectsReverseIterator;
public:
  CStoreRestoreObjectContainer(){};
  ~CStoreRestoreObjectContainer(){};

  void Store(const CPDFTemplate* pTemplate);
  void Restore(CPDFTemplate* pTemplate);
protected:
  CObjects m_cObjects;
};
// mik <--

class CPDFDictionary
{
public:
  ~CPDFDictionary();

  static CPDFDictionary *MakeDisctionary(CPDFTemplate *i_pTemplate, long i_lObjNum);

  bool_a HasKey(const char *i_pczKey);

  CPDFDictionary *GetSubDictionary(const char *i_pczKey);
  bool_a GetSubObject(const char *i_pczKey, const char *i_pczBegin, const char *i_pczEnd, std::string& o_strSubDic);
  long GetNumValue(const char *i_pczKey);
  bool_a GetDictionary(std::string& o_strSubDic);
  bool_a SetDictionary(std::string& o_strSubDic);

  //bool_a Add(std::string i_strPath, std::string strValue, std::string i_strStart,
  //            std::string i_strEnd, bool_a i_bBegin);

protected:
  CPDFDictionary(CPDFTemplate *i_pTemplate);
  bool_a ReadDictionary(long i_lObjNum);

private:
  void Clean();

  std::map<std::string, CPDFDictionary *> m_mapSubDictionary;
  std::string m_strDictionary;
  bool_a m_bChanged;
  CPDFTemplate *m_pTemplate;
};

class CPDFXObject
{
public:
  ~CPDFXObject();
  static CPDFXObject *MakeXObject(CPDFTemplate *i_pTemplate, long i_lObjNum);

  std::string &GetStream(){return m_strStream;};
  bool_a isImage(){return m_bIsImage;}


protected:
  CPDFXObject(CPDFTemplate *i_pTemplate);
  bool_a ReadXObject(long i_lObjNum);

private:

  long m_lObjIndex;
  bool_a m_bIsImage;
  std::string m_strStream;
  PDFCTM m_ctmXObject;
  CPDFTemplate *m_pTemplate;
};

//////////////////////////////////////////
//info about pages and objects on pages
struct PDFTempPage
{
  long          lObjIndex;
  size_t        ObjOffset;
  long          m_lRotate;
  std::map<std::string, long> m_mapFont;
  std::vector<std::wstring> m_WStringBlocks;
  std::vector<CPDFObjects *> m_Blocks;
  PDFObjectsMap m_mapObjects;
  std::multimap<std::string, CPDFXObject *> m_pXObjects;
  std::multimap<std::string, PDFCTM> m_mapImages;
};
//////////////////////////////////////////

class XPUBPDF_EXPORTIMPORT CPDFStrBlockControl
{
protected:
  CPDFStrBlockControl(){};
  virtual ~CPDFStrBlockControl(){};

public:
  virtual void SetLanguageID(CPDFObjects *io_pPDFObj) = 0;
  virtual float GetDeltaY(CPDFObjects *io_pPDFObj);
  virtual bool CanJoin(CPDFObjects *io_pPDFObjFirst, CPDFObjects *io_pPDFObjSecond, std::wstring &io_wstrJoin);
  virtual bool CanDivide(CPDFObjects *io_pPDFObjFirst, CPDFObjects *io_pPDFObjSecond, std::wstring &o_wstrJoin);
};

class XPUBPDF_EXPORTIMPORT CPDFObjects
{
public:
  CPDFObjects(CPDFTemplate *i_pTemplate, PDFCTM i_CTM, const char *i_pczValue, const char *i_pczFontName,
              float i_fSize, long i_lPage, float i_fWSpacing, float i_fChSpacing, float i_fHScaling);
  virtual ~CPDFObjects();

  const CPDFBaseFont *GetFont();

  const char *GetFontResName();
  void SetFontResName(const char *i_pczFontName);

  const char *GetFontBaseName();
  void SetFontBaseName(const char *i_pczFontName);

  float GetFontSize();
  void SetFontSize(float i_fSize);

  long GetLanguageID();
  void SetLanguageID(long i_lID);

  const wchar_t *GetWValue();
  const char *GetValue();
  void SetValue(const char *i_pczValue);
  void AppendToValue(const char *i_pczValue);

  float GetWidth(){return m_fWidth;};

  float GetWordSpacing();
  void SetWordSpacing(float i_fSpacing);
  float GetCharSpacing();
  void SetCharSpacing(float i_fSpacing);
  float GetHorizontalSpacing();
  void SetHorizontalrSpacing(float i_fScaling);

  long GetPage();
  void SetPage(long i_lPage);

  CPDFObjects *GetNextObjects();
  CPDFObjects *GetLastObjects();
  void AppendObjects(CPDFObjects *i_pPDFObjects);

  PDFCTM GetCTM();
  void SetCTM(PDFCTM i_CTM);

  float CalculateScale();
  bool_a AppendToBegin();
  bool_a HasSameDirection(CPDFObjects *i_pPDFObj);
  bool_a CompareObjects(CPDFObjects *i_pPDFObj, float &o_fXWidth, float &o_fYWidth, float &o_fXHalfWidth, float &o_fYHalfWidth);

protected:
  void CountWidth();
  bool ConvertToUnicode();

private:
  //next object, which have same CTM but
  //different font or font size or char spacing or word spacing or horizontal scaling
  CPDFObjects *m_pNextObjects;

  //unicode representation of string (only string!!!) in this object.
  std::wstring m_strWValue;

  //contains all that can be set with operators "TJ" and "Tj"
  //see "Text-Showing Operators" in PDFReference(specification)
  std::string m_strValue;

  //symbolic name of font, this name is used in content stream. for example /F8, /Fpdf1 ...
  std::string m_strFontResName;

  //name of font for example /Arial, /Courier, ...
  std::string m_strFontBaseName;

  //size of font
  float m_fFontSize;

  //current transformation matrix
  //see "Text-positioning operators" in PDFReference(specification)
  PDFCTM m_CTM;


  //see "Text State Parameters and Operators" in PDFReference(specification)
  float m_fWordSpacing;

  //see "Text State Parameters and Operators" in PDFReference(specification)
  float m_fCharSpacing;

  //see "Text State Parameters and Operators" in PDFReference(specification)
  float m_fHScaling;

  //width of string in this object - width of string in m_strWValue
  float m_fWidth;

  //number of page, where is this string
  long m_lPage;

  // language ID
  long m_lLanguageID;


  CPDFTemplate *m_pTemplate;
};


class CPDFField
{
public:
  CPDFField();
  virtual ~CPDFField();

  enum eFielType
  {
    eFieldUnknown = -1,
    eFieldText = 0,
    eFieldRadioButton = 1,
    eFieldCheckButton = 2,
    eFieldPushButton = 3
  };

  void CopyAttributes(CPDFField &i_Field);
//  bool_a Overflow(const char *i_pValue){return false_a;};

  eFielType m_eFieldType;
  bool_a m_bChecked;
  bool_a m_bChanged;
  CPDFString m_Value;
  long m_lObjNum;
  long m_lGenNum;
  std::string m_strDA;
  PdfRgb m_ColorBG;
  PdfRgb m_ColorBC;
  std::string m_strAP;
  std::string m_strFontID;

  long m_lFontObjNum;
  //std::string m_strFontName;

  long m_lFontSize;
  long m_lMaxLen;
  std::string m_strID;

  std::vector<char *> m_Childs;

  long m_lQ;
  float m_fBorderWidth;

  long m_lAP_NObjNum;
  long m_lAP_NGenNum;

  long m_lFf;

  char *m_pYes;
  char *m_pNo;

  long m_lPage;

  CPDFTemplate *m_pTemplate;

  CPDFField *m_pParent;

  frec_a m_Rec;
};

class XPUBPDF_EXPORTIMPORT CPDFTemplate
{
  friend class CStoreRestoreObject; // mik
public:
  CPDFTemplate(CPDFDocument *i_pDoc);
  ~CPDFTemplate();

  enum eAddPagesType
  {
    eAppendPages = 0,
    eOverlayPages = 1
  };

  enum eStrBlockType
  {
    eNotBuilded = -1,
    eSimple = 0,
    eJoined = 1
  };

  bool_a Read(const char *i_pPDFFile);


  long GetPageCount(){return (long)m_PDFPages.size();};
  bool_a AddPageToTemplate(CPDFPage *i_pPage, const char *i_pFilter);
  bool_a AddFontToPage(long i_lPage, CPDFBaseFont *i_pFont);
  bool_a AddFontToTemplate(CPDFBaseFont *i_pFont);
  bool_a AssignTemplateObjNum(CPDFBaseFont *i_pFont);
  bool_a AddImageToPage(long i_lPage, CPDFImage *i_pImg);
  bool_a AddImageToTemplate(CPDFImage *i_pImg);
  bool_a AddLayerToPage(long i_lPage, CPDFLayer *i_pLayer);
  bool_a AddLayerToTemplate(CPDFLayer *i_pLayer);
  bool_a AddLayersToCatalog(std::string& i_OCstream);
  bool_a AddFormToTemplate(CPDFAnnotation *i_pAnnot);
  bool_a AddFormToPage(long i_lPage, CPDFAnnotation *i_pAnnot);

  void SetLayer(CPDFLayer *i_pLayer);
  bool_a ApplyLayer();

  bool_a AddPDFTemplate(CPDFTemplate *i_pTemplatePDF, long i_lPageCount);

  bool_a DoDecrypt();
  bool_a DoEncrypt();
  bool_a Encrypt();
  void SetTeplatePassword(const char *i_pczPassw){if (i_pczPassw)m_strPassword = i_pczPassw;};
  const char *GetTeplatePassword(){return m_strPassword.c_str();};
  void SetPDFPosition(long i_lPos){m_lPDFPosition = i_lPos;};
  long GetPDFPosition(){return m_lPDFPosition;};
  bool_a ApplyPDFPosition(long i_lPageCount);
  bool_a SetFormField(const char *i_pName, const char *i_pValue);
  bool_a SetFormFieldFont(const char *i_pName, CPDFBaseFont *i_pFont, float i_fFontSize);

  bool_a MoveFormField(long i_lDeltaX, long i_lDeltaY);
  bool_a MoveFormField(const char *i_pName, long i_lDeltaX, long i_lDeltaY);

  bool_a SetFormFieldBG(const char *i_pName, PdfRgb i_rgb);
  bool_a CheckFormFieldOverflow(const char *i_pName, const char *i_pValue);
  const char *GetFormFieldValue(const char *i_pName);
  CPDFBaseFont *GetFormFieldFont(const char *i_pName);
  long GetFormFieldFontSize(const char *i_pName);

  long GetFormFieldCount();
  const char *GetFormFieldName(long i_lIndex, bool_a i_bSorted = false_a);

  const char *GetFileName(){return m_pFileName;};
  const char *GetPDFStream(long &o_lSize);

  long GetImageCount(long i_lPage);
  const char *GetImageName(long i_lPage, long i_lIndex);
  PDFCTM GetImageCTM(long i_lPage, long i_lIndex);

  long GetStrBlockCount(long i_lPage);
  const wchar_t *GetStrBlock(long i_lPage, size_t i_Block);
  const char *GetStrBlockFontName(long i_lPage, size_t i_Block);
  float GetStrBlockFontSize(long i_lPage, size_t i_Block);
  void GetStrBlockPosition(long i_lPage, size_t i_Block, float &o_fX, float &o_fY);
  bool_a BuildStrBlock(eStrBlockType i_eType, CPDFStrBlockControl *i_pStrBlockCtrl);

  //bool_a UpdateFontRef(const char *i_pFontName, long i_lObj);

protected:
  friend class CPDFObjects;
  friend class CPDFDictionary;
  friend class CPDFXObject;

//////////////////////////////////////////
//Functions for fonts
  CPDFBaseFont *GetFont(long i_lPage, const char *i_pczFontName);
  CPDFBaseFont *GetFont(CPDFTemplate *i_pTemplate, long i_lObj);
//////////////////////////////////////////


  char *ReadObject(size_t i_Pos, long &o_lSize);//don't use this function //TODO: out
  bool_a ReadObject(size_t i_Pos, std::string &i_strObj);

private:

  bool_a CheckObjects();
  bool_a ReadXREFPos();
  bool_a ReadObjOffset(std::string &i_xref, size_t i_Offset);
  char *ReadTrailer(size_t& i_Pos);

  bool_a WriteEncryption(size_t i_Off);
  bool_a WriteInfo(size_t i_Off);
  bool_a WriteNewAndChangedObjToXref();


  bool_a UpdateOffsets(size_t i_Offset, size_t i_AddedSize);

  bool_a WriteLinearizedObj();
  bool_a WriteOffset();
  bool_a UpdateTrailer();
  bool_a UpdatePreview();

  void ReadLinearizedObj(long i_lObj, long i_lOff);
  void Clean();


//////////////////////////////////////////
//common function
  bool_a ReplaceNumValue(std::string &io_strObj, const char *i_pKey, const char *i_pFKey, long i_lValue);
  bool_a ReplaceTextValue(std::string &io_strObj, const char *i_pKey, const char *i_pFKey,
                          const char *i_pValue, long i_lSize = NOT_USED);
  bool_a ReplaceNameValue(std::string &io_strObj, const char *i_pKey, const char *i_pFKey, const char *i_pValue);
  bool_a ReplaceDictValue(std::string &io_strObj, const char *i_pKey, const char *i_pFKey, const char *i_pValue);
  bool_a ReplaceRGBValue(std::string &io_strObj, const char *i_pKey, const char *i_pFKey, PdfRgb i_rgb);
  bool_a ReplaceRectValue(std::string &io_strObj, const char *i_pKey, const char *i_pFKey, frec_a i_rec);
  bool_a ReplaceRefValue(std::string &io_strObj, const char *i_pKey, const char *i_pFKey, long i_lObjNum, long i_lGenNum);
  bool_a ReplaceRefValue(std::string &io_strObj, size_t i_Off, long i_lObjNum, long i_lGenNum);

  void ReplaceNewLine(std::string &str);

  bool_a RemoveDictValue(std::string &io_strObj, const char *i_pKey);

  long GetObjOffset(long i_lObj);
  long ChangeIndObjToArray(std::string& io_strObj, size_t i_index);

  long AddIndObjToArray(std::string& io_strObj, size_t i_index, long i_lObj, bool_a bBegin);
  long AddIndObjToArray(std::string& io_strObj, size_t i_index, const char *i_pObjRef, bool_a bBegin);

  bool_a ReadTextValueParent(long i_lParentObj, const char *i_pKey, std::string& o_strValue);
  bool_a ReadTextValueParent(long i_lParentObj, const char *i_pKey, CPDFString& o_strValue);
  long ReadNumValueParent(long i_lParentObj, const char *i_pKey);

  std::string ReadDict(size_t i_pos);
  //char *ReadDict(const char *i_pObj, const char *i_pKey);//obsolete

  //char *ReadItem(const char *i_pObj, const char *i_pKey, const char *i_pStart, const char *i_pEnd);//obsolete
  //char *ReadItem(const char *i_pObj, size_t i_Size, size_t i_Pos, const char *i_pStart, const char *i_pEnd);//obsolete

  bool ReadItem(std::string i_strObj, std::string i_strKey, std::string i_strStart,
                std::string i_strEnd, bool_a i_bCheck, std::string &i_strValue);
  bool ReadItem(std::string i_strObj, size_t i_Pos, std::string i_strStart,
                std::string i_strEnd, bool_a i_bCheck, std::string &i_strValue);

  bool AddItem(std::string &i_strObj, std::string i_strKey, std::string i_strStart,
                std::string i_strEnd, bool_a i_bCheck, std::string i_strValue);
  bool AddItem(std::string &i_strObj, size_t i_Pos, std::string i_strStart,
                std::string i_strEnd, bool_a i_bCheck, std::string i_strValue);

  bool CopyItem(std::string &i_strDest, std::string i_strSource,
                const char ***i_pKeyTable, long i_lKeysCount);

  void MoveOverWhiteChars(char **pPos);//obsolete
  void MoveOverWhiteChars(std::string strObj, std::basic_string<char>::size_type &index);

  bool_a GetObjAndPosForKey(long i_lObjNum, std::string& i_strPath, long& o_lObjNum, size_t& o_index);

  long GetNewObjOffet();
//////////////////////////////////////////

//////////////////////////////////////////
//Functions for pages
  bool_a ReadPageObject(std::string i_strPageObj);
  bool_a ReadXObject(std::string i_strPageObj, PDFTempPage& io_Page);
  bool_a ReadPageFont(std::string i_strPageObj, PDFTempPage& io_Page);
  bool_a CreateNewPage(CPDFPage *i_pPage);
  bool_a AddNewPageRefToPages(long i_lRef);
  bool_a AddNewContentRefToPage(long i_lPage, const char *i_pContRef,
            bool_a i_bAddSaveRestoreCont = true_a, bool_a i_bBegin = false_a);
  bool_a WriteRestoreContent();
  bool_a WriteSaveContent();
  bool_a WriteStartLayerContent();
  bool_a WriteEndLayerContent();
//////////////////////////////////////////

//////////////////////////////////////////
//Functions for fonts
  bool_a ReadFont(long i_lFontObj);
  bool_a ReadDescendantFontsKey(std::string& i_strObj, CPDFBaseFont& io_Font);
  bool_a ReadToUnicodeFontKey(std::string& i_strObj, CPDFBaseFont& io_Font);
  bool_a ReadEncodingFontKey(std::string& i_strObj, CPDFBaseFont& io_Font);
  bool_a ReadDescriptorFontKey(std::string& i_strObj, CPDFBaseFont& io_Font);
  bool_a CreateDefFont(long i_lIndex);
  bool_a ReadFontObjNumFromDR(std::string& i_strFontID, long& o_lFontObjNum);
//////////////////////////////////////////

//////////////////////////////////////////
//Functions for objects in contents of pages
  bool_a ReadContentStreamObject();
  bool_a ParseContentStream(std::vector <std::string> *i_pContents);
  void ProcessNotInTextToken(std::string& i_strToken, std::vector <std::string> *i_pValues); // mik
  void ProcessInTextToken(std::string& i_strToken, std::vector <std::string> *i_pValues);

  void BuildOperatorsMap();
  void Tm_Operator_Handler(std::vector<std::string> *i_pValues);
  void Td_Operator_Handler(std::vector<std::string> *i_pValues);
  void TD_Operator_Handler(std::vector<std::string> *i_pValues);
  void T_STAR_Operator_Handler(std::vector<std::string> *i_pValues);
  void Tj_Operator_Handler(std::vector<std::string> *i_pValues);
  void TJ_Operator_Handler(std::vector<std::string> *i_pValues);
  void APOSTROPHE_Operator_Handler(std::vector<std::string> *i_pValues);
  void QUOTES_Operator_Handler(std::vector<std::string> *i_pValues);
  void Tc_Operator_Handler(std::vector<std::string> *i_pValues);
  void Tw_Operator_Handler(std::vector<std::string> *i_pValues);
  void Tz_Operator_Handler(std::vector<std::string> *i_pValues);
  void TL_Operator_Handler(std::vector<std::string> *i_pValues);
  void Tf_Operator_Handler(std::vector<std::string> *i_pValues);
  void Tr_Operator_Handler(std::vector<std::string> *i_pValues);
  void Ts_Operator_Handler(std::vector<std::string> *i_pValues);

  void CM_Operator_Handler(std::vector<std::string> *i_pValues);
  void Do_Operator_Handler(std::vector<std::string> *i_pValues);
  void BI_Operator_Handler(std::vector<std::string> *i_pValues);
  void ID_Operator_Handler(std::vector<std::string> *i_pValues);
  void EI_Operator_Handler(std::vector<std::string> *i_pValues);

  void Restore_Operator_Handler(std::vector<std::string> *i_pValues); // mik
  void Store_Operator_Handler(std::vector<std::string> *i_pValues); // mik
//////////////////////////////////////////

//////////////////////////////////////////
//Functions for fields
  bool_a ReadFields();
  bool_a WriteFields();

  bool_a ReadFieldObject(std::string& i_strObj, const char *i_pIDPref, const char *i_pType, long i_lParent);
  bool_a ReadCheckboxState(std::string& i_strAP, char **o_ppYes, char **o_ppNo);
  bool_a SetFieldsPage();
  bool_a SetFieldsPage(long i_lField, long i_lPage);
  bool_a MakeSortedFields();
  char *MakeFieldPosition(CPDFField *i_pField);
  char *CreateAPStream(CPDFField *i_pfield, size_t &o_Size);
  bool_a ReadFontIDFromDA(std::string& i_strDA, std::string& o_strID);
  bool_a AddFontToDR(const char *i_pFont, long i_lObjNum);
//////////////////////////////////////////

//////////////////////////////////////////
//add CPDFTemplate to CPDFTemplate
  bool_a AddResourcesToPage(long i_lPage, std::string strRes);
  bool_a AddToPage(long i_lPage, long i_lOff);
  bool_a AddAnnotsToPage(long i_lPage, std::string strAnnots);
  bool_a ChangeParent(std::string strObjList, std::string strKey, long i_lPage);
  bool_a CheckFieldsID(CPDFTemplate *i_pTemplatePDF);
  bool_a ProcessContentRes(std::string &strRes, std::string strContent);
  bool_a ChangeResNames(std::string &strNames, std::vector <std::string> &contents);
  bool_a FillContentsVector(std::string strContent, std::vector <std::string> &contents);
  bool_a UpdateContentsVector(std::string strContent, std::vector <std::string> &contents);
  bool_a ProcessAcroForm(std::string strRes, long &lLastObj);
//////////////////////////////////////////




//////////////////////////////////////////
//info about pages and objects on pages
  std::map<long, PDFTempPage> m_PDFPages;
//////////////////////////////////////////

//////////////////////////////////////////
//order of xref tab and offsets
  std::map <long, size_t> m_PdfXrefMap;
//////////////////////////////////////////

//////////////////////////////////////////
//all objects used in pdf file
  std::map <long, size_t> m_PdfObjOffMap;
//////////////////////////////////////////

//////////////////////////////////////////
//all forms in pdf file and sorted sorted map of forms
  std::multimap <std::string, CPDFField *> m_PdfFieldObj;
  std::multimap <const char *, const char *, str_comp_greater > m_PdfFieldNames;
//////////////////////////////////////////

//////////////////////////////////////////
// root dict.
  CPDFDictionary *m_pRootDic;
  bool_a GetAcroForm(std::string& o_strAcroForm);
  bool_a GetDR(std::string& o_strDR);
  bool_a GetDA(std::string& o_strDA);
//////////////////////////////////////////


  long m_lInfoObj;
  size_t m_RootOffset;
  long m_lRootObj;
  size_t m_PagesOffset;
  long m_lPagesObj;
  long m_lLastObj;
  long m_lShiftObj;
  size_t m_NewObjOff; //out, not used now
  long m_lLinearObj;
  long m_lLinearL; //length of the file in bytes
  size_t m_LinearE; // offset of the end of the first page
  size_t m_LinearT; //offset of the white-space character preceding
                   // the first entry of the main cross-referrence table
  size_t m_LinearH[4]; // offset of the primary hint stream

  std::string m_strRawPDF;

  char *m_pFileName;
  bool_a m_bFlush;

  long m_lEncryptObj;
  CPDFDictionary *m_pEncryptDic;
  std::string m_strPassword;
  std::string m_strFileID;

  long m_lPDFPosition;

  long m_lSaveContObj;
  long m_lRestContObj;
  long m_lNewResCounter;
  long m_lStartLayerContObj;
  long m_lEndLayerContObj;


  std::map<long, CPDFBaseFont *> m_mapFont;//font used in loaded pdf file


  eStrBlockType m_eStrBlockType;


  CPDFObjects *AddToObjects(std::string i_str);
  void AddImageToObjects(std::string i_strImgName);
  bool_a IsObjectEqual(CPDFObjects *i_pPDFObj);
  void ClearStrBlock();
  bool_a BuildStrBlockSimple();
  bool_a BuildStrBlockJoined();



  CPDFStrBlockControl *m_pStrBlockControl;
  PDFObjectsMap m_mapObjects;
  std::multimap<std::string, PDFCTM> m_mapImages;
  OperatorMap m_mapTextOperators; // mik
  OperatorMap m_mapNotTextOperators; // mik
  std::string m_strFontName;  // mik - store/restore
  long m_lPage;               // mik - store/restore
  long m_lRotate;             // mik - store/restore
  float m_fFontSize;          // mik - store/restore
  PDFCTM m_Tm;                // mik - store/restore
  PDFCTM m_Tlm;               // mik - store/restore
  PDFCTM m_CTM;               // mik - store/restore
  float m_fLeading;           // mik + store/restore
  float m_fWordSpacing;       // mik + store/restore
  float m_fCharSpacing;       // mik + store/restore
  float m_fHScaling;          // mik + store/restore
  bool_a m_bInlineImage;


  CStoreRestoreObjectContainer m_cStoreRestoreContainer;


  CPDFLayer *m_pLayer;
  CPDFDocument *m_pDoc;
};

#if defined(WIN32)
#pragma warning(default : 4251)
#endif // WIN32

#endif  /*  __PDFTEMPLATE_H__  */
