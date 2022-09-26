#include "pdferr.h"
#include "pdffnc.h"

#if defined(LINUX_OS) || defined (MAC_OS_MWE)
#include <unistd.h>
#elif defined(WIN32)
#include <io.h>
#endif

#include <string>

using namespace std;

void PDFSetPageLayout(PDFDocPTR i_pPDFDoc, ePAGELAYOUTATTR i_ePageLayout)
{
  i_pPDFDoc->strCatalog.ePageLayout = i_ePageLayout;
}

void PDFSetPageMode(PDFDocPTR i_pPDFDoc, ePAGEMODEATTR i_ePageMode)
{
  i_pPDFDoc->strCatalog.ePageMode = i_ePageMode;
}

void PDFSetViewerPreferences(PDFDocPTR i_pPDFDoc, PPDFViewerPreferences i_pViewerPrfs)
{
  memcpy(&((i_pPDFDoc->strCatalog).strViewerPreferences), i_pViewerPrfs, sizeof(PDFViewerPreferences));
}


void PDFSetVersion(PDFDocPTR i_pPDFDoc, long i_lMajorLevel, long i_lMinorLeverl)
{
  if (i_pPDFDoc->lMajorLevel < i_lMajorLevel)
  {
    i_pPDFDoc->lMajorLevel = i_lMajorLevel;
    i_pPDFDoc->lMinorLever = i_lMinorLeverl;
  }
  else
  if (i_pPDFDoc->lMinorLever < i_lMinorLeverl)
    i_pPDFDoc->lMinorLever = i_lMinorLeverl;
}

void PDFSetOwnerPassword(PDFDocPTR i_pPDFDoc, char *i_pOwnerPassword)
{
  size_t len;
  if (!i_pOwnerPassword)
    return;
  len = strlen(i_pOwnerPassword);
  len = (len > 32) ? 32 : len;
  memcpy(i_pPDFDoc->ucOwnerPassw, i_pOwnerPassword, len);
}

void PDFSetUserPassword(PDFDocPTR i_pPDFDoc, char *i_pUserPassword)
{
  size_t len;
  if (!i_pUserPassword)
    return;
  len = strlen(i_pUserPassword);
  len = (len > 32) ? 32 : len;
  memcpy(i_pPDFDoc->ucUserPassw, i_pUserPassword, len);
}

void PDFSetPermissionPassword(PDFDocPTR i_pPDFDoc, long i_lPermission)
{
  short iTemp;
//1.-2. bit: must be 0
  iTemp = 0;
//3. bit: Print the document
  if (i_lPermission & ePrint)
  {
    iTemp |= 0x0004;
  }
//4. bit: Modify the contents of the document by operations other than
//        those controlled by bits 6, 9, and 11.
  if (i_lPermission & eModify)
  {
    iTemp |= 0x0008;
  }
//5. bit: Copy or otherwise extract text and graphics from the document
//        by operations other than that controlled by bit 10
  if (i_lPermission & eCopy)
  {
    iTemp |= 0x0010;
  }
//6. bit: Add or modify text annotations, fill in interactive form fields,
//        and, if bit 4 is also set, create or modify interactive form
//        fields (including signature fields).
  if (i_lPermission & eAnnotation)
  {
    iTemp |= 0x0020;
  }
//7.-8. bit: Reserved; must be 1.
  iTemp |= 0x00c0;
//9. bit: Fill in exidting interactive form fields (including signature fields),
//        even if bit 6 is clear.
  if (i_lPermission & eForm)
  {
    iTemp |= 0x0100;
  }
//10. bit: Extract text and graphics (in support of accessibility to disabled
//         users or for other purposes).
  if (i_lPermission & eExtract)
  {
    iTemp |= 0x0200;
  }
//11. bit: Assemble the document (insert, rotate, or delete pages and
//         create bookmarks or thumbnail images), even if bit 4 is clear.
  if (i_lPermission & eAssemble)
  {
    iTemp |= 0x0400;
  }
//12. bit: Print the document to a representation from which a faithful
//         digital copy of the PDF content could be generated. When this
//         bit is clear (and bit 3 is set), printing is limited to a
//         low-level representation of the appearance, possibly of
//         degraded quality.
  if (i_lPermission & ePrint2)
  {
    iTemp |= 0x0800;
  }
//13.-32. bit: Reserved: must be1.
  iTemp |= 0xf000;


  i_pPDFDoc->iPermission = iTemp;
}








