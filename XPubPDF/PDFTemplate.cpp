

#include "pdffnc.h"
#include "PDFTemplate.h"

#include "PDFDocument.h"
#include "PDFBaseFont.h"
#include "PDFTTFFont.h"
#include <string>

#include "zlib.h"


using namespace std;


typedef pair <string, CPDFField *>                                Field_Pair;
typedef multimap <string, CPDFField *>::iterator   Field_iterator;

#define PDF_SPACE_CHR     ' '
#define PDF_TAB_CHR       '\t'
#define PDF_NL_CHR1       '\r'
#define PDF_NL_CHR2       '\n'

static const char czSAVE[] = "\x78\x9c\x2b\x04\x00\x00\x72\x00\x72";
static const char czRESTORE[] = "\x78\x9c\x0b\x04\x00\x00\x52\x00\x52";
static const long lSAVE_RESTORE = 9;
//0x78 0x9c 0x2b 0x04 0x00 0x00 0x72 0x00 0x72  //save
//0x78 0x9c 0x0b 0x04 0x00 0x00 0x52 0x00 0x52  //restore

//static const char czSAVE[] = "\x78\x9c\xe3\x2a\xe4\x02\x00\x01\x0d\x00\x86";
//static const char czRESTORE[] = "\x78\x9c\xe3\x0a\xe4\x02\x00\x00\xcd\x00\x66";
//static const long lSAVE_RESTORE = 11;
//0x78 0x9c 0xe3 0x2a 0xe4 0x02 0x00 0x01 0x0d 0x00 0x86
//0x78 0x9c 0xe3 0x0a 0xe4 0x02 0x00 0x00 0xcd 0x00 0x66




// mik -->
CStoreRestoreObject::CStoreRestoreObject()
{
}
CStoreRestoreObject::CStoreRestoreObject(const CStoreRestoreObject& cObject)
{
  m_fLeading = cObject.m_fLeading;
  m_fWordSpacing = cObject.m_fWordSpacing;
  m_fCharSpacing = cObject.m_fCharSpacing;
  m_fHScaling = cObject.m_fHScaling;
  m_ctm = cObject.m_ctm;
}
CStoreRestoreObject::~CStoreRestoreObject()
{
}
void CStoreRestoreObject::Store(const CPDFTemplate* pTemplate)
{
  if (!pTemplate)
    return;

  m_fLeading = pTemplate->m_fLeading;
  m_fWordSpacing = pTemplate->m_fWordSpacing;
  m_fCharSpacing = pTemplate->m_fCharSpacing;
  m_fHScaling = pTemplate->m_fHScaling;
  m_ctm = pTemplate->m_CTM;
}
void CStoreRestoreObject::Restore(CPDFTemplate* pTemplate)
{
  if (!pTemplate)
    return;

  pTemplate->m_fLeading = m_fLeading;
  pTemplate->m_fWordSpacing = m_fWordSpacing;
  pTemplate->m_fCharSpacing = m_fCharSpacing;
  pTemplate->m_fHScaling = m_fHScaling;
  pTemplate->m_CTM = m_ctm;
}
void CStoreRestoreObjectContainer::Store(const CPDFTemplate* pTemplate)
{
  if (!pTemplate)
    return;

  CStoreRestoreObject cNewObject;
  cNewObject.Store(pTemplate);
  m_cObjects.push_back(cNewObject);
}
void CStoreRestoreObjectContainer::Restore(CPDFTemplate* pTemplate)
{
  if (!pTemplate)
    return;

  if (!m_cObjects.size())
    return;

  CObjectsReverseIterator itr = m_cObjects.rbegin();
  (*itr).Restore(pTemplate);
  CObjectsIterator it = m_cObjects.end();
  it--;
  m_cObjects.erase(it);
}
// mik <--




CPDFTemplate::CPDFTemplate(CPDFDocument *i_pDoc)
{
  m_pDoc = i_pDoc;
  m_RootOffset = string::npos;
  m_pRootDic = NULL;
  m_pEncryptDic = NULL;
  m_strPassword = "";
  m_strFileID = "";
  m_lInfoObj = NOT_USED;
  m_lRootObj = NOT_USED;
  m_PagesOffset = string::npos;
  m_lPagesObj = NOT_USED;
  m_lLastObj = NOT_USED;
  m_lShiftObj = 0;
  m_NewObjOff = string::npos;
  m_strRawPDF = "";
  m_pFileName = NULL;
  m_bFlush = false_a;
  m_lEncryptObj = NOT_USED;
  m_lLinearObj = NOT_USED;
  m_lLinearL = NOT_USED;
  m_LinearE = string::npos;
  m_LinearT = string::npos;
  m_LinearH[0] = string::npos;
  m_LinearH[1] = string::npos;
  m_LinearH[2] = string::npos;
  m_LinearH[3] = string::npos;
  m_lSaveContObj = NOT_USED;
  m_lRestContObj = NOT_USED;
  m_lStartLayerContObj = NOT_USED;
  m_lEndLayerContObj = NOT_USED;
  m_lNewResCounter = 0;
  m_lPDFPosition = 0;
  m_pLayer = NULL;

  m_eStrBlockType = eNotBuilded;
  m_strFontName = "";
  m_fFontSize = 0.f;
  m_Tm = PDFIdentMatrix();
  m_Tlm = PDFIdentMatrix();
  m_bInlineImage = false_a;
  m_fLeading = 0.0;
  m_fWordSpacing = 0.0;
  m_fCharSpacing = 0.0;
  m_fHScaling = 100.0;
  BuildOperatorsMap();
}

CPDFTemplate::~CPDFTemplate()
{
  Clean();
}

void CPDFTemplate::BuildOperatorsMap()
{
  ADD_TEXTOPERATOR(Tm_Operator_Handler, "Tm");
  ADD_TEXTOPERATOR(Td_Operator_Handler, "Td");
  ADD_TEXTOPERATOR(TD_Operator_Handler, "TD");
  ADD_TEXTOPERATOR(T_STAR_Operator_Handler, "T*");
  ADD_TEXTOPERATOR(APOSTROPHE_Operator_Handler, "'");
  ADD_TEXTOPERATOR(QUOTES_Operator_Handler, "\"");
  ADD_TEXTOPERATOR(Tj_Operator_Handler, "Tj");
  ADD_TEXTOPERATOR(TJ_Operator_Handler, "TJ");


  ADD_TEXTOPERATOR(Tc_Operator_Handler, "Tc");
  ADD_TEXTOPERATOR(Tw_Operator_Handler, "Tw");
  ADD_TEXTOPERATOR(Tz_Operator_Handler, "Tz");
  ADD_TEXTOPERATOR(TL_Operator_Handler, "TL");
  ADD_TEXTOPERATOR(Tf_Operator_Handler, "Tf");
  ADD_TEXTOPERATOR(Tr_Operator_Handler, "Tr");
  ADD_TEXTOPERATOR(Ts_Operator_Handler, "Ts");

  ADD_NOTTEXTOPERATOR(CM_Operator_Handler, "cm");
  ADD_NOTTEXTOPERATOR(Do_Operator_Handler, "Do");
  ADD_NOTTEXTOPERATOR(BI_Operator_Handler, "BI");
  ADD_NOTTEXTOPERATOR(ID_Operator_Handler, "ID");
  ADD_NOTTEXTOPERATOR(EI_Operator_Handler, "EI");
  // mik -->
  ADD_NOTTEXTOPERATOR(Restore_Operator_Handler, "Q");  // Q - restore  PDF Reference 1.6 strana 166 tabulka operatorov
  ADD_NOTTEXTOPERATOR(Store_Operator_Handler, "q");    // q - store    PDF Reference 1.6 strana 166 tabulka operatorov
  // mik <--
}

bool_a CPDFTemplate::Read(const char *i_pPDFFile)
{
  if (!i_pPDFFile)
    return false_a;
  Clean();
  long lSize = getFileSize(i_pPDFFile);
  char *pBuf = NULL;
  if (!lSize)
    return false_a;
  m_pFileName = PDFStrDup(i_pPDFFile);
  pBuf = (char *)PDFMalloc(lSize);
  if (!pBuf)
    return false_a;
  FILE *pPdfFile = fopen(i_pPDFFile, "rb");
  if (!pPdfFile)
    return false_a;
  fread(pBuf, 1, lSize, pPdfFile);
  fclose(pPdfFile);
  m_strRawPDF.assign(pBuf, lSize);
  free(pBuf);
  string strTemp;

  if (!ReadXREFPos())
    return false_a;

#ifdef __PDF_CHECK_OBJ_OFFSETS_
  CheckObjects();
#endif

  m_pRootDic = CPDFDictionary::MakeDisctionary(this, m_lRootObj);
  if (!m_pRootDic)
    return false_a;
  if (m_lEncryptObj > 0)
  {
    m_pEncryptDic = CPDFDictionary::MakeDisctionary(this, m_lEncryptObj);
    if (!m_pEncryptDic)
      return false_a;

    if (!DoDecrypt())
      return false_a;

#ifdef __PDF_CHECK_OBJ_OFFSETS_
  CheckObjects();
#endif
  }

  if (!ReadObject(m_PagesOffset, strTemp))
    return false_a;
  if (!ReadPageObject(strTemp))
    return false_a;

  if (!ReadFields())
    return false_a;

  if (!SetFieldsPage())
    return false_a;

  if (!MakeSortedFields())
    return false_a;

  return true_a;
}

bool_a CPDFTemplate::CheckObjects()
{
  long i, lSize = (long)m_PdfObjOffMap.size();
  size_t Off;
  string strObj;

  for (i = 1; i < lSize; i++)
  {
    Off = GetObjOffset(i);

    if (Off != -1)
    {
      if (!ReadObject(Off, strObj))
        return false_a;
      if (strObj[0] < '0' || strObj[0] > '9')
        return false_a;

      //while ((*pPos >= '0' && *pPos <= '9'))
      //  pPos++;
      //while (*pPos == PDF_SPACE_CHR || *pPos == PDF_TAB_CHR)
      //  pPos++;
      //lTemp1 = atol(pPos);
      //if (lTemp1 < 0)
      //  return false_a;
      //while ((*pPos >= '0' && *pPos <= '9'))
      //  pPos++;

      //if (strObj.
    }

  }
  return true_a;
}

bool_a CPDFTemplate::ReadXREFPos()
{
  FILE *pPdfFile = fopen(m_pFileName, "rb");
  bool_a bFind = false_a;
  long lSize, lSaveSize;
  char cTemp;
  char czEnd[sizeof(czEOF) + 1] = "";
  short sEofIndex = (short)PDFStrLenC(czEOF) - 1;
  char czNum[NUM_SIZE + 1] = "";
  char *pTemp1;
  if (NULL == pPdfFile)
    return false_a;
  fseek(pPdfFile, 0, SEEK_END);
  lSize = ftell(pPdfFile);
  lSaveSize = lSize;
  string strTemp;

  while (lSize >= 0)
  {
    fseek(pPdfFile, --lSize, SEEK_SET);
    cTemp = (char)fgetc(pPdfFile);
    if (bFind)
    {
      if (cTemp != PDF_NL_CHR2 && cTemp != PDF_NL_CHR1 && cTemp != PDF_SPACE_CHR && cTemp != PDF_TAB_CHR)
      {
        if (cTemp >= '0' && cTemp <= '9')
        {
          czNum[sEofIndex--] = cTemp;
        }
        else
        {
          if (sEofIndex > 0)
          {
            m_PdfXrefMap[0] = atol(czNum);

            strTemp = PDFReadXrefObj(pPdfFile, m_PdfXrefMap[0], lSaveSize);
            m_lRootObj = PDFGetNumValue(strTemp, czROOT);
            if (m_lRootObj < 0)
              return false_a;

            m_lEncryptObj = PDFGetNumValue(strTemp, "/Encrypt"); //optional
            m_lInfoObj = PDFGetNumValue(strTemp, "/Info"); //optional
            m_lLastObj = PDFGetNumValue(strTemp, czSIZE);
            if (m_lLastObj < 0)
              return false_a;

            pTemp1 = PDFGetTextHEXValue(strTemp, "/ID");
            if (pTemp1)
            {
              if (m_pDoc)
                m_pDoc->SetFileID(pTemp1);
              m_strFileID = pTemp1;
              free(pTemp1);
            }

            if (!ReadObjOffset(strTemp, m_PdfXrefMap[0]))
              return false_a;

            lSize = 1;
            m_PdfXrefMap[lSize] = PDFGetNumValue(strTemp, czPREV);
            while (m_PdfXrefMap[lSize] != -1)
            {
              strTemp = PDFReadXrefObj(pPdfFile, m_PdfXrefMap[lSize], lSaveSize);
              if (!ReadObjOffset(strTemp, m_PdfXrefMap[lSize]))
                return false_a;
              lSize++;
              m_PdfXrefMap[lSize] = PDFGetNumValue(strTemp, czPREV);
            }

            lSize = 0;
            m_RootOffset = string::npos;
            m_RootOffset = GetObjOffset(m_lRootObj);
            if (m_RootOffset == -1)
              return false_a;

            strTemp = ReadDict(m_RootOffset);
            if (strTemp.empty())
              return false_a;

            m_lPagesObj = PDFGetNumValue(strTemp, czPAGES);
            if (m_lPagesObj < 0)
              return false_a;
            lSize = 0;
            m_PagesOffset = string::npos;
            m_PagesOffset = GetObjOffset(m_lPagesObj);
            if (m_PagesOffset == -1)
              return false_a;

            //pTemp = ReadDict(m_PagesOffset);
            //if (!pTemp)
            //  return false_a;
            //strTemp.assign(pTemp, lDicSize);
            //free(pTemp);
            //if (!ReadPageObject(strTemp))
            //  return false_a;

            break;
          }
        }
      }
    }
    else
    {
      if (cTemp != PDF_NL_CHR2 && cTemp != PDF_NL_CHR1)
      {
        if (cTemp == czEOF[sEofIndex])
        {
          czEnd[sEofIndex] = cTemp;
          sEofIndex--;
        }
        else
        {
          sEofIndex = (short)PDFStrLenC(czEOF) - 1;
          memset(czEnd, 0, sizeof(char) * sizeof(czEOF) + 1);
        }
        if (sEofIndex < 0)
        {
          bFind = true_a;
          sEofIndex = NUM_SIZE - 1;
          memset(czNum, PDF_SPACE_CHR, sizeof(char) * NUM_SIZE);
          czNum[NUM_SIZE] = 0;
        }
      }
    }
  }
  fclose(pPdfFile);
  return true_a;
}

bool_a CPDFTemplate::ReadFields()
{
  string strFields, strTemp;
  size_t i;
  long lTemp;

  if (!GetAcroForm(strTemp))
    return false_a;
  if (strTemp.empty())
    return true_a;

  if (ReadItem(strTemp, czFIELDS, "[", "]", true_a, strFields))
  {
    if (!strFields.size())
      return true_a;
    i = 0;
    while (strFields.size() > i)
    {
      strTemp = PDFGetStringUpToWhiteSpace(strFields, i); //object number
      if (strTemp.empty())
        break;
      if (!PDFIsNumber(strTemp))
        return false_a;
      lTemp = atol(strTemp.c_str());
      if (lTemp <= 0)
        return false_a;
      lTemp = GetObjOffset(lTemp);
      if (lTemp <= 0)
        return false_a;
      if (!ReadObject(lTemp, strTemp))
        return false_a;

      if (!ReadFieldObject(strTemp, NULL, NULL, NOT_USED))
        return false_a;

      strTemp = PDFGetStringUpToWhiteSpace(strFields, i); //Gen number
      if (strTemp.empty())
        break;
      if (!PDFIsNumber(strTemp))
        return false_a;

      strTemp = PDFGetStringUpToWhiteSpace(strFields, i); //R
    }
  }

  return true_a;
}

bool_a CPDFTemplate::ReadFieldObject(std::string& i_strObj, const char *i_pIDPref, const char *i_pType, long i_lParent)
{
  char *pTemp = NULL;
  long lTemp;
  string strValue, strKids, strType;
  CPDFField *pField = new CPDFField();
  size_t i;
  if (!pField)
    return false_a;

  pField->m_pTemplate = this;

  i = 0;
  strValue = PDFGetStringUpToWhiteSpace(i_strObj, i); //object number
  lTemp = atol(strValue.c_str());
  if (lTemp <= 0)
   return false_a;
  pField->m_lObjNum = lTemp;

  strValue = PDFGetStringUpToWhiteSpace(i_strObj, i); //gen number
  lTemp = atol(strValue.c_str());
  if (lTemp < 0)
    return false_a;
  pField->m_lGenNum = lTemp;

  PDFGetTextValue(i_strObj, czT, pField->m_strID);

  if (!PDFGetTextValue(i_strObj, "/DA", pField->m_strDA))
  {
    if (!ReadTextValueParent(i_lParent, "/DA", pField->m_strDA))
    {
      if (GetDA(strValue))
        pField->m_strDA = strValue;
    }
  }

  if (!pField->m_strDA.empty())
  {
    if (!ReadFontIDFromDA(pField->m_strDA, pField->m_strFontID))
      return false_a;
    pField->m_lFontSize = PDFGetNumValue(pField->m_strDA, pField->m_strFontID.c_str());

    if (!ReadFontObjNumFromDR(pField->m_strFontID, pField->m_lFontObjNum))
      return false_a;
  }

  ReadItem(i_strObj, czKIDS, "[", "]", true_a, strKids);
  if (strKids.empty())
  {
    if (i_pIDPref)
    {
      if (pField->m_strID.empty())
      {
        long lIndex = 0;
        char czTemp[20];
        Field_iterator iter;
        for (;lIndex < 100000; lIndex++)
        {
          sprintf(czTemp, "%s%ld", ACROFORM_POSTFIX, lIndex);
          pField->m_strID = i_pIDPref;
          pField->m_strID.append(czTemp);
          iter = m_PdfFieldObj.find(pField->m_strID);
          if (iter == m_PdfFieldObj.end())
            break;
        }
        iter = m_PdfFieldObj.find(i_pIDPref);
        if (iter != m_PdfFieldObj.end())
        {
          CPDFField *pTempField = (*iter).second;
          char *pChild = PDFStrDup(pField->m_strID.c_str());
          pField->m_pParent = pTempField;
          pTempField->m_Childs.push_back(pChild);
        }
      }
      else
      {
        pField->m_strID.insert(0, ".");
        pField->m_strID.insert(0, i_pIDPref);
      }
    }
    else
    {
      if (pField->m_strID.empty())
        return true_a;
    }

    pField->m_lFf = PDFGetNumValue(i_strObj, "/Ff");
    if (pField->m_lFf < 0)
      pField->m_lFf = 0;

    pField->m_lMaxLen = PDFGetNumValue(i_strObj, "/MaxLen");
    if (pField->m_lMaxLen < 0)
      pField->m_lMaxLen  = ReadNumValueParent(i_lParent, "/MaxLen");


    if (ReadItem(i_strObj, "/MK", "<<", ">>", true_a, strValue))
    {
      pField->m_ColorBC = PDFGetRGBValue(strValue, "/BC");
      pField->m_ColorBG = PDFGetRGBValue(strValue, "/BG");
    }
    pField->m_Rec = PDFGetRectValue(i_strObj, "/Rect");

    if (ReadItem(i_strObj, "/BS", "<<", ">>", true_a, strValue))
    {
      lTemp = PDFGetNumValue(strValue, "/W");
      if (lTemp >= 0)
        pField->m_fBorderWidth = (float)lTemp;
    }

    lTemp = PDFGetNumValue(i_strObj, "/Q");
    if (lTemp >= 0)
      pField->m_lQ = lTemp;
    else
    {
      lTemp = ReadNumValueParent(i_lParent, "/Q");
      if (lTemp >= 0)
        pField->m_lQ = lTemp;
      else
        pField->m_lQ = 0;//def. value
    }

    ReadItem(i_strObj, "/AP", "<<", ">>", true_a, pField->m_strAP);

    pTemp = PDFGetNameValue(i_strObj, "/FT");
    if (!pTemp)
    {
      if (!i_pType)
        return false_a;
      strValue = i_pType;
    } else
    {
      strValue = pTemp;
      free(pTemp);
    }

    if (0 == strValue.compare("/Tx"))
    {
      pField->m_eFieldType = CPDFField::eFieldText;
      pField->m_lAP_NObjNum = PDFGetObjValue(pField->m_strAP, "/N", pField->m_lAP_NGenNum);
    }
    else if (0 == strValue.compare("/Btn"))
    {
      if (pField->m_lFf & 0x10000)
        pField->m_eFieldType = CPDFField::eFieldPushButton;
      else
      {
        if (pField->m_lFf & 0x8000)
          pField->m_eFieldType = CPDFField::eFieldRadioButton;
        else
          pField->m_eFieldType = CPDFField::eFieldCheckButton;
        if (!ReadCheckboxState(pField->m_strAP, &pField->m_pYes, &pField->m_pNo))
          return false_a;
      }
    }
    else
      pField->m_eFieldType = CPDFField::eFieldUnknown;

    if (!PDFGetTextValue(i_strObj, "/V", pField->m_Value))
    {
      if (!ReadTextValueParent(i_lParent, "/V", pField->m_Value))
        pField->m_Value.SetString("");
    }

    m_PdfFieldObj.insert ( Field_Pair ( pField->m_strID, pField ) );
    return true_a;
  }

  strType = "";
  pTemp = PDFGetNameValue(i_strObj, "/FT");
  if (pTemp)
  {
    strType = pTemp;
    free(pTemp);
  }

  if (0 == strType.compare("/Tx"))
  {
    pField->m_eFieldType = CPDFField::eFieldText;

    m_PdfFieldObj.insert ( Field_Pair ( pField->m_strID, pField ) );
  }
  else if (0 == strType.compare("/Btn") && !pField->m_strID.empty())
  {
    if (pField->m_lFf & 0x10000)
    {
      pField->m_eFieldType = CPDFField::eFieldPushButton;
    }
    else
    {
      if (pField->m_lFf & 0x8000)
        pField->m_eFieldType = CPDFField::eFieldRadioButton;
      else
        pField->m_eFieldType = CPDFField::eFieldCheckButton;
    }

    char *pValue = PDFGetNameValue(i_strObj, "/V");
    if (pValue)
    {
      pField->m_Value.SetString(pValue);
      free(pValue);
    }

    m_PdfFieldObj.insert ( Field_Pair ( pField->m_strID, pField ) );
  }
  else
  {
    m_PdfFieldObj.insert ( Field_Pair ( pField->m_strID, pField ) );
  }

  if (!strKids.size())
    return true_a;
  i = 0;
  while (strKids.size() > i)
  {
    strValue = PDFGetStringUpToWhiteSpace(strKids, i); //object number
    if (strValue.empty())
      break;
    if (!PDFIsNumber(strValue))
      return false_a;
    lTemp = atol(strValue.c_str());
    if (lTemp <= 0)
      return false_a;
    lTemp = GetObjOffset(lTemp);
    if (lTemp <= 0)
      return false_a;

    if (!ReadObject(lTemp, strValue))
      return false_a;
    if (!ReadFieldObject(strValue, pField->m_strID.c_str(), strType.c_str(), pField->m_lObjNum))
      return false_a;

    strValue = PDFGetStringUpToWhiteSpace(strKids, i); //Gen number
    if (strValue.empty())
      return false_a;
    if (!PDFIsNumber(strValue))
      return false_a;
    strValue = PDFGetStringUpToWhiteSpace(strKids, i); //R
  }

  return true_a;
}

bool_a CPDFTemplate::SetFieldsPage(long i_lField, long i_lPage)
{
  Field_iterator iter, iterChild;
  CPDFField *pField/*, *pFieldChild*/;
  //size_t i;

  for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
  {
    pField = (*iter).second;
    if (pField->m_lObjNum == i_lField)
    {
      pField->m_lPage = i_lPage;
    }
  }
  return true_a;
}

bool_a CPDFTemplate::SetFieldsPage()
{
  size_t index, Off;
  long lTemp, i, lSize = (long)m_PDFPages.size();
  string strObj, strAnnots;

  for (i = 0; i < lSize; i++)
  {
    Off = m_PDFPages[i].ObjOffset;
    if (!ReadObject(Off, strObj))
      return false_a;
    if (ReadItem(strObj, czANNOTS, "[", "]", true_a, strAnnots))
    {
      if (!strAnnots.size())
        return true_a;
      index = 0;
      while (strAnnots.size() > index)
      {
        strObj = PDFGetStringUpToWhiteSpace(strAnnots, index); //object number
        if (strObj.empty())
          break;
        if (!PDFIsNumber(strObj))
          return false_a;
        lTemp = atol(strObj.c_str());
        if (lTemp <= 0)
          return false_a;

        if (!SetFieldsPage(lTemp, i + 1))
          return false_a;

        strObj = PDFGetStringUpToWhiteSpace(strAnnots, index); //Gen number
        if (strObj.empty())
          break;
        if (!PDFIsNumber(strObj))
          return false_a;
        strObj = PDFGetStringUpToWhiteSpace(strAnnots, index); //R
      }
    }
  }
  return true_a;
}

char *CPDFTemplate::MakeFieldPosition(CPDFField *i_pField)
{
  char *pID = NULL;
  CPDFField *pField = NULL, *pFieldChild;
  Field_iterator iterChild;

  if (!i_pField)
    return NULL;

  if (i_pField->m_Childs.size())
  {
    for (size_t i = 0; i < i_pField->m_Childs.size(); i++)
    {
      pID = (i_pField->m_Childs)[i];
      iterChild = m_PdfFieldObj.find(pID);
      if (iterChild != m_PdfFieldObj.end())
      {
        pFieldChild = (*iterChild).second;
        if (!pField)
          pField = pFieldChild;
        else if (pField->m_lPage > pFieldChild->m_lPage)
          pField = pFieldChild;
        else if (pField->m_lPage == pFieldChild->m_lPage)
        {
          if (pField->m_Rec.fTop < pFieldChild->m_Rec.fTop)
            pField = pFieldChild;
          else if (pField->m_Rec.fTop == pFieldChild->m_Rec.fTop)
          {
            if (pField->m_Rec.fLeft > pFieldChild->m_Rec.fLeft)
              pField = pFieldChild;
          }
        }
      }
    }
  }
  else
    pField = i_pField;

  pID = (char *)PDFMalloc(61);
  if (!pID)
    return false_a;

  sprintf(pID, "%20ld%20ld%20ld", (long)(m_PDFPages.size() - pField->m_lPage),
                                  (long)(pField->m_Rec.fTop),
                                  (long)(10000.f - pField->m_Rec.fLeft));

  return pID;
}

bool_a CPDFTemplate::MakeSortedFields()
{
  Field_iterator iter;
  CPDFField *pField;
  char *pName, *pID;
  basic_string <char>::size_type index;

  if (0 == m_PdfFieldObj.size())
    return true_a;
  for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
  {
    pField = (*iter).second;
    index = pField->m_strID.find(ACROFORM_POSTFIX);
    if (index != -1)
      continue;

    pID = MakeFieldPosition(pField);
    if (!pID)
      return false_a;

    pName = PDFStrDup(pField->m_strID.c_str());
    if (!pName)
    {
      free(pID);
      return false_a;
    }

    m_PdfFieldNames.insert(pair <const char *, const char *>(pID, pName));
  }
  return true_a;
}

bool_a CPDFTemplate::ReadCheckboxState(string& i_strAP, char **o_ppYes, char **o_ppNo)
{
  if (i_strAP.empty())
    return false_a;
  long lObj, lGen, lSize;
  string strN, strD;

  *o_ppYes = NULL;
  *o_ppNo = NULL;

  if (!ReadItem(i_strAP, "/N", czBGNDIC, czENDDIC, true_a, strN))
    return false_a;
  strN.insert(0, czBGNDIC);
  strN.append(czENDDIC);
  if (!ReadItem(i_strAP, "/D", czBGNDIC, czENDDIC, true_a, strD))
    return false_a;
  strD.insert(0, czBGNDIC);
  strD.append(czENDDIC);

  *o_ppYes = PDFGetNameValue(strN, "<<");
  *o_ppNo = PDFGetNameValue(strN, " R");

  if (!(*o_ppNo))
  {
    *o_ppNo = PDFGetNameValue(strD, " R");
    if (*o_ppYes && *o_ppNo && 0 == strcmp(*o_ppNo, *o_ppYes))
    {
      free(*o_ppNo);
      *o_ppNo = PDFGetNameValue(strD, "<<");
    }
  }
  if (!(*o_ppYes))
    *o_ppYes = PDFGetNameValue(strD, "<<");

  if (*o_ppYes && *o_ppNo)
    return true_a;
  return false_a;
}

bool_a CPDFTemplate::ReadFontIDFromDA(string& i_strDA, std::string& o_strID)
{
  size_t i = 0;

  o_strID = PDFGetStringUpToWhiteSpace(i_strDA, i);
  if (o_strID.empty())
    return false_a;
  if (o_strID[0] != '/')
    return false_a;

  return true_a;
}

bool_a CPDFTemplate::CheckFieldsID(CPDFTemplate *i_pTemplatePDF)
{
  Field_iterator iter, iter1, iter2;
  CPDFField *pField;
  if (0 == m_PdfFieldObj.size())
    return true_a;
  char czTemp[20], *pID, *pTemp;
  long lIndex;
  for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
  {
    pTemp = (char *)iter->first.c_str();
    iter1 = i_pTemplatePDF->m_PdfFieldObj.find(pTemp);
    if (iter1 != i_pTemplatePDF->m_PdfFieldObj.end())
    {
      for (lIndex = 0; lIndex < 100000; lIndex++)
      {
        sprintf(czTemp, "%s%ld", ACROFORM_POSTFIX_DUP, lIndex);
        pID = PDFStrJoin(pTemp, czTemp);
        if (!pID)
          return false_a;
        iter2 = m_PdfFieldObj.find(pID);
        if (iter2 == m_PdfFieldObj.end())
        {
          iter2 = i_pTemplatePDF->m_PdfFieldObj.find(pID);
          if (iter2 == i_pTemplatePDF->m_PdfFieldObj.end())
          {
            pField = (*iter1).second;
            pField->m_bChanged = true_a;
//            iter = m_PdfFieldObj.begin();
            i_pTemplatePDF->m_PdfFieldObj.erase(iter1);
            pField->m_strID = pID;
            free(pID);
            i_pTemplatePDF->m_PdfFieldObj.insert ( Field_Pair ( pField->m_strID, pField ) );
            break;
          } else
            free(pID);
        } else
          free(pID);
      }
    }
    pField = (*iter).second;

    CPDFField *pNewField = new CPDFField();
    pNewField->CopyAttributes(*pField);


    pTemp = PDFStrDup((char *)iter->first.c_str());
    if (!pTemp)
      return false_a;
    if (pNewField->m_lAP_NObjNum > 0)
    {
      pNewField->m_lAP_NGenNum = 0;
      pNewField->m_lAP_NObjNum = i_pTemplatePDF->m_lLastObj + pNewField->m_lAP_NObjNum/* - m_lShiftObj*/ - 1;
    }
    pNewField->m_lObjNum = i_pTemplatePDF->m_lLastObj + pNewField->m_lObjNum/* - m_lShiftObj*/ - 1;
    pNewField->m_lGenNum = 0;
    pNewField->m_strID = pTemp;
    free(pTemp);
    i_pTemplatePDF->m_PdfFieldObj.insert ( Field_Pair ( pNewField->m_strID, pNewField ) );
  }

//  m_PdfFieldObj.clear();

  return true_a;
}









///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
//////////////Functions for fonts
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
bool_a CPDFTemplate::ReadFont(long i_lFontObj)
{
  if (i_lFontObj <= 0)
    return false_a;
  long lOff, lObj;
  size_t i;
  char *pTemp;
  string strObj, strFrom, strFromTo, strTo;
  CPDFBaseFont *pFont = NULL;

  pFont = GetFont(this, i_lFontObj);
  if (pFont)
    return true_a;

  CPDFFontEncoding *pEnc = new CPDFFontEncoding();
  if (!pEnc)
    return false_a;
  pEnc->SetEncoding(CPDFFontEncoding::eStandardEncoding);

  lOff = GetObjOffset(i_lFontObj);
  if (lOff < 0)
    return false_a;

  if (!ReadObject(lOff, strObj))
    return false_a;

  pTemp = PDFGetNameValue(strObj, "/BaseFont");
  if (!pTemp)
    return false_a;

  lObj = CPDFBaseFont::GetBaseFontIndex(pTemp + 1);
  if (lObj < 0)
  {
    pFont = new CPDFBaseFont(pTemp + 1, i_lFontObj);
  } else //def. 14 fonts
  {
    pFont = CPDFBaseFont::MakeFont(pTemp + 1, i_lFontObj, CPDFBaseFont::ePDFDontUse);
  }
  if (!pFont)
  {
    free(pTemp);
    return false_a;
  }
  free(pTemp);

  pFont->SetFontObjNum(i_lFontObj);
  pFont->SetFontEncoding(pEnc);

  pTemp = PDFGetNameValue(strObj, "/Subtype");
  if (!pTemp)
  {
    delete pFont;
    return false_a;
  }

  if (0 == strcmp(pTemp, "/Type0"))
    pFont->SetFontType(CPDFBaseFont::eType0);
  else
    pFont->SetFontType(CPDFBaseFont::eType1); //TO DO: now is importand only Type0, all other fonts set to Type1, will be corrected
  free(pTemp);

//read widths
////////////////////////////////////////////////////////////////////////
  if (lObj < 0)
  {
    lObj = PDFGetNumValue(strObj, "/FirstChar");
    //if (lObj < 0)
    //  ; // TO DO - now nothing
    lOff = PDFGetNumValue(strObj, "/LastChar");
    //if (lOff < 0)
    //  ; // TO DO - now nothing
    if (lObj >= 0 && lOff >= 0 && ReadItem(strObj, "/Widths", "[", "]", true_a, strFrom))
    {
      pFont->SetFontFirstChar(lObj);
      pFont->SetFontLastChar(lOff);
      if (!strFrom.size())
      {
        delete pFont;
        return false_a;
      }
      i = 0;
      while (strFrom.size() > i)
      {
        strTo = PDFGetStringUpToWhiteSpace(strFrom, i);
        if (strTo.empty())
          break;
        if (lObj > lOff)
        {
          delete pFont;
          return false_a;
        }
        if (!PDFIsNumber(strTo))
        {
          delete pFont;
          return false_a;
        }
        pFont->SetCharWidth(lObj, atol(strTo.c_str()));
        lObj++;
      }
    }
    else
    {
      if (CPDFBaseFont::eType0 == pFont->GetFontType())
      {
        if (!ReadDescendantFontsKey(strObj, *pFont))
        {
          delete pFont;
          return false_a;
        }
      }
    }
    //else
    //  return false_a; //TO DO - now nothing
  }
  else
  {
    pFont->ReadStdFont((short)lObj);
  }
//read ToUnicode
////////////////////////////////////////////////////////////////////////
  if (!ReadToUnicodeFontKey(strObj, *pFont))
  {
    delete pFont;
    return false_a;
  }
//read Encoding
////////////////////////////////////////////////////////////////////////
  if (!ReadEncodingFontKey(strObj, *pFont))
  {
    delete pFont;
    return false_a;
  }
//read Descriptor
////////////////////////////////////////////////////////////////////////
  if (!ReadDescriptorFontKey(strObj, *pFont))
  {
    delete pFont;
    return false_a;
  }
////////////////////////////////////////////////////////////////////////

  m_mapFont[i_lFontObj] = pFont;

  return true_a;
}

bool_a CPDFTemplate::ReadDescendantFontsKey(string& i_strObj, CPDFBaseFont& io_Font)
{
  string strTemp, strW;
  long lOff;

  if (ReadItem(i_strObj, "/DescendantFonts", "[", "]", true_a, strW))
  {
    lOff = GetObjOffset(atol(strW.c_str()));
    if (lOff <= 0)
      return false_a;
    if (!ReadObject(lOff, strTemp))
      return false_a;

//read Descriptor
////////////////////////////////////////////////////////////////////////
    if (!ReadDescriptorFontKey(strTemp, io_Font))
      return false_a;
////////////////////////////////////////////////////////////////////////


    lOff = PDFGetNumValue(strTemp, "/DW");
    if (lOff < 0)
      lOff = 1000; //Def. value see PDFReference
    io_Font.SetDefComWidth(lOff);
    if (ReadItem(strTemp, "/W", "[", "]", false_a, strW))
    {
      long lFirst, lLast, lWidth;
      if (!strW.size())
        return false_a;
      size_t i = 0;
      while (strW.size() > i)
      {
        strTemp = PDFGetStringUpToWhiteSpace(strW, i);
        if (strTemp.empty())
          break;
        if (PDFIsNumber(strTemp))
        {
          lFirst = atol(strTemp.c_str());
        }
        else
          return false_a;

        strTemp = PDFGetStringUpToWhiteSpace(strW, i);
        if (strTemp.empty())
          return false_a;
        if (PDFIsNumber(strTemp))
        {
          lLast = atol(strTemp.c_str());

          strTemp = PDFGetStringUpToWhiteSpace(strW, i);
          if (strTemp.empty())
            return false_a;
          if (PDFIsNumber(strTemp))
          {
            lWidth = atol(strTemp.c_str());

            for (;lFirst <= lLast;lFirst++)
            {
              io_Font.SetComWidth(lFirst, lWidth);
            }
          }
          else
            return false_a;

        }
        else if ('[' == strTemp[0])
        {
            strTemp.erase(0, 1);
            strTemp.erase(strTemp.size() - 1, 1);

            size_t j = 0;
            string strWidth;
            while (strTemp.size() > j)
            {
              strWidth = PDFGetStringUpToWhiteSpace(strTemp, j);
              if (strWidth.empty())
                break;
              if (PDFIsNumber(strWidth))
              {
                lWidth = atol(strWidth.c_str());
                io_Font.SetComWidth(lFirst, lWidth);
                lFirst++;
              }
              else
                return false_a;
            }
        }
        else
          return false_a;

      }
    }
    return true_a;
  }

  return false_a;
}

bool_a CPDFTemplate::ReadToUnicodeFontKey(string& i_strObj, CPDFBaseFont& io_Font)
{
  string strObj, strFrom, strFromTo, strTo;
  long lObj, lGen;
  size_t lIndex;
  basic_string <char>::size_type index;
  basic_string <char>::iterator strIter;

  lObj = PDFGetObjValue(i_strObj, "/ToUnicode", lGen);
  if (lObj < 0 || lGen < 0)
    return true_a;

  lObj = GetObjOffset(lObj);
  if (lObj < 0)
    return false_a;
  if (!ReadObject(lObj, strObj))
    return false_a;

  lObj = PDFGetObjValue(strObj, "/Length", lGen);
  if (lObj < 0 || lGen < 0)
    lObj = PDFGetNumValue(strObj, "/Length");
  else
  {
    lObj = GetObjOffset(lObj);
    if (lObj <= 0)
      return false_a;
    if (!ReadObject(lObj, strFrom))
      return false_a;
    lObj = PDFGetNumValue(strFrom, "obj");
  }

  index = strObj.find("stream");
  if (index == -1)
    return false_a;
  strObj.erase(strObj.begin(), strObj.begin() + index + 6);

  strIter = strObj.begin();
  if (*strIter == PDF_NL_CHR2)
    strObj.erase(strIter);
  else if (*strIter == PDF_NL_CHR1 && *(strIter + 1) == PDF_NL_CHR2)
    strObj.erase(strIter, strIter + 2);
  else if (*strIter == PDF_NL_CHR1)
    strObj.erase(strIter);

  strObj.erase(strObj.begin() + lObj, strObj.end());

  if (!CPDFDocument::UncompressStream(strObj))
    return false_a;

  index = 0;
  while (true_a)
  {
    index = strObj.find("beginbfchar", index);
    if (index == -1)
      break;
    //index = lObj = index + 11;
    index = lIndex = index + 11;
    strFrom = PDFGetStringUpToWhiteSpace(strObj, lIndex);
    while (strFrom.compare("endbfchar"))
    {
      if (strFrom[0] != '<' || strFrom[strFrom.size() - 1] != '>')
        return false_a;
      strFrom.erase(0, 1);
      strFrom.erase(strFrom.size() - 1, 1);

      strTo = PDFGetStringUpToWhiteSpace(strObj, lIndex);
      if (strTo[0] != '<' || strTo[strTo.size() - 1] != '>')
        return false_a;
      strTo.erase(0, 1);
      strTo.erase(strTo.size() - 1, 1);

      PDFMakeUpper(strFrom);
      PDFMakeUpper(strTo);

      if (strFrom.compare(strTo) != 0)//TO DO: also compare strings with different length - "20" is same as "0020"
      {
        io_Font.SetToUnicode(strFrom, strTo);
      }

      strFrom = PDFGetStringUpToWhiteSpace(strObj, lIndex);
    }
  }

  index = 0;
  while (true_a)
  {
    index = strObj.find("beginbfrange", index);
    if (index == -1)
      break;
    //index = lObj = index + 12;
    index = lIndex = index + 12;
    strFrom = PDFGetStringUpToWhiteSpace(strObj, lIndex);
    while (strFrom.compare("endbfrange"))
    {
      if (strFrom[0] != '<' || strFrom[strFrom.size() - 1] != '>')
        return false_a;
      strFrom.erase(0, 1);
      strFrom.erase(strFrom.size() - 1, 1);


      strFromTo = PDFGetStringUpToWhiteSpace(strObj, lIndex);
      if (strFromTo[0] != '<' || strFromTo[strFromTo.size() - 1] != '>')
        return false_a;
      strFromTo.erase(0, 1);
      strFromTo.erase(strFromTo.size() - 1, 1);


      strTo = PDFGetStringUpToWhiteSpace(strObj, lIndex);
      if (strTo[0] != '<' || strTo[strTo.size() - 1] != '>')
        return false_a;
      strTo.erase(0, 1);
      strTo.erase(strTo.size() - 1, 1);


      PDFMakeUpper(strFrom);
      PDFMakeUpper(strFromTo);
      PDFMakeUpper(strTo);

      if (strFrom.compare(strTo) != 0)//TO DO: also compare strings with different length - "20" is same as "0020"
      {
        while (strFromTo.compare(strFrom))
        {
          io_Font.SetToUnicode(strFrom, strTo);

          PDFIncHEX(strFrom);
          PDFIncHEX(strTo);
        }
        io_Font.SetToUnicode(strFrom, strTo);
      }

      strFrom = PDFGetStringUpToWhiteSpace(strObj, lIndex);
    }
  }

  return true_a;
}

bool_a CPDFTemplate::ReadEncodingFontKey(string& i_strObj, CPDFBaseFont& io_Font)
{
  long lObj, lGen;
  char *pTemp;
  string strObj, strDiff, strTemp;
  size_t i;
  unsigned short usCharCode;

  if (!io_Font.GetFontEncoding())
    return false_a;

  pTemp = PDFGetNameValue(i_strObj, "/Encoding");
  if (pTemp)
  {
    strTemp = pTemp + 1; // / - out
    free(pTemp);

    lGen = 0;
    while (pczPDF_FONT_ENCODINGS[lGen])
    {
      if (!strTemp.compare(pczPDF_FONT_ENCODINGS[lGen]))
      {
        io_Font.GetFontEncoding()->SetEncoding((CPDFFontEncoding::eFontEncodingType)lGen);
        break;
      }
      lGen++;
    }
    for (lGen = 0; lGen < 256; lGen++)
    {
      io_Font.GetFontEncoding()->SetCharCodeToGlyphName(lGen,
                                    PDFGetGlyphName(lGen, io_Font.GetFontEncoding()->GetEncoding()));
    }
    return true_a;
  }

  lObj = PDFGetObjValue(i_strObj, "/Encoding", lGen);
  if (lObj < 0 || lGen < 0)
  {
    return true_a;
  }

  lObj = GetObjOffset(lObj);
  if (lObj < 0)
    return false_a;
  if (!ReadObject(lObj, strObj))
    return false_a;

  pTemp = PDFGetNameValue(strObj, "/BaseEncoding");
  if (pTemp)
  {
    strTemp = pTemp + 1; // / - out
    free(pTemp);

    lGen = 0;
    while (pczPDF_FONT_ENCODINGS[lGen])
    {
      if (!strTemp.compare(pczPDF_FONT_ENCODINGS[lGen]))
      {
        io_Font.GetFontEncoding()->SetEncoding((CPDFFontEncoding::eFontEncodingType)lGen);
        break;
      }
      lGen++;
    }
  }
  for (lGen = 0; lGen < 256; lGen++)
  {
    io_Font.GetFontEncoding()->SetCharCodeToGlyphName(lGen,
                                  PDFGetGlyphName(lGen, io_Font.GetFontEncoding()->GetEncoding()));
  }

  if (ReadItem(strObj, "/Differences", "[", "]", false, strDiff))
  {
    if (!strDiff.size())
      return false_a;
    i = 0;
    while (strDiff.size() > i)
    {
      strTemp = PDFGetStringUpToWhiteSpace(strDiff, i);
      if (strTemp.empty())
        break;
      if (PDFIsNumber(strTemp))
      {
        usCharCode = (unsigned short)atoi(strTemp.c_str());
      }
      else if ('/' == strTemp[0])
      {
        strTemp.erase(0, 1);

        io_Font.GetFontEncoding()->SetCharCodeToGlyphName(usCharCode, strTemp);
        usCharCode++;
      }
      else
        return false_a;
    }
  }

  return true_a;
}

bool_a CPDFTemplate::ReadDescriptorFontKey(string& i_strObj, CPDFBaseFont& io_Font)
{
  long lObj, lGen;
  string strObj;
  frec_a frec;
  rec_a rec;

  lObj = PDFGetObjValue(i_strObj, "/FontDescriptor", lGen);
  if (lObj < 0 || lGen < 0)
    return true_a;

  lGen = GetObjOffset(lObj);
  if (lGen <= 0)
    return false_a;
  if (!ReadObject(lGen, strObj))
    return false_a;

  CPDFFontDescriptor *pDesc = new CPDFFontDescriptor(m_pDoc);
  if (!pDesc)
    return false_a;
  frec = PDFGetRectValue(strObj, "/FontBBox");
  rec.lBottom = (long)frec.fBottom; rec.lLeft = (long)frec.fLeft;
  rec.lRight = (long)frec.fRight; rec.lTop = (long)frec.fTop;
  pDesc->SetFontBBox(rec);

  pDesc->SetAscent((short)PDFGetNumValue(strObj, "/Ascent"));
  pDesc->SetCapHeight((short)PDFGetNumValue(strObj, "/CapHeight"));
  pDesc->SetDescent((short)PDFGetNumValue(strObj, "/Descent"));
  pDesc->SetFlags(PDFGetNumValue(strObj, "/Flags"));
  pDesc->SetItalicAngle((short)PDFGetNumValue(strObj, "/ItalicAngle"));
  pDesc->SetStemH((short)PDFGetNumValue(strObj, "/StemH"));
  pDesc->SetStemV((short)PDFGetNumValue(strObj, "/StemV"));
  pDesc->SetXHeight((short)PDFGetNumValue(strObj, "/XHeight"));


  io_Font.SetFontDescriptor(pDesc);

  return true_a;

}

//in some cases font(def. 14) used in forms need not be present
bool_a CPDFTemplate::CreateDefFont(long i_lIndex)
{
  if (i_lIndex >= sSTANDART_FONT_COUNT)
    return false_a;

  map<long, CPDFBaseFont *>::iterator iterFont;
  iterFont = m_mapFont.find(-i_lIndex);
  if (iterFont != m_mapFont.end()) //already done
    return true_a;

  CPDFBaseFont *pNewFont = CPDFBaseFont::MakeFont(pczSTANDART_FONT_NAME[i_lIndex][0], -i_lIndex,
                                          CPDFBaseFont::ePDFDontUse, NOT_USED, m_pDoc); //ToDo: code page???
  if (!pNewFont)
    return false_a;
  pNewFont->SetFontObjNum(-i_lIndex);

  m_mapFont[-i_lIndex] = pNewFont;
  return true_a;
}

//read font from DR dict. and return its obj. number
bool_a CPDFTemplate::ReadFontObjNumFromDR(string& i_strFontID, long& o_lFontObjNum)
{
  string strTemp, strFont;
  long lObj, lGen;

  if (!GetDR(strTemp))
    return false_a;
  if (!ReadItem(strTemp, czFONT, "<<", ">>", true_a, strFont))
    return false_a;

  lObj = PDFGetObjValue(strFont, i_strFontID.c_str(), lGen);
  if (lObj < 0 || lGen < 0)
  {
    for (short i = 0; i < sSTANDART_FONT_COUNT; i++)
    {
      if (0 != i_strFontID.compare(1, i_strFontID.size(), pczDEF_AFM_FONTS_SHORTCUT[i]))
      {
        map<long, CPDFBaseFont *>::iterator iterFont;
        iterFont = m_mapFont.find(-i);// special use case - def. 14 pdf fonts are wit neg. key
        if (iterFont != m_mapFont.end()) //already done
          return true_a;
        if (!CreateDefFont(i))
          return false_a;
        o_lFontObjNum = -i;
        return true_a;
      }
    }
    return false_a;
  }

  if (!ReadFont(lObj))
    return false_a;
  o_lFontObjNum = lObj;

  return true_a;
}

CPDFBaseFont *CPDFTemplate::GetFont(CPDFTemplate *i_pTemplate, long i_lObj)
{
  CPDFBaseFont *pFont = NULL;
  map<long, CPDFBaseFont *>::iterator iterFont;
  iterFont = i_pTemplate->m_mapFont.find(i_lObj);
  if (iterFont != i_pTemplate->m_mapFont.end())
    pFont = (CPDFBaseFont *)iterFont->second;

  if (!pFont && i_lObj < 0 && m_pDoc) //special use case, used by form because of font from CPDFDocument
  {
    i_lObj = -i_lObj;
    i_lObj -= 20;
    pFont = i_pTemplate->m_pDoc->GetFont(i_lObj);
  }

  return pFont;
}

//returns font used on page according to font name
CPDFBaseFont *CPDFTemplate::GetFont(long i_lPage, const char *i_pczFontName)
{
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return NULL;

  map<string, long>::iterator iterPage;
  iterPage = (m_PDFPages[i_lPage].m_mapFont).find(i_pczFontName);
  if (iterPage == (m_PDFPages[i_lPage].m_mapFont).end())
    return NULL;
  long lFontObj = iterPage->second;

  map<long, CPDFBaseFont *>::iterator iterFont;
  iterFont = m_mapFont.find(lFontObj);
  if (iterFont == m_mapFont.end())
    return NULL;
  return (CPDFBaseFont *)iterFont->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
//////////////Functions for pages
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
bool_a CPDFTemplate::ReadPageObject(string i_strPageObj)
{
  size_t pos;
  long lTemp, lSize;
  string strTemp, strKids;
  size_t i;
  if (i_strPageObj.empty())
    return false_a;
  pos = PDFStrStrArrayKey(i_strPageObj, czKIDS);
  if (pos == string::npos)
  {
    PDFTempPage page;
    lTemp = atol(i_strPageObj.c_str());
    if (lTemp <= 0)
      return false_a;

    if (!ReadPageFont(i_strPageObj, page))
      return false_a;

    if (!ReadXObject(i_strPageObj, page))
      return false_a;

    page.lObjIndex = lTemp;
    lTemp = GetObjOffset(lTemp);
    if (lTemp <= 0)
      return false_a;
    page.ObjOffset = lTemp;
    m_PDFPages[(long)m_PDFPages.size()] = page;
    return true_a;
  }

  if (ReadItem(i_strPageObj, czKIDS, "[", "]", false_a, strKids))
  {
    if (!strKids.size())
      return false_a;
    i = 0;
    while (strKids.size() > i)
    {
      strTemp = PDFGetStringUpToWhiteSpace(strKids, i); //Object number
      if (strTemp.empty())
        break;
      if (!PDFIsNumber(strTemp))
        return false_a;

      lTemp = atol(strTemp.c_str());
      if (lTemp <= 0)
        return false_a;
      lTemp = GetObjOffset(lTemp);
      if (lTemp <= 0)
        return false_a;
      strTemp = ReadDict(lTemp);
      if (!ReadPageObject(strTemp))
        return false_a;

      strTemp = PDFGetStringUpToWhiteSpace(strKids, i); //Gen number
      if (strTemp.empty())
        break;
      if (!PDFIsNumber(strTemp))
        return false_a;

      strTemp = PDFGetStringUpToWhiteSpace(strKids, i); //R
    }
  }

  return true_a;
}

bool_a CPDFTemplate::ReadXObject(string i_strPageObj, PDFTempPage& io_Page)
{
  string strTemp, strXObject, strName;
  size_t lIndex;
  long lXObjectObj;

  if (!ReadItem(i_strPageObj, czRESOURCES, "<<", ">>", true_a, strTemp))
    return true_a;
  if (strTemp.empty())
    return true_a;

  if (ReadItem(strTemp, "/XObject", "<<", ">>", true_a, strXObject))
  {
    if (strXObject.empty())
      return true_a;
    lIndex = 0;
    while (strXObject.size() > lIndex)
    {
      strName = PDFGetStringUpToWhiteSpace(strXObject, lIndex);
      if (strName.empty())
        break;
      if (strName[0] != czDELIMITER_CHAR9)
        return false_a;

      strTemp = PDFGetStringUpToWhiteSpace(strXObject, lIndex);
      if (!PDFIsNumber(strTemp))
        return false_a;
      lXObjectObj = atol(strTemp.c_str());

      strTemp = PDFGetStringUpToWhiteSpace(strXObject, lIndex);
      if (!PDFIsNumber(strTemp))
        return false_a;

      strTemp = PDFGetStringUpToWhiteSpace(strXObject, lIndex);
      if (strTemp[0] != 'R')
        return false_a;

      CPDFXObject *pXObject = CPDFXObject::MakeXObject(this, lXObjectObj);
      io_Page.m_pXObjects.insert(map<string, CPDFXObject *>::value_type(strName, pXObject));


      long lOff = GetObjOffset(lXObjectObj);
      if (lOff <= 0)
        return false_a;
      string strTemp1;
      if (!ReadObject(lOff, strTemp1))
        return false_a;

      if (!ReadPageFont(strTemp1, io_Page))
        return false_a;
    }
    
  }

  return true_a;
}

bool_a CPDFTemplate::ReadPageFont(string i_strPageObj, PDFTempPage& io_Page)
{
  string strFont, strTemp, strName;
  size_t lIndex;
  long lFontObj;

  if (!ReadItem(i_strPageObj, czRESOURCES, "<<", ">>", true_a, strTemp))
    return true_a;
  if (strTemp.empty())
    return true_a;

  if (ReadItem(strTemp, czFONT, "<<", ">>", true_a, strFont))
  {
    if (strFont.empty())
      return true_a;
    lIndex = 0;

    while (strFont.size() > lIndex)
    {
      strName = PDFGetStringUpToWhiteSpace(strFont, lIndex);
      if (strName.empty())
        break;
      if (strName[0] != czDELIMITER_CHAR9)
        return false_a;


      strTemp = PDFGetStringUpToWhiteSpace(strFont, lIndex);
      if (!PDFIsNumber(strTemp))
        return false_a;
      lFontObj = atol(strTemp.c_str());

      strTemp = PDFGetStringUpToWhiteSpace(strFont, lIndex);
      if (!PDFIsNumber(strTemp))
        return false_a;

      strTemp = PDFGetStringUpToWhiteSpace(strFont, lIndex);
      if (strTemp[0] != 'R')
        return false_a;

      io_Page.m_mapFont[strName] = lFontObj;

      if (!ReadFont(lFontObj))
        return false_a;
    }
  }

  return true_a;
}

bool_a CPDFTemplate::CreateNewPage(CPDFPage *i_pPage)
{
  long lAddedSize = 0, lOff;
  char czTemp[100] = {0};
  char czPage[500] = {0};
  PDFTempPage page;

  if (!AddNewPageRefToPages(m_lLastObj))
    return false_a;

  lOff = GetNewObjOffet();

  sprintf(czPage, "%ld 0 obj\n", m_lLastObj);
  strcat(czPage, "<<\n");
  strcat(czPage, "/Type /Page\n");

  sprintf(czTemp, "/Parent %ld 0 R\n", m_lPagesObj);
  strcat(czPage, czTemp);

  strcat(czPage, "/Resources <<\n");

  strcat(czPage, "/ProcSet [ /PDF /Text /ImageC ] >>\n");

  rec_a rec = i_pPage->GetMediaBox();

  sprintf(czTemp, "%s [%ld %ld %ld %ld]\n", czMEDIABOX, PDFConvertDimensionToDefaultL(rec.lLeft),
            PDFConvertDimensionToDefaultL(rec.lBottom), PDFConvertDimensionToDefaultL(rec.lRight),
            PDFConvertDimensionToDefaultL(rec.lTop));
  strcat(czPage, czTemp);

  rec = i_pPage->GetCropBox();

  sprintf(czTemp, "%s [%ld %ld %ld %ld]\n", czCROPBOX, PDFConvertDimensionToDefaultL(rec.lLeft),
            PDFConvertDimensionToDefaultL(rec.lBottom), PDFConvertDimensionToDefaultL(rec.lRight),
            PDFConvertDimensionToDefaultL(rec.lTop));
  strcat(czPage, czTemp);

  sprintf(czTemp, "/Rotate %ld\n", i_pPage->GetRotation());
  strcat(czPage, czTemp);

  strcat(czPage, ">>\n");
  strcat(czPage, "endobj\n");

  lAddedSize = PDFStrLenC(czPage);
  if (!UpdateOffsets(lOff, lAddedSize))
    return false_a;

  m_strRawPDF.insert(lOff, czPage, lAddedSize);

  m_PdfObjOffMap[m_lLastObj] = lOff;

  page.lObjIndex = m_lLastObj;
  page.ObjOffset = lOff;
  m_PDFPages[(long)m_PDFPages.size()] = page;
  m_lLastObj++;
  return true_a;
}

bool_a CPDFTemplate::AddNewPageRefToPages(long i_lRef)
{
  long lTemp;
  size_t AddedSize = 0;
  //char *pTemp = ReadDict(m_PagesOffset, lSize);
  char czTemp[100] = {0};

  size_t index, index1;
  string strKids, strTemp;
  //if (!pTemp)
  //  return false_a;
  if (!ReadObject(m_PagesOffset,strKids))
    return false_a;
  //strKids.assign(pTemp, lSize);
  //free(pTemp);

  index = CPDFDocument::PDFStrStrArrayKey(strKids, czKIDS);
  if (index != -1)
  {
    AddedSize += AddIndObjToArray(m_strRawPDF, m_PagesOffset + index, i_lRef, false_a);
    if (!UpdateOffsets(m_PagesOffset + index, AddedSize))
      return false_a;
  } else //indirect object array ??? check it
  {
    long lObj, lGen;
    lObj = PDFGetObjValue(strKids, czKIDS, lGen);
    if (lObj < 0 || lGen < 0)
      return false_a;
    index = GetObjOffset(lObj);
    if (lGen < 0)
      return false_a;
    index = m_strRawPDF.find("[", index);
    if (index != -1)
      return false_a;
    AddedSize += AddIndObjToArray(m_strRawPDF, index, i_lRef, false_a);
    if (!UpdateOffsets(index, AddedSize))
      return false_a;
  }
  //also update count of pages
  if (!ReadObject(m_PagesOffset,strKids))
    return false_a;
  index = CPDFDocument::PDFStrStrNumKey(strKids, czCOUNT);
  if (index != -1)
  {
    index = index1 = m_PagesOffset + index;
    strTemp = PDFGetStringUpToWhiteSpace(m_strRawPDF, index1);
    if (!PDFIsNumber(strTemp))
      return false_a;
    lTemp = atol(strTemp.c_str());
    MoveOverWhiteChars(m_strRawPDF, index);
    m_strRawPDF.erase(index, strTemp.size());
    sprintf(czTemp, "%ld", lTemp + 1);
    m_strRawPDF.insert(index, czTemp);
    AddedSize = PDFStrLenC(czTemp) - strTemp.size();
    if (!UpdateOffsets(index, AddedSize))
      return false_a;
  }
  else
    return false_a;

  return true_a;
}

bool_a CPDFTemplate::AddNewContentRefToPage(long i_lPage, const char *i_pContRef,
                                            bool_a i_bAddSaveRestoreCont , bool_a i_bBegin)
{
  if (i_lPage <= 0 || i_lPage > (long)m_PDFPages.size() || !i_pContRef)
    return false_a;

  long lAddedSize = 0;
  size_t Off;
  Off = m_PDFPages[i_lPage - 1].ObjOffset;

  if (!WriteSaveContent())
    return false_a;
  if (!WriteRestoreContent())
    return false_a;

  size_t index, indexTmp;
  string strTemp, strNum;
  if (!ReadObject(Off, strTemp))
    return false_a;

  indexTmp = CPDFDocument::PDFStrStrKey(strTemp, czCONTENTS);
  if (indexTmp == -1)
  {
    char czTemp[100];
    indexTmp = strTemp.rfind(czENDDIC);
    if (indexTmp == -1)
      return false_a;
    sprintf(czTemp, " %s %s ", czCONTENTS, i_pContRef);
    lAddedSize = PDFStrLenC(czTemp);
    m_strRawPDF.insert(Off + indexTmp, czTemp, lAddedSize);
    indexTmp += lAddedSize;
    if (!UpdateOffsets(Off + indexTmp, lAddedSize))
      return false_a;
  }
  else
  {
    indexTmp += PDFStrLenC(czCONTENTS);
    index = indexTmp;

    strNum = PDFGetStringUpToWhiteSpace(strTemp, indexTmp);// Obj num
    if (strNum.empty())
      return false_a;
    if (PDFIsNumber(strNum))
    {
      string strObj;
      basic_string <char>::size_type indexF;
      Off = GetObjOffset(atol(strNum.c_str()));
      if (!ReadObject(Off, strObj))
        return false_a;

      indexF = strObj.find("/Length");
      if (indexF == -1)// Is it indir. array or content stram?
      {
        //array
        indexF = strObj.find("[");
        index = indexF;
        strTemp = strObj;
      }
      else
      {
        Off = m_PDFPages[i_lPage - 1].ObjOffset;
        lAddedSize += ChangeIndObjToArray(m_strRawPDF, Off + index);
      }

    } else if ('[' != strNum[0])
      return false_a;

    if (i_bAddSaveRestoreCont)
    {
      lAddedSize += AddIndObjToArray(m_strRawPDF, Off + index, m_lSaveContObj, true_a);
      lAddedSize += AddIndObjToArray(m_strRawPDF, Off + index, m_lRestContObj, false_a);
    }
    if (i_bBegin)
      lAddedSize += AddIndObjToArray(m_strRawPDF, Off + index, i_pContRef, true_a);
    else
      lAddedSize += AddIndObjToArray(m_strRawPDF, Off + index, i_pContRef, false_a);

    if (!UpdateOffsets(Off + index, lAddedSize))
      return false_a;
  }

  return true_a;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
//////////////Functions for
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////













bool_a CPDFTemplate::WriteFields()
{
  Field_iterator iter;
  map <long, size_t>::iterator iter1;
  CPDFField *pField;
  char *pObj, czNew[200];
  string strObj;
  long lTemp;
  size_t Off;
  basic_string <char>::size_type index;
  //if (0 == m_PdfFieldObj.size())
  //  return true_a;

  //for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
  //{
  //  pField = (*iter).second;
  //  if (!pField)
  //    continue;
  //  const char *pFieldId = pField->m_strID.c_str();
  //  lTemp = GetObjOffset(pField->m_lObjNum);

  //  if (!ReadObject(lTemp, strObj))
  //    return false_a;
  //}


  for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
  {
    pField = (*iter).second;
    //pPos = (char *)iter->first;
    //if (this != pField->m_pTemplate)
    //  continue;
    if (!pField->m_bChanged)
      continue;
    lTemp = GetObjOffset(pField->m_lObjNum);

    if (!ReadObject(lTemp, strObj))
      return false_a;

    if (pField->m_ColorBG.fR >= 0 && pField->m_ColorBG.fG >= 0 && pField->m_ColorBG.fB >= 0)
    {
      if (!ReplaceRGBValue(strObj, "/BG", "/CA", pField->m_ColorBG))
        if (!ReplaceRGBValue(strObj, "/BG", "/BC", pField->m_ColorBG))
          return false_a;
    }

    if (pField->m_Rec.fBottom >= 0 && pField->m_Rec.fLeft >= 0 &&
        pField->m_Rec.fRight >= 0 && pField->m_Rec.fTop >= 0)
    {
      if (!ReplaceRectValue(strObj, "/Rect", "/T", pField->m_Rec))
        if (!ReplaceRectValue(strObj, "/Rect", "/FT", pField->m_Rec))
          if (!ReplaceRectValue(strObj, "/Rect", "/Subtype", pField->m_Rec))
            return false_a;
    }

    index = pField->m_strID.find(ACROFORM_POSTFIX);
    if (index == -1)
    {
      if (!ReplaceTextValue(strObj, "/T", "/Rect", pField->m_strID.c_str()))
        if (!ReplaceTextValue(strObj, "/T", "/Subtype", pField->m_strID.c_str()))
          if (!ReplaceTextValue(strObj, "/T", "/FT", pField->m_strID.c_str()))
            return false_a;
    }

    if (!ReplaceTextValue(strObj, "/DA", "/Rect", pField->m_strDA.c_str()))
      if (!ReplaceTextValue(strObj, "/DA", "/FT", pField->m_strDA.c_str()))
        if (!ReplaceTextValue(strObj, "/DA", "/T", pField->m_strDA.c_str()))
          return false_a;

    switch (pField->m_eFieldType)
    {
    case CPDFField::eFieldText:
    {
      string strOut;
      CPDFBaseFont *pFont = GetFont(pField->m_pTemplate, pField->m_lFontObjNum);
      if (pFont)
      {
        if (pField->m_Value.GetStringType() == CPDFChar::eMultibyteString)
        {
          if (!pFont->GetUnicodeValue((string)*pField->m_Value.GetString(), strOut))
            return false_a;
        } else {
          if (!pFont->GetUnicodeValue((wstring)*pField->m_Value.GetWString(), strOut))
            return false_a;
        }
      }
      pObj = PDFescapeSpecialCharsBinary(strOut.c_str(), strOut.size(), &lTemp);

      if (!ReplaceTextValue(strObj, "/V", "/T", pObj, lTemp))
        if (!ReplaceTextValue(strObj, "/V", "/Rect", pObj, lTemp))
          if (!ReplaceTextValue(strObj, "/V", "/FT", pObj, lTemp))
            return false_a;
      if (!ReplaceTextValue(strObj, "/DV", "/T", pObj, lTemp))
        if (!ReplaceTextValue(strObj, "/DV", "/Rect", pObj, lTemp))
          if (!ReplaceTextValue(strObj, "/DV", "/FT", pObj, lTemp))
            return false_a;
      if (!pField->m_Childs.size())
      {
        if (pField->m_lAP_NObjNum <= 0)
        {
          pField->m_lAP_NObjNum = m_lLastObj;
          pField->m_lAP_NGenNum = 0;
          sprintf(czNew, "<< /N %ld 0 R >>", m_lLastObj);
          if (!RemoveDictValue(strObj, "/AP"))
            return false_a;
          if (!ReplaceDictValue(strObj, "/AP", "/Rect", czNew))
            if (!ReplaceDictValue(strObj, "/AP", "/T", czNew))
              if (!ReplaceDictValue(strObj, "/AP", "/FT", czNew))
                return false_a;
        }
        else
        {
          pField->m_lAP_NObjNum = m_lLastObj;
          if (!ReplaceRefValue(pField->m_strAP, "/N", ">>", m_lLastObj, 0))
            return false_a;
          pField->m_strAP.insert(0, "<< ");
          pField->m_strAP.append(" >> ");

          if (!RemoveDictValue(strObj, "/AP"))
            return false_a;
          if (!ReplaceDictValue(strObj, "/AP", "/Rect", pField->m_strAP.c_str()))
            if (!ReplaceDictValue(strObj, "/AP", "/T", pField->m_strAP.c_str()))
              if (!ReplaceDictValue(strObj, "/AP", "/FT", pField->m_strAP.c_str()))
                return false_a;
        }
      }
      free(pObj);
      break;
    }
    case CPDFField::eFieldRadioButton:
    case CPDFField::eFieldCheckButton:
      if (/*1 != pField->m_Childs.size()*/ !pField->m_Value.Empty())
      {
        if (!ReplaceNameValue(strObj, "/V", "/Rect", pField->m_Value.GetString()->c_str()))
          if (!ReplaceNameValue(strObj, "/V", "/FT", pField->m_Value.GetString()->c_str()))
            if (!ReplaceNameValue(strObj, "/V", "/T", pField->m_Value.GetString()->c_str()))
              return false_a;
        if (0 == pField->m_Childs.size())
        {
          if (!ReplaceNameValue(strObj, "/AS", "/Rect", pField->m_Value.GetString()->c_str()))
            if (!ReplaceNameValue(strObj, "/AS", "/FT", pField->m_Value.GetString()->c_str()))
              if (!ReplaceNameValue(strObj, "/AS", "/T", pField->m_Value.GetString()->c_str()))
                return false_a;
        }
      }
      break;
    case CPDFField::eFieldUnknown:
    default:
      break;
    }
    Off = m_strRawPDF.size();

    iter1 = m_PdfObjOffMap.find(pField->m_lObjNum);
    if (iter1 == m_PdfObjOffMap.end())
      return false_a;
    m_PdfObjOffMap[pField->m_lObjNum] = Off;

    m_strRawPDF.insert(Off, strObj);
    m_strRawPDF.insert(Off + strObj.size(), "\n");

    if (CPDFField::eFieldText == pField->m_eFieldType && !pField->m_Childs.size())
    {
      pObj = CreateAPStream(pField, Off);
      if (!pObj)
        return false_a;
      strObj.assign(pObj, Off);
      free(pObj);

      Off = m_strRawPDF.size();

      m_PdfObjOffMap[pField->m_lAP_NObjNum] = Off;

      m_strRawPDF.insert(Off, strObj);
      m_strRawPDF.insert(Off + strObj.size(), "\n");
    }
  }

  return true_a;
}

char *CPDFTemplate::CreateAPStream(CPDFField *i_pfield, size_t &o_Size)
{
  char *pTemp, *pObj, czX[20], czY[20], czH[20] = {"0"}, czTemp[200];
  size_t Size;
  float fX = 4, fY = 2, fWidth, f, fFontSize;
  vector <string> values;
  vector <float> Xshift;
  string strValue, strFont;
  CPDFBaseFont *pFont = NULL;

  fX = i_pfield->m_fBorderWidth * 2.f;
  fY = i_pfield->m_fBorderWidth * 2.f;

  pFont = GetFont(i_pfield->m_pTemplate, i_pfield->m_lFontObjNum);
  if (!pFont)
    return false_a;

  //if (!i_pfield->m_pTemplate->GetDR(strValue))
  //  return NULL;
  //if (!ReadItem(strValue, czFONT, "<<", ">>", true_a, strFont)) //TODO: add only used fonts
  //  return NULL;

  //if (!strFont.empty())
  //{
    strFont = "/Font << ";
    sprintf(czTemp, "%s %ld 0 R", i_pfield->m_strFontID.c_str()/*pFont->GetFontResName()*/, pFont->GetFontObjNum());
    strFont.append(czTemp);
    strFont.append(" >>");
  //}

  strValue = "";

  if (pFont)
  {
    float fHeight, fDesc;
    rec_a bbox = pFont->GetFontBBox();

    fHeight = pFont->GetFontHeight(i_pfield->m_lFontSize);


    fHeight = (float)((bbox.lTop - bbox.lBottom) / 1000.0);//ToDo:Is it correct???
    if (0 != i_pfield->m_lFontSize)
      fHeight *= i_pfield->m_lFontSize;
    else
    {
      fHeight = ((i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom) - (2 * i_pfield->m_fBorderWidth));
      fHeight /= (float)((bbox.lTop - bbox.lBottom) / 1000.0);
      fHeight /= (float)((bbox.lTop - bbox.lBottom) / 1000.0);
      
      //fHeight = pFont->CalcFontHeight(i_pfield->m_pValue, fHeight,
      //            (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - (2 * i_pfield->m_fBorderWidth));
    }

    fDesc = pFont->GetFontDescent();
    if (0 != i_pfield->m_lFontSize)
    {
      fDesc *= i_pfield->m_lFontSize;
    }
    else
    {
      fDesc *= fHeight;
      fHeight *= (float)((bbox.lTop - bbox.lBottom) / 1000.0);

      fHeight = pFont->CalcFontHeight(*i_pfield->m_Value.GetString(), fHeight,
                  (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - (2 * i_pfield->m_fBorderWidth));
    }
    fFontSize = fHeight;


    sprintf(czH, "%.4f", fHeight);
    fHeight = (i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom) - fHeight;
    if (i_pfield->m_lFf & 0x1000)//multiline
    {
      fY = fHeight - fY;

      const char *pSpace = NULL, *pB, *pE;
      fWidth = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - (2 * i_pfield->m_fBorderWidth * 2.f);
      pE = pB = i_pfield->m_Value.GetString()->c_str();
      while (0 != *pE)
      {
        if (PDF_SPACE_CHR == *pE || PDF_TAB_CHR == *pE)
          pSpace = pE;
        strValue.append(pE, 1);
        f = pFont->GetFontStringWidth(strValue, i_pfield->m_lFontSize);
        if (f > fWidth || *pE == PDF_NL_CHR1)
        {
          if (!pSpace || f < fWidth)
            pSpace = pE;
          else
            pSpace++;
          pE = pSpace;
          strValue.erase();
          strValue.append(pB, pE - pB);

          pObj = PDFescapeSpecialChars(strValue.c_str());
          strValue.assign(pObj);
          free(pObj);

          basic_string <char>::size_type index = strValue.find_last_of(" ");
          if (index != -1 && index == (strValue.length() - 1))
            strValue.erase(index, 1);

          switch (i_pfield->m_lQ)
          {
          case 2:
            f = pFont->GetFontStringWidth(strValue, i_pfield->m_lFontSize);
            fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - f - (i_pfield->m_fBorderWidth * 2.f);
            break;
          case 1:
            f = pFont->GetFontStringWidth(strValue, i_pfield->m_lFontSize);
            fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - f;
            fX /= 2;
            break;
          case 0:
          default:
            break;
          }
          strValue.append("\\012");

          values.push_back(strValue);
          Xshift.push_back(fX);
          strValue.erase();
          if (*pSpace == PDF_NL_CHR1)
          {
            pSpace++;
          }

          pB = pE = pSpace;
          pSpace = NULL;
        }
        else
          pE++;
        if (0 == *pE && !strValue.empty())
        {
          switch (i_pfield->m_lQ)
          {
          case 2:
            f = pFont->GetFontStringWidth(strValue, i_pfield->m_lFontSize);
            fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - f - (i_pfield->m_fBorderWidth * 2.f);
            break;
          case 1:
            f = pFont->GetFontStringWidth(strValue, i_pfield->m_lFontSize);
            fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - f;
            fX /= 2;
            break;
          case 0:
          default:
            break;
          }

          pObj = PDFescapeSpecialChars(strValue.c_str());
          strValue.assign(pObj);
          free(pObj);

          values.push_back(strValue);
          Xshift.push_back(fX);
        }
      }
    }
    else
    {
      fHeight /= 2;
      fY = fHeight - fDesc;
      switch (i_pfield->m_lQ)
      {
      case 2:
        fWidth = pFont->GetFontStringWidth(i_pfield->m_Value, i_pfield->m_lFontSize);
        fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - fWidth - fX;
        break;
      case 1:
        fWidth = pFont->GetFontStringWidth(i_pfield->m_Value, i_pfield->m_lFontSize);
        fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - fWidth;
        fX /= 2;
        break;
      case 0:
      default:
        break;
      }

      pObj = PDFescapeSpecialChars(i_pfield->m_Value.GetString()->c_str());
      strValue.assign(pObj);
      free(pObj);

//      strValue = i_pfield->m_pValue;
      values.push_back(strValue);
      Xshift.push_back(fX);
    }
  }
  else
  {
    pObj = PDFescapeSpecialChars(i_pfield->m_Value.GetString()->c_str());
    strValue.assign(pObj);
    free(pObj);

//    strValue = i_pfield->m_pValue;
    values.push_back(strValue);
    Xshift.push_back(fX);
  }

  strValue.erase();

  if (i_pfield->m_ColorBG.fR >= 0 && i_pfield->m_ColorBG.fG >= 0 && i_pfield->m_ColorBG.fB >= 0)
  {
    sprintf(czTemp, "%.4f %.4f %.4f rg ", i_pfield->m_ColorBG.fR, i_pfield->m_ColorBG.fG, i_pfield->m_ColorBG.fB);
    strValue.append(czTemp);
    sprintf(czTemp, "0 0 %.4f %.4f re f ", (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft),
            (i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom));
    strValue.append(czTemp);
  }
  if (i_pfield->m_ColorBC.fR >= 0 && i_pfield->m_ColorBC.fG >= 0 && i_pfield->m_ColorBC.fB >= 0)
  {
    sprintf(czTemp, "%.4f %.4f %.4f RG %.4f w ", i_pfield->m_ColorBC.fR,
              i_pfield->m_ColorBC.fG, i_pfield->m_ColorBC.fB, i_pfield->m_fBorderWidth);
    strValue.append(czTemp);
    sprintf(czTemp, "%.4f %.4f %.4f %.4f re s ", i_pfield->m_fBorderWidth / 2, i_pfield->m_fBorderWidth / 2,
              (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - i_pfield->m_fBorderWidth,
              (i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom) - i_pfield->m_fBorderWidth);
    strValue.append(czTemp);
  }


  strValue.append("/Tx BMC q ");

  sprintf(czTemp, "%.4f %.4f %.4f %.4f re W n ", i_pfield->m_fBorderWidth, i_pfield->m_fBorderWidth,
            (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - (2 * i_pfield->m_fBorderWidth),
            (i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom) - (2 * i_pfield->m_fBorderWidth));
  strValue.append(czTemp);

  strValue.append("BT\n");

  strValue.append(i_pfield->m_strDA.c_str());
  if (pFont && 0 == i_pfield->m_lFontSize)
  {

    sprintf(czTemp, " %s %.4f Tf", i_pfield->m_strFontID.c_str(), fFontSize);
    strValue.append(czTemp);
  }
  strValue.append("\n");

  for (size_t ii = 0; ii < values.size(); ii++)
  {
    string strOut;
    CPDFString pdfStrIn(values.at(ii));
    if (!ii)
    {
      if (pFont->GetGidxString(pdfStrIn, strOut))
      {
        strOut.append(" Tj\n");
      } else
      {
        strOut.assign("(");
        strOut.append(values.at(ii));
        strOut.append(") Tj\n");
      }

      fX = Xshift.at(ii);
      sprintf(czX, "%.4f", fX);
      sprintf(czY, "%.4f", fY);
      strValue.append(czX);
      strValue.append(" ");
      strValue.append(czY);
      strValue.append(" Td\n");
      strValue.append(czH);
      strValue.append(" TL\n");
      strValue.append(strOut);
    }
    else
    {
      if (pFont->GetGidxString(pdfStrIn, strOut))
      {
        strOut.append(" '\n");
      } else
      {
        strOut.assign("(");
        strOut.append(values.at(ii));
        strOut.append(") '\n");
      }

      sprintf(czX, "%.4f", Xshift.at(ii) - fX);
      fX = Xshift.at(ii);
      sprintf(czY, "%ld", 0);
      strValue.append(czX);
      strValue.append(" ");
      strValue.append(czY);
      strValue.append(" Td\n");
      strValue.append(strOut);
    }
  }
  strValue.append("ET Q EMC");

  Size = PDFStrLenC(" 0 obj\n<< /Length  /Subtype /Form /BBox [     ] /Resources << /ProcSet [ /PDF /Text ]  >> >> \nstream\n\nendstream\nendobj")
            + strFont.size() + strValue.length() + 20 + (4 * 25);

  pObj = pTemp = (char *)PDFMalloc(Size);
  if (!pTemp)
    return NULL;

  sprintf(pTemp, "%ld 0 obj\n<< /Length %ld /Subtype /Form /BBox [ %.5f %.5f %.5f %.5f ] /Resources << /ProcSet [ /PDF /Text ] %s >> >> \nstream\n",//%s\nendstream\nendobj",
    m_lLastObj, strValue.length(), 0.f, 0.f, (float)(i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft),
    (float)(i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom), strFont.c_str());
  Size = PDFStrLenC(pTemp);
  pObj += Size;

  memcpy(pObj, strValue.c_str(), strValue.length());
  pObj += strValue.size();
  Size += strValue.size();

  memcpy(pObj, "\nendstream\nendobj", PDFStrLenC("\nendstream\nendobj"));
  Size += PDFStrLenC("\nendstream\nendobj");

  i_pfield->m_lAP_NObjNum = m_lLastObj;
  i_pfield->m_lAP_NGenNum = 0;
  m_lLastObj++;

  o_Size = Size;

  return pTemp;
}

long CPDFTemplate::GetFormFieldCount()
{
  long lRet = 0;
  lRet = (long)m_PdfFieldNames.size();
  return lRet;
}

const char *CPDFTemplate::GetFormFieldName(long i_lIndex, bool_a i_bSorted)
{
  long lRet = 0;
  CPDFField *pField;
  basic_string <char>::size_type index;

  if (!i_bSorted)
  {
    Field_iterator iter;
    for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
    {
      pField = (*iter).second;
      if (!pField)
        continue;
      index = pField->m_strID.find(ACROFORM_POSTFIX);
      if (index != -1)
        continue;
      lRet++;

      if (lRet - 1 == i_lIndex)
        return (*iter).first.c_str();
    }
  }
  else
  {
    multimap <const char *, const char *, str_comp_greater >::iterator iter;
    for (iter = m_PdfFieldNames.begin(); iter != m_PdfFieldNames.end(); iter++)
    {
      lRet++;

      if (lRet - 1 == i_lIndex)
        return (*iter).second;
    }
  }

  return NULL;
}

CPDFBaseFont *CPDFTemplate::GetFormFieldFont(const char *i_pName)
{
  if (!i_pName)
    return NULL;
  CPDFBaseFont *pFont = NULL;
  char *pName = PDFescapeSpecialChars(i_pName);
  Field_iterator iter;
  iter = m_PdfFieldObj.find(pName);
  if (iter != m_PdfFieldObj.end())
  {
    CPDFField *pField = (*iter).second;
    if (pField)
      pFont = GetFont(pField->m_pTemplate, pField->m_lFontObjNum);
  }
  free(pName);
  return pFont;
}

long CPDFTemplate::GetFormFieldFontSize(const char *i_pName)
{
  if (!i_pName)
    return NULL;
  long lRet = NOT_USED;
  char *pName = PDFescapeSpecialChars(i_pName);
  Field_iterator iter;
  iter = m_PdfFieldObj.find(pName);
  if (iter != m_PdfFieldObj.end())
  {
    CPDFField *pField = (*iter).second;
    if (pField)
      lRet = pField->m_lFontSize;
  }
  free(pName);
  return lRet;
}

const char *CPDFTemplate::GetFormFieldValue(const char *i_pName)
{
  if (!i_pName)
    return false_a;
  char *pName = PDFescapeSpecialChars(i_pName);
  const char *pRet = NULL;
  Field_iterator iter;
  iter = m_PdfFieldObj.find(pName);
  if (iter != m_PdfFieldObj.end())
  {
    CPDFField *pField = (*iter).second;
    if (pField)
      pRet = pField->m_Value.GetString()->c_str();
  }
  free(pName);
  return pRet;
}

bool_a CPDFTemplate::SetFormField(const char *i_pName, const char *i_pValue)
{
  if (!i_pName)
    return false_a;
  char *pName = PDFescapeSpecialChars(i_pName);
  string str(i_pValue);//*/PDFescapeSpecialChars(pValue/*"VN Besch\r�igu\nngen Text"/*i_pValue*/);
  ReplaceNewLine(str);
  Field_iterator iter;
  iter = m_PdfFieldObj.find(pName);
  if (iter != m_PdfFieldObj.end())
  {
    CPDFField *pField = (*iter).second;
    while (!(*iter).first.compare(pName))
    {
      switch (pField->m_eFieldType)
      {
      case CPDFField::eFieldText:
        {
          pField->m_Value.SetString(str);
          if (pField->m_lMaxLen > 0 && pField->m_Value.Length() > pField->m_lMaxLen)
            pField->m_Value.Erase(pField->m_lMaxLen, pField->m_Value.Length() - pField->m_lMaxLen);

          Field_iterator iterChild;
          char *pChild;
          for (size_t i = 0; i < pField->m_Childs.size(); i++)
          {
            pChild = (pField->m_Childs)[i];
            iterChild = m_PdfFieldObj.find(pChild);
            if (iterChild != m_PdfFieldObj.end())
            {
              CPDFField *pFieldChild = (*iterChild).second;
              pFieldChild->m_Value.SetString(str);
              if (pFieldChild->m_lMaxLen > 0 && pFieldChild->m_Value.Length() > pFieldChild->m_lMaxLen)
                pFieldChild->m_Value.Erase(pFieldChild->m_lMaxLen, pFieldChild->m_Value.Length() - pFieldChild->m_lMaxLen);
              pFieldChild->m_bChanged = true_a;
            }
          }
          pField->m_bChanged = true_a;
          break;
        }
      case CPDFField::eFieldRadioButton:
      case CPDFField::eFieldCheckButton:
        {
  /*
          if (0 == strcmp(pValue, "0"))
          {
            if (pField->m_pValue)
              free(pField->m_pValue);
            pField->m_pValue = PDFStrDup(pField->m_pNo);
          }
          else if (0 == strcmp(pValue, "1"))
          {
            if (pField->m_pValue)
              free(pField->m_pValue);
            pField->m_pValue = PDFStrDup(pField->m_pYes);
          }
          else
  */
          {
            if ('/' == str.at(0))
              pField->m_Value.SetString(str);
            else
            {
              str.insert(0, "/");
              pField->m_Value.SetString(str);
            }
          }
          Field_iterator iterChild;
          char *pChild;
          for (size_t i = 0; i < pField->m_Childs.size(); i++)
          {
            pChild = (pField->m_Childs)[i];
            iterChild = m_PdfFieldObj.find(pChild);
            if (iterChild != m_PdfFieldObj.end())
            {
              CPDFField *pFieldChild = (*iterChild).second;
              if (str.compare(pFieldChild->m_pYes) == 0 || (1 == pField->m_Childs.size()))
              {
                SetFormField(pChild, str.c_str());
              }
              else
              {
                SetFormField(pChild, pFieldChild->m_pNo);
              }
            }
          }
          pField->m_bChanged = true_a;
          break;
        }
      case CPDFField::eFieldUnknown:
      default:
        break;
      }
      //m_PdfFieldObj[(*iter).first] = pField;
      //m_PdfFieldObj.insert ( Field_Pair ( (const char *)(*iter).first, pField ) );
      //iter = m_PdfFieldObj.find(pName);
      //if (iter != m_PdfFieldObj.end())
      //{
      //  pField = (*iter).second;
      //  pField->m_lObjNum = pField->m_lObjNum;
      //}
      free(pName);
      return true_a;
    }
  }
  free(pName);
  return false_a;
}

bool_a CPDFTemplate::SetFormFieldFont(const char *i_pName, CPDFBaseFont *i_pFont, float i_fFontSize)
{
  if (!i_pName || !i_pFont)
    return false_a;
  char *pName = PDFescapeSpecialChars(i_pName);
  basic_string <char>::size_type index, index1;
  char czBuff[100];

  i_pFont->SetUsedInForm();

  Field_iterator iter;
  iter = m_PdfFieldObj.find(pName);
  if (iter != m_PdfFieldObj.end())
  {
    CPDFField *pField = (*iter).second;

    if (i_fFontSize < 0)
      i_fFontSize = pField->m_lFontSize;

    if (!pField->m_strDA.empty())
    {
      sprintf(czBuff, "%ld", pField->m_lFontSize);

      index = pField->m_strDA.find(pField->m_strFontID);
      if (index == -1)
        return false_a;
      pField->m_strDA.erase(index, pField->m_strFontID.length());

      index1 = pField->m_strDA.find(czBuff);
      if (index1 == -1)
        return false_a;
      pField->m_strDA.erase(index, (index1 - index) + PDFStrLenC(czBuff));
      pField->m_strDA.insert(index, "/");
      index++;
      pField->m_strDA.insert(index, i_pFont->GetFontResName());
      index += PDFStrLenC(i_pFont->GetFontResName());
      pField->m_strDA.insert(index, " ");
      index++;
      sprintf(czBuff, "%ld", (long)i_fFontSize);
      pField->m_strDA.insert(index, czBuff);
    }
    pField->m_strFontID = "/";
    pField->m_strFontID.append(i_pFont->GetFontResName());
    pField->m_lFontSize = i_fFontSize;
    pField->m_lFontObjNum = -(i_pFont->GetFontID() + 20);//special use case, used by form because of font from CPDFDocument

    Field_iterator iterChild;
    char *pChild;
    for (size_t i = 0; i < pField->m_Childs.size(); i++)
    {
      pChild = (pField->m_Childs)[i];
      iterChild = m_PdfFieldObj.find(pChild);
      if (iterChild != m_PdfFieldObj.end())
      {
        CPDFField *pFieldChild = (*iterChild).second;
        if (!SetFormFieldFont(pFieldChild->m_strID.c_str(), i_pFont, i_fFontSize))
          return false_a;
      }
    }
    pField->m_bChanged = true_a;
  }
  free(pName);
  return true_a;
}

bool_a CPDFTemplate::MoveFormField(long i_lDeltaX, long i_lDeltaY)
{
  Field_iterator iter;
  CPDFField *pField;
  if (0 == m_PdfFieldObj.size())
    return true_a;
  char *pTemp;
  for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
  {
    pTemp = (char *)iter->first.c_str();
    if (!pTemp)
      return false_a;
    pField = (*iter).second;
    if (pField && pField->m_pParent)
      continue;
    if (!MoveFormField(pTemp, i_lDeltaX, i_lDeltaY))
      return false_a;
  }
  return true_a;
}

bool_a CPDFTemplate::MoveFormField(const char *i_pName, long i_lDeltaX, long i_lDeltaY)
{
  if (!i_pName)
    return false_a;
  char *pName = PDFescapeSpecialChars(i_pName);

  Field_iterator iter;
  iter = m_PdfFieldObj.find(pName);
  if (iter != m_PdfFieldObj.end())
  {
    CPDFField *pField = (*iter).second;

    if (pField->m_Rec.fBottom >= 0 && pField->m_Rec.fLeft >= 0 &&
        pField->m_Rec.fRight >= 0 && pField->m_Rec.fTop >= 0)
    {
      pField->m_Rec.fBottom += PDFConvertDimensionToDefaultL(i_lDeltaY);
      pField->m_Rec.fLeft += PDFConvertDimensionToDefaultL(i_lDeltaX);
      pField->m_Rec.fRight += PDFConvertDimensionToDefaultL(i_lDeltaX);
      pField->m_Rec.fTop += PDFConvertDimensionToDefaultL(i_lDeltaY);

      pField->m_bChanged = true_a;
    }

    Field_iterator iterChild;
    char *pChild;
    for (size_t i = 0; i < pField->m_Childs.size(); i++)
    {
      pChild = (pField->m_Childs)[i];
      iterChild = m_PdfFieldObj.find(pChild);
      if (iterChild != m_PdfFieldObj.end())
      {
        CPDFField *pFieldChild = (*iterChild).second;
        if (!MoveFormField(pFieldChild->m_strID.c_str(), i_lDeltaX, i_lDeltaY))
          return false_a;
      }
    }
  }
  free(pName);
  return true_a;
}

bool_a CPDFTemplate::SetFormFieldBG(const char *i_pName, PdfRgb i_rgb)
{
  if (!i_pName)
    return false_a;
  char *pName = PDFescapeSpecialChars(i_pName);

  Field_iterator iter;
  iter = m_PdfFieldObj.find(pName);
  if (iter != m_PdfFieldObj.end())
  {
    CPDFField *pField = (*iter).second;

    if (pField->m_ColorBG.fB >= 0 && pField->m_ColorBG.fB >= 0 && pField->m_ColorBG.fB >= 0)
    {
      pField->m_ColorBG = i_rgb;

      pField->m_bChanged = true_a;
    }

    Field_iterator iterChild;
    char *pChild;
    for (size_t i = 0; i < pField->m_Childs.size(); i++)
    {
      pChild = (pField->m_Childs)[i];
      iterChild = m_PdfFieldObj.find(pChild);
      if (iterChild != m_PdfFieldObj.end())
      {
        CPDFField *pFieldChild = (*iterChild).second;
        if (!SetFormFieldBG(pFieldChild->m_strID.c_str(), i_rgb))
          return false_a;
      }
    }
  }
  free(pName);
  return true_a;
}

bool_a CPDFTemplate::CheckFormFieldOverflow(const char *i_pName, const char *i_pValue)
{
  if (!i_pName || !i_pValue)
    return false_a;
  char *pName = PDFescapeSpecialChars(i_pName);
  char *pValue;
  string strValue = i_pValue;
  ReplaceNewLine(strValue);
  pValue = PDFStrDup(strValue.c_str());
  Field_iterator iter;
  iter = m_PdfFieldObj.find(pName);
  if (iter != m_PdfFieldObj.end())
  {
    CPDFField *pField = (*iter).second;
    switch (pField->m_eFieldType)
    {
    case CPDFField::eFieldText:
      {
                      vector <string> values;
                      float fWidth, f, fFontH;
                      const char *pSpace = NULL, *pB, *pE;
                      CPDFBaseFont *pFont = NULL;
                      rec_a bbox;

                      strValue.erase();
                      fWidth = (pField->m_Rec.fRight - pField->m_Rec.fLeft) - (2 * pField->m_fBorderWidth * 2.f);

                      pFont = GetFont(pField->m_pTemplate, pField->m_lFontObjNum);

                      if (!pFont)
                      {
                        free(pName);
                        free(pValue);
                        return false_a;
                      }
                      if (pField->m_lFf & 0x1000)//multiline
                      {
                        if (pField->m_lMaxLen > 0 && pField->m_lMaxLen < PDFStrLenC(pValue))
                        {
                          free(pName);
                          free(pValue);
                          return true_a;
                        }

                        bbox = pFont->GetFontBBox();
                        fFontH = (float)((bbox.lTop - bbox.lBottom) / 1000.0);
                        fFontH *= pField->m_lFontSize;

                        pE = pB = pValue;
                        while (0 != *pE)
                        {
                          if (PDF_SPACE_CHR == *pE || PDF_TAB_CHR == *pE)
                            pSpace = pE;
                          strValue.append(pE, 1);
                          f= pFont->GetFontStringWidth(strValue, pField->m_lFontSize);
                          if (f > fWidth || *pE == PDF_NL_CHR1)
                          {
                            if (!pSpace || f < fWidth)
                              pSpace = pE;
                            else
                              pSpace++;
                            pE = pSpace;
                            strValue.erase();
                            strValue.append(pB, pE - pB);

                            basic_string <char>::size_type index = strValue.find_last_of(" ");
                            if (index != -1 && index == (strValue.length() - 1))
                              strValue.erase(index, 1);
                            strValue.append("\\012");

                            values.push_back(strValue);
                            strValue.erase();
                            if (*pSpace == PDF_NL_CHR1)
                              pSpace++;

                            pB = pE = pSpace;
                            pSpace = NULL;
                          }
                          else
                            pE++;
                          if (0 == *pE && !strValue.empty())
                          {
                            values.push_back(strValue);
                          }
                        }
                        if (fFontH * values.size() > ((pField->m_Rec.fTop - pField->m_Rec.fBottom)
                                                      - (2 * pField->m_fBorderWidth * 2.f)))
                        {
                          free(pName);
                          free(pValue);
                          return true_a;
                        }
                      }
                      else
                      {
                        f = pFont->GetFontStringWidth(pValue, pField->m_lFontSize);
                        if (fWidth <= f || (pField->m_lMaxLen > 0 && pField->m_lMaxLen < PDFStrLenC(pValue)))
                        {
                          free(pName);
                          free(pValue);
                          return true_a;
                        }
                      }
      }
      break;
    case CPDFField::eFieldUnknown:
    default:
      break;
    }
  }
  free(pName);
  free(pValue);
  return false_a;
}

long CPDFTemplate::GetObjOffset(long i_lObj)
{
  //if (i_lObj < 0 || i_lObj >= (long)m_PdfObjOffMap.size())
  //   return NOT_USED;
//  long lSize = 0;
  map <long, size_t>::iterator iter1;

  iter1 = m_PdfObjOffMap.find(i_lObj);
  if (iter1 == m_PdfObjOffMap.end())
    return NOT_USED;

  long lOffset = m_PdfObjOffMap[i_lObj];

  return lOffset;
}

bool_a CPDFTemplate::ReadObjOffset(std::string &i_xref, size_t i_Offset)
{
  if (i_xref.empty())
    return false_a;
  size_t pos = i_xref.find(czXREF);
  size_t pos1 = i_xref.find(czTRAILER);
  bool_a bDeleted;
  map <long, size_t>::iterator iter;
  if (pos == string::npos || pos1 == string::npos || pos >= pos1)
    return false_a;
  pos += PDFStrLenC(czXREF);
  long lTemp, lTemp1, lStartObj = NOT_USED, lCount;
  while (pos < pos1)
  {
    if (i_xref.at(pos) == PDF_SPACE_CHR || i_xref.at(pos) == PDF_TAB_CHR || i_xref.at(pos) == PDF_NL_CHR1 || i_xref.at(pos) == PDF_NL_CHR2)
    {
      pos++;
      continue;
    }
    bDeleted = false_a;
    lTemp = atol(i_xref.data() + pos);
    if (lTemp < 0)
      return false_a;
    while ((i_xref.at(pos) >= '0' && i_xref.at(pos) <= '9'))
      pos++;
    while (i_xref.at(pos) == PDF_SPACE_CHR || i_xref.at(pos) == PDF_TAB_CHR)
      pos++;
    lTemp1 = atol(i_xref.data() + pos);
    if (lTemp1 < 0)
      return false_a;
    while ((i_xref.at(pos) >= '0' && i_xref.at(pos) <= '9'))
      pos++;
    while (i_xref.at(pos) == PDF_SPACE_CHR || i_xref.at(pos) == PDF_TAB_CHR)
      pos++;
    if (i_xref.at(pos) == 'f')
      bDeleted = true_a;
    if (i_xref.at(pos) == 'n' || i_xref.at(pos) == 'f')
      pos++;
    else if (i_xref.at(pos) == PDF_NL_CHR1 || i_xref.at(pos) == PDF_NL_CHR2)
    {
      lStartObj = lTemp;
      lCount = lTemp1;
      continue;
    }
    if (m_lLinearObj < 0)
    {
      if (lTemp > 0)
          ReadLinearizedObj(lStartObj, lTemp);
    }
    iter = m_PdfObjOffMap.find(lStartObj);
    if (iter != m_PdfObjOffMap.end())
    {
      if (m_PdfObjOffMap[lStartObj] == -1 && !bDeleted)
        m_PdfObjOffMap[lStartObj] = lTemp;
    }
    else
    {
      if (bDeleted)
        m_PdfObjOffMap[lStartObj] = string::npos;
      else
        m_PdfObjOffMap[lStartObj] = lTemp;
    }
    lStartObj++;
    if (lStartObj == m_lLastObj)
      m_NewObjOff = i_Offset;
  }
  return true_a;
}

void CPDFTemplate::ReadLinearizedObj(long i_lObj, long i_lOff)
{
  if (i_lOff < 0)
    return;
  string strObj, strH, strTemp;
  size_t index;
  long i, lTemp;

  if (!ReadObject(i_lOff, strObj))
    return;
  index = CPDFDocument::PDFStrStrNumKey(strObj, czLINEAR);
  if (index == -1)
    return;
  m_lLinearL = PDFGetNumValue(strObj, czL);
  m_LinearE = PDFGetNumValue(strObj, czE);
  m_LinearT = PDFGetNumValue(strObj, czT);

  index = CPDFDocument::PDFStrStrArrayKey(strObj, czH);
  if (index == -1)
    return;

  ReadItem(strObj, czH, "[", "]", false_a, strH);
  if (strH.empty())
    return;
  index = 0;
  i = 0;
  while (strH.size() > index)
  {
    strTemp = PDFGetStringUpToWhiteSpace(strH, index);
    if (strTemp.empty())
      break;
    if (!PDFIsNumber(strTemp))
      return;
    lTemp = atol(strTemp.c_str());
    if (lTemp <= 0)
      return;
    m_LinearH[i] = lTemp;
    i++;
  }
  m_lLinearObj = i_lObj;
}

long CPDFTemplate::ReadNumValueParent(long i_lParentObj, const char *i_pKey)
{
  long lTemp, lRet;
  string strObj;
  if (i_lParentObj < 0 || !i_pKey)
    return NOT_USED;
  lTemp = GetObjOffset(i_lParentObj);
  if (!ReadObject(lTemp, strObj))
    return false_a;
  lRet = PDFGetNumValue(strObj, i_pKey);
  if (lRet >= 0)
    return lRet;
  lTemp = PDFGetNumValue(strObj, "/Parent");
  if (lTemp < 0)
    return NOT_USED;
  return ReadNumValueParent(lTemp, i_pKey);
}

bool_a CPDFTemplate::ReadTextValueParent(long i_lParentObj, const char *i_pKey, std::string& o_strValue)
{
  long lTemp;
  string strObj;
  if (i_lParentObj < 0 || !i_pKey)
    return NULL;
  lTemp = GetObjOffset(i_lParentObj);
  if (!ReadObject(lTemp, strObj))
    return false_a;
  if (PDFGetTextValue(strObj, i_pKey, o_strValue))
    return true_a;
  lTemp = PDFGetNumValue(strObj, "/Parent");
  if (lTemp < 0)
    return false_a;
  return ReadTextValueParent(lTemp, i_pKey, o_strValue);
}

bool_a CPDFTemplate::ReadTextValueParent(long i_lParentObj, const char *i_pKey, CPDFString& o_strValue)
{
  long lTemp;
  string strObj;
  if (i_lParentObj < 0 || !i_pKey)
    return false_a;
  lTemp = GetObjOffset(i_lParentObj);
  if (!ReadObject(lTemp, strObj))
    return false_a;
  if (PDFGetTextValue(strObj, i_pKey, o_strValue))
    return true_a;
  lTemp = PDFGetNumValue(strObj, "/Parent");
  if (lTemp < 0)
    return false_a;
  return ReadTextValueParent(lTemp, i_pKey, o_strValue);
}

bool_a CPDFTemplate::ReadObject(size_t i_Pos, string &i_strObj)
{
  char *pRetBuf = NULL;
  const char *pPos = NULL;
  long lBufLen = 0;
  long lAllocLen = ALLOC_STEP;
  long lDic = 0;

  i_strObj.erase();

  if (i_Pos < 0 || i_Pos >= m_strRawPDF.size())
    return false_a;
  pRetBuf = (char *)PDFMalloc(ALLOC_STEP * sizeof(char));
  if (!pRetBuf)
    return false_a;
  pPos = m_strRawPDF.c_str();
  while (i_Pos < m_strRawPDF.size())
  {
    if (lBufLen >= lAllocLen - 1)
    {
      lAllocLen += ALLOC_STEP;
      PDFRealloc((void **)&pRetBuf, sizeof(char) * lAllocLen, ALLOC_STEP * sizeof(char));
    }
    pRetBuf[lBufLen] = pPos[i_Pos];
    lBufLen++;
    pRetBuf[lBufLen] = 0;
    i_Pos++;
    if (0 == strcmp(pRetBuf + lBufLen - 6, "endobj") &&
        (pPos[i_Pos - 7] == PDF_SPACE_CHR ||
        pPos[i_Pos - 7] == PDF_TAB_CHR ||
        pPos[i_Pos - 7] == PDF_NL_CHR1 ||
        pPos[i_Pos - 7] == PDF_NL_CHR2) &&
        (pPos[i_Pos] == PDF_SPACE_CHR ||
        pPos[i_Pos] == PDF_TAB_CHR ||
        pPos[i_Pos] == PDF_NL_CHR1 ||
        pPos[i_Pos] == PDF_NL_CHR2) )

    {
      lDic--;
      if (!lDic)
      {
    //MAK: hier läuft die Obj Erkennung falsch, Positionierung stimmt nicht
        if(pRetBuf[0] == 'j')
          i_strObj.assign(pRetBuf+1, lBufLen-1);
        else
          i_strObj.assign(pRetBuf, lBufLen);

        free(pRetBuf);

        string strTemp;
        size_t indexTemp = 0;
        strTemp = PDFGetStringUpToWhiteSpace(i_strObj, indexTemp);// Obj num
        if (strTemp.empty() || !PDFIsNumber(strTemp))
        {
          i_strObj.clear();
          return false_a;
        }
        strTemp = PDFGetStringUpToWhiteSpace(i_strObj, indexTemp);// Gen num
        if (strTemp.empty() || !PDFIsNumber(strTemp))
        {
          i_strObj.clear();
          return false_a;
        }
        strTemp = PDFGetStringUpToWhiteSpace(i_strObj, indexTemp);// obj
        if (strTemp.empty() || strTemp.size() > 3 || strTemp.compare("obj"))
        {
          i_strObj.clear();
          return false_a;
        }

        return true_a;
      }
    }
    if (0 == strcmp(pRetBuf + lBufLen - 3, "obj") &&
        (pPos[i_Pos - 4] == PDF_SPACE_CHR ||
        pPos[i_Pos - 4] == PDF_TAB_CHR ||
        pPos[i_Pos - 4] == PDF_NL_CHR1 ||
        pPos[i_Pos - 4] == PDF_NL_CHR2)/* &&
        (pPos[i_Pos] == PDF_SPACE_CHR ||
        pPos[i_Pos] == PDF_TAB_CHR ||
        pPos[i_Pos] == PDF_NL_CHR1 ||
        pPos[i_Pos] == PDF_NL_CHR2)*/ )
    {
      lDic++;
    }
  }
  free(pRetBuf);
  pRetBuf = NULL;
  return false_a;
}

char *CPDFTemplate::ReadObject(size_t i_Pos, long &o_lSize)
{
  char *pRetBuf = NULL;
  const char *pPos = NULL;
  long lBufLen = 0;
  long lAllocLen = ALLOC_STEP;
  long lDic = 0;

  if (i_Pos < 0 || i_Pos >= m_strRawPDF.size())
    return pRetBuf;
  pRetBuf = (char *)PDFMalloc(ALLOC_STEP * sizeof(char));
  if (!pRetBuf)
    return NULL;
  pPos = m_strRawPDF.c_str();
  while (i_Pos < m_strRawPDF.size())
  {
    if (lBufLen >= lAllocLen - 1)
    {
      lAllocLen += ALLOC_STEP;
      PDFRealloc((void **)&pRetBuf, sizeof(char) * lAllocLen, ALLOC_STEP * sizeof(char));
    }
    pRetBuf[lBufLen] = pPos[i_Pos];
    lBufLen++;
    pRetBuf[lBufLen] = 0;
    i_Pos++;
    if (0 == strcmp(pRetBuf + lBufLen - 6, "endobj") &&
        (pPos[i_Pos - 7] == PDF_SPACE_CHR ||
        pPos[i_Pos - 7] == PDF_TAB_CHR ||
        pPos[i_Pos - 7] == PDF_NL_CHR1 ||
        pPos[i_Pos - 7] == PDF_NL_CHR2) &&
        (pPos[i_Pos] == PDF_SPACE_CHR ||
        pPos[i_Pos] == PDF_TAB_CHR ||
        pPos[i_Pos] == PDF_NL_CHR1 ||
        pPos[i_Pos] == PDF_NL_CHR2) )

    {
      lDic--;
      if (!lDic)
      {
        o_lSize = lBufLen;
        return pRetBuf;
      }
    }
    if (0 == strcmp(pRetBuf + lBufLen - 3, "obj") &&
        (pPos[i_Pos - 4] == PDF_SPACE_CHR ||
        pPos[i_Pos - 4] == PDF_TAB_CHR ||
        pPos[i_Pos - 4] == PDF_NL_CHR1 ||
        pPos[i_Pos - 4] == PDF_NL_CHR2)/* &&
        (pPos[i_Pos] == PDF_SPACE_CHR ||
        pPos[i_Pos] == PDF_TAB_CHR ||
        pPos[i_Pos] == PDF_NL_CHR1 ||
        pPos[i_Pos] == PDF_NL_CHR2)*/ )
    {
      lDic++;
    }
  }
  free(pRetBuf);
  pRetBuf = NULL;
  return pRetBuf;
}

string CPDFTemplate::ReadDict(size_t i_pos)
{
  string strRet("");

  char *pRetBuf = NULL;
  long lBufLen = 0;
  long lDic = 0;

  if (i_pos < 0 || i_pos >= m_strRawPDF.size())
    return strRet;
  while (i_pos < m_strRawPDF.size())
  {
    strRet.append(&m_strRawPDF.at(i_pos), 1);
    i_pos++;
    if (strRet.size() >= 2 && strRet.at(strRet.size() - 1) == '<' && strRet.at(strRet.size() - 2) == '<')
    {
      lDic++;
      if (i_pos + 1 < m_strRawPDF.size())
      {
        strRet.append(&m_strRawPDF.at(i_pos), 1);
        i_pos++;
      }
    }
    if (strRet.size() >= 2 && strRet.at(strRet.size() - 1) == '>' && strRet.at(strRet.size() - 2) == '>')
    {
      lDic--;
      if (!lDic)
      {
        return strRet;
      }
      if (i_pos + 1 < m_strRawPDF.size())
      {
        strRet.append(&m_strRawPDF.at(i_pos), 1);
        i_pos++;
      }
    }
  }
  strRet.clear();
  return strRet;
}

char *CPDFTemplate::ReadTrailer(size_t& i_Pos)
{
  char *pRetBuf = NULL;
  const char *pPos = NULL;
  long lBufLen = 0;
  long lAllocLen = ALLOC_STEP;
  long lDic = 0;
  size_t Pos = i_Pos;

  if (i_Pos < 0 || i_Pos >= m_strRawPDF.size())
    return pRetBuf;
  pPos = strstr(m_strRawPDF.c_str() + i_Pos, czTRAILER);
  if (!pPos)
    return pRetBuf;
  Pos = i_Pos = pPos - m_strRawPDF.c_str();
  pPos = m_strRawPDF.c_str();
  pRetBuf = (char *)PDFMalloc(ALLOC_STEP * sizeof(char));
  if (!pRetBuf)
    return NULL;
  while (Pos < m_strRawPDF.size())
  {
    if (lBufLen >= lAllocLen - 1)
    {
      lAllocLen += ALLOC_STEP;
      PDFRealloc((void **)&pRetBuf, sizeof(char) * lAllocLen, ALLOC_STEP * sizeof(char));
    }
    pRetBuf[lBufLen] = pPos[Pos];
    lBufLen++;
    pRetBuf[lBufLen] = 0;
    Pos++;
    if (0 == strcmp(pRetBuf + lBufLen - 2, czBGNDIC))
    {
      lDic++;
      if (Pos + 1 < m_strRawPDF.size())
      {
        pRetBuf[lBufLen] = pPos[Pos];
        lBufLen++;
        Pos++;
      }
    }
    if (0 == strcmp(pRetBuf + lBufLen - 2, czENDDIC))
    {
      lDic--;
      if (!lDic)
      {
        return pRetBuf;
      }
      if (Pos + 1 < m_strRawPDF.size())
      {
        pRetBuf[lBufLen] = pPos[Pos];
        lBufLen++;
        Pos++;
      }
    }
  }
  free(pRetBuf);
  pRetBuf = NULL;
  return pRetBuf;
}

long CPDFTemplate::ChangeIndObjToArray(string& io_strObj, size_t i_index)
{
  if (io_strObj.empty() || i_index < 0 || i_index >= io_strObj.size())
    return 0;

  string strTemp;
  size_t indexTmp = i_index;

  strTemp = PDFGetStringUpToWhiteSpace(io_strObj, indexTmp);// Obj num
  if (strTemp.empty())
    return 0;
  if (!PDFIsNumber(strTemp))
    return 0;
  strTemp = PDFGetStringUpToWhiteSpace(io_strObj, indexTmp);// Gen num
  if (strTemp.empty())
    return 0;
  if (!PDFIsNumber(strTemp))
    return 0;
  strTemp = PDFGetStringUpToWhiteSpace(io_strObj, indexTmp);// R
  indexTmp -= strTemp.size() - 1;
  if (strTemp.empty())
    return 0;
  if ('R' != strTemp[0])
    return 0;

  io_strObj.insert(i_index, "[ ");
  io_strObj.insert(indexTmp + 2, " ]");

  return 4; // 4 chars added
}

long CPDFTemplate::AddIndObjToArray(std::string& io_strObj, size_t i_index, const char *i_pObjRef, bool_a bBegin)
{
  if (io_strObj.empty() || !i_pObjRef || i_index < 0 || i_index >= io_strObj.size())
    return 0;

  string strTemp;
  long lLen = 0;

  MoveOverWhiteChars(io_strObj, i_index);
  if (io_strObj[i_index] != '[')
    return 0;
  i_index++;

  if (!bBegin)
  {
    while (io_strObj.size() > i_index)
    {
      strTemp = PDFGetStringUpToWhiteSpace(io_strObj, i_index); //object number
      if (strTemp.empty())
        return 0;
      if (!PDFIsNumber(strTemp))
        return 0;

      strTemp = PDFGetStringUpToWhiteSpace(io_strObj, i_index); //Gen number
      if (strTemp.empty())
        return 0;
      if (!PDFIsNumber(strTemp))
        return 0;

      MoveOverWhiteChars(io_strObj, i_index);
      if ('R' != io_strObj[i_index])
        return 0;
      i_index++;
      MoveOverWhiteChars(io_strObj, i_index);
      if (']' == io_strObj[i_index])
      {
        if (io_strObj[i_index - 1] != ' ')
        {
          io_strObj.insert(i_index, " ");
          lLen += 1;
          i_index++;
        }
        break;
      }
    }
  } else {
    if (io_strObj[i_index] != ' ')
    {
      io_strObj.insert(i_index, " ");
      lLen += 1;
    }
  }

  lLen += PDFStrLenC(i_pObjRef);

  io_strObj.insert(i_index, i_pObjRef);

  return lLen;
}

long CPDFTemplate::AddIndObjToArray(std::string& io_strObj, size_t i_index, long i_lObj, bool_a bBegin)
{
  if (io_strObj.empty() || i_lObj <= 0 || i_index < 0 || i_index >= io_strObj.size())
    return 0;
  char czTemp[100];
  sprintf(czTemp, " %ld 0 R ", i_lObj);
  return AddIndObjToArray(io_strObj, i_index, czTemp, bBegin);
}

bool_a CPDFTemplate::UpdateOffsets(size_t i_Offset, size_t i_AddedSize)
{
  if (i_AddedSize == 0)
    return true_a;
  if (m_RootOffset != -1 && m_RootOffset > i_Offset)
    m_RootOffset += i_AddedSize;
  if (m_PagesOffset != -1 && m_PagesOffset > i_Offset)
    m_PagesOffset += i_AddedSize;
  if (m_NewObjOff != -1 && m_NewObjOff > i_Offset)
    m_NewObjOff += i_AddedSize;
  if (m_LinearE != -1 && m_LinearE > i_Offset)
    m_LinearE += i_AddedSize;
  if (m_LinearT != -1 && m_LinearT > i_Offset)
    m_LinearT += i_AddedSize;

  if (m_LinearH[0] != -1 && m_LinearH[0] > i_Offset)
    m_LinearH[0] += i_AddedSize;
  if (m_LinearH[2] != -1 && m_LinearH[2] > i_Offset)
    m_LinearH[2] += i_AddedSize;

  size_t i;
  for (i = 0; i < m_PDFPages.size(); i++)
  {
    if (m_PDFPages[(long)i].ObjOffset != -1 && m_PDFPages[(long)i].ObjOffset >= i_Offset)
      m_PDFPages[(long)i].ObjOffset += i_AddedSize;
  }

  for (i = 0; i < m_PdfXrefMap.size(); i++)
  {
    if (m_PdfXrefMap[(long)i] != -1 && m_PdfXrefMap[(long)i] >= i_Offset)
      m_PdfXrefMap[(long)i] += i_AddedSize;
  }

  for (i = 1; i < m_PdfObjOffMap.size(); i++)
  {
    //size_t tmpSize = m_PdfObjOffMap[(long)i];
    if (m_PdfObjOffMap[(long)i] != -1 && m_PdfObjOffMap[(long)i] >= i_Offset)
      m_PdfObjOffMap[(long)i] += i_AddedSize;
  }

  return true_a;
}

long CPDFTemplate::GetNewObjOffet()
{
  return m_strRawPDF.size();

  //map <long, size_t>::reverse_iterator iter;
  //long lRet = -1;
  //for (iter = m_PdfObjOffMap.rbegin(); iter != m_PdfObjOffMap.rend(); iter++)
  //{
  //   lRet = iter->second;
  //   if (lRet >= 0)
  //      break;
  //}
  //return lRet;
}

bool_a CPDFTemplate::WriteNewAndChangedObjToXref()
{
  char *pObj;
  string strObj;
  char czNew[100];
  long lCount;
  size_t index;
  CPDFField *pField;

  index = m_PdfXrefMap[0];
  pObj = ReadTrailer(index);
  if (!pObj)
    return false_a;
  strObj = pObj;
  free(pObj);

  if (Encrypt())
  {
    index = strObj.find("/Root");
    if (index == -1)
      return false_a;
    if (m_lEncryptObj < 0)
    {
      m_lEncryptObj = m_lLastObj;
      m_lLastObj++;

      strObj.insert(index, "/Encrypt");
      index += PDFStrLenC("/Encrypt");
      sprintf(czNew, " %ld", m_lEncryptObj);
      strObj.insert(index, czNew);
      index += PDFStrLenC(czNew);
      strObj.insert(index, " 0 R");
    }

    if (!WriteEncryption(m_strRawPDF.size()))
      return false_a;
  } else
  { //Document has no password => leave it decrypted
    //if (m_lEncryptObj > 0)
    //{
    //  string strTemp;
    //  size_t index, indexEnd;
    //  index = CPDFDocument::PDFStrStrNumKey(strObj, "/Encrypt");
    //  if (index == -1)
    //  {
    //    indexEnd = index = CPDFDocument::PDFStrStrKey(strObj, "/Encrypt");
    //    strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // /Encrypt
    //    strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // encrypt dict
    //  } else 
    //  {
    //    indexEnd = index = CPDFDocument::PDFStrStrKey(strObj, "/Encrypt");
    //    strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // /Encrypt
    //    strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // Obj num
    //    strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // Gen num
    //    strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // 'R'
    //  }
    //  strObj.erase(index, indexEnd - index);
    //}
  }


  if (Encrypt())
  {
    size_t indexXref, OldSize;
    map <long, size_t>::iterator iterXref = m_PdfXrefMap.begin();
    string strXref;
    long iiii = 0;
    while(iterXref != m_PdfXrefMap.end())
    {
      indexXref = iterXref->second;
      if (indexXref != -1)
      {
        pObj = ReadTrailer(indexXref);
        if (!pObj)
          return false_a;
        strXref = pObj;
        free(pObj);
        OldSize = strXref.size();

        if (!ReplaceRefValue(strXref, "/Encrypt", "/Size", m_lEncryptObj, 0))
          return false_a;
        m_strRawPDF.erase(indexXref, OldSize);
        m_strRawPDF.insert(indexXref, strXref);
        if (OldSize != strXref.size())
        {
          if (!UpdateOffsets(indexXref, strXref.size() - OldSize))
            return false_a;
          //iterXref = m_PdfXrefMap.begin();
          continue;
        }
      }
      iiii++;
      iterXref++;
    }
  }



  size_t Temp;
  size_t Off = m_strRawPDF.size();
  lCount = PDFGetNumValue(strObj, czSIZE);
  if (lCount < 0)
    return false_a;


  if (!ReplaceNumValue(strObj, czPREV, "/Root", m_PdfXrefMap[0]))
    return false_a;
  if (!ReplaceNumValue(strObj, czSIZE, "/Root", m_lLastObj))
    return false_a;

  for (Temp = m_PdfXrefMap.size() - 1; Temp != -1; Temp--)
    m_PdfXrefMap[(long)Temp + 1] = m_PdfXrefMap[(long)Temp];
  m_PdfXrefMap[0] = Off;

  m_strRawPDF.append(czXREF);
  m_strRawPDF.append("\r");

  //info
  Temp = m_PdfObjOffMap[m_lInfoObj];
  sprintf(czNew, "%ld 1\n", m_lInfoObj);
  m_strRawPDF.append(czNew);
  sprintf(czNew, "%010ld %05ld n \r", (long)Temp, 0);
  m_strRawPDF.append(czNew);

  if (Encrypt())
  {
    Temp = m_PdfObjOffMap[m_lEncryptObj];
    sprintf(czNew, "%ld 1\n", m_lEncryptObj);
    m_strRawPDF.append(czNew);
    sprintf(czNew, "%010ld %05ld n \r", (long)Temp, 0);
    m_strRawPDF.append(czNew);
  }


  Field_iterator iter;
  for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
  {
    pField = (*iter).second;
    if (!pField->m_bChanged)
      continue;
    Temp = m_PdfObjOffMap[pField->m_lObjNum];

    sprintf(czNew, "%ld 1\n", pField->m_lObjNum);
    m_strRawPDF.append(czNew);
    sprintf(czNew, "%010ld %05ld n \r", (long)Temp, pField->m_lGenNum);
    m_strRawPDF.append(czNew);

    if (CPDFField::eFieldText == pField->m_eFieldType && pField->m_lAP_NObjNum > 0)
    {
      Temp = m_PdfObjOffMap[pField->m_lAP_NObjNum];
      sprintf(czNew, "%ld 1\n", pField->m_lAP_NObjNum);
      m_strRawPDF.append(czNew);
      sprintf(czNew, "%010ld %05ld n \r", (long)Temp, pField->m_lAP_NGenNum);
      m_strRawPDF.append(czNew);
    }
  }

  if (lCount < m_lLastObj)
  {
    sprintf(czNew, "%ld %ld\n", lCount, m_lLastObj - lCount);
    m_strRawPDF.append(czNew);

    for (long i = 0; i < (m_lLastObj - lCount); i++)
    {
      sprintf(czNew, "%010ld %05ld n \r", m_PdfObjOffMap[lCount + i], 0);
      m_strRawPDF.append(czNew);
    }
  }

  m_strRawPDF.append(strObj);
  m_strRawPDF.append("\r");

  return true_a;
}

bool_a CPDFTemplate::UpdateTrailer()
{
  if (0 == m_PdfXrefMap.size())
      return false_a;
  map <long, size_t>::reverse_iterator iter;
  char czNew[200], *pObj;
  size_t Temp, Off, AddedSize;
  string strObj, strTemp;
  size_t index, index1;

  for (iter = m_PdfXrefMap.rbegin(); iter != m_PdfXrefMap.rend(); iter++)
  {
    index1 = Temp = iter->second;
    if (Temp != -1)
    {
      pObj = ReadTrailer(index1);
      if (!pObj)
        return false_a;
      strObj = pObj;
      free(pObj);

      if (m_lEncryptObj > 0)
      {
        size_t indexBegin, indexEnd;
        indexBegin = CPDFDocument::PDFStrStrNumKey(strObj, "/Encrypt");
        if (indexBegin == -1)
        {
          indexEnd = indexBegin = CPDFDocument::PDFStrStrKey(strObj, "/Encrypt");
          strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // /Encrypt
          strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // encrypt dict
        } else 
        {
          indexEnd = indexBegin = CPDFDocument::PDFStrStrKey(strObj, "/Encrypt");
          strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // /Encrypt
          strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // Obj num
          strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // Gen num
          strTemp = PDFGetStringUpToWhiteSpace(strObj, indexEnd); // 'R'
          indexEnd -= strTemp.size() - 1;
        }
        indexEnd += index1;
        indexBegin += index1;
        //m_strRawPDF.erase(indexBegin, indexEnd - indexBegin);
        //if (!UpdateOffsets(indexBegin, indexBegin - indexEnd))
        //  return false_a;
        for (size_t ii = indexBegin; ii < indexEnd; ii++)
        {
          m_strRawPDF.replace(/*index1 + */ii, 1, " ");
        }
      }

      index = CPDFDocument::PDFStrStrNumKey(strObj, czPREV);
      if (index != -1)
      {
        index1 = index = index + index1;
        strTemp = PDFGetStringUpToWhiteSpace(m_strRawPDF, index1);
        if (!PDFIsNumber(strTemp))
          return false_a;
        MoveOverWhiteChars(m_strRawPDF, index);
        m_strRawPDF.erase(index, strTemp.size());
        sprintf(czNew, "%ld", (long)Off);
        m_strRawPDF.insert(index, czNew);
        AddedSize = strTemp.size() - PDFStrLenC(czNew);
        if (!UpdateOffsets(index, AddedSize))
          return false_a;
      }
    }
    Off = Temp;
  }
  return true_a;
}

bool_a CPDFTemplate::UpdatePreview()
{
  if (0 == m_PdfXrefMap.size())
      return false_a;
  map <long, size_t>::reverse_iterator iter;
  char czNew[200], *pObj;
  size_t Temp, Off, AddedSize;
  string strObj, strTemp;
  size_t index, index1;

  for (iter = m_PdfXrefMap.rbegin(); iter != m_PdfXrefMap.rend(); iter++)
  {
    index1 = Temp = iter->second;
    if (Temp != -1)
    {
      pObj = ReadTrailer(index1);
      if (!pObj)
        return false_a;
      strObj = pObj;
      free(pObj);

      index = CPDFDocument::PDFStrStrNumKey(strObj, czPREV);
      if (index != -1)
      {
        index1 = index = index + index1;
        strTemp = PDFGetStringUpToWhiteSpace(m_strRawPDF, index1);
        if (!PDFIsNumber(strTemp))
          return false_a;
        MoveOverWhiteChars(m_strRawPDF, index);
        m_strRawPDF.erase(index, strTemp.size());
        sprintf(czNew, "%ld", (long)Off);
        m_strRawPDF.insert(index, czNew);
        AddedSize = strTemp.size() - PDFStrLenC(czNew);
        if (!UpdateOffsets(index, AddedSize))
          return false_a;
      }
    }
    Off = Temp;
  }
  return true_a;
}

bool_a CPDFTemplate::WriteLinearizedObj()
{
  if (m_lLinearObj < 0)
    return true_a;

  string strObj, strTemp;
  size_t index, index1, OldSize;
  char czTemp[100];
  long lOff = GetObjOffset(m_lLinearObj);

  if (!ReadObject(lOff, strObj))
    return false_a;
  OldSize = strObj.size();
  m_strRawPDF.erase(lOff, strObj.size());

  if (!ReplaceNumValue(strObj, czL, ">>", (long)m_strRawPDF.size()))
    return false_a;
  if (!ReplaceNumValue(strObj, czE, ">>", (long)m_LinearE))
    return false_a;
  if (!ReplaceNumValue(strObj, czT, ">>", (long)m_LinearT))
    return false_a;

  index1 = index = CPDFDocument::PDFStrStrArrayKey(strObj, czH);
  if (index == -1)
    return false_a;

  strTemp = PDFGetStringUpToWhiteSpace(strObj, index1);
  MoveOverWhiteChars(strObj, index);
  strObj.erase(index, strTemp.size());

  if (m_LinearH[0] == -1)
    return false_a;
  strTemp = "[ ";
  sprintf(czTemp, "%ld %ld ", m_LinearH[0], m_LinearH[1]);
  strTemp.append(czTemp);
  if (m_LinearH[2] != -1)
  {
    sprintf(czTemp, "%ld %ld ", m_LinearH[2], m_LinearH[3]);
    strTemp.append(czTemp);
  }
  strTemp.append("]");
  strObj.insert(index, strTemp);


  if (OldSize < strObj.size())
  {
    m_strRawPDF.erase(lOff, strObj.size() - OldSize);
  } else if (OldSize > strObj.size())
  {
    OldSize -= strObj.size();
    for (size_t i = 0; i < OldSize; i++)
      m_strRawPDF.insert(lOff, " ");
  }
  m_strRawPDF.insert(lOff, strObj);

  return true_a;
}

bool_a CPDFTemplate::WriteOffset()
{
  size_t index, index1, indexSave, i;
  char czTemp[11] = {0};
  bool_a bDeleted;
  size_t Offset;
  long lTemp, lTemp1, lStartObj = NOT_USED, lCount;
  string strTemp;
  map <long, size_t>::iterator iter;
  for (i = 0; i < m_PdfXrefMap.size(); i++)
  {
    Offset = m_PdfXrefMap[(long)i];
    if (Offset == -1)
      continue;

    index = CPDFDocument::PDFStrStrKey(m_strRawPDF, Offset, czXREF);
    index1 = CPDFDocument::PDFStrStrKey(m_strRawPDF, Offset, czTRAILER);

    if (index == -1 || index1 == -1 || index >= index1)
      return false_a;


    index += PDFStrLenC(czXREF);
    lStartObj = NOT_USED;
    while (index < index1)
    {
      MoveOverWhiteChars(m_strRawPDF, index);

      bDeleted = false_a;
      lTemp = atol(m_strRawPDF.c_str() + index);
      indexSave = index;
      if (lTemp < 0)
        return false_a;
      while ((m_strRawPDF[index] >= '0' && m_strRawPDF[index] <= '9'))
        index++;
      while (m_strRawPDF[index] == PDF_SPACE_CHR || m_strRawPDF[index] == PDF_TAB_CHR)
        index++;
      lTemp1 = atol(m_strRawPDF.c_str() + index);
      if (lTemp1 < 0)
        return false_a;
      while ((m_strRawPDF[index] >= '0' && m_strRawPDF[index] <= '9'))
        index++;
      while (m_strRawPDF[index] == PDF_SPACE_CHR || m_strRawPDF[index] == PDF_TAB_CHR)
        index++;
      if (m_strRawPDF[index] == 'f')
        bDeleted = true_a;
      if (m_strRawPDF[index] == 'n' || m_strRawPDF[index] == 'f')
        index++;
      else if (m_strRawPDF[index] == PDF_NL_CHR1 || m_strRawPDF[index] == PDF_NL_CHR2)
      {
        lStartObj = lTemp;
        lCount = lTemp1;
        continue;
      }
      iter = m_PdfObjOffMap.find(lStartObj);
      if (iter == m_PdfObjOffMap.end())
        return false_a;
      if (m_PdfObjOffMap[lStartObj] != lTemp && m_PdfObjOffMap[lStartObj] >= 0 && !bDeleted)
      {
        sprintf(czTemp, "%010ld", m_PdfObjOffMap[lStartObj]);
        m_strRawPDF.replace(indexSave, 10, czTemp);
      }
      lStartObj++;
      MoveOverWhiteChars(m_strRawPDF, index);
    }
  }

  return true_a;
}

const char *CPDFTemplate::GetPDFStream(long &o_lSize)
{
  o_lSize = 0;
  if (!m_bFlush)
  {

    if (!UpdateTrailer())
       return NULL;

    //if (0 == m_PdfFieldObj.size() && Encrypt())
    //{
    //  m_lEncryptObj = m_lLastObj;
    //  m_lLastObj++;
    //  if (!WriteEncryption())
    //    return false_a;
    //}
    if (m_pDoc && !WriteInfo(m_strRawPDF.size()))
      return NULL;

    if (!WriteFields())
      return NULL;

    if (m_pDoc)
      if (!m_pDoc->CopyResourcesToTemplate())
        return NULL;

    if (!WriteNewAndChangedObjToXref())
      return NULL;

    if (!DoEncrypt())
      return NULL;

    char czNew[21];
    sprintf(czNew, "%ld\n", m_PdfXrefMap[0]);
    m_strRawPDF.append("startxref\n");
    m_strRawPDF.append(czNew);
    m_strRawPDF.append("%%EOF");

    if (!WriteLinearizedObj())
      return NULL;

    if (!UpdatePreview())
       return NULL;

    if (!WriteOffset())
      return NULL;

    m_bFlush = true_a;
  }

  o_lSize = (long)m_strRawPDF.size();
  return m_strRawPDF.c_str();
}





















bool_a CPDFTemplate::AddPageToTemplate(CPDFPage *i_pPage, const char *i_pFilter)
{
  if (!i_pPage)
    return false_a;
  CPDFBaseFont *pFont;
  CPDFImage *pImg;
  CPDFLayer *pLayer;
  CPDFAnnotation *pAnnot;
  long lSize, lPage = i_pPage->GetPageNumber();
  const char *pContent = i_pPage->GetContent(lSize);
  if (lPage > (long)m_PDFPages.size() + 1 || lPage <= 0 || !pContent || !lSize)
    return false_a;

  if (!WriteSaveContent())
    return false_a;
  if (!WriteRestoreContent())
    return false_a;

  if ((long)m_PDFPages.size() + 1 == lPage)
  {
    if (!CreateNewPage(i_pPage))
      return false_a;
  }

  long lOff, lAddedSize = 0;
  char czTemp[200];

  sprintf(czTemp, "%ld 0 R", m_lLastObj);
  if (!AddNewContentRefToPage(lPage, czTemp))
    return false_a;

  lOff = GetNewObjOffet();

  sprintf(czTemp, "%ld 0 obj <</Length %ld /Filter %s >>\nstream\n", m_lLastObj, lSize, i_pFilter);
  m_strRawPDF.insert(lOff, czTemp);
  lAddedSize = PDFStrLenC(czTemp);
  m_strRawPDF.insert(lOff + lAddedSize, pContent, lSize);
  lAddedSize += lSize;
  m_strRawPDF.insert(lOff + lAddedSize, "\nendstream\nendobj\n");
  lAddedSize += PDFStrLenC("\nendstream\nendobj\n");

  if (!UpdateOffsets(lOff, lAddedSize))
    return false_a;
  m_PdfObjOffMap[m_lLastObj] = lOff;
  m_lLastObj++;

  if (m_pDoc)
  {
    long j;
    for (j = 0; j < i_pPage->GetFontsCount(); j++)
    {
      pFont = m_pDoc->GetFont(i_pPage->GetFontID(j));
      if (pFont)
        if (!AddFontToPage(i_pPage->GetPageNumber(), pFont))
          return false_a;
    }

    for (j = 0; j < i_pPage->GetImagesCount(); j++)
    {
      pImg = m_pDoc->GetImage(i_pPage->GetImageID(j));
      if (pImg)
        if (!AddImageToPage(i_pPage->GetPageNumber(), pImg))
          return false_a;
    }

    for (j = 0; j < m_pDoc->GetLayerCount(); j++)
    {
      pLayer = m_pDoc->GetLayer(j);
      if (pLayer)
        if (!AddLayerToPage(i_pPage->GetPageNumber(), pLayer))
          return false_a;
    }

    for (j = 0; j < i_pPage->GetAnnotsCount(); j++)
    {
      pAnnot = m_pDoc->GetForm(i_pPage->GetAnnotID(j));
      if (pAnnot && pAnnot->GetAnnotationType() != eAnnotWidget)
      {
        if (!AddFormToPage(i_pPage->GetPageNumber(), pAnnot))
          return false_a;
      }
    }
  }

  return true_a;
}

bool_a CPDFTemplate::WriteSaveContent()
{
  if (m_lSaveContObj > 0)
    return true_a;
  long lOff = GetNewObjOffet();
  char czTemp[200];
  long lAddedSize = 0;

  sprintf(czTemp, "%ld 0 obj\n<<\n/Length %ld\n/Filter /FlateDecode\n>>\nstream\n", m_lLastObj, lSAVE_RESTORE);
  m_strRawPDF.insert(lOff, czTemp);
  lAddedSize = PDFStrLenC(czTemp);

  m_strRawPDF.insert(lOff + lAddedSize, czSAVE, lSAVE_RESTORE);
  lAddedSize += lSAVE_RESTORE;

  m_strRawPDF.insert(lOff + lAddedSize, "\nendstream\nendobj\n");
  lAddedSize += PDFStrLenC("\nendstream\nendobj\n");

  if (!UpdateOffsets(lOff, lAddedSize))
    return false_a;
  m_PdfObjOffMap[m_lLastObj] = lOff;
  m_lSaveContObj = m_lLastObj;
  m_lLastObj++;
  return true_a;
}

bool_a CPDFTemplate::WriteRestoreContent()
{
  if (m_lRestContObj > 0)
    return true_a;
  long lOff = GetNewObjOffet();
  char czTemp[200];
  long lAddedSize = 0;

  sprintf(czTemp, "%ld 0 obj\n<<\n/Length %ld\n/Filter /FlateDecode\n>>\nstream\n", m_lLastObj, lSAVE_RESTORE);
  m_strRawPDF.insert(lOff, czTemp);
  lAddedSize = PDFStrLenC(czTemp);

  m_strRawPDF.insert(lOff + lAddedSize, czRESTORE, lSAVE_RESTORE);
  lAddedSize += lSAVE_RESTORE;

  m_strRawPDF.insert(lOff + lAddedSize, "\nendstream\nendobj\n");
  lAddedSize += PDFStrLenC("\nendstream\nendobj\n");

  if (!UpdateOffsets(lOff, lAddedSize))
    return false_a;
  m_PdfObjOffMap[m_lLastObj] = lOff;
  m_lRestContObj = m_lLastObj;
  m_lLastObj++;
  return true_a;
}

bool_a CPDFTemplate::WriteStartLayerContent()
{
  if (m_lStartLayerContObj > 0)
    return true_a;
  if (!m_pLayer)
    return false_a;
  long lOff = GetNewObjOffet();
  char czTemp[200];
  long lAddedSize = 0;

  string strLayerCont("/OC /");
  strLayerCont.append(m_pLayer->GetLayerResName());
  strLayerCont.append(" BDC\n");

  //if (!CPDFDocument::CompressStream(strLayerCont))
  //  return false_a;

  sprintf(czTemp, "%ld 0 obj << /Length %ld >>\nstream\n",
      m_lLastObj, (long)strLayerCont.size());
  m_strRawPDF.insert(lOff, czTemp);
  lAddedSize = PDFStrLenC(czTemp);

  m_strRawPDF.insert(lOff + lAddedSize, strLayerCont);
  lAddedSize += strLayerCont.size();

  m_strRawPDF.insert(lOff + lAddedSize, "\nendstream endobj\n");
  lAddedSize += PDFStrLenC("\nendstream endobj\n");

  if (!UpdateOffsets(lOff, lAddedSize))
    return false_a;
  m_PdfObjOffMap[m_lLastObj] = lOff;
  m_lStartLayerContObj = m_lLastObj;
  m_lLastObj++;
  return true_a;
}

bool_a CPDFTemplate::WriteEndLayerContent()
{
  if (m_lEndLayerContObj > 0)
    return true_a;
  if (!m_pLayer)
    return false_a;
  long lOff = GetNewObjOffet();
  char czTemp[200];
  long lAddedSize = 0;

  string strLayerCont("\nEMC\n");

  //if (!CPDFDocument::CompressStream(strLayerCont))
  //  return false_a;

  sprintf(czTemp, "%ld 0 obj << /Length %ld >>\nstream\n",
            m_lLastObj, (long)strLayerCont.size());
  m_strRawPDF.insert(lOff, czTemp);
  lAddedSize = PDFStrLenC(czTemp);

  m_strRawPDF.insert(lOff + lAddedSize, strLayerCont);
  lAddedSize += strLayerCont.size();

  m_strRawPDF.insert(lOff + lAddedSize, "\nendstream endobj\n");
  lAddedSize += PDFStrLenC("\nendstream endobj\n");

  if (!UpdateOffsets(lOff, lAddedSize))
    return false_a;
  m_PdfObjOffMap[m_lLastObj] = lOff;
  m_lEndLayerContObj = m_lLastObj;
  m_lLastObj++;
  return true_a;
}

bool_a CPDFTemplate::AddFontToTemplate(CPDFBaseFont *i_pFont)
{
  if (!i_pFont)
    return false_a;

  long lTemp;
  size_t Off;
  vector<string> vctStreams;

  //if (i_pFont->UsedInForm())
  //  i_pFont->SetEmbedded();

  //lTemp = m_lLastObj;
  //i_pFont->AssignObjNum(lTemp);
  if (!i_pFont->GetEntireFontStream(vctStreams))
    return false_a;

  for(size_t ii = 0; ii < vctStreams.size(); ii++)
  {
    Off = GetNewObjOffet();
    m_strRawPDF.insert(Off, vctStreams[ii]);
    if (!UpdateOffsets(Off, vctStreams[ii].size()))
      return false_a;
    lTemp = atol(vctStreams[ii].c_str());//TODO: make it better
    m_PdfObjOffMap[lTemp/*m_lLastObj*/] = Off;
    //m_lLastObj++;
  }

  //if (i_pFont->UsedInForm())
  //{
  //  string strObj;
  //  lTemp = i_pFont->GetFontObjNum();
  //  i_pFont->SetFontObjNum(m_lLastObj++);
  //  i_pFont->GetFontStreamWinEncoding(strObj);
  //  i_pFont->SetFontObjNum(lTemp);
  //  Off = GetNewObjOffet();
  //  m_strRawPDF.insert(Off, strObj);
  //  if (!UpdateOffsets(Off, strObj.size()))
  //    return false_a;
  //  m_PdfObjOffMap[m_lLastObj - 1] = Off;

  //  if (!AddFontToDR(i_pFont->GetFontResName(), m_lLastObj - 1))
  //    return false_a;

  //}

  return true_a;
}

bool_a CPDFTemplate::AssignTemplateObjNum(CPDFBaseFont *i_pFont)
{
  if (!i_pFont)
    return false_a;

  //long lTemp;
  //size_t Off;
  vector<string> vctStreams;

  if (i_pFont->UsedInForm())
    i_pFont->SetEmbedded();

  //lTemp = m_lLastObj;
  i_pFont->AssignObjNum(m_lLastObj/*lTemp*/);
  //if (!i_pFont->GetEntireFontStream(vctStreams))
  //  return false_a;

  //for(size_t ii = 0; ii < vctStreams.size(); ii++)
  //{
  //  Off = GetNewObjOffet();
  //  m_strRawPDF.insert(Off, vctStreams[ii]);
  //  if (!UpdateOffsets(Off, vctStreams[ii].size()))
  //    return false_a;
  //  m_PdfObjOffMap[m_lLastObj] = Off;
  //  m_lLastObj++;
  //}

  if (i_pFont->UsedInForm())
  {
    //string strObj;
    //lTemp = i_pFont->GetFontObjNum();
    //i_pFont->SetFontObjNum(m_lLastObj++);
    //i_pFont->GetFontStreamWinEncoding(strObj);
    //i_pFont->SetFontObjNum(lTemp);
    //Off = GetNewObjOffet();
    //m_strRawPDF.insert(Off, strObj);
    //if (!UpdateOffsets(Off, strObj.size()))
    //  return false_a;
    //m_PdfObjOffMap[m_lLastObj - 1] = Off;

    if (!AddFontToDR(i_pFont->GetFontResName(), i_pFont->GetFontObjNum()/*m_lLastObj - 1*/))
      return false_a;

  }

  return true_a;
}

bool_a CPDFTemplate::AddFontToPage(long i_lPage, CPDFBaseFont *i_pFont)
{
  string strTemp;
  long lSize, lTemp;
  size_t str, pos;
  char czTemp[200];
  size_t index, Off;
  map <long, size_t>::iterator iter1;

  if (i_lPage <= 0 || !i_pFont || i_lPage > (long)m_PDFPages.size())
    return false_a;

  Off = m_PDFPages[i_lPage - 1].ObjOffset;
  sprintf(czTemp, "/%s %ld 0 R ", i_pFont->GetFontResName(), i_pFont->GetFontObjNum());
  lSize = PDFStrLenC(czTemp);

  strTemp = ReadDict(Off);
  if (strTemp.empty())
    return false_a;
  str = PDFStrStrKey(strTemp, czRESOURCES, 0);
  if (str == string::npos)
    return false_a;
  lTemp = PDFGetNumValue(strTemp, czRESOURCES);
  if (lTemp != NOT_USED) // is indirect object?
  {
    iter1 = m_PdfObjOffMap.find(lTemp);
    if (iter1 == m_PdfObjOffMap.end())
      return false_a;
    Off = m_PdfObjOffMap[lTemp];
    strTemp = ReadDict(Off);
    str = 0;
    if (strTemp.empty())
      return false_a;
  }

  pos = PDFStrStrKey(strTemp, czFONT, str);
  if (pos == string::npos)
  {
    sprintf(czTemp, " /Font << /%s %ld 0 R >> ", i_pFont->GetFontResName(), i_pFont->GetFontObjNum());
    lSize = PDFStrLenC(czTemp);
    pos = PDFStrStrKey(strTemp, czBGNDIC, str);
    if (pos == string::npos)
      return false_a;
    pos += PDFStrLenC(czBGNDIC);
  }
  else
  {
    lTemp = PDFGetNumValue(strTemp, czFONT);
    if (lTemp != NOT_USED) // is indirect object?
    {
      iter1 = m_PdfObjOffMap.find(lTemp);
      if (iter1 == m_PdfObjOffMap.end())
        return false_a;
      Off = m_PdfObjOffMap[lTemp];
      strTemp = ReadDict(Off);
      if (strTemp.empty())
        return false_a;
      pos = 0;
    }
    pos = PDFStrStrKey(strTemp, czENDDIC, pos);
    if (pos == string::npos)
      return false_a;
  }

  lSize = PDFStrLenC(czTemp);
  index = Off + pos;
  m_strRawPDF.insert(index, czTemp, lSize);
  index += lSize;
  if (!UpdateOffsets(index, lSize))
    return false_a;

  return true_a;
}

bool_a CPDFTemplate::AddImageToTemplate(CPDFImage *i_pImg)
{
  if (!i_pImg)
    return false_a;

  long lTemp;
  size_t Off;
  vector<string> vctStreams;

  lTemp = m_lLastObj;
  i_pImg->AssignObjNum(lTemp);
  if (!i_pImg->GetEntireImageStream(vctStreams))
    return false_a;

  for(size_t ii = 0; ii < vctStreams.size(); ii++)
  {
    Off = GetNewObjOffet();
    m_strRawPDF.insert(Off, vctStreams[ii]);
    if (!UpdateOffsets(Off, vctStreams[ii].size()))
      return false_a;
    m_PdfObjOffMap[m_lLastObj] = Off;
    m_lLastObj++;
  }

  return true_a;
}

bool_a CPDFTemplate::AddImageToPage(long i_lPage, CPDFImage *i_pImg)
{
  string strTemp;
  size_t str, pos;
  long lSize, lTemp;
  char czTemp[200];
  size_t index, Off;
  map <long, size_t>::iterator iter1;

  if (i_lPage <= 0 || !i_pImg || i_lPage > (long)m_PDFPages.size())
    return false_a;

  Off = m_PDFPages[i_lPage - 1].ObjOffset;
  sprintf(czTemp, "/%s %ld 0 R ", i_pImg->GetImageResName(), i_pImg->GetImageObjNum());
  lSize = PDFStrLenC(czTemp);

  strTemp = ReadDict(Off);
  if (strTemp.empty())
    return false_a;
  str = PDFStrStrKey(strTemp, czRESOURCES, 0);
  if (str == string::npos)
    return false_a;
  lTemp = PDFGetNumValue(strTemp, czRESOURCES);
  if (lTemp != NOT_USED) // is indirect object?
  {
    iter1 = m_PdfObjOffMap.find(lTemp);
    if (iter1 == m_PdfObjOffMap.end())
      return false_a;
    Off = m_PdfObjOffMap[lTemp];
    strTemp = ReadDict(Off);
    str = 0;
    if (strTemp.empty())
      return false_a;
  }

  pos = PDFStrStrKey(strTemp, "/XObject", 0);
  if (pos == string::npos)
  {
    sprintf(czTemp, " /XObject << /%s %ld 0 R >> ", i_pImg->GetImageResName(), i_pImg->GetImageObjNum());
    lSize = PDFStrLenC(czTemp);
    pos = PDFStrStrKey(strTemp, czBGNDIC, str);
    if (pos == string::npos)
      return false_a;
    pos += PDFStrLenC(czBGNDIC);
  }
  else
  {
    lTemp = PDFGetNumValue(strTemp, czFONT);
    if (lTemp != NOT_USED) // is indirect object?
    {
      iter1 = m_PdfObjOffMap.find(lTemp);
      if (iter1 == m_PdfObjOffMap.end())
        return false_a;
      Off = m_PdfObjOffMap[lTemp];
      strTemp = ReadDict(Off);
      if (strTemp.empty())
        return false_a;
      pos = 0;
    }
    pos = PDFStrStrKey(strTemp, czENDDIC, pos);
    if (pos == string::npos)
      return false_a;
  }

  lSize = PDFStrLenC(czTemp);
  index = Off + pos;
  m_strRawPDF.insert(index, czTemp, lSize);
  index += lSize;
  if (!UpdateOffsets(index, lSize))
    return false_a;

  return true_a;
}

bool_a CPDFTemplate::AddFormToTemplate(CPDFAnnotation *i_pAnnot)
{
  if (!i_pAnnot)
    return false_a;

  long lTemp;
  size_t Off;
  vector<string> vctStreams;

  lTemp = m_lLastObj;
  i_pAnnot->AssignObjNum(lTemp);
  if (!i_pAnnot->GetEntireFormStream(vctStreams))
    return false_a;

  for(size_t ii = 0; ii < vctStreams.size(); ii++)
  {
    Off = GetNewObjOffet();
    m_strRawPDF.insert(Off, vctStreams[ii]);
    if (!UpdateOffsets(Off, vctStreams[ii].size()))
      return false_a;
    m_PdfObjOffMap[m_lLastObj] = Off;
    m_lLastObj++;
  }

  if (i_pAnnot->GetAnnotationType() == eAnnotWidget)
  {
    if (!AddFormToPage(i_pAnnot->GetPage(), i_pAnnot))
      return false_a;
  }

  return true_a;
}

bool_a CPDFTemplate::AddFormToPage(long i_lPage, CPDFAnnotation *i_pAnnot)
{
  char czTemp[200];

  if (i_lPage <= 0 || !i_pAnnot || i_lPage > (long)m_PDFPages.size())
    return false_a;

  sprintf(czTemp, "%ld 0 R", i_pAnnot->GetFormObjNum());

  if (!AddAnnotsToPage(i_lPage, czTemp))
    return false_a;

  return true_a;
}

bool_a CPDFTemplate::AddLayerToPage(long i_lPage, CPDFLayer *i_pLayer)
{
  long lSize, lObj;
  char czTemp[200];
  size_t index, index1, Off, OldSize;
  string strObj, strTemp;
  if (i_lPage <= 0 || !i_pLayer || i_lPage > (long)m_PDFPages.size())
    return false_a;


  // check whether layer already exists
  string strPath = "/Resources/Properties/";
  strPath.append(i_pLayer->GetLayerResName());
  if (GetObjAndPosForKey(m_PDFPages[i_lPage - 1].lObjIndex, strPath, lObj, index))//returns true if found
    return true_a;



  Off = m_PDFPages[i_lPage - 1].ObjOffset;
  sprintf(czTemp, "/%s %ld 0 R ", i_pLayer->GetLayerResName(), i_pLayer->GetObjNum());
  lSize = PDFStrLenC(czTemp);

  if (!ReadObject(Off, strObj))
    return false_a;

  index = CPDFDocument::PDFStrStrKey(strObj, czRESOURCES);
  if (-1 == index)
  {
    OldSize = strObj.size();
    index = strObj.rfind(">>");
    if (-1 == index)
      return false_a;
    strObj.insert(index, " >> ");
    strObj.insert(index, " >> ");
    strObj.insert(index, czTemp);
    strObj.insert(index, " /Properties << ");
    strObj.insert(index, " /Resources << ");

    if (!UpdateOffsets(Off + 1, strObj.size() - OldSize))
      return false_a;
    m_strRawPDF.erase(Off, OldSize);
    m_strRawPDF.insert(Off, strObj);
    return true_a;
  }

  strPath = czRESOURCES;
  if (!GetObjAndPosForKey(m_PDFPages[i_lPage - 1].lObjIndex, strPath, lObj, index))
    return true_a;

  Off = GetObjOffset(lObj);
  if (!ReadObject(Off, strObj))
    return false_a;

  index1 = CPDFDocument::PDFStrStrKey(strObj, index, "/Properties");
  if (-1 == index1)
  {
    if (0 == index)//if it is whole object, take end
    {
      index = strObj.rfind(">>");
    } else {
      index += PDFStrLenC(czRESOURCES);
      if (!ReadItem(strObj, index, "<<", ">>", false_a, strTemp))
        return false_a;
      if (strTemp.empty())
        return false_a;
      index = strObj.find(strTemp, index); //because of possible white spaces
      index += strTemp.size();
    }

    if (-1 == index)
      return false_a;

    OldSize = strObj.size();
    strObj.insert(index, " >> ");
    strObj.insert(index, czTemp);
    strObj.insert(index, " /Properties << ");

    if (!UpdateOffsets(Off + 1, strObj.size() - OldSize))
      return false_a;
    m_strRawPDF.erase(Off, OldSize);
    m_strRawPDF.insert(Off, strObj);
    return true_a;
  }

  strPath = "/Resources/Properties";
  if (!GetObjAndPosForKey(m_PDFPages[i_lPage - 1].lObjIndex, strPath, lObj, index))
    return true_a;

  Off = GetObjOffset(lObj);
  if (!ReadObject(Off, strObj))
    return false_a;

  OldSize = strObj.size();

  index = strObj.find("<<", index);
  if (-1 == index)
    return false_a;
  index++;
  index++;

  strObj.insert(index, czTemp);
  strObj.insert(index, " ");
  if (!UpdateOffsets(Off + 1, strObj.size() - OldSize))
    return false_a;
  m_strRawPDF.erase(Off, OldSize);
  m_strRawPDF.insert(Off, strObj);


  return true_a;
}

bool_a CPDFTemplate::AddLayerToTemplate(CPDFLayer *i_pLayer)
{
  if (!i_pLayer)
    return false_a;

  size_t Off;
  string strStream;

  i_pLayer->SetObjNum(m_lLastObj);
  if (!i_pLayer->GetLayerStream(strStream))
    return false_a;

  Off = GetNewObjOffet();
  m_strRawPDF.insert(Off, strStream);
  if (!UpdateOffsets(Off, strStream.size()))
    return false_a;
  m_PdfObjOffMap[m_lLastObj] = Off;
  m_lLastObj++;

  return true_a;
}

bool_a CPDFTemplate::AddLayersToCatalog(string& i_OCstream)
{
  if (i_OCstream.empty())
    return true_a;
  long lObj;
  size_t index = string::npos, Off, OldSize;
  string strTemp, strObj;

  if (!ReadObject(m_RootOffset, strObj))
    return false_a;

  ReadItem(strObj, "/OCProperties", "<<", ">>", true_a, strTemp);
  if (strTemp.empty())
  {
    OldSize = strObj.size();

    index = strObj.rfind(">>");
    if (-1 == index)
      return false_a;

    strObj.insert(index, " >> ");
    strObj.insert(index, i_OCstream);
    strObj.insert(index, " /OCProperties << ");

    if (!UpdateOffsets(m_RootOffset + 1, strObj.size() - OldSize))
      return false_a;
    m_strRawPDF.erase(m_RootOffset, OldSize);
    m_strRawPDF.insert(m_RootOffset, strObj);
    return true_a;
  }

  string strPath;
  //append OCGs
  ReadItem(i_OCstream, "/OCGs", "[", "]", true_a, strTemp);
  if (!strTemp.empty())
  {
    strPath = "/OCProperties/OCGs";
    if (!GetObjAndPosForKey(m_lRootObj, strPath, lObj, index))
      return true_a;

    Off = GetObjOffset(lObj);
    if (!ReadObject(Off, strObj))
      return false_a;

    OldSize = strObj.size();

    index = strObj.find("[", index);
    if (-1 == index)
      return false_a;
    index++;

    strObj.insert(index, " ");
    strObj.insert(index, strTemp);
    if (!UpdateOffsets(Off + 1, strObj.size() - OldSize))
      return false_a;
    m_strRawPDF.erase(Off, OldSize);
    m_strRawPDF.insert(Off, strObj);
  }

  return true_a;
}

void CPDFTemplate::SetLayer(CPDFLayer *i_pLayer)
{
  m_pLayer = i_pLayer;
}

bool_a CPDFTemplate::ApplyLayer()
{
  if (!m_pLayer)
    return true_a;

  long i, lSize = (long)m_PDFPages.size();
  char czTemp[200];

  if (!WriteStartLayerContent())
    return false_a;
  if (!WriteEndLayerContent())
    return false_a;

  for (i = 0; i < lSize; i++)
  {
    //if (!AddLayerToPage(i + 1, m_pLayer))
    //  return false_a;


    sprintf(czTemp, "%ld 0 R", m_lStartLayerContObj);
    if (!AddNewContentRefToPage(i + 1, czTemp, false_a, true_a))
      return false_a;

    sprintf(czTemp, "%ld 0 R", m_lEndLayerContObj);
    if (!AddNewContentRefToPage(i + 1, czTemp, false_a, false_a))
      return false_a;
  }

  return true_a;
}

bool_a CPDFTemplate::AddPDFTemplate(CPDFTemplate *i_pTemplatePDF, long i_lPageCount)
{
  if (!i_pTemplatePDF)
    return false_a;
  long lOff, lObj, lGen, i, lRObj;
  char *pObj, czTemp[100];
  string strObj, strContent, strRes;
  basic_string <char>::size_type index, indexSave;
  map <long, size_t>::iterator iter;

  if (!WriteSaveContent())
    return false_a;
  if (!WriteRestoreContent())
    return false_a;
  if (!i_pTemplatePDF->CheckFieldsID(this))
    return false_a;


  //Field_iterator iterField;
  //for (iterField = m_PdfFieldObj.begin(); iterField != m_PdfFieldObj.end(); iterField++)
  //{
  //  CPDFField *pField = (*iterField).second;
  //  if (!pField)
  //    continue;
  //  const char *pFieldId = pField->m_strID.c_str();
  //  long lTemp = GetObjOffset(pField->m_lObjNum);

  //  if (!ReadObject(lTemp, strObj))
  //    return false_a;
  //  pFieldId = pFieldId;
  //}


  for (i = 1; i < (long)i_pTemplatePDF->m_PdfObjOffMap.size(); i++)
  {
    strObj.clear();
    lOff = i_pTemplatePDF->GetObjOffset(i);
    if (lOff > 0)
    {
      if (!i_pTemplatePDF->ReadObject(lOff, strObj))
        return false_a;
      strObj.append("\n");
    }

    if (strObj.empty())
    {
      sprintf(czTemp, "%ld 0 obj\nendobj\n", i);
      strObj = czTemp;
    }

    index = 0;

    if (strObj[index] < '0' || strObj[index] > '9')
       return false_a;
    lObj = atol(strObj.c_str());

    while ((strObj[index] >= '0' && strObj[index] <= '9'))
       strObj.erase(index, 1);
    while (strObj[index] == PDF_SPACE_CHR || strObj[index] == PDF_TAB_CHR ||
            strObj[index] == PDF_NL_CHR1 || strObj[index] == PDF_NL_CHR2)
      strObj.erase(index, 1);
    if (strObj[index] < '0' || strObj[index] > '9')
      return false_a;
    while (strObj[index] >= '0' && strObj[index] <= '9')
      strObj.erase(index, 1);

    sprintf(czTemp, "%ld 0", m_lLastObj + lObj - 1);
    strObj.insert(index, czTemp);

    index = 0;
    do
    {
      index = strObj.find(" R", index);
      if (index == -1)
        break;

      indexSave = strObj.length() - (index + 2);
      index--;

      if (strObj[index] < '0' || strObj[index] > '9')
      {
        index = strObj.length() - indexSave;
        continue;
      }

      while ((strObj[index] >= '0' && strObj[index] <= '9'))
        index--;
      lGen = atol(strObj.c_str() + index);

      if (strObj[index] != PDF_SPACE_CHR)
      {
        index = strObj.length() - indexSave;
        continue;
      }
      index--;

      if (strObj[index] < '0' || strObj[index] > '9')
      {
        index = strObj.length() - indexSave;
        continue;
      }

      while ((strObj[index] >= '0' && strObj[index] <= '9'))
        index--;
      index++;
      lObj = atol(strObj.c_str() + index);

      if (!ReplaceRefValue(strObj, index, m_lLastObj + lObj - 1, 0))
        return false_a;

      index = strObj.length() - indexSave;
    } while (index != -1);

    pObj = PDFGetNameValue(strObj, "/Type");
    if (pObj)
    {
      if (!strcmp(pObj, "/Page"))
      {
        if (!ReplaceRefValue(strObj, "/Parent", NULL, m_lPagesObj, 0)) //TODO: read gen. num. value
        {
          free(pObj);
          return false_a;
        }
      }
      free(pObj);
    }

    lOff = GetNewObjOffet();
    if (!UpdateOffsets(lOff, strObj.length()))
      return false_a;
    m_strRawPDF.insert(lOff, strObj);

    lObj = atol(strObj.c_str());

    iter = m_PdfObjOffMap.find(lObj);
    if (iter != m_PdfObjOffMap.end())
    {
      lGen = m_PdfObjOffMap[lObj];
    }

    m_PdfObjOffMap[lObj] = lOff;
  }

  map<long, CPDFBaseFont *>::iterator iterFont;
  for (iterFont = i_pTemplatePDF->m_mapFont.begin(); iterFont != i_pTemplatePDF->m_mapFont.end(); iterFont++)
  {
    CPDFBaseFont *pFont = (CPDFBaseFont *)iterFont->second;
    pFont->SetFontObjNum(pFont->GetFontObjNum() + m_lLastObj - 1);
  }

  lRObj = m_lLastObj + i_pTemplatePDF->m_PdfObjOffMap.size() - 1;

  //Acro
  lOff = GetObjOffset(m_lLastObj + i_pTemplatePDF->m_lRootObj - 1);
  if (!ReadObject(lOff, strObj))
    return false_a;
  if (ReadItem(strObj, czACROFORM, "<<", ">>", true_a, strRes))
  {
    if (!strRes.empty())
      if (!ProcessAcroForm(strRes, lRObj))
        return false_a;
  }

  for (i = 0; i < (long)i_pTemplatePDF->m_PDFPages.size(); i++)
  {
    if (!i_pTemplatePDF->GetPDFPosition())
    {
      PDFTempPage page;

      if (!AddNewPageRefToPages(m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1))
        return false_a;

      lOff = GetObjOffset(m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1);

      page.lObjIndex = m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1;
      page.ObjOffset = lOff;
      m_PDFPages[(long)m_PDFPages.size()] = page;
    }
    else if (i_pTemplatePDF->GetPDFPosition() > 0)
    {
      if (i + i_pTemplatePDF->GetPDFPosition() - 1 < m_PDFPages.size())
      {
        if (!AddToPage(i + i_pTemplatePDF->GetPDFPosition(), m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1))
          return false_a;
      }
      else
      {
        PDFTempPage page;

        if (!AddNewPageRefToPages(m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1))
          return false_a;

        lOff = GetObjOffset(m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1);

        page.lObjIndex = m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1;
        page.ObjOffset = lOff;
        m_PDFPages[(long)m_PDFPages.size()] = page;
      }
    }
    else
    {
      for (long j = 0; j < i_lPageCount; j++)
      {
        if (j < m_PDFPages.size())
        {
          if (!AddToPage(j + 1, m_lLastObj + i_pTemplatePDF->m_PDFPages[0].lObjIndex - 1))
            return false_a;
        }
        else
        {
          PDFTempPage page;

          if (!AddNewPageRefToPages(m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1))
            return false_a;

          lOff = GetObjOffset(m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1);

          page.lObjIndex = m_lLastObj + i_pTemplatePDF->m_PDFPages[i].lObjIndex - 1;
          page.ObjOffset = lOff;
          m_PDFPages[(long)m_PDFPages.size()] = page;
        }
      }
      break;
    }

  }

  i_pTemplatePDF->m_lShiftObj += m_lLastObj - 1;
  m_lLastObj = lRObj;

  return true_a;
}

bool_a CPDFTemplate::AddToPage(long i_lPage, long i_lOff)
{
  string strObj, strContent, strRes, strTemp;
  long lObj, lGen;
  char czTemp[100];

  i_lOff = GetObjOffset(i_lOff);
  if (i_lOff <= 0)
    return false_a;

  if (!ReadObject(i_lOff, strObj))
    return false_a;
  //Content
  if (!ReadItem(strObj, czCONTENTS, "[", "]", true_a, strContent))
  {
    lObj = PDFGetObjValue(strObj, czCONTENTS, lGen);
    if (lObj < 0 || lGen < 0)
      return false_a;
    sprintf(czTemp, "%ld %ld R", lObj, lGen );
    strContent = czTemp;
  }
  if (!AddNewContentRefToPage(i_lPage, strContent.c_str()))
    return false_a;
  //Resources
  if (!ReadItem(strObj, czRESOURCES, "<<", ">>", true_a, strRes))
    return false_a;
  if (!ProcessContentRes(strRes, strContent))
    return false_a;
  if (!AddResourcesToPage(i_lPage, strRes))
    return false_a;
  //Annots
  strTemp.erase();
  if (!ReadItem(strObj, czANNOTS, "[", "]", true_a, strTemp))
  {
    lObj = PDFGetObjValue(strObj, czANNOTS, lGen);
    if (lObj > 0 && lGen >= 0)
    {
      sprintf(czTemp, "%ld %ld R", lObj, lGen );
      strTemp = czTemp;
    }
  }
  if (!strTemp.empty())
  {
    if (!AddAnnotsToPage(i_lPage, strTemp))
      return false_a;
  }
  return true_a;
}

//bool_a CPDFTemplate::UpdateFontRef(const char *i_pFontName, long i_lObj)
//{
//  string strTemp, strObj;
//  long lOff, lObjACRO, lObjDR, lObjFont, lGen;
//  size_t lSize;
//  char *pObj;
//
//  if (!ReadObject(m_RootOffset, strObj))
//    return false_a;
//  lOff = m_RootOffset;
//
//  if (!ReadItem(strObj, czACROFORM, "<<", ">>", true_a, strTemp))
//     return false_a;
//
//  if (strTemp.empty())
//    return true_a;
//
//  lObjACRO = PDFGetObjValue(strObj, czACROFORM, lGen);
//
//  if (!ReadItem(strTemp, "/DR", "<<", ">>", true_a, strObj))
//    return false_a;
//
//  lObjDR = PDFGetObjValue(strTemp, "/DR", lGen);
//
//  if (!ReadItem(strObj, "/Font", "<<", ">>", true_a, strTemp))
//    return false_a;
//
//  lObjFont = PDFGetObjValue(strObj, "/Font", lGen);
//
//  lObjFont = lObjFont > 0 ? lObjFont : lObjDR;
//  lObjFont = lObjFont > 0 ? lObjFont : lObjACRO;
//  lOff = GetObjOffset(lObjFont);
//  if (lOff <= 0)
//    return false_a;
//  if (!ReadObject(lOff, strObj))
//    return false_a;
//
//  lSize = strObj.length();
//
//  if (!ReplaceRefValue(strObj, i_pFontName, NULL, i_lObj, 0))
//    return false_a;
//
//  pObj = m_RawPDF.pStream + lOff;
//  if (lSize > strObj.size())
//  {
//    memset(pObj, ' ', lSize);
//  }
//  else if (lSize < strObj.size())
//  {
//    if (!MoveStream(&pObj, strObj.size() - lSize))
//      return false_a;
//    if (!UpdateOffsets(lOff + lSize, strObj.size() - lSize))
//      return false_a;
//  }
//  memcpy(pObj, strObj.c_str(), strObj.size());
//
//  return true_a;
//
//}

void CPDFTemplate::Clean()
{
  if (m_pRootDic)
  {
    delete m_pRootDic;
    m_pRootDic = NULL;
  }
  if (m_pEncryptDic)
  {
    delete m_pEncryptDic;
    m_pEncryptDic = NULL;
  }
  m_PdfXrefMap.clear();
  m_PdfObjOffMap.clear();
  if (m_pFileName != NULL)
    free(m_pFileName);
  m_RootOffset = string::npos;
  m_lRootObj = NOT_USED;
  m_PagesOffset = string::npos;
  m_lPagesObj = NOT_USED;
  m_lLinearObj = NOT_USED;
  m_lLastObj = NOT_USED;
  m_lLinearL = NOT_USED;
  m_LinearE = string::npos;
  m_LinearT = string::npos;
  m_LinearH[0] = string::npos;
  m_LinearH[1] = string::npos;
  m_LinearH[2] = string::npos;
  m_LinearH[3] = string::npos;
  Field_iterator iter;
  CPDFField *pField;
  char *pFirst;
  for (iter = m_PdfFieldObj.begin(); iter != m_PdfFieldObj.end(); iter++)
  {
    pField = (*iter).second;
    if (this != pField->m_pTemplate)
      continue;
    delete pField;
  }
  m_PdfFieldObj.clear();

  multimap <const char *, const char *, str_comp_greater >::iterator iter1;
  for (iter1 = m_PdfFieldNames.begin(); iter1 != m_PdfFieldNames.end(); iter1++)
  {
    pFirst = (char *)(*iter1).first;
    free(pFirst);
    pFirst = (char *)(*iter1).second;
    free(pFirst);
  }
  m_PdfFieldNames.clear();

  CPDFObjects *pPDFObj = NULL;
  map<long, PDFTempPage>::iterator iter2;
  std::multimap<std::string, CPDFXObject *>::iterator iterXObject;
  PDFObjectsMap::iterator iter3;
  for (iter2 = m_PDFPages.begin(); iter2 != m_PDFPages.end(); iter2++)
  {
    for (iter3 = iter2->second.m_mapObjects.begin(); iter3 != iter2->second.m_mapObjects.end(); iter3++)
    {
      pPDFObj = (CPDFObjects *)iter3->second;
      if (pPDFObj)
        delete pPDFObj;
    }
    iter2->second.m_mapObjects.clear();

    for (iterXObject = iter2->second.m_pXObjects.begin(); iterXObject != iter2->second.m_pXObjects.end(); iterXObject++)
    {
      CPDFXObject *pXObject = (CPDFXObject *)iterXObject->second;
      if (pXObject)
        delete pXObject;
    }
  }
  m_PDFPages.clear();

  map<long, CPDFBaseFont *>::iterator iterFont;
  for (iterFont = m_mapFont.begin(); iterFont != m_mapFont.end(); iterFont++)
  {
    CPDFBaseFont *pFont = (CPDFBaseFont *)iterFont->second;
    delete pFont;
  }
  m_mapFont.clear();


}

bool_a CPDFTemplate::ReplaceNumValue(string &io_strObj, const char *i_pKey, const char *i_pFKey, long i_lValue)
{
  if (io_strObj.empty() || !i_pKey)
    return false_a;
  char czTemp[200];
  basic_string <char>::size_type index;

  sprintf(czTemp, "%ld", i_lValue);
  index = CPDFDocument::PDFStrStrNumKey(io_strObj, i_pKey);
  if (index == -1)
  {
    if (!i_pFKey)
      return false_a;
    index = CPDFDocument::PDFStrStrNumKey(io_strObj, i_pFKey);
    if (index == -1)
      return false_a;

    index -= PDFStrLenC(i_pFKey);
    io_strObj.insert(index, i_pKey);
    index += PDFStrLenC(i_pKey);
    io_strObj.insert(index, " ");
    index++;
    io_strObj.insert(index, czTemp);
    index += PDFStrLenC(czTemp);
    io_strObj.insert(index, "\n");
    return true_a;
  }

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    index++;
  if (io_strObj[index] < '0' || io_strObj[index] > '9')
    return false_a;
  while ((io_strObj[index] >= '0' && io_strObj[index] <= '9'))
    io_strObj.erase(index, 1);
  io_strObj.insert(index, czTemp);
  return true_a;
}

bool_a CPDFTemplate::ReplaceRefValue(string &io_strObj, const char *i_pKey, const char *i_pFKey,
                                     long i_lObjNum, long i_lGenNum)
{
  if (io_strObj.empty() || !i_pKey)
    return false_a;
  char czTemp[200];
  basic_string <char>::size_type index;

  sprintf(czTemp, "%ld", i_lObjNum);
  index = io_strObj.find(i_pKey);
  if (index == -1)
  {
    if (!i_pFKey)
      return true_a;
    index = 0;
    while (true_a)
    {
      index = io_strObj.find(i_pFKey, index);
      if (index == -1)
        return false_a;
      index += PDFStrLenC(i_pFKey);
      if (!IsDelimiter(io_strObj[index]) && !IsWhiteSpace(io_strObj[index]))
        continue;
      index -= PDFStrLenC(i_pFKey);
      io_strObj.insert(index, i_pKey);
      index += PDFStrLenC(i_pKey);
      io_strObj.insert(index, " ");
      index++;
      io_strObj.insert(index, czTemp);
      index += PDFStrLenC(czTemp);
      io_strObj.insert(index, " ");
      index++;
      sprintf(czTemp, "%ld", i_lGenNum);
      io_strObj.insert(index, czTemp);
      index += PDFStrLenC(czTemp);
      io_strObj.insert(index, " R\n");
      return true_a;
    }
  }
  index += PDFStrLenC(i_pKey);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    index++;
  if (io_strObj[index] < '0' || io_strObj[index] > '9')
    return false_a;
  while ((io_strObj[index] >= '0' && io_strObj[index] <= '9'))
    io_strObj.erase(index, 1);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    io_strObj.erase(index, 1);
  if (io_strObj[index] < '0' || io_strObj[index] > '9')
    return false_a;
  while (io_strObj[index] >= '0' && io_strObj[index] <= '9')
    io_strObj.erase(index, 1);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    io_strObj.erase(index, 1);
  if (io_strObj[index] != 'R')
    return false_a;
  io_strObj.erase(index, 1);

  io_strObj.insert(index, czTemp);
  index += PDFStrLenC(czTemp);
  io_strObj.insert(index, " ");
  index++;
  sprintf(czTemp, "%ld", i_lGenNum);
  io_strObj.insert(index, czTemp);
  index += PDFStrLenC(czTemp);
  io_strObj.insert(index, " R");
  return true_a;
}

bool_a CPDFTemplate::ReplaceRefValue(string &io_strObj, size_t i_Off, long i_lObjNum, long i_lGenNum)
{
  if (io_strObj.empty() || i_Off < 0 || io_strObj.length() <= i_Off)
    return false_a;
  char czTemp[200];
  basic_string <char>::size_type index;

  sprintf(czTemp, "%ld %ld R", i_lObjNum, i_lGenNum);
  index = i_Off;

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    index++;
  if (io_strObj[index] < '0' || io_strObj[index] > '9')
    return false_a;
  while ((io_strObj[index] >= '0' && io_strObj[index] <= '9'))
    io_strObj.erase(index, 1);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    io_strObj.erase(index, 1);
  if (io_strObj[index] < '0' || io_strObj[index] > '9')
    return false_a;
  while (io_strObj[index] >= '0' && io_strObj[index] <= '9')
    io_strObj.erase(index, 1);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    io_strObj.erase(index, 1);
  if (io_strObj[index] != 'R')
    return false_a;
  io_strObj.erase(index, 1);

  io_strObj.insert(index, czTemp);
  index += PDFStrLenC(czTemp);

  return true_a;
}

bool_a CPDFTemplate::ReplaceTextValue(string &io_strObj, const char *i_pKey, const char *i_pFKey,
                                      const char *i_pValue, long i_lSize)
{
  if (io_strObj.empty() || !i_pKey || !i_pValue)
    return false_a;
  if (i_lSize < 0)
    i_lSize = PDFStrLenC(i_pValue);
  basic_string <char>::size_type index = 0, index1;

while (true_a)
{
  index = io_strObj.find(i_pKey, index);
  if (index == -1)
  {
    if (!i_pFKey)
      return true_a;
    index = 0;
    while (true_a)
    {
      index = io_strObj.find(i_pFKey, index);
      if (index == -1)
        return false_a;
      index += PDFStrLenC(i_pFKey);
      if (!IsDelimiter(io_strObj[index]) && !IsWhiteSpace(io_strObj[index]))
        continue;
      index -= PDFStrLenC(i_pFKey);
      io_strObj.insert(index, i_pKey);
      index += PDFStrLenC(i_pKey);
      io_strObj.insert(index, " (");
      index += 2;
      io_strObj.insert(index, i_pValue, i_lSize);
      index += i_lSize;//PDFStrLenC(i_pValue);
      io_strObj.insert(index, ")\n");
      return true_a;
    }
  }
  index += PDFStrLenC(i_pKey);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    index++;
  if (io_strObj[index] == '(')
  {
    break;
  }
}
if (index == -1)
  return false_a;

  index++;
  index1 = index;
  while (index1 != -1)
  {
    index1 = io_strObj.find(")", index1);
    if (index1 == -1)
      return false_a;
    index1--;
    if (io_strObj[index1] == '\\')
      index1 += 2;
    else
      break;
  }
  io_strObj.erase(index, index1 - index + 1);
  io_strObj.insert(index, i_pValue, i_lSize);

  return true_a;
}

bool_a CPDFTemplate::ReplaceNameValue(string &io_strObj, const char *i_pKey, const char *i_pFKey, const char *i_pValue)
{
  if (io_strObj.empty() || !i_pKey || !i_pValue)
    return false_a;
  basic_string <char>::size_type index;

  index = io_strObj.find(i_pKey);
  if (index == -1)
  {
    if (!i_pFKey)
      return true_a;
    index = 0;
    while (true_a)
    {
      index = io_strObj.find(i_pFKey, index);
      if (index == -1)
        return false_a;
      index += PDFStrLenC(i_pFKey);
      if (!IsDelimiter(io_strObj[index]) && !IsWhiteSpace(io_strObj[index]))
        continue;
      index -= PDFStrLenC(i_pFKey);
      io_strObj.insert(index, i_pKey);
      index += PDFStrLenC(i_pKey);
      io_strObj.insert(index, " ");
      index++;
      io_strObj.insert(index, i_pValue);
      index += PDFStrLenC(i_pValue);
      io_strObj.insert(index, "\n");
      return true_a;
    }
  }
  index += PDFStrLenC(i_pKey);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    index++;
  if (io_strObj[index] != '/')
    return false_a;
  io_strObj.erase(index, 1);

  while (io_strObj[index] != PDF_SPACE_CHR && io_strObj[index] != PDF_TAB_CHR &&
          io_strObj[index] != PDF_NL_CHR1 && io_strObj[index] != PDF_NL_CHR2 &&
          io_strObj[index] != '/' && io_strObj[index] != '>')
    io_strObj.erase(index, 1);
  io_strObj.insert(index, i_pValue);
  return true_a;
}

bool_a CPDFTemplate::ReplaceDictValue(string &io_strObj, const char *i_pKey, const char *i_pFKey, const char *i_pValue)
{
  if (io_strObj.empty() || !i_pKey || !i_pValue)
    return false_a;
  basic_string <char>::size_type index;

  index = io_strObj.find(i_pKey);
  if (index == -1)
  {
    if (!i_pFKey)
      return true_a;
    index = 0;
    while (true_a)
    {
      index = io_strObj.find(i_pFKey, index);
      if (index == -1)
        return false_a;
      index += PDFStrLenC(i_pFKey);
      if (!IsDelimiter(io_strObj[index]) && !IsWhiteSpace(io_strObj[index]))
        continue;
      index -= PDFStrLenC(i_pFKey);
      io_strObj.insert(index, i_pKey);
      index += PDFStrLenC(i_pKey);
      io_strObj.insert(index, " ");
      index++;
      io_strObj.insert(index, i_pValue);
      index += PDFStrLenC(i_pValue);
      io_strObj.insert(index, "\n");
      return true_a;
    }
  }
  return false_a; // TO DO
/*
  index += PDFStrLenC(i_pKey);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    index++;
  if (io_strObj[index] != '<' || io_strObj[index + 1] != '<')
    return false_a;
  index += 2;
  index1 = index;
  while (index1 != -1)
  {
    index1 = io_strObj.find(")", index1);
    if (index1 == -1)
      return false_a;
    index1--;
    if (io_strObj[index1] == '\\')
      index1 += 2;
    else
      break;
  }
  io_strObj.erase(index, index1 - index + 1);
  io_strObj.insert(index, i_pValue);
*/
  return true_a;
}

bool_a CPDFTemplate::ReplaceRGBValue(string &io_strObj, const char *i_pKey, const char *i_pFKey, PdfRgb i_rgb)
{
  if (io_strObj.empty() || !i_pKey)
    return false_a;
  basic_string <char>::size_type index, index1;
  char czBuff[200];

  sprintf(czBuff, "%.4f %.4f %.4f", i_rgb.fR, i_rgb.fG, i_rgb.fB);

  index = io_strObj.find(i_pKey);
  if (index == -1)
  {
    if (!i_pFKey)
      return true_a;
    index = 0;
    while (true_a)
    {
      index = io_strObj.find(i_pFKey, index);
      if (index == -1)
        return false_a;
      index += PDFStrLenC(i_pFKey);
      if (!IsDelimiter(io_strObj[index]) && !IsWhiteSpace(io_strObj[index]))
        continue;
      index -= PDFStrLenC(i_pFKey);
      io_strObj.insert(index, i_pKey);
      index += PDFStrLenC(i_pKey);
      io_strObj.insert(index, " ");
      index++;
      io_strObj.insert(index, czBuff);
      index += PDFStrLenC(czBuff);
      io_strObj.insert(index, "\n");
      return true_a;
    }
  }

  index += PDFStrLenC(i_pKey);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    index++;
  if (io_strObj[index] != '[')
    return false_a;
  index++;

  index1 = io_strObj.find("]", index);
  if (index1 == -1)
    return false_a;
  index1--;

  io_strObj.erase(index, index1 - index + 1);
  io_strObj.insert(index, czBuff);
  return true_a;
}

bool_a CPDFTemplate::ReplaceRectValue(string &io_strObj, const char *i_pKey, const char *i_pFKey, frec_a i_rec)
{
  if (io_strObj.empty() || !i_pKey)
    return false_a;
  basic_string <char>::size_type index, index1;
  char czBuff[250];

  sprintf(czBuff, "%.4f %.4f %.4f %.4f", i_rec.fLeft, i_rec.fBottom, i_rec.fRight, i_rec.fTop);

  index = io_strObj.find(i_pKey);
  if (index == -1)
  {
    if (!i_pFKey)
      return true_a;
    index = 0;
    while (true_a)
    {
      index = io_strObj.find(i_pFKey, index);
      if (index == -1)
        return false_a;
      index += PDFStrLenC(i_pFKey);
      if (!IsDelimiter(io_strObj[index]) && !IsWhiteSpace(io_strObj[index]))
        continue;
      index -= PDFStrLenC(i_pFKey);
      io_strObj.insert(index, i_pKey);
      index += PDFStrLenC(i_pKey);
      io_strObj.insert(index, " ");
      index++;
      io_strObj.insert(index, czBuff);
      index += PDFStrLenC(czBuff);
      io_strObj.insert(index, "\n");
      return true_a;
    }
  }

  index += PDFStrLenC(i_pKey);

  while (io_strObj[index] == PDF_SPACE_CHR || io_strObj[index] == PDF_TAB_CHR ||
          io_strObj[index] == PDF_NL_CHR1 || io_strObj[index] == PDF_NL_CHR2)
    index++;
  if (io_strObj[index] != '[')
    return false_a;
  index++;

  index1 = io_strObj.find("]", index);
  if (index1 == -1)
    return false_a;
  index1--;

  io_strObj.erase(index, index1 - index + 1);
  io_strObj.insert(index, czBuff);
  return true_a;
}

bool_a CPDFTemplate::RemoveDictValue(string &io_strObj, const char *i_pKey)
{
  if (io_strObj.empty() || !i_pKey)
    return false_a;
  basic_string <char>::size_type index, index1;
  long lDic = 0;

  index = io_strObj.find(i_pKey);
  if (index == -1)
    return true_a;
  index1 = index;
  while (index1 < io_strObj.length())
  {
    if ('<' == io_strObj[index1] && '<' == io_strObj[index1 - 1])
    {
      lDic++;
      if (index1 + 1 < io_strObj.size())
      {
        index1++;
      }
    }
    if ('>' == io_strObj[index1] && '>' == io_strObj[index1 - 1])
    {
      lDic--;
      if (!lDic)
        break;
      if (index1 + 1 < io_strObj.size())
      {
        index1++;
      }
    }
    index1++;
  }
  index1++;
  while (io_strObj[index1] == PDF_SPACE_CHR || io_strObj[index1] == PDF_TAB_CHR ||
          io_strObj[index1] == PDF_NL_CHR1 || io_strObj[index1] == PDF_NL_CHR2)
    index1++;

  io_strObj.erase(index, index1 - index);

  return true_a;
}

bool_a CPDFTemplate::Encrypt()
{
  if (!m_pDoc)
    return false_a;
  if (PDFStrLenC(m_pDoc->GetOwnerPassword()) > 0)
    return true_a;

  return false_a;
}

bool_a CPDFTemplate::DoDecrypt()
{
  if (m_lEncryptObj < 0 || !m_pEncryptDic)
    return true_a;

  map <long, size_t>::reverse_iterator iter;
  string strObj;
  basic_string <char>::size_type index, index1, index2;
  char *pObj, *pB/*, *pE*/;
  long lObjNum, lGenNum, lSize, lOldSize;
  size_t Off, Temp;
  string strTemp;

  string strO;
  int iPerm, iEncRevision, iLen;

  if (!m_pEncryptDic->GetSubObject("/O", "(", ")", strO))
    return false_a;
  PDFRemoveEscapeChar(strO);
  iPerm = m_pEncryptDic->GetNumValue("/P");
  iEncRevision = m_pEncryptDic->GetNumValue("/R");
  iLen = m_pEncryptDic->GetNumValue("/Length");
  if (iLen < 0)
    iLen = 40;

  for (iter = m_PdfObjOffMap.rbegin(); iter != m_PdfObjOffMap.rend(); iter++)
  {
    if (iter->first == 0 || iter->first == m_lEncryptObj)
      continue;
    lObjNum = iter->first;
    Off = iter->second;
    pObj = ReadObject(Off, lSize);
    if (!pObj)
      continue;

    lObjNum = atol(pObj);
    pB = strstr(pObj, " ");
    if (!pB)
      return false_a;
    lGenNum = atol(pB);

    strObj.assign(pObj, lSize);
    lOldSize = lSize;

    index = 0;
    while (index != -1)
    {
      index = strObj.find("stream", index);
      if (index != -1)
      {
        index += PDFStrLenC("stream");
        while (strObj[index] == PDF_NL_CHR1 || strObj[index] == PDF_NL_CHR2)
          index++;
        index1 = strObj.find("endstream", index);
        if (index1 != -1)
        {
          index1--;
          while (strObj[index1] == PDF_NL_CHR1 || strObj[index1] == PDF_NL_CHR2)
            index1--;
          if (!PDFEncryptWithPassword((unsigned char *)pObj + index, index1 - index + 1, lObjNum, lGenNum,
                                      m_strFileID, strO, iPerm, m_strPassword, iEncRevision, iLen))
          {
            free(pObj);
            return false_a;
          }
          strObj.assign(pObj, lSize);
          index1 = strObj.find("endstream", index1);
          index1 += 9;
        } else
        {
          free(pObj);
          return false_a;
        }
        index = index1;
      }
    }

    index = 0;
    while (index != -1)
    {
      index = strObj.find("(", index);
      if (index != -1)
      {
        if (strObj[index - 1] == '\\')
        {
          index++;
          continue;
        }
        index2 = strObj.find("stream", index);
        index1 = strObj.find("endstream", index);
        if (index2 > index1 && index1 != -1)
        {
          index++;
          continue;
        }
        index++;
        index1 = index;
        while (index1 != -1)
        {
          index1 = strObj.find(")", index1);
          if (index1 != -1)
          {
            if (strObj[index1 - 1] == '\\' && strObj[index1 - 2] != '\\')
            {
              index1++;
              continue;
            }
            if ((index1 - index) > 0)
            {
              strTemp.assign(pObj + index, index1 - index);
              Temp = strTemp.size();
              PDFRemoveEscapeChar(strTemp);
              //TODO: process hex string

              pB = (char *)PDFMalloc(strTemp.size());
              memcpy(pB, strTemp.c_str(), strTemp.size());

              if (!PDFEncryptWithPassword((unsigned char *)pB, strTemp.size(), lObjNum, lGenNum,
                                          m_strFileID, strO, iPerm, m_strPassword, iEncRevision, iLen))
              {
                free(pB);
                return false_a;
              }
              strTemp.assign(pB, strTemp.size());
              free(pB);
              pB = PDFescapeSpecialChars(strTemp.c_str());
              strTemp.assign(pB);
              free(pB);

              if (strTemp.size() > Temp)
              {
                PDFRealloc((void **)&pObj, lSize + (strTemp.size() - Temp), (strTemp.size() - Temp));
                memmove(pObj + index + (strTemp.size() - Temp), pObj + index, lSize - index);
                memcpy(pObj + index, strTemp.c_str(), strTemp.size());
                lSize += (strTemp.size() - Temp);
              }
              else if (strTemp.size() < Temp)
              {
                memmove(pObj + index, pObj + index + (Temp - strTemp.size()),
                          lSize - (index + (Temp - strTemp.size())));
                memcpy(pObj + index, strTemp.c_str(), strTemp.size());
                lSize += (strTemp.size() - Temp);
              } else
                memcpy(pObj + index, strTemp.c_str(), strTemp.size());
              strObj.assign(pObj, lSize);
            }

            index1++;
            break;
          }
        }
//        index = index1;
      }
    }

    if (lSize != lOldSize)
    {
      if (!UpdateOffsets(Off + 1, lSize - lOldSize))
      {
        free(pObj);
        return false_a;
      }
    }

    m_strRawPDF.erase(Off, lOldSize);
    m_strRawPDF.insert(Off, pObj, lSize);
    free(pObj);
  }
  return true_a;
}

bool_a CPDFTemplate::DoEncrypt()
{
  if (!Encrypt())
    return true_a;
  if (!m_pDoc)
    return false_a;
  map <long, size_t>::reverse_iterator iter;
  string strObj;
  basic_string <char>::size_type index, index1, index2;
  char *pObj, *pB/*, *pE*/, *pTemp;
  long lObjNum, lGenNum, lSize, lTemp, lOldSize;
  size_t Off;

  for (iter = m_PdfObjOffMap.rbegin(); iter != m_PdfObjOffMap.rend(); iter++)
  {
    if (iter->first == 0 || iter->first == m_lEncryptObj)
      continue;
    lObjNum = iter->first;
    Off = iter->second;
    pObj = ReadObject(Off, lSize);
    if (!pObj)
      continue;

    lObjNum = atol(pObj);
    pB = strstr(pObj, " ");
    if (!pB)
      return false_a;
    lGenNum = atol(pB);

    strObj.assign(pObj, lSize);
    lOldSize = lSize;

//    pB = pObj;
    index = 0;
    while (index != -1)
    {
      index = strObj.find("stream", index/*, strObj.length() - index*/);
      if (index != -1)
      {
        index += PDFStrLenC("stream");
        while (strObj[index] == PDF_NL_CHR1 || strObj[index] == PDF_NL_CHR2)
          index++;

        index1 = strObj.find("endstream", index/*, strObj.length() - index*/);
        if (index1 != -1)
        {
          index1--;
          while (strObj[index1] == PDF_NL_CHR1 || strObj[index1] == PDF_NL_CHR2)
            index1--;

          if (!PDFEncrypt((PDFDocPTR)m_pDoc, (unsigned char *)pObj + index, index1 - index + 1, lObjNum, lGenNum))
          {
            free(pObj);
            return false_a;
          }

        }
        index = index1;
      }
    }

    index = 0;
    while (index != -1)
    {
      index = strObj.find("(", index/*, strObj.length() - index*/);
      if (index != -1)
      {
        if (strObj[index - 1] == '\\')
        {
          index++;
          continue;
        }

        index2 = strObj.find("stream", index/*, strObj.length() - index*/);
        index1 = strObj.find("endstream", index/*, strObj.length() - index*/);
        if (index2 > index1 && index1 != -1)
        {
          index++;
          continue;
        }

        index += PDFStrLenC("(");
        index1 = index;
        while (index1 != -1)
        {
          index1 = strObj.find(")", index1/*, strObj.length() - index1*/);
          if (index1 != -1)
          {
            if (strObj[index1 - 1] == '\\')
            {
              index1++;
              continue;
            }
//            index1--;

            if ((index1 - index) > 0)
            {
              pB = (char *)PDFMalloc(index1 - index);
              memcpy(pB, pObj + index, index1 - index);

              if (!PDFEncrypt((PDFDocPTR)m_pDoc, (unsigned char *)/*pObj + index*/pB, index1 - index, lObjNum, lGenNum))
              {
                free(pB);
                free(pTemp);
                free(pObj);
                return false_a;
              }

              pTemp = PDFescapeSpecialCharsBinary(pB, index1 - index, &lTemp);
              if (lTemp > (index1 - index))
              {
                PDFRealloc((void **)&pObj, lSize + (lTemp - (index1 - index)), (lTemp - (index1 - index)));

                memmove(pObj + index + (lTemp - (index1 - index)), pObj + index, lSize - index);

                memcpy(pObj + index, pTemp, lTemp);

                lSize += (lTemp - (index1 - index));
                strObj.assign(pObj, lSize);
              }
              else
                memcpy(pObj + index, pTemp, lTemp);

              free(pB);
              free(pTemp);
            }

            index1++;
            break;
          }
        }
//        index = index1;
      }
    }

    if (lSize > lOldSize)
    {
      if (!UpdateOffsets(Off + 1, lSize - lOldSize))
      {
        free(pObj);
        return false_a;
      }
    }

    m_strRawPDF.erase(Off, lOldSize);
    m_strRawPDF.insert(Off, pObj, lSize);
    free(pObj);
  }
  return true_a;
}

bool_a CPDFTemplate::WriteEncryption(size_t i_Off)
{
  if (!m_pDoc)
    return false_a;
  char *pTemp = NULL, czTemp[200];
  long lTemp;
  string strObj;

//  m_lEncryptObj = m_lLastObj;
//  m_lLastObj++;

  if (!PDFEncrypt((PDFDocPTR)m_pDoc, (unsigned char *)czTemp, 1, 1, 0))//only for initialization of key /O /U
    return false_a;


  sprintf(czTemp, "%ld 0 obj\n", m_lEncryptObj);
  strObj.append(czTemp);
  strObj.append("<<\n");

  strObj.append("/Filter /Standard\n");
  strObj.append("/R 3\n");

  strObj.append("/O (");
  pTemp = PDFescapeSpecialCharsBinary((const char *)m_pDoc->GetEncryptionOwnerKey(), 32, &lTemp);
  strObj.append(pTemp, lTemp);
  free(pTemp);
  strObj.append(")\n");

  strObj.append("/U (");
  pTemp = PDFescapeSpecialCharsBinary((const char *)m_pDoc->GetEncryptionUserKey(), 32, &lTemp);
  strObj.append(pTemp, lTemp);
  free(pTemp);
  strObj.append(")\n");

  sprintf(czTemp, "/P %d\n", m_pDoc->GetEncryptionPermissionKey());
  strObj.append(czTemp);
  strObj.append("/V 2\n");
  strObj.append("/Length 128\n");

  strObj.append(">>\n");
  strObj.append("endobj\n");

  if (!UpdateOffsets(i_Off, strObj.size()))
    return false_a;

  m_PdfObjOffMap[m_lEncryptObj] = i_Off;
  m_strRawPDF.insert(i_Off, strObj);

  return true_a;
}

bool_a CPDFTemplate::WriteInfo(size_t i_Off)
{
  if (!m_pDoc)
    return false_a;
  char czTemp[200];
  long lTemp;
  string strInfo;

  if (m_lInfoObj > 0)
  {
    lTemp = GetObjOffset(m_lInfoObj);
    if (lTemp <= 0)
      return false_a;
    if (!ReadObject(lTemp, strInfo))
        return false_a;
  } else
  {
    m_lInfoObj = m_lLastObj;
    m_lLastObj++;
    sprintf(czTemp, "%ld 0 obj\n", m_lLastObj);
    strInfo.append(czTemp);
    strInfo.append("<<\n");
    strInfo.append(">>\n");
    strInfo.append("endobj");
  }

  if (PDFStrLenC(m_pDoc->GetDocInfo(eTitle)))
    if (!ReplaceTextValue(strInfo, "/Title", ">>", m_pDoc->GetDocInfo(eTitle)))
      return false_a;

  if (PDFStrLenC(m_pDoc->GetDocInfo(eAuthor)))
    if (!ReplaceTextValue(strInfo, "/Author", ">>", m_pDoc->GetDocInfo(eAuthor)))
      return false_a;

  if (PDFStrLenC(m_pDoc->GetDocInfo(eSubject)))
    if (!ReplaceTextValue(strInfo, "/Subject", ">>", m_pDoc->GetDocInfo(eSubject)))
      return false_a;

  if (PDFStrLenC(m_pDoc->GetDocInfo(eKeywords)))
    if (!ReplaceTextValue(strInfo, "/Keywords", ">>", m_pDoc->GetDocInfo(eKeywords)))
      return false_a;

  if (PDFStrLenC(m_pDoc->GetDocInfo(eCreator)))
    if (!ReplaceTextValue(strInfo, "/Creator", ">>", m_pDoc->GetDocInfo(eCreator)))
      return false_a;

  if (PDFStrLenC(m_pDoc->GetDocInfo(eProducer)))
    if (!ReplaceTextValue(strInfo, "/Producer", ">>", m_pDoc->GetDocInfo(eProducer)))
      return false_a;

  if (PDFStrLenC(m_pDoc->GetDocInfo(eCreationDate)))
    if (!ReplaceTextValue(strInfo, "/CreationDate", ">>", m_pDoc->GetDocInfo(eCreationDate)))
      return false_a;

  if (PDFStrLenC(m_pDoc->GetDocInfo(eModDate)))
    if (!ReplaceTextValue(strInfo, "/ModDate", ">>", m_pDoc->GetDocInfo(eModDate)))
      return false_a;

  strInfo.append("\n");

  if (!UpdateOffsets(i_Off, strInfo.size()))
    return false_a;
  m_PdfObjOffMap[m_lInfoObj] = i_Off;
  m_strRawPDF.insert(i_Off, strInfo);

  return true_a;
}

void CPDFTemplate::ReplaceNewLine(string &str)
{
  basic_string <char>::size_type index = 0;

  while (index != -1)
  {
    index = str.find("\\r");
    if (index != -1)
    {
      str.erase(index, 2);
      str.insert(index, "\r");
    }
  }
  index = 0;
  while (index != -1)
  {
    index = str.find("\\n");
    if (index != -1)
    {
      str.erase(index, 2);
      str.insert(index, "\r");
    }
  }
  index = 0;
  while (index != -1)
  {
    index = str.find("\n");
    if (index != -1)
    {
      str.erase(index, 1);
      str.insert(index, "\r");
    }
  }
}

bool_a CPDFTemplate::AddFontToDR(const char *i_pFont, long i_lObjNum)//TODO:
{
  if (!m_pRootDic || !i_pFont)
    return false_a;

  long lObjDR = NOT_USED;
  size_t indexDR = string::npos, Off;
  string strObj, strPath("/AcroForm/DR/Font");
  size_t OldSize;
  string strTemp;
  char czTemp[100];

  strTemp = "/";
  strTemp.append(i_pFont);
  sprintf(czTemp, " %ld 0 R", i_lObjNum);
  strTemp.append(czTemp);

  if (!GetObjAndPosForKey(m_lRootObj, strPath, lObjDR, indexDR))
    return true_a;

  Off = GetObjOffset(lObjDR);
  if (!ReadObject(Off, strObj))
    return false_a;

  OldSize = strObj.size();

  indexDR = strObj.find(czBGNDIC, indexDR);
  if (-1 == indexDR)
    return false_a;
  indexDR += PDFStrLenC(czBGNDIC);

  strObj.insert(indexDR, strTemp);
  if (!UpdateOffsets(Off + 1, strObj.size() - OldSize))
    return false_a;
  m_strRawPDF.erase(Off, OldSize);
  m_strRawPDF.insert(Off, strObj);

  return true_a;
}

//char *CPDFTemplate::ReadItem(const char *i_pObj, size_t i_Size, size_t i_Pos, const char *i_pStart, const char *i_pEnd)
//{
//  if (!i_pObj || i_Pos < 0 || !i_pStart || !i_pEnd || i_Pos >= i_Size)
//    return NULL;
//  char *pPos;
//
//  pPos = (char *)(i_pObj + i_Pos);
//
//  MoveOverWhiteChars(&pPos);
//
//  if (0 != strncmp(pPos, i_pStart, PDFStrLenC(i_pStart)))
//    return NULL;
//
//  char *pRetBuf = NULL;
//  size_t BufLen = 0, Pos = 0;
//  long lRetBufPos = 0;
//  long lDic = 0;
//
//  BufLen = i_Size - (pPos - i_pObj);
//  pRetBuf = (char *)PDFMalloc((BufLen + 1) * sizeof(char));
//  if (!pRetBuf)
//    return NULL;
//  while (Pos < BufLen)
//  {
//    if (lDic > 0)
//    {
//      pRetBuf[lRetBufPos] = pPos[Pos];
//      lRetBufPos++;
//      pRetBuf[lRetBufPos] = 0;
//    }
//    Pos++;
//    if (0 == strncmp(pPos + Pos - PDFStrLenC(i_pStart), i_pStart, PDFStrLenC(i_pStart)))
//    {
//      lDic++;
//      if (Pos + PDFStrLenC(i_pStart) - 1 < BufLen)
//      {
//        for (long ii = 0; ii < PDFStrLenC(i_pStart) - 1; ii++)
//        {
//          pRetBuf[lRetBufPos] = pPos[Pos];
//          lRetBufPos++;
//          Pos++;
//        }
//      }
//    }
//    if (0 == strncmp(pPos + Pos - PDFStrLenC(i_pEnd), i_pEnd, PDFStrLenC(i_pEnd)))
//    {
//      lDic--;
//      if (!lDic)
//      {
//        pRetBuf[lRetBufPos - PDFStrLenC(i_pEnd)] = 0;
//        return pRetBuf;
//      }
//      if (Pos + PDFStrLenC(i_pEnd) - 1 < BufLen)
//      {
//        for (long ii = 0; ii < PDFStrLenC(i_pEnd) - 1; ii++)
//        {
//          pRetBuf[lRetBufPos] = pPos[Pos];
//          lRetBufPos++;
//          Pos++;
//        }
//      }
//    }
//  }
//  free(pRetBuf);
//  pRetBuf = NULL;
//  return pRetBuf;
//}

bool CPDFTemplate::ReadItem(string i_strObj, string i_strKey, string i_strStart,
              string i_strEnd, bool_a i_bCheck, string &i_strValue)
{
  if (i_strObj.empty() || i_strKey.empty() || i_strStart.empty() || i_strEnd.empty())
    return false_a;

  basic_string <char>::size_type index;
  long lObj, lGen;
  bool_a bEscape = false_a;

  if (1 == i_strEnd.size() && IsDelimiter(i_strEnd[0]))
    bEscape = true_a;

  i_strValue.erase();

  //if (i_strKey[i_strKey.size() - 1] != PDF_SPACE_CHR)
  //  i_strKey.append(" ");

  index = i_strObj.find(i_strKey);
  if (index == -1)
    return true_a;

  index += i_strKey.size();

  MoveOverWhiteChars(i_strObj, index);

  if ( 0 != i_strObj.compare(index, i_strStart.size(), i_strStart))
  {
    if (!i_bCheck)
      return false_a;
    lObj = PDFGetObjValue(i_strObj, i_strKey.c_str(), lGen);
    if (lObj < 0 || lGen < 0)
      return false_a;

    lGen = GetObjOffset(lObj);
    if (lGen > 0)
    {
      i_strObj.erase();
      if (!ReadObject(lGen, i_strObj))
        return false_a;

      index = i_strObj.find("obj");
      if (index == -1)
        return false_a;
      return ReadItem(i_strObj, index + 3, i_strStart, i_strEnd, false_a, i_strValue);
    }
    else
      return false_a;
  }
  //index += i_strStart.size();

  //if (i_strObj[index] != PDF_SPACE_CHR && i_strObj[index] != PDF_TAB_CHR &&
  //    i_strObj[index] != PDF_NL_CHR1 && i_strObj[index] != PDF_NL_CHR2)
  //  return false_a;

  long lDic = 0;
  while (index < i_strObj.size())
  {
    if (lDic > 0)
    {
      i_strValue.append(i_strObj.c_str() + index, 1);
    }
    index++;
    if (0 == strncmp(i_strObj.c_str() + index - i_strStart.size(), i_strStart.c_str(), i_strStart.size()))
    {
      lDic++;
      if (index + i_strStart.size() - 1 < i_strObj.size())
      {
        for (size_t ii = 0; ii < i_strStart.size() - 1; ii++)
        {
          i_strValue.append(i_strObj.c_str() + index, 1);
          index++;
        }
      }
    }
    if (0 == strncmp(i_strObj.c_str() + index - i_strEnd.size(), i_strEnd.c_str(), i_strEnd.size()))
    {
      if (!bEscape || (bEscape && '\\' != i_strObj[index - i_strEnd.size() - 1]))
      {
        lDic--;
        if (!lDic)
        {
          i_strValue.erase(i_strValue.size() - i_strEnd.size(), i_strEnd.size());
          return true_a;
        }
        if (index + i_strEnd.size() - 1 < i_strObj.size())
        {
          for (size_t ii = 0; ii < i_strEnd.size() - 1; ii++)
          {
            i_strValue.append(i_strObj.c_str() + index, 1);
            index++;
          }
        }
      }
    }
  }
  return false_a;
}

bool CPDFTemplate::ReadItem(string i_strObj, size_t i_Pos, string i_strStart,
              string i_strEnd, bool_a i_bCheck, string &i_strValue)
{
  if (i_strObj.empty() || i_Pos < 0 || i_Pos >= i_strObj.size() || i_strStart.empty() || i_strEnd.empty())
    return false_a;

  basic_string <char>::size_type index;
  bool_a bEscape = false_a;

  if (1 == i_strEnd.size() && IsDelimiter(i_strEnd[0]))
    bEscape = true_a;


  i_strValue.erase();

  index = i_Pos;

  MoveOverWhiteChars(i_strObj, index);

  if ( 0 != i_strObj.compare(index, i_strStart.size(), i_strStart))
  {
    return false_a; //TO DO
  }

  long lDic = 0;
  while (index < i_strObj.size())
  {
    if (lDic > 0)
    {
      i_strValue.append(i_strObj.c_str() + index, 1);
    }
    index++;
    if (0 == strncmp(i_strObj.c_str() + index - i_strStart.size(), i_strStart.c_str(), i_strStart.size()))
    {
      lDic++;
      if (index + i_strStart.size() - 1 < i_strObj.size())
      {
        for (size_t ii = 0; ii < i_strStart.size() - 1; ii++)
        {
          i_strValue.append(i_strObj.c_str() + index, 1);
          index++;
        }
      }
    }
    if (0 == strncmp(i_strObj.c_str() + index - i_strEnd.size(), i_strEnd.c_str(), i_strEnd.size()))
    {
      if (!bEscape || (bEscape && '\\' != i_strObj[index - i_strEnd.size() - 1]))
      {
        lDic--;
        if (!lDic)
        {
          i_strValue.erase(i_strValue.size() - i_strEnd.size(), i_strEnd.size());
          return true_a;
        }
        if (index + i_strEnd.size() - 1 < i_strObj.size())
        {
          for (size_t ii = 0; ii < i_strEnd.size() - 1; ii++)
          {
            i_strValue.append(i_strObj.c_str() + index, 1);
            index++;
          }
        }
      }
    }
  }
  return false_a;
}

bool CPDFTemplate::AddItem(string &i_strObj, string i_strKey, string i_strStart,
              string i_strEnd, bool_a i_bCheck, string i_strValue)
{
  if (i_strObj.empty() || i_strKey.empty() || i_strStart.empty() || i_strEnd.empty())
    return false_a;

  basic_string <char>::size_type index;
  long lObj, lGen;

  //if (i_strKey[i_strKey.size() - 1] != PDF_SPACE_CHR)
  //  i_strKey.append(" ");

  index = i_strObj.find(i_strKey);
  if (index == -1)
    return true_a;

  index += i_strKey.size();

  MoveOverWhiteChars(i_strObj, index);

  if ( 0 != i_strObj.compare(index, i_strStart.size(), i_strStart))
  {
    string strTemp(i_strObj);
    if (!i_bCheck)
      return false_a;
    lObj = PDFGetObjValue(strTemp, i_strKey.c_str(), lGen);
    if (lObj < 0 || lGen < 0)
      return false_a;

    lGen = GetObjOffset(lObj);
    if (lGen > 0)
    {
      strTemp.erase();
      if (!ReadObject(lGen, strTemp))
        return false_a;

      index = strTemp.find("obj");
      if (index == -1)
        return false_a;

      size_t lSize = strTemp.length();
      if (!AddItem(strTemp, index + 3, i_strStart, i_strEnd, false_a, i_strValue))
        return false_a;

      if (lSize != strTemp.size())
      {
        if (!UpdateOffsets(lGen + 1, strTemp.size() - lSize))
          return false_a;
      }
      m_strRawPDF.erase(lGen, lSize);
      m_strRawPDF.insert(lGen, strTemp);
      return true_a;
    }
    else
      return false_a;
  }
  //index += i_strStart.size();

  //if (i_strObj[index] != PDF_SPACE_CHR && i_strObj[index] != PDF_TAB_CHR &&
  //    i_strObj[index] != PDF_NL_CHR1 && i_strObj[index] != PDF_NL_CHR2)
  //  return false_a;

  //long lDic = 0;
  while (index < i_strObj.size())
  {
    index++;
    if (0 == strncmp(i_strObj.c_str() + index - i_strStart.size(), i_strStart.c_str(), i_strStart.size()))
    {
      i_strObj.insert(index, " ");
      i_strObj.insert(index, i_strValue);
      return true_a;
    }
  }
  return false_a;
}

bool CPDFTemplate::AddItem(string &i_strObj, size_t i_Pos, string i_strStart,
              string i_strEnd, bool_a i_bCheck, string i_strValue)
{
  if (i_strObj.empty() || i_Pos < 0 || i_Pos >= i_strObj.size() || i_strStart.empty() || i_strEnd.empty())
    return false_a;

  basic_string <char>::size_type index;

  index = i_Pos;

  MoveOverWhiteChars(i_strObj, index);

  if ( 0 != i_strObj.compare(index, i_strStart.size(), i_strStart))
  {
    return false_a; //TO DO
  }

  //long lDic = 0;
  while (index < i_strObj.size())
  {
    index++;
    if (0 == strncmp(i_strObj.c_str() + index - i_strStart.size(), i_strStart.c_str(), i_strStart.size()))
    {
      i_strObj.insert(index, i_strValue);
      return true_a;
    }
  }
  return false_a;
}

bool CPDFTemplate::CopyItem(string &i_strDest, string i_strSource,
                            const char ***i_pKeyTable, long i_lKeysCount)
{
  if (i_strDest.empty() || i_strSource.empty() || !i_pKeyTable)
    return false_a;
  string strNew, strOld;
  basic_string <char>::size_type index;
  long i/*, lSize*/;

  for (i = 0; i < 3 * i_lKeysCount; i += 3)
  {
    if (ReadItem(i_strSource, (const char *)i_pKeyTable[i], (const char *)i_pKeyTable[i + 1],
                  (const char *)i_pKeyTable[i + 2], true_a, strNew) && !strNew.empty())
    {
      if (ReadItem(i_strDest, (const char *)i_pKeyTable[i], (const char *)i_pKeyTable[i + 1],
                    (const char *)i_pKeyTable[i + 2], true_a, strOld) && !strOld.empty())
      {
        //lSize = i_strDest.length();
        if (!AddItem(i_strDest, (const char *)i_pKeyTable[i], (const char *)i_pKeyTable[i + 1],
                    (const char *)i_pKeyTable[i + 2], true_a, strNew))
                    return false_a;
        //strOld.insert(0, (const char *)i_pKeyTable[i + 1]);
        //index = i_strDest.find(strOld);
        //index += PDFStrLenC((const char *)i_pKeyTable[i + 1]);
        //strOld.erase(0, PDFStrLenC((const char *)i_pKeyTable[i + 1]));
        //if (index != -1)
        //{
        //  i_strDest.erase(index, strOld.size());
        //  strOld.append(" ");
        //  strOld.append(strNew);
        //  i_strDest.insert(index, strOld);
        //}
        //else
        //  return false_a;
      }
      else
      {
        index = 0;
        i_strDest.insert(index, " ");
        index++;
        i_strDest.insert(index, (const char *)i_pKeyTable[i]);
        index += PDFStrLenC((const char *)i_pKeyTable[i]);
        i_strDest.insert(index, (const char *)i_pKeyTable[i + 1]);
        index += PDFStrLenC((const char *)i_pKeyTable[i + 1]);
        i_strDest.insert(index, " ");
        index++;
        i_strDest.insert(index, strNew);
        index += strNew.size();
        i_strDest.insert(index, " ");
        index++;
        i_strDest.insert(index, (const char *)i_pKeyTable[i + 2]);
        index += PDFStrLenC((const char *)i_pKeyTable[i + 2]);
        i_strDest.insert(index, " ");
        index++;
      }
    }
  }

  return true_a;
}

//char *CPDFTemplate::ReadItem(const char *i_pObj, const char *i_pKey, const char *i_pStart, const char *i_pEnd)
//{
//  if (!i_pObj || !i_pKey || !i_pStart || !i_pEnd)
//    return NULL;
//  char *pPos;
//
//  //pFind = PDFStrJoin(i_pKey, " ");
//  //if (!pFind)
//  //  return NULL;
//  pPos = PDFStrStrKey(i_pObj, i_pKey);
//  if (!pPos)
//  {
//    //free(pFind);
//    return NULL;
//  }
//  pPos += PDFStrLenC(i_pKey);
//  //free(pFind);
//
//  MoveOverWhiteChars(&pPos);
//
//  if (0 != strncmp(pPos, i_pStart, PDFStrLenC(i_pStart)))
//    return NULL;
//
//  char *pRetBuf = NULL;
//  long lBufLen = 0, lPos = 0, lRetBufPos = 0;
//  long lDic = 0;
//
//  lBufLen = PDFStrLenC(pPos);
//  pRetBuf = (char *)PDFMalloc((lBufLen + 1) * sizeof(char));
//  if (!pRetBuf)
//    return NULL;
//  while (lPos < lBufLen)
//  {
//    if (lDic > 0)
//    {
//      pRetBuf[lRetBufPos] = pPos[lPos];
//      lRetBufPos++;
//      pRetBuf[lRetBufPos] = 0;
//    }
//    lPos++;
//    if (0 == strncmp(pPos + lPos - PDFStrLenC(i_pStart), i_pStart, PDFStrLenC(i_pStart)))
//    {
//      lDic++;
//      if (lPos + PDFStrLenC(i_pStart) - 1 < lBufLen)
//      {
//        for (long ii = 0; ii < PDFStrLenC(i_pStart) - 1; ii++)
//        {
//          pRetBuf[lRetBufPos] = pPos[lPos];
//          lRetBufPos++;
//          lPos++;
//        }
//      }
//    }
//    if (0 == strncmp(pPos + lPos - PDFStrLenC(i_pEnd), i_pEnd, PDFStrLenC(i_pEnd)))
//    {
//      lDic--;
//      if (!lDic)
//      {
//        pRetBuf[lRetBufPos - PDFStrLenC(i_pEnd)] = 0;
//        return pRetBuf;
//      }
//      if (lPos + PDFStrLenC(i_pEnd) - 1 < lBufLen)
//      {
//        for (long ii = 0; ii < PDFStrLenC(i_pEnd) - 1; ii++)
//        {
//          pRetBuf[lRetBufPos] = pPos[lPos];
//          lRetBufPos++;
//          lPos++;
//        }
//      }
//    }
//  }
//  free(pRetBuf);
//  pRetBuf = NULL;
//  return pRetBuf;
//}

bool_a CPDFTemplate::ApplyPDFPosition(long i_lPageCount)
{
  //if (!m_lPDFPosition)
  //  return true_a;
  if (m_lPDFPosition < 0 && m_PDFPages.size())
  {
    long lOff;
    string strObj, strNew;
    char czTemp[100];
    basic_string <char>::size_type index;
    PDFTempPage page;

    lOff = GetObjOffset(m_PDFPages[0].lObjIndex);
    if (lOff > 0)
    {
      if (!ReadObject(lOff, strObj))
        return false_a;

      index = strObj.find("obj");
      if (index == -1)
        return false_a;
      strObj.erase(strObj.begin(), strObj.begin() + index);
    }
    for (long j = 1; j < i_lPageCount/*(long)m_PDFPages.size()*/; j++)
    {
      strNew = strObj;
      if (!AddNewPageRefToPages(m_lLastObj))
        return false_a;
      sprintf(czTemp, "%ld 0 ", m_lLastObj);
      strNew.insert(0, czTemp);
      lOff = GetNewObjOffet();

      if (!UpdateOffsets(lOff, strNew.size()))
        return false_a;

      m_strRawPDF.insert(lOff, strNew);

      page.lObjIndex = m_lLastObj;
      page.ObjOffset = lOff;
      m_PDFPages[(long)m_PDFPages.size()] = page;

      m_lLastObj++;
    }
  }
  return true_a;
}

bool_a CPDFTemplate::FillContentsVector(string strContent, vector <string> &contents)
{
  basic_string <char>::iterator strIter, strIter1;
  basic_string <char>::size_type index;
  string strTemp, strLen;

  long lTemp, lLength;
  for (strIter = strContent.begin(); strIter != strContent.end(); strIter++)
  {
    if (*strIter == PDF_SPACE_CHR || *strIter == PDF_TAB_CHR || *strIter == PDF_NL_CHR1 || *strIter == PDF_NL_CHR2)
      continue;
    lTemp = atol(strContent.c_str() + (strIter - strContent.begin()));
    if (lTemp <= 0)
      return false_a;
    lTemp = GetObjOffset(lTemp);
    if (lTemp <= 0)
      return false_a;
//TO DO length read from /Length
    if (!ReadObject(lTemp, strTemp))
      return false_a;

    lLength = PDFGetObjValue(strTemp, "/Length", lTemp);
    if (lLength < 0 || lTemp < 0)
      lLength = PDFGetNumValue(strTemp, "/Length");
    else
    {
      lTemp = GetObjOffset(lLength);
      if (lTemp <= 0)
        return false_a;
      if (!ReadObject(lTemp, strLen))
        return false_a;
      lLength = PDFGetNumValue(strLen, "obj");
    }

    bool_a bFlatDecode = false_a;
    char *pTemp = PDFGetNameValue(strTemp, "/Filter");
    if (pTemp != NULL)
    {
      if (0 == strcmp(pTemp, "/FlateDecode"))
        bFlatDecode = true_a;
      free(pTemp);
    }

    if (ReadItem(strTemp, "/Filter", "[", "]", true_a, strLen))
    {
      index = strLen.find("/FlateDecode");
      if (index != -1)
        bFlatDecode = true_a;
    }


    index = strTemp.find("stream");
    if (index == -1)
      return false_a;
    strTemp.erase(strTemp.begin(), strTemp.begin() + index + 6);

    strIter1 = strTemp.begin();
    if (*strIter1 == PDF_NL_CHR2)
      strTemp.erase(strIter1);
    else if (*strIter1 == PDF_NL_CHR1 && *(strIter1 + 1) == PDF_NL_CHR2)
      strTemp.erase(strIter1, strIter1 + 2);
    else if (*strIter1 == PDF_NL_CHR1)
      strTemp.erase(strIter1);

    strTemp.erase(strTemp.begin() + lLength, strTemp.end());

    //index = strTemp.find("endstream");
    //if (index == -1)
    //  return false_a;
    //strTemp.erase(strTemp.begin() + index, strTemp.end());

    //strIter1 = strTemp.end() - 1;
    //if (*strIter1 == PDF_NL_CHR2)
    //  strTemp.erase(strIter1);
    //else if (*strIter1 == PDF_NL_CHR2 && *(strIter1 - 1) == PDF_NL_CHR1)
    //  strTemp.erase(strIter1, strIter1 + 2);

    if (bFlatDecode)
    {
      if (!CPDFDocument::UncompressStream(strTemp))
        return false_a;
    }

    contents.push_back(strTemp);

    while (*strIter != 'R')
    {
      if (*strIter != PDF_SPACE_CHR && *strIter != PDF_TAB_CHR  && *strIter != PDF_NL_CHR1 && *strIter != PDF_NL_CHR2
          && (*strIter < '0' || *strIter > '9'))
        return false_a;
      strIter++;
    }
  }
  return true_a;
}

bool_a CPDFTemplate::UpdateContentsVector(string strContent, vector <string> &contents)
{
  basic_string <char>::iterator strIter;
  basic_string <char>::size_type index, index1;
  string strTemp, strCompContent, strLenObj;

  long lTemp, lObj, lGen;
  size_t Size, i = 0;
  char czTemp[50];
  for (strIter = strContent.begin(); strIter != strContent.end(); strIter++)
  {
    if (*strIter == PDF_SPACE_CHR || *strIter == PDF_TAB_CHR || *strIter == PDF_NL_CHR1 || *strIter == PDF_NL_CHR2)
      continue;
    lTemp = atol(strContent.c_str() + (strIter - strContent.begin()));
    if (lTemp <= 0)
      return false_a;
    lTemp = GetObjOffset(lTemp);
    if (lTemp <= 0)
      return false_a;
    if (!ReadObject(lTemp, strTemp))
      return false_a;
    Size = strTemp.size();

    if (i >= contents.size())
      return false_a;
    strCompContent = contents[i];

    if (!CPDFDocument::CompressStream(strCompContent))
      return false_a;

    index = strTemp.find("stream");
    if (index == -1)
      return false_a;
    index1 = strTemp.find("endstream");
    if (index1 == -1)
      return false_a;
    strTemp.erase(index, index1 - index);
    strTemp.insert(index, "stream\n");
    index += PDFStrLenC("stream\n");
    strTemp.insert(index, strCompContent);
    index += strCompContent.size();
    strTemp.insert(index, "\n");


    lObj = PDFGetObjValue(strTemp, "/Length", lGen);
    if (lObj < 0 || lGen < 0)
    {
      lObj = PDFGetNumValue(strTemp, "/Length");
      if (lObj < 0)
        return false_a;
      sprintf(czTemp, "%ld", lObj);
      index = strTemp.find("/Length");
      if (index == -1)
        return false_a;
      index += PDFStrLenC("/Length");
      MoveOverWhiteChars(strTemp, index);
      strTemp.erase(index, PDFStrLenC(czTemp));
      sprintf(czTemp, "%ld", strCompContent.size());
      strTemp.insert(index, czTemp);
    }
    else
    {
      lObj = GetObjOffset(lObj);
      if (lObj <= 0)
        return false_a;
      if (!ReadObject(lObj, strLenObj))
        return false_a;
      size_t LenSize = strLenObj.size();

      index = strLenObj.find("obj");
      if (index == -1)
        return false_a;
      index1 = strLenObj.find("endobj");
      if (index1 == -1)
        return false_a;
      strLenObj.erase(index, index1 - index);
      strLenObj.insert(index, "obj\n");
      index += PDFStrLenC("obj\n");
      sprintf(czTemp, "%ld", strCompContent.size());
      strLenObj.insert(index, czTemp);
      index += PDFStrLenC(czTemp);
      strLenObj.insert(index, "\n");

      if (LenSize != strLenObj.size())
      {
        if (!UpdateOffsets(lObj + 1, strLenObj.size() - LenSize))
          return false_a;

        //take updated offset
        lTemp = atol(strContent.c_str() + (strIter - strContent.begin()));
        if (lTemp <= 0)
          return false_a;
        lTemp = GetObjOffset(lTemp);
        if (lTemp <= 0)
          return false_a;
      }
      m_strRawPDF.erase(lObj, LenSize);
      m_strRawPDF.insert(lObj, strLenObj);
    }

    if (Size != strTemp.size())
    {
      if (!UpdateOffsets(lTemp + 1, strTemp.size() - Size))
        return false_a;
    }
    m_strRawPDF.erase(lTemp, Size);
    m_strRawPDF.insert(lTemp, strTemp);

    while (*strIter != 'R')
    {
      if (*strIter != PDF_SPACE_CHR && *strIter != PDF_TAB_CHR  && *strIter != PDF_NL_CHR1 && *strIter != PDF_NL_CHR2
          && (*strIter < '0' || *strIter > '9'))
        return false_a;
      strIter++;
    }
    i++;
  }
  return true_a;
}

bool_a CPDFTemplate::ChangeResNames(string &strNames, vector <string> &contents)
{
  basic_string <char>::iterator strIter, strIter1;
  basic_string <char>::size_type index;
  string strTemp;
  char czTemp[100], cTemp;

  size_t i;
  for (strIter = strNames.begin(); strIter != strNames.end(); strIter++)
  {
    if (*strIter == PDF_SPACE_CHR || *strIter == PDF_TAB_CHR || *strIter == PDF_NL_CHR1 || *strIter == PDF_NL_CHR2)
      continue;
    if (*strIter != '/')
      return false_a;

    index = strNames.find(PDF_SPACE_CHR, strIter - strNames.begin());
    if (index == -1)
      return false_a;
    strTemp.assign(strIter, strNames.begin() + index);
    strNames.erase(strIter, strNames.begin() + index);
    sprintf(czTemp, "/XPubName__%ld", m_lNewResCounter);
    m_lNewResCounter++;
    index = strIter - strNames.begin();
    strNames.insert(index, czTemp);
    strIter = strNames.begin() + index + PDFStrLenC(czTemp);

    for (i = 0; i < contents.size(); i++)
    {
      index = contents[i].find(strTemp); //TO DO: check for space after name
      while (index != -1)
      {
        if (index + strTemp.size() < contents[i].size())
          cTemp = contents[i].at(index + strTemp.size());
        else
          cTemp = PDF_SPACE_CHR;
        if (cTemp == PDF_SPACE_CHR || cTemp == PDF_TAB_CHR ||
            cTemp == PDF_NL_CHR1 || cTemp == PDF_NL_CHR2)
        {
          contents[i].erase(index, strTemp.size());
          contents[i].insert(index, czTemp);
        }
        else
          index++;

        //contents[i].replace(index, index + strTemp.size() - 1, czTemp);
        index = contents[i].find(strTemp, index);
      }
    }

    while (*strIter != 'R')
    {
      czTemp[0] = *strIter;
      if (*strIter != PDF_SPACE_CHR && *strIter != PDF_TAB_CHR  && *strIter != PDF_NL_CHR1 && *strIter != PDF_NL_CHR2
          && (*strIter < '0' || *strIter > '9'))
        return false_a;
      strIter++;
    }
  }
  return true_a;
}

bool_a CPDFTemplate::AddAnnotsToPage(long i_lPage, string strAnnots)
{
  if (strAnnots.empty())
    return true_a;
  if (i_lPage <= 0 || i_lPage > (long)m_PDFPages.size())
    return false_a;

  long lOff, lObj = -1, lGen = -1;
  size_t lSize;
  string strPage, strAnnotObj("");
  basic_string <char>::size_type index;

  lOff = GetObjOffset(m_PDFPages[i_lPage - 1].lObjIndex);
  if (lOff <= 0)
    return false_a;

  if (!ReadObject(lOff, strPage))
    return false_a;
  lSize = strPage.size();

/*
  if (!ReadItem(strPage, czANNOTS, "[", "]", true_a, strAnnotObj))
  {
    lObj = PDFGetObjValue(strPage, czANNOTS, lGen);
    if (lObj > 0 && lGen >= 0)
    {
      sprintf(czTemp, "%ld %ld R", lObj, lGen );
      strAnnotObj = czTemp;
    }
  }
  if (!strAnnotObj.empty())
  {
  }
*/

  string strObj;
  if (!ReadItem(strPage, czANNOTS, "[", "]", true_a, strObj))
  {
    lObj = PDFGetObjValue(strPage, czANNOTS, lGen);
    if (lObj > 0 && lGen >= 0)
    {
      lOff = GetObjOffset(lObj);
      if (lOff <= 0)
        return false_a;

      if (!ReadObject(lOff, strAnnotObj))
        return false_a;
      lSize = strAnnotObj.size();

      if (!ReadItem(strAnnotObj, strAnnotObj.find("obj") + PDFStrLenC("obj"), "[", "]", true_a, strObj))
      {
        char czTemp[100];
        sprintf(czTemp, "%ld %ld R", lObj, lGen );
        strObj.assign(czTemp);
      }
    }
  }
  if (!strObj.empty())
  {
    if (!strAnnotObj.empty())
      strPage = strAnnotObj;
    else
    {
      lOff = GetObjOffset(m_PDFPages[i_lPage - 1].lObjIndex);
      if (lOff <= 0)
        return false_a;
      lSize = strPage.size();
    }
    index = strPage.find(strObj);
    if (index == string::npos)
      return false_a;
  }
  else
  {
    index = strPage.find(czPARENT);
    if (index == -1)
      return false_a;
    strPage.insert(index, "\n");
    strPage.insert(index, czANNOTS);
    index += PDFStrLenC(czANNOTS);
    strPage.insert(index, " ");
    index++;
  }

  if ((lObj < 0 || lGen < 0) || !strAnnotObj.empty())
  {
    strPage.insert(index, " ");
    index++;
    strPage.insert(index, strAnnots);
    index += strAnnots.size();
    strPage.insert(index, " ");
  }
  else
  {
    strPage.insert(index, " [ ");
    index += 3;
    strPage.insert(index, strAnnots);
    index += strAnnots.size();
    strPage.insert(index, " ");
    strPage.insert(index, strObj);
    index += strObj.size();
    strPage.insert(index, " ]");
  }

  if (lSize != strPage.size())
  {
    if (!UpdateOffsets(lOff + 1, strPage.size() - lSize))
      return false_a;
  }
  m_strRawPDF.erase(lOff, lSize);
  m_strRawPDF.insert(lOff, strPage);

  if (!ChangeParent(strAnnots, "/P", i_lPage))
    return false_a;

  return true_a;
}

bool_a CPDFTemplate::ChangeParent(string strObjList, string strKey, long i_lPage)
{
  basic_string <char>::iterator strIter, strIter1;
  string strTemp;
  long lTemp;
  size_t lSize;
  for (strIter = strObjList.begin(); strIter != strObjList.end(); strIter++)
  {
    if (*strIter == PDF_SPACE_CHR || *strIter == PDF_TAB_CHR ||
        *strIter == PDF_NL_CHR1 || *strIter == PDF_NL_CHR2)
      continue;
    lTemp = atol(strObjList.c_str() + (strIter - strObjList.begin()));
    if (lTemp <= 0)
      return false_a;
    lTemp = GetObjOffset(lTemp);
    if (lTemp <= 0)
      return false_a;

    if (!ReadObject(lTemp, strTemp))
      return false_a;

    lSize = strTemp.size();

    if (!ReplaceRefValue(strTemp, strKey.c_str(), ">>", m_PDFPages[i_lPage - 1].lObjIndex, 0))
      return false_a;

    if (lSize != strTemp.size())
    {
      if (!UpdateOffsets(lTemp + 1, strTemp.size() - lSize))
        return false_a;
    }
    m_strRawPDF.erase(lTemp, lSize);
    m_strRawPDF.insert(lTemp, strTemp);

    while (*strIter != 'R')
    {
      if (*strIter != PDF_SPACE_CHR && *strIter != PDF_TAB_CHR  && *strIter != PDF_NL_CHR1 && *strIter != PDF_NL_CHR2
          && (*strIter < '0' || *strIter > '9'))
        return false_a;
      strIter++;
    }
  }
  return true_a;
}

bool_a CPDFTemplate::AddResourcesToPage(long i_lPage, string strRes)
{
  if (strRes.empty())
    return true_a;
  if (i_lPage > (long)m_PDFPages.size())
    return false_a;

  long lOff, lObj = -1, lGen = -1;
  size_t lSize;
  string strPage, strTemp, strTemp1, strPageRes, strPageResSave;
  basic_string <char>::size_type index;

  lOff = GetObjOffset(m_PDFPages[i_lPage - 1].lObjIndex);
  if (lOff <= 0)
    return false_a;

  if (!ReadObject(lOff, strPage))
    return false_a;

  if (!ReadItem(strPage, czRESOURCES, "<<", ">>", true_a, strPageRes))
  {
    lObj = PDFGetObjValue(strPage, czRESOURCES, lGen);
    if (lObj < 0 || lGen < 0)
      return false_a;

    lOff = GetObjOffset(/*m_lLastObj + */lObj);
    if (lOff <= 0)
      return false_a;

    if (!ReadObject(lOff, strTemp))
      return false_a;

    if (!ReadItem(strTemp, strTemp.find("obj") + PDFStrLenC("obj"), "<<", ">>", true_a, strPageRes))
      return false_a;
  }
  strPageResSave = strPageRes;

  if (!CopyItem(strPageRes, strRes, (const char ***)pczRESOURES_KEY, RESOURES_KEY_NUM - 1))// "/ProcSet" is not needed
    return false_a;


  if (lObj < 0 || lGen < 0)
  {
    lOff = GetObjOffset(m_PDFPages[i_lPage - 1].lObjIndex);
    if (lOff <= 0)
      return false_a;
  }
  else
  {
    lOff = GetObjOffset(/*m_lLastObj + */lObj);
    if (lOff <= 0)
      return false_a;

    if (!ReadObject(lOff, strPage))
      return false_a;
  }

  index = strPage.find(strPageResSave);
  if (index == -1)
    return false_a;
  lSize = strPage.size();
  strPage.erase(index, strPageResSave.size());
  strPage.insert(index, strPageRes);

  if (lSize != strPage.size())
  {
    if (!UpdateOffsets(lOff + 1, strPage.size() - lSize))
      return false_a;
  }
  m_strRawPDF.erase(lOff, lSize);
  m_strRawPDF.insert(lOff, strPage);

  return true_a;
}

bool_a CPDFTemplate::GetObjAndPosForKey(long i_lObjNum, string& i_strPath, long& o_lObjNum, size_t& o_index)
{
  if (i_strPath.empty())
    return true_a;

  size_t index, Off, index1, indexRet = 0;
  string strTemp, strObj, strItem;
  long lObj, lGen;

  Off = GetObjOffset(i_lObjNum);
  if (!ReadObject(Off, strObj))
    return false_a;


  while (!strObj.empty())
  {
    index = i_strPath.find("/", 1);//skip first '/'
    if (-1 == index && !i_strPath.empty())
      index = i_strPath.size();
    strTemp.assign(i_strPath.begin(), i_strPath.begin() + index);
    i_strPath.erase(0, index);

    lObj = PDFGetObjValue(strObj, strTemp.c_str(), lGen);
    if (lObj > 0 && lGen >= 0)
    {
      if (i_strPath.empty())
      {
        o_lObjNum = lObj;
        o_index = 0;
        return true_a;
      }
      else
        return GetObjAndPosForKey(lObj, i_strPath, o_lObjNum, o_index);
    }
    index1 = CPDFDocument::PDFStrStrKey(strObj, 0, strTemp.c_str());
    indexRet += index1;
    if (index1 == -1)
      return false_a;
    if (i_strPath.empty())
    {
      o_lObjNum = i_lObjNum;
      o_index = indexRet;
      return true_a;
    }
    index1 += strTemp.size();
    indexRet += strTemp.size();
    index = index1;
    MoveOverWhiteChars(strObj, index);
    indexRet += index - index1;
    strItem = strObj;
    if (!ReadItem(strItem, index1, "<<", ">>", false_a, strObj))
      return false_a;
    if (strObj.empty())
      return false_a;
    index = strItem.find(strObj);
    indexRet += index - index1;
  }





  indexRet = index1 = CPDFDocument::PDFStrStrKey(strObj, strTemp.c_str());
  while (index1 != -1)
  {
    if (i_strPath.empty())
    {
      o_lObjNum = i_lObjNum;
      o_index = indexRet;
      return true_a;
    }
    index1 += strTemp.size();
    indexRet += strTemp.size();
    if (!ReadItem(strObj, index1, "<<", ">>", false_a, strItem))
      return false_a;
    if (strItem.empty())
      return false_a;

    index = i_strPath.find("/", 1);//skip first '/'
    if (-1 == index && !i_strPath.empty())
      index = i_strPath.size();
    strTemp.assign(i_strPath.begin(), i_strPath.begin() + index);
    i_strPath.erase(0, index);

    lObj = PDFGetObjValue(strItem, strTemp.c_str(), lGen);
    if (lObj > 0 && lGen >= 0)
    {
      if (i_strPath.empty())
      {
        o_lObjNum = lObj;
        o_index = 0;
        return true_a;
      }
      else
        return GetObjAndPosForKey(lObj, i_strPath, o_lObjNum, o_index);
    }
    index1 = CPDFDocument::PDFStrStrKey(strItem, 0, strTemp.c_str());

  }

  return false_a;
}

bool_a CPDFTemplate::ProcessAcroForm(string strRes, long &lLastObj)
{
  if (strRes.empty())
    return true_a;

  long lObjDR = NOT_USED, i;
  size_t indexDR = string::npos, Off, OldSize;
  string strTemp, strPath, strObj;

  //append Fields
  ReadItem(strRes, "/Fields", "[", "]", true_a, strTemp);
  if (!strTemp.empty())
  {
    strPath = "/AcroForm/Fields";
    if (!GetObjAndPosForKey(m_lRootObj, strPath, lObjDR, indexDR))
      return true_a;

    Off = GetObjOffset(lObjDR);
    if (!ReadObject(Off, strObj))
      return false_a;

    OldSize = strObj.size();

    indexDR = strObj.find("[", indexDR);
    if (-1 == indexDR)
      return false_a;
    indexDR++;

    strObj.insert(indexDR, " ");
    strObj.insert(indexDR, strTemp);
    if (!UpdateOffsets(Off + 1, strObj.size() - OldSize))
      return false_a;
    m_strRawPDF.erase(Off, OldSize);
    m_strRawPDF.insert(Off, strObj);
  }
  //append CO
  ReadItem(strRes, "/CO", "[", "]", true_a, strTemp);
  if (!strTemp.empty())
  {
    strPath = "/AcroForm/CO";
    if (!GetObjAndPosForKey(m_lRootObj, strPath, lObjDR, indexDR))
      return true_a;

    Off = GetObjOffset(lObjDR);
    if (!ReadObject(Off, strObj))
      return false_a;

    OldSize = strObj.size();

    indexDR = strObj.find("[", indexDR);
    if (-1 == indexDR)
      return false_a;
    indexDR++;

    strTemp.append(" ");

    strObj.insert(indexDR, strTemp);
    if (!UpdateOffsets(Off + 1, strObj.size() - OldSize))
      return false_a;
    m_strRawPDF.erase(Off, OldSize);
    m_strRawPDF.insert(Off, strObj);
  }

  ReadItem(strRes, "/DR", czBGNDIC, czENDDIC, true_a, strTemp);
  if (strTemp.empty())
    return true_a;
  strRes = strTemp;
  for (i = 0; i < RESOURES_KEY_NUM - 1; i++) // "/ProcSet" is not needed
  {
    ReadItem(strRes, pczRESOURES_KEY[i][0], pczRESOURES_KEY[i][1], pczRESOURES_KEY[i][2], true_a, strTemp);
    if (!strTemp.empty())
    {
      strPath = "/AcroForm/DR";
      strPath.append(pczRESOURES_KEY[i][0]);
      if (!GetObjAndPosForKey(m_lRootObj, strPath, lObjDR, indexDR))
        return true_a;

      Off = GetObjOffset(lObjDR);
      if (!ReadObject(Off, strObj))
        return false_a;

      OldSize = strObj.size();

      indexDR = strObj.find(pczRESOURES_KEY[i][1], indexDR);
      if (-1 == indexDR)
        return false_a;
      indexDR += PDFStrLenC(pczRESOURES_KEY[i][1]);

      strObj.insert(indexDR, strTemp);
      if (!UpdateOffsets(Off + 1, strObj.size() - OldSize))
        return false_a;
      m_strRawPDF.erase(Off, OldSize);
      m_strRawPDF.insert(Off, strObj);
    }
  }

  return true_a;
}

bool_a CPDFTemplate::ProcessContentRes(string &strRes, string strContent)
{
  if (strRes.empty())
    return true_a;
  if (strContent.empty())
    return false_a;
  string strTemp, strSave;
  basic_string <char>::size_type index;

  vector <string> contents;

  if (!FillContentsVector(strContent, contents))
    return false_a;

  if (!contents.size())
    return false_a;

  long i;
  for (i = 0; i < RESOURES_KEY_NUM - 1; i++) // "/ProcSet" is not needed
  {
    if (ReadItem(strRes, pczRESOURES_KEY[i][0], pczRESOURES_KEY[i][1], pczRESOURES_KEY[i][2], true_a, strTemp))
    {
      strSave = strTemp;
      if (!ChangeResNames(strTemp, contents))
        return false_a;
      index = strRes.find(strSave);
      if (index != -1)
      {
        strRes.erase(index, strSave.size());
        strRes.insert(index, strTemp);
      }
      else
        return false_a;
    }
  }

  if (!UpdateContentsVector(strContent, contents))
    return false_a;

  return true_a;
}

void CPDFTemplate::MoveOverWhiteChars(char **i_pPos)
{
  if (!(**i_pPos) || !(*i_pPos))
    return;
  while (**i_pPos == PDF_SPACE_CHR || **i_pPos == PDF_TAB_CHR || **i_pPos == PDF_NL_CHR1 || **i_pPos == PDF_NL_CHR2)
    (*i_pPos)++;
}

void CPDFTemplate::MoveOverWhiteChars(string strObj, basic_string <char>::size_type &index)
{
  if (strObj.empty() || strObj.size() <= index)
    return;
  basic_string <char>::iterator strIter;
  strIter = strObj.begin() + index;
  while (strIter != strObj.end() && (*strIter == PDF_SPACE_CHR || *strIter == PDF_TAB_CHR || *strIter == PDF_NL_CHR1 || *strIter == PDF_NL_CHR2))
    strIter++;
  index = strIter - strObj.begin();
}

long CPDFTemplate::GetImageCount(long i_lPage)
{
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return 0;
  return (long)m_PDFPages[i_lPage].m_mapImages.size();
}

const char *CPDFTemplate::GetImageName(long i_lPage, long i_lIndex)
{
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return NULL;
  if (i_lIndex < 0 || i_lIndex >= m_PDFPages[i_lPage].m_mapImages.size())
    return NULL;

  long lRet = 0;
  std::multimap<std::string, PDFCTM>::iterator iter;
  for (iter = m_PDFPages[i_lPage].m_mapImages.begin(); iter != m_PDFPages[i_lPage].m_mapImages.end(); iter++)
  {
    lRet++;
    if (lRet - 1 == i_lIndex)
      return (*iter).first.c_str();
  }
  return NULL;
}

PDFCTM CPDFTemplate::GetImageCTM(long i_lPage, long i_lIndex)
{
  PDFCTM M = PDFIdentMatrix();
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return M;
  if (i_lIndex < 0 || i_lIndex >= m_PDFPages[i_lPage].m_mapImages.size())
    return M;

  long lRet = 0;
  std::multimap<std::string, PDFCTM>::iterator iter;
  for (iter = m_PDFPages[i_lPage].m_mapImages.begin(); iter != m_PDFPages[i_lPage].m_mapImages.end(); iter++)
  {
    lRet++;
    if (lRet - 1 == i_lIndex)
      return (*iter).second;
  }
  return M;
}

long CPDFTemplate::GetStrBlockCount(long i_lPage)
{
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return 0;
  return (long)m_PDFPages[i_lPage].m_WStringBlocks.size();
}

const wchar_t *CPDFTemplate::GetStrBlock(long i_lPage, size_t i_Block)
{
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return NULL;
  if (i_Block < 0 || i_Block >= m_PDFPages[i_lPage].m_WStringBlocks.size())
    return NULL;

  return m_PDFPages[i_lPage].m_WStringBlocks[i_Block].c_str();
}

const char *CPDFTemplate::GetStrBlockFontName(long i_lPage, size_t i_Block)
{
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return NULL;
  if (i_Block < 0 || i_Block >= m_PDFPages[i_lPage].m_WStringBlocks.size())
    return NULL;

  return m_PDFPages[i_lPage].m_Blocks[i_Block]->GetFontBaseName();
}

float CPDFTemplate::GetStrBlockFontSize(long i_lPage, size_t i_Block)
{
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return NULL;
  if (i_Block < 0 || i_Block >= m_PDFPages[i_lPage].m_WStringBlocks.size())
    return NULL;

  return m_PDFPages[i_lPage].m_Blocks[i_Block]->GetFontSize();
}

void CPDFTemplate::GetStrBlockPosition(long i_lPage, size_t i_Block, float &o_fX, float &o_fY)
{
  if (i_lPage >= (long)m_PDFPages.size() || i_lPage < 0)
    return;
  if (i_Block < 0 || i_Block >= m_PDFPages[i_lPage].m_WStringBlocks.size())
    return;

  o_fX = m_PDFPages[i_lPage].m_Blocks[i_Block]->GetCTM().m_fE;
  o_fY = m_PDFPages[i_lPage].m_Blocks[i_Block]->GetCTM().m_fF;
}

bool_a CPDFTemplate::ReadContentStreamObject()
{
  string strObj, strContent;
  long lObj, lGen, lPage;
  vector <string> contents;
  char czTemp[100];


  for (lPage = 0; lPage < GetPageCount(); lPage++)
  {
    if (!ReadObject(m_PDFPages[lPage].ObjOffset, strObj))
      return false_a;
    //Content
    if (!ReadItem(strObj, czCONTENTS, "[", "]", true_a, strContent))
    {
      lObj = PDFGetObjValue(strObj, czCONTENTS, lGen);
      if (lObj < 0 || lGen < 0)
        return false_a;
      sprintf(czTemp, "%ld %ld R", lObj, lGen );
      strContent = czTemp;
    }

    if (!FillContentsVector(strContent, contents))
      return false_a;
    if (contents.empty())
      return false_a;

    m_lRotate = PDFGetNumValue(strObj, "/Rotate");
    if (m_lRotate < 0)
      m_lRotate = 0;
    m_lPage = lPage;

    if (!ParseContentStream(&contents))
      return false_a;

    m_PDFPages[lPage].m_lRotate = m_lRotate;
    m_PDFPages[lPage].m_mapObjects = m_mapObjects;
    m_PDFPages[lPage].m_mapImages = m_mapImages;

    m_mapObjects.clear();
    m_mapImages.clear();
    contents.clear();
  }

  //if (!BuildStrBlockSimple())
  //  return false_a;

  return true_a;
}

void CPDFTemplate::ClearStrBlock()
{
  long lPage;
  for (lPage = 0; lPage < GetPageCount(); lPage++)
  {
    m_PDFPages[lPage].m_WStringBlocks.clear();
    m_PDFPages[lPage].m_Blocks.clear();
  }
}

bool_a CPDFTemplate::BuildStrBlock(eStrBlockType i_eType, CPDFStrBlockControl *i_pStrBlockCtrl)
{
  bool_a bRet = true_a;
  m_pStrBlockControl = i_pStrBlockCtrl;
  if (eNotBuilded == m_eStrBlockType)
  {
    if (!ReadContentStreamObject())
      bRet = false_a;
  }
  if (bRet)
  {
    switch (i_eType)
    {
      case eSimple:
        bRet = BuildStrBlockSimple();
        break;
      case eJoined:
        bRet = BuildStrBlockJoined();
        break;
      default:
        bRet = false_a;
        break;
    }
  }
  m_pStrBlockControl = NULL;
  return bRet;
}

bool_a CPDFTemplate::BuildStrBlockSimple()
{
  long lPage;
  wstring strW;
  CPDFObjects *pPDFObj = NULL;
  PDFObjectsMap::iterator iter;

  ClearStrBlock();

  for (lPage = 0; lPage < GetPageCount(); lPage++)
  {
    for (iter = m_PDFPages[lPage].m_mapObjects.begin(); iter != m_PDFPages[lPage].m_mapObjects.end(); iter++)
    {
      pPDFObj = (CPDFObjects *)iter->second;
      while (pPDFObj)
      {
        strW.assign(pPDFObj->GetWValue());
        if (!strW.empty())
        {
          m_PDFPages[lPage].m_WStringBlocks.push_back(strW);
          m_PDFPages[lPage].m_Blocks.push_back(pPDFObj);
        }
        pPDFObj = pPDFObj->GetNextObjects();
      }
    }
    if (!strW.empty() && pPDFObj)
    {
      m_PDFPages[lPage].m_WStringBlocks.push_back(strW);
      strW.clear();
      m_PDFPages[lPage].m_Blocks.push_back(pPDFObj);
    }
  }

  m_eStrBlockType = eSimple;
  return true_a;
}

bool_a CPDFTemplate::BuildStrBlockJoined()
{
  long lPage;
  //float fWidth = 0;
  wstring wstrJoin;
  wstring strW;

  ClearStrBlock();

  for (lPage = 0; lPage < GetPageCount(); lPage++)
  {
    CPDFObjects *pPDFObj = NULL, *pPDFObjSave = NULL;
    PDFObjectsMap::iterator iter;
    string strFont;

    for (iter = m_PDFPages[lPage].m_mapObjects.begin(); iter != m_PDFPages[lPage].m_mapObjects.end(); iter++)
    {
      pPDFObj = (CPDFObjects *)iter->second;

      wstrJoin.clear();
      if (pPDFObjSave && !strW.empty())
      {
        CPDFObjects *pFirst = pPDFObj->AppendToBegin() ? pPDFObj : pPDFObjSave;
        CPDFObjects *pSecond = pPDFObj->AppendToBegin() ? pPDFObjSave : pPDFObj;
        float fXWidth, fYWidth, fXHalfWidth, fYHalfWidth;
        if (pPDFObj->CompareObjects(pPDFObjSave, fXWidth, fYWidth, fXHalfWidth, fYHalfWidth))
        {
          if (m_pStrBlockControl && !m_pStrBlockControl->CanDivide(pFirst, pSecond, wstrJoin))
          {
            if (pPDFObj->AppendToBegin())
              strW.insert(0, wstrJoin);
            else
              strW += wstrJoin;
          }
          else
          {
            m_PDFPages[lPage].m_WStringBlocks.push_back(strW);
            strW.clear();
            m_PDFPages[lPage].m_Blocks.push_back(pPDFObjSave);
          }
        }
        else if (fXHalfWidth <= pSecond->GetCTM().m_fE && fXWidth >= pSecond->GetCTM().m_fE &&
          fYHalfWidth <= pSecond->GetCTM().m_fF && fYWidth >= pSecond->GetCTM().m_fF)
        {
          wstrJoin = L" ";
          if (m_pStrBlockControl && !m_pStrBlockControl->CanJoin(pFirst, pSecond, wstrJoin))
          {
            m_PDFPages[lPage].m_WStringBlocks.push_back(strW);
            strW.clear();
            m_PDFPages[lPage].m_Blocks.push_back(pPDFObjSave);
          }
          else
          {
            if (pPDFObj->AppendToBegin())
              strW.insert(0, wstrJoin);
            else
              strW += wstrJoin;
          }
        }
        else
        {
          if (m_pStrBlockControl && !m_pStrBlockControl->CanJoin(pFirst, pSecond, wstrJoin))
          {
            m_PDFPages[lPage].m_WStringBlocks.push_back(strW);
            strW.clear();
            m_PDFPages[lPage].m_Blocks.push_back(pPDFObjSave);
          }
          else
          {
            if (pPDFObj->AppendToBegin())
              strW.insert(0, wstrJoin);
            else
              strW += wstrJoin;
          }
        }
      }

      pPDFObjSave = pPDFObj;
      while (pPDFObj)
      {
        strFont = pPDFObj->GetFontResName();

        if (pPDFObj->AppendToBegin())
          strW.insert(0, pPDFObj->GetWValue());
        else
          strW.append(pPDFObj->GetWValue());

        //if (pPDFObj->GetNextObjects() && !strW.empty())
        //{
        //  if (!strW.empty() && pPDFObj->GetNextObjects() &&
        //      (strFont.compare(pPDFObj->GetNextObjects()->GetFontResName()) != 0
        //        || pPDFObj->GetFontSize() != pPDFObj->GetNextObjects()->GetFontSize()))
        //  {
        //    if (m_pStrBlockControl && !m_pStrBlockControl->CanDivide(pPDFObjSave, pPDFObj->GetNextObjects(), wstrJoin))
        //    {
        //    if (pPDFObj->AppendToBegin())
        //      strW.insert(0, wstrJoin);
        //    else
        //      strW += wstrJoin;
        //    }
        //    else
        //    {
        //      m_PDFPages[lPage].m_WStringBlocks.push_back(strW);
        //      strW.clear();
        //      m_PDFPages[lPage].m_Blocks.push_back(pPDFObj);
        //    }
        //  }
        //  else
        //  {
        //    if (m_pStrBlockControl && !m_pStrBlockControl->CanJoin(pPDFObjSave, pPDFObj->GetNextObjects(), wstrJoin))
        //    {
        //      m_PDFPages[lPage].m_WStringBlocks.push_back(strW);
        //      strW.clear();
        //      m_PDFPages[lPage].m_Blocks.push_back(pPDFObj);
        //    }
        //    else
        //    {
        //    if (pPDFObj->AppendToBegin())
        //      strW.insert(0, wstrJoin);
        //    else
        //      strW += wstrJoin;
        //    }
        //  }
        //}

        pPDFObj = pPDFObj->GetNextObjects();
        if (pPDFObj)
          pPDFObjSave = pPDFObj;
      }
    }

    if (!strW.empty() && pPDFObjSave)
    {
      m_PDFPages[lPage].m_WStringBlocks.push_back(strW);
      strW.clear();
      m_PDFPages[lPage].m_Blocks.push_back(pPDFObjSave);
    }
  }

  m_eStrBlockType = eJoined;
  return true_a;
}

bool_a CPDFTemplate::ParseContentStream(vector <string> *i_pContents)
{
  if (!i_pContents || i_pContents->empty())
    return false_a;
  size_t i, index;
  string strToken;
  vector <string> strValues;
  bool_a bInText = false_a;
  bool_a bRet = true_a;

  m_strFontName = "";
  m_fFontSize = 0.f;
  //m_Tm = PDFIdentMatrix();
  //m_Tlm = PDFIdentMatrix();
  m_fLeading = 0.0;
  m_fWordSpacing = 0.0;
  m_fCharSpacing = 0.0;
  m_fHScaling = 100.0;
  m_bInlineImage = false_a;
  PDFCTM ctmTemp;

  switch (m_lRotate)
  {
  case 0:
    m_CTM.m_fA = 1.f; m_CTM.m_fB = 0.f; m_CTM.m_fC = 0.f; m_CTM.m_fD = 1.f; m_CTM.m_fE = 0.f; m_CTM.m_fF = 0.f;
    break;
  case 90:
    m_CTM.m_fA = 0.f; m_CTM.m_fB = -1.f; m_CTM.m_fC = 1.f; m_CTM.m_fD = 0.f; m_CTM.m_fE = 0.f; m_CTM.m_fF = 0.f;
    break;
  case 180:
    m_CTM.m_fA = -1.f; m_CTM.m_fB = 0.f; m_CTM.m_fC = 0.f; m_CTM.m_fD = -1.f; m_CTM.m_fE = 0.f; m_CTM.m_fF = 0.f;
    break;
  case 270:
    m_CTM.m_fA = 0.f; m_CTM.m_fB = 1.f; m_CTM.m_fC = -1.f; m_CTM.m_fD = 0.f; m_CTM.m_fE = 0.f; m_CTM.m_fF = 0.f;
    break;
  default:
    return NULL;
  }

  try
  {
    for (i = 0; i < i_pContents->size(); i++)
    {
      index = 0;
      //bInText = false_a;
      strToken = PDFGetStringUpToWhiteSpace((*i_pContents)[i], index);
      while (!strToken.empty())
      {
        if (!strToken.compare(pczOperatorsList[BT_OPERATOR_POS])) //Text Block start - add new empty block to vector;
        {
          bInText = true_a;
          //m_StringBlocks.push_back("");
          m_Tm = PDFIdentMatrix();
          ctmTemp = m_CTM;
          switch (m_lRotate)
          {
          case 0:
            m_CTM.m_fA = 1.f; m_CTM.m_fB = 0.f; m_CTM.m_fC = 0.f; m_CTM.m_fD = 1.f; m_CTM.m_fE = 0.f; m_CTM.m_fF = 0.f;
            break;
          case 90:
            m_CTM.m_fA = 0.f; m_CTM.m_fB = -1.f; m_CTM.m_fC = 1.f; m_CTM.m_fD = 0.f; m_CTM.m_fE = 0.f; m_CTM.m_fF = 0.f;
            break;
          case 180:
            m_CTM.m_fA = -1.f; m_CTM.m_fB = 0.f; m_CTM.m_fC = 0.f; m_CTM.m_fD = -1.f; m_CTM.m_fE = 0.f; m_CTM.m_fF = 0.f;
            break;
          case 270:
            m_CTM.m_fA = 0.f; m_CTM.m_fB = 1.f; m_CTM.m_fC = -1.f; m_CTM.m_fD = 0.f; m_CTM.m_fE = 0.f; m_CTM.m_fF = 0.f;
            break;
          default:
            return NULL;
          }
          m_Tlm = m_Tm = PDFMulMatrix(m_Tm, m_CTM);
          //m_strFontName = "";
          //m_fLeading = 0.f;
        }
        if (!strToken.compare(pczOperatorsList[ET_OPERATOR_POS]))
        {
          bInText = false_a;
          strValues.clear();
          m_CTM = ctmTemp;
        }

        if (bInText) {
          ProcessInTextToken(strToken, &strValues);
        }
        else if (strToken.size()) {
          if (!strToken.compare(pczOperatorsList[Do_OPERATOR_POS]))
          {
            if (strValues.size() > 0)
            {
              std::multimap<std::string, CPDFXObject *>::iterator iter = m_PDFPages[m_lPage].m_pXObjects.find(strValues[0]);
              if (iter != m_PDFPages[m_lPage].m_pXObjects.end())
              {
                if (!((*iter).second)->isImage())
                {
                  (*i_pContents)[i].insert(index, ((*iter).second)->GetStream());
                  strValues.clear();
                }
              }
            }
          }
          else
          {
            ProcessNotInTextToken(strToken, &strValues);
            if (m_bInlineImage) {
              index++;
            }
          }
        }

        strToken = PDFGetStringUpToWhiteSpace((*i_pContents)[i], index);
      }
    }
  }
  catch (const char *e)
  {
    e;
    bRet = false_a;
  }

  return bRet;
}


void CPDFTemplate::ProcessNotInTextToken(string& i_strToken, std::vector <std::string> *i_pValues)
{
  if (!i_pValues)
    throw "NULL values pointer!";
  if (i_strToken.empty())
    throw "Empty Token!";

  OperatorMap::iterator iter;

  iter = m_mapNotTextOperators.find(i_strToken);
  if (iter != m_mapNotTextOperators.end())
  {
    (this->*(iter->second))(i_pValues);
    i_pValues->clear();
  }
  else
  {
    if (PDFIsPDFOperator(i_strToken)) {
      i_pValues->clear();
    }
    else {
      i_pValues->push_back(i_strToken);
    }
  }
}
void CPDFTemplate::ProcessInTextToken(string& i_strToken, vector <string> *i_pValues)
{
  if (!i_pValues)
    throw "NULL values pointer!";
  if (i_strToken.empty())
    throw "Empty Token!";

  OperatorMap::iterator iter;

  iter = m_mapTextOperators.find(i_strToken);
  if (iter != m_mapTextOperators.end())
  {
    (this->*(iter->second))(i_pValues);
    i_pValues->clear();
  }
  else
  {
    if (PDFIsPDFOperator(i_strToken))
      i_pValues->clear();
    else
      i_pValues->push_back(i_strToken);
  }
}

void CPDFTemplate::AddImageToObjects(std::string i_strImgName)
{
  if (i_strImgName.empty())
    return;
  PDFCTM M;

  M = m_CTM;
  
  m_mapImages.insert(map<string, PDFCTM>::value_type(i_strImgName, M));
}

CPDFObjects *CPDFTemplate::AddToObjects(string i_str)
{
  if (i_str.empty())
    return NULL;
  CPDFObjects *pPDFObj;
  //PDFObjectsMap::iterator iter;
  PDFCTM M;

  //switch (m_lRotate)
  //{
  //case 0:
  //  M.fA = 1.f; M.fB = 0.f; M.fC = 0.f; M.fD = 1.f; M.fE = 0.f; M.fF = 0.f;
  //  break;
  //case 90:
  //  M.fA = 0.f; M.fB = 1.f; M.fC = -1.f; M.fD = 0.f; M.fE = 0.f; M.fF = 0.f;
  //  break;
  //case 180:
  //  M.fA = -1.f; M.fB = 0.f; M.fC = 0.f; M.fD = -1.f; M.fE = 0.f; M.fF = 0.f;
  //  break;
  //case 270:
  //  M.fA = 0.f; M.fB = -1.f; M.fC = 1.f; M.fD = 0.f; M.fE = 0.f; M.fF = 0.f;
  //  break;
  //default:
  //  return NULL;
  //}

  M = /*PDFMulMatrix(M, */m_Tm;//);

  M.m_vct.SetX(m_Tm.m_fA * 1.f/* + m_Tm.fC * 0.f*//* + m_Tm.fE*/);
  M.m_vct.SetY(m_Tm.m_fB * 1.f/* + m_Tm.fD * 0.f*//* + m_Tm.fF*/);

  pPDFObj = new CPDFObjects(this, M, i_str.c_str(), m_strFontName.c_str(), m_fFontSize,
                            m_lPage, m_fWordSpacing, m_fCharSpacing, m_fHScaling);
  if (!pPDFObj)
    return NULL;

/////////////////////////////////////////////////////////////////////////////////////////////
    PDFObjectsMap::iterator iterL, iterU;
    PDFCTM M1(0.f, -1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    double fDelta1, fDelta2;

    if (m_mapObjects.size() > 0)
    {
      iterU = iterL = m_mapObjects.lower_bound(M);
      if (m_mapObjects.end() != iterL)
      {
        M1 = (PDFCTM)iterL->first;
        // count distance between lines
        //now only vert. and horiz. supported
        if (M.m_vct.GetY() == 0.f)
          fDelta1 = M.m_fF - M1.m_fF;
        else 
          fDelta1 = M1.m_fE - M.m_fE;

        if (m_mapObjects.begin() != iterU)
        {
          iterU--;
          M1 = (PDFCTM)iterU->first;
          if (M.m_vct.GetY() == 0.f)
            fDelta2 = M1.m_fF - M.m_fF;
          else
            fDelta2 = M.m_fE - M1.m_fE;
        }
        else
          fDelta2 = fDelta1 + 1;
      } else
      {
        iterU = m_mapObjects.end();
        iterU--;
        M1 = (PDFCTM)iterU->first;
        if (M.m_vct.GetY() == 0.f)
          fDelta2 = M1.m_fF - M.m_fF;
        else
          fDelta2 = M.m_fE - M1.m_fE;
        fDelta1 = fDelta2 + 1;
      }

      if (m_pStrBlockControl)
      {
        if (fDelta1 > fDelta2)
        {
          if (fDelta2 <= m_pStrBlockControl->GetDeltaY(pPDFObj))
          {
            if (M.m_vct.IsParallel(iterU->first.m_vct))
            {
              if (M.m_vct.GetY() == 0.f)
                M.m_fF = iterU->first.m_fF;
              else
                M.m_fE = iterU->first.m_fE;
              pPDFObj->SetCTM(M);
            }
          }
        }
        else if (fDelta1 <= m_pStrBlockControl->GetDeltaY(pPDFObj))
        {
          if (M.m_vct.IsParallel(iterL->first.m_vct))
          {
            if (M.m_vct.GetY() == 0.f)
              M.m_fF = iterL->first.m_fF;
            else
              M.m_fE = iterL->first.m_fE;
            pPDFObj->SetCTM(M);
          }
        }
      }
    }
/////////////////////////////////////////////////////////////////////////////////////////////


  if (m_pStrBlockControl)
    m_pStrBlockControl->SetLanguageID(pPDFObj);
  m_mapObjects.insert(PDFObjectsMap::value_type(M, pPDFObj));

  return pPDFObj;

  //M = /*PDFMulMatrix(M, */m_Tm;//);

  //iter = m_mapObjects.find(M);
  //if (iter != m_mapObjects.end())
  //{
  //  //pPDFObj = (CPDFObjects *)iter->second;
  //  //if (!pPDFObj)
  //  //  return NULL;
  //  //pPDFObj = pPDFObj->GetLastObjects();
  //  //if (IsObjectEqual(pPDFObj))
  //  //{
  //  //  pPDFObj->AppendToValue(i_str.c_str());
  //  //  if (m_pStrBlockControl)
  //  //    m_pStrBlockControl->SetLanguageID(pPDFObj);
  //  //}
  //  //else
  //  //{
  //    CPDFObjects *pPDFObjNew = new CPDFObjects(this, M, i_str.c_str(), m_strFontName.c_str(), m_fFontSize,
  //                                        m_lPage, m_fWordSpacing, m_fCharSpacing, m_fHScaling);
  //    if (!pPDFObjNew)
  //      return false_a;
  //    //pPDFObj->AppendObjects(pPDFObjNew);
  //    if (m_pStrBlockControl)
  //      m_pStrBlockControl->SetLanguageID(pPDFObjNew);

  //    m_mapObjects.insert(PDFObjectsMap::value_type(M, pPDFObjNew));

  //    pPDFObj = pPDFObjNew; //returns pPDFObj
  //  //}
  //}
  //else
  //{
  //  PDFObjectsMap::iterator iterL, iterU;
  //  PDFCTM M1;
  //  double fDelta1, fDelta2;
  //  M1.fA = 0.f; M1.fB = -1.f; M1.fC = 1.f; M1.fD = 0.f; M1.fE = 0.f; M1.fF = 0.f;


  //  pPDFObj = new CPDFObjects(this, M, i_str.c_str(), m_strFontName.c_str(), m_fFontSize,
  //                            m_lPage, m_fWordSpacing, m_fCharSpacing, m_fHScaling);
  //  if (!pPDFObj)
  //    return NULL;

  //  if (m_mapObjects.size() > 0)
  //  {
  //    iterU = iterL = m_mapObjects.lower_bound(M);
  //    if (m_mapObjects.end() != iterL)
  //    {
  //      M1 = (PDFCTM)iterL->first;
  //      fDelta1 = M.fF - M1.fF;

  //      if (m_mapObjects.begin() != iterU)
  //      {
  //        iterU--;
  //        M1 = (PDFCTM)iterU->first;
  //        fDelta2 = M1.fF - M.fF;
  //      }
  //      else
  //        fDelta2 = fDelta1 + 1;
  //    } else
  //    {
  //      iterU = m_mapObjects.end();
  //      iterU--;
  //      M1 = (PDFCTM)iterU->first;
  //      fDelta2 = M1.fF - M.fF;
  //      fDelta1 = fDelta2 + 1;
  //    }

  //    if (m_pStrBlockControl)
  //    {
  //      if (fDelta1 > fDelta2)
  //      {
  //        if (fDelta2 <= m_pStrBlockControl->GetDeltaY(pPDFObj))
  //        {
  //          M.fF = iterU->first.fF;
  //          pPDFObj->SetCTM(M);
  //        }
  //      }
  //      else if (fDelta1 <= m_pStrBlockControl->GetDeltaY(pPDFObj))
  //      {
  //        M.fF = iterL->first.fF;
  //        pPDFObj->SetCTM(M);
  //      }
  //    }
  //  }

  //  if (m_pStrBlockControl)
  //    m_pStrBlockControl->SetLanguageID(pPDFObj);
  //  m_mapObjects.insert(PDFObjectsMap::value_type(M, pPDFObj));
  //}

  //return pPDFObj;
}

bool_a CPDFTemplate::IsObjectEqual(CPDFObjects *i_pPDFObj) //TO DO
{
  if (!i_pPDFObj)
    return false_a;
  if (m_strFontName.compare(i_pPDFObj->GetFontResName()) != 0)
    return false_a;
  if (i_pPDFObj->GetFontSize() != m_fFontSize)
    return false_a;
  if (i_pPDFObj->GetWordSpacing() != m_fWordSpacing)
    return false_a;
  if (i_pPDFObj->GetCharSpacing() != m_fCharSpacing)
    return false_a;
  if (i_pPDFObj->GetHorizontalSpacing() != m_fHScaling)
    return false_a;

  return true_a;
}
//#########################################################
//#########################################################
//#########################################################
void CPDFTemplate::Tm_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty() || i_pValues->size() < 6)
    throw "No values for \\Tm\\ operator!";
  if (i_pValues->size() > 6)
    i_pValues; // warning or error ???? now nothing // ToDo

  float fV[6];
  size_t i;
  string str;
  //char czTemp[200];
  for (i = 0; i < 6; i++)
  {
    str = (*i_pValues)[i_pValues->size() - i - 1];
    if (str.empty())
      throw "Value for \\Tm\\ operator is empty!";
    if (!PDFIsNumber(str))
      throw "Invalid value parameter for \\Tm\\ operator!";
    fV[6 - i - 1] = (float)atof(str.c_str());
  }

  m_Tm.m_fA = fV[0];
  m_Tm.m_fB = fV[1];
  m_Tm.m_fC = fV[2];
  m_Tm.m_fD = fV[3];
  m_Tm.m_fE = fV[4];
  m_Tm.m_fF = fV[5];

  m_Tlm = m_Tm = PDFMulMatrix(m_Tm, m_CTM);
}

void CPDFTemplate::Td_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty() || i_pValues->size() < 2)
    throw "No values for \\Td\\ operator!";
  if (i_pValues->size() > 2)
    i_pValues; // warning or error ???? now nothing // ToDo

  float fV[2];
  size_t i;
  string str;
  for (i = 0; i < 2; i++)
  {
    str = (*i_pValues)[i_pValues->size() - i - 1];
    if (str.empty())
      throw "Value for \\Td\\ operator is empty!";
    if (!PDFIsNumber(str))
      throw "Invalid value parameter for \\Td\\ operator!";
    fV[2 - i - 1] = (float)atof(str.c_str());
  }

  PDFCTM M(1.0, 0.0, 0.0, 1.0, fV[0], fV[1], 0.0, 0.0);

  //M = PDFMulMatrix(M, m_CTM);

  m_Tm = m_Tlm = PDFMulMatrix(M, m_Tlm);
}

void CPDFTemplate::TD_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty() || i_pValues->size() < 2)
    throw "No values for \\TD\\ operator!";
  if (i_pValues->size() > 2)
    i_pValues; // warning or error ???? now nothing // ToDo

  float fV[2];
  size_t i;
  string str;
  //char czTemp[200];
  for (i = 0; i < 2; i++)
  {
    str = (*i_pValues)[i_pValues->size() - i - 1];
    if (str.empty())
      throw "Value for \\TD\\ operator is empty!";
    if (!PDFIsNumber(str))
      throw "Invalid value parameter for \\TD\\ operator!";
    fV[2 - i - 1] = (float)atof(str.c_str());
  }

  PDFCTM M(1.0, 0.0, 0.0, 1.0, fV[0], fV[1], 0.0, 0.0);

  //M = PDFMulMatrix(M, m_CTM);

  m_fLeading = -fV[1];
  m_Tm = m_Tlm = PDFMulMatrix(M, m_Tlm);
}

void CPDFTemplate::T_STAR_Operator_Handler(vector<string> *i_pValues)
{
  PDFCTM M(1.0, 0.0, 0.0, 1.0, 0.0, -m_fLeading, 0.0, 0.0);

  //M = PDFMulMatrix(M, m_CTM);

  m_Tm = m_Tlm = PDFMulMatrix(M, m_Tlm);
  i_pValues;//because of warning
}

void CPDFTemplate::APOSTROPHE_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty())
    throw "No text for \\'\\ operator!";
  if (i_pValues->size() > 1)
    i_pValues; // warning or error ???? now nothing // ToDo

  //get last value
  string str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Text for \\'\\ operator is empty!";
  if (str[0] != '(' || str[str.size() - 1] != ')')
  {
    if (str[0] != '<' || str[str.size() - 1] != '>')
      throw "Invalid text parameter for \\'\\ operator!";
  }

  PDFCTM M(1.0, 0.0, 0.0, 1.0, 0.0, -m_fLeading, 0.0, 0.0);

  //M = PDFMulMatrix(M, m_CTM);

  m_Tm = PDFMulMatrix(M, m_Tm);

  CPDFObjects *pPDFObj = AddToObjects(str);
  if (!pPDFObj)
    throw "AddToObjects failed - \\'\\ operator";

  //move to end of string
  M.m_fA = 1.0; M.m_fB = 0.0;
  M.m_fC = 0.0; M.m_fD = 1.0;
  M.m_fE = pPDFObj->GetWidth(); M.m_fF = 0.0;

  //M = PDFMulMatrix(M, m_CTM);

  m_Tm = PDFMulMatrix(M, m_Tm);
}

void CPDFTemplate::QUOTES_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty() || i_pValues->size() < 3)
    throw "No values for \\\"\\ operator!";
  if (i_pValues->size() > 3)
    i_pValues; // warning or error ???? now nothing // ToDo

  //Aw
  string str = (*i_pValues)[i_pValues->size() - 2];
  if (PDFIsNumber(str))
    throw "Array for \\\"\\ operator contain invalid value!";
  m_fWordSpacing = (float)atof(str.c_str());

  //Ac
  str = (*i_pValues)[i_pValues->size() - 3];
  if (PDFIsNumber(str))
    throw "Array for \\\"\\ operator contain invalid value!";
  m_fCharSpacing = (float)atof(str.c_str());

  //get last value
  str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Text for \\\"\\ operator is empty!";
  if (str[0] != '(' || str[str.size() - 1] != ')')
  {
    if (str[0] != '<' || str[str.size() - 1] != '>')
      throw "Invalid text parameter for \\\"\\ operator!";
  }

  PDFCTM M(1.0, 0.0, 0.0, 1.0, 0.0, -m_fLeading, 0.0, 0.0);

  //M = PDFMulMatrix(M, m_CTM);

  m_Tm = PDFMulMatrix(M, m_Tm);

  CPDFObjects *pPDFObj = AddToObjects(str);
  if (!pPDFObj)
    throw "AddToObjects failed - \\\"\\ operator";

  //move to end of string
  M.m_fA = 1.0; M.m_fB = 0.0;
  M.m_fC = 0.0; M.m_fD = 1.0;
  M.m_fE = pPDFObj->GetWidth(); M.m_fF = 0.0; // TO DO: add vertical moving

  //M = PDFMulMatrix(M, m_CTM);

  m_Tm = PDFMulMatrix(M, m_Tm);
}

void CPDFTemplate::Tj_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty())
    throw "No text for \\Tj\\ operator!";
  if (i_pValues->size() > 1)
    i_pValues; // warning or error ???? now nothing // ToDo

  //get last value
  string str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Text for \\Tj\\ operator is empty!";

  if (str[0] != '(' || str[str.size() - 1] != ')')
  {
    if (str[0] != '<' || str[str.size() - 1] != '>')
      throw "Invalid text parameter for \\Tj\\ operator!";
  }

  CPDFObjects *pPDFObj = AddToObjects(str);
  if (!pPDFObj)
    throw "AddToObjects failed - \\Tj\\ operator";

  //move to end of string
  PDFCTM M(1.0, 0.0, 0.0, 1.0, pPDFObj->GetWidth(), 0.0, 0.0, 0.0);

  //M = PDFMulMatrix(M, m_CTM);

  m_Tm = PDFMulMatrix(M, m_Tm);
}

void CPDFTemplate::TJ_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty())
    throw "No array for \\TJ\\ operator!";
  if (i_pValues->size() > 1)
    i_pValues; // warning or error ???? now nothing // ToDo

  //get last value
  string str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Array for \\TJ\\ operator is empty!";
  if (str[0] != '[' || str[str.size() - 1] != ']')
    throw "Invalid array parameter for \\TJ\\ operator!";

  //erase '[' and ']'
  str.erase(0, 1);
  str.erase(str.size() - 1, 1);

  float fPos;
  size_t lIndex = 0;
  string strToken = PDFGetStringUpToWhiteSpace(str, lIndex);
  while (!strToken.empty())
  {
    if ((strToken[0] == '<' && strToken[strToken.size() - 1] == '>') ||
        (strToken[0] == '(' && strToken[strToken.size() - 1] == ')'))
    {
      CPDFObjects *pPDFObj = AddToObjects(strToken);
      if (!pPDFObj)
        throw "AddToObjects failed - \\TJ\\ operator";

      //move to end of string
      PDFCTM M(1.0, 0.0, 0.0, 1.0, pPDFObj->GetWidth(), 0.0, 0.0, 0.0);

      //M = PDFMulMatrix(M, m_CTM);

      m_Tm = PDFMulMatrix(M, m_Tm);
    }
    else if (PDFIsNumber(strToken))
    {
      fPos = (float)atof(strToken.c_str());
      fPos *= 0.001f;

      //move to end of string
      PDFCTM M(1.0, 0.0, 0.0, 1.0, -fPos, 0.0, 0.0, 0.0);

      //M = PDFMulMatrix(M, m_CTM);
      m_Tm = PDFMulMatrix(M, m_Tm);
    }

    strToken = PDFGetStringUpToWhiteSpace(str, lIndex);
  }
}

void CPDFTemplate::Tc_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty())
    throw "No value for \\Tc\\ operator!";
  if (i_pValues->size() > 1)
    i_pValues; // warning or error ???? now nothing // ToDo

  //get last value
  string str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Value for \\Tc\\ operator is empty!";
  if (!PDFIsNumber(str))
    throw "Invalid value parameter for \\Tc\\ operator!";

  m_fCharSpacing = (float)atof(str.c_str());
}

void CPDFTemplate::Tw_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty())
    throw "No value for \\Tw\\ operator!";
  if (i_pValues->size() > 1)
    i_pValues; // warning or error ???? now nothing // ToDo

  //get last value
  string str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Value for \\Tw\\ operator is empty!";
  if (!PDFIsNumber(str))
    throw "Invalid value parameter for \\Tw\\ operator!";

  m_fWordSpacing = (float)atof(str.c_str());
}

void CPDFTemplate::Tz_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty())
    throw "No value for \\Tz\\ operator!";
  if (i_pValues->size() > 1)
    i_pValues; // warning or error ???? now nothing // ToDo

  //get last value
  string str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Value for \\Tz\\ operator is empty!";
  if (!PDFIsNumber(str))
    throw "Invalid value parameter for \\Tz\\ operator!";

  m_fHScaling = (float)atof(str.c_str());
}

void CPDFTemplate::TL_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty())
    throw "No value for \\TL\\ operator!";
  if (i_pValues->size() > 1)
    i_pValues; // warning or error ???? now nothing // ToDo

  //get last value
  string str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Value for \\TL\\ operator is empty!";
  if (!PDFIsNumber(str))
    throw "Invalid value parameter for \\TL\\ operator!";

  m_fLeading = (float)atof(str.c_str());
}

void CPDFTemplate::Tf_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty() || i_pValues->size() < 2)
    throw "No value for \\Tf\\ operator!";
  if (i_pValues->size() > 2)
    i_pValues; // warning or error ???? now nothing // ToDo

  //get last value
  string str = (*i_pValues)[i_pValues->size() - 1];
  if (str.empty())
    throw "Value for \\Tf\\ operator is empty!";
  if (!PDFIsNumber(str))
    throw "Invalid value parameter for \\Tf\\ operator!";

  m_fFontSize = m_fLeading = (float)atof(str.c_str());
  m_strFontName = (*i_pValues)[i_pValues->size() - 2];
}

void CPDFTemplate::Tr_Operator_Handler(vector<string> *i_pValues)
{
  i_pValues;//because of warning
}

void CPDFTemplate::Ts_Operator_Handler(vector<string> *i_pValues)
{
  i_pValues;//because of warning
}

void CPDFTemplate::CM_Operator_Handler(vector<string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty() || i_pValues->size() < 6)
    throw "No values for \\Tm\\ operator!";
  if (i_pValues->size() > 6)
    i_pValues; // warning or error ???? now nothing // ToDo

  float fV[6];
  size_t i;
  string str;
  //char czTemp[200];
  for (i = 0; i < 6; i++)
  {
    str = (*i_pValues)[i_pValues->size() - i - 1];
    if (str.empty())
      throw "Value for \\Tm\\ operator is empty!";
    if (!PDFIsNumber(str))
      throw "Invalid value parameter for \\Tm\\ operator!";
    fV[6 - i - 1] = (float)atof(str.c_str());
  }

  PDFCTM M(fV[0], fV[1], fV[2], fV[3], fV[4], fV[5], 0.0, 0.0);

  m_CTM = PDFMulMatrix(M, m_CTM);
}

void CPDFTemplate::Do_Operator_Handler(std::vector<std::string> *i_pValues)
{
  if (!i_pValues || i_pValues->empty())
    throw "No values for \\Tm\\ operator!";
  if (i_pValues->size() > 1)
    i_pValues; // warning or error ???? now nothing // ToDo

  string str = (*i_pValues)[0];


  AddImageToObjects(str);

}

void CPDFTemplate::BI_Operator_Handler(std::vector<std::string> *i_pValues)
{
  i_pValues;
//  m_bInlineImage = true_a;
}

void CPDFTemplate::ID_Operator_Handler(std::vector<std::string> *i_pValues)
{
  i_pValues;
  m_bInlineImage = true_a;
}

void CPDFTemplate::EI_Operator_Handler(std::vector<std::string> *i_pValues)
{
  i_pValues;
  m_bInlineImage = false_a;
}

// mik -->
void CPDFTemplate::Restore_Operator_Handler(std::vector<std::string> *i_pValues)
{
  i_pValues;//because of warning
  m_cStoreRestoreContainer.Restore(this);
}
void CPDFTemplate::Store_Operator_Handler(std::vector<std::string> *i_pValues)
{
  i_pValues;//because of warning
  m_cStoreRestoreContainer.Store(this);
}
// mik <--





bool_a CPDFTemplate::GetAcroForm(std::string& o_strAcroForm)
{
  if (!m_pRootDic)
    return false_a;

  o_strAcroForm.clear();

  if (!m_pRootDic->HasKey(czACROFORM))
    return true_a;//no fields

  CPDFDictionary *pDic = m_pRootDic->GetSubDictionary(czACROFORM);
  if (!pDic)
  {
    if (!m_pRootDic->GetSubObject(czACROFORM, "<<", ">>", o_strAcroForm))
      return false_a;
  } else
  {
    if (!pDic->GetDictionary(o_strAcroForm))
      return false_a;
  }

  return true_a;
}

bool_a CPDFTemplate::GetDR(std::string& o_strDR)
{
  if (!m_pRootDic)
    return false_a;
  string strAcro;

  o_strDR.clear();
  if (!GetAcroForm(strAcro))
    return false_a;

  return ReadItem(strAcro, "/DR", "<<", ">>", true_a, o_strDR);
}

bool_a CPDFTemplate::GetDA(std::string& o_strDA)
{
  if (!m_pRootDic)
    return false_a;
  string strAcro;

  o_strDA.clear();
  if (!GetAcroForm(strAcro))
    return false_a;

  return ReadItem(strAcro, "/DA", "(", ")", true_a, o_strDA);
}

//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################


CPDFXObject::CPDFXObject(CPDFTemplate *i_pTemplate)
{
  m_lObjIndex = NOT_USED;
  m_bIsImage = false_a;
  m_pTemplate = i_pTemplate;
}

CPDFXObject::~CPDFXObject()
{
}

CPDFXObject *CPDFXObject::MakeXObject(CPDFTemplate *i_pTemplate, long i_lObjNum)
{
  CPDFXObject *pRet = new CPDFXObject(i_pTemplate);
  if (!pRet->ReadXObject(i_lObjNum))
  {
    delete pRet;
    return NULL;
  }

  return pRet;
}

bool_a CPDFXObject::ReadXObject(long i_lObjNum)
{
  m_lObjIndex = i_lObjNum;
  if (!m_pTemplate)
    return false_a;
  long lOff = m_pTemplate->GetObjOffset(i_lObjNum);
  if (lOff <= 0)
    return false_a;
  string strTemp;
  if (!m_pTemplate->ReadObject(lOff, strTemp))
    return false_a;

  char *pTemp = PDFGetNameValue(strTemp, "/Subtype");
  if (!pTemp)
    return false_a;
  bool_a bForm = false_a;
  if (0 == strcmp(pTemp, "/Image"))
    m_bIsImage = true_a;
  if (0 == strcmp(pTemp, "/Form"))
    bForm = true_a;
  free(pTemp);

  if (!m_bIsImage)
  {
    if (bForm)
    {
      m_ctmXObject;


      string strLen;
      long lTemp;
      long lLength = PDFGetObjValue(strTemp, "/Length", lTemp);
      if (lLength < 0 || lTemp < 0)
        lLength = PDFGetNumValue(strTemp, "/Length");
      else
      {
        lTemp = m_pTemplate->GetObjOffset(lLength);
        if (lTemp <= 0)
          return false_a;
        if (!m_pTemplate->ReadObject(lTemp, strLen))
          return false_a;
        lLength = PDFGetNumValue(strLen, "obj");
      }

      bool_a bFlatDecode = false_a;
      pTemp = PDFGetNameValue(strTemp, "/Filter");
      if (pTemp != NULL)
      {
        if (0 == strcmp(pTemp, "/FlateDecode"))
          bFlatDecode = true_a;
        free(pTemp);
      }

      basic_string <char>::iterator strIter, strIter1;
      basic_string <char>::size_type index = strTemp.find("stream");
      if (index == -1)
        return false_a;
      strTemp.erase(strTemp.begin(), strTemp.begin() + index + 6);

      strIter1 = strTemp.begin();
      if (*strIter1 == PDF_NL_CHR2)
        strTemp.erase(strIter1);
      else if (*strIter1 == PDF_NL_CHR1 && *(strIter1 + 1) == PDF_NL_CHR2)
        strTemp.erase(strIter1, strIter1 + 2);
      else if (*strIter1 == PDF_NL_CHR1)
        strTemp.erase(strIter1);

      strTemp.erase(strTemp.begin() + lLength, strTemp.end());

      if (bFlatDecode)
      {
        if (!CPDFDocument::UncompressStream(strTemp))
          return false_a;
      }

      m_strStream = strTemp;
    }
  }

  return true_a;
}

//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################

CPDFDictionary::CPDFDictionary(CPDFTemplate *i_pTemplate)
{
  //m_mapSubDictionary;
  m_strDictionary = "";
  m_pTemplate = i_pTemplate;
  m_bChanged = false_a;
}

CPDFDictionary::~CPDFDictionary()
{
  Clean();
}

void CPDFDictionary::Clean()
{
  CPDFDictionary *pDic = NULL;
  map<string, CPDFDictionary *>::iterator iter = m_mapSubDictionary.begin();
  for (; iter != m_mapSubDictionary.end(); iter++)
  {
    pDic = (CPDFDictionary *)iter->second;
    delete pDic;
  }
  m_mapSubDictionary.clear();
}

CPDFDictionary *CPDFDictionary::MakeDisctionary(CPDFTemplate *i_pTemplate, long i_lObjNum)
{
  CPDFDictionary *pRet = new CPDFDictionary(i_pTemplate);
  if (!pRet->ReadDictionary(i_lObjNum))
  {
    delete pRet;
    return NULL;
  }

  return pRet;
}

bool_a CPDFDictionary::ReadDictionary(long i_lObjNum)
{
  if (!m_pTemplate)
    return false_a;
  long lOff = m_pTemplate->GetObjOffset(i_lObjNum);
  if (lOff <= 0)
    return false_a;
  if (!m_pTemplate->ReadObject(lOff, m_strDictionary))
    return false_a;

  return true_a;
}

bool_a CPDFDictionary::HasKey(const char *i_pczKey)
{
  if (!CPDFDocument::PDFStrStrKey(m_strDictionary, i_pczKey))
    return false_a;
  return true_a;
}

CPDFDictionary *CPDFDictionary::GetSubDictionary(const char *i_pczKey)
{
  if (!i_pczKey)
    return NULL;
  CPDFDictionary *pDic = NULL;

  map<string, CPDFDictionary *>::iterator iter = m_mapSubDictionary.find(i_pczKey);
  if (iter != m_mapSubDictionary.end())
  {
    pDic = (CPDFDictionary *)iter->second;
    return pDic;
  }

  if (!HasKey(i_pczKey))
    return NULL;
  long lObj, lGen;
  lObj = PDFGetObjValue(m_strDictionary, i_pczKey, lGen);
  if (lObj < 0 || lGen < 0)
    return NULL;

  pDic = CPDFDictionary::MakeDisctionary(m_pTemplate, lObj);
  if (!pDic)
    return NULL;

  m_mapSubDictionary.insert(map<string, CPDFDictionary *>::value_type(i_pczKey, pDic));

  return pDic;
}

//CPDFDictionary *CPDFDictionary::GetSubDictionary(string i_strPath)
//{
//  if (i_strPath.empty())
//    return NULL;
//
//  CPDFDictionary *pDic = NULL;
//
//
//
//  string strTemp;
//  size_t index;
//  CPDFDictionary *pDic;
//
//  while (!i_strPath.empty())
//  {
//    index  = i_strPath.find(1, "/");//skip first '/'
//    strTemp.assign(i_strPath.begin(), i_strPath.begin() + index);
//    i_strPath.erase(0, index);
//
//    if (!HasKey(strTemp.c_str()))
//      return false_a;
//
//    pDic = GetSubDictionary(strTemp.c_str());
//    if (pDic)
//    {
//      if (i_strPath.empty())
//        return pDic;
//      else
//        return pDic->GetSubDictionary(i_strPath);
//    }
//
//    if (i_strPath.empty())
//    {
//      if (!GetSubObject(strTemp.c_str(), i_strStart.c_str(), i_strEnd.c_str(), strTemp))
//        return false_a;
//    }
//
//  }
//
//
//  map<string, CPDFDictionary *>::iterator iter = m_mapSubDictionary.find(i_pczKey);
//  if (iter != m_mapSubDictionary.end())
//  {
//    pDic = (CPDFDictionary *)iter->second;
//    return pDic;
//  }
//
//  if (!HasKey(i_pczKey))
//    return NULL;
//  long lObj, lGen;
//  lObj = PDFGetObjValue(m_strDictionary, i_pczKey, lGen);
//  if (lObj < 0 || lGen < 0)
//    return NULL;
//
//  pDic = CPDFDictionary::MakeDisctionary(m_pTemplate, lObj);
//  if (!pDic)
//    return NULL;
//
//  m_mapSubDictionary.insert(map<string, CPDFDictionary *>::value_type(i_pczKey, pDic));
//
//  return pDic;
//}

bool_a CPDFDictionary::GetSubObject(const char *i_pczKey, const char *i_pczBegin,
                                      const char *i_pczEnd, std::string& o_strSubDic)
{
  if (!m_pTemplate)
    return false_a;
  o_strSubDic.c_str();

  if (!HasKey(i_pczKey))
    return false_a;

  return m_pTemplate->ReadItem(m_strDictionary, i_pczKey, i_pczBegin, i_pczEnd, false_a, o_strSubDic);
}

long CPDFDictionary::GetNumValue(const char *i_pczKey)
{
  if (!m_pTemplate)
    return false_a;

  if (!HasKey(i_pczKey))
    return false_a;

  size_t index = CPDFDocument::PDFStrStrNumKey(m_strDictionary, i_pczKey);
  long lRet = atol(m_strDictionary.c_str() + index);

  return lRet;
}

//bool_a CPDFDictionary::Add(std::string i_strPath, std::string strValue, std::string i_strStart,
//                            std::string i_strEnd, bool_a i_bBegin)
//{
//  string strTemp;
//  size_t index;
//  CPDFDictionary *pDic;
//
//  while (!i_strPath.empty())
//  {
//    index  = i_strPath.find(1, "/");//skip first '/'
//    strTemp.assign(i_strPath.begin(), i_strPath.begin() + index);
//    i_strPath.erase(0, index);
//
//    if (!HasKey(strTemp.c_str()))
//      return false_a;
//    if (!i_strPath.empty())
//    {
//      pDic = GetSubDictionary(strTemp.c_str());
//      if (pDic)
//        return pDic->Add(i_strPath, i_strValue, i_strStart, i_strEnd, i_bBegin);
//    }
//
//    if (i_strPath.empty())
//    {
//      if (!GetSubObject(strTemp.c_str(), i_strStart.c_str(), i_strEnd.c_str(), strTemp))
//        return false_a;
//    }
//
//  }
//
//  return false_a;
//}

bool_a CPDFDictionary::GetDictionary(std::string& o_strSubDic)
{
  o_strSubDic = m_strDictionary;
  return true_a;
}

bool_a CPDFDictionary::SetDictionary(std::string& o_strSubDic)
{
  m_strDictionary = o_strSubDic;
  Clean();

  return true_a;
}

//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################


CPDFField::CPDFField()
  :m_Value("")
{
  m_pTemplate = NULL;
  m_pParent = NULL;
  m_lPage = -1;
  m_eFieldType = eFieldUnknown;
  m_bChecked = false_a;
  m_bChanged = false_a;
  m_lObjNum = NOT_USED;
  m_lGenNum = NOT_USED;
  m_strID.erase();
  m_strDA = "";
  m_lMaxLen = -1;
  m_ColorBG.fB = -1;
  m_ColorBG.fG = -1;
  m_ColorBG.fR = -1;
  m_ColorBC.fB = -1;
  m_ColorBC.fG = -1;
  m_ColorBC.fR = -1;
  m_strFontID = "";
  m_lFontSize = 0;
  m_lQ = 0;
  m_fBorderWidth = 1.f;
  m_lAP_NObjNum = NOT_USED;
  m_lAP_NGenNum = NOT_USED;
  m_lFontObjNum = NOT_USED;
  m_pYes = NULL;
  m_pNo = NULL;
  m_strAP = "";
  m_lFf = 0;
  m_Rec.fBottom = -1;
  m_Rec.fLeft = -1;
  m_Rec.fRight = -1;
  m_Rec.fTop = -1;
};

CPDFField::~CPDFField()
{
  char *pChild;
  for (size_t i = 0; i < m_Childs.size(); i++)
  {
    pChild = m_Childs[i];
    free(pChild);
  }
  m_Childs.clear();
  if (m_pYes)
    free(m_pYes);
  if (m_pNo)
    free(m_pNo);
};


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CPDFObjects::CPDFObjects(CPDFTemplate *i_pTemplate, PDFCTM i_CTM, const char *i_pczValue, const char *i_pczFontName,
                         float i_fSize, long i_lPage, float i_fWSpacing, float i_fChSpacing, float i_fHScaling)
{
  m_strValue = i_pczValue;
  m_strWValue = L"";
  m_strFontResName = i_pczFontName;
  m_fFontSize = i_fSize;
  m_CTM = i_CTM;
  m_pNextObjects = NULL;
  m_fWordSpacing = i_fWSpacing;
  m_fCharSpacing = i_fChSpacing;
  m_fHScaling = i_fHScaling;
  m_lPage = i_lPage;
  m_lLanguageID = -1;
  m_pTemplate = i_pTemplate;

  if (m_pTemplate)
  {
    CPDFBaseFont *pFont = m_pTemplate->GetFont(i_lPage, m_strFontResName.c_str());
    if (pFont)
      m_strFontBaseName = pFont->GetFontBaseName();
  }

  CountWidth();
  ConvertToUnicode();
}

CPDFObjects::~CPDFObjects()
{
}

const char *CPDFObjects::GetFontResName()
{
  return m_strFontResName.c_str();
}

void CPDFObjects::SetFontResName(const char *i_pczFontName)
{
  if (!i_pczFontName)
    return;
  m_strFontResName = i_pczFontName;
  CountWidth();
  ConvertToUnicode();
}
const char *CPDFObjects::GetFontBaseName()
{
  return m_strFontBaseName.c_str();
}

void CPDFObjects::SetFontBaseName(const char *i_pczFontName)
{
  if (!i_pczFontName)
    return;
  m_strFontBaseName = i_pczFontName;
  CountWidth();
  ConvertToUnicode();
}

float CPDFObjects::GetFontSize()
{
  return m_fFontSize;
}

void CPDFObjects::SetFontSize(float i_fSize)
{
  m_fFontSize = i_fSize;
  CountWidth();
}

long CPDFObjects::GetLanguageID()
{
  return m_lLanguageID;
}

void CPDFObjects::SetLanguageID(long i_lID)
{
  m_lLanguageID = i_lID;
}

const wchar_t *CPDFObjects::GetWValue()
{
  return m_strWValue.c_str();
}

const char *CPDFObjects::GetValue()
{
  return m_strValue.c_str();
}

void CPDFObjects::SetValue(const char *i_pczValue)
{
  if (!i_pczValue)
    return;
  m_strValue = i_pczValue;
  CountWidth();
  ConvertToUnicode();
}

void CPDFObjects::AppendToValue(const char *i_pczValue)
{
  if (!i_pczValue)
    return;
  m_strValue.append(i_pczValue);
  CountWidth();
  ConvertToUnicode();
}

float CPDFObjects::GetWordSpacing()
{
  return m_fWordSpacing;
}

void CPDFObjects::SetWordSpacing(float i_fSpacing)
{
  m_fWordSpacing = i_fSpacing;
  CountWidth();
}

float CPDFObjects::GetCharSpacing()
{
  return m_fCharSpacing;
}

void CPDFObjects::SetCharSpacing(float i_fSpacing)
{
  m_fCharSpacing = i_fSpacing;
  CountWidth();
}

float CPDFObjects::GetHorizontalSpacing()
{
  return m_fHScaling;
}

void CPDFObjects::SetHorizontalrSpacing(float i_fScaling)
{
  m_fHScaling = i_fScaling;
  CountWidth();
}

long CPDFObjects::GetPage()
{
  return m_lPage;
}

void CPDFObjects::SetPage(long i_lPage)
{
  m_lPage = i_lPage;
  ConvertToUnicode();
}

PDFCTM CPDFObjects::GetCTM()
{
  return m_CTM;
}

void CPDFObjects::SetCTM(PDFCTM i_CTM)
{
  m_CTM = i_CTM;
}

void CPDFObjects::AppendObjects(CPDFObjects *i_pPDFObjects)
{
  CPDFObjects *pPDFObj = GetLastObjects();
  pPDFObj->m_pNextObjects = i_pPDFObjects;
}

CPDFObjects *CPDFObjects::GetNextObjects()
{
  return this->m_pNextObjects;
}

CPDFObjects *CPDFObjects::GetLastObjects()
{
  CPDFObjects *pPDFObj = this;
  while (pPDFObj->GetNextObjects())
  {
    pPDFObj = pPDFObj->m_pNextObjects;
  }

  return pPDFObj;
}

const CPDFBaseFont *CPDFObjects::GetFont()
{
  if (!m_pTemplate)
    return NULL;

  return m_pTemplate->GetFont(m_lPage, m_strFontResName.c_str());
}

void CPDFObjects::CountWidth()
{
  string strTmp, strToken;
  float fPos;
  bool_a bHex;
  size_t lIndex, lC;
  CPDFBaseFont *pFont = m_pTemplate->GetFont(m_lPage, m_strFontResName.c_str());
  //PDFCTM M1;

  if (!pFont)
    return;
  m_fWidth = 0;
  lIndex = 0;
  strToken = PDFGetStringUpToWhiteSpace(m_strValue, lIndex);
  while (!strToken.empty())
  {
    if (strToken[0] == '<' && strToken[strToken.size() - 1] == '>')
      bHex = true_a;
    else if (strToken[0] == '(' && strToken[strToken.size() - 1] == ')')
      bHex = false_a;
    else
    {
      //M1.fA = m_fFontSize * (m_fHScaling / 100.f); M1.fB = 0.0;
      //M1.fC = 0.0; M1.fD = m_fFontSize;
      //M1.fE = 0.0; M1.fF = 0.0;// TO DO: add Trise

      //M1 = PDFMulMatrix(M1, m_CTM);

        //m_fWidth += PDFGetFontStringWidth(strTmp.c_str(), strTmp.size(), m_fFontSize/* * M1.fD*/, m_fCharSpacing,
        //                                  m_fWordSpacing, m_fHScaling, pFont);
      fPos = (float)atof(strToken.c_str());
      m_fWidth -= fPos * 0.001f;
        //strTmp.clear();
      strToken = PDFGetStringUpToWhiteSpace(m_strValue, lIndex);
    }
    strToken.erase(0, 1);
    strToken.erase(strToken.size() - 1, 1);

    PDFRemoveEscapeChar(strToken);

    if (bHex)
    {
      //size_t lC;
      //string strHEX;
      if (pFont->GetToUnicodeSize())
        lC = pFont->GetToUnicodeLen();
      //else
      //  lC = 2;

      //for (i = 0; i < strToken.size(); i+=lC)
      //{
      //  strHEX.assign(strToken.begin() + i, strToken.begin() + i + lC);
      //  strTmp += PDFConvertHEXToLong(strHEX);
      //}
      //strToken.clear();

      m_fWidth += pFont->GetFontHexStringWidth(strToken.c_str(), m_fFontSize, m_fCharSpacing,
                                          m_fWordSpacing, m_fHScaling);
      //m_fWidth += PDFGetFontHexStringWidth(strToken.c_str(), strToken.size(), lC, m_fFontSize/* * M1.fD*/, m_fCharSpacing,
      //                                  m_fWordSpacing, m_fHScaling, pFont);
    }
    else
    {
      m_fWidth += pFont->GetFontStringWidth(strToken.c_str(), m_fFontSize, m_fCharSpacing,
                                          m_fWordSpacing, m_fHScaling);
      //m_fWidth += PDFGetFontStringWidth(strToken.c_str(), strToken.size(), m_fFontSize/* * M1.fD*/, m_fCharSpacing,
      //                                  m_fWordSpacing, m_fHScaling, pFont);
      //strTmp += strToken;
    }


    strToken = PDFGetStringUpToWhiteSpace(m_strValue, lIndex);
  }

  //M1.fA = m_fFontSize * (m_fHScaling / 100.f); M1.fB = 0.0;
  //M1.fC = 0.0; M1.fD = m_fFontSize;
  //M1.fE = 0.0; M1.fF = 0.0;// TO DO: add Trise

  //M1 = PDFMulMatrix(M1, m_CTM);

  //m_fWidth += PDFGetFontStringWidth(strTmp.c_str(), strTmp.size(), m_fFontSize/* * M1.fD*/, m_fCharSpacing,
  //                                  m_fWordSpacing, m_fHScaling, pFont);
}

bool CPDFObjects::ConvertToUnicode()
{

  CPDFBaseFont *pFont = m_pTemplate->GetFont(m_lPage, m_strFontResName.c_str());
  if (!pFont)
    return false_a;

  m_strWValue.clear();

  map<string, string>::iterator iterUnicode;
  string strToken, strTmp;
  bool_a bHex;
  size_t lIndex = 0;
  char czTemp[100];

  strToken = PDFGetStringUpToWhiteSpace(m_strValue, lIndex);
  while (!strToken.empty())
  {
    if (strToken[0] == '<' && strToken[strToken.size() - 1] == '>')
      bHex = true_a;
    else if (strToken[0] == '(' && strToken[strToken.size() - 1] == ')')
      bHex = false_a;
    else if (PDFIsNumber(strToken))
    {
      strToken = PDFGetStringUpToWhiteSpace(m_strValue, lIndex);
      continue;
    }
    else
      return false_a;
    strToken.erase(0, 1);
    strToken.erase(strToken.size() - 1, 1);

    PDFRemoveEscapeChar(strToken);

    for (size_t i = 0; i < strToken.size(); i++)
    {

      if (pFont->GetToUnicodeSize())
      {
        if (bHex)
        {
          strTmp = strToken[i];
          i++;
          for (size_t j = 1; j < pFont->GetToUnicodeLen(); j++)
          {
            if (i < strToken.size())
              strTmp += strToken[i];
            else
              strTmp += "00";
            i++;
          }
          i--;
        }
        else
        {
          sprintf(czTemp, "%02x", (long)strToken[i]);
          strTmp = czTemp;
          i++;

          for (size_t j = 1; j < pFont->GetToUnicodeLen() / 2; j++)
          {
            if (i < strToken.size())
            {
              sprintf(czTemp, "%02x", (long)strToken[i]);
              strTmp += czTemp;
            }
            else
              strTmp += "\0";
            i++;
          }
          i--;

        }

        PDFMakeUpper(strTmp);

        strTmp = pFont->GetToUnicode(strTmp);

        m_strWValue += ((wchar_t)PDFConvertHEXToLong(strTmp));
      }
      else
      {
        strTmp = PDFGetGlyphNameUnicode(pFont->GetFontEncoding()->
                              GetCharCodeToGlyphName((unsigned char)strToken[i]));
        m_strWValue += ((wchar_t)PDFConvertHEXToLong(strTmp));
        //m_strWValue += ((wchar_t)(unsigned char)strToken[i]);
      }
    }

    strToken = PDFGetStringUpToWhiteSpace(m_strValue, lIndex);
  }

  return true_a;
}

float CPDFObjects::CalculateScale()
{
  float fRet = 0.f;
  PDFCTM M = GetCTM();
  if (M.m_vct.GetX() != 0.f && M.m_vct.GetY() == 0.f)
    fRet = M.m_vct.GetX();
  else if (M.m_vct.GetX() == 0.f && M.m_vct.GetY() != 0.f)
    fRet = M.m_vct.GetY();
  else
    fRet = sqrt((M.m_vct.GetX() * M.m_vct.GetX()) + (M.m_vct.GetY() * M.m_vct.GetY()));
  return fRet;
}

bool_a CPDFObjects::AppendToBegin()
{
  bool_a bRet = false_a;
  PDFCTM M = GetCTM();
  if (M.m_vct.GetY() <= 0.f && M.m_vct.GetX() >= 0.f)
    bRet = false_a;
  if (M.m_vct.GetY() < 0.f && M.m_vct.GetX() < 0.f)
    bRet = false_a;
  if (M.m_vct.GetY() >= 0.f && M.m_vct.GetX() <= 0.f)
    bRet = true_a;
  if (M.m_vct.GetY() > 0.f && M.m_vct.GetX() > 0.f)
    bRet = true_a;
  return bRet;
}

bool_a CPDFObjects::HasSameDirection(CPDFObjects *i_pPDFObj)
{
  if (((this->GetCTM().m_vct.GetY() < 0.f && i_pPDFObj->GetCTM().m_vct.GetY() < 0.f) ||
    (this->GetCTM().m_vct.GetY() >= 0.f && i_pPDFObj->GetCTM().m_vct.GetY() >= 0.f)) &&
    (this->GetCTM().m_vct.GetX() < 0.f && i_pPDFObj->GetCTM().m_vct.GetX() < 0.f) ||
    (this->GetCTM().m_vct.GetX() >= 0.f && i_pPDFObj->GetCTM().m_vct.GetX() >= 0.f))
  {
      return true_a;
  }
  return false_a;
}

bool_a CPDFObjects::CompareObjects(CPDFObjects *i_pPDFObj, float &o_fXWidth, float &o_fYWidth, float &o_fXHalfWidth, float &o_fYHalfWidth)
{
  if (!i_pPDFObj)
    return false_a;
  CPDFObjects *pObjFirst, *pObjSecond;

  if (AppendToBegin())
  {
    pObjFirst = this;
    pObjSecond = i_pPDFObj;
  }
  else
  {
    pObjFirst = i_pPDFObj;
    pObjSecond = this;
  }

  string strFont = pObjFirst->GetFontResName();
  CPDFBaseFont *pFont = (CPDFBaseFont *)pObjFirst->GetFont();
  if (!pFont)
    return false_a;

//  float fScale = pObjFirst->CalculateScale();
  float fSpaceWidth = pFont->GetFontSpaceWidth(pObjFirst->GetFontSize(), pObjFirst->GetHorizontalSpacing());
//  fSpaceWidth *= /*fScale * */(pObjFirst->GetHorizontalSpacing() / 100.f);

  float fLength = pObjFirst->GetWidth()/* * fScale * (pObjFirst->GetHorizontalSpacing() / 100.f)*/;

  //fX = Xo + S1 * t
  //fY = Yo + S2 * t
  o_fXWidth = pObjFirst->GetCTM().m_fE + 
    (pObjFirst->GetCTM().m_vct.GetX() == 0.f ? 0.f : (pObjFirst->GetCTM().m_vct.GetX() * (fLength + fSpaceWidth + fSpaceWidth / 2)));
  o_fYWidth = pObjFirst->GetCTM().m_fF + 
    (pObjFirst->GetCTM().m_vct.GetY() == 0.f ? 0.f : pObjFirst->GetCTM().m_vct.GetY() * (fLength + fSpaceWidth + fSpaceWidth / 2));
  o_fXHalfWidth = pObjFirst->GetCTM().m_fE + 
    (pObjFirst->GetCTM().m_vct.GetX() == 0.f ? 0.f : pObjFirst->GetCTM().m_vct.GetX() * (fLength + fSpaceWidth / 2));
  o_fYHalfWidth = pObjFirst->GetCTM().m_fF + 
    (pObjFirst->GetCTM().m_vct.GetY() == 0.f ? 0.f : pObjFirst->GetCTM().m_vct.GetY() * (fLength + fSpaceWidth / 2));

  bool x = pObjFirst->GetCTM().m_vct.GetX() < 0 ? (o_fXWidth > pObjSecond->GetCTM().m_fE) : (o_fXWidth < pObjSecond->GetCTM().m_fE);
  bool y = pObjFirst->GetCTM().m_vct.GetY() < 0 ? (o_fYWidth > pObjSecond->GetCTM().m_fF) : (o_fYWidth < pObjSecond->GetCTM().m_fF);

  if (strFont.compare(pObjSecond->GetFontResName()) != 0 ||
      pObjFirst->GetFontSize() * pObjFirst->CalculateScale() != pObjSecond->GetFontSize() * pObjSecond->CalculateScale() ||
      pObjFirst->AppendToBegin() != pObjSecond->AppendToBegin() ||
      !pObjFirst->GetCTM().InLineVector(pObjSecond->GetCTM()) ||
      x || y)
    return true_a;

  return false_a;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

float CPDFStrBlockControl::GetDeltaY(CPDFObjects *io_pPDFObj)
{
  //PDFCTM M(io_pPDFObj->GetFontSize() * (io_pPDFObj->GetHorizontalSpacing() / 100.f), 0.0, 0.0, io_pPDFObj->GetFontSize(), 0.0, 0.0, 0.0, 0.0);

  //M = PDFMulMatrix(M, io_pPDFObj->GetCTM());

  float fSize = io_pPDFObj->CalculateScale() * io_pPDFObj->GetFontSize() * (io_pPDFObj->GetHorizontalSpacing() / 100.f);

  return fSize / 2;
}

bool CPDFStrBlockControl::CanJoin(CPDFObjects *io_pPDFObjFirst, CPDFObjects *io_pPDFObjSecond, wstring &io_wstrJoin)
{
  if (!io_pPDFObjFirst || !io_pPDFObjSecond)
    return false;

  string strFont = io_pPDFObjFirst->GetFontResName();
  CPDFBaseFont *pFont = (CPDFBaseFont *)io_pPDFObjFirst->GetFont();
  if (!pFont)
    return false;

  wstring strW = io_pPDFObjFirst->GetWValue();

//  float fScale = io_pPDFObjFirst->CalculateScale();
  //get width of space
  float fSpaceWidth = pFont->GetFontSpaceWidth(io_pPDFObjFirst->GetFontSize(), io_pPDFObjFirst->GetHorizontalSpacing());
//  fSpaceWidth *= /*fScale * */(io_pPDFObjFirst->GetHorizontalSpacing() / 100.f);

  float fLength = io_pPDFObjFirst->GetWidth()/* * fScale * (io_pPDFObjFirst->GetHorizontalSpacing() / 100.f)*/;

  //fX = Xo + S1 * t
  //fY = Yo + S2 * t
  float fX = io_pPDFObjFirst->GetCTM().m_fE + 
    (io_pPDFObjFirst->GetCTM().m_vct.GetX() == 0.f ? 0.f : (io_pPDFObjFirst->GetCTM().m_vct.GetX() * (fLength + fSpaceWidth + fSpaceWidth / 2)));
  float fY = io_pPDFObjFirst->GetCTM().m_fF + 
    (io_pPDFObjFirst->GetCTM().m_vct.GetY() == 0.f ? 0.f : io_pPDFObjFirst->GetCTM().m_vct.GetY() * (fLength + fSpaceWidth + fSpaceWidth / 2));
  float fX1 = io_pPDFObjFirst->GetCTM().m_fE + 
    (io_pPDFObjFirst->GetCTM().m_vct.GetX() == 0.f ? 0.f : io_pPDFObjFirst->GetCTM().m_vct.GetX() * (fLength + fSpaceWidth / 2));
  float fY1 = io_pPDFObjFirst->GetCTM().m_fF + 
    (io_pPDFObjFirst->GetCTM().m_vct.GetY() == 0.f ? 0.f : io_pPDFObjFirst->GetCTM().m_vct.GetY() * (fLength + fSpaceWidth / 2));

  bool x = io_pPDFObjFirst->GetCTM().m_vct.GetX() < 0 ? (fX > io_pPDFObjSecond->GetCTM().m_fE) : (fX < io_pPDFObjSecond->GetCTM().m_fE);
  bool y = io_pPDFObjFirst->GetCTM().m_vct.GetY() < 0 ? (fY > io_pPDFObjSecond->GetCTM().m_fF) : (fY < io_pPDFObjSecond->GetCTM().m_fF);

  if (strFont.compare(io_pPDFObjSecond->GetFontResName()) != 0 || // Is it same font?
      /*io_pPDFObjFirst->GetFontSize() * io_pPDFObjFirst->CalculateScale() != io_pPDFObjSecond->GetFontSize() * io_pPDFObjSecond->CalculateScale() ||*/ // Is it same font size?
      io_pPDFObjFirst->AppendToBegin() != io_pPDFObjSecond->AppendToBegin() ||
      !io_pPDFObjFirst->GetCTM().InLineVector(io_pPDFObjSecond->GetCTM()) || //same line ?
      x ||//Is second string 'near'first string?
      y)//(1.5 * width of space)
  {
    return false;
  }
  else if (fX1 <= io_pPDFObjSecond->GetCTM().m_fE && fX >= io_pPDFObjSecond->GetCTM().m_fE &&
          fY1 >= io_pPDFObjSecond->GetCTM().m_fF && fY <= io_pPDFObjSecond->GetCTM().m_fF)
  {
    io_wstrJoin = L" ";
    return true;
  }

  io_wstrJoin = L"";
  return true;
}

bool CPDFStrBlockControl::CanDivide(CPDFObjects *io_pPDFObjFirst, CPDFObjects *io_pPDFObjSecond, wstring &o_wstrJoin)
{
  if (!io_pPDFObjFirst || !io_pPDFObjSecond)
    return true;

  string strFont = io_pPDFObjFirst->GetFontResName();
  CPDFBaseFont *pFont = (CPDFBaseFont *)io_pPDFObjFirst->GetFont();
  if (!pFont)
    return false;

  wstring strW = io_pPDFObjFirst->GetWValue();

//  float fScale = io_pPDFObjFirst->CalculateScale();
  //get width of space
  float fSpaceWidth = pFont->GetFontSpaceWidth(io_pPDFObjFirst->GetFontSize(), io_pPDFObjFirst->GetHorizontalSpacing());
//  fSpaceWidth *= /*fScale * */(io_pPDFObjFirst->GetHorizontalSpacing() / 100.f);

  float fLength = io_pPDFObjFirst->GetWidth()/* * fScale * (io_pPDFObjFirst->GetHorizontalSpacing() / 100.f)*/;

  //fX = Xo + S1 * t
  //fY = Yo + S2 * t
  float fX = io_pPDFObjFirst->GetCTM().m_fE + 
    (io_pPDFObjFirst->GetCTM().m_vct.GetX() == 0.f ? 0.f : (io_pPDFObjFirst->GetCTM().m_vct.GetX() * (fLength + fSpaceWidth + fSpaceWidth / 2)));
  float fY = io_pPDFObjFirst->GetCTM().m_fF + 
    (io_pPDFObjFirst->GetCTM().m_vct.GetY() == 0.f ? 0.f : io_pPDFObjFirst->GetCTM().m_vct.GetY() * (fLength + fSpaceWidth + fSpaceWidth / 2));
  float fX1 = io_pPDFObjFirst->GetCTM().m_fE + 
    (io_pPDFObjFirst->GetCTM().m_vct.GetX() == 0.f ? 0.f : io_pPDFObjFirst->GetCTM().m_vct.GetX() * (fLength + fSpaceWidth / 2));
  float fY1 = io_pPDFObjFirst->GetCTM().m_fF + 
    (io_pPDFObjFirst->GetCTM().m_vct.GetY() == 0.f ? 0.f : io_pPDFObjFirst->GetCTM().m_vct.GetY() * (fLength + fSpaceWidth / 2));

  bool x = io_pPDFObjSecond->GetCTM().m_vct.GetX() < 0 ? (fX > io_pPDFObjSecond->GetCTM().m_fE) : (fX < io_pPDFObjSecond->GetCTM().m_fE);
  bool y = io_pPDFObjSecond->GetCTM().m_vct.GetY() < 0 ? (fY > io_pPDFObjSecond->GetCTM().m_fF) : (fY < io_pPDFObjSecond->GetCTM().m_fF);

  if (strFont.compare(io_pPDFObjSecond->GetFontResName()) != 0 || // Is it same font?
      /*io_pPDFObjFirst->GetFontSize() * io_pPDFObjFirst->CalculateScale() != io_pPDFObjSecond->GetFontSize() * io_pPDFObjSecond->CalculateScale() ||*/ // Is it same font size?
      io_pPDFObjFirst->AppendToBegin() != io_pPDFObjSecond->AppendToBegin() ||
      !io_pPDFObjFirst->GetCTM().InLineVector(io_pPDFObjSecond->GetCTM()) || //same line ?
      x ||//Is second string 'near'first string?
      y)//(1.5 * width of space)
  {
    return true;
  }
  else if (fX1 <= io_pPDFObjSecond->GetCTM().m_fE && fX >= io_pPDFObjSecond->GetCTM().m_fE &&
          fY1 >= io_pPDFObjSecond->GetCTM().m_fF && fY <= io_pPDFObjSecond->GetCTM().m_fF)
  {
    o_wstrJoin = L" ";
    return false;
  }

  o_wstrJoin = L"";
  return false;
}

void CPDFField::CopyAttributes(CPDFField &i_Field)
{

  m_eFieldType = i_Field.m_eFieldType;
  m_bChecked = i_Field.m_bChecked;
  m_bChanged = i_Field.m_bChanged;
  m_Value = i_Field.m_Value;
  m_lObjNum = i_Field.m_lObjNum;
  m_lGenNum = i_Field.m_lGenNum;
  m_strDA = i_Field.m_strDA;
  m_ColorBG = i_Field.m_ColorBG;
  m_ColorBC = i_Field.m_ColorBC;
  m_strAP = i_Field.m_strAP;
  m_strFontID = i_Field.m_strFontID;

  m_lFontObjNum = i_Field.m_lFontObjNum;
  //m_strFontName = i_Field.m_strFontName;

  m_lFontSize = i_Field.m_lFontSize;
  m_lMaxLen = i_Field.m_lMaxLen;
  m_strID = i_Field.m_strID;

  //std::vector<char *> m_Childs;

  m_lQ = i_Field.m_lQ;
  m_fBorderWidth = i_Field.m_fBorderWidth;

  m_lAP_NObjNum = i_Field.m_lAP_NObjNum;
  m_lAP_NGenNum = i_Field.m_lAP_NGenNum;

  m_lFf = i_Field.m_lFf;

  m_pYes = PDFStrDup(i_Field.m_pYes);
  m_pNo = PDFStrDup(i_Field.m_pNo);

  m_lPage = i_Field.m_lPage;

  m_pTemplate = i_Field.m_pTemplate;

  //CPDFField *m_pParent;

  m_Rec = i_Field.m_Rec;
}

/*
bool_a CPDFField::Overflow(const char *i_pValue)
{
  char *pValue, *pDA, *pPos, *pTemp, *pObj, *pBaseFont, czX[20], czY[20], czH[20] = {"0"}, czTemp[200];
  long lSize, lObj = NOT_USED, lGen, lIndex, lTemp;
  float fX = 4, fY = 2, fWidth, f;
  vector <string> values;
  vector <float> Xshift;
  string strValue;

  fX = i_pfield->m_fBorderWidth * 2.f;
  fY = i_pfield->m_fBorderWidth * 2.f;

  pPos = ReadDict(m_strDR.c_str(), czFONT);
  if (!pPos)
  {
    lObj = PDFGetObjValue(m_strDR, czFONT, lGen);
    if (lObj < 0 || lGen < 0)
      return NULL;
    pPos = (char *)PDFMalloc(100);
    if (!pPos)
      return NULL;
    sprintf(pPos, "/Font %ld %ld R", lObj, lGen);

    lGen = GetObjOffset(lObj);
    pBaseFont = ReadObject(lGen, lSize);
    if (!pBaseFont)
      return NULL;

    lObj = PDFGetObjValue(pBaseFont, i_pfield->m_strFontID.c_str(), lGen);
    if (lObj < 0 || lGen < 0)
      return NULL;
    free(pBaseFont);
  }
  else
  {
    lObj = PDFGetObjValue(pPos, i_pfield->m_strFontID.c_str(), lGen);
    if (lObj < 0 || lGen < 0)
      return NULL;
  }

  lTemp = GetObjOffset(lObj);
  pBaseFont = ReadObject(lTemp, lSize);
  if (!pBaseFont)
    return NULL;

  pObj = PDFGetNameValue(pBaseFont, "/BaseFont");
  free(pBaseFont);
  if (!pObj)
    return NULL;
  pBaseFont = pObj + 1;
  lIndex = CPDFBaseFont::GetBaseFontIndex(pBaseFont);
  free(pObj);
  if (lIndex >= 0 && lIndex < sSTANDART_FONT_COUNT)
  {
    float fHeight, fDesc;
    fHeight = (float)((STANDART_FONT_FONTBBOX[lIndex][3] - STANDART_FONT_FONTBBOX[lIndex][1]) / 1000.0 );
    fHeight *= i_pfield->m_lFontSize;
    fDesc = (float)(sSTANDART_FONT_DESCENT[lIndex] / 1000.0);
    fDesc *= i_pfield->m_lFontSize;
    sprintf(czH, "%.4f", fHeight);
    fHeight = (i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom) - fHeight;
    if (i_pfield->m_lFf & 0x1000)//multiline
    {
      fY = fHeight - fY;

      char *pSpace = NULL, *pB, *pE;
      fWidth = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - (2 * i_pfield->m_fBorderWidth * 2.f);
      pE = pB = i_pfield->m_pValue;
      while (0 != *pE)
      {
        if (PDF_SPACE_CHR == *pE || PDF_TAB_CHR == *pE)
          pSpace = pE;
        strValue.append(pE, 1);
        f = GetStringWidth(strValue.c_str(), i_pfield->m_lFontSize, lIndex);
        if (f > fWidth || *pE == PDF_NL_CHR1)
        {
          if (!pSpace || f < fWidth)
            pSpace = pE;
          else
            pSpace++;
          pE = pSpace;
          strValue.erase();
          strValue.append(pB, pE - pB);

          pObj = PDFescapeSpecialChars(strValue.c_str());
          strValue.assign(pObj);
          free(pObj);

          basic_string <char>::size_type index = strValue.find_last_of(" ");
          if (index != -1 && index == (strValue.length() - 1))
            strValue.erase(index, 1);

          switch (i_pfield->m_lQ)
          {
          case 2:
            f = GetStringWidth(strValue.c_str(), i_pfield->m_lFontSize,lIndex);
            fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - f - (i_pfield->m_fBorderWidth * 2.f);
            break;
          case 1:
            f = GetStringWidth(strValue.c_str(), i_pfield->m_lFontSize,lIndex);
            fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - f;
            fX /= 2;
            break;
          case 0:
          default:
            break;
          }
          strValue.append("\\012");

          values.push_back(strValue);
          Xshift.push_back(fX);
          strValue.erase();
          if (*pSpace == PDF_NL_CHR1)
          {
            pSpace++;
          }

          pB = pE = pSpace;
          pSpace = NULL;
        }
        else
          pE++;
        if (0 == *pE && !strValue.empty())
        {
          switch (i_pfield->m_lQ)
          {
          case 2:
            f = GetStringWidth(strValue.c_str(), i_pfield->m_lFontSize,lIndex);
            fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - f - (i_pfield->m_fBorderWidth * 2.f);
            break;
          case 1:
            f = GetStringWidth(strValue.c_str(), i_pfield->m_lFontSize,lIndex);
            fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - f;
            fX /= 2;
            break;
          case 0:
          default:
            break;
          }

          pObj = PDFescapeSpecialChars(strValue.c_str());
          strValue.assign(pObj);
          free(pObj);

          values.push_back(strValue);
          Xshift.push_back(fX);
        }
      }
    }
    else
    {
      fHeight /= 2;
      fY = fHeight - fDesc;
      switch (i_pfield->m_lQ)
      {
      case 2:
        fWidth = GetStringWidth(i_pfield->m_pValue, i_pfield->m_lFontSize,lIndex);
        fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - fWidth - fX;
        break;
      case 1:
        fWidth = GetStringWidth(i_pfield->m_pValue, i_pfield->m_lFontSize,lIndex);
        fX = (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - fWidth;
        fX /= 2;
        break;
      case 0:
      default:
        break;
      }

      pObj = PDFescapeSpecialChars(i_pfield->m_pValue);
      strValue.assign(pObj);
      free(pObj);

//      strValue = i_pfield->m_pValue;
      values.push_back(strValue);
      Xshift.push_back(fX);
    }
  }
  else
  {
    pObj = PDFescapeSpecialChars(i_pfield->m_pValue);
    strValue.assign(pObj);
    free(pObj);

//    strValue = i_pfield->m_pValue;
    values.push_back(strValue);
    Xshift.push_back(fX);
  }

  strValue.erase();

  if (i_pfield->m_ColorBG.fR >= 0 && i_pfield->m_ColorBG.fG >= 0 && i_pfield->m_ColorBG.fB >= 0)
  {
    sprintf(czTemp, "%.4f %.4f %.4f rg ", i_pfield->m_ColorBG.fR, i_pfield->m_ColorBG.fG, i_pfield->m_ColorBG.fB);
    strValue.append(czTemp);
    sprintf(czTemp, "0 0 %.4f %.4f re f ", (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft),
            (i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom));
    strValue.append(czTemp);
  }
  if (i_pfield->m_ColorBC.fR >= 0 && i_pfield->m_ColorBC.fG >= 0 && i_pfield->m_ColorBC.fB >= 0)
  {
    sprintf(czTemp, "%.4f %.4f %.4f RG %.4f w ", i_pfield->m_ColorBC.fR,
              i_pfield->m_ColorBC.fG, i_pfield->m_ColorBC.fB, i_pfield->m_fBorderWidth);
    strValue.append(czTemp);
    sprintf(czTemp, "%.4f %.4f %.4f %.4f re s ", i_pfield->m_fBorderWidth / 2, i_pfield->m_fBorderWidth / 2,
              (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - i_pfield->m_fBorderWidth,
              (i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom) - i_pfield->m_fBorderWidth);
    strValue.append(czTemp);
  }


  strValue.append("/Tx BMC q ");

  sprintf(czTemp, "%.4f %.4f %.4f %.4f re W n ", i_pfield->m_fBorderWidth, i_pfield->m_fBorderWidth,
            (i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft) - (2 * i_pfield->m_fBorderWidth),
            (i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom) - (2 * i_pfield->m_fBorderWidth));
  strValue.append(czTemp);

  strValue.append("BT\n");

  strValue.append(i_pfield->m_strDA.c_str());
  strValue.append("\n");

  for (long ii = 0; ii < values.size(); ii++)
  {
    if (!ii)
    {
      fX = Xshift.at(ii);
      sprintf(czX, "%.4f", fX);
      sprintf(czY, "%.4f", fY);
      strValue.append(czX);
      strValue.append(" ");
      strValue.append(czY);
      strValue.append(" Td\n");
      strValue.append(czH);
      strValue.append(" TL\n");
      strValue.append("(");
      strValue.append(values.at(ii));
      strValue.append(") Tj\n");
    }
    else
    {
      sprintf(czX, "%.4f", Xshift.at(ii) - fX);
      fX = Xshift.at(ii);
      sprintf(czY, "%ld", 0);
      strValue.append(czX);
      strValue.append(" ");
      strValue.append(czY);
      strValue.append(" Td\n");
      strValue.append("(");
      strValue.append(values.at(ii));
      strValue.append(") '\n");
    }
  }
  strValue.append("ET Q EMC");

  if (Encrypt())
  {
    lSize = strValue.length();
    pObj = (char *)PDFMalloc(strValue.length() + 1);
    if (!pObj)
      return NULL;
    strcpy(pObj, strValue.c_str());
    pObj[strValue.length()] = 0;
    if (!PDFEncrypt((PDFDocPTR)m_pDoc, (unsigned char *)pObj, strValue.length(), m_lLastObj, 0))
      return false_a;

//    pTemp = PDFescapeSpecialCharsBinary(pObj, lSize, &lSize);

    strValue.assign(pObj, lSize);
    free(pObj);
  }

  lSize = PDFStrLenC(" 0 obj\n<< /Length  /Subtype /Form /BBox [     ] /Resources << /ProcSet [ /PDF /Text ]  >> >> \nstream\n\nendstream\nendobj")
                      + PDFStrLenC(pPos)+ strValue.length() + 20 + (4 * 25);

  pObj = pTemp = (char *)PDFMalloc(lSize);
  if (!pTemp)
  {
    free(pPos);
    return NULL;
  }

  sprintf(pTemp, "%ld 0 obj\n<< /Length %ld /Subtype /Form /BBox [ %f %f %.4f %.4f ] /Resources << /ProcSet [ /PDF /Text ] %s >> >> \nstream\n",//%s\nendstream\nendobj",
    m_lLastObj, strValue.length(), 0.f, 0.f, (float)(i_pfield->m_Rec.fRight - i_pfield->m_Rec.fLeft),
  (float)(i_pfield->m_Rec.fTop - i_pfield->m_Rec.fBottom), pPos, strValue.c_str());
  lSize = PDFStrLenC(pTemp);
  pObj += lSize;

  memcpy(pObj, strValue.c_str(), strValue.length());
  pObj += strValue.length();
  lSize += strValue.length();

  memcpy(pObj, "\nendstream\nendobj", PDFStrLenC("\nendstream\nendobj"));
  lSize += PDFStrLenC("\nendstream\nendobj");

  free(pPos);
  i_pfield->m_lAP_NObjNum = m_lLastObj;
  i_pfield->m_lAP_NGenNum = 0;
  m_lLastObj++;

  o_lSize = lSize;

  return pTemp;
}
*/