#define lALLOCSTEP    50
#define lNUMSIZE      50
#define lBOOLSIZE     5
#define lATTRSIZE     15


bool_a IsWhiteSpace(char i_cChar)
{
  if (czWHITE_SPACE_CHAR1 == i_cChar || czWHITE_SPACE_CHAR2 == i_cChar ||
      czWHITE_SPACE_CHAR3 == i_cChar || czWHITE_SPACE_CHAR4 == i_cChar ||
      czWHITE_SPACE_CHAR5 == i_cChar || czWHITE_SPACE_CHAR6 == i_cChar)
    return true_a;
  return false_a;
}

bool_a IsDelimiter(char i_cChar)
{
  if (czDELIMITER_CHAR1 == i_cChar || czDELIMITER_CHAR2 == i_cChar ||
      czDELIMITER_CHAR3 == i_cChar || czDELIMITER_CHAR4 == i_cChar ||
      czDELIMITER_CHAR5 == i_cChar || czDELIMITER_CHAR6 == i_cChar ||
      czDELIMITER_CHAR7 == i_cChar || czDELIMITER_CHAR8 == i_cChar ||
      czDELIMITER_CHAR9 == i_cChar || czDELIMITER_CHAR10 == i_cChar)
    return true_a;
  return false_a;
}

size_t PDFStrStrKey(string& i_str, const char *i_pczFind, size_t i_index)
{
  size_t pos, pos1, actPos;
  long lDic, lObj;
  if (i_str.empty() || !i_pczFind)
    return string::npos;

  actPos = i_index;
  pos = i_str.find(i_pczFind, actPos);
  while (pos != string::npos)
  {
    lObj = lDic = 0;
    pos1 = i_str.find("<<", actPos);
    while (pos1 != string::npos && pos1 < pos)
    {
      lDic++;
      pos1++;
      pos1 = i_str.find("<<", pos1);
    }
    pos1 = i_str.find(">>", actPos);
    while (pos1 != string::npos && pos1 < pos)
    {
      lDic--;
      pos1++;
      pos1 = i_str.find(">>", pos1);
    }

    pos1 = pos + PDFStrLenC(i_pczFind);
    if (IsDelimiter(i_str.at(pos1)) || IsWhiteSpace(i_str.at(pos1)))
    {
      if (1 == lDic || 0 == lDic)
        return pos;
    }

    actPos = pos + 1;
    pos = i_str.find(i_pczFind, actPos);
  }

  return string::npos;
}

size_t PDFStrStrBoolKey(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, actPos;
  if (!i_pczFind || i_str.empty())
    return string::npos;

  actPos = 0;
  pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  while (pos != string::npos)
  {
    pos += strlen(i_pczFind) * sizeof(char);
    while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
        pos++;
    if ('f' == i_str.at(pos) || 'F' == i_str.at(pos) || 't' == i_str.at(pos) || 'T' == i_str.at(pos))
      break;
    actPos = pos + 1;
    pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  }

  return pos;
}

size_t PDFStrStrNumKey(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, actPos;
  if (!i_pczFind || i_str.empty())
    return string::npos;

  actPos = 0;
  pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  while (pos != string::npos)
  {
    pos += strlen(i_pczFind) * sizeof(char);
    while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
        pos++;
    if (('0' <= i_str.at(pos) && '9' >= i_str.at(pos)) ||
        (('0' <= i_str.at(pos + 1) && '9' >= i_str.at(pos + 1)) && ('-' == i_str.at(pos) || '+' == i_str.at(pos) || '.' == i_str.at(pos))) ||
        (('0' <= i_str.at(pos + 2) && '9' >= i_str.at(pos + 2)) && ('-' == i_str.at(pos) || '+' == i_str.at(pos)) && ('.' == i_str.at(pos + 1)))
        )
      break;
    actPos = pos + 1;
    pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  }
  return pos;
}

size_t PDFStrStrStringKey(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, actPos;
  if (!i_pczFind || i_str.empty())
    return string::npos;

  actPos = 0;
  pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  while (pos != string::npos)
  {
    pos += strlen(i_pczFind) * sizeof(char);
    while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
        pos++;
    if ('(' == i_str.at(pos))
      break;
    actPos = pos + 1;
    pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  }
  return pos;
}

size_t PDFStrStrHEXStringKey(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, actPos;
  if (!i_pczFind || i_str.empty())
    return string::npos;

  actPos = 0;
  pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  while (pos != string::npos)
  {
    pos += strlen(i_pczFind) * sizeof(char);
    while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
        pos++;
    if ('<' == i_str.at(pos))
      break;
    actPos = pos + 1;
    pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  }
  return pos;
}

size_t PDFStrStrNameKey(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, actPos;
  if (!i_pczFind || i_str.empty())
    return string::npos;

  actPos = 0;
  pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  while (pos != string::npos)
  {
    pos += strlen(i_pczFind) * sizeof(char);
    while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
        pos++;
    if ('/' == i_str.at(pos))
      break;
    actPos = pos + 1;
    pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  }
  return pos;
}

size_t PDFStrStrArrayKey(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, actPos;
  if (!i_pczFind || i_str.empty())
    return string::npos;

  actPos = 0;
  pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  while (pos != string::npos)
  {
    pos += strlen(i_pczFind) * sizeof(char);
    while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
        pos++;
    if ('[' == i_str.at(pos))
      break;
    actPos = pos + 1;
    pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  }
  return pos;
}

size_t PDFStrStrDicKey(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, actPos;
  if (!i_pczFind || i_str.empty())
    return string::npos;

  actPos = 0;
  pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  while (pos != string::npos)
  {
    pos += strlen(i_pczFind) * sizeof(char);
    while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
        pos++;
    if ('<' == i_str.at(pos) && '<' == i_str.at(pos + 1))
      break;
    actPos = pos + 1;
    pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  }
  return pos;
}

bool_a PDFGetEndChar(char &io_cEnd)
{
  switch (io_cEnd)
  {
  case '(':
    io_cEnd = ')';
    return true_a;
  case '<':
    io_cEnd = '>';
    return true_a;
  case '[':
    io_cEnd = ']';
    return true_a;
  }
  return false_a;
}

bool_a PDFIsNumber(string i_str)
{
  if (i_str.empty())
    return false_a;
  if ((i_str[0] >= '0' && i_str[0] <= '9')
    || i_str[0] == '-'
    || i_str[0] == '+'
    || i_str[0] == '.')
    return true_a;
  return false_a;
}

bool_a PDFIncHEXChar(char &io_cHEX)
{
  io_cHEX++;
  if (io_cHEX > '9' && io_cHEX < 'A')
  {
    io_cHEX = 'A';
    return false_a;
  }
  if (io_cHEX > 'f')
  {
    io_cHEX = '0';
    return true_a;
  }
  if (io_cHEX > 'F')
  {
    io_cHEX = '0';
    return true_a;
  }
  return false_a;
}

void PDFIncHEX(string &io_strHex)
{
  char czTmp;
  string::reverse_iterator iter;
  for (iter = io_strHex.rbegin(); iter != io_strHex.rend(); iter++)
  {
    czTmp = *iter;
    if (!PDFIncHEXChar(czTmp))
    {
      *iter = czTmp;
      return;
    }
    *iter = czTmp;
  }
  io_strHex.insert(0, "1");
}

long PDFConvertHEXToLong(string i_strHex)
{
  long lNum, lRet = 0, i;
  long lSize = (long)i_strHex.size() - 1;
  for (i = lSize; i >= 0; i--)
  {
    if (i_strHex[i] >= 'a' && i_strHex[i] <= 'f')
      lNum = 10 + (i_strHex[i] - 'a');
    else if (i_strHex[i] >= 'A' && i_strHex[i] <= 'F')
      lNum = 10 + (i_strHex[i] - 'A');
    else if (i_strHex[i] >= '0' && i_strHex[i] <= '9')
      lNum = (i_strHex[i] - '0');
    else
      return 0;

    lRet += lNum * (long)pow((double)16,(lSize - i));
  }

  return lRet;
}

long PDFConvertOCTToLong(string i_strOct)
{
  long lNum, lRet = 0, i;
  long lSize = (long)i_strOct.size() - 1;
  for (i = lSize; i >= 0; i--)
  {
    if (i_strOct[i] >= '0' && i_strOct[i] <= '9')
      lNum = (i_strOct[i] - '0');
    else
      return 0;

    lRet += lNum * (long)pow((double)8,(lSize - i));
  }

  return lRet;
}

void PDFMakeUpper(string& sText)
{
  string::iterator iterator;
  for (iterator = sText.begin(); iterator != sText.end(); iterator++)
    *iterator = toupper(*iterator);
}

void PDFRemoveEscapeChar(string &i_str)
{
  basic_string <char>::size_type index;

  index = i_str.find(czESCAPE_CHAR);
  while (index != string::npos)
  {
    if (index == i_str.size() - 1)
      break;

    if (IsDelimiter(i_str[index + 1]))
      i_str.erase(index, 1);
    else if (i_str[index + 1] >= '0' && i_str[index + 1] <= '7') //octal number
    {
      i_str.erase(index, 1);
      string strOct("");
      strOct += i_str[index];

      if (i_str[index + 1] >= '0' && i_str[index + 1] <= '7')
      {
        i_str.erase(index, 1);
        strOct += i_str[index];
      }
      if (i_str[index + 1] >= '0' && i_str[index + 1] <= '7')
      {
        i_str.erase(index, 1);
        strOct += i_str[index];
      }
      i_str.erase(index, 1);

      unsigned char uczOct[2];
      uczOct[0] = (unsigned char)PDFConvertOCTToLong(strOct);
      uczOct[1] = 0;
      i_str.insert(index, (char *)&uczOct);
    }
    else
    {
      i_str.erase(index, 1);
      if (index < i_str.size())
      {
        switch (i_str[index])
        {
        case 'r':
          i_str.erase(index, 1);
          i_str.insert(index, "\r");
          break;
        case 'n':
          i_str.erase(index, 1);
          i_str.insert(index, "\n");
          break;
        case 'f':
          i_str.erase(index, 1);
          i_str.insert(index, "\f");
          break;
        case 'b':
          i_str.erase(index, 1);
          i_str.insert(index, "\b");
          break;
        case 't':
          i_str.erase(index, 1);
          i_str.insert(index, "\t");
          break;
        }
      }
    }

    index++;//case '\\'
    index = i_str.find(czESCAPE_CHAR, index);
  }

}

string PDFGetStringUpToWhiteSpace(string& i_str, size_t &i_lIndex)
{
  string strRet = "";

  if (i_str.empty() || i_lIndex >= i_str.size())
    return strRet;

  i_lIndex = i_str.find_first_not_of(WHITE_SPACES, i_lIndex);
  if (i_lIndex == string::npos)
    i_lIndex = i_str.size();

  //for (; i_lIndex < i_str.size(); i_lIndex++)
  //{
  //  if (!IsWhiteSpace(i_str[i_lIndex]))
  //    break;
  //}

  size_t lS = i_lIndex;
  char cEnd = i_str[i_lIndex];
  bool_a bSet = PDFGetEndChar(cEnd);

  if (i_lIndex < i_str.size())
    i_lIndex++;
  string strDel("\x00\x09\x0a\x0c\x0d\x20\x28\x3c\x5b\x7b\x2f\x5c", 12);

  for (; i_lIndex < i_str.size(); i_lIndex++)
  {
    if (!bSet)
    {
      i_lIndex = i_str.find_first_of(strDel, i_lIndex);
      if (i_lIndex == string::npos)
        i_lIndex = i_str.size();
      //if (IsWhiteSpace(i_str[i_lIndex]) || (lS != i_lIndex && (i_str[i_lIndex] == czDELIMITER_CHAR1 ||
      //    i_str[i_lIndex] == czDELIMITER_CHAR3 || i_str[i_lIndex] == czDELIMITER_CHAR5 ||
      //    i_str[i_lIndex] == czDELIMITER_CHAR7 || i_str[i_lIndex] == czDELIMITER_CHAR9 ||
      //    i_str[i_lIndex] == czESCAPE_CHAR)))
        break;
    }
    else
    {
      i_lIndex = i_str.find_first_of(cEnd, i_lIndex);
      if (i_lIndex == string::npos)
        i_lIndex = i_str.size();

      if (i_str[i_lIndex] == cEnd && i_lIndex > lS &&
          (
           (i_str[i_lIndex - 1] != '\\') ||
           ((i_str[i_lIndex - 1] == '\\' && i_str[i_lIndex - 2] == '\\'))
          )
         )
      {
        i_lIndex++;
        break;
      }
    }
  }

  if (i_lIndex > i_str.size())
    i_lIndex = i_str.size();

  if (lS < i_lIndex)
    strRet.assign(i_str.begin() + lS, i_str.begin() + i_lIndex);

  return strRet;
}

bool_a PDFIsPDFOperator(string i_str)
{
  if (i_str.empty())
    return false_a;
  long i = 0;
  while (pczOperatorsList[i][0] != '\0')
  {
    if (!i_str.compare(pczOperatorsList[i]))
      return true_a;
    i++;
  }
  return false_a;
}

long PDFGetNumValue(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos;
  long lRet = NOT_USED;
  if (!i_pczFind || i_str.empty())
    return lRet;

  pos = PDFStrStrNumKey(i_str, i_pczFind);
  if (pos == string::npos)
    return lRet;
  lRet = atol(i_str.data() + pos);
  //if (0 == lRet)
  //  lRet = NOT_USED;
  return lRet;
}

long PDFGetObjValue(std::string& i_str, const char *i_pczFind, long& o_lGenNum)
{
  size_t pos = string::npos;
  long lRet = NOT_USED;
  o_lGenNum = NOT_USED;
  if (!i_pczFind)
    return lRet;

  pos = PDFStrStrNumKey(i_str, i_pczFind);
  if (pos == string::npos)
    return lRet;
  lRet = atol(i_str.data() + pos);
  if (0 == lRet)
    lRet = NOT_USED;
  while (i_str.at(pos) >= '0' && i_str.at(pos) <= '9')
      pos++;
  while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
      pos++;
  if ('0' > i_str.at(pos) || '9' < i_str.at(pos))
  {
    lRet = -1;
    return lRet;
  }
  o_lGenNum = atol(i_str.data() + pos);
  return lRet;
}

frec_a PDFGetRectValue(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos;
  short sIndex = 0;
  float fRet[4];
  frec_a rec = {0};
  if (!i_pczFind)
    return rec;

  pos = PDFStrStrArrayKey(i_str, i_pczFind);
  if (pos == string::npos)
    return rec;

  if ('[' != i_str.at(pos))
    return rec;

  pos++;
  while (i_str.at(pos) != ']' && pos < i_str.size())
  {
    if (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
    {
      pos++;
      continue;
    }
    if (sIndex > 3)
      return rec;
    fRet[sIndex] = (float)atof(i_str.data() + pos);
    //if (fRet[sIndex] < 0)
    //  return rec;
    sIndex++;
    while (i_str.at(pos) != ' ' && i_str.at(pos) != ']')
    {
      if (i_str.at(pos) != '.' && i_str.at(pos) != ',' && (i_str.at(pos) < '0' || i_str.at(pos) > '9') && i_str.at(pos) != '-' && i_str.at(pos) != '+')
        return rec;
      pos++;
    }
  }

  rec.fLeft = fRet[0];
  rec.fBottom = fRet[1];
  rec.fRight = fRet[2];
  rec.fTop = fRet[3];
  return rec;
}

PdfRgb PDFGetRGBValue(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos;
  short sIndex = 0;
  float fRet[3];
  PdfRgb rgb = {-1, -1, -1};
  if (!i_pczFind)
    return rgb;

  pos = PDFStrStrArrayKey(i_str, i_pczFind);
  if (pos == string::npos)
    return rgb;
  if ('[' != i_str.at(pos))
    return rgb;
  pos++;
  while (i_str.at(pos) != ']' && pos < i_str.size())
  {
    if (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
    {
      pos++;
      continue;
    }
    if (sIndex > 2)
      return rgb;
    fRet[sIndex] = (float)atof(i_str.data() + pos);
    if (fRet[sIndex] < 0)
      return rgb;
    sIndex++;
    while (i_str.at(pos) != ' ' && i_str.at(pos) != ']')
    {
      if (i_str.at(pos) != '.' && i_str.at(pos) != ',' && (i_str.at(pos) < '0' || i_str.at(pos) > '9'))
        return rgb;
      pos++;
    }
  }

  rgb.fR = fRet[0];
  rgb.fG = fRet[1];
  rgb.fB = fRet[2];
  return rgb;
}


bool_a PDFGetBoolValue(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos;
  char czBool[lBOOLSIZE + 1] = "";
  long lIndex = 0;
  bool_a bFind = false_a;
  if (!i_pczFind || i_str.empty())
    return false_a;

  pos = PDFStrStrBoolKey(i_str, i_pczFind);
  if (pos == string::npos)
    return false_a;

  while (pos < i_str.size())
  {
    if ((i_str.at(pos) != '\n' && i_str.at(pos) != '\r' && i_str.at(pos) != ' ' && i_str.at(pos) != '\t') || bFind)
    {
      if ((i_str.at(pos) >= 'a' && i_str.at(pos) <= 'z') || (i_str.at(pos) >= 'A' && i_str.at(pos) <= 'Z'))
      {
        czBool[lIndex++] = i_str.at(pos);
        bFind = true_a;
      }
      else
      {
        if ((0 == strcmp(czTRUE_U, czBool)) || (0 == strcmp(czTRUE_L, czBool)))
          return true_a;
      }
    }
    pos++;
  }

  return false_a;
}

bool_a PDFGetTextValue(std::string& i_str, const char *i_pczKey, std::string& o_strValue)
{
  size_t pos, b, e;
  bool_a bFind = false_a;
  if (!i_pczKey || i_str.empty())
    return false_a;

  pos = PDFStrStrStringKey(i_str, i_pczKey);
  if (pos == string::npos)
    return false_a;

  b = i_str.find('(', pos);
  if (b == string::npos)
    return false_a;
  b++;
  e = b;
  while (!bFind)
  {
    e = i_str.find(')', e);
    if (e == string::npos)
      return false_a;
    if (i_str.at(e-1) == '\\')
      e++;
    else
      bFind = true_a;
  }
  size_t size = (e - b);
  if (size >= 0)
  {
    o_strValue.assign(i_str.data() + b, size);
    return true_a;
  }
  return false_a;
}

bool_a PDFGetTextValue(std::string& i_str, const char *i_pczKey, CPDFString& o_strValue)
{
  size_t pos, b, e;
  bool_a bFind = false_a;
  if (!i_pczKey || i_str.empty())
    return false_a;

  pos = PDFStrStrStringKey(i_str, i_pczKey);
  if (pos == string::npos)
    return false_a;

  b = i_str.find('(', pos);
  if (b == string::npos)
    return false_a;
  b++;
  e = b;
  while (!bFind)
  {
    e = i_str.find(')', e);
    if (e == string::npos)
      return false_a;
    if (i_str.at(e-1) == '\\')
      e++;
    else
      bFind = true_a;
  }
  size_t size = (e - b);
  if (size >= 0)
  {
    pos = i_str.find("\xFE\xFF", b);
    if (pos == b) {
      if (size > 2)
      {
        CPDFString strRet((wchar_t *)(i_str.data() + b + 2), (size / 2) - 2);
        o_strValue = strRet;
      } else {
        o_strValue.SetString("");
      }
    } else {
      CPDFString strRet(i_str.data() + b, size);
      o_strValue = strRet;
    }
    return true_a;
  }
  return false_a;
}

char *PDFGetTextHEXValue(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, b = string::npos, e = string::npos, actPos;
  bool_a bFind = false_a;
  if (!i_pczFind)
    return NULL;

  actPos = 0;
  pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  while (pos != string::npos)
  {
    pos += strlen(i_pczFind) * sizeof(char);
    while (i_str.at(pos) == ' ' || i_str.at(pos) == '\t' || i_str.at(pos) == '\n' || i_str.at(pos) == '\r')
        pos++;
    if ('<' == i_str.at(pos) || '[' == i_str.at(pos))
      break;
    actPos = pos + 1;
    pos = PDFStrStrKey(i_str, i_pczFind, actPos);
  }
  if (pos == string::npos)
    return NULL;

  b = i_str.find('<', pos);
  if (b == string::npos)
    return NULL;
  b++;
  e = b;
  while (!bFind)
  {
    e = i_str.find('>', e);
    if (e == string::npos)
      return NULL;
    if (i_str.at(e-1) == '\\')
      e++;
    else
      bFind = true_a;
  }
  size_t size = (e - b);
  if (size >= 0)
  {
    char *pRet = (char*)PDFMalloc(size + 1);
    if (!pRet)
      return NULL;
    strncpy(pRet, i_str.data() + b, size);
    pRet[size] = 0;
    return pRet;
  }
  return NULL;
}

char *PDFGetNameValue(std::string& i_str, const char *i_pczFind)
{
  size_t pos = string::npos, b = string::npos, e = string::npos;
  if (!i_pczFind)
    return NULL;

  pos = PDFStrStrNameKey(i_str, i_pczFind);
  if (pos == string::npos)
    return NULL;
  if (i_str.at(pos) != '/')
    return NULL;
  b = pos;
  pos++;

  while (i_str.at(pos) != '\n' && i_str.at(pos) != '\r' && i_str.at(pos) != ' ' && i_str.at(pos) != '\t' &&
          i_str.at(pos) != '/' && i_str.at(pos) != '>')
    pos++;
  e = pos;

  size_t size = e - b;
  if (size > 0)
  {
    char *pRet = (char*)PDFMalloc(size + 1);
    if (!pRet)
      return NULL;
    strncpy(pRet, i_str.data() + b, size);
    pRet[size] = 0;
    return pRet;
  }
  return NULL;
}

string PDFReadXrefObj(FILE *i_pFile, size_t i_Pos, size_t i_Size)
{
  string strRet("");

  char cTemp;
  char czXref[sizeof(czXREF) + 1] = "";
  short sIndex = 0;
  long lDic = 0;

  if (!i_pFile)
    return strRet;
  fseek(i_pFile, i_Pos, SEEK_SET);

  while (i_Pos < i_Size)
  {
    cTemp = (char)fgetc(i_pFile);
    i_Pos++;

    if (cTemp == czXREF[sIndex])
    {
      czXref[sIndex] = cTemp;
      sIndex++;
    }
    else
    {
      sIndex = 0;
      memset(czXref, 0, sizeof(czXREF));
    }
    if (0 == strcmp(czXref, czXREF))
    {
      strRet.assign(czXref);
      break;
    }
  }
  while (i_Pos < i_Size)
  {
    char cTemp = fgetc(i_pFile);
    strRet.insert(strRet.size(), &cTemp, 1);
    i_Pos++;
    if (strRet.at(strRet.size() - 1) == '<' && strRet.at(strRet.size() - 2) == '<')
    {
      lDic++;
    }
    if (strRet.at(strRet.size() - 1) == '>' && strRet.at(strRet.size() - 2) == '>')
    {
      lDic--;
      if (!lDic)
      {
        return strRet;
      }
    }
  }
  strRet.clear();
  return strRet;
}
